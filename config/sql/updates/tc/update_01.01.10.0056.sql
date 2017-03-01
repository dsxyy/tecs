------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人:杜文超
--修改时间：2013.10.28
--修改补丁号：0056
--修改说明：增加vmcfg_pool\vmtemplate_pool表字段enable_livemigrate
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
 
ALTER TABLE vmcfg_pool ADD COLUMN enable_livemigrate integer NOT NULL DEFAULT 1;
ALTER TABLE vmtemplate_pool ADD COLUMN enable_livemigrate integer NOT NULL DEFAULT 1;

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0056', 'duwch', now(), 'add table vmcfg_pool column enable_livemigrate');
