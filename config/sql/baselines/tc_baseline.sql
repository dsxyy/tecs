--*********************************************************************
-- 版权所有 (C)2012, 深圳市中兴通讯股份有限公司。
--
-- 文件名称： cc.sql
-- 文件标识：
-- 内容摘要：
-- 其它说明：
-- 当前版本：
-- 作    者： zte
-- 完成日期： 2012-2-8
--
--    修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
--    修改日期：
--    版 本 号：
--    修 改 人：
--    修改内容：
-- 修改记录2：…
-- 修 改 人：
-- 修改日期：
-- 修改内容：
--*********************************************************************

CREATE TABLE public."_schema_log" (
	version_number       varchar( 64 ) DEFAULT 0 NOT NULL,
	update_by            varchar( 128 ),
	update_at            varchar( 32 ) NOT NULL,
	description          varchar( 512 ),
	CONSTRAINT "_schema_log_pkey" PRIMARY KEY ( version_number )
 );

CREATE TABLE cluster_pool (
	oid                  numeric( 19 ) NOT NULL,
	name                 varchar( 64 ) NOT NULL,
	description          varchar( 512 ),
	register_time        varchar( 32 ) NOT NULL,
	ip_address           varchar( 64 ),
	enabled              int4 NOT NULL,
	is_online            int4 NOT NULL,
	core_free_max        int4 NOT NULL,
	tcu_unit_free_max    int4 NOT NULL,
	tcu_max_total        int4 NOT NULL,
	tcu_free_total       int4 NOT NULL,
	tcu_free_max         int4 NOT NULL,
	disk_max_total       numeric( 19 ) NOT NULL,
	disk_free_total      numeric( 19 ) NOT NULL,
	disk_free_max        numeric( 19 ) NOT NULL,
	share_disk_max_total numeric( 19 ) NOT NULL,
	share_disk_free_total numeric( 19 ) NOT NULL,
	mem_max_total        numeric( 19 ) NOT NULL,
	mem_free_total       numeric( 19 ) NOT NULL,
	mem_free_max         numeric( 19 ) NOT NULL,
	cpu_userate_average  int4 NOT NULL,
	cpu_userate_max      int4 NOT NULL,
	cpu_userate_min      int4 NOT NULL,
	db_name              varchar( 64 ) NOT NULL,
	db_server_url        varchar( 256 ) NOT NULL,
	db_server_port       int4 NOT NULL,
	db_user              varchar( 32 ) NOT NULL,
	db_passwd            varchar( 32 ) NOT NULL,
	host_count           int4 NOT NULL,
	CONSTRAINT cluster_pool_pkey PRIMARY KEY ( oid ),
	CONSTRAINT pk_cluster_pool UNIQUE ( name )
 );

ALTER TABLE cluster_pool ADD CONSTRAINT ck_21 CHECK ( enabled = 0 OR enabled = 1 );

ALTER TABLE cluster_pool ADD CONSTRAINT ck_22 CHECK ( is_online = 0 OR is_online = 1 );

ALTER TABLE cluster_pool ADD CONSTRAINT ck_23 CHECK ( db_server_port >= 0 AND db_server_port <= 65535 );

CREATE TABLE current_alarm (
	oid                  numeric( 19 ) NOT NULL,
	alarm_id             int4 NOT NULL,
	alarm_code           int4 NOT NULL,
	alarm_time           numeric( 19 ) NOT NULL,
	alarm_addr           varchar( 512 ) NOT NULL,
	tc_name              varchar( 64 ),
	cc_name              varchar( 64 ),
	hc_name              varchar( 64 ),
	location             varchar( 512 ),
	addinfo              text,
	alarm_key            text NOT NULL,
	crccode0             int4 NOT NULL,
	crccode1             int4 NOT NULL,
	crccode2             int4 NOT NULL,
	crccode3             int4 NOT NULL,
	CONSTRAINT current_alarm_pkey PRIMARY KEY ( oid ),
	CONSTRAINT current_alarm_alarm_key_key UNIQUE ( alarm_key )
 );

