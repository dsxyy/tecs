/*
 * Copyright (C) 2009      Citrix Ltd.
 * Author Stefano Stabellini <stefano.stabellini@eu.citrix.com>
 * Author Vincent Hanquez <vincent.hanquez@eu.citrix.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 2.1 only. with the special
 * exception on linking described in file LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 */

//#include "libxl_osdeps.h"
#ifdef __XEN_DEFINE__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/utsname.h> /* for utsname in xl info */
#include <xentoollog.h>
#include <ctype.h>
#include <inttypes.h>
#include <limits.h>

#include "libxl_impl.h"
#include "sky.h"

#define CHK_ERRNO( call ) ({                                            \
        int chk_errno = (call);                                         \
        if (chk_errno < 0) {                                                \
            fprintf(stderr,"xl: fatal error: %s:%d: %s: %s\n",          \
                    __FILE__,__LINE__, strerror(chk_errno), #call);     \
            exit(-ERROR_FAIL);                                          \
        }                                                               \
    })

#define MUST( call ) ({                                                 \
        int must_rc = (call);                                           \
        if (must_rc < 0) {                                                  \
            fprintf(stderr,"xl: fatal error: %s:%d, rc=%d: %s\n",       \
                    __FILE__,__LINE__, must_rc, #call);                 \
            exit(-must_rc);                                             \
        }                                                               \
    })


#ifndef UINT16_MAX
#define UINT16_MAX             (65535U)
#endif
#ifndef UINT32_MAX
#define UINT32_MAX             (4294967295U)
#endif

# define PRIx32                     "x"
# define PRIu32                     "u"
# define PRIu64                     "lu"

# define XL_GLOBAL_CONFIG  "/etc/xen/xl.conf"

typedef struct {
    /* every struct like this must be in XLCHILD_LIST */
    pid_t pid; /* 0: not in use */
    int reaped; /* valid iff pid!=0 */
    int status; /* valid iff reaped */
} xlchild;

typedef enum {
    child_console, child_waitdaemon, child_migration,
    child_max
} xlchildnum;


xlchild children[child_max];
/* every libxl action in xl uses this same libxl context */
libxl_ctx *ctx;


int xl_child_pid(xlchildnum child)
{
    xlchild *ch = &children[child];
    return ch->pid;
}



pid_t xl_waitpid(xlchildnum child, int *status, int flags)
{
    xlchild *ch = &children[child];
    pid_t got = ch->pid;
    SkyAssert(got);
    if (ch->reaped) {
        *status = ch->status;
        ch->pid = 0;
        return got;
    }
    for (;;) {
        got = waitpid(ch->pid, status, flags);
        if (got < 0 && errno == EINTR) continue;
        if (got > 0) {
            SkyAssert(got == ch->pid);
            ch->pid = 0;
        }
        return got;
    }
}


pid_t xl_fork(xlchildnum child) {
    xlchild *ch = &children[child];
    int i;

    assert(!ch->pid);
    ch->reaped = 0;

    ch->pid = fork();
    if (ch->pid == -1) {
        perror("fork failed");
        exit(-1);
    }

    if (!ch->pid) {
        /* We are in the child now.  So all these children are not ours. */
        for (i=0; i<child_max; i++)
            children[i].pid = 0;
    }

    return ch->pid;
}

static int xl_reaped_callback(pid_t got, int status, void *user)
{
    int i;
    SkyAssert(got);
    for (i=0; i<child_max; i++) {
        xlchild *ch = &children[i];
        if (ch->pid == got) {
            ch->reaped = 1;
            ch->status = status;
            return 0;
        }
    }
    return ERROR_UNKNOWN_CHILD;
}

static const libxl_childproc_hooks childproc_hooks = {\
    libxl_sigchld_owner_libxl,\
    0,\
    xl_reaped_callback,\
};


int logfile = 2;

xentoollog_logger *logger;


int dryrun_only = 0;
int force_execution;
int autoballoon = 1;
char *blkdev_start;
int run_hotplug_scripts = 1;
char *default_vifscript = NULL;
char *default_bridge = NULL;
char *log_file= "/var/log/libxl.log";


int xl_get_logger(void)
{
    FILE *logger_file = fopen(log_file, "a");
    if (logger_file == NULL)
    {
        fprintf(stderr, "fopen log_file err!");
        return -1;
    }
    
    logger = (xentoollog_logger *)xtl_createlogger_stdiostream(logger_file, XTL_DEBUG,  0);
    if (!logger) {
        fprintf(stderr, "cannot create logger for libxenlight");
        return -1;
    }
    return 0;
}

int xl_ctx_alloc(void) 
{
    if (libxl_ctx_alloc(&ctx, LIBXL_VERSION, 0, (xentoollog_logger*)logger)) {
        fprintf(stderr, "cannot init xl context\n");
        return -1;
    }

    libxl_childproc_setmode(ctx, &childproc_hooks, 0);
    return 0;
}

void postfork(void)
{
    libxl_postfork_child_noexec(ctx); /* in case we don't exit/exec */
    ctx = 0;

    xl_ctx_alloc();
}


static int parse_global_config(const char *configfile,
                              const char *configfile_data,
                              int configfile_len)
{
    long l;
    XLU_Config *config;
    int e;
    const char *buf;

    config = xlu_cfg_init(stderr, configfile);
    if (!config) {
        fprintf(stderr, "Failed to allocate for configuration\n");
        return -1;
    }

    e = xlu_cfg_readdata(config, configfile_data, configfile_len);
    if (e) {
        fprintf(stderr, "Failed to parse config file: %s\n", strerror(e));
        return -1;
    }

    if (!xlu_cfg_get_long (config, "autoballoon", &l, 0))
        autoballoon = l;

    if (!xlu_cfg_get_long (config, "run_hotplug_scripts", &l, 0))
        run_hotplug_scripts = l;


    if (!xlu_cfg_get_string (config, "vifscript", &buf, 0))
        default_vifscript = strdup(buf);

    if (!xlu_cfg_get_string (config, "defaultbridge", &buf, 0))
        default_bridge = strdup(buf);

    if (!xlu_cfg_get_string (config, "blkdev_start", &buf, 0))
        blkdev_start = strdup(buf);
    xlu_cfg_destroy(config);
    return 0;
}


/* when we operate on a domain, it is this one: */
#define INVALID_DOMID ~0
static uint32_t domid = INVALID_DOMID;
static const char *common_domname;

/* Stash for specific vcpu to pcpu mappping */
static int *vcpu_to_pcpu;

static const char savefileheader_magic[32]=
    "Xen saved domain, xl format\n";

static const char migrate_receiver_banner[]=
    "xl migration receiver ready, send binary domain data.\n";
static const char migrate_receiver_ready[]=
    "domain received, ready to unpause";
static const char migrate_permission_to_go[]=
    "domain is yours, you are cleared to unpause";
static const char migrate_report[]=
    "my copy unpause results are as follows";
  /* followed by one byte:
   *     0: everything went well, domain is running
   *            next thing is we all exit
   * non-0: things went badly
   *            next thing should be a migrate_permission_to_go
   *            from target to source
   */

struct save_file_header {
    char magic[32]; /* savefileheader_magic */
    /* All uint32_ts are in domain's byte order. */
    uint32_t byteorder; /* SAVEFILE_BYTEORDER_VALUE */
    uint32_t mandatory_flags; /* unknown flags => reject restore */
    uint32_t optional_flags; /* unknown flags => reject restore */
    uint32_t optional_data_len; /* skip, or skip tail, if not understood */
};
 // 下面这断注释不能删除，主要表示
// action_on_shutdown_names 这个顺序，如果XEN顺序变化
// 需要调整
#if 0
typedef enum libxl_action_on_shutdown {    
LIBXL_ACTION_ON_SHUTDOWN_DESTROY = 1,   
LIBXL_ACTION_ON_SHUTDOWN_RESTART = 2,    
LIBXL_ACTION_ON_SHUTDOWN_RESTART_RENAME = 3,    
LIBXL_ACTION_ON_SHUTDOWN_PRESERVE = 4,    
LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_DESTROY = 5,    
LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_RESTART = 6,} 
libxl_action_on_shutdown;


static const char *action_on_shutdown_names[] = {
    [LIBXL_ACTION_ON_SHUTDOWN_DESTROY] = "destroy",

    [LIBXL_ACTION_ON_SHUTDOWN_RESTART] = "restart",
    [LIBXL_ACTION_ON_SHUTDOWN_RESTART_RENAME] = "rename-restart",

    [LIBXL_ACTION_ON_SHUTDOWN_PRESERVE] = "preserve",

    [LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_DESTROY] = "coredump-destroy",
    [LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_RESTART] = "coredump-restart",
};
#endif
static const char *action_on_shutdown_names[] = {"null",
                                                                           "destroy",
                                                                           "restart",
                                                                           "rename-restart",

                                                                           "preserve",

                                                                            "coredump-destroy",
                                                                            "coredump-restart",
};

/* Optional data, in order:
 *   4 bytes uint32_t  config file size
 *   n bytes           config file in Unix text file format
 */

#define SAVEFILE_BYTEORDER_VALUE ((uint32_t)0x01020304UL)

struct domain_create {
    int debug;
    int daemonize;
    int monitor; /* handle guest reboots etc */
    int paused;
    int dryrun;
    int quiet;
    int vnc;
    int vncautopass;
    int console_autoconnect;
    const char *config_file;
    const char *extra_config; /* extra config string */
    const char *restore_file;
    int migrate_fd; /* -1 means none */
    char **migration_domname_r; /* from malloc */
};


int libxl_init(void)
{
    void *config_data = 0;
    int config_len = 0;
    
    if (0 != xl_get_logger())
    {
        return -1;
    }
    
    if (0 != xl_ctx_alloc())
    {
        return -1;
    }
    
    int ret = libxl_read_file_contents(ctx, XL_GLOBAL_CONFIG,
            &config_data, &config_len);
    if (ret)
        fprintf(stderr, "Failed to read config file: %s: %s\n",
                XL_GLOBAL_CONFIG, strerror(errno));
    parse_global_config(XL_GLOBAL_CONFIG, (char*)config_data, config_len);
    free(config_data);
    return 0;
}

static int qualifier_to_id(const char *p, uint32_t *id_r)
{
    int i, alldigit;

    alldigit = 1;
    for (i = 0; p[i]; i++) {
        if (!isdigit((uint8_t)p[i])) {
            alldigit = 0;
            break;
        }
    }

    if (i > 0 && alldigit) {
        *id_r = strtoul(p, NULL, 10);
        return 0;
    } else {
        /* check here if it's a uuid and do proper conversion */
    }
    return 1;
}

static int domain_qualifier_to_domid(const char *p, uint32_t *domid_r,
                                     int *was_name_r)
{
    int was_name, rc;

    was_name = qualifier_to_id(p, domid_r);
    if (was_name_r)
        *was_name_r = was_name;

    if (was_name) {
        rc = libxl_name_to_domid(ctx, p, domid_r);
        if (rc)
            return rc;
    } else {
        rc = libxl_domain_info(ctx, NULL, *domid_r);
        /* error only if domain does not exist */
        if (rc == ERROR_INVAL)
            return rc;
    }

    return 0;
}

static int cpupool_qualifier_to_cpupoolid(const char *p, uint32_t *poolid_r,
                                     int *was_name_r)
{
    int was_name;

    was_name = qualifier_to_id(p, poolid_r);
    if (was_name_r) *was_name_r = was_name;
    return was_name ? libxl_name_to_cpupoolid(ctx, p, poolid_r) : 0;
}

static void find_domain(const char *p)
{
    int rc, was_name;

    rc = domain_qualifier_to_domid(p, &domid, &was_name);
    if (rc) {
        fprintf(stderr, "%s is an invalid domain identifier (rc=%d)\n", p, rc);
        exit(2);
    }
    common_domname = was_name ? p : libxl_domid_to_name(ctx, domid);
}

static int vncviewer(uint32_t domid, int autopass)
{
    libxl_vncviewer_exec(ctx, domid, autopass);
    fprintf(stderr, "Unable to execute vncviewer\n");
    return 1;
}

// 替换为进程信号量
static int acquire_lock(void)
{
      return 1;
}

static int release_lock(void)
{
    return 1;
}

static void *xrealloc(void *ptr, size_t sz) {
    void *r;
    if (!sz) { free(ptr); return 0; }
      /* realloc(non-0, 0) has a useless return value;
       * but xrealloc(anything, 0) is like free
       */
    r = realloc(ptr, sz);
    if (!r) { fprintf(stderr,"xl: Unable to realloc to %lu bytes.\n",
                      (unsigned long)sz); exit(-ERROR_FAIL); }
    return r;
}

#define LOG(_f, _a...)   dolog(__FILE__, __LINE__, __func__, _f "\n", ##_a)

//static void dolog(const char *file, int line, const char *func, char *fmt, ...)
//     __attribute__((format(printf,4,5)));

static void dolog(const char *file, int line, const char *func, const char *fmt, ...)
{
    va_list ap;
    char *s = NULL;
    int rc;

    va_start(ap, fmt);
    rc = vasprintf(&s, fmt, ap);
    va_end(ap);
    if (rc >= 0)
        libxl_write_exactly(NULL, logfile, s, rc, NULL, NULL);
    free(s);
}

static int parse_action_on_shutdown(const char *buf, libxl_action_on_shutdown *a)
{
    uint32 i;
    const char *n;

    for (i = 0; i < sizeof(action_on_shutdown_names) / sizeof(action_on_shutdown_names[0]); i++) {
        n = action_on_shutdown_names[i];

        if (!n) continue;

        if (strcmp(buf, n) == 0) {
            *a = (libxl_action_on_shutdown)i;
            return 1;
        }
    }
    return 0;
}

#define DSTATE_INITIAL   0
#define DSTATE_TAP       1
#define DSTATE_PHYSPATH  2
#define DSTATE_VIRTPATH  3
#define DSTATE_VIRTTYPE  4
#define DSTATE_RW        5
#define DSTATE_TERMINAL  6

static void parse_disk_config_multistring(XLU_Config **config,
                                          int nspecs, const char *const *specs,
                                          libxl_device_disk *disk)
{
    int e;

    libxl_device_disk_init(disk);

    if (!*config) {
        *config = xlu_cfg_init(stderr, "command line");
        if (!*config) { perror("xlu_cfg_init"); exit(-1); }
    }

    e = xlu_disk_parse(*config, nspecs, specs, disk);
    if (e == EINVAL) exit(-1);
    if (e) {
        fprintf(stderr,"xlu_disk_parse failed: %s\n",strerror(errno));
        exit(-1);
    }
}

static void parse_disk_config(XLU_Config **config, const char *spec,
                              libxl_device_disk *disk)
{
    parse_disk_config_multistring(config, 1, &spec, disk);
}

static void parse_vif_rate(XLU_Config **config, const char *rate,
                           libxl_device_nic *nic)
{
    int e;

    e = xlu_vif_parse_rate(*config, rate, nic);
    if (e == EINVAL || e == EOVERFLOW) exit(-1);
    if (e) {
        fprintf(stderr,"xlu_vif_parse_rate failed: %s\n",strerror(errno));
        exit(-1);
    }
}

static void split_string_into_string_list(const char *str,
                                          const char *delim,
                                          libxl_string_list *psl)
{
    char *s, *saveptr;
    const char *p;
    libxl_string_list sl;

    int i = 0, nr = 0;

    s = strdup(str);
    if (s == NULL) {
        fprintf(stderr, "unable to allocate memory to parse bootloader args\n");
        exit(-1);
    }

    /* Count number of entries */
    p = strtok_r(s, delim, &saveptr);
    do {
        nr++;
    } while ((p = strtok_r(NULL, delim, &saveptr)));

    free(s);

    s = strdup(str);

    sl = (libxl_string_list)malloc((nr+1) * sizeof (char *));
    if (sl == NULL) {
        fprintf(stderr, "unable to allocate memory for bootloader args\n");
        exit(-1);
    }

    p = strtok_r(s, delim, &saveptr);
    do {
        sl[i] = strdup(p);
        i++;
    } while ((p = strtok_r(NULL, delim, &saveptr)));
    sl[i] = NULL;

    *psl = sl;

    free(s);
}

static int vcpupin_parse(char *cpu, libxl_bitmap *cpumap)
{
    libxl_bitmap exclude_cpumap;
    uint32_t cpuida, cpuidb;
    char *endptr, *toka, *tokb, *saveptr = NULL;
    int i, rc = 0, rmcpu;

    if (!strcmp(cpu, "all")) {
        libxl_bitmap_set_any(cpumap);
        return 0;
    }

    if (libxl_cpu_bitmap_alloc(ctx, &exclude_cpumap, 0)) {
        fprintf(stderr, "Error: Failed to allocate cpumap.\n");
        return ENOMEM;
    }

    for (toka = strtok_r(cpu, ",", &saveptr); toka;
         toka = strtok_r(NULL, ",", &saveptr)) {
        rmcpu = 0;
        if (*toka == '^') {
            /* This (These) Cpu(s) will be removed from the map */
            toka++;
            rmcpu = 1;
        }
        /* Extract a valid (range of) cpu(s) */
        cpuida = cpuidb = strtoul(toka, &endptr, 10);
        if (endptr == toka) {
            fprintf(stderr, "Error: Invalid argument.\n");
            rc = EINVAL;
            goto vcpp_out;
        }
        if (*endptr == '-') {
            tokb = endptr + 1;
            cpuidb = strtoul(tokb, &endptr, 10);
            if (endptr == tokb || cpuida > cpuidb) {
                fprintf(stderr, "Error: Invalid argument.\n");
                rc = EINVAL;
                goto vcpp_out;
            }
        }
        while (cpuida <= cpuidb) {
            rmcpu == 0 ? libxl_bitmap_set(cpumap, cpuida) :
                         libxl_bitmap_set(&exclude_cpumap, cpuida);
            cpuida++;
        }
    }

    /* Clear all the cpus from the removal list */
    libxl_for_each_set_bit(i, exclude_cpumap) {
        libxl_bitmap_reset(cpumap, i);
    }

vcpp_out:
    libxl_bitmap_dispose(&exclude_cpumap);

    return rc;
}

static void parse_config_data(const char *config_source,
                              const char *config_data,
                              int config_len,
                              libxl_domain_config *d_config,
                              struct domain_create *dom_info)

{
    const char *buf;
    long l;
    XLU_Config *config;
    XLU_ConfigList *cpus, *vbds, *nics, *pcis, *cvfbs, *cpuids;
    XLU_ConfigList *ioports, *irqs;
    int num_ioports, num_irqs;
    int pci_power_mgmt = 0;
    int pci_msitranslate = 0;
    int pci_permissive = 0;
    int i, e;

    libxl_domain_create_info *c_info = &d_config->c_info;
    libxl_domain_build_info *b_info = &d_config->b_info;

    config= xlu_cfg_init(stderr, config_source);
    if (!config) {
        fprintf(stderr, "Failed to allocate for configuration\n");
        exit(1);
    }

    e= xlu_cfg_readdata(config, config_data, config_len);
    if (e) {
        fprintf(stderr, "Failed to parse config: %s\n", strerror(e));
        exit(1);
    }

    if (!xlu_cfg_get_string (config, "seclabel", &buf, 0)) {
        e = libxl_flask_context_to_sid(ctx, (char *)buf, strlen(buf),
                                    &c_info->ssidref);
        if (e) {
            if (errno == ENOSYS) {
                fprintf(stderr, "XSM Disabled: seclabel not supported\n");
            } else {
                fprintf(stderr, "Invalid seclabel: %s\n", buf);
                exit(1);
            }
        }
    }

    libxl_defbool_set(&c_info->run_hotplug_scripts, run_hotplug_scripts);
    c_info->type = LIBXL_DOMAIN_TYPE_PV;
    if (!xlu_cfg_get_string (config, "builder", &buf, 0) &&
        !strncmp(buf, "hvm", strlen(buf)))
        c_info->type = LIBXL_DOMAIN_TYPE_HVM;

    xlu_cfg_get_defbool(config, "hap", &c_info->hap, 0);

    if (xlu_cfg_replace_string (config, "name", &c_info->name, 0)) {
        fprintf(stderr, "Domain name must be specified.\n");
        exit(1);
    }

