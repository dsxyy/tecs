--*********************************************************************
-- 版权所有 (C)2012, 深圳市中兴通讯股份有限公司。
--
-- 文件名称： cc.sql
-- 文件标识：
-- 内容摘要：
-- 其它说明：
-- 当前版本：
-- 作    者： zte
-- 完成日期： 2012-8-8
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

CREATE TABLE "_schema_log" ( 
	version_number       varchar( 64 ) DEFAULT 0 NOT NULL,
	update_by            varchar( 128 ),
	update_at            varchar( 32 ),
	description          varchar( 512 ),
	CONSTRAINT "_schema_log_pkey" PRIMARY KEY ( version_number )
 );

CREATE TABLE config_cluster ( 
	config_name          varchar( 64 ) NOT NULL,
	config_value         varchar( 64 ),
	CONSTRAINT config_cluster_pkey PRIMARY KEY ( config_name )
 );

CREATE TABLE d_dhcp_ippool ( 
	mac                  varchar( 32 ) NOT NULL,
	oid                  numeric( 19 ),
	ip                   int4,
	mask                 int4,
	gateway              int4,
	is_alloc             int4,
	is_assign            int4,
	CONSTRAINT pk_d_dhcp_ippool_alloc PRIMARY KEY ( mac )
 );

CREATE INDEX idx_d_dhcp_ippool ON d_dhcp_ippool ( ip, mac );

CREATE TABLE dhcpd_config ( 
	oid                  numeric( 19 ) NOT NULL,
	is_start             int4 DEFAULT 0 NOT NULL,
	pxe_file             varchar( 128 ),
	pxe_server_addr      varchar( 64 )
 );

CREATE TABLE dhcpd_map ( 
	id                   numeric( 19 ) NOT NULL,
	type                 int4 NOT NULL,
	owner_id             numeric( 19 ) DEFAULT 0,
	owner_type           int4 DEFAULT 0,
	CONSTRAINT pk_dhcp_map UNIQUE ( id, type ),
	CONSTRAINT pk_dhcpd_map_0 UNIQUE ( id )
 );

ALTER TABLE dhcpd_map ADD CONSTRAINT ck_6 CHECK ( type >= 101 AND type <= 108 );

COMMENT ON TABLE dhcpd_map IS 'declare map table ';

COMMENT ON COLUMN dhcpd_map.id IS 'id';

COMMENT ON COLUMN dhcpd_map.type IS 'declare table typen101 sharenet;102 subnet;103 range;104 group;105 host; 106 allowdeny; 107 option; 108 parameter';

CREATE TABLE dhcpd_option ( 
	id                   numeric( 19 ) NOT NULL,
	type                 int4 DEFAULT 107 NOT NULL,
	broadcast_addr       varchar( 64 ),
	routers              varchar( 64 ),
	subnet_mask          varchar( 64 ),
	domain_name          varchar( 32 ),
	domain_name_servers  varchar( 32 ),
	host_name            varchar( 64 ),
	nis_name             varchar( 32 ),
	ntp_server           varchar( 32 ),
	time_off             int4,
	CONSTRAINT dhcpd_option_pkey PRIMARY KEY ( id )
 );

CREATE TABLE dhcpd_param ( 
	id                   numeric( 19 ) NOT NULL,
	type                 int4 DEFAULT 108 NOT NULL,
	ddns_update          int4,
	dft_lease_time       int4,
	max_lease_time       int4,
	hardware_type        int4 DEFAULT 1,
	hardware_mac         varchar( 18 ),
	server_name          varchar( 32 ),
	get_lease_hostname   int4,
	fixed_ip             varchar( 64 ),
	auth                 int4,
	CONSTRAINT dhcpd_param_pkey PRIMARY KEY ( id )
 );

CREATE TABLE dhcpd_range ( 
	id                   numeric( 19 ) NOT NULL,
	type                 int4 DEFAULT 103 NOT NULL,
	dynamic_bootp        int4,
	ip_start             varchar( 64 ),
	ip_end               varchar( 64 ),
	CONSTRAINT dhcpd_range_pkey PRIMARY KEY ( id )
 );

COMMENT ON COLUMN dhcpd_range.type IS 'range type';

