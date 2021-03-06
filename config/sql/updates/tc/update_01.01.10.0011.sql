﻿------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：李春 10112629
--修改时间：2012.10.19
--修改补丁号：0011
--修改内容说明：本文件是数据库schema升级脚本的示例范本
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！

ALTER TABLE image_pool DROP COLUMN oid;

INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0011', 'tecs',now(),'ALTER TABLE,drop COLUMN oid; and change _image_info_with_running_vms_and_refcount into image_all_info');
------------------------------------------------------------------------------------