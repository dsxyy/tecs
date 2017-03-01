------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人:张海峰
--修改时间：2012.12.31
--修改补丁号：0019
--修改内容说明：将cluster_pool中share_disk的两个字段去掉
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

ALTER TABLE cluster_pool DROP COLUMN share_disk_max_total;
ALTER TABLE cluster_pool DROP COLUMN share_disk_free_total;

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0019', 'tecs', now(), 'delete share_disk items in cluster_pool');