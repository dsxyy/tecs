------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人:张文剑
--修改时间：2013.3.25
--修改补丁号：0029
--修改内容说明：在工作流的action表中增加synchronous字段，
--在workflow中增加upstream_sender和upstream_action_id字段
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

ALTER TABLE action_pool ADD COLUMN  synchronous int4 DEFAULT 1 NOT NULL;
ALTER TABLE action_pool ADD CONSTRAINT ck_11 CHECK ( (synchronous= 1) OR (synchronous= 0) );
ALTER TABLE workflow ADD COLUMN  upstream_sender VARCHAR(512);
ALTER TABLE workflow ADD COLUMN  upstream_action_id VARCHAR(64);

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0029', 'tecs', now(), 'add synchronous in action_pool ');