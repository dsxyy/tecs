------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：lixch
--修改时间：2013.3.28
--修改补丁号：0025
--修改内容说明：vm_pool表修改检查规则
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

ALTER TABLE vm_pool DROP  CONSTRAINT ck_37;
ALTER TABLE vm_pool ADD CONSTRAINT ck_37 CHECK ( last_op >= 1 AND last_op <= 26);

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0025', 'lixch',now(),'modify vm_pool last_op rangth');
------------------------------------------------------------------------------------