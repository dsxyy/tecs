------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：liuyi
--修改时间：2013.3.28
--修改补丁号：0026
--修改内容说明：vm_pool表增加serial_port字段
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

ALTER TABLE vm_pool ADD COLUMN enable_serial int4 DEFAULT 0 NOT NULL;

ALTER TABLE vm_pool ADD CONSTRAINT ck_56 CHECK ( enable_serial = 0 OR enable_serial = 1 );
------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0028', 'tecs',now(),'alter table vm_pool add column enable_serial');
------------------------------------------------------------------------------------