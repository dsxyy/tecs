/*******************************************************************************

  LLDP Agent Daemon (LLDPAD) Software
  Copyright(c) 2007-2010 Intel Corporation.

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information:
  open-lldp Mailing List <lldp-devel@open-lldp.org>

*******************************************************************************/

#ifndef _LLDP_H
#define _LLDP_H

/*#include <asm/types.h>*/
/*#include <stdbool.h>*/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _EVB_TECS 
#include "evb_comm.h"

#ifndef Brs_Malloc
#define Brs_Malloc(a,b)      GLUE_ALLOC(b)
#define Brs_Free             GLUE_FREE
#endif

#ifndef BUG_ASSERT
#define BUG_ASSERT(X)
#endif

#endif 


#ifdef SWITCH_VERSION

#ifndef htons
#define htons(x)     HTONS(x)
#define ntohs(x)     NTOHS(x)
#define htonl(x)     HTONL(x)
#define ntohl(x)     NTOHL(x)
#endif

#define true                 1
#define false                0
#define bool                 u8
#define __func__             ""

#ifndef GLUE_ALLOC
#define GLUE_ALLOC(x)        XOS_GetUB(x)
#define GLUE_FREE(x)         XOS_RetUB(x)
#endif

#ifndef Brs_Malloc
#define Brs_Malloc(a,b)      GLUE_ALLOC(b)
#define Brs_Free             GLUE_FREE
#endif

#ifndef BUG_ASSERT
#define BUG_ASSERT(X)
#endif

/* * List declarations. */
#define	LIST_HEAD(name, type)						\
struct name {								\
	struct type *lh_first;	/* first element */			\
}

#define	LIST_HEAD_INITIALIZER(head)					\
	{ NULL }

#define	LIST_ENTRY(type)						\
struct {								\
	struct type *le_next;	/* next element */			\
	struct type **le_prev;	/* address of previous next element */	\
}

/* * List functions. */

#define	LIST_EMPTY(head)	((head)->lh_first == NULL)

#define	LIST_FIRST(head)	((head)->lh_first)

#define	LIST_FOREACH(var, head, field)					\
	for ((var) = LIST_FIRST((head));				\
	    (var);							\
	    (var) = LIST_NEXT((var), field))

#define	LIST_FOREACH_SAFE(var, head, field, tvar)			\
	for ((var) = LIST_FIRST((head));				\
	    (var) && ((tvar) = LIST_NEXT((var), field), 1);		\
	    (var) = (tvar))

#define	LIST_INIT(head) do {						\
	LIST_FIRST((head)) = NULL;					\
} while (0)

#define	LIST_INSERT_AFTER(listelm, elm, field) do {			\
	if ((LIST_NEXT((elm), field) = LIST_NEXT((listelm), field)) != NULL)\
		LIST_NEXT((listelm), field)->field.le_prev =		\
		    &LIST_NEXT((elm), field);				\
	LIST_NEXT((listelm), field) = (elm);				\
	(elm)->field.le_prev = &LIST_NEXT((listelm), field);		\
} while (0)

#define	LIST_INSERT_BEFORE(listelm, elm, field) do {			\
	(elm)->field.le_prev = (listelm)->field.le_prev;		\
	LIST_NEXT((elm), field) = (listelm);				\
	*(listelm)->field.le_prev = (elm);				\
	(listelm)->field.le_prev = &LIST_NEXT((elm), field);		\
} while (0)

#define	LIST_INSERT_HEAD(head, elm, field) do {				\
	if ((LIST_NEXT((elm), field) = LIST_FIRST((head))) != NULL)	\
		LIST_FIRST((head))->field.le_prev = &LIST_NEXT((elm), field);\
	LIST_FIRST((head)) = (elm);					\
	(elm)->field.le_prev = &LIST_FIRST((head));			\
} while (0)

#define	LIST_NEXT(elm, field)	((elm)->field.le_next)

#define	LIST_REMOVE(elm, field) do {					\
	if (LIST_NEXT((elm), field) != NULL)				\
		LIST_NEXT((elm), field)->field.le_prev = 		\
		    (elm)->field.le_prev;				\
	*(elm)->field.le_prev = LIST_NEXT((elm), field);		\
} while (0)
#endif

