------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人:冯骏
--修改时间：2013.3.13
--修改补丁号：0025
--修改内容说明：修改storage_vg，storage_address表对storage_adaptor表为级联删除
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
ALTER TABLE storage_vg DROP CONSTRAINT fk_storage_vg;
ALTER TABLE storage_vg ADD CONSTRAINT fk_storage_vg FOREIGN KEY (sid) REFERENCES storage_adaptor (sid) MATCH SIMPLE  ON UPDATE NO ACTION ON DELETE CASCADE;
ALTER TABLE storage_address DROP CONSTRAINT fk_storage_address;
ALTER TABLE storage_address ADD CONSTRAINT fk_storage_address FOREIGN KEY (sid) REFERENCES storage_adaptor (sid) MATCH SIMPLE  ON UPDATE NO ACTION ON DELETE CASCADE;

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0026', 'tecs', now(), 'change fk_storage_vg and  fk_storage_address ');