CREATE TABLE dhcpd_sharenet ( 
	id                   numeric( 19 ) NOT NULL,
	type                 int4 DEFAULT 101 NOT NULL,
	vlan_id              int4,
	CONSTRAINT dhcpd_sharenet_pkey PRIMARY KEY ( id )
 );

COMMENT ON COLUMN dhcpd_sharenet.type IS 'sharenet type';

CREATE TABLE dhcpd_subnet ( 
	id                   numeric( 19 ) NOT NULL,
	type                 int4 DEFAULT 102 NOT NULL,
	net                  varchar( 64 ) NOT NULL,
	mask                 varchar( 64 ) NOT NULL,
	CONSTRAINT pk_dhcpd_subnet PRIMARY KEY ( id )
 );

COMMENT ON COLUMN dhcpd_subnet.type IS 'subnet type';

CREATE TABLE host_cpuinfo ( 
	cpu_info_id          numeric( 19 ) NOT NULL,
	processor_num        int4 NOT NULL,
	physical_id_num      int4 NOT NULL,
	cpu_cores            int4 NOT NULL,
	vendor_id            varchar( 128 ) NOT NULL,
	cpu_family           int4 NOT NULL,
	model                int4 NOT NULL,
	model_name           varchar( 1024 ) NOT NULL,
	stepping             int4 NOT NULL,
	cache_size           int4 NOT NULL,
	fpu                  varchar( 128 ) NOT NULL,
	fpu_exception        varchar( 128 ) NOT NULL,
	cpuid_level          int4 NOT NULL,
	wp                   varchar( 128 ) NOT NULL,
	flags                varchar( 1024 ) NOT NULL,
	bogomips             numeric( 10, 2 ) NOT NULL,
	cpu_bench            int4 NOT NULL,
	CONSTRAINT host_cpuinfo_pkey PRIMARY KEY ( cpu_info_id ),
	CONSTRAINT idx_host_cpuinfo UNIQUE ( processor_num, physical_id_num, cpu_cores, vendor_id, cpu_family, model, model_name, stepping, cache_size, fpu, fpu_exception, cpuid_level, wp, flags, bogomips )
 );

CREATE TABLE host_pool ( 
	oid                  numeric( 19 ) NOT NULL,
	name                 varchar( 64 ) NOT NULL,
	description          varchar( 512 ),
	create_time          numeric( 19 ) NOT NULL,
	ip_address           varchar( 64 ),
	run_state            int4 NOT NULL,
	enabled              int4 NOT NULL,
	cpu_info_id          numeric( 19 ) NOT NULL,
	vmm_info             varchar( 256 ) NOT NULL,
	os_info              varchar( 256 ) NOT NULL,
	mem_total            numeric( 19 ) NOT NULL,
	mem_max              numeric( 19 ) NOT NULL,
	disk_total           numeric( 19 ) NOT NULL,
	disk_max             numeric( 19 ) NOT NULL,
	cpu_usage_1m         int4 NOT NULL,
	cpu_usage_5m         int4 NOT NULL,
	cpu_usage_30m        int4 NOT NULL,
	hardware_state       numeric( 19 ) NOT NULL,
	location             varchar( 512 ) NOT NULL,
	is_support_hvm       int4 NOT NULL,
	ssh_key              varchar( 2048 ) NOT NULL,
	shelf_type           varchar( 32 ),
	CONSTRAINT host_pool_pkey PRIMARY KEY ( oid ),
	CONSTRAINT idx_host_pool UNIQUE ( name )
 );

ALTER TABLE host_pool ADD CONSTRAINT ck_0 CHECK ( run_state >= 0 AND run_state <= 2 );

ALTER TABLE host_pool ADD CONSTRAINT ck_1 CHECK ( enabled = 0 OR enabled = 1 );

ALTER TABLE host_pool ADD CONSTRAINT ck_2 CHECK ( is_support_hvm = 0 OR is_support_hvm = 1 );

ALTER TABLE host_pool ADD CONSTRAINT ck_3 CHECK ( cpu_usage_1m >= 0 AND cpu_usage_1m <= 1000 );

ALTER TABLE host_pool ADD CONSTRAINT ck_4 CHECK ( cpu_usage_5m >= 0 AND cpu_usage_5m <= 1000 );

ALTER TABLE host_pool ADD CONSTRAINT ck_5 CHECK ( cpu_usage_30m >= 0 AND cpu_usage_30m <= 1000 );

