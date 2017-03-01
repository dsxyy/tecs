------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：王明辉
--修改时间：2013.1.6
--修改补丁号：0020
--修改内容说明：增加用户与模板之间的关联删除
------------------------------------------------------------------------------------
ALTER TABLE vmtemplate_pool DROP CONSTRAINT fk_vmtemplate_pool_user_pool;
ALTER TABLE vmtemplate_pool ADD CONSTRAINT fk_vmtemplate_pool_user_pool_1 FOREIGN KEY ( uid ) REFERENCES user_pool( oid ) ON DELETE CASCADE;

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0000', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by,update_at,description) VALUES ('01.01.10.0020', 'wangmh', now(),'delete template when user deleted');