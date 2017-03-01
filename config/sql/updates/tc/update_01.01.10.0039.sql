------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：duwch 
--修改时间：2013.5.13
--修改补丁号：0039
--修改说明：image_pool表增加字段disk_size,在视图view_vmcfg_image,image_all_info,web_view_image 中增加os_type字段
------------------------------------------------------------------------------------
ALTER TABLE image_pool ADD COLUMN  disk_size numeric(19,0) NOT NULL DEFAULT 0;
UPDATE image_pool SET disk_size = size, disk_format = 'raw' WHERE disk_size = 0;
------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0000', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0039', 'duwch', now(),'add disk_size in table image_pool ');