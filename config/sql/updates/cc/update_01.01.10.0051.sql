------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：yanwei
--修改时间：2013.9.29
--修改补丁号：0051
--修改内容说明：image_base/vm_disk/vm_image的access_url字段长度改为2048
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

ALTER TABLE image_base ALTER COLUMN access_url TYPE character varying(2048);
ALTER TABLE vm_disk ALTER COLUMN access_url TYPE character varying(2048);
ALTER TABLE vm_image ALTER COLUMN access_url TYPE character varying(2048);

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0051', 'yanwei',now(),'alter access_url of image_base/vm_disk/vm_image to character varying(2048)');
------------------------------------------------------------------------------------