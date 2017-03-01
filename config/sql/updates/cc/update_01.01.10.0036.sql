------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人:颜伟
--修改时间：2013.5.21
--修改补丁号：0036
--修改说明：vm_image增加字段request_id,access_url
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
ALTER TABLE vm_image ADD COLUMN request_id VARCHAR(64);
ALTER TABLE vm_image ADD COLUMN access_url VARCHAR(1024);

INSERT INTO vm_image SELECT vid, v.target, image_id, 0, 2, v.request_id, disk_url FROM vm_disk_share v, image_cache i WHERE v.target = 'hda';
DELETE FROM vm_disk WHERE target = 'hda';

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0036', 'yanwei', now(), 'add column request_id,access_url to vm_image');
