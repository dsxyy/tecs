------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人:liuyi
--修改时间：2013.4.9
--修改补丁号：0035

------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
--DROP TABLE vm_nic_cancel
--DROP TABLE vm_nic
--DROP TABLE d_dhcp_range;
--DROP TABLE vlan_pool;
--DROP TABLE vlan_range;
 
--DROP TABLE netplane_map_default;
--DROP TABLE trunk_port;
--DROP TABLE host_trunk;
--DROP TABLE host_port;
--DROP TABLE netplane;

------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0035', 'tecs', now(), 'delete  ');