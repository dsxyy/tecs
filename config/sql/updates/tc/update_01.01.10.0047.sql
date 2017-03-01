------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：yanwei
--修改时间：2013.7.11
--修改补丁号：0047
--修改说明：修改vmcfg_image和image_pool的字段
------------------------------------------------------------------------------------

ALTER TABLE vmcfg_image ALTER COLUMN image_id DROP NOT NULL;
ALTER TABLE image_pool ALTER COLUMN name TYPE VARCHAR( 256 );
ALTER TABLE image_pool ALTER COLUMN name SET NOT NULL;

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0000', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0047', 'yanwei', now(), 'alter column of vmcfg_image and image_pool');