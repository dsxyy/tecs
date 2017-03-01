------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人: xiett
--修改时间：2013.7.25
--修改补丁号：0046
--修改说明：创建表 host_disks
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
CREATE TABLE host_disks ( 
	hid                  numeric(19,0) NOT NULL,
	name                 varchar( 128 ) NOT NULL,
	type                 int4 NOT NULL,
	status               int4 NOT NULL,
	CONSTRAINT pk_host_disks PRIMARY KEY ( hid, name ),
	CONSTRAINT fk_host_disks FOREIGN KEY (hid)
      REFERENCES host_pool (oid) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE CASCADE
 );

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0046', 'xiett', now(), 'Create one new table host_disks');