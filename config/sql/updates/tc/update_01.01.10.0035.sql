------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人: 张文剑
--修改时间：2013.4.25
--修改补丁号：0035
--修改说明：将action的反馈标识放宽限制只要是大于等于0均可以
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
ALTER TABLE action_pool DROP CONSTRAINT  ck_4;
ALTER TABLE action_pool DROP CONSTRAINT  ck_5;
ALTER TABLE action_pool DROP CONSTRAINT  ck_6;
ALTER TABLE action_pool ADD CONSTRAINT ck_4 CHECK (success_feedback >= 0);
ALTER TABLE action_pool ADD CONSTRAINT ck_5 CHECK (failure_feedback >= 0);
ALTER TABLE action_pool ADD CONSTRAINT ck_6 CHECK (timeout_feedback >= 0);

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0035', 'tecs', now(), 'modify CONSTRAINT of action feedback flag');