------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：liuyi
--修改时间：2013.3.28
--修改补丁号：0024
--修改内容说明：vm_pool表增加serial_port字段
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

ALTER TABLE vm_pool ADD COLUMN serial_port int4 DEFAULT 0 NOT NULL;

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0024', 'tecs',now(),'alter table vm_pool add column serial_port');
------------------------------------------------------------------------------------