#ifndef _WIN32
#ifndef  u8
#define  u8 unsigned char
#define  u16 unsigned short
#define  u32 unsigned int
#define  u64 unsigned long long
#define  socklen_t unsigned int
#endif
#endif


#define LLDPAD_ERR          if ( g_print ) printf
#define LLDPAD_WARN         if ( g_print ) printf
#define LLDPAD_INFO         if ( g_print ) printf
#define LLDPAD_DBG          if ( g_print ) printf

#define MAX_DEVICE_NAME_LEN         256 /* NDIS supports 256 */
#define LLDP_MOD_MAND               1
#define LLDP_TIMER_FREQUENCY        1000

#define is_router(x)        1
#define is_vlan(x)          0
#define is_bridge(x)        1
#define is_wlan(x)          0
#define is_phone(x)         0
#define is_docsis(x)        0
#define is_repeater(x)      0
#define is_tpmr(x)          0
#define is_other(x)         0

typedef enum {
    ret_success = 0,
    ret_error,
    ret_failed,
}ret_status;


typedef enum {
    cmd_success = 0,
    cmd_failed,
    cmd_device_not_found,
    cmd_invalid,
    cmd_bad_params,
    cmd_peer_not_present,
    cmd_ctrl_vers_not_compatible,
    cmd_not_capable,
    cmd_not_applicable,
    cmd_no_access,
}cmd_status;

typedef enum {
    op_local     = 0x0001,
    op_neighbor  = 0x0002,
    op_arg       = 0x0004,
    op_argval    = 0x0008,
    op_config    = 0x0010,
    op_delete    = 0x0020,
} lldp_op;

#ifndef __u8
#define __u8  u8
#define __u16 u16
#define __u32 u32
#define __u64 u64
#define bool  u8
#endif


#ifndef NULL
#define NULL 0
#endif

#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif

#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif

#ifndef ntoh24
#define ntoh24(p) (((p)[0] << 16) | ((p)[1] << 8) | ((p)[2]))
#endif

#ifndef hton24
#define hton24(p, v)    do {            \
        p[0] = (((v) >> 16) & 0xFF);    \
        p[1] = (((v) >> 8) & 0xFF); \
        p[2] = ((v) & 0xFF);        \
    } while (0)
#endif

/*
 * Organizationally Unique Identifier (OUI)
 * http://standards.ieee.org/regauth/oui/oui.txt
 */

#define INIT_IEEE8021_OUI   {0x00, 0x80, 0xc2}

#define OUI_SIZE    3
#define SUB_SIZE    1
#define OUI_SUB_SIZE    (OUI_SIZE + SUB_SIZE)
#define OUI_INTEL_CORP  0x001b21
#define OUI_CEE_DCBX    OUI_INTEL_CORP
#define OUI_IEEE_8021   0x0080c2
#define OUI_IEEE_8023   0x00120f
#define OUI_IEEE_80211  0x000fac
/* Telecommunications Industry Association TR-41 Committee */
#define OUI_TIA_TR41    0x0012bb

#define OUI_IEEE_8021Qbg    0x001b3f

/* IEEE 802.3AB Clause 9: TLV Types */
#define CHASSIS_ID_TLV          1
#define PORT_ID_TLV             2
#define TIME_TO_LIVE_TLV        3
#define PORT_DESCRIPTION_TLV    4
#define SYSTEM_NAME_TLV         5
#define SYSTEM_DESCRIPTION_TLV  6
#define SYSTEM_CAPABILITIES_TLV 7
#define MANAGEMENT_ADDRESS_TLV  8
#define ORG_SPECIFIC_TLV        127
#define END_OF_LLDPDU_TLV       0

/* IEEE 802.3AB Clause 9.5.2: Chassis subtypes */
#define CHASSIS_ID_RESERVED          0
#define CHASSIS_ID_CHASSIS_COMPONENT 1
#define CHASSIS_ID_INTERFACE_ALIAS   2
#define CHASSIS_ID_PORT_COMPONENT    3
#define CHASSIS_ID_MAC_ADDRESS       4
#define CHASSIS_ID_NETWORK_ADDRESS   5
#define CHASSIS_ID_INTERFACE_NAME    6
#define CHASSIS_ID_LOCALLY_ASSIGNED  7
#define CHASSIS_ID_INVALID(t)   (((t) == 0) || ((t) > 7))

