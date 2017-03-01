------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人:姚远
--修改时间：2013.4.25
--修改补丁号：0032
--增加字段，保存对虚拟机的预期状态
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
ALTER TABLE vm_pool ADD COLUMN expected_state integer NOT NULL DEFAULT 2;
ALTER TABLE vm_pool ADD CONSTRAINT ck_57 CHECK (expected_state >= 2 AND expected_state <= 4);

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0032', 'tecs', now(), 'add expected state');