    if (!xlu_cfg_get_string (config, "uuid", &buf, 0) ) {
        if ( libxl_uuid_from_string(&c_info->uuid, buf) ) {
            fprintf(stderr, "Failed to parse UUID: %s\n", buf);
            exit(1);
        }
    }else{
        libxl_uuid_generate(&c_info->uuid);
    }

    xlu_cfg_get_defbool(config, "oos", &c_info->oos, 0);

    if (!xlu_cfg_get_string (config, "pool", &buf, 0)) {
        c_info->poolid = -1;
        cpupool_qualifier_to_cpupoolid(buf, &c_info->poolid, NULL);
    }
    if (!libxl_cpupoolid_to_name(ctx, c_info->poolid)) {
        fprintf(stderr, "Illegal pool specified\n");
        exit(1);
    }

    libxl_domain_build_info_init_type(b_info, c_info->type);
    if (blkdev_start)
        b_info->blkdev_start = strdup(blkdev_start);

    /* the following is the actual config parsing with overriding
     * values in the structures */
    if (!xlu_cfg_get_long (config, "cpu_weight", &l, 0))
        b_info->sched_params.weight = l;
    if (!xlu_cfg_get_long (config, "cap", &l, 0))
        b_info->sched_params.cap = l;
    if (!xlu_cfg_get_long (config, "period", &l, 0))
        b_info->sched_params.period = l;
    if (!xlu_cfg_get_long (config, "slice", &l, 0))
        b_info->sched_params.slice = l;
    if (!xlu_cfg_get_long (config, "latency", &l, 0))
        b_info->sched_params.latency = l;
    if (!xlu_cfg_get_long (config, "extratime", &l, 0))
        b_info->sched_params.extratime = l;

    if (!xlu_cfg_get_long (config, "vcpus", &l, 0)) {
        b_info->max_vcpus = l;

        if (libxl_cpu_bitmap_alloc(ctx, &b_info->avail_vcpus, l)) {
            fprintf(stderr, "Unable to allocate cpumap\n");
            exit(1);
        }
        libxl_bitmap_set_none(&b_info->avail_vcpus);
        while (l-- > 0)
            libxl_bitmap_set((&b_info->avail_vcpus), l);
    }

    if (!xlu_cfg_get_long (config, "maxvcpus", &l, 0))
        b_info->max_vcpus = l;

    if (!xlu_cfg_get_list (config, "cpus", &cpus, 0, 1)) {
        int i, n_cpus = 0;

        if (libxl_cpu_bitmap_alloc(ctx, &b_info->cpumap, 0)) {
            fprintf(stderr, "Unable to allocate cpumap\n");
            exit(1);
        }

        /* Prepare the array for single vcpu to pcpu mappings */
        vcpu_to_pcpu = (int*)malloc(sizeof(int) * b_info->max_vcpus);
        memset(vcpu_to_pcpu, -1, sizeof(int) * b_info->max_vcpus);

        /*
         * Idea here is to let libxl think all the domain's vcpus
         * have cpu affinity with all the pcpus on the list.
         * It is then us, here in xl, that matches each single vcpu
         * to its pcpu (and that's why we need to stash such info in
         * the vcpu_to_pcpu array now) after the domain has been created.
         * Doing it like this saves the burden of passing to libxl
         * some big array hosting the single mappings. Also, using
         * the cpumap derived from the list ensures memory is being
         * allocated on the proper nodes anyway.
         */
        libxl_bitmap_set_none(&b_info->cpumap);
        while ((buf = xlu_cfg_get_listitem(cpus, n_cpus)) != NULL) {
            i = atoi(buf);
            if (!libxl_bitmap_cpu_valid(&b_info->cpumap, i)) {
                fprintf(stderr, "cpu %d illegal\n", i);
                exit(1);
            }
            libxl_bitmap_set(&b_info->cpumap, i);
            if (n_cpus < b_info->max_vcpus)
                vcpu_to_pcpu[n_cpus] = i;
            n_cpus++;
        }

        /* We have a cpumap, disable automatic placement */
        libxl_defbool_set(&b_info->numa_placement, false);
    }
    else if (!xlu_cfg_get_string (config, "cpus", &buf, 0)) {
        char *buf2 = strdup(buf);

        if (libxl_cpu_bitmap_alloc(ctx, &b_info->cpumap, 0)) {
            fprintf(stderr, "Unable to allocate cpumap\n");
            exit(1);
        }

        libxl_bitmap_set_none(&b_info->cpumap);
        if (vcpupin_parse(buf2, &b_info->cpumap))
            exit(1);
        free(buf2);

        libxl_defbool_set(&b_info->numa_placement, false);
    }

    if (!xlu_cfg_get_long (config, "memory", &l, 0)) {
        b_info->max_memkb = l * 1024;
        b_info->target_memkb = b_info->max_memkb;
    }

    if (!xlu_cfg_get_long (config, "maxmem", &l, 0))
        b_info->max_memkb = l * 1024;

    if (xlu_cfg_get_string (config, "on_poweroff", &buf, 0))
        buf = "destroy";
    if (!parse_action_on_shutdown(buf, &d_config->on_poweroff)) {
        fprintf(stderr, "Unknown on_poweroff action \"%s\" specified\n", buf);
        exit(1);
    }

    if (xlu_cfg_get_string (config, "on_reboot", &buf, 0))
        buf = "restart";
    if (!parse_action_on_shutdown(buf, &d_config->on_reboot)) {
        fprintf(stderr, "Unknown on_reboot action \"%s\" specified\n", buf);
        exit(1);
    }

    if (xlu_cfg_get_string (config, "on_watchdog", &buf, 0))
        buf = "destroy";
    if (!parse_action_on_shutdown(buf, &d_config->on_watchdog)) {
        fprintf(stderr, "Unknown on_watchdog action \"%s\" specified\n", buf);
        exit(1);
    }


    if (xlu_cfg_get_string (config, "on_crash", &buf, 0))
        buf = "destroy";
    if (!parse_action_on_shutdown(buf, &d_config->on_crash)) {
        fprintf(stderr, "Unknown on_crash action \"%s\" specified\n", buf);
        exit(1);
    }

    /* libxl_get_required_shadow_memory() must be called after final values
     * (default or specified) for vcpus and memory are set, because the
     * calculation depends on those values. */
    b_info->shadow_memkb = !xlu_cfg_get_long(config, "shadow_memory", &l, 0)
        ? l * 1024
        : libxl_get_required_shadow_memory(b_info->max_memkb,
                                           b_info->max_vcpus);

    xlu_cfg_get_defbool(config, "nomigrate", &b_info->disable_migrate, 0);

    if (!xlu_cfg_get_long(config, "tsc_mode", &l, 1)) {
        const char *s = libxl_tsc_mode_to_string((libxl_tsc_mode)l);
        fprintf(stderr, "WARNING: specifying \"tsc_mode\" as an integer is deprecated. "
                "Please use the named parameter variant. %s%s%s\n",
                s ? "e.g. tsc_mode=\"" : "",
                s ? s : "",
                s ? "\"" : "");

        if (l < LIBXL_TSC_MODE_DEFAULT ||
            l > LIBXL_TSC_MODE_NATIVE_PARAVIRT) {
            fprintf(stderr, "ERROR: invalid value %ld for \"tsc_mode\"\n", l);
            exit (1);
        }
        b_info->tsc_mode = (libxl_tsc_mode)l;
    } else if (!xlu_cfg_get_string(config, "tsc_mode", &buf, 0)) {
        fprintf(stderr, "got a tsc mode string: \"%s\"\n", buf);
        if (libxl_tsc_mode_from_string(buf, &b_info->tsc_mode)) {
            fprintf(stderr, "ERROR: invalid value \"%s\" for \"tsc_mode\"\n",
                    buf);
            exit (1);
        }
    }

    if (!xlu_cfg_get_long(config, "rtc_timeoffset", &l, 0))
        b_info->rtc_timeoffset = l;

    if (dom_info && !xlu_cfg_get_long(config, "vncviewer", &l, 0)) {
        /* Command line arguments must take precedence over what's
         * specified in the configuration file. */
        if (!dom_info->vnc)
            dom_info->vnc = l;
    }

    xlu_cfg_get_defbool(config, "localtime", &b_info->localtime, 0);

    if (!xlu_cfg_get_long (config, "videoram", &l, 0))
        b_info->video_memkb = l * 1024;

    switch(b_info->type) {
    case LIBXL_DOMAIN_TYPE_HVM:
        if (!xlu_cfg_get_string (config, "kernel", &buf, 0))
            fprintf(stderr, "WARNING: ignoring \"kernel\" directive for HVM guest. "
                    "Use \"firmware_override\" instead if you really want a non-default firmware\n");

        xlu_cfg_replace_string (config, "firmware_override",
                                &b_info->u.hvm.firmware, 0);
        if (!xlu_cfg_get_string(config, "bios", &buf, 0) &&
            libxl_bios_type_from_string(buf, &b_info->u.hvm.bios)) {
                fprintf(stderr, "ERROR: invalid value \"%s\" for \"bios\"\n",
                    buf);
                exit (1);
        }

        xlu_cfg_get_defbool(config, "pae", &b_info->u.hvm.pae, 0);
        xlu_cfg_get_defbool(config, "apic", &b_info->u.hvm.apic, 0);
        xlu_cfg_get_defbool(config, "acpi", &b_info->u.hvm.acpi, 0);
        xlu_cfg_get_defbool(config, "acpi_s3", &b_info->u.hvm.acpi_s3, 0);
        xlu_cfg_get_defbool(config, "acpi_s4", &b_info->u.hvm.acpi_s4, 0);
        xlu_cfg_get_defbool(config, "nx", &b_info->u.hvm.nx, 0);
        xlu_cfg_get_defbool(config, "viridian", &b_info->u.hvm.viridian, 0);
        xlu_cfg_get_defbool(config, "hpet", &b_info->u.hvm.hpet, 0);
        xlu_cfg_get_defbool(config, "vpt_align", &b_info->u.hvm.vpt_align, 0);

        if (!xlu_cfg_get_long(config, "timer_mode", &l, 1)) {
            const char *s = libxl_timer_mode_to_string((libxl_timer_mode)l);
            fprintf(stderr, "WARNING: specifying \"timer_mode\" as an integer is deprecated. "
                    "Please use the named parameter variant. %s%s%s\n",
                    s ? "e.g. timer_mode=\"" : "",
                    s ? s : "",
                    s ? "\"" : "");

            if (l < LIBXL_TIMER_MODE_DELAY_FOR_MISSED_TICKS ||
                l > LIBXL_TIMER_MODE_ONE_MISSED_TICK_PENDING) {
                fprintf(stderr, "ERROR: invalid value %ld for \"timer_mode\"\n", l);
                exit (1);
            }
            b_info->u.hvm.timer_mode = (libxl_timer_mode)l;
        } else if (!xlu_cfg_get_string(config, "timer_mode", &buf, 0)) {
            if (libxl_timer_mode_from_string(buf, &b_info->u.hvm.timer_mode)) {
                fprintf(stderr, "ERROR: invalid value \"%s\" for \"timer_mode\"\n",
                        buf);
                exit (1);
            }
        }

        xlu_cfg_get_defbool(config, "nestedhvm", &b_info->u.hvm.nested_hvm, 0);
        break;
    case LIBXL_DOMAIN_TYPE_PV:
    {
        char *cmdline = NULL;
        const char *root = NULL, *extra = "";

        xlu_cfg_replace_string (config, "kernel", &b_info->u.pv.kernel, 0);

        xlu_cfg_get_string (config, "root", &root, 0);
        xlu_cfg_get_string (config, "extra", &extra, 0);

        if (root) {
            if (asprintf(&cmdline, "root=%s %s", root, extra) == -1)
                cmdline = NULL;
        } else {
            cmdline = strdup(extra);
        }

        if ((root || extra) && !cmdline) {
            fprintf(stderr, "Failed to allocate memory for cmdline\n");
            exit(1);
        }

        xlu_cfg_replace_string (config, "bootloader", &b_info->u.pv.bootloader, 0);
        switch (xlu_cfg_get_list_as_string_list(config, "bootloader_args",
                                      &b_info->u.pv.bootloader_args, 1))
        {

        case 0: break; /* Success */
        case ESRCH: break; /* Option not present */
        case EINVAL:
            if (!xlu_cfg_get_string(config, "bootloader_args", &buf, 0)) {

                fprintf(stderr, "WARNING: Specifying \"bootloader_args\""
                        " as a string is deprecated. "
                        "Please use a list of arguments.\n");
                split_string_into_string_list(buf, " \t\n",
                                              &b_info->u.pv.bootloader_args);
            }
            break;
        default:
            fprintf(stderr,"xl: Unable to parse bootloader_args.\n");
            exit(-ERROR_FAIL);
        }

        if (!b_info->u.pv.bootloader && !b_info->u.pv.kernel) {
            fprintf(stderr, "Neither kernel nor bootloader specified\n");
            exit(1);
        }

        b_info->u.pv.cmdline = cmdline;
        xlu_cfg_replace_string (config, "ramdisk", &b_info->u.pv.ramdisk, 0);
        break;
    }
    default:
        abort();
    }

    if (!xlu_cfg_get_list(config, "ioports", &ioports, &num_ioports, 0)) {
        b_info->num_ioports = num_ioports;
        b_info->ioports = (libxl_ioport_range*)calloc(num_ioports, sizeof(*b_info->ioports));
        if (b_info->ioports == NULL) {
            fprintf(stderr, "unable to allocate memory for ioports\n");
            exit(-1);
        }

        for (i = 0; i < num_ioports; i++) {
            const char *buf2;
            char *ep;
            uint32_t start, end;
            unsigned long ul;

            buf = xlu_cfg_get_listitem (ioports, i);
            if (!buf) {
                fprintf(stderr,
                        "xl: Unable to get element #%d in ioport list\n", i);
                exit(1);
            }
            ul = strtoul(buf, &ep, 16);
            if (ep == buf) {
                fprintf(stderr, "xl: Invalid argument parsing ioport: %s\n",
                        buf);
                exit(1);
            }
            if (ul >= UINT32_MAX) {
                fprintf(stderr, "xl: ioport %lx too big\n", ul);
                exit(1);
            }
            start = end = ul;

            if (*ep == '-') {
                buf2 = ep + 1;
                ul = strtoul(buf2, &ep, 16);
                if (ep == buf2 || *ep != '\0' || start > end) {
                    fprintf(stderr,
                            "xl: Invalid argument parsing ioport: %s\n", buf);
                    exit(1);
                }
                if (ul >= UINT32_MAX) {
                    fprintf(stderr, "xl: ioport %lx too big\n", ul);
                    exit(1);
                }
                end = ul;
            } else if ( *ep != '\0' )
                fprintf(stderr,
                        "xl: Invalid argument parsing ioport: %s\n", buf);
            b_info->ioports[i].first = start;
            b_info->ioports[i].number = end - start + 1;
        }
    }

    if (!xlu_cfg_get_list(config, "irqs", &irqs, &num_irqs, 0)) {
        b_info->num_irqs = num_irqs;
        b_info->irqs = (uint32_t*)calloc(num_irqs, sizeof(*b_info->irqs));
        if (b_info->irqs == NULL) {
            fprintf(stderr, "unable to allocate memory for ioports\n");
            exit(-1);
        }
        for (i = 0; i < num_irqs; i++) {
            char *ep;
            unsigned long ul;
            buf = xlu_cfg_get_listitem (irqs, i);
            if (!buf) {
                fprintf(stderr,
                        "xl: Unable to get element %d in irq list\n", i);
                exit(1);
            }
            ul = strtoul(buf, &ep, 10);
            if (ep == buf) {
                fprintf(stderr,
                        "xl: Invalid argument parsing irq: %s\n", buf);
                exit(1);
            }
            if (ul >= UINT32_MAX) {
                fprintf(stderr, "xl: irq %lx too big\n", ul);
                exit(1);
            }
            b_info->irqs[i] = ul;
        }
    }

    if (!xlu_cfg_get_list (config, "disk", &vbds, 0, 0)) {
        d_config->num_disks = 0;
        d_config->disks = NULL;
        while ((buf = xlu_cfg_get_listitem (vbds, d_config->num_disks)) != NULL) {
            libxl_device_disk *disk;
            char *buf2 = strdup(buf);

            d_config->disks = (libxl_device_disk *) realloc(d_config->disks, sizeof (libxl_device_disk) * (d_config->num_disks + 1));
            disk = d_config->disks + d_config->num_disks;
            parse_disk_config(&config, buf2, disk);

            free(buf2);
            d_config->num_disks++;
        }
    }

    if (!xlu_cfg_get_list (config, "vif", &nics, 0, 0)) {
        d_config->num_nics = 0;
        d_config->nics = NULL;
        while ((buf = xlu_cfg_get_listitem (nics, d_config->num_nics)) != NULL) {
            libxl_device_nic *nic;
            char *buf2 = strdup(buf);
            char *p, *p2;

            d_config->nics = (libxl_device_nic *) realloc(d_config->nics, sizeof (libxl_device_nic) * (d_config->num_nics+1));
            nic = d_config->nics + d_config->num_nics;
            libxl_device_nic_init(nic);
            nic->devid = d_config->num_nics;

            if (default_vifscript) {
                free(nic->script);
                nic->script = strdup(default_vifscript);
            }

            if (default_bridge) {
                free(nic->bridge);
                nic->bridge = strdup(default_bridge);
            }

            p = strtok(buf2, ",");
            if (!p)
                goto skip;
            do {
                while (*p == ' ')
                    p++;
                if ((p2 = strchr(p, '=')) == NULL)
                    break;
                *p2 = '\0';
                if (!strcmp(p, "model")) {
                    free(nic->model);
                    nic->model = strdup(p2 + 1);
                } else if (!strcmp(p, "mac")) {
                    char *p3 = p2 + 1;
                    *(p3 + 2) = '\0';
                    nic->mac[0] = strtol(p3, NULL, 16);
                    p3 = p3 + 3;
                    *(p3 + 2) = '\0';
                    nic->mac[1] = strtol(p3, NULL, 16);
                    p3 = p3 + 3;
                    *(p3 + 2) = '\0';
                    nic->mac[2] = strtol(p3, NULL, 16);
                    p3 = p3 + 3;
                    *(p3 + 2) = '\0';
                    nic->mac[3] = strtol(p3, NULL, 16);
                    p3 = p3 + 3;
                    *(p3 + 2) = '\0';
                    nic->mac[4] = strtol(p3, NULL, 16);
                    p3 = p3 + 3;
                    *(p3 + 2) = '\0';
                    nic->mac[5] = strtol(p3, NULL, 16);
                } else if (!strcmp(p, "bridge")) {
                    free(nic->bridge);
                    nic->bridge = strdup(p2 + 1);
                } else if (!strcmp(p, "type")) {
                    if (!strcmp(p2 + 1, "ioemu"))
                        nic->nictype = LIBXL_NIC_TYPE_VIF_IOEMU;
                    else
                        nic->nictype = LIBXL_NIC_TYPE_VIF;
                } else if (!strcmp(p, "ip")) {
                    free(nic->ip);
                    nic->ip = strdup(p2 + 1);
                } else if (!strcmp(p, "script")) {
                    free(nic->script);
                    nic->script = strdup(p2 + 1);
                } else if (!strcmp(p, "vifname")) {
                    free(nic->ifname);
                    nic->ifname = strdup(p2 + 1);
                } else if (!strcmp(p, "backend")) {
                    if(libxl_name_to_domid(ctx, (p2 + 1), &(nic->backend_domid))) {
                        fprintf(stderr, "Specified backend domain does not exist, defaulting to Dom0\n");
                        nic->backend_domid = 0;
                    }
                } else if (!strcmp(p, "rate")) {
                    parse_vif_rate(&config, (p2 + 1), nic);
                } else if (!strcmp(p, "accel")) {
                    fprintf(stderr, "the accel parameter for vifs is currently not supported\n");
                }
            } while ((p = strtok(NULL, ",")) != NULL);
skip:
            free(buf2);
            d_config->num_nics++;
        }
    }

    if (!xlu_cfg_get_list(config, "vif2", NULL, 0, 0)) {
        fprintf(stderr, "WARNING: vif2: netchannel2 is deprecated and not supported by xl\n");
    }

