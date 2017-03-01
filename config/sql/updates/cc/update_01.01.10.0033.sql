------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人:颜伟
--修改时间：2013.5.7
--修改补丁号：0033
--修改说明：vm_disk增加字段request_id,access_url和is_need_release
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
ALTER TABLE vm_disk ADD COLUMN request_id VARCHAR(64);
ALTER TABLE vm_disk ADD COLUMN access_url VARCHAR(1024);
ALTER TABLE vm_disk ADD COLUMN is_need_release integer NOT NULL DEFAULT 0;

INSERT INTO vm_disk SELECT vid, target, 0, 2, request_id, disk_url, 1 FROM vm_disk_share;
UPDATE vm_disk SET is_need_release = 1 WHERE position = 2;

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0033', 'yanwei', now(), 'add column request_id,access_url and is_need_release to vm_disk');

