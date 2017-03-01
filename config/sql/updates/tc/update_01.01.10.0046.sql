------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：chenww
--修改时间：2013.7.16
--修改补丁号：0046
--修改说明：vmcfg_pool、vmtemplate_pool增加hypervisor的字段
------------------------------------------------------------------------------------

ALTER TABLE vmcfg_pool ADD COLUMN hypervisor character varying(32);
ALTER TABLE vmtemplate_pool ADD COLUMN hypervisor character varying(32);
UPDATE vmcfg_pool set hypervisor = 'xen';
UPDATE vmtemplate_pool set hypervisor = 'xen';
ALTER TABLE vmcfg_pool ADD CONSTRAINT ck_hypervisor CHECK  (hypervisor::text = 'xen'::text OR hypervisor::text = 'kvm'::text OR hypervisor::text = 'any'::text);
ALTER TABLE vmtemplate_pool ADD CONSTRAINT ck_hypervisor CHECK  (hypervisor::text = 'xen'::text OR hypervisor::text = 'kvm'::text OR hypervisor::text = 'any'::text);

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0000', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0046', 'chenww', now(), 'add hypervisor to vmcfg_pool and vmtemplate_pool view_vmcfg_with_project');