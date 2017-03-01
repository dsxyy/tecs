------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：颜伟155391
--修改时间：2012.11.19
--修改补丁号：0016
--修改内容说明：增加镜像共享相关表
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

CREATE TABLE cache_image ( 
	cache_id             numeric( 19 ) NOT NULL,
	image_id             numeric( 19 ) NOT NULL,
	CONSTRAINT pk_cache_image PRIMARY KEY ( cache_id )
 );

ALTER TABLE cache_image ADD CONSTRAINT fk_cache_image FOREIGN KEY ( image_id ) REFERENCES image_pool( image_id ) 

ON DELETE RESTRICT;

CREATE TABLE cache_image_cluster ( 
	cache_id             numeric( 19 ) NOT NULL,
	cluster_name         varchar( 64 ) NOT NULL,
	state                int4 NOT NULL,
	detail               varchar( 256 ) NOT NULL,
	last_op              int4 NOT NULL,
	last_op_at           varchar( 32 ) NOT NULL,
	CONSTRAINT pk_cache_image_cluster PRIMARY KEY ( cache_id, cluster_name )
 );

ALTER TABLE cache_image_cluster ADD CONSTRAINT fk_cache_image_cluster FOREIGN KEY ( cluster_name ) REFERENCES cluster_pool( name ) 

ON DELETE RESTRICT;

ALTER TABLE storage_volume ALTER COLUMN volume_uuid DROP NOT NULL;

ALTER TABLE storage_volume ADD COLUMN request_id varchar( 64 ) NOT NULL;

ALTER TABLE storage_volume ADD CONSTRAINT un_storage_volume_2 UNIQUE ( request_id );

ALTER TABLE cluster_pool ADD COLUMN img_usage varchar( 64 );


------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0017', 'tecs', now(), 'add tables for image share');