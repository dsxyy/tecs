------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人:liuyi
--修改时间：2013.3.26
--修改补丁号：0031
--deleted_vmcfg_pool,vmtemplate_pool,vmcfg_pool中增加is_serial_disabled字段
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
ALTER TABLE deleted_vmcfg_pool ADD COLUMN enable_serial int4 DEFAULT 0 NOT NULL;

ALTER TABLE deleted_vmcfg_pool ADD CONSTRAINT ck_56 CHECK ( enable_serial = 0 OR enable_serial = 1 );

ALTER TABLE vmtemplate_pool ADD COLUMN enable_serial int4 DEFAULT 0 NOT NULL;

ALTER TABLE vmtemplate_pool ADD CONSTRAINT ck_57 CHECK ( enable_serial = 0 OR enable_serial = 1 );

ALTER TABLE vmcfg_pool ADD COLUMN enable_serial int4 DEFAULT 0 NOT NULL;

ALTER TABLE vmcfg_pool ADD CONSTRAINT ck_58 CHECK ( enable_serial = 0 OR enable_serial = 1 );
------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0033', 'tecs', now(), 'add enable_serial in deleted_vmcfg_pool,vmtemplate_pool and vmcfg_pool ');