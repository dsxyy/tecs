------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：姚远147328
--修改时间：2012.10.22
--修改补丁号：0007
--修改内容说明：为v4提供tdm功能
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
CREATE TABLE host_device ( 
	hid                  numeric( 19 ) NOT NULL,
	type                 int4 NOT NULL,
	"number"             int4 NOT NULL,
	description          varchar( 512 ),
	CONSTRAINT idx_host_device PRIMARY KEY ( hid, type )
 );

ALTER TABLE host_device ADD CONSTRAINT ck_7 CHECK ( type = 1 );

ALTER TABLE host_device ADD CONSTRAINT ck_8 CHECK ( number > 0 );

CREATE INDEX idx_host_device_0 ON host_device ( hid );

CREATE TABLE vm_device ( 
	vid                  numeric( 19 ) NOT NULL,
	type                 int4 NOT NULL,
	count                int4 NOT NULL,
	CONSTRAINT idx_vm_device PRIMARY KEY ( vid, type )
 );

ALTER TABLE vm_device ADD CONSTRAINT ck_9 CHECK ( type = 1 );

ALTER TABLE vm_device ADD CONSTRAINT ck_10 CHECK ( count > 0 );

CREATE INDEX idx_vm_device_0 ON vm_device ( vid );

CREATE TABLE vm_device_cancel ( 
	vid                  numeric( 19 ) NOT NULL,
	hid                  numeric( 19 ) NOT NULL,
	type                 int4 NOT NULL,
	count                int4 NOT NULL,
	CONSTRAINT idx_vm_device_cancel PRIMARY KEY ( vid, type, hid )
 );

ALTER TABLE vm_device_cancel ADD CONSTRAINT ck_11 CHECK ( type = 1 );

ALTER TABLE vm_device_cancel ADD CONSTRAINT ck_12 CHECK ( count > 0 );

CREATE INDEX idx_vm_device_cancel_0 ON vm_device_cancel ( vid );

ALTER TABLE host_device ADD CONSTRAINT fk_host_device_host_pool FOREIGN KEY ( hid ) REFERENCES host_pool( oid ) ON DELETE CASCADE;

ALTER TABLE vm_device ADD CONSTRAINT fk_vm_device_vm_pool FOREIGN KEY ( vid ) REFERENCES vm_pool( vid ) ON DELETE CASCADE;

ALTER TABLE vm_device_cancel ADD CONSTRAINT fk_vm_device_cancel FOREIGN KEY ( vid, hid ) REFERENCES vm_cancel_pool( vid, hid ) ON DELETE CASCADE;


------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0007', 'tecs',now(),'add tdm for v4');
------------------------------------------------------------------------------------