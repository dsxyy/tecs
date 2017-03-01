------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：李孝成
--修改时间：2013.2.5
--修改补丁号：0024
--修改内容说明：增加vmcfg_pool 表中一个enable_coredump的选项
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
ALTER TABLE vmcfg_pool ADD COLUMN enable_coredump integer DEFAULT 0 CHECK (enable_coredump =0 or enable_coredump =1);
ALTER TABLE deleted_vmcfg_pool ADD COLUMN enable_coredump integer DEFAULT 0 CHECK (enable_coredump =0 or enable_coredump =1);
ALTER TABLE vmtemplate_pool ADD COLUMN enable_coredump integer DEFAULT 0 CHECK (enable_coredump =0 or enable_coredump =1);

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0015', 'tecs',now(),'first version');
------------------------------------------------------------------------------------

INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0024', 'lixch',now(), 'add vmcfg_pool,deleted_vmcfg_pool,vmtempalte_pool  item enable_coredump.');
