------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人:陈文文
--修改时间：2013.5.20
--修改补丁号：0037
--修改内容说明：删除表vmcfg_vsi、vmcfg_mac、视图view_vmcfg_network，更新表vmcfg_nic、vmtemplate_nic
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

-- ---------------------------------------对接网络新架构的表设计从下面开始  -----------------------------------------
--首先创建两个临时表专门用于存放nic信息
CREATE TABLE vmcfg_network ( 
  vid        numeric(19,0) NOT NULL,
  nic_index  integer NOT NULL,
  sriov      integer NOT NULL,
  loop       integer NOT NULL,
  logic_network_id         character varying(128) NOT NULL,  
  model      character varying(32) NOT NULL,
  vsi_profile_id           character varying(128),
  CONSTRAINT idxx_vmcfg_nic PRIMARY KEY (vid, nic_index),
  CONSTRAINT fkey_vmcfg_nic_vmcfg_pool FOREIGN KEY (vid)
      REFERENCES vmcfg_pool (oid) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE CASCADE,
  CONSTRAINT ck_1 CHECK (sriov = 0 OR sriov = 1),
  CONSTRAINT ck_2 CHECK (loop = 0 OR loop  = 1),
  CONSTRAINT ck_3 CHECK (model::text = 'e1000'::text OR model::text = 'rtl8139'::text OR model::text = 'ne2k_pci'::text OR model::text = 'pcnet'::text OR model::text = 'netfront'::text OR model::text = ''::text)
 );
 
 CREATE TABLE vmtemplate_network ( 
  vmtemplate_id        numeric(19,0) NOT NULL,
  nic_index  integer NOT NULL,
  sriov      integer NOT NULL,
  loop       integer NOT NULL,
  logic_network_id         character varying(128) NOT NULL,  
  model      character varying(32) NOT NULL,
  CONSTRAINT idxx_vmtemplatenic PRIMARY KEY (vmtemplate_id, nic_index),
  CONSTRAINT fkey_vmtemplate_nic_vmtemplate_pool FOREIGN KEY (vmtemplate_id)
      REFERENCES vmtemplate_pool (oid) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE CASCADE,
  CONSTRAINT ck_1 CHECK (sriov = 0 OR sriov = 1),
  CONSTRAINT ck_2 CHECK (loop = 0 OR loop  = 1),
  CONSTRAINT ck_3 CHECK (model::text = 'e1000'::text OR model::text = 'rtl8139'::text OR model::text = 'ne2k_pci'::text OR model::text = 'pcnet'::text OR model::text = 'netfront'::text OR model::text = ''::text)
 );

 --将已有表中的labels导入到新表中
INSERT INTO vmcfg_network      SELECT vid,  nic_index, is_sriov as sriov, 0, model, '', '' FROM vmcfg_nic;
INSERT INTO vmtemplate_network SELECT vmtemplate_id, nic_index, is_sriov as sriov, 0, model, ''  FROM vmtemplate_nic;

--完成网络数据导入到网络模块的表中后，才能执行下面的操作

DROP  TABLE vmcfg_vsi; 
DROP  TABLE vmcfg_mac; 
DROP  TABLE vmcfg_nic;
DROP  TABLE vmtemplate_nic;
DROP  TABLE cluster_netplane;


--改表的名称
ALTER TABLE vmcfg_network       RENAME TO vmcfg_nic;
ALTER TABLE vmtemplate_network  RENAME TO  vmtemplate_nic;


------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0038', 'tecs', now(), 'del table vmcfg_vsi、vmcfg_mac、view_vmcfg_network，update vmcfg_nic、vmtemplate_nic ');