    if (!xlu_cfg_get_list (config, "vfb", &cvfbs, 0, 0)) {
        d_config->num_vfbs = 0;
        d_config->num_vkbs = 0;
        d_config->vfbs = NULL;
        d_config->vkbs = NULL;
        while ((buf = xlu_cfg_get_listitem (cvfbs, d_config->num_vfbs)) != NULL) {
            libxl_device_vfb *vfb;
            libxl_device_vkb *vkb;

            char *buf2 = strdup(buf);
            char *p, *p2;

            d_config->vfbs = (libxl_device_vfb *) realloc(d_config->vfbs, sizeof(libxl_device_vfb) * (d_config->num_vfbs + 1));
            vfb = d_config->vfbs + d_config->num_vfbs;
            libxl_device_vfb_init(vfb);
            vfb->devid = d_config->num_vfbs;

            d_config->vkbs = (libxl_device_vkb *) realloc(d_config->vkbs, sizeof(libxl_device_vkb) * (d_config->num_vkbs + 1));
            vkb = d_config->vkbs + d_config->num_vkbs;
            libxl_device_vkb_init(vkb);
            vkb->devid = d_config->num_vkbs;

            p = strtok(buf2, ",");
            if (!p)
                goto skip_vfb;
            do {
                while (*p == ' ')
                    p++;
                if ((p2 = strchr(p, '=')) == NULL)
                    break;
                *p2 = '\0';
                if (!strcmp(p, "vnc")) {
                    libxl_defbool_set(&vfb->vnc.enable, atoi(p2 + 1));
                } else if (!strcmp(p, "vnclisten")) {
                    free(vfb->vnc.listen);
                    vfb->vnc.listen = strdup(p2 + 1);
                } else if (!strcmp(p, "vncpasswd")) {
                    free(vfb->vnc.passwd);
                    vfb->vnc.passwd = strdup(p2 + 1);
                } else if (!strcmp(p, "vncdisplay")) {
                    vfb->vnc.display = atoi(p2 + 1);
                } else if (!strcmp(p, "vncunused")) {
                    libxl_defbool_set(&vfb->vnc.findunused, atoi(p2 + 1));
                } else if (!strcmp(p, "keymap")) {
                    free(vfb->keymap);
                    vfb->keymap = strdup(p2 + 1);
                } else if (!strcmp(p, "sdl")) {
                    libxl_defbool_set(&vfb->sdl.enable, atoi(p2 + 1));
                } else if (!strcmp(p, "opengl")) {
                    libxl_defbool_set(&vfb->sdl.opengl, atoi(p2 + 1));
                } else if (!strcmp(p, "display")) {
                    free(vfb->sdl.display);
                    vfb->sdl.display = strdup(p2 + 1);
                } else if (!strcmp(p, "xauthority")) {
                    free(vfb->sdl.xauthority);
                    vfb->sdl.xauthority = strdup(p2 + 1);
                }
            } while ((p = strtok(NULL, ",")) != NULL);
skip_vfb:
            free(buf2);
            d_config->num_vfbs++;
            d_config->num_vkbs++;
        }
    }

    if (!xlu_cfg_get_long (config, "pci_msitranslate", &l, 0))
        pci_msitranslate = l;

    if (!xlu_cfg_get_long (config, "pci_power_mgmt", &l, 0))
        pci_power_mgmt = l;

    if (!xlu_cfg_get_long (config, "pci_permissive", &l, 0))
        pci_permissive = l;

    /* To be reworked (automatically enabled) once the auto ballooning
     * after guest starts is done (with PCI devices passed in). */
    if (c_info->type == LIBXL_DOMAIN_TYPE_PV) {
        xlu_cfg_get_defbool(config, "e820_host", &b_info->u.pv.e820_host, 0);
    }

    if (!xlu_cfg_get_list (config, "pci", &pcis, 0, 0)) {
        int i;
        d_config->num_pcidevs = 0;
        d_config->pcidevs = NULL;
        for(i = 0; (buf = xlu_cfg_get_listitem (pcis, i)) != NULL; i++) {
            libxl_device_pci *pcidev;

            d_config->pcidevs = (libxl_device_pci *) realloc(d_config->pcidevs, sizeof (libxl_device_pci) * (d_config->num_pcidevs + 1));
            pcidev = d_config->pcidevs + d_config->num_pcidevs;
            libxl_device_pci_init(pcidev);

            pcidev->msitranslate = pci_msitranslate;
            pcidev->power_mgmt = pci_power_mgmt;
            pcidev->permissive = pci_permissive;
            if (!xlu_pci_parse_bdf(config, pcidev, buf))
                d_config->num_pcidevs++;
        }
        if (d_config->num_pcidevs && c_info->type == LIBXL_DOMAIN_TYPE_PV)
            libxl_defbool_set(&b_info->u.pv.e820_host, true);
    }

    switch (xlu_cfg_get_list(config, "cpuid", &cpuids, 0, 1)) {
    case 0:
        {
            int i;
            const char *errstr;

            for (i = 0; (buf = xlu_cfg_get_listitem(cpuids, i)) != NULL; i++) {
                e = libxl_cpuid_parse_config_xend(&b_info->cpuid, buf);
                switch (e) {
                case 0: continue;
                case 1:
                    errstr = "illegal leaf number";
                    break;
                case 2:
                    errstr = "illegal subleaf number";
                    break;
                case 3:
                    errstr = "missing colon";
                    break;
                case 4:
                    errstr = "invalid register name (must be e[abcd]x)";
                    break;
                case 5:
                    errstr = "policy string must be exactly 32 characters long";
                    break;
                default:
                    errstr = "unknown error";
                    break;
                }
                fprintf(stderr, "while parsing CPUID line: \"%s\":\n", buf);
                fprintf(stderr, "  error #%i: %s\n", e, errstr);
            }
        }
        break;
    case EINVAL:    /* config option is not a list, parse as a string */
        if (!xlu_cfg_get_string(config, "cpuid", &buf, 0)) {
            char *buf2, *p, *strtok_ptr = NULL;
            const char *errstr;

            buf2 = strdup(buf);
            p = strtok_r(buf2, ",", &strtok_ptr);
            if (p == NULL) {
                free(buf2);
                break;
            }
            if (strcmp(p, "host")) {
                fprintf(stderr, "while parsing CPUID string: \"%s\":\n", buf);
                fprintf(stderr, "  error: first word must be \"host\"\n");
                free(buf2);
                break;
            }
            for (p = strtok_r(NULL, ",", &strtok_ptr); p != NULL;
                 p = strtok_r(NULL, ",", &strtok_ptr)) {
                e = libxl_cpuid_parse_config(&b_info->cpuid, p);
                switch (e) {
                case 0: continue;
                case 1:
                    errstr = "missing \"=\" in key=value";
                    break;
                case 2:
                    errstr = "unknown CPUID flag name";
                    break;
                case 3:
                    errstr = "illegal CPUID value (must be: [0|1|x|k|s])";
                    break;
                default:
                    errstr = "unknown error";
                    break;
                }
                fprintf(stderr, "while parsing CPUID flag: \"%s\":\n", p);
                fprintf(stderr, "  error #%i: %s\n", e, errstr);
            }
            free(buf2);
        }
        break;
    default:
        break;
    }

    /* parse device model arguments, this works for pv, hvm and stubdom */
    if (!xlu_cfg_get_string (config, "device_model", &buf, 0)) {
        fprintf(stderr,
                "WARNING: ignoring device_model directive.\n"
                "WARNING: Use \"device_model_override\" instead if you"
                " really want a non-default device_model\n");
        if (strstr(buf, "stubdom-dm")) {
            if (c_info->type == LIBXL_DOMAIN_TYPE_HVM)
                fprintf(stderr, "WARNING: Or use"
                        " \"device_model_stubdomain_override\" if you "
                        " want to enable stubdomains\n");
            else
                fprintf(stderr, "WARNING: ignoring"
                        " \"device_model_stubdomain_override\" directive"
                        " for pv guest\n");
        }
    }


    xlu_cfg_replace_string (config, "device_model_override",
                            &b_info->device_model, 0);
    if (!xlu_cfg_get_string (config, "device_model_version", &buf, 0)) {
        if (!strcmp(buf, "qemu-xen-traditional")) {
            b_info->device_model_version
                = LIBXL_DEVICE_MODEL_VERSION_QEMU_XEN_TRADITIONAL;
        } else if (!strcmp(buf, "qemu-xen")) {
            b_info->device_model_version
                = LIBXL_DEVICE_MODEL_VERSION_QEMU_XEN;
        } else {
            fprintf(stderr,
                    "Unknown device_model_version \"%s\" specified\n", buf);
            exit(1);
        }
    } else if (b_info->device_model)
        fprintf(stderr, "WARNING: device model override given without specific DM version\n");
    xlu_cfg_get_defbool (config, "device_model_stubdomain_override",
                         &b_info->device_model_stubdomain, 0);

