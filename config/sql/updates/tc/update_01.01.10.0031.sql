------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人:张文剑
--修改时间：2013.3.29
--修改补丁号：0031
--修改内容说明：增加工作流历史记录表和相关视图
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
CREATE TABLE workflow_history ( 
	workflow_id          varchar( 64 ) NOT NULL,
	rollback_workflow_id varchar( 64 ),
	category             varchar( 256 ) NOT NULL,
	name                 varchar( 256 ) NOT NULL,
	application          varchar( 64 ) NOT NULL,
	process              varchar( 64 ) NOT NULL,
	state                int4 DEFAULT 0,
	detail               varchar( 4096 ),
	failed_action_category varchar( 256 ),
	failed_action_name   varchar( 256 ),
	create_time          varchar( 32 ) NOT NULL,
	delete_time          varchar( 32 ) NOT NULL,
	description          varchar( 1024 ),
	CONSTRAINT pk_workflow_history PRIMARY KEY ( workflow_id )
 );

CREATE TABLE workflow_labels_history ( 
	workflow_id          varchar( 64 ) NOT NULL,
	label_int64_1        numeric( 19 ),
	label_int64_2        numeric( 19 ),
	label_int64_3        numeric( 19 ),
	label_int64_4        numeric( 19 ),
	label_int64_5        numeric( 19 ),
	label_int64_6        numeric( 19 ),
	label_int64_7        numeric( 19 ),
	label_int64_8        numeric( 19 ),
	label_int64_9        numeric( 19 ),
	label_string_1       varchar( 4096 ),
	label_string_2       varchar( 4096 ),
	label_string_3       varchar( 4096 ),
	label_string_4       varchar( 4096 ),
	label_string_5       varchar( 4096 ),
	label_string_6       varchar( 4096 ),
	label_string_7       varchar( 4096 ),
	label_string_8       varchar( 4096 ),
	label_string_9       varchar( 4096 ),
	CONSTRAINT idx_workflow_labels_history PRIMARY KEY ( workflow_id ),
	CONSTRAINT fk_workflow_labels_history_workflow_history FOREIGN KEY ( workflow_id ) REFERENCES workflow_history( workflow_id ) ON DELETE CASCADE 
 );
 
------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0031', 'tecs', now(), 'add workflow history tables and views');
