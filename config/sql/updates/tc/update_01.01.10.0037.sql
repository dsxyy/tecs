------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：颜伟
--修改时间：2013.5.7
--修改补丁号：0037
--修改说明：创建表storage_user_volume,storage_snapshot
------------------------------------------------------------------------------------
CREATE TABLE storage_user_volume (
    uid                  numeric( 19 ) NOT NULL, 
	request_id           varchar( 64 ) NOT NULL,
	usage                varchar( 256 ),
	description          varchar( 512 ),
	CONSTRAINT pk_storage_user_volume PRIMARY KEY ( uid , request_id )
 );

ALTER TABLE storage_user_volume ADD CONSTRAINT fk_storage_user_volume FOREIGN KEY ( request_id ) REFERENCES storage_volume( request_id ) ON DELETE RESTRICT;
 
INSERT INTO storage_user_volume ( uid , request_id ) SELECT -1, request_id FROM storage_volume;
 
CREATE TABLE storage_snapshot (
    base_uuid            varchar( 64 ) NOT NULL, 
	snapshot_uuid        varchar( 64 ) NOT NULL,
	CONSTRAINT pk_storage_snapshot PRIMARY KEY ( snapshot_uuid )
 );

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0000', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0037', 'yanwei', now(),'create table storage_user_volume and storage_snapshot ');