    if (!xlu_cfg_get_string (config, "device_model_stubdomain_seclabel",
                             &buf, 0)) {
        e = libxl_flask_context_to_sid(ctx, (char *)buf, strlen(buf),
                                    &b_info->device_model_ssidref);
        if (e) {
            if (errno == ENOSYS) {
                fprintf(stderr, "XSM Disabled:"
                        " device_model_stubdomain_seclabel not supported\n");
            } else {
                fprintf(stderr, "Invalid device_model_stubdomain_seclabel:"
                        " %s\n", buf);
                exit(1);
            }
        }
    }
#define parse_extra_args(type)                                            \
    e = xlu_cfg_get_list_as_string_list(config, "device_model_args"#type, \
                                    &b_info->extra##type, 0);            \
    if (e && e != ESRCH) {                                                \
        fprintf(stderr,"xl: Unable to parse device_model_args"#type".\n");\
        exit(-ERROR_FAIL);                                                \
    }

    /* parse extra args for qemu, common to both pv, hvm */
    parse_extra_args();

    /* parse extra args dedicated to pv */
    parse_extra_args(_pv);

    /* parse extra args dedicated to hvm */
    parse_extra_args(_hvm);

#undef parse_extra_args

    if (c_info->type == LIBXL_DOMAIN_TYPE_HVM) {
        if (!xlu_cfg_get_long(config, "stdvga", &l, 0))
            b_info->u.hvm.vga.kind = l ? LIBXL_VGA_INTERFACE_TYPE_STD :
                                         LIBXL_VGA_INTERFACE_TYPE_CIRRUS;

        xlu_cfg_get_defbool(config, "vnc", &b_info->u.hvm.vnc.enable, 0);
        xlu_cfg_replace_string (config, "vnclisten",
                                &b_info->u.hvm.vnc.listen, 0);
        xlu_cfg_replace_string (config, "vncpasswd",
                                &b_info->u.hvm.vnc.passwd, 0);
        if (!xlu_cfg_get_long (config, "vncdisplay", &l, 0))
            b_info->u.hvm.vnc.display = l;
        xlu_cfg_get_defbool(config, "vncunused",
                            &b_info->u.hvm.vnc.findunused, 0);
        xlu_cfg_replace_string (config, "keymap", &b_info->u.hvm.keymap, 0);
        xlu_cfg_get_defbool(config, "sdl", &b_info->u.hvm.sdl.enable, 0);
        xlu_cfg_get_defbool(config, "opengl", &b_info->u.hvm.sdl.opengl, 0);
        xlu_cfg_get_defbool (config, "spice", &b_info->u.hvm.spice.enable, 0);
        if (!xlu_cfg_get_long (config, "spiceport", &l, 0))
            b_info->u.hvm.spice.port = l;
        if (!xlu_cfg_get_long (config, "spicetls_port", &l, 0))
            b_info->u.hvm.spice.tls_port = l;
        xlu_cfg_replace_string (config, "spicehost",
                                &b_info->u.hvm.spice.host, 0);
        xlu_cfg_get_defbool(config, "spicedisable_ticketing",
                            &b_info->u.hvm.spice.disable_ticketing, 0);
        xlu_cfg_replace_string (config, "spicepasswd",
                                &b_info->u.hvm.spice.passwd, 0);
        xlu_cfg_get_defbool(config, "spiceagent_mouse",
                            &b_info->u.hvm.spice.agent_mouse, 0);
        xlu_cfg_get_defbool(config, "nographic", &b_info->u.hvm.nographic, 0);
        xlu_cfg_get_defbool(config, "gfx_passthru", 
                            &b_info->u.hvm.gfx_passthru, 0);
        xlu_cfg_replace_string (config, "serial", &b_info->u.hvm.serial, 0);
        xlu_cfg_replace_string (config, "boot", &b_info->u.hvm.boot, 0);
        xlu_cfg_get_defbool(config, "usb", &b_info->u.hvm.usb, 0);
        xlu_cfg_replace_string (config, "usbdevice",
                                &b_info->u.hvm.usbdevice, 0);
        xlu_cfg_replace_string (config, "soundhw", &b_info->u.hvm.soundhw, 0);
        xlu_cfg_get_defbool(config, "xen_platform_pci",
                            &b_info->u.hvm.xen_platform_pci, 0);
    }

    xlu_cfg_destroy(config);
}

static void reload_domain_config(uint32_t domid,
                                 uint8_t **config_data, int *config_len)
{
    uint8_t *t_data;
    int ret, t_len;

    ret = libxl_userdata_retrieve(ctx, domid, "xl", &t_data, &t_len);
    if (ret) {
        LOG("failed to retrieve guest configuration (rc=%d). "
            "reusing old configuration", ret);
        return;
    }

    free(*config_data);
    *config_data = t_data;
    *config_len = t_len;
}

/* Returns 1 if domain should be restarted,
 * 2 if domain should be renamed then restarted, or 0
 * Can update r_domid if domain is destroyed etc */
static int handle_domain_death(uint32_t *r_domid,
                               libxl_event *event,
                               uint8_t **config_data, int *config_len,
                               libxl_domain_config *d_config)

{
    int restart = 0;
    libxl_action_on_shutdown action;

    switch (event->u.domain_shutdown.shutdown_reason) {
    case LIBXL_SHUTDOWN_REASON_POWEROFF:
        action = d_config->on_poweroff;
        break;
    case LIBXL_SHUTDOWN_REASON_REBOOT:
        action = d_config->on_reboot;
        break;
    case LIBXL_SHUTDOWN_REASON_SUSPEND:
        LOG("Domain has suspended.");
        return 0;
    case LIBXL_SHUTDOWN_REASON_CRASH:
        action = d_config->on_crash;
        break;
    case LIBXL_SHUTDOWN_REASON_WATCHDOG:
        action = d_config->on_watchdog;
        break;
    default:
        LOG("Unknown shutdown reason code %d. Destroying domain.",
            event->u.domain_shutdown.shutdown_reason);
        action = LIBXL_ACTION_ON_SHUTDOWN_DESTROY;
    }

    LOG("Action for shutdown reason code %d is %s",
        event->u.domain_shutdown.shutdown_reason,
        action_on_shutdown_names[action]);

    if (action == LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_DESTROY || action == LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_RESTART) {
        char *corefile;
        int rc;
        Datetime now;
        string now_str = now.serialize();
        string::size_type loc = now_str.find(" ");
        if (loc != string::npos)
        {
            now_str.replace(loc,1,"_");
        }
        if (asprintf(&corefile, "/var/lib/xen/dump/%s/%s-%s.core", 
                          d_config->c_info.name,now_str.c_str(),d_config->c_info.name) < 0) {
            LOG("failed to construct core dump path");
        } else {
            LOG("dumping core to %s", corefile);
            rc=libxl_domain_core_dump(ctx, *r_domid, corefile, NULL);
            if (rc) LOG("core dump failed (rc=%d).", rc);
        }
        /* No point crying over spilled milk, continue on failure. */

        if (action == LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_DESTROY)
            action = LIBXL_ACTION_ON_SHUTDOWN_DESTROY;
        else
            action = LIBXL_ACTION_ON_SHUTDOWN_RESTART;
    }

    switch (action) {
    case LIBXL_ACTION_ON_SHUTDOWN_PRESERVE:
        break;

    case LIBXL_ACTION_ON_SHUTDOWN_RESTART_RENAME:
        reload_domain_config(*r_domid, config_data, config_len);
        restart = 2;
        break;

    case LIBXL_ACTION_ON_SHUTDOWN_RESTART:
        reload_domain_config(*r_domid, config_data, config_len);

        restart = 1;
        /* fall-through */
    case LIBXL_ACTION_ON_SHUTDOWN_DESTROY:
        LOG("Domain %d needs to be cleaned up: destroying the domain",
            *r_domid);
        libxl_domain_destroy(ctx, *r_domid, 0);
        *r_domid = INVALID_DOMID;
        break;

    case LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_DESTROY:
    case LIBXL_ACTION_ON_SHUTDOWN_COREDUMP_RESTART:
        /* Already handled these above. */
        abort();
    }

    return restart;
}

/* for now used only by main_networkattach, but can be reused elsewhere */
static int match_option_size(const char *prefix, size_t len,
        char *arg, char **argopt)
{
    int rc = strncmp(prefix, arg, len);
    if (!rc) *argopt = arg+len;
    return !rc;
}
#define MATCH_OPTION(prefix, arg, oparg) \
    match_option_size((prefix "="), sizeof((prefix)), (arg), &(oparg))

static void replace_string(char **str, const char *val)
{
    free(*str);
    *str = strdup(val);
}


/* Preserve a copy of a domain under a new name. Updates *r_domid */
static int preserve_domain(uint32_t *r_domid, libxl_event *event,
                           libxl_domain_config *d_config)
{
    time_t now;
    struct tm tm;
    char stime[24];

    libxl_uuid new_uuid;

    int rc;

    now = time(NULL);
    if (now == ((time_t) -1)) {
        LOG("Failed to get current time for domain rename");
        return 0;
    }

    tzset();
    if (gmtime_r(&now, &tm) == NULL) {
        LOG("Failed to convert time to UTC");
        return 0;
    }

    if (!strftime(&stime[0], sizeof(stime), "-%Y%m%dT%H%MZ", &tm)) {
        LOG("Failed to format time as a string");
        return 0;
    }

    libxl_uuid_generate(&new_uuid);

    LOG("Preserving domain %d %s with suffix%s",
        *r_domid, d_config->c_info.name, stime);
    rc = libxl_domain_preserve(ctx, *r_domid, &d_config->c_info,
                               stime, new_uuid);

    /*
     * Although the domain still exists it is no longer the one we are
     * concerned with.
     */
    *r_domid = INVALID_DOMID;

    return rc == 0 ? 1 : 0;
}

static int freemem(libxl_domain_build_info *b_info)
{
    int rc, retries = 3;
    uint32_t need_memkb, free_memkb;

    if (!autoballoon)
        return 0;

    rc = libxl_domain_need_memory(ctx, b_info, &need_memkb);
    if (rc < 0)
        return rc;

    do {
        rc = libxl_get_free_memory(ctx, &free_memkb);
        if (rc < 0)
            return rc;

        if (free_memkb >= need_memkb)
            return 0;

        rc = libxl_set_memory_target(ctx, 0, free_memkb - need_memkb, 1, 0);
        if (rc < 0)
            return rc;

        rc = libxl_wait_for_free_memory(ctx, domid, need_memkb, 10);
        if (!rc)
            return 0;
        else if (rc != ERROR_NOMEM)
            return rc;

        /* the memory target has been reached but the free memory is still
         * not enough: loop over again */
        rc = libxl_wait_for_memory_target(ctx, 0, 1);
        if (rc < 0)
            return rc;

        retries--;
    } while (retries > 0);

    return ERROR_NOMEM;
}

static void console_child_report(void)
{
    if (xl_child_pid(child_console)) {
        int status;
        pid_t got = xl_waitpid(child_console, &status, 0);
        if (got < 0)
            perror("xl: warning, failed to waitpid for console child");
        else if (status)
            libxl_report_child_exitstatus(ctx, XTL_ERROR, "console child",
                                          xl_child_pid(child_console), status);
    }
}

static int domain_wait_event(libxl_event **event_r, int domid)
{
    int ret;
    for (;;) {
        ret = libxl_event_wait(ctx, event_r, LIBXL_EVENTMASK_ALL, 0,0);
        if (ret) {
            LOG("Domain %d, failed to get event, quitting (rc=%d)", domid, ret);
            return ret;
        }
        if ((*event_r)->domid != domid) {
            char *evstr = libxl_event_to_json(ctx, *event_r);
            LOG("INTERNAL PROBLEM - ignoring unexpected event for"
                " domain %d (expected %d): event=%s",
                (*event_r)->domid, domid, evstr);
            free(evstr);
            libxl_event_free(ctx, *event_r);
            continue;
        }
        return ret;
    }
}

static void evdisable_disk_ejects(libxl_evgen_disk_eject **diskws,
                                 int num_disks)
{
    int i;

    for (i = 0; i < num_disks; i++) {
        if (diskws[i])
            libxl_evdisable_disk_eject(ctx, diskws[i]);
        diskws[i] = NULL;
    }
}

static int create_domain(struct domain_create *dom_info, uint32_t *pdomid)
{
    libxl_domain_config d_config;

//    int debug = dom_info->debug;
    int monitor = dom_info->monitor;
    int paused = dom_info->paused;
    int daemonize = dom_info->daemonize;
    int vncautopass = dom_info->vncautopass;
    const char *config_file = dom_info->config_file;
    const char *extra_config = dom_info->extra_config;
    const char *restore_file = dom_info->restore_file;
    const char *config_source = NULL;
    const char *restore_source = NULL;
    int migrate_fd = dom_info->migrate_fd;

    int i;
    int ret, rc;
    int need_daemon = daemonize;
    libxl_evgen_domain_death *deathw = NULL;
    libxl_evgen_disk_eject **diskws = NULL; /* one per disk */
    void *config_data = 0;
    void *q = 0;
    int config_len = 0;
    int restore_fd = -1;
    int status = 0;
    const libxl_asyncprogress_how *autoconnect_console_how;
    struct save_file_header hdr;

    int restoring = (restore_file || (migrate_fd >= 0));

    libxl_domain_config_init(&d_config);

    if (restoring) {
        uint8_t *optdata_begin = 0;
        const uint8_t *optdata_here = 0;
        union { uint32_t u32; char b[4]; } u32buf;
        uint32_t badflags;

        if (migrate_fd >= 0) {
            restore_source = "<incoming migration stream>";
            restore_fd = migrate_fd;
        } else {
            restore_source = restore_file;
            restore_fd = open(restore_file, O_RDONLY);
            rc = libxl_fd_set_cloexec(ctx, restore_fd, 1);
            if (rc) return rc;
        }

        CHK_ERRNO( libxl_read_exactly(ctx, restore_fd, &hdr,
                   sizeof(hdr), restore_source, "header") );
        if (memcmp(hdr.magic, savefileheader_magic, sizeof(hdr.magic))) {
            fprintf(stderr, "File has wrong magic number -"
                    " corrupt or for a different tool?\n");
            return ERROR_INVAL;
        }
        if (hdr.byteorder != SAVEFILE_BYTEORDER_VALUE) {
            fprintf(stderr, "File has wrong byte order\n");
            return ERROR_INVAL;
        }
        fprintf(stderr, "Loading new save file %s"
                " (new xl fmt info"
                " 0x%"PRIx32"/0x%"PRIx32"/%"PRIu32")\n",
                restore_source, hdr.mandatory_flags, hdr.optional_flags,
                hdr.optional_data_len);

        badflags = hdr.mandatory_flags & ~( 0 /* none understood yet */ );
        if (badflags) {
            fprintf(stderr, "Savefile has mandatory flag(s) 0x%"PRIx32" "
                    "which are not supported; need newer xl\n",
                    badflags);
            return ERROR_INVAL;
        }
        if (hdr.optional_data_len) {
            optdata_begin = (uint8_t*)malloc(hdr.optional_data_len);
            CHK_ERRNO( libxl_read_exactly(ctx, restore_fd, optdata_begin,
                   hdr.optional_data_len, restore_source, "optdata") );
        }

#define OPTDATA_LEFT  (hdr.optional_data_len - (optdata_here - optdata_begin))
#define WITH_OPTDATA(amt, body)                                 \
            if (OPTDATA_LEFT < (amt)) {                         \
                fprintf(stderr, "Savefile truncated.\n");       \
                return ERROR_INVAL;                             \
            } else {                                            \
                body;                                           \
                optdata_here += (amt);                          \
            }

        optdata_here = optdata_begin;

        if (OPTDATA_LEFT) {
            fprintf(stderr, " Savefile contains xl domain config\n");
            WITH_OPTDATA(4, {
                memcpy(u32buf.b, optdata_here, 4);
                config_len = u32buf.u32;
            });
            WITH_OPTDATA(config_len, {
                config_data = malloc(config_len);
                memcpy(config_data, optdata_here, config_len);
            });
        }

    }

    if (config_file) {
        free(config_data);  config_data = 0;
        /* /dev/null represents special case (read config. from command line) */
        if (!strcmp(config_file, "/dev/null")) {
            config_len = 0;
        } else {
            ret = libxl_read_file_contents(ctx, config_file,
                                           &config_data, &config_len);
            if (ret) { fprintf(stderr, "Failed to read config file: %s: %s\n",
                               config_file, strerror(errno)); return ERROR_FAIL; }
        }
        if (!restoring && extra_config && strlen(extra_config)) {
            if (config_len > INT_MAX - (strlen(extra_config) + 2 + 1)) {
                fprintf(stderr, "Failed to attach extra configration\n");
                return ERROR_FAIL;
            }
            /* allocate space for the extra config plus two EOLs plus \0 */
            q = config_data;
            config_data = realloc(q, config_len
                + strlen(extra_config) + 2 + 1);
            if (!config_data) {
                fprintf(stderr, "Failed to realloc config_data\n");
                free(q);
                return ERROR_FAIL;
            }
            config_len += sprintf((char*)config_data + config_len, "\n%s\n",
                extra_config);
        }
        config_source=config_file;
    } else {
        if (!config_data) {
            fprintf(stderr, "Config file not specified and"
                    " none in save file\n");
            return ERROR_INVAL;
        }
        config_source = "<saved>";
    }

    if (!dom_info->quiet)
        printf("Parsing config from %s\n", config_source);

    parse_config_data(config_source, (const char*)config_data, config_len, &d_config, dom_info);

    if (migrate_fd >= 0) {
        if (d_config.c_info.name) {
            /* when we receive a domain we get its name from the config
             * file; and we receive it to a temporary name */

            common_domname = d_config.c_info.name;
            d_config.c_info.name = 0; /* steals allocation from config */

            if (asprintf(&d_config.c_info.name,
                         "%s--incoming", common_domname) < 0) {
                fprintf(stderr, "Failed to allocate memory in asprintf\n");
                exit(1);
            }
            *dom_info->migration_domname_r = strdup(d_config.c_info.name);
        }
    }

    //if (debug || dom_info->dryrun)
        //printf_info(default_output_format, -1, &d_config);

    ret = 0;
    if (dom_info->dryrun)
        goto out;

start:

    rc = acquire_lock();
    if (rc < 0)
        goto error_out;

    ret = freemem(&d_config.b_info);
    if (ret < 0) {
        fprintf(stderr, "failed to free memory for the domain\n");
        ret = ERROR_FAIL;
        goto error_out;
    }

//    libxl_asyncprogress_how autoconnect_console_how_buf;
    if ( dom_info->console_autoconnect ) {
//        autoconnect_console_how_buf.callback = autoconnect_console;
//        autoconnect_console_how = &autoconnect_console_how_buf;
        autoconnect_console_how = 0;
    }else{
        autoconnect_console_how = 0;
    }

    if ( restoring ) {
        ret = libxl_domain_create_restore(ctx, &d_config,
                                          pdomid, restore_fd,
                                          0, autoconnect_console_how);
        /*
         * On subsequent reboot etc we should create the domain, not
         * restore/migrate-receive it again.
         */
        restore_file = NULL;
    }else{
        ret = libxl_domain_create_new(ctx, &d_config, pdomid,
                                      0, autoconnect_console_how);
    }
    if ( ret )
        goto error_out;

    /* If single vcpu to pcpu mapping was requested, honour it */
    if (vcpu_to_pcpu) {
        libxl_bitmap vcpu_cpumap;

        ret = libxl_cpu_bitmap_alloc(ctx, &vcpu_cpumap, 0);
        if (ret)
            goto error_out;
        for (i = 0; i < d_config.b_info.max_vcpus; i++) {

            if (vcpu_to_pcpu[i] != -1) {
                libxl_bitmap_set_none(&vcpu_cpumap);
                libxl_bitmap_set(&vcpu_cpumap, vcpu_to_pcpu[i]);
            } else {
                libxl_bitmap_set_any(&vcpu_cpumap);
            }
            if (libxl_set_vcpuaffinity(ctx, *pdomid, i, &vcpu_cpumap)) {
                fprintf(stderr, "setting affinity failed on vcpu `%d'.\n", i);
                libxl_bitmap_dispose(&vcpu_cpumap);
                free(vcpu_to_pcpu);
                ret = ERROR_FAIL;
                goto error_out;
            }
        }
        libxl_bitmap_dispose(&vcpu_cpumap);
        free(vcpu_to_pcpu); vcpu_to_pcpu = NULL;
    }

    ret = libxl_userdata_store(ctx, *pdomid, "xl",
                                    (const uint8_t*)config_data, config_len);
    if (ret) {
        perror("cannot save config file");
        ret = ERROR_FAIL;
        goto error_out;
    }

    release_lock();

    if (!paused)
        libxl_domain_unpause(ctx, *pdomid);

    ret = *pdomid; /* caller gets success in parent */
    if (!daemonize && !monitor)
        goto out;

    if (dom_info->vnc)
        vncviewer(*pdomid, vncautopass);

    if (need_daemon) {
        char *fullname, *name;
        pid_t child1, got_child;
        int nullfd;

        child1 = xl_fork(child_waitdaemon);
        if (child1) {
            printf("Daemon running with PID %d\n", child1);

            for (;;) {
                got_child = xl_waitpid(child_waitdaemon, &status, 0);
                if (got_child == child1) break;
                assert(got_child == -1);
                perror("failed to wait for daemonizing child");
                ret = ERROR_FAIL;
                goto out;
            }
            if (status) {
                libxl_report_child_exitstatus(ctx, XTL_ERROR,
                           "daemonizing child", child1, status);
                ret = ERROR_FAIL;
                goto out;
            }
            ret = *pdomid;
            goto out;
        }

        postfork();

        if (asprintf(&name, "xl-%s", d_config.c_info.name) < 0) {
            LOG("Failed to allocate memory in asprintf");
            exit(1);
        }
        rc = libxl_create_logfile(ctx, name, &fullname);
        if (rc) {
            LOG("failed to open logfile %s: %s",fullname,strerror(errno));
            exit(-1);
        }

        CHK_ERRNO(( logfile = open(fullname, O_WRONLY|O_CREAT|O_APPEND,
                                   0644) )<0);
        free(fullname);
        free(name);

        CHK_ERRNO(( nullfd = open("/dev/null", O_RDONLY) )<0);
        dup2(nullfd, 0);
        dup2(logfile, 1);
        dup2(logfile, 2);

        CHK_ERRNO(daemon(0, 1) < 0);
        need_daemon = 0;
    }
    
    LOG("Waiting for domain %s (domid %d) to die [pid %ld]",
        d_config.c_info.name, *pdomid, (long)getpid());

    ret = libxl_evenable_domain_death(ctx, *pdomid, 0, &deathw);
    if (ret) goto out;

    if (!diskws) {
        diskws = (libxl_evgen_disk_eject **)malloc(sizeof(*diskws) * d_config.num_disks);
        for (i = 0; i < d_config.num_disks; i++)
            diskws[i] = NULL;
    }
    for (i = 0; i < d_config.num_disks; i++) {
        if (d_config.disks[i].removable) {
            ret = libxl_evenable_disk_eject(ctx, *pdomid, d_config.disks[i].vdev,
                                            0, &diskws[i]);
            if (ret) goto out;
        }
    }
    while (1) {
        libxl_event *event;
        ret = domain_wait_event(&event, *pdomid);
        if (ret) goto out;

        switch (event->type) {

        case LIBXL_EVENT_TYPE_DOMAIN_SHUTDOWN:
            LOG("Domain %d has shut down, reason code %d 0x%x", *pdomid,
                event->u.domain_shutdown.shutdown_reason,
                event->u.domain_shutdown.shutdown_reason);
            switch (handle_domain_death(pdomid, event,
                                        (uint8_t **)&config_data, &config_len,
                                        &d_config)) {
            case 2:
                if (!preserve_domain(pdomid, event, &d_config)) {
                    /* If we fail then exit leaving the old domain in place. */
                    ret = -1;
                    goto out;
                }

                /* Otherwise fall through and restart. */
            case 1:
                libxl_event_free(ctx, event);
                libxl_evdisable_domain_death(ctx, deathw);
                deathw = NULL;
                evdisable_disk_ejects(diskws, d_config.num_disks);
                /* discard any other events which may have been generated */
                while (!(ret = libxl_event_check(ctx, &event,
                                                 LIBXL_EVENTMASK_ALL, 0,0))) {
                    libxl_event_free(ctx, event);
                }
                if (ret != ERROR_NOT_READY) {
                    LOG("warning, libxl_event_check (cleanup) failed (rc=%d)",
                        ret);
                }

                /*
                 * Do not attempt to reconnect if we come round again due to a
                 * guest reboot -- the stdin/out will be disconnected by then.
                 */
                dom_info->console_autoconnect = 0;

                /* Some settings only make sense on first boot. */
                paused = 0;
                if (common_domname
                    && strcmp(d_config.c_info.name, common_domname)) {
                    d_config.c_info.name = strdup(common_domname);
                }

                /* Reparse the configuration in case it has changed */
                libxl_domain_config_dispose(&d_config);
                libxl_domain_config_init(&d_config);
                parse_config_data(config_source, (const char *)config_data, config_len,
                                  &d_config, dom_info);

                /*
                 * XXX FIXME: If this sleep is not there then domain
                 * re-creation fails sometimes.
                 */
                LOG("Done. Rebooting now");
                sleep(2);
                goto start;

            case 0:
                LOG("Done. Exiting now");
                ret = 0;
                goto out;

            default:
                abort();
            }

        case LIBXL_EVENT_TYPE_DOMAIN_DEATH:
            LOG("Domain %d has been destroyed.", *pdomid);
            ret = 0;
            goto out;

        case LIBXL_EVENT_TYPE_DISK_EJECT:
            /* XXX what is this for? */
            libxl_cdrom_insert(ctx, *pdomid, &event->u.disk_eject.disk, NULL);
            break;

        default:;
            char *evstr = libxl_event_to_json(ctx, event);
            LOG("warning, got unexpected event type %d, event=%s",
                event->type, evstr);
            free(evstr);
        }

        libxl_event_free(ctx, event);
    }

error_out:
    release_lock();
    if (libxl_domid_valid_guest(*pdomid)) {
        libxl_domain_destroy(ctx, *pdomid, 0);
        *pdomid = INVALID_DOMID;
    }

out:
    if (logfile != 2)
        close(logfile);

    libxl_domain_config_dispose(&d_config);

    free(config_data);

    console_child_report();

    if (deathw)
        libxl_evdisable_domain_death(ctx, deathw);
    if (diskws) {
        evdisable_disk_ejects(diskws, d_config.num_disks);
        free(diskws);
    }

    /*
     * If we have daemonized then do not return to the caller -- this has
     * already happened in the parent.
     */
    if ( daemonize && !need_daemon )
        exit(ret);

    return ret;
}


static int64_t parse_mem_size_kb(const char *mem)
{
    char *endptr;
    int64_t kbytes;

    kbytes = strtoll(mem, &endptr, 10);

    if (strlen(endptr) > 1)
        return -1;

    switch (tolower((uint8_t)*endptr)) {
    case 't':
        kbytes <<= 10;
    case 'g':
        kbytes <<= 10;
    case '\0':
    case 'm':
        kbytes <<= 10;
    case 'k':
        break;
    case 'b':
        kbytes >>= 10;
        break;
    default:
        return -1;
    }

    return kbytes;
}

static int def_getopt(int argc, char * const argv[], const char *optstring,
                      const char* helpstr, int reqargs)
{
    int opt;

    opterr = 0;
    while ((opt = getopt(argc, argv, optstring)) == '?') {
        if (optopt == 'h') {
            fprintf(stderr, "option `%c' not supported.\n", optopt);
            return 0;
        }
        fprintf(stderr, "option `%c' not supported.\n", optopt);
    }
    if (opt != -1)
        return opt;

    if (argc - optind <= reqargs - 1) {
        fprintf(stderr, "'xl %s' requires at least %d argument%s.\n\n",
                helpstr, reqargs, reqargs > 1 ? "s" : "");
        return 2;
    }
    return -1;
}

static int set_memory_max(const char *p, const char *mem)
{
    int64_t memorykb;
    int rc;

    find_domain(p);

    memorykb = parse_mem_size_kb(mem);
    if (memorykb == -1) {
        fprintf(stderr, "invalid memory size: %s\n", mem);
        exit(3);
    }

    rc = libxl_domain_setmaxmem(ctx, domid, memorykb);

    return rc;
}

int main_memmax(int argc, char **argv)
{
    int opt = 0;
    char *p = NULL, *mem;
    int rc;

    if ((opt = def_getopt(argc, argv, "", "mem-max", 2)) != -1)
        return opt;

    p = argv[optind];
    mem = argv[optind + 1];

    rc = set_memory_max(p, mem);
    if (rc) {
        fprintf(stderr, "cannot set domid %d static max memory to : %s\n", domid, mem);
        return 1;
    }

    return 0;
}

static void set_memory_target(const char *p, const char *mem)
{
    long long int memorykb;

    find_domain(p);

    memorykb = parse_mem_size_kb(mem);
    if (memorykb == -1)  {
        fprintf(stderr, "invalid memory size: %s\n", mem);
        exit(3);
    }

    libxl_set_memory_target(ctx, domid, memorykb, 0, /* enforce */ 1);
}

int main_memset(int argc, char **argv)
{
    int opt = 0;
    const char *p = NULL, *mem;

    if ((opt = def_getopt(argc, argv, "", "mem-set", 2)) != -1)
        return opt;

    p = argv[optind];
    mem = argv[optind + 1];

    set_memory_target(p, mem);
    return 0;
}

static void cd_insert(const char *dom, const char *virtdev, char *phys)
{
    libxl_device_disk disk; /* we don't free disk's contents */
    char *buf = NULL;
    XLU_Config *config = 0;

    find_domain(dom);

    if (asprintf(&buf, "vdev=%s,access=r,devtype=cdrom,target=%s",
                 virtdev, phys ? phys : "") < 0) {
        fprintf(stderr, "out of memory\n");
        return;
    }

    parse_disk_config(&config, buf, &disk);

    disk.backend_domid = 0;

    libxl_cdrom_insert(ctx, domid, &disk, NULL);

    libxl_device_disk_dispose(&disk);
    free(buf);
}

int main_cd_eject(int argc, char **argv)
{
    int opt = 0;
    const char *p = NULL, *virtdev;

    if ((opt = def_getopt(argc, argv, "", "cd-eject", 2)) != -1)
        return opt;

    p = argv[optind];
    virtdev = argv[optind + 1];

    cd_insert(p, virtdev, NULL);
    return 0;
}

int main_cd_insert(int argc, char **argv)
{
    int opt = 0;
    const char *p = NULL, *virtdev;
    char *file = NULL; /* modified by cd_insert tokenising it */

    if ((opt = def_getopt(argc, argv, "", "cd-insert", 3)) != -1)
        return opt;

    p = argv[optind];
    virtdev = argv[optind + 1];
    file = argv[optind + 2];

    cd_insert(p, virtdev, file);
    return 0;
}


static void pcilist(const char *dom)
{
    libxl_device_pci *pcidevs;
    int num, i;

    find_domain(dom);

    pcidevs = libxl_device_pci_list(ctx, domid, &num);
    if (pcidevs == NULL)
        return;
    printf("Vdev Device\n");
    for (i = 0; i < num; i++) {
        printf("%02x.%01x %04x:%02x:%02x.%01x\n",
               (pcidevs[i].vdevfn >> 3) & 0x1f, pcidevs[i].vdevfn & 0x7,
               pcidevs[i].domain, pcidevs[i].bus, pcidevs[i].dev, pcidevs[i].func);
        libxl_device_pci_dispose(&pcidevs[i]);
    }
    free(pcidevs);
}

int main_pcilist(int argc, char **argv)
{
    int opt;
    const char *domname = NULL;

    if ((opt = def_getopt(argc, argv, "", "pci-list", 1)) != -1)
        return opt;

    domname = argv[optind];

    pcilist(domname);
    return 0;
}

static void pcidetach(const char *dom, const char *bdf, int force)
{
    libxl_device_pci pcidev;
    XLU_Config *config;

    find_domain(dom);

    libxl_device_pci_init(&pcidev);
    
    config = xlu_cfg_init(stderr, "command line");
    if (!config) { perror("xlu_cfg_inig"); exit(-1); }

    if (xlu_pci_parse_bdf(config, &pcidev, bdf)) {
        fprintf(stderr, "pci-detach: malformed BDF specification \"%s\"\n", bdf);
        exit(2);
    }
    if (force)
        libxl_device_pci_destroy(ctx, domid, &pcidev, 0);
    else
        libxl_device_pci_remove(ctx, domid, &pcidev, 0);

    libxl_device_pci_dispose(&pcidev);
    xlu_cfg_destroy(config);
}

int main_pcidetach(int argc, char **argv)
{
    int opt;
    int force = 0;
    const char *domname = NULL, *bdf = NULL;

    while ((opt = def_getopt(argc, argv, "f", "pci-detach", 2)) != -1) {
        switch (opt) {
        case 0: case 2:
            return opt;
        case 'f':
            force = 1;
            break;
        }
    }

    domname = argv[optind];
    bdf = argv[optind + 1];

    pcidetach(domname, bdf, force);
    return 0;
}
static void pciattach(const char *dom, const char *bdf, const char *vs)
{
    libxl_device_pci pcidev;
    XLU_Config *config;

    find_domain(dom);

    libxl_device_pci_init(&pcidev);

    config = xlu_cfg_init(stderr, "command line");
    if (!config) { perror("xlu_cfg_inig"); exit(-1); }

    if (xlu_pci_parse_bdf(config, &pcidev, bdf)) {
        fprintf(stderr, "pci-attach: malformed BDF specification \"%s\"\n", bdf);
        exit(2);
    }
    libxl_device_pci_add(ctx, domid, &pcidev, 0);

    libxl_device_pci_dispose(&pcidev);
    xlu_cfg_destroy(config);
}

int main_pciattach(int argc, char **argv)
{
    int opt;
    const char *domname = NULL, *bdf = NULL, *vs = NULL;

    if ((opt = def_getopt(argc, argv, "", "pci-attach", 2)) != -1)
        return opt;

    domname = argv[optind];
    bdf = argv[optind + 1];

    if (optind + 1 < argc)
        vs = argv[optind + 2];

    pciattach(domname, bdf, vs);
    return 0;
}

static void pciassignable_list(void)
{
    libxl_device_pci *pcidevs;
    int num, i;

    pcidevs = libxl_device_pci_assignable_list(ctx, &num);

    if ( pcidevs == NULL )
        return;
    for (i = 0; i < num; i++) {
        printf("%04x:%02x:%02x.%01x\n",
               pcidevs[i].domain, pcidevs[i].bus, pcidevs[i].dev, pcidevs[i].func);
        libxl_device_pci_dispose(&pcidevs[i]);
    }
    free(pcidevs);
}

int main_pciassignable_list(int argc, char **argv)
{
    int opt;

    if ((opt = def_getopt(argc, argv, "", "pci-assignable-list", 0)) != -1)
        return opt;

    pciassignable_list();
    return 0;
}

static void pciassignable_add(const char *bdf, int rebind)
{
    libxl_device_pci pcidev;
    XLU_Config *config;

    libxl_device_pci_init(&pcidev);

    config = xlu_cfg_init(stderr, "command line");
    if (!config) { perror("xlu_cfg_init"); exit(-1); }

    if (xlu_pci_parse_bdf(config, &pcidev, bdf)) {
        fprintf(stderr, "pci-assignable-add: malformed BDF specification \"%s\"\n", bdf);
        exit(2);
    }
    libxl_device_pci_assignable_add(ctx, &pcidev, rebind);

    libxl_device_pci_dispose(&pcidev);
    xlu_cfg_destroy(config);
}

int main_pciassignable_add(int argc, char **argv)
{
    int opt;
    const char *bdf = NULL;

    while ((opt = def_getopt(argc, argv, "", "pci-assignable-add", 1)) != -1) {
        switch (opt) {
        case 0: case 2:
            return opt;
        }
    }

    bdf = argv[optind];

    pciassignable_add(bdf, 1);
    return 0;
}

static void pciassignable_remove(const char *bdf, int rebind)
{
    libxl_device_pci pcidev;
    XLU_Config *config;

    libxl_device_pci_init(&pcidev);

    config = xlu_cfg_init(stderr, "command line");
    if (!config) { perror("xlu_cfg_init"); exit(-1); }

    if (xlu_pci_parse_bdf(config, &pcidev, bdf)) {
        fprintf(stderr, "pci-assignable-remove: malformed BDF specification \"%s\"\n", bdf);
        exit(2);
    }
    libxl_device_pci_assignable_remove(ctx, &pcidev, rebind);

    libxl_device_pci_dispose(&pcidev);
    xlu_cfg_destroy(config);
}

int main_pciassignable_remove(int argc, char **argv)
{
    int opt;
    const char *bdf = NULL;
    int rebind = 0;

    while ((opt = def_getopt(argc, argv, "r", "pci-assignable-remove", 1)) != -1) {
        switch (opt) {
        case 0: case 2:
            return opt;
        case 'r':
            rebind=1;
            break;
        }
    }

    bdf = argv[optind];

    pciassignable_remove(bdf, rebind);
    return 0;
}

int pause_domain(int domid)
{
    libxl_domain_pause(ctx, domid);
    return 0;
}

int unpause_domain(int domid)
{
    libxl_domain_unpause(ctx, domid);
    return 0;
}

static int destroy_domain(int domid)
{
    int rc;
    if (domid == 0) {
        fprintf(stderr, "Cannot destroy privileged domain 0.\n\n");
        return(-1);
    }

    // destroy的时候，先释放，然后申请，不然异常，具体
    // 原因不明
    libxl_ctx_free(ctx);
    ctx = NULL;
    if (0 != xl_ctx_alloc())
    {
        fprintf(stderr, "xl_ctx_alloc fail.\n\n");
        return -1;
    }
    
    rc = libxl_domain_destroy(ctx, domid, 0);
    if (rc) { fprintf(stderr,"destroy failed (rc=%d)\n",rc); return(-1); }
}

int shutdown_domain(int domid, int wait, int fallback_trigger)
{
    int rc;
    libxl_event *event;

    rc=libxl_domain_shutdown(ctx, domid);
    if (rc == ERROR_NOPARAVIRT) {
        if (fallback_trigger) {
            fprintf(stderr, "PV control interface not available:"
                    " sending ACPI power button event.\n");
            rc = libxl_send_trigger(ctx, domid, LIBXL_TRIGGER_POWER, 0);
        } else {
            fprintf(stderr, "PV control interface not available:"
                    " external graceful shutdown not possible.\n");
            fprintf(stderr, "Use \"-F\" to fallback to ACPI power event.\n");
        }
    }
    if (rc) {
        fprintf(stderr,"shutdown failed (rc=%d)\n",rc);return(-1);
    }

    if (wait) {
        libxl_evgen_domain_death *deathw;

        rc = libxl_evenable_domain_death(ctx, domid, 0, &deathw);
        if (rc) {
            fprintf(stderr,"wait for death failed (evgen, rc=%d)\n",rc);
            return(-1);
        }

        for (;;) {
            rc = domain_wait_event(&event, domid);
            if (rc) return(-1);

            switch (event->type) {

            case LIBXL_EVENT_TYPE_DOMAIN_DEATH:
                LOG("Domain %d has been destroyed", domid);
                goto done;

            case LIBXL_EVENT_TYPE_DOMAIN_SHUTDOWN:
                LOG("Domain %d has been shut down, reason code %d %x", domid,
                    event->u.domain_shutdown.shutdown_reason,
                    event->u.domain_shutdown.shutdown_reason);
                goto done;

            default:
                LOG("Unexpected event type %d", event->type);
                break;
            }
            libxl_event_free(ctx, event);
        }
    done:
        libxl_event_free(ctx, event);
        libxl_evdisable_domain_death(ctx, deathw);
        return rc;
    }
}

int reboot_domain(int domid )
{
    int rc;
    // 这个是外部应该传过来的参数，移植放到内部
    int fallback_trigger = 0;  
    rc=libxl_domain_reboot(ctx, domid);
    if (rc == ERROR_NOPARAVIRT) {
        if (fallback_trigger) {
            fprintf(stderr, "PV control interface not available:"
                    " sending ACPI reset button event.\n");
            rc = libxl_send_trigger(ctx, domid, LIBXL_TRIGGER_RESET, 0);
        } else {
            fprintf(stderr, "PV control interface not available:"
                    " external graceful reboot not possible.\n");
            fprintf(stderr, "Use \"-F\" to fallback to ACPI reset event.\n");
        }
    }
    if (rc) {
        fprintf(stderr,"reboot failed (rc=%d)\n",rc);return -1;
    }
}


int list_vm(int* ids)
{
    libxl_vminfo *info;
    int nb_vm, i;

    info = libxl_list_vm(ctx, &nb_vm);

    if (info < 0) {
        fprintf(stderr, "libxl_domain_infolist failed.\n");
        return -1;
    }
    for (i = 0; i < nb_vm; i++) {
        ids[i] =  info[i].domid;
    }
    libxl_vminfo_list_free(info, nb_vm);
    return 0;
}

static void save_domain_core_begin(int domid,
                                   const char *override_config_file,
                                   uint8_t **config_data_r,
                                   int *config_len_r)
{
    int rc;

    /* configuration file in optional data: */

    if (override_config_file) {
        void *config_v = 0;
        rc = libxl_read_file_contents(ctx, override_config_file,
                                      &config_v, config_len_r);
        *config_data_r = (uint8_t*)config_v;
    } else {
        rc = libxl_userdata_retrieve(ctx, domid, "xl",
                                     config_data_r, config_len_r);
    }
    if (rc) {
        fputs("Unable to get config file\n",stderr);
        exit(2);
    }
}

static void save_domain_core_writeconfig(int fd, const char *source,
                                  const uint8_t *config_data, int config_len)
{
    struct save_file_header hdr;
    uint8_t *optdata_begin;
    union { uint32_t u32; char b[4]; } u32buf;

    memset(&hdr, 0, sizeof(hdr));
    memcpy(hdr.magic, savefileheader_magic, sizeof(hdr.magic));
    hdr.byteorder = SAVEFILE_BYTEORDER_VALUE;

    optdata_begin= 0;

#define ADD_OPTDATA(ptr, len) ({                                            \
    if ((len)) {                                                        \
        hdr.optional_data_len += (len);                                 \
        optdata_begin = (uint8_t*)xrealloc(optdata_begin, hdr.optional_data_len); \
        memcpy(optdata_begin + hdr.optional_data_len - (len),           \
               (ptr), (len));                                           \
    }                                                                   \
                          })

