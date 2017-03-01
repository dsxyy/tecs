
--*********************************************************************
-- 版权所有 (C)2012, 深圳市中兴通讯股份有限公司。
--
-- 文件名称： vnm.sql
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



CREATE TABLE  "_schema_log" ( 
	version_number       varchar( 64 ) DEFAULT 0 NOT NULL,
	update_by            varchar( 128 ),
	update_at            varchar( 32 ),
	description          varchar( 512 ),
	CONSTRAINT pk__schema_log PRIMARY KEY ( version_number )
 );

CREATE TABLE  netplane ( 
	id                   numeric( 19 ) NOT NULL,
	name                 varchar( 32 ) NOT NULL,
	uuid                 varchar( 64 ) NOT NULL,
	description          varchar( 512 ) NOT NULL,
	mtu                  int4 DEFAULT 1500 NOT NULL,
	CONSTRAINT pk_netplane PRIMARY KEY ( id ),
	CONSTRAINT idx_netplane UNIQUE ( name ),
	CONSTRAINT idx_netplane_0 UNIQUE ( uuid )
 );

CREATE TABLE  port_group ( 
	id                   numeric( 19 ) NOT NULL,
	name                 varchar( 64 ) NOT NULL,
	pg_type              int4 DEFAULT 0 NOT NULL,
	uuid                 varchar( 64 ) NOT NULL,
	"acl"                varchar( 64 ) NOT NULL,
	qos                  varchar( 64 ) NOT NULL,
	allowed_priorities   varchar( 32 ) NOT NULL,
	receive_bandwidth_limit varchar( 32 ) NOT NULL,
	receive_bandwidth_reserve varchar( 32 ) NOT NULL,
	default_vlan_id      int4 DEFAULT 0 NOT NULL,
	promiscuous          int4 DEFAULT 0 NOT NULL,
	macvlan_filter_enable int4 DEFAULT 0 NOT NULL,
	allowed_transmit_macs varchar( 64 ) NOT NULL,
	allowed_transmit_macvlans varchar( 64 ) NOT NULL,
	policy_block_override varchar( 64 ) NOT NULL,
	policy_vlan_override varchar( 64 ) NOT NULL,
	"version"            int4 NOT NULL,
	mgr_id               varchar( 64 ) NOT NULL,
	type_id              varchar( 64 ) NOT NULL,
	allow_mac_change     int4 DEFAULT 0 NOT NULL,
	switchport_mode      int4 DEFAULT 0 NOT NULL,
	netplane_id          numeric( 19 ),
	mtu                  int4 DEFAULT 1500 NOT NULL,
	description          varchar( 512 ),
	CONSTRAINT pk_port_profile_group_4 PRIMARY KEY ( id ),
	CONSTRAINT pk_port_profile_group_5 UNIQUE ( name ),
	CONSTRAINT pk_port_group UNIQUE ( id, switchport_mode ),
	CONSTRAINT pk_port_group_0 UNIQUE ( id, pg_type ),
	CONSTRAINT pk_port_group_1 UNIQUE ( id, switchport_mode, pg_type ),
	CONSTRAINT pk_port_group_2 UNIQUE ( id, pg_type, switchport_mode ),
	CONSTRAINT idx_port_group_0 UNIQUE ( uuid )
 );

ALTER TABLE  port_group ADD CONSTRAINT ck_27 CHECK ( switchport_mode = 0 OR switchport_mode = 1 );

ALTER TABLE  port_group ADD CONSTRAINT ck_28 CHECK ( promiscuous = 0 OR promiscuous = 1 OR promiscuous = 2 OR promiscuous = 3 );

ALTER TABLE  port_group ADD CONSTRAINT ck_29 CHECK ( allow_mac_change = 0 OR allow_mac_change = 1 );

ALTER TABLE  port_group ADD CONSTRAINT ck_47 CHECK ( macvlan_filter_enable = 0 OR macvlan_filter_enable = 1 );

ALTER TABLE  port_group ADD CONSTRAINT ck_65 CHECK ( pg_type = 0 OR pg_type = 10 OR pg_type = 11 OR pg_type = 20 OR pg_type = 21 );

CREATE INDEX idx_port_group ON  port_group ( netplane_id );

COMMENT ON COLUMN  port_group.name IS '端口组名称';

COMMENT ON COLUMN  port_group.pg_type IS '=0 uplink pg; n=10 kernel_mgr pg; n=11 kernel_storage pg; n=20 vm_share pg; n=21 vm_private pg;n';

COMMENT ON COLUMN  port_group.allowed_priorities IS 'allowed values of 802.1Q PCP bitsllowedPriorities[] llowed values of 802.1Q PCP bits nthat this port is allowed to transmit';

COMMENT ON COLUMN  port_group.receive_bandwidth_limit IS 'Maximum receive bandwidth this port is allowed to use';

COMMENT ON COLUMN  port_group.receive_bandwidth_reserve IS 'Minimum receive bandwidth allocated to this portallocated  is ort';

COMMENT ON COLUMN  port_group.default_vlan_id IS '默认VLAN id';

COMMENT ON COLUMN  port_group.promiscuous IS '混杂模式 n=0 no;n=1 unicast promiscuous;n=2 multicast promiscuous;n=3 unicast multicast promiscuous;';

COMMENT ON COLUMN  port_group.macvlan_filter_enable IS '= 0 no; = 1 yes;nmac vlan 过滤';

COMMENT ON COLUMN  port_group.allowed_transmit_macs IS 'indexed array of MAC addresses this port is allowed to transmit';

COMMENT ON COLUMN  port_group.allowed_transmit_macvlans IS 'indexed arrays of MAC addresses and VIDs to support filtering on MAC VIDMAC addresses and VIDs  to support filtering on MAC,VID npairs.';

COMMENT ON COLUMN  port_group."version" IS '对应vdp vsi type version ';

COMMENT ON COLUMN  port_group.type_id IS 'vsi type id';

COMMENT ON COLUMN  port_group.allow_mac_change IS '=0 no; = 1 yes';

COMMENT ON COLUMN  port_group.switchport_mode IS '=0 trunk;n= 1 access;';

CREATE TABLE  switch ( 
	switch_type          int4 DEFAULT 1 NOT NULL,
	id                   numeric( 19 ) NOT NULL,
	name                 varchar( 64 ) NOT NULL,
	uuid                 varchar( 64 ) NOT NULL,
	state                int4 NOT NULL,
	max_mtu              int4 DEFAULT 4000 NOT NULL,
	CONSTRAINT pk_vswitch_1 PRIMARY KEY ( id ),
	CONSTRAINT idx_switch UNIQUE ( name ),
	CONSTRAINT pk_switch UNIQUE ( id, switch_type ),
	CONSTRAINT idx_switch_0 UNIQUE ( uuid )
 );

ALTER TABLE  switch ADD CONSTRAINT ck_19 CHECK ( state = 0 OR state = 1 );

ALTER TABLE  switch ADD CONSTRAINT ck_67 CHECK ( switch_type = 0 OR switch_type = 1 OR switch_type = 2 );

COMMENT ON COLUMN  switch.switch_type IS '=0 physical switch;n=1 virtual switch sdvs ;n=2 virtual switch edvs;';

COMMENT ON COLUMN  switch.state IS '= 0 stopn= 1 run; n';

CREATE TABLE  uplink_pg ( 
	pg_id                numeric( 19 ) NOT NULL,
	pg_type              int4 DEFAULT 0 NOT NULL,
	CONSTRAINT idx_mgr_pg_0 UNIQUE ( pg_id ),
	CONSTRAINT pk_uplink_pg UNIQUE ( pg_id ),
	CONSTRAINT pk_uplink_pg_0 PRIMARY KEY ( pg_id )
 );

ALTER TABLE  uplink_pg ADD CONSTRAINT ck_62 CHECK ( pg_type = 0 );

CREATE TABLE  virtual_switch ( 
	switch_id            numeric( 19 ) NOT NULL,
	switch_type          int4 DEFAULT 1 NOT NULL,
	evb_mode             int4 DEFAULT 0 NOT NULL,
	uplink_pg_id         numeric( 19 ) NOT NULL,
	nic_max_num_cfg      int4 NOT NULL,
	is_active            int4 DEFAULT 1 NOT NULL,
	deploy_vsi_num       int4 DEFAULT 0 NOT NULL,
	CONSTRAINT pk_normal_switch PRIMARY KEY ( switch_id ),
	CONSTRAINT pk_virtual_switch UNIQUE ( switch_id, switch_type )
 );

ALTER TABLE  virtual_switch ADD CONSTRAINT ck_20 CHECK ( is_active = 0 OR is_active = 1 );

ALTER TABLE  virtual_switch ADD CONSTRAINT ck_21 CHECK ( evb_mode = 0 OR evb_mode = 1 OR evb_mode = 2 );

ALTER TABLE  virtual_switch ADD CONSTRAINT ck_66 CHECK ( switch_type = 1 OR switch_type = 2 );

CREATE INDEX idx_virtual_switch ON  virtual_switch ( uplink_pg_id );

COMMENT ON COLUMN  virtual_switch.evb_mode IS '模式 =0 normal; =1 veb; = 2 vepa';

COMMENT ON COLUMN  virtual_switch.is_active IS '=1 active; = 0 no；  monitor模块更新，影响调度';

CREATE TABLE  vlan_pool ( 
	id                   numeric( 19 ) NOT NULL,
	netplane_id          numeric( 19 ) NOT NULL,
	vlan_num             int4 NOT NULL,
	CONSTRAINT pk_vlan_pool_0 PRIMARY KEY ( id ),
	CONSTRAINT idx_vlan_pool_2 UNIQUE ( netplane_id, vlan_num )
 );

ALTER TABLE  vlan_pool ADD CONSTRAINT ck_41 CHECK ( vlan_num >=2 AND vlan_num <= 4094 );

CREATE INDEX idx_vlan_pool ON  vlan_pool ( netplane_id );

CREATE TABLE  vlan_range ( 
	id                   numeric( 19 ) NOT NULL,
	netplane_id          numeric( 19 ) NOT NULL,
	vlan_begin           int4 NOT NULL,
	vlan_end             int4 NOT NULL,
	CONSTRAINT pk_vlan_range PRIMARY KEY ( id )
 );

ALTER TABLE  vlan_range ADD CONSTRAINT ck_59 CHECK ( vlan_end >= vlan_begin );

ALTER TABLE  vlan_range ADD CONSTRAINT ck_60 CHECK ( vlan_begin >= 2 AND vlan_begin <= 4094 );

ALTER TABLE  vlan_range ADD CONSTRAINT ck_61 CHECK ( vlan_end >= 2 AND vlan_end <= 4094 );

CREATE INDEX idx_vlan_range ON  vlan_range ( netplane_id );

CREATE TABLE  vm ( 
	vm_id                numeric( 19 ) NOT NULL,
	project_id           numeric( 19 ) NOT NULL,
	is_enable_watchdog   int4 DEFAULT 0 NOT NULL,
	watchdog_timeout     int4 DEFAULT 0 NOT NULL,
	cluster_name         varchar( 64 ),
	host_name            varchar( 64 ),
	CONSTRAINT pk_vm PRIMARY KEY ( vm_id )
 );

CREATE TABLE  vm_migrate ( 
	vm_id                numeric( 19 ) NOT NULL,
	src_clustername      varchar( 64 ) NOT NULL,
	src_hostname         varchar( 64 ) NOT NULL,
	dst_clustername      varchar( 64 ) NOT NULL,
	dst_hostname         varchar( 64 ) NOT NULL,
	state                int4 NOT NULL,
	CONSTRAINT pk_vm_migrate PRIMARY KEY ( vm_id )
 );

COMMENT ON COLUMN  vm_migrate.state IS '= 0 int;n= 1 BEGINn= 2 END n';

CREATE TABLE  vm_pg ( 
	pg_id                numeric( 19 ) NOT NULL,
	pg_type              int4 DEFAULT 20 NOT NULL,
	CONSTRAINT idx_mgr_pg_2 UNIQUE ( pg_id ),
	CONSTRAINT pk_vnic_pg UNIQUE ( pg_id ),
	CONSTRAINT pk_vm_pg PRIMARY KEY ( pg_id ),
	CONSTRAINT idx_vm_pg UNIQUE ( pg_id, pg_type )
 );

ALTER TABLE  vm_pg ADD CONSTRAINT ck_63 CHECK ( pg_type = 20 OR pg_type = 21 );

COMMENT ON COLUMN  vm_pg.pg_type IS '=20 sharen=21 private';

CREATE TABLE  vna ( 
	id                   numeric( 19 ) NOT NULL,
	vna_uuid             varchar( 64 ) NOT NULL,
	vna_application      varchar( 64 ) NOT NULL,
	vnm_application      varchar( 64 ) NOT NULL,
	host_name            varchar( 64 ) NOT NULL,
	os_name              varchar( 128 ) NOT NULL,
	is_online            int4 DEFAULT 0 NOT NULL,
	CONSTRAINT pk_vna PRIMARY KEY ( id ),
	CONSTRAINT pk_host_0 UNIQUE ( id ),
	CONSTRAINT idx_vna UNIQUE ( vna_uuid ),
	CONSTRAINT idx_vna_0 UNIQUE ( vna_application )
 );

ALTER TABLE  vna ADD CONSTRAINT ck_2 CHECK ( is_online = 0 OR is_online = 1 OR is_online = 2 );

CREATE TABLE  wildcast_cfg_create_virtual_port ( 
	id                   numeric( 19 ) NOT NULL,
	uuid                 varchar( 64 ) NOT NULL,
	port_type            int4 DEFAULT 3 NOT NULL,
	port_name            varchar( 32 ) NOT NULL,
	CONSTRAINT pk_wildcast_cfg_port_0 PRIMARY KEY ( id ),
	CONSTRAINT pk_wildcast_cfg_create_virtual_port UNIQUE ( id, port_type ),
	CONSTRAINT idx_wildcast_cfg_create_virtual_port UNIQUE ( uuid ),
	CONSTRAINT idx_wildcast_cfg_create_virtual_port_0 UNIQUE ( port_name )
 );

ALTER TABLE  wildcast_cfg_create_virtual_port ADD CONSTRAINT ck_82 CHECK ( port_type = 3 OR port_type = 4 );

COMMENT ON COLUMN  wildcast_cfg_create_virtual_port.port_type IS '=3 kernel port;n=4 uplink loop port;';

CREATE TABLE  wildcast_cfg_loopback_port ( 
	id                   numeric( 19 ) NOT NULL,
	uuid                 varchar( 64 ) NOT NULL,
	port_name            varchar( 32 ) NOT NULL,
	is_loop              int4 DEFAULT 0 NOT NULL,
	CONSTRAINT pk_wildcast_cfg_port_1 PRIMARY KEY ( id )
 );

ALTER TABLE  wildcast_cfg_loopback_port ADD CONSTRAINT ck_40 CHECK ( is_loop = 0 OR is_loop = 1 );

CREATE TABLE  wildcast_cfg_switch ( 
	id                   numeric( 19 ) NOT NULL,
	uuid                 varchar( 64 ) NOT NULL,
	switch_name          varchar( 64 ) NOT NULL,
	switch_type          int4 DEFAULT 1 NOT NULL,
	state                int4 DEFAULT 0 NOT NULL,
	evb_mode             int4 DEFAULT 0 NOT NULL,
	upg_id               numeric( 19 ) NOT NULL,
	mtu_max              int4 DEFAULT 1500 NOT NULL,
	nic_max_num          int4 DEFAULT 100 NOT NULL,
	port_name            varchar( 64 ) NOT NULL,
	port_type            int4 DEFAULT 0 NOT NULL,
	CONSTRAINT pk_port_switch_mgr_0 PRIMARY KEY ( id ),
	CONSTRAINT idx_switch_wildcast_cfg UNIQUE ( switch_name ),
	CONSTRAINT pk_wildcast_cfg_switch UNIQUE ( id, port_type ),
	CONSTRAINT idx_wildcast_cfg_switch UNIQUE ( uuid )
 );

ALTER TABLE  wildcast_cfg_switch ADD CONSTRAINT ck_37 CHECK ( switch_type = 1 OR switch_type = 2 );

ALTER TABLE  wildcast_cfg_switch ADD CONSTRAINT ck_69 CHECK ( port_type = 0 OR port_type = 1 OR port_type = 4 );

CREATE INDEX idx_switch_uplinkport_cfg_0 ON  wildcast_cfg_switch ( upg_id );

COMMENT ON COLUMN  wildcast_cfg_switch.switch_type IS '=1 sdvs; n=2 edvs;';

COMMENT ON COLUMN  wildcast_cfg_switch.port_type IS '=0 physical port;n=1 bond port;n=4 uplink loop port;';

CREATE TABLE  wildcast_cfg_switch_bond ( 
	wildcast_cfg_switch_id numeric( 19 ) NOT NULL,
	port_type            int4 DEFAULT 1 NOT NULL,
	bond_mode            int4 DEFAULT 1 NOT NULL,
	CONSTRAINT pk_su_cfg_port_bond PRIMARY KEY ( wildcast_cfg_switch_id )
 );

ALTER TABLE  wildcast_cfg_switch_bond ADD CONSTRAINT ck_36 CHECK ( bond_mode = 1 OR bond_mode = 4 );

ALTER TABLE  wildcast_cfg_switch_bond ADD CONSTRAINT ck_80 CHECK ( port_type = 1 );

CREATE INDEX idx_wildcast_cfg_bond ON  wildcast_cfg_switch_bond ( wildcast_cfg_switch_id, port_type );

CREATE TABLE  wildcast_cfg_switch_bond_map ( 
	id                   numeric( 19 ) NOT NULL,
	wildcast_cfg_bond_id numeric( 19 ) NOT NULL,
	phy_port_name        varchar( 64 ) NOT NULL,
	CONSTRAINT pk_su_cfg_port_bond_map PRIMARY KEY ( id ),
	CONSTRAINT idx_switch_wildcast_cfg_port_bond_map UNIQUE ( wildcast_cfg_bond_id, phy_port_name )
 );

CREATE INDEX idx_su_cfg_port_bond_map ON  wildcast_cfg_switch_bond_map ( wildcast_cfg_bond_id );

CREATE INDEX idx_su_cfg_port_bond_map_0 ON  wildcast_cfg_switch_bond_map ( phy_port_name );

CREATE TABLE  wildcast_task_create_vport ( 
	id                   numeric( 19 ) NOT NULL,
	uuid                 varchar( 64 ) NOT NULL,
	vna_id               numeric( 19 ) NOT NULL,
	wildcast_cfg_create_vport_id numeric( 19 ) NOT NULL,
	state                int4 NOT NULL,
	send_num             int4,
	CONSTRAINT pk_wildcast_task_switch_0 PRIMARY KEY ( id ),
	CONSTRAINT idx_wildcast_task_create_vport_0 UNIQUE ( vna_id, wildcast_cfg_create_vport_id )
 );

ALTER TABLE  wildcast_task_create_vport ADD CONSTRAINT ck_81 CHECK ( state = 0 OR state = 1 OR state = 2 OR state = 3 OR state = 4);

CREATE INDEX idx_wildcast_task_switch_0 ON  wildcast_task_create_vport ( wildcast_cfg_create_vport_id );

CREATE INDEX idx_wildcast_task_create_vport ON  wildcast_task_create_vport ( vna_id );

COMMENT ON COLUMN  wildcast_task_create_vport.state IS '= 0 init;n= 1 消息已经发给VNM 等待应答;n= 2 消息应答失败;n= 3 消息应答成功;n';

CREATE TABLE  wildcast_task_loopback ( 
	id                   numeric( 19 ) NOT NULL,
	uuid                 varchar( 64 ) NOT NULL,
	vna_id               numeric( 19 ) NOT NULL,
	wildcast_cfg_loopback_port_id numeric( 19 ) NOT NULL,
	state                int4 NOT NULL,
	send_num             int4,
	CONSTRAINT pk_wildcast_task_switch_1 PRIMARY KEY ( id ),
	CONSTRAINT idx_wildcast_task_loopback_0 UNIQUE ( vna_id, wildcast_cfg_loopback_port_id )
 );

CREATE INDEX idx_wildcast_task_switch_1 ON  wildcast_task_loopback ( wildcast_cfg_loopback_port_id );

CREATE INDEX idx_wildcast_task_loopback ON  wildcast_task_loopback ( vna_id );

COMMENT ON COLUMN  wildcast_task_loopback.state IS '= 0 init;n= 1 消息已经发给VNM 等待应答;n= 2 消息应答失败;n= 3 消息应答成功;n';

CREATE TABLE  wildcast_task_switch ( 
	id                   numeric( 19 ) NOT NULL,
	uuid                 varchar( 64 ) NOT NULL,
	vna_id               numeric( 19 ) NOT NULL,
	wildcast_cfg_switch_id numeric( 19 ) NOT NULL,
	state                int4 NOT NULL,
	send_num             int4,
	CONSTRAINT pk_wildcast_task_switch PRIMARY KEY ( id ),
	CONSTRAINT idx_wildcast_task_switch_3 UNIQUE ( vna_id, wildcast_cfg_switch_id )
 );

ALTER TABLE  wildcast_task_switch ADD CONSTRAINT ck_79 CHECK ( state = 0 OR state = 1 OR state = 2 OR state = 3 OR state = 4 );

CREATE INDEX idx_wildcast_task_switch ON  wildcast_task_switch ( wildcast_cfg_switch_id );

CREATE INDEX idx_wildcast_task_switch_2 ON  wildcast_task_switch ( vna_id );

COMMENT ON COLUMN  wildcast_task_switch.state IS '= 0 init;n= 1 消息已经发给VNM 等待应答;n= 2 消息应答失败;n= 3 消息应答成功;n';

CREATE TABLE  ip_range ( 
	id                   numeric( 19 ) NOT NULL,
	netplane_id          numeric( 19 ) NOT NULL,
	ip_begin             varchar( 64 ) NOT NULL,
	ip_end               varchar( 64 ) NOT NULL,
	mask                 varchar( 64 ) NOT NULL,
	ip_begin_num         numeric( 19 ) NOT NULL,
	ip_end_num           numeric( 19 ) NOT NULL,
	mask_num             numeric( 19 ) NOT NULL,
	CONSTRAINT pk_vlan_range_1 PRIMARY KEY ( id )
 );

CREATE INDEX idx_mac_range_1 ON  ip_range ( netplane_id );

CREATE TABLE  kernel_cfg_report ( 
	id                   numeric( 19 ) NOT NULL,
	uuid                 varchar( 64 ) NOT NULL,
	vna_id               numeric( 19 ) NOT NULL,
	switch_name          varchar( 64 ) NOT NULL,
	ip                   varchar( 64 ) NOT NULL,
	mask                 varchar( 64 ) NOT NULL,
	is_online            int4 DEFAULT 0 NOT NULL,
	kernel_port_name     varchar( 64 ) NOT NULL,
	uplink_port_name     varchar( 64 ) NOT NULL,
	uplink_port_type     int4 DEFAULT 0 NOT NULL,
	is_dhcp              int4 DEFAULT 0 NOT NULL,
	CONSTRAINT pk_kernel_nic_report PRIMARY KEY ( id ),
	CONSTRAINT idx_kernel_cfg_report UNIQUE ( vna_id ),
	CONSTRAINT idx_kernel_cfg_report_0 UNIQUE ( vna_id, switch_name ),
	CONSTRAINT pk_kernel_cfg_report UNIQUE ( id, vna_id ),
	CONSTRAINT pk_kernel_cfg_report_0 UNIQUE ( vna_id, id ),
	CONSTRAINT idx_kernel_cfg_report_1 UNIQUE ( uuid ),
	CONSTRAINT pk_kernel_cfg_report_1 UNIQUE ( id, vna_id, uplink_port_type )
 );

ALTER TABLE  kernel_cfg_report ADD CONSTRAINT ck_25 CHECK ( is_online = 0 OR is_online = 1 );

ALTER TABLE  kernel_cfg_report ADD CONSTRAINT ck_45 CHECK ( uplink_port_type = 0 OR uplink_port_type = 1 );

COMMENT ON COLUMN  kernel_cfg_report.switch_name IS 'kernel';

COMMENT ON COLUMN  kernel_cfg_report.uplink_port_type IS '=0 physical port;n=1 bond port;';

COMMENT ON COLUMN  kernel_cfg_report.is_dhcp IS '=1 通过dhcp方式来获取地址';

CREATE TABLE  kernel_cfg_report_bond ( 
	kernel_cfg_report_id numeric( 19 ) NOT NULL,
	vna_id               numeric( 19 ) NOT NULL,
	uplink_port_type     int4 DEFAULT 1 NOT NULL,
	bond_mode            int4 DEFAULT 1 NOT NULL,
	CONSTRAINT pk_su_cfg_port_bond_0 PRIMARY KEY ( kernel_cfg_report_id ),
	CONSTRAINT pk_kernel_cfg_report_bond UNIQUE ( kernel_cfg_report_id, vna_id )
 );

ALTER TABLE  kernel_cfg_report_bond ADD CONSTRAINT ck_26 CHECK ( bond_mode = 1 OR bond_mode = 4 );

ALTER TABLE  kernel_cfg_report_bond ADD CONSTRAINT ck_75 CHECK ( uplink_port_type = 1 );

CREATE INDEX idx_kernel_cfg_report_bond ON  kernel_cfg_report_bond ( kernel_cfg_report_id, vna_id, uplink_port_type );

CREATE TABLE  kernel_cfg_report_bond_map ( 
	id                   numeric( 19 ) NOT NULL,
	kernel_cfg_report_bond_id numeric( 19 ) NOT NULL,
	phy_port_name        varchar( 64 ) NOT NULL,
	CONSTRAINT pk_su_cfg_port_bond_map_0 PRIMARY KEY ( id ),
	CONSTRAINT idx_kernel_cfg_report_port_bond_map UNIQUE ( kernel_cfg_report_bond_id, phy_port_name )
 );

CREATE INDEX idx_su_cfg_port_bond_map_1 ON  kernel_cfg_report_bond_map ( kernel_cfg_report_bond_id );

CREATE INDEX idx_su_cfg_port_bond_map_2 ON  kernel_cfg_report_bond_map ( phy_port_name );

CREATE TABLE  kernel_pg ( 
	pg_id                numeric( 19 ) NOT NULL,
	pg_type              int4 DEFAULT 10 NOT NULL,
	CONSTRAINT idx_mgr_pg_3 UNIQUE ( pg_id ),
	CONSTRAINT pk_mgr_pg_0 UNIQUE ( pg_id ),
	CONSTRAINT pk_kernel_data_pg PRIMARY KEY ( pg_id ),
	CONSTRAINT idx_kernel_data_pg UNIQUE ( pg_id, pg_type )
 );

ALTER TABLE  kernel_pg ADD CONSTRAINT ck_30 CHECK ( pg_type = 10 OR pg_type = 11 );

COMMENT ON COLUMN  kernel_pg.pg_type IS '=10 kernel mgrn=11 kernel storage';

CREATE TABLE  logic_network ( 
	id                   numeric( 19 ) NOT NULL,
	name                 varchar( 64 ) NOT NULL,
	uuid                 varchar( 64 ) NOT NULL,
	vm_pg_id             numeric( 19 ) NOT NULL,
	description          varchar( 512 ),
	ip_bootprotocolmode  int4 DEFAULT 0 NOT NULL,
	CONSTRAINT pk_logic_network PRIMARY KEY ( id ),
	CONSTRAINT idx_logic_network_0 UNIQUE ( name ),
	CONSTRAINT idx_logic_network_1 UNIQUE ( uuid )
 );

ALTER TABLE  logic_network ADD CONSTRAINT ck_92 CHECK ( ip_bootprotocolmode = 0 OR ip_bootprotocolmode = 1 OR ip_bootprotocolmode = 2 );

CREATE INDEX idx_logic_network ON  logic_network ( vm_pg_id );

COMMENT ON COLUMN  logic_network.ip_bootprotocolmode IS '=0 static;n=1 dynamic';

CREATE TABLE  logic_network_ip_pool ( 
	id                   numeric( 19 ) NOT NULL,
	logic_network_id     numeric( 19 ) NOT NULL,
	ip                   varchar( 64 ) NOT NULL,
	mask                 varchar( 64 ) NOT NULL,
	ip_num               numeric( 19 ) NOT NULL,
	mask_num             numeric( 19 ) NOT NULL,
	CONSTRAINT pk_vlan_pool_2 PRIMARY KEY ( id ),
	CONSTRAINT idx_logic_network_ip_pool UNIQUE ( ip, logic_network_id )
 );

CREATE INDEX idx_vlan_pool_1 ON  logic_network_ip_pool ( logic_network_id );

CREATE TABLE  logic_network_ip_range ( 
	id                   numeric( 19 ) NOT NULL,
	logic_network_id     numeric( 19 ) NOT NULL,
	ip_begin             varchar( 64 ) NOT NULL,
	ip_end               varchar( 64 ) NOT NULL,
	mask                 varchar( 64 ) NOT NULL,
	ip_begin_num         numeric( 19 ) NOT NULL,
	ip_end_num           numeric( 19 ) NOT NULL,
	mask_num             numeric( 19 ) NOT NULL,
	CONSTRAINT pk_logic_network_ip_range PRIMARY KEY ( id )
 );

CREATE INDEX idx_logic_network_ip_range ON  logic_network_ip_range ( logic_network_id );

CREATE TABLE  mac_pool ( 
	id                   numeric( 19 ) NOT NULL,
	netplane_id          numeric( 19 ) NOT NULL,
	mac                  varchar( 64 ) NOT NULL,
	mac_num              numeric( 19 ) NOT NULL,
	CONSTRAINT pk_vlan_pool_1 PRIMARY KEY ( id ),
	CONSTRAINT idx_mac_pool UNIQUE ( mac, netplane_id )
 );

CREATE INDEX idx_vlan_pool_0 ON  mac_pool ( netplane_id );

CREATE TABLE  mac_range ( 
	id                   numeric( 19 ) NOT NULL,
	netplane_id          numeric( 19 ) NOT NULL,
	mac_begin            varchar( 64 ) NOT NULL,
	mac_end              varchar( 64 ) NOT NULL,
	mac_begin_num        numeric( 19 ) NOT NULL,
	mac_end_num          numeric( 19 ) NOT NULL,
	CONSTRAINT pk_vlan_range_0 PRIMARY KEY ( id )
 );

CREATE INDEX idx_mac_range_0 ON  mac_range ( netplane_id );

CREATE TABLE  pg_switchport_access_isolate ( 
	pg_id                numeric( 19 ) NOT NULL,
	pg_type              int4 DEFAULT 21 NOT NULL,
	switchport_mode      int4 NOT NULL,
	isolate_no           int4 DEFAULT 0 NOT NULL,
	CONSTRAINT pk_pg_trunk_vlan_2 PRIMARY KEY ( pg_id )
 );

ALTER TABLE  pg_switchport_access_isolate ADD CONSTRAINT ck_73 CHECK ( pg_type = 21 );

ALTER TABLE  pg_switchport_access_isolate ADD CONSTRAINT ck_74 CHECK ( isolate_no >= 1 AND isolate_no <= 100 );

ALTER TABLE  pg_switchport_access_isolate ADD CONSTRAINT ck_39 CHECK ( switchport_mode = 1 );

CREATE INDEX idx_pg_switchport_trunk_isolate_0 ON  pg_switchport_access_isolate ( pg_id, pg_type );

CREATE INDEX idx_pg_switchport_access_isolate ON  pg_switchport_access_isolate ( pg_id, pg_type, switchport_mode );

CREATE TABLE  pg_switchport_access_vlan ( 
	pg_id                numeric( 19 ) NOT NULL,
	pg_type              int4 NOT NULL,
	switchport_mode      int4 NOT NULL,
	vlan_num             int4 NOT NULL,
	CONSTRAINT pk_pg_trunk_vlan_1 PRIMARY KEY ( pg_id )
 );

ALTER TABLE  pg_switchport_access_vlan ADD CONSTRAINT ck_71 CHECK ( pg_type = 0 OR pg_type = 10 OR pg_type = 11 OR pg_type = 20   );

ALTER TABLE  pg_switchport_access_vlan ADD CONSTRAINT ck_72 CHECK ( vlan_num >=0 AND vlan_num <= 4094 );

ALTER TABLE  pg_switchport_access_vlan ADD CONSTRAINT ck_1 CHECK ( switchport_mode = 1 );

CREATE INDEX idx_pg_switchport_access_vlan ON  pg_switchport_access_vlan ( pg_id, pg_type );

CREATE INDEX idx_pg_switchport_access_vlan_0 ON  pg_switchport_access_vlan ( vlan_num );

CREATE INDEX idx_pg_switchport_access_vlan_1 ON  pg_switchport_access_vlan ( pg_id, pg_type, switchport_mode );

CREATE TABLE  pg_switchport_trunk ( 
	pg_id                numeric( 19 ) NOT NULL,
	switchport_mode      int4 NOT NULL,
	native_vlan_num      int4 NOT NULL,
	CONSTRAINT pk_pg_trunk PRIMARY KEY ( pg_id ),
	CONSTRAINT pk_pg_switchport_trunk UNIQUE ( pg_id )
 );

ALTER TABLE  pg_switchport_trunk ADD CONSTRAINT ck_83 CHECK ( switchport_mode = 0 );

ALTER TABLE  pg_switchport_trunk ADD CONSTRAINT ck_85 CHECK ( native_vlan_num >= 0 AND native_vlan_num <= 4094 );

CREATE TABLE  pg_switchport_trunk_vlan_range ( 
	id                   numeric( 19 ) NOT NULL,
	pg_switchport_trunk_id numeric( 19 ) NOT NULL,
	vlan_begin           int4 NOT NULL,
	vlan_end             int4 NOT NULL,
	CONSTRAINT pk_pg_filter_vlan_range PRIMARY KEY ( id )
 );

ALTER TABLE  pg_switchport_trunk_vlan_range ADD CONSTRAINT ck_0 CHECK ( vlan_end >= vlan_begin );

ALTER TABLE  pg_switchport_trunk_vlan_range ADD CONSTRAINT ck_58 CHECK ( vlan_begin >=1 AND vlan_begin <= 4094 );

ALTER TABLE  pg_switchport_trunk_vlan_range ADD CONSTRAINT ck_22 CHECK ( vlan_end >=1 AND vlan_end <= 4094 );

CREATE INDEX idx_pg_filter_vlan_range ON  pg_switchport_trunk_vlan_range ( pg_switchport_trunk_id );

CREATE TABLE  physical_switch ( 
	switch_id            numeric( 19 ) NOT NULL,
	switch_type          int4 DEFAULT 0 NOT NULL,
	CONSTRAINT pk_loop_switch PRIMARY KEY ( switch_id )
 );

ALTER TABLE  physical_switch ADD CONSTRAINT ck_68 CHECK ( switch_type = 0 );

CREATE TABLE  port ( 
	id                   numeric( 19 ) NOT NULL,
	port_type            int4 NOT NULL,
	vna_id               numeric( 19 ) NOT NULL,
	name                 varchar( 32 ) NOT NULL,
	uuid                 varchar( 64 ) NOT NULL,
	state                int4 DEFAULT 0 NOT NULL,
	is_broadcast         int4 DEFAULT 0 NOT NULL,
	is_running           int4 DEFAULT 0 NOT NULL,
	is_multicast         int4 DEFAULT 0 NOT NULL,
	promiscuous          int4 NOT NULL,
	mtu                  int4 DEFAULT 1500 NOT NULL,
	is_master            int4 DEFAULT 1 NOT NULL,
	admin_state          int4 DEFAULT 1 NOT NULL,
	is_online            int4 DEFAULT 0 NOT NULL,
	is_cfg               int4 DEFAULT 0 NOT NULL,
	is_consistency       int4 DEFAULT 0 NOT NULL,
	total_sriovvf_num    int4 DEFAULT 0 NOT NULL,
	free_sriovvf_num     int4 DEFAULT 0 NOT NULL,
	CONSTRAINT pk_port_0 UNIQUE ( id ),
	CONSTRAINT pk_port_1 UNIQUE ( id ),
	CONSTRAINT pk_port_2 PRIMARY KEY ( id ),
	CONSTRAINT idx_port_0 UNIQUE ( vna_id, name ),
	CONSTRAINT idx_port_1 UNIQUE ( uuid ),
	CONSTRAINT idx_port_2 UNIQUE ( id, port_type, vna_id ),
	CONSTRAINT pk_port_3 UNIQUE ( id, vna_id, port_type )
 );

ALTER TABLE  port ADD CONSTRAINT ck_4 CHECK ( is_online = 0 OR is_online = 1 );

ALTER TABLE  port ADD CONSTRAINT ck_5 CHECK ( state = 0 OR state =1 );

ALTER TABLE  port ADD CONSTRAINT ck_6 CHECK ( is_broadcast = 0 OR is_broadcast = 1 );

ALTER TABLE  port ADD CONSTRAINT ck_7 CHECK ( is_running = 0 OR is_running = 1 );

ALTER TABLE  port ADD CONSTRAINT ck_8 CHECK ( is_multicast = 0 OR is_multicast = 1 );

ALTER TABLE  port ADD CONSTRAINT ck_9 CHECK ( promiscuous = 0 OR promiscuous = 1 OR promiscuous = 2 OR promiscuous = 3 );

ALTER TABLE  port ADD CONSTRAINT ck_10 CHECK ( admin_state = 0 OR admin_state = 1 );

ALTER TABLE  port ADD CONSTRAINT ck_38 CHECK ( is_master = 0 OR is_master = 1 );

ALTER TABLE  port ADD CONSTRAINT ck_31 CHECK ( port_type = 0 OR port_type = 1 OR port_type = 2 OR port_type = 3 OR port_type = 4 OR port_type = 5  );

ALTER TABLE  port ADD CONSTRAINT ck_33 CHECK ( is_cfg = 0 OR is_cfg = 1 OR is_cfg = 2 );

ALTER TABLE  port ADD CONSTRAINT ck_90 CHECK ( is_consistency = 0 OR is_consistency = 1 );

CREATE INDEX idx_port ON  port ( vna_id );

COMMENT ON COLUMN  port.port_type IS '=0 physical port;n=1 bond port;n=2 sriov vf port;n=3 kernel port(mgr, storage);n=4 uplink loop port;n=5 vswitch port;';

COMMENT ON COLUMN  port.state IS '=1 upn=0 down';

COMMENT ON COLUMN  port.is_broadcast IS '=1 setn=0 unset';

COMMENT ON COLUMN  port.is_running IS '=0 unset n=1 set';

COMMENT ON COLUMN  port.is_multicast IS '=0 unset;n=1 set';

COMMENT ON COLUMN  port.promiscuous IS '=0 none;n=1 unicast promiscuous; n=2 multicast promiscuous; n=3 unicast and multicast promiscuous';

COMMENT ON COLUMN  port.is_master IS '=1 master;n=0 slave';

COMMENT ON COLUMN  port.admin_state IS '=0 n=1 ';

COMMENT ON COLUMN  port.is_online IS '=0 off line;n=1 on line';

CREATE TABLE  port_ip ( 
	id                   numeric( 19 ) NOT NULL,
	port_id              numeric( 64 ) NOT NULL,
	port_name            varchar( 32 ) NOT NULL,
	ip                   varchar( 64 ) NOT NULL,
	mask                 varchar( 64 ) NOT NULL,
	is_cfg               int4 DEFAULT 0 NOT NULL,
	is_online            int4 DEFAULT 0 NOT NULL,
	CONSTRAINT pk_port_ip PRIMARY KEY ( id ),
	CONSTRAINT idx_port_ip_1 UNIQUE ( port_id, port_name )
 );

ALTER TABLE  port_ip ADD CONSTRAINT ck_11 CHECK ( is_cfg = 0 OR is_cfg = 1 OR is_cfg = 2 );

CREATE INDEX idx_bond_address ON  port_ip ( port_id );

CREATE INDEX idx_port_ip ON  port_ip ( id );

COMMENT ON COLUMN  port_ip.port_name IS '存在别名情况';

CREATE TABLE  port_mac ( 
	id                   numeric( 19 ) NOT NULL,
	port_id              numeric( 19 ) NOT NULL,
	mac                  varchar( 64 ) NOT NULL,
	CONSTRAINT pk_port_mac PRIMARY KEY ( id )
 );

CREATE INDEX idx_port_mac ON  port_mac ( port_id );

CREATE TABLE  regist_module ( 
	id                   numeric( 19 ) NOT NULL,
	vna_id               numeric( 19 ) NOT NULL,
	module_name          varchar( 64 ) NOT NULL,
	module_uuid          varchar( 64 ) NOT NULL,
	role                 int4 NOT NULL,
	is_online            int4 NOT NULL,
	CONSTRAINT pk_reg_module PRIMARY KEY ( id ),
	CONSTRAINT idx_regist_module_0 UNIQUE ( module_uuid ),
	CONSTRAINT idx_regist_module UNIQUE ( vna_id, module_name, role )
 );

ALTER TABLE  regist_module ADD CONSTRAINT ck_3 CHECK ( is_online = 0 OR is_online = 1 OR is_online = 2 );

ALTER TABLE  regist_module ADD CONSTRAINT ck_88 CHECK ( role = 1 OR role = 2 OR role = 3 OR role = 4  OR role = 5 OR role = 6 OR role = 7 );

CREATE INDEX idx_reg_module ON  regist_module ( vna_id );

COMMENT ON COLUMN  regist_module.role IS '=1 hc;n=2 cc;n=3 dhcp;n=4 nat;n=5 watchdog;n=6 TC main;n=7 api sever;';

CREATE TABLE  uplink_loop_port ( 
	port_id              numeric( 19 ) NOT NULL,
	vna_id               numeric( 19 ) NOT NULL,
	port_type            int4 DEFAULT 4 NOT NULL,
	is_cfg               int4 DEFAULT 0 NOT NULL,
	CONSTRAINT idx_switch_port UNIQUE ( port_id ),
	CONSTRAINT pk_virtualswitch_port UNIQUE ( port_id ),
	CONSTRAINT pk_uplink_loop_port PRIMARY KEY ( port_id )
 );

ALTER TABLE  uplink_loop_port ADD CONSTRAINT ck_23 CHECK ( is_cfg = 0 OR is_cfg = 1 );

ALTER TABLE  uplink_loop_port ADD CONSTRAINT ck_78 CHECK ( port_type = 4 );

CREATE INDEX idx_uplink_loop_port ON  uplink_loop_port ( port_id, vna_id, port_type );

CREATE TABLE  virtual_switch_map ( 
	id                   numeric( 19 ) NOT NULL,
	port_id              numeric( 19 ) NOT NULL,
	virtual_switch_id    numeric( 19 ) NOT NULL,
	is_cfg               int4 DEFAULT 0 NOT NULL,
	is_selected          int4 DEFAULT 0 NOT NULL,
	is_consistency       int4 DEFAULT 0 NOT NULL,
	CONSTRAINT pk_map_virtualswitch_uplinkport UNIQUE ( id ),
	CONSTRAINT pk_map_virtualswitch_uplinkport_0 PRIMARY KEY ( id ),
	CONSTRAINT idx_map_virtualswitch_uplinkport UNIQUE ( port_id, virtual_switch_id )
 );

CREATE INDEX idx_map_nss_uplinkport ON  virtual_switch_map ( virtual_switch_id );

CREATE TABLE  vlan_map ( 
	id                   numeric( 19 ) NOT NULL,
	project_id           numeric( 19 ) NOT NULL,
	vm_pg_id             numeric( 19 ) NOT NULL,
	vlan_id              numeric( 19 ) DEFAULT 0 NOT NULL,
	isolate_no           int4 DEFAULT 0 NOT NULL,
	CONSTRAINT pk_vlan_pool PRIMARY KEY ( id ),
	CONSTRAINT idx_vlan_map UNIQUE ( project_id, isolate_no, vm_pg_id )
 );

CREATE INDEX idx_vlan_map_0 ON  vlan_map ( vm_pg_id );

CREATE TABLE  vsi_profile ( 
	id                   numeric( 19 ) NOT NULL,
	vsi_id_value         varchar( 64 ) NOT NULL,
	vsi_id_format        int4 DEFAULT 5 NOT NULL,
	vm_pg_id             numeric( 19 ) NOT NULL,
	mac_id               numeric( 19 ) NOT NULL,
	logic_network_ip_id  numeric( 19 ),
	virtual_switch_id    numeric( 19 ),
	CONSTRAINT pk_vsi_profile_0 PRIMARY KEY ( id ),
	CONSTRAINT idx_vsi_profile UNIQUE ( mac_id ),
	CONSTRAINT idx_vsi_profile_1 UNIQUE ( vsi_id_value )
 );

ALTER TABLE  vsi_profile ADD CONSTRAINT ck_32 CHECK ( vsi_id_format = 5 );

CREATE INDEX pk_vsi_profile ON  vsi_profile ( vm_pg_id );

CREATE INDEX idx_vsi_profile_0 ON  vsi_profile ( logic_network_ip_id );

CREATE INDEX idx_vsi_profile_2 ON  vsi_profile ( virtual_switch_id );

CREATE TABLE  vswitch_port ( 
	port_id              numeric( 19 ) NOT NULL,
	vna_id               numeric( 19 ) NOT NULL,
	port_type            int4 NOT NULL,
	CONSTRAINT pk_vswitch_port PRIMARY KEY ( port_id )
 );

ALTER TABLE  vswitch_port ADD CONSTRAINT ck_84 CHECK ( port_type = 5  );

CREATE INDEX idx_vswitch_port ON  vswitch_port ( port_id, vna_id, port_type );

CREATE TABLE  wildcast_cfg_create_kernel_port ( 
	wildcast_create_vport_id numeric( 19 ) NOT NULL,
	port_type            int4 DEFAULT 3 NOT NULL,
	kernel_type          int4 DEFAULT 0 NOT NULL,
	kernel_pg_id         numeric( 19 ) NOT NULL,
	switch_id            numeric( 19 ) NOT NULL,
	is_dhcp              int4 DEFAULT 0 NOT NULL,
	CONSTRAINT pk_wildcast_cfg_kernel_port PRIMARY KEY ( wildcast_create_vport_id )
 );

ALTER TABLE  wildcast_cfg_create_kernel_port ADD CONSTRAINT ck_46 CHECK ( port_type = 3 );

CREATE INDEX idx_wildcast_cfg_kernel_port ON  wildcast_cfg_create_kernel_port ( wildcast_create_vport_id, port_type );

CREATE INDEX idx_wildcast_cfg_create_kernel_port ON  wildcast_cfg_create_kernel_port ( kernel_pg_id );

CREATE INDEX idx_wildcast_cfg_create_kernel_port_0 ON  wildcast_cfg_create_kernel_port ( switch_id );

COMMENT ON COLUMN  wildcast_cfg_create_kernel_port.kernel_type IS '=11 kernel_storage';

COMMENT ON COLUMN  wildcast_cfg_create_kernel_port.is_dhcp IS '=1 通过dhcp方式来获取地址';

CREATE TABLE  bond ( 
	port_id              numeric( 19 ) NOT NULL,
	port_type            int4 DEFAULT 1 NOT NULL,
	vna_id               numeric( 19 ) NOT NULL,
	bond_mode            int4 DEFAULT 1 NOT NULL,
	policy               varchar( 64 ) NOT NULL,
	total_sriovport_num  int4 DEFAULT 0 NOT NULL,
	valid_sriovport_num  int4 DEFAULT 0 NOT NULL,
	CONSTRAINT pk_bond_port_0 UNIQUE ( port_id ),
	CONSTRAINT pk_bond_1 PRIMARY KEY ( port_id ),
	CONSTRAINT pk_bond_5 UNIQUE ( port_id, vna_id ),
	CONSTRAINT pk_bond_6 UNIQUE ( port_id, bond_mode )
 );

ALTER TABLE  bond ADD CONSTRAINT ck_15 CHECK ( bond_mode = 1 OR bond_mode = 4 );

ALTER TABLE  bond ADD CONSTRAINT ck_42 CHECK ( port_type = 1 );

COMMENT ON COLUMN  bond.bond_mode IS 'bond模式  =1 backup; = 4 lacp';

COMMENT ON COLUMN  bond.policy IS '策略';

CREATE TABLE  bond_backup ( 
	bond_id              numeric( 19 ) NOT NULL,
	bond_mode            int4 DEFAULT 1 NOT NULL,
	CONSTRAINT pk_bond_0 UNIQUE ( bond_id ),
	CONSTRAINT pk_bond_backup PRIMARY KEY ( bond_id )
 );

ALTER TABLE  bond_backup ADD CONSTRAINT ck_87 CHECK ( bond_mode = 1 );

CREATE TABLE  bond_lacp ( 
	bond_id              numeric( 19 ) NOT NULL,
	bond_mode            int4 DEFAULT 4 NOT NULL,
	state                int4 DEFAULT 0 NOT NULL,
	aggregator_id        varchar( 64 ),
	partner_mac          varchar( 64 ),
	CONSTRAINT pk_bond UNIQUE ( bond_id ),
	CONSTRAINT pk_bond_lacp PRIMARY KEY ( bond_id )
 );

ALTER TABLE  bond_lacp ADD CONSTRAINT ck_16 CHECK ( state = 0 OR state = 1 );

ALTER TABLE  bond_lacp ADD CONSTRAINT ck_86 CHECK ( bond_mode = 4 );

COMMENT ON COLUMN  bond_lacp.state IS '协议状态 = 0 failed; = 1 success;';

CREATE TABLE  hc_info ( 
	regist_module_id     numeric( 19 ) NOT NULL,
	cluster_name         varchar( 64 ) NOT NULL,
	role                 int4 DEFAULT 1 NOT NULL,
	CONSTRAINT pk_host_1 UNIQUE ( regist_module_id ),
	CONSTRAINT pk_hc_info PRIMARY KEY ( regist_module_id )
 );

ALTER TABLE  hc_info ADD CONSTRAINT ck_89 CHECK ( role = 1 );

CREATE TABLE  kernel_port ( 
	port_id              numeric( 19 ) NOT NULL,
	port_type            int4 DEFAULT 3 NOT NULL,
	vna_id               numeric( 19 ) NOT NULL,
	virtual_switch_id    numeric( 19 ),
	kernel_pg_id         numeric( 19 ),
	type                 int4 DEFAULT 10 NOT NULL,
	is_cfg               int4 DEFAULT 0 NOT NULL,
	is_dhcp              int4 DEFAULT 0 NOT NULL,
	CONSTRAINT pk_kernel_nic UNIQUE ( port_id ),
	CONSTRAINT pk_kernel_port PRIMARY KEY ( port_id )
 );

ALTER TABLE  kernel_port ADD CONSTRAINT ck_24 CHECK ( is_cfg = 0 OR is_cfg = 1 OR is_cfg = 2 );

ALTER TABLE  kernel_port ADD CONSTRAINT ck_43 CHECK ( type = 10 OR type = 11 );

ALTER TABLE  kernel_port ADD CONSTRAINT ck_77 CHECK ( port_type = 3 );

CREATE INDEX idx_mnic_profile_0 ON  kernel_port ( virtual_switch_id );

CREATE INDEX idx_kernel_port ON  kernel_port ( kernel_pg_id );

COMMENT ON COLUMN  kernel_port.type IS '=10 kernel_mgr;n=11 kernel_storage';

COMMENT ON COLUMN  kernel_port.is_dhcp IS '=1 通过dhcp方式来获取地址';

CREATE TABLE  physical_port ( 
	port_id              numeric( 19 ) NOT NULL,
	port_type            int4 DEFAULT 0 NOT NULL,
	vna_id               numeric( 19 ) NOT NULL,
	supported_ports      varchar( 64 ) NOT NULL,
	supported_link_modes varchar( 64 ) NOT NULL,
	is_support_auto_negotiation int4 DEFAULT 1 NOT NULL,
	advertised_link_modes varchar( 64 ) NOT NULL,
	advertised_pause_frame_use varchar( 64 ) NOT NULL,
	is_advertised_auto_negotiation int4 DEFAULT 1 NOT NULL,
	speed                varchar( 64 ) NOT NULL,
	duplex               varchar( 32 ) NOT NULL,
	port                 varchar( 64 ) NOT NULL,
	phyad                varchar( 32 ) NOT NULL,
	transceiver          varchar( 32 ) NOT NULL,
	auto_negotiate       varchar( 32 ) NOT NULL,
	supports_wake_on     varchar( 32 ) NOT NULL,
	wake_on              varchar( 32 ) NOT NULL,
	current_msg_level    varchar( 32 ) NOT NULL,
	is_linked            int4 DEFAULT 0 NOT NULL,
	CONSTRAINT pk_port PRIMARY KEY ( port_id ),
	CONSTRAINT pk_physical_port UNIQUE ( port_id, vna_id )
 );

ALTER TABLE  physical_port ADD CONSTRAINT ck_12 CHECK ( is_linked = 0 OR is_linked = 1 );

ALTER TABLE  physical_port ADD CONSTRAINT ck_56 CHECK ( is_support_auto_negotiation = 0 OR is_support_auto_negotiation = 1 );

ALTER TABLE  physical_port ADD CONSTRAINT ck_57 CHECK ( is_advertised_auto_negotiation = 0 OR is_advertised_auto_negotiation = 1 );

ALTER TABLE  physical_port ADD CONSTRAINT ck_64 CHECK ( port_type = 0 );

COMMENT ON TABLE  physical_port IS '物理主机端口表';

COMMENT ON COLUMN  physical_port.is_support_auto_negotiation IS '=0 no;n=1 yes';

COMMENT ON COLUMN  physical_port.is_advertised_auto_negotiation IS '=0 no;n=1 yesn';

COMMENT ON COLUMN  physical_port.speed IS '速率';

COMMENT ON COLUMN  physical_port.duplex IS '双工模式';

COMMENT ON COLUMN  physical_port.auto_negotiate IS '协商模式';

COMMENT ON COLUMN  physical_port.is_linked IS '=0 no;n=1 yes';

CREATE TABLE  physical_port_extend ( 
	physical_port_id     numeric( 19 ) NOT NULL,
	is_rx_checksumming   int4 DEFAULT 0 NOT NULL,
	is_tx_checksumming   int4 DEFAULT 0 NOT NULL,
	is_scatter_gather    int4 DEFAULT 0 NOT NULL,
	is_tcp_segmentation_offload int4 DEFAULT 0 NOT NULL,
	is_udp_fragmentation_offload int4 DEFAULT 0 NOT NULL,
	is_generic_segmentation_offload int4 DEFAULT 0 NOT NULL,
	is_generic_receive_offload int4 DEFAULT 0 NOT NULL,
	is_large_receive_offload int4 DEFAULT 0 NOT NULL,
	CONSTRAINT pk_physical_port_extend PRIMARY KEY ( physical_port_id )
 );

ALTER TABLE  physical_port_extend ADD CONSTRAINT ck_48 CHECK ( is_rx_checksumming = 0 OR is_rx_checksumming =1  );

ALTER TABLE  physical_port_extend ADD CONSTRAINT ck_49 CHECK ( is_tx_checksumming = 0 OR is_tx_checksumming = 1  );

ALTER TABLE  physical_port_extend ADD CONSTRAINT ck_50 CHECK ( is_scatter_gather = 0 OR is_scatter_gather = 1  );

ALTER TABLE  physical_port_extend ADD CONSTRAINT ck_51 CHECK ( is_tcp_segmentation_offload = 0 OR is_tcp_segmentation_offload = 1  );

ALTER TABLE  physical_port_extend ADD CONSTRAINT ck_52 CHECK ( is_udp_fragmentation_offload = 0 OR is_udp_fragmentation_offload = 1 );

ALTER TABLE  physical_port_extend ADD CONSTRAINT ck_53 CHECK ( is_generic_segmentation_offload = 0 OR is_generic_segmentation_offload = 1  );

ALTER TABLE  physical_port_extend ADD CONSTRAINT ck_54 CHECK ( is_generic_receive_offload = 0 OR is_generic_receive_offload = 1  );

ALTER TABLE  physical_port_extend ADD CONSTRAINT ck_55 CHECK ( is_large_receive_offload = 0 OR is_large_receive_offload = 1  );

COMMENT ON COLUMN  physical_port_extend.is_rx_checksumming IS '=0 off; n=1 on;';

COMMENT ON COLUMN  physical_port_extend.is_tx_checksumming IS '=0 off;n=1 on';

COMMENT ON COLUMN  physical_port_extend.is_scatter_gather IS '=0 off;n=1 on';

COMMENT ON COLUMN  physical_port_extend.is_tcp_segmentation_offload IS '=0 off;n=1 on';

COMMENT ON COLUMN  physical_port_extend.is_udp_fragmentation_offload IS '=0 off;n=1 on';

COMMENT ON COLUMN  physical_port_extend.is_generic_segmentation_offload IS '=0 off;n=1 on';

COMMENT ON COLUMN  physical_port_extend.is_generic_receive_offload IS '=0 off;n=1 on';

COMMENT ON COLUMN  physical_port_extend.is_large_receive_offload IS '=0 off;n=1 on';

CREATE TABLE  sriov_port ( 
	physical_port_id     numeric( 19 ) NOT NULL,
	vna_id               numeric( 19 ) NOT NULL,
	is_loop_report       int4 DEFAULT 0 NOT NULL,
	is_loop_cfg          int4 DEFAULT 0 NOT NULL,
	CONSTRAINT pk_sriov_port PRIMARY KEY ( physical_port_id ),
	CONSTRAINT pk_sriov_port_0 UNIQUE ( physical_port_id, vna_id )
 );

ALTER TABLE  sriov_port ADD CONSTRAINT ck_13 CHECK ( is_loop_cfg = 0 OR is_loop_cfg = 1 OR is_loop_cfg = 2 );

ALTER TABLE  sriov_port ADD CONSTRAINT ck_70 CHECK ( is_loop_report = 0 OR is_loop_report = 1 );

CREATE TABLE  sriov_vf_port ( 
	port_id              numeric( 19 ) NOT NULL,
	port_type            int4 DEFAULT 2 NOT NULL,
	vna_id               numeric( 19 ) NOT NULL,
	sriov_port_id        numeric( 19 ) NOT NULL,
	pci                  varchar( 64 ) NOT NULL,
	vlan_num             int4 DEFAULT 0 NOT NULL,
	CONSTRAINT idx_virtual_port UNIQUE ( port_id ),
	CONSTRAINT pk_virtual_port UNIQUE ( port_id ),
	CONSTRAINT pk_sriov_vf_port PRIMARY KEY ( port_id ),
	CONSTRAINT idx_sriov_vf_port_0 UNIQUE ( sriov_port_id, pci )
 );

ALTER TABLE  sriov_vf_port ADD CONSTRAINT ck_44 CHECK ( vlan_num >=0 AND vlan_num <= 4094 );

ALTER TABLE  sriov_vf_port ADD CONSTRAINT ck_76 CHECK ( port_type = 2 );

CREATE INDEX idx_sriov_vf_port ON  sriov_vf_port ( sriov_port_id );

CREATE TABLE  vnic ( 
	id                   numeric( 19 ) NOT NULL,
	vm_id                numeric( 19 ) NOT NULL,
	nic_index            int4 NOT NULL,
	is_sriov             int4 NOT NULL,
	is_loop              int4 NOT NULL,
	logic_network_uuid   varchar( 64 ) NOT NULL,
	port_type            varchar( 64 ) NOT NULL,
	vsi_profile_id       numeric( 19 ) NOT NULL,
	CONSTRAINT pk_vm_nic_1 PRIMARY KEY ( id ),
	CONSTRAINT idx_vnic_0 UNIQUE ( vm_id, nic_index )
 );

ALTER TABLE  vnic ADD CONSTRAINT ck_34 CHECK ( is_sriov = 0 OR is_sriov = 1 );

ALTER TABLE  vnic ADD CONSTRAINT ck_35 CHECK ( is_loop = 0 OR is_loop = 1 );

CREATE INDEX idx_vnic_1 ON  vnic ( vsi_profile_id );

CREATE INDEX idx_vnic ON  vnic ( vm_id );

CREATE TABLE  vsi_sriov ( 
	vsi_profile_id       numeric( 19 ) NOT NULL,
	sriov_vf_port_id     numeric( 19 ) NOT NULL,
	CONSTRAINT pk_vnic_sriov PRIMARY KEY ( vsi_profile_id )
 );

CREATE INDEX idx_vnic_sriov ON  vsi_sriov ( sriov_vf_port_id );

CREATE TABLE  bond_map ( 
	id                   numeric( 19 ) NOT NULL,
	vna_id               numeric( 19 ) NOT NULL,
	bond_id              numeric( 19 ) NOT NULL,
	physical_port_id     numeric( 19 ) NOT NULL,
	is_cfg               int4 DEFAULT 0 NOT NULL,
	is_selected          int4 DEFAULT 0 NOT NULL,
	lacp_is_success      int4 DEFAULT 0 NOT NULL,
	backup_is_active     int4 DEFAULT 0 NOT NULL,
	CONSTRAINT pk_bond_port PRIMARY KEY ( id ),
	CONSTRAINT idx_bond_port UNIQUE ( bond_id, physical_port_id )
 );

ALTER TABLE  bond_map ADD CONSTRAINT ck_17 CHECK ( is_cfg = 0 OR is_cfg = 1 OR is_cfg = 2 );

ALTER TABLE  bond_map ADD CONSTRAINT ck_18 CHECK ( is_selected = 0 OR is_selected = 1 );

ALTER TABLE  bond_map ADD CONSTRAINT ck_14 CHECK ( lacp_is_success = 0 OR lacp_is_success =1  );

ALTER TABLE  bond_map ADD CONSTRAINT ck_91 CHECK ( backup_is_active = 0 OR backup_is_active = 1  );

CREATE INDEX idx_bond_port_0 ON  bond_map ( bond_id );

COMMENT ON COLUMN  bond_map.lacp_is_success IS '=0 init/aggregate failed;;n=1 aggregate success;';

COMMENT ON COLUMN  bond_map.backup_is_active IS '=0 init/other nic;n=1 active nic;';

ALTER TABLE  bond ADD CONSTRAINT fk_bond_port FOREIGN KEY ( port_id, vna_id, port_type ) REFERENCES  port( id, vna_id, port_type ) ON DELETE CASCADE;

ALTER TABLE  bond_backup ADD CONSTRAINT fk_bond_backup_bond_port FOREIGN KEY ( bond_id, bond_mode ) REFERENCES  bond( port_id, bond_mode ) ON DELETE CASCADE;

ALTER TABLE  bond_lacp ADD CONSTRAINT fk_bond_lacp_bond_port FOREIGN KEY ( bond_id, bond_mode ) REFERENCES  bond( port_id, bond_mode ) ON DELETE CASCADE;

ALTER TABLE  bond_map ADD CONSTRAINT fk_bond_port_bond FOREIGN KEY ( bond_id, vna_id ) REFERENCES  bond( port_id, vna_id ) ON DELETE CASCADE;

ALTER TABLE  bond_map ADD CONSTRAINT fk_bond_port_host_port FOREIGN KEY ( physical_port_id, vna_id ) REFERENCES  physical_port( port_id, vna_id ) ON DELETE CASCADE;

ALTER TABLE  hc_info ADD CONSTRAINT fk_hc_info_reg_module FOREIGN KEY ( regist_module_id ) REFERENCES  regist_module( id ) ON DELETE CASCADE;

ALTER TABLE  ip_range ADD CONSTRAINT fk_ip_range_netplane FOREIGN KEY ( netplane_id ) REFERENCES  netplane( id ) ON DELETE RESTRICT;

ALTER TABLE  kernel_cfg_report ADD CONSTRAINT fk_kernel_cfg_report_vna FOREIGN KEY ( vna_id ) REFERENCES  vna( id ) ON DELETE CASCADE;

ALTER TABLE  kernel_cfg_report_bond ADD CONSTRAINT fk_kernel_cfg_report_bond FOREIGN KEY ( kernel_cfg_report_id, vna_id, uplink_port_type ) REFERENCES  kernel_cfg_report( id, vna_id, uplink_port_type ) ON DELETE CASCADE;

ALTER TABLE  kernel_cfg_report_bond_map ADD CONSTRAINT fk_kernel_cfg_report_port_bond_map FOREIGN KEY ( kernel_cfg_report_bond_id ) REFERENCES  kernel_cfg_report_bond( kernel_cfg_report_id ) ON DELETE CASCADE;

ALTER TABLE  kernel_pg ADD CONSTRAINT fk_kernel_data_pg_port_group FOREIGN KEY ( pg_id, pg_type ) REFERENCES  port_group( id, pg_type ) ON DELETE CASCADE;

ALTER TABLE  kernel_port ADD CONSTRAINT fk_kernel_port_virtual_switch FOREIGN KEY ( virtual_switch_id ) REFERENCES  virtual_switch( switch_id ) ON DELETE RESTRICT;

ALTER TABLE  kernel_port ADD CONSTRAINT fk_kernel_port_kernel_pg FOREIGN KEY ( kernel_pg_id ) REFERENCES  kernel_pg( pg_id ) ON DELETE RESTRICT;

ALTER TABLE  kernel_port ADD CONSTRAINT fk_kernel_nic_port FOREIGN KEY ( port_id, port_type, vna_id ) REFERENCES  port( id, port_type, vna_id ) ON DELETE CASCADE;

ALTER TABLE  logic_network ADD CONSTRAINT fk_logic_network_vnic_pg FOREIGN KEY ( vm_pg_id ) REFERENCES  vm_pg( pg_id ) ON DELETE RESTRICT;

ALTER TABLE  logic_network_ip_pool ADD CONSTRAINT fk_ln_ip_pool_logic_network FOREIGN KEY ( logic_network_id ) REFERENCES  logic_network( id ) ON DELETE RESTRICT;

ALTER TABLE  logic_network_ip_range ADD CONSTRAINT fk_logic_network_ip_range FOREIGN KEY ( logic_network_id ) REFERENCES  logic_network( id ) ON DELETE CASCADE;

ALTER TABLE  mac_pool ADD CONSTRAINT fk_mac_pool_netplane FOREIGN KEY ( netplane_id ) REFERENCES  netplane( id ) ON DELETE RESTRICT;

ALTER TABLE  mac_range ADD CONSTRAINT fk_mac_range_netplane FOREIGN KEY ( netplane_id ) REFERENCES  netplane( id ) ON DELETE RESTRICT;

ALTER TABLE  pg_switchport_access_isolate ADD CONSTRAINT fk_pg_switchport_access_isolate FOREIGN KEY ( pg_id, pg_type, switchport_mode ) REFERENCES  port_group( id, pg_type, switchport_mode ) ON DELETE CASCADE;

ALTER TABLE  pg_switchport_access_vlan ADD CONSTRAINT fk_pg_switchport_access_vlan FOREIGN KEY ( pg_id, pg_type, switchport_mode ) REFERENCES  port_group( id, pg_type, switchport_mode ) ON DELETE CASCADE;

ALTER TABLE  pg_switchport_trunk ADD CONSTRAINT fk_pg_trunk_port_group FOREIGN KEY ( pg_id, switchport_mode ) REFERENCES  port_group( id, switchport_mode ) ON DELETE CASCADE;

ALTER TABLE  pg_switchport_trunk_vlan_range ADD CONSTRAINT fk_pg_trunk_vlan_range FOREIGN KEY ( pg_switchport_trunk_id ) REFERENCES  pg_switchport_trunk( pg_id ) ON DELETE CASCADE;

ALTER TABLE  physical_port ADD CONSTRAINT fk_host_phy_port_port FOREIGN KEY ( port_id, port_type, vna_id ) REFERENCES  port( id, port_type, vna_id ) ON DELETE CASCADE;

ALTER TABLE  physical_port_extend ADD CONSTRAINT fk_physical_port_extend FOREIGN KEY ( physical_port_id ) REFERENCES  physical_port( port_id ) ON DELETE CASCADE;

ALTER TABLE  physical_switch ADD CONSTRAINT fk_loop_switch_switch FOREIGN KEY ( switch_id, switch_type ) REFERENCES  switch( id, switch_type ) ON DELETE CASCADE;

ALTER TABLE  port ADD CONSTRAINT fk_port_vna FOREIGN KEY ( vna_id ) REFERENCES  vna( id ) ON DELETE CASCADE;

ALTER TABLE  port_group ADD CONSTRAINT fk_port_group_netplane FOREIGN KEY ( netplane_id ) REFERENCES  netplane( id ) ON DELETE RESTRICT;

ALTER TABLE  port_ip ADD CONSTRAINT fk_port_ip_port FOREIGN KEY ( port_id ) REFERENCES  port( id ) ON DELETE CASCADE;

ALTER TABLE  port_mac ADD CONSTRAINT fk_port_mac_port FOREIGN KEY ( port_id ) REFERENCES  port( id ) ON DELETE CASCADE;

ALTER TABLE  regist_module ADD CONSTRAINT fk_reg_module_vna FOREIGN KEY ( vna_id ) REFERENCES  vna( id ) ON DELETE CASCADE;

ALTER TABLE  sriov_port ADD CONSTRAINT fk_sriov_port_host_port FOREIGN KEY ( physical_port_id, vna_id ) REFERENCES  physical_port( port_id, vna_id ) ON DELETE CASCADE;

ALTER TABLE  sriov_vf_port ADD CONSTRAINT fk_sriov_vf_port_sriov_port FOREIGN KEY ( sriov_port_id, vna_id ) REFERENCES  sriov_port( physical_port_id, vna_id ) ON DELETE CASCADE;

ALTER TABLE  sriov_vf_port ADD CONSTRAINT fk_sriov_vf_port_port FOREIGN KEY ( port_id, vna_id, port_type ) REFERENCES  port( id, vna_id, port_type ) ON DELETE CASCADE;

ALTER TABLE  uplink_loop_port ADD CONSTRAINT fk_uplink_loop_port_port FOREIGN KEY ( port_id, vna_id, port_type ) REFERENCES  port( id, vna_id, port_type ) ON DELETE CASCADE;

ALTER TABLE  uplink_pg ADD CONSTRAINT fk_uplink_pg_port_group FOREIGN KEY ( pg_id, pg_type ) REFERENCES  port_group( id, pg_type ) ON DELETE CASCADE;

ALTER TABLE  virtual_switch ADD CONSTRAINT fk_virtual_switch_uplink_pg FOREIGN KEY ( uplink_pg_id ) REFERENCES  uplink_pg( pg_id ) ON DELETE RESTRICT;

ALTER TABLE  virtual_switch ADD CONSTRAINT fk_normal_switch_switch FOREIGN KEY ( switch_id, switch_type ) REFERENCES  switch( id, switch_type ) ON DELETE CASCADE;

ALTER TABLE  virtual_switch_map ADD CONSTRAINT fk_map_sdvs_uplinkport_port FOREIGN KEY ( port_id ) REFERENCES  port( id ) ON DELETE CASCADE;

ALTER TABLE  virtual_switch_map ADD CONSTRAINT fk_map_virtualswitch_uplinkport FOREIGN KEY ( virtual_switch_id ) REFERENCES  virtual_switch( switch_id ) ON DELETE CASCADE;

ALTER TABLE  vlan_map ADD CONSTRAINT fk_vlan_vlan_pool FOREIGN KEY ( vlan_id ) REFERENCES  vlan_pool( id ) ON DELETE RESTRICT;

ALTER TABLE  vlan_map ADD CONSTRAINT fk_vlan_map_vm_pg FOREIGN KEY ( vm_pg_id ) REFERENCES  vm_pg( pg_id ) ON DELETE RESTRICT;

ALTER TABLE  vlan_pool ADD CONSTRAINT fk_vlan_pool_netplane FOREIGN KEY ( netplane_id ) REFERENCES  netplane( id ) ON DELETE RESTRICT;

ALTER TABLE  vlan_range ADD CONSTRAINT fk_vlan_range_netplane FOREIGN KEY ( netplane_id ) REFERENCES  netplane( id ) ON DELETE RESTRICT;

ALTER TABLE  vm_pg ADD CONSTRAINT fk_vm_pg_port_group FOREIGN KEY ( pg_id, pg_type ) REFERENCES  port_group( id, pg_type ) ON DELETE CASCADE;

ALTER TABLE  vnic ADD CONSTRAINT fk_vnic_vsi_profile FOREIGN KEY ( vsi_profile_id ) REFERENCES  vsi_profile( id ) ON DELETE CASCADE;

ALTER TABLE  vnic ADD CONSTRAINT fk_vnic_vm FOREIGN KEY ( vm_id ) REFERENCES  vm( vm_id ) ON DELETE CASCADE;

ALTER TABLE  vsi_profile ADD CONSTRAINT fk_vnic_profile_vnic_pg FOREIGN KEY ( vm_pg_id ) REFERENCES  vm_pg( pg_id ) ON DELETE RESTRICT;

ALTER TABLE  vsi_profile ADD CONSTRAINT fk_vsi_profile_mac_pool FOREIGN KEY ( mac_id ) REFERENCES  mac_pool( id ) ON DELETE RESTRICT;

ALTER TABLE  vsi_profile ADD CONSTRAINT fk_vsi_profile FOREIGN KEY ( logic_network_ip_id ) REFERENCES  logic_network_ip_pool( id ) ON DELETE RESTRICT;

ALTER TABLE  vsi_profile ADD CONSTRAINT fk_vsi_profile_virtual_switch FOREIGN KEY ( virtual_switch_id ) REFERENCES  virtual_switch( switch_id ) ON DELETE RESTRICT;

ALTER TABLE  vsi_sriov ADD CONSTRAINT fk_vnic_sriov_sriov_vf_port FOREIGN KEY ( vsi_profile_id ) REFERENCES  vsi_profile( id ) ON DELETE CASCADE;

ALTER TABLE  vsi_sriov ADD CONSTRAINT fk_vnic_sriov_sriov_vf_port_1 FOREIGN KEY ( sriov_vf_port_id ) REFERENCES  sriov_vf_port( port_id ) ON DELETE RESTRICT;

ALTER TABLE  vswitch_port ADD CONSTRAINT fk_vswitch_port_port FOREIGN KEY ( port_id, vna_id, port_type ) REFERENCES  port( id, vna_id, port_type ) ON DELETE CASCADE;

ALTER TABLE  wildcast_cfg_create_kernel_port ADD CONSTRAINT fk_wildcast_cfg_kernel_port FOREIGN KEY ( wildcast_create_vport_id, port_type ) REFERENCES  wildcast_cfg_create_virtual_port( id, port_type ) ON DELETE CASCADE;

ALTER TABLE  wildcast_cfg_create_kernel_port ADD CONSTRAINT fk_wildcast_cfg_create_kernel_port FOREIGN KEY ( kernel_pg_id ) REFERENCES  kernel_pg( pg_id ) ON DELETE RESTRICT;

ALTER TABLE  wildcast_cfg_create_kernel_port ADD CONSTRAINT fk_wildcast_cfg_create_kernel_port_001 FOREIGN KEY ( switch_id ) REFERENCES  virtual_switch( switch_id ) ON DELETE RESTRICT;

ALTER TABLE  wildcast_cfg_switch ADD CONSTRAINT fk_switch_uplinkport_cfg FOREIGN KEY ( upg_id ) REFERENCES  uplink_pg( pg_id ) ON DELETE RESTRICT;

ALTER TABLE  wildcast_cfg_switch_bond ADD CONSTRAINT fk_wildcast_cfg_bond FOREIGN KEY ( wildcast_cfg_switch_id, port_type ) REFERENCES  wildcast_cfg_switch( id, port_type ) ON DELETE CASCADE;

ALTER TABLE  wildcast_cfg_switch_bond_map ADD CONSTRAINT fk_su_cfg_port_bond_map FOREIGN KEY ( wildcast_cfg_bond_id ) REFERENCES  wildcast_cfg_switch_bond( wildcast_cfg_switch_id ) ON DELETE CASCADE;

ALTER TABLE  wildcast_task_create_vport ADD CONSTRAINT fk_wildcast_task_create_vport FOREIGN KEY ( wildcast_cfg_create_vport_id ) REFERENCES  wildcast_cfg_create_virtual_port( id ) ON DELETE RESTRICT;

ALTER TABLE  wildcast_task_create_vport ADD CONSTRAINT fk_wildcast_task_create_vport_1 FOREIGN KEY ( vna_id ) REFERENCES  vna( id ) ON DELETE CASCADE;

ALTER TABLE  wildcast_task_loopback ADD CONSTRAINT fk_wildcast_task_loopback FOREIGN KEY ( wildcast_cfg_loopback_port_id ) REFERENCES  wildcast_cfg_loopback_port( id ) ON DELETE RESTRICT;

ALTER TABLE  wildcast_task_loopback ADD CONSTRAINT fk_wildcast_task_loopback_vna FOREIGN KEY ( vna_id ) REFERENCES  vna( id ) ON DELETE CASCADE;

ALTER TABLE  wildcast_task_switch ADD CONSTRAINT fk_wildcast_task_switch FOREIGN KEY ( wildcast_cfg_switch_id ) REFERENCES  wildcast_cfg_switch( id ) ON DELETE RESTRICT;

ALTER TABLE  wildcast_task_switch ADD CONSTRAINT fk_wildcast_task_switch_vna FOREIGN KEY ( vna_id ) REFERENCES  vna( id ) ON DELETE CASCADE;


  

	

-- create procedure	
-- 1 serial.txt

-- create serial_virtual_switch_map  
CREATE SEQUENCE SERIAL_VIRTUAL_SWITCH_MAP START 1;


-- VSI_PROFILE SERIAL
CREATE SEQUENCE SERIAL_VSI_PROFILE START 1;
CREATE SEQUENCE SERIAL_VNIC START 1;


-- vlan range SERIAL
CREATE SEQUENCE SERIAL_VLAN_RANGE START 1;


-- vlan map SERIAL
CREATE SEQUENCE SERIAL_VLAN_MAP START 1;


-- vlan pool SERIAL
CREATE SEQUENCE SERIAL_VLAN_POOL START 1;



-- NETPLANE SERIAL
CREATE SEQUENCE SERIAL_LOGICNETWORK_IP_POOL START 1;



-- mac pool SERIAL
CREATE SEQUENCE SERIAL_MAC_POOL START 1;


-- NETPLANE SERIAL
CREATE SEQUENCE SERIAL_LOGICNETWORK_IP_RANGE START 1;



-- NETPLANE SERIAL
CREATE SEQUENCE SERIAL_IP_RANGE START 1;


-- NETPLANE SERIAL
CREATE SEQUENCE SERIAL_MAC_RANGE START 1;


-- NETPLANE SERIAL
CREATE SEQUENCE SERIAL_PORT START 1;


-- NETPLANE SERIAL
CREATE SEQUENCE SERIAL_BOND_MAP START 1;



-- NETPLANE SERIAL
CREATE SEQUENCE SERIAL_LOGICNETWORK START 1;


-- 创建switch serial 
CREATE SEQUENCE SERIAL_SWITCH START 1;


-- create serial_port_group_trunk_vlan_range 
CREATE SEQUENCE SERIAL_PORT_GROUP_TRUNK_VLAN_RANGE START 1;



-- create serial_port_group
CREATE SEQUENCE SERIAL_PORT_GROUP START 1;



-- NETPLANE SERIAL
CREATE SEQUENCE SERIAL_NETPLANE START 1;


-- vna resigt module SERIAL
CREATE SEQUENCE SERIAL_VNA_REGIST_MODULE START 1;


-- port mac serial
CREATE SEQUENCE SERIAL_PORT_MAC START 1;

-- port ip serial
CREATE SEQUENCE SERIAL_PORT_IP START 1;

-- NETPLANE SERIAL
CREATE SEQUENCE SERIAL_VNA START 1;


-- KERNEL REPORT SERIAL
CREATE SEQUENCE SERIAL_KERNEL_CFG_REPORT START 1;

-- KERNEL REPORT PORT_BOND SERIAL
CREATE SEQUENCE SERIAL_KERNEL_CFG_REPORT_BOND_MAP START 1;


-- WILDCAST CREATE VIRTUAL PORT SERIAL
CREATE SEQUENCE SERIAL_WILDCAST_CREATE_VIRTUAL_PORT START 1;

-- WILDCAST LOOPBACK PORT SERIAL
CREATE SEQUENCE SERIAL_WILDCAST_LOOPBACK_PORT START 1;

-- WILDCAST SWITCH SERIAL
CREATE SEQUENCE SERIAL_WILDCAST_SWITCH START 1;

-- WILDCAST SWITCH BOND MAP SERIAL
CREATE SEQUENCE SERIAL_WILDCAST_SWITCH_BOND_MAP START 1;

-- WILDCAST TASK SWITCH SERIAL
CREATE SEQUENCE SERIAL_WILDCAST_TASK_SWITCH START 1;

-- WILDCAST TASK CREATE VPORT SERIAL
CREATE SEQUENCE SERIAL_WILDCAST_TASK_CREATE_VPORT START 1;

-- WILDCAST TASK LOOPBACK SERIAL
CREATE SEQUENCE SERIAL_WILDCAST_TASK_LOOPBACK START 1;



-- 2 type

-- vsi 
CREATE TYPE t_query_vsi  AS(
    ret integer, -- proc ret
    vm_id numeric(19),
    nic_index integer, 
    is_sriov integer, 
    is_loop integer, 
    logic_network_uuid character varying(64),
    port_type character varying(64),
	
    vsi_id_value character varying(64),
    vsi_id_format integer,
    vm_pg_uuid character varying(64),
    mac character varying(64),
    is_has_ip integer, 
    ip  character varying(64),
    mask character varying(64),
    is_has_vswitch integer,  
    vswitch_uuid character varying(64),
    is_has_sriovvf integer,
    sriovvfport_uuid character varying(64)
);



-- debug 
-- RAISE EXCEPTION 'is_same here is %, %, %, %', is_same, $1,$2,$3;

-- 通用添加修改删除存贮过程结果 
-- add modify del proc result 
--drop type t_operate_result;
create TYPE t_operate_result AS (
    ret integer,    -- =0 suc, = other failed
    id numeric(19,0),  -- if ret = 0, add modify valid.
    uuid character varying(64) -- if ret = 0, add modify valid.(netplane,port group,logic network,port)
);



-- 创建 TYPE t_port_query

create TYPE t_port_query AS (
    ret integer, -- proc ret
    vna_uuid  character varying(64),  -- vna uuid
    port_type integer, --- port type
    name character varying(64), -- port name 
    uuid character varying(64),  -- port uuid
    state integer, 
    is_broadcast integer,
    is_running integer,
    is_multicast integer,
    promiscuous integer,
    mtu integer,
    is_master integer,
    admin_state integer,
    is_online integer
);



-- 创建 TYPE t_port_summary_query
-- drop type t_port_summary_query;
create TYPE t_port_summary_query AS (
    ret integer, -- proc ret
    vna_uuid  character varying(64),  -- vna uuid
    port_type integer, --- port type
    name character varying(64), -- port name 
    uuid character varying(64)  -- port uuid
);




-- 创建 TYPE t_bond_query
--drop type t_bond_query;
create TYPE t_bond_query AS (
    ret integer, -- proc ret
    vna_uuid  character varying(64),  -- vna uuid
    port_type integer, --- port type
    name character varying(64), -- port name 
    uuid character varying(64),  -- port uuid
    state integer, 
    is_broadcast integer,
    is_running integer,
    is_multicast integer,
    promiscuous integer,
    mtu integer,
    is_master integer,
    admin_state integer,
    is_online integer,    
    is_port_cfg integer, 
    is_consistency integer, 
    
    bond_mode integer,
    policy  character varying(64),
    
    --lacp 信息
    is_has_lacpinfo integer, 
    lacp_state integer,
    lacp_aggregator_id character varying(64),
    lacp_partner_mac character varying(64)
);



-- 创建 TYPE t_phyport_query
--drop type t_phyport_query;
create TYPE t_phyport_query AS (
    ret integer, -- proc ret
    vna_uuid  character varying(64),  -- vna uuid
    port_type integer, --- port type
    name character varying(64), -- port name 
    uuid character varying(64),  -- port uuid
    state integer, 
    is_broadcast integer,
    is_running integer,
    is_multicast integer,
    promiscuous integer,
    mtu integer,
    is_master integer,
    admin_state integer,
    is_online integer,    
    is_port_cfg integer, 
    is_consistency integer, 
    
    supported_ports character varying(64),     -- physical port info
    supported_link_modes character varying(64),
    is_support_auto_negotiation integer,
    advertised_link_modes character varying(64),
    advertised_pause_frame_use character varying(64),
    is_advertised_auto_negotiation integer,
    speed character varying(64),
    duplex character varying(64),
    port character varying(64),
    phyad character varying(64),
    transceiver character varying(64),
    auto_negotiate character varying(64),
    supports_wake_on character varying(64),
    wake_on character varying(64),
    current_msg_level character varying(64),
    is_linked integer,
    is_rx_checksumming integer, -- extend info 
    is_tx_checksumming integer, 
    is_scatter_gather integer, 
    is_tcp_segmentation_offload integer,
    is_udp_fragmentation_offload integer,
    is_generic_segmentation_offload integer,
    is_generic_receive_offload integer, 
    is_large_receive_offload integer,
    is_sriov integer,       -- sriov info
    is_loop_report integer, 
    is_loop_cfg integer
);


create type t_logicnetwork_query AS (
   ret integer, -- proc ret
   uuid character varying,
   name character varying,
   pg_uuid character varying,
   des character varying,
   ipmode integer   
);


-- 创建 TYPE t_vna_query
create TYPE t_vna_query AS (
    ret integer, -- proc ret
    uuid character varying(64),  -- vna uuid
    vna_app character varying(64),  -- vna application    
    vnm_app character varying(64),  -- vnm application    
    host_name character varying(64),  -- host name        
    os_name character varying(64),  -- os name    
    is_online integer
);



CREATE TYPE t_query_total  AS(
    ret integer, -- proc ret
    total_num integer  --- 
);




-- 创建 TYPE t_vswtich_query
--drop type t_vswtich_query;
--DROP type t_vswtich_query;
create TYPE t_vswtich_query AS (
    ret integer, -- proc ret
    name character varying(64), -- port name 
    uuid character varying(64),  -- port uuid
    switch_type integer, --- switch type
    state integer, 
    max_mtu integer,
    evb_mode integer,
    pg_uuid character varying(64), 
    nic_max_num_cfg integer,        
    is_active integer
);





-- 创建 TYPE t_portgroup_query
--drop type t_portgroup_query;
create TYPE t_portgroup_query AS (
    ret integer, -- proc ret
    name  character varying(64),
    pg_type integer,
    uuid  character varying(64),
    acl character varying(64),
    qos character varying(64),
    allowed_priorities character varying(32),
    receive_bandwidth_limit character varying(32),
    receive_bandwidth_reserve character varying(32),
    default_vlan_id integer,
    promiscuous integer,
    macvlan_filter_enable integer,
    allowed_transmit_macs character varying(64),
    allowed_transmit_macvlans character varying(64),
    policy_block_override character varying(64),
    policy_vlan_override character varying(64),
    version integer,
    mgr_id character varying(64),
    type_id character varying(64),
    allow_mac_change integer,
    switchport_mode integer,    
    is_cfg_netplane integer,
    netplane_uuid character varying(64),
    mtu integer,    
    trunk_native_vlan_num integer,  --- if switchport_mode =0, valid.
    access_vlan_num integer,        --- if switchport_mode =1 & pg_type != 21, valid.
    access_isolate_no integer,      --- if switchport_mode =1 & pg_type = 21, valid.    
	description character varying(512)
);



-- 创建 TYPE t_sriovvfport_query
--drop type t_sriovvfport_query;
create TYPE t_sriovvfport_query AS (
    ret integer, -- proc ret
    vna_uuid  character varying(64),  -- vna uuid
    port_type integer, --- port type
    name character varying(64), -- port name 
    uuid character varying(64),  -- port uuid
    state integer, 
    is_broadcast integer,
    is_running integer,
    is_multicast integer,
    promiscuous integer,
    mtu integer,
    is_master integer,
    admin_state integer,
    is_online integer,    
    is_port_cfg integer, 
    is_consistency integer, 
    
    physical_sriov_uuid  character varying(64), -- physical sriov port
    pci  character varying(64),
    vlan_num integer
);


-- 创建 TYPE t_uplinkloopport_query
--drop type t_uplinkloopport_query;
create TYPE t_uplinkloopport_query AS (
    ret integer, -- proc ret
    vna_uuid  character varying(64),  -- vna uuid
    port_type integer, --- port type
    name character varying(64), -- port name 
    uuid character varying(64),  -- port uuid
    state integer, 
    is_broadcast integer,
    is_running integer,
    is_multicast integer,
    promiscuous integer,
    mtu integer,
    is_master integer,
    admin_state integer,
    is_online integer,    
    is_port_cfg integer, 
    is_consistency integer, 
        
    is_cfg integer
);

-- 创建 TYPE t_vswitchport_query
--drop type t_vswitchport_query;
create TYPE t_vswitchport_query AS (
    ret integer, -- proc ret
    vna_uuid  character varying(64),  -- vna uuid
    port_type integer, --- port type
    name character varying(64), -- port name 
    uuid character varying(64),  -- port uuid
    state integer, 
    is_broadcast integer,
    is_running integer,
    is_multicast integer,
    promiscuous integer,
    mtu integer,
    is_master integer,
    admin_state integer,
    is_online integer,    
    is_port_cfg integer, 
    is_consistency integer
);



-- 创建 TYPE t_kernelport_query
--drop type t_kernelport_query;
create TYPE t_kernelport_query AS (
    ret integer, -- proc ret
    vna_uuid  character varying(64),  -- vna uuid
    port_type integer, --- port type
    name character varying(64), -- port name 
    uuid character varying(64),  -- port uuid
    state integer, 
    is_broadcast integer,
    is_running integer,
    is_multicast integer,
    promiscuous integer,
    mtu integer,
    is_master integer,
    admin_state integer,
    is_online integer,    
    is_port_cfg integer, 
    is_consistency integer, 
        
    is_cfg_vswitch integer,
    vswitch_uuid  character varying(64),
    is_cfg_pg integer,
    kernel_pg_uuid  character varying(64),
    type integer,
    is_cfg integer,
	is_dhcp integer
);




-- 创建 TYPE t_netplane_query
create TYPE t_netplane_query AS (
    ret integer, -- proc ret
    uuid character varying(64),  -- netplane uuid
    netplane_name character varying(32),  -- netplane name    
    description character varying(512),
    mtu integer
);



-- 创建 TYPE t_vna_reg_module_query
create TYPE t_vna_reg_module_query AS (
    ret integer, -- proc ret
    vna_uuid character varying(64),  -- vna uuid
    module_uuid character varying(64),  -- module uuid     
    module_name character varying(64),  -- module name    
    role integer,  -- role
    is_online integer,
    is_has_cluster integer,
    cluster_name character varying(64)  -- cluster name    
);
-- 创建 TYPE t_port_mac_query
create TYPE t_port_mac_query AS (
    ret integer, -- proc ret
    vna_uuid character varying(64),  -- vna uuid
    vna_name character varying(64),  -- vna name
    port_uuid character varying(64),  -- port uuid     
    port_name character varying(64),  -- port name     
    port_type integer,
    mac character varying(64)  -- mac
);

-- 创建 TYPE t_port_ip_query
create TYPE t_port_ip_query AS (
    ret integer, -- proc ret
    vna_uuid character varying(64),  -- vna uuid
    vna_name character varying(64),  -- vna name
    port_uuid character varying(64),  -- port uuid     
    port_name character varying(64),  -- port name     
    port_type integer,
    port_ip_name character varying(64),  -- port name     
    ip character varying(64), 
    mask character varying(64), 
    is_cfg integer,
    is_online integer
);

-- 创建 TYPE t_kernelreport_query
create TYPE t_kernelreport_query AS (
    ret integer, -- proc ret
    vna_uuid character varying(64),  -- vna uuid
    uuid character varying(64),  --  uuid     
    switch_name character varying(64),    
    ip character varying(64), 
    mask character varying(64), 
    is_online integer,
    kernel_port_name character varying(64),
    uplink_port_name character varying(64),
    uplink_port_type integer,
	is_dhcp integer 
);


-- 创建 TYPE t_wildcast_create_vport_query
create TYPE t_wildcast_create_vport_query AS (
    ret integer, -- proc ret
    uuid character varying(64),  -- vna uuid
    port_name character varying(64),  -- port name     
    port_type integer,
    is_has_kernel_type integer, 
    kernel_type integer,
    kernel_pg_uuid character varying(64),
    switch_uuid character varying(64),
    is_dhcp integer
);

-- 创建 TYPE t_wildcast_loopback_port_query
create TYPE t_wildcast_loopback_port_query AS (
    ret integer, -- proc ret
    uuid character varying(64),  -- vna uuid
    port_name character varying(64),  -- port name    
    is_loop integer
);


-- 创建 TYPE t_wildcast_switch_query
create TYPE t_wildcast_switch_query AS (
    ret integer, -- proc ret
    uuid character varying(64),  --  uuid     
    switch_name character varying(64),  
    switch_type integer,
	state  integer,
	evb_mode integer,
    pg_uuid character varying(64),
    mtu_max integer,
    nic_max_num integer,
    port_name character varying(64),
    port_type integer,
    bond_mode integer
);

-- 创建 TYPE t_wildcast_task_switch_query
create TYPE t_wildcast_task_switch_query AS (
    ret integer, -- proc ret
    task_uuid character varying(64),  --  uuid     
	task_state integer,
	task_send_num integer,
	switch_uuid character varying(64),  
    switch_name character varying(64),  
    switch_type integer,
    pg_uuid character varying(64),
    mtu_max integer,
    nic_max_num integer,
    port_name character varying(64),
    port_type integer,
    bond_mode integer,
	vna_uuid character varying(64),
	vna_is_online integer
);

-- 创建 TYPE t_wildcast_task_create_vport_query
create TYPE t_wildcast_task_create_vport_query AS (
    ret integer, -- proc ret
    task_uuid character varying(64),  --  uuid     
	task_state integer,
	task_send_num integer,
	create_vport_uuid character varying(64),  
    port_name character varying(64),  
    port_type integer,
    is_has_kernel_type integer,
    kernel_type integer,
	kernel_pg_uuid character varying(64),
	switch_type integer,
	switch_uuid character varying(64),
	is_dhcp integer,
	vna_uuid character varying(64),
	vna_is_online integer
);

-- 创建 TYPE t_wildcast_task_loopback_query
create TYPE t_wildcast_task_loopback_query AS (
    ret integer, -- proc ret
    task_uuid character varying(64),  --  uuid     
	task_state integer,
	task_send_num integer,
	loopback_uuid character varying(64),  
    port_name character varying(64), 
    is_loop integer,
	vna_uuid character varying(64),
	vna_is_online integer
);

-- 创建 Type t_vm_migrate_query
create TYPE t_vm_migrate_query AS (
    ret integer, -- proc ret
    vm_id numeric(19,0),  --  uuid    
	src_clustername character varying(64),  
    src_hostname character varying(64), 
	dst_clustername character varying(64),  
    dst_hostname character varying(64), 
    state integer
);

-- 创建type  t_vm_query
create TYPE t_vm_query AS (
    ret integer, -- proc ret
    vm_id numeric(19,0),  --  uuid    
	project_id numeric(19,0),
	is_enable_watchdog integer,
	watchdog_timeout integer,
	is_has_cluster integer,
	cluster_name character varying(64),  
	is_has_host integer,
    host_name character varying(64)
);



-- 3 statistic_sriovvf_port
-- 触发器 内部错误码，统一加上100，便于定位
-- pf_net_tr_calc_bond_sriovport_num 此函数特殊，在触发器内部被调用，错误码+200

--- bond 的 total_sriov_port, valid sriov port 时,计算
CREATE OR REPLACE FUNCTION pf_net_tr_calc_bond_sriovport_num(in_port_id numeric(19,0))
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  oldtotalsriovnum integer := 0;
  oldvalidsriovnum integer := 0;
  totalsriovnum integer := 0;
  validsriovnum integer := 0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+200;
  RTN_HAS_SAME_ITEM integer := 2+200;
  RTN_ITEM_CONFLICT integer := 3+200;
  RTN_ITEM_NO_EXIST integer := 4+200;
BEGIN
  result := 0; 
    
    SELECT count(*) INTO is_exist FROM port a, bond b WHERE a.id = b.port_id AND a.id = in_port_id ;     
    SELECT b.total_sriovport_num INTO oldtotalsriovnum FROM port a, bond b WHERE a.id = b.port_id AND a.id = in_port_id ; 
    SELECT b.valid_sriovport_num INTO oldvalidsriovnum FROM port a, bond b WHERE a.id = b.port_id AND a.id = in_port_id ;     
    IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
    END IF;
    
    SELECT count(*) INTO totalsriovnum FROM bond_map a, port b, sriov_port c
    WHERE a.bond_id = in_port_id AND
          a.physical_port_id = b.id  AND b.id = c.physical_port_id ;
          
    SELECT count(*) INTO validsriovnum FROM bond_map a, port b, sriov_port c
    WHERE a.bond_id = in_port_id AND
          a.physical_port_id = b.id  AND b.id = c.physical_port_id AND
          b.is_consistency = 1 AND 
          b.free_sriovvf_num > 0;           
          
    -- 没有变化 
    IF (oldtotalsriovnum = totalsriovnum AND oldvalidsriovnum = validsriovnum ) THEN
        return result;
    END IF;
          
  BEGIN 
      UPDATE bond SET total_sriovport_num = totalsriovnum, valid_sriovport_num = validsriovnum WHERE port_id = in_port_id ;
      
  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 







--添加SRIOV VF 时触发 流程 
CREATE OR REPLACE FUNCTION pf_net_tr_add_sriovvfport(in_sriovvfport_id numeric(19,0), in_port_id numeric(19,0))
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  oldtotalnum integer := 0;
  oldfreenum integer := 0;
  bondid numeric(19,0);
  edvsid numeric(19,0);
  oldedvsnicmaxnum integer :=0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0; 
  
  SELECT count(*) INTO is_exist FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
  SELECT a.total_sriovvf_num INTO oldtotalnum FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
  SELECT a.free_sriovvf_num INTO oldfreenum FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
  -- update port total_num / free_num 
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
  END IF;
  
  BEGIN 
        UPDATE port SET total_sriovvf_num = (oldtotalnum +1), free_sriovvf_num = (oldfreenum + 1) WHERE id = in_port_id;
        
        
        -- 判断该SRIOV 网口是否加入了EDVS， 如时，需更新EDVS nic_max_num_cfg
        SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id AND a.is_consistency = 1;
        SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id;
        SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id;
        IF is_exist > 0 THEN 
            UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum + 1) WHERE switch_id = edvsid;
        END IF;

        -- 判断该SRIOV 网口是否加入了BOND
        SELECT count(*) INTO is_exist FROM bond_map WHERE physical_port_id = in_port_id;
        SELECT bond_id INTO bondid FROM bond_map WHERE physical_port_id = in_port_id;
        IF is_exist > 0 THEN 
        
            SELECT total_sriovvf_num INTO oldtotalnum FROM port WHERE id = bondid ;
            SELECT free_sriovvf_num INTO oldfreenum FROM port WHERE id = bondid ;
            UPDATE port SET total_sriovvf_num = (oldtotalnum +1), free_sriovvf_num = (oldfreenum + 1) WHERE id = bondid;
            
            -- bond 口的total_sriov_num, valid_sriov_num 
            result := pf_net_tr_calc_bond_sriovport_num(bondid);
            IF result > 0 THEN 
            
            END IF;
            
            -- 判断该BOND口是否加入了EDVS ， 如时，需更新EDVS nic_max_num_cfg
            SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid AND a.is_consistency = 1;
            SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid;
            SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid;
            IF is_exist > 0 THEN 
                UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum + 1) WHERE switch_id = edvsid;
            END IF;            
            
        END IF;
                
  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- SRIOV VF 修改时触发 流程 (online -> offline or offline -> online ) 主要考虑换后插卡 
CREATE OR REPLACE FUNCTION pf_net_tr_modify_sriovvfport(in_sriovvfport_id numeric(19,0), in_port_id numeric(19,0), in_status integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  oldtotalnum integer := 0;
  oldfreenum integer := 0;
  bondid numeric(19,0);
  edvsid numeric(19,0);
  oldedvsnicmaxnum integer :=0;
  oldsriovvfisconsistency integer :=0;
  ADJUSTVALUE integer :=0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0;
  
  SELECT count(*) INTO is_exist FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
  SELECT a.total_sriovvf_num INTO oldtotalnum FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
  SELECT a.free_sriovvf_num INTO oldfreenum FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
  -- update port total_num / free_num 
  IF is_exist <= 0 THEN 
    result := RTN_ITEM_NO_EXIST;
    return result;
  END IF; 
  
  SELECT count(*)INTO is_exist FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.id = in_sriovvfport_id;
  SELECT a.is_consistency INTO oldsriovvfisconsistency FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.id = in_sriovvfport_id;
  IF is_exist <= 0 THEN 
    result := RTN_ITEM_NO_EXIST;
    return result;
  END IF;
/*  由上层来保证 确保变化后调用此函数  (因为需考虑事物操作)
  -- 没有变化，无需操作 
  IF oldsriovvfisconsistency = in_status THEN 
      result := 0;
      return result;
  END IF;
*/  
  IF in_status <= 0 THEN 
      ADJUSTVALUE = -1;
  ELSE
      ADJUSTVALUE = 1;
  END IF;
  
  BEGIN     
        UPDATE port SET free_sriovvf_num = (oldfreenum + ADJUSTVALUE) WHERE id = in_port_id;
        
        -- 判断该SRIOV 网口是否加入了EDVS， 如时，需更新EDVS nic_max_num_cfg
        SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id AND a.is_consistency = 1;
        SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id;
        SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id;
        IF is_exist > 0 THEN 
            UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum + ADJUSTVALUE) WHERE switch_id = edvsid;
        END IF;
        
        -- 判断该SRIOV 网口是否加入了BOND
        SELECT count(*) INTO is_exist FROM bond_map WHERE physical_port_id = in_port_id;
        SELECT bond_id INTO bondid FROM bond_map WHERE physical_port_id = in_port_id;
        IF is_exist > 0 THEN 
            SELECT total_sriovvf_num INTO oldtotalnum FROM port WHERE id = bondid ;
            SELECT free_sriovvf_num INTO oldfreenum FROM port WHERE id = bondid ;
            UPDATE port SET free_sriovvf_num = (oldfreenum + ADJUSTVALUE) WHERE id = bondid;
            
            -- bond 口的total_sriov_num, valid_sriov_num 
            result := pf_net_tr_calc_bond_sriovport_num(bondid);
            IF result > 0 THEN 
            
            END IF;
            
            -- 判断该BOND口是否加入了EDVS ， 如时，需更新EDVS nic_max_num_cfg
            SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid AND a.is_consistency = 1;
            SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid;
            SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid;
            IF is_exist > 0 THEN 
                UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum + ADJUSTVALUE) WHERE switch_id = edvsid;
            END IF;    

        END IF;
  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 



--修改SRIOV网口时 时触发 流程 
-- 流程 (online -> offline or offline -> online ) 
CREATE OR REPLACE FUNCTION pf_net_tr_modify_sriovport( in_port_id numeric(19,0), in_status integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  oldPortTotalnum integer := 0;
  oldPortFreenum integer := 0;
  oldtotalnum integer := 0;
  oldfreenum integer := 0;
  bondid numeric(19,0);
  edvsid numeric(19,0);
  oldedvsnicmaxnum integer :=0;
  oldsriovvfisconsistency integer :=0;
  ADJUSTVALUE integer :=0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0;
  
  SELECT count(*) INTO is_exist FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
  SELECT a.total_sriovvf_num INTO oldPortTotalnum FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
  SELECT a.free_sriovvf_num INTO oldPortFreenum FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
  -- update port total_num / free_num 
  IF is_exist <= 0 THEN 
    result := RTN_ITEM_NO_EXIST;
    return result;
  END IF; 
  
/*  由上层来保证 确保变化后调用此函数  (因为需考虑事物操作), 不能重复调用 
  -- 没有变化，无需操作 
  IF oldsriovvfisconsistency = in_status THEN 
      result := 0;
      return result;
  END IF;
*/  
  IF in_status <= 0 THEN 
      ADJUSTVALUE = -1;
  ELSE
      ADJUSTVALUE = 1;
  END IF;
  
  BEGIN     
        -- UPDATE port SET free_sriovvf_num = (oldfreenum + ADJUSTVALUE) WHERE id = in_port_id;
        
        -- 判断该SRIOV 网口是否加入了EDVS， 如时，需更新EDVS nic_max_num_cfg
        SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id AND a.is_consistency = 1;
        SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id;
        SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id;
        IF is_exist > 0 THEN 
            IF in_status > 0 THEN 
                 UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum + oldPortFreenum) WHERE switch_id = edvsid;
            ELSE 
                 UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum - oldPortFreenum) WHERE switch_id = edvsid;
            END IF;
        END IF;
        
        -- 判断该SRIOV 网口是否加入了BOND
        SELECT count(*) INTO is_exist FROM bond_map WHERE physical_port_id = in_port_id;
        SELECT bond_id INTO bondid FROM bond_map WHERE physical_port_id = in_port_id;
        IF is_exist > 0 THEN 
            SELECT total_sriovvf_num INTO oldtotalnum FROM port WHERE id = bondid ;
            SELECT free_sriovvf_num INTO oldfreenum FROM port WHERE id = bondid ;
            IF in_status > 0 THEN 
                UPDATE port SET free_sriovvf_num = (oldfreenum + oldPortFreenum), total_sriovvf_num = (oldtotalnum + oldPortTotalnum) WHERE id = bondid;
            ELSE 
                UPDATE port SET free_sriovvf_num = (oldfreenum - oldPortFreenum), total_sriovvf_num = (oldtotalnum - oldPortTotalnum) WHERE id = bondid;
            END IF;
            
            -- bond 口的total_sriov_num, valid_sriov_num 
            result := pf_net_tr_calc_bond_sriovport_num(bondid);
            IF result > 0 THEN 
            
            END IF;
            
            -- 判断该BOND口是否加入了EDVS ， 如时，需更新EDVS nic_max_num_cfg
            SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid AND a.is_consistency = 1;
            SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid;
            SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid;
            IF is_exist > 0 THEN 
                IF in_status > 0 THEN 
                    UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum + oldPortFreenum) WHERE switch_id = edvsid;
                ELSE 
                     UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum - oldPortFreenum) WHERE switch_id = edvsid;
                END IF;    
            END IF;    

        END IF;
  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


--删除SRIOV网口时 时触发 流程 
-- 流程 ( 网口从sriov-->非sriov) 
-- 流程 ( 网口从非sriov-->sriov)  
CREATE OR REPLACE FUNCTION pf_net_tr_sriovport_change_phyport( in_port_id numeric(19,0))
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  oldPortTotalnum integer := 0;
  oldPortFreenum integer := 0;
  oldtotalnum integer := 0;
  oldfreenum integer := 0;
  bondid numeric(19,0);
  edvsid numeric(19,0);
  oldedvsnicmaxnum integer :=0;
  oldsriovvfisconsistency integer :=0;
  ADJUSTVALUE integer :=0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0;
  
  SELECT count(*) INTO is_exist FROM port  WHERE id = in_port_id;
  SELECT total_sriovvf_num INTO oldPortTotalnum FROM port  WHERE id = in_port_id ;
  SELECT free_sriovvf_num INTO oldPortFreenum FROM port  WHERE id = in_port_id ;
  -- update port total_num / free_num 
  IF is_exist <= 0 THEN 
    result := RTN_ITEM_NO_EXIST;
    return result;
  END IF; 
  
/*  由上层来保证 确保变化后调用此函数  (因为需考虑事物操作), 不能重复调用 
  -- 没有变化，无需操作 
  IF oldsriovvfisconsistency = in_status THEN 
      result := 0;
      return result;
  END IF;
*/  
  IF (oldPortTotalnum =0 AND oldPortFreenum = 0 ) THEN 
      result := 0;
      return result;
  END IF;
  
  BEGIN     
        -- UPDATE port SET free_sriovvf_num = (oldfreenum + ADJUSTVALUE) WHERE id = in_port_id;
        
        -- 判断该SRIOV 网口是否加入了EDVS， 如时，需更新EDVS nic_max_num_cfg
        SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id AND a.is_consistency = 1;
        SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id;
        SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id;
        IF is_exist > 0 THEN             
            UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum - oldPortFreenum) WHERE switch_id = edvsid;            
        END IF;
        
        -- 判断该SRIOV 网口是否加入了BOND
        SELECT count(*) INTO is_exist FROM bond_map WHERE physical_port_id = in_port_id;
        SELECT bond_id INTO bondid FROM bond_map WHERE physical_port_id = in_port_id;
        IF is_exist > 0 THEN 
            SELECT total_sriovvf_num INTO oldtotalnum FROM port WHERE id = bondid ;
            SELECT free_sriovvf_num INTO oldfreenum FROM port WHERE id = bondid ;
            
            UPDATE port SET free_sriovvf_num = (oldfreenum - oldPortFreenum), total_sriovvf_num = (oldtotalnum - oldPortTotalnum) WHERE id = bondid;
            
            -- bond 口的total_sriov_num, valid_sriov_num 
            result := pf_net_tr_calc_bond_sriovport_num(bondid);
            IF result > 0 THEN 
            
            END IF;
                        
            -- 判断该BOND口是否加入了EDVS ， 如时，需更新EDVS nic_max_num_cfg
            SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid AND a.is_consistency = 1;
            SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid;
            SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid;
            IF is_exist > 0 THEN                 
                UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum - oldPortFreenum) WHERE switch_id = edvsid;                
            END IF;    

        END IF;
  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

--修改BOND网口时 时触发 流程 
-- 流程 (online -> offline or offline -> online ) 
CREATE OR REPLACE FUNCTION pf_net_tr_modify_bondport( in_port_id numeric(19,0), in_status integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  oldPortTotalnum integer := 0;
  oldPortFreenum integer := 0;
  oldtotalnum integer := 0;
  oldfreenum integer := 0;
  bondid numeric(19,0);
  edvsid numeric(19,0);
  oldedvsnicmaxnum integer :=0;
  oldsriovvfisconsistency integer :=0;
  ADJUSTVALUE integer :=0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0;
  
  SELECT count(*) INTO is_exist FROM port a, bond b WHERE a.id = in_port_id AND a.id = b.port_id ;
  SELECT a.total_sriovvf_num INTO oldPortTotalnum FROM port a, bond b WHERE a.id = in_port_id AND a.id = b.port_id ;
  SELECT a.free_sriovvf_num INTO oldPortFreenum FROM port a, bond b WHERE a.id = in_port_id AND a.id = b.port_id ;
  -- update port total_num / free_num 
  IF is_exist <= 0 THEN 
    result := RTN_ITEM_NO_EXIST;
    return result;
  END IF; 
  
  -- 该BOND为非SRIOV 口bond，无需更新EDVS 
  IF (oldPortTotalnum = 0 AND oldPortFreenum = 0) THEN 
      result := 0;
      return result;
  END IF;
  
/*  由上层来保证 确保变化后调用此函数  (因为需考虑事物操作), 不能重复调用 
  -- 没有变化，无需操作 
  IF oldsriovvfisconsistency = in_status THEN 
      result := 0;
      return result;
  END IF;
*/  
  IF in_status <= 0 THEN 
      ADJUSTVALUE = -1;
  ELSE
      ADJUSTVALUE = 1;
  END IF;
  
  BEGIN     
        -- UPDATE port SET free_sriovvf_num = (oldfreenum + ADJUSTVALUE) WHERE id = in_port_id;
        
        -- 判断该bond 网口是否加入了EDVS， 如时，需更新EDVS nic_max_num_cfg
        SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id AND a.is_consistency = 1;
        SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id;
        SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id;
        IF is_exist > 0 THEN 
            IF in_status > 0 THEN 
                 UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum + oldPortFreenum) WHERE switch_id = edvsid;
            ELSE 
                 UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum - oldPortFreenum) WHERE switch_id = edvsid;
            END IF;
        END IF;
        
  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 添加 bond_map 时触发
CREATE OR REPLACE FUNCTION pf_net_tr_add_bondmap(in_bond_id numeric(19,0), in_port_id numeric(19,0))
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  oldtotalnum integer := 0;
  oldfreenum integer := 0;
  porttotalnum integer := 0;
  portfreenum integer := 0;
  edvsid numeric(19,0);
  oldedvsnicmaxnum integer :=0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0; 
  
    SELECT count(*) INTO is_exist FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
    SELECT a.total_sriovvf_num INTO porttotalnum FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
    SELECT a.free_sriovvf_num INTO portfreenum FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
    -- update port total_num / free_num 
    IF is_exist <= 0 THEN 
      --result := RTN_ITEM_NO_EXIST;  -- 存在非SRIOV也能触发此流程
      return result;
    END IF;
    
    SELECT count(*) INTO is_exist FROM port  WHERE id = in_bond_id ;
    SELECT total_sriovvf_num INTO oldtotalnum FROM port  WHERE id = in_bond_id ;
    SELECT free_sriovvf_num INTO oldfreenum FROM port  WHERE id = in_bond_id ;
    IF is_exist <= 0 THEN 
      --result := RTN_ITEM_NO_EXIST; -- 存在非SRIOV也能触发此流程
      return result;
    END IF;
    
  BEGIN 
        UPDATE port SET total_sriovvf_num = (oldtotalnum +porttotalnum), free_sriovvf_num = (oldfreenum + portfreenum) WHERE id = in_bond_id;
        
        -- bond 口的total_sriov_num, valid_sriov_num 
        result := pf_net_tr_calc_bond_sriovport_num(in_bond_id);
        IF result > 0 THEN 
            
        END IF;
        
        -- 判断该BOND 网口是否加入了EDVS， 如时，需更新EDVS nic_max_num_cfg
        SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_bond_id AND a.is_consistency = 1;
        SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_bond_id;
        SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_bond_id;
        IF is_exist > 0 THEN 
            UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum + portfreenum) WHERE switch_id = edvsid;
        END IF;

  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 删除 bond_map 时触发
CREATE OR REPLACE FUNCTION pf_net_tr_del_bondmap(in_bond_id numeric(19,0), in_port_id numeric(19,0))
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  oldtotalnum integer := 0;
  oldfreenum integer := 0;
  porttotalnum integer := 0;
  portfreenum integer := 0;
  edvsid numeric(19,0);
  oldedvsnicmaxnum integer :=0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0; 
  
    SELECT count(*) INTO is_exist FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
    SELECT a.total_sriovvf_num INTO porttotalnum FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
    SELECT a.free_sriovvf_num INTO portfreenum FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
    -- update port total_num / free_num 
    IF is_exist <= 0 THEN 
      --result := RTN_ITEM_NO_EXIST;  -- 存在非SRIOV也能触发此流程
      return result;
    END IF;
    
    SELECT count(*) INTO is_exist FROM port  WHERE id = in_bond_id ;
    SELECT total_sriovvf_num INTO oldtotalnum FROM port  WHERE id = in_bond_id ;
    SELECT free_sriovvf_num INTO oldfreenum FROM port  WHERE id = in_bond_id ;
    IF is_exist <= 0 THEN 
      --result := RTN_ITEM_NO_EXIST;  -- 存在非SRIOV也能触发此流程
      return result;
    END IF;
    
  BEGIN 
        UPDATE port SET total_sriovvf_num = (oldtotalnum - porttotalnum), free_sriovvf_num = (oldfreenum - portfreenum) WHERE id = in_bond_id;
        
        -- bond 口的total_sriov_num, valid_sriov_num 
        result := pf_net_tr_calc_bond_sriovport_num(in_bond_id);
        IF result > 0 THEN 
            
        END IF;
        
        -- 判断网口是否加入了SWITCH
        -- 判断该BOND 网口是否加入了EDVS， 如时，需更新EDVS nic_max_num_cfg
        SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_bond_id AND a.is_consistency = 1;
        SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_bond_id;
        SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_bond_id;
        IF is_exist > 0 THEN 
            UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum - portfreenum) WHERE switch_id = edvsid;
        END IF;
        
  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 添加 edvs_map 时触发
CREATE OR REPLACE FUNCTION pf_net_tr_add_edvsmap(in_evds_id numeric(19,0), in_port_id numeric(19,0))
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  oldtotalnum integer := 0;
  oldfreenum integer := 0;
  porttotalnum integer := 0;
  portfreenum integer := 0;
  edvsid numeric(19,0);
  oldedvsnicmaxnum integer :=0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0; 
  
    SELECT count(*) INTO is_exist FROM virtual_switch WHERE switch_id = in_evds_id AND switch_type = 2;
    SELECT nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch WHERE switch_id = in_evds_id AND switch_type = 2;
    IF is_exist <= 0 THEN 
      --sdvs 也会调用 
      --result := RTN_ITEM_NO_EXIST;
      return result;
    END IF;
  
    SELECT count(*) INTO is_exist FROM port WHERE id = in_port_id ;
    SELECT total_sriovvf_num INTO porttotalnum FROM port WHERE id = in_port_id ;
    SELECT free_sriovvf_num INTO portfreenum FROM port WHERE id = in_port_id ;
    -- update port total_num / free_num 
    IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
    END IF;
        
  BEGIN 
      UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum +portfreenum)  WHERE switch_id = in_evds_id;    
        
  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 删除 edvs_map 时触发
CREATE OR REPLACE FUNCTION pf_net_tr_del_edvsmap(in_evds_id numeric(19,0), in_port_id numeric(19,0))
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  oldtotalnum integer := 0;
  oldfreenum integer := 0;
  porttotalnum integer := 0;
  portfreenum integer := 0;
  edvsid numeric(19,0);
  oldedvsnicmaxnum integer :=0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
BEGIN
  result := 0; 
  
    SELECT count(*) INTO is_exist FROM virtual_switch WHERE switch_id = in_evds_id AND switch_type = 2;
    SELECT nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch WHERE switch_id = in_evds_id AND switch_type = 2;
    IF is_exist <= 0 THEN 
      --sdvs 也会调用 
      --result := RTN_ITEM_NO_EXIST;
      return result;
    END IF;
  
    SELECT count(*) INTO is_exist FROM port WHERE id = in_port_id ;
    SELECT total_sriovvf_num INTO porttotalnum FROM port WHERE id = in_port_id ;
    SELECT free_sriovvf_num INTO portfreenum FROM port WHERE id = in_port_id ;
    -- update port total_num / free_num 
    IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
    END IF;
        
  BEGIN 
      UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum - portfreenum)  WHERE switch_id = in_evds_id;            
  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 修改 edvs_map 时触发 
-- in_status = 1, is_consistency 0-->1
-- in_status = 0, is_consistency 1-->0
CREATE OR REPLACE FUNCTION pf_net_tr_modify_edvsmap(in_evds_id numeric(19,0), in_port_id numeric(19,0), in_status integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  oldtotalnum integer := 0;
  oldfreenum integer := 0;
  porttotalnum integer := 0;
  portfreenum integer := 0;
  edvsid numeric(19,0);
  oldedvsnicmaxnum integer :=0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0; 
  
    SELECT count(*) INTO is_exist FROM virtual_switch WHERE switch_id = in_evds_id AND switch_type = 2;
    SELECT nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch WHERE switch_id = in_evds_id AND switch_type = 2;
    IF is_exist <= 0 THEN 
      --sdvs 也会调用 
      --result := RTN_ITEM_NO_EXIST;
      return result;
    END IF;
  
    SELECT count(*) INTO is_exist FROM port WHERE id = in_port_id ;
    SELECT total_sriovvf_num INTO porttotalnum FROM port WHERE id = in_port_id ;
    SELECT free_sriovvf_num INTO portfreenum FROM port WHERE id = in_port_id ;
    -- update port total_num / free_num 
    IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
    END IF;
        
  BEGIN 
      IF in_status > 0 THEN 
          UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum + portfreenum)  WHERE switch_id = in_evds_id;            
      ELSE
          UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum - portfreenum)  WHERE switch_id = in_evds_id;            
      END IF;
  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 添加 vsi_sriov 时触发
CREATE OR REPLACE FUNCTION pf_net_tr_add_vsisriov(in_vsi_id numeric(19,0), in_port_id numeric(19,0))
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  oldtotalnum integer := 0;
  oldfreenum integer := 0;
  porttotalnum integer := 0;
  portfreenum integer := 0;
  sriovportid numeric(19,0);
  bondid numeric(19,0);
  edvsid numeric(19,0);
  oldedvsnicmaxnum integer :=0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0; 
    
    SELECT count(*) INTO is_exist FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.id = in_port_id ;    
    SELECT b.sriov_port_id INTO sriovportid FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.id = in_port_id ;   
    IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
    END IF;
    
    SELECT total_sriovvf_num INTO porttotalnum FROM port WHERE id = sriovportid ;
    SELECT free_sriovvf_num INTO portfreenum FROM port WHERE id = sriovportid ;
    -- update port total_num / free_num 
        
  BEGIN 
      UPDATE port SET free_sriovvf_num = (portfreenum - 1) WHERE id = sriovportid ;
      
      -- 判断该SRIOV 网口是否加入了EDVS， 如时，需更新EDVS nic_max_num_cfg
        SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = sriovportid AND a.is_consistency = 1;
        SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = sriovportid;
        SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = sriovportid;
        IF is_exist > 0 THEN 
            UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum - 1) WHERE switch_id = edvsid;
        END IF;
        
        -- 判断该SRIOV 网口是否加入了BOND
        SELECT count(*) INTO is_exist FROM bond_map WHERE physical_port_id = sriovportid;
        SELECT bond_id INTO bondid FROM bond_map WHERE physical_port_id = sriovportid;
        IF is_exist > 0 THEN 
            SELECT free_sriovvf_num INTO oldfreenum FROM port WHERE id = bondid ;
            UPDATE port SET free_sriovvf_num = (oldfreenum - 1) WHERE id = bondid;
            
            -- bond 口的total_sriov_num, valid_sriov_num 
            result := pf_net_tr_calc_bond_sriovport_num(bondid);
            IF result > 0 THEN 
            
            END IF;
            
            -- 判断该BOND口是否加入了EDVS ， 如时，需更新EDVS nic_max_num_cfg
            SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid AND a.is_consistency = 1;
            SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid;
            SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid;
            IF is_exist > 0 THEN 
                UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum - 1) WHERE switch_id = edvsid;
            END IF;            
        END IF;
        
  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 删除 vsi_sriov 时触发
CREATE OR REPLACE FUNCTION pf_net_tr_del_vsisriov(in_vsi_id numeric(19,0), in_port_id numeric(19,0))
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  oldtotalnum integer := 0;
  oldfreenum integer := 0;
  porttotalnum integer := 0;
  portfreenum integer := 0;
  sriovportid numeric(19,0);
  bondid numeric(19,0);
  edvsid numeric(19,0);
  oldedvsnicmaxnum integer :=0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0; 
    
    SELECT count(*) INTO is_exist FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.id = in_port_id ;    
    SELECT b.sriov_port_id INTO sriovportid FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.id = in_port_id ;   
    IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
    END IF;
    
    SELECT total_sriovvf_num INTO porttotalnum FROM port WHERE id = sriovportid ;
    SELECT free_sriovvf_num INTO portfreenum FROM port WHERE id = sriovportid ;
    -- update port total_num / free_num 
        
        
  BEGIN 
      UPDATE port SET free_sriovvf_num = (portfreenum + 1) WHERE id = sriovportid ;
      
      -- 判断该SRIOV 网口是否加入了EDVS， 如时，需更新EDVS nic_max_num_cfg
        SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = sriovportid AND a.is_consistency = 1;
        SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = sriovportid;
        SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = sriovportid;
        IF is_exist > 0 THEN 
            UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum + 1) WHERE switch_id = edvsid;
        END IF;
        
        -- 判断该SRIOV 网口是否加入了BOND
        SELECT count(*) INTO is_exist FROM bond_map WHERE physical_port_id = sriovportid;
        SELECT bond_id INTO bondid FROM bond_map WHERE physical_port_id = sriovportid;
        IF is_exist > 0 THEN 
            SELECT free_sriovvf_num INTO oldfreenum FROM port WHERE id = bondid ;
            UPDATE port SET free_sriovvf_num = (oldfreenum + 1) WHERE id = bondid;
            
            -- bond 口的total_sriov_num, valid_sriov_num 
            result := pf_net_tr_calc_bond_sriovport_num(bondid);
            IF result > 0 THEN 
            
            END IF;
            
            -- 判断该BOND口是否加入了EDVS ， 如时，需更新EDVS nic_max_num_cfg
            SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid AND a.is_consistency = 1;
            SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid;
            SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid;
            IF is_exist > 0 THEN 
                UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum + 1) WHERE switch_id = edvsid;
            END IF;            
        END IF;
        
  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 



-- 4 statistic_switch


-- 触发器 内部返回值，统一加上100，便于定位
-- 

-- vsi attach vswitch 时触发 流程 
CREATE OR REPLACE FUNCTION pf_net_tr_vsi_attach_vswitch(in_vsi_id numeric(19,0), in_switch_id numeric(19,0))
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  olddeployvsinum integer := 0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0; 
  
  SELECT count(*) INTO is_exist FROM virtual_switch WHERE switch_id = in_switch_id;
  SELECT deploy_vsi_num INTO olddeployvsinum FROM virtual_switch WHERE switch_id = in_switch_id;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
  END IF;
  
  BEGIN 
        UPDATE virtual_switch SET deploy_vsi_num = (olddeployvsinum +1) WHERE switch_id = in_switch_id;
  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- vsi unattach vswitch 时触发 流程 
CREATE OR REPLACE FUNCTION pf_net_tr_vsi_unattach_vswitch(in_vsi_id numeric(19,0), in_switch_id numeric(19,0))
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  olddeployvsinum integer := 0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0; 
  
  SELECT count(*) INTO is_exist FROM virtual_switch WHERE switch_id = in_switch_id;
  SELECT deploy_vsi_num INTO olddeployvsinum FROM virtual_switch WHERE switch_id = in_switch_id;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
  END IF;
  
  BEGIN 
      UPDATE virtual_switch SET deploy_vsi_num = (olddeployvsinum - 1) WHERE switch_id = in_switch_id;                        
  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100; 

-- 5 vna







--- 查询VNA summary
--- 输出 name,uuid,
-- Function: pf_net_query_vna_summary()

-- DROP FUNCTION pf_net_query_vna_summary();

CREATE OR REPLACE FUNCTION pf_net_query_vna_summary()
  RETURNS refcursor AS
$BODY$
DECLARE
        ref refcursor;
BEGIN
       OPEN ref FOR SELECT vna_uuid,vna_application FROM vna;
       RETURN ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 






-- 查询VNA 
-- 输出 uuid,name,description,mtu
-- Function: pf_net_query_vna(character varying)

-- DROP FUNCTION pf_net_query_vna(character varying);

CREATE OR REPLACE FUNCTION pf_net_query_vna(in_vna_uuid character varying)
  RETURNS t_vna_query AS
$BODY$
DECLARE
  result  t_vna_query;
  is_exist integer;
  RTN_ITEM_NO_EXIST integer :=4;
BEGIN
  result.ret := 0;
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = $1;
  IF is_exist > 0 THEN
     result.ret := 0;
  ELSE
     result.ret := RTN_ITEM_NO_EXIST; -- no exist item
     return result;     
  END IF;

   SELECT vna_uuid into result.uuid  FROM vna WHERE vna_uuid = $1;
   SELECT vna_application into result.vna_app  FROM vna WHERE vna_uuid = $1;
   SELECT vnm_application into result.vnm_app  FROM vna WHERE vna_uuid = $1;
   SELECT host_name into result.host_name  FROM vna WHERE vna_uuid = $1;
   SELECT os_name into result.os_name  FROM vna WHERE vna_uuid = $1;   
   SELECT is_online into result.is_online  FROM vna WHERE vna_uuid = $1;   
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 



-- VNA添加 
-- Function: pf_net_add_vna(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_vna(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_vna(in_vna_uuid character varying, in_vna_app character varying, in_vnm_app character varying,in_host_name character varying,in_os_name character varying, in_is_online integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_conflict integer;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否已存在相同记录
  SELECT count(*) INTO is_same FROM vna WHERE vna_application = $2 AND vnm_application = $3 AND host_name = $4 AND os_name = $5 AND is_online = $6;  
  IF is_same > 0 THEN
     result.ret := RTN_HAS_SAME_ITEM;   -- has same item
     SELECT id INTO result.id FROM vna WHERE vna_application = $2;
     SELECT vna_uuid INTO result.uuid FROM vna WHERE vna_application = $2;
     return result;
  END IF;

  -- vna 名称是否冲突
  SELECT count(*) INTO is_conflict FROM vna WHERE vna_application = $2 ;
  IF is_conflict > 0 THEN
     result.ret := RTN_ITEM_CONFLICT; -- conflict 
     return result;
  END IF;

  -- insert item
  result.id := nextval('serial_vna');
  result.uuid := $1;
  BEGIN 
    INSERT INTO vna values(result.id,$1,$2,$3,$4,$5,$6);
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- vna修改
-- Function: pf_net_modify_vna(in_vna_uuid character varying, in_vna_app character varying, in_vnm_app character varying,in_host_name character varying,in_os_name character varying, in_is_online integer)

-- DROP FUNCTION pf_net_modify_vna(in_vna_uuid character varying, in_vna_app character varying, in_vnm_app character varying,in_host_name character varying,in_os_name character varying, in_is_online integer)

CREATE OR REPLACE FUNCTION pf_net_modify_vna(in_vna_uuid character varying, in_vna_app character varying, in_vnm_app character varying,in_host_name character varying,in_os_name character varying, in_is_online integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG  integer := 5;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否已存在相同记录
  SELECT count(*) INTO is_same FROM vna WHERE vna_uuid = $1 AND vna_application = $2 AND vnm_application = $3 AND host_name = $4 AND os_name = $5 AND is_online = $6;  
  IF is_same > 0 THEN
     result.ret := RTN_HAS_SAME_ITEM; -- same item
     SELECT id INTO result.id FROM vna WHERE vna_uuid = $1;
     return result;
  END IF;

  --  vna uuid记录是否存在
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = $1 ;
  IF is_exist <= 0 THEN
     result.ret := RTN_ITEM_NO_EXIST;-- don't exist item 
     return result;     
  END IF;
  
  -- vna uuid application
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = $1 AND vna_application = $2;
  IF is_exist <= 0 THEN
     SELECT count(*) INTO is_exist FROM vna WHERE vna_application = $2;
     IF is_exist > 0 THEN
       result.ret := RTN_ITEM_CONFLICT;-- netplane
       return result;          
     END IF;
  END IF;
  
  -- 
   
  -- modify item
  BEGIN 
    SELECT id INTO result.id FROM vna WHERE vna_uuid = $1;
    SELECT vna_uuid INTO result.uuid FROM vna WHERE vna_uuid = $1;    
    UPDATE vna SET vna_application = $2, vnm_application = $3,  host_name = $4 ,os_name = $5,is_online = $6 WHERE vna_uuid = $1 ;

    -- vna 离线后，将注册在此VNA上的MODULE is_online置为2
    IF $6 = 2 THEN 
        UPDATE regist_module SET is_online = 2 WHERE vna_id = result.id ;
    END IF;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 删除网络平面
-- Function: pf_net_del_vna(character varying)

-- DROP FUNCTION pf_net_del_vna(character varying);

CREATE OR REPLACE FUNCTION pf_net_del_vna(in_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  isonline integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_VNA_IS_ONLINE  integer := 20;
BEGIN
  result.ret := 0;
  isonline := 0;
  -- 入参检查 后面做
  -- 是否存在记录
  SELECT count(*) INTO is_same FROM vna WHERE vna_uuid = $1 ;  
  SELECT is_online INTO isonline FROM vna WHERE vna_uuid = $1 ;    
  IF is_same <= 0 THEN
     result.ret := RTN_ITEM_NO_EXIST;-- does not exist item
     return result;
  END IF;

  IF isonline = 1 THEN
     result.ret := RTN_VNA_IS_ONLINE;-- does not exist item
     return result;
  END IF;
  
    
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM vna WHERE vna_uuid = $1 ;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


/* 此存贮过程由网络提供给web，用于删除离线vna*/ 
CREATE OR REPLACE FUNCTION pf_net_web_del_vna(in_uuid character varying)
   RETURNS integer AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  isonline integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_VNA_IS_ONLINE  integer := 20;
BEGIN
  result  := 0;
  isonline := 0;
  -- 入参检查 后面做
  -- 是否存在记录
  SELECT count(*) INTO is_same FROM vna WHERE vna_uuid = $1 ;  
  SELECT is_online INTO isonline FROM vna WHERE vna_uuid = $1 ;    
  IF is_same <= 0 THEN
     result  := RTN_ITEM_NO_EXIST;-- does not exist item
     return result;
  END IF;

  IF isonline = 1 THEN
     result  := RTN_VNA_IS_ONLINE;-- vna is online 
     return result;
  END IF;
  
    
  -- del item
  result  := 0;
  BEGIN 
    DELETE FROM vna WHERE vna_uuid = $1 ;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 6 vna_reg_module.txt







--- 查询VNA reg module summary
--- 输出 name,uuid,
-- Function: pf_net_query_vna_reg_module_summary()

-- DROP FUNCTION pf_net_query_vna_reg_module_summary();

CREATE OR REPLACE FUNCTION pf_net_query_vna_reg_module_summary()
  RETURNS refcursor AS
$BODY$
DECLARE
        ref refcursor;
BEGIN
       OPEN ref FOR SELECT a.vna_uuid, b.module_uuid,b.module_name FROM vna a, regist_module b WHERE a.id = b.vna_id;
       RETURN ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 





-- 查询VNA reg mdoule
-- 输出 vna_uuid,module_uuid,module_name,role,is_online,cluster_name
-- Function: pf_net_query_vna_reg_module(character varying)

-- DROP FUNCTION pf_net_query_vna_reg_module(character varying);

CREATE OR REPLACE FUNCTION pf_net_query_vna_reg_module(in_module_uuid character varying)
  RETURNS t_vna_reg_module_query AS
$BODY$
DECLARE
  result  t_vna_reg_module_query;
  is_exist integer;
  RTN_ITEM_NO_EXIST integer :=4;
BEGIN
  result.ret := 0;
  SELECT count(*) INTO is_exist FROM regist_module WHERE module_uuid = $1;
  IF is_exist > 0 THEN
     result.ret := 0;
  ELSE
     result.ret := RTN_ITEM_NO_EXIST; -- no exist item
     return result;     
  END IF;

   SELECT vna_uuid into result.vna_uuid  FROM vna a, regist_module b WHERE module_uuid = $1 AND a.id = b.vna_id;
   SELECT module_name into result.module_name  FROM regist_module WHERE module_uuid = $1;
   SELECT module_uuid into result.module_uuid  FROM regist_module WHERE module_uuid = $1;
   SELECT role into result.role  FROM regist_module WHERE module_uuid = $1; 
   SELECT is_online into result.is_online  FROM regist_module WHERE module_uuid = $1;   
   
   SELECT count(*) into is_exist FROM regist_module a, hc_info b WHERE module_uuid = $1 AND a.id = b.regist_module_id;  
   IF is_exist <= 0 THEN 
       result.is_has_cluster := 0;
   ELSE 
       result.is_has_cluster := 1;
       SELECT cluster_name into result.cluster_name  FROM regist_module a, hc_info b WHERE module_uuid = $1 AND a.id = b.regist_module_id;   
   END IF;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- VNA reg module 添加 
-- Function: pf_net_add_vna_reg_module(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_vna_reg_module(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_vna_reg_module(in_vna_uuid character varying,in_module_uuid character varying, in_module_name character varying, in_role integer, 
              in_is_online integer, in_is_has_cluster integer,  in_cluster_name character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_same_hc integer;
  is_conflict integer;
  vnaid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否已存在相同记录
  -- check role
  IF in_role <= 0 THEN 
      result.ret := RTN_PARAM_INVALID; 
      return result;  
  END IF;
  
  IF in_role > 7 THEN 
      result.ret := RTN_PARAM_INVALID; 
      return result;  
  END IF;
  
  SELECT count(*) INTO is_same FROM regist_module a, vna b WHERE a.vna_id = b.id AND b.vna_uuid = $1 AND a.module_name = $3 AND a.role = $4 AND a.is_online = $5;  
  IF is_same > 0 THEN
     IF in_role = 1 THEN 
       SELECT count(*) INTO is_same_hc FROM regist_module a, hc_info c,vna b WHERE a.vna_id = b.id AND b.vna_uuid = $1 AND a.module_name = $3  AND a.id = c.regist_module_id AND cluster_name = in_cluster_name;
       IF is_same_hc > 0 THEN 
          result.ret := RTN_HAS_SAME_ITEM;   -- has same item
          SELECT a.id INTO result.id FROM regist_module a, vna b WHERE a.vna_id = b.id AND b.vna_uuid = $1 AND a.module_name = $3;
          SELECT a.module_uuid INTO result.uuid FROM regist_module a, vna b  WHERE a.vna_id = b.id AND b.vna_uuid = $1 AND a.module_name = $3;
          return result;
       END IF;
     ELSE
       result.ret := RTN_HAS_SAME_ITEM;   -- has same item
       SELECT a.id INTO result.id FROM regist_module a, vna b WHERE a.vna_id = b.id AND b.vna_uuid = $1 AND a.module_name = $3;
       SELECT a.module_uuid INTO result.uuid FROM regist_module a, vna b  WHERE a.vna_id = b.id AND b.vna_uuid = $1 AND a.module_name = $3;
       return result;
     END IF;
  END IF;

  -- vna uuid 是否存在  
  SELECT id into vnaid FROM vna WHERE vna_uuid = $1;
  SELECT count(*) INTO is_same FROM vna WHERE vna_uuid = $1;
  IF is_same = 0 THEN
     result.ret := RTN_ITEM_NO_EXIST;   -- vna uuid no exist
     return result;
  END IF;
  
  -- reg module 名称是否冲突
  SELECT count(*) INTO is_conflict FROM regist_module a, vna b WHERE a.vna_id = b.id AND b.vna_uuid = $1 AND a.module_name = $3 AND a.role = in_role;
  IF is_conflict > 0 THEN
     result.ret := RTN_ITEM_CONFLICT; -- conflict 
     return result;
  END IF;

  -- insert item
  result.id := nextval('serial_vna_regist_module');
  result.uuid := $2;
  BEGIN 
    INSERT INTO regist_module(id, vna_id, module_name,module_uuid,role,is_online) values(result.id,vnaid, in_module_name, in_module_uuid,in_role,in_is_online);
    IF in_role = 1 THEN 
       IF in_is_has_cluster = 1 THEN 
           INSERT INTO hc_info(regist_module_id,cluster_name,role)  values(result.id,in_cluster_name,in_role);
       END IF;
    END IF;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- vna修改
-- Function: pf_net_modify_vna_reg_module(in_vna_uuid character varying, in_vna_app character varying, in_vnm_app character varying,in_host_name character varying,in_os_name character varying, in_is_online integer)

-- DROP FUNCTION pf_net_modify_vna_reg_module(in_vna_uuid character varying, in_vna_app character varying, in_vnm_app character varying,in_host_name character varying,in_os_name character varying, in_is_online integer)

CREATE OR REPLACE FUNCTION pf_net_modify_vna_reg_module(in_vna_uuid character varying,in_module_uuid character varying, in_module_name character varying,
            in_role integer, in_is_online integer,in_is_has_cluster integer,in_cluster_name character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_same_hc integer;
  is_exist integer;
  is_exist_hc integer;
  regmoduleid numeric(19,0);
  vnaid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG  integer := 5;
  RTN_PARAM_INVALID integer := 7;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否已存在相同记录
 
  -- check role
  IF in_role <= 0 THEN 
      result.ret := RTN_PARAM_INVALID; 
      return result;  
  END IF;
  
  IF in_role > 7 THEN 
      result.ret := RTN_PARAM_INVALID; 
      return result;  
  END IF;
 
  -- vna uuid module uuid是否存在  
  SELECT id into vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT count(*) INTO is_same FROM vna b,regist_module a WHERE a.vna_id = b.id AND b.vna_uuid = in_vna_uuid AND a.module_uuid = in_module_uuid;
  IF is_same = 0 THEN
     result.ret := RTN_ITEM_NO_EXIST;   -- vna uuid no exist
     return result;
  END IF;
  
  -- reg module 名称是否冲突
  -- vna uuid application
  SELECT count(*) INTO is_exist FROM vna a, regist_module b WHERE a.id = b.vna_id AND a.vna_uuid =in_vna_uuid AND b.module_uuid = in_module_uuid AND b.module_name = in_module_name  
     AND b.role = in_role;
  IF is_exist <= 0 THEN
     SELECT  count(*) INTO is_exist FROM vna a, regist_module b WHERE a.id = b.vna_id AND a.vna_uuid =in_vna_uuid AND b.module_name = in_module_name;
     IF is_exist > 0 THEN
       result.ret := RTN_ITEM_CONFLICT;-- netplane
       return result;          
     END IF;
  END IF;
     
  -- modify item
  BEGIN 
    SELECT b.id INTO result.id FROM vna a, regist_module b WHERE a.id = b.vna_id AND a.vna_uuid =in_vna_uuid AND b.module_uuid = in_module_uuid;
    result.uuid := in_module_uuid;    
    UPDATE regist_module SET module_name = in_module_name, role = in_role,  is_online = in_is_online WHERE module_uuid = in_module_uuid;

    IF in_is_has_cluster = 1 THEN
       SELECT count(*) INTO is_exist_hc FROM regist_module a, hc_info b WHERE a.id = b.regist_module_id AND a.module_uuid = in_module_uuid; 
       SELECT id INTO regmoduleid FROM regist_module WHERE module_uuid = in_module_uuid; 
       IF is_exist_hc > 0 THEN                  
          UPDATE hc_info SET role = in_role, cluster_name = in_cluster_name WHERE regist_module_id = regmoduleid;
       ELSE
          INSERT INTO hc_info(regist_module_id,cluster_name,role) values(regmoduleid, in_cluster_name,in_role);
       END IF;
    ELSE
       SELECT count(*) INTO is_exist_hc FROM regist_module a, hc_info b WHERE a.id = b.regist_module_id AND a.module_uuid = in_module_uuid; 
       SELECT a.id INTO regmoduleid FROM regist_module a, hc_info b WHERE a.id = b.regist_module_id AND a.module_uuid = in_module_uuid; 
       IF is_exist_hc > 0 THEN      
          DELETE FROM hc_info WHERE regist_module_id = regmoduleid;
       END IF;
    END IF;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 删除网络平面
-- Function: pf_net_del_vna_reg_module(character varying)

-- DROP FUNCTION pf_net_del_vna_reg_module(character varying);

CREATE OR REPLACE FUNCTION pf_net_del_vna_reg_module(in_module_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否存在记录
  SELECT count(*) INTO is_exist FROM regist_module WHERE module_uuid = in_module_uuid ;  
  IF is_exist <= 0 THEN
     result.ret := RTN_ITEM_NO_EXIST;-- does not exist item
     return result;
  END IF;
    
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM regist_module WHERE module_uuid = in_module_uuid ;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 7 netplane


-- 网络平面添加 
-- Function: pf_net_add_netplane(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_netplane(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_netplane(in_netplane_name character varying, in_netplane_uuid character varying, in_des character varying, in_mtu integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_conflict integer;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否已存在相同记录
  SELECT count(*) INTO is_same FROM netplane WHERE name = in_netplane_name AND description = in_des  AND mtu = in_mtu;  
--  SELECT count(*) INTO is_same FROM netplane WHERE name = in_netplane_name  AND description = in_des AND mtu = in_mtu;    
  IF is_same > 0 THEN
     result.ret := RTN_HAS_SAME_ITEM;   -- has same item
     SELECT id INTO result.id FROM netplane WHERE name = in_netplane_name;
     SELECT uuid INTO result.uuid FROM netplane WHERE name = in_netplane_name;
     return result;
  END IF;

  -- netplane 名称是否冲突
  SELECT count(*) INTO is_conflict FROM netplane WHERE name = in_netplane_name ;
  IF is_conflict > 0 THEN
     result.ret := RTN_ITEM_CONFLICT; -- conflict 
     return result;
  END IF;

  -- insert item
  result.id := nextval('serial_netplane');
  result.uuid := $2;
  BEGIN 
    INSERT INTO netplane values(result.id,in_netplane_name,in_netplane_uuid,in_des,in_mtu);
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 网络平面修改
-- Function: pf_net_modify_netplane(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_modify_netplane(in_netplane_uuid character varying, in_netplane_name character varying, in_des character varying, in_mtu integer);

CREATE OR REPLACE FUNCTION pf_net_modify_netplane(in_netplane_uuid character varying, in_netplane_name character varying, in_des character varying, in_mtu integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  netplaneid  numeric(19,0);
  max_pg_mtu integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG  integer := 5;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否已存在相同记录
  SELECT count(*) INTO is_same FROM netplane WHERE uuid = in_netplane_uuid AND name = in_netplane_name AND description = in_des AND mtu = in_mtu;  
  IF is_same > 0 THEN
     result.ret := RTN_HAS_SAME_ITEM; -- same item
     SELECT id INTO result.id FROM netplane WHERE uuid = in_netplane_uuid;
     return result;
  END IF;

  --  netplane uuid记录是否存在
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_netplane_uuid ;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_netplane_uuid ;
  IF is_exist <= 0 THEN
     result.ret := RTN_ITEM_NO_EXIST;-- don't exist item 
     return result;     
  END IF;
  
  -- netplane uuid name
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_netplane_uuid AND name = in_netplane_name;
  IF is_exist <= 0 THEN
     SELECT count(*) INTO is_exist FROM netplane WHERE name = in_netplane_name;
     IF is_exist > 0 THEN
       result.ret := RTN_ITEM_CONFLICT;-- netplane
       return result;          
     END IF;
  END IF;

  -- mtu 检查 不能比PG mtu小
  SELECT max(mtu) INTO max_pg_mtu FROM port_group WHERE netplane_id = netplaneid;
  IF max_pg_mtu > in_mtu THEN
      result.ret := RTN_NETPLANE_MTU_LESS_PG;-- pg mtu > np mtu
      return result;
  END IF; 
/*  
  SELECT count('PG_mtu') into is_exist FROM v_net_netplane_uplinkpg WHERE 'NP_uuid' = in_netplane_uuid ;
  IF is_exist > 0 THEN  
    SELECT max('PG_mtu') into max_pg_mtu FROM v_net_netplane_uplinkpg WHERE 'NP_uuid' = in_netplane_uuid ;
--    RAISE EXCEPTION 'max_pg_mtu here is %', max_pg_mtu;
    IF max_pg_mtu > in_mtu THEN
      result.ret := RTN_NETPLANE_MTU_LESS_PG;-- pg mtu > np mtu
      return result;
    END IF;
  END IF;
   */
  -- modify item
  BEGIN 
    SELECT id INTO result.id FROM netplane WHERE uuid = in_netplane_uuid;
    SELECT uuid INTO result.uuid FROM netplane WHERE uuid = in_netplane_uuid;    
    UPDATE netplane SET name = in_netplane_name, description = in_des, mtu = in_mtu WHERE uuid = in_netplane_uuid ;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 删除网络平面
-- Function: pf_net_del_netplane(character varying)

-- DROP FUNCTION pf_net_del_netplane(character varying);

CREATE OR REPLACE FUNCTION pf_net_del_netplane(netplane_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  max_pg_mtu integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否存在记录
  SELECT count(*) INTO is_same FROM netplane WHERE uuid = $1 ;  
  IF is_same <= 0 THEN
     result.ret := RTN_ITEM_NO_EXIST;-- does not exist item
     return result;
  END IF;

  -- netplane 是否被占用  
  -- PG
  SELECT count(*) INTO is_exist FROM netplane,port_group WHERE netplane.uuid = $1 AND netplane.id = port_group.netplane_id;
  IF is_exist > 0 THEN
     result.ret := RTN_RESOURCE_USING;-- resource has been using
     return result;
  END IF;

  -- vlan_range 
  SELECT count(*) INTO is_exist FROM netplane,vlan_range WHERE netplane.uuid = $1 AND netplane.id = vlan_range.netplane_id;
  IF is_exist > 0 THEN
     result.ret := RTN_RESOURCE_USING;-- resource has been using
     return result;
  END IF;

  -- mac_pool 
  SELECT count(*) INTO is_exist FROM netplane,mac_pool WHERE netplane.uuid = $1 AND netplane.id = mac_pool.netplane_id;
  IF is_exist > 0 THEN
     result.ret := RTN_RESOURCE_USING;-- resource has been using
     return result;
  END IF;

  -- mac_range 
  SELECT count(*) INTO is_exist FROM netplane,mac_range WHERE netplane.uuid = $1 AND netplane.id = mac_range.netplane_id;
  IF is_exist > 0 THEN
     result.ret := RTN_RESOURCE_USING;-- resource has been using
     return result;
  END IF;

  -- vlan_pool 
  SELECT count(*) INTO is_exist FROM netplane,vlan_pool WHERE netplane.uuid = $1 AND netplane.id = vlan_pool.netplane_id;
  IF is_exist > 0 THEN
     result.ret := RTN_RESOURCE_USING;-- resource has been using
     return result;
  END IF;

  -- ip_range 
  SELECT count(*) INTO is_exist FROM netplane,ip_range WHERE netplane.uuid = $1 AND netplane.id = ip_range.netplane_id;
  IF is_exist > 0 THEN
     result.ret := RTN_RESOURCE_USING;-- resource has been using
     return result;
  END IF;
   
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM netplane WHERE uuid = $1 ;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



--- 查询网络平面 summary
--- 输出 name,uuid
-- Function: pf_net_query_netplane_summary()

-- DROP FUNCTION pf_net_query_netplane_summary();

CREATE OR REPLACE FUNCTION pf_net_query_netplane_summary()
  RETURNS refcursor AS
$BODY$
DECLARE
        ref refcursor;
BEGIN
       OPEN ref FOR SELECT name,uuid FROM netplane;
       RETURN ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 查询网络平面 
-- 输出 uuid,name,description,mtu
-- Function: pf_net_query_netplane(character varying)

-- DROP FUNCTION pf_net_query_netplane(character varying);

CREATE OR REPLACE FUNCTION pf_net_query_netplane(netplane_uuid character varying)
  RETURNS t_netplane_query AS
$BODY$
DECLARE
  result  t_netplane_query;
  is_exist integer;
  RTN_ITEM_NO_EXIST integer :=4;
BEGIN
--  SELECT uuid into result.uuid, name into result.name,description into result.description,mtu into result.mtu FROM netplane WHERE uuid = $1;
  result.ret := 0;
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = $1;
  IF is_exist > 0 THEN
     result.ret := 0;
  ELSE
     result.ret := RTN_ITEM_NO_EXIST; -- no exist item
     return result;     
  END IF;

   SELECT uuid into result.uuid  FROM netplane WHERE uuid = $1;
   SELECT name into result.netplane_name  FROM netplane WHERE uuid = $1;
   SELECT description into result.description  FROM netplane WHERE uuid = $1;
   SELECT mtu into result.mtu  FROM netplane WHERE uuid = $1;   
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 8 netplane_macrange.txt



-- 判断netplane MAC RANGE 能否添加
CREATE OR REPLACE FUNCTION pf_net_check_add_netplane_macrange(in_uuid character varying, 
                           in_num_begin numeric(19), in_num_end numeric(19),
                           in_str_begin character varying,in_str_end character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_SWITCH_UPLINKPORT_INVALID integer := 9;
  RTN_PORT_HAS_JOIN_OTHER_SWITCH integer := 10;
  RTN_PG_MUST_CFG_NETPLANE   integer := 13;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- begin end 
  IF in_num_begin > in_num_end THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
/*  由应用来保准
  -- ip begin end mask 互相不能重叠  
  SELECT count(*) INTO is_exist FROM mac_range WHERE  ( in_num_begin >= mac_begin_num AND in_num_begin <= mac_end_num ) OR 
           ( in_num_end >= mac_begin_num AND in_num_end <= mac_end_num ) OR 
           ( in_num_begin <= mac_begin_num AND in_num_end >= mac_end_num );  
  IF is_exist > 0 THEN 
     result := RTN_ITEM_CONFLICT; 
     return result;
  END IF;  
*/
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- NETPLANE MAC RANGE添加 
-- Function: pf_net_add_netplane_macrange(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_netplane_macrange(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_netplane_macrange(in_uuid character varying, 
                           in_num_begin numeric(19), in_num_end numeric(19),
                           in_str_begin character varying,in_str_end character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_add_netplane_macrange(in_uuid,in_num_begin,in_num_end,
                            in_str_begin,in_str_end);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- 不支持重复添加 

  -- insert item 
  result.id := nextval('serial_mac_range');
  --result.uuid := in_uuid;
  BEGIN 
    INSERT INTO mac_range(id,netplane_id,mac_begin,mac_end,mac_begin_num,mac_end_num) 
            values(result.id,netplaneid,in_str_begin, in_str_end,in_num_begin,in_num_end);
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


                 
     

-- 删除 netplane mac range
CREATE OR REPLACE FUNCTION pf_net_del_netplane_macrange(in_uuid character varying,
                            in_num_begin numeric(19), in_num_end numeric(19),
                           in_str_begin character varying,in_str_end character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  netplaneid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result.ret  := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- begin end 
  IF in_num_begin > in_num_end THEN 
      result.ret  := RTN_PARAM_INVALID; 
      return result;
  END IF;
  
  -- del 不作检查，由应用保准
      
  SELECT count(*) INTO is_exist FROM mac_range WHERE netplane_id = netplaneid 
                AND mac_begin_num = in_num_begin AND mac_end_num = in_num_end 
                AND mac_begin = in_str_begin AND mac_end = in_str_end;
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;                
    
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM mac_range WHERE netplane_id = netplaneid 
                AND mac_begin_num = in_num_begin AND mac_end_num = in_num_end 
                AND mac_begin = in_str_begin AND mac_end = in_str_end;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


                           
                           
/*  -- 已有定义
CREATE TYPE t_query_total  AS(
    ret integer, -- proc ret
    total_num integer  --- 
);

*/                            

-- 检查 check netplane mac range 是否已有资源被分配
CREATE OR REPLACE FUNCTION pf_net_query_netplane_macrange_alloc(in_uuid character varying,
                            in_num_begin numeric(19), in_num_end numeric(19))
  RETURNS t_query_total AS
$BODY$
DECLARE
  result  t_query_total;
  is_same integer;
  is_exist integer;
  netplaneid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result.ret  := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- begin end 
  IF in_num_begin > in_num_end THEN 
      result.ret  := RTN_PARAM_INVALID; 
      return result;
  END IF;
        
  SELECT count(*) INTO result.total_num FROM mac_pool WHERE netplane_id = netplaneid 
                AND (mac_num >= in_num_begin AND mac_num <= in_num_end );
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 查询macrange(通过netplane uuid)

CREATE OR REPLACE FUNCTION pf_net_query_netplane_macrange(in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    netplaneid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    -- logicnetwork uuid 
    SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
    SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF;  
      
    OPEN ref FOR SELECT in_uuid, mac_begin_num,mac_end_num, mac_begin, mac_end 
      FROM mac_range  
      WHERE netplane_id = netplaneid;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 9 netplane_macpool


-- 判断NETPLANE MAC POOL 能否添加
CREATE OR REPLACE FUNCTION pf_net_check_add_netplane_macpool(in_uuid character varying, 
                           in_num_mac numeric(19), in_str_mac character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
    
  -- ip range 必须已经配置
  SELECT count(*) INTO is_exist FROM mac_range WHERE netplane_id = netplaneid  
       AND ( in_num_mac >= mac_begin_num AND in_num_mac <= mac_end_num ) ;  
  IF is_exist <= 0 THEN 
     result := RTN_MUST_CFG_VALID_RANGE; 
     return result;
  END IF;  
  
  -- mac pool 不能冲突
  SELECT count(*) INTO is_exist FROM mac_pool WHERE mac_num = in_num_mac;
  IF is_exist > 0 THEN 
     result := RTN_ITEM_CONFLICT; 
     return result;
  END IF;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- NETPLANE MAC POOL 添加 
-- Function: pf_net_add_netplane_macpool(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_netplane_macpool(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_netplane_macpool(in_uuid character varying, 
                           in_num_mac numeric(19), in_str_mac character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_add_netplane_macpool(in_uuid,in_num_mac,in_str_mac);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
   -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
    
  -- 不支持重复添加 

  -- insert item 
  result.id := nextval('serial_mac_pool');
  --result.uuid := in_uuid;
  BEGIN 
    INSERT INTO mac_pool(id,netplane_id,mac,mac_num) 
            values(result.id,netplaneid,in_str_mac, in_num_mac);
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


                 


-- 判断NETPLANE MAC POOL 能否del
CREATE OR REPLACE FUNCTION pf_net_check_del_netplane_macpool(in_uuid character varying, 
                           in_num_mac numeric(19), in_str_mac character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  netplaneid numeric(19,0);
  macid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
    
  SELECT count(*) INTO is_exist FROM mac_pool WHERE netplane_id = netplaneid 
                AND mac_num = in_num_mac AND mac = in_str_mac;
  SELECT id INTO macid FROM mac_pool WHERE netplane_id = netplaneid 
                AND mac_num = in_num_mac AND mac = in_str_mac;            
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;                
  
  -- 查看该资源是否在使用
  SELECT count(*) INTO is_exist FROM vsi_profile WHERE mac_id = macid;
  IF is_exist > 0 THEN 
     result := RTN_RESOURCE_USING; 
     return result;
  END IF;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;

                           

-- 删除 logicnetwork ip pool
CREATE OR REPLACE FUNCTION pf_net_del_netplane_macpool(in_uuid character varying, 
                           in_num_mac numeric(19), in_str_mac character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  netplaneid numeric(19,0);
  macid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- del 不作检查，由应用保准
  result.ret := pf_net_check_del_netplane_macpool(in_uuid,in_num_mac,in_str_mac);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
   -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
        
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM mac_pool WHERE netplane_id = netplaneid 
                AND mac_num = in_num_mac AND mac = in_str_mac;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;




-- 查询mac_pool(通过netplane_uuid)

CREATE OR REPLACE FUNCTION pf_net_query_netplane_macpool(in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    netplaneid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
     -- netplane uuid 
    SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
    SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
       --result := RTN_ITEM_NO_EXIST; 
       return ref;
    END IF;  
    
    SELECT count(*) INTO is_exist FROM mac_pool  
      WHERE netplane_id = netplaneid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF; 
  
    OPEN ref FOR SELECT in_uuid, mac_num, mac  
      FROM mac_pool  
      WHERE netplane_id = netplaneid;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;




-- 查询mac_pool(通过netplane_uuid, mac begin ,mac_end)

CREATE OR REPLACE FUNCTION pf_net_query_netplane_macpool_by_nprange(in_uuid character varying, in_begin numeric(19,0), in_end numeric(19,0))
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    netplaneid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
	RTN_PARAM_INVALID integer := 7;
BEGIN
     -- netplane uuid 
    SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
    SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
       --result := RTN_ITEM_NO_EXIST; 
       return ref;
    END IF;  
	    
    SELECT count(*) INTO is_exist FROM mac_pool  
      WHERE netplane_id = netplaneid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF; 
  
    OPEN ref FOR SELECT in_uuid, mac_num, mac  
      FROM mac_pool  
      WHERE netplane_id = netplaneid 
	      AND (mac_num >= in_begin AND mac_num <= in_end);
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 10 netplane_iprange.txt



-- 判断逻辑网络IP RANGE 能否添加
CREATE OR REPLACE FUNCTION pf_net_check_add_netplane_iprange(in_uuid character varying, 
                           in_num_begin numeric(19), in_num_end numeric(19),in_num_mask numeric(19),
                           in_str_begin character varying,in_str_end character varying,in_str_mask character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_SWITCH_UPLINKPORT_INVALID integer := 9;
  RTN_PORT_HAS_JOIN_OTHER_SWITCH integer := 10;
  RTN_PG_MUST_CFG_NETPLANE   integer := 13;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- begin end 
  IF in_num_begin > in_num_end THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  
/* 由应用来保准  
  -- ip begin end mask 互相不能重叠  
  SELECT count(*) INTO is_exist FROM ip_range WHERE  netplane_id = netplaneid AND (( in_num_begin >= ip_begin_num AND in_num_begin <= ip_end_num ) OR 
           ( in_num_end >= ip_begin_num AND in_num_end <= ip_end_num ) OR 
           ( in_num_begin <= ip_begin_num AND in_num_end >= ip_end_num ) );  
  IF is_exist > 0 THEN 
     result := RTN_ITEM_CONFLICT; 
     return result;
  END IF;  
*/             
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 逻辑网络IP RANGE添加 
-- Function: pf_net_add_netplane_iprange(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_netplane_iprange(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_netplane_iprange(in_uuid character varying, 
                           in_num_begin numeric(19), in_num_end numeric(19),in_num_mask numeric(19),
                           in_str_begin character varying,in_str_end character varying,in_str_mask character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_add_netplane_iprange(in_uuid,in_num_begin,in_num_end,in_num_mask, 
                            in_str_begin,in_str_end,in_str_mask);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- 不支持重复添加 

  -- insert item 
  result.id := nextval('serial_ip_range');
  --result.uuid := in_uuid;
  BEGIN 
    INSERT INTO ip_range(id,netplane_id,ip_begin,ip_end,mask,ip_begin_num,ip_end_num,mask_num) 
            values(result.id,netplaneid,in_str_begin, in_str_end,in_str_mask,in_num_begin,in_num_end,in_num_mask);
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



                           
     

-- 删除 netplane iprange
CREATE OR REPLACE FUNCTION pf_net_del_netplane_iprange(in_uuid character varying,
                            in_num_begin numeric(19), in_num_end numeric(19),in_num_mask numeric(19),
                           in_str_begin character varying,in_str_end character varying,in_str_mask character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  netplaneid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- begin end 
  IF in_num_begin > in_num_end THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  
  -- del 不作检查，由应用保准
      
  SELECT count(*) INTO is_exist FROM ip_range WHERE netplane_id = netplaneid 
                AND ip_begin_num = in_num_begin AND ip_end_num = in_num_end AND mask_num = in_num_mask 
                AND ip_begin = in_str_begin AND ip_end = in_str_end AND mask = in_str_mask;
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;                
  
  -- 如果逻辑网络在用，不能删除 
  SELECT count(*) INTO is_exist FROM port_group a, logic_network b, logic_network_ip_range c
     WHERE a.netplane_id = netplaneid AND b.vm_pg_id = a.id AND c.logic_network_id = b.id;
  IF is_exist > 0 THEN 
     result.ret := RTN_RESOURCE_USING; 
     return result;
  END IF; 
      
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM ip_range WHERE netplane_id = netplaneid 
                AND ip_begin_num = in_num_begin AND ip_end_num = in_num_end AND mask_num = in_num_mask 
                AND ip_begin = in_str_begin AND ip_end = in_str_end AND mask = in_str_mask;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 强制删除 netplane iprange
CREATE OR REPLACE FUNCTION pf_net_force_del_netplane_iprange(in_uuid character varying,
                            in_num_begin numeric(19), in_num_end numeric(19),in_num_mask numeric(19),
                           in_str_begin character varying,in_str_end character varying,in_str_mask character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  netplaneid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- begin end 
  IF in_num_begin > in_num_end THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  
  -- del 不作检查，由应用保正
      
  SELECT count(*) INTO is_exist FROM ip_range WHERE netplane_id = netplaneid 
                AND ip_begin_num = in_num_begin AND ip_end_num = in_num_end AND mask_num = in_num_mask 
                AND ip_begin = in_str_begin AND ip_end = in_str_end AND mask = in_str_mask;
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
      
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM ip_range WHERE netplane_id = netplaneid 
                AND ip_begin_num = in_num_begin AND ip_end_num = in_num_end AND mask_num = in_num_mask 
                AND ip_begin = in_str_begin AND ip_end = in_str_end AND mask = in_str_mask;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 查询netplane iprange 是否已经被logicnetwork使用                            
CREATE OR REPLACE FUNCTION pf_net_query_netplane_iprange_is_using(in_netplane_uuid character varying,
                              in_num_begin numeric(19), in_num_end numeric(19) )
 RETURNS t_query_total AS
$BODY$
DECLARE
  result  t_query_total;
  is_conflict integer;
  netplaneid numeric(19,0);
  is_exist integer; 
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- 检查netplane uuid 是否有效
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_netplane_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_netplane_uuid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  -- 查询logic network ip range 是否已有分配的
  SELECT count(*)  INTO  result.total_num FROM  v_net_logicnetwork_portgroup_netplane a, logic_network_ip_range b WHERE a.logicnetwork_id = b.logic_network_id 
          AND a.netplane_uuid = in_netplane_uuid AND ( in_num_begin <= b.ip_begin_num AND in_num_end >= b.ip_end_num);
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;

-- 查询iprange(通过netplane uuid)

CREATE OR REPLACE FUNCTION pf_net_query_netplane_iprange(in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    netplaneid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    -- logicnetwork uuid 
    SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
    SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF;  
    
    SELECT count(*) INTO is_exist FROM ip_range  
      WHERE netplane_id = netplaneid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF; 
  
    OPEN ref FOR SELECT in_uuid, ip_begin_num,ip_end_num, mask_num, ip_begin, ip_end, mask 
      FROM ip_range  
      WHERE netplane_id = netplaneid;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;




-- 11 netplane_vlanrange.tx



-- 判断netplane MAC RANGE 能否添加
CREATE OR REPLACE FUNCTION pf_net_check_add_netplane_vlanrange(in_uuid character varying, 
                           in_num_begin integer, in_num_end integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_SWITCH_UPLINKPORT_INVALID integer := 9;
  RTN_PORT_HAS_JOIN_OTHER_SWITCH integer := 10;
  RTN_PG_MUST_CFG_NETPLANE   integer := 13;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- begin end 
  IF in_num_begin > in_num_end THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  
  -- VLAN RANGE (2,4094)
  IF in_num_begin < 2 THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  IF in_num_end > 4094 THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  
  -- ip begin end 互相不能重叠  
/*  
  SELECT count(*) INTO is_exist FROM vlan_range WHERE  (( in_num_begin >= vlan_begin AND in_num_begin <= vlan_end ) OR 
           ( in_num_end >= vlan_begin AND in_num_end <= vlan_end ) OR 
           ( in_num_begin <= vlan_begin AND in_num_end >= vlan_end ) ) AND netplane_id = netplaneid;  
  IF is_exist > 0 THEN 
     result := RTN_ITEM_CONFLICT; 
     return result;
  END IF;  
*/
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- NETPLANE MAC RANGE添加 
-- Function: pf_net_add_netplane_vlanrange(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_netplane_vlanrange(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_netplane_vlanrange(in_uuid character varying, 
                           in_num_begin integer, in_num_end integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_add_netplane_vlanrange(in_uuid,in_num_begin,in_num_end);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- 不支持重复添加 

  -- insert item 
  result.id := nextval('serial_vlan_range');
  --result.uuid := in_uuid;
  BEGIN 
    INSERT INTO vlan_range(id,netplane_id,vlan_begin,vlan_end) 
            values(result.id,netplaneid,in_num_begin,in_num_end);
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


                 
     

-- 删除 netplane mac range
CREATE OR REPLACE FUNCTION pf_net_del_netplane_vlanrange(in_uuid character varying,
                            in_num_begin integer, in_num_end integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  netplaneid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result.ret  := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- begin end 
  IF in_num_begin > in_num_end THEN 
      result.ret  := RTN_PARAM_INVALID; 
      return result;
  END IF;
  
  -- del 不作检查，由应用保准
      
  SELECT count(*) INTO is_exist FROM vlan_range WHERE netplane_id = netplaneid 
                AND vlan_begin = in_num_begin AND vlan_end = in_num_end ;
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;                
    
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM vlan_range WHERE netplane_id = netplaneid 
                AND vlan_begin = in_num_begin AND vlan_end = in_num_end ;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



                            
/*  -- 已有定义
CREATE TYPE t_query_total  AS(
    ret integer, -- proc ret
    total_num integer  --- 
);

*/                            

-- 检查 check netplane vlan range 是否已有资源被分配 (查询)
CREATE OR REPLACE FUNCTION pf_net_query_netplane_vlanrange_alloc(in_uuid character varying,
                            in_num_begin integer, in_num_end integer)
  RETURNS t_query_total AS
$BODY$
DECLARE
  result  t_query_total;
  is_same integer;
  is_exist integer;
  netplaneid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result.ret  := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- begin end 
  IF in_num_begin > in_num_end THEN 
      result.ret  := RTN_PARAM_INVALID; 
      return result;
  END IF;
        
  SELECT count(*) INTO result.total_num FROM vlan_pool WHERE netplane_id = netplaneid 
                AND (vlan_num >= in_num_begin AND vlan_num <= in_num_end );
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;

                            

-- 查询vlanrange(通过netplane uuid)

CREATE OR REPLACE FUNCTION pf_net_query_netplane_vlanrange(in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    netplaneid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    -- logicnetwork uuid 
    SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
    SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF;  
    
    SELECT count(*) INTO is_exist FROM vlan_range  
      WHERE netplane_id = netplaneid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF; 
  
    OPEN ref FOR SELECT in_uuid, vlan_begin,vlan_end 
      FROM vlan_range  
      WHERE netplane_id = netplaneid;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


/* integer ---> char */
CREATE OR REPLACE FUNCTION pf_net_integer_to_char(in_int integer) 
RETURNS character varying AS  
$BODY$ 
DECLARE is_exist integer; 
    retchar character varying;    
    tmpchar character varying(8);
BEGIN 
    retchar := '';

    /*int32 最大值为65535 */
    IF in_int > 9999 THEN 
       retchar := to_char(in_int,'99999');
       return retchar;
    END IF;

    IF in_int > 999 THEN 
       retchar := to_char(in_int,'9999');
       return retchar;
    END IF;

    IF in_int > 99 THEN 
       retchar := to_char(in_int,'999');
       return retchar;
    END IF;

    IF in_int > 9 THEN 
       retchar := to_char(in_int,'99');
       return retchar;
    END IF;

    IF in_int > 0 THEN 
       retchar := to_char(in_int,'9');
       return retchar;
    END IF;
    
    return retchar;
END; 
$BODY$ LANGUAGE plpgsql VOLATILE
  COST 100; 


-- Function: pf_net_query_netplane_vmshare_vlans(character varying)

-- DROP FUNCTION pf_net_query_netplane_vmshare_vlans(character varying);

CREATE OR REPLACE FUNCTION pf_net_query_netplane_vmshare_vlans(in_uuid 
character varying)

  RETURNS character varying AS
$BODY$ 
DECLARE is_exist integer; 
    retchar character varying;    
    tmpchar character varying(8);
    tmpcharB character varying(8);
    tmpcharE character varying(8);
    netplaneid numeric(19,0);
    mvlanrangelists RECORD;
    i integer := 0;
    aindex integer := 0; 
    index integer := 1;
    vlanpre integer := 1;
    vlanbegin integer := 1;
BEGIN retchar := '';
    /* netplane id*/
    SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid; 
   SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid; 
   IF is_exist <= 0 THEN 
       return retchar; 
   END IF; 
   i := 1;        

   FOR mvlanrangelists IN SELECT vlan_begin,vlan_end  FROM vlan_range 
        WHERE netplane_id = netplaneid ORDER BY vlan_begin LOOP
        
       vlanbegin = mvlanrangelists.vlan_begin;
       
       /* 第一条记录 */
       IF vlanpre <= 1 THEN 
           IF vlanpre < vlanbegin THEN 
              IF vlanpre < (vlanbegin-1) THEN 
                  tmpcharB := pf_net_integer_to_char(1);
                  tmpcharE := pf_net_integer_to_char(vlanbegin-1);
                  retchar = retchar || tmpcharB || ' ~' || tmpcharE || ',' ;
              END IF;
              IF vlanpre = (vlanbegin-1) THEN 
                  tmpcharB := pf_net_integer_to_char(1);
                  retchar = retchar || tmpcharB || ',' ;
              END IF;
           END IF;
       END IF;
       IF vlanpre > 1 THEN 
           IF vlanpre < vlanbegin THEN 
              IF vlanpre < (vlanbegin-2) THEN 
                  tmpcharB := pf_net_integer_to_char(vlanpre+1);
                  tmpcharE := pf_net_integer_to_char(vlanbegin-1);
                  retchar = retchar || tmpcharB || ' ~' || tmpcharE || ',' ;
              END IF;
              IF vlanpre = (vlanbegin-2) THEN 
                  tmpcharB := pf_net_integer_to_char(vlanpre+1);
                  retchar = retchar || tmpcharB || ',' ;
              END IF;
           END IF; 
       END IF;

       vlanpre = mvlanrangelists.vlan_end;
   END LOOP;

   /*判断vlan range end 是否是4094*/
   /* 当没有配置vlan range 的时候，需考虑边界值1 */
   IF vlanpre = 1 THEN 
       vlanpre = 0;
   END IF;

   IF vlanpre < 4094 THEN 
       IF vlanpre < (4094-1) THEN 
           tmpcharB := pf_net_integer_to_char(vlanpre+1);
           tmpcharE := to_char((4094),'9999');
           retchar = retchar || tmpcharB || ' ~' || tmpcharE  ;
       END IF;
       IF vlanpre = (4094-1) THEN 
           tmpcharB := pf_net_integer_to_char(vlanpre+1);
           retchar = retchar || tmpcharB  ;
       END IF;
   END IF;
        
   /* RAISE WARNING  ' retchar --> %', retchar; */
   return retchar;
END; 
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 12 netplane_vlanpool.txt



-- 判断NETPLANE VLAN POOL 能否添加
CREATE OR REPLACE FUNCTION pf_net_check_add_netplane_vlanpool(in_uuid character varying, 
                           in_vlan_num integer )
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
  
  -- VLAN RANGE (2,4094)
  IF in_vlan_num < 2 THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  IF in_vlan_num > 4094 THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
    
  -- vlan range 必须已经配置
  SELECT count(*) INTO is_exist FROM vlan_range WHERE netplane_id = netplaneid  
       AND ( in_vlan_num >= vlan_begin AND in_vlan_num <= vlan_end ) ;  
  IF is_exist <= 0 THEN 
     result := RTN_MUST_CFG_VALID_RANGE; 
     return result;
  END IF;  
  
  -- vlan pool 不能冲突
  SELECT count(*) INTO is_exist FROM vlan_pool WHERE netplane_id = netplaneid  AND vlan_num = in_vlan_num ;
  IF is_exist > 0 THEN 
     result := RTN_ITEM_CONFLICT; 
     return result;
  END IF;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- NETPLANE VLAN POOL 添加 
-- Function: pf_net_add_netplane_vlanpool(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_netplane_vlanpool(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_netplane_vlanpool(in_uuid character varying, 
                           in_vlan_num integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_add_netplane_vlanpool(in_uuid,in_vlan_num);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
   -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
    
  -- 不支持重复添加 

  -- insert item 
  result.id := nextval('serial_vlan_pool');
  --result.uuid := in_uuid;
  BEGIN 
    INSERT INTO vlan_pool(id,netplane_id,vlan_num) 
            values(result.id,netplaneid,in_vlan_num);
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


                 


-- 判断NETPLANE VLAN POOL 能否删除
CREATE OR REPLACE FUNCTION pf_net_check_del_netplane_vlanpool(in_uuid character varying, 
                           in_vlan_num integer )
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  vlanpoolid numeric(19,0);   
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
  
  -- VLAN RANGE (2,4094)
  IF in_vlan_num < 2 THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  IF in_vlan_num > 4094 THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
    
  SELECT count(*) INTO is_exist FROM vlan_pool WHERE netplane_id = netplaneid 
                AND vlan_num = in_vlan_num;
  SELECT id INTO vlanpoolid FROM vlan_pool WHERE netplane_id = netplaneid 
                AND vlan_num = in_vlan_num;            
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;                
  
  -- 查看该资源是否在使用
  SELECT count(*) INTO is_exist FROM vlan_map WHERE vlan_id = vlanpoolid;
  IF is_exist > 0 THEN 
     result := RTN_RESOURCE_USING; 
     return result;
  END IF;
      
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;

                 

-- 删除 netplane vlan pool
CREATE OR REPLACE FUNCTION pf_net_del_netplane_vlanpool(in_uuid character varying, 
                           in_vlan_num integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  netplaneid numeric(19,0);
  vlanpoolid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- del 不作检查，由应用保准  
  result.ret := pf_net_check_del_netplane_vlanpool(in_uuid,in_vlan_num);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
   -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
  
    
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM vlan_pool WHERE netplane_id = netplaneid 
                AND vlan_num = in_vlan_num;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;




-- 查询vlan_pool(通过netplane_uuid)

CREATE OR REPLACE FUNCTION pf_net_query_netplane_vlanpool(in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    netplaneid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
     -- netplane uuid 
    SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
    SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
       --result := RTN_ITEM_NO_EXIST; 
       return ref;
    END IF;  
    
    SELECT count(*) INTO is_exist FROM vlan_pool  
      WHERE netplane_id = netplaneid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF; 
  
    OPEN ref FOR SELECT in_uuid, vlan_num 
      FROM vlan_pool  
      WHERE netplane_id = netplaneid;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 查询 netplane 下 vm_share access vlan 列表
CREATE OR REPLACE FUNCTION pf_net_query_vm_share_pg_access_list_by_netplane( in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    netplaneid numeric(19,0);
BEGIN
     -- netplane uuid 
    SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
    SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
       --result := RTN_ITEM_NO_EXIST; 
       return ref;
    END IF;   
    
    OPEN ref FOR 
    SELECT vlan_num
    FROM port_group a,pg_switchport_access_vlan b 
    WHERE a.id = b.pg_id AND a.pg_type = 20 AND a.netplane_id = netplaneid;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 查询 netplane 下 vm_share access vlan 列表
CREATE OR REPLACE FUNCTION pf_net_query_vm_share_pg_trunk_list_by_netplane( in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    netplaneid numeric(19,0);
BEGIN
     -- netplane uuid 
    SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
    SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
       --result := RTN_ITEM_NO_EXIST; 
       return ref;
    END IF;   
    
    OPEN ref FOR 
    SELECT vlan_begin, vlan_end, native_vlan_num
    FROM port_group a,pg_switchport_trunk b, pg_switchport_trunk_vlan_range c
    WHERE a.id = b.pg_id AND b.pg_id = c.pg_switchport_trunk_id AND a.pg_type = 20 AND a.netplane_id = netplaneid;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 查询 netplane 下 vm_private isolate no 列表
CREATE OR REPLACE FUNCTION pf_net_query_vm_private_pg_isolate_no_list_by_netplane( in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    netplaneid numeric(19,0);
BEGIN
     -- netplane uuid 
    SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
    SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
       --result := RTN_ITEM_NO_EXIST; 
       return ref;
    END IF;   
    
    OPEN ref FOR 
    SELECT isolate_no
    FROM port_group a,pg_switchport_access_isolate b 
    WHERE a.id = b.pg_id AND a.pg_type = 21 AND a.netplane_id = netplaneid;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 13 netplane_vlanmap.txt



-- 判断NETPLANE VLAN POOL 能否添加
CREATE OR REPLACE FUNCTION pf_net_check_add_vlanmap(in_project_id numeric(19,0),
                           in_pg_uuid character varying, 
                           in_vlan_num integer, in_isolate_no integer )
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  isolateno integer; 
  switchportmode  integer;
  pgid numeric(19,0);
  vlanid numeric(19,0);
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE integer := 13;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- pg uuid  
  -- vm pg, 必须是vm_pg private 
  SELECT count(*) INTO is_exist FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_pg_uuid AND a.pg_type = 21;
  SELECT a.id INTO pgid FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_pg_uuid AND a.pg_type = 21;
  SELECT a.switchport_mode INTO switchportmode FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_pg_uuid AND a.pg_type = 21;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
  
  -- pg uuid switchport_mode 必须是access 
  IF switchportmode = 1 THEN 
     result := 0;
  ELSE 
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;
    
  -- 查看隔离域是否一致 
  SELECT count(*) INTO is_exist FROM pg_switchport_access_isolate WHERE pg_id = pgid; 
  SELECT isolate_no INTO isolateno FROM pg_switchport_access_isolate WHERE pg_id = pgid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
  
  IF isolateno = in_isolate_no THEN 
     result := 0;
  ELSE 
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;
  
  -- VLAN RANGE (2,4094)
  IF in_vlan_num < 2 THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  IF in_vlan_num > 4094 THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
          
  -- 通过pd 获取pg的netplane id
  SELECT count(*) into is_exist FROM v_net_netplane_vmpg WHERE pg_uuid = in_pg_uuid;
  SELECT netplane_id into netplaneid FROM v_net_netplane_vmpg WHERE pg_uuid = in_pg_uuid;  
  IF is_exist <= 0 THEN 
     result := RTN_PG_MUST_CFG_NETPLANE; 
     return result;
  END IF;
      
  -- vlan pool 
  SELECT count(*) INTO is_exist FROM vlan_pool WHERE netplane_id = netplaneid  AND vlan_num = in_vlan_num ;
  SELECT id INTO vlanid FROM vlan_pool WHERE netplane_id = netplaneid  AND vlan_num = in_vlan_num ;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
    
  -- vlan map 是否已存在project_id,pg_id, isolate_no 
  SELECT count(*) INTO is_exist FROM vlan_map WHERE project_id = in_project_id AND vm_pg_id = pgid AND isolate_no = in_isolate_no;
  IF is_exist > 0 THEN 
      SELECT count(*) INTO is_exist FROM vlan_map WHERE project_id = in_project_id AND vm_pg_id = pgid AND isolate_no = in_isolate_no AND vlan_id = vlanid;
      IF is_exist > 0 THEN 
          result := RTN_HAS_SAME_ITEM; 
          return result;
      ELSE          
          result := RTN_ITEM_CONFLICT; 
          return result;
      END IF;
  END IF;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- VLAN MAP 添加 
-- Function: pf_net_add_vlanmap(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_vlanmap(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_vlanmap(in_project_id numeric(19,0),
                           in_pg_uuid character varying, 
                           in_vlan_num integer,in_isolate_no integer )
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  pgid numeric(19,0);
  vlanid numeric(19,0);
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_add_vlanmap(in_project_id,in_pg_uuid,in_vlan_num,in_isolate_no);  
  -- 重复添加 直接返回
  IF result.ret = RTN_HAS_SAME_ITEM THEN      
     result.ret := 0;
     return result;
  END IF;
  IF result.ret > 0 THEN      
     return result;
  END IF;
  
  -- pg uuid  
  SELECT count(*) INTO is_exist FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_pg_uuid;
  SELECT a.id INTO pgid FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_pg_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
  
  -- 通过pd 获取pg的netplane id
  SELECT count(*) into is_exist FROM v_net_netplane_vmpg WHERE pg_uuid = in_pg_uuid;
  SELECT netplane_id into netplaneid FROM v_net_netplane_vmpg WHERE pg_uuid = in_pg_uuid;  
  IF is_exist <= 0 THEN 
     result := RTN_PG_MUST_CFG_NETPLANE; 
     return result;
  END IF;
  
  -- vlan pool 
  SELECT count(*) INTO is_exist FROM vlan_pool WHERE netplane_id = netplaneid  AND vlan_num = in_vlan_num ;
  SELECT id INTO vlanid FROM vlan_pool WHERE netplane_id = netplaneid  AND vlan_num = in_vlan_num ;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
  

  -- insert item 
  result.id := nextval('serial_vlan_map');
  --result.uuid := in_uuid;
  BEGIN 
    INSERT INTO vlan_map(id,project_id,vm_pg_id,vlan_id,isolate_no) 
            values(result.id,in_project_id,pgid,vlanid,in_isolate_no);
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


                 


-- 判断NETPLANE VLAN POOL 能否删除
CREATE OR REPLACE FUNCTION pf_net_check_del_vlanmap(in_project_id numeric(19,0),
                           in_pg_uuid character varying, 
                           in_vlan_num integer,in_isolate_no integer  )
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  isolateno integer; 
  switchportmode integer; 
  pgid numeric(19,0);
  vlanid numeric(19,0);
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE integer := 13;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- pg uuid  
  -- vm pg, 必须是vm_pg private 
  SELECT count(*) INTO is_exist FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_pg_uuid AND a.pg_type = 21;
  SELECT a.id INTO pgid FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_pg_uuid AND a.pg_type = 21;
  SELECT a.switchport_mode INTO switchportmode FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_pg_uuid AND a.pg_type = 21;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
  
  -- pg uuid switchport_mode 必须是access 
  IF switchportmode = 1 THEN 
     result := 0;
  ELSE 
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;
  
  -- 查看隔离域是否一致 
  SELECT count(*) INTO is_exist FROM pg_switchport_access_isolate WHERE pg_id = pgid; 
  SELECT isolate_no INTO isolateno FROM pg_switchport_access_isolate WHERE pg_id = pgid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
  
  IF isolateno = in_isolate_no THEN 
     result := 0;
  ELSE 
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;
  
  -- VLAN RANGE (2,4094)
  IF in_vlan_num < 2 THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  IF in_vlan_num > 4094 THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
          
  -- 通过pd 获取pg的netplane id
  SELECT count(*) into is_exist FROM v_net_netplane_vmpg WHERE pg_uuid = in_pg_uuid;
  SELECT netplane_id into netplaneid FROM v_net_netplane_vmpg WHERE pg_uuid = in_pg_uuid;  
  IF is_exist <= 0 THEN 
     result := RTN_PG_MUST_CFG_NETPLANE; 
     return result;
  END IF;
  
      
  -- vlan pool 
  SELECT count(*) INTO is_exist FROM vlan_pool WHERE netplane_id = netplaneid  AND vlan_num = in_vlan_num ;
  SELECT id INTO vlanid FROM vlan_pool WHERE netplane_id = netplaneid  AND vlan_num = in_vlan_num ;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
    
  -- vlan map 是否已存在project_id,pg_id 条目  
  SELECT count(*) INTO is_exist FROM vlan_map WHERE project_id = in_project_id  
                AND vm_pg_id = pgid AND vlan_id = vlanid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
  
  -- 判断project id, vm_pg_id 是否还在使用  (vnic, vsi_profile)
  SELECT count(*) INTO is_exist FROM  vnic a, vsi_profile b, vm c WHERE 
          c.vm_id = a.vm_id AND a.vsi_profile_id = b.id 
         AND c.project_id = in_project_id AND vm_pg_id = pgid;
  IF is_exist > 0 THEN 
      result := RTN_RESOURCE_USING; 
      return result;
  END IF;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
                
                 

-- 删除 netplane vlan map
CREATE OR REPLACE FUNCTION pf_net_del_vlanmap(in_project_id numeric(19,0),
                           in_pg_uuid character varying, 
                           in_vlan_num integer ,in_isolate_no integer )
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  pgid numeric(19,0);
  vlanid numeric(19,0);
  netplaneid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- check del 
  result.ret := pf_net_check_del_vlanmap(in_project_id,in_pg_uuid,in_vlan_num,in_isolate_no);  
  IF result.ret > 0 THEN      
     return result;
  END IF;
  
  -- pg uuid  
  SELECT count(*) INTO is_exist FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_pg_uuid;
  SELECT a.id INTO pgid FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_pg_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
  
  -- 通过pd 获取pg的netplane id
  SELECT count(*) into is_exist FROM v_net_netplane_vmpg WHERE pg_uuid = in_pg_uuid;
  SELECT netplane_id into netplaneid FROM v_net_netplane_vmpg WHERE pg_uuid = in_pg_uuid;  
  IF is_exist <= 0 THEN 
     result := RTN_PG_MUST_CFG_NETPLANE; 
     return result;
  END IF;
  
  -- vlan pool 
  SELECT count(*) INTO is_exist FROM vlan_pool WHERE netplane_id = netplaneid  AND vlan_num = in_vlan_num ;
  SELECT id INTO vlanid FROM vlan_pool WHERE netplane_id = netplaneid  AND vlan_num = in_vlan_num ;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
  
    
  SELECT count(*) INTO is_exist FROM vlan_map WHERE project_id = in_project_id  
                AND vm_pg_id = pgid AND vlan_id = vlanid;
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;                
  
    
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM vlan_map WHERE project_id = in_project_id  
                AND vm_pg_id = pgid AND vlan_id = vlanid;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;




-- 查询vlan_map(通过netplane_uuid)

CREATE OR REPLACE FUNCTION pf_net_query_vlanmap_by_projectid(in_project_id numeric(19))
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    netplaneid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN  
    OPEN ref FOR SELECT netplane_uuid,netplane_name,pg_uuid,pg_name,project_id, vlan_num,isolate_no
       FROM v_net_vlanmap_netplane_pg_vlan   
       WHERE project_id = in_project_id;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 查询vlan_map(通过 vm pg uuid )
CREATE OR REPLACE FUNCTION pf_net_query_vlanmap_by_portgroup(in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    netplaneid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    
    OPEN ref FOR SELECT netplane_uuid,netplane_name,pg_uuid,pg_name,project_id, vlan_num ,isolate_no
       FROM v_net_vlanmap_netplane_pg_vlan   
       WHERE pg_uuid = in_uuid;
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 查询vlan_map(通过 vm pg uuid, project  )
CREATE OR REPLACE FUNCTION pf_net_query_vlanmap_by_projectid_and_pg(in_project_id numeric(19), in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    
    OPEN ref FOR SELECT netplane_uuid,netplane_name,pg_uuid,pg_name,project_id, vlan_num ,isolate_no
       FROM v_net_vlanmap_netplane_pg_vlan   
       WHERE project_id = in_project_id AND pg_uuid = in_uuid;
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 查询vlan_map(通过 project, isolate_no )
CREATE OR REPLACE FUNCTION pf_net_query_vlanmap_by_projectid_and_isolate(in_project_id numeric(19), in_isolate_no integer)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    
    OPEN ref FOR SELECT netplane_uuid,netplane_name,pg_uuid,pg_name,project_id, vlan_num ,isolate_no
       FROM v_net_vlanmap_netplane_pg_vlan   
       WHERE project_id = in_project_id AND isolate_no = in_isolate_no;
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 查询vlan_map(通过 netplane uuid )
CREATE OR REPLACE FUNCTION pf_net_query_vlanmap_by_netplane( in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    
    OPEN ref FOR SELECT netplane_uuid,netplane_name,pg_uuid,pg_name,project_id, vlan_num ,isolate_no
       FROM v_net_vlanmap_netplane_pg_vlan   
       WHERE  netplane_uuid = in_uuid;
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 查询vlan_map(通过 netplane uuid, project id, isolate no )
CREATE OR REPLACE FUNCTION pf_net_query_vlanmap_by_netplane_and_project_and_isolate( in_np_uuid character varying,in_project_id numeric(19), in_isolate_no integer)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    
    OPEN ref FOR SELECT netplane_uuid,netplane_name,pg_uuid,pg_name,project_id, vlan_num ,isolate_no
       FROM v_net_vlanmap_netplane_pg_vlan   
       WHERE  netplane_uuid = in_np_uuid AND 
	         project_id = in_project_id AND 
			 isolate_no = in_isolate_no;
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


                                 
                                 
-- 14 port.txt


-- 网络平面添加 
-- Function: pf_net_add_port(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_port(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_port(in_vna_uuid character varying, in_type integer,in_name character varying,
        in_uuid character varying, in_state integer, in_is_broadcast integer,in_is_running integer,in_is_multicast integer,
        in_promiscuous integer, in_mtu integer, in_is_master integer, in_admin_state integer, in_is_online integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_conflict integer;
  vnaid numeric(19,0);
  is_exist integer; 
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- 检查vna uuid 是否有效
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
    
  -- 是否已存在相同记录
  SELECT count(*) INTO is_same FROM port WHERE port_type = in_type AND vna_id = vnaid  AND name = in_name AND state = in_state 
         AND is_broadcast = in_is_broadcast AND is_running = in_is_running AND is_multicast = in_is_multicast AND promiscuous = in_promiscuous 
         AND mtu = in_mtu AND is_master = in_is_master AND admin_state = in_admin_state AND is_online = in_is_online;  
  IF is_same > 0 THEN
     result.ret := RTN_HAS_SAME_ITEM;   -- has same item
     SELECT id INTO result.id FROM port WHERE port_type = in_type AND vna_id = vnaid  AND name = in_name ;
     SELECT uuid INTO result.uuid FROM port WHERE port_type = in_type AND vna_id = vnaid  AND name = in_name ;
     return result;
  END IF;

  -- port name 名称是否冲突
  SELECT count(*) INTO is_conflict FROM  port WHERE vna_id = vnaid  AND name = in_name ;
  IF is_conflict > 0 THEN
     result.ret := RTN_ITEM_CONFLICT; -- conflict 
     return result;
  END IF;

  -- insert item
  result.id := nextval('serial_port');
  result.uuid := in_uuid;
  BEGIN 
    INSERT INTO port values(result.id,in_type,vnaid,in_name,in_uuid,in_state,in_is_broadcast,in_is_running,in_is_multicast,in_promiscuous,in_mtu,in_is_master,in_admin_state,in_is_online);
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- port修改
-- Function: pf_net_modify_port(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_modify_port(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_modify_port(in_vna_uuid character varying, in_type integer,in_name character varying,
        in_uuid character varying, in_state integer, in_is_broadcast integer,in_is_running integer,in_is_multicast integer,
        in_promiscuous integer, in_mtu integer, in_is_master integer, in_admin_state integer, in_is_online integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  vnaid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG  integer := 5;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  -- 检查vna uuid 是否有效
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  -- 检查port uuid 是否有效
  SELECT count(*) INTO is_exist FROM port WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;    
  
  -- 是否已存在相同记录
  SELECT count(*) INTO is_same FROM port WHERE uuid = in_uuid AND port_type = in_type AND vna_id = vnaid  AND name = in_name AND state = in_state 
         AND is_broadcast = in_is_broadcast AND is_running = in_is_running AND is_multicast = in_is_multicast AND promiscuous = in_promiscuous 
         AND mtu = in_mtu AND is_master = in_is_master AND admin_state = in_admin_state AND is_online = in_is_online;  
  IF is_same > 0 THEN
     result.ret := RTN_HAS_SAME_ITEM;   -- has same item
     result.uuid := in_uuid;
     SELECT id INTO result.id FROM port WHERE uuid = in_uuid; 
     return result;
  END IF;
    
  -- port vnaid name
  SELECT count(*) INTO is_exist FROM port WHERE vna_id = vnaid AND name = in_name AND uuid = in_uuid;
  IF is_exist <= 0 THEN
     SELECT count(*) INTO is_exist FROM port WHERE vna_id = vnaid AND name = in_name ;
     IF is_exist > 0 THEN
       result.ret := RTN_ITEM_CONFLICT;-- 
       return result;          
     END IF;
  END IF;
   
  -- modify item
  -- 修改port type时，必须先删除后添加
  BEGIN 
    SELECT id INTO result.id FROM port WHERE uuid = in_uuid;
    result.uuid := in_uuid;
    UPDATE port SET port_type = in_type, name = in_name, state = in_state, is_broadcast = in_is_broadcast,is_running = in_is_running,is_multicast = in_is_multicast, 
            promiscuous = in_promiscuous , mtu = in_mtu, is_master = in_is_master, admin_state = in_admin_state,is_online = in_is_online WHERE uuid = in_uuid ;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 删除port
-- Function: pf_net_del_port(character varying)

-- DROP FUNCTION pf_net_del_port(character varying);

CREATE OR REPLACE FUNCTION pf_net_del_port(netplane_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否存在记录
  SELECT count(*) INTO is_exist FROM port WHERE uuid = $1 ;  
  IF is_exist <= 0 THEN
     result.ret := RTN_ITEM_NO_EXIST;-- does not exist item
     return result;
  END IF;

  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM port WHERE uuid = $1 ;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



--- 查询port summary
--- 输出 name,uuid
-- Function: pf_net_query_port_summary()

-- DROP FUNCTION pf_net_query_port_summary();

CREATE OR REPLACE FUNCTION pf_net_query_port_summary(in_vna_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
        ref refcursor;
BEGIN
       OPEN ref FOR SELECT a.name,a.uuid, a.port_type FROM port a, vna b WHERE a.vna_id = b.id AND b.vna_uuid = in_vna_uuid ;
       RETURN ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 查询port 
-- 输出 uuid,name,description,mtu
-- Function: pf_net_query_port(character varying)

-- DROP FUNCTION pf_net_query_port(character varying);

CREATE OR REPLACE FUNCTION pf_net_query_port(in_port_uuid character varying)
  RETURNS t_port_query AS
$BODY$
DECLARE
  result  t_port_query;
  is_exist integer;
  RTN_ITEM_NO_EXIST integer :=4;
BEGIN
--  SELECT uuid into result.uuid, name into result.name,description into result.description,mtu into result.mtu FROM netplane WHERE uuid = $1;
  result.ret := 0;
  SELECT count(*) INTO is_exist FROM port a, vna b WHERE a.vna_id = b.id AND a.uuid = in_port_uuid;
  SELECT b.vna_uuid INTO result.vna_uuid FROM port a, vna b WHERE a.vna_id = b.id AND a.uuid = in_port_uuid;
  IF is_exist > 0 THEN
     result.ret := 0;
  ELSE
     result.ret := RTN_ITEM_NO_EXIST; -- no exist item
     return result;     
  END IF;
   
   result.uuid := in_port_uuid;
   SELECT port_type into result.port_type  FROM port WHERE uuid = in_port_uuid;
   SELECT name into result.name  FROM port WHERE uuid = in_port_uuid;
   SELECT state into result.state FROM port WHERE uuid = in_port_uuid;
   SELECT is_broadcast into result.is_broadcast  FROM port WHERE uuid = in_port_uuid;
   SELECT is_running into result.is_running  FROM port WHERE uuid = in_port_uuid;
   SELECT is_multicast into result.is_multicast  FROM port WHERE uuid = in_port_uuid;
   SELECT promiscuous into result.promiscuous  FROM port WHERE uuid = in_port_uuid;
   SELECT mtu into result.mtu  FROM port WHERE uuid = in_port_uuid;
   SELECT is_master into result.is_master  FROM port WHERE uuid = in_port_uuid;
   SELECT admin_state into result.admin_state  FROM port WHERE uuid = in_port_uuid;
   SELECT is_online into result.is_online  FROM port WHERE uuid = in_port_uuid;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 查询port summary by vna_uuid, port_name
-- 输出 uuid,name,port_type
CREATE OR REPLACE FUNCTION pf_net_query_port_summary_by_vna_portname(in_vna_uuid character varying,in_port_name character varying)
  RETURNS t_port_summary_query AS
$BODY$
DECLARE
  result  t_port_summary_query;
  is_exist integer;
  vnaid numeric(19,0);
  portid numeric(19,0);
  RTN_ITEM_NO_EXIST integer :=4;
BEGIN
   result.ret := 0;
   SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
   SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
   IF is_exist > 0 THEN
      result.ret := 0;
   ELSE
      result.ret := RTN_ITEM_NO_EXIST; -- no exist item
      return result;     
   END IF;
   
   SELECT count(*) INTO is_exist FROM port WHERE vna_id = vnaid AND name = in_port_name;
   SELECT id INTO portid FROM port WHERE vna_id = vnaid AND name = in_port_name;
   IF is_exist > 0 THEN
      result.ret := 0;
   ELSE
      result.ret := RTN_ITEM_NO_EXIST; -- no exist item
      return result;     
   END IF;
   
   result.vna_uuid := in_vna_uuid;
   result.name := in_port_name;
   SELECT uuid INTO  result.uuid  FROM port WHERE id = portid;
   SELECT port_type INTO  result.port_type  FROM port WHERE id = portid;
   
   return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



CREATE OR REPLACE FUNCTION pf_net_check_portname_is_conflict(in_vna_uuid character varying, in_port_name character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  vnaid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
BEGIN
  result := 0;
  -- 入参检查 后面做
          
  --查询是否存在
  SELECT count(*) INTO is_exist FROM port a, vna b WHERE a.name = in_port_name AND a.vna_id = b.id AND b.vna_uuid = in_vna_uuid;
  IF is_exist > 0 THEN 
      result := RTN_ITEM_CONFLICT; 
      return result;
  END IF;

  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 15 phy_port.txt



-- physical port 添加 
-- Function: pf_net_add_phyport(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_phyport(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_phyport(in_vna_uuid character varying, in_type integer,in_name character varying,
        in_uuid character varying, in_state integer, in_is_broadcast integer,in_is_running integer,in_is_multicast integer,
        in_promiscuous integer, in_mtu integer, in_is_master integer, in_admin_state integer, in_is_online integer,
        in_is_port_cfg integer, in_is_consistency integer, 
        in_supported_ports character varying, in_supported_link_modes character varying,in_is_support_auto_negotiation integer,
        in_advertised_link_modes character varying,in_advertised_pause_frame_use character varying,in_is_advertised_auto_negotiation integer,
        in_speed character varying,in_duplex character varying,in_port character varying,in_phyad character varying,in_transceiver character varying,
        in_auto_negotiate character varying,in_supports_wake_on character varying,in_wake_on character varying,in_current_msg_level character varying,
        in_is_linked integer,
        in_is_rx_checksumming integer,in_is_tx_checksumming integer, in_is_scatter_gather integer,  in_is_tcp_segmentation_offload integer,
        in_is_udp_fragmentation_offload integer,in_is_generic_segmentation_offload integer,in_is_generic_receive_offload integer, in_is_large_receive_offload integer,
        in_is_sriov integer, in_is_loop_report integer, in_is_loop_cfg integer )
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_same_port integer;
  is_same_port_ex integer;
  is_same_port_sriov integer;
  is_conflict integer;
  vnaid numeric(19,0);
  portid numeric(19,0);
  is_exist integer; 
  db_uuid character varying;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- 检查vna uuid 是否有效
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  -- 是否已存在相同记录

  SELECT count(*) INTO is_same_port FROM port WHERE port_type = in_type AND vna_id = vnaid  AND name = in_name AND state = in_state 
         AND is_broadcast = in_is_broadcast AND is_running = in_is_running AND is_multicast = in_is_multicast AND promiscuous = in_promiscuous 
         AND mtu = in_mtu AND is_master = in_is_master AND admin_state = in_admin_state AND is_online = in_is_online 
         AND is_cfg = in_is_port_cfg AND is_consistency = in_is_consistency ;
  SELECT id INTO portid FROM port WHERE port_type = in_type AND vna_id = vnaid  AND name = in_name AND state = in_state 
         AND is_broadcast = in_is_broadcast AND is_running = in_is_running AND is_multicast = in_is_multicast AND promiscuous = in_promiscuous 
         AND mtu = in_mtu AND is_master = in_is_master AND admin_state = in_admin_state AND is_online = in_is_online 
         AND is_cfg = in_is_port_cfg AND is_consistency = in_is_consistency ;
  SELECT uuid INTO db_uuid FROM port WHERE port_type = in_type AND vna_id = vnaid  AND name = in_name AND state = in_state 
         AND is_broadcast = in_is_broadcast AND is_running = in_is_running AND is_multicast = in_is_multicast AND promiscuous = in_promiscuous 
         AND mtu = in_mtu AND is_master = in_is_master AND admin_state = in_admin_state AND is_online = in_is_online 
         AND is_cfg = in_is_port_cfg AND is_consistency = in_is_consistency ;         
  IF is_same_port > 0 THEN 
     SELECT count(*) INTO is_same FROM physical_port WHERE port_id = portid AND  port_type = in_type AND vna_id = vnaid  AND supported_ports = in_supported_ports
         AND supported_link_modes = in_supported_link_modes AND  is_support_auto_negotiation = in_is_support_auto_negotiation
         AND advertised_link_modes = in_advertised_link_modes AND advertised_pause_frame_use = in_advertised_pause_frame_use 
         AND is_advertised_auto_negotiation = in_is_advertised_auto_negotiation AND speed = in_speed AND duplex = in_duplex 
         AND port = in_port AND phyad = in_phyad AND transceiver = in_transceiver AND auto_negotiate = in_auto_negotiate 
         AND supports_wake_on = in_supports_wake_on AND wake_on = in_wake_on AND current_msg_level = in_current_msg_level 
         AND is_linked = in_is_linked; 
     IF is_same > 0 THEN
         SELECT count(*) INTO is_same_port_ex FROM physical_port_extend WHERE physical_port_id = portid  
             AND is_rx_checksumming = in_is_rx_checksumming AND is_tx_checksumming = in_is_tx_checksumming AND is_scatter_gather = in_is_scatter_gather 
             AND is_tcp_segmentation_offload = in_is_tcp_segmentation_offload AND is_udp_fragmentation_offload = in_is_udp_fragmentation_offload 
             AND is_generic_segmentation_offload = in_is_generic_segmentation_offload AND is_generic_receive_offload = in_is_generic_receive_offload 
             AND is_large_receive_offload = in_is_large_receive_offload; 
         IF is_same_port_ex > 0 THEN                     
           IF in_is_sriov > 0 THEN      
              SELECT count(*) INTO is_same_port_sriov FROM  sriov_port WHERE physical_port_id = portid AND vna_id = vnaid 
                 AND is_loop_report = in_is_loop_report AND is_loop_cfg = in_is_loop_cfg; 
              IF is_same_port_sriov > 0 THEN     
                 result.ret := RTN_HAS_SAME_ITEM;   -- has same item
                 result.id  := portid;
                 result.uuid := db_uuid;
                 return result;
              ELSE 
                 result.ret := RTN_ITEM_CONFLICT; -- conflict 
                 return result;
              END IF;
           ELSE 
              SELECT count(*) INTO is_exist FROM  sriov_port WHERE physical_port_id = portid AND vna_id = vnaid ;
              IF is_exist > 0 THEN 
                 result.ret := RTN_ITEM_CONFLICT; -- conflict 
                 return result;
              END IF;
           END IF;
         ELSE
           result.ret := RTN_ITEM_CONFLICT; -- conflict 
           return result;
         END IF;
     ELSE
       result.ret := RTN_ITEM_CONFLICT; -- conflict 
       return result;       
     END IF;     
  END IF;

  -- port name 名称是否冲突
  SELECT count(*) INTO is_conflict FROM  port WHERE vna_id = vnaid  AND name = in_name ;
  IF is_conflict > 0 THEN
     result.ret := RTN_ITEM_CONFLICT; -- conflict 
     return result;
  END IF;

  -- insert item
  result.id := nextval('serial_port');
  result.uuid := in_uuid;
  
  BEGIN 
    INSERT INTO port values(result.id,in_type,vnaid,in_name,in_uuid,in_state,in_is_broadcast,in_is_running,in_is_multicast,in_promiscuous,in_mtu,in_is_master,in_admin_state,in_is_online, in_is_port_cfg, in_is_consistency);
    INSERT INTO physical_port values(result.id,in_type,vnaid,in_supported_ports,in_supported_link_modes,in_is_support_auto_negotiation,
        in_advertised_link_modes,in_advertised_pause_frame_use,in_is_advertised_auto_negotiation,
        in_speed,in_duplex,in_port,in_phyad,in_transceiver,in_auto_negotiate,
        in_supports_wake_on,in_wake_on,in_current_msg_level,in_is_linked);
    INSERT INTO physical_port_extend  values(result.id, in_is_rx_checksumming, in_is_tx_checksumming, in_is_scatter_gather, in_is_tcp_segmentation_offload,
        in_is_udp_fragmentation_offload, in_is_generic_segmentation_offload, in_is_generic_receive_offload, in_is_large_receive_offload);
    IF in_is_sriov > 0 THEN
       INSERT INTO sriov_port values(result.id,vnaid,in_is_loop_report, in_is_loop_cfg);
    END IF;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



		
		
-- port修改
-- Function: pf_net_modify_phyport(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_modify_phyport(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_modify_phyport(in_vna_uuid character varying, in_type integer,in_name character varying,
        in_uuid character varying, in_state integer, in_is_broadcast integer,in_is_running integer,in_is_multicast integer,
        in_promiscuous integer, in_mtu integer, in_is_master integer, in_admin_state integer, in_is_online integer,
        in_is_port_cfg integer, in_is_consistency integer, 
        in_supported_ports character varying, in_supported_link_modes character varying,in_is_support_auto_negotiation integer,
        in_advertised_link_modes character varying,in_advertised_pause_frame_use character varying,in_is_advertised_auto_negotiation integer,
        in_speed character varying,in_duplex character varying,in_port character varying,in_phyad character varying,in_transceiver character varying,
        in_auto_negotiate character varying,in_supports_wake_on character varying,in_wake_on character varying,in_current_msg_level character varying,
        in_is_linked integer,
        in_is_rx_checksumming integer,in_is_tx_checksumming integer, in_is_scatter_gather integer,  in_is_tcp_segmentation_offload integer,
        in_is_udp_fragmentation_offload integer,in_is_generic_segmentation_offload integer,in_is_generic_receive_offload integer, in_is_large_receive_offload integer,
        in_is_sriov integer, in_is_loop_report integer, in_is_loop_cfg integer )
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  vnaid numeric(19,0);
  portid numeric(19,0);
  is_sriov_exist integer;
  oldIsConsistency integer := 0;
  oldIsCfgLoop integer := 0;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG  integer := 5;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  -- 检查vna uuid 是否有效
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  -- 检查port uuid 是否有效
  SELECT count(*) INTO is_exist FROM port WHERE uuid = in_uuid AND port_type = in_type;
  SELECT id INTO portid FROM port WHERE uuid = in_uuid AND port_type = in_type;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;   
  
  SELECT count(*) INTO is_exist FROM physical_port WHERE port_id = portid AND port_type = in_type;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
           
  -- port name 
  SELECT count(*) INTO is_exist FROM port WHERE vna_id = vnaid AND name = in_name AND uuid = in_uuid;
  SELECT is_consistency INTO oldIsConsistency FROM port WHERE vna_id = vnaid AND name = in_name AND uuid = in_uuid;  
  IF is_exist <= 0 THEN
     SELECT count(*) INTO is_exist FROM port WHERE vna_id = vnaid AND name = in_name ;
     IF is_exist > 0 THEN
       result.ret := RTN_ITEM_CONFLICT;-- 
       return result;          
     END IF;
  END IF;  
           
  -- sriov 网口，如果已经有虚拟机部署了，是否环回不能修改 
  -- db 单独提供接口，由PORT模块调用判断 
           
  -- modify item
  result.id  := portid;
  result.uuid := in_uuid;
  BEGIN 
    UPDATE port SET name = in_name, state = in_state, is_broadcast =in_is_broadcast, is_multicast = in_is_multicast,
        promiscuous = in_promiscuous, mtu = in_mtu, is_master = in_is_master, admin_state = in_admin_state, is_online = in_is_online, 
        is_cfg = in_is_port_cfg , is_consistency = in_is_consistency 
        WHERE uuid = in_uuid AND port_type = in_type;
    UPDATE physical_port SET supported_ports = in_supported_ports, supported_link_modes = in_supported_link_modes, 
        is_support_auto_negotiation = in_is_support_auto_negotiation,advertised_link_modes = in_advertised_link_modes ,
        advertised_pause_frame_use = in_advertised_pause_frame_use,is_advertised_auto_negotiation = in_is_advertised_auto_negotiation ,
        speed = in_speed,duplex = in_duplex,port = in_port , phyad = in_phyad,transceiver = in_transceiver,auto_negotiate = in_auto_negotiate,
        supports_wake_on = in_supports_wake_on,wake_on = in_wake_on ,current_msg_level = in_current_msg_level,is_linked = in_is_linked 
        WHERE port_id = portid AND vna_id = vnaid AND port_type = in_type;    
    UPDATE physical_port_extend SET is_rx_checksumming = in_is_rx_checksumming, is_tx_checksumming = in_is_tx_checksumming,
        is_scatter_gather = in_is_scatter_gather, is_tcp_segmentation_offload = in_is_tcp_segmentation_offload,
        is_udp_fragmentation_offload = in_is_udp_fragmentation_offload, is_generic_segmentation_offload = in_is_generic_segmentation_offload, 
        is_generic_receive_offload = in_is_generic_receive_offload, is_large_receive_offload = in_is_large_receive_offload
        WHERE physical_port_id = portid;
    
    SELECT count(*) INTO is_sriov_exist FROM sriov_port WHERE physical_port_id = portid AND vna_id = vnaid;    
    IF is_sriov_exist > 0 THEN 
       IF in_is_sriov > 0 THEN 
          UPDATE sriov_port SET is_loop_report = in_is_loop_report, is_loop_cfg = in_is_loop_cfg WHERE physical_port_id = portid AND vna_id = vnaid;    
       ELSE
          DELETE FROM sriov_port WHERE physical_port_id = portid;    
       END IF;
    ELSE 
       IF in_is_sriov > 0 THEN
          INSERT INTO sriov_port values(portid,vnaid,in_is_loop_report, in_is_loop_cfg);
       END IF;
    END IF;
    
    -- trigger 
    -- 1 从sriov->sriov 
    ---  1.1 consistency change, 根据old状态和new状态 来决定调用pf_net_tr_modify_sriovport(portid,) 入参是0还是1
    -- 2 从非sriov->非sriov ， 无需操作
    -- 3 sriov->非sriov
    --   3.1 如果oldconsistency = 1, 则需要调用 pf_net_tr_sriovport_change_phyport(portid) ， 同时将
    -- 4 非sriov->sriov 
    --     
    IF is_sriov_exist > 0 THEN 
       IF in_is_sriov > 0 THEN 
          IF oldIsConsistency > in_is_consistency THEN 
              result.ret := pf_net_tr_modify_sriovport(portid, 0);
          END IF;
          IF oldIsConsistency < in_is_consistency THEN 
              result.ret := pf_net_tr_modify_sriovport(portid, 1);
             END IF;
       ELSE  -- sriov --> 非SRIOV，仅仅改变free_sriov_num 效果和is_consistency 变化一样
           IF oldIsConsistency  > 0 THEN 
              result.ret := pf_net_tr_sriovport_change_phyport(portid);
           END IF;
       END IF;
    ELSE 
       IF in_is_sriov > 0 THEN
          
       END IF;
    END IF;
            
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 删除port

CREATE OR REPLACE FUNCTION pf_net_del_phyport(in_port_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否存在记录
  SELECT count(*) INTO is_exist FROM port a, physical_port b WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.uuid = in_port_uuid ;  
  IF is_exist <= 0 THEN
     result.ret := RTN_ITEM_NO_EXIST;-- does not exist item
     return result;
  END IF;

  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM port WHERE uuid = in_port_uuid ;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


--- 查询phy port summary
--- 输出 name,uuid
-- Function: pf_net_query_phyport_summary()

-- DROP FUNCTION pf_net_query_phyport_summary();

CREATE OR REPLACE FUNCTION pf_net_query_phyport_summary(in_vna_uuid character varying, in_is_sriov integer)
  RETURNS refcursor AS
$BODY$
DECLARE
        ref refcursor;
BEGIN
      IF in_is_sriov > 0 THEN 
          OPEN ref FOR SELECT b.vna_uuid, a.name, a.uuid , a.port_type FROM port a, vna b, physical_port c, sriov_port d WHERE a.vna_id = b.id AND b.vna_uuid = in_vna_uuid 
                AND c.port_id = a.id AND c.port_type = a.port_type AND c.vna_id = a.vna_id 
                AND d.physical_port_id = c.port_id AND d.vna_id = c.vna_id ;
      ELSE
          OPEN ref FOR SELECT b.vna_uuid, a.name, a.uuid , a.port_type  FROM port a, vna b, physical_port c WHERE a.vna_id = b.id AND b.vna_uuid = in_vna_uuid 
                AND c.port_id = a.id AND c.port_type = a.port_type AND c.vna_id = a.vna_id                      
                AND a.id not in (select physical_port_id from sriov_port);
      END IF;
      return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 查询port 
-- 输出 uuid,name,description,mtu
-- Function: pf_net_query_phyport(character varying)

-- DROP FUNCTION pf_net_query_phyport(character varying);

CREATE OR REPLACE FUNCTION pf_net_query_phyport(in_port_uuid character varying)
  RETURNS t_phyport_query AS
$BODY$
DECLARE
  result  t_phyport_query;
  is_exist integer;
  portid numeric(19,0);
  RTN_ITEM_NO_EXIST integer :=4;
BEGIN
--  SELECT uuid into result.uuid, name into result.name,description into result.description,mtu into result.mtu FROM netplane WHERE uuid = $1;
  result.ret := 0;
  SELECT count(*) INTO is_exist FROM port a, vna b, physical_port c WHERE a.vna_id = b.id AND a.uuid = in_port_uuid AND
             c.port_id = a.id AND c.port_type = a.port_type AND c.vna_id = a.vna_id;
  SELECT b.vna_uuid INTO result.vna_uuid FROM port a, vna b WHERE a.vna_id = b.id AND a.uuid = in_port_uuid;
  SELECT b.port_id INTO portid FROM port a, physical_port b WHERE a.id = b.port_id AND a.uuid = in_port_uuid;
  IF is_exist > 0 THEN
     result.ret := 0;
  ELSE
     result.ret := RTN_ITEM_NO_EXIST; -- no exist item
     return result;     
  END IF;
   
   result.uuid := in_port_uuid;
   -- port 
   SELECT port_type into result.port_type  FROM port WHERE uuid = in_port_uuid;
   SELECT name into result.name  FROM port WHERE uuid = in_port_uuid;
   SELECT state into result.state FROM port WHERE uuid = in_port_uuid;
   SELECT is_broadcast into result.is_broadcast  FROM port WHERE uuid = in_port_uuid;
   SELECT is_running into result.is_running  FROM port WHERE uuid = in_port_uuid;
   SELECT is_multicast into result.is_multicast  FROM port WHERE uuid = in_port_uuid;
   SELECT promiscuous into result.promiscuous  FROM port WHERE uuid = in_port_uuid;
   SELECT mtu into result.mtu  FROM port WHERE uuid = in_port_uuid;
   SELECT is_master into result.is_master  FROM port WHERE uuid = in_port_uuid;
   SELECT admin_state into result.admin_state  FROM port WHERE uuid = in_port_uuid;
   SELECT is_online into result.is_online  FROM port WHERE uuid = in_port_uuid;
   SELECT is_cfg into result.is_port_cfg  FROM port WHERE uuid = in_port_uuid;
   SELECT is_consistency into result.is_consistency  FROM port WHERE uuid = in_port_uuid;
       
   -- physical port 
   SELECT port_type into result.port_type  FROM physical_port WHERE port_id = portid;
   SELECT supported_ports into result.supported_ports  FROM physical_port WHERE port_id = portid;
   SELECT supported_link_modes into result.supported_link_modes FROM physical_port WHERE port_id = portid;
   SELECT is_support_auto_negotiation into result.is_support_auto_negotiation  FROM physical_port WHERE port_id = portid;
   SELECT advertised_link_modes into result.advertised_link_modes  FROM physical_port WHERE port_id = portid;
   SELECT advertised_pause_frame_use into result.advertised_pause_frame_use  FROM physical_port WHERE port_id = portid;
   SELECT is_advertised_auto_negotiation into result.is_advertised_auto_negotiation  FROM physical_port WHERE port_id = portid;
   SELECT speed into result.speed  FROM physical_port WHERE port_id = portid;
   SELECT duplex into result.duplex  FROM physical_port WHERE port_id = portid;
   SELECT port into result.port  FROM physical_port WHERE port_id = portid;
   SELECT phyad into result.phyad  FROM physical_port WHERE port_id = portid;
   SELECT transceiver into result.transceiver  FROM physical_port WHERE port_id = portid;
   SELECT auto_negotiate into result.auto_negotiate  FROM physical_port WHERE port_id = portid;
   SELECT supports_wake_on into result.supports_wake_on  FROM physical_port WHERE port_id = portid;
   SELECT wake_on into result.wake_on  FROM physical_port WHERE port_id = portid;
   SELECT current_msg_level into result.current_msg_level  FROM physical_port WHERE port_id = portid;
   SELECT is_linked into result.is_linked  FROM physical_port WHERE port_id = portid;
   
   -- physical port extend 
   SELECT is_rx_checksumming into result.is_rx_checksumming  FROM physical_port_extend WHERE physical_port_id = portid;
   SELECT is_tx_checksumming into result.is_tx_checksumming  FROM physical_port_extend WHERE physical_port_id = portid;
   SELECT is_scatter_gather into result.is_scatter_gather  FROM physical_port_extend WHERE physical_port_id = portid;
   SELECT is_tcp_segmentation_offload into result.is_tcp_segmentation_offload  FROM physical_port_extend WHERE physical_port_id = portid;
   SELECT is_udp_fragmentation_offload into result.is_udp_fragmentation_offload  FROM physical_port_extend WHERE physical_port_id = portid;
   SELECT is_generic_segmentation_offload into result.is_generic_segmentation_offload  FROM physical_port_extend WHERE physical_port_id = portid;
   SELECT is_generic_receive_offload into result.is_generic_receive_offload  FROM physical_port_extend WHERE physical_port_id = portid;
   SELECT is_large_receive_offload into result.is_large_receive_offload  FROM physical_port_extend WHERE physical_port_id = portid;
   
   -- sriov
   SELECT count(*) INTO is_exist FROM sriov_port WHERE physical_port_id = portid;
   IF is_exist > 0 THEN 
      result.is_sriov := 1;
      SELECT is_loop_report INTO result.is_loop_report FROM sriov_port WHERE physical_port_id = portid;
      SELECT is_loop_cfg INTO result.is_loop_cfg FROM sriov_port WHERE physical_port_id = portid;
   ELSE
      result.is_sriov := 0;
   END IF;   
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 判断SRIOV网口下是否已经有虚拟机部署使用sriov vf了
CREATE OR REPLACE FUNCTION pf_net_query_sriovport_vsi_num(in_uuid character varying )
  RETURNS t_query_total AS
$BODY$
DECLARE
  result  t_query_total;
  is_same integer;
  is_same_port integer;
  is_same_port_ex integer;
  is_same_port_sriov integer;
  is_conflict integer;
  portid numeric(19,0);
  is_exist integer; 
  db_uuid character varying;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  
BEGIN
  result.ret := 0;
  result.total_num := 0;
  -- 入参检查 后面做
  
  -- 检查port uuid 是否有效
  SELECT count(*) INTO is_exist FROM port a, sriov_port b WHERE a.id = b.physical_port_id AND a.uuid = in_uuid;
  SELECT a.id INTO portid FROM port a, sriov_port b WHERE a.id = b.physical_port_id AND a.uuid = in_uuid;  
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist. 
    return result;
  END IF;  
  
  -- 是否已存在相同记录
  SELECT count(*) INTO result.total_num FROM sriov_vf_port a, vsi_sriov b WHERE a.port_id = b.sriov_vf_port_id AND a.sriov_port_id = portid; 
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 判断能否设置sriov 环回属性 
CREATE OR REPLACE FUNCTION pf_net_check_sriovport_loopback(in_uuid character 
varying, in_is_loop integer)

  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  portid numeric(19,0);
  bondid numeric(19,0);  
  switchid numeric(19,0);   
  vnaid numeric(19,0);
  pg_mtu integer;
  portname character varying;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
BEGIN
  result := 0;
  -- 入参检查 后面做
  
  -- 检查port uuid 是否有效
  SELECT count(*) INTO is_exist FROM port a, sriov_port b WHERE a.uuid = 
in_uuid AND a.id = b.physical_port_id;

  SELECT a.vna_id INTO vnaid FROM port a, sriov_port b WHERE a.uuid = in_uuid 
AND a.id = b.physical_port_id;  

  SELECT a.id INTO portid FROM port a, sriov_port b WHERE a.uuid = in_uuid 
AND a.id = b.physical_port_id;

  SELECT a.name INTO portname FROM port a, sriov_port b WHERE a.uuid = 
in_uuid AND a.id = b.physical_port_id;

  SELECT a.id INTO portid FROM port a, sriov_port b WHERE a.uuid = in_uuid 
AND a.id = b.physical_port_id;

  IF is_exist <= 0 THEN 
    result := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  

  -- 检查该网口下是否存在VM 使用VF
  SELECT count(*) INTO is_exist FROM sriov_vf_port a, vsi_sriov b WHERE 
  a.port_id  = b.sriov_vf_port_id AND a.sriov_port_id = portid;
  IF is_exist > 0 THEN 
      result := RTN_RESOURCE_USING;   -- vm using
      return result;
  END IF;

  /* 取消环回，没有这么多限制*/
  IF in_is_loop <= 0 THEN 
      result := 0;
      return result;
  END IF;

  -- 检查port是否加入了bond，加入bond后，不能修改其环回属性
  SELECT count(*) INTO is_exist FROM bond_map WHERE physical_port_id = portid; 
  IF is_exist > 0 THEN 
    result := RTN_PARAM_INVALID;   -- item has not exist.
    return result;
  END IF; 

  -- 如果该网口已经加入SDVS EDVS 就不能再设置为loop
 
  SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
WHERE a.virtual_switch_id = b.switch_id AND a.port_id = portid;

  IF is_exist > 0 THEN 
    result := RTN_PARAM_INVALID;   
    return result;
  END IF;
  
  /*如果该网口已经加入 wildcast loop task, 
  也将不能进行loop设置 */

  SELECT count(*) INTO is_exist 
  FROM wildcast_task_loopback a, 
  wildcast_cfg_loopback_port b 
  WHERE 
     a.wildcast_cfg_loopback_port_id = b.id AND 
     a.vna_id = vnaid AND 
     b.port_name = portname; 
  IF is_exist > 0 THEN      
     result := RTN_ITEM_CONFLICT;   
    return result;
  END IF;

  /* 如果该网口已经加入 wildcast switch task,  switch port name 存在两个地方 
仅对BOND口需限制 */

  SELECT count(*) INTO is_exist 
  FROM wildcast_task_switch a, 
  wildcast_cfg_switch_bond_map b 
  WHERE 
     a.wildcast_cfg_switch_id = b.wildcast_cfg_bond_id AND 
     a.vna_id = vnaid AND 
     b.phy_port_name = portname; 
  IF is_exist > 0 THEN      
     result := RTN_ITEM_CONFLICT;   
    return result;
  END IF;

  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 16 sriov_vf_port.txt


-- physical port 添加 
-- Function: pf_net_add_sriovvfport(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_sriovvfport(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_sriovvfport(in_vna_uuid character varying, in_type integer,in_name character varying,
        in_uuid character varying, in_state integer, in_is_broadcast integer,in_is_running integer,in_is_multicast integer,
        in_promiscuous integer, in_mtu integer, in_is_master integer, in_admin_state integer, in_is_online integer,
        in_is_port_cfg integer, in_is_consistency integer, 
        in_sriovport_uuid character varying,
        in_pci character varying, in_vlan_num integer )
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_same_port integer;
  is_same_port_ex integer;
  is_same_port_sriov integer;
  is_conflict integer;
  vnaid numeric(19,0);
  sriovportid numeric(19,0);
  portid numeric(19,0);
  is_exist integer; 
  db_uuid character varying;    
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- 检查vna uuid 是否有效
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  -- sriov port uuid 是否有效
  SELECT count(*) INTO is_exist FROM port a, physical_port b , sriov_port c WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
             AND b.port_id = c.physical_port_id AND b.vna_id = c.vna_id 
             AND a.vna_id = vnaid AND a.uuid = in_sriovport_uuid  ;
  SELECT a.id INTO sriovportid FROM port a, physical_port b, sriov_port c  WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
             AND b.port_id = c.physical_port_id AND b.vna_id = c.vna_id 
             AND a.vna_id = vnaid AND a.uuid = in_sriovport_uuid  ;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;
  
  -- 是否已存在相同记录
  SELECT count(*) INTO is_same_port FROM port WHERE port_type = in_type AND vna_id = vnaid  AND name = in_name AND state = in_state 
         AND is_broadcast = in_is_broadcast AND is_running = in_is_running AND is_multicast = in_is_multicast AND promiscuous = in_promiscuous 
         AND mtu = in_mtu AND is_master = in_is_master AND admin_state = in_admin_state AND is_online = in_is_online 
         AND is_cfg = in_is_port_cfg AND is_consistency = in_is_consistency ;
  SELECT id INTO portid FROM port WHERE port_type = in_type AND vna_id = vnaid  AND name = in_name AND state = in_state 
         AND is_broadcast = in_is_broadcast AND is_running = in_is_running AND is_multicast = in_is_multicast AND promiscuous = in_promiscuous 
         AND mtu = in_mtu AND is_master = in_is_master AND admin_state = in_admin_state AND is_online = in_is_online 
         AND is_cfg = in_is_port_cfg AND is_consistency = in_is_consistency ;
  SELECT uuid INTO db_uuid FROM port WHERE port_type = in_type AND vna_id = vnaid  AND name = in_name AND state = in_state 
         AND is_broadcast = in_is_broadcast AND is_running = in_is_running AND is_multicast = in_is_multicast AND promiscuous = in_promiscuous 
         AND mtu = in_mtu AND is_master = in_is_master AND admin_state = in_admin_state AND is_online = in_is_online 
         AND is_cfg = in_is_port_cfg AND is_consistency = in_is_consistency ;         
  IF is_same_port > 0 THEN 
     SELECT count(*) INTO is_same FROM sriov_vf_port WHERE port_id = portid AND  port_type = in_type AND vna_id = vnaid  
         AND sriov_port_id = sriovportid AND pci = in_pci AND vlan_num = in_vlan_num; 
     IF is_same > 0 THEN
            result.ret := RTN_HAS_SAME_ITEM;   -- has same item
            result.id  := portid;
            result.uuid := db_uuid;
            return result;
     ELSE
           result.ret := RTN_ITEM_CONFLICT; -- conflict 
           return result;            
     END IF;     
  END IF;

  -- port name 名称是否冲突
  SELECT count(*) INTO is_conflict FROM  port WHERE vna_id = vnaid  AND name = in_name ;
  IF is_conflict > 0 THEN
     result.ret := RTN_ITEM_CONFLICT; -- conflict 
     return result;
  END IF;
  
  -- sriov pci 是否冲突
  SELECT count(*) INTO is_conflict FROM  sriov_vf_port WHERE sriov_port_id = sriovportid  AND pci = in_pci ;
  IF is_conflict > 0 THEN
     result.ret := RTN_ITEM_CONFLICT; -- conflict 
     return result;
  END IF;
  
  -- insert item
  result.id := nextval('serial_port');
  result.uuid := in_uuid;
  
  BEGIN 
    INSERT INTO port values(result.id,in_type,vnaid,in_name,in_uuid,in_state,in_is_broadcast,in_is_running,in_is_multicast,in_promiscuous,in_mtu,in_is_master,in_admin_state,in_is_online, in_is_port_cfg, in_is_consistency);
    INSERT INTO sriov_vf_port values(result.id,in_type,vnaid,sriovportid,in_pci,in_vlan_num);    
    
    -- trigger 
    result.ret := pf_net_tr_add_sriovvfport(result.id,sriovportid);
    
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- port修改
-- Function: pf_net_modify_sriovvfport(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_modify_sriovvfport(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_modify_sriovvfport(in_vna_uuid character varying, in_type integer,in_name character varying,
        in_uuid character varying, in_state integer, in_is_broadcast integer,in_is_running integer,in_is_multicast integer,
        in_promiscuous integer, in_mtu integer, in_is_master integer, in_admin_state integer, in_is_online integer,
        in_is_port_cfg integer, in_is_consistency integer, 
        in_phyport_uuid character varying,
        in_pci character varying, in_vlan_num integer ) 
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  vnaid numeric(19,0);
  portid numeric(19,0);
  phyportid numeric(19,0);
  is_sriov_exist integer :=0;
  oldIsConsistency integer :=0;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG  integer := 5;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  -- 检查vna uuid 是否有效
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  -- 检查vf port uuid 是否有效
  SELECT count(*) INTO is_exist FROM port WHERE uuid = in_uuid AND port_type = in_type;
  SELECT id INTO portid FROM port WHERE uuid = in_uuid AND port_type = in_type;
  SELECT is_consistency INTO oldIsConsistency FROM port WHERE uuid = in_uuid AND port_type = in_type;
  
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;   
  
  -- phy port uuid 是否有效
  SELECT count(*) INTO is_exist FROM port a, physical_port b , sriov_port c WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
             AND b.port_id = c.physical_port_id AND b.vna_id = c.vna_id 
             AND a.vna_id = vnaid AND a.uuid = in_phyport_uuid  ;
  SELECT a.id INTO phyportid FROM port a, physical_port b ,sriov_port c WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
             AND b.port_id = c.physical_port_id AND b.vna_id = c.vna_id 
             AND a.vna_id = vnaid AND a.uuid = in_phyport_uuid  ;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;
  
  -- sriov vf port 表是否有效  
  SELECT count(*) INTO is_exist FROM sriov_vf_port WHERE port_id = portid AND port_type = in_type AND vna_id = vnaid 
            AND sriov_port_id = phyportid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
           
  -- pci name 
  SELECT count(*) INTO is_exist FROM sriov_vf_port WHERE port_id = portid AND port_type = in_type AND vna_id = vnaid 
            AND sriov_port_id = phyportid AND pci = in_pci;
  IF is_exist <= 0 THEN
     SELECT count(*) INTO is_exist FROM sriov_vf_port WHERE sriov_port_id = phyportid AND pci = in_pci;
     IF is_exist > 0 THEN
       result.ret := RTN_ITEM_CONFLICT;-- 
       return result;          
     END IF;
  END IF;  
  
  -- port name 
  SELECT count(*) INTO is_exist FROM port WHERE vna_id = vnaid AND name = in_name AND uuid = in_uuid;
  IF is_exist <= 0 THEN
     SELECT count(*) INTO is_exist FROM port WHERE vna_id = vnaid AND name = in_name ;
     IF is_exist > 0 THEN
       result.ret := RTN_ITEM_CONFLICT;-- 
       return result;          
     END IF;
  END IF;  
           
  -- modify item
  result.id  := portid;
  result.uuid := in_uuid;
  BEGIN 
    UPDATE port SET name = in_name, state = in_state, is_broadcast =in_is_broadcast, is_multicast = in_is_multicast,
        promiscuous = in_promiscuous, mtu = in_mtu, is_master = in_is_master, admin_state = in_admin_state, is_online = in_is_online ,
        is_cfg = in_is_port_cfg , is_consistency = in_is_consistency 
        WHERE uuid = in_uuid AND port_type = in_type;
    UPDATE sriov_vf_port SET pci = in_pci, vlan_num = in_vlan_num 
        WHERE port_id = portid AND vna_id = vnaid AND port_type = in_type;        
        
    -- trigger
    IF oldIsConsistency > in_is_consistency THEN 
        result.ret := pf_net_tr_modify_sriovvfport(portid,phyportid,0);
    END IF;    
    IF oldIsConsistency < in_is_consistency THEN 
        result.ret := pf_net_tr_modify_sriovvfport(portid,phyportid,1);
    END IF;
    
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 删除port

CREATE OR REPLACE FUNCTION pf_net_del_sriovvfport(in_port_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否存在记录
  SELECT count(*) INTO is_exist FROM port a, sriov_vf_port b WHERE a.vna_id = b.vna_id AND a.id = b.port_id AND a.port_type = b.port_type AND a.uuid = in_port_uuid ;  
  IF is_exist <= 0 THEN
     result.ret := RTN_ITEM_NO_EXIST;-- does not exist item
     return result;
  END IF;

  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM port WHERE uuid = in_port_uuid ;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

--- 查询phy port summary
--- 输出 name,uuid
-- Function: pf_net_query_sriovvfport_summary()

-- DROP FUNCTION pf_net_query_sriovvfport_summary();

CREATE OR REPLACE FUNCTION pf_net_query_sriovvfport_summary(in_vna_uuid character varying, in_sriov_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
  ref refcursor;
  is_same integer;
  is_exist integer;
  vnaid numeric(19,0);
  phyportid numeric(19,0);
BEGIN
      -- vna 
      SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
      SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
      IF is_exist <= 0 THEN 
        return ref;
      END IF;
      
      -- port 
      SELECT count(*) INTO is_exist FROM port a, physical_port b, sriov_port c WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
          AND b.port_id = c.physical_port_id AND b.vna_id = c.vna_id 
          AND a.vna_id = vnaid AND a.uuid = in_sriov_uuid;
      
      SELECT a.id INTO phyportid FROM port a, physical_port b, sriov_port c WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
          AND b.port_id = c.physical_port_id AND b.vna_id = c.vna_id 
          AND a.vna_id = vnaid AND a.uuid = in_sriov_uuid;
      
      IF  is_exist <= 0 THEN 
          return ref;
      END IF;
    
      -- 
      OPEN ref FOR SELECT in_vna_uuid, a.uuid, a.name, a.port_type FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
           AND a.vna_id = vnaid AND b.sriov_port_id = phyportid ;      
      return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 查询 空闲的sriov vf信息 
CREATE OR REPLACE FUNCTION pf_net_query_sriovvfport_free(in_sriov_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
  ref refcursor;
  is_same integer;
  is_exist integer;
  vnaid numeric(19,0);
  phyportid numeric(19,0);
BEGIN            
      -- port 
      SELECT count(*) INTO is_exist FROM port a, physical_port b, sriov_port c WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
          AND b.port_id = c.physical_port_id AND b.vna_id = c.vna_id 
          AND a.uuid = in_sriov_uuid;
      
      SELECT a.id INTO phyportid FROM port a, physical_port b, sriov_port c WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
          AND b.port_id = c.physical_port_id AND b.vna_id = c.vna_id 
          AND a.uuid = in_sriov_uuid;
      
      IF  is_exist <= 0 THEN 
          return ref;
      END IF;
    
      -- 
      OPEN ref FOR SELECT in_sriov_uuid, a.uuid, b.pci,b.vlan_num FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
           AND b.sriov_port_id = phyportid 
           AND b.port_id not in (SELECT sriov_vf_port_id FROM vsi_sriov );
      return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 查询port 
-- 输出 uuid,name,description,mtu
-- Function: pf_net_query_sriovvfport(character varying)

-- DROP FUNCTION pf_net_query_sriovvfport(character varying);

CREATE OR REPLACE FUNCTION pf_net_query_sriovvfport(in_port_uuid character varying)
  RETURNS t_sriovvfport_query AS
$BODY$
DECLARE
  result  t_sriovvfport_query;
  is_exist integer;
  vfportid numeric(19,0);
  sriovportid numeric(19,0);
  RTN_ITEM_NO_EXIST integer :=4;
BEGIN
--  SELECT uuid into result.uuid, name into result.name,description into result.description,mtu into result.mtu FROM netplane WHERE uuid = $1;
  result.ret := 0;
  
  SELECT count(*) INTO is_exist FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
           AND a.uuid = in_port_uuid ;
  SELECT a.id INTO vfportid FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
           AND a.uuid = in_port_uuid ;
  SELECT b.sriov_port_id INTO sriovportid FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
           AND a.uuid = in_port_uuid ;
  IF is_exist <= 0 THEN   
     result.ret := RTN_ITEM_NO_EXIST; -- no exist item
     return result; 
  END IF;

   SELECT count(*) INTO is_exist FROM port a,vna b WHERE a.vna_id = b.id AND a.id = vfportid ;
   SELECT b.vna_uuid INTO result.vna_uuid FROM port a,vna b WHERE a.vna_id = b.id AND a.id = vfportid ;
   IF is_exist <= 0 THEN   
     result.ret := RTN_ITEM_NO_EXIST; -- no exist item
     return result; 
   END IF;
  
   result.uuid := in_port_uuid;
   -- port 
   SELECT port_type into result.port_type  FROM port WHERE uuid = in_port_uuid;
   SELECT name into result.name  FROM port WHERE uuid = in_port_uuid;
   SELECT state into result.state FROM port WHERE uuid = in_port_uuid;
   SELECT is_broadcast into result.is_broadcast  FROM port WHERE uuid = in_port_uuid;
   SELECT is_running into result.is_running  FROM port WHERE uuid = in_port_uuid;
   SELECT is_multicast into result.is_multicast  FROM port WHERE uuid = in_port_uuid;
   SELECT promiscuous into result.promiscuous  FROM port WHERE uuid = in_port_uuid;
   SELECT mtu into result.mtu  FROM port WHERE uuid = in_port_uuid;
   SELECT is_master into result.is_master  FROM port WHERE uuid = in_port_uuid;
   SELECT admin_state into result.admin_state  FROM port WHERE uuid = in_port_uuid;
   SELECT is_online into result.is_online  FROM port WHERE uuid = in_port_uuid;
   SELECT is_cfg into result.is_port_cfg  FROM port WHERE uuid = in_port_uuid;
   SELECT is_consistency into result.is_consistency  FROM port WHERE uuid = in_port_uuid;
   
   -- physical sriov  port 
   SELECT a.uuid into result.physical_sriov_uuid  FROM port a, sriov_port b WHERE a.id = b.physical_port_id AND a.vna_id = b.vna_id 
       AND b.physical_port_id = sriovportid;
   
   -- sriov vf
   SELECT pci INTO result.pci FROM sriov_vf_port WHERE port_id = vfportid;
   SELECT vlan_num INTO result.vlan_num FROM sriov_vf_port WHERE port_id = vfportid;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 17 uplink_loop_port


-- physical port 添加 
-- Function: pf_net_add_uplinkloopport(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_uplinkloopport(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_uplinkloopport(in_vna_uuid character varying, in_type integer,in_name character varying,
        in_uuid character varying, in_state integer, in_is_broadcast integer,in_is_running integer,in_is_multicast integer,
        in_promiscuous integer, in_mtu integer, in_is_master integer, in_admin_state integer, in_is_online integer,
        in_is_port_cfg integer, in_is_consistency integer, 
        in_is_cfg integer )
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_same_port integer;
  is_same_port_ex integer;
  is_same_port_sriov integer;
  is_conflict integer;
  vnaid numeric(19,0);
  sriovportid numeric(19,0);
  portid numeric(19,0);
  is_exist integer; 
  db_uuid character varying;  
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- 检查vna uuid 是否有效
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  
  -- 是否已存在相同记录
  SELECT count(*) INTO is_same_port FROM port WHERE port_type = in_type AND vna_id = vnaid  AND name = in_name AND state = in_state 
         AND is_broadcast = in_is_broadcast AND is_running = in_is_running AND is_multicast = in_is_multicast AND promiscuous = in_promiscuous 
         AND mtu = in_mtu AND is_master = in_is_master AND admin_state = in_admin_state AND is_online = in_is_online 
         AND is_cfg = in_is_port_cfg AND is_consistency = in_is_consistency ;         
  SELECT id INTO portid FROM port WHERE port_type = in_type AND vna_id = vnaid  AND name = in_name AND state = in_state 
         AND is_broadcast = in_is_broadcast AND is_running = in_is_running AND is_multicast = in_is_multicast AND promiscuous = in_promiscuous 
         AND mtu = in_mtu AND is_master = in_is_master AND admin_state = in_admin_state AND is_online = in_is_online 
         AND is_cfg = in_is_port_cfg AND is_consistency = in_is_consistency ;         
  SELECT uuid INTO db_uuid FROM port WHERE port_type = in_type AND vna_id = vnaid  AND name = in_name AND state = in_state 
         AND is_broadcast = in_is_broadcast AND is_running = in_is_running AND is_multicast = in_is_multicast AND promiscuous = in_promiscuous 
         AND mtu = in_mtu AND is_master = in_is_master AND admin_state = in_admin_state AND is_online = in_is_online 
         AND is_cfg = in_is_port_cfg AND is_consistency = in_is_consistency ;               
  IF is_same_port > 0 THEN 
     SELECT count(*) INTO is_same FROM uplink_loop_port WHERE port_id = portid AND  port_type = in_type AND vna_id = vnaid  
         AND is_cfg = in_is_cfg; 
     IF is_same > 0 THEN
            result.ret := RTN_HAS_SAME_ITEM;   -- has same item
            result.id  := portid;
            result.uuid := db_uuid;
            return result;
     ELSE
           result.ret := RTN_ITEM_CONFLICT; -- conflict 
           return result;            
     END IF;     
  END IF;

  -- port name 名称是否冲突
  SELECT count(*) INTO is_conflict FROM  port WHERE vna_id = vnaid  AND name = in_name ;
  IF is_conflict > 0 THEN
     result.ret := RTN_ITEM_CONFLICT; -- conflict 
     return result;
  END IF;
    
  -- insert item
  result.id := nextval('serial_port');
  result.uuid := in_uuid;
  
  BEGIN 
    INSERT INTO port values(result.id,in_type,vnaid,in_name,in_uuid,in_state,in_is_broadcast,in_is_running,in_is_multicast,in_promiscuous,in_mtu,in_is_master,in_admin_state,in_is_online,in_is_port_cfg, in_is_consistency);
    INSERT INTO uplink_loop_port values(result.id,vnaid,in_type,in_is_cfg);    
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- port修改
-- Function: pf_net_modify_uplinkloopport(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_modify_uplinkloopport(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_modify_uplinkloopport(in_vna_uuid character varying, in_type integer,in_name character varying,
        in_uuid character varying, in_state integer, in_is_broadcast integer,in_is_running integer,in_is_multicast integer,
        in_promiscuous integer, in_mtu integer, in_is_master integer, in_admin_state integer, in_is_online integer,
        in_is_port_cfg integer, in_is_consistency integer, 
        in_is_cfg integer ) 
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  vnaid numeric(19,0);
  portid numeric(19,0);
  phyportid numeric(19,0);
  is_sriov_exist integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG  integer := 5;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  -- 检查vna uuid 是否有效
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  -- 检查port uuid 是否有效
  SELECT count(*) INTO is_exist FROM port WHERE uuid = in_uuid AND port_type = in_type;
  SELECT id INTO portid FROM port WHERE uuid = in_uuid AND port_type = in_type;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;   
  
  SELECT count(*) INTO is_exist FROM uplink_loop_port WHERE port_id = portid AND port_type = in_type AND vna_id = vnaid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF; 
  
  -- port name 
  SELECT count(*) INTO is_exist FROM port WHERE vna_id = vnaid AND name = in_name AND uuid = in_uuid;
  IF is_exist <= 0 THEN
     SELECT count(*) INTO is_exist FROM port WHERE vna_id = vnaid AND name = in_name ;
     IF is_exist > 0 THEN
       result.ret := RTN_ITEM_CONFLICT;-- 
       return result;          
     END IF;
  END IF;  
                 
  -- modify item
  result.id  := portid;
  result.uuid := in_uuid;
  BEGIN 
    UPDATE port SET name = in_name, state = in_state, is_broadcast =in_is_broadcast, is_multicast = in_is_multicast,
        promiscuous = in_promiscuous, mtu = in_mtu, is_master = in_is_master, admin_state = in_admin_state, is_online = in_is_online ,
        is_cfg = in_is_port_cfg , is_consistency = in_is_consistency 
        WHERE uuid = in_uuid AND port_type = in_type;
    UPDATE uplink_loop_port SET is_cfg = in_is_cfg
        WHERE port_id = portid AND vna_id = vnaid AND port_type = in_type;            
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 判断是否能够删除uplink loop port 		
CREATE OR REPLACE FUNCTION pf_net_check_del_uplinkloopport(in_uuid character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  portid numeric(19,0);
  is_exist integer; 
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- swtich uuid 
  SELECT count(*) INTO is_exist FROM port a, uplink_loop_port b WHERE a.id = b.port_id AND a.uuid = in_uuid;
  SELECT a.id INTO portid FROM port a, uplink_loop_port b WHERE a.id = b.port_id AND a.uuid = in_uuid;       
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  --如果swtich 已选用此uplink loop port 作为其uplink,将不能删除 
  SELECT count(*) INTO is_exist FROM virtual_switch_map WHERE port_id = portid;
  IF is_exist > 0 THEN 
      result := RTN_RESOURCE_USING;   
      return result;
  END IF;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 
-- 删除port

CREATE OR REPLACE FUNCTION pf_net_del_uplinkloopport(in_port_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否存在记录
  SELECT count(*) INTO is_exist FROM port a, uplink_loop_port b WHERE a.vna_id = b.vna_id AND a.id = b.port_id AND a.port_type = b.port_type AND a.uuid = in_port_uuid ;  
  IF is_exist <= 0 THEN
     result.ret := RTN_ITEM_NO_EXIST;-- does not exist item
     return result;
  END IF;

  result.ret := pf_net_check_del_uplinkloopport(in_port_uuid);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM port WHERE uuid = in_port_uuid ;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

--- 查询phy port summary
--- 输出 name,uuid
-- Function: pf_net_query_sriovvfport_summary()

-- DROP FUNCTION pf_net_query_uplinkloopport_summary();

CREATE OR REPLACE FUNCTION pf_net_query_uplinkloopport_summary(in_vna_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
  ref refcursor;
  is_same integer;
  is_exist integer;
  vnaid numeric(19,0);
  phyportid numeric(19,0);
BEGIN
      -- vna 
      SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
      SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
      IF is_exist <= 0 THEN 
        return ref;
      END IF;
          
      -- 
      OPEN ref FOR SELECT in_vna_uuid, a.uuid, a.name, a.port_type FROM port a, uplink_loop_port b  WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
          AND a.vna_id = vnaid  ;      
      return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 



-- 查询port 
-- 输出 uuid,name,description,mtu
-- Function: pf_net_query_uplinkloopport(character varying)

-- DROP FUNCTION pf_net_query_uplinkloopport(character varying);

CREATE OR REPLACE FUNCTION pf_net_query_uplinkloopport(in_port_uuid character varying)
  RETURNS t_uplinkloopport_query AS
$BODY$
DECLARE
  result  t_uplinkloopport_query;
  is_exist integer;
  uplinkloopportid numeric(19,0);
  RTN_ITEM_NO_EXIST integer :=4;
BEGIN
--  SELECT uuid into result.uuid, name into result.name,description into result.description,mtu into result.mtu FROM netplane WHERE uuid = $1;
  result.ret := 0;
  
  SELECT count(*) INTO is_exist FROM port a, uplink_loop_port b WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
           AND a.uuid = in_port_uuid ;
  SELECT a.id INTO uplinkloopportid FROM port a, uplink_loop_port b WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
           AND a.uuid = in_port_uuid ;
  IF is_exist <= 0 THEN   
     result.ret := RTN_ITEM_NO_EXIST; -- no exist item
     return result; 
  END IF;

   SELECT count(*) INTO is_exist FROM port a,vna b WHERE a.vna_id = b.id AND a.id = uplinkloopportid ;
   SELECT b.vna_uuid INTO result.vna_uuid FROM port a,vna b WHERE a.vna_id = b.id AND a.id = uplinkloopportid ;
   IF is_exist <= 0 THEN   
     result.ret := RTN_ITEM_NO_EXIST; -- no exist item
     return result; 
   END IF;
  
   result.uuid := in_port_uuid;
   -- port 
   SELECT port_type into result.port_type  FROM port WHERE uuid = in_port_uuid;
   SELECT name into result.name  FROM port WHERE uuid = in_port_uuid;
   SELECT state into result.state FROM port WHERE uuid = in_port_uuid;
   SELECT is_broadcast into result.is_broadcast  FROM port WHERE uuid = in_port_uuid;
   SELECT is_running into result.is_running  FROM port WHERE uuid = in_port_uuid;
   SELECT is_multicast into result.is_multicast  FROM port WHERE uuid = in_port_uuid;
   SELECT promiscuous into result.promiscuous  FROM port WHERE uuid = in_port_uuid;
   SELECT mtu into result.mtu  FROM port WHERE uuid = in_port_uuid;
   SELECT is_master into result.is_master  FROM port WHERE uuid = in_port_uuid;
   SELECT admin_state into result.admin_state  FROM port WHERE uuid = in_port_uuid;
   SELECT is_online into result.is_online  FROM port WHERE uuid = in_port_uuid;
   SELECT is_cfg into result.is_port_cfg  FROM port WHERE uuid = in_port_uuid;
   SELECT is_consistency into result.is_consistency  FROM port WHERE uuid = in_port_uuid;
   
   -- uplink loop port 
   SELECT is_cfg into result.is_cfg  FROM uplink_loop_port WHERE port_id = uplinkloopportid;
   
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 18 kernel_port


-- physical port 添加 
-- Function: pf_net_add_kernelport(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_kernelport(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_kernelport(in_vna_uuid character varying, in_type integer,in_name character varying,
        in_uuid character varying, in_state integer, in_is_broadcast integer,in_is_running integer,in_is_multicast integer,
        in_promiscuous integer, in_mtu integer, in_is_master integer, in_admin_state integer, in_is_online integer,
        in_is_port_cfg integer, in_is_consistency integer, 
        in_is_cfg_vswitch integer, in_vswitch_uuid character varying,
        in_is_cfg_pg integer, in_kernel_pg_uuid character varying,
        in_pg_type integer, in_is_cfg integer, in_is_dhcp integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_same_port integer;
  is_same_port_ex integer;
  is_same_port_sriov integer;
  is_conflict integer;
  vnaid numeric(19,0);
  vswitchid numeric(19,0);
  pgid numeric(19,0);
  portid numeric(19,0);
  kernelpgMtu integer;
  uplinkpgMtu integer;
  is_exist integer; 
  db_uuid character varying;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_UPLINKPGMTU_LESS_KERNELPG integer := 18;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- 检查vna uuid 是否有效
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  -- switch 
  IF in_is_cfg_vswitch > 0 THEN 
     -- 必须是sdvs
     SELECT count(*) INTO is_exist FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type  
           AND a.uuid = in_vswitch_uuid;
     SELECT a.id INTO vswitchid FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type  
           AND a.uuid = in_vswitch_uuid;
     IF is_exist <= 0 THEN 
        result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
        return result;
     END IF;

     -- 获取switch uplink pg mtu
     SELECT a.mtu INTO uplinkpgMtu FROM port_group a, virtual_switch b 
     WHERE b.uplink_pg_id = a.id AND b.switch_id = vswitchid;
  END IF;
  
  -- kernel pg 
  IF in_is_cfg_pg > 0 THEN 
     SELECT count(*) INTO is_exist FROM port_group a, kernel_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type  
           AND a.uuid = in_kernel_pg_uuid;
     SELECT a.id INTO pgid FROM port_group a, kernel_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type  
           AND a.uuid = in_kernel_pg_uuid;
     -- 获取kernel pg mtu
     SELECT a.mtu INTO kernelpgMtu FROM port_group a, kernel_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type  
           AND a.uuid = in_kernel_pg_uuid;           
     IF is_exist <= 0 THEN 
        result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
        return result;
     END IF;
  END IF;
  
  -- 是否已存在相同记录
  SELECT count(*) INTO is_same_port FROM port WHERE port_type = in_type AND vna_id = vnaid  AND name = in_name AND state = in_state 
         AND is_broadcast = in_is_broadcast AND is_running = in_is_running AND is_multicast = in_is_multicast AND promiscuous = in_promiscuous 
         AND mtu = in_mtu AND is_master = in_is_master AND admin_state = in_admin_state AND is_online = in_is_online 
         AND is_cfg = in_is_port_cfg AND is_consistency = in_is_consistency ;                  
  SELECT id INTO portid FROM port WHERE port_type = in_type AND vna_id = vnaid  AND name = in_name AND state = in_state 
         AND is_broadcast = in_is_broadcast AND is_running = in_is_running AND is_multicast = in_is_multicast AND promiscuous = in_promiscuous 
         AND mtu = in_mtu AND is_master = in_is_master AND admin_state = in_admin_state AND is_online = in_is_online 
         AND is_cfg = in_is_port_cfg AND is_consistency = in_is_consistency ;         
  SELECT uuid INTO db_uuid FROM port WHERE port_type = in_type AND vna_id = vnaid  AND name = in_name AND state = in_state 
         AND is_broadcast = in_is_broadcast AND is_running = in_is_running AND is_multicast = in_is_multicast AND promiscuous = in_promiscuous 
         AND mtu = in_mtu AND is_master = in_is_master AND admin_state = in_admin_state AND is_online = in_is_online 
         AND is_cfg = in_is_port_cfg AND is_consistency = in_is_consistency ;         
  IF is_same_port > 0 THEN 
     SELECT count(*) INTO is_same FROM kernel_port WHERE port_id = portid AND  port_type = in_type AND vna_id = vnaid  
         AND type = in_pg_type AND is_cfg = in_is_cfg AND is_dhcp = in_is_dhcp ; 
     IF is_same > 0 THEN        
        IF in_is_cfg_pg > 0 THEN 
            SELECT count(*) INTO is_same FROM kernel_port WHERE port_id = portid AND  port_type = in_type AND vna_id = vnaid  
                 AND kernel_pg_id = pgid;
            IF is_same > 0 THEN 
               IF in_is_cfg_vswitch > 0 THEN 
                   SELECT count(*) INTO is_same FROM kernel_port WHERE port_id = portid AND  port_type = in_type AND vna_id = vnaid  
                      AND virtual_switch_id = vswitchid;
                   IF is_same > 0 THEN
                       result.ret := RTN_HAS_SAME_ITEM;   -- has same item
                       result.id  := portid;
                       result.uuid := db_uuid;
                       return result;
                   ELSE
                       result.ret := RTN_ITEM_CONFLICT; -- conflict 
                       return result;    
                   END IF;
               ELSE
                  SELECT count(*) INTO is_same FROM kernel_port WHERE port_id = portid AND  port_type = in_type AND vna_id = vnaid  
                      AND virtual_switch_id is null;
                   IF is_same > 0 THEN
                       result.ret := RTN_HAS_SAME_ITEM;   -- has same item
                       result.id  := portid;
                       result.uuid := db_uuid;
                       return result;
                   ELSE
                       result.ret := RTN_ITEM_CONFLICT; -- conflict 
                       return result;    
                   END IF;
               END IF;
            ELSE 
               result.ret := RTN_ITEM_CONFLICT; -- conflict 
               return result;    
            END IF;
        ELSE 
           SELECT count(*) INTO is_same FROM kernel_port WHERE port_id = portid AND  port_type = in_type AND vna_id = vnaid  
                 AND kernel_pg_id is null;
            IF is_same > 0 THEN 
               IF in_is_cfg_vswitch > 0 THEN 
                   SELECT count(*) INTO is_same FROM kernel_port WHERE port_id = portid AND  port_type = in_type AND vna_id = vnaid  
                      AND virtual_switch_id = vswitchid;
                   IF is_same > 0 THEN
                       result.ret := RTN_HAS_SAME_ITEM;   -- has same item
                       result.id  := portid;
                       result.uuid := db_uuid;
                       return result;
                   ELSE
                       result.ret := RTN_ITEM_CONFLICT; -- conflict 
                       return result;    
                   END IF;
               ELSE
                  SELECT count(*) INTO is_same FROM kernel_port WHERE port_id = portid AND  port_type = in_type AND vna_id = vnaid  
                      AND virtual_switch_id is null;
                   IF is_same > 0 THEN
                       result.ret := RTN_HAS_SAME_ITEM;   -- has same item
                       result.id  := portid;
                       result.uuid := db_uuid;
                       return result;
                   ELSE
                       result.ret := RTN_ITEM_CONFLICT; -- conflict 
                       return result;    
                   END IF;
               END IF;
            ELSE 
               result.ret := RTN_ITEM_CONFLICT; -- conflict 
               return result;    
            END IF;
        END IF;
     ELSE
           result.ret := RTN_ITEM_CONFLICT; -- conflict 
           return result;            
     END IF;     
  END IF;

  -- port name 名称是否冲突
  SELECT count(*) INTO is_conflict FROM  port WHERE vna_id = vnaid  AND name = in_name ;
  IF is_conflict > 0 THEN
     result.ret := RTN_ITEM_CONFLICT; -- conflict 
     return result;
  END IF;
    
  -- kernel pg mtu 必须小于 switch uplink pg mtu 
/* 应该由上层来保证   
  IF in_is_cfg_pg > 0 THEN 
     IF in_is_cfg_vswitch > 0 THEN 
         IF kernelpgMtu > uplinkpgMtu THEN 
            result.ret := RTN_UPLINKPGMTU_LESS_KERNELPG; -- uplink pg mtu < kernel pg mtu 
         END IF;
     END IF;
  END IF;
*/        
  -- insert item
  result.id := nextval('serial_port');
  result.uuid := in_uuid;
  
  BEGIN 
    INSERT INTO port values(result.id,in_type,vnaid,in_name,in_uuid,in_state,in_is_broadcast,in_is_running,in_is_multicast,in_promiscuous,in_mtu,in_is_master,in_admin_state,in_is_online,in_is_port_cfg, in_is_consistency);
    IF in_is_cfg_vswitch > 0 THEN 
       IF in_is_cfg_pg > 0 THEN 
          INSERT INTO kernel_port values(result.id,in_type,vnaid,vswitchid,pgid,in_pg_type,in_is_cfg, in_is_dhcp); 
       ELSE
          INSERT INTO kernel_port(port_id,port_type,vna_id,virtual_switch_id,type,is_cfg, is_dhcp) values(result.id,in_type,vnaid,vswitchid,in_pg_type,in_is_cfg, in_is_dhcp); 
       END IF;
    ELSE 
       IF in_is_cfg_pg > 0 THEN 
          INSERT INTO kernel_port(port_id,port_type,vna_id,kernel_pg_id,type,is_cfg, is_dhcp) values(result.id,in_type,vnaid,pgid,in_pg_type,in_is_cfg, in_is_dhcp); 
       ELSE
          INSERT INTO kernel_port(port_id,port_type,vna_id,type,is_cfg, is_dhcp) values(result.id,in_type,vnaid,in_pg_type,in_is_cfg, in_is_dhcp); 
       END IF;       
    END IF;       
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- port修改
-- Function: pf_net_modify_kernelport(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_modify_kernelport(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_modify_kernelport(in_vna_uuid character varying, in_type integer,in_name character varying,
        in_uuid character varying, in_state integer, in_is_broadcast integer,in_is_running integer,in_is_multicast integer,
        in_promiscuous integer, in_mtu integer, in_is_master integer, in_admin_state integer, in_is_online integer,
        in_is_port_cfg integer, in_is_consistency integer, 
        in_is_cfg_vswitch integer, in_vswitch_uuid character varying,
        in_is_cfg_pg integer, in_kernel_pg_uuid character varying,
        in_pg_type integer, in_is_cfg integer, in_is_dhcp integer) 
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  vnaid numeric(19,0);
  vswitchid numeric(19,0);
  pgid numeric(19,0);
  portid numeric(19,0);
  kernelpgMtu integer;
  uplinkpgMtu integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG  integer := 5;
  RTN_UPLINKPGMTU_LESS_KERNELPG integer := 18;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  -- 检查vna uuid 是否有效
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  -- 检查port uuid 是否有效
  SELECT count(*) INTO is_exist FROM port WHERE uuid = in_uuid AND port_type = in_type;
  SELECT id INTO portid FROM port WHERE uuid = in_uuid AND port_type = in_type;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;   
      
  SELECT count(*) INTO is_exist FROM kernel_port WHERE port_id = portid AND port_type = in_type AND vna_id = vnaid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF; 
    
  -- switch 
  IF in_is_cfg_vswitch > 0 THEN 
     SELECT count(*) INTO is_exist FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type  
           AND a.uuid = in_vswitch_uuid;
     SELECT a.id INTO vswitchid FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type  
           AND a.uuid = in_vswitch_uuid;
     IF is_exist <= 0 THEN 
        result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
        return result;
     END IF;

     -- 获取switch uplink pg mtu
     SELECT a.mtu INTO uplinkpgMtu FROM port_group a, virtual_switch b 
     WHERE b.uplink_pg_id = a.id AND b.switch_id = vswitchid;
  END IF;
  
  -- kernel pg 
  IF in_is_cfg_pg > 0 THEN 
     SELECT count(*) INTO is_exist FROM port_group a, kernel_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type  
           AND a.uuid = in_kernel_pg_uuid;
     SELECT a.id INTO pgid FROM port_group a, kernel_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type  
           AND a.uuid = in_kernel_pg_uuid;
     IF is_exist <= 0 THEN 
        result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
        return result;
     END IF;

     -- 获取kernel pg mtu
     SELECT a.mtu INTO kernelpgMtu FROM port_group a, kernel_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type  
           AND a.uuid = in_kernel_pg_uuid; 
  END IF;
  
  -- port name 
  SELECT count(*) INTO is_exist FROM port WHERE vna_id = vnaid AND name = in_name AND uuid = in_uuid;
  IF is_exist <= 0 THEN
     SELECT count(*) INTO is_exist FROM port WHERE vna_id = vnaid AND name = in_name ;
     IF is_exist > 0 THEN
       result.ret := RTN_ITEM_CONFLICT;-- 
       return result;          
     END IF;
  END IF;  
                 
  -- kernel pg mtu 必须小于 switch uplink pg mtu 
/* 应该由上层来保证   
  IF in_is_cfg_pg > 0 THEN 
     IF in_is_cfg_vswitch > 0 THEN 
         IF kernelpgMtu > uplinkpgMtu THEN 
            result.ret := RTN_UPLINKPGMTU_LESS_KERNELPG; -- uplink pg mtu < kernel pg mtu 
         END IF;
     END IF;
  END IF;
*/                 
  -- modify item
  result.id  := portid;
  result.uuid := in_uuid;
  BEGIN 
    UPDATE port SET name = in_name, state = in_state, is_broadcast =in_is_broadcast, is_multicast = in_is_multicast,
        promiscuous = in_promiscuous, mtu = in_mtu, is_master = in_is_master, admin_state = in_admin_state, is_online = in_is_online ,
        is_cfg = in_is_port_cfg , is_consistency = in_is_consistency 
        WHERE uuid = in_uuid AND port_type = in_type;
    IF in_is_cfg_pg > 0 THEN 
        IF in_is_cfg_vswitch > 0 THEN 
            UPDATE kernel_port SET is_dhcp = in_is_dhcp, is_cfg = in_is_cfg, type = in_pg_type, virtual_switch_id = vswitchid, kernel_pg_id = pgid 
                  WHERE port_id = portid AND vna_id = vnaid AND port_type = in_type;    
        ELSE
            UPDATE kernel_port SET is_dhcp = in_is_dhcp, is_cfg = in_is_cfg, type = in_pg_type, kernel_pg_id = pgid  
                  WHERE port_id = portid AND vna_id = vnaid AND port_type = in_type;
        END IF;
    ELSE
       IF in_is_cfg_vswitch > 0 THEN 
            UPDATE kernel_port SET is_dhcp = in_is_dhcp, is_cfg = in_is_cfg, type = in_pg_type, virtual_switch_id = vswitchid 
                  WHERE port_id = portid AND vna_id = vnaid AND port_type = in_type;    
        ELSE
            UPDATE kernel_port SET is_dhcp = in_is_dhcp, is_cfg = in_is_cfg, type = in_pg_type 
                  WHERE port_id = portid AND vna_id = vnaid AND port_type = in_type;
        END IF;
    END IF;            
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 删除port

CREATE OR REPLACE FUNCTION pf_net_del_kernelport(in_port_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否存在记录
  SELECT count(*) INTO is_exist FROM port a, kernel_port b WHERE a.vna_id = b.vna_id AND a.id = b.port_id AND a.port_type = b.port_type AND a.uuid = in_port_uuid ;  
  IF is_exist <= 0 THEN
     result.ret := RTN_ITEM_NO_EXIST;-- does not exist item
     return result;
  END IF;

  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM port WHERE uuid = in_port_uuid ;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


--- 查询phy port summary
--- 输出 name,uuid
-- Function: pf_net_query_kernelport_summary()

-- DROP FUNCTION pf_net_query_kernelport_summary();

CREATE OR REPLACE FUNCTION pf_net_query_kernelport_summary(in_vna_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
  ref refcursor;
  is_same integer;
  is_exist integer;
  vnaid numeric(19,0);
BEGIN
      -- vna 
      SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
      SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
      IF is_exist <= 0 THEN 
        return ref;
      END IF;
          
      -- 
      OPEN ref FOR SELECT in_vna_uuid, a.uuid, a.name, a.port_type FROM port a, kernel_port b WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
          AND a.vna_id = vnaid  ;      
      return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 查询port 
-- 输出 uuid,name,description,mtu
-- Function: pf_net_query_kernelport(character varying)

-- DROP FUNCTION pf_net_query_kernelport(character varying);

CREATE OR REPLACE FUNCTION pf_net_query_kernelport(in_port_uuid character varying)
  RETURNS t_kernelport_query AS
$BODY$
DECLARE
  result  t_kernelport_query;
  is_exist integer;
  kernelportid numeric(19,0);
  vswitchid numeric(19,0);
  pgid numeric(19,0);
  RTN_ITEM_NO_EXIST integer :=4;
BEGIN
--  SELECT uuid into result.uuid, name into result.name,description into result.description,mtu into result.mtu FROM netplane WHERE uuid = $1;
  result.ret := 0;
  
  SELECT count(*) INTO is_exist FROM port a, kernel_port b WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
           AND a.uuid = in_port_uuid ;
  SELECT a.id INTO kernelportid FROM port a, kernel_port b WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
           AND a.uuid = in_port_uuid ;
  IF is_exist <= 0 THEN   
     result.ret := RTN_ITEM_NO_EXIST; -- no exist item
     return result; 
  END IF;

   SELECT count(*) INTO is_exist FROM port a,vna b WHERE a.vna_id = b.id AND a.id = kernelportid ;
   SELECT b.vna_uuid INTO result.vna_uuid FROM port a,vna b WHERE a.vna_id = b.id AND a.id = kernelportid ;
   IF is_exist <= 0 THEN   
     result.ret := RTN_ITEM_NO_EXIST; -- no exist item
     return result; 
   END IF;
  
   result.uuid := in_port_uuid;
   -- port 
   SELECT port_type into result.port_type  FROM port WHERE uuid = in_port_uuid;
   SELECT name into result.name  FROM port WHERE uuid = in_port_uuid;
   SELECT state into result.state FROM port WHERE uuid = in_port_uuid;
   SELECT is_broadcast into result.is_broadcast  FROM port WHERE uuid = in_port_uuid;
   SELECT is_running into result.is_running  FROM port WHERE uuid = in_port_uuid;
   SELECT is_multicast into result.is_multicast  FROM port WHERE uuid = in_port_uuid;
   SELECT promiscuous into result.promiscuous  FROM port WHERE uuid = in_port_uuid;
   SELECT mtu into result.mtu  FROM port WHERE uuid = in_port_uuid;
   SELECT is_master into result.is_master  FROM port WHERE uuid = in_port_uuid;
   SELECT admin_state into result.admin_state  FROM port WHERE uuid = in_port_uuid;
   SELECT is_online into result.is_online  FROM port WHERE uuid = in_port_uuid;
   SELECT is_cfg into result.is_port_cfg  FROM port WHERE uuid = in_port_uuid;
   SELECT is_consistency into result.is_consistency  FROM port WHERE uuid = in_port_uuid;
   
   -- kernel port 
   SELECT type into result.type  FROM kernel_port WHERE port_id = kernelportid;
   SELECT is_cfg into result.is_cfg  FROM kernel_port WHERE port_id = kernelportid;
   SELECT is_dhcp into result.is_dhcp  FROM kernel_port WHERE port_id = kernelportid;
   
   -- vswitch 
   SELECT count(*) INTO is_exist FROM kernel_port WHERE port_id = kernelportid AND virtual_switch_id is not null;
   SELECT virtual_switch_id INTO vswitchid FROM kernel_port WHERE port_id = kernelportid AND virtual_switch_id is not null;
   IF is_exist > 0 THEN 
      result.is_cfg_vswitch := 1;
      SELECT a.uuid INTO result.vswitch_uuid FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
             AND a.id = vswitchid;
   ELSE
      result.is_cfg_vswitch := 0;
   END IF;
   
   -- kernel_pg 
   SELECT count(*) INTO is_exist FROM kernel_port WHERE port_id = kernelportid AND kernel_pg_id is not null;
   SELECT kernel_pg_id INTO pgid FROM kernel_port WHERE port_id = kernelportid AND kernel_pg_id is not null;
   IF is_exist > 0 THEN 
      result.is_cfg_pg := 1;
      SELECT a.uuid INTO result.kernel_pg_uuid FROM port_group a, kernel_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type 
             AND a.id = pgid;
   ELSE
      result.is_cfg_pg := 0;
   END IF;
   
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 19 bond


-- 判断bond port 能否添加
CREATE OR REPLACE FUNCTION pf_net_check_add_bond(in_vna_uuid character varying, in_type integer,in_name character varying,
        in_uuid character varying, in_state integer, in_is_broadcast integer,in_is_running integer,in_is_multicast integer,
        in_promiscuous integer, in_mtu integer, in_is_master integer, in_admin_state integer, in_is_online integer,
        in_is_port_cfg integer, in_is_consistency integer, 
        in_bond_mode integer,
        in_policy character varying    )
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  vnaid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面
  
  -- vna uuid 
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
    result := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  -- port name 名称是否冲突
  SELECT count(*) INTO is_conflict FROM  port WHERE vna_id = vnaid  AND name = in_name ;
  IF is_conflict > 0 THEN
     result := RTN_ITEM_CONFLICT; -- conflict 
     return result;
  END IF;
  
  -- port type 判断
  IF in_type = 1 THEN 
     result :=0;
  ELSE
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;
  
  -- bond mode 判断
  IF in_bond_mode = 4 THEN 
      result :=0;
  ELSE 
      IF in_bond_mode = 1 THEN 
          result :=0;
      ELSE 
          result := RTN_PARAM_INVALID; 
          return result;
      END IF;
  END IF;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- physical port 添加 
-- Function: pf_net_check_add_bond(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_check_add_bond(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_bond(in_vna_uuid character varying, in_type integer,in_name character varying,
        in_uuid character varying, in_state integer, in_is_broadcast integer,in_is_running integer,in_is_multicast integer,
        in_promiscuous integer, in_mtu integer, in_is_master integer, in_admin_state integer, in_is_online integer,
        in_is_port_cfg integer, in_is_consistency integer, 
        in_bond_mode integer,
        in_policy character varying,
        in_is_has_lacpinfo integer, in_lacp_state integer,
        in_lacp_aggregator_id character varying,in_lacp_partner_mac character varying     )
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_same_port integer;
  is_same_port_ex integer;
  is_same_port_sriov integer;
  is_conflict integer;
  vnaid numeric(19,0);
  portid numeric(19,0);
  is_exist integer;  
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_add_bond(in_vna_uuid,in_type,in_name,in_uuid,in_state,in_is_broadcast,in_is_running,in_is_multicast,
            in_promiscuous,in_mtu,in_is_master,in_admin_state,in_is_online,in_is_port_cfg,in_is_consistency,
            in_bond_mode,in_policy);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
   -- vna uuid 
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  -- insert item
  result.id := nextval('serial_port');
  result.uuid := in_uuid;
  
  BEGIN 
    INSERT INTO port values(result.id,in_type,vnaid,in_name,in_uuid,in_state,in_is_broadcast,in_is_running,in_is_multicast,in_promiscuous,in_mtu,in_is_master,in_admin_state,in_is_online, in_is_port_cfg, in_is_consistency);
    INSERT INTO bond values(result.id,in_type,vnaid,in_bond_mode,in_policy);   
    IF in_bond_mode = 1 THEN 
       INSERT INTO bond_backup(bond_id,bond_mode) values(result.id,in_bond_mode);
    END IF;    
    
    IF in_bond_mode = 4 THEN 
       IF in_is_has_lacpinfo > 0 THEN 
           INSERT INTO bond_lacp(bond_id,bond_mode,state,aggregator_id,partner_mac) 
               values(result.id,in_bond_mode,in_lacp_state,in_lacp_aggregator_id,in_lacp_partner_mac);
       ELSE 
           INSERT INTO bond_lacp(bond_id,bond_mode) values(result.id,in_bond_mode);
       END IF;
    END IF;    
    
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- check modify bond port
CREATE OR REPLACE FUNCTION pf_net_check_modify_bond(in_vna_uuid character varying, in_type integer,in_name character varying,
        in_uuid character varying, in_state integer, in_is_broadcast integer,in_is_running integer,in_is_multicast integer,
        in_promiscuous integer, in_mtu integer, in_is_master integer, in_admin_state integer, in_is_online integer,
        in_is_port_cfg integer, in_is_consistency integer, 
        in_bond_mode integer,
        in_policy character varying )
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  vnaid numeric(19,0);
  portid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面
  
  -- vna uuid 
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
    result := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  --port uuid 
  SELECT count(*) INTO is_exist FROM port a,bond b WHERE a.id = b.port_id AND a.vna_id = vnaid AND a.uuid = in_uuid; 
  SELECT a.id INTO portid FROM port a,bond b WHERE a.id = b.port_id AND a.vna_id = vnaid AND a.uuid = in_uuid;   
  IF is_exist <= 0 THEN 
    result := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  -- port name 名称是否冲突
  SELECT count(*) INTO is_exist FROM  port WHERE id = portid  AND name = in_name ;
  IF is_exist <= 0 THEN 
     SELECT count(*) INTO is_exist FROM  port WHERE name = in_name ;
     IF is_exist > 0 THEN 
         result := RTN_ITEM_CONFLICT; -- conflict 
         return result;
     END IF;
  END IF;
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
     
        
        
-- port修改
-- Function: pf_net_modify_bond(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_modify_bond(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_modify_bond(in_vna_uuid character varying, in_type integer,in_name character varying,
        in_uuid character varying, in_state integer, in_is_broadcast integer,in_is_running integer,in_is_multicast integer,
        in_promiscuous integer, in_mtu integer, in_is_master integer, in_admin_state integer, in_is_online integer,
        in_is_port_cfg integer, in_is_consistency integer, 
        in_bond_mode integer,
        in_policy character varying ,
        in_is_has_lacpinfo integer, in_lacp_state integer,
        in_lacp_aggregator_id character varying,in_lacp_partner_mac character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  vnaid numeric(19,0);
  portid numeric(19,0);
  phyportid numeric(19,0);
  is_sriov_exist integer;
  oldmodechange integer;
  oldIsConsistency integer := 0;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG  integer := 5;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_modify_bond(in_vna_uuid,in_type,in_name,in_uuid,in_state,in_is_broadcast,in_is_running,in_is_multicast,
            in_promiscuous,in_mtu,in_is_master,in_admin_state,in_is_online,in_is_port_cfg,in_is_consistency,
            in_bond_mode,in_policy);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
   -- vna uuid 
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;
  
  --port uuid 
  SELECT count(*) INTO is_exist FROM port a,bond b WHERE a.id = b.port_id AND a.vna_id = vnaid AND a.uuid = in_uuid; 
  SELECT a.id INTO portid FROM port a,bond b WHERE a.id = b.port_id AND a.vna_id = vnaid AND a.uuid = in_uuid;   
  SELECT b.bond_mode INTO oldmodechange FROM port a,bond b WHERE a.id = b.port_id AND a.vna_id = vnaid AND a.uuid = in_uuid;   
  SELECT a.is_consistency INTO oldIsConsistency FROM port a,bond b WHERE a.id = b.port_id AND a.vna_id = vnaid AND a.uuid = in_uuid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF; 
  
               
  -- modify item
  result.id  := portid;
  result.uuid := in_uuid;
  BEGIN 
    -- 需要新删除 bond_backup, lacp
    IF in_bond_mode = oldmodechange THEN 
        result.ret :=  0;
    ELSE
        IF in_bond_mode = 1 THEN 
            DELETE FROM bond_lacp WHERE bond_id = portid;
            UPDATE bond_map SET lacp_is_success = 0 WHERE bond_id = portid;
        END IF;
        
        IF in_bond_mode = 4 THEN             
            DELETE FROM bond_backup WHERE bond_id = portid;
            UPDATE bond_map SET backup_is_active = 0 WHERE bond_id = portid;
        END IF;
    END IF;
        
    
    UPDATE port SET name = in_name, state = in_state, is_broadcast =in_is_broadcast, is_multicast = in_is_multicast,
        promiscuous = in_promiscuous, mtu = in_mtu, is_master = in_is_master, admin_state = in_admin_state, is_online = in_is_online ,
        is_cfg = in_is_port_cfg , is_consistency = in_is_consistency 
        WHERE uuid = in_uuid AND port_type = in_type;
    UPDATE bond SET bond_mode = in_bond_mode, policy = in_policy 
        WHERE port_id = portid;        

    IF in_bond_mode = oldmodechange THEN 
        IF in_bond_mode = 4 THEN 
            IF in_is_has_lacpinfo > 0 THEN 
               UPDATE bond_lacp SET state =in_lacp_state, aggregator_id = in_lacp_aggregator_id,partner_mac = in_lacp_partner_mac 
                   WHERE bond_id = portid;
            ELSE
               UPDATE bond_lacp SET state =0, aggregator_id = NULL,partner_mac = NULL  
                   WHERE bond_id = portid;
            END IF;
        END IF;
    ELSE
        IF in_bond_mode = 1 THEN 
            INSERT INTO bond_backup(bond_id,bond_mode) values( portid, in_bond_mode);
        END IF;
        
        IF in_bond_mode = 4 THEN 
            IF in_is_has_lacpinfo > 0 THEN 
               INSERT INTO bond_lacp(bond_id,bond_mode,state,aggregator_id,partner_mac) 
                    values( portid, in_bond_mode,in_lacp_state,in_lacp_aggregator_id,in_lacp_partner_mac);            
            ELSE
                INSERT INTO bond_lacp(bond_id,bond_mode) values( portid, in_bond_mode);
            END IF;
        END IF;
        
    END IF;    
    
    -- trigger
    IF oldIsConsistency > in_is_consistency THEN 
        result.ret := pf_net_tr_modify_bondport(portid, 0);
    END IF;
    IF oldIsConsistency > in_is_consistency THEN 
        result.ret := pf_net_tr_modify_bondport(portid, 1);
    END IF;
    
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


        

-- check del bond port
CREATE OR REPLACE FUNCTION pf_net_check_del_bond(in_port_uuid character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  vnaid numeric(19,0);
  portid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面
    
  --port uuid 
  SELECT count(*) INTO is_exist FROM port a,bond b WHERE a.id = b.port_id AND a.uuid = in_port_uuid; 
  SELECT a.id INTO portid FROM port a,bond b WHERE a.id = b.port_id  AND a.uuid = in_port_uuid;   
  IF is_exist <= 0 THEN 
    result := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  -- 是否已经作为switch uplink
  SELECT count(*) INTO is_exist FROM v_net_switch_port_vna WHERE port_uuid = in_port_uuid;
  IF is_exist > 0 THEN 
    result := RTN_RESOURCE_USING;   
    return result;
  END IF;  
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
        

-- 删除port
CREATE OR REPLACE FUNCTION pf_net_del_bond(in_port_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否存在记录
  result.ret := pf_net_check_del_bond(in_port_uuid);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM port WHERE uuid = in_port_uuid ;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 查询 bond
-- Function: pf_net_query_bond(character varying)

-- DROP FUNCTION pf_net_query_bond(character varying);

CREATE OR REPLACE FUNCTION pf_net_query_bond(in_port_uuid character varying)
  RETURNS t_bond_query AS
$BODY$
DECLARE
  result  t_bond_query;
  is_exist integer;
  portid numeric(19,0);
  sriovportid numeric(19,0);
  RTN_ITEM_NO_EXIST integer :=4;
BEGIN
--  SELECT uuid into result.uuid, name into result.name,description into result.description,mtu into result.mtu FROM netplane WHERE uuid = $1;
  result.ret := 0;
  
  SELECT count(*) INTO is_exist FROM port a, bond b WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
           AND a.uuid = in_port_uuid ;
  SELECT a.id INTO portid FROM port a, bond b WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
           AND a.uuid = in_port_uuid ;
  IF is_exist <= 0 THEN   
     result.ret := RTN_ITEM_NO_EXIST; -- no exist item
     return result; 
  END IF;

   SELECT  b.vna_uuid INTO result.vna_uuid FROM port a, vna b WHERE a.vna_id = b.id  AND a.uuid = in_port_uuid;
  
   result.uuid := in_port_uuid;
   -- port 
   SELECT port_type into result.port_type  FROM port WHERE uuid = in_port_uuid;
   SELECT name into result.name  FROM port WHERE uuid = in_port_uuid;
   SELECT state into result.state FROM port WHERE uuid = in_port_uuid;
   SELECT is_broadcast into result.is_broadcast  FROM port WHERE uuid = in_port_uuid;
   SELECT is_running into result.is_running  FROM port WHERE uuid = in_port_uuid;
   SELECT is_multicast into result.is_multicast  FROM port WHERE uuid = in_port_uuid;
   SELECT promiscuous into result.promiscuous  FROM port WHERE uuid = in_port_uuid;
   SELECT mtu into result.mtu  FROM port WHERE uuid = in_port_uuid;
   SELECT is_master into result.is_master  FROM port WHERE uuid = in_port_uuid;
   SELECT admin_state into result.admin_state  FROM port WHERE uuid = in_port_uuid;
   SELECT is_online into result.is_online  FROM port WHERE uuid = in_port_uuid;
   SELECT is_cfg into result.is_port_cfg  FROM port WHERE uuid = in_port_uuid;
   SELECT is_consistency into result.is_consistency  FROM port WHERE uuid = in_port_uuid;
   
   -- bond port
   SELECT bond_mode INTO result.bond_mode FROM bond WHERE port_id = portid;
   SELECT policy INTO result.policy FROM bond WHERE port_id = portid;
  
   -- bond lacp
   SELECT count(*) INTO is_exist FROM bond_lacp WHERE bond_id = portid;
   IF is_exist <= 0 THEN 
      result.is_has_lacpinfo := 0;
      return result;
   END IF;
   
   result.is_has_lacpinfo := 1;
   SELECT state INTO result.lacp_state FROM bond_lacp WHERE bond_id = portid;
   SELECT aggregator_id INTO result.lacp_aggregator_id FROM bond_lacp WHERE bond_id = portid;
   SELECT partner_mac INTO result.lacp_partner_mac FROM bond_lacp WHERE bond_id = portid;
   
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



--- 查询 bond summary
--- 输出 name,uuid

CREATE OR REPLACE FUNCTION pf_net_query_bond_summary(in_vna_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    vnaid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN  
    -- vna 
      SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
      SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
      IF is_exist <= 0 THEN 
        return ref;
      END IF;
      
      -- port 
      SELECT count(*) INTO is_exist FROM port a, bond b WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
          AND a.vna_id = vnaid;
            
      IF  is_exist <= 0 THEN 
          return ref;
      END IF;
      
    OPEN ref FOR SELECT vna_uuid,bond_uuid,bond_name,bond_mode  
       FROM v_net_bond_vna    
       WHERE vna_uuid = in_vna_uuid;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;

--- bond map 信息表 



-- 判断SRIOV网口下是否已经有虚拟机部署使用sriov vf了
CREATE OR REPLACE FUNCTION pf_net_query_bondsriovport_vsi_num(in_uuid character varying )
  RETURNS t_query_total AS
$BODY$
DECLARE
  result  t_query_total;
  is_same integer;
  is_same_port integer;
  is_same_port_ex integer;
  is_same_port_sriov integer;
  is_conflict integer;
  bondid numeric(19,0);
  is_exist integer; 
  db_uuid character varying;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
BEGIN
  result.ret := 0;
  result.total_num := 0;
  -- 入参检查 后面做
  
  -- 检查port uuid 是否有效
  SELECT count(*) INTO is_exist FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_uuid;
  SELECT a.id INTO bondid FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_uuid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  -- 是否已存在相同记录
  SELECT count(*) INTO result.total_num FROM bond_map a, sriov_vf_port b, vsi_sriov c
  WHERE 
  a.bond_id = bondid AND 
  a.physical_port_id = b.sriov_port_id AND 
  c.sriov_vf_port_id = b.port_id;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 20 bond_backup.txt



-- 判断bond backup 能否添加
CREATE OR REPLACE FUNCTION pf_net_check_bondbackup( in_bond_uuid character varying, in_phy_uuid character varying  )
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  bondid numeric(19,0);
  phyid numeric(19,0);
  vlanid numeric(19,0);
  vnaid numeric(19,0);
  bondmode integer;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE integer := 13;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
        
  -- bond uuid  
  SELECT count(*) INTO is_exist FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid ;
  SELECT a.id INTO bondid FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;
  SELECT b.bond_mode INTO bondmode FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;  
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
  
  -- bond mode 必须是backup
  IF bondmode = 1 THEN 
     result := 0;
  ELSE
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
  
  -- phy uuid  
  SELECT count(*) INTO is_exist FROM port a, physical_port b WHERE a.id = b.port_id AND a.uuid = in_phy_uuid;
  SELECT a.id INTO phyid FROM port a, physical_port b WHERE a.id = b.port_id AND a.uuid = in_phy_uuid;  
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
  -- 判断bond_map中必须存在记录
  SELECT count(*) INTO is_exist FROM bond_map WHERE bond_id = bondid AND physical_port_id = phyid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
      
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- bond bakcup 添加 
-- Function: pf_net_set_bondbackup_active(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_set_bondbackup_active(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_set_bondbackup_active(in_bond_uuid character varying, in_phy_uuid character varying  )
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  bondid numeric(19,0);
  phyid numeric(19,0);
  vlanid numeric(19,0);
  vnaid numeric(19,0);
  bondmode integer;
  is_exist_bondbackup integer;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_bondbackup( in_bond_uuid,in_phy_uuid);  
  IF result.ret > 0 THEN      
     return result;
  END IF;
  
  -- bond uuid  
  SELECT count(*) INTO is_exist FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;
  SELECT a.id INTO bondid FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;
  SELECT b.bond_mode INTO bondmode FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;  
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
  
  -- phy uuid  
  SELECT count(*) INTO is_exist FROM port a, physical_port b WHERE a.id = b.port_id AND a.uuid = in_phy_uuid;
  SELECT a.id INTO phyid FROM port a, physical_port b WHERE a.id = b.port_id AND a.uuid = in_phy_uuid;  
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
  -- 查看bond backup表中是否存在记录
  SELECT count(*) INTO is_exist_bondbackup FROM bond_backup WHERE bond_id = bondid;
    
  
  -- insert item 
  result.id := bondid;
  --result.uuid := in_uuid;
  BEGIN 
    -- DELETE bond_lacp 
    DELETE FROM bond_lacp WHERE bond_id = bondid;
    
    -- INSERT bond_backup
    IF is_exist_bondbackup <= 0 THEN         
        INSERT INTO bond_backup(bond_id,bond_mode) 
            values(bondid,bondmode);
    ELSE 
           result.ret := 0;
    END IF;
    
    -- 将bond_map中 lacp信息清除 
    UPDATE bond_map SET lacp_is_success = 0, backup_is_active = 0 WHERE bond_id = bondid;
    UPDATE bond_map SET backup_is_active = 1 WHERE bond_id = bondid AND physical_port_id = phyid;    
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- bond bakcup clear 清除 
CREATE OR REPLACE FUNCTION pf_net_unset_bondbackup_active(in_bond_uuid character varying, in_phy_uuid character varying  )
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  bondid numeric(19,0);
  phyid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  
  -- check 
  result.ret := pf_net_check_bondbackup( in_bond_uuid,in_phy_uuid);  
  IF result.ret > 0 THEN      
     return result;
  END IF;
  
  -- bond uuid  
  SELECT count(*) INTO is_exist FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;
  SELECT a.id INTO bondid FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
  
  -- phy uuid  
  SELECT count(*) INTO is_exist FROM port a, physical_port b WHERE a.id = b.port_id AND a.uuid = in_phy_uuid;
  SELECT a.id INTO phyid FROM port a, physical_port b WHERE a.id = b.port_id AND a.uuid = in_phy_uuid;  
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
  -- update item 
  result.id := bondid;
  --result.uuid := in_uuid;
  BEGIN 
    UPDATE bond_map SET backup_is_active = 0 WHERE bond_id = bondid AND physical_port_id = phyid;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 21 bond_lacp.txt



-- 判断bond lacp 能否设置/clear active nic
CREATE OR REPLACE FUNCTION pf_net_check_bondlacp( in_bond_uuid character varying, in_phy_uuid character varying  )
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  bondid numeric(19,0);
  phyid numeric(19,0);
  vlanid numeric(19,0);
  vnaid numeric(19,0);
  bondmode integer;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE integer := 13;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
        
  -- bond uuid  
  SELECT count(*) INTO is_exist FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;
  SELECT a.id INTO bondid FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;
  SELECT b.bond_mode INTO bondmode FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;  
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
  
  -- bond mode 必须是backup
  IF bondmode = 4 THEN 
     result := 0;
  ELSE
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
  
  -- phy uuid  
  SELECT count(*) INTO is_exist FROM port a, physical_port b WHERE a.id = b.port_id AND a.uuid = in_phy_uuid;
  SELECT a.id INTO phyid FROM port a, physical_port b WHERE a.id = b.port_id AND a.uuid = in_phy_uuid;  
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
  -- 判断bond_map中必须存在记录
  SELECT count(*) INTO is_exist FROM bond_map WHERE bond_id = bondid AND physical_port_id = phyid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
      
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- bond lacp set success 添加 
-- Function: pf_net_set_bondlacp_success(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_set_bondlacp_success(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_set_bondlacp_success(in_bond_uuid character varying, in_phy_uuid character varying  )
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  bondid numeric(19,0);
  phyid numeric(19,0);
  vlanid numeric(19,0);
  vnaid numeric(19,0);
  bondmode integer;
  is_exist_bondlacp integer;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_bondlacp( in_bond_uuid,in_phy_uuid);  
  IF result.ret > 0 THEN      
     return result;
  END IF;
  
  -- bond uuid  
  SELECT count(*) INTO is_exist FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;
  SELECT a.id INTO bondid FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;
  SELECT b.bond_mode INTO bondmode FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;  
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
  
  -- phy uuid  
  SELECT count(*) INTO is_exist FROM port a, physical_port b WHERE a.id = b.port_id AND a.uuid = in_phy_uuid;
  SELECT a.id INTO phyid FROM port a, physical_port b WHERE a.id = b.port_id AND a.uuid = in_phy_uuid;  
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
  -- update item 
  result.id := bondid;
  --result.uuid := in_uuid;
  BEGIN 
    -- DELETE bond_backup 
    DELETE FROM bond_backup WHERE bond_id = bondid;
        
    -- 将bond_map中 backup 信息清除 
    UPDATE bond_map SET backup_is_active = 0 WHERE bond_id = bondid;
    UPDATE bond_map SET lacp_is_success = 1 WHERE bond_id = bondid AND physical_port_id = phyid;    
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- bond lacp clear success
-- Function: pf_net_clear_bondlacp_success(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_clear_bondlacp_success(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_unset_bondlacp_success(in_bond_uuid character varying, in_phy_uuid character varying  )
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  bondid numeric(19,0);
  phyid numeric(19,0);
  vlanid numeric(19,0);
  vnaid numeric(19,0);
  bondmode integer;
  is_exist_bondlacp integer;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_bondlacp( in_bond_uuid,in_phy_uuid);  
  IF result.ret > 0 THEN      
     return result;
  END IF;
  
  -- bond uuid  
  SELECT count(*) INTO is_exist FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;
  SELECT a.id INTO bondid FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;
  SELECT b.bond_mode INTO bondmode FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;  
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
  
  -- phy uuid  
  SELECT count(*) INTO is_exist FROM port a, physical_port b WHERE a.id = b.port_id AND a.uuid = in_phy_uuid;
  SELECT a.id INTO phyid FROM port a, physical_port b WHERE a.id = b.port_id AND a.uuid = in_phy_uuid;  
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
  -- update item 
  result.id := bondid;
  --result.uuid := in_uuid;
  BEGIN 
        
    -- 将bond_map中 success 信息清除 
    UPDATE bond_map SET lacp_is_success = 0 WHERE bond_id = bondid AND physical_port_id = phyid;    
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 22 bond_map



-- 判断bond map 能否添加
CREATE OR REPLACE FUNCTION pf_net_check_add_bondmap(in_vna_uuid character varying, in_bond_uuid character varying, 
                           in_phy_uuid character varying, 
                           in_is_cfg integer,in_is_selected integer )
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  bondid numeric(19,0);
  phyid numeric(19,0);
  vlanid numeric(19,0);
  vnaid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE integer := 13;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
   -- vna uuid  
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
  
  
  -- bond uuid  
  SELECT count(*) INTO is_exist FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid AND a.vna_id = vnaid;
  SELECT a.id INTO bondid FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
            
  -- phy port uuid 
  SELECT count(*) INTO is_exist FROM port a, physical_port b WHERE a.id = b.port_id AND a.uuid = in_phy_uuid AND a.vna_id = vnaid;
  SELECT a.id INTO phyid FROM port a, physical_port b WHERE a.id = b.port_id AND a.uuid = in_phy_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
  -- phy port 是否已经加入其它bond口
  SELECT count(*) INTO is_exist FROM bond_map WHERE physical_port_id = phyid;
  IF is_exist > 0 THEN 
     result := RTN_ITEM_CONFLICT; 
     return result;
  END IF;
  
  -- phy port 如果已经作为switch的uplink，也不能再作为bond的成员口
  SELECT count(*) into is_exist FROM virtual_switch_map  WHERE port_id = phyid;
  IF is_exist > 0 THEN 
     result := RTN_ITEM_CONFLICT; 
     return result;
  END IF;
  
  --如果是sriov网口，如果有vnic在使用，也不能再作为bond的成员口
  SELECT count(*) into is_exist FROM sriov_vf_port a, vsi_sriov b WHERE a.port_id = b.sriov_vf_port_id AND 
             a.vna_id = vnaid AND a.sriov_port_id = phyid;
  IF is_exist > 0 THEN 
     result := RTN_RESOURCE_USING; 
     return result;
  END IF; 
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- VLAN MAP 添加 
-- Function: pf_net_add_bondmap(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_bondmap(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_bondmap(in_vna_uuid character varying, in_bond_uuid character varying, 
                           in_phy_uuid character varying, 
                           in_is_cfg integer,in_is_selected integer )
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  bondid numeric(19,0);
  phyid numeric(19,0);
  vlanid numeric(19,0);
  vnaid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_add_bondmap(in_vna_uuid, in_bond_uuid,in_phy_uuid,in_is_cfg,in_is_selected);  
  -- 重复添加 直接返回
  IF result.ret = RTN_HAS_SAME_ITEM THEN      
     result.ret := 0;
     return result;
  END IF;
  IF result.ret > 0 THEN      
     return result;
  END IF;
  
   -- vna uuid  
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
  -- bond uuid  
  SELECT count(*) INTO is_exist FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;
  SELECT a.id INTO bondid FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
            
  -- phy port uuid 
  SELECT count(*) INTO is_exist FROM port a, physical_port b WHERE a.id = b.port_id AND a.uuid = in_phy_uuid;
  SELECT a.id INTO phyid FROM port a, physical_port b WHERE a.id = b.port_id AND a.uuid = in_phy_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
  -- insert item 
  result.id := nextval('serial_bond_map');
  --result.uuid := in_uuid;
  BEGIN 
    INSERT INTO bond_map(id,vna_id,bond_id,physical_port_id,is_cfg,is_selected) 
            values(result.id,vnaid,bondid,phyid,in_is_cfg,in_is_selected);
            
    -- trigger
    SELECT count(*) INTO is_exist FROM sriov_port WHERE physical_port_id = phyid;
    IF is_exist > 0 THEN 
        result.ret := pf_net_tr_add_bondmap(bondid,phyid);
    END IF;
    
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 判断bond map  能否修改 
CREATE OR REPLACE FUNCTION pf_net_check_modify_bondmap(in_vna_uuid character varying,in_bond_uuid character varying, 
                           in_phy_uuid character varying, in_is_cfg integer,in_is_selected integer  )
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  bondid numeric(19,0);
  phyid numeric(19,0);
  vlanid numeric(19,0);
  vnaid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE integer := 13;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- vna uuid  
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
  -- bond uuid  
  SELECT count(*) INTO is_exist FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;
  SELECT a.id INTO bondid FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
            
  -- phy port uuid 
  SELECT count(*) INTO is_exist FROM port a, physical_port b WHERE a.id = b.port_id AND a.uuid = in_phy_uuid;
  SELECT a.id INTO phyid FROM port a, physical_port b WHERE a.id = b.port_id AND a.uuid = in_phy_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
  --判断bondmap中是否存在
  SELECT count(*) into is_exist FROM bond_map WHERE vna_id = vnaid AND bond_id = bondid AND physical_port_id = phyid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
                                  

                           
-- 修改bond map
CREATE OR REPLACE FUNCTION pf_net_modify_bondmap(in_vna_uuid character varying,in_bond_uuid character varying, 
                           in_phy_uuid character varying , in_is_cfg integer,in_is_selected integer )
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  bondid numeric(19,0);
  phyid numeric(19,0);
  vlanid numeric(19,0);
  vnaid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- check del 
  result.ret := pf_net_check_modify_bondmap(in_vna_uuid,in_bond_uuid,in_phy_uuid,in_is_cfg,in_is_selected);  
  IF result.ret > 0 THEN      
     return result;
  END IF;
  
  -- vna uuid  
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
  -- bond uuid  
  SELECT count(*) INTO is_exist FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;
  SELECT a.id INTO bondid FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
            
  -- phy port uuid 
  SELECT count(*) INTO is_exist FROM port a, physical_port b WHERE a.id = b.port_id AND a.uuid = in_phy_uuid;
  SELECT a.id INTO phyid FROM port a, physical_port b WHERE a.id = b.port_id AND a.uuid = in_phy_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
           
  -- update item
  result.ret := 0;
  BEGIN 
    UPDATE bond_map SET is_cfg = in_is_cfg, is_selected = in_is_selected  
       WHERE vna_id = vnaid AND bond_id = bondid AND physical_port_id = phyid;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 判断bond map  能否删除
CREATE OR REPLACE FUNCTION pf_net_check_del_bondmap(in_vna_uuid character varying,in_bond_uuid character varying, 
                           in_phy_uuid character varying )
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  bondid numeric(19,0);
  phyid numeric(19,0);
  vlanid numeric(19,0);
  vnaid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE integer := 13;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- vna uuid  
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
  -- bond uuid  
  SELECT count(*) INTO is_exist FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;
  SELECT a.id INTO bondid FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
            
  -- phy port uuid 
  SELECT count(*) INTO is_exist FROM port a, physical_port b WHERE a.id = b.port_id AND a.uuid = in_phy_uuid;
  SELECT a.id INTO phyid FROM port a, physical_port b WHERE a.id = b.port_id AND a.uuid = in_phy_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
  --判断bondmap中是否存在
  SELECT count(*) into is_exist FROM bond_map WHERE vna_id = vnaid AND bond_id = bondid AND physical_port_id = phyid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
  --- 判断该物理口是否是SRIOV nic在使用vf
  SELECT count(*) into is_exist FROM sriov_vf_port a, vsi_sriov b WHERE a.port_id = b.sriov_vf_port_id AND 
             a.vna_id = vnaid AND a.sriov_port_id = phyid;
  IF is_exist > 0 THEN 
     result := RTN_RESOURCE_USING; 
     return result;
  END IF; 
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
                
                 

-- 删除 netplane vlan map
CREATE OR REPLACE FUNCTION pf_net_del_bondmap(in_vna_uuid character varying,in_bond_uuid character varying, 
                           in_phy_uuid character varying )
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  bondid numeric(19,0);
  phyid numeric(19,0);
  vlanid numeric(19,0);
  vnaid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- check del 
  result.ret := pf_net_check_del_bondmap(in_vna_uuid,in_bond_uuid,in_phy_uuid);  
  IF result.ret > 0 THEN      
     return result;
  END IF;
  
  -- vna uuid  
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
  -- bond uuid  
  SELECT count(*) INTO is_exist FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;
  SELECT a.id INTO bondid FROM port a, bond b WHERE a.id = b.port_id AND a.uuid = in_bond_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
            
  -- phy port uuid 
  SELECT count(*) INTO is_exist FROM port a, physical_port b WHERE a.id = b.port_id AND a.uuid = in_phy_uuid;
  SELECT a.id INTO phyid FROM port a, physical_port b WHERE a.id = b.port_id AND a.uuid = in_phy_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
     
    
  -- del item
  result.ret := 0;
  BEGIN 
	-- trigger
	result.ret := pf_net_tr_del_bondmap(bondid,phyid);
	
    DELETE FROM bond_map WHERE vna_id = vnaid AND bond_id = bondid AND physical_port_id = phyid;
	
	
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;




-- 查询bond_map(通过vna_uuid)

CREATE OR REPLACE FUNCTION pf_net_query_bondmap_by_vna(in_vna_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    netplaneid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN  
    OPEN ref FOR SELECT vna_uuid,bond_uuid,bond_name,phy_uuid,phy_name,bond_mode,lacp_is_success,backup_is_active , is_cfg,is_selected 
       FROM v_net_bondmap_bond_phy    
       WHERE vna_uuid = in_vna_uuid;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 查询bond_map(通过 vm pg uuid )
CREATE OR REPLACE FUNCTION pf_net_query_bondmap_by_bond(in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    netplaneid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    
    OPEN ref FOR SELECT vna_uuid,bond_uuid,bond_name,phy_uuid,phy_name,bond_mode,lacp_is_success,backup_is_active, is_cfg,is_selected 
       FROM v_net_bondmap_bond_phy    
       WHERE bond_uuid = in_uuid;
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 查询bond_map(通过 phy uuid )
CREATE OR REPLACE FUNCTION pf_net_query_bondmap_by_phy(in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    netplaneid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    
    OPEN ref FOR SELECT vna_uuid,bond_uuid,bond_name,phy_uuid,phy_name,bond_mode,lacp_is_success,backup_is_active,is_cfg,is_selected 
       FROM v_net_bondmap_bond_phy    
       WHERE phy_uuid = in_uuid;
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;




-- 23 vswitch_port


-- vswitch port 添加 
-- Function: pf_net_add_vswitchport(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_vswitchport(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_vswitchport(in_vna_uuid character varying, in_type integer,in_name character varying,
        in_uuid character varying, in_state integer, in_is_broadcast integer,in_is_running integer,in_is_multicast integer,
        in_promiscuous integer, in_mtu integer, in_is_master integer, in_admin_state integer, in_is_online integer,
        in_is_port_cfg integer, in_is_consistency integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_same_port integer;
  is_same_port_ex integer;
  is_same_port_sriov integer;
  is_conflict integer;
  vnaid numeric(19,0);
  sriovportid numeric(19,0);
  portid numeric(19,0);
  is_exist integer; 
  db_uuid character varying;  
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- 检查vna uuid 是否有效
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  
  -- 是否已存在相同记录
  SELECT count(*) INTO is_same_port FROM port WHERE port_type = in_type AND vna_id = vnaid  AND name = in_name AND state = in_state 
         AND is_broadcast = in_is_broadcast AND is_running = in_is_running AND is_multicast = in_is_multicast AND promiscuous = in_promiscuous 
         AND mtu = in_mtu AND is_master = in_is_master AND admin_state = in_admin_state AND is_online = in_is_online 
         AND is_cfg = in_is_port_cfg AND is_consistency = in_is_consistency ;         
  SELECT id INTO portid FROM port WHERE port_type = in_type AND vna_id = vnaid  AND name = in_name AND state = in_state 
         AND is_broadcast = in_is_broadcast AND is_running = in_is_running AND is_multicast = in_is_multicast AND promiscuous = in_promiscuous 
         AND mtu = in_mtu AND is_master = in_is_master AND admin_state = in_admin_state AND is_online = in_is_online 
         AND is_cfg = in_is_port_cfg AND is_consistency = in_is_consistency ;         
  SELECT uuid INTO db_uuid FROM port WHERE port_type = in_type AND vna_id = vnaid  AND name = in_name AND state = in_state 
         AND is_broadcast = in_is_broadcast AND is_running = in_is_running AND is_multicast = in_is_multicast AND promiscuous = in_promiscuous 
         AND mtu = in_mtu AND is_master = in_is_master AND admin_state = in_admin_state AND is_online = in_is_online 
         AND is_cfg = in_is_port_cfg AND is_consistency = in_is_consistency ;               
  IF is_same_port > 0 THEN 
     SELECT count(*) INTO is_same FROM vswitch_port WHERE port_id = portid AND  port_type = in_type AND vna_id = vnaid; 
     IF is_same > 0 THEN
            result.ret := RTN_HAS_SAME_ITEM;   -- has same item
            result.id  := portid;
            result.uuid := db_uuid;
            return result;
     ELSE
           result.ret := RTN_ITEM_CONFLICT; -- conflict 
           return result;            
     END IF;     
  END IF;

  -- port name 名称是否冲突
  SELECT count(*) INTO is_conflict FROM  port WHERE vna_id = vnaid  AND name = in_name ;
  IF is_conflict > 0 THEN
     result.ret := RTN_ITEM_CONFLICT; -- conflict 
     return result;
  END IF;
    
  -- insert item
  result.id := nextval('serial_port');
  result.uuid := in_uuid;
  
  BEGIN 
    INSERT INTO port values(result.id,in_type,vnaid,in_name,in_uuid,in_state,in_is_broadcast,in_is_running,in_is_multicast,in_promiscuous,in_mtu,in_is_master,in_admin_state,in_is_online,in_is_port_cfg, in_is_consistency);
    INSERT INTO vswitch_port values(result.id,vnaid,in_type);    
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- port修改
-- Function: pf_net_modify_vswitchport(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_modify_vswitchport(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_modify_vswitchport(in_vna_uuid character varying, in_type integer,in_name character varying,
        in_uuid character varying, in_state integer, in_is_broadcast integer,in_is_running integer,in_is_multicast integer,
        in_promiscuous integer, in_mtu integer, in_is_master integer, in_admin_state integer, in_is_online integer,
        in_is_port_cfg integer, in_is_consistency integer) 
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  vnaid numeric(19,0);
  portid numeric(19,0);
  phyportid numeric(19,0);
  is_sriov_exist integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG  integer := 5;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  -- 检查vna uuid 是否有效
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  -- 检查port uuid 是否有效
  SELECT count(*) INTO is_exist FROM port WHERE uuid = in_uuid AND port_type = in_type;
  SELECT id INTO portid FROM port WHERE uuid = in_uuid AND port_type = in_type;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;   
  
  SELECT count(*) INTO is_exist FROM vswitch_port WHERE port_id = portid AND port_type = in_type AND vna_id = vnaid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF; 
  
  -- port name 
  SELECT count(*) INTO is_exist FROM port WHERE vna_id = vnaid AND name = in_name AND uuid = in_uuid;
  IF is_exist <= 0 THEN
     SELECT count(*) INTO is_exist FROM port WHERE vna_id = vnaid AND name = in_name ;
     IF is_exist > 0 THEN
       result.ret := RTN_ITEM_CONFLICT;-- 
       return result;          
     END IF;
  END IF;  
                 
  -- modify item
  result.id  := portid;
  result.uuid := in_uuid;
  BEGIN 
    UPDATE port SET name = in_name, state = in_state, is_broadcast =in_is_broadcast, is_multicast = in_is_multicast,
        promiscuous = in_promiscuous, mtu = in_mtu, is_master = in_is_master, admin_state = in_admin_state, is_online = in_is_online ,
        is_cfg = in_is_port_cfg , is_consistency = in_is_consistency 
        WHERE uuid = in_uuid AND port_type = in_type;            
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 删除port

CREATE OR REPLACE FUNCTION pf_net_del_vswitchport(in_port_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否存在记录
  SELECT count(*) INTO is_exist FROM port a, vswitch_port b WHERE a.vna_id = b.vna_id AND a.id = b.port_id AND a.port_type = b.port_type AND a.uuid = in_port_uuid ;  
  IF is_exist <= 0 THEN
     result.ret := RTN_ITEM_NO_EXIST;-- does not exist item
     return result;
  END IF;

  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM port WHERE uuid = in_port_uuid ;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

--- 查询vswitch port summary
--- 输出 name,uuid
-- Function: pf_net_query_vswitchport_summary()

-- DROP FUNCTION pf_net_query_vswitchport_summary();

CREATE OR REPLACE FUNCTION pf_net_query_vswitchport_summary(in_vna_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
  ref refcursor;
  is_same integer;
  is_exist integer;
  vnaid numeric(19,0);
  phyportid numeric(19,0);
BEGIN
      -- vna 
      SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
      SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
      IF is_exist <= 0 THEN 
        return ref;
      END IF;
          
      -- 
      OPEN ref FOR SELECT in_vna_uuid, a.uuid, a.name, a.port_type FROM port a, vswitch_port b  WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
          AND a.vna_id = vnaid  ;      
      return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 



-- 查询port 
-- 输出 uuid,name,description,mtu
-- Function: pf_net_query_vswitchport(character varying)

-- DROP FUNCTION pf_net_query_vswitchport(character varying);

CREATE OR REPLACE FUNCTION pf_net_query_vswitchport(in_port_uuid character varying)
  RETURNS t_vswitchport_query AS
$BODY$
DECLARE
  result  t_vswitchport_query;
  is_exist integer;
  vswitchportid numeric(19,0);
  RTN_ITEM_NO_EXIST integer :=4;
BEGIN
--  SELECT uuid into result.uuid, name into result.name,description into result.description,mtu into result.mtu FROM netplane WHERE uuid = $1;
  result.ret := 0;
  
  SELECT count(*) INTO is_exist FROM port a, vswitch_port b WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
           AND a.uuid = in_port_uuid ;
  SELECT a.id INTO vswitchportid FROM port a, vswitch_port b WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
           AND a.uuid = in_port_uuid ;
  IF is_exist <= 0 THEN   
     result.ret := RTN_ITEM_NO_EXIST; -- no exist item
     return result; 
  END IF;

   SELECT count(*) INTO is_exist FROM port a,vna b WHERE a.vna_id = b.id AND a.id = vswitchportid ;
   SELECT b.vna_uuid INTO result.vna_uuid FROM port a,vna b WHERE a.vna_id = b.id AND a.id = vswitchportid ;
   IF is_exist <= 0 THEN   
     result.ret := RTN_ITEM_NO_EXIST; -- no exist item
     return result; 
   END IF;
  
   result.uuid := in_port_uuid;
   -- port 
   SELECT port_type into result.port_type  FROM port WHERE uuid = in_port_uuid;
   SELECT name into result.name  FROM port WHERE uuid = in_port_uuid;
   SELECT state into result.state FROM port WHERE uuid = in_port_uuid;
   SELECT is_broadcast into result.is_broadcast  FROM port WHERE uuid = in_port_uuid;
   SELECT is_running into result.is_running  FROM port WHERE uuid = in_port_uuid;
   SELECT is_multicast into result.is_multicast  FROM port WHERE uuid = in_port_uuid;
   SELECT promiscuous into result.promiscuous  FROM port WHERE uuid = in_port_uuid;
   SELECT mtu into result.mtu  FROM port WHERE uuid = in_port_uuid;
   SELECT is_master into result.is_master  FROM port WHERE uuid = in_port_uuid;
   SELECT admin_state into result.admin_state  FROM port WHERE uuid = in_port_uuid;
   SELECT is_online into result.is_online  FROM port WHERE uuid = in_port_uuid;
   SELECT is_cfg into result.is_port_cfg  FROM port WHERE uuid = in_port_uuid;
   SELECT is_consistency into result.is_consistency  FROM port WHERE uuid = in_port_uuid;
      
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 



-- 24 port_ip







--- 查询 port ip
--- 输出 name,uuid,
-- Function: pf_net_query_portip()

-- DROP FUNCTION pf_net_query_portip();

CREATE OR REPLACE FUNCTION pf_net_query_portip(in_port_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer;
    portid numeric(19,0);
BEGIN
    SELECT count(*) INTO is_exist FROM port WHERE uuid = in_port_uuid;
    SELECT id INTO portid FROM port WHERE uuid = in_port_uuid;    
    IF is_exist <= 0 THEN 
        return ref;
    END IF;  
    
    OPEN ref FOR SELECT  b.vna_uuid, b.port_uuid, b.port_name, b.port_type , a.port_name AS port_ip_name, a.ip, a.mask, a.is_cfg, a.is_online  
      FROM  port_ip a, v_net_vna_port b WHERE a.port_id = b.port_id AND b.port_uuid = in_port_uuid;
    
    RETURN ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


CREATE OR REPLACE FUNCTION pf_net_query_portip_by_vna(in_vna_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer;
    portid numeric(19,0);
BEGIN
    SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
    IF is_exist <= 0 THEN 
        return ref;
    END IF;  
    
    OPEN ref FOR SELECT  b.vna_uuid, b.port_uuid, b.port_name, b.port_type, a.port_name AS port_ip_name, a.ip, a.mask, a.is_cfg, a.is_online  
      FROM  port_ip a, v_net_vna_port b WHERE a.port_id = b.port_id AND b.vna_uuid = in_vna_uuid;
    
    RETURN ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- port ip 添加 
-- Function: pf_net_add_portip(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_portip(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_portip(in_port_uuid character varying, in_port_ip_name character varying, 
            in_ip character varying, in_mask character varying, in_is_cfg integer, in_is_online integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  portid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否已存在相同记录
  SELECT count(*) INTO is_exist FROM port WHERE uuid = in_port_uuid;
  SELECT id INTO portid FROM port WHERE uuid = in_port_uuid;    
  IF is_exist <= 0 THEN 
      result.ret := RTN_ITEM_NO_EXIST;
      return result;
  END IF;   

  -- ip 冲突检查
  -- portid + port_ip_name 为唯一
  SELECT count(*) INTO is_exist FROM port_ip WHERE port_id = portid AND port_name = in_port_ip_name;
  IF is_exist > 0 THEN 
      result.ret := RTN_ITEM_CONFLICT;
      return result;
  END IF;
  
  -- insert item
  result.id := nextval('serial_port_ip');
  result.uuid := $1;
  BEGIN 
    INSERT INTO port_ip (id, port_id, port_name,ip,mask,is_cfg,is_online) values(result.id, portid, in_port_ip_name, in_ip, in_mask, in_is_cfg, in_is_online);
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 修改port ip
CREATE OR REPLACE FUNCTION pf_net_modify_portip(in_port_uuid character varying, in_port_ip_name character varying, 
            in_ip character varying, in_mask character varying, in_is_cfg integer, in_is_online integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  portid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否已存在相同记录
  SELECT count(*) INTO is_exist FROM port WHERE uuid = in_port_uuid;
  SELECT id INTO portid FROM port WHERE uuid = in_port_uuid;    
  IF is_exist <= 0 THEN 
      result.ret := RTN_ITEM_NO_EXIST;
      return result;
  END IF;   

  -- portid + port_ip_name 为唯一 检查是否存在
  SELECT count(*) INTO is_exist FROM port_ip WHERE port_id = portid AND port_name = in_port_ip_name;
  IF is_exist <= 0 THEN 
      result.ret := RTN_ITEM_NO_EXIST;
      return result;
  END IF;
  
  -- update item
  --result.id := nextval('serial_port_ip');
  --result.uuid := $1;
  BEGIN 
    UPDATE port_ip SET ip = in_ip, mask = in_mask, is_cfg = in_is_cfg, is_online = in_is_online 
       WHERE port_id = portid AND port_name = in_port_ip_name;    
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
            
            
-- 删除port ip
-- Function: pf_net_del_portip(character varying)

-- DROP FUNCTION pf_net_del_portip(character varying);

CREATE OR REPLACE FUNCTION pf_net_del_portip(in_port_uuid character varying, in_port_ip_name character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  portid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否存在记录
  SELECT count(*) INTO is_exist FROM port WHERE uuid = in_port_uuid;
  SELECT id INTO portid FROM port WHERE uuid = in_port_uuid;    
  IF is_exist <= 0 THEN 
      result.ret := RTN_ITEM_NO_EXIST;
      return result;
  END IF;   
  
  SELECT count(*) INTO is_same FROM port_ip WHERE port_id = portid AND port_name = in_port_ip_name ;  
  IF is_same <= 0 THEN
     result.ret := RTN_ITEM_NO_EXIST;-- does not exist item
     return result;
  END IF;
    
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM port_ip WHERE port_id = portid AND port_name = in_port_ip_name ;   
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 删除ip by port
CREATE OR REPLACE FUNCTION pf_net_del_portip_by_port(in_port_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  portid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否存在记录
  SELECT count(*) INTO is_exist FROM port WHERE uuid = in_port_uuid;
  SELECT id INTO portid FROM port WHERE uuid = in_port_uuid;    
  IF is_exist <= 0 THEN 
      result.ret := RTN_ITEM_NO_EXIST;
      return result;
  END IF;   
      
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM port_ip WHERE port_id = portid;  
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 25 port_mac







--- 查询 port mac 
--- 输出 name,uuid,
-- Function: pf_net_query_port_mac()

-- DROP FUNCTION pf_net_query_port_mac();

CREATE OR REPLACE FUNCTION pf_net_query_port_mac(in_port_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer;
    portid numeric(19,0);
BEGIN
    SELECT count(*) INTO is_exist FROM port WHERE uuid = in_port_uuid;
    SELECT id INTO portid FROM port WHERE uuid = in_port_uuid;    
    IF is_exist <= 0 THEN 
        return ref;
    END IF;  
    
    OPEN ref FOR SELECT  b.vna_uuid, b.port_uuid, b.port_name, b.port_type , a.mac 
      FROM  port_mac a, v_net_vna_port b WHERE a.port_id = b.port_id AND b.port_uuid = in_port_uuid;
    
    RETURN ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


CREATE OR REPLACE FUNCTION pf_net_query_port_mac_by_vna(in_vna_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer;
    portid numeric(19,0);
BEGIN
    SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
    IF is_exist <= 0 THEN 
        return ref;
    END IF;  
    
    OPEN ref FOR SELECT  b.vna_uuid, b.port_uuid, b.port_name, b.port_type, a.mac 
      FROM  port_mac a, v_net_vna_port b WHERE a.port_id = b.port_id AND b.vna_uuid = in_vna_uuid;
    
    RETURN ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- port mac 添加 
-- Function: pf_net_add_port_mac(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_port_mac(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_port_mac(in_port_uuid character varying, in_mac character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  portid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否已存在相同记录
  SELECT count(*) INTO is_exist FROM port WHERE uuid = in_port_uuid;
  SELECT id INTO portid FROM port WHERE uuid = in_port_uuid;    
  IF is_exist <= 0 THEN 
      result.ret := RTN_ITEM_NO_EXIST;
      return result;
  END IF;   

  -- mac 不做冲突检查
  
  -- insert item
  result.id := nextval('serial_port_mac');
  result.uuid := $1;
  BEGIN 
    INSERT INTO port_mac (id, port_id, mac) values(result.id, portid, in_mac);
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 删除port mac
-- Function: pf_net_del_port_mac(character varying)

-- DROP FUNCTION pf_net_del_port_mac(character varying);

CREATE OR REPLACE FUNCTION pf_net_del_port_mac(in_port_uuid character varying, in_mac character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  portid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否存在记录
  SELECT count(*) INTO is_exist FROM port WHERE uuid = in_port_uuid;
  SELECT id INTO portid FROM port WHERE uuid = in_port_uuid;    
  IF is_exist <= 0 THEN 
      result.ret := RTN_ITEM_NO_EXIST;
      return result;
  END IF;   
  
  SELECT count(*) INTO is_same FROM port_mac WHERE port_id = portid AND mac = in_mac ;  
  IF is_same <= 0 THEN
     result.ret := RTN_ITEM_NO_EXIST;-- does not exist item
     return result;
  END IF;
    
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM port_mac WHERE port_id = portid AND mac = in_mac;  
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



CREATE OR REPLACE FUNCTION pf_net_del_port_mac_by_port(in_port_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  portid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否存在记录
  SELECT count(*) INTO is_exist FROM port WHERE uuid = in_port_uuid;
  SELECT id INTO portid FROM port WHERE uuid = in_port_uuid;    
  IF is_exist <= 0 THEN 
      result.ret := RTN_ITEM_NO_EXIST;
      return result;
  END IF;   
  
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM port_mac WHERE port_id = portid;  
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 26 port_group



-- physical port 添加 
-- Function: pf_net_add_portgroup(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_portgroup(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_portgroup(in_name character varying, in_pg_type integer,
        in_uuid character varying, in_acl character varying, in_qos character varying,in_allowed_priorities character varying,
        in_receive_bandwidth_limit character varying, in_receive_bandwidth_reserve character varying, 
        in_default_vlan_id integer, in_promiscuous integer, in_macvlan_filter_enable integer, 
        in_allowed_transmit_macs character varying, in_allowed_transmit_macvlans character varying, 
        in_policy_block_override character varying, in_policy_vlan_override character varying, 
        in_version integer,in_mgr_id character varying, in_type_id character varying,
        in_allow_mac_change integer, in_switchport_mode integer,
        in_is_cfg_netplane integer, in_netplane_uuid character varying,
        in_mtu integer,
        in_trunk_native_vlan_num integer,in_access_vlan_num integer,in_access_isolate integer , in_desc character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_same_pg integer;
  is_conflict integer;
  npmtu integer;
  pgid numeric(19,0);
  netplaneid numeric(19,0);
  is_exist integer; 
  db_pg_uuid character varying;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG integer := 5;
  RTN_PARAM_INVALID integer := 7;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  IF in_is_cfg_netplane > 0 THEN 
     SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_netplane_uuid;
     SELECT id INTO netplaneid FROM netplane WHERE uuid = in_netplane_uuid;
	 SELECT mtu INTO npmtu FROM netplane WHERE uuid = in_netplane_uuid;
     IF is_exist <= 0 THEN 
        result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
        return result;
     END IF;  
	 IF npmtu < in_mtu THEN 
	    result.ret := RTN_NETPLANE_MTU_LESS_PG;   -- PG MTU 不能大于 netplane mtu 
        return result;
	 END IF;
  END IF;
    
  -- 不支持重复添加 
  -- 判断name是否冲突
  SELECT count(*) INTO is_exist FROM port_group WHERE name = in_name;
  IF is_exist > 0 THEN 
     result.ret := RTN_ITEM_CONFLICT; -- conflict 
     return result;
  END IF;
  
  -- 检查switch_mode
  IF in_switchport_mode < 0 THEN 
    result.ret := RTN_PARAM_INVALID; -- param invalid 
    return result;   
  END IF;
  
  IF in_switchport_mode > 1  THEN 
    result.ret := RTN_PARAM_INVALID; -- param invalid  
    return result;   
  END IF;
  
  -- 检查pg_type 
  
  -- insert item
  result.id := nextval('serial_port_group');
  result.uuid := in_uuid;
    
  BEGIN 
    IF in_is_cfg_netplane > 0 THEN     
        INSERT INTO port_group(id,name,pg_type,uuid,acl,qos,allowed_priorities,
            receive_bandwidth_limit,receive_bandwidth_reserve,default_vlan_id,
            promiscuous,macvlan_filter_enable,allowed_transmit_macs,allowed_transmit_macvlans,
            policy_block_override,policy_vlan_override,version,mgr_id,type_id,
            allow_mac_change,switchport_mode,netplane_id, mtu, description) 
        values(result.id,in_name,in_pg_type,in_uuid,in_acl,in_qos,in_allowed_priorities,
            in_receive_bandwidth_limit,in_receive_bandwidth_reserve,in_default_vlan_id,
            in_promiscuous,in_macvlan_filter_enable,in_allowed_transmit_macs,in_allowed_transmit_macvlans,
            in_policy_block_override,in_policy_vlan_override,in_version,in_mgr_id,in_type_id,
            in_allow_mac_change,in_switchport_mode,netplaneid, in_mtu,in_desc);     
    ELSE 
       INSERT INTO port_group(id,name,pg_type,uuid,acl,qos,allowed_priorities,
            receive_bandwidth_limit,receive_bandwidth_reserve,default_vlan_id,
            promiscuous,macvlan_filter_enable,allowed_transmit_macs,allowed_transmit_macvlans,
            policy_block_override,policy_vlan_override,version,mgr_id,type_id,
            allow_mac_change,switchport_mode, mtu,description) 
        values(result.id,in_name,in_pg_type,in_uuid,in_acl,in_qos,in_allowed_priorities,
            in_receive_bandwidth_limit,in_receive_bandwidth_reserve,in_default_vlan_id,
            in_promiscuous,in_macvlan_filter_enable,in_allowed_transmit_macs,in_allowed_transmit_macvlans,
            in_policy_block_override,in_policy_vlan_override,in_version,in_mgr_id,in_type_id,
            in_allow_mac_change,in_switchport_mode, in_mtu,in_desc);    
    END IF;
    
    -- uplink pg
    IF in_pg_type = 0 THEN 
      INSERT INTO uplink_pg values(result.id, in_pg_type);
    END IF;
    
    -- kernel_pg
    IF in_pg_type = 10 THEN 
      INSERT INTO kernel_pg values(result.id, in_pg_type);
    END IF;
    IF in_pg_type = 11 THEN 
      INSERT INTO kernel_pg values(result.id, in_pg_type);
    END IF;
    
    -- vm_pg
    IF in_pg_type = 20 THEN 
      INSERT INTO vm_pg values(result.id, in_pg_type);
    END IF;
    IF in_pg_type = 21 THEN 
      INSERT INTO vm_pg values(result.id, in_pg_type);
    END IF;
    
    -- pg_switchport_trunk 
    IF in_switchport_mode = 0 THEN 
        INSERT INTO pg_switchport_trunk values(result.id, in_switchport_mode, in_trunk_native_vlan_num );
    END IF;
    
    IF in_switchport_mode = 1 THEN 
      IF in_pg_type = 21 THEN 
          INSERT INTO pg_switchport_access_isolate values(result.id, in_pg_type,in_switchport_mode, in_access_isolate );      
      ELSE
          INSERT INTO pg_switchport_access_vlan values(result.id, in_pg_type,in_switchport_mode, in_access_vlan_num );      
      END IF;
    END IF; 
    
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- port修改
-- Function: pf_net_modify_portgroup(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_modify_portgroup(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_modify_portgroup(in_name character varying, in_pg_type integer,
        in_uuid character varying, in_acl character varying, in_qos character varying,in_allowed_priorities character varying,
        in_receive_bandwidth_limit character varying, in_receive_bandwidth_reserve character varying, 
        in_default_vlan_id integer, in_promiscuous integer, in_macvlan_filter_enable integer, 
        in_allowed_transmit_macs character varying, in_allowed_transmit_macvlans character varying, 
        in_policy_block_override character varying, in_policy_vlan_override character varying, 
        in_version integer,in_mgr_id character varying, in_type_id character varying,
        in_allow_mac_change integer, in_switchport_mode integer,
        in_is_cfg_netplane integer, in_netplane_uuid character varying,        
        in_mtu integer,
        in_trunk_native_vlan_num integer,in_access_vlan_num integer,in_access_isolate integer, in_desc character varying  )
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  is_modify_netplane integer;
  npmtu integer;
  netplaneid numeric(19,0);
  oldnetplaneid numeric(19,0);
  pgid numeric(19,0);
  db_pg_type integer;
  ismodifyswitchportmode integer;
  oldswitchportmode integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG integer := 5;
  RTN_PARAM_INVALID integer := 7;
BEGIN
  result.ret := 0;
  
  -- 入参检查 后面做  
  -- netplane check
  IF in_is_cfg_netplane > 0 THEN 
     SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_netplane_uuid;
     SELECT id INTO netplaneid FROM netplane WHERE uuid = in_netplane_uuid;
     IF is_exist <= 0 THEN 
        result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
        return result;
     END IF;  
  END IF;
    
  -- uuid check
  SELECT count(*) INTO is_exist FROM port_group WHERE uuid = in_uuid;
  SELECT pg_type INTO db_pg_type FROM port_group WHERE uuid = in_uuid;  
  SELECT id INTO pgid FROM port_group WHERE uuid = in_uuid;  
  SELECT switchport_mode INTO oldswitchportmode FROM port_group WHERE uuid = in_uuid;   
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST; -- conflict 
     return result;
  END IF;
  
  -- pg_type check 
  SELECT count(*) INTO is_exist FROM port_group WHERE uuid = in_uuid AND db_pg_type = in_pg_type;
  IF is_exist <= 0 THEN 
     result.ret := RTN_PARAM_INVALID; -- param invalid 
     return result;
  END IF;
    
  -- 判断name是否冲突
  SELECT count(*) INTO is_exist FROM port_group WHERE name = in_name AND  uuid = in_uuid;
  IF is_exist <= 0 THEN 
     SELECT count(*) INTO is_exist FROM port_group WHERE name = in_name;
     IF is_exist > 0 THEN 
        result.ret := RTN_ITEM_CONFLICT; -- conflict 
        return result;
     END IF;
  END IF;
  
  -- 检查switch_mode
  IF in_switchport_mode < 0 THEN 
    result.ret := RTN_PARAM_INVALID; -- param invalid 
    return result;   
  END IF;
  
  IF in_switchport_mode > 1  THEN 
    result.ret := RTN_PARAM_INVALID; -- param invalid  
    return result;   
  END IF;
  
  -- 检查 switch_mode 是否变化
  IF in_switchport_mode = oldswitchportmode THEN 
      ismodifyswitchportmode := 0;
  ELSE
      ismodifyswitchportmode := 1;
  END IF;
  
  -- 检查pg_type 
  
  -- 检查netplane modify，之前已经配置了netplane，能将之修改，但必须mtu >= pg_mtu
  is_modify_netplane := 0;
  SELECT count(*) INTO is_exist FROM port_group WHERE uuid = in_uuid AND netplane_id is not null;
  SELECT netplane_id INTO oldnetplaneid FROM port_group WHERE uuid = in_uuid AND netplane_id is not null; 
  IF is_exist > 0 THEN 
     IF in_is_cfg_netplane <= 0 THEN 
        result.ret := RTN_PARAM_INVALID; -- param invalid  原先配置netplane，不能将之修改为NULL
        return result; 
     ELSE        
		SELECT mtu INTO npmtu FROM netplane WHERE id = netplaneid;
        IF npmtu < in_mtu THEN 
            result.ret := RTN_NETPLANE_MTU_LESS_PG; -- param invalid  netplane mtu 小于pg_mtu，不能将之修改
            return result;
        END IF;    
     END IF;
  END IF; 
          
  -- modify item
  result.id  := pgid;
  result.uuid := in_uuid;
  BEGIN 
    
    -- 将老的switch mode 数据删除
    IF ismodifyswitchportmode > 0 THEN 
       -- 修改switchport mode需要删除以前记录
       IF oldswitchportmode = 0 THEN 
            DELETE FROM pg_switchport_trunk WHERE pg_id = pgid;            
       ELSE 
          IF in_pg_type = 21 THEN 
              DELETE FROM pg_switchport_access_isolate WHERE pg_id = pgid;
          ELSE 
              DELETE FROM pg_switchport_access_vlan WHERE pg_id = pgid;
          END IF;
       END IF;    
    END IF;
    
    IF in_is_cfg_netplane > 0 THEN     
       UPDATE port_group SET name = in_name, pg_type = in_pg_type,acl = in_acl,qos = in_qos,allowed_priorities = in_allowed_priorities,
            receive_bandwidth_limit = in_receive_bandwidth_limit,receive_bandwidth_reserve = in_receive_bandwidth_reserve ,default_vlan_id = in_default_vlan_id,
            promiscuous = in_promiscuous,macvlan_filter_enable = in_macvlan_filter_enable,allowed_transmit_macs = in_allowed_transmit_macs,
            allowed_transmit_macvlans = in_allowed_transmit_macvlans,
            policy_block_override = in_policy_block_override,policy_vlan_override = in_policy_vlan_override,version = in_version,mgr_id = in_mgr_id,type_id = in_type_id,
            allow_mac_change = in_allow_mac_change ,switchport_mode = in_switchport_mode, netplane_id = netplaneid, mtu = in_mtu, description = in_desc    
          WHERE uuid = in_uuid ;    
    ELSE 
       UPDATE port_group SET name = in_name, pg_type = in_pg_type,acl = in_acl,qos = in_qos,allowed_priorities = in_allowed_priorities,
            receive_bandwidth_limit = in_receive_bandwidth_limit,receive_bandwidth_reserve = in_receive_bandwidth_reserve ,default_vlan_id = in_default_vlan_id,
            promiscuous = in_promiscuous,macvlan_filter_enable = in_macvlan_filter_enable,allowed_transmit_macs = in_allowed_transmit_macs,
            allowed_transmit_macvlans = in_allowed_transmit_macvlans,
            policy_block_override = in_policy_block_override,policy_vlan_override = in_policy_vlan_override,version = in_version,mgr_id = in_mgr_id,type_id = in_type_id,
            allow_mac_change = in_allow_mac_change ,switchport_mode = in_switchport_mode, mtu = in_mtu, description = in_desc 
          WHERE uuid = in_uuid ;  
    END IF;
    
    
    -- 
    -- pg_switchport_trunk 
    IF in_switchport_mode = 0 THEN 
        IF ismodifyswitchportmode > 0 THEN 
            INSERT INTO pg_switchport_trunk values(pgid, in_switchport_mode, in_trunk_native_vlan_num );
        ELSE 
            UPDATE pg_switchport_trunk SET native_vlan_num = in_trunk_native_vlan_num WHERE pg_id = pgid;
        END IF;
    END IF;
    
    -- PG access
    IF in_switchport_mode = 1 THEN 
      IF in_pg_type = 21 THEN 
          IF ismodifyswitchportmode > 0 THEN 
              INSERT INTO pg_switchport_access_isolate values(pgid, in_pg_type,in_switchport_mode, in_access_isolate );      
          ELSE
              UPDATE pg_switchport_access_isolate SET isolate_no = in_access_isolate  WHERE pg_id = pgid;  
          END IF;
      ELSE
          IF ismodifyswitchportmode > 0 THEN 
              INSERT INTO pg_switchport_access_vlan values(pgid, in_pg_type,in_switchport_mode, in_access_vlan_num );    
          ELSE
              UPDATE pg_switchport_access_vlan SET vlan_num = in_access_vlan_num  WHERE pg_id = pgid; 
          END IF;          
      END IF;
    END IF; 
            
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


CREATE OR REPLACE FUNCTION pf_net_check_del_portgroup(in_uuid character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  pgtype integer; 
  vnaid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
BEGIN
  result := 0;
  -- 入参检查 后面做
          
  --查询是否存在
  SELECT count(*) INTO is_exist FROM port_group WHERE uuid = in_uuid ;  
  SELECT pg_type INTO pgtype FROM port_group WHERE uuid = in_uuid ;  
  IF is_exist <= 0 THEN
     result := RTN_ITEM_NO_EXIST;-- does not exist item
     return result;
  END IF;
  
  -- 资源是否被使用
  --uplink 
  IF pgtype = 0 THEN 
     SELECT count(*) INTO is_exist FROM port_group a, virtual_switch b WHERE a.id = b.uplink_pg_id AND  a.uuid = in_uuid ;  
     IF is_exist > 0 THEN 
         result := RTN_RESOURCE_USING;-- using 
         return result;
     END IF;
     
     SELECT count(*) INTO is_exist FROM port_group a, wildcast_cfg_switch b WHERE a.id = b.upg_id AND  a.uuid = in_uuid ;  
     IF is_exist > 0 THEN 
         result := RTN_RESOURCE_USING;-- using 
         return result;
     END IF;
  END IF;
  
  -- kernel 
  IF pgtype >= 10 THEN 
      IF pgtype <= 11 THEN 
           SELECT count(*) INTO is_exist FROM port_group a, kernel_port b WHERE a.id = b.kernel_pg_id AND  a.uuid = in_uuid ;  
           IF is_exist > 0 THEN 
              result := RTN_RESOURCE_USING;-- using 
              return result;
           END IF;
      END IF;
  END IF;
  
  -- vm 
  IF pgtype >= 20 THEN 
      IF pgtype <= 21 THEN 
           SELECT count(*) INTO is_exist FROM port_group a, vsi_profile b WHERE a.id = b.vm_pg_id AND  a.uuid = in_uuid ;  
           IF is_exist > 0 THEN 
              result := RTN_RESOURCE_USING;-- using 
              return result;
           END IF;
           SELECT count(*) INTO is_exist FROM port_group a, logic_network b WHERE a.id = b.vm_pg_id AND  a.uuid = in_uuid ;  
           IF is_exist > 0 THEN 
              result := RTN_RESOURCE_USING;-- using 
              return result;
           END IF;
           SELECT count(*) INTO is_exist FROM port_group a, vlan_map b WHERE a.id = b.vm_pg_id AND  a.uuid = in_uuid ;  
           IF is_exist > 0 THEN 
              result := RTN_RESOURCE_USING;-- using 
              return result;
           END IF;           
      END IF;
  END IF;

  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;

-- 删除port

CREATE OR REPLACE FUNCTION pf_net_del_portgroup(in_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  pgtype integer; 
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否存在记录
  result.ret = pf_net_check_del_portgroup(in_uuid);
  IF result.ret > 0 THEN
     return result;
  END IF;
  
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM port_group WHERE uuid = in_uuid ;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


--- 查询phy port summary
--- 输出 name,uuid
-- Function: pf_net_query_portgroup_summary()

-- DROP FUNCTION pf_net_query_portgroup_summary();

CREATE OR REPLACE FUNCTION pf_net_query_portgroup_summary()
  RETURNS refcursor AS
$BODY$
DECLARE
      ref refcursor;
BEGIN
      OPEN ref FOR SELECT uuid, name, pg_type,switchport_mode FROM port_group;
      return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 查询port group
-- 输出 uuid,name,description,mtu
-- Function: pf_net_query_portgroup(character varying)

-- DROP FUNCTION pf_net_query_portgroup(character varying);

CREATE OR REPLACE FUNCTION pf_net_query_portgroup(in_uuid character varying)
  RETURNS t_portgroup_query AS
$BODY$
DECLARE
  result  t_portgroup_query;
  is_exist integer;
  pgid numeric(19,0);
  iscfgnetplane integer;
  pgtype integer;
  swithmode integer;
  RTN_ITEM_NO_EXIST integer :=4;
BEGIN

  result.ret := 0;
  SELECT count(*) INTO is_exist FROM port_group WHERE uuid = in_uuid;
  SELECT id INTO pgid FROM port_group WHERE uuid = in_uuid;
  SELECT pg_type INTO pgtype FROM port_group WHERE uuid = in_uuid;
  SELECT switchport_mode INTO swithmode FROM port_group WHERE uuid = in_uuid; 
  IF is_exist > 0 THEN
     result.ret := 0;
  ELSE
     result.ret := RTN_ITEM_NO_EXIST; -- no exist item
     return result;     
  END IF;
   
   result.uuid := in_uuid;
       
   -- port_group 
   SELECT pg_type into result.pg_type  FROM port_group WHERE uuid = in_uuid;
   SELECT name into result.name  FROM port_group WHERE uuid = in_uuid;
   SELECT acl into result.acl FROM port_group WHERE uuid = in_uuid;
   SELECT qos into result.qos  FROM port_group WHERE uuid = in_uuid;
   SELECT allowed_priorities into result.allowed_priorities  FROM port_group WHERE uuid = in_uuid;
   SELECT receive_bandwidth_limit into result.receive_bandwidth_limit  FROM port_group WHERE uuid = in_uuid;
   SELECT receive_bandwidth_reserve into result.receive_bandwidth_reserve  FROM port_group WHERE uuid = in_uuid;
      
   SELECT default_vlan_id into result.default_vlan_id  FROM port_group WHERE uuid = in_uuid;
   SELECT promiscuous into result.promiscuous  FROM port_group WHERE uuid = in_uuid;
   SELECT macvlan_filter_enable into result.macvlan_filter_enable  FROM port_group WHERE uuid = in_uuid;
   SELECT allowed_transmit_macs into result.allowed_transmit_macs  FROM port_group WHERE uuid = in_uuid;
   SELECT allowed_transmit_macvlans into result.allowed_transmit_macvlans  FROM port_group WHERE uuid = in_uuid;
   SELECT policy_block_override into result.policy_block_override  FROM port_group WHERE uuid = in_uuid;
   SELECT policy_vlan_override into result.policy_vlan_override  FROM port_group WHERE uuid = in_uuid;
   SELECT version into result.version  FROM port_group WHERE uuid = in_uuid;
   SELECT mgr_id into result.mgr_id  FROM port_group WHERE uuid = in_uuid;
   SELECT type_id into result.type_id  FROM port_group WHERE uuid = in_uuid;
   SELECT allow_mac_change into result.allow_mac_change  FROM port_group WHERE uuid = in_uuid;
   SELECT switchport_mode into result.switchport_mode  FROM port_group WHERE uuid = in_uuid;
   
   SELECT count(*) INTO is_exist FROM port_group WHERE  uuid = in_uuid AND netplane_id is not null;
   SELECT count(*) INTO is_exist FROM port_group WHERE  uuid = in_uuid AND netplane_id is not null;
   IF is_exist > 0 THEN 
       result.is_cfg_netplane := 1;
       SELECT a.uuid INTO result.netplane_uuid FROM netplane a, port_group b WHERE a.id = b.netplane_id AND b.uuid = in_uuid;
   ELSE 
       result.is_cfg_netplane := 0;
   END IF;
   
   SELECT mtu into result.mtu FROM port_group WHERE uuid = in_uuid;
   SELECT description into result.description FROM port_group WHERE uuid = in_uuid;
    
   -- trunk 
   IF swithmode = 0 THEN 
      SELECT native_vlan_num INTO result.trunk_native_vlan_num FROM pg_switchport_trunk WHERE pg_id = pgid;   
   END IF;
   
   -- access 
   IF swithmode = 1 THEN 
      IF pgtype = 21 THEN 
          SELECT isolate_no INTO result.access_isolate_no FROM pg_switchport_access_isolate WHERE pg_id = pgid;   
      ELSE 
          SELECT vlan_num INTO result.access_vlan_num FROM pg_switchport_access_vlan WHERE pg_id = pgid;   
      END IF;
   END IF;
   
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;





-- 27 pg_trunk_vlan_range



-- physical port 添加 
-- Function: pf_net_add_pgtrunkvlanrange(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_pgtrunkvlanrange(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_pgtrunkvlanrange(in_uuid character varying, 
               in_vlan_begin integer,in_vlan_end integer )
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_same_pg integer;
  is_conflict integer;
  pgid numeric(19,0);
  is_exist integer; 
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
   
  -- uuid    
  SELECT count(*) INTO is_exist FROM port_group WHERE uuid = in_uuid;
  SELECT id INTO pgid FROM port_group WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
     return result;
  END IF;  
  
  SELECT count(*) INTO is_exist FROM pg_switchport_trunk WHERE pg_id = pgid;
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
     return result;
  END IF;  

  -- vlan check 
  IF in_vlan_begin > in_vlan_end THEN 
     result.ret := RTN_PARAM_INVALID;   -- item has not exist.
     return result;
  END IF;  

  IF in_vlan_begin < 0 THEN 
     result.ret := RTN_PARAM_INVALID;   -- item has not exist.
     return result;
  END IF;
  IF in_vlan_end < 0 THEN 
     result.ret := RTN_PARAM_INVALID;   -- item has not exist.
     return result;
  END IF;  
  IF in_vlan_begin > 4094 THEN 
     result.ret := RTN_PARAM_INVALID;   -- item has not exist.
     return result;
  END IF;
  IF in_vlan_end > 4094 THEN 
     result.ret := RTN_PARAM_INVALID;   -- item has not exist.
     return result;
  END IF;
      
  -- 不支持重复添加 
/*  
  -- 不能重叠设置
  SELECT count(*) INTO is_exist FROM pg_switchport_trunk_vlan_range WHERE pg_switchport_trunk_id = pgid 
       AND (( in_vlan_begin >= vlan_begin AND in_vlan_begin <= vlan_end ) OR 
           ( in_vlan_end >= vlan_begin AND in_vlan_end <= vlan_end ) OR 
           ( in_vlan_begin <= vlan_begin AND in_vlan_end >= vlan_end ) );
  IF is_exist > 0 THEN 
      result.ret := RTN_PARAM_INVALID;   -- item has not exist.
      return result;
  END IF;
*/  
   -- insert item
  result.id := nextval('serial_port_group_trunk_vlan_range');
  --result.uuid := in_uuid;
  
  BEGIN 
     INSERT INTO pg_switchport_trunk_vlan_range 
        values(result.id,pgid,in_vlan_begin,in_vlan_end);        
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- port修改
-- Function: pf_net_modify_pgtrunkvlanrange(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_modify_pgtrunkvlanrange(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_modify_pgtrunkvlanrange(in_uuid character varying, 
               in_old_vlan_begin integer,in_old_vlan_end integer, in_new_vlan_begin integer,in_new_vlan_end integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  pgid numeric(19,0);
  pgtrunkvlanrangeid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
BEGIN
  result.ret := 0;
  
  -- 入参检查 后面做  
  -- uuid    
  SELECT count(*) INTO is_exist FROM port_group WHERE uuid = in_uuid;
  SELECT id INTO pgid FROM port_group WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
     return result;
  END IF;  
  
    SELECT count(*) INTO is_exist FROM pg_switchport_trunk WHERE pg_id = pgid;
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
     return result;
  END IF;  

  -- pg_switchport_trunk_vlan_range 
  SELECT count(*) INTO is_exist FROM pg_switchport_trunk_vlan_range WHERE pg_switchport_trunk_id = pgid 
        AND vlan_begin = in_old_vlan_begin AND vlan_end = in_old_vlan_end;
  SELECT id INTO pgtrunkvlanrangeid FROM pg_switchport_trunk_vlan_range WHERE pg_switchport_trunk_id = pgid 
        AND vlan_begin = in_old_vlan_begin AND vlan_end = in_old_vlan_end;        
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
     return result;
  END IF;  
  
  -- vlan check 
  IF in_new_vlan_begin > in_new_vlan_end THEN 
     result.ret := RTN_PARAM_INVALID;   -- item has not exist.
     return result;
  END IF;  
  IF in_new_vlan_begin < 0 THEN 
     result.ret := RTN_PARAM_INVALID;   -- item has not exist.
     return result;
  END IF;
  IF in_new_vlan_end < 0 THEN 
     result.ret := RTN_PARAM_INVALID;   -- item has not exist.
     return result;
  END IF;  
  IF in_new_vlan_begin > 4094 THEN 
     result.ret := RTN_PARAM_INVALID;   -- item has not exist.
     return result;
  END IF;
  IF in_new_vlan_end > 4094 THEN 
     result.ret := RTN_PARAM_INVALID;   -- item has not exist.
     return result;
  END IF;
            
  -- modify 不检查是否冲突，由应用保准  
  
  -- modify item
  result.id  := pgtrunkvlanrangeid;
  --result.uuid := in_uuid;
  BEGIN         
    UPDATE pg_switchport_trunk_vlan_range SET vlan_begin = in_new_vlan_begin, vlan_end = in_new_vlan_end     
          WHERE id = pgtrunkvlanrangeid ;   
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 删除port

CREATE OR REPLACE FUNCTION pf_net_del_pgtrunkvlanrange(in_uuid character varying,in_vlan_begin integer,in_vlan_end integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  pgid numeric(19,0);
  pgtrunkvlanrangeid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- uuid    
  SELECT count(*) INTO is_exist FROM port_group WHERE uuid = in_uuid;
  SELECT id INTO pgid FROM port_group WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
     return result;
  END IF;  
  SELECT count(*) INTO is_exist FROM pg_switchport_trunk WHERE pg_id = pgid;
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
     return result;
  END IF; 

  -- pg_switchport_trunk_vlan_range 
  SELECT count(*) INTO is_exist FROM pg_switchport_trunk_vlan_range WHERE pg_switchport_trunk_id = pgid 
        AND vlan_begin = in_vlan_begin AND vlan_end = in_vlan_end;
  SELECT id INTO pgtrunkvlanrangeid FROM pg_switchport_trunk_vlan_range WHERE pg_switchport_trunk_id = pgid 
        AND vlan_begin = in_vlan_begin AND vlan_end = in_vlan_end;        
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
     return result;
  END IF;  
  
  -- del 不检查是否冲突，由应用保准  
  
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM pg_switchport_trunk_vlan_range WHERE id = pgtrunkvlanrangeid ;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 查询port group 的trunk vlan range 

CREATE OR REPLACE FUNCTION pf_net_query_pgtrunkvlanrange(in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    pgid numeric(19,0);
BEGIN
    SELECT count(*) into is_exist FROM port_group a, pg_switchport_trunk b WHERE a.id = b.pg_id AND a.switchport_mode = b.switchport_mode 
          AND a.uuid = in_uuid;
    SELECT id into pgid FROM port_group a, pg_switchport_trunk b WHERE a.id = b.pg_id AND a.switchport_mode = b.switchport_mode  
          AND a.uuid = in_uuid;
    IF is_exist <= 0 THEN 
       return ref;
    END IF;
    
    OPEN ref FOR SELECT in_uuid, vlan_begin, vlan_end FROM pg_switchport_trunk_vlan_range WHERE pg_switchport_trunk_id = pgid ORDER BY vlan_begin;
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;




-- 28 logicnetwork



-- 判断逻辑网络能否添加
CREATE OR REPLACE FUNCTION pf_net_check_add_logicnetwork(in_name character varying, in_uuid character varying, 
                           in_pg_uuid character varying,in_des character varying, in_ip_mode integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  pgid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_SWITCH_UPLINKPORT_INVALID integer := 9;
  RTN_PORT_HAS_JOIN_OTHER_SWITCH integer := 10;
  RTN_PG_MUST_CFG_NETPLANE   integer := 13;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- pg uuid 
  SELECT count(*) INTO is_exist FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type 
        AND a.uuid = in_pg_uuid;
  SELECT a.id INTO pgid FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type 
        AND a.uuid = in_pg_uuid;    
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
  
  -- pg 必须已经是配置了netplane
  SELECT count(*) INTO is_exist FROM port_group WHERE id = pgid AND netplane_id is not null; 
  IF is_exist <= 0 THEN 
     result := RTN_PG_MUST_CFG_NETPLANE; 
     return result;
  END IF;
  
  -- ip mode 只能为0,1,2
  IF (in_ip_mode  = 0 OR in_ip_mode = 1 OR in_ip_mode = 2)THEN 
      result := 0;
  ELSE     
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
    
  -- 查看是否重复添加 
  SELECT count(*) INTO is_exist FROM logic_network WHERE name = in_name;
  IF is_exist > 0 THEN 
     SELECT count(*) INTO is_same FROM logic_network WHERE name = in_name AND vm_pg_id = pgid AND description = in_des AND ip_bootprotocolmode = in_ip_mode;
     IF is_same > 0 THEN 
         result := RTN_HAS_SAME_ITEM; 
         return result;
     ELSE 
         result := RTN_ITEM_CONFLICT; 
         return result;
     END IF; 
  END IF;
  
          
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 逻辑网络添加 
-- Function: pf_net_add_logicnetwork(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_logicnetwork(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_logicnetwork(in_name character varying, in_uuid character varying, 
                 in_pg_uuid character varying,in_des character varying,in_ip_mode integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  pgid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_add_logicnetwork(in_name,in_uuid,in_pg_uuid,in_des,in_ip_mode);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  -- pg uuid 
  SELECT count(*) INTO is_exist FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type 
        AND a.uuid = in_pg_uuid;
  SELECT a.id INTO pgid FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type 
        AND a.uuid = in_pg_uuid;    
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
  -- 不支持重复添加 

  -- insert item 
  result.id := nextval('serial_logicnetwork');
  result.uuid := in_uuid;
  BEGIN 
    INSERT INTO logic_network(id,name,uuid,vm_pg_id,description,ip_bootprotocolmode) values(result.id,in_name,in_uuid,pgid,in_des,in_ip_mode);
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;

                 

-- check modify        
CREATE OR REPLACE FUNCTION pf_net_check_modify_logicnetwork(in_name character varying, in_uuid character varying, 
                 in_pg_uuid character varying,in_des character varying,in_ip_mode integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  oldpgid numeric(19,0);
  pgid numeric(19,0);
  lnid numeric(19,0);
  ipmode integer; 
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE   integer := 13;
BEGIN
  result := 0;
  -- 入参检查 后面做
  
  -- pg uuid 
  SELECT count(*) INTO is_exist FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type 
        AND a.uuid = in_pg_uuid;
  SELECT a.id INTO pgid FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type 
        AND a.uuid = in_pg_uuid;    
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
  
  -- pg 必须已经是配置了netplane
  SELECT count(*) INTO is_exist FROM port_group WHERE id = pgid AND netplane_id is not null; 
  IF is_exist <= 0 THEN 
     result := RTN_PG_MUST_CFG_NETPLANE; 
     return result;
  END IF;
  
  -- logicnetwork uuid 
  SELECT count(*) INTO is_exist FROM logic_network WHERE uuid = in_uuid; 
  SELECT ip_bootprotocolmode INTO ipmode FROM logic_network WHERE uuid = in_uuid; 
  SELECT id INTO lnid FROM logic_network WHERE uuid = in_uuid; 
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
  
  --如果已有logic_network_ip_pool记录， 则ip mode不允许修改,
  IF ipmode = in_ip_mode THEN 
     result := 0; 
  ELSE
     SELECT count(*) INTO is_exist FROM logic_network_ip_pool WHERE logic_network_id = lnid;
	 IF is_exist > 0 THEN 	 
         result := RTN_RESOURCE_USING; 
         return result;
	 END IF;	 
  END IF;
    
  -- 查看name 是否和其他冲突 
  SELECT count(*) INTO is_exist FROM logic_network WHERE uuid = in_uuid AND name = in_name; 
  IF is_exist <=  0 THEN 
     SELECT count(*) INTO is_exist FROM logic_network WHERE name = in_name; 
     IF is_exist > 0 THEN 
         result := RTN_ITEM_CONFLICT; 
         return result;
     END IF;
  END IF;
  
  -- 修改logicnetwork pg uuid 
  --如果VNIC存在logic network uuid的情况下，不能修改pg_id
  SELECT count(*) INTO is_exist FROM logic_network WHERE uuid = in_uuid AND vm_pg_id = pgid; 
  IF is_exist <= 0 THEN 
     -- modify pg uuid 
     SELECT count(*) INTO is_exist FROM vnic WHERE logic_network_uuid = in_uuid;
     IF is_exist > 0 THEN 
         result := RTN_RESOURCE_USING; 
         return result;
     END IF;
  END IF;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
                         


CREATE OR REPLACE FUNCTION pf_net_modify_logicnetwork(in_name character varying, in_uuid character varying, 
                 in_pg_uuid character varying,in_des character varying,in_ip_mode integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  pgid numeric(19,0);
  lgnetworkid numeric(19,0);
  oldIpmode integer := 0;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG  integer := 5;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  
  -- 检查能否add
  result.ret := pf_net_check_modify_logicnetwork(in_name,in_uuid,in_pg_uuid,in_des,in_ip_mode);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  -- pg uuid 
  SELECT count(*) INTO is_exist FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type 
        AND a.uuid = in_pg_uuid;
  SELECT a.id INTO pgid FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type 
        AND a.uuid = in_pg_uuid;    
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
  
  -- logicnetwork 
  SELECT count(*) INTO is_exist FROM logic_network WHERE uuid = in_uuid; 
  SELECT id INTO lgnetworkid FROM logic_network WHERE uuid = in_uuid; 
  SELECT ip_bootprotocolmode INTO oldIpmode FROM logic_network WHERE uuid = in_uuid;   
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
  
  -- modify item
  result.id  := lgnetworkid;
  result.uuid := in_uuid;
  BEGIN 
  
    -- 设置为none后，需将range删除 
    IF in_ip_mode = 2 THEN 
		DELETE  FROM logic_network_ip_range WHERE logic_network_id = lgnetworkid;
	END IF;
    
    UPDATE logic_network SET name = in_name, ip_bootprotocolmode = in_ip_mode,  vm_pg_id = pgid, description =in_des 
        WHERE uuid = in_uuid;    
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;

                                 
-- check logic network del
CREATE OR REPLACE FUNCTION pf_net_check_del_logicnetwork(in_uuid character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
BEGIN
  result := 0;
  -- 入参检查 后面做
    
  -- logicnetwork uuid 
  SELECT count(*) INTO is_exist FROM logic_network WHERE uuid = in_uuid; 
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
      
  --如果VNIC存在logic network uuid的情况下，不能删除
  SELECT count(*) INTO is_exist FROM vnic WHERE logic_network_uuid = in_uuid;
  IF is_exist > 0 THEN 
      result := RTN_RESOURCE_USING; 
      return result;
  END IF;

  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



CREATE OR REPLACE FUNCTION pf_net_del_logicnetwork(in_uuid character varying) 
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  switchid numeric(19,0);
  portid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  result.ret := pf_net_check_del_logicnetwork(in_uuid);
  IF result.ret > 0 THEN 
     return result;
  END IF;
    
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM logic_network WHERE uuid = in_uuid ;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 查询logicnetwork (通过 uuid)


CREATE OR REPLACE FUNCTION pf_net_query_logicnetwork(in_uuid character varying)
  RETURNS t_logicnetwork_query AS
$BODY$
DECLARE
    result t_logicnetwork_query;
    is_exist integer; 
    pgid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    result.ret := 0;
    
    -- logic_network uuid 
    SELECT count(*) INTO is_exist FROM logic_network WHERE uuid = in_uuid;
    SELECT vm_pg_id INTO pgid FROM logic_network WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
         result.ret := RTN_ITEM_NO_EXIST; 
         return result;
    END IF;  
    
    result.uuid  := in_uuid;
    SELECT name INTO result.name FROM  logic_network WHERE uuid = in_uuid;
    
    -- pg uuid 
    SELECT uuid INTO result.pg_uuid FROM port_group WHERE id = pgid;
  
    SELECT description INTO result.des FROM  logic_network WHERE uuid = in_uuid;
    
    SELECT ip_bootprotocolmode INTO result.ipmode FROM  logic_network WHERE uuid = in_uuid;
    
    return result;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 查询logicnetwork (通过netplane uuid)

CREATE OR REPLACE FUNCTION pf_net_query_logicnetwork_by_netplane(in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    -- swtich uuid 
    SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF;  
  
    OPEN ref FOR SELECT netplane_uuid, netplane_name,netplane_mtu,
         pg_uuid,pg_name,pg_mtu,
         logicnetwork_uuid,logicnetwork_name,logicnetwork_ipmode 
      FROM v_net_logicnetwork_portgroup_netplane 
      WHERE netplane_uuid = in_uuid;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 查询logicnetwork (通过pg uuid)
CREATE OR REPLACE FUNCTION pf_net_query_logicnetwork_by_portgroup(in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    -- pg uuid 
    SELECT count(*) INTO is_exist FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND 
        uuid = in_uuid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF;  
  
    OPEN ref FOR SELECT netplane_uuid, netplane_name,netplane_mtu,
         pg_uuid,pg_name,pg_mtu,
         logicnetwork_uuid,logicnetwork_name,logicnetwork_ipmode
      FROM v_net_logicnetwork_portgroup_netplane 
      WHERE pg_uuid = in_uuid;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;





                                 
-- 29 logicnetwork_iprange 



-- 判断逻辑网络IP RANGE 能否添加
CREATE OR REPLACE FUNCTION pf_net_check_add_logicnetwork_iprange(in_uuid character varying, 
                           in_num_begin numeric(19), in_num_end numeric(19),in_num_mask numeric(19),
                           in_str_begin character varying,in_str_end character varying,in_str_mask character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  lgnetworkid numeric(19,0);
  netplaneuuid character varying;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_SWITCH_UPLINKPORT_INVALID integer := 9;
  RTN_PORT_HAS_JOIN_OTHER_SWITCH integer := 10;
  RTN_PG_MUST_CFG_NETPLANE   integer := 13;
  RTN_MUST_CFG_VALID_RANGE integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- logicnetwork uuid 
  SELECT count(*) INTO is_exist FROM logic_network WHERE uuid = in_uuid;
  SELECT id INTO lgnetworkid FROM logic_network WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- begin end 
  IF in_num_begin > in_num_end THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  
/*  
  -- ip begin end mask 互相不能重叠    由应用控制   
  SELECT count(*) INTO is_exist FROM logic_network_ip_range WHERE 
logic_network_id = lgnetworkid  

       AND (( in_num_begin >= ip_begin_num AND in_num_begin <= ip_end_num ) OR 
           ( in_num_end >= ip_begin_num AND in_num_end <= ip_end_num ) OR 
           ( in_num_begin <= ip_begin_num AND in_num_end >= ip_end_num ) );  
  IF is_exist > 0 THEN 
     result := RTN_ITEM_CONFLICT; 
     return result;
  END IF;  
 
*/  
/*
  --netplane 必须首先配置 iprange
  SELECT count(*) into is_exist FROM v_net_logicnetwork_portgroup_netplane WHERE logicnetwork_uuid = in_uuid;
  SELECT netplane_uuid into netplaneuuid FROM v_net_logicnetwork_portgroup_netplane WHERE logicnetwork_uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
  --ip begin end必须在netplane iprange 范围内
  SELECT count(*) into is_exist FROM ip_range a, netplane b WHERE b.uuid = netplaneuuid AND a.netplane_id = b.id  
              AND (( ip_begin_num <= in_num_begin ) AND (ip_end_num >= in_num_end)) 
              AND ( mask_num = in_num_mask );
  IF is_exist <= 0 THEN 
     result := RTN_MUST_CFG_VALID_RANGE; 
     return result;
  END IF; 
*/  
  


  SELECT count(*) into is_exist FROM v_net_logicnetwork_portgroup_netplane WHERE logicnetwork_uuid = in_uuid;
  SELECT netplane_uuid into netplaneuuid FROM v_net_logicnetwork_portgroup_netplane WHERE logicnetwork_uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
  
  -- ip begin end不能和该网络平面下其他的逻辑网络IP RANGE重合冲突
  SELECT count(*) into is_exist FROM logic_network_ip_range a, v_net_logicnetwork_portgroup_netplane b WHERE b.netplane_uuid = netplaneuuid AND a.logic_network_id = b.logicnetwork_id 
              AND (b.logicnetwork_uuid <> in_uuid)
              AND (( ip_begin_num <= in_num_end) AND (ip_end_num >= in_num_begin)) 
              AND ( mask_num = in_num_mask );
  IF is_exist > 0 THEN 
     result := RTN_MUST_CFG_VALID_RANGE; 
     return result;
  END IF;  
  
  return result;  
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 逻辑网络IP RANGE添加 
-- Function: pf_net_add_logicnetwork_iprange(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_logicnetwork_iprange(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_logicnetwork_iprange(in_uuid character varying, 
                            in_num_begin numeric(19), in_num_end numeric(19),in_num_mask numeric(19),
                           in_str_begin character varying,in_str_end character varying,in_str_mask character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  lgnetworkid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_add_logicnetwork_iprange(in_uuid,in_num_begin,in_num_end,in_num_mask, 
                            in_str_begin,in_str_end,in_str_mask);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  -- logicnetwork uuid 
  SELECT count(*) INTO is_exist FROM logic_network WHERE uuid = in_uuid;
  SELECT id INTO lgnetworkid FROM logic_network WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
    
  -- 不支持重复添加 

  -- insert item 
  result.id := nextval('serial_logicnetwork_ip_range');
  --result.uuid := in_uuid;
  BEGIN 
    INSERT INTO logic_network_ip_range(id,logic_network_id,ip_begin,ip_end,mask,ip_begin_num,ip_end_num,mask_num) 
            values(result.id,lgnetworkid,in_str_begin, in_str_end,in_str_mask,in_num_begin,in_num_end,in_num_mask);
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
                 
     

-- 删除 logicnetwork iprange
CREATE OR REPLACE FUNCTION pf_net_del_logicnetwork_iprange(in_uuid character varying,
                            in_num_begin numeric(19), in_num_end numeric(19),in_num_mask numeric(19),
                           in_str_begin character varying,in_str_end character varying,in_str_mask character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  lgnetworkid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- del 不作检查，由应用保准
  
  -- logicnetwork uuid 
  SELECT count(*) INTO is_exist FROM logic_network WHERE uuid = in_uuid;
  SELECT id INTO lgnetworkid FROM logic_network WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
    
  SELECT count(*) INTO is_exist FROM logic_network_ip_range WHERE logic_network_id = lgnetworkid 
                AND ip_begin_num = in_num_begin AND ip_end_num = in_num_end AND mask_num = in_num_mask 
                AND ip_begin = in_str_begin AND ip_end = in_str_end AND mask = in_str_mask;
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;                
    
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM logic_network_ip_range WHERE logic_network_id = lgnetworkid 
                AND ip_begin_num = in_num_begin AND ip_end_num = in_num_end AND mask_num = in_num_mask 
                AND ip_begin = in_str_begin AND ip_end = in_str_end AND mask = in_str_mask;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;




-- 检查 check logicnetwork ip range 是否已有资源被分配
CREATE OR REPLACE FUNCTION pf_net_query_logicnetwork_iprange_alloc(in_uuid character varying,
                            in_num_begin numeric(19), in_num_end numeric(19))
  RETURNS t_query_total AS
$BODY$
DECLARE
  result  t_query_total;
  is_same integer;
  is_exist integer;
  lgnetworkid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
   -- logicnetwork uuid 
  SELECT count(*) INTO is_exist FROM logic_network WHERE uuid = in_uuid;
  SELECT id INTO lgnetworkid FROM logic_network WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
  
  -- begin end 
  IF in_num_begin > in_num_end THEN 
      result.ret  := RTN_PARAM_INVALID; 
      return result;
  END IF;
        
  SELECT count(*) INTO result.total_num FROM logic_network_ip_pool WHERE logic_network_id = lgnetworkid 
                AND (ip_num >= in_num_begin AND ip_num <= in_num_end );
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
                           
                           

-- 查询logicnetwork_iprange(通过logic network uuid, type)

CREATE OR REPLACE FUNCTION pf_net_query_logicnetwork_iprange(in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    lgnetworkid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    -- logicnetwork uuid 
    SELECT count(*) INTO is_exist FROM logic_network WHERE uuid = in_uuid;
    SELECT id INTO lgnetworkid FROM logic_network WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF;  
    
    SELECT count(*) INTO is_exist FROM logic_network_ip_range  
      WHERE logic_network_id = lgnetworkid ;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF; 
  
    OPEN ref FOR SELECT in_uuid, ip_begin_num,ip_end_num, mask_num, ip_begin, ip_end, mask 
      FROM logic_network_ip_range  
      WHERE logic_network_id = lgnetworkid ;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 30 logicnetwork_ippool


-- 判断逻辑网络IP POOL 能否添加
CREATE OR REPLACE FUNCTION pf_net_check_add_logicnetwork_ippool(in_uuid character varying, 
                           in_num_ip numeric(19), in_num_mask numeric(19),
                           in_str_ip character varying,in_str_mask character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  lgnetworkid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- logicnetwork uuid 
  SELECT count(*) INTO is_exist FROM logic_network WHERE uuid = in_uuid;
  SELECT id INTO lgnetworkid FROM logic_network WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
  
  -- ip range 必须已经配置
  SELECT count(*) INTO is_exist FROM logic_network_ip_range WHERE logic_network_id = lgnetworkid  
       AND ( in_num_ip >= ip_begin_num AND in_num_ip <= ip_end_num ) ;  
  IF is_exist <= 0 THEN 
     result := RTN_MUST_CFG_VALID_RANGE; 
     return result;
  END IF;  
  
  -- ip pool 不能冲突
  SELECT count(*) INTO is_exist FROM logic_network_ip_pool WHERE logic_network_id = lgnetworkid  
       AND ip_num = in_num_ip;
  IF is_exist > 0 THEN 
     result := RTN_ITEM_CONFLICT; 
     return result;
  END IF;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 逻辑网络IP POOL 添加 
-- Function: pf_net_add_logicnetwork_ippool(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_logicnetwork_ippool(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_logicnetwork_ippool(in_uuid character varying, 
                            in_num_ip numeric(19), in_num_mask numeric(19),
                           in_str_ip character varying,in_str_mask character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  lgnetworkid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_add_logicnetwork_ippool(in_uuid,in_num_ip,in_num_mask,
                            in_str_ip,in_str_mask);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  -- logicnetwork uuid 
  SELECT count(*) INTO is_exist FROM logic_network WHERE uuid = in_uuid;
  SELECT id INTO lgnetworkid FROM logic_network WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
    
  -- 不支持重复添加 

  -- insert item 
  result.id := nextval('serial_logicnetwork_ip_pool');
  --result.uuid := in_uuid;
  BEGIN 
    INSERT INTO logic_network_ip_pool(id,logic_network_id,ip,mask,ip_num,mask_num) 
            values(result.id,lgnetworkid,in_str_ip, in_str_mask,in_num_ip,in_num_mask);
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;

                 

-- 判断逻辑网络IP POOL 能否删除 
CREATE OR REPLACE FUNCTION pf_net_check_del_logicnetwork_ippool(in_uuid character varying, 
                           in_num_ip numeric(19), in_num_mask numeric(19),
                           in_str_ip character varying,in_str_mask character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  lgnetworkid numeric(19,0);
  lnippoolid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- logicnetwork uuid 
  SELECT count(*) INTO is_exist FROM logic_network WHERE uuid = in_uuid;
  SELECT id INTO lgnetworkid FROM logic_network WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- ip pool 必须存在
  SELECT count(*) INTO is_exist FROM logic_network_ip_pool WHERE logic_network_id = lgnetworkid  
       AND ip_num = in_num_ip AND mask_num = in_num_mask 
       AND ip = in_str_ip AND mask = in_str_mask; 
  SELECT id INTO lnippoolid FROM logic_network_ip_pool WHERE logic_network_id = lgnetworkid  
       AND ip_num = in_num_ip AND mask_num = in_num_mask 
       AND ip = in_str_ip AND mask = in_str_mask; 
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
  
  -- 判断vsi是否在用 
  -- 查看该资源是否在使用
  SELECT count(*) INTO is_exist FROM vsi_profile WHERE logic_network_ip_id = lnippoolid;
  IF is_exist > 0 THEN 
     result := RTN_RESOURCE_USING; 
     return result;
  END IF;
  
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
    

-- 删除 logicnetwork ip pool
CREATE OR REPLACE FUNCTION pf_net_del_logicnetwork_ippool(in_uuid character varying, 
                            in_num_ip numeric(19), in_num_mask numeric(19),
                           in_str_ip character varying,in_str_mask character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  lgnetworkid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- del 不作检查，由应用保准
  
  -- logicnetwork uuid 
  SELECT count(*) INTO is_exist FROM logic_network WHERE uuid = in_uuid;
  SELECT id INTO lgnetworkid FROM logic_network WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
    
  SELECT count(*) INTO is_exist FROM logic_network_ip_pool WHERE logic_network_id = lgnetworkid 
                AND ip_num = in_num_ip AND mask_num = in_num_mask 
                AND ip = in_str_ip AND mask = in_str_mask;
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;                
    
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM logic_network_ip_pool WHERE logic_network_id = lgnetworkid 
                AND ip_num = in_num_ip AND mask_num = in_num_mask 
                AND ip = in_str_ip AND mask = in_str_mask;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;




-- 查询logicnetwork_ippool(通过logic network uuid, type)

CREATE OR REPLACE FUNCTION pf_net_query_logicnetwork_ippool(in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    lgnetworkid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    -- logicnetwork uuid 
    SELECT count(*) INTO is_exist FROM logic_network WHERE uuid = in_uuid;
    SELECT id INTO lgnetworkid FROM logic_network WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF;  
    
    SELECT count(*) INTO is_exist FROM logic_network_ip_pool  
      WHERE logic_network_id = lgnetworkid ;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF; 
  
    OPEN ref FOR SELECT in_uuid,  ip_num, mask_num, ip, mask 
      FROM logic_network_ip_pool   
      WHERE logic_network_id = lgnetworkid;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;




-- 查询logicnetwork_ippool(通过logic network uuid, begin, end)

CREATE OR REPLACE FUNCTION pf_net_query_logicnetwork_ippool_by_lnrange(in_uuid character varying, in_begin numeric(19,0), in_end numeric(19,0))
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    lgnetworkid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    -- logicnetwork uuid 
    SELECT count(*) INTO is_exist FROM logic_network WHERE uuid = in_uuid;
    SELECT id INTO lgnetworkid FROM logic_network WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF;  
    
    SELECT count(*) INTO is_exist FROM logic_network_ip_pool  
      WHERE logic_network_id = lgnetworkid ;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF; 
  
    OPEN ref FOR SELECT in_uuid,  ip_num, mask_num, ip, mask 
      FROM logic_network_ip_pool   
      WHERE logic_network_id = lgnetworkid
	      AND (ip_num >= in_begin AND ip_num <= in_end);
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 31 vswitch

-- 查询vswitch vsi num数
CREATE OR REPLACE FUNCTION pf_net_query_vswitch_vsi_num(in_uuid character varying )
 RETURNS t_query_total AS
$BODY$
DECLARE
  result  t_query_total;
  is_conflict integer;
  switchid numeric(19,0);
  is_exist integer; 
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- 检查pg uuid 是否有效
  SELECT count(*) INTO is_exist FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.uuid = in_uuid;
  SELECT id INTO switchid FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.uuid = in_uuid;  
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  

  -- switch vsi_profile
  SELECT count(*) INTO result.total_num FROM virtual_switch a, vsi_profile b WHERE a.switch_id = b.virtual_switch_id AND 
        a.switch_id = switchid;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 查询vswitch vsi num数 通过switch vna uuid
CREATE OR REPLACE FUNCTION pf_net_query_vswitchvna_vsi_num(in_switch_uuid character varying ,in_vna_uuid character varying )
 RETURNS t_query_total AS
$BODY$
DECLARE
  result  t_query_total;
  is_conflict integer;
  switchid numeric(19,0);
  vnaid numeric(19,0);
  is_exist integer; 
  kernelnum integer := 0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- 检查switch uuid 是否有效
  SELECT count(*) INTO is_exist FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.uuid = in_switch_uuid;
  SELECT id INTO switchid FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.uuid = in_switch_uuid;  
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  -- 检查 vna uuid 是否有效
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  

  -- switch vsi_profile
  SELECT count(*) INTO result.total_num 
  FROM virtual_switch a, vsi_profile b ,
  v_net_vna_hc_cluster c,
  v_net_vsi_vnic d
  WHERE a.switch_id = b.virtual_switch_id AND 
        a.switch_id = switchid AND 
        d.vsi_virtual_switch_id = a.switch_id AND  
        c.cluster_name = d.vnic_cluster_name AND 
        c.module_name = d.vnic_host_name AND 
        c.vna_id = vnaid;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 查询vswitch kernel num数
CREATE OR REPLACE FUNCTION pf_net_query_vswitch_kernel_num(in_uuid character varying )
 RETURNS t_query_total AS
$BODY$
DECLARE
  result  t_query_total;
  is_conflict integer;
  switchid numeric(19,0);
  is_exist integer; 
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- 检查pg uuid 是否有效
  SELECT count(*) INTO is_exist FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.uuid = in_uuid;
  SELECT id INTO switchid FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.uuid = in_uuid;  
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  

  -- switch kernel
  SELECT count(*) INTO result.total_num FROM kernel_port 
  WHERE virtual_switch_id = switchid;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 查询vswitch kernel num数 通过switch vna uuid
-- 将kernel 数目也添加进来 
CREATE OR REPLACE FUNCTION pf_net_query_vswitchvna_kernel_num(in_switch_uuid character varying ,in_vna_uuid character varying )
 RETURNS t_query_total AS
$BODY$
DECLARE
  result  t_query_total;
  is_conflict integer;
  switchid numeric(19,0);
  vnaid numeric(19,0);
  is_exist integer; 
  kernelnum integer := 0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- 检查switch uuid 是否有效
  SELECT count(*) INTO is_exist FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.uuid = in_switch_uuid;
  SELECT id INTO switchid FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.uuid = in_switch_uuid;  
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  -- 检查 vna uuid 是否有效
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  

  -- switch kernel num
  SELECT count(*) INTO result.total_num 
  FROM virtual_switch a, kernel_port b
  WHERE a.switch_id = b.virtual_switch_id AND 
        a.switch_id = switchid AND 
        b.vna_id = vnaid;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 判断能否add switch 
CREATE OR REPLACE FUNCTION pf_net_can_add_vswitch(in_switch_type integer, in_name character varying,
        in_uuid character varying, in_state integer, in_max_mtu integer,
        in_evb_mode integer,in_pg_uuid character varying,in_max_nic_num integer, in_is_active integer )
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  switchid numeric(19,0);
  is_exist integer; 
  pgid numeric(19,0);
  pg_mtu integer;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
BEGIN
  result := 0;
  -- 入参检查 后面做
  
  -- 检查pg uuid 是否有效
  SELECT count(*) INTO is_exist FROM port_group a, uplink_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type AND a.uuid = in_pg_uuid;
  SELECT a.id INTO pgid FROM port_group a, uplink_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type AND a.uuid = in_pg_uuid;  
  SELECT a.mtu INTO pg_mtu FROM port_group a, uplink_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type AND a.uuid = in_pg_uuid;  
  IF is_exist <= 0 THEN 
    result := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  

  -- switch type
  IF in_switch_type < 1 THEN 
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;
  
  IF in_switch_type > 2 THEN 
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;
  
  -- 检查switch name 是否冲突 
  SELECT count(*) INTO is_exist FROM switch WHERE name = in_name;
  IF is_exist > 0 THEN 
    result := RTN_ITEM_CONFLICT;   
    return result;
  END IF;
  
  -- check max_mtu 
  /* switch mtu 将废弃 
  IF in_max_mtu > pg_mtu THEN 
     result := RTN_SWITCH_MTU_LESS_PG;   
     return result;
  END IF;
  */
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

        
        
-- physical port 添加 
-- Function: pf_net_add_vswitch(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_vswitch(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_vswitch(in_switch_type integer, in_name character varying,
        in_uuid character varying, in_state integer, in_max_mtu integer,
        in_evb_mode integer,in_pg_uuid character varying,in_max_nic_num integer, in_is_active integer )
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_conflict integer;
  switchid numeric(19,0);
  is_exist integer; 
  pgid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- 检查能否add
  result.ret := pf_net_can_add_vswitch(in_switch_type,in_name,in_uuid,in_state,in_max_mtu,in_evb_mode,in_pg_uuid,in_max_nic_num,in_is_active);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  SELECT count(*) INTO is_exist FROM port_group a, uplink_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type AND a.uuid = in_pg_uuid;
  SELECT a.id INTO pgid FROM port_group a, uplink_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type AND a.uuid = in_pg_uuid;  
  IF is_exist <= 0 THEN 
    result := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
    
  -- insert item
  result.id := nextval('serial_switch');
  result.uuid := in_uuid;
  
  BEGIN 
    INSERT INTO switch(id,switch_type,name,uuid,state,max_mtu) values(result.id,in_switch_type,in_name,in_uuid,in_state,in_max_mtu);

    -- edvs nic_max_num_cfg 添加的时候 必须为0
    IF in_switch_type = 2 THEN 
        INSERT INTO virtual_switch(switch_id,switch_type,evb_mode,uplink_pg_id,nic_max_num_cfg,is_active)
            values(result.id,in_switch_type,in_evb_mode,pgid,0,in_is_active); 
    ELSE 
        INSERT INTO virtual_switch(switch_id,switch_type,evb_mode,uplink_pg_id,nic_max_num_cfg,is_active)
            values(result.id,in_switch_type,in_evb_mode,pgid,in_max_nic_num,in_is_active);    
    END IF;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- check can modify        
CREATE OR REPLACE FUNCTION pf_net_can_modify_vswitch(in_switch_type integer, in_name character varying,
        in_uuid character varying, in_state integer, in_max_mtu integer,
        in_evb_mode integer,in_pg_uuid character varying,in_max_nic_num integer, in_is_active integer )
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  switchid numeric(19,0);
  switchtype integer;
  is_exist integer; 
  uplinkportid numeric(19,0);
  oldpgid numeric(19,0);
  pgid numeric(19,0);
  pg_mtu integer;
  maxkernelmtu integer;
  /*错误返回码定义*/
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_UPLINKPGMTU_LESS_KERNELPG integer := 18;
BEGIN
  result := 0;
  -- 入参检查 后面做
  
  -- 检查pg uuid 是否有效
  SELECT count(*) INTO is_exist FROM port_group a, uplink_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type AND a.uuid = in_pg_uuid;
  SELECT a.id INTO pgid FROM port_group a, uplink_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type AND a.uuid = in_pg_uuid;  
  SELECT a.mtu INTO pg_mtu FROM port_group a, uplink_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type AND a.uuid = in_pg_uuid;  
  IF is_exist <= 0 THEN 
    result := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  -- switch type
  IF in_switch_type < 1 THEN 
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;
  
  IF in_switch_type > 2 THEN 
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;
  
  -- swtich uuid 
  SELECT count(*) INTO is_exist FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_uuid;
  SELECT a.switch_type INTO switchtype FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_uuid;    
  SELECT b.uplink_pg_id INTO oldpgid FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_uuid;    
  SELECT a.id INTO switchid FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_uuid;            
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
  
  -- switch type 不能修改 
  IF switchtype = in_switch_type THEN 
      result := 0;
  ELSE
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  
  -- 检查switch name 是否冲突 
  SELECT count(*) INTO is_exist FROM switch WHERE uuid = in_uuid AND name = in_name;
  IF is_exist <= 0 THEN 
     SELECT count(*) INTO is_exist FROM switch WHERE name = in_name;
     IF is_exist > 0 THEN 
        result := RTN_ITEM_CONFLICT;   
        return result;
     END IF;
  END IF;
  
  /* check max_mtu 
  IF in_max_mtu > pg_mtu THEN 
     result := RTN_SWITCH_MTU_LESS_PG;   
     return result;
  END IF;
  */

  
  
  --如果swtich存在虚拟nic的情况下，不能修改pg_id 
  SELECT count(*) INTO is_exist FROM virtual_switch a, vsi_profile b WHERE a.switch_id = b.virtual_switch_id AND  a.switch_id = switchid;
  IF is_exist > 0 THEN 
       IF oldpgid = pgid THEN 
           result :=0;
       ELSE
           result := RTN_ITEM_CONFLICT;   
           return result;
       END IF;
  END IF;

  --如果swtich的uplink 在SDVS EDVS混用的情况下，不能修改pg_id 
  SELECT count(*) INTO is_exist FROM virtual_switch_map WHERE virtual_switch_id  =   switchid;
  SELECT port_id INTO uplinkportid FROM virtual_switch_map WHERE virtual_switch_id  =   switchid;
  IF is_exist > 0 THEN 
      SELECT count(*) INTO is_exist FROM virtual_switch_map WHERE port_id  =   uplinkportid;
      -- is_exist > 1 说明存在混用情况 
      IF is_exist > 1 THEN 
          IF oldpgid = pgid THEN 
              result :=0;
          ELSE
              result := RTN_ITEM_CONFLICT;   
           return result;
          END IF;
      END IF;
  END IF;
  
  /*
  如果switch存在kernel port的情况下，uplink pg mtu 必须大于 kernel pg mtu 
  */
  SELECT MAX(b.mtu) INTO maxkernelmtu 
  FROM kernel_port a, port_group b 
  WHERE a.virtual_switch_id = switchid AND a.kernel_pg_id = b.id ;
  
  SELECT count(*) INTO is_exist 
  FROM kernel_port a, port_group b 
  WHERE a.virtual_switch_id = switchid AND a.kernel_pg_id = b.id ;
  
  IF is_exist > 0 THEN 
      IF maxkernelmtu > pg_mtu THEN        
          result := RTN_UPLINKPGMTU_LESS_KERNELPG;   
          return result;
      END IF;
  END IF;
  
  
  /*看看是否需要判断virtual_switch_map表*/
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 
        
-- switch修改
-- Function: pf_net_modify_vswitch(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_modify_vswitch(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_modify_vswitch(in_switch_type integer, in_name character varying,
        in_uuid character varying, in_state integer, in_max_mtu integer,
        in_evb_mode integer,in_pg_uuid character varying,in_max_nic_num integer, in_is_active integer )
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  pgid numeric(19,0);
  switchid numeric(19,0);
  phyportid numeric(19,0);
  is_sriov_exist integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG  integer := 5;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  
  -- 检查能否add
  result.ret := pf_net_can_modify_vswitch(in_switch_type,in_name,in_uuid,in_state,in_max_mtu,in_evb_mode,in_pg_uuid,in_max_nic_num,in_is_active);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  SELECT count(*) INTO is_exist FROM port_group a, uplink_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type AND a.uuid = in_pg_uuid;
  SELECT a.id INTO pgid FROM port_group a, uplink_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type AND a.uuid = in_pg_uuid;  
  IF is_exist <= 0 THEN 
    result := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  SELECT count(*) INTO is_exist FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_uuid;
  SELECT a.id INTO switchid FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_uuid;                
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
                 
  -- modify item
  result.id  := switchid;
  result.uuid := in_uuid;
  BEGIN 
    
    UPDATE switch SET name = in_name, state = in_state, max_mtu =in_max_mtu
       WHERE uuid = in_uuid;
    
    -- EDVS 的 nic_max_num_cfg 由db tr 来控制
    IF in_switch_type = 2 THEN 
        UPDATE virtual_switch SET evb_mode = in_evb_mode,uplink_pg_id = pgid, is_active = in_is_active 
           WHERE switch_id = switchid;            
    ELSE 
        UPDATE virtual_switch SET evb_mode = in_evb_mode,uplink_pg_id = pgid, nic_max_num_cfg = in_max_nic_num, is_active = in_is_active 
           WHERE switch_id = switchid;
    END IF;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

        
-- can del switch         

CREATE OR REPLACE FUNCTION pf_net_can_del_vswitch(in_uuid character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  switchid numeric(19,0);
  pgid numeric(19,0);
  oldpgid numeric(19,0);
  is_exist integer; 
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- swtich uuid 
  SELECT count(*) INTO is_exist FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_uuid;
  SELECT a.id INTO switchid FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_uuid;        
  SELECT b.uplink_pg_id INTO oldpgid FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_uuid;     
        
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
/*    
  --如果swtich存在虚拟nic的情况下，不能修改pg_id 
  SELECT count(*) INTO is_exist FROM virtual_switch a, vsi_profile b WHERE a.switch_id = b.virtual_switch_id AND  a.switch_id = switchid;
  IF is_exist > 0 THEN 
       IF oldpgid = pgid THEN 
           result :=0;
       ELSE
           result := RTN_SWITCH_MTU_LESS_PG;   
           return result;
       END IF;
  END IF;
  */
/*  
  --如果switch存在kernel port的情况下，不能修改pg_id
  SELECT count(*) INTO is_exist FROM virtual_switch a, kernel_port b WHERE a.switch_id = b.virtual_switch_id AND  a.switch_id = switchid;
  IF is_exist > 0 THEN 
       IF oldpgid = pgid THEN 
           result :=0;
       ELSE
           result := RTN_SWITCH_MTU_LESS_PG;   
           return result;
       END IF;
  END IF;
*/   
  -- 看看是否需要判断virtual_switch_map表
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


         
-- 删除switch
CREATE OR REPLACE FUNCTION pf_net_del_vswitch(in_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否存在记录
  result.ret := pf_net_can_del_vswitch(in_uuid);
  IF result.ret > 0 THEN 
     return result;
  END IF;

  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM switch WHERE uuid = in_uuid ;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

--- 查询phy port summary
--- 输出 name,uuid
-- Function: pf_net_query_vswitch_summary()

-- DROP FUNCTION pf_net_query_vswitch_summary();

CREATE OR REPLACE FUNCTION pf_net_query_vswitch_summary()
  RETURNS refcursor AS
$BODY$
DECLARE
  ref refcursor;
  is_same integer;
  is_exist integer;
  vnaid numeric(19,0);
  phyportid numeric(19,0);
BEGIN
      -- 
      OPEN ref FOR SELECT a.uuid, a.name, a.switch_type FROM switch a, virtual_switch b  WHERE a.id = b.switch_id AND a.switch_type = b.switch_type ;
      return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 查询switch
-- 输出 
-- Function: pf_net_query_vswitch(character varying)

-- DROP FUNCTION pf_net_query_vswitch(character varying);

CREATE OR REPLACE FUNCTION pf_net_query_vswitch(in_uuid character varying)
  RETURNS t_vswtich_query AS
$BODY$
DECLARE
  result  t_vswtich_query;
  is_exist integer;
  switchid numeric(19,0);
  pgid numeric(19,0);
  RTN_ITEM_NO_EXIST integer :=4;
BEGIN
  result.ret := 0;
  
  SELECT count(*) INTO is_exist FROM switch a, virtual_switch b  WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
           AND a.uuid = in_uuid ;
  SELECT a.id INTO switchid  FROM switch a, virtual_switch b  WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
           AND a.uuid = in_uuid ;
  SELECT b.uplink_pg_id INTO pgid  FROM switch a, virtual_switch b  WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
           AND a.uuid = in_uuid ;           
  IF is_exist <= 0 THEN   
     result.ret := RTN_ITEM_NO_EXIST; -- no exist item
     return result; 
  END IF;
  
   result.uuid := in_uuid;
   -- swtich 
   SELECT switch_type into result.switch_type  FROM switch WHERE uuid = in_uuid;
   SELECT name into result.name  FROM switch WHERE uuid = in_uuid;
   SELECT state into result.state FROM switch WHERE uuid = in_uuid;
   SELECT max_mtu into result.max_mtu  FROM switch WHERE uuid = in_uuid;
   
   --uplink pg 
   SELECT a.uuid INTO result.pg_uuid FROM port_group a, uplink_pg b, virtual_switch c WHERE a.id = b.pg_id AND b.pg_id = c.uplink_pg_id 
          AND c.switch_id = switchid;
   
   -- virtual switch 
   SELECT evb_mode into result.evb_mode  FROM virtual_switch WHERE switch_id = switchid;
   SELECT nic_max_num_cfg into result.nic_max_num_cfg  FROM virtual_switch WHERE switch_id = switchid;
   SELECT is_active into result.is_active  FROM virtual_switch WHERE switch_id = switchid;
      
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 32 vswitch_map 

CREATE OR REPLACE FUNCTION pf_net_can_add_vswitchmap(in_vswitch_uuid character varying,in_port_uuid character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  portid numeric(19,0);
  switchid numeric(19,0);
  porttype integer;
  switchtype integer;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_SWITCH_UPLINKPORT_INVALID integer := 9;
  RTN_PORT_HAS_JOIN_OTHER_SWITCH integer := 10;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- swtich uuid 
  SELECT count(*) INTO is_exist FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_vswitch_uuid;
  SELECT a.id INTO switchid FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_vswitch_uuid;        
  SELECT a.switch_type INTO switchtype FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_vswitch_uuid;    
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  --- port
  SELECT count(*) INTO is_exist FROM port WHERE uuid = in_port_uuid;
  SELECT id INTO portid FROM port WHERE uuid = in_port_uuid;
  SELECT port_type INTO porttype FROM port WHERE uuid = in_port_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
  
  -- port_type check 
  IF porttype = 2 THEN  -- sriov vf port;
     result := RTN_SWITCH_UPLINKPORT_INVALID; 
     return result;
  END IF;
  
  IF porttype = 2 THEN  -- kernel port(mgr, storage);
     result := RTN_SWITCH_UPLINKPORT_INVALID; 
     return result;
  END IF;
  
  -- same add 
  SELECT count(*) INTO is_exist FROM virtual_switch_map WHERE port_id = portid AND virtual_switch_id = switchid;
  IF is_exist > 0 THEN 
     result := 0; -- RTN_HAS_SAME_ITEM; 
     return result;  
  END IF;
  
  -- port 是否已在其他switch中
  -- 需考虑 sriov 网桥混用情况  
  SELECT count(*) INTO is_exist FROM v_net_switch_port_vna WHERE port_uuid = in_port_uuid AND switch_type = switchtype ;
  IF is_exist > 0 THEN 
     result := RTN_PORT_HAS_JOIN_OTHER_SWITCH; 
     return result;
  END IF;
  
  -- port 是否已经是bond的成员口
  SELECT count(*) INTO is_exist FROM bond_map WHERE physical_port_id = portid;
  IF is_exist > 0 THEN 
     result := RTN_ITEM_CONFLICT; 
     return result;
  END IF;
  
  -- port 需判断edvs 加入的网口是否是sriov网口，待bond完成后，完善。
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- map 添加 
-- Function: pf_net_add_vswitchmap(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_vswitchmap(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_vswitchmap(in_vswitch_uuid character varying, 
               in_port_uuid character varying , in_is_cfg integer, in_is_selected integer, in_is_consistency integer )
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_same_pg integer;
  is_conflict integer;
  portid numeric(19,0);
  switchid numeric(19,0);
  is_exist integer; 
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  result.ret := pf_net_can_add_vswitchmap(in_vswitch_uuid,in_port_uuid);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
   -- swtich uuid 
  SELECT count(*) INTO is_exist FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_vswitch_uuid;
  SELECT a.id INTO switchid FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_vswitch_uuid;        
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  --- port
  SELECT count(*) INTO is_exist FROM port WHERE uuid = in_port_uuid;
  SELECT id INTO portid FROM port WHERE uuid = in_port_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
  
  
  -- same add
  SELECT count(*) INTO is_exist FROM virtual_switch_map WHERE port_id = portid AND virtual_switch_id = switchid;
  SELECT id INTO result.id FROM virtual_switch_map WHERE port_id = portid AND virtual_switch_id = switchid;
  IF is_exist > 0 THEN 
     result.ret  := RTN_HAS_SAME_ITEM; 
     return result;  
  END IF;
  
  -- add item 
  result.id := nextval('serial_virtual_switch_map');
  --result.uuid := in_uuid;
  
  BEGIN 
     INSERT INTO virtual_switch_map (id, port_id, virtual_switch_id, is_cfg, is_selected, is_consistency)
        values(result.id,portid,switchid, in_is_cfg, in_is_selected, in_is_consistency);     

    -- trigger
	IF in_is_consistency > 0 THEN 
        result.ret :=  pf_net_tr_add_edvsmap(switchid, portid);
	END IF;
	
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

--修改switch map 检查 
CREATE OR REPLACE FUNCTION pf_net_check_modify_vswitchmap(in_vswitch_uuid character varying,in_port_uuid character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  portid numeric(19,0);
  switchid numeric(19,0);
  porttype integer;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_SWITCH_UPLINKPORT_INVALID integer := 9;
  RTN_PORT_HAS_JOIN_OTHER_SWITCH integer := 10;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- swtich uuid 
  SELECT a.id INTO switchid FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_vswitch_uuid;    
  SELECT count(*) INTO is_exist FROM switch a, virtual_switch b, port c, virtual_switch_map d
     WHERE a.id = b.switch_id AND a.switch_type = b.switch_type AND a.uuid = in_vswitch_uuid 
	     AND d.virtual_switch_id = a.id AND d.port_id = c.id 
		 AND c.uuid = in_port_uuid;				
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  --- port
  SELECT count(*) INTO is_exist FROM port WHERE uuid = in_port_uuid;
  SELECT id INTO portid FROM port WHERE uuid = in_port_uuid;
  SELECT port_type INTO porttype FROM port WHERE uuid = in_port_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
  
  -- port_type check 
  IF porttype = 2 THEN  -- sriov vf port;
     result := RTN_SWITCH_UPLINKPORT_INVALID; 
     return result;
  END IF;
  
  IF porttype = 2 THEN  -- kernel port(mgr, storage);
     result := RTN_SWITCH_UPLINKPORT_INVALID; 
     return result;
  END IF;
  
  -- same add 
  SELECT count(*) INTO is_exist FROM virtual_switch_map WHERE port_id = portid AND virtual_switch_id = switchid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; -- ; 
     return result;  
  END IF;
  
  -- port 需判断edvs 加入的网口是否是sriov网口，待bond完成后，完善。
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
             

CREATE OR REPLACE FUNCTION pf_net_modify_vswitchmap(in_vswitch_uuid character varying, 
               in_port_uuid character varying , in_is_cfg integer, in_is_selected integer, in_is_consistency integer )
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_same_pg integer;
  is_conflict integer;
  portid numeric(19,0);
  switchid numeric(19,0);
  is_exist integer; 
  oldconsistency integer := 0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  result.ret := pf_net_check_modify_vswitchmap(in_vswitch_uuid,in_port_uuid);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  -- swtich uuid   
  SELECT a.id INTO switchid FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_vswitch_uuid;      
  SELECT d.is_consistency INTO oldconsistency FROM switch a, virtual_switch b, port c, virtual_switch_map d
     WHERE a.id = b.switch_id AND a.switch_type = b.switch_type AND a.uuid = in_vswitch_uuid 
	     AND d.virtual_switch_id = a.id AND d.port_id = c.id 
		 AND c.uuid = in_port_uuid; 
  SELECT count(*) INTO is_exist FROM switch a, virtual_switch b, port c, virtual_switch_map d
     WHERE a.id = b.switch_id AND a.switch_type = b.switch_type AND a.uuid = in_vswitch_uuid 
	     AND d.virtual_switch_id = a.id AND d.port_id = c.id 
		 AND c.uuid = in_port_uuid;		 
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  --- port
  SELECT count(*) INTO is_exist FROM port WHERE uuid = in_port_uuid;
  SELECT id INTO portid FROM port WHERE uuid = in_port_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
      
  -- update item 
  --result.id := nextval('serial_virtual_switch_map');
  --result.uuid := in_uuid;
  
  BEGIN 
     UPDATE  virtual_switch_map SET is_cfg = in_is_cfg, is_selected = in_is_selected, is_consistency = in_is_consistency 
         WHERE     port_id = portid AND  virtual_switch_id =switchid ;      
     IF oldconsistency > in_is_consistency THEN 
	     result.ret := pf_net_tr_modify_edvsmap(switchid, portid, 0);
	 END IF;
	 
	 IF oldconsistency < in_is_consistency THEN 
	     result.ret := pf_net_tr_modify_edvsmap(switchid, portid, 1);
	 END IF;
	 
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
             
        
            
---------            ------------------------------------------------------

CREATE OR REPLACE FUNCTION pf_net_can_del_vswitchmap(in_vswitch_uuid character varying,in_port_uuid character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  portid numeric(19,0);
  vnaid numeric(19,0);  
  vnauuid character varying(64);
  switch_vsi_num t_query_total;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_SWITCH_UPLINKPORT_INVALID integer := 9;
  RTN_PORT_HAS_JOIN_OTHER_SWITCH integer := 10;
  RTN_SWITCH_HAS_DEPLOY_VSI integer := 11;
  RTN_GET_SWITCH_VSI_NUM_FAILED integer := 12;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- swtich uuid 
  SELECT count(*) INTO is_exist FROM switch a, virtual_switch b, port c, virtual_switch_map d
     WHERE a.id = b.switch_id AND a.switch_type = b.switch_type AND a.uuid = in_vswitch_uuid 
	     AND d.virtual_switch_id = a.id AND d.port_id = c.id 
		 AND c.uuid = in_port_uuid;	
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  --- port
  SELECT count(*) INTO is_exist FROM port WHERE uuid = in_port_uuid;
  SELECT id INTO portid FROM port WHERE uuid = in_port_uuid;
  SELECT b.vna_uuid INTO vnauuid FROM port a, vna b  WHERE a.uuid = 
in_port_uuid AND a.vna_id = b.id ;  
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
  
  -- vswitch vsi num
  switch_vsi_num = pf_net_query_vswitchvna_vsi_num(in_vswitch_uuid,vnauuid);
  IF switch_vsi_num.ret = 0 THEN 
      IF switch_vsi_num.total_num > 0 THEN 
          result := RTN_SWITCH_HAS_DEPLOY_VSI;  -- switch has vsi 
          return result;
      END IF;
  ELSE       
      result := RTN_GET_SWITCH_VSI_NUM_FAILED;  -- get switch vsi num failed
      return result;  
  END IF;
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

            

-- 删除switch map

CREATE OR REPLACE FUNCTION pf_net_del_vswitchmap(in_vswitch_uuid character varying,in_port_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  switchid numeric(19,0);
  portid numeric(19,0);
  oldconsistency integer := 0;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  result.ret := pf_net_can_del_vswitchmap(in_vswitch_uuid,in_port_uuid);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
   -- swtich uuid 
  SELECT a.id INTO switchid FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_vswitch_uuid;   
  SELECT d.is_consistency INTO oldconsistency FROM switch a, virtual_switch b, port c, virtual_switch_map d
     WHERE a.id = b.switch_id AND a.switch_type = b.switch_type AND a.uuid = in_vswitch_uuid 
	     AND d.virtual_switch_id = a.id AND d.port_id = c.id 
		 AND c.uuid = in_port_uuid; 
  SELECT count(*) INTO is_exist FROM switch a, virtual_switch b, port c, virtual_switch_map d
     WHERE a.id = b.switch_id AND a.switch_type = b.switch_type AND a.uuid = in_vswitch_uuid 
	     AND d.virtual_switch_id = a.id AND d.port_id = c.id 
		 AND c.uuid = in_port_uuid;	
		 
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  --- port
  SELECT count(*) INTO is_exist FROM port WHERE uuid = in_port_uuid;
  SELECT id INTO portid FROM port WHERE uuid = in_port_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM virtual_switch_map WHERE port_id = portid AND virtual_switch_id = switchid ;
	
	-- trigger
	IF oldconsistency > 0 THEN 
        result.ret :=  pf_net_tr_del_edvsmap(switchid, portid);
	END IF;
	
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 查询switch_map (通过switch uuid)

CREATE OR REPLACE FUNCTION pf_net_query_vswitchmap_by_switch(in_switch_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    -- swtich uuid 
    SELECT count(*) INTO is_exist FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_switch_uuid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF;  
  
    OPEN ref FOR SELECT switch_uuid,switch_name,switch_type,
        vna_uuid, vna_application,vnm_application,host_name,
        port_uuid,port_name,port_type,switch_map_is_cfg,switch_map_is_selected,switch_map_is_consistency 
        FROM v_net_switch_port_vna WHERE switch_uuid = in_switch_uuid;
      
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

--- 查询swtich map （通过vna）　
CREATE OR REPLACE FUNCTION pf_net_query_vswitchmap_by_vna(in_vna_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    -- vna uuid 
    SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF;  
    
    -- 
    OPEN ref FOR SELECT switch_uuid,switch_name,switch_type,
        vna_uuid, vna_application,vnm_application,host_name,
        port_uuid,port_name,port_type ,switch_map_is_cfg,switch_map_is_selected,switch_map_is_consistency 
        FROM v_net_switch_port_vna WHERE vna_uuid = in_vna_uuid;
        
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

--- 查询swtich map （通过switch vna）　
CREATE OR REPLACE FUNCTION pf_net_query_vswitchmap_by_switchvna(in_switch_uuid character varying,in_vna_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    -- vna uuid 
    SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF;  
    
    -- swtich uuid 
    SELECT count(*) INTO is_exist FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_switch_uuid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF;  
    
    -- 
    OPEN ref FOR SELECT switch_uuid,switch_name,switch_type,
        vna_uuid, vna_application,vnm_application,host_name,
        port_uuid,port_name,port_type ,switch_map_is_cfg,switch_map_is_selected,switch_map_is_consistency 
        FROM v_net_switch_port_vna WHERE vna_uuid = in_vna_uuid AND switch_uuid = in_switch_uuid;
        
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 33 vsi



-- 判断VSI 能否添加
CREATE OR REPLACE FUNCTION pf_net_check_vsi_param(in_vm_id numeric(19,0),in_nic_index integer,
                           in_is_sriov integer, in_is_loop integer, in_logicnetwork_uuid character varying, 
                           in_port_type character varying, 
                           in_vsi_id_value character varying,  in_vsi_id_format integer,
                           in_vm_pg_uuid character varying, in_mac character varying,  in_is_has_ip integer, in_ip character varying, 
                           in_is_has_vswitch integer, in_vswitch_uuid character varying,
                           in_is_has_sriovvf integer, in_sriov_vf_port_uuid character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  switchportmode integer;
  pgid numeric(19,0);
  logicnetworkid numeric(19,0);  
  netplaneid numeric(19,0);
  vswitchid numeric(19,0);
  sriovportid numeric(19,0);
  phyid numeric(19,0);
  phyuuid character varying(64);
  bonduuid character varying(64);
  bondid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE integer := 13;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
  
  -- in_vsi_id_format 必须为5 
  IF in_vsi_id_format = 5 THEN 
      result := 0;
  ELSE
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  
  -- logicnetwork uuid 
  SELECT count(*) INTO is_exist FROM logic_network WHERE  uuid = in_logicnetwork_uuid;
  SELECT id INTO logicnetworkid FROM logic_network WHERE  uuid = in_logicnetwork_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 

  -- vm pg uuid  
  -- vm pg 
  SELECT count(*) INTO is_exist FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_vm_pg_uuid;
  SELECT a.id INTO pgid FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_vm_pg_uuid;
  SELECT a.switchport_mode INTO switchportmode FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_vm_pg_uuid;  
  SELECT netplane_id INTO netplaneid FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_vm_pg_uuid AND a.netplane_id is not null;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
    
  -- logicnetwork 必须 属于vm pg uuid 
  SELECT count(*) INTO is_exist FROM v_net_logicnetwork_portgroup_netplane WHERE pg_uuid = in_vm_pg_uuid AND logicnetwork_uuid = in_logicnetwork_uuid ;
  IF is_exist <= 0 THEN 
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;  
  
  -- mac 
  SELECT count(*) INTO is_exist FROM  mac_pool WHERE mac = in_mac AND netplane_id = netplaneid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
  -- ip 
  IF in_is_has_ip > 0 THEN 
     SELECT count(*) INTO is_exist FROM logic_network_ip_pool WHERE logic_network_id = logicnetworkid AND ip = in_ip;
     IF is_exist <= 0 THEN 
         result := RTN_ITEM_NO_EXIST; 
         return result;
     END IF; 
  END IF;
  
  -- switch uuid 
  IF in_is_has_vswitch > 0 THEN 
      SELECT count(*) INTO is_exist FROM switch a,virtual_switch b WHERE a.id = b.switch_id AND a.uuid = in_vswitch_uuid;
      SELECT a.id INTO vswitchid FROM switch a,virtual_switch b WHERE a.id = b.switch_id AND a.uuid = in_vswitch_uuid;      
      IF is_exist <= 0 THEN 
         result := RTN_ITEM_NO_EXIST; 
         return result;
      END IF;
            
      -- sriov 时，switch 必须是edvs
      IF in_is_sriov > 0 THEN 
          SELECT count(*) INTO is_exist FROM switch WHERE id = vswitchid AND switch_type = 2;
          IF is_exist <= 0 THEN 
             result := RTN_PARAM_INVALID; 
             return result;
          END IF;     
      ELSE 
          SELECT count(*) INTO is_exist FROM switch WHERE id = vswitchid AND switch_type = 1;
          IF is_exist <= 0 THEN 
             result := RTN_PARAM_INVALID; 
             return result;
          END IF;
      END IF;
  END IF;
  
  -- sriov uuid 
  IF in_is_sriov > 0 THEN 
      IF in_is_has_sriovvf > 0 THEN 
          SELECT count(*) INTO is_exist FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.uuid = in_sriov_vf_port_uuid;
          IF is_exist <= 0 THEN 
             result := RTN_ITEM_NO_EXIST; 
             return result;
          END IF;
      
          SELECT b.port_id INTO sriovportid FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.uuid = in_sriov_vf_port_uuid;
          SELECT b.sriov_port_id INTO phyid FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.uuid = in_sriov_vf_port_uuid;      
          SELECT uuid INTO phyuuid FROM port WHERE id = phyid;
          IF is_exist <= 0 THEN 
             result := RTN_ITEM_NO_EXIST; 
             return result;
          END IF;
      END IF;      
  END IF;
  
  -- switch uuid, sriov uuid 
  IF in_is_sriov > 0 THEN 
     IF in_is_has_vswitch > 0 THEN 
        IF in_is_has_sriovvf > 0 THEN 
            -- 需要判断是否是switch的uplink口(bond成员口)    
            -- 物理端口是否在bond中 
            SELECT count(*) INTO is_exist FROM v_net_bondmap_bond_phy WHERE phy_uuid = phyuuid;
            SELECT bond_uuid INTO bonduuid FROM v_net_bondmap_bond_phy WHERE phy_uuid = phyuuid;
            IF is_exist > 0 THEN 
                -- 已经做了bond
                SELECT count(*) INTO is_exist FROM v_net_switch_port_vna WHERE switch_uuid = in_vswitch_uuid AND port_uuid = bonduuid;
                IF is_exist <= 0 THEN 
                    -- bond口不是switch uplink
                    result := RTN_PARAM_INVALID; 
                    return result;
                END IF;
            ELSE 
                -- PHY 并没有加入bond
                SELECT count(*) INTO is_exist FROM v_net_switch_port_vna WHERE switch_uuid = in_vswitch_uuid AND port_uuid = phyuuid;
                IF is_exist <= 0 THEN 
                    -- phy口不是switch uplink
                    result := RTN_PARAM_INVALID; 
                    return result;
                END IF;
            END IF;
        ELSE
            -- switch 和 sriov 成对出现
            result := RTN_PARAM_INVALID; 
            return result;
        END IF;
     END IF;
  END IF;
  
  --   
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 
                           
-- check add vsi 
CREATE OR REPLACE FUNCTION pf_net_check_add_vsi(in_vm_id numeric(19,0),in_nic_index integer,
                           in_is_sriov integer, in_is_loop integer, in_logicnetwork_uuid character varying, 
                           in_port_type character varying, 
                           in_vsi_id_value character varying,  in_vsi_id_format integer,
                           in_vm_pg_uuid character varying, in_mac character varying,  in_is_has_ip integer, in_ip character varying, 
                           in_is_has_vswitch integer, in_vswitch_uuid character varying,
                           in_is_has_sriovvf integer, in_sriov_vf_port_uuid character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  
  logicnetworkid numeric(19,0);
  netplaneid numeric(19,0);
  pgid numeric(19,0);
  macid numeric(19,0);
  ipid  numeric(19,0);
  sriovvfportid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE integer := 13;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
  
  result := pf_net_check_vsi_param(in_vm_id,in_nic_index, in_is_sriov, in_is_loop, in_logicnetwork_uuid,
                      in_port_type,
                      in_vsi_id_value,in_vsi_id_format,in_vm_pg_uuid,in_mac,in_is_has_ip,in_ip,
                      in_is_has_vswitch,in_vswitch_uuid,in_is_has_sriovvf,in_sriov_vf_port_uuid);
  IF  result > 0 THEN 
      return result;
  END IF;  
  
  --vm 必须存在
  SELECT count(*) INTO is_exist FROM vm WHERE  vm_id = in_vm_id;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
  
  -- logicnetwork uuid 
  SELECT count(*) INTO is_exist FROM logic_network WHERE  uuid = in_logicnetwork_uuid;
  SELECT id INTO logicnetworkid FROM logic_network WHERE  uuid = in_logicnetwork_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
  -- vm pg uuid  
  -- vm pg 
  SELECT count(*) INTO is_exist FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_vm_pg_uuid;
  SELECT a.id INTO pgid FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_vm_pg_uuid;
  SELECT netplane_id INTO netplaneid FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_vm_pg_uuid AND a.netplane_id is not null;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
  
  -- vm_id, vnic_index 检查
  SELECT count(*) INTO is_exist FROM vnic WHERE vm_id = in_vm_id AND nic_index = in_nic_index;
  IF is_exist > 0 THEN 
      result := RTN_ITEM_CONFLICT; 
      return result;
  END IF;
  
  --mac 
  SELECT count(*) INTO is_exist FROM  mac_pool WHERE mac = in_mac AND netplane_id = netplaneid;
  SELECT id INTO  macid  FROM  mac_pool WHERE mac = in_mac AND netplane_id = netplaneid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
  -- mac conflict check
  SELECT count(*) INTO is_exist FROM vsi_profile WHERE mac_id = macid;
  IF is_exist > 0 THEN 
     result := RTN_ITEM_CONFLICT; 
     return result;
  END IF; 
  
  -- ip 
  IF in_is_has_ip > 0 THEN 
     SELECT count(*) INTO is_exist FROM logic_network_ip_pool WHERE logic_network_id = logicnetworkid AND ip = in_ip;
     SELECT id INTO ipid FROM logic_network_ip_pool WHERE logic_network_id = logicnetworkid AND ip = in_ip;
     IF is_exist <= 0 THEN 
         result := RTN_ITEM_NO_EXIST; 
         return result;
     END IF; 
     
     -- ip conflict check 
     SELECT count(*) INTO is_exist FROM vsi_profile WHERE logic_network_ip_id = ipid;
     IF is_exist > 0 THEN 
         result := RTN_ITEM_CONFLICT; 
         return result;
     END IF; 
  END IF;
  
  -- sriov 
  -- sriov uuid 
  IF in_is_has_sriovvf > 0 THEN 
      SELECT count(*) INTO is_exist FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.uuid = in_sriov_vf_port_uuid;
      IF is_exist <= 0 THEN 
         result := RTN_ITEM_NO_EXIST; 
         return result;
      END IF;
      
      SELECT b.port_id INTO sriovvfportid FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.uuid = in_sriov_vf_port_uuid;
      
      --sriov vf port  conflict check
      SELECT count(*) INTO is_exist FROM vsi_sriov WHERE sriov_vf_port_id = sriovvfportid;      
      IF is_exist > 0 THEN 
         result := RTN_ITEM_CONFLICT; 
         return result;
      END IF;
  END IF;
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 
                           

                           

-- vsi 添加 
-- Function: pf_net_add_vsi(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_vsi(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_vsi(in_vm_id numeric(19,0), in_nic_index integer,
                           in_is_sriov integer, in_is_loop integer, in_logicnetwork_uuid character varying, 
                           in_port_type character varying, 
                           in_vsi_id_value character varying,  in_vsi_id_format integer,
                           in_vm_pg_uuid character varying, in_mac character varying,  in_is_has_ip integer, in_ip character varying, 
                           in_is_has_vswitch integer, in_vswitch_uuid character varying,
                           in_is_has_sriovvf integer, in_sriov_vf_port_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  
  vnicid numeric(19,0);
  ipid numeric(19,0);
  macid numeric(19,0);
  pgid numeric(19,0);
  logicnetworkid numeric(19,0);  
  netplaneid numeric(19,0);
  vswitchid numeric(19,0);
  sriovportid numeric(19,0);
  phyid numeric(19,0);
  phyuuid character varying(64);
  bonduuid character varying(64);
  bondid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_add_vsi(in_vm_id,in_nic_index, in_is_sriov, in_is_loop, in_logicnetwork_uuid,
                      in_port_type,
                      in_vsi_id_value,in_vsi_id_format,in_vm_pg_uuid,in_mac,in_is_has_ip,in_ip,
                      in_is_has_vswitch,in_vswitch_uuid,in_is_has_sriovvf,in_sriov_vf_port_uuid);
  IF result.ret > 0 THEN      
     return result;
  END IF;
  
  -- logicnetwork uuid 
  SELECT count(*) INTO is_exist FROM logic_network WHERE  uuid = in_logicnetwork_uuid;
  SELECT id INTO logicnetworkid FROM logic_network WHERE  uuid = in_logicnetwork_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 

  -- vm pg uuid   
  SELECT count(*) INTO is_exist FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_vm_pg_uuid;
  SELECT a.id INTO pgid FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_vm_pg_uuid;  
  SELECT netplane_id INTO netplaneid FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_vm_pg_uuid AND a.netplane_id is not null;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
    
  
  -- mac 
  SELECT count(*) INTO is_exist FROM  mac_pool WHERE mac = in_mac AND netplane_id = netplaneid;
  SELECT id INTO macid FROM  mac_pool WHERE mac = in_mac AND netplane_id = netplaneid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
  -- ip 
  IF in_is_has_ip > 0 THEN 
     SELECT count(*) INTO is_exist FROM logic_network_ip_pool WHERE logic_network_id = logicnetworkid AND ip = in_ip;
     SELECT id INTO ipid FROM logic_network_ip_pool WHERE logic_network_id = logicnetworkid AND ip = in_ip;
     IF is_exist <= 0 THEN 
         result := RTN_ITEM_NO_EXIST; 
         return result;
     END IF; 
  END IF;
  
  -- switch uuid 
  IF in_is_has_vswitch > 0 THEN 
      SELECT count(*) INTO is_exist FROM switch a,virtual_switch b WHERE a.id = b.switch_id AND a.uuid = in_vswitch_uuid;
      SELECT a.id INTO vswitchid FROM switch a,virtual_switch b WHERE a.id = b.switch_id AND a.uuid = in_vswitch_uuid;      
      IF is_exist <= 0 THEN 
         result := RTN_ITEM_NO_EXIST; 
         return result;
      END IF;
  END IF;
  
  -- sriov uuid 
  IF in_is_has_sriovvf > 0 THEN 
      SELECT count(*) INTO is_exist FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.uuid = in_sriov_vf_port_uuid;
      SELECT b.port_id INTO sriovportid FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.uuid = in_sriov_vf_port_uuid;
      SELECT b.sriov_port_id INTO phyid FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.uuid = in_sriov_vf_port_uuid;      
      SELECT uuid INTO phyuuid FROM port WHERE id = phyid;
      IF is_exist <= 0 THEN 
         result := RTN_ITEM_NO_EXIST; 
         return result;
      END IF;
  END IF;
  
  
  -- insert item 
  result.id := nextval('serial_vsi_profile');
  vnicid := nextval('serial_vnic');
  result.uuid := in_vsi_id_value;
  BEGIN 
      -- INSERT vsi profile 
      IF in_is_has_ip > 0 THEN           
          IF in_is_has_vswitch > 0 THEN 
              INSERT INTO vsi_profile(id,vsi_id_value, vsi_id_format, vm_pg_id, mac_id, logic_network_ip_id,virtual_switch_id) 
                values(result.id,in_vsi_id_value,in_vsi_id_format, pgid,macid, ipid, vswitchid);
          ELSE 
              INSERT INTO vsi_profile(id,vsi_id_value, vsi_id_format, vm_pg_id, mac_id, logic_network_ip_id) 
                values(result.id,in_vsi_id_value,in_vsi_id_format, pgid,macid, ipid);
          END IF;
      ELSE 
          IF in_is_has_vswitch > 0 THEN 
              INSERT INTO vsi_profile(id,vsi_id_value, vsi_id_format, vm_pg_id, mac_id, virtual_switch_id) 
                values(result.id,in_vsi_id_value,in_vsi_id_format, pgid,macid,  vswitchid);
                
             -- trigger
            result.ret :=  pf_net_tr_vsi_attach_vswitch(result.id, vswitchid);
             
          ELSE 
              INSERT INTO vsi_profile(id,vsi_id_value, vsi_id_format, vm_pg_id, mac_id) 
                values(result.id,in_vsi_id_value,in_vsi_id_format, pgid,macid);                
          END IF;                   
      END IF;
      
      -- INSERT VNIC 
     INSERT INTO vnic(id, vm_id, nic_index, is_sriov ,is_loop, logic_network_uuid, port_type, 
             vsi_profile_id )
      VALUES(vnicid,in_vm_id,in_nic_index,in_is_sriov,in_is_loop,in_logicnetwork_uuid,in_port_type, 
             result.id);
                 
      -- INSERT VSI SRIOV 
      IF in_is_has_sriovvf > 0 THEN 
         INSERT INTO vsi_sriov(vsi_profile_id,sriov_vf_port_id) values(result.id,sriovportid);
         
         -- trigger
         result.ret :=  pf_net_tr_add_vsisriov(result.id,sriovportid);
      END IF;      
      
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 判断是否可以进行修改                  

CREATE OR REPLACE FUNCTION pf_net_check_modify_vsi(in_vm_id numeric(19,0),in_nic_index integer, 
                           in_is_sriov integer, in_is_loop integer, in_logicnetwork_uuid character varying, 
                           in_port_type character varying, 
                           in_vsi_id_value character varying,  in_vsi_id_format integer,
                           in_vm_pg_uuid character varying, in_mac character varying,  in_is_has_ip integer, in_ip character varying, 
                           in_is_has_vswitch integer, in_vswitch_uuid character varying,
                           in_is_has_sriovvf integer, in_sriov_vf_port_uuid character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  vsiid numeric(19,0);
  sriovvfportid numeric(19,0);
  vswitchid numeric(19,0);
  
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE integer := 13;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
  
  result := pf_net_check_vsi_param(in_vm_id,in_nic_index, in_is_sriov, in_is_loop, in_logicnetwork_uuid,
                      in_port_type,
                      in_vsi_id_value,in_vsi_id_format,in_vm_pg_uuid,in_mac,in_is_has_ip,in_ip,
                      in_is_has_vswitch,in_vswitch_uuid,in_is_has_sriovvf,in_sriov_vf_port_uuid);
  IF  result > 0 THEN 
      return result;
  END IF;  
  
  -- check vsi_id 是否存在
  SELECT count(*) INTO is_exist FROM vsi_profile WHERE vsi_id_value = in_vsi_id_value;
  SELECT id INTO vsiid FROM vsi_profile WHERE vsi_id_value = in_vsi_id_value;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST; 
      return result;
  END IF; 
  
  -- vm_id, nic_index, is_sriov, is_loop, logicnetwork_uuid, port_type 不能修改
  SELECT count(*) INTO is_exist FROM vnic WHERE vm_id = in_vm_id AND nic_index = in_nic_index 
          AND is_sriov = in_is_sriov AND is_loop = in_is_loop AND logic_network_uuid = in_logicnetwork_uuid AND port_type = in_port_type 
          AND vsi_profile_id = vsiid;
  IF is_exist <= 0 THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF; 
  
  -- mac 不能改变
  SELECT count(*) INTO is_exist FROM vsi_profile a, mac_pool b WHERE a.mac_id = b.id AND b.mac = in_mac AND a.id = vsiid;
  IF is_exist <= 0 THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  
  -- ip 不能改变
  IF in_is_has_ip > 0 THEN 
     SELECT count(*) INTO is_exist FROM vsi_profile a, logic_network_ip_pool b WHERE a.logic_network_ip_id = b.id 
             AND b.ip = in_ip AND a.id = vsiid;
     IF is_exist <= 0 THEN 
        result := RTN_PARAM_INVALID; 
        return result;
     END IF;
  ELSE
     SELECT count(*) INTO is_exist FROM vsi_profile WHERE id = vsiid AND logic_network_ip_id is NULL;
     IF is_exist <= 0 THEN 
        result := RTN_PARAM_INVALID; 
        return result;
     END IF;
  END IF;
  
  -- sriov conflict check 
  -- sriov uuid 
  IF in_is_has_sriovvf > 0 THEN 
      SELECT count(*) INTO is_exist FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.uuid = in_sriov_vf_port_uuid;
      IF is_exist <= 0 THEN 
         result := RTN_ITEM_NO_EXIST; 
         return result;
      END IF;
      
      SELECT b.port_id INTO sriovvfportid FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.uuid = in_sriov_vf_port_uuid;
      
      --sriov vf port  conflict check
      SELECT count(*) INTO is_exist FROM vsi_sriov WHERE sriov_vf_port_id = sriovvfportid AND vsi_profile_id = vsiid ;      
      IF is_exist <= 0 THEN 
          SELECT count(*) INTO is_exist FROM vsi_sriov WHERE sriov_vf_port_id = sriovvfportid;      
          IF is_exist > 0 THEN 
             result := RTN_ITEM_CONFLICT; 
             return result;
          END IF;
          
          -- 已分配sriov，不能再修改
          SELECT count(*) INTO is_exist FROM vsi_sriov WHERE vsi_profile_id = vsiid ;      
          IF is_exist > 0 THEN 
             result := RTN_ITEM_CONFLICT; 
             return result;
          END IF;
      END IF;
  END IF;
  
  -- 考虑实际情况，部署vnic, Switch，sriov确定后，将不能再进行修改
  -- switch uuid 
  IF in_is_has_vswitch > 0 THEN 
      SELECT count(*) INTO is_exist FROM switch a,virtual_switch b WHERE a.id = b.switch_id AND a.uuid = in_vswitch_uuid;
      SELECT a.id INTO vswitchid FROM switch a,virtual_switch b WHERE a.id = b.switch_id AND a.uuid = in_vswitch_uuid;      
      IF is_exist <= 0 THEN 
         result := RTN_ITEM_NO_EXIST; 
         return result;
      END IF;
      
      -- switch can not change
      SELECT count(*) INTO is_exist FROM vsi_profile WHERE id = vsiid AND virtual_switch_id is not null;
      IF is_exist > 0 THEN 
          SELECT count(*) INTO is_exist FROM vsi_profile WHERE id = vsiid AND virtual_switch_id = vswitchid;
          IF is_exist <= 0 THEN 
              result := RTN_ITEM_CONFLICT; 
              return result;
          END IF;
      END IF;      
  END IF;
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

                           
-- vsi 修改 
CREATE OR REPLACE FUNCTION pf_net_modify_vsi(in_vm_id numeric(19,0),in_nic_index integer,
                           in_is_sriov integer, in_is_loop integer, in_logicnetwork_uuid character varying, 
                           in_port_type character varying, 
                           in_vsi_id_value character varying,  in_vsi_id_format integer,
                           in_vm_pg_uuid character varying, in_mac character varying,  in_is_has_ip integer, in_ip character varying, 
                           in_is_has_vswitch integer, in_vswitch_uuid character varying,
                           in_is_has_sriovvf integer, in_sriov_vf_port_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  
  vsiid numeric(19,0);
  vnicid numeric(19,0);
  ipid numeric(19,0);
  macid numeric(19,0);
  pgid numeric(19,0);
  logicnetworkid numeric(19,0);  
  netplaneid numeric(19,0);
  vswitchid numeric(19,0);
  oldvswitchid numeric(19,0);
  sriovportid numeric(19,0);
  phyid numeric(19,0);
  phyuuid character varying(64);
  bonduuid character varying(64);
  bondid numeric(19,0);
  oldsriovvfportid numeric(19,0);
  oldexistswitch integer := 0;
  oldexistsriovvfport integer := 0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_modify_vsi(in_vm_id,in_nic_index, in_is_sriov, in_is_loop, in_logicnetwork_uuid,
                      in_port_type,
                      in_vsi_id_value,in_vsi_id_format,in_vm_pg_uuid,in_mac,in_is_has_ip,in_ip,
                      in_is_has_vswitch,in_vswitch_uuid,in_is_has_sriovvf,in_sriov_vf_port_uuid);
  IF result.ret > 0 THEN      
     return result;
  END IF;
    
  -- check vsi_id 是否存在
  SELECT count(*) INTO is_exist FROM vsi_profile WHERE vsi_id_value = in_vsi_id_value;
  SELECT id INTO vsiid FROM vsi_profile WHERE vsi_id_value = in_vsi_id_value;
  SELECT id INTO result.id FROM vsi_profile WHERE vsi_id_value = in_vsi_id_value;  
  
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST; 
      return result;
  END IF; 
  
  -- switch uuid 
  IF in_is_has_vswitch > 0 THEN 
      SELECT count(*) INTO is_exist FROM switch a,virtual_switch b WHERE a.id = b.switch_id AND a.uuid = in_vswitch_uuid;
      SELECT a.id INTO vswitchid FROM switch a,virtual_switch b WHERE a.id = b.switch_id AND a.uuid = in_vswitch_uuid;      
      IF is_exist <= 0 THEN 
         result := RTN_ITEM_NO_EXIST; 
         return result;
      END IF;
  END IF;
  
  -- sriov uuid 
  IF in_is_has_sriovvf > 0 THEN 
      SELECT count(*) INTO is_exist FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.uuid = in_sriov_vf_port_uuid;
      SELECT b.port_id INTO sriovportid FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.uuid = in_sriov_vf_port_uuid;
      IF is_exist <= 0 THEN 
         result := RTN_ITEM_NO_EXIST; 
         return result;
      END IF;
  END IF;
  
  
  SELECT count(*) INTO oldexistswitch FROM vsi_profile WHERE virtual_switch_id is not null  AND vsi_id_value = in_vsi_id_value;
  SELECT virtual_switch_id INTO oldvswitchid FROM vsi_profile WHERE virtual_switch_id is not null  AND vsi_id_value = in_vsi_id_value;
  SELECT sriov_vf_port_id INTO oldexistsriovvfport FROM vsi_sriov WHERE sriov_vf_port_id is not null  AND vsi_profile_id = vsiid;
  
      
  -- update item 
  --result.id := nextval('serial_vsi_profile');
  --vnicid := nextval('serial_vnic');
  --result.uuid := in_uuid;
  result.uuid := in_vsi_id_value;
  BEGIN 
      -- UPDATE vsi profile      
      IF in_is_has_vswitch > 0 THEN 
          UPDATE vsi_profile SET virtual_switch_id = vswitchid WHERE id = vsiid;
          
          -- trigger
          IF oldexistswitch <= 0 THEN
              result.ret :=  pf_net_tr_vsi_attach_vswitch(vsiid, vswitchid);
          END IF;
          
      ELSE           
          -- trigger
          IF oldexistswitch > 0 THEN
              result.ret :=  pf_net_tr_vsi_unattach_vswitch(vsiid,oldvswitchid);
          END IF;
          
          UPDATE vsi_profile SET virtual_switch_id = NULL WHERE id = vsiid;
          
      END IF;
            
      -- UPDATE VNIC 
     
      -- UPDATE vsi_sriov       
      -- 考虑sriov没有修改， 以及sriov vf port 修改~~ 
      -- sriov vf 本身不能修改，只能uuid->null, null->uuid
      IF in_is_has_sriovvf > 0 THEN 
          SELECT count(*) INTO is_exist FROM vsi_sriov WHERE vsi_profile_id = vsiid;
          IF is_exist <= 0 THEN 
              INSERT INTO vsi_sriov(vsi_profile_id,sriov_vf_port_id) values(vsiid,sriovportid);
			  
              -- trigger
              result.ret :=  pf_net_tr_add_vsisriov(vsiid,sriovportid);
          ELSE           
              SELECT count(*) INTO is_exist FROM vsi_sriov WHERE vsi_profile_id = vsiid AND sriov_vf_port_id = sriovportid;
              IF  is_exist <= 0 THEN 
                  DELETE FROM vsi_sriov WHERE vsi_profile_id = vsiid;
                  INSERT INTO vsi_sriov(vsi_profile_id,sriov_vf_port_id) values(vsiid,sriovportid);
				  
                  -- trigger
                  result.ret :=  pf_net_tr_add_vsisriov(vsiid,sriovportid);
              END IF;          
          END IF;
      ELSE      
          SELECT count(*) INTO is_exist FROM vsi_sriov WHERE vsi_profile_id = vsiid;
          SELECT sriov_vf_port_id INTO oldsriovvfportid FROM vsi_sriov WHERE vsi_profile_id = vsiid;

          -- 之前配置了sriov vf 现在freeOnHc后将此sriov vf 释放了，需更新tr
          IF is_exist > 0 THEN               
              -- trigger
              result.ret :=  pf_net_tr_del_vsisriov(vsiid,oldsriovvfportid);
          END IF; 
          
          DELETE FROM vsi_sriov WHERE vsi_profile_id = vsiid;
      END IF;
      
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

                           
                           
-- 判断能否删除
CREATE OR REPLACE FUNCTION pf_net_check_del_vsi(in_vsi_id_value character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  vsiid numeric(19,0);
  
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE integer := 13;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
    
  -- check vsi_id 是否存在
  SELECT count(*) INTO is_exist FROM vsi_profile WHERE vsi_id_value = in_vsi_id_value;
  SELECT id INTO vsiid FROM vsi_profile WHERE vsi_id_value = in_vsi_id_value;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST; 
      return result;
  END IF; 
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 删除 vsi 
CREATE OR REPLACE FUNCTION pf_net_del_vsi(in_vsi_id_value character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  vsiid numeric(19,0);
  sriovvfportid numeric(19,0);
  oldexistswitch integer := 0;
  oldvswitchid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_del_vsi(in_vsi_id_value);
  IF result.ret > 0 THEN      
     return result;
  END IF;
  
    
  -- delete item 
  --result.id := nextval('serial_vsi_profile');
  --vnicid := nextval('serial_vnic');
  --result.uuid := in_uuid;
  BEGIN 
      
      -- trigger
      SELECT count(*) INTO is_exist FROM vsi_sriov a, vsi_profile b WHERE a.vsi_profile_id = b.id AND b.vsi_id_value =  in_vsi_id_value;
      SELECT b.id INTO vsiid FROM vsi_sriov a, vsi_profile b WHERE a.vsi_profile_id = b.id AND b.vsi_id_value =  in_vsi_id_value;
      SELECT a.sriov_vf_port_id INTO sriovvfportid FROM vsi_sriov a, vsi_profile b WHERE a.vsi_profile_id = b.id AND b.vsi_id_value =  in_vsi_id_value;
      IF is_exist > 0 THEN 
          result.ret :=  pf_net_tr_del_vsisriov(vsiid,sriovvfportid);
      END IF;
      
      --trigger 
      SELECT count(*) INTO oldexistswitch FROM vsi_profile WHERE virtual_switch_id is not null  AND vsi_id_value = in_vsi_id_value;
      SELECT virtual_switch_id INTO oldvswitchid FROM vsi_profile WHERE virtual_switch_id is not null  AND vsi_id_value = in_vsi_id_value;  
      IF oldexistswitch > 0 THEN 
          result.ret :=  pf_net_tr_vsi_unattach_vswitch(vsiid,oldvswitchid);
      END IF;
      
      -- DELETE vsi profile      
      DELETE FROM vsi_profile WHERE vsi_id_value =  in_vsi_id_value;   
      
      
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 删除 vsi 通过vm_id
CREATE OR REPLACE FUNCTION pf_net_del_vsi_by_vmid(in_vm_id numeric(19,0))
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  oldexistswitch integer := 0;
  oldvswitchid numeric(19,0);
  msriovlists RECORD;
  mswitchlists RECORD;
  
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  
  SELECT count(*) INTO is_exist FROM vnic WHERE vm_id = in_vm_id;
  IF is_exist <= 0 THEN 
      result.ret := RTN_ITEM_NO_EXIST; 
      return result;
  END IF; 
  
  -- delete item 
  --result.id := nextval('serial_vsi_profile');
  --vnicid := nextval('serial_vnic');
  --result.uuid := in_uuid;
  BEGIN 
     -- trigger 
     FOR msriovlists IN SELECT b.vsi_profile_id AS vsiid, b.sriov_vf_port_id AS sriovvfportid FROM vnic a, vsi_sriov b 
        WHERE a.vsi_profile_id = b.vsi_profile_id AND a.vm_id = in_vm_id LOOP
         result.ret :=  pf_net_tr_del_vsisriov(msriovlists.vsiid, msriovlists.sriovvfportid);
     END LOOP;
     
     --trigger
     FOR mswitchlists IN SELECT b.id AS vsiid, b.virtual_switch_id  FROM vnic a, vsi_profile b 
        WHERE a.vsi_profile_id = b.id AND a.vm_id = in_vm_id AND b.virtual_switch_id is not null LOOP
         result.ret :=  pf_net_tr_vsi_unattach_vswitch(mswitchlists.vsiid, mswitchlists.virtual_switch_id);
     END LOOP;
     
      -- DELETE vsi profile      
      DELETE FROM vsi_profile WHERE id in (SELECT vnic.vsi_profile_id FROM vnic, vm WHERE vm.vm_id = vnic.vm_id AND vm.vm_id = in_vm_id );  
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 查询 (通过vsi_id_value)
CREATE OR REPLACE FUNCTION pf_net_query_vsi(in_vsi_id_value character varying)
  RETURNS t_query_vsi AS
$BODY$
DECLARE
    result t_query_vsi;
    is_exist integer; 
    vsiid numeric(19,0);
    logicnetworkid numeric(19,0);
    vswitchid numeric(19,0);
    sriovvfportid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN  
    result.ret := 0;
    SELECT count(*) INTO is_exist FROM vsi_profile WHERE vsi_id_value = in_vsi_id_value;
    SELECT id INTO vsiid FROM vsi_profile WHERE vsi_id_value = in_vsi_id_value;
    IF is_exist <= 0 THEN 
        result.ret := RTN_ITEM_NO_EXIST; 
        return result;
    END IF; 

    -- vnic 
    SELECT vm_id INTO result.vm_id FROM vnic WHERE vsi_profile_id = vsiid;
        
    SELECT nic_index INTO result.nic_index FROM vnic WHERE vsi_profile_id = vsiid;
    SELECT is_sriov INTO result.is_sriov FROM vnic WHERE vsi_profile_id = vsiid;
    SELECT is_loop INTO result.is_loop FROM vnic WHERE vsi_profile_id = vsiid;
    SELECT logic_network_uuid INTO result.logic_network_uuid FROM vnic WHERE vsi_profile_id = vsiid;
    SELECT port_type INTO result.port_type FROM vnic WHERE vsi_profile_id = vsiid;
        
    -- vsi profile 
    result.vsi_id_value := in_vsi_id_value;
    SELECT vsi_id_format INTO result.vsi_id_format FROM vsi_profile WHERE vsi_id_value = in_vsi_id_value;
    
    -- vm pg uuid 
    SELECT b.uuid INTO result.vm_pg_uuid FROM vsi_profile a, port_group b WHERE a.vm_pg_id = b.id AND a.id = vsiid;
    
    -- mac 
    SELECT b.mac INTO result.mac FROM vsi_profile a, mac_pool b WHERE a.mac_id = b.id AND a.id = vsiid;
    
    -- ip 
    SELECT count(*) INTO is_exist FROM vsi_profile  WHERE id = vsiid AND logic_network_ip_id is not null;
    SELECT logic_network_ip_id INTO logicnetworkid FROM vsi_profile  WHERE id = vsiid AND logic_network_ip_id is not null;    
    IF is_exist <= 0 THEN 
        result.is_has_ip := 0;
    ELSE 
        result.is_has_ip := 1;
        SELECT ip INTO result.ip FROM logic_network_ip_pool WHERE id = logicnetworkid;
        SELECT mask INTO result.mask FROM logic_network_ip_pool WHERE id = logicnetworkid;
    END IF;
    
    -- vswitch 
    SELECT count(*) INTO is_exist FROM vsi_profile  WHERE id = vsiid AND virtual_switch_id is not null;
    SELECT virtual_switch_id INTO vswitchid FROM vsi_profile  WHERE id = vsiid AND virtual_switch_id is not null;    
    IF is_exist <= 0 THEN 
        result.is_has_vswitch := 0;
    ELSE 
        result.is_has_vswitch := 1;
        SELECT uuid INTO result.vswitch_uuid FROM switch WHERE id = vswitchid;
    END IF;
    
    -- sriov 
    SELECT count(*) INTO is_exist FROM vsi_sriov  WHERE vsi_profile_id = vsiid;
    SELECT sriov_vf_port_id INTO sriovvfportid FROM vsi_sriov  WHERE vsi_profile_id = vsiid;    
    IF is_exist <= 0 THEN 
        result.is_has_sriovvf := 0;
    ELSE 
        result.is_has_sriovvf := 1;
        SELECT uuid INTO result.sriovvfport_uuid FROM port WHERE id = sriovvfportid;
    END IF;
            
    return result;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 查询 (通过vm_id nic_indxe)

CREATE OR REPLACE FUNCTION pf_net_query_vsi_by_vmnic(in_vm_id numeric(19,0),in_nic_index integer)
  RETURNS t_query_vsi AS
$BODY$
DECLARE
    result t_query_vsi;
    is_exist integer; 
    vsiid numeric(19,0);
    logicnetworkid numeric(19,0);
    vswitchid numeric(19,0);
    sriovvfportid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN  
    result.ret := 0;
    SELECT count(*) INTO is_exist FROM vnic WHERE vm_id = in_vm_id AND nic_index = in_nic_index;
    SELECT vsi_profile_id INTO vsiid FROM vnic WHERE vm_id = in_vm_id AND nic_index = in_nic_index;
    IF is_exist <= 0 THEN 
        result.ret := RTN_ITEM_NO_EXIST; 
        return result;
    END IF; 

    -- vnic 
    SELECT vm_id INTO result.vm_id FROM vnic WHERE vsi_profile_id = vsiid;
    
    SELECT nic_index INTO result.nic_index FROM vnic WHERE vsi_profile_id = vsiid;
    SELECT is_sriov INTO result.is_sriov FROM vnic WHERE vsi_profile_id = vsiid;
    SELECT is_loop INTO result.is_loop FROM vnic WHERE vsi_profile_id = vsiid;
    SELECT logic_network_uuid INTO result.logic_network_uuid FROM vnic WHERE vsi_profile_id = vsiid;
    SELECT port_type INTO result.port_type FROM vnic WHERE vsi_profile_id = vsiid;
            
    -- vsi profile 
    SELECT vsi_id_value INTO result.vsi_id_value FROM vsi_profile WHERE id = vsiid;
    SELECT vsi_id_format INTO result.vsi_id_format FROM vsi_profile WHERE id = vsiid;
    
    -- vm pg uuid 
    SELECT b.uuid INTO result.vm_pg_uuid FROM vsi_profile a, port_group b WHERE a.vm_pg_id = b.id AND a.id = vsiid;
    
    -- mac 
    SELECT b.mac INTO result.mac FROM vsi_profile a, mac_pool b WHERE a.mac_id = b.id AND a.id = vsiid;
    
    -- ip 
    SELECT count(*) INTO is_exist FROM vsi_profile  WHERE id = vsiid AND logic_network_ip_id is not null;
    SELECT logic_network_ip_id INTO logicnetworkid FROM vsi_profile  WHERE id = vsiid AND logic_network_ip_id is not null;    
    IF is_exist <= 0 THEN 
        result.is_has_ip := 0;
    ELSE 
        result.is_has_ip := 1;
        SELECT ip INTO result.ip FROM logic_network_ip_pool WHERE id = logicnetworkid;
        SELECT mask INTO result.mask FROM logic_network_ip_pool WHERE id = logicnetworkid;
    END IF;
    
    -- vswitch 
    SELECT count(*) INTO is_exist FROM vsi_profile  WHERE id = vsiid AND virtual_switch_id is not null;
    SELECT virtual_switch_id INTO vswitchid FROM vsi_profile  WHERE id = vsiid AND virtual_switch_id is not null;    
    IF is_exist <= 0 THEN 
        result.is_has_vswitch := 0;
    ELSE 
        result.is_has_vswitch := 1;
        SELECT uuid INTO result.vswitch_uuid FROM switch WHERE id = vswitchid;
    END IF;
    
    -- sriov 
    SELECT count(*) INTO is_exist FROM vsi_sriov  WHERE vsi_profile_id = vsiid;
    SELECT sriov_vf_port_id INTO sriovvfportid FROM vsi_sriov  WHERE vsi_profile_id = vsiid;    
    IF is_exist <= 0 THEN 
        result.is_has_sriovvf := 0;
    ELSE 
        result.is_has_sriovvf := 1;
        SELECT uuid INTO result.sriovvfport_uuid FROM port WHERE id = sriovvfportid;
    END IF;
            
    return result;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 查询 vsi summary ( all)

CREATE OR REPLACE FUNCTION pf_net_query_vsisummary()
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    vsiid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
  
    OPEN ref FOR SELECT a.vm_id, a.nic_index, c.project_id, b.vsi_id_value, b.vsi_id_format 
      FROM vnic a, vsi_profile b , vm c
      WHERE a.vsi_profile_id = b.id  AND c.vm_id = a.vm_id;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 查询 vsi summary ( by vm_id)

CREATE OR REPLACE FUNCTION pf_net_query_vsisummary_by_vm(in_vm_id numeric(19,0) )
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    vsiid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
     -- vm id
    SELECT count(*) INTO is_exist FROM vnic WHERE vm_id = in_vm_id;
    IF is_exist <= 0 THEN 
       --result := RTN_ITEM_NO_EXIST; 
       return ref;
    END IF;  
    
    SELECT count(*) INTO is_exist FROM vnic a, vsi_profile b   
      WHERE a.vsi_profile_id = b.id AND a.vm_id = in_vm_id;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF; 
  
    OPEN ref FOR SELECT a.vm_id, a.nic_index, c.project_id, b.vsi_id_value, b.vsi_id_format 
      FROM vnic a, vsi_profile b ,vm c
      WHERE a.vsi_profile_id = b.id AND c.vm_id = a.vm_id AND a.vm_id = in_vm_id;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 34 NEW_schedule



--调度模块 检查资源 mac 是否可用 
CREATE OR REPLACE FUNCTION pf_net_check_schedule_resource_mac(in_netplane_uuid character varying, in_mac_num integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  total_num numeric(19,0) := 0;
  alloc_num numeric(19,0) := 0;
  free_num numeric(19,0) := 0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE integer := 13;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
  RTN_NO_MAC_RESOURCE  integer := 15;
BEGIN
  result := 0;
  
  SELECT count(*) INTO is_exist FROM mac_range a, netplane b WHERE a.netplane_id = b.id AND b.uuid = in_netplane_uuid;
  SELECT sum(mac_end_num - mac_begin_num +1) INTO total_num FROM mac_range a, netplane b WHERE a.netplane_id = b.id AND b.uuid = in_netplane_uuid;
  IF is_exist <= 0 THEN 
      result := RTN_NO_MAC_RESOURCE;
      return result;
  END IF;
  
  SELECT count(*) INTO alloc_num FROM mac_pool a, netplane b WHERE a.netplane_id = b.id AND b.uuid = in_netplane_uuid;
  
  free_num := (total_num - alloc_num);
  free_num := (free_num - in_mac_num);
  IF free_num < 0 THEN 
      result := RTN_NO_MAC_RESOURCE;
      return result;
  END IF;
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  

--调度模块 检查资源 IP 是否可用 
CREATE OR REPLACE FUNCTION pf_net_check_schedule_resource_ip(in_logicnetwork_uuid character varying, in_ip_num integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  total_num numeric(19,0) := 0;
  alloc_num numeric(19,0) := 0;
  free_num numeric(19,0) := 0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE integer := 13;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
  RTN_NO_MAC_RESOURCE  integer := 15;
BEGIN
  result := 0;

  SELECT count(*) INTO is_exist FROM logic_network_ip_range a, logic_network b WHERE a.logic_network_id = b.id AND b.uuid = in_logicnetwork_uuid AND b.ip_bootprotocolmode = 0;
  IF is_exist <= 0 THEN 
      return result;
  END IF;
  
  SELECT count(*) INTO is_exist FROM logic_network_ip_range a, logic_network b WHERE a.logic_network_id = b.id AND b.uuid = in_logicnetwork_uuid;
  SELECT sum(ip_end_num - ip_begin_num +1) INTO total_num  FROM logic_network_ip_range a, logic_network b WHERE a.logic_network_id = b.id AND b.uuid = in_logicnetwork_uuid;
  IF is_exist <= 0 THEN 
      result := RTN_NO_MAC_RESOURCE;
      return result;
  END IF;
  
  SELECT count(*) INTO alloc_num FROM logic_network_ip_pool a, logic_network b WHERE a.logic_network_id = b.id AND b.uuid = in_logicnetwork_uuid;
  free_num := (total_num - alloc_num);
  free_num := (free_num - in_ip_num);
  IF free_num < 0 THEN 
      result := RTN_NO_MAC_RESOURCE;
      return result;
  END IF;
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


--调度模块 检查资源 vlan 是否可用 
CREATE OR REPLACE FUNCTION pf_net_check_schedule_resource_vlan(in_project_id numeric(19,0), in_vmpg_uuid character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  total_num numeric(19,0) := 0;
  alloc_num numeric(19,0) := 0;
  free_num numeric(19,0) := 0;
  
  switchportmode integer;
  isolateno integer;
  pgtype integer;
  
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE integer := 13;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
  RTN_NO_MAC_RESOURCE  integer := 15;
BEGIN
  result := 0;
  
  -- vm pg, switchport_mode 
  SELECT count(*) INTO is_exist FROM port_group WHERE uuid = in_vmpg_uuid AND netplane_id is not null;
  SELECT switchport_mode INTO switchportmode FROM port_group WHERE uuid = in_vmpg_uuid;
  SELECT pg_type INTO pgtype FROM port_group WHERE uuid = in_vmpg_uuid;
  SELECT netplane_id INTO netplaneid FROM port_group WHERE uuid = in_vmpg_uuid;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
  END IF;
  
  -- 必须是vm pg, pg_type = 20/21
  IF pgtype >= 20 THEN 
      IF  pgtype <= 21 THEN 
          result := 0;
      ELSE
          result := RTN_PARAM_INVALID;
          return result;
      END IF;
  END IF;
  
  -- 如果trunk， vlan资源认为是ok的
  IF switchportmode = 0 THEN 
      result := 0;
      return result;
  END IF;
  
  -- access 模式 vm_share 
  IF switchportmode = 1 THEN 
     IF pgtype = 20 THEN 
         result := 0;
         return result;     
     END IF;
  END IF;
  
  -- access 模式 vm_private 
  SELECT count(*) INTO is_exist FROM port_group a, pg_switchport_access_isolate b WHERE a.id = b.pg_id AND a.uuid = in_vmpg_uuid;
  SELECT isolate_no INTO isolateno FROM port_group a, pg_switchport_access_isolate b WHERE a.id = b.pg_id AND a.uuid = in_vmpg_uuid;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
  END IF;
  
  -- vlan_map project_id +  isolate_no 
  SELECT count(*) INTO is_exist FROM vlan_map WHERE project_id = in_project_id AND isolate_no = isolateno;
  IF is_exist > 0 THEN 
      result := 0;
      return result;
  END IF;
  
  --vlan_range   
  SELECT count(*) INTO is_exist FROM vlan_range WHERE netplane_id = netplaneid;
  SELECT sum(vlan_end - vlan_begin +1) INTO total_num  FROM vlan_range WHERE netplane_id = netplaneid;
  IF is_exist <= 0 THEN 
      result := RTN_NO_MAC_RESOURCE;
      return result;
  END IF;
  
  SELECT count(*) INTO alloc_num FROM vlan_pool WHERE netplane_id = netplaneid;
  free_num := (total_num - alloc_num);
  IF free_num <= 0 THEN 
      result := RTN_NO_MAC_RESOURCE;
      return result;
  END IF;
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 调度模块 获取 sdvs has wdg 
CREATE OR REPLACE FUNCTION pf_net_sdvs_schedule_has_wdg(in_netplane_id numeric(19,0), in_netplane_uuid character varying, 
                           in_is_sriov integer, in_is_loop integer, in_nic_num integer, in_is_bond_nic integer,
                           in_mtu integer , in_is_watchdog integer,
                           in_cluster_id numeric(19,0), in_cluster_name character varying, 
                           in_vna_id numeric(19,0) )                           
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    check_ret integer;
    netplaneid numeric(19,0);
    
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    /* input param valid: in_netplane_id, in_nic_num, in_mtu, in_is_loop          
     switch chioce  */   
    IF in_cluster_id > 0 THEN 
        IF in_vna_id > 0 THEN 
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,
                 cluster_name,vna_uuid,module_name, switch_uuid, switch_sriov_free_num, switch_free_num, 
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_sdvs_schedule_has_wdg 
            WHERE 
                 cluster_name = in_cluster_name AND vna_id = in_vna_id AND
                 /* cluster_id = in_cluster_id AND vna_id = in_vna_id AND      */
                 /*cluster vna  filter*/
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu;
                 
        ELSE
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,
                 cluster_name,vna_uuid,module_name, switch_uuid, switch_sriov_free_num, switch_free_num, 
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_sdvs_schedule_has_wdg 
            WHERE
                 cluster_name = in_cluster_name AND 
                 /*cluster_id = in_cluster_id AND */   /* cluster filter   */
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu;        
        END IF;             
    ELSE
         IF in_vna_id > 0 THEN 
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,
                 cluster_name,vna_uuid,module_name, switch_uuid, switch_sriov_free_num, switch_free_num, 
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_sdvs_schedule_has_wdg 
            WHERE 
                 vna_id = in_vna_id AND     /*  vna filter */
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu;    
        ELSE
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,
                 cluster_name,vna_uuid,module_name, switch_uuid, switch_sriov_free_num, switch_free_num, 
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_sdvs_schedule_has_wdg 
            WHERE netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu;        
        END IF;    
    END IF;         
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 调度模块 获取 sdvs no wdg 
CREATE OR REPLACE FUNCTION pf_net_sdvs_schedule_no_wdg(in_netplane_id numeric(19,0), in_netplane_uuid character varying, 
                           in_is_sriov integer, in_is_loop integer, in_nic_num integer, in_is_bond_nic integer,
                           in_mtu integer , in_is_watchdog integer,
                           in_cluster_id numeric(19,0), in_cluster_name character varying, 
                           in_vna_id numeric(19,0) )                           
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    check_ret integer;
    netplaneid numeric(19,0);
    
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    /* input param valid: in_netplane_id, in_nic_num, in_mtu, in_is_loop          
     switch chioce   */
    IF in_cluster_id > 0 THEN 
        IF in_vna_id > 0 THEN     
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,
                 cluster_name,vna_uuid,module_name, switch_uuid, switch_sriov_free_num, switch_free_num, 
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_sdvs_schedule_no_wdg 
            WHERE 
                 cluster_name = in_cluster_name AND vna_id = in_vna_id AND 
                 /* cluster_id = in_cluster_id AND vna_id = in_vna_id AND */    /* cluster vna filter */
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu;    
        ELSE
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,
                 cluster_name,vna_uuid,module_name, switch_uuid, switch_sriov_free_num, switch_free_num, 
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_sdvs_schedule_no_wdg 
            WHERE 
                 cluster_name = in_cluster_name AND 
                 /* cluster_id = in_cluster_id AND */   /* cluster filter */
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu;    
        END IF;
    ELSE
        IF in_vna_id > 0 THEN     
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,
                 cluster_name,vna_uuid,module_name, switch_uuid, switch_sriov_free_num, switch_free_num, 
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_sdvs_schedule_no_wdg 
            WHERE 
                 vna_id = in_vna_id AND     /*  vna filter */
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu;    
        ELSE
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,
                 cluster_name,vna_uuid,module_name, switch_uuid, switch_sriov_free_num, switch_free_num, 
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_sdvs_schedule_no_wdg 
            WHERE 
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu;    
        END IF;
    END IF;
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
       
                           


-- 调度模块 获取 edvs no bond no wdg 
CREATE OR REPLACE FUNCTION pf_net_edvs_schedule_no_bond_no_wdg(in_netplane_id numeric(19,0), in_netplane_uuid character varying, 
                           in_is_sriov integer, in_is_loop integer, in_nic_num integer, in_is_bond_nic integer,
                           in_mtu integer , in_is_watchdog integer,
                           in_cluster_id numeric(19,0),  in_cluster_name character varying, 
                           in_vna_id numeric(19,0) )                           
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    check_ret integer;
    netplaneid numeric(19,0);
    
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    /* input param valid: in_netplane_id, in_nic_num, in_mtu, in_is_loop          
     switch chioce  */
    /*
       edvs physical, switch_sriov_free_num 存放的是 physical口剩余可用VF数目
    */
    IF in_cluster_id > 0 THEN 
        IF in_vna_id > 0 THEN     
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 port_free_sriovvf_num,port_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_edvs_schedule_no_bond_no_wdg 
            WHERE 
                 cluster_name = in_cluster_name AND vna_id = in_vna_id AND 
                 /* cluster_id = in_cluster_id AND vna_id = in_vna_id AND */     /* cluster vna filter */
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu;        
        ELSE
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 port_free_sriovvf_num,port_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_edvs_schedule_no_bond_no_wdg 
            WHERE 
                 cluster_name = in_cluster_name AND
                 /* cluster_id = in_cluster_id AND  */   /* cluster filter */
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu;    
        END IF;
    ELSE
        IF in_vna_id > 0 THEN     
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 port_free_sriovvf_num,port_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_edvs_schedule_no_bond_no_wdg 
            WHERE 
                 vna_id = in_vna_id AND     /* vna filter */
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu;        
        ELSE
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 port_free_sriovvf_num,port_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_edvs_schedule_no_bond_no_wdg 
            WHERE 
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu;    
        END IF;
    END IF;    
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
                                  

-- 调度模块 获取 edvs has bond no wdg 
CREATE OR REPLACE FUNCTION pf_net_edvs_schedule_has_bond_no_wdg(in_netplane_id numeric(19,0), in_netplane_uuid character varying, 
                           in_is_sriov integer, in_is_loop integer, in_nic_num integer, in_is_bond_nic integer,
                           in_mtu integer , in_is_watchdog integer,
                           in_cluster_id numeric(19,0), in_cluster_name character varying, 
                           in_vna_id numeric(19,0))                               
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    check_ret integer;
    netplaneid numeric(19,0);
    
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    /* input param valid: in_netplane_id, in_nic_num, in_mtu, in_is_loop          
     switch chioce     */
    /*
       edvs bond, switch_sriov_free_num 存放的是 bond口剩余可用VF数目
    */
    IF in_cluster_id > 0 THEN 
        IF in_vna_id > 0 THEN         
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 bond_free_sriovvf_num,bond_free_sriovvf_num, 
                 /*switch_sriov_free_num,switch_free_num, */ 
                 pg_mtu,  port_uuid,
                 port_type,bond_total_phy_num,bond_valid_phy_num,phy_port_uuid,phy_port_free_sriovvf_num
            FROM v_net_edvs_schedule_has_bond_no_wdg 
            WHERE             
                 cluster_name = in_cluster_name AND vna_id = in_vna_id AND 
                 /* cluster_id = in_cluster_id AND vna_id = in_vna_id AND */    /* cluster vna  filter */
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu 
                 AND bond_valid_phy_num > 0 ;    
        ELSE
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 bond_free_sriovvf_num,bond_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num,*/
                 pg_mtu,  port_uuid,
                 port_type,bond_total_phy_num,bond_valid_phy_num,phy_port_uuid,phy_port_free_sriovvf_num
            FROM v_net_edvs_schedule_has_bond_no_wdg 
            WHERE             
                 cluster_name = in_cluster_name AND 
                 /* cluster_id = in_cluster_id AND */    /* cluster vna filter */
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu 
                 AND bond_valid_phy_num > 0 ;
        END IF;
    ELSE
        IF in_vna_id > 0 THEN         
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 bond_free_sriovvf_num,bond_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,bond_total_phy_num,bond_valid_phy_num,phy_port_uuid,phy_port_free_sriovvf_num
            FROM v_net_edvs_schedule_has_bond_no_wdg 
            WHERE             
                 vna_id = in_vna_id AND     /* vna filter   */
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu 
                 AND bond_valid_phy_num > 0 ;    
        ELSE
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 bond_free_sriovvf_num,bond_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,bond_total_phy_num,bond_valid_phy_num,phy_port_uuid,phy_port_free_sriovvf_num
            FROM v_net_edvs_schedule_has_bond_no_wdg 
            WHERE             
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu 
                 AND bond_valid_phy_num > 0 ;
        END IF;
    END IF;    
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
        
                           

-- 调度模块 获取 edvs no bond has wdg 
CREATE OR REPLACE FUNCTION pf_net_edvs_schedule_no_bond_has_wdg(in_netplane_id numeric(19,0), in_netplane_uuid character varying, 
                           in_is_sriov integer, in_is_loop integer, in_nic_num integer, in_is_bond_nic integer,
                           in_mtu integer , in_is_watchdog integer,
                           in_cluster_id numeric(19,0), in_cluster_name character varying, 
                           in_vna_id numeric(19,0))                               
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    check_ret integer;
    netplaneid numeric(19,0);
    
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    /* input param valid: in_netplane_id, in_nic_num, in_mtu, in_is_loop          
     switch chioce  */ 
    /*
       edvs physical, switch_sriov_free_num 存放的是 phy口剩余可用VF数目
    */
    
    IF in_cluster_id > 0 THEN 
        IF in_vna_id > 0 THEN         
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 port_free_sriovvf_num,port_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num,  */
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_edvs_schedule_no_bond_has_wdg 
            WHERE 
                 cluster_name = in_cluster_name AND vna_id = in_vna_id AND 
                 -- cluster_id = in_cluster_id AND vna_id = in_vna_id AND     -- cluster vna filter
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu ; 
        ELSE
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 port_free_sriovvf_num,port_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_edvs_schedule_no_bond_has_wdg 
            WHERE 
                 cluster_name = in_cluster_name AND 
                 -- cluster_id = in_cluster_id AND     -- cluster vna filter
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu ; 
        END IF;
    ELSE
        IF in_vna_id > 0 THEN         
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 port_free_sriovvf_num,port_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_edvs_schedule_no_bond_has_wdg 
            WHERE 
                 vna_id = in_vna_id AND     -- cluster vna filter
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu ; 
        ELSE
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 port_free_sriovvf_num,port_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_edvs_schedule_no_bond_has_wdg 
            WHERE 
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu ; 
        END IF;
    END IF;    
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
            

-- 调度模块 获取 edvs has bond has wdg 
CREATE OR REPLACE FUNCTION pf_net_edvs_schedule_has_bond_has_wdg(in_netplane_id numeric(19,0), in_netplane_uuid character varying, 
                           in_is_sriov integer, in_is_loop integer, in_nic_num integer, in_is_bond_nic integer,
                           in_mtu integer , in_is_watchdog integer,
                           in_cluster_id numeric(19,0), in_cluster_name character varying, 
                           in_vna_id numeric(19,0))                               
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    check_ret integer;
    netplaneid numeric(19,0);
    
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    /* input param valid: in_netplane_id, in_nic_num, in_mtu, in_is_loop          
     switch chioce     */
    /*
       edvs bond, switch_sriov_free_num 存放的是 bond口剩余可用VF数目
    */
         
    IF in_cluster_id > 0 THEN 
        IF in_vna_id > 0 THEN     
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 bond_free_sriovvf_num,bond_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,bond_total_phy_num,bond_valid_phy_num,phy_port_uuid,phy_port_free_sriovvf_num
            FROM v_net_edvs_schedule_has_bond_has_wdg 
            WHERE 
                 cluster_name = in_cluster_name AND vna_id = in_vna_id AND 
                 /* cluster_id = in_cluster_id AND vna_id = in_vna_id AND */    /* cluster vna filter */
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu 
                 AND bond_valid_phy_num > 0 ;
        ELSE
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 bond_free_sriovvf_num,bond_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,bond_total_phy_num,bond_valid_phy_num,phy_port_uuid,phy_port_free_sriovvf_num
            FROM v_net_edvs_schedule_has_bond_has_wdg 
            WHERE 
                 cluster_name = in_cluster_name AND 
                 /* cluster_id = in_cluster_id AND */    /*cluster vna filter*/
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu 
                 AND bond_valid_phy_num > 0 ;
        END IF;
    ELSE
        IF in_vna_id > 0 THEN     
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 bond_free_sriovvf_num,bond_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,bond_total_phy_num,bond_valid_phy_num,phy_port_uuid,phy_port_free_sriovvf_num
            FROM v_net_edvs_schedule_has_bond_has_wdg 
            WHERE 
                 vna_id = in_vna_id AND     /* cluster vna filter*/
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu 
                 AND bond_valid_phy_num > 0 ;
        ELSE
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 bond_free_sriovvf_num,bond_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,bond_total_phy_num,bond_valid_phy_num,phy_port_uuid,phy_port_free_sriovvf_num
            FROM v_net_edvs_schedule_has_bond_has_wdg 
            WHERE 
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu 
                 AND bond_valid_phy_num > 0 ;
        END IF;
    END IF;    
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
          
                           
                            

-- 调度模块 获取 switch port vna list
CREATE OR REPLACE FUNCTION pf_net_schedule(in_netplane_uuid character varying, 
                           in_is_sriov integer, in_is_loop integer, in_nic_num integer, in_is_bond_nic integer,
                           in_mtu integer , in_is_watchdog integer)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    check_ret integer;
    netplaneid numeric(19,0);
    
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
   
    SELECT count(*) INTO is_exist  FROM netplane WHERE uuid = in_netplane_uuid;
    SELECT id INTO netplaneid  FROM netplane WHERE uuid = in_netplane_uuid;
    IF is_exist <= 0 THEN 
        -- 直接返回 参数有误
        return ref;
    END IF;
                
    -- switch chioce 
IF     in_is_watchdog <= 0 THEN     
    IF in_is_sriov > 0 THEN       
        IF in_is_bond_nic > 0 THEN 
            return pf_net_edvs_schedule_has_bond_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',0);             
        ELSE        
            return pf_net_edvs_schedule_no_bond_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',0);             
        END IF;
    ELSE
        return pf_net_sdvs_schedule_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',0);    
    END IF;
ELSE
    IF in_is_sriov > 0 THEN       
        IF in_is_bond_nic > 0 THEN 
            return pf_net_edvs_schedule_has_bond_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',0);             
        ELSE        
            return pf_net_edvs_schedule_no_bond_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',0);             
        END IF;
    ELSE
        return pf_net_sdvs_schedule_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',0);    
    END IF;
END IF;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
    

                           
-- 调度模块 获取 switch port vna list (通过cluster)
CREATE OR REPLACE FUNCTION pf_net_schedule_by_cluster(in_netplane_uuid character varying, 
                           in_is_sriov integer, in_is_loop integer, in_nic_num integer, in_is_bond_nic integer,
                           in_mtu integer, in_cluster_name character varying, in_is_watchdog integer)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    check_ret integer;
    netplaneid numeric(19,0);
    clusterid  numeric(19,0);
    
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
   
    SELECT count(*) INTO is_exist  FROM netplane WHERE uuid = in_netplane_uuid;
    SELECT id INTO netplaneid  FROM netplane WHERE uuid = in_netplane_uuid;
    IF is_exist <= 0 THEN 
        -- 直接返回 参数有误
        return ref;
    END IF;
                    
    -- switch chioce 
IF     in_is_watchdog <= 0 THEN     
    IF in_is_sriov > 0 THEN       
        IF in_is_bond_nic > 0 THEN 
            return pf_net_edvs_schedule_has_bond_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,1,in_cluster_name,0);             
        ELSE        
            return pf_net_edvs_schedule_no_bond_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,1,in_cluster_name,0);             
        END IF;
    ELSE
        return pf_net_sdvs_schedule_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,1,in_cluster_name,0);    
    END IF;
ELSE
    IF in_is_sriov > 0 THEN       
        IF in_is_bond_nic > 0 THEN 
            return pf_net_edvs_schedule_has_bond_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,1,in_cluster_name,0);             
        ELSE        
            return pf_net_edvs_schedule_no_bond_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,1,in_cluster_name,0);             
        END IF;
    ELSE
        return pf_net_sdvs_schedule_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,1,in_cluster_name,0);    
    END IF;
END IF;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
    
                           

-- 调度模块 获取 switch port vna list (通过vna)
CREATE OR REPLACE FUNCTION pf_net_schedule_by_vna(in_netplane_uuid character varying, 
                           in_is_sriov integer, in_is_loop integer, in_nic_num integer, in_is_bond_nic integer,
                           in_mtu integer, in_vna_uuid character varying, in_is_watchdog integer)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    check_ret integer;
    netplaneid numeric(19,0);
    vnaid numeric(19,0);
    
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
   
    SELECT count(*) INTO is_exist  FROM netplane WHERE uuid = in_netplane_uuid;
    SELECT id INTO netplaneid  FROM netplane WHERE uuid = in_netplane_uuid;
    IF is_exist <= 0 THEN 
        -- 直接返回 参数有误
        return ref;
    END IF;

    SELECT count(*) INTO is_exist  FROM vna WHERE vna_uuid = in_vna_uuid;
    SELECT id INTO vnaid  FROM vna WHERE vna_uuid = in_vna_uuid;
    IF is_exist <= 0 THEN 
        -- 直接返回 参数有误
        return ref;
    END IF;
    
    -- switch chioce 
IF     in_is_watchdog <= 0 THEN     
    IF in_is_sriov > 0 THEN       
        IF in_is_bond_nic > 0 THEN 
            return pf_net_edvs_schedule_has_bond_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',vnaid);             
        ELSE        
            return pf_net_edvs_schedule_no_bond_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',vnaid);             
        END IF;
    ELSE
        return pf_net_sdvs_schedule_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',vnaid);    
    END IF;
ELSE
    IF in_is_sriov > 0 THEN       
        IF in_is_bond_nic > 0 THEN 
            return pf_net_edvs_schedule_has_bond_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',vnaid);             
        ELSE        
            return pf_net_edvs_schedule_no_bond_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',vnaid);             
        END IF;
    ELSE
        return pf_net_sdvs_schedule_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',vnaid);    
    END IF;
END IF;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
   



-- 调度模块 获取 switch port vna list (通过project id)
CREATE OR REPLACE FUNCTION pf_net_schedule_query_deploy_by_project(in_project_id numeric(19,0), 
                           in_is_sriov integer, in_is_loop integer)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    check_ret integer;
    netplaneid numeric(19,0);
    
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
                   
    -- switch chioce     
    IF in_is_sriov > 0 THEN       
        IF in_is_loop > 0 THEN 
            OPEN ref FOR 
            SELECT distinct 
               in_project_id,logic_network_uuid,in_is_sriov,in_is_loop,
               cluster_name,vna_uuid,module_name, switch_uuid, port_uuid
            FROM v_net_vsi_deploy_vna_switch_port 
            WHERE project_id = in_project_id  
              AND is_sriov = 1  AND is_loop = 1;
        ELSE        
            OPEN ref FOR 
            SELECT distinct 
               in_project_id,logic_network_uuid,in_is_sriov,in_is_loop,
               cluster_name,vna_uuid,module_name, switch_uuid, port_uuid
            FROM v_net_vsi_deploy_vna_switch_port 
            WHERE project_id = in_project_id 
               AND is_sriov = 1  AND is_loop = 0;
        END IF;
    ELSE
        IF in_is_loop > 0 THEN 
            OPEN ref FOR 
            SELECT distinct 
               in_project_id,logic_network_uuid,in_is_sriov,in_is_loop,
               cluster_name,vna_uuid,module_name, switch_uuid, port_uuid
            FROM v_net_vsi_deploy_vna_switch_port 
            WHERE project_id = in_project_id  AND is_sriov = 0  
               AND is_loop = 1;        
        ELSE        
            OPEN ref FOR 
            SELECT distinct 
               in_project_id,logic_network_uuid,in_is_sriov,in_is_loop,
               cluster_name,vna_uuid,module_name, switch_uuid, port_uuid
            FROM v_net_vsi_deploy_vna_switch_port 
            WHERE project_id = in_project_id  AND is_sriov = 0  
               AND is_loop = 0;    
        END IF;
    END IF;
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
          



-- 调度模块 获取 switch port vna list (通过project id, logicnetwork)
CREATE OR REPLACE FUNCTION pf_net_schedule_query_deploy_by_project_ln(in_project_id numeric(19,0), in_lgnetwork_uuid character varying,
                           in_is_sriov integer, in_is_loop integer)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    check_ret integer;
    netplaneid numeric(19,0);
    
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
                   
    -- switch chioce     
    IF in_is_sriov > 0 THEN       
        IF in_is_loop > 0 THEN 
            OPEN ref FOR 
            SELECT distinct 
               in_project_id,in_lgnetwork_uuid,in_is_sriov,in_is_loop,
               cluster_name,vna_uuid,module_name, switch_uuid, port_uuid
            FROM v_net_vsi_deploy_vna_switch_port 
            WHERE project_id = in_project_id  AND logic_network_uuid = in_lgnetwork_uuid 
              AND is_sriov = 1  AND is_loop = 1;
        ELSE        
            OPEN ref FOR 
            SELECT distinct 
               in_project_id,in_lgnetwork_uuid,in_is_sriov,in_is_loop,
               cluster_name,vna_uuid,module_name, switch_uuid, port_uuid
            FROM v_net_vsi_deploy_vna_switch_port 
            WHERE project_id = in_project_id AND logic_network_uuid = in_lgnetwork_uuid 
               AND is_sriov = 1  AND is_loop = 0;
        END IF;
    ELSE
        IF in_is_loop > 0 THEN 
            OPEN ref FOR 
            SELECT distinct 
               in_project_id,in_lgnetwork_uuid,in_is_sriov,in_is_loop,
               cluster_name,vna_uuid,module_name, switch_uuid, port_uuid
            FROM v_net_vsi_deploy_vna_switch_port 
            WHERE project_id = in_project_id  AND logic_network_uuid = in_lgnetwork_uuid  AND is_sriov = 0  
               AND is_loop = 1;        
        ELSE        
            OPEN ref FOR 
            SELECT distinct 
               in_project_id,in_lgnetwork_uuid,in_is_sriov,in_is_loop,
               cluster_name,vna_uuid,module_name, switch_uuid, port_uuid
            FROM v_net_vsi_deploy_vna_switch_port 
            WHERE project_id = in_project_id  AND logic_network_uuid = in_lgnetwork_uuid  AND is_sriov = 0  
               AND is_loop = 0;    
        END IF;
    END IF;
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 调度模块 获取 wdg vna list ()
CREATE OR REPLACE FUNCTION pf_net_schedule_wdg()
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    check_ret integer;
    netplaneid numeric(19,0);
    
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
                   
    OPEN ref FOR 
    SELECT 
      distinct 
      vna_uuid,module_name,cluster_name
    FROM v_net_vna_hc_cluster 
    WHERE         
      module_is_online = 1 
      AND vna_is_online = 1
      AND is_has_watchdog = 1 AND watchdog_is_online = 1;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 调度模块 获取 wdg vna list (通过cluster)
CREATE OR REPLACE FUNCTION pf_net_schedule_wdg_by_cluster(in_cluster character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    check_ret integer;
    netplaneid numeric(19,0);
    
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
                   
    OPEN ref FOR 
    SELECT 
      distinct 
      vna_uuid,module_name,cluster_name
    FROM v_net_vna_hc_cluster 
    WHERE         
      module_is_online = 1 
      AND vna_is_online = 1
      AND is_has_watchdog = 1 AND watchdog_is_online = 1
      AND cluster_name = in_cluster;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 调度模块 获取 wdg vna list (通过vna)
CREATE OR REPLACE FUNCTION pf_net_schedule_wdg_by_vna(in_vna_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    check_ret integer;
    netplaneid numeric(19,0);
    
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
                   
    OPEN ref FOR 
    SELECT 
      distinct 
      vna_uuid,module_name,cluster_name
    FROM v_net_vna_hc_cluster 
    WHERE         
      module_is_online = 1 
      AND vna_is_online = 1
      AND is_has_watchdog = 1 AND watchdog_is_online = 1
      AND vna_uuid = in_vna_uuid;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  
-- 40 kernel_report



-- 判断逻辑网络能否添加
CREATE OR REPLACE FUNCTION pf_net_check_add_kernelreport(in_vna_uuid character varying, in_uuid character varying, in_switch_name character varying, 
                 in_ip character varying,in_mask character varying,in_is_online integer, in_kernel_port_name character varying,
                 in_uplink_port_name character varying, in_uplink_port_type integer, in_uplink_port_mode integer, in_is_dhcp integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  vnaid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_SWITCH_UPLINKPORT_INVALID integer := 9;
  RTN_PORT_HAS_JOIN_OTHER_SWITCH integer := 10;
  RTN_PG_MUST_CFG_NETPLANE   integer := 13;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- vna uuid 
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;  
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- check uplink port type
  IF in_uplink_port_type < 0 THEN 
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;
  
  IF in_uplink_port_type > 1 THEN 
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;
  
  -- check uplink port bond mode 
  IF in_uplink_port_type = 1 THEN 
     IF in_uplink_port_mode = 1 THEN 
         result := 0; 
     ELSE
         IF in_uplink_port_mode = 4 THEN 
             result := 0; 
         ELSE             
             result := RTN_PARAM_INVALID; 
             return result;
         END IF;
     END IF;
  END IF;
    
  -- 查看是否重复添加 
  SELECT count(*) INTO is_exist FROM kernel_cfg_report WHERE vna_id = vnaid AND switch_name = in_switch_name;
  IF is_exist > 0 THEN 
     SELECT count(*) INTO is_same FROM kernel_cfg_report WHERE vna_id = vnaid AND switch_name = in_switch_name 
               AND ip = in_ip AND mask = in_mask AND is_online = in_is_online;
     IF is_same > 0 THEN 
         result := RTN_HAS_SAME_ITEM; 
         return result;
     ELSE 
         result := RTN_ITEM_CONFLICT; 
         return result;
     END IF; 
  END IF;
  
  -- 一个vna只能一个kernel report
  SELECT count(*) INTO is_exist FROM kernel_cfg_report WHERE vna_id = vnaid;
  IF is_exist > 0 THEN 
      result := RTN_ITEM_CONFLICT; 
      return result;
  END IF;
          
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 逻辑kernel_report添加 
-- Function: pf_net_add_kernelreport(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_kernelreport(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_kernelreport(in_vna_uuid character varying, in_uuid character varying, in_switch_name character varying, 
                 in_ip character varying,in_mask character varying,in_is_online integer,in_kernel_port_name character varying,
                 in_uplink_port_name character varying, in_uplink_port_type integer, in_uplink_port_mode integer,in_is_dhcp integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  vnaid numeric(19,0);
  uplinkportid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_add_kernelreport(in_vna_uuid,in_uuid,in_switch_name,in_ip,in_mask,in_is_online,in_kernel_port_name,
                  in_uplink_port_name,in_uplink_port_type,in_uplink_port_mode,in_is_dhcp);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  -- vna uuid 
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;  
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
  
  -- 不支持重复添加 

  -- insert item 
  result.id := nextval('serial_kernel_cfg_report');
  --uplinkportid ：= nextval('serial_kernel_cfg_report_bond');
  --result.uuid := in_uuid;
  BEGIN 
      INSERT INTO kernel_cfg_report(id,uuid,vna_id,switch_name,ip, mask,is_online,kernel_port_name, uplink_port_name, uplink_port_type, is_dhcp) 
          values(result.id,in_uuid,vnaid,in_switch_name,in_ip,in_mask,in_is_online,in_kernel_port_name,in_uplink_port_name,in_uplink_port_type, in_is_dhcp);
      
      -- insert uplink bond                  
      IF in_uplink_port_type = 1 THEN 
          INSERT INTO kernel_cfg_report_bond(kernel_cfg_report_id,vna_id,uplink_port_type,bond_mode) 
             values(result.id, vnaid,in_uplink_port_type,in_uplink_port_mode);
      END IF;
      
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;

                 

-- check modify        
CREATE OR REPLACE FUNCTION pf_net_check_modify_kernelreport(in_vna_uuid character varying, in_uuid character varying, in_switch_name character varying, 
                 in_ip character varying,in_mask character varying,in_is_online integer,in_kernel_port_name character varying,
                 in_uplink_port_name character varying, in_uplink_port_type integer, in_uplink_port_mode integer,in_is_dhcp integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  vnaid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE   integer := 13;
BEGIN
  result := 0;
  -- 入参检查 后面做
  
  -- vna uuid 
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;  
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
  
  
  -- 是否存在
  SELECT count(*) INTO is_exist FROM kernel_cfg_report WHERE vna_id = vnaid AND uuid = in_uuid; 
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST; 
      return result;
  END IF;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
                         


CREATE OR REPLACE FUNCTION pf_net_modify_kernelreport(in_vna_uuid character varying,  in_uuid character varying,in_switch_name character varying, 
                 in_ip character varying,in_mask character varying,in_is_online integer,in_kernel_port_name character varying,
                 in_uplink_port_name character varying, in_uplink_port_type integer, in_uplink_port_mode integer,in_is_dhcp integer) 
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  vnaid numeric(19,0);
  kernelreportid numeric(19,0);
  olduplinkporttype integer;
  oldportname character varying;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG  integer := 5;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  
  -- 检查能否add
  result.ret := pf_net_check_modify_kernelreport(in_vna_uuid,in_uuid,in_switch_name,in_ip,in_mask,in_is_online,in_kernel_port_name,
        in_uplink_port_name,in_uplink_port_type,in_uplink_port_mode,in_is_dhcp);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
   -- vna uuid 
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;  
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
  
  -- old uplink port type
  SELECT count(*) INTO is_exist FROM kernel_cfg_report WHERE vna_id = vnaid AND uuid = in_uuid; 
  SELECT uplink_port_type INTO olduplinkporttype FROM kernel_cfg_report WHERE vna_id = vnaid AND uuid = in_uuid; 
  SELECT uplink_port_name INTO oldportname FROM kernel_cfg_report WHERE vna_id = vnaid AND uuid = in_uuid; 
  SELECT id INTO kernelreportid FROM kernel_cfg_report WHERE vna_id = vnaid AND uuid = in_uuid;   
  IF is_exist <= 0 THEN 
      result.ret := RTN_ITEM_NO_EXIST; 
      return result;
  END IF;
  
  -- modify item
  result.id  := kernelreportid;
  result.uuid := in_uuid;
  BEGIN   
    
    -- uplink bond ,需要将bond表删除
    IF olduplinkporttype = 1 THEN 
        IF olduplinkporttype = in_uplink_port_type THEN 
            IF oldportname = in_uplink_port_name THEN                 
                result.ret := 0;
            ELSE 
                DELETE FROM kernel_cfg_report_bond WHERE kernel_cfg_report_id =  kernelreportid;
            END IF;
        ELSE
            DELETE FROM kernel_cfg_report_bond WHERE kernel_cfg_report_id =  kernelreportid;
        END IF;
    END IF;
    
    UPDATE kernel_cfg_report SET switch_name = in_switch_name, ip = in_ip, mask = in_mask, is_online = in_is_online ,kernel_port_name = in_kernel_port_name,
           uplink_port_name = in_uplink_port_name, uplink_port_type = in_uplink_port_type, is_dhcp = in_is_dhcp
        WHERE vna_id = vnaid AND uuid = in_uuid ;    
        
    -- modify uplink port         
    
    -- uplink bond ,不管是type or name变化，都需要将bond表删除
    IF olduplinkporttype = 1 THEN 
        IF olduplinkporttype = in_uplink_port_type THEN 
            IF oldportname = in_uplink_port_name THEN  
                UPDATE kernel_cfg_report_bond SET bond_mode = in_uplink_port_mode WHERE kernel_cfg_report_id = kernelreportid;
            ELSE 
                INSERT INTO kernel_cfg_report_bond(kernel_cfg_report_id,vna_id,uplink_port_type,bond_mode) 
                      values (kernelreportid, vnaid, in_uplink_port_type, in_uplink_port_mode);
            END IF;
        END IF;
    END IF;
    
    IF olduplinkporttype = 0 THEN 
        IF in_uplink_port_type = 1 THEN 
               INSERT INTO kernel_cfg_report_bond(kernel_cfg_report_id,vna_id,uplink_port_type,bond_mode) 
                values (kernelreportid, vnaid, in_uplink_port_type, in_uplink_port_mode);
        END IF;
    END IF;
    
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;

                                 
-- check switch_cfg_report del
CREATE OR REPLACE FUNCTION pf_net_check_del_kernelreport(in_uuid character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  vnaid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
BEGIN
  result := 0;
  -- 入参检查 后面做
          
  --查询是否存在
  SELECT count(*) INTO is_exist FROM kernel_cfg_report WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST; 
      return result;
  END IF;

  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



CREATE OR REPLACE FUNCTION pf_net_del_kernelreport(in_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  vnaid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  result.ret := pf_net_check_del_kernelreport(in_uuid);
  IF result.ret > 0 THEN 
     return result;
  END IF;
      
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM kernel_cfg_report WHERE uuid = in_uuid;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 查询kernel cfg switch report (通过 vna uuid)


CREATE OR REPLACE FUNCTION pf_net_query_kernelreport_by_vna(in_vna_uuid character varying)
  RETURNS t_kernelreport_query AS
$BODY$
DECLARE
    result t_kernelreport_query;
    is_exist integer; 
    vnaid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    result.ret := 0;
    
    -- vna uuid 
    SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
    SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;  
    IF is_exist <= 0 THEN 
       result.ret := RTN_ITEM_NO_EXIST; 
       return result;
    END IF;  
      
    result.vna_uuid  := in_vna_uuid;
    SELECT uuid INTO result.uuid FROM  kernel_cfg_report WHERE vna_id = vnaid;
    SELECT switch_name INTO result.switch_name FROM  kernel_cfg_report WHERE vna_id = vnaid;
    SELECT ip INTO result.ip FROM  kernel_cfg_report WHERE vna_id = vnaid;
    SELECT mask INTO result.mask FROM  kernel_cfg_report WHERE vna_id = vnaid;
    SELECT is_online INTO result.is_online FROM  kernel_cfg_report WHERE vna_id = vnaid;
    SELECT kernel_port_name INTO result.kernel_port_name FROM  kernel_cfg_report WHERE vna_id = vnaid;
    SELECT uplink_port_name INTO result.uplink_port_name FROM  kernel_cfg_report WHERE vna_id = vnaid;
    SELECT uplink_port_type INTO result.uplink_port_type FROM  kernel_cfg_report WHERE vna_id = vnaid;
    SELECT is_dhcp INTO result.is_dhcp FROM  kernel_cfg_report WHERE vna_id = vnaid;    
	 
    return result;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



-- 查询kernel cfg switch report (通过 uuid)
CREATE OR REPLACE FUNCTION pf_net_query_kernelreport(in_uuid character varying)
  RETURNS t_kernelreport_query AS
$BODY$
DECLARE
    result t_kernelreport_query;
    is_exist integer; 
    vnaid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    result.ret := 0;
    
    -- vna uuid 
    SELECT count(*) INTO is_exist FROM kernel_cfg_report WHERE uuid = in_uuid;
    SELECT vna_id INTO vnaid FROM kernel_cfg_report WHERE uuid = in_uuid;  
    IF is_exist <= 0 THEN 
       result.ret := RTN_ITEM_NO_EXIST; 
       return result;
    END IF;  
    
    -- vna uuid
    SELECT vna_uuid INTO result.vna_uuid FROM vna WHERE id = vnaid;
      
    result.uuid  := in_uuid;
    
    SELECT switch_name INTO result.switch_name FROM  kernel_cfg_report WHERE uuid = in_uuid;
    SELECT ip INTO result.ip FROM  kernel_cfg_report WHERE uuid = in_uuid;
    SELECT mask INTO result.mask FROM  kernel_cfg_report WHERE uuid = in_uuid;
    SELECT is_online INTO result.is_online FROM  kernel_cfg_report WHERE uuid = in_uuid;
    SELECT kernel_port_name INTO result.kernel_port_name FROM  kernel_cfg_report WHERE uuid = in_uuid;
    SELECT uplink_port_name INTO result.uplink_port_name FROM  kernel_cfg_report WHERE uuid = in_uuid;
    SELECT uplink_port_type INTO result.uplink_port_type FROM  kernel_cfg_report WHERE uuid = in_uuid;
    SELECT is_dhcp INTO result.is_dhcp FROM  kernel_cfg_report WHERE vna_id = vnaid;    
	
    return result;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;

-- 41 kernel_report_bond_map




-- 判断 bond map 
CREATE OR REPLACE FUNCTION pf_net_check_add_kernelreport_bondmap(in_uuid character varying, in_phy_port_name character varying )
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  kernelreportid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_SWITCH_UPLINKPORT_INVALID integer := 9;
  RTN_PORT_HAS_JOIN_OTHER_SWITCH integer := 10;
  RTN_PG_MUST_CFG_NETPLANE   integer := 13;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- kernelreport uuid 
  SELECT count(*) INTO is_exist FROM kernel_cfg_report a, kernel_cfg_report_bond b WHERE a.id = b.kernel_cfg_report_id AND a.uuid = in_uuid;
  SELECT id INTO kernelreportid FROM kernel_cfg_report a, kernel_cfg_report_bond b WHERE a.id = b.kernel_cfg_report_id AND a.uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- 查看map表中是否已存在
  SELECT count(*) INTO is_exist FROM kernel_cfg_report_bond_map WHERE kernel_cfg_report_bond_id = kernelreportid AND phy_port_name = in_phy_port_name;
  IF is_exist > 0 THEN 
     result := RTN_ITEM_CONFLICT; 
     return result;
  END IF;  
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 逻辑kernel_report添加 
-- Function: pf_net_add_kernelreport_bondmap(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_kernelreport_bondmap(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_kernelreport_bondmap(in_uuid character varying, in_phy_port_name character varying )
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  kernelreportid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_add_kernelreport_bondmap(in_uuid,in_phy_port_name);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  -- kernelreport uuid 
  SELECT count(*) INTO is_exist FROM kernel_cfg_report a, kernel_cfg_report_bond b WHERE a.id = b.kernel_cfg_report_id AND a.uuid = in_uuid;
  SELECT a.id INTO kernelreportid FROM kernel_cfg_report a, kernel_cfg_report_bond b WHERE a.id = b.kernel_cfg_report_id AND a.uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
  
  -- 不支持重复添加 

  -- insert item 
  result.id := nextval('serial_kernel_cfg_report_bond_map');
  --uplinkportid ：= nextval('serial_kernel_cfg_report_bond');
  --result.uuid := in_uuid;
  BEGIN 
      INSERT INTO kernel_cfg_report_bond_map(id,kernel_cfg_report_bond_id,phy_port_name) 
          values(result.id,kernelreportid,in_phy_port_name );
            
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;

                 

                                 
-- check switch_cfg_report bond map del
CREATE OR REPLACE FUNCTION pf_net_check_del_kernelreport_bond_map(in_uuid character varying, in_phy_port_name character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  kernelreportid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
BEGIN
  result := 0;
  -- 入参检查 后面做

 -- kernelreport uuid 
  SELECT count(*) INTO is_exist FROM kernel_cfg_report a, kernel_cfg_report_bond b WHERE a.id = b.kernel_cfg_report_id AND a.uuid = in_uuid;
  SELECT id INTO kernelreportid FROM kernel_cfg_report a, kernel_cfg_report_bond b WHERE a.id = b.kernel_cfg_report_id AND a.uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
  
  --查询是否存在
  SELECT count(*) INTO is_exist FROM kernel_cfg_report_bond_map WHERE kernel_cfg_report_bond_id = kernelreportid  AND phy_port_name = in_phy_port_name;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST; 
      return result;
  END IF;

  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



CREATE OR REPLACE FUNCTION pf_net_del_kernelreport_bond_map(in_uuid character varying, in_phy_port_name character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  kernelreportid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  result.ret := pf_net_check_del_kernelreport_bond_map(in_uuid,in_phy_port_name);
  IF result.ret > 0 THEN 
     return result;
  END IF;

  -- kernelreport uuid 
  SELECT count(*) INTO is_exist FROM kernel_cfg_report a, kernel_cfg_report_bond b WHERE a.id = b.kernel_cfg_report_id AND a.uuid = in_uuid;
  SELECT id INTO kernelreportid FROM kernel_cfg_report a, kernel_cfg_report_bond b WHERE a.id = b.kernel_cfg_report_id AND a.uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
  
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM kernel_cfg_report_bond_map WHERE kernel_cfg_report_bond_id = kernelreportid  AND phy_port_name = in_phy_port_name;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 查询kernel cfg switch report (通过 vna uuid)

CREATE OR REPLACE FUNCTION pf_net_query_kernelreport_bondmap_by_vna(in_vna_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    vnaid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN      
    
    OPEN ref FOR 
     SELECT  vna_uuid, uuid, switch_name,uplink_port_name,uplink_port_bond_mode,phy_port_name 
     FROM v_net_kernelreport_bondmap 
     WHERE vna_uuid = in_vna_uuid;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 查询kernel cfg switch report (通过 uuid)
CREATE OR REPLACE FUNCTION pf_net_query_kernelreport_bondmap(in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    vnaid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN      
    
    OPEN ref FOR 
     SELECT  vna_uuid, uuid, switch_name,uplink_port_name,uplink_port_bond_mode,phy_port_name 
     FROM v_net_kernelreport_bondmap 
     WHERE uuid = in_uuid;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 43 wildcast_create_vport







--- 查询wildcast create vport summary
--- 输出 
-- Function: pf_net_wildcast_query_create_vport_summary()

-- DROP FUNCTION pf_net_wildcast_query_create_vport_summary();

CREATE OR REPLACE FUNCTION pf_net_wildcast_query_create_vport_summary()
  RETURNS refcursor AS
$BODY$
DECLARE
        ref refcursor;
BEGIN
       OPEN ref FOR
	   SELECT 
	      uuid,port_name,port_type,is_has_kernel_type,kernel_type,kernel_pg_uuid, switch_uuid, is_dhcp
	   FROM v_net_wildcast_create_vport;
       RETURN ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 



-- 查询VNA reg mdoule
-- 输出 vna_uuid,module_uuid,module_name,role,is_online,cluster_name
-- Function: pf_net_wildcast_query_create_vport(character varying)

-- DROP FUNCTION pf_net_wildcast_query_create_vport(character varying);

CREATE OR REPLACE FUNCTION pf_net_wildcast_query_create_vport(in_uuid character varying)
  RETURNS t_wildcast_create_vport_query AS
$BODY$
DECLARE
  result  t_wildcast_create_vport_query;
  is_exist integer;
  RTN_ITEM_NO_EXIST integer :=4;
BEGIN
  result.ret := 0;
  SELECT count(*) INTO is_exist FROM wildcast_cfg_create_virtual_port WHERE uuid = in_uuid;
  IF is_exist > 0 THEN
     result.ret := 0;
  ELSE
     result.ret := RTN_ITEM_NO_EXIST; -- no exist item
     return result;     
  END IF;

   SELECT uuid into result.uuid  FROM v_net_wildcast_create_vport WHERE uuid = in_uuid;
   SELECT port_name into result.port_name  FROM v_net_wildcast_create_vport WHERE uuid = in_uuid;
   SELECT port_type into result.port_type  FROM v_net_wildcast_create_vport WHERE uuid = in_uuid;
   SELECT is_has_kernel_type into result.is_has_kernel_type  FROM v_net_wildcast_create_vport WHERE uuid = in_uuid;
   SELECT kernel_type into result.kernel_type  FROM v_net_wildcast_create_vport WHERE uuid = in_uuid;
   SELECT kernel_pg_uuid into result.kernel_pg_uuid  FROM v_net_wildcast_create_vport WHERE uuid = in_uuid;
   SELECT switch_uuid into result.switch_uuid  FROM v_net_wildcast_create_vport WHERE uuid = in_uuid;
   SELECT is_dhcp into result.is_dhcp  FROM v_net_wildcast_create_vport WHERE uuid = in_uuid;
   
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 检查create virtual port 参数 
CREATE OR REPLACE FUNCTION pf_net_check_add_wildcast_create_vport(in_uuid character varying, in_port_name character varying, in_port_type integer,
           in_is_has_kernel_type integer, in_kernel_type integer,
		   in_kernel_pg_uuid character varying, in_switch_uuid character varying, in_is_dhcp integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE integer := 13;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
  
  -- port type 必须是3/4
  IF in_port_type < 3 THEN
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;
  
  IF in_port_type > 4 THEN
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;
  
  -- port_type = 3, in_is_has_kernel_type 才有效
  IF in_port_type = 3 THEN 
      IF in_is_has_kernel_type = 0 THEN 
          result := RTN_PARAM_INVALID; 
          return result;
	  END IF;
  ELSE
      IF in_is_has_kernel_type = 0 THEN 
          result := 0;           
      ELSE
          result := RTN_PARAM_INVALID; 
          return result;          	   
	  END IF;
  END IF;
  
  IF in_is_has_kernel_type > 0 THEN 
	 SELECT count(*) INTO is_exist FROM port_group a, kernel_pg b WHERE a.id = b.pg_id AND a.uuid = in_kernel_pg_uuid ;
	 IF is_exist <= 0 THEN 
	     result := RTN_PARAM_INVALID; 
         return result; 
	 END IF;
	 
	 SELECT count(*) INTO is_exist FROM switch WHERE uuid = in_switch_uuid ;
	 IF is_exist <= 0 THEN 
	     result := RTN_PARAM_INVALID; 
         return result; 
	 END IF;
	 
  END IF;
  
  --port name 不能冲突
  IF in_port_name <> '' THEN 
      SELECT count(*) into is_conflict FROM  wildcast_cfg_create_virtual_port WHERE port_name = in_port_name AND port_name <> '';  
      IF is_conflict > 0 THEN
         result := RTN_ITEM_CONFLICT; -- conflict 
         return result;
      END IF;
  END IF;
  
  -- kernel type 检查
  IF in_is_has_kernel_type = 1 THEN 
      IF in_kernel_type = 10 OR in_kernel_type = 11 THEN 
          result := 0;    
	  ELSE
          result := RTN_PARAM_INVALID; 
          return result;  
	  END IF;
  END IF;

  /*kernel pg 不能小于 uplink pg */
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

		   
-- wildcast create vport 添加 
-- Function: pf_net_add_wildcast_create_vport(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_wildcast_create_vport(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_wildcast_create_vport(in_uuid character varying, in_port_name character varying, in_port_type integer,
           in_is_has_kernel_type integer, in_kernel_type integer,
		   in_kernel_pg_uuid character varying, in_switch_uuid character varying, in_is_dhcp integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_same_hc integer;
  is_exist integer;
  is_conflict integer;
  kernelpgid numeric(19,0);
  switchid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否已存在相同记录

  -- 参数检查 
  result.ret := pf_net_check_add_wildcast_create_vport(in_uuid, in_port_name, in_port_type, in_is_has_kernel_type, in_kernel_type,
					in_kernel_pg_uuid,in_switch_uuid,in_is_dhcp);
  IF result.ret > 0 THEN 
      return result;
  END IF;
      
  IF in_is_has_kernel_type > 0 THEN 
	 SELECT count(*) INTO is_exist FROM port_group a, kernel_pg b WHERE a.id = b.pg_id AND a.uuid = in_kernel_pg_uuid ;
	 SELECT a.id INTO kernelpgid FROM port_group a, kernel_pg b WHERE a.id = b.pg_id AND a.uuid = in_kernel_pg_uuid ;
	 IF is_exist <= 0 THEN 
	     result := RTN_PARAM_INVALID; 
         return result; 
	 END IF;
	 
	 SELECT count(*) INTO is_exist FROM switch WHERE uuid = in_switch_uuid ;
	 SELECT id INTO switchid FROM switch WHERE uuid = in_switch_uuid ;
	 IF is_exist <= 0 THEN 
	     result := RTN_PARAM_INVALID; 
         return result; 
	 END IF;
  END IF;	
  	  
  -- insert item
  result.id := nextval('serial_wildcast_create_virtual_port');
  result.uuid := in_uuid;
  BEGIN 
    INSERT INTO wildcast_cfg_create_virtual_port(id,uuid, port_name, port_type) values(result.id,in_uuid, in_port_name, in_port_type);
    IF in_is_has_kernel_type > 0 THEN 
       INSERT INTO wildcast_cfg_create_kernel_port(wildcast_create_vport_id,port_type,kernel_type, kernel_pg_id,switch_id, is_dhcp) 
	     values(result.id,in_port_type,in_kernel_type,kernelpgid,switchid, in_is_dhcp);
    END IF;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 



-- check del 		   
CREATE OR REPLACE FUNCTION pf_net_check_del_wildcast_create_vport(in_uuid character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  wcid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE integer := 13;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
  RTN_WILDCAST_TASK_IS_USING integer := 19;
BEGIN
  result := 0;
  -- 入参检查 后面做
  
  SELECT count(*) INTO is_exist FROM wildcast_cfg_create_virtual_port WHERE uuid = in_uuid ;  
  SELECT id INTO wcid FROM wildcast_cfg_create_virtual_port WHERE uuid = in_uuid ; 
  IF is_exist <= 0 THEN
     result := RTN_ITEM_NO_EXIST;-- does not exist item
     return result;
  END IF;

  /* task是否在使用 */
  SELECT count(*) INTO is_exist FROM wildcast_task_create_vport WHERE 
  wildcast_cfg_create_vport_id = wcid;   
  IF is_exist > 0 THEN
     result := RTN_WILDCAST_TASK_IS_USING;
     return result;
  END IF;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 
		   
-- 删除 
-- Function: pf_net_del_wildcast_create_vporte(character varying)

-- DROP FUNCTION pf_net_del_wildcast_create_vporte(character varying);

CREATE OR REPLACE FUNCTION pf_net_del_wildcast_create_vport(in_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否存在记录
  result.ret := pf_net_check_del_wildcast_create_vport(in_uuid);
  IF result.ret > 0 THEN 
      return result;
  END IF;
    
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM wildcast_cfg_create_virtual_port WHERE uuid = in_uuid ;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 44 wildcast_loopback_port







--- 查询wildcast loopback port 
--- 输出 
-- Function: pf_net_wildcast_loopback_port_summary()

-- DROP FUNCTION pf_net_wildcast_loopback_port_summary();

CREATE OR REPLACE FUNCTION pf_net_wildcast_loopback_port_summary()
  RETURNS refcursor AS
$BODY$
DECLARE
        ref refcursor;
BEGIN
       OPEN ref FOR SELECT uuid,port_name,is_loop FROM wildcast_cfg_loopback_port;
       RETURN ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 



-- 查询loopback port
-- 输出 
-- Function: pf_net_wildcast_query_loopback_port(character varying)

-- DROP FUNCTION pf_net_wildcast_query_loopback_port(character varying);

CREATE OR REPLACE FUNCTION pf_net_wildcast_query_loopback_port(in_uuid character varying)
  RETURNS t_wildcast_loopback_port_query AS
$BODY$
DECLARE
  result  t_wildcast_loopback_port_query;
  is_exist integer;
  RTN_ITEM_NO_EXIST integer :=4;
BEGIN
  result.ret := 0;
  SELECT count(*) INTO is_exist FROM wildcast_cfg_loopback_port WHERE uuid = in_uuid;
  IF is_exist > 0 THEN
     result.ret := 0;
  ELSE
     result.ret := RTN_ITEM_NO_EXIST; -- no exist item
     return result;     
  END IF;

   result.uuid := in_uuid;
   SELECT port_name into result.port_name  FROM wildcast_cfg_loopback_port WHERE uuid = in_uuid;
   SELECT is_loop into result.is_loop  FROM wildcast_cfg_loopback_port WHERE uuid = in_uuid;
        
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 检查create virtual port 参数 
CREATE OR REPLACE FUNCTION pf_net_check_add_wildcast_loopback_port(in_uuid character varying, in_port_name character varying, in_is_loop integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE integer := 13;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
  
      
  --port name 不能冲突
  SELECT count(*) into is_conflict FROM  wildcast_cfg_loopback_port WHERE port_name = in_port_name;  
  IF is_conflict > 0 THEN
     result := RTN_ITEM_CONFLICT; -- conflict 
     return result;
  END IF;
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 
		   
-- wildcast loopback port 添加 
-- Function: pf_net_add_wildcast_loopback_port(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_wildcast_loopback_port(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_wildcast_loopback_port(in_uuid character varying, in_port_name character varying, in_is_loop integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_same_hc integer;
  is_conflict integer;
  vnaid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否已存在相同记录

  -- 参数检查 
  result.ret := pf_net_check_add_wildcast_loopback_port(in_uuid, in_port_name, in_is_loop );
  IF result.ret > 0 THEN 
      return result;
  END IF;
      
  -- insert item
  result.id := nextval('serial_wildcast_loopback_port');
  result.uuid := in_uuid;
  BEGIN 
    INSERT INTO wildcast_cfg_loopback_port(id,uuid, port_name, is_loop) values(result.id,in_uuid, in_port_name, in_is_loop);

  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 检查create virtual port 参数 
CREATE OR REPLACE FUNCTION pf_net_check_modify_wildcast_loopback_port(in_uuid character varying, in_port_name character varying, in_is_loop integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE integer := 13;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
  
  SELECT count(*) into is_exist FROM  wildcast_cfg_loopback_port WHERE uuid = in_uuid;  
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST;  
     return result;
  END IF;
  
  --port name 不能冲突
  SELECT count(*) into is_exist FROM  wildcast_cfg_loopback_port WHERE uuid = in_uuid AND  port_name = in_port_name;  
  IF is_exist <= 0 THEN 
      SELECT count(*) into is_conflict FROM  wildcast_cfg_loopback_port WHERE port_name = in_port_name;  
      IF is_conflict > 0 THEN
         result := RTN_ITEM_CONFLICT; -- conflict 
         return result;
      END IF;
  END IF;  
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 
CREATE OR REPLACE FUNCTION pf_net_modify_wildcast_loopback_port(in_uuid character varying, in_port_name character varying, in_is_loop integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_same_hc integer;
  is_conflict integer;
  vnaid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否已存在相同记录

  -- 参数检查 
  result.ret := pf_net_check_modify_wildcast_loopback_port(in_uuid, in_port_name, in_is_loop );
  IF result.ret > 0 THEN 
      return result;
  END IF;
      
  -- update item
  --result.id := nextval('serial_wildcast_loopback_port');
  result.uuid := in_uuid;
  BEGIN 
    UPDATE wildcast_cfg_loopback_port SET port_name = in_port_name, is_loop = in_is_loop WHERE uuid = in_uuid;

  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- check loopback port
CREATE OR REPLACE FUNCTION pf_net_check_del_wildcast_loopback_port(in_uuid character varying) 
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  wcid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE integer := 13;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
  RTN_WILDCAST_TASK_IS_USING integer := 19;
BEGIN
  result := 0;
  -- 入参检查 后面做
  
  SELECT count(*) into is_exist FROM  wildcast_cfg_loopback_port WHERE uuid = in_uuid;  
  SELECT id into wcid FROM  wildcast_cfg_loopback_port WHERE uuid = in_uuid;  
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST;  
     return result;
  END IF;

  /* task是否在使用 */
  SELECT count(*) INTO is_exist FROM wildcast_task_loopback WHERE 
  wildcast_cfg_loopback_port_id = wcid;   
  IF is_exist > 0 THEN
     result := RTN_WILDCAST_TASK_IS_USING;
     return result;
  END IF;
      
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 
-- 删除 
-- Function: pf_net_del_wildcast_loopback_port(character varying)

-- DROP FUNCTION pf_net_del_wildcast_loopback_port(character varying);

CREATE OR REPLACE FUNCTION pf_net_del_wildcast_loopback_port(in_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否存在记录
  result.ret := pf_net_check_del_wildcast_loopback_port(in_uuid );
  IF result.ret > 0 THEN 
      return result;
  END IF;
    
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM wildcast_cfg_loopback_port WHERE uuid = in_uuid ;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 45 wildcast_switch



-- 判断通配switch 参数
CREATE OR REPLACE FUNCTION pf_net_check_param_wildcast_switch( in_uuid character varying, in_switch_name character varying, in_switch_type integer,
                 in_state integer, in_evb_mode integer,
                 in_uplinkpg_uuid character varying,in_mtu_max integer,in_nic_max_num integer, 
                 in_port_name character varying,in_port_type integer , in_port_bond_mode integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  uplinkpgid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_SWITCH_UPLINKPORT_INVALID integer := 9;
  RTN_PORT_HAS_JOIN_OTHER_SWITCH integer := 10;
  RTN_PG_MUST_CFG_NETPLANE   integer := 13;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- in_uplinkpg_uuid   
  SELECT count(*) INTO is_exist FROM port_group a, uplink_pg b WHERE a.id  = b.pg_id AND a.uuid = in_uplinkpg_uuid;
  SELECT id INTO uplinkpgid FROM port_group a, uplink_pg b WHERE a.id  = b.pg_id AND a.uuid = in_uplinkpg_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  

/* 钟春山要求，switch portgroup 的 netplane 不做检查   
   pg must cfg netplane
  SELECT count(*) INTO is_exist FROM port_group WHERE id = uplinkpgid AND netplane_id is not null;
  IF is_exist <= 0 THEN 
     result := RTN_PG_MUST_CFG_NETPLANE; 
     return result;
  END IF;  
*/  

  -- check switch_type
  IF in_switch_type < 1 THEN 
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;
  
  IF in_switch_type > 2 THEN 
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;
    
  -- check uplink port type
  IF in_port_type < 0 THEN 
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;
  
  IF in_port_type > 1 THEN 
     IF in_port_type = 4 THEN
         result := 0;
     ELSE
         result := RTN_PARAM_INVALID; 
         return result;
     END IF;
  END IF;
  
  -- check uplink port bond mode 
  IF in_port_type = 1 THEN 
     IF in_port_bond_mode = 1 THEN 
         result := 0; 
     ELSE
         IF in_port_bond_mode = 4 THEN 
             result := 0; 
         ELSE             
             result := RTN_PARAM_INVALID; 
             return result;
         END IF;
     END IF;
  END IF;
              
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


CREATE OR REPLACE FUNCTION pf_net_check_add_wildcast_switch( in_uuid character varying, in_switch_name character varying, in_switch_type integer,
                 in_state integer, in_evb_mode integer,
                 in_uplinkpg_uuid character varying,in_mtu_max integer,in_nic_max_num integer, 
                 in_port_name character varying,in_port_type integer , in_port_bond_mode integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  uplinkpgid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_SWITCH_UPLINKPORT_INVALID integer := 9;
  RTN_PORT_HAS_JOIN_OTHER_SWITCH integer := 10;
  RTN_PG_MUST_CFG_NETPLANE   integer := 13;
BEGIN
  result := 0;
  -- 入参检查 后面做
  result := pf_net_check_param_wildcast_switch(in_uuid,in_switch_name,in_switch_type,in_state,in_evb_mode,in_uplinkpg_uuid,
              in_mtu_max,in_nic_max_num,in_port_name,in_port_type,in_port_bond_mode);
  IF result > 0 THEN 
     return result;
  END IF;          
              
              
  -- switch name 不能冲突
  SELECT count(*) INTO is_exist FROM wildcast_cfg_switch WHERE switch_name = in_switch_name;
  IF is_exist > 0 THEN 
      result := RTN_ITEM_CONFLICT;
      return result;
  END IF;
  

  -- port 
  IF in_port_type = 0 THEN 
      SELECT count(*) INTO is_exist FROM wildcast_cfg_switch_bond_map WHERE phy_port_name = in_port_name;
      IF is_exist > 0 THEN 
          result := RTN_ITEM_CONFLICT;
          return result;
      END IF;
  END IF;
  
  SELECT count(*) INTO is_exist FROM wildcast_cfg_switch WHERE switch_type = in_switch_type AND port_name = in_port_name AND in_port_name <> '';
  IF is_exist > 0 THEN 
      result := RTN_ITEM_CONFLICT;
      return result;
  END IF;
                
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 
                 
-- 逻辑kernel_report添加 
-- Function: pf_net_add_kernelreport(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_kernelreport(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_wildcast_switch( in_uuid character varying, in_switch_name character varying, in_switch_type integer,
                 in_state integer, in_evb_mode integer,
                 in_uplinkpg_uuid character varying,in_mtu_max integer,in_nic_max_num integer, 
                 in_port_name character varying,in_port_type integer, in_port_bond_mode integer) 
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  vnaid numeric(19,0);
  uplinkpgid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_add_wildcast_switch(in_uuid,in_switch_name,in_switch_type,in_state,in_evb_mode,in_uplinkpg_uuid,in_mtu_max,in_nic_max_num,
                  in_port_name,in_port_type,in_port_bond_mode);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  SELECT count(*) INTO is_exist FROM port_group a, uplink_pg b WHERE a.id  = b.pg_id AND a.uuid = in_uplinkpg_uuid;
  SELECT id INTO uplinkpgid FROM port_group a, uplink_pg b WHERE a.id  = b.pg_id AND a.uuid = in_uplinkpg_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
    
  -- 不支持重复添加 
  -- insert item 
  result.id := nextval('serial_wildcast_switch');
  --uplinkportid ：= nextval('serial_kernel_cfg_report_bond');
  result.uuid := in_uuid;
  BEGIN 
      INSERT INTO wildcast_cfg_switch(id,uuid,switch_name,switch_type, upg_id,mtu_max,nic_max_num, port_name, port_type,
             state, evb_mode) 
          values(result.id,in_uuid,in_switch_name,in_switch_type,uplinkpgid,in_mtu_max,in_nic_max_num,in_port_name,in_port_type,
             in_state, in_evb_mode );
      
      -- insert uplink bond                  
      IF in_port_type = 1 THEN 
          INSERT INTO wildcast_cfg_switch_bond(wildcast_cfg_switch_id,port_type,bond_mode) 
             values(result.id, in_port_type,in_port_bond_mode);
      END IF;
      
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

                 

-- check modify        
CREATE OR REPLACE FUNCTION pf_net_check_modify_wildcast_switch( in_uuid character varying, in_switch_name character varying, in_switch_type integer,
                 in_state integer, in_evb_mode integer,
                 in_uplinkpg_uuid character varying,in_mtu_max integer,in_nic_max_num integer, 
                 in_port_name character varying,in_port_type integer, in_port_bond_mode integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  vnaid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE   integer := 13;
BEGIN
  result := 0;
  -- 入参检查 后面做
  
  result := pf_net_check_param_wildcast_switch(in_uuid,in_switch_name,in_switch_type,in_state,in_evb_mode,in_uplinkpg_uuid,
              in_mtu_max,in_nic_max_num,in_port_name,in_port_type,in_port_bond_mode);
  IF result > 0 THEN 
     return result;
  END IF;          
  
  -- 是否存在
  SELECT count(*) INTO is_exist FROM wildcast_cfg_switch WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
  END IF;    
              
  -- switch name 不能冲突
  SELECT count(*) INTO is_exist FROM wildcast_cfg_switch WHERE uuid = in_uuid AND switch_name = in_switch_name;
  IF is_exist <= 0 THEN 
      SELECT count(*) INTO is_exist FROM wildcast_cfg_switch WHERE switch_name = in_switch_name;
      IF is_exist > 0 THEN 
          result := RTN_ITEM_CONFLICT;
          return result;
      END IF;
  END IF;
  
  -- port 
  IF in_port_type = 0 THEN 
      SELECT count(*) INTO is_exist FROM wildcast_cfg_switch_bond_map WHERE phy_port_name = in_port_name;
      IF is_exist > 0 THEN 
          result := RTN_ITEM_CONFLICT;
          return result;
      END IF;
  END IF;

  /*bond口时，所有名称都一样*/
  IF in_port_type = 0 AND in_port_name <> '' THEN 
      SELECT count(*) INTO is_exist FROM wildcast_cfg_switch WHERE uuid = in_uuid AND port_name = in_port_name;
      IF is_exist <= 0 THEN 
          SELECT count(*) INTO is_exist FROM wildcast_cfg_switch WHERE switch_type = in_switch_type AND port_name = in_port_name;
          IF is_exist > 0 THEN 
              result := RTN_ITEM_CONFLICT;
              return result;
          END IF;
      END IF;
  END IF;
   
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 
                         


CREATE OR REPLACE FUNCTION pf_net_modify_wildcast_switch( in_uuid character varying, in_switch_name character varying, in_switch_type integer,
                 in_state integer, in_evb_mode integer,
                 in_uplinkpg_uuid character varying,in_mtu_max integer,in_nic_max_num integer, 
                 in_port_name character varying,in_port_type integer, in_port_bond_mode integer) 
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  uplinkpgid numeric(19,0);
  wcswitchid numeric(19,0);
  oldporttype integer;
  oldportname character varying;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG  integer := 5;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  
  result.ret := pf_net_check_modify_wildcast_switch(in_uuid,in_switch_name,in_switch_type,in_state,in_evb_mode,in_uplinkpg_uuid,in_mtu_max,in_nic_max_num,
                  in_port_name,in_port_type,in_port_bond_mode);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  SELECT count(*) INTO is_exist FROM port_group a, uplink_pg b WHERE a.id  = b.pg_id AND a.uuid = in_uplinkpg_uuid;
  SELECT id INTO uplinkpgid FROM port_group a, uplink_pg b WHERE a.id  = b.pg_id AND a.uuid = in_uplinkpg_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
    
  -- old uplink port type
  SELECT count(*) INTO is_exist FROM wildcast_cfg_switch WHERE uuid = in_uuid; 
  SELECT id INTO wcswitchid FROM wildcast_cfg_switch WHERE uuid = in_uuid; 
  SELECT port_type INTO oldporttype FROM wildcast_cfg_switch WHERE uuid = in_uuid; 
  SELECT port_name INTO oldportname FROM wildcast_cfg_switch WHERE uuid = in_uuid; 
  IF is_exist <= 0 THEN 
      result.ret := RTN_ITEM_NO_EXIST; 
      return result;
  END IF;
  
  -- modify item
  result.id  := wcswitchid;
  result.uuid := in_uuid;
  BEGIN   
    
    -- uplink bond ,需要将bond表删除
    IF oldporttype = 1 THEN 
        IF oldporttype = in_port_type THEN 
            IF oldportname = in_port_name THEN                 
                result.ret := 0;
            ELSE 
                DELETE FROM wildcast_cfg_switch_bond WHERE wildcast_cfg_switch_id =  wcswitchid;
            END IF;
        ELSE
            DELETE FROM wildcast_cfg_switch_bond WHERE wildcast_cfg_switch_id =  wcswitchid;
        END IF;
    END IF;
    
    UPDATE wildcast_cfg_switch SET switch_name = in_switch_name, switch_type = in_switch_type,upg_id = uplinkpgid, mtu_max = in_mtu_max,
           nic_max_num = in_nic_max_num,
           port_name = in_port_name, port_type = in_port_type,
           state = in_state, evb_mode = in_evb_mode 
        WHERE uuid = in_uuid ;    
        
    -- modify uplink port         
    
    -- uplink bond ,不管是type or name变化，都需要将bond表删除
    IF oldporttype = 1 THEN 
        IF oldporttype = in_port_type THEN 
            IF oldportname = in_port_name THEN        
                UPDATE wildcast_cfg_switch_bond SET bond_mode = in_port_bond_mode WHERE wildcast_cfg_switch_id = wcswitchid;
            ELSE 
                INSERT INTO wildcast_cfg_switch_bond(wildcast_cfg_switch_id,port_type,bond_mode) 
                      values (wcswitchid, in_port_type, in_port_bond_mode);
            END IF;
        END IF;
    END IF;
    
    IF oldporttype = 0 THEN 
        IF in_port_type = 1 THEN 
               INSERT INTO wildcast_cfg_switch_bond(wildcast_cfg_switch_id,port_type,bond_mode) 
                values (wcswitchid, in_port_type, in_port_bond_mode);
        END IF;
    END IF;
    
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

                                 
-- check switch_cfg_report del
CREATE OR REPLACE FUNCTION pf_net_check_del_wildcast_switch(in_uuid character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  vnaid numeric(19,0);
  wcid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
  RTN_WILDCAST_TASK_IS_USING integer := 19;
BEGIN
  result := 0;
  -- 入参检查 后面做
          
  --查询是否存在
  SELECT count(*) INTO is_exist FROM wildcast_cfg_switch WHERE uuid = in_uuid;
  SELECT id INTO wcid FROM wildcast_cfg_switch WHERE uuid = in_uuid;  
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST; 
      return result;
  END IF;

  /* task是否在使用 */
  SELECT count(*) INTO is_exist FROM wildcast_task_switch WHERE 
  wildcast_cfg_switch_id = wcid;   
  IF is_exist > 0 THEN
     result := RTN_WILDCAST_TASK_IS_USING;
     return result;
  END IF;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


CREATE OR REPLACE FUNCTION pf_net_del_wildcast_switch(in_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  vnaid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  result.ret := pf_net_check_del_wildcast_switch(in_uuid);
  IF result.ret > 0 THEN 
     return result;
  END IF;
      
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM wildcast_cfg_switch WHERE uuid = in_uuid;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 查询wildcast switch (通过uuid)


CREATE OR REPLACE FUNCTION pf_net_query_wildcast_switch(in_uuid character varying)
  RETURNS t_wildcast_switch_query AS
$BODY$
DECLARE
    result t_wildcast_switch_query;
    is_exist integer; 
    wcswitchid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    result.ret := 0;
    
    -- vna uuid 
    SELECT count(*) INTO is_exist FROM wildcast_cfg_switch WHERE uuid = in_uuid;
    SELECT id INTO wcswitchid FROM wildcast_cfg_switch WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
       result.ret := RTN_ITEM_NO_EXIST; 
       return result;
    END IF;  
      
    result.uuid  := in_uuid;
    SELECT switch_name INTO result.switch_name FROM  v_net_wildcast_switch WHERE uuid = in_uuid;
    SELECT switch_type INTO result.switch_type FROM  v_net_wildcast_switch WHERE uuid = in_uuid;
    SELECT state INTO result.state FROM  v_net_wildcast_switch WHERE uuid = in_uuid;
    SELECT evb_mode INTO result.evb_mode FROM  v_net_wildcast_switch WHERE uuid = in_uuid;
    
    SELECT pg_uuid INTO result.pg_uuid FROM  v_net_wildcast_switch WHERE uuid = in_uuid;
    SELECT mtu_max INTO result.mtu_max FROM  v_net_wildcast_switch WHERE uuid = in_uuid;
    SELECT nic_max_num INTO result.nic_max_num FROM  v_net_wildcast_switch WHERE uuid = in_uuid;
    SELECT port_name INTO result.port_name FROM  v_net_wildcast_switch WHERE uuid = in_uuid;
    SELECT port_type INTO result.port_type FROM  v_net_wildcast_switch WHERE uuid = in_uuid;
    SELECT bond_mode INTO result.bond_mode FROM  v_net_wildcast_switch WHERE uuid = in_uuid;
    
    return result;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 查询wildcast switch
CREATE OR REPLACE FUNCTION pf_net_query_wildcast_switch_summary()
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    vnaid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    
    OPEN ref FOR SELECT uuid,switch_name,switch_type,state, evb_mode, pg_uuid,mtu_max,nic_max_num,port_name,port_type,bond_mode FROM v_net_wildcast_switch ;
        
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 查询wildcast switch by portname 
CREATE OR REPLACE FUNCTION pf_net_query_wildcast_switch_by_portname(in_port_name character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    vnaid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    
    OPEN ref FOR 
    SELECT uuid,switch_name,switch_type,state, evb_mode, pg_uuid,mtu_max,nic_max_num,port_name,port_type,bond_mode 
    FROM v_net_wildcast_switch 
    WHERE port_name = in_port_name;
        
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 



-- 46 wildcast_switch_bond_map



-- 判断 bond map 
CREATE OR REPLACE FUNCTION pf_net_check_add_wildcast_switch_bondmap(in_uuid character varying, in_phy_port_name character varying )
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  wcswitchid numeric(19,0);
  switchtype integer;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_SWITCH_UPLINKPORT_INVALID integer := 9;
  RTN_PORT_HAS_JOIN_OTHER_SWITCH integer := 10;
  RTN_PG_MUST_CFG_NETPLANE   integer := 13;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- switch uuid 
  SELECT count(*) INTO is_exist FROM wildcast_cfg_switch a, wildcast_cfg_switch_bond b WHERE a.id = b.wildcast_cfg_switch_id AND a.uuid = in_uuid;
  SELECT id INTO wcswitchid FROM wildcast_cfg_switch a, wildcast_cfg_switch_bond b WHERE a.id = b.wildcast_cfg_switch_id AND a.uuid = in_uuid;
  SELECT switch_type INTO switchtype FROM wildcast_cfg_switch a, wildcast_cfg_switch_bond b WHERE a.id = b.wildcast_cfg_switch_id AND a.uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  /* 查看map表中是否已存在 */  
  /* 需考虑switch 混用 */
  
  SELECT count(*) INTO is_exist 
  FROM 
     wildcast_cfg_switch_bond_map a, 
     wildcast_cfg_switch b 
  WHERE a.phy_port_name = in_phy_port_name AND 
     a.wildcast_cfg_bond_id = b.id AND 
     b.switch_type = switchtype ;
  IF is_exist > 0 THEN 
     result := RTN_ITEM_CONFLICT; 
     return result;
  END IF;  
  
  -- 查看wildcast_switch是否已存在
  SELECT count(*) INTO is_exist FROM wildcast_cfg_switch WHERE port_name = in_phy_port_name AND port_type = 0;
  IF is_exist > 0 THEN 
     result := RTN_ITEM_CONFLICT; 
     return result;
  END IF;
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 逻辑kernel_report添加 
-- Function: pf_net_add_wildcast_switch_bondmap(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_wildcast_switch_bondmap(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_wildcast_switch_bondmap(in_uuid character varying, in_phy_port_name character varying )
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  wcswitchid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_add_wildcast_switch_bondmap(in_uuid,in_phy_port_name);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  -- switch uuid 
  SELECT count(*) INTO is_exist FROM wildcast_cfg_switch a, wildcast_cfg_switch_bond b WHERE a.id = b.wildcast_cfg_switch_id AND a.uuid = in_uuid;
  SELECT id INTO wcswitchid FROM wildcast_cfg_switch a, wildcast_cfg_switch_bond b WHERE a.id = b.wildcast_cfg_switch_id AND a.uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
  
  -- 不支持重复添加 

  -- insert item 
  result.id := nextval('serial_wildcast_switch_bond_map');
  --result.uuid := in_uuid;
  BEGIN 
      INSERT INTO wildcast_cfg_switch_bond_map(id,wildcast_cfg_bond_id,phy_port_name) 
          values(result.id,wcswitchid,in_phy_port_name );
            
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 
                 

                                 
-- check switch_cfg_report bond map del
CREATE OR REPLACE FUNCTION pf_net_check_del_wildcast_switch_bondmap(in_uuid character varying, in_phy_port_name character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  wcswitchid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
BEGIN
  result := 0;
  -- 入参检查 后面做

 -- switch uuid 
  SELECT count(*) INTO is_exist FROM wildcast_cfg_switch a, wildcast_cfg_switch_bond b WHERE a.id = b.wildcast_cfg_switch_id AND a.uuid = in_uuid;
  SELECT id INTO wcswitchid FROM wildcast_cfg_switch a, wildcast_cfg_switch_bond b WHERE a.id = b.wildcast_cfg_switch_id AND a.uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
  
  --查询是否存在
  SELECT count(*) INTO is_exist FROM wildcast_cfg_switch_bond_map WHERE wildcast_cfg_bond_id = wcswitchid  AND phy_port_name = in_phy_port_name;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST; 
      return result;
  END IF;

  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


CREATE OR REPLACE FUNCTION pf_net_del_wildcast_switch_bondmap(in_uuid character varying, in_phy_port_name character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  wcswitchid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  result.ret := pf_net_check_del_wildcast_switch_bondmap(in_uuid,in_phy_port_name);
  IF result.ret > 0 THEN 
     return result;
  END IF;

 -- switch uuid 
  SELECT count(*) INTO is_exist FROM wildcast_cfg_switch a, wildcast_cfg_switch_bond b WHERE a.id = b.wildcast_cfg_switch_id AND a.uuid = in_uuid;
  SELECT id INTO wcswitchid FROM wildcast_cfg_switch a, wildcast_cfg_switch_bond b WHERE a.id = b.wildcast_cfg_switch_id AND a.uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
  
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM wildcast_cfg_switch_bond_map WHERE wildcast_cfg_bond_id = wcswitchid  AND phy_port_name = in_phy_port_name;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 查询 wildcast switch (通过 vna uuid)

CREATE OR REPLACE FUNCTION pf_net_query_wildcast_switch_bondmap(in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    vnaid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN      
    
    OPEN ref FOR 
     SELECT  switch_uuid, switch_name, switch_type, pg_uuid, uplink_port_name,bond_mode,phy_port_name 
     FROM v_net_wildcast_switch_bondmap 
     WHERE switch_uuid = in_uuid;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

CREATE OR REPLACE FUNCTION pf_net_query_wildcast_switch_bondmap_summary()
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    vnaid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN      
    
    OPEN ref FOR 
     SELECT  switch_uuid, switch_name, switch_type, pg_uuid, uplink_port_name,bond_mode,phy_port_name 
     FROM v_net_wildcast_switch_bondmap;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

/* 通过bond phy port name 来获取 switch bondmap 信息 */
CREATE OR REPLACE FUNCTION 
pf_net_query_wildcast_switch_bondmap_by_phyportname(in_phyportname character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    vnaid numeric(19,0);
    switchuuid character varying(64);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN      

    SELECT  count(*) into  is_exist 
     FROM v_net_wildcast_switch_bondmap 
     WHERE phy_port_name = in_phyportname; 
    IF is_exist <= 0 THEN 
        return ref;
    END IF;
    
    OPEN ref FOR 
     SELECT  a.switch_uuid, a.switch_name, a.switch_type, a.pg_uuid, a.uplink_port_name,a.bond_mode,a.phy_port_name 
     FROM v_net_wildcast_switch_bondmap a,
          v_net_wildcast_switch_bondmap b 
     WHERE a.switch_uuid = b.switch_uuid AND b.phy_port_name = in_phyportname ;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 




-- 47 wildcast_task_switch





CREATE OR REPLACE FUNCTION pf_net_check_add_wildcast_task_switch( in_vna_uuid character varying, in_wc_switch_uuid character varying, in_state integer,
                  in_send_num integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  uplinkpgid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_SWITCH_UPLINKPORT_INVALID integer := 9;
  RTN_PORT_HAS_JOIN_OTHER_SWITCH integer := 10;
  RTN_PG_MUST_CFG_NETPLANE   integer := 13;
BEGIN
  result := 0;
  -- 入参检查 后面做
			  
  -- switch uuid 
  SELECT count(*) INTO is_exist FROM wildcast_cfg_switch WHERE uuid = in_wc_switch_uuid;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
  END IF;
  
  -- vna uuid 
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN  
      result := RTN_ITEM_NO_EXIST;
      return result;
  END IF;
  
  -- switch vna 
  SELECT count(*) INTO is_exist FROM v_net_wildcast_task_switch WHERE switch_uuid = in_wc_switch_uuid AND vna_uuid = in_vna_uuid;
  IF is_exist > 0 THEN  
      result := RTN_ITEM_CONFLICT;
      return result;
  END IF;
                
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 
				 
-- 逻辑 wildcast task add 添加 
CREATE OR REPLACE FUNCTION pf_net_add_wildcast_task_switch( in_uuid character varying, in_vna_uuid character varying, in_wc_switch_uuid character varying, 
        in_state integer,in_send_num integer) 
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  vnaid numeric(19,0);
  switchid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  
  result.ret := pf_net_check_add_wildcast_task_switch(in_vna_uuid,in_wc_switch_uuid,in_state,in_send_num);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  -- switch uuid 
  SELECT count(*) INTO is_exist FROM wildcast_cfg_switch WHERE uuid = in_wc_switch_uuid;
  SELECT id INTO switchid FROM wildcast_cfg_switch WHERE uuid = in_wc_switch_uuid;
  IF is_exist <= 0 THEN 
      result.ret := RTN_ITEM_NO_EXIST;
      return result;
  END IF;
  
  -- vna uuid 
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN  
      result.ret := RTN_ITEM_NO_EXIST;
      return result;
  END IF;
  
  -- 不能冲突 
  SELECT count(*) INTO is_exist FROM wildcast_task_switch WHERE uuid = in_uuid;
  IF is_exist > 0 THEN  
      result.ret := RTN_ITEM_CONFLICT;
      return result;
  END IF;
    
  -- 不支持重复添加 
  -- insert item 
  result.id := nextval('serial_wildcast_task_switch');
  --uplinkportid ：= nextval('serial_kernel_cfg_report_bond');
  result.uuid := in_uuid;
  BEGIN 
      INSERT INTO wildcast_task_switch(id,uuid,vna_id,wildcast_cfg_switch_id,state, send_num) 
          values(result.id,in_uuid,vnaid,switchid,in_state,in_send_num );
            
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

                 

-- check modify        
CREATE OR REPLACE FUNCTION pf_net_check_modify_wildcast_task_switch( in_uuid character varying, in_vna_uuid character varying, in_wc_switch_uuid character varying, 
        in_state integer,in_send_num integer) 
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  vnaid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE   integer := 13;
BEGIN
  result := 0;
  -- 入参检查 后面做
  
  -- uuid 
  -- switch_uuid, vna_uuid 不提供修改功能 
  SELECT count(*) INTO is_exist FROM v_net_wildcast_task_switch WHERE task_uuid = in_uuid;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
  END IF;	
  
  SELECT count(*) INTO is_exist FROM v_net_wildcast_task_switch WHERE task_uuid = in_uuid 
       AND switch_uuid = in_wc_switch_uuid AND vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
      result := RTN_PARAM_INVALID;
      return result;
  END IF;
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 
                         


CREATE OR REPLACE FUNCTION pf_net_modify_wildcast_task_switch( in_uuid character varying, in_vna_uuid character varying, in_wc_switch_uuid character varying, 
        in_state integer,in_send_num integer)  
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  taskid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG  integer := 5;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  
  result.ret := pf_net_check_modify_wildcast_task_switch(in_uuid,in_vna_uuid,in_wc_switch_uuid,in_state,in_send_num);
  IF result.ret > 0 THEN 
     return result;
  END IF;
    
  SELECT id INTO taskid FROM wildcast_task_switch WHERE uuid = in_uuid;
  
  -- modify item
  result.id  := taskid;
  result.uuid := in_uuid;
  BEGIN   
     UPDATE wildcast_task_switch SET  state = in_state , send_num = in_send_num WHERE uuid = in_uuid;
    
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

                                 
-- check switch_cfg_report del
CREATE OR REPLACE FUNCTION pf_net_check_del_wildcast_task_switch(in_uuid character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  taskstate integer; 
  vnaid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
BEGIN
  result := 0;
  -- 入参检查 后面做
          
  --查询是否存在
  SELECT count(*) INTO is_exist FROM wildcast_task_switch WHERE uuid = in_uuid;
  SELECT state INTO taskstate FROM wildcast_task_switch WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST; 
      return result;
  END IF;
  
  IF ((taskstate = 0 ) OR (taskstate = 2 ) ) THEN
      result := 0; 
  ELSE 
      result := RTN_RESOURCE_USING; 
  END IF;

  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


CREATE OR REPLACE FUNCTION pf_net_del_wildcast_task_switch(in_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  vnaid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- result.ret := pf_net_check_del_wildcast_task_switch(in_uuid);
  IF result.ret > 0 THEN 
     return result;
  END IF;
      
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM wildcast_task_switch WHERE uuid = in_uuid;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 查询wildcast switch (通过uuid)


CREATE OR REPLACE FUNCTION pf_net_query_wildcast_task_switch(in_uuid character varying)
  RETURNS t_wildcast_task_switch_query AS
$BODY$
DECLARE
    result t_wildcast_task_switch_query;
    is_exist integer; 
    wcswitchid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    result.ret := 0;
    
    -- vna uuid 
    SELECT count(*) INTO is_exist FROM wildcast_task_switch WHERE uuid = in_uuid;
    SELECT id INTO wcswitchid FROM wildcast_task_switch WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
       result.ret := RTN_ITEM_NO_EXIST; 
       return result;
    END IF;  
      
    SELECT task_uuid INTO result.task_uuid FROM  v_net_wildcast_task_switch WHERE task_uuid = in_uuid;
    SELECT task_state INTO result.task_state FROM  v_net_wildcast_task_switch WHERE task_uuid = in_uuid;
    SELECT task_send_num INTO result.task_send_num FROM  v_net_wildcast_task_switch WHERE task_uuid = in_uuid;
    SELECT switch_uuid INTO result.switch_uuid FROM  v_net_wildcast_task_switch WHERE task_uuid = in_uuid;
    SELECT switch_name INTO result.switch_name FROM  v_net_wildcast_task_switch WHERE task_uuid = in_uuid;
    SELECT switch_type INTO result.switch_type FROM  v_net_wildcast_task_switch WHERE task_uuid = in_uuid;
	SELECT pg_uuid INTO result.pg_uuid FROM  v_net_wildcast_task_switch WHERE task_uuid = in_uuid;
	SELECT mtu_max INTO result.mtu_max FROM  v_net_wildcast_task_switch WHERE task_uuid = in_uuid;
	SELECT nic_max_num INTO result.nic_max_num FROM  v_net_wildcast_task_switch WHERE task_uuid = in_uuid;
	SELECT port_name INTO result.port_name FROM  v_net_wildcast_task_switch WHERE task_uuid = in_uuid;
	SELECT port_type INTO result.port_type FROM  v_net_wildcast_task_switch WHERE task_uuid = in_uuid;
	SELECT bond_mode INTO result.bond_mode FROM  v_net_wildcast_task_switch WHERE task_uuid = in_uuid;
	SELECT vna_uuid INTO result.vna_uuid FROM  v_net_wildcast_task_switch WHERE task_uuid = in_uuid;
	SELECT vna_is_online INTO result.vna_is_online FROM  v_net_wildcast_task_switch WHERE task_uuid = in_uuid;
	
    return result;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 查询wildcast task switch
CREATE OR REPLACE FUNCTION pf_net_query_wildcast_task_switch_summary()
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    vnaid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    
	OPEN ref FOR 
	SELECT 
	    task_uuid,task_state,task_send_num,switch_uuid,switch_name,switch_type,pg_uuid,mtu_max,nic_max_num,port_name,port_type,bond_mode,
		vna_uuid,vna_is_online
	FROM v_net_wildcast_task_switch ;
        
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;

 
-- 查询wildcast task switch by vna
CREATE OR REPLACE FUNCTION pf_net_query_wildcast_task_switch_summary_by_vna(in_vna_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    vnaid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    
	OPEN ref FOR 
	SELECT 
	    task_uuid,task_state,task_send_num,switch_uuid,switch_name,switch_type,pg_uuid,mtu_max,nic_max_num,port_name,port_type,bond_mode,
		vna_uuid,vna_is_online
	FROM v_net_wildcast_task_switch 
	WHERE vna_uuid = in_vna_uuid;
        
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100; 

-- 48 wildcast_task_loopback






CREATE OR REPLACE FUNCTION pf_net_check_add_wildcast_task_loopback( in_vna_uuid character varying, in_wc_loopback_uuid character varying, in_state integer,
                  in_send_num integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  uplinkpgid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_SWITCH_UPLINKPORT_INVALID integer := 9;
  RTN_PORT_HAS_JOIN_OTHER_SWITCH integer := 10;
  RTN_PG_MUST_CFG_NETPLANE   integer := 13;
BEGIN
  result := 0;
  -- 入参检查 后面做
			  
  -- create vport uuid 
  SELECT count(*) INTO is_exist FROM wildcast_cfg_loopback_port WHERE uuid = in_wc_loopback_uuid;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
  END IF;
  
  -- vna uuid 
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN  
      result := RTN_ITEM_NO_EXIST;
      return result;
  END IF;
  
   -- create vport vna 
  SELECT count(*) INTO is_exist FROM v_net_wildcast_task_loopback WHERE loopback_uuid = in_wc_loopback_uuid AND vna_uuid = in_vna_uuid;
  IF is_exist > 0 THEN  
      result := RTN_ITEM_CONFLICT;
      return result;
  END IF;
                
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 
				 
-- 逻辑 wildcast task add 添加 
CREATE OR REPLACE FUNCTION pf_net_add_wildcast_task_loopback( in_uuid character varying, in_vna_uuid character varying, in_wc_loopback_uuid character varying, 
        in_state integer,in_send_num integer) 
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  vnaid numeric(19,0);
  loopbackid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  
  result.ret := pf_net_check_add_wildcast_task_loopback(in_vna_uuid,in_wc_loopback_uuid,in_state,in_send_num);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  -- create vport uuid 
  SELECT count(*) INTO is_exist FROM wildcast_cfg_loopback_port WHERE uuid = in_wc_loopback_uuid;
  SELECT id INTO loopbackid FROM wildcast_cfg_loopback_port WHERE uuid = in_wc_loopback_uuid;
  IF is_exist <= 0 THEN 
      result.ret := RTN_ITEM_NO_EXIST;
      return result;
  END IF;
  
  -- vna uuid 
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN  
      result.ret := RTN_ITEM_NO_EXIST;
      return result;
  END IF;
  
  -- 不能冲突 
  SELECT count(*) INTO is_exist FROM wildcast_task_loopback WHERE uuid = in_uuid;
  IF is_exist > 0 THEN  
      result.ret := RTN_ITEM_CONFLICT;
      return result;
  END IF;
    
  -- 不支持重复添加 
  -- insert item 
  result.id := nextval('serial_wildcast_task_loopback');
  --uplinkportid ：= nextval('serial_kernel_cfg_report_bond');
  result.uuid := in_uuid;
  BEGIN 
      INSERT INTO wildcast_task_loopback(id,uuid,vna_id,wildcast_cfg_loopback_port_id,state, send_num) 
          values(result.id,in_uuid,vnaid,loopbackid,in_state,in_send_num );
            
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

                 

-- check modify        
CREATE OR REPLACE FUNCTION pf_net_check_modify_wildcast_task_loopback( in_uuid character varying, in_vna_uuid character varying, in_wc_loopback_uuid character varying, 
        in_state integer,in_send_num integer) 
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  vnaid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE   integer := 13;
BEGIN
  result := 0;
  -- 入参检查 后面做
  
  -- uuid 
  -- switch_uuid, vna_uuid 不提供修改功能 
  SELECT count(*) INTO is_exist FROM v_net_wildcast_task_loopback WHERE task_uuid = in_uuid;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
  END IF;	
  
  SELECT count(*) INTO is_exist FROM v_net_wildcast_task_loopback WHERE task_uuid = in_uuid 
       AND loopback_uuid = in_wc_loopback_uuid AND vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
      result := RTN_PARAM_INVALID;
      return result;
  END IF;
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 
                         


CREATE OR REPLACE FUNCTION pf_net_modify_wildcast_task_loopback( in_uuid character varying, in_vna_uuid character varying, in_wc_loopback_uuid character varying, 
        in_state integer,in_send_num integer) 
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  taskid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG  integer := 5;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  
  result.ret := pf_net_check_modify_wildcast_task_loopback(in_uuid,in_vna_uuid,in_wc_loopback_uuid,in_state,in_send_num);
  IF result.ret > 0 THEN 
     return result;
  END IF;
    
  SELECT id INTO taskid FROM wildcast_task_loopback WHERE uuid = in_uuid;
  
  -- modify item
  result.id  := taskid;
  result.uuid := in_uuid;
  BEGIN   
     UPDATE wildcast_task_loopback SET  state = in_state , send_num = in_send_num WHERE uuid = in_uuid;
    
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

                                 
-- check switch_cfg_report del
CREATE OR REPLACE FUNCTION pf_net_check_del_wildcast_task_loopback(in_uuid character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  taskstate integer;
  vnaid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
BEGIN
  result := 0;
  -- 入参检查 后面做
          
  --查询是否存在
  SELECT count(*) INTO is_exist FROM wildcast_task_loopback WHERE uuid = in_uuid;
  SELECT state INTO taskstate FROM wildcast_task_loopback WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST; 
      return result;
  END IF;
  
  IF ((taskstate = 0 ) OR (taskstate = 2 ) ) THEN
      result := 0; 
  ELSE 
      result := RTN_RESOURCE_USING; 
  END IF;

  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


CREATE OR REPLACE FUNCTION pf_net_del_wildcast_task_loopback(in_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  vnaid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  --result.ret := pf_net_check_del_wildcast_task_loopback(in_uuid);
  IF result.ret > 0 THEN 
     return result;
  END IF;
      
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM wildcast_task_loopback WHERE uuid = in_uuid;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 查询wildcast switch (通过uuid)


CREATE OR REPLACE FUNCTION pf_net_query_wildcast_task_loopback(in_uuid character varying)
  RETURNS t_wildcast_task_loopback_query AS
$BODY$
DECLARE
    result t_wildcast_task_loopback_query;
    is_exist integer; 
    taskid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    result.ret := 0;
    
    -- vna uuid 
    SELECT count(*) INTO is_exist FROM wildcast_task_loopback WHERE uuid = in_uuid;
    SELECT id INTO taskid FROM wildcast_task_loopback WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
       result.ret := RTN_ITEM_NO_EXIST; 
       return result;
    END IF;  
      
    SELECT task_uuid INTO result.task_uuid FROM  v_net_wildcast_task_loopback WHERE task_uuid = in_uuid;
    SELECT task_state INTO result.task_state FROM  v_net_wildcast_task_loopback WHERE task_uuid = in_uuid;
    SELECT task_send_num INTO result.task_send_num FROM  v_net_wildcast_task_loopback WHERE task_uuid = in_uuid;
    SELECT loopback_uuid INTO result.loopback_uuid FROM  v_net_wildcast_task_loopback WHERE task_uuid = in_uuid;
    SELECT port_name INTO result.port_name FROM  v_net_wildcast_task_loopback WHERE task_uuid = in_uuid;
    SELECT is_loop INTO result.is_loop FROM  v_net_wildcast_task_loopback WHERE task_uuid = in_uuid;
	SELECT vna_uuid INTO result.vna_uuid FROM  v_net_wildcast_task_loopback WHERE task_uuid = in_uuid;
	SELECT vna_is_online INTO result.vna_is_online FROM  v_net_wildcast_task_loopback WHERE task_uuid = in_uuid;
	
    return result;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 查询wildcast task switch
CREATE OR REPLACE FUNCTION pf_net_query_wildcast_task_loopback_summary()
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    vnaid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    
	OPEN ref FOR 
	SELECT 
	    task_uuid,task_state,task_send_num,loopback_uuid,port_name,is_loop,
		vna_uuid,vna_is_online
	FROM v_net_wildcast_task_loopback ;
        
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 查询wildcast task switch by vna 
CREATE OR REPLACE FUNCTION pf_net_query_wildcast_task_loopback_summary_by_vna(in_vna_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    vnaid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    
	OPEN ref FOR 
	SELECT 
	    task_uuid,task_state,task_send_num,loopback_uuid,port_name,is_loop,
		vna_uuid,vna_is_online
	FROM v_net_wildcast_task_loopback 
	WHERE 
	    vna_uuid = in_vna_uuid;
        
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100; 

-- 49 wildcast_task_create_vport





CREATE OR REPLACE FUNCTION pf_net_check_add_wildcast_task_create_vport( in_vna_uuid character varying, 
                  in_wc_create_vport_uuid character varying, in_state integer,
                  in_send_num integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  uplinkpgid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_SWITCH_UPLINKPORT_INVALID integer := 9;
  RTN_PORT_HAS_JOIN_OTHER_SWITCH integer := 10;
  RTN_PG_MUST_CFG_NETPLANE   integer := 13;
BEGIN
  result := 0;
  -- 入参检查 后面做
              
  -- create vport uuid 
  SELECT count(*) INTO is_exist FROM wildcast_cfg_create_virtual_port WHERE uuid = in_wc_create_vport_uuid;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
  END IF;
  
  -- vna uuid 
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN  
      result := RTN_ITEM_NO_EXIST;
      return result;
  END IF;
  
   -- create vport vna 
  SELECT count(*) INTO is_exist FROM v_net_wildcast_task_create_vport WHERE create_vport_uuid = in_wc_create_vport_uuid AND vna_uuid = in_vna_uuid;
  IF is_exist > 0 THEN  
      result := RTN_ITEM_CONFLICT;
      return result;
  END IF;
                
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 
                 
-- 逻辑 wildcast task add 添加 
CREATE OR REPLACE FUNCTION pf_net_add_wildcast_task_create_vport( in_uuid character varying, in_vna_uuid character varying, in_wc_create_vport_uuid character varying, 
        in_state integer,in_send_num integer) 
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  vnaid numeric(19,0);
  createvportid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  
  result.ret := pf_net_check_add_wildcast_task_create_vport(in_vna_uuid,in_wc_create_vport_uuid,in_state,in_send_num);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  -- create vport uuid 
  SELECT count(*) INTO is_exist FROM wildcast_cfg_create_virtual_port WHERE uuid = in_wc_create_vport_uuid;
  SELECT id INTO createvportid FROM wildcast_cfg_create_virtual_port WHERE uuid = in_wc_create_vport_uuid;
  IF is_exist <= 0 THEN 
      result.ret := RTN_ITEM_NO_EXIST;
      return result;
  END IF;
  
  -- vna uuid 
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN  
      result.ret := RTN_ITEM_NO_EXIST;
      return result;
  END IF;
  
  -- 不能冲突 
  SELECT count(*) INTO is_exist FROM wildcast_task_create_vport WHERE uuid = in_uuid;
  IF is_exist > 0 THEN  
      result.ret := RTN_ITEM_CONFLICT;
      return result;
  END IF;
    
  -- 不支持重复添加 
  -- insert item 
  result.id := nextval('serial_wildcast_task_create_vport');
  --uplinkportid ：= nextval('serial_kernel_cfg_report_bond');
  result.uuid := in_uuid;
  BEGIN 
      INSERT INTO wildcast_task_create_vport(id,uuid,vna_id,wildcast_cfg_create_vport_id,state, send_num) 
          values(result.id,in_uuid,vnaid,createvportid,in_state,in_send_num );
            
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

                 

-- check modify        
CREATE OR REPLACE FUNCTION pf_net_check_modify_wildcast_task_create_vport( in_uuid character varying, in_vna_uuid character varying, in_wc_create_vport_uuid character varying, 
        in_state integer,in_send_num integer) 
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  vnaid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE   integer := 13;
BEGIN
  result := 0;
  -- 入参检查 后面做
  
  -- uuid 
  -- switch_uuid, vna_uuid 不提供修改功能 
  SELECT count(*) INTO is_exist FROM v_net_wildcast_task_create_vport WHERE task_uuid = in_uuid;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
  END IF;    
  
  SELECT count(*) INTO is_exist FROM v_net_wildcast_task_create_vport WHERE task_uuid = in_uuid 
       AND create_vport_uuid = in_wc_create_vport_uuid AND vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
      result := RTN_PARAM_INVALID;
      return result;
  END IF;
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 
                         


CREATE OR REPLACE FUNCTION pf_net_modify_wildcast_task_create_vport( in_uuid character varying, in_vna_uuid character varying, in_wc_create_vport_uuid character varying, 
        in_state integer,in_send_num integer)  
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  taskid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG  integer := 5;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  
  result.ret := pf_net_check_modify_wildcast_task_create_vport(in_uuid,in_vna_uuid,in_wc_create_vport_uuid,in_state,in_send_num);
  IF result.ret > 0 THEN 
     return result;
  END IF;
    
  SELECT id INTO taskid FROM wildcast_task_create_vport WHERE uuid = in_uuid;
  
  -- modify item
  result.id  := taskid;
  result.uuid := in_uuid;
  BEGIN   
     UPDATE wildcast_task_create_vport SET  state = in_state , send_num = in_send_num WHERE uuid = in_uuid;
    
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

                                 
-- check switch_cfg_report del
CREATE OR REPLACE FUNCTION pf_net_check_del_wildcast_task_create_vport(in_uuid character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  taskstate integer;
  vnaid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
BEGIN
  result := 0;
  -- 入参检查 后面做
          
  --查询是否存在
  SELECT count(*) INTO is_exist FROM wildcast_task_create_vport WHERE uuid = in_uuid;
  SELECT state INTO taskstate FROM wildcast_task_create_vport WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST; 
      return result;
  END IF;
  
  IF ((taskstate = 0 ) OR (taskstate = 2 ) ) THEN
      result := 0; 
  ELSE 
      result := RTN_RESOURCE_USING; 
  END IF;

  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


CREATE OR REPLACE FUNCTION pf_net_del_wildcast_task_create_vport(in_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  vnaid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  --result.ret := pf_net_check_del_wildcast_task_create_vport(in_uuid);
  IF result.ret > 0 THEN 
     return result;
  END IF;
      
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM wildcast_task_create_vport WHERE uuid = in_uuid;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 查询wildcast switch (通过uuid)


CREATE OR REPLACE FUNCTION pf_net_query_wildcast_task_create_vport(in_uuid character varying)
  RETURNS t_wildcast_task_create_vport_query AS
$BODY$
DECLARE
    result t_wildcast_task_create_vport_query;
    is_exist integer; 
    wcswitchid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    result.ret := 0;
    
    -- vna uuid 
    SELECT count(*) INTO is_exist FROM wildcast_task_create_vport WHERE uuid = in_uuid;
    SELECT id INTO wcswitchid FROM wildcast_task_create_vport WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
       result.ret := RTN_ITEM_NO_EXIST; 
       return result;
    END IF;  
      
    SELECT task_uuid INTO result.task_uuid FROM  v_net_wildcast_task_create_vport WHERE task_uuid = in_uuid;
    SELECT task_state INTO result.task_state FROM  v_net_wildcast_task_create_vport WHERE task_uuid = in_uuid;
    SELECT task_send_num INTO result.task_send_num FROM  v_net_wildcast_task_create_vport WHERE task_uuid = in_uuid;
    SELECT create_vport_uuid INTO result.create_vport_uuid FROM  v_net_wildcast_task_create_vport WHERE task_uuid = in_uuid;
    SELECT port_name INTO result.port_name FROM  v_net_wildcast_task_create_vport WHERE task_uuid = in_uuid;
    SELECT port_type INTO result.port_type FROM  v_net_wildcast_task_create_vport WHERE task_uuid = in_uuid;
    SELECT is_has_kernel_type INTO result.is_has_kernel_type FROM  v_net_wildcast_task_create_vport WHERE task_uuid = in_uuid;
    SELECT kernel_type INTO result.kernel_type FROM  v_net_wildcast_task_create_vport WHERE task_uuid = in_uuid;
    SELECT kernel_pg_uuid INTO result.kernel_pg_uuid FROM  v_net_wildcast_task_create_vport WHERE task_uuid = in_uuid;
    SELECT switch_type INTO result.switch_type FROM  v_net_wildcast_task_create_vport WHERE task_uuid = in_uuid;
    SELECT switch_uuid INTO result.switch_uuid FROM  v_net_wildcast_task_create_vport WHERE task_uuid = in_uuid;
    SELECT is_dhcp INTO result.is_dhcp FROM  v_net_wildcast_task_create_vport WHERE task_uuid = in_uuid;
    
    SELECT vna_uuid INTO result.vna_uuid FROM  v_net_wildcast_task_create_vport WHERE task_uuid = in_uuid;
    SELECT vna_is_online INTO result.vna_is_online FROM  v_net_wildcast_task_create_vport WHERE task_uuid = in_uuid;
    
    return result;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 查询wildcast task switch
CREATE OR REPLACE FUNCTION pf_net_query_wildcast_task_create_vport_summary()
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    vnaid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    
    OPEN ref FOR 
    SELECT 
        task_uuid,task_state,task_send_num,create_vport_uuid,port_name,port_type,is_has_kernel_type,kernel_type,
        kernel_pg_uuid,switch_type,switch_uuid,is_dhcp,
        vna_uuid,vna_is_online
    FROM v_net_wildcast_task_create_vport ;
        
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 
-- 查询wildcast task switch
CREATE OR REPLACE FUNCTION pf_net_query_wildcast_task_create_vport_summary_by_vna(in_vna_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    vnaid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    
    OPEN ref FOR 
    SELECT 
        task_uuid,task_state,task_send_num,create_vport_uuid,port_name,port_type,is_has_kernel_type,kernel_type,
        kernel_pg_uuid,switch_type,switch_uuid,is_dhcp,
        vna_uuid,vna_is_online
    FROM v_net_wildcast_task_create_vport 
    WHERE vna_uuid = in_vna_uuid;
        
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

 -- 50 vm_migrate 



CREATE OR REPLACE FUNCTION pf_net_check_add_vm_migrate( in_vm_id numeric(19,0), in_src_clustername character varying, in_src_hostname character varying, 
           in_dst_clustername character varying, in_dst_hostname character varying, in_state integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  uplinkpgid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_SWITCH_UPLINKPORT_INVALID integer := 9;
  RTN_PORT_HAS_JOIN_OTHER_SWITCH integer := 10;
  RTN_PG_MUST_CFG_NETPLANE   integer := 13;
BEGIN
  result := 0;
  -- 入参检查 后面做
			  
  -- vm_id
  SELECT count(*) INTO is_exist FROM vm_migrate WHERE vm_id = in_vm_id;
  IF is_exist > 0 THEN 
      result := RTN_ITEM_CONFLICT;
      return result;
  END IF;
  
  -- vm_id in_src_hostname 是否真的部署在hc上
  SELECT count(*) INTO is_exist FROM vm WHERE vm_id = in_vm_id AND cluster_name = in_src_clustername AND host_name = in_src_hostname;
  IF is_exist <= 0 THEN  
      result := RTN_ITEM_NO_EXIST;
      return result;
  END IF;
                  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
   

-- vm migrate add 添加 
CREATE OR REPLACE FUNCTION pf_net_add_vm_migrate( in_vm_id numeric(19,0), in_src_clustername character varying, in_src_hostname character varying, 
           in_dst_clustername character varying, in_dst_hostname character varying, in_state integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  vnaid numeric(19,0);
  loopbackid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  
  result.ret := pf_net_check_add_vm_migrate(in_vm_id,in_src_clustername,in_src_hostname,in_dst_clustername,in_dst_hostname,in_state);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  -- 不支持重复添加 
  -- insert item 
  --result.id := nextval('serial_wildcast_task_loopback');
  --uplinkportid ：= nextval('serial_kernel_cfg_report_bond');
  result.id = in_vm_id;
  --result.uuid := in_uuid;
  BEGIN 
      INSERT INTO vm_migrate(vm_id,src_clustername,src_hostname,dst_clustername,dst_hostname, state) 
          values(in_vm_id,in_src_clustername,in_src_hostname,in_dst_clustername,in_dst_hostname,in_state );
            
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

		   
CREATE OR REPLACE FUNCTION pf_net_check_modify_vm_migrate( in_vm_id numeric(19,0), in_src_clustername character varying, in_src_hostname character varying, 
           in_dst_clustername character varying, in_dst_hostname character varying, in_state integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  uplinkpgid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_SWITCH_UPLINKPORT_INVALID integer := 9;
  RTN_PORT_HAS_JOIN_OTHER_SWITCH integer := 10;
  RTN_PG_MUST_CFG_NETPLANE   integer := 13;
BEGIN
  result := 0;
  -- 入参检查 后面做
			  
  -- vm_id
  SELECT count(*) INTO is_exist FROM vm_migrate WHERE vm_id = in_vm_id;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
  END IF;
                    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
   

-- vm migrate modify 添加 
CREATE OR REPLACE FUNCTION pf_net_modify_vm_migrate( in_vm_id numeric(19,0), in_src_clustername character varying, in_src_hostname character varying, 
           in_dst_clustername character varying, in_dst_hostname character varying, in_state integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  vnaid numeric(19,0);
  loopbackid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  
  result.ret := pf_net_check_modify_vm_migrate(in_vm_id,in_src_clustername,in_src_hostname,in_dst_clustername,in_dst_hostname,in_state);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  -- 不支持重复添加 
  -- insert item 
  --result.id := nextval('serial_wildcast_task_loopback');
  --uplinkportid ：= nextval('serial_kernel_cfg_report_bond');
  result.id = in_vm_id;
  --result.uuid := in_uuid;
  BEGIN 
      UPDATE vm_migrate SET src_clustername = in_src_clustername, src_hostname = in_src_hostname,
	      dst_clustername = in_dst_clustername, dst_hostname = in_dst_hostname, state = in_state
	  WHERE vm_id = in_vm_id;
            
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

CREATE OR REPLACE FUNCTION pf_net_del_vm_migrate( in_vm_id numeric(19,0))
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_NO_EXIST integer := 4;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  
  -- vm_id
  SELECT count(*) INTO is_exist FROM vm_migrate WHERE vm_id = in_vm_id;
  IF is_exist <= 0 THEN 
      result.ret  := RTN_ITEM_NO_EXIST;
      return result;
  END IF;
  
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM vm_migrate WHERE vm_id = in_vm_id;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
  
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 查询接口 
CREATE OR REPLACE FUNCTION pf_net_query_vm_migrate(in_vm_id numeric(19,0))
  RETURNS t_vm_migrate_query AS
$BODY$
DECLARE
    result t_vm_migrate_query;
    is_exist integer; 
    taskid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    result.ret := 0;
    
    -- vna uuid 
    SELECT count(*) INTO is_exist FROM vm_migrate WHERE vm_id = in_vm_id;
    IF is_exist <= 0 THEN 
       result.ret := RTN_ITEM_NO_EXIST; 
       return result;
    END IF;  
      	
    SELECT vm_id INTO result.vm_id FROM vm_migrate WHERE vm_id = in_vm_id;
	SELECT src_clustername INTO result.src_clustername FROM vm_migrate WHERE vm_id = in_vm_id;
	SELECT src_hostname INTO result.src_hostname FROM vm_migrate WHERE vm_id = in_vm_id;
	SELECT dst_clustername INTO result.dst_clustername FROM vm_migrate WHERE vm_id = in_vm_id;
	SELECT dst_hostname INTO result.dst_hostname FROM vm_migrate WHERE vm_id = in_vm_id;
	SELECT state INTO result.state FROM vm_migrate WHERE vm_id = in_vm_id;
		
    return result;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

CREATE OR REPLACE FUNCTION pf_net_query_vm_migrate_summary()
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    vnaid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    
	OPEN ref FOR 
	SELECT 
	    vm_id,src_clustername,src_hostname,dst_clustername,dst_hostname,state
	FROM vm_migrate ;
        
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 
-- 51 vm



CREATE OR REPLACE FUNCTION pf_net_check_add_vm( in_vm_id numeric(19,0), in_project_id numeric(19,0), in_is_enable_watchdog integer, in_watchdog_timeout integer, in_is_has_cluster integer,
           in_cluster_name character varying, in_is_has_host integer, in_host_name character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  uplinkpgid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_SWITCH_UPLINKPORT_INVALID integer := 9;
  RTN_PORT_HAS_JOIN_OTHER_SWITCH integer := 10;
  RTN_PG_MUST_CFG_NETPLANE   integer := 13;
BEGIN
  result := 0;
  -- 入参检查 后面做
			  
  -- vm_id
  SELECT count(*) INTO is_exist FROM vm WHERE vm_id = in_vm_id;
  IF is_exist > 0 THEN 
      result := RTN_ITEM_CONFLICT;
      return result;
  END IF;
                    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- vm add 添加 
CREATE OR REPLACE FUNCTION pf_net_add_vm( in_vm_id numeric(19,0), in_project_id numeric(19,0),in_is_enable_watchdog integer, in_watchdog_timeout integer, in_is_has_cluster integer,
           in_cluster_name character varying, in_is_has_host integer, in_host_name character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  vnaid numeric(19,0);
  loopbackid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  
  result.ret := pf_net_check_add_vm(in_vm_id,in_project_id,in_is_enable_watchdog,in_watchdog_timeout,
      in_is_has_cluster,in_cluster_name,in_is_has_host,in_host_name);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  -- 不支持重复添加 
  -- insert item 
  result.id = in_vm_id;
  --result.uuid := in_uuid;
  BEGIN 
      IF in_is_has_cluster > 0 THEN 
	      IF in_is_has_host > 0 THEN 
              INSERT INTO vm(vm_id,project_id,is_enable_watchdog,watchdog_timeout,cluster_name,host_name) 
                values(in_vm_id,in_project_id,in_is_enable_watchdog,in_watchdog_timeout,in_cluster_name,in_host_name );
		  ELSE
		      INSERT INTO vm(vm_id,project_id,is_enable_watchdog,watchdog_timeout,cluster_name) 
                values(in_vm_id,in_project_id,in_is_enable_watchdog,in_watchdog_timeout,in_cluster_name );
		  END IF;
	  ELSE
	     IF in_is_has_host > 0 THEN 
              INSERT INTO vm(vm_id,project_id,is_enable_watchdog,watchdog_timeout,host_name) 
                values(in_vm_id,in_project_id,in_is_enable_watchdog,in_watchdog_timeout,in_host_name );
		  ELSE
		      INSERT INTO vm(vm_id,project_id,is_enable_watchdog,watchdog_timeout) 
                values(in_vm_id,in_project_id,in_is_enable_watchdog,in_watchdog_timeout );
		  END IF;
	  END IF;
            
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

		   

CREATE OR REPLACE FUNCTION pf_net_check_modify_vm( in_vm_id numeric(19,0),in_project_id numeric(19,0), in_is_enable_watchdog integer, in_watchdog_timeout integer, in_is_has_cluster integer,
           in_cluster_name character varying, in_is_has_host integer, in_host_name character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  uplinkpgid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_SWITCH_UPLINKPORT_INVALID integer := 9;
  RTN_PORT_HAS_JOIN_OTHER_SWITCH integer := 10;
  RTN_PG_MUST_CFG_NETPLANE   integer := 13;
BEGIN
  result := 0;
  -- 入参检查 后面做
			  
  -- vm_id
  SELECT count(*) INTO is_exist FROM vm WHERE vm_id = in_vm_id;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
  END IF;
  
  -- project id 不能变
  SELECT count(*) INTO is_exist FROM vm WHERE vm_id = in_vm_id AND project_id = in_project_id;
  IF is_exist <= 0 THEN 
      result := RTN_PARAM_INVALID;
      return result;
  END IF;
                    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
    


-- vm add 添加 
CREATE OR REPLACE FUNCTION pf_net_modify_vm( in_vm_id numeric(19,0), in_project_id numeric(19,0),in_is_enable_watchdog integer, in_watchdog_timeout integer, in_is_has_cluster integer,
           in_cluster_name character varying, in_is_has_host integer, in_host_name character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  vnaid numeric(19,0);
  loopbackid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  
  result.ret := pf_net_check_modify_vm(in_vm_id, in_project_id,in_is_enable_watchdog,in_watchdog_timeout,
      in_is_has_cluster,in_cluster_name,in_is_has_host,in_host_name);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  -- 不支持重复添加 
  -- insert item 
  result.id = in_vm_id;
  --result.uuid := in_uuid;
  BEGIN 
      IF in_is_has_cluster > 0 THEN 
	      IF in_is_has_host > 0 THEN 
              UPDATE vm SET is_enable_watchdog = in_is_enable_watchdog, watchdog_timeout = in_watchdog_timeout,
	             cluster_name = in_cluster_name, host_name = in_host_name
		         WHERE vm_id = in_vm_id;
		  ELSE
		      UPDATE vm SET is_enable_watchdog = in_is_enable_watchdog, watchdog_timeout = in_watchdog_timeout,
	             cluster_name = in_cluster_name ,host_name = null 
		         WHERE vm_id = in_vm_id;
		  END IF;
      ELSE
          IF in_is_has_host > 0 THEN 
              UPDATE vm SET is_enable_watchdog = in_is_enable_watchdog, watchdog_timeout = in_watchdog_timeout,
	             cluster_name = null, host_name = in_host_name
		         WHERE vm_id = in_vm_id;
		  ELSE
		      UPDATE vm SET is_enable_watchdog = in_is_enable_watchdog, watchdog_timeout = in_watchdog_timeout,
			     cluster_name = null ,host_name = null 
		         WHERE vm_id = in_vm_id;
		  END IF; 
      END IF;
	  
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
   

CREATE OR REPLACE FUNCTION pf_net_del_vm( in_vm_id numeric(19,0))
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_NO_EXIST integer := 4;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  
  -- vm_id
  SELECT count(*) INTO is_exist FROM vm WHERE vm_id = in_vm_id;
  IF is_exist <= 0 THEN 
      result.ret  := RTN_ITEM_NO_EXIST;
      return result;
  END IF;
  
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM vm WHERE vm_id = in_vm_id;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
  
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 查询接口 
CREATE OR REPLACE FUNCTION pf_net_query_vm(in_vm_id numeric(19,0))
  RETURNS t_vm_query AS
$BODY$
DECLARE
    result t_vm_query;
    is_exist integer; 
    taskid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    result.ret := 0;
    
    -- vna uuid 
    SELECT count(*) INTO is_exist FROM vm WHERE vm_id = in_vm_id;
    IF is_exist <= 0 THEN 
       result.ret := RTN_ITEM_NO_EXIST; 
       return result;
    END IF;  
      	
    SELECT vm_id INTO result.vm_id FROM vm WHERE vm_id = in_vm_id;
	SELECT project_id INTO result.project_id FROM vm WHERE vm_id = in_vm_id;
	SELECT is_enable_watchdog INTO result.is_enable_watchdog FROM vm WHERE vm_id = in_vm_id;
	SELECT watchdog_timeout INTO result.watchdog_timeout FROM vm WHERE vm_id = in_vm_id;
	
	SELECT cluster_name INTO result.cluster_name FROM vm WHERE vm_id = in_vm_id;
	SELECT host_name INTO result.host_name FROM vm WHERE vm_id = in_vm_id;
	
	-- is has clustername 
    SELECT count(*) INTO is_exist FROM vm  WHERE vm_id = in_vm_id AND cluster_name is not null;
    IF is_exist <= 0 THEN 
        result.is_has_cluster := 0;
    ELSE 
        result.is_has_cluster := 1;
    END IF;
	
	-- is has hostname 
    SELECT count(*) INTO is_exist FROM vm  WHERE vm_id = in_vm_id AND host_name is not null;
    IF is_exist <= 0 THEN 
        result.is_has_host := 0;
    ELSE 
        result.is_has_host := 1;
    END IF;
		
    return result;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
   

CREATE OR REPLACE FUNCTION pf_net_query_vm_summary()
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    vnaid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    
	OPEN ref FOR 
	SELECT 
	    vm_id,project_id,is_enable_watchdog,watchdog_timeout,
		CASE 
		   WHEN (cluster_name is null ) THEN
		   (0)
		   ELSE (1)
		END AS is_has_cluster,
		cluster_name,
		CASE 
		   WHEN (host_name is null ) THEN
		   (0)
		   ELSE (1)
		END AS is_has_host,
		host_name
	FROM vm ;
        
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 
		
		 



                                 
                                 
								 



 



 
                                 
								 



                                 								 



  

  


 


                                 
								 






  



  







  

                                 
								 
                                 

								 


                                 
                                 



                                 
								 


  
 
 
 
  
      
  
  

  
  





   
   
  
  
  
  
  
  
  


 



  
 
  
    







