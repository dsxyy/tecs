------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人:刘毅
--修改时间：2013.4.15
--修改补丁号：0034
--cmm节能降耗功能
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

CREATE TABLE host_atca ( 
	hid          numeric( 19 ) NOT NULL,
        bureau       int4 NOT NULL,
        rack         int4 NOT NULL,
        shelf        int4 NOT NULL,
        slot         int4 NOT NULL,
	board_type   varchar( 32 ) NOT NULL,
	power_state int4 DEFAULT 0 NOT NULL,
	power_operate int4 DEFAULT 0 NOT NULL,
	rate_power int4 DEFAULT 0 NOT NULL,
	poweroff_time numeric( 19 )  DEFAULT 0 NOT NULL,
	total_poweroff_time numeric( 19 ) DEFAULT 0 NOT NULL,
	description   varchar( 512 ),	
	CONSTRAINT pk_host_atca PRIMARY KEY ( hid )
 );
ALTER TABLE host_atca ADD CONSTRAINT fk_atca_host_pool FOREIGN KEY ( hid ) REFERENCES host_pool( oid ) ON DELETE CASCADE;
------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0034', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0034', 'tecs', now(), 'add host_atca table');