    u32buf.u32 = config_len;
    ADD_OPTDATA(u32buf.b,    4);
    ADD_OPTDATA(config_data, config_len);

    /* that's the optional data */

    CHK_ERRNO( libxl_write_exactly(ctx, fd,
        &hdr, sizeof(hdr), source, "header") );
    CHK_ERRNO( libxl_write_exactly(ctx, fd,
        optdata_begin, hdr.optional_data_len, source, "header") );

    fprintf(stderr, "Saving to %s new xl format (info"
            " 0x%"PRIx32"/0x%"PRIx32"/%"PRIu32")\n",
            source, hdr.mandatory_flags, hdr.optional_flags,
            hdr.optional_data_len);
}

static int save_domain(const char *p, const char *filename, int checkpoint,
                const char *override_config_file)
{
    int fd;
    uint8_t *config_data;
    int config_len = 0;
// 没有使用上，临时注释
//    save_domain_core_begin(p, override_config_file, &config_data, &config_len);

    if (!config_len) {
        fputs(" Savefile will not contain xl domain config\n", stderr);
    }

    fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0644);
    if (fd < 0) {
        fprintf(stderr, "Failed to open temp file %s for writing\n", filename);
        exit(2);
    }

    save_domain_core_writeconfig(fd, filename, config_data, config_len);

    MUST(libxl_domain_suspend(ctx, domid, fd, 0, NULL));
    close(fd);

    if (checkpoint)
        libxl_domain_resume(ctx, domid, 1, 0);
    else
        libxl_domain_destroy(ctx, domid, 0);

    exit(0);
}

static pid_t create_migration_child(const char *rune, int *send_fd,
                                        int *recv_fd)
{
    int sendpipe[2], recvpipe[2];
    pid_t child = -1;

    if (!rune || !send_fd || !recv_fd)
        return -1;

    MUST( libxl_pipe(ctx, sendpipe) );
    MUST( libxl_pipe(ctx, recvpipe) );

    child = xl_fork(child_migration);

    if (!child) {
        dup2(sendpipe[0], 0);
        dup2(recvpipe[1], 1);
        close(sendpipe[0]); close(sendpipe[1]);
        close(recvpipe[0]); close(recvpipe[1]);
        execlp("sh","sh","-c",rune,(char*)0);
        perror("failed to exec sh");
        exit(-1);
    }

    close(sendpipe[0]);
    close(recvpipe[1]);
    *send_fd = sendpipe[1];
    *recv_fd = recvpipe[0];

    /* if receiver dies, we get an error and can clean up
       rather than just dying */
    signal(SIGPIPE, SIG_IGN);

    return child;
}

static int migrate_read_fixedmessage(int fd, const void *msg, int msgsz,
                                     const char *what, const char *rune) {
    char buf[msgsz];
    const char *stream;
    int rc;

    stream = rune ? "migration receiver stream" : "migration stream";
    rc = libxl_read_exactly(ctx, fd, buf, msgsz, stream, what);
    if (rc) return ERROR_FAIL;

    if (memcmp(buf, msg, msgsz)) {
        fprintf(stderr, "%s contained unexpected data instead of %s\n",
                stream, what);
        if (rune)
            fprintf(stderr, "(command run was: %s )\n", rune);
        return ERROR_FAIL;
    }
    return 0;
}

static void migration_child_report(int recv_fd) {
    pid_t child;
    int status, sr;
    struct timeval now, waituntil, timeout;
    static const struct timeval pollinterval = { 0, 1000 }; /* 1ms */

    if (!xl_child_pid(child_migration)) return;

    CHK_ERRNO( gettimeofday(&waituntil, 0) );
    waituntil.tv_sec += 2;

    for (;;) {
        pid_t migration_child = xl_child_pid(child_migration);
        child = xl_waitpid(child_migration, &status, WNOHANG);

        if (child == migration_child) {
            if (status)
                libxl_report_child_exitstatus(ctx, XTL_INFO,
                                              "migration target process",
                                              migration_child, status);
            break;
        }
        if (child == -1) {
            fprintf(stderr, "wait for migration child [%ld] failed: %s\n",
                    (long)migration_child, strerror(errno));
            break;
        }
        SkyAssert(child == 0);

        CHK_ERRNO( gettimeofday(&now, 0) );
        if (timercmp(&now, &waituntil, >)) {
            fprintf(stderr, "migration child [%ld] not exiting, no longer"
                    " waiting (exit status will be unreported)\n",
                    (long)migration_child);
            break;
        }
        timersub(&waituntil, &now, &timeout);

        if (recv_fd >= 0) {
            fd_set readfds, exceptfds;
            FD_ZERO(&readfds);
            FD_ZERO(&exceptfds);
            FD_SET(recv_fd, &readfds);
            FD_SET(recv_fd, &exceptfds);
            sr = select(recv_fd+1, &readfds,0,&exceptfds, &timeout);
        } else {
            if (timercmp(&timeout, &pollinterval, >))
                timeout = pollinterval;
            sr = select(0,0,0,0, &timeout);
        }
        if (sr > 0) {
            recv_fd = -1;
        } else if (sr == 0) {
        } else if (sr == -1) {
            if (errno != EINTR) {
                fprintf(stderr, "migration child [%ld] exit wait select"
                        " failed unexpectedly: %s\n",
                        (long)migration_child, strerror(errno));
                break;
            }
        }
    }
}

static void migrate_do_preamble(int send_fd, int recv_fd, pid_t child,
                                uint8_t *config_data, int config_len,
                                const char *rune)
{
    int rc = 0;

    if (send_fd < 0 || recv_fd < 0) {
        fprintf(stderr, "migrate_do_preamble: invalid file descriptors\n");
        exit(1);
    }

    rc = migrate_read_fixedmessage(recv_fd, migrate_receiver_banner,
                                   sizeof(migrate_receiver_banner)-1,
                                   "banner", rune);
    if (rc) {
        close(send_fd);
        migration_child_report(recv_fd);
        exit(-rc);
    }

    save_domain_core_writeconfig(send_fd, "migration stream",
                                 config_data, config_len);

}

static int migrate_domain(int domid, const char *rune,
                           const char *override_config_file)
{
    pid_t child = -1;
    int rc;
    int send_fd = -1, recv_fd = -1;
    char *away_domname;
    char rc_buf;
    uint8_t *config_data;
    int config_len;

    save_domain_core_begin(domid, override_config_file,
                           &config_data, &config_len);

    if (!config_len) {
        fprintf(stderr, "No config file stored for running domain and "
                "none supplied - cannot migrate.\n");
        return(1);
    }

    child = create_migration_child(rune, &send_fd, &recv_fd);

    migrate_do_preamble(send_fd, recv_fd, child, config_data, config_len,
                        rune);


    rc = libxl_domain_suspend(ctx, domid, send_fd, LIBXL_SUSPEND_LIVE, NULL);
    if (rc) {
        fprintf(stderr, "migration sender: libxl_domain_suspend failed"
                " (rc=%d)\n", rc);
        if (rc == ERROR_GUEST_TIMEDOUT)
            goto failed_suspend;
        else
            goto failed_resume;
    }

    //fprintf(stderr, "migration sender: Transfer complete.\n");
    // Should only be printed when debugging as it's a bit messy with
    // progress indication.

    rc = migrate_read_fixedmessage(recv_fd, migrate_receiver_ready,
                                   sizeof(migrate_receiver_ready),
                                   "ready message", rune);
    if (rc) goto failed_resume;


    /* right, at this point we are about give the destination
     * permission to rename and resume, so we must first rename the
     * domain away ourselves */

    fprintf(stderr, "migration sender: Target has acknowledged transfer.\n");

    if (common_domname) {
        if (asprintf(&away_domname, "%s--migratedaway", common_domname) < 0)
            goto failed_resume;
        rc = libxl_domain_rename(ctx, domid, common_domname, away_domname);
        if (rc) goto failed_resume;
    }

    /* point of no return - as soon as we have tried to say
     * "go" to the receiver, it's not safe to carry on.  We leave
     * the domain renamed to %s--migratedaway in case that's helpful.
     */

    fprintf(stderr, "migration sender: Giving target permission to start.\n");

    rc = libxl_write_exactly(ctx, send_fd,
                             migrate_permission_to_go,
                             sizeof(migrate_permission_to_go),
                             "migration stream", "GO message");
    if (rc) goto failed_badly;

    rc = migrate_read_fixedmessage(recv_fd, migrate_report,
                                   sizeof(migrate_report),
                                   "success/failure report message", rune);
    if (rc) goto failed_badly;

    rc = libxl_read_exactly(ctx, recv_fd,
                            &rc_buf, 1,
                            "migration ack stream", "success/failure status");
    if (rc) goto failed_badly;

    if (rc_buf) {
        fprintf(stderr, "migration sender: Target reports startup failure"
                " (status code %d).\n", rc_buf);

        rc = migrate_read_fixedmessage(recv_fd, migrate_permission_to_go,
                                       sizeof(migrate_permission_to_go),
                                       "permission for sender to resume",
                                       rune);
        if (rc) goto failed_badly;

        fprintf(stderr, "migration sender: Trying to resume at our end.\n");

        if (common_domname) {
            libxl_domain_rename(ctx, domid, away_domname, common_domname);
        }
        rc = libxl_domain_resume(ctx, domid, 0, 0);
        if (!rc) fprintf(stderr, "migration sender: Resumed OK.\n");

        fprintf(stderr, "Migration failed due to problems at target.\n");
        return(-ERROR_FAIL);
    }

    fprintf(stderr, "migration sender: Target reports successful startup.\n");
    libxl_domain_destroy(ctx, domid, 0); /* bang! */
    fprintf(stderr, "Migration successful.\n");
    return(0);

 failed_suspend:
    close(send_fd);
    migration_child_report(recv_fd);
    fprintf(stderr, "Migration failed, failed to suspend at sender.\n");
    return(-ERROR_FAIL);

 failed_resume:
    close(send_fd);
    migration_child_report(recv_fd);
    fprintf(stderr, "Migration failed, resuming at sender.\n");
    libxl_domain_resume(ctx, domid, 0, 0);
    return(-ERROR_FAIL);

 failed_badly:
    fprintf(stderr,
 "** Migration failed during final handshake **\n"
 "Domain state is now undefined !\n"
 "Please CHECK AT BOTH ENDS for running instances, before renaming and\n"
 " resuming at most one instance.  Two simultaneous instances of the domain\n"
 " would probably result in SEVERE DATA LOSS and it is now your\n"
 " responsibility to avoid that.  Sorry.\n");

    close(send_fd);
    migration_child_report(recv_fd);
    return(-ERROR_BADFAIL);
}

