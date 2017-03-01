------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人:姚远
--修改时间：2013.7.31
--修改补丁号：0044
--修改说明：修改vm_pool，增加requirement appoint_host
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
ALTER TABLE vm_pool ADD COLUMN requirement text;
ALTER TABLE vm_pool ADD COLUMN appoint_host character varying(64);

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0044', 'tecs', now(), 'Add requirement and appoint_host from vm_pool');