ALTER TABLE host_pool ADD CONSTRAINT ck_31 CHECK ( is_support_hvm = 0 OR is_support_hvm = 1 );

CREATE TABLE image_cache ( 
	image_id             numeric( 19 ) NOT NULL,
	file_url             varchar( 1024 ),
	size                 numeric( 19 ),
	source               varchar( 256 ),
	target               varchar( 256 ),
	cached_at            varchar( 32 ),
	state                int4,
	CONSTRAINT image_cache_pkey PRIMARY KEY ( image_id ),
	CONSTRAINT image_cache_source_key UNIQUE ( source ),
	CONSTRAINT image_cache_target_key UNIQUE ( target )
 );

CREATE TABLE netplane ( 
	name                 varchar( 32 ) NOT NULL,
	description          varchar( 512 ),
	CONSTRAINT idx_netplane PRIMARY KEY ( name )
 );

CREATE TABLE netplane_map_public ( 
	if_name              varchar( 32 ) NOT NULL,
	netplane             varchar( 32 ) NOT NULL,
	CONSTRAINT idx_netplane_map_public PRIMARY KEY ( if_name )
 );

CREATE INDEX idx_netplane_map_public_0 ON netplane_map_public ( netplane );

CREATE TABLE vlan_pool ( 
	netplane             varchar( 32 ) NOT NULL,
	project_id           numeric( 19 ) NOT NULL,
	user_vlan            int4 NOT NULL,
	is_vlan_trunk        int4 NOT NULL,
	c_vlan               int4 NOT NULL,
	s_vlan               int4 NOT NULL,
	CONSTRAINT pk_vlan_pool UNIQUE ( project_id, netplane, user_vlan ),
	CONSTRAINT pk_vlan_pool_0 UNIQUE ( project_id, netplane, user_vlan, is_vlan_trunk ),
	CONSTRAINT idx_vlan_pool PRIMARY KEY ( netplane, project_id, user_vlan )
 );

ALTER TABLE vlan_pool ADD CONSTRAINT ck_53 CHECK ( user_vlan >= 0 AND user_vlan <= 4095 );

ALTER TABLE vlan_pool ADD CONSTRAINT ck_54 CHECK ( is_vlan_trunk = 0 OR is_vlan_trunk = 1 );

CREATE INDEX idx_vlan_pool_1 ON vlan_pool ( netplane );

CREATE TABLE vlan_range ( 
	oid                  numeric( 19 ) NOT NULL,
	netplane             varchar( 32 ),
	range_begin          int4,
	range_end            int4,
	CONSTRAINT vlan_range_pkey PRIMARY KEY ( oid ),
	CONSTRAINT idx_vlan_range_0 UNIQUE ( netplane, range_begin, range_end )
 );

CREATE INDEX idx_vlan_range ON vlan_range ( netplane );

CREATE TABLE vm_cancel_pool ( 
	oid                  numeric( 19 ) NOT NULL,
	vid                  numeric( 19 ) NOT NULL,
	project_id           numeric( 19 ) NOT NULL,
	hid                  numeric( 19 ) NOT NULL,
	config               text NOT NULL,
	vcpu                 int4 NOT NULL,
	tcu                  int4 NOT NULL,
	memory               numeric( 19 ) NOT NULL,
	state                int4 NOT NULL,
	lcm_state            int4 NOT NULL,
	create_at            varchar( 32 ) NOT NULL,
	repeated             int4 NOT NULL,
	CONSTRAINT vmtask_pool_pkey PRIMARY KEY ( oid ),
	CONSTRAINT pk_vm_task_pool UNIQUE ( hid, vid ),
	CONSTRAINT vmtask_pool_vid_hid_key UNIQUE ( vid, hid ),
	CONSTRAINT pk_vm_cancel UNIQUE ( vid, hid, project_id )
 );

ALTER TABLE vm_cancel_pool ADD CONSTRAINT ck_42 CHECK ( state >= 0 AND state <= 11 );

ALTER TABLE vm_cancel_pool ADD CONSTRAINT ck_43 CHECK ( lcm_state >= 0 AND lcm_state <= 15 );

CREATE INDEX idx_vm_task_pool ON vm_cancel_pool ( hid );

