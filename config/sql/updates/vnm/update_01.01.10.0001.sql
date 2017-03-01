﻿------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：张文剑
--修改时间：2012.5.28
--修改补丁号：0000
--修改内容说明：本文件是数据库schema升级脚本的示例范本
------------------------------------------------------------------------------------
--请在此加入vnm表结构, 存贮过程修改的语句，视图的修改请直接编辑tc/cc/vnm_view.sql：
--

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0000', 'tecs',now(),'first version');
------------------------------------------------------------------------------------

CREATE TABLE  "_vnm_test" ( 
	version_number       varchar( 64 ) DEFAULT 0 NOT NULL,
	update_by            varchar( 128 ),
	update_at            varchar( 32 ),
	description          varchar( 512 ),
	CONSTRAINT pk__vnm_testlog PRIMARY KEY ( version_number )
 );
 
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
  VALUES ('01.01.10.0001', 'tecs-vnm_test',now(),'first version');