/* IEEE 802.3AB Clause 9.5.3: Port subtype */
#define PORT_ID_RESERVED         0
#define PORT_ID_INTERFACE_ALIAS  1
#define PORT_ID_PORT_COMPONENT   2
#define PORT_ID_MAC_ADDRESS      3
#define PORT_ID_NETWORK_ADDRESS  4
#define PORT_ID_INTERFACE_NAME   5
#define PORT_ID_AGENT_CIRCUIT_ID 6
#define PORT_ID_LOCALLY_ASSIGNED 7
#define PORT_ID_INVALID(t)  (((t) == 0) || ((t) > 7))

/* IEEE 802.1AB: 8.5.8, Table 8-4: System Capabilities */
#define SYSCAP_OTHER    (1 << 0)
#define SYSCAP_REPEATER (1 << 1)
#define SYSCAP_BRIDGE   (1 << 2)
#define SYSCAP_WLAN     (1 << 3)
#define SYSCAP_ROUTER   (1 << 4)
#define SYSCAP_PHONE    (1 << 5)
#define SYSCAP_DOCSIS   (1 << 6)
#define SYSCAP_STATION  (1 << 7)
#define SYSCAP_CVLAN    (1 << 8)    /* TPID: 0x8100 */
#define SYSCAP_SVLAN    (1 << 9)    /* TPID: 0x88a8 */
#define SYSCAP_TPMR     (1 << 10)
#define SYSCAP_BITMASK  0x0fff

/*
 * IETF RFC 3232:
 * http://www.iana.org/assignments/ianaaddressfamilynumbers-mib
 */
enum {
    MANADDR_OTHER            =  0,
    MANADDR_IPV4             =  1,
    MANADDR_IPV6             =  2,
    MANADDR_NSAP             =  3,
    MANADDR_HDLC             =  4,
    MANADDR_BBN1822          =  5,
    MANADDR_ALL802           =  6,
    MANADDR_E163             =  7,
    MANADDR_E164             =  8,
    MANADDR_F69              =  9,
    MANADDR_X121             =  10,
    MANADDR_IPX              =  11,
    MANADDR_APPLETALK        =  12,
    MANADDR_DECNETIV         =  13,
    MANADDR_BANYANVINES      =  14,
    MANADDR_E164WITHNSAP     =  15,
    MANADDR_DNS              =  16,
    MANADDR_DISTINGUISHEDNAME=  17,
    MANADDR_ASNUMBER         =  18,
    MANADDR_XTPOVERIPV4      =  19,
    MANADDR_XTPOVERIPV6      =  20,
    MANADDR_XTPNATIVEMODEXTP =  21,
    MANADDR_FIBRECHANNELWWPN =  22,
    MANADDR_FIBRECHANNELWWNN =  23,
    MANADDR_GWID             =  24,
    MANADDR_AFI              =  25,
    MANADDR_RESERVED         =  65535
}; 
#define MANADDR_MAX (MANADDR_AFI - MANADDR_OTHER + 2)

#define IFNUM_UNKNOWN      1
#define IFNUM_IFINDEX      2
#define IFNUM_SYS_PORT_NUM 3

/* LLDP-MED subtypes */
#define LLDP_MED_RESERVED           0
#define LLDP_MED_CAPABILITIES       1
#define LLDP_MED_NETWORK_POLICY     2
#define LLDP_MED_LOCATION_ID        3
#define LLDP_MED_EXTENDED_PVMDI     4
#define LLDP_MED_INV_HWREV          5
#define LLDP_MED_INV_FWREV          6
#define LLDP_MED_INV_SWREV          7
#define LLDP_MED_INV_SERIAL         8
#define LLDP_MED_INV_MANUFACTURER   9
#define LLDP_MED_INV_MODELNAME      10
#define LLDP_MED_INV_ASSETID        11