CREATE TABLE deleted_user_pool (
	oid                  numeric( 19 ) NOT NULL,
	name                 varchar( 64 ) NOT NULL,
	description          varchar( 512 ),
	create_time          varchar( 32 ) NOT NULL,
	enabled              int4 NOT NULL,
	role                 int4 NOT NULL,
	group_id             numeric( 19 ) NOT NULL,
	password             varchar( 256 ) NOT NULL,
	email                varchar( 256 ),
	phone                varchar( 256 ),
	location             varchar( 512 ),
	pass_version         varchar( 32 ),
	CONSTRAINT user_pool_pkey_0 PRIMARY KEY ( oid )
 );

ALTER TABLE deleted_user_pool ADD CONSTRAINT ck_25 CHECK ( role >= 1 AND role <= 3 );

ALTER TABLE deleted_user_pool ADD CONSTRAINT ck_26 CHECK ( enabled = 0 OR enabled = 1 );

CREATE TABLE deleted_vmcfg_pool (
	oid                  numeric( 19 ) NOT NULL,
	name                 varchar( 32 ),
	description          varchar( 512 ),
	create_time          varchar( 32 ) NOT NULL,
	vcpu                 int4 NOT NULL,
	tcu                  int4 NOT NULL,
	memory               numeric( 19 ) NOT NULL,
	virt_type            varchar( 16 ) NOT NULL,
	root_device          varchar( 256 ),
	kernel_command       varchar( 256 ),
	bootloader           varchar( 256 ),
	raw_data             varchar( 256 ),
	vnc_password         varchar( 32 ),
	qualification        varchar( 4096 ),
	watchdog             int4 DEFAULT 0 NOT NULL,
	appointed_cluster    varchar( 64 ),
	appointed_host       varchar( 64 ),
	project_id           numeric( 19 ) NOT NULL,
	deployed_cluster     varchar( 64 ),
	CONSTRAINT vmcfg_pool_pkey_0 PRIMARY KEY ( oid )
 );

ALTER TABLE deleted_vmcfg_pool ADD CONSTRAINT ck_27 CHECK ( virt_type = 'hvm' OR virt_type = 'pvm' );

CREATE TABLE history_alarm (
	oid                  numeric( 19 ) NOT NULL,
	alarm_id             int4 NOT NULL,
	alarm_code           int4 NOT NULL,
	alarm_time           numeric( 19 ) NOT NULL,
	alarm_addr           varchar( 512 ) NOT NULL,
	tc_name              varchar( 64 ),
	cc_name              varchar( 64 ),
	hc_name              varchar( 64 ),
	location             varchar( 512 ),
	addinfo              text,
	restore_time         numeric( 19 ) NOT NULL,
	restore_type         int4 NOT NULL,
	CONSTRAINT current_alarm_pkey_0 PRIMARY KEY ( oid )
 );

ALTER TABLE history_alarm ADD CONSTRAINT ck_8 CHECK ( restore_type = 1 );

CREATE TABLE operation_log (
	oid                  numeric( 19 ) NOT NULL,
	uid                  numeric( 19 ),
	user_name            varchar( 32 ),
	client_ip            varchar( 64 ),
	user_agent           varchar( 512 ),
	rpc_name             varchar( 128 ),
	parameters           text,
	returns              text,
	log_calendar_time    numeric( 19 ),
	begin_calendar_time  numeric( 19 ),
	end_calendar_time    numeric( 19 ),
	log_utc_time         varchar( 32 ),
	begin_utc_time       varchar( 32 ),
	end_utc_time         varchar( 32 ),
	visible              int4,
	CONSTRAINT operatorlog_pkey PRIMARY KEY ( oid )
 );

CREATE TABLE snmp_configuration (
	oid                  numeric( 19 ) NOT NULL,
	community            varchar( 64 ) NOT NULL,
	trap_community       varchar( 64 ) NOT NULL,
	trap_version         int4 NOT NULL,
	CONSTRAINT snmp_configuration_pkey PRIMARY KEY ( oid )
 );

ALTER TABLE snmp_configuration ADD CONSTRAINT ck_29 CHECK ( trap_version = 0 OR trap_version = 1 );

CREATE TABLE trap_address (
	oid                  numeric( 19 ) NOT NULL,
	ip_address           varchar( 64 ),
	port                 int4,
	CONSTRAINT trap_address_pkey PRIMARY KEY ( oid ),
	CONSTRAINT idx_trap_address UNIQUE ( ip_address, port )
 );

