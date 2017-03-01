------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：yanwei
--修改时间：2013.4.25
--修改补丁号：0031
--修改内容说明：创建表image_base和vm_image_snapshot
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

CREATE TABLE image_base ( 
	base_uuid            varchar( 64 ) NOT NULL,
	image_id             numeric( 19 ) NOT NULL,
	size                 numeric( 19 ) NOT NULL,
	access_url           varchar( 1024 ) NOT NULL,
	CONSTRAINT pk_image_base PRIMARY KEY ( base_uuid )
 );
 
INSERT INTO image_base SELECT cast(cache_id AS varchar(64)), image_id, size, target_url From image_cache;
 
CREATE TABLE vm_image_snapshot ( 
	request_id           varchar( 64 ) NOT NULL,
	base_uuid            varchar( 64 ) NOT NULL,
	access_url           varchar( 1024 ) NOT NULL,
	CONSTRAINT pk_vm_image_snapshot PRIMARY KEY ( request_id )
 );
 
ALTER TABLE vm_image_snapshot ADD CONSTRAINT fk_vm_image_snapshot FOREIGN KEY ( base_uuid ) REFERENCES image_base( base_uuid ) ON DELETE RESTRICT;
 
INSERT INTO vm_image_snapshot SELECT request_id, cast(cache_id AS varchar(64)), target_url From image_cache;
 
------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0031', 'tecs',now(),'create table image_base and vm_image_snapshot');
------------------------------------------------------------------------------------