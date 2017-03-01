------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：张文剑
--修改时间：2013.2.21
--修改补丁号：0020
--修改内容说明：增加工作流相关的表和视图
------------------------------------------------------------------------------------
CREATE TABLE workflow ( 
	workflow_id          varchar( 64 ) NOT NULL,
	rollback_workflow_id varchar( 64 ) NOT NULL,
	category             varchar( 256 ) NOT NULL,
	name                 varchar( 256 ) NOT NULL,
	application          varchar( 64 ) NOT NULL,
	process              varchar( 64 ) NOT NULL,
	engine               int4 DEFAULT 0 NOT NULL,
	paused               int4 DEFAULT 0 NOT NULL,
	create_time          varchar( 32 ) NOT NULL,
	pause_time           varchar( 32 ),
	label_int64_1        numeric( 19 ),
	label_int64_2        numeric( 19 ),
	label_int64_3        numeric( 19 ),
	label_string_1       varchar( 4096 ),
	label_string_2       varchar( 4096 ),
	label_string_3       varchar( 4096 ),
	description          varchar( 1024 ),
	CONSTRAINT pk_workflow_0 PRIMARY KEY ( workflow_id )
 );

ALTER TABLE workflow ADD CONSTRAINT ck_9 CHECK ( (paused = 1) OR (paused = 0) );

ALTER TABLE workflow ADD CONSTRAINT ck_8 CHECK ( engine >= 0 );

CREATE INDEX idx_workflow ON workflow ( rollback_workflow_id );

CREATE TABLE action_pool ( 
	action_id            varchar( 64 ) NOT NULL,
	workflow_id          varchar( 64 ) NOT NULL,
	rollback_action_id   varchar( 64 ),
	category             varchar( 256 ) NOT NULL,
	name                 varchar( 256 ) NOT NULL,
	sender               varchar( 512 ) NOT NULL,
	receiver             varchar( 512 ) NOT NULL,
	message_id           int4 NOT NULL,
	message_priority     int4 DEFAULT 0 NOT NULL,
	message_version      int4 DEFAULT 0 NOT NULL,
	message_req          text,
	message_ack_id       int4 DEFAULT 0 NOT NULL,
	message_ack_priority int4 DEFAULT 0 NOT NULL,
	message_ack_version  int4 DEFAULT 0 NOT NULL,
	message_ack          text,
	success_feedback     int4 DEFAULT 1 NOT NULL,
	failure_feedback     int4 DEFAULT 1 NOT NULL,
	timeout_feedback     int4 DEFAULT 0 NOT NULL,
	state                int4 DEFAULT 1 NOT NULL,
	progress             int4 DEFAULT 0 NOT NULL,
	progress_weight      int4 DEFAULT 1 NOT NULL,
	timeout              int4 DEFAULT 0 NOT NULL,
	history_repeat_count int4 DEFAULT 0 NOT NULL,
	skipped              int4 DEFAULT 0 NOT NULL,
	finished             int4 DEFAULT 0 NOT NULL,
	finish_time          varchar( 32 ),
	last_repeat_time     varchar( 32 ),
	create_time          varchar( 32 ) NOT NULL,
	label_int64_1        numeric( 19 ),
	label_int64_2        numeric( 19 ),
	label_int64_3        numeric( 19 ),
	label_string_1       varchar( 4096 ),
	label_string_2       varchar( 4096 ),
	label_string_3       varchar( 4096 ),
	description          varchar( 1024 ),
	CONSTRAINT pk_action_pool PRIMARY KEY ( action_id ),
	CONSTRAINT fk_action_pool FOREIGN KEY ( workflow_id ) REFERENCES workflow( workflow_id ) ON DELETE CASCADE 
 );

ALTER TABLE action_pool ADD CONSTRAINT ck_6 CHECK ( (timeout_feedback = 0) OR (timeout_feedback = 1) );

ALTER TABLE action_pool ADD CONSTRAINT ck_5 CHECK ( (failure_feedback = 0) OR (failure_feedback = 1) );

ALTER TABLE action_pool ADD CONSTRAINT ck_4 CHECK ( (success_feedback = 0) OR (success_feedback = 1) );

ALTER TABLE action_pool ADD CONSTRAINT ck_3 CHECK ( message_id > 0 );

ALTER TABLE action_pool ADD CONSTRAINT ck_2 CHECK ( (finished = 1) OR (finished = 0) );

ALTER TABLE action_pool ADD CONSTRAINT ck_1 CHECK ( (skipped = 1) OR (skipped = 0) );

ALTER TABLE action_pool ADD CONSTRAINT ck_0 CHECK ( (progress >= 0) AND (progress <= 100) );

ALTER TABLE action_pool ADD CONSTRAINT ck_7 CHECK ( progress_weight > 0 );

CREATE INDEX idx_action_pool ON action_pool ( rollback_action_id );

CREATE TABLE action_sequences ( 
	action_prev          varchar( 64 ) NOT NULL,
	action_next          varchar( 64 ) NOT NULL,
	CONSTRAINT idx_action_sequences PRIMARY KEY ( action_prev, action_next ),
	CONSTRAINT fk_action_sequences_0 FOREIGN KEY ( action_prev ) REFERENCES action_pool( action_id ) ON DELETE CASCADE ,
	CONSTRAINT fk_action_sequences_1 FOREIGN KEY ( action_next ) REFERENCES action_pool( action_id ) ON DELETE CASCADE 
 );

ALTER TABLE action_sequences ADD CONSTRAINT ck_10 CHECK ( (action_next)::text <> (action_prev)::text );

 ------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0020', 'tecs',now(),'create workflow tables and views');
------------------------------------------------------------------------------------

