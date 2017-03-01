------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：zhoubin
--修改时间：2013.10.17
--修改补丁号：0053
--修改内容说明：host_pool表中增加字段running_time
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

ALTER TABLE host_pool  ADD COLUMN running_time  numeric( 19 ) NOT NULL default 0;

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0053', 'zhoubin',now(),'add running_time field to host_pool table');
------------------------------------------------------------------------------------