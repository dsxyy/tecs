------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：颜伟155391
--修改时间：2012.8.24
--修改补丁号：0005
--修改内容说明：vm_pool表增加config_version和run_version字段
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
ALTER TABLE vm_pool ADD COLUMN config_version INTEGER;
ALTER TABLE vm_pool ADD COLUMN run_version INTEGER;
------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0005', 'tecs',now(),'ALTER vm_pool ADD (config_version and run_version)');
------------------------------------------------------------------------------------