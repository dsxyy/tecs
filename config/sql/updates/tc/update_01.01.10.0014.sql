------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：颜伟155391
--修改时间：2012.9.27
--修改补丁号：0014
--修改内容说明：本文件是数据库schema升级脚本的示例范本
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

CREATE TABLE storage_adaptor ( 
	name                 varchar( 64 ) NOT NULL,
	ip_address           varchar( 64 ) NOT NULL,
	CONSTRAINT pk_storage_adaptor PRIMARY KEY ( name )
 );

COMMENT ON COLUMN storage_adaptor.name IS 'sa_name SA的application';

COMMENT ON COLUMN storage_adaptor.ip_address IS 'SA所驻留机器与QPID通信的IP地址，用于后续SSH升级RPM包';

CREATE TABLE storage_device_pool ( 
	sid                  numeric( 19 ) NOT NULL,
	name                 varchar( 256 ) NOT NULL,
	sa_name              varchar( 64 ) NOT NULL,
	ctrl_address         varchar( 64 ) NOT NULL,
	type                 varchar( 64 ) NOT NULL,
	is_online            int4 NOT NULL,
	enabled              int4 NOT NULL,
	description          varchar( 512 ),
	register_time        varchar( 32 ) NOT NULL,
	CONSTRAINT pk_storage_device_pool PRIMARY KEY ( sid ),
	CONSTRAINT un_storage_device_pool UNIQUE ( sid, name )
 );

ALTER TABLE storage_device_pool ADD CONSTRAINT ck_0 CHECK ( type='IPSAN' OR type='FCSAN' OR type='LOCALSAN' );

ALTER TABLE storage_device_pool ADD CONSTRAINT ck_1 CHECK ( is_online = 0 OR is_online = 1 );

ALTER TABLE storage_device_pool ADD CONSTRAINT ck_2 CHECK ( enabled = 0 OR enabled = 1 );

COMMENT ON COLUMN storage_device_pool.name IS '通信地址的 unit';

COMMENT ON COLUMN storage_device_pool.sa_name IS 'SA的application';

COMMENT ON COLUMN storage_device_pool.ctrl_address IS 'web_view，设备的管理地址，非SA的地址';

COMMENT ON COLUMN storage_device_pool.type IS 'web_view';

COMMENT ON COLUMN storage_device_pool.is_online IS '是否在线';

COMMENT ON COLUMN storage_device_pool.enabled IS '是否维护状态';

CREATE TABLE storage_target ( 
	sid                  numeric( 19 ) NOT NULL,
	name                 varchar( 128 ) NOT NULL,
	target_id            numeric( 19 ) NOT NULL,
	CONSTRAINT pk_storage_target PRIMARY KEY ( sid, name ),
	CONSTRAINT un_storage_target_0 UNIQUE ( sid, target_id ),
	CONSTRAINT un_storage_target_1 UNIQUE ( sid, name )
 );

COMMENT ON COLUMN storage_target.name IS 'target_name';

CREATE TABLE storage_target_host ( 
	sid                  numeric( 19 ) NOT NULL,
	target_name          varchar( 128 ) NOT NULL,
	ip_address           varchar( 64 ) NOT NULL,
	iscsi_initiator      varchar( 128 ) NOT NULL,
	CONSTRAINT pk_storage_target_host PRIMARY KEY ( sid, target_name, ip_address, iscsi_initiator )
 );

CREATE TABLE storage_vg ( 
	sid                  numeric( 19 ) NOT NULL,
	name                 varchar( 128 ) NOT NULL,
	size                 numeric( 19 ) NOT NULL,
	CONSTRAINT pk_storage_vg PRIMARY KEY ( sid, name )
 );

COMMENT ON COLUMN storage_vg.name IS 'volume_name';

CREATE TABLE storage_volume ( 
	sid                  numeric( 19 ) NOT NULL,
	vg_name              varchar( 128 ) NOT NULL,
	name                 varchar( 128 ) NOT NULL,
	size                 numeric( 19 ) NOT NULL,
	volume_id            numeric( 19 ) NOT NULL,
	volume_uuid          varchar( 64 ) NOT NULL,
	CONSTRAINT pk_storage_volume PRIMARY KEY ( sid, vg_name, name ),
	CONSTRAINT un_storage_volume_0 UNIQUE ( volume_id ),
	CONSTRAINT un_storage_volume_1 UNIQUE ( volume_uuid )
 );
 
COMMENT ON COLUMN storage_volume.name IS 'volume_name';

COMMENT ON COLUMN storage_volume.size IS '单位B';

