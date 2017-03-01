------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人:颜伟
--修改时间：2013.8.21
--修改补丁号：0052
--修改说明：给工作流历史记录表增加上游sender信息字段
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
ALTER TABLE cluster_pool ADD COLUMN share_img_usage character varying(64);

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0052', 'yw', now(), 'add share_img_usage to cluster_pool table');