CREATE TABLE vm_disk_cancel ( 
	vid                  numeric( 19 ) NOT NULL,
	hid                  numeric( 19 ) NOT NULL,
	target               varchar( 16 ) NOT NULL,
	size                 numeric( 19 ) NOT NULL,
	position             int4 NOT NULL,
	CONSTRAINT idx_vm_disk_pool_1 PRIMARY KEY ( vid, target, hid )
 );

ALTER TABLE vm_disk_cancel ADD CONSTRAINT ck_49 CHECK ( position >= 0 AND position <= 2 );

CREATE INDEX idx_vm_disk_cancel ON vm_disk_cancel ( hid, vid );

CREATE INDEX idx_vm_disk_pool_2 ON vm_disk_cancel ( vid );

CREATE TABLE vm_image_cancel ( 
	vid                  numeric( 19 ) NOT NULL,
	hid                  numeric( 19 ) NOT NULL,
	target               varchar( 16 ) NOT NULL,
	image_id             numeric( 19 ) NOT NULL,
	occupied_size        numeric( 19 ) NOT NULL,
	position             int4 NOT NULL,
	CONSTRAINT vm_image_cancel_pkey PRIMARY KEY ( vid, hid, target )
 );

ALTER TABLE vm_image_cancel ADD CONSTRAINT ck_52 CHECK ( position >= 0 AND position <= 2 );

CREATE TABLE vm_nic_cancel ( 
	vid                  numeric( 19 ) NOT NULL,
	hid                  numeric( 19 ) NOT NULL,
	nic_index            int4 NOT NULL,
	project_id           numeric( 19 ) NOT NULL,
	is_sriov             int4 NOT NULL,
	netplane             varchar( 32 ) NOT NULL,
	user_vlan            int4 NOT NULL,
	is_vlan_trunk        int4 NOT NULL,
	if_name              varchar( 32 ) NOT NULL,
	CONSTRAINT idx_vm_nic_cancel PRIMARY KEY ( vid, hid, nic_index )
 );

ALTER TABLE vm_nic_cancel ADD CONSTRAINT ck_34 CHECK ( is_vlan_trunk = 0 OR is_vlan_trunk = 1 );

ALTER TABLE vm_nic_cancel ADD CONSTRAINT ck_41 CHECK ( is_sriov = 0 OR is_sriov = 1 );

ALTER TABLE vm_nic_cancel ADD CONSTRAINT ck_51 CHECK ( user_vlan >= 0 AND user_vlan <= 4095 );

CREATE INDEX idx_vm_nic_cancel_0 ON vm_nic_cancel ( vid, hid, project_id );

CREATE INDEX idx_vm_nic_cancel_1 ON vm_nic_cancel ( project_id, netplane, user_vlan );

CREATE TABLE vm_pool ( 
	oid                  numeric( 19 ) NOT NULL,
	vid                  numeric( 19 ) NOT NULL,
	hid                  numeric( 19 ) NOT NULL,
	hid_next             numeric( 19 ),
	project_id           numeric( 19 ) NOT NULL,
	config               text NOT NULL,
	vcpu                 int4 NOT NULL,
	tcu                  int4 NOT NULL,
	cpu_percent          int4 NOT NULL,
	memory               numeric( 19 ) NOT NULL,
	cpu_rate             int4 NOT NULL,
	memory_rate          int4 NOT NULL,
	net_tx               int4 NOT NULL,
	net_rx               int4 NOT NULL,
	state                int4 NOT NULL,
	lcm_state            int4 NOT NULL,
	last_moni_at         varchar( 32 ),
	last_op              int4 NOT NULL,
	last_op_at           varchar( 32 ) NOT NULL,
	last_op_result       int4 NOT NULL,
	last_op_timeout      int4 NOT NULL,
	last_op_stamp        varchar( 128 ) NOT NULL,
	dnat_out_ip          varchar( 64 ),
	dnat_out_port        int4 NOT NULL,
	vnc_port             int4 NOT NULL,
	last_op_progress     int4 NOT NULL,
	last_op_detail       varchar( 64 ),
	CONSTRAINT vm_pool_pkey PRIMARY KEY ( oid ),
	CONSTRAINT idx_vm_pool_1 UNIQUE ( vid ),
	CONSTRAINT pk_vm_pool UNIQUE ( vid, project_id )
 );

