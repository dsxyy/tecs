﻿------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人:yanwei
--修改时间：2013.6.27
--修改补丁号：0040
--修改说明：cc_view modify 
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0040', 'yanwei', now(), 'modify cc_view.sql all_vm_local_images_in_host');
