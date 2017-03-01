------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：yaoyuan
--修改时间：2013.11.13
--修改补丁号：0052
--修改内容说明：vm_pool.hid 可以非空
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

alter table vm_pool alter  column  hid DROP NOT NULL;
ALTER TABLE vm_pool ADD COLUMN log_state int4 DEFAULT 0;

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0052', 'yaoyuan',now(),'set vm_pool.hid null ');
------------------------------------------------------------------------------------