ALTER TABLE vm_pool ADD CONSTRAINT ck_35 CHECK ( state >= 0 AND state <= 11 );

ALTER TABLE vm_pool ADD CONSTRAINT ck_36 CHECK ( lcm_state >= 0 AND lcm_state <= 15 );

ALTER TABLE vm_pool ADD CONSTRAINT ck_37 CHECK ( last_op >= 1 AND last_op <= 19 );

ALTER TABLE vm_pool ADD CONSTRAINT ck_38 CHECK ( dnat_out_port = 0 OR ( dnat_out_port >= 5901 AND dnat_out_port <= 65535 ) );

ALTER TABLE vm_pool ADD CONSTRAINT ck_39 CHECK ( vnc_port = 0 OR ( vnc_port >= 5901 AND vnc_port < 65535 ) );

ALTER TABLE vm_pool ADD CONSTRAINT ck_45 CHECK ( cpu_percent >= 1 AND cpu_percent <= 100 );

ALTER TABLE vm_pool ADD CONSTRAINT ck_46 CHECK ( memory_rate >= 0 AND memory_rate <= 100 );

ALTER TABLE vm_pool ADD CONSTRAINT ck_47 CHECK ( cpu_rate >= 0 AND cpu_rate <= 1000 );

ALTER TABLE vm_pool ADD CONSTRAINT ck_55 CHECK ( last_op_progress >= 0 and last_op_progress <= 100 );

CREATE INDEX idx_vm_pool ON vm_pool ( hid );

CREATE INDEX idx_vm_pool_0 ON vm_pool ( hid_next );

CREATE TABLE config_tcu ( 
	cpu_info_id          numeric( 19 ) NOT NULL,
	tcu_num              int4 NOT NULL,
	description          varchar( 512 ),
	CONSTRAINT config_tcu_pkey PRIMARY KEY ( cpu_info_id )
 );

CREATE TABLE d_dhcp_range ( 
	oid                  numeric( 19 ) NOT NULL,
	netplane             varchar( 32 ),
	ip_start             int4,
	ip_end               int4,
	mask                 int4,
	gateway              int4,
	CONSTRAINT pk_d_dhcp_range PRIMARY KEY ( oid )
 );

CREATE INDEX idx_d_dhcp_range ON d_dhcp_range ( netplane );

COMMENT ON TABLE d_dhcp_range IS 'distribute dhcp range table';

COMMENT ON COLUMN d_dhcp_range.netplane IS 'netplane name';

COMMENT ON COLUMN d_dhcp_range.ip_start IS 'ip start';

COMMENT ON COLUMN d_dhcp_range.mask IS 'net mask';

CREATE TABLE dhcpd_allowdeny ( 
	id                   numeric( 19 ) NOT NULL,
	type                 int4 DEFAULT 106 NOT NULL,
	unknown_client       int4,
	bootp                int4,
	booting              int4,
	CONSTRAINT pk_dhcpd_allowdeny PRIMARY KEY ( id )
 );

COMMENT ON COLUMN dhcpd_allowdeny.type IS 'allowdeny type';

CREATE TABLE dhcpd_group ( 
	id                   numeric( 19 ) NOT NULL,
	type                 int4 DEFAULT 104 NOT NULL,
	name                 varchar( 32 )
 );

COMMENT ON COLUMN dhcpd_group.type IS 'group type';

CREATE TABLE dhcpd_host ( 
	id                   numeric( 19 ) NOT NULL,
	type                 int4 DEFAULT 105 NOT NULL,
	name                 varchar( 64 ),
	CONSTRAINT pk_dhcpd_host PRIMARY KEY ( id )
 );

COMMENT ON COLUMN dhcpd_host.type IS 'host type';

CREATE TABLE host_ctrl_info ( 
	hid                  numeric( 19 ) NOT NULL,
	node_type            varchar( 32 ),
	node_manager         varchar( 64 ),
	node_address         varchar( 64 ),
	CONSTRAINT idx_host_ctrl_info PRIMARY KEY ( hid )
 );

