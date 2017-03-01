------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人:王明辉
--修改时间：2014.3.11
--修改补丁号：0064
--修改说明：告警表字段改名并新增字段
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
ALTER TABLE current_alarm RENAME COLUMN tc_name TO level1;
ALTER TABLE current_alarm RENAME COLUMN cc_name TO level2;
ALTER TABLE current_alarm RENAME COLUMN hc_name TO level3;

ALTER TABLE current_alarm ADD COLUMN subsystem VARCHAR(64);

ALTER TABLE history_alarm RENAME COLUMN tc_name TO level1;
ALTER TABLE history_alarm RENAME COLUMN cc_name TO level2;
ALTER TABLE history_alarm RENAME COLUMN hc_name TO level3;

ALTER TABLE history_alarm ADD COLUMN subsystem VARCHAR(64);

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0064', 'wangmh', now(), 'change tc_name/cc_name/hc_name to level1/level2/level3 and add subsystem in table current_alarm/history_alarm');