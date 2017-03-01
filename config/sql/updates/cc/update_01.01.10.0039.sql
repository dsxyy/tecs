------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人:yanwei
--修改时间：2013.6.21
--修改补丁号：0039
--修改说明：vm_image增加字段reserved_backup,增加vm_image_backup表
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

ALTER TABLE vm_image ADD COLUMN reserved_backup int4 DEFAULT 0;

CREATE TABLE vm_image_backup ( 
    request_id            varchar( 64 ) NOT NULL,
    vid                   numeric( 19 ) NOT NULL,
    target                varchar( 16 ) NOT NULL,
    state                 int4,
    parent_request_id     varchar( 64 ) NOT NULL,
    create_time           varchar( 32 ) NOT NULL,
    description           varchar( 512 ) ,
    CONSTRAINT pk_vm_image_backup PRIMARY KEY ( request_id )
 );

ALTER TABLE vm_image_backup ADD CONSTRAINT ck_1 CHECK ( state = 0 OR state = 1 );
ALTER TABLE vm_image_backup ADD CONSTRAINT fk_vm_image_backup FOREIGN KEY ( vid , target ) REFERENCES vm_image( vid , target ) ON DELETE RESTRICT;

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0039', 'yanwei', now(), 'add column reserved_backup to vm_image and create new table vm_image_backup');
