------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人:谷微微
--修改时间：2013.12.11
--修改补丁号：0059
--修改说明：新增storage_adaptor_with_multivg_and_cluster_view
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--


------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0059', 'guww', now(), 'add storage_adaptor_with_multivg_and_cluster_view');