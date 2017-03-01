------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：龚协丰170393	
--修改时间：2012.8.29
--修改补丁号：0002
--修改内容说明：删除host_port表字段，修改host_trunk表约束
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

ALTER TABLE host_port DROP COLUMN delay_up_time ;
ALTER TABLE host_port DROP COLUMN work_speed ;
ALTER TABLE host_port DROP COLUMN work_duplex ;

ALTER TABLE host_trunk DROP CONSTRAINT ck_32;
ALTER TABLE host_trunk ADD CONSTRAINT ck_32 CHECK ( type = 1 OR type = 0 OR type = 4 );
------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0006', 'tecs',now(),'change host_port andhost_trunk');
------------------------------------------------------------------------------------