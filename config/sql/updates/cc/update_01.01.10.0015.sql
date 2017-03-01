------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：颜伟155391
--修改时间：2012.12.21
--修改补丁号：0015
--修改内容说明：删除image_cache表的source和target唯一约束
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

ALTER TABLE image_cache ADD COLUMN cache_id numeric( 19 ) NOT NULL;

ALTER TABLE image_cache ADD COLUMN position int4 NOT NULL;

ALTER TABLE image_cache ADD COLUMN progress int4 NOT NULL;

ALTER TABLE image_cache ADD COLUMN request_id varchar( 64 ) NOT NULL;

ALTER TABLE image_cache ADD COLUMN volume_uuid varchar( 64 );

ALTER TABLE image_cache ADD COLUMN target_url varchar( 1024 );

ALTER TABLE image_cache ADD COLUMN check_point int4 NOT NULL;

ALTER TABLE image_cache DROP CONSTRAINT image_cache_pkey;

ALTER TABLE image_cache ADD CONSTRAINT pk_image_cache PRIMARY KEY ( cache_id );

ALTER TABLE image_cache ADD CONSTRAINT un_image_cache_1 UNIQUE ( request_id );

CREATE TABLE image_cache_snapshot ( 
	request_id           varchar( 64 ) NOT NULL,
	hid                  numeric( 19 ) NOT NULL,
	vid                  numeric( 19 ) NOT NULL,
	target               varchar( 16 ) NOT NULL,
	cache_id             numeric( 19 ) NOT NULL,
	volume_uuid          varchar( 64 ) ,
	image_url            varchar( 1024 ),
	CONSTRAINT pk_image_cache_snapshot PRIMARY KEY ( request_id )
 );

ALTER TABLE image_cache_snapshot ADD CONSTRAINT un_image_cache_snapshot_1 UNIQUE ( hid, vid, target );

CREATE TABLE vm_disk_share ( 
	request_id           varchar( 64 ) NOT NULL,
	hid                  numeric( 19 ) NOT NULL,
	vid                  numeric( 19 ) NOT NULL,
	target               varchar( 16 ) NOT NULL,
	volume_uuid          varchar( 64 ) ,
	disk_url             varchar( 1024 ),
	CONSTRAINT pk_vm_disk_share PRIMARY KEY ( request_id )
 );

ALTER TABLE vm_disk_share ADD CONSTRAINT un_vm_disk_share_1 UNIQUE ( hid, vid, target );


ALTER TABLE image_cache DROP CONSTRAINT image_cache_source_key;
ALTER TABLE image_cache DROP CONSTRAINT image_cache_target_key;

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0015', 'tecs',now(),'alter table image_cache drop constraint image_cache_source_key and image_cache_target_key');
------------------------------------------------------------------------------------