------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：刘毅144824
--修改时间：2012.10.31
--修改补丁号：0010
--修改内容说明：host_port表增加loopback_mode字段
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

ALTER TABLE host_port ADD COLUMN loopback_mode INTEGER;
ALTER TABLE host_port ADD CONSTRAINT ck_55 CHECK ( loopback_mode = 0 OR loopback_mode = 1 );
------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0012', 'tecs',now(),'add  loopback_mode to host_port');
------------------------------------------------------------------------------------