------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人:颜伟
--修改时间：2013.1.9
--修改补丁号：0021
--修改内容说明：删除storage_authorize_lun外键约束,删除storage_lun的lun_uuid和storage_authorize的auth_uuid的非空和唯一约束
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

ALTER TABLE storage_authorize_lun DROP CONSTRAINT fk_storage_authorize_auth_uuid;

ALTER TABLE storage_authorize_lun DROP CONSTRAINT fk_storage_authorize_lun_uuid;

ALTER TABLE storage_lun ALTER COLUMN lun_uuid DROP NOT NULL;

ALTER TABLE storage_lun DROP CONSTRAINT un_storage_lun;

ALTER TABLE storage_authorize ALTER COLUMN auth_uuid DROP NOT NULL;

ALTER TABLE storage_authorize DROP CONSTRAINT un_storage_authorize;

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0021', 'tecs', now(), 'drop storage_authorize_lun foreign key,drop storage_lun and storage_authorize not null and unique');