ALTER TABLE trap_address ADD CONSTRAINT ck_30 CHECK ( port >= 0 AND port <= 65535 );

CREATE TABLE usergroup_pool (
	oid                  numeric( 19 ) NOT NULL,
	name                 varchar( 64 ) NOT NULL,
	description          varchar( 512 ),
	create_time          varchar( 32 ) NOT NULL,
	enabled              int4 NOT NULL,
	type                 int4 NOT NULL,
	CONSTRAINT usergroup_pool_pkey PRIMARY KEY ( oid ),
	CONSTRAINT idx_usergroup_pool UNIQUE ( name )
 );

ALTER TABLE usergroup_pool ADD CONSTRAINT ck_4 CHECK ( enabled = 0 OR enabled = 1 );

CREATE TABLE version (
	oid                  numeric( 19 ) NOT NULL,
	destination          varchar( 256 ),
	grade                int4,
	filename             varchar( 256 ),
	version              varchar( 128 ),
	flag                 int4,
	CONSTRAINT version_pkey PRIMARY KEY ( oid )
 );

CREATE TABLE cluster_netplane (
	cluster_name         varchar( 64 ) NOT NULL,
	netplane             varchar( 32 ) NOT NULL,
	CONSTRAINT idx_cluster_netplane PRIMARY KEY ( cluster_name, netplane )
 );

CREATE TABLE user_pool (
	oid                  numeric( 19 ) NOT NULL,
	name                 varchar( 64 ) NOT NULL,
	description          varchar( 512 ),
	create_time          varchar( 32 ) NOT NULL,
	enabled              int4 NOT NULL,
	role                 int4 NOT NULL,
	group_id             numeric( 19 ) NOT NULL,
	password             varchar( 256 ) NOT NULL,
	email                varchar( 256 ),
	phone                varchar( 256 ),
	location             varchar( 512 ),
	pass_version         varchar( 32 ),
	CONSTRAINT user_pool_pkey PRIMARY KEY ( oid ),
	CONSTRAINT user_pool_user_name_key UNIQUE ( name )
 );

ALTER TABLE user_pool ADD CONSTRAINT ck_2 CHECK ( enabled = 0 OR enabled = 1 );

ALTER TABLE user_pool ADD CONSTRAINT ck_3 CHECK ( role >= 1 AND role <= 3 );

CREATE INDEX idx_user_pool ON user_pool ( group_id );

CREATE TABLE vmtemplate_pool (
	oid                  numeric( 19 ) NOT NULL,
	name                 varchar( 32 ) NOT NULL,
	description          varchar( 512 ),
	create_time          varchar( 32 ) NOT NULL,
	vcpu                 int4 NOT NULL,
	tcu                  int4 NOT NULL,
	memory               numeric( 19 ) NOT NULL,
	virt_type            varchar( 16 ) NOT NULL,
	root_device          varchar( 256 ),
	kernel_command       varchar( 256 ),
	bootloader           varchar( 256 ),
	raw_data             varchar( 256 ),
	vnc_password         varchar( 32 ),
	qualification        varchar( 4096 ),
	watchdog             int4 DEFAULT 0 NOT NULL,
	appointed_cluster    varchar( 64 ),
	appointed_host       varchar( 64 ),
	uid                  numeric( 19 ),
	is_public            int4,
	CONSTRAINT vmcfg_pool_pkey_1 PRIMARY KEY ( oid ),
	CONSTRAINT vmcfg_pool_pkey_1 UNIQUE ( oid ),
	CONSTRAINT idx_vmtemplate_pool UNIQUE ( name )
 );

ALTER TABLE vmtemplate_pool ADD CONSTRAINT ck_0 CHECK ( is_public = 0 OR is_public = 1 );

CREATE INDEX idx_vmtemplate_pool_0 ON vmtemplate_pool ( uid );

CREATE TABLE image_pool (
	image_id             numeric( 19 ) NOT NULL,
	oid                  numeric( 19 ) NOT NULL,
	name                 varchar( 32 ) NOT NULL,
	description          varchar( 512 ),
	register_time        varchar( 32 ) NOT NULL,
	uid                  numeric( 19 ),
	type                 varchar( 16 ) NOT NULL,
	bus                  varchar( 16 ) NOT NULL,
	size                 numeric( 19 ) NOT NULL,
	is_public            int4 NOT NULL,
	enabled              int4 NOT NULL,
	location             varchar( 512 ) NOT NULL,
	fileurl              varchar( 1024 ) NOT NULL,
	disk_format          varchar( 32 ),
	container_format     varchar( 32 ),
	os_type              varchar( 512 ),
	os_version           varchar( 512 ),
	os_arch              varchar( 32 ),
	CONSTRAINT image_pool_pkey PRIMARY KEY ( image_id )
 );

