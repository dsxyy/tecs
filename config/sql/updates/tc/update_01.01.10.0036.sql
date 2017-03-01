------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：颜伟、冯骏
--修改时间：2013.4.25
--修改补丁号：0036
--修改说明：cluster_pool中增加img_srctype和img_spcexp字段,创建表base_image和base_image_cluster
--修改说明：将sc与cluster的外键删除
------------------------------------------------------------------------------------
ALTER TABLE cluster_pool ADD COLUMN img_srctype VARCHAR(32);

ALTER TABLE cluster_pool ADD CONSTRAINT ck_24 CHECK ( img_srctype = 'file' OR img_srctype = 'block' );

ALTER TABLE cluster_pool ADD COLUMN img_spcexp int4 DEFAULT 0;

CREATE TABLE base_image ( 
	base_uuid            varchar( 64 ) NOT NULL,
	base_id              numeric( 19 ) NOT NULL,
	image_id             numeric( 19 ) NOT NULL,
	CONSTRAINT pk_base_image PRIMARY KEY ( base_uuid )
 );

ALTER TABLE base_image ADD CONSTRAINT fk_base_image FOREIGN KEY ( image_id ) REFERENCES image_pool( image_id ) ON DELETE RESTRICT;

ALTER TABLE base_image ADD CONSTRAINT un_base_image UNIQUE ( base_id );

INSERT INTO base_image SELECT cast(cache_id AS varchar(64)), cache_id, image_id From cache_image;

CREATE TABLE base_image_cluster ( 
	base_uuid            varchar( 64 ) NOT NULL,
	cluster_name         varchar( 64 ) NOT NULL,
	CONSTRAINT pk_base_image_cluster PRIMARY KEY ( base_uuid ,  cluster_name)
 );
 
ALTER TABLE base_image_cluster ADD CONSTRAINT fk_base_image_cluster FOREIGN KEY ( base_uuid ) REFERENCES base_image( base_uuid ) ON DELETE RESTRICT;

INSERT INTO base_image_cluster SELECT cast(cache_id AS varchar(64)), cluster_name From cache_image_cluster;

ALTER TABLE storage_cluster DROP CONSTRAINT fk_storage_cluster1;

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0000', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0036', 'yanwei/fengjun', now(),'delete storage_cluster foreign key');