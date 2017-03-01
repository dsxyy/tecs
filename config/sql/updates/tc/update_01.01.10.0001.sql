------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：张文剑
--修改时间：2012.5.28
--修改补丁号：0000
--修改内容说明：本文件是数据库schema升级脚本的示例范本
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

ALTER TABLE vmcfg_pool ALTER COLUMN project_id DROP NOT NULL;

ALTER TABLE vmcfg_pool DROP CONSTRAINT fk_vmcfg_pool_project_pool;

ALTER TABLE vmcfg_pool ADD CONSTRAINT fk_vmcfg_pool_project_pool_1 FOREIGN KEY (project_id)
      REFERENCES project_pool (oid) MATCH SIMPLE ON UPDATE NO ACTION ON DELETE SET NULL;

ALTER TABLE deleted_vmcfg_pool ALTER COLUMN project_id DROP NOT NULL;


--该位置为版本升级版本号，一定要填写：

INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0001', 'tecs',now(),'modify table: vmcfg_pool, deleted_vmcfg_pool');

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0001', 'tecs',now(),'first version');
------------------------------------------------------------------------------------