ALTER TABLE image_pool ADD CONSTRAINT ck_9 CHECK ( type = 'machine' OR type = 'kernel' OR type = 'ramdisk' OR type = 'datablock' OR type = 'cdrom' );

ALTER TABLE image_pool ADD CONSTRAINT ck_10 CHECK ( bus = 'ide' OR bus = 'scsi' );

ALTER TABLE image_pool ADD CONSTRAINT ck_11 CHECK ( is_public = 0 OR is_public = 1 );

ALTER TABLE image_pool ADD CONSTRAINT ck_12 CHECK ( enabled = 0 OR enabled = 1 );

CREATE TABLE project_pool (
	oid                  numeric( 19 ) NOT NULL,
	name                 varchar( 32 ) NOT NULL,
	description          varchar( 512 ),
	create_time          varchar( 32 ) NOT NULL,
	uid                  numeric( 19 ) NOT NULL,
	CONSTRAINT project_pool_pkey PRIMARY KEY ( oid ),
	CONSTRAINT project_pool_uid_name_key UNIQUE ( uid, name )
 );

CREATE INDEX idx_project_pool ON project_pool ( uid );

CREATE TABLE snmp_user (
	user_name            varchar( 32 ) NOT NULL,
	CONSTRAINT pk_snmp_user UNIQUE ( user_name )
 );

CREATE TABLE vmcfg_pool (
	oid                  numeric( 19 ) NOT NULL,
	name                 varchar( 32 ),
	description          varchar( 512 ),
	create_time          varchar( 32 ) NOT NULL,
	vcpu                 int4 NOT NULL,
	tcu                  int4 NOT NULL,
	memory               numeric( 19 ) NOT NULL,
	virt_type            varchar( 16 ) NOT NULL,
	root_device          varchar( 256 ),
	kernel_command       varchar( 256 ),
	bootloader           varchar( 256 ),
	raw_data             varchar( 256 ),
	vnc_password         varchar( 32 ),
	qualification        varchar( 4096 ),
	watchdog             int4 DEFAULT 0 NOT NULL,
	appointed_cluster    varchar( 64 ),
	appointed_host       varchar( 64 ),
	project_id           numeric( 19 ) NOT NULL,
	deployed_cluster     varchar( 64 ),
	CONSTRAINT vmcfg_pool_pkey PRIMARY KEY ( oid ),
	CONSTRAINT vmcfg_pool_pkey UNIQUE ( oid )
 );

ALTER TABLE vmcfg_pool ADD CONSTRAINT ck_6 CHECK ( virt_type = 'hvm' OR virt_type = 'pvm' );

CREATE INDEX idx_vmcfg_pool ON vmcfg_pool ( project_id );

CREATE INDEX idx_vmcfg_pool_0 ON vmcfg_pool ( deployed_cluster );

CREATE INDEX idx_vmcfg_pool_1 ON vmcfg_pool ( appointed_cluster );

CREATE TABLE vmcfg_repel (
	vid1                 numeric( 19 ) NOT NULL,
	vid2                 numeric( 19 ) NOT NULL,
	CONSTRAINT idx_vmcfg_repel PRIMARY KEY ( vid1, vid2 )
 );

CREATE INDEX idx_vmcfg_repel_0 ON vmcfg_repel ( vid1 );

CREATE INDEX idx_vmcfg_repel_1 ON vmcfg_repel ( vid2 );

CREATE TABLE vmtemplate_context (
	file_name            varchar( 256 ) NOT NULL,
	file_content         text,
	vmtemplate_id        numeric( 19 ) NOT NULL,
	CONSTRAINT idx_vmcfg_context_1 PRIMARY KEY ( vmtemplate_id, file_name )
 );

