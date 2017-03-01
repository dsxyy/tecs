﻿------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：颜伟
--修改时间：2013.1.28
--修改补丁号：0019
--修改内容说明：host_pool表增加media_address字段
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

ALTER TABLE host_pool ADD COLUMN media_address varchar( 64 );

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0019', 'tecs',now(),'alter table host_pool add column media_address');
------------------------------------------------------------------------------------