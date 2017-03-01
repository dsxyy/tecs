------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：颜伟155391
--修改时间：2013.1.7
--修改补丁号：0017
--修改内容说明：删除image_cache_snapshot表,删除image_cache表中的check_point字段
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

DROP TABLE image_cache_snapshot;
ALTER TABLE image_cache DROP COLUMN check_point;
ALTER TABLE vm_disk_share ADD COLUMN cache_id numeric( 19 ) NOT NULL;
ALTER TABLE vm_disk_share ADD COLUMN state int4 NOT NULL;
ALTER TABLE vm_disk_share ADD COLUMN detail varchar( 256 ) NOT NULL;
ALTER TABLE vm_disk_share ADD COLUMN last_op int4 NOT NULL;



------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0017', 'tecs',now(),'alter table image_cache drop column check_point and drop table image_cache_snapshot');
------------------------------------------------------------------------------------