CREATE TABLE vmtemplate_disk (
	vmtemplate_id        numeric( 19 ) NOT NULL,
	target               varchar( 16 ) NOT NULL,
	position             int4 DEFAULT 0 NOT NULL,
	bus                  varchar( 16 ) NOT NULL,
	size                 numeric( 19 ) NOT NULL,
	fstype               varchar( 16 ) NOT NULL,
	CONSTRAINT idx_vm_disk_1 PRIMARY KEY ( vmtemplate_id, target )
 );

ALTER TABLE vmtemplate_disk ADD CONSTRAINT ck_5 CHECK ( position >= 0 AND position <= 2 );

ALTER TABLE vmtemplate_disk ADD CONSTRAINT ck_24 CHECK ( bus = 'ide' OR bus = 'scsi' );

ALTER TABLE vmtemplate_disk ADD CONSTRAINT ck_31 CHECK ( fstype = '' OR fstype = 'ext2' OR fstype = 'ext3' OR fstype = 'ext4' );

CREATE TABLE vmtemplate_image (
	vmtemplate_id        numeric( 19 ) NOT NULL,
	target               varchar( 16 ) NOT NULL,
	position             int4 DEFAULT 0 NOT NULL,
	image_id             numeric( 19 ) NOT NULL
 );

ALTER TABLE vmtemplate_image ADD CONSTRAINT ck_1 CHECK ( position >= 0 AND position <= 2 );

CREATE INDEX idx_vmcfg_image_3 ON vmtemplate_image ( image_id );

CREATE TABLE vmtemplate_nic (
	vmtemplate_id        numeric( 19 ) NOT NULL,
	nic_index            int4 NOT NULL,
	is_sriov             int4 NOT NULL,
	model                varchar( 32 ) NOT NULL,
	netplane             varchar( 32 ) NOT NULL,
	user_vlan            int4 NOT NULL,
	is_vlan_trunk        int4 NOT NULL,
	is_dhcp              int4 NOT NULL,
	ip_address           varchar( 64 ),
	netmask              varchar( 64 ),
	gateway              varchar( 64 )
 );

ALTER TABLE vmtemplate_nic ADD CONSTRAINT ck_32 CHECK ( is_sriov = 0 OR is_sriov = 1 );

ALTER TABLE vmtemplate_nic ADD CONSTRAINT ck_33 CHECK ( is_vlan_trunk = 0 OR is_vlan_trunk =1 );

ALTER TABLE vmtemplate_nic ADD CONSTRAINT ck_34 CHECK ( is_dhcp = 0 OR is_dhcp = 1 );

ALTER TABLE vmtemplate_nic ADD CONSTRAINT ck_35 CHECK ( model = 'e1000' OR model = 'rtl8139' OR model = 'ne2k_pci' OR model = 'pcnet' OR model = 'e100' OR model = 'netfront' OR model = '' );

ALTER TABLE vmtemplate_nic ADD CONSTRAINT ck_36 CHECK ( user_vlan >= 0 AND user_vlan <= 4096 );

CREATE TABLE vmcfg_context (
	vid                  numeric( 19 ) NOT NULL,
	file_name            varchar( 256 ) NOT NULL,
	file_content         text,
	CONSTRAINT idx_vmcfg_context PRIMARY KEY ( vid, file_name )
 );

CREATE TABLE vmcfg_disk (
	vid                  numeric( 19 ) NOT NULL,
	target               varchar( 16 ) NOT NULL,
	position             int4 DEFAULT 0 NOT NULL,
	bus                  varchar( 16 ) NOT NULL,
	size                 numeric( 19 ) NOT NULL,
	fstype               varchar( 16 ) NOT NULL,
	CONSTRAINT idx_vm_disk PRIMARY KEY ( vid, target )
 );

ALTER TABLE vmcfg_disk ADD CONSTRAINT ck_13 CHECK ( position >= 0 AND position <= 2 );

ALTER TABLE vmcfg_disk ADD CONSTRAINT ck_14 CHECK ( bus = 'ide' OR bus = 'scsi' );

ALTER TABLE vmcfg_disk ADD CONSTRAINT ck_15 CHECK ( fstype = '' OR fstype = 'ext2' OR fstype = 'ext3' OR fstype = 'ext4' );

CREATE INDEX idx_vm_disk_0 ON vmcfg_disk ( vid );