CREATE TABLE host_port ( 
	hid                  numeric( 19 ) NOT NULL,
	name                 varchar( 32 ) NOT NULL,
	negotiate            int4 NOT NULL,
	speed                int4 NOT NULL,
	duplex               int4 NOT NULL,
	master               int4 NOT NULL,
	delay_up_time        int4 NOT NULL,
	sriov_num            int4 NOT NULL,
	work_speed           int4 NOT NULL,
	work_duplex          int4 NOT NULL,
	state                int4 NOT NULL,
	linked               int4 NOT NULL,
	netplane             varchar( 32 ),
	enabled              int4 NOT NULL,
	CONSTRAINT idx_if_port PRIMARY KEY ( hid, name )
 );

ALTER TABLE host_port ADD CONSTRAINT ck_22 CHECK ( state = 0 OR state = 1 );

ALTER TABLE host_port ADD CONSTRAINT ck_23 CHECK ( negotiate = 0 OR negotiate = 1 );

ALTER TABLE host_port ADD CONSTRAINT ck_24 CHECK ( duplex >= 0 AND duplex <= 2 );

ALTER TABLE host_port ADD CONSTRAINT ck_25 CHECK ( speed >= 0 AND speed <= 4 );

ALTER TABLE host_port ADD CONSTRAINT ck_26 CHECK ( master >= 0 AND master <= 3 );

ALTER TABLE host_port ADD CONSTRAINT ck_27 CHECK ( work_speed >= 0 AND work_speed <= 4 );

ALTER TABLE host_port ADD CONSTRAINT ck_28 CHECK ( linked >= 0 AND linked <= 2 );

ALTER TABLE host_port ADD CONSTRAINT ck_29 CHECK ( work_duplex >= 0 AND work_duplex <= 2 );

ALTER TABLE host_port ADD CONSTRAINT ck_30 CHECK ( enabled = 0 OR enabled = 1 );

CREATE INDEX idx_if_port_0 ON host_port ( hid );

CREATE INDEX idx_host_port ON host_port ( netplane );

CREATE TABLE host_trunk ( 
	hid                  numeric( 19 ) NOT NULL,
	name                 varchar( 32 ) NOT NULL,
	type                 int4 NOT NULL,
	state                int4 NOT NULL,
	netplane             varchar( 32 ),
	CONSTRAINT idx_if_trunk PRIMARY KEY ( hid, name )
 );

ALTER TABLE host_trunk ADD CONSTRAINT ck_32 CHECK ( type = 1 OR type = 2 );

CREATE INDEX idx_if_trunk_1 ON host_trunk ( hid );

CREATE INDEX idx_host_trunk ON host_trunk ( netplane );

CREATE TABLE trunk_port ( 
	hid                  numeric( 19 ) NOT NULL,
	trunk_name           varchar( 32 ) NOT NULL,
	port_name            varchar( 32 ) NOT NULL,
	CONSTRAINT idx_trunk_port PRIMARY KEY ( hid, trunk_name, port_name )
 );

CREATE INDEX idx_trunk_port_0 ON trunk_port ( hid, trunk_name );

CREATE INDEX idx_trunk_port_1 ON trunk_port ( hid, port_name );

CREATE TABLE vm_disk ( 
	vid                  numeric( 19 ) NOT NULL,
	target               varchar( 16 ) NOT NULL,
	size                 numeric( 19 ) NOT NULL,
	position             int4 NOT NULL,
	CONSTRAINT idx_vm_disk_pool PRIMARY KEY ( vid, target )
 );

ALTER TABLE vm_disk ADD CONSTRAINT ck_48 CHECK ( position >= 0 AND position <= 2 );

CREATE INDEX idx_vm_disk_pool_0 ON vm_disk ( vid );

CREATE TABLE vm_image ( 
	vid                  numeric( 19 ) NOT NULL,
	target               varchar( 16 ) NOT NULL,
	image_id             numeric( 19 ) NOT NULL,
	occupied_size        numeric( 19 ) NOT NULL,
	position             int4 NOT NULL,
	CONSTRAINT vm_image_pool_pkey PRIMARY KEY ( vid, target )
 );

ALTER TABLE vm_image ADD CONSTRAINT ck_44 CHECK ( position >= 0 AND position <= 2 );

