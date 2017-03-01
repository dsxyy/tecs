------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人:张文剑
--修改时间：2013.4.15
--修改补丁号：0034
--增加工作流历史进度条字段
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
ALTER TABLE workflow_history ADD COLUMN progress integer NOT NULL DEFAULT 0;
ALTER TABLE workflow_history ADD CONSTRAINT ck_0 CHECK (progress >= 0 AND progress <= 100);

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0034', 'tecs', now(), 'add workflow history progress');