/* LLDP-MED Capability Values: ANSI/TIA-1057-2006, 10.2.2.1, Table 10 */
#define LLDP_MED_CAPABILITY_CAPAPILITIES    (1 << 0)
#define LLDP_MED_CAPABILITY_NETWORK_POLICY  (1 << 1)
#define LLDP_MED_CAPABILITY_LOCATION_ID     (1 << 2)
#define LLDP_MED_CAPABILITY_EXTENDED_PSE    (1 << 3)
#define LLDP_MED_CAPABILITY_EXTENDED_PD     (1 << 4)
#define LLDP_MED_CAPABILITY_INVENTORY       (1 << 5)

/* LLDP-MED Device Type Values: ANSI/TIA-1057-2006, 10.2.2.2, Table 11 */
#define LLDP_MED_DEVTYPE_NOT_DEFINED            0
#define LLDP_MED_DEVTYPE_ENDPOINT_CLASS_I       1
#define LLDP_MED_DEVTYPE_ENDPOINT_CLASS_II      2
#define LLDP_MED_DEVTYPE_ENDPOINT_CLASS_III     3
#define LLDP_MED_DEVTYPE_NETWORK_CONNECTIVITY   4
#define LLDP_MED_DEVTYPE_INVALID(t) ((t) > 4)
#define LLDP_MED_DEVTYPE_DEFINED(t) \
    (((t) == LLDP_MED_DEVTYPE_ENDPOINT_CLASS_I) ||  \
     ((t) == LLDP_MED_DEVTYPE_ENDPOINT_CLASS_II) || \
     ((t) == LLDP_MED_DEVTYPE_ENDPOINT_CLASS_III) ||\
     ((t) == LLDP_MED_DEVTYPE_NETWORK_CONNECTIVITY))

#define LLDP_MED_LOCID_INVALID          0
#define LLDP_MED_LOCID_COORDINATE       1 
#define LLDP_MED_LOCID_CIVIC_ADDRESS    2
#define LLDP_MED_LOCID_ECS_ELIN         3
#define LLDP_MED_LOCID_FORMAT_INVALID(t) (((t) == 0) || ((t) > 3))

/* IEEE 802.1Qau Organizationally Specific TLV Subtypes */
#define LLDP_8021QAU_QCN    8

/* IEEE 802.1Qaz Organizationally Specific TLV Subtypes */
#define LLDP_8021QAZ_ETSCFG 9
#define LLDP_8021QAZ_ETSREC 10
#define LLDP_8021QAZ_PFC    11
#define LLDP_8021QAZ_APP    12

/* IEEE 802.3 Organizationally Specific TLV Subtypes: 802.1AB-2005 Annex G */
#define LLDP_8023_RESERVED              0
#define LLDP_8023_MACPHY_CONFIG_STATUS  1
#define LLDP_8023_POWER_VIA_MDI         2   
#define LLDP_8023_LINK_AGGREGATION      3
#define LLDP_8023_MAXIMUM_FRAME_SIZE    4
/* 802.3AB-2005 G.2.1 Table G-2 */
#define LLDP_8023_MACPHY_AUTONEG_SUPPORT    (1 << 0)
#define LLDP_8023_MACPHY_AUTONEG_ENABLED    (1 << 1)
/* 802.3AB-2005 G.4.1 Table G-4 */
#define LLDP_8023_LINKAGG_CAPABLE   (1 << 0)
#define LLDP_8023_LINKAGG_ENABLED   (1 << 1)

/* IEEE 802.1Qbg subtype */
#define LLDP_EVB_SUBTYPE        0x0
#define LLDP_CDCP_SUBTYPE       0x1
#define LLDP_VDP_SUBTYPE        0x2

/* forwarding mode */
#define LLDP_EVB_CAPABILITY_FORWARD_STANDARD        (1 << 7)
#define LLDP_EVB_CAPABILITY_FORWARD_REFLECTIVE_RELAY    (1 << 6)

/* EVB supported protocols */
#define LLDP_EVB_CAPABILITY_PROTOCOL_RTE        (1 << 2)
#define LLDP_EVB_CAPABILITY_PROTOCOL_ECP        (1 << 1)
#define LLDP_EVB_CAPABILITY_PROTOCOL_VDP        (1 << 0)

/* EVB specific values */
#define LLDP_EVB_DEFAULT_MAX_VSI            65535
#define LLDP_EVB_DEFAULT_SVSI               3295
#define LLDP_EVB_DEFAULT_RTE                15


#ifdef __cplusplus
}
#endif

#endif /* _LLDP_H */