CREATE TABLE vm_nic ( 
	vid                  numeric( 19 ) NOT NULL,
	nic_index            int4 NOT NULL,
	project_id           numeric( 19 ) NOT NULL,
	is_sriov             int4 NOT NULL,
	netplane             varchar( 32 ) NOT NULL,
	user_vlan            int4 NOT NULL,
	is_vlan_trunk        int4 NOT NULL,
	if_name              varchar( 32 ) NOT NULL,
	CONSTRAINT idx_vm_nic_pool PRIMARY KEY ( vid, nic_index )
 );

ALTER TABLE vm_nic ADD CONSTRAINT ck_40 CHECK ( is_sriov = 0 OR is_sriov = 1 );

ALTER TABLE vm_nic ADD CONSTRAINT ck_33 CHECK ( is_vlan_trunk = 0 OR is_vlan_trunk = 1 );

ALTER TABLE vm_nic ADD CONSTRAINT ck_50 CHECK ( user_vlan >= 0 AND user_vlan <= 4095 );

CREATE INDEX idx_vm_nic_pool_0 ON vm_nic ( vid, project_id );

CREATE INDEX idx_vm_nic_pool_1 ON vm_nic ( project_id, netplane, user_vlan );

ALTER TABLE config_tcu ADD CONSTRAINT config_tcu_cpu_info_id_fkey FOREIGN KEY ( cpu_info_id ) REFERENCES host_cpuinfo( cpu_info_id ) ON DELETE CASCADE;

ALTER TABLE d_dhcp_range ADD CONSTRAINT fk_d_dhcp_range_netplane FOREIGN KEY ( netplane ) REFERENCES netplane( name ) ON DELETE CASCADE;

ALTER TABLE dhcpd_allowdeny ADD CONSTRAINT fk_dhcpd_allowdeny_dhcpd_map FOREIGN KEY ( id, type ) REFERENCES dhcpd_map( id, type ) ON DELETE CASCADE;

ALTER TABLE dhcpd_group ADD CONSTRAINT fk_dhcpd_group_dhcpd_map FOREIGN KEY ( id, type ) REFERENCES dhcpd_map( id, type ) ON DELETE CASCADE;

ALTER TABLE dhcpd_host ADD CONSTRAINT fk_dhcpd_host_dhcpd_map FOREIGN KEY ( id, type ) REFERENCES dhcpd_map( id, type ) ON DELETE CASCADE;

ALTER TABLE dhcpd_option ADD CONSTRAINT fk_dhcpd_option_dhcpd_map FOREIGN KEY ( id, type ) REFERENCES dhcpd_map( id, type ) ON DELETE CASCADE;

ALTER TABLE dhcpd_param ADD CONSTRAINT fk_dhcpd_param_dhcpd_map FOREIGN KEY ( id, type ) REFERENCES dhcpd_map( id, type ) ON DELETE CASCADE;

ALTER TABLE dhcpd_range ADD CONSTRAINT fk_dhcpd_range_dhcpd_map FOREIGN KEY ( id, type ) REFERENCES dhcpd_map( id, type ) ON DELETE CASCADE;

ALTER TABLE dhcpd_sharenet ADD CONSTRAINT fk_dhcpd_sharenet_dhcpd_map FOREIGN KEY ( id, type ) REFERENCES dhcpd_map( id, type ) ON DELETE CASCADE;

ALTER TABLE dhcpd_subnet ADD CONSTRAINT fk_dhcpd_subnet_dhcpd_map FOREIGN KEY ( id, type ) REFERENCES dhcpd_map( id, type ) ON DELETE CASCADE;

ALTER TABLE host_ctrl_info ADD CONSTRAINT fk_host_ctrl_info_host_pool FOREIGN KEY ( hid ) REFERENCES host_pool( oid ) ON DELETE CASCADE;

ALTER TABLE host_pool ADD CONSTRAINT host_pool_cpu_info_id_fkey FOREIGN KEY ( cpu_info_id ) REFERENCES host_cpuinfo( cpu_info_id ) ON DELETE RESTRICT;

ALTER TABLE host_port ADD CONSTRAINT fk_if_port_host_pool FOREIGN KEY ( hid ) REFERENCES host_pool( oid ) ON DELETE CASCADE;

ALTER TABLE host_port ADD CONSTRAINT fk_host_port_netplane FOREIGN KEY ( netplane ) REFERENCES netplane( name ) ON DELETE RESTRICT;