CREATE TABLE vmcfg_image (
	vid                  numeric( 19 ) NOT NULL,
	target               varchar( 16 ) NOT NULL,
	position             int4 DEFAULT 0 NOT NULL,
	image_id             numeric( 19 ) NOT NULL,
	CONSTRAINT idx_vmcfg_image PRIMARY KEY ( vid, target )
 );

ALTER TABLE vmcfg_image ADD CONSTRAINT ck_7 CHECK ( position >= 0 AND position <= 2 );

CREATE INDEX idx_vmcfg_image_0 ON vmcfg_image ( image_id );

CREATE INDEX idx_vmcfg_image_1 ON vmcfg_image ( vid );

CREATE TABLE vmcfg_nic (
	vid                  numeric( 19 ) NOT NULL,
	nic_index            int4 NOT NULL,
	is_sriov             int4 NOT NULL,
	model                varchar( 32 ) NOT NULL,
	netplane             varchar( 32 ) NOT NULL,
	user_vlan            int4 NOT NULL,
	is_vlan_trunk        int4 NOT NULL,
	is_dhcp              int4 NOT NULL,
	ip_address           varchar( 64 ),
	netmask              varchar( 64 ),
	gateway              varchar( 64 ),
	CONSTRAINT idx_vmcfg_nic PRIMARY KEY ( vid, nic_index )
 );

ALTER TABLE vmcfg_nic ADD CONSTRAINT ck_16 CHECK ( is_sriov = 0 OR is_sriov = 1 );

ALTER TABLE vmcfg_nic ADD CONSTRAINT ck_17 CHECK ( is_vlan_trunk = 0 OR is_vlan_trunk =1 );

ALTER TABLE vmcfg_nic ADD CONSTRAINT ck_18 CHECK ( is_dhcp = 0 OR is_dhcp = 1 );

ALTER TABLE vmcfg_nic ADD CONSTRAINT ck_19 CHECK ( model = 'e1000' OR model = 'rtl8139' OR model = 'ne2k_pci' OR model = 'pcnet' OR model = 'netfront' OR model = '' );

ALTER TABLE vmcfg_nic ADD CONSTRAINT ck_20 CHECK ( user_vlan >= 0 AND user_vlan <= 4095 );

CREATE TABLE vmcfg_vsi (
	vid                  numeric( 19 ) NOT NULL,
	nic_index            int4 NOT NULL,
	vsi_id               varchar( 64 ) NOT NULL,
	vsi_id_format        int4 NOT NULL,
	mgr_id               varchar( 64 ) NOT NULL,
	vsi_type_id          varchar( 64 ) NOT NULL,
	vsi_type_ver         int4 NOT NULL,
	CONSTRAINT idx_vmcfg_vsi PRIMARY KEY ( vid, nic_index ),
	CONSTRAINT idx_vmcfg_vsi_0 UNIQUE ( vsi_id )
 );

CREATE TABLE vmcfg_mac (
	vid                  numeric( 19 ) NOT NULL,
	nic_index            int4 NOT NULL,
	mac                  int4 NOT NULL,
	CONSTRAINT idx_vmcfg_mac_0 UNIQUE ( mac ),
	CONSTRAINT idx_vmcfg_mac PRIMARY KEY ( vid, nic_index )
 );

ALTER TABLE cluster_netplane ADD CONSTRAINT fk_cluster_netplane FOREIGN KEY ( cluster_name ) REFERENCES cluster_pool( name ) ON DELETE CASCADE;

ALTER TABLE image_pool ADD CONSTRAINT fk_image_pool_user_pool FOREIGN KEY ( uid ) REFERENCES user_pool( oid ) ON DELETE SET NULL;

ALTER TABLE project_pool ADD CONSTRAINT fk_project_pool_user_pool FOREIGN KEY ( uid ) REFERENCES user_pool( oid ) ON DELETE CASCADE;

ALTER TABLE snmp_user ADD CONSTRAINT fk_snmp_user_user_pool FOREIGN KEY ( user_name ) REFERENCES user_pool( name ) ON DELETE CASCADE;

ALTER TABLE user_pool ADD CONSTRAINT fk_user_pool_usergroup_pool FOREIGN KEY ( group_id ) REFERENCES usergroup_pool( oid ) ON DELETE RESTRICT;