static void core_dump_domain(const char *domain_spec, const char *filename)
{
    int rc;
    find_domain(domain_spec);
    rc=libxl_domain_core_dump(ctx, domid, filename, NULL);
    if (rc) { fprintf(stderr,"core dump failed (rc=%d)\n",rc);exit(-1); }
}

static void migrate_receive(int debug, int daemonize, int monitor,
                            int send_fd, int recv_fd, int remus)
{
    int rc, rc2;
    char rc_buf;
    char *migration_domname;
    struct domain_create dom_info;

    signal(SIGPIPE, SIG_IGN);
    /* if we get SIGPIPE we'd rather just have it as an error */

    fprintf(stderr, "migration target: Ready to receive domain.\n");

    CHK_ERRNO( libxl_write_exactly(ctx, send_fd,
                                   migrate_receiver_banner,
                                   sizeof(migrate_receiver_banner)-1,
                                   "migration ack stream",
                                   "banner") );

    memset(&dom_info, 0, sizeof(dom_info));
    dom_info.debug = debug;
    dom_info.daemonize = daemonize;
    dom_info.monitor = monitor;
    dom_info.paused = 1;
    dom_info.migrate_fd = recv_fd;
    dom_info.migration_domname_r = &migration_domname;

    uint32_t domid=0;
    rc = create_domain(&dom_info, &domid);
    if (rc < 0) {
        fprintf(stderr, "migration target: Domain creation failed"
                " (code %d).\n", rc);
        exit(-rc);
    }

    if (remus) {
        /* If we are here, it means that the sender (primary) has crashed.
         * TODO: Split-Brain Check.
         */
        fprintf(stderr, "migration target: Remus Failover for domain %u\n",
                domid);

        /*
         * If domain renaming fails, lets just continue (as we need the domain
         * to be up & dom names may not matter much, as long as its reachable
         * over network).
         *
         * If domain unpausing fails, destroy domain ? Or is it better to have
         * a consistent copy of the domain (memory, cpu state, disk)
         * on atleast one physical host ? Right now, lets just leave the domain
         * as is and let the Administrator decide (or troubleshoot).
         */
        if (migration_domname) {
            rc = libxl_domain_rename(ctx, domid, migration_domname,
                                     common_domname);
            if (rc)
                fprintf(stderr, "migration target (Remus): "
                        "Failed to rename domain from %s to %s:%d\n",
                        migration_domname, common_domname, rc);
        }

        rc = libxl_domain_unpause(ctx, domid);
        if (rc)
            fprintf(stderr, "migration target (Remus): "
                    "Failed to unpause domain %s (id: %u):%d\n",
                    common_domname, domid, rc);

        exit(rc ? -ERROR_FAIL: 0);
    }

    fprintf(stderr, "migration target: Transfer complete,"
            " requesting permission to start domain.\n");

    rc = libxl_write_exactly(ctx, send_fd,
                             migrate_receiver_ready,
                             sizeof(migrate_receiver_ready),
                             "migration ack stream", "ready message");
    if (rc) exit(-rc);

    rc = migrate_read_fixedmessage(recv_fd, migrate_permission_to_go,
                                   sizeof(migrate_permission_to_go),
                                   "GO message", 0);
    if (rc) goto perhaps_destroy_notify_rc;

    fprintf(stderr, "migration target: Got permission, starting domain.\n");

    if (migration_domname) {
        rc = libxl_domain_rename(ctx, domid, migration_domname, common_domname);
        if (rc) goto perhaps_destroy_notify_rc;
    }

    rc = libxl_domain_unpause(ctx, domid);
    if (rc) goto perhaps_destroy_notify_rc;

    fprintf(stderr, "migration target: Domain started successsfully.\n");
    rc = 0;

 perhaps_destroy_notify_rc:
    rc2 = libxl_write_exactly(ctx, send_fd,
                              migrate_report, sizeof(migrate_report),
                              "migration ack stream",
                              "success/failure report");
    if (rc2) exit(-ERROR_BADFAIL);

    rc_buf = -rc;
    SkyAssert(!!rc_buf == !!rc);
    rc2 = libxl_write_exactly(ctx, send_fd, &rc_buf, 1,
                              "migration ack stream",
                              "success/failure code");
    if (rc2) exit(-ERROR_BADFAIL);

    if (rc) {
        fprintf(stderr, "migration target: Failure, destroying our copy.\n");

        rc2 = libxl_domain_destroy(ctx, domid, 0);
        if (rc2) {
            fprintf(stderr, "migration target: Failed to destroy our copy"
                    " (code %d).\n", rc2);
            exit(-ERROR_BADFAIL);
        }

        fprintf(stderr, "migration target: Cleanup OK, granting sender"
                " permission to resume.\n");

        rc2 = libxl_write_exactly(ctx, send_fd,
                                  migrate_permission_to_go,
                                  sizeof(migrate_permission_to_go),
                                  "migration ack stream",
                                  "permission to sender to have domain back");
        if (rc2) exit(-ERROR_BADFAIL);
    }

    exit(0);
}

int main_restore(int argc, char **argv)
{
    const char *checkpoint_file = NULL;
    const char *config_file = NULL;
    struct domain_create dom_info;
    int paused = 0, debug = 0, daemonize = 1, monitor = 1,
        console_autoconnect = 0, vnc = 0, vncautopass = 0;
    int opt, rc;
    int option_index = 0;
    static struct option long_options[] = {
        {"vncviewer", 0, 0, 'V'},
        {"vncviewer-autopass", 0, 0, 'A'},
        {0, 0, 0, 0}
    };

    while (1) {
        opt = getopt_long(argc, argv, "FhcpdeVA", long_options, &option_index);
        if (opt == -1)
            break;

        switch (opt) {
        case 0: case 2:
            return opt;
        case 'c':
            console_autoconnect = 1;
            break;
        case 'p':
            paused = 1;
            break;
        case 'd':
            debug = 1;
            break;
        case 'F':
            daemonize = 0;
            break;
        case 'e':
            daemonize = 0;
            monitor = 0;
            break;
        case 'V':
            vnc = 1;
            break;
        case 'A':
            vnc = vncautopass = 1;
            break;
        }
    }

    if (argc-optind == 1) {
        checkpoint_file = argv[optind];
    } else if (argc-optind == 2) {
        config_file = argv[optind];
        checkpoint_file = argv[optind + 1];
    } else {
        return 2;
    }

    memset(&dom_info, 0, sizeof(dom_info));
    dom_info.debug = debug;
    dom_info.daemonize = daemonize;
    dom_info.monitor = monitor;
    dom_info.paused = paused;
    dom_info.config_file = config_file;
    dom_info.restore_file = checkpoint_file;
    dom_info.migrate_fd = -1;
    dom_info.vnc = vnc;
    dom_info.vncautopass = vncautopass;
    dom_info.console_autoconnect = console_autoconnect;

    uint32_t domid;
    rc = create_domain(&dom_info, &domid);
    if (rc < 0)
        return -rc;

    return 0;
}

int main_migrate_receive(int argc, char **argv)
{
    int debug = 0, daemonize = 1, monitor = 1, remus = 0;
    int opt;

    while ((opt = def_getopt(argc, argv, "Fedr", "migrate-receive", 0)) != -1) {
        switch (opt) {
        case 0: case 2:
            return opt;
        case 'F':
            daemonize = 0;
            break;
        case 'e':
            daemonize = 0;
            monitor = 0;
            break;
        case 'd':
            debug = 1;
            break;
        case 'r':
            remus = 1;
            break;
        }
    }

    if (argc-optind != 0) {
        return 2;
    }
    migrate_receive(debug, daemonize, monitor,
                    STDOUT_FILENO, STDIN_FILENO,
                    remus);

    return 0;
}

int main_save(int argc, char **argv)
{
    const char *filename, *p;
    const char *config_filename = NULL;
    int checkpoint = 0;
    int opt;

    while ((opt = def_getopt(argc, argv, "c", "save", 2)) != -1) {
        switch (opt) {
        case 0: case 2:
            return opt;
        case 'c':
            checkpoint = 1;
            break;
        }
    }

    if (argc-optind > 3) {
        return 2;
    }

    p = argv[optind];
    filename = argv[optind + 1];
    if ( argc - optind >= 3 )
        config_filename = argv[optind + 2];

    save_domain(p, filename, checkpoint, config_filename);
    return 0;
}

int main_migrate(int domid, const char *host)
{
    const char *p = NULL;
    const char *config_filename = NULL;
    const char *ssh_command = "ssh";
    char *rune;
    int daemonize = 1, debug = 0;

    if (asprintf(&rune, "exec %s %s xl migrate-receive%s%s",
                 ssh_command, host,
                 daemonize ? "" : " -e",
                 debug ? " -d" : "") < 0)
        return 1;
    
    return migrate_domain(domid, rune, config_filename);
}

int main_dump_core(int argc, char **argv)
{
    int opt;

    if ((opt = def_getopt(argc, argv, "", "dump-core", 2)) != -1)
        return opt;

    core_dump_domain(argv[optind], argv[optind + 1]);
    return 0;
}


int main_destroy(int domid)
{
    return destroy_domain(domid);
}

int main_shutdown(int domid)
{
    int wait = 0;
    int fallback_trigger = 0;

    return shutdown_domain(domid, wait, fallback_trigger);
}


int domain_info_show(int domid,  libxl_dominfo &info)
{
    int  rc;

    rc = libxl_domain_info(ctx, &info, domid);
    if (rc == ERROR_INVAL) {
        fprintf(stderr, "Error: Domain \'%d\' does not exist.\n",
            domid);
        return -rc;
    }

    return 0;
}


uint32_t main_create(const char *filename)
{
    struct domain_create dom_info;
    int paused = 0, debug = 0, daemonize = 1, console_autoconnect = 0,
        quiet = 0, monitor = 1, vnc = 0, vncautopass = 0;
    int  rc;
//    char extra_config[1024];

    memset(&dom_info, 0, sizeof(dom_info));
    dom_info.debug = debug;
    dom_info.daemonize = daemonize;
    dom_info.monitor = monitor;
    dom_info.paused = paused;
    dom_info.dryrun = dryrun_only;
    dom_info.quiet = quiet;
    dom_info.config_file = filename;
    dom_info.extra_config = NULL;
    dom_info.migrate_fd = -1;
    dom_info.vnc = vnc;
    dom_info.vncautopass = vncautopass;
    dom_info.console_autoconnect = console_autoconnect;

    uint32_t domid = 0;
    rc = create_domain(&dom_info, &domid);
    if (rc < 0)
        return -1;

    return domid;
}

int main_config_update(int argc, char **argv)
{
    const char *filename = NULL;
    char *p;
    char extra_config[1024];
    void *config_data = 0;
    int config_len = 0;
    libxl_domain_config d_config;
    int opt, rc;
    int option_index = 0;
    int debug = 0;
    static struct option long_options[] = {
        {"defconfig", 1, 0, 'f'},
        {0, 0, 0, 0}
    };

    if (argc < 2) {
        fprintf(stderr, "xl config-update requires a domain argument\n");
        exit(1);
    }

    find_domain(argv[1]);
    argc--; argv++;

    if (argv[1] && argv[1][0] != '-' && !strchr(argv[1], '=')) {
        filename = argv[1];
        argc--; argv++;
    }

    while (1) {
        opt = getopt_long(argc, argv, "dhqf:", long_options, &option_index);
        if (opt == -1)
            break;

        switch (opt) {
        case 'd':
            debug = 1;
            break;
        case 'f':
            filename = optarg;
            break;
        default:
            fprintf(stderr, "option `%c' not supported.\n", optopt);
            break;
        }
    }

    extra_config[0] = '\0';
    for (p = extra_config; optind < argc; optind++) {
        if (strchr(argv[optind], '=') != NULL) {
            p += snprintf(p, sizeof(extra_config) - (p - extra_config),
                "%s\n", argv[optind]);
        } else if (!filename) {
            filename = argv[optind];
        } else {
            return 2;
        }
    }
    if (filename) {
        free(config_data);  config_data = 0;
        rc = libxl_read_file_contents(ctx, filename,
                                      &config_data, &config_len);
        if (rc) { fprintf(stderr, "Failed to read config file: %s: %s\n",
                           filename, strerror(errno)); return ERROR_FAIL; }
        if (strlen(extra_config)) {
            if (config_len > INT_MAX - (strlen(extra_config) + 2 + 1)) {
                fprintf(stderr, "Failed to attach extra configration\n");
                exit(1);
            }
            /* allocate space for the extra config plus two EOLs plus \0 */
            config_data = realloc(config_data, config_len
                + strlen(extra_config) + 2 + 1);
            if (!config_data) {
                fprintf(stderr, "Failed to realloc config_data\n");
                exit(1);
            }
            config_len += sprintf((char *)config_data + config_len, "\n%s\n",
                extra_config);
        }
    } else {
        fprintf(stderr, "Config file not specified\n");
        exit(1);
    }

    libxl_domain_config_init(&d_config);

    parse_config_data(filename, (char *)config_data, config_len, &d_config, NULL);

    //if (debug || dryrun_only)
        //printf_info(default_output_format, -1, &d_config);

    if (!dryrun_only) {
        fprintf(stderr, "setting dom%d configuration\n", domid);
        rc = libxl_userdata_store(ctx, domid, "xl",
                                   (uint8_t *)config_data, config_len);
        if (rc) {
            fprintf(stderr, "failed to update configuration\n");
            exit(1);
        }
    }

    libxl_domain_config_dispose(&d_config);

    free(config_data);

    return 0;
}

static void button_press(const char *p, const char *b)
{
    libxl_trigger trigger;

    find_domain(p);

    if (!strcmp(b, "power")) {
        trigger = LIBXL_TRIGGER_POWER;
    } else if (!strcmp(b, "sleep")) {
        trigger = LIBXL_TRIGGER_SLEEP;
    } else {
        fprintf(stderr, "%s is an invalid button identifier\n", b);
        exit(2);
    }

    libxl_send_trigger(ctx, domid, trigger, 0);
}

int main_button_press(int argc, char **argv)
{
    int opt;

    fprintf(stderr, "WARNING: \"button-press\" is deprecated. "
            "Please use \"trigger\"\n");

    if ((opt = def_getopt(argc, argv, "", "button-press", 2)) != -1)
        return opt;

    button_press(argv[optind], argv[optind + 1]);

    return 0;
}

static void print_bitmap(uint8_t *map, int maplen, FILE *stream)
{
    int i;
    uint8_t pmap = 0, bitmask = 0;
    int firstset = 0, state = 0;

    for (i = 0; i < maplen; i++) {
        if (i % 8 == 0) {
            pmap = *map++;
            bitmask = 1;
        } else bitmask <<= 1;

        switch (state) {
        case 0:
        case 2:
            if ((pmap & bitmask) != 0) {
                firstset = i;
                state++;
            }
            continue;
        case 1:
        case 3:
            if ((pmap & bitmask) == 0) {
                fprintf(stream, "%s%d", state > 1 ? "," : "", firstset);
                if (i - 1 > firstset)
                    fprintf(stream, "-%d", i - 1);
                state = 2;
            }
            continue;
        }
    }
    switch (state) {
        case 0:
            fprintf(stream, "none");
            break;
        case 2:
            break;
        case 1:
            if (firstset == 0) {
                fprintf(stream, "any cpu");
                break;
            }
        case 3:
            fprintf(stream, "%s%d", state > 1 ? "," : "", firstset);
            if (i - 1 > firstset)
                fprintf(stream, "-%d", i - 1);
            break;
    }
}

static void print_vcpuinfo(uint32_t tdomid,
                           const libxl_vcpuinfo *vcpuinfo,
                           uint32_t nr_cpus)
{
    char *domname;

    /*      NAME  ID  VCPU */
    domname = libxl_domid_to_name(ctx, tdomid);
    printf("%-32s %5u %5u",
           domname, tdomid, vcpuinfo->vcpuid);
    free(domname);
    if (!vcpuinfo->online) {
        /*      CPU STA */
        printf("%5c %3c%cp ", '-', '-', '-');
    } else {
        /*      CPU STA */
        printf("%5u %3c%c- ", vcpuinfo->cpu,
               vcpuinfo->running ? 'r' : '-',
               vcpuinfo->blocked ? 'b' : '-');
    }
    /*      TIM */
    printf("%9.1f  ", ((float)vcpuinfo->vcpu_time / 1e9));
    /* CPU AFFINITY */
    print_bitmap(vcpuinfo->cpumap.map, nr_cpus, stdout);
    printf("\n");
}

static void print_domain_vcpuinfo(uint32_t domid, uint32_t nr_cpus)
{
    libxl_vcpuinfo *vcpuinfo;
    int i, nb_vcpu, nrcpus;

    vcpuinfo = libxl_list_vcpu(ctx, domid, &nb_vcpu, &nrcpus);

    if (!vcpuinfo) {
        fprintf(stderr, "libxl_list_vcpu failed.\n");
        return;
    }

    for (i = 0; i < nb_vcpu; i++) {
        print_vcpuinfo(domid, &vcpuinfo[i], nr_cpus);
    }

    libxl_vcpuinfo_list_free(vcpuinfo, nb_vcpu);
}

static void vcpulist(int argc, char **argv)
{
    libxl_dominfo *dominfo;
    libxl_physinfo physinfo;
    int i, nb_domain;

    if (libxl_get_physinfo(ctx, &physinfo) != 0) {
        fprintf(stderr, "libxl_physinfo failed.\n");
        goto vcpulist_out;
    }

    printf("%-32s %5s %5s %5s %5s %9s %s\n",
           "Name", "ID", "VCPU", "CPU", "State", "Time(s)", "CPU Affinity");
    if (!argc) {
        if (!(dominfo = libxl_list_domain(ctx, &nb_domain))) {
            fprintf(stderr, "libxl_list_domain failed.\n");
            goto vcpulist_out;
        }

        for (i = 0; i<nb_domain; i++)
            print_domain_vcpuinfo(dominfo[i].domid, physinfo.nr_cpus);

        libxl_dominfo_list_free(dominfo, nb_domain);
    } else {
        for (; argc > 0; ++argv, --argc) {
            if (domain_qualifier_to_domid(*argv, &domid, 0) < 0) {
                fprintf(stderr, "%s is an invalid domain identifier\n", *argv);
                goto vcpulist_out;
            }

            print_domain_vcpuinfo(domid, physinfo.nr_cpus);
        }
    }
  vcpulist_out:
    libxl_physinfo_dispose(&physinfo);
}

int main_vcpulist(int argc, char **argv)
{
    int opt;

    if ((opt = def_getopt(argc, argv, "", "cpu-list", 0)) != -1)
        return opt;

    vcpulist(argc - optind, argv + optind);
    return 0;
}

