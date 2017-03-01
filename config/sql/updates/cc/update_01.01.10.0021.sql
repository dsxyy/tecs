------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：张文剑
--修改时间：2013.3.19
--修改补丁号：0021
--修改内容说明：增加工作流labels
------------------------------------------------------------------------------------
--首先创建两个新表专门用于存放labels
CREATE TABLE workflow_labels ( 
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
	CONSTRAINT idx_workflow_labels PRIMARY KEY ( workflow_id ),
	CONSTRAINT fk_workflow_labels_workflow FOREIGN KEY ( workflow_id ) REFERENCES workflow( workflow_id ) ON DELETE CASCADE 
 );

CREATE TABLE action_labels ( 
	action_id            varchar( 64 ) NOT NULL,
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
	CONSTRAINT idx_action_labels PRIMARY KEY ( action_id ),
	CONSTRAINT fk_action_labels_action_pool FOREIGN KEY ( action_id ) REFERENCES action_pool( action_id ) ON DELETE CASCADE 
 );

--将已有表中的labels导入到新表中
INSERT INTO workflow_labels SELECT workflow_id,label_int64_1,label_int64_2,label_int64_3,-1,-1,-1,-1,-1,-1,label_string_1,label_string_2,label_string_3,'','','','','','' FROM workflow;
INSERT INTO action_labels SELECT action_id,label_int64_1,label_int64_2,label_int64_3,-1,-1,-1,-1,-1,-1,label_string_1,label_string_2,label_string_3,'','','','','','' FROM action_pool;

--将原有表中的labels字段删除
ALTER TABLE workflow DROP COLUMN label_int64_1;
ALTER TABLE workflow DROP COLUMN label_int64_2;
ALTER TABLE workflow DROP COLUMN label_int64_3;
ALTER TABLE workflow DROP COLUMN label_string_1;
ALTER TABLE workflow DROP COLUMN label_string_2;
ALTER TABLE workflow DROP COLUMN label_string_3;
ALTER TABLE action_pool DROP COLUMN label_int64_1;
ALTER TABLE action_pool DROP COLUMN label_int64_2;
ALTER TABLE action_pool DROP COLUMN label_int64_3;
ALTER TABLE action_pool DROP COLUMN label_string_1;
ALTER TABLE action_pool DROP COLUMN label_string_2;
ALTER TABLE action_pool DROP COLUMN label_string_3;

 ------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0021', 'tecs',now(),'add workflow and action labels');
------------------------------------------------------------------------------------