ALTER TABLE vmcfg_context ADD CONSTRAINT fk_vmcfg_context_vmcfg_pool FOREIGN KEY ( vid ) REFERENCES vmcfg_pool( oid ) ON DELETE CASCADE;

ALTER TABLE vmcfg_disk ADD CONSTRAINT fk_vm_disk_vmcfg_pool FOREIGN KEY ( vid ) REFERENCES vmcfg_pool( oid ) ON DELETE CASCADE;

ALTER TABLE vmcfg_image ADD CONSTRAINT fk_vmcfg_image_image_pool FOREIGN KEY ( image_id ) REFERENCES image_pool( image_id ) ON DELETE RESTRICT;

ALTER TABLE vmcfg_image ADD CONSTRAINT fk_vmcfg_image_vmcfg_pool FOREIGN KEY ( vid ) REFERENCES vmcfg_pool( oid ) ON DELETE CASCADE;

ALTER TABLE vmcfg_mac ADD CONSTRAINT fk_vmcfg_mac_vmcfg_nic FOREIGN KEY ( vid, nic_index ) REFERENCES vmcfg_nic( vid, nic_index ) ON DELETE CASCADE;

ALTER TABLE vmcfg_nic ADD CONSTRAINT fk_vmcfg_nic_vmcfg_pool FOREIGN KEY ( vid ) REFERENCES vmcfg_pool( oid ) ON DELETE CASCADE;

ALTER TABLE vmcfg_pool ADD CONSTRAINT fk_vmcfg_pool_project_pool FOREIGN KEY ( project_id ) REFERENCES project_pool( oid ) ON DELETE RESTRICT;

ALTER TABLE vmcfg_pool ADD CONSTRAINT fk_vmcfg_pool_cluster_pool FOREIGN KEY ( deployed_cluster ) REFERENCES cluster_pool( name ) ON DELETE RESTRICT;

ALTER TABLE vmcfg_pool ADD CONSTRAINT fk_vmcfg_pool_cluster_pool_1 FOREIGN KEY ( appointed_cluster ) REFERENCES cluster_pool( name ) ON DELETE SET NULL;

ALTER TABLE vmcfg_repel ADD CONSTRAINT fk_vmcfg_repel_vmcfg_pool FOREIGN KEY ( vid1 ) REFERENCES vmcfg_pool( oid ) ON DELETE CASCADE;

ALTER TABLE vmcfg_repel ADD CONSTRAINT fk_vmcfg_repel_vmcfg_pool_1 FOREIGN KEY ( vid2 ) REFERENCES vmcfg_pool( oid ) ON DELETE CASCADE;

ALTER TABLE vmcfg_vsi ADD CONSTRAINT fk_vmcfg_vsi_vmcfg_nic FOREIGN KEY ( vid, nic_index ) REFERENCES vmcfg_nic( vid, nic_index ) ON DELETE CASCADE;

ALTER TABLE vmtemplate_context ADD CONSTRAINT fk_vmtemplate_context FOREIGN KEY ( vmtemplate_id ) REFERENCES vmtemplate_pool( oid ) ON DELETE CASCADE;

ALTER TABLE vmtemplate_disk ADD CONSTRAINT fk_vmtemplate_disk FOREIGN KEY ( vmtemplate_id ) REFERENCES vmtemplate_pool( oid ) ON DELETE CASCADE;

ALTER TABLE vmtemplate_image ADD CONSTRAINT fk_vmtemplate_image FOREIGN KEY ( vmtemplate_id ) REFERENCES vmtemplate_pool( oid ) ON DELETE CASCADE;

ALTER TABLE vmtemplate_image ADD CONSTRAINT fk_vmtemplate_image_image_pool FOREIGN KEY ( image_id ) REFERENCES image_pool( image_id ) ON DELETE RESTRICT;

ALTER TABLE vmtemplate_nic ADD CONSTRAINT fk_vmtemplate_nic FOREIGN KEY ( vmtemplate_id ) REFERENCES vmtemplate_pool( oid ) ON DELETE CASCADE;

ALTER TABLE vmtemplate_pool ADD CONSTRAINT fk_vmtemplate_pool_user_pool FOREIGN KEY ( uid ) REFERENCES user_pool( oid ) ON DELETE SET NULL;

INSERT INTO _schema_log(version_number, update_by,update_at,description) VALUES ('01.01.10.0000', 'tecs',now(),'baseline');


