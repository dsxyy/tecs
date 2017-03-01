------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：李春
--修改时间：2012.8.31
--修改补丁号：0004
--修改内容说明：将_image_info_with_running_vms_and_refcount_view中，ref_count_by_privat_vt改成ref_count_by_private_vt
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
VALUES ('01.01.10.0004', 'tecs',now(), 'modify ref_count_by_privat_vt to ref_count_by_private_vt in tc_view');

