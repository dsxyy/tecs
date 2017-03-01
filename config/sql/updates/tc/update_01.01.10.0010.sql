------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：姚远147328
--修改时间：2012.10.22
--修改补丁号：0010
--修改内容说明：为v4提供tdm功能
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
CREATE TABLE vmcfg_device ( 
	vid                  numeric( 19 ) NOT NULL,
	type                 int4 NOT NULL,
	count                int4 NOT NULL,
	CONSTRAINT idx_vmcfg_device_pool PRIMARY KEY ( vid, type )
 );

ALTER TABLE vmcfg_device ADD CONSTRAINT ck_28 CHECK ( type=1 );

ALTER TABLE vmcfg_device ADD CONSTRAINT ck_39 CHECK ( count>0 );

CREATE TABLE vmtemplate_device ( 
	vmtemplate_id        numeric( 19 ) NOT NULL,
	type                 int4 NOT NULL,
	count                int4 NOT NULL,
	CONSTRAINT idx_vmcfg_device_pool_0 PRIMARY KEY ( vmtemplate_id, type )
 );

ALTER TABLE vmtemplate_device ADD CONSTRAINT ck_37 CHECK ( type=1 );

ALTER TABLE vmtemplate_device ADD CONSTRAINT ck_38 CHECK ( count > 0 );

CREATE INDEX idx_vmcfg_device_pool_1 ON vmtemplate_device ( vmtemplate_id );

ALTER TABLE vmcfg_device ADD CONSTRAINT fk_vmcfg_device_pool FOREIGN KEY ( vid ) REFERENCES vmcfg_pool( oid ) ON DELETE CASCADE;

ALTER TABLE vmtemplate_device ADD CONSTRAINT fk_vmtemplate_device FOREIGN KEY ( vmtemplate_id ) REFERENCES vmtemplate_pool( oid ) ON DELETE CASCADE;


------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0010', 'tecs',now(),'add tdm for v4');
------------------------------------------------------------------------------------