CREATE TABLE user_volume_pool ( 
	uid                  numeric( 19 ) NOT NULL,
	volume_uuid          varchar( 64 ) NOT NULL,
	CONSTRAINT pk_user_volume_pool PRIMARY KEY ( uid, volume_uuid ),
	CONSTRAINT un_user_volume_pool UNIQUE ( volume_uuid )
 );

CREATE TABLE storage_address ( 
	sid                  numeric( 19 ) NOT NULL,
	media_address        varchar( 64 ) NOT NULL,
	CONSTRAINT pk_storage_address PRIMARY KEY ( sid, media_address )
 );

COMMENT ON COLUMN storage_address.media_address IS 'Storage Device的数据面IP';

CREATE TABLE storage_cluster ( 
	sid                  numeric( 19 ) NOT NULL,
	cluster_name         varchar( 64 ) NOT NULL,
	CONSTRAINT pk_storage_cluster PRIMARY KEY ( sid, cluster_name )
 );

CREATE TABLE storage_lun ( 
	sid                  numeric( 19 ) NOT NULL,
	target_name          varchar( 128 ) NOT NULL,
	lun_id               numeric( 19 ) NOT NULL,
	CONSTRAINT pk_storage_lun PRIMARY KEY ( sid, target_name, lun_id )
 );

CREATE TABLE storage_target_volume ( 
	sid                  numeric( 19 ) NOT NULL,
	vg_name              varchar( 128 ) NOT NULL,
	volume_name          varchar( 128 ) NOT NULL,
	target_name          varchar( 128 ) NOT NULL,
	lun_id               numeric( 19 ) NOT NULL,
	CONSTRAINT pk_storage_target_volume PRIMARY KEY ( sid, vg_name, volume_name, target_name, lun_id )
 );

ALTER TABLE storage_address ADD CONSTRAINT fk_storage_address FOREIGN KEY ( sid ) REFERENCES storage_device_pool( sid ) ON 

DELETE CASCADE;

ALTER TABLE storage_cluster ADD CONSTRAINT fk_storage_cluster FOREIGN KEY ( sid ) REFERENCES storage_device_pool( sid ) ON 

DELETE CASCADE;

ALTER TABLE storage_cluster ADD CONSTRAINT fk_storage_cluster1 FOREIGN KEY ( cluster_name ) REFERENCES cluster_pool( name ) 

ON DELETE CASCADE;

ALTER TABLE storage_device_pool ADD CONSTRAINT fk_storage_device_pool FOREIGN KEY ( sa_name ) REFERENCES storage_adaptor( 

name ) ON DELETE CASCADE;

ALTER TABLE storage_lun ADD CONSTRAINT fk_storage_lun FOREIGN KEY ( sid, target_name ) REFERENCES storage_target( sid, name ) 

ON DELETE RESTRICT;

ALTER TABLE storage_target ADD CONSTRAINT fk_storage_vg FOREIGN KEY ( sid ) REFERENCES storage_device_pool( sid ) ON DELETE 

RESTRICT;

ALTER TABLE storage_target_host ADD CONSTRAINT fk_storage_target_host FOREIGN KEY ( sid, target_name ) REFERENCES 

storage_target( sid, name ) ON DELETE RESTRICT;

ALTER TABLE storage_target_volume ADD CONSTRAINT fk_storage_volume_2 FOREIGN KEY ( sid, target_name, lun_id ) REFERENCES 

storage_lun( sid, target_name, lun_id ) ON DELETE RESTRICT;

ALTER TABLE storage_target_volume ADD CONSTRAINT fk_storage_volume_1 FOREIGN KEY ( sid, vg_name, volume_name ) REFERENCES 

storage_volume( sid, vg_name, name ) ON DELETE RESTRICT;

ALTER TABLE storage_vg ADD CONSTRAINT fk_storage_volume FOREIGN KEY ( sid ) REFERENCES storage_device_pool( sid ) ON DELETE 

CASCADE;

ALTER TABLE storage_volume ADD CONSTRAINT fk_storage_volume_storage_vg FOREIGN KEY ( sid, vg_name ) REFERENCES storage_vg( 

sid, name ) ON DELETE RESTRICT;

ALTER TABLE user_volume_pool ADD CONSTRAINT fk_user_volume_pool FOREIGN KEY ( volume_uuid ) REFERENCES storage_volume( 

volume_uuid ) ON DELETE RESTRICT;

ALTER TABLE vmcfg_disk ADD COLUMN volume_uuid varchar( 64 );

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0014', 'tecs', now(), 'add tables for sc');