ALTER TABLE host_trunk ADD CONSTRAINT fk_if_trunk_host_pool FOREIGN KEY ( hid ) REFERENCES host_pool( oid ) ON DELETE CASCADE;

ALTER TABLE host_trunk ADD CONSTRAINT fk_host_trunk_netplane FOREIGN KEY ( netplane ) REFERENCES netplane( name ) ON DELETE RESTRICT;

ALTER TABLE netplane_map_public ADD CONSTRAINT fk_netplane_map_public FOREIGN KEY ( netplane ) REFERENCES netplane( name ) ON DELETE RESTRICT;

ALTER TABLE trunk_port ADD CONSTRAINT fk_trunk_port_if_trunk FOREIGN KEY ( hid, trunk_name ) REFERENCES host_trunk( hid, name ) ON DELETE CASCADE;

ALTER TABLE trunk_port ADD CONSTRAINT fk_trunk_port_if_port FOREIGN KEY ( hid, port_name ) REFERENCES host_port( hid, name ) ON DELETE CASCADE;

ALTER TABLE vlan_pool ADD CONSTRAINT fk_vlan_pool_netplane FOREIGN KEY ( netplane ) REFERENCES netplane( name ) ON DELETE RESTRICT;

ALTER TABLE vlan_range ADD CONSTRAINT fk_vlan_range_netplane FOREIGN KEY ( netplane ) REFERENCES netplane( name ) ON DELETE CASCADE;

ALTER TABLE vm_cancel_pool ADD CONSTRAINT fk_vm_task_pool_host_pool FOREIGN KEY ( hid ) REFERENCES host_pool( oid ) ON DELETE RESTRICT;

ALTER TABLE vm_disk ADD CONSTRAINT fk_vm_disk_pool_vm_pool FOREIGN KEY ( vid ) REFERENCES vm_pool( vid ) ON DELETE CASCADE;

ALTER TABLE vm_disk_cancel ADD CONSTRAINT fk_vm_disk_cancel_vm_task_pool FOREIGN KEY ( hid, vid ) REFERENCES vm_cancel_pool( hid, vid ) ON DELETE CASCADE;

ALTER TABLE vm_image ADD CONSTRAINT vm_image_pool_vid_fkey FOREIGN KEY ( vid ) REFERENCES vm_pool( vid ) ON DELETE CASCADE;

ALTER TABLE vm_image_cancel ADD CONSTRAINT vm_image_cancel_vid_fkey FOREIGN KEY ( vid, hid ) REFERENCES vm_cancel_pool( vid, hid ) ON DELETE CASCADE;

ALTER TABLE vm_nic ADD CONSTRAINT fk_vm_nic_pool_vm_pool FOREIGN KEY ( vid, project_id ) REFERENCES vm_pool( vid, project_id ) ON DELETE CASCADE;

ALTER TABLE vm_nic ADD CONSTRAINT fk_vm_nic_pool_vlan_pool FOREIGN KEY ( project_id, netplane, user_vlan, is_vlan_trunk ) REFERENCES vlan_pool( project_id, netplane, user_vlan, is_vlan_trunk ) ON DELETE RESTRICT;

ALTER TABLE vm_nic_cancel ADD CONSTRAINT fk_vm_nic_cancel_vm_cancel FOREIGN KEY ( vid, hid, project_id ) REFERENCES vm_cancel_pool( vid, hid, project_id ) ON DELETE CASCADE;

ALTER TABLE vm_nic_cancel ADD CONSTRAINT fk_vm_nic_cancel_vlan_pool FOREIGN KEY ( project_id, netplane, user_vlan, is_vlan_trunk ) REFERENCES vlan_pool( project_id, netplane, user_vlan, is_vlan_trunk ) ON DELETE RESTRICT;

ALTER TABLE vm_pool ADD CONSTRAINT fk_vm_pool_host_pool FOREIGN KEY ( hid ) REFERENCES host_pool( oid ) ON DELETE RESTRICT;

ALTER TABLE vm_pool ADD CONSTRAINT fk_vm_pool_host_pool_1 FOREIGN KEY ( hid_next ) REFERENCES host_pool( oid ) ON DELETE RESTRICT;

INSERT INTO _schema_log(version_number, update_by,update_at,description) VALUES ('01.01.10.0000', 'tecs',now(),'baseline');

