------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：张业兵
--修改时间：2012.10.22
--修改补丁号：0013
--修改内容说明：增加版本管理升级日志表
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
CREATE TABLE upgrade_log ( 
	oid                  numeric( 19 ) NOT NULL,
	node_name            varchar( 256 ) NOT NULL,
	node_type            varchar( 32 ) NOT NULL,
	package              varchar( 256 ) NOT NULL,
	repo_version         varchar( 64 ),
	old_version          varchar( 64 ),
	begin_time           varchar( 64 ),
	end_time             varchar( 64 ),
	percent              int4,
	result               int4,
	error_string         varchar( 512 ),
	detail               varchar( 40960 ),
	CONSTRAINT pk_upgrade_log PRIMARY KEY ( oid )
 );

ALTER TABLE public.upgrade_log ADD CONSTRAINT ck_28 CHECK ( percent >=0 and percent <=100 );

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0013', 'tecs',now(),'add version upgrade_log table ');
------------------------------------------------------------------------------------