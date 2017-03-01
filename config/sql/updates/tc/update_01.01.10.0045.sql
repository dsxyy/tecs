------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：chenww
--修改时间：2013.6.18
--修改补丁号：0045
--修改说明：增加view_vmtemplate_image的字段
------------------------------------------------------------------------------------

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0000', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0045', 'chenww', now(), 'add view_vmtemplate_image disk_size disk_format');