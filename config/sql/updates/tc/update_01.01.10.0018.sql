------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：颜伟155391
--修改时间：2012.12.24
--修改补丁号：0018
--修改内容说明：增加镜像共享相关表
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

DROP TABLE user_volume_pool;

ALTER TABLE storage_volume DROP CONSTRAINT un_storage_volume_1;

ALTER TABLE storage_vg DROP CONSTRAINT fk_storage_volume;

ALTER TABLE storage_target DROP CONSTRAINT fk_storage_vg;

ALTER TABLE storage_cluster DROP CONSTRAINT fk_storage_cluster;

ALTER TABLE storage_address DROP CONSTRAINT fk_storage_address;

DROP TABLE storage_device_pool;

DROP TABLE storage_adaptor;

CREATE TABLE storage_adaptor_address ( 
	application          varchar( 64 ) NOT NULL,
	ip_address           varchar( 64 ) NOT NULL,
	CONSTRAINT pk_storage_adaptor_address PRIMARY KEY ( application )
 );
 
 CREATE TABLE storage_adaptor ( 
    sid                  numeric( 19 ) NOT NULL,
	name                 varchar( 256 ) NOT NULL,
	application          varchar( 64 ) NOT NULL,
	ctrl_address         varchar( 64 ) NOT NULL,
	type                 varchar( 64 ) NOT NULL,
	is_online            int4 NOT NULL,
	enabled              int4 NOT NULL,
	description          varchar( 512 ),
	register_time        varchar( 32 ) NOT NULL,
	refresh_time         varchar( 32 ) NOT NULL,
	CONSTRAINT pk_storage_adaptor PRIMARY KEY ( sid ),
	CONSTRAINT un_storage_adaptor UNIQUE ( sid, name )
 );

ALTER TABLE storage_adaptor ADD CONSTRAINT fk_storage_adaptor FOREIGN KEY ( application ) REFERENCES storage_adaptor_address( application );

ALTER TABLE storage_vg ADD CONSTRAINT fk_storage_vg FOREIGN KEY ( sid ) REFERENCES storage_adaptor( sid );

ALTER TABLE storage_target ADD CONSTRAINT fk_storage_target FOREIGN KEY ( sid ) REFERENCES storage_adaptor( sid );

ALTER TABLE storage_cluster ADD CONSTRAINT fk_storage_cluster FOREIGN KEY ( sid ) REFERENCES storage_adaptor( sid );

ALTER TABLE storage_address ADD CONSTRAINT fk_storage_address FOREIGN KEY ( sid ) REFERENCES storage_adaptor( sid );
 
ALTER TABLE storage_lun ADD COLUMN lun_uuid varchar( 64 ) NOT NULL;

ALTER TABLE storage_lun ADD COLUMN volume_uuid varchar( 64 ) NOT NULL;

ALTER TABLE storage_lun ADD CONSTRAINT un_storage_lun UNIQUE ( lun_uuid );

DROP TABLE storage_target_host;

DROP TABLE storage_target_volume; 

CREATE TABLE storage_authorize ( 
	sid                  numeric( 19 ) NOT NULL,
	target_name          varchar( 128 ) NOT NULL,
	ip_address           varchar( 64 ) NOT NULL,
	iscsi_initiator      varchar( 128 ) NOT NULL,
	auth_uuid            varchar( 64 ) NOT NULL,
	CONSTRAINT pk_storage_authorize PRIMARY KEY ( sid, target_name, ip_address, iscsi_initiator )
 );

ALTER TABLE storage_authorize ADD CONSTRAINT un_storage_authorize UNIQUE ( auth_uuid );

ALTER TABLE storage_authorize ADD CONSTRAINT fk_storage_authorize FOREIGN KEY ( sid, target_name ) REFERENCES storage_target( sid, name );

CREATE TABLE storage_authorize_lun ( 
	auth_uuid            varchar( 64 ) NOT NULL,
	lun_uuid             varchar( 64 ) NOT NULL,
	CONSTRAINT pk_storage_authorize_lun PRIMARY KEY ( auth_uuid, lun_uuid )
 );

ALTER TABLE storage_authorize_lun ADD CONSTRAINT fk_storage_authorize_auth_uuid FOREIGN KEY ( auth_uuid ) REFERENCES storage_authorize( auth_uuid );

ALTER TABLE storage_authorize_lun ADD CONSTRAINT fk_storage_authorize_lun_uuid FOREIGN KEY ( lun_uuid ) REFERENCES storage_lun( lun_uuid );

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0018', 'tecs', now(), 'modify share storage tables');