int vcpupin(int domid, const char *vcpu, char *cpu)
{
    libxl_vcpuinfo *vcpuinfo;
    libxl_bitmap cpumap;

    uint32_t vcpuid;
    char *endptr;
    int i, nb_vcpu;
    bool is_success = true;

    vcpuid = strtoul(vcpu, &endptr, 10);
    if (vcpu == endptr) {
        if (strcmp(vcpu, "all")) {
            fprintf(stderr, "Error: Invalid argument.\n");
            return -1;
        }
        vcpuid = -1;
    }


    if (libxl_cpu_bitmap_alloc(ctx, &cpumap, 0)) {
        is_success = false;
        goto vcpupin_out;
    }

    if (vcpupin_parse(cpu, &cpumap))
    {
        is_success = false;
        goto vcpupin_out1;
    }
        

    if (vcpuid != (uint32_t)-1) {
        if (libxl_set_vcpuaffinity(ctx, domid, vcpuid, &cpumap) == -1) {
            fprintf(stderr, "Could not set affinity for vcpu `%u'.\n", vcpuid);
            is_success = false;
        }
    }
    else {
        if (!(vcpuinfo = libxl_list_vcpu(ctx, domid, &nb_vcpu, &i))) {
            fprintf(stderr, "libxl_list_vcpu failed.\n");
            is_success = false;
            goto vcpupin_out1;
        }
        for (i = 0; i < nb_vcpu; i++) {
            if (libxl_set_vcpuaffinity(ctx, domid, vcpuinfo[i].vcpuid,
                                       &cpumap) == -1) {
                fprintf(stderr, "libxl_set_vcpuaffinity failed"
                                " on vcpu `%u'.\n", vcpuinfo[i].vcpuid);
                is_success = false;
            }
        }
        libxl_vcpuinfo_list_free(vcpuinfo, nb_vcpu);
    }
  vcpupin_out1:
    libxl_bitmap_dispose(&cpumap);
  vcpupin_out:
    ;
  if (is_success == true)
        return 0;
  else 
        return -1;
}

static void vcpuset(const char *d, const char* nr_vcpus)
{
    char *endptr;
    unsigned int max_vcpus, i;
    libxl_bitmap cpumap;

    max_vcpus = strtoul(nr_vcpus, &endptr, 10);
    if (nr_vcpus == endptr) {
        fprintf(stderr, "Error: Invalid argument.\n");
        return;
    }

    find_domain(d);

    if (libxl_cpu_bitmap_alloc(ctx, &cpumap, 0)) {
        fprintf(stderr, "libxl_cpu_bitmap_alloc failed\n");
        return;
    }
    for (i = 0; i < max_vcpus; i++)
        libxl_bitmap_set(&cpumap, i);

    if (libxl_set_vcpuonline(ctx, domid, &cpumap) < 0)
        fprintf(stderr, "libxl_set_vcpuonline failed domid=%d max_vcpus=%d\n", domid, max_vcpus);

    libxl_bitmap_dispose(&cpumap);
}

int main_vcpuset(int argc, char **argv)
{
    int opt;

    if ((opt = def_getopt(argc, argv, "", "vcpu-set", 2)) != -1)
        return opt;

    vcpuset(argv[optind], argv[optind+1]);
    return 0;
}


static void sharing(const libxl_dominfo *info, int nb_domain)
{
    int i;

    printf("Name                                        ID   Mem Shared\n");

    for (i = 0; i < nb_domain; i++) {
        char *domname;
        unsigned shutdown_reason;
        domname = libxl_domid_to_name(ctx, info[i].domid);
        shutdown_reason = info[i].shutdown ? info[i].shutdown_reason : 0;
        printf("%-40s %5d %5lu  %5lu\n",
                domname,
                info[i].domid,
                (unsigned long) (info[i].current_memkb / 1024),
                (unsigned long) (info[i].shared_memkb / 1024));
        free(domname);
    }
}

int main_sharing(int argc, char **argv)
{
    int opt = 0;
    libxl_dominfo info_buf;
    libxl_dominfo *info, *info_free = NULL;
    int nb_domain, rc;

    if ((opt = def_getopt(argc, argv, "", "sharing", 0)) != -1)
        return opt;

    if (optind >= argc) {
        info = libxl_list_domain(ctx, &nb_domain);
        if (!info) {
            fprintf(stderr, "libxl_domain_infolist failed.\n");
            return 1;
        }
        info_free = info;
    } else if (optind == argc-1) {
        find_domain(argv[optind]);
        rc = libxl_domain_info(ctx, &info_buf, domid);
        if (rc == ERROR_INVAL) {
            fprintf(stderr, "Error: Domain \'%s\' does not exist.\n",
                argv[optind]);
            return -rc;
        }
        if (rc) {
            fprintf(stderr, "libxl_domain_info failed (code %d).\n", rc);
            return -rc;
        }
        info = &info_buf;
        nb_domain = 1;
    } else {
        return 2;
    }

    sharing(info, nb_domain);

    if (info_free)
        libxl_dominfo_list_free(info_free, nb_domain);
    else
        libxl_dominfo_dispose(info);

    return 0;
}

static int sched_domain_get(libxl_scheduler sched, int domid,
                            libxl_domain_sched_params *scinfo)
{
    int rc;

    rc = libxl_domain_sched_params_get(ctx, domid, scinfo);
    if (rc) {
        fprintf(stderr, "libxl_domain_sched_params_get failed.\n");
        return rc;
    }
    if (scinfo->sched != sched) {
        fprintf(stderr, "libxl_domain_sched_params_get returned %s not %s.\n",
                libxl_scheduler_to_string(scinfo->sched),
                libxl_scheduler_to_string(sched));
        return ERROR_INVAL;
    }

    return 0;
}

static int sched_domain_set(int domid, const libxl_domain_sched_params *scinfo)
{
    int rc;

    rc = libxl_domain_sched_params_set(ctx, domid, scinfo);
    if (rc)
        fprintf(stderr, "libxl_domain_sched_params_set failed.\n");

    return rc;
}

#if 0
static int sched_credit_params_set(int poolid, libxl_sched_credit_params *scinfo)
{
    int rc;

    rc = libxl_sched_credit_params_set(ctx, poolid, scinfo);
    if (rc)
        fprintf(stderr, "libxl_sched_credit_params_set failed.\n");

    return rc;
}


static int sched_credit_params_get(int poolid, libxl_sched_credit_params *scinfo)
{
    int rc;

    rc = libxl_sched_credit_params_get(ctx, poolid, scinfo);
    if (rc)
        fprintf(stderr, "libxl_sched_credit_params_get failed.\n");

    return rc;
}
#endif
static int sched_credit_domain_output(int domid)
{
    char *domname;
    libxl_domain_sched_params scinfo;
    int rc;

    if (domid < 0) {
        printf("%-33s %4s %6s %4s\n", "Name", "ID", "Weight", "Cap");
        return 0;
    }
    rc = sched_domain_get(LIBXL_SCHEDULER_CREDIT, domid, &scinfo);
    if (rc)
        return rc;
    domname = libxl_domid_to_name(ctx, domid);
    printf("%-33s %4d %6d %4d\n",
        domname,
        domid,
        scinfo.weight,
        scinfo.cap);
    free(domname);
    libxl_domain_sched_params_dispose(&scinfo);
    return 0;
}

#if 0
static int sched_credit_pool_output(uint32_t poolid)
{
    libxl_sched_credit_params scparam;
    char *poolname;
    int rc;

    poolname = libxl_cpupoolid_to_name(ctx, poolid);
    rc = sched_credit_params_get(poolid, &scparam);
    if (rc) {
        printf("Cpupool %s: [sched params unavailable]\n",
               poolname);
    } else {
        printf("Cpupool %s: tslice=%dms ratelimit=%dus\n",
               poolname,
               scparam.tslice_ms,
               scparam.ratelimit_us);
    }
    free(poolname);
    return 0;
}
#endif

static int sched_credit2_domain_output(
    int domid)
{
    char *domname;
    libxl_domain_sched_params scinfo;
    int rc;

    if (domid < 0) {
        printf("%-33s %4s %6s\n", "Name", "ID", "Weight");
        return 0;
    }
    rc = sched_domain_get(LIBXL_SCHEDULER_CREDIT2, domid, &scinfo);
    if (rc)
        return rc;
    domname = libxl_domid_to_name(ctx, domid);
    printf("%-33s %4d %6d\n",
        domname,
        domid,
        scinfo.weight);
    free(domname);
    libxl_domain_sched_params_dispose(&scinfo);
    return 0;
}

static int sched_sedf_domain_output(
    int domid)
{
    char *domname;
    libxl_domain_sched_params scinfo;
    int rc;

    if (domid < 0) {
        printf("%-33s %4s %6s %-6s %7s %5s %6s\n", "Name", "ID", "Period",
               "Slice", "Latency", "Extra", "Weight");
        return 0;
    }
    rc = sched_domain_get(LIBXL_SCHEDULER_SEDF, domid, &scinfo);
    if (rc)
        return rc;
    domname = libxl_domid_to_name(ctx, domid);
    printf("%-33s %4d %6d %6d %7d %5d %6d\n",
        domname,
        domid,
        scinfo.period,
        scinfo.slice,
        scinfo.latency,
        scinfo.extratime,
        scinfo.weight);
    free(domname);
    libxl_domain_sched_params_dispose(&scinfo);
    return 0;
}

static int sched_default_pool_output(uint32_t poolid)
{
    char *poolname;

    poolname = libxl_cpupoolid_to_name(ctx, poolid);
    printf("Cpupool %s:\n",
           poolname);
    free(poolname);
    return 0;
}

static int sched_domain_output(libxl_scheduler sched, int (*output)(int),
                               int (*pooloutput)(uint32_t), const char *cpupool)
{
    libxl_dominfo *info;
    libxl_cpupoolinfo *poolinfo = NULL;
    uint32_t poolid;
    int nb_domain, n_pools = 0, i, p;
    int rc = 0;

    if (cpupool) {
        if (cpupool_qualifier_to_cpupoolid(cpupool, &poolid, NULL) ||
            !libxl_cpupoolid_to_name(ctx, poolid)) {
            fprintf(stderr, "unknown cpupool \'%s\'\n", cpupool);
            return -ERROR_FAIL;
        }
    }

    info = libxl_list_domain(ctx, &nb_domain);
    if (!info) {
        fprintf(stderr, "libxl_domain_infolist failed.\n");
        return 1;
    }
    poolinfo = libxl_list_cpupool(ctx, &n_pools);
    if (!poolinfo) {
        fprintf(stderr, "error getting cpupool info\n");
        return -ERROR_NOMEM;
    }

    for (p = 0; !rc && (p < n_pools); p++) {
        if ((poolinfo[p].sched != sched) ||
            (cpupool && (poolid != poolinfo[p].poolid)))
            continue;

        pooloutput(poolinfo[p].poolid);

        output(-1);
        for (i = 0; i < nb_domain; i++) {
            if (info[i].cpupool != poolinfo[p].poolid)
                continue;
            rc = output(info[i].domid);
            if (rc)
                break;
        }
    }
    if (poolinfo)
        libxl_cpupoolinfo_list_free(poolinfo, n_pools);
    return 0;
}

/* 
 * <nothing>             : List all domain params and sched params from all pools
 * -d [domid]            : List domain params for domain
 * -d [domid] [params]   : Set domain params for domain
 * -p [pool]             : list all domains and sched params for pool
 * -s                    : List sched params for poolid 0
 * -s [params]           : Set sched params for poolid 0
 * -p [pool] -s          : List sched params for pool
 * -p [pool] -s [params] : Set sched params for pool
 * -p [pool] -d...       : Illegal
 */
static int sched_credit_set(int domid, int weight=256, int cap = 0, 
                                      bool opt_w = true, bool opt_c = true)
{
    int rc;

    libxl_domain_sched_params scinfo;
    libxl_domain_sched_params_init(&scinfo);
    scinfo.sched = LIBXL_SCHEDULER_CREDIT;
    if (opt_w)
        scinfo.weight = weight;
    if (opt_c)
        scinfo.cap = cap;
    rc = sched_domain_set(domid, &scinfo);
    libxl_domain_sched_params_dispose(&scinfo);
    if (rc)
        return -rc;

    return 0;
}

//  设置权重
int sched_credit_weight_set(int domid, int weight)
{
    return sched_credit_set(domid, weight, 0, true, false);
}

//  设置上限
int sched_credit_cap_set(int domid, int cap)
{
    return sched_credit_set(domid, 0, cap, false, true);
}

int sched_credit_list(int domid)
{
    sched_credit_domain_output(-1);
    return -sched_credit_domain_output(domid);
    return 0;
}

int main_sched_credit2(int argc, char **argv)
{
    const char *dom = NULL;
    const char *cpupool = NULL;
    int weight = 256, opt_w = 0;
    int opt, rc;
    int option_index = 0;
    static struct option long_options[] = {
        {"domain", 1, 0, 'd'},
        {"weight", 1, 0, 'w'},
        {"cpupool", 1, 0, 'p'},
        {0, 0, 0, 0}
    };

    while (1) {
        opt = getopt_long(argc, argv, "d:w:p:h", long_options, &option_index);
        if (opt == -1)
            break;
        switch (opt) {
        case 0: case 2:
            return opt;
        case 'd':
            dom = optarg;
            break;
        case 'w':
            weight = strtol(optarg, NULL, 10);
            opt_w = 1;
            break;
        case 'p':
            cpupool = optarg;
            break;
        }
    }

    if (cpupool && (dom || opt_w)) {
        fprintf(stderr, "Specifying a cpupool is not allowed with other "
                "options.\n");
        return 1;
    }
    if (!dom && opt_w) {
        fprintf(stderr, "Must specify a domain.\n");
        return 1;
    }

    if (!dom) { /* list all domain's credit scheduler info */
        return -sched_domain_output(LIBXL_SCHEDULER_CREDIT2,
                                    sched_credit2_domain_output,
                                    sched_default_pool_output,
                                    cpupool);
    } else {
        find_domain(dom);

        if (!opt_w) { /* output credit2 scheduler info */
            sched_credit2_domain_output(-1);
            return -sched_credit2_domain_output(domid);
        } else { /* set credit2 scheduler paramaters */
            libxl_domain_sched_params scinfo;
            libxl_domain_sched_params_init(&scinfo);
            scinfo.sched = LIBXL_SCHEDULER_CREDIT2;
            if (opt_w)
                scinfo.weight = weight;
            rc = sched_domain_set(domid, &scinfo);
            libxl_domain_sched_params_dispose(&scinfo);
            if (rc)
                return -rc;
        }
    }

    return 0;
}

int main_sched_sedf(int argc, char **argv)
{
    const char *dom = NULL;
    const char *cpupool = NULL;
    int period = 0, opt_p = 0;
    int slice = 0, opt_s = 0;
    int latency = 0, opt_l = 0;
    int extra = 0, opt_e = 0;
    int weight = 0, opt_w = 0;
    int opt, rc;
    int option_index = 0;
    static struct option long_options[] = {
        {"period", 1, 0, 'p'},
        {"slice", 1, 0, 's'},
        {"latency", 1, 0, 'l'},
        {"extra", 1, 0, 'e'},
        {"weight", 1, 0, 'w'},
        {"cpupool", 1, 0, 'c'},
        {0, 0, 0, 0}
    };

    while (1) {
        opt = getopt_long(argc, argv, "d:p:s:l:e:w:c:h", long_options,
                          &option_index);
        if (opt == -1)
            break;
        switch (opt) {
        case 0: case 2:
            return opt;
        case 'd':
            dom = optarg;
            break;
        case 'p':
            period = strtol(optarg, NULL, 10);
            opt_p = 1;
            break;
        case 's':
            slice = strtol(optarg, NULL, 10);
            opt_s = 1;
            break;
        case 'l':
            latency = strtol(optarg, NULL, 10);
            opt_l = 1;
            break;
        case 'e':
            extra = strtol(optarg, NULL, 10);
            opt_e = 1;
            break;
        case 'w':
            weight = strtol(optarg, NULL, 10);
            opt_w = 1;
            break;
        case 'c':
            cpupool = optarg;
            break;
        }
    }

    if (cpupool && (dom || opt_p || opt_s || opt_l || opt_e || opt_w)) {
        fprintf(stderr, "Specifying a cpupool is not allowed with other "
                "options.\n");
        return 1;
    }
    if (!dom && (opt_p || opt_s || opt_l || opt_e || opt_w)) {
        fprintf(stderr, "Must specify a domain.\n");
        return 1;
    }
    if (opt_w && (opt_p || opt_s)) {
        fprintf(stderr, "Specifying a weight AND period or slice is not "
                "allowed.\n");
    }

    if (!dom) { /* list all domain's credit scheduler info */
        return -sched_domain_output(LIBXL_SCHEDULER_SEDF,
                                    sched_sedf_domain_output,
                                    sched_default_pool_output,
                                    cpupool);
    } else {
        find_domain(dom);

        if (!opt_p && !opt_s && !opt_l && !opt_e && !opt_w) {
            /* output sedf scheduler info */
            sched_sedf_domain_output(-1);
            return -sched_sedf_domain_output(domid);
        } else { /* set sedf scheduler paramaters */
            libxl_domain_sched_params scinfo;
            libxl_domain_sched_params_init(&scinfo);
            scinfo.sched = LIBXL_SCHEDULER_SEDF;

            if (opt_p) {
                scinfo.period = period;
                scinfo.weight = 0;
            }
            if (opt_s) {
                scinfo.slice = slice;
                scinfo.weight = 0;
            }
            if (opt_l)
                scinfo.latency = latency;
            if (opt_e)
                scinfo.extratime = extra;
            if (opt_w) {
                scinfo.weight = weight;
                scinfo.period = 0;
                scinfo.slice = 0;
            }
            rc = sched_domain_set(domid, &scinfo);
            libxl_domain_sched_params_dispose(&scinfo);
            if (rc)
                return -rc;
        }
    }

    return 0;
}

int main_domid(const char *domname)
{
    uint32_t domid = INVALID_DOMID;

    if (libxl_name_to_domid(ctx, domname, &domid)) {
//        fprintf(stderr, "Can't get domid of domain name '%s', maybe this domain does not exist.\n", domname);
        return -1;
    }


    return domid;
}

string main_domname(int domid)
{
    if (domid == 0) {
        /*no digits at all*/
        fprintf(stderr, "Invalid domain id.\n\n");
        return "";
    }

    char *domname = libxl_domid_to_name(ctx, domid);
    if (!domname) {
        fprintf(stderr, "Can't get domain name of domain id '%d', maybe this domain does not exist.\n", domid);
        return "";
    }

    string name_out = domname;
    free(domname);

    return name_out;
}

int main_rename(int argc, char **argv)
{
    int opt;
    const char *dom;
    const char *new_name;

    if ((opt = def_getopt(argc, argv, "", "rename", 2)) != -1)
        return opt;

    dom = argv[optind++];

    find_domain(dom);
    new_name = argv[optind];

    if (libxl_domain_rename(ctx, domid, common_domname, new_name)) {
        fprintf(stderr, "Can't rename domain '%s'.\n", dom);
        return 1;
    }

    return 0;
}

int main_trigger(int argc, char **argv)
{
    int opt;
    char *endptr = NULL;
    const char *dom = NULL;
    int vcpuid = 0;
    const char *trigger_name = NULL;
    libxl_trigger trigger;

    if ((opt = def_getopt(argc, argv, "", "trigger", 2)) != -1)
        return opt;

    dom = argv[optind++];

    find_domain(dom);

    trigger_name = argv[optind++];
    if (libxl_trigger_from_string(trigger_name, &trigger)) {
        fprintf(stderr, "Invalid trigger \"%s\"\n", trigger_name);
        return -1;
    }

    if (argv[optind]) {
        vcpuid = strtol(argv[optind], &endptr, 10);
        if (vcpuid == 0 && !strcmp(endptr, argv[optind])) {
            fprintf(stderr, "Invalid vcpuid, using default vcpuid=0.\n\n");
        }
    }

    libxl_send_trigger(ctx, domid, trigger, vcpuid);

    return 0;
}


int main_top(int argc, char **argv)
{
    int opt;

    if ((opt = def_getopt(argc, argv, "", "top", 0)) != -1)
        return opt;

    return system("xentop");
}

int main_networkattach(int argc, char **argv)
{
    int opt;
    libxl_device_nic nic;
    XLU_Config *config = 0;
    char *endptr, *oparg;
    const char *tok;
    int i;
    unsigned int val;

    if ((opt = def_getopt(argc, argv, "", "network-attach", 1)) != -1)
        return opt;

    if (argc-optind > 11) {
        return 0;
    }

    if (domain_qualifier_to_domid(argv[optind], &domid, 0) < 0) {
        fprintf(stderr, "%s is an invalid domain identifier\n", argv[optind]);
        return 1;
    }

    config= xlu_cfg_init(stderr, "command line");
    if (!config) {
        fprintf(stderr, "Failed to allocate for configuration\n");
        return 1;
    }

    libxl_device_nic_init(&nic);
    for (argv += optind+1, argc -= optind+1; argc > 0; ++argv, --argc) {
        if (MATCH_OPTION("type", *argv, oparg)) {
            if (!strcmp("vif", oparg)) {
                nic.nictype = LIBXL_NIC_TYPE_VIF;
            } else if (!strcmp("ioemu", oparg)) {
                nic.nictype = LIBXL_NIC_TYPE_VIF_IOEMU;
            } else {
                fprintf(stderr, "Invalid parameter `type'.\n");
                return 1;
            }
        } else if (MATCH_OPTION("mac", *argv, oparg)) {
            tok = strtok(oparg, ":");
            for (i = 0; tok && i < 6; tok = strtok(NULL, ":"), ++i) {
                val = strtoul(tok, &endptr, 16);
                if ((tok == endptr) || (val > 255)) {
                    fprintf(stderr, "Invalid parameter `mac'.\n");
                    return 1;
                }
                nic.mac[i] = val;
            }
        } else if (MATCH_OPTION("bridge", *argv, oparg)) {
            replace_string(&nic.bridge, oparg);
        } else if (MATCH_OPTION("ip", *argv, oparg)) {
            replace_string(&nic.ip, oparg);
        } else if (MATCH_OPTION("script", *argv, oparg)) {
            replace_string(&nic.script, oparg);
        } else if (MATCH_OPTION("backend", *argv, oparg)) {
            if(libxl_name_to_domid(ctx, oparg, &val)) {
                fprintf(stderr, "Specified backend domain does not exist, defaulting to Dom0\n");
                val = 0;
            }
            nic.backend_domid = val;
        } else if (MATCH_OPTION("vifname", *argv, oparg)) {
            replace_string(&nic.ifname, oparg);
        } else if (MATCH_OPTION("model", *argv, oparg)) {
            replace_string(&nic.model, oparg);
        } else if (MATCH_OPTION("rate", *argv, oparg)) {
            parse_vif_rate(&config, oparg, &nic);
        } else if (MATCH_OPTION("accel", *argv, oparg)) {
        } else {
            fprintf(stderr, "unrecognized argument `%s'\n", *argv);
            return 1;
        }
    }

    if (dryrun_only) {
        char *json = libxl_device_nic_to_json(ctx, &nic);
        printf("vif: %s\n", json);
        free(json);
        libxl_device_nic_dispose(&nic);
        if (ferror(stdout) || fflush(stdout)) { perror("stdout"); exit(-1); }
        return 0;
    }

    if (libxl_device_nic_add(ctx, domid, &nic, 0)) {
        fprintf(stderr, "libxl_device_nic_add failed.\n");
        return 1;
    }
    libxl_device_nic_dispose(&nic);
    xlu_cfg_destroy(config);
    return 0;
}

int main_networklist(int domid, char **argv)
{
    libxl_device_nic *nics;
    libxl_nicinfo nicinfo;
    int nb=0, i;

    /*      Idx  BE   MAC   Hdl  Sta  evch txr/rxr  BE-path */
    nics = libxl_device_nic_list(ctx, domid, &nb);
    for (i = 0; i < nb; ++i) {
        if (!libxl_device_nic_getinfo(ctx, domid, &nics[i], &nicinfo)) {
            /* Hdl  Sta  evch txr/rxr  BE-path */
            printf("%6d %5d %6d %5d/%-11d %-30s\n",
                   nicinfo.devid, nicinfo.state, nicinfo.evtch,
                   nicinfo.rref_tx, nicinfo.rref_rx, nicinfo.backend);
            libxl_nicinfo_dispose(&nicinfo);
        }
        libxl_device_nic_dispose(&nics[i]);
    }
    free(nics);
    return 0;
}

int main_networkdetach(int argc, char **argv)
{
    int opt;
    libxl_device_nic nic;

    if ((opt = def_getopt(argc, argv, "", "network-detach", 2)) != -1)
        return opt;

    if (domain_qualifier_to_domid(argv[optind], &domid, 0) < 0) {
        fprintf(stderr, "%s is an invalid domain identifier\n", argv[optind]);
        return 1;
    }

    if (!strchr(argv[optind+1], ':')) {
        if (libxl_devid_to_device_nic(ctx, domid, atoi(argv[optind+1]), &nic)) {
            fprintf(stderr, "Unknown device %s.\n", argv[optind+1]);
            return 1;
        }
    } else {
        if (libxl_mac_to_device_nic(ctx, domid, argv[optind+1], &nic)) {
            fprintf(stderr, "Unknown device %s.\n", argv[optind+1]);
            return 1;
        }
    }
    if (libxl_device_nic_remove(ctx, domid, &nic, 0)) {
        fprintf(stderr, "libxl_device_nic_del failed.\n");
        return 1;
    }
    libxl_device_nic_dispose(&nic);
    return 0;
}

int main_blockattach(int argc, char **argv)
{
    int opt;
    uint32_t fe_domid, be_domid = 0;
    libxl_device_disk disk = { 0 };
    XLU_Config *config = 0;

    if ((opt = def_getopt(argc, argv, "", "block-attach", 2)) != -1)
        return opt;

    if (domain_qualifier_to_domid(argv[optind], &fe_domid, 0) < 0) {
        fprintf(stderr, "%s is an invalid domain identifier\n", argv[optind]);
        return 1;
    }
    optind++;

    parse_disk_config_multistring
        (&config, argc-optind, (const char* const*)argv + optind, &disk);

    disk.backend_domid = be_domid;

    if (dryrun_only) {
        char *json = libxl_device_disk_to_json(ctx, &disk);
        printf("disk: %s\n", json);
        free(json);
        if (ferror(stdout) || fflush(stdout)) { perror("stdout"); exit(-1); }
        return 0;
    }

    if (libxl_device_disk_add(ctx, fe_domid, &disk, 0)) {
        fprintf(stderr, "libxl_device_disk_add failed.\n");
    }
    return 0;
}

int main_blocklist(int argc, char **argv)
{
    int opt;
    int i, nb;
    libxl_device_disk *disks;
    libxl_diskinfo diskinfo;

    if ((opt = def_getopt(argc, argv, "", "block-list", 1)) != -1)
        return opt;

    printf("%-5s %-3s %-6s %-5s %-6s %-8s %-30s\n",
           "Vdev", "BE", "handle", "state", "evt-ch", "ring-ref", "BE-path");
    for (argv += optind, argc -= optind; argc > 0; --argc, ++argv) {
        if (domain_qualifier_to_domid(*argv, &domid, 0) < 0) {
            fprintf(stderr, "%s is an invalid domain identifier\n", *argv);
            continue;
        }
        disks = libxl_device_disk_list(ctx, domid, &nb);
        if (!disks) {
            continue;
        }
        for (i=0; i<nb; i++) {
            if (!libxl_device_disk_getinfo(ctx, domid, &disks[i], &diskinfo)) {
                /*      Vdev BE   hdl  st   evch rref BE-path*/
                printf("%-5d %-3d %-6d %-5d %-6d %-8d %-30s\n",
                       diskinfo.devid, diskinfo.backend_id, diskinfo.frontend_id,
                       diskinfo.state, diskinfo.evtch, diskinfo.rref, diskinfo.backend);
                libxl_diskinfo_dispose(&diskinfo);
            }
            libxl_device_disk_dispose(&disks[i]);
        }
        free(disks);
    }
    return 0;
}

int main_blockdetach(int argc, char **argv)
{
    int opt, rc = 0;
    libxl_device_disk disk;

    if ((opt = def_getopt(argc, argv, "", "block-detach", 2)) != -1)
        return opt;

    if (domain_qualifier_to_domid(argv[optind], &domid, 0) < 0) {
        fprintf(stderr, "%s is an invalid domain identifier\n", argv[optind]);
        return 1;
    }
    if (libxl_vdev_to_device_disk(ctx, domid, argv[optind+1], &disk)) {
        fprintf(stderr, "Error: Device %s not connected.\n", argv[optind+1]);
        return 1;
    }
    rc = libxl_device_disk_remove(ctx, domid, &disk, 0);
    if (rc) {
        fprintf(stderr, "libxl_device_disk_remove failed.\n");
    }
    libxl_device_disk_dispose(&disk);
    return rc;
}

static char *uptime_to_string(unsigned long time, int short_mode)
{
    int sec, min, hour, day;
    char *time_string;
    int ret;

    day = (int)(time / 86400);
    time -= (day * 86400);
    hour = (int)(time / 3600);
    time -= (hour * 3600);
    min = (int)(time / 60);
    time -= (min * 60);
    sec = time;

    if (short_mode)
        if (day > 1)
            ret = asprintf(&time_string, "%d days, %2d:%02d", day, hour, min);
        else if (day == 1)
            ret = asprintf(&time_string, "%d day, %2d:%02d", day, hour, min);
        else
            ret = asprintf(&time_string, "%2d:%02d", hour, min);
    else
        if (day > 1)
            ret = asprintf(&time_string, "%d days, %2d:%02d:%02d", day, hour, min, sec);
        else if (day == 1)
            ret = asprintf(&time_string, "%d day, %2d:%02d:%02d", day, hour, min, sec);
        else
            ret = asprintf(&time_string, "%2d:%02d:%02d", hour, min, sec);

    if (ret < 0)
        return NULL;
    return time_string;
}

static char *current_time_to_string(time_t now)
{
    char now_str[100];
    struct tm *tmp;

    tmp = localtime(&now);
    if (tmp == NULL) {
        fprintf(stderr, "Get localtime error");
        exit(-1);
    }
    if (strftime(now_str, sizeof(now_str), "%H:%M:%S", tmp) == 0) {
        fprintf(stderr, "strftime returned 0");
        exit(-1);
    }
    return strdup(now_str);
}

static void print_dom0_uptime(int short_mode, time_t now)
{
    int fd;
    char buf[512];
    uint32_t uptime = 0;
    char *uptime_str = NULL;
    char *now_str = NULL;
    char *domname;

    fd = open("/proc/uptime", O_RDONLY);
    if (fd == -1)
        goto err;

    if (read(fd, buf, sizeof(buf)) == -1) {
        close(fd);
        goto err;
    }
    close(fd);

    strtok(buf, " ");
    uptime = strtoul(buf, NULL, 10);

    domname = libxl_domid_to_name(ctx, 0);
    if (short_mode)
    {
        now_str = current_time_to_string(now);
        uptime_str = uptime_to_string(uptime, 1);
        printf(" %s up %s, %s (%d)\n", now_str, uptime_str,
               domname, 0);
    }
    else
    {
        now_str = NULL;
        uptime_str = uptime_to_string(uptime, 0);
        printf("%-33s %4d %s\n", domname,
               0, uptime_str);
    }

    free(now_str);
    free(uptime_str);
    free(domname);
    return;
err:
    fprintf(stderr, "Can not get Dom0 uptime.\n");
    exit(-1);
}

static void print_domU_uptime(uint32_t domuid, int short_mode, time_t now)
{
    uint32_t s_time = 0;
    uint32_t uptime = 0;
    char *uptime_str = NULL;
    char *now_str = NULL;
    char *domname;

    s_time = libxl_vm_get_start_time(ctx, domuid);
    if (s_time == (uint32_t)-1)
        return;
    uptime = now - s_time;
    domname = libxl_domid_to_name(ctx, domuid);
    if (short_mode)
    {
        now_str = current_time_to_string(now);
        uptime_str = uptime_to_string(uptime, 1);
        printf(" %s up %s, %s (%d)\n", now_str, uptime_str,
               domname, domuid);
    }
    else
    {
        now_str = NULL;
        uptime_str = uptime_to_string(uptime, 0);
        printf("%-33s %4d %s\n", domname,
               domuid, uptime_str);
    }

    free(domname);
    free(now_str);
    free(uptime_str);
    return;
}

static void print_uptime(int short_mode, uint32_t doms[], int nb_doms)
{
    libxl_vminfo *info;
    time_t now;
    int nb_vm, i;

    now = time(NULL);

    if (!short_mode)
        printf("%-33s %4s %s\n", "Name", "ID", "Uptime");

    if (nb_doms == 0) {
        print_dom0_uptime(short_mode, now);
        info = libxl_list_vm(ctx, &nb_vm);
        for (i = 0; i < nb_vm; i++)
            print_domU_uptime(info[i].domid, short_mode, now);
        libxl_vminfo_list_free(info, nb_vm);
    } else {
        for (i = 0; i < nb_doms; i++) {
            if (doms[i] == 0)
                print_dom0_uptime(short_mode, now);
            else
                print_domU_uptime(doms[i], short_mode, now);
        }
    }
}

int main_uptime(int argc, char **argv)
{
    const char *dom = NULL;
    int short_mode = 0;
    uint32_t domains[100];
    int nb_doms = 0;
    int opt;

    while ((opt = def_getopt(argc, argv, "s", "uptime", 1)) != -1) {
        switch (opt) {
        case 0: case 2:
            return opt;
        case 's':
            short_mode = 1;
            break;
        }
    }

    for (;(dom = argv[optind]) != NULL; nb_doms++,optind++) {
        find_domain(dom);
        domains[nb_doms] = domid;
    }

    print_uptime(short_mode, domains, nb_doms);

    return 0;
}




int main_tmem_set(int argc, char **argv)
{
    const char *dom = NULL;
    uint32_t weight = 0, cap = 0, compress = 0;
    int opt_w = 0, opt_c = 0, opt_p = 0;
    int all = 0;
    int opt;

    while ((opt = def_getopt(argc, argv, "aw:c:p:", "tmem-set", 0)) != -1) {
        switch (opt) {
        case 0: case 2:
            return opt;
        case 'a':
            all = 1;
            break;
        case 'w':
            weight = strtol(optarg, NULL, 10);
            opt_w = 1;
            break;
        case 'c':
            cap = strtol(optarg, NULL, 10);
            opt_c = 1;
            break;
        case 'p':
            compress = strtol(optarg, NULL, 10);
            opt_p = 1;
            break;
        }
    }

    dom = argv[optind];
    if (!dom && all == 0) {
        fprintf(stderr, "You must specify -a or a domain id.\n\n");
        return 1;
    }

    if (all)
        domid = -1;
    else
        find_domain(dom);

    if (!opt_w && !opt_c && !opt_p) {
        fprintf(stderr, "No set value specified.\n\n");
        return 1;
    }

    if (opt_w)
        libxl_tmem_set(ctx, domid, "weight", weight);
    if (opt_c)
        libxl_tmem_set(ctx, domid, "cap", cap);
    if (opt_p)
        libxl_tmem_set(ctx, domid, "compress", compress);

    return 0;
}


int main_tmem_freeable(int argc, char **argv)
{
    int opt;
    int mb;

    if ((opt = def_getopt(argc, argv, "", "tmem-freeable", 0)) != -1)
        return opt;

    mb = libxl_tmem_freeable(ctx);
    if (mb == -1)
        return -1;

    printf("%d\n", mb);
    return 0;
}

int main_getenforce(int argc, char **argv)
{
    int ret;

    ret = libxl_flask_getenforce(ctx);

    if (ret < 0) {
        if (errno == ENOSYS)
            printf("Flask XSM Disabled\n");
        else
            fprintf(stderr, "Failed to get enforcing mode\n");
    }
    else if (ret == 1)
        printf("Enforcing\n");
    else if (ret == 0)
        printf("Permissive\n");

    return ret;
}

int main_setenforce(int argc, char **argv)
{
    int ret, mode = -1;
    const char *p = NULL;

    if (optind >= argc) {
        return 2;
    }

    p = argv[optind];

    if (!strcmp(p, "0"))
        mode = 0;
    else if (!strcmp(p, "1"))
        mode = 1;
    else if (!strcasecmp(p, "permissive"))
        mode = 0;
    else if (!strcasecmp(p, "enforcing"))
        mode = 1;
    else {
        return 2;
    }

    ret = libxl_flask_setenforce(ctx, mode);

    if (ret) {
        if (errno == ENOSYS) {
            fprintf(stderr, "Flask XSM disabled\n");
        }
        else
            fprintf(stderr, "error occured while setting enforcing mode (%i)\n", ret);
    }

    return ret;
}

int main_loadpolicy(int argc, char **argv)
{
    const char *polFName;
    int polFd = 0;
    void *polMemCp = NULL;
    struct stat info;
    int ret;

    if (optind >= argc) {
        return 2;
    }

    polFName = argv[optind];
    polFd = open(polFName, O_RDONLY);
    if ( polFd < 0 ) {
        fprintf(stderr, "Error occurred opening policy file '%s': %s\n",
                polFName, strerror(errno));
        ret = -1;
        goto done;
    }

    ret = stat(polFName, &info);
    if ( ret < 0 ) {
        fprintf(stderr, "Error occurred retrieving information about"
                "policy file '%s': %s\n", polFName, strerror(errno));
        goto done;
    }

    polMemCp = malloc(info.st_size);

    ret = read(polFd, polMemCp, info.st_size);
    if ( ret < 0 ) {
        fprintf(stderr, "Unable to read new Flask policy file: %s\n",
                strerror(errno));
        goto done;
    }

    ret = libxl_flask_loadpolicy(ctx, polMemCp, info.st_size);

    if (ret < 0) {
        if (errno == ENOSYS) {
            fprintf(stderr, "Flask XSM disabled\n");
        } else {
            errno = -ret;
            fprintf(stderr, "Unable to load new Flask policy: %s\n",
                    strerror(errno));
            ret = -1;
        }
    } else {
        printf("Successfully loaded policy.\n");
    }

done:
    free(polMemCp);
    if ( polFd > 0 )
        close(polFd);

    return ret;
}

int main_remus(int argc, char **argv)
{
    int opt, rc, daemonize = 1;
    const char *ssh_command = "ssh";
    char *host = NULL, *rune = NULL, *domain = NULL;
    libxl_domain_remus_info r_info;
    int send_fd = -1, recv_fd = -1;
    pid_t child = -1;
    uint8_t *config_data;
    int config_len;

    memset(&r_info, 0, sizeof(libxl_domain_remus_info));
    /* Defaults */
    r_info.interval = 200;
    r_info.blackhole = 0;
    r_info.compression = 1;

    while ((opt = def_getopt(argc, argv, "bui:s:e", "remus", 2)) != -1) {
        switch (opt) {
        case 0: case 2:
            return opt;

        case 'i':
	    r_info.interval = atoi(optarg);
            break;
        case 'b':
            r_info.blackhole = 1;
            break;
        case 'u':
	    r_info.compression = 0;
            break;
        case 's':
            ssh_command = optarg;
            break;
        case 'e':
            daemonize = 0;
            break;
        }
    }

    domain = argv[optind];
    host = argv[optind + 1];

    if (r_info.blackhole) {
        find_domain(domain);
        send_fd = open("/dev/null", O_RDWR, 0644);
        if (send_fd < 0) {
            perror("failed to open /dev/null");
            exit(-1);
        }
    } else {

        if (!ssh_command[0]) {
            rune = host;
        } else {
            if (asprintf(&rune, "exec %s %s xl migrate-receive -r %s",
                         ssh_command, host,
                         daemonize ? "" : " -e") < 0)
                return 1;
        }

// 没有使用上，临时注释
//        save_domain_core_begin(domain, NULL, &config_data, &config_len);

        if (!config_len) {
            fprintf(stderr, "No config file stored for running domain and "
                    "none supplied - cannot start remus.\n");
            exit(1);
        }

        child = create_migration_child(rune, &send_fd, &recv_fd);

        migrate_do_preamble(send_fd, recv_fd, child, config_data, config_len,
                            rune);
    }

    /* Point of no return */
    rc = libxl_domain_remus_start(ctx, &r_info, domid, send_fd, recv_fd, 0);

    /* If we are here, it means backup has failed/domain suspend failed.
     * Try to resume the domain and exit gracefully.
     * TODO: Split-Brain check.
     */
    fprintf(stderr, "remus sender: libxl_domain_suspend failed"
            " (rc=%d)\n", rc);

    if (rc == ERROR_GUEST_TIMEDOUT)
        fprintf(stderr, "Failed to suspend domain at primary.\n");
    else {
        fprintf(stderr, "Remus: Backup failed? resuming domain at primary.\n");
        libxl_domain_resume(ctx, domid, 1, 0);
    }

    close(send_fd);
    return -ERROR_FAIL;
}



#endif

/*
 * Local variables:
 * mode: C
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
