﻿------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人:zhoubin
--修改时间：2014.3.4
--修改补丁号：0063
--修改说明：新增mac字段
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
ALTER TABLE vmcfg_nic ADD COLUMN mac character varying(64);
------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0063', 'zhoubin', now(), 'add mac to table vmcfg_nic');