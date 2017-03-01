------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人:张文剑
--修改时间：2013.3.29
--修改补丁号：0027
--修改内容说明：增加uuid历史记录表
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

CREATE TABLE public.uuid_history ( 
	uuid                 varchar( 64 ) NOT NULL,
	delete_time          varchar( 32 ) NOT NULL,
	CONSTRAINT uuid_history_pkey PRIMARY KEY ( uuid )
 );
 
------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0027', 'tecs', now(), 'add uuid_history table');
