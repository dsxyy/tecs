------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：姚远
--修改时间：2012.11.20
--修改补丁号：0016
--修改内容说明：硬盘位置不再支持“默认”，原有的“默认”都改为“本地”
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--


------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0015', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
update vmcfg_image set position = 1 where position = 0;
update vmcfg_disk set position = 1 where position = 0;

INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0016', 'yaoyuan',now(), 'change position=0 to position=1.');
