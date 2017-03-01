
--*********************************************************************
-- 版权所有 (C)2012, 深圳市中兴通讯股份有限公司。
--
-- 文件名称： vnm.sql
-- 文件标识：
-- 内容摘要：
-- 其它说明：
-- 当前版本：
-- 作    者： zte
-- 完成日期： 2012-2-8
--
--    修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
--    修改日期：
--    版 本 号：
--    修 改 人：
--    修改内容：
-- 修改记录2：…
-- 修 改 人：
-- 修改日期：
-- 修改内容：
--*********************************************************************


-- 创建视图


-- port 获取port vna名称 
CREATE OR REPLACE VIEW v_net_vna_port AS  
SELECT 
  vna.id AS vna_id,
  vna.vna_uuid, 
  vna.vna_application, 
  vna.vnm_application AS vna_vnm, 
  vna.host_name AS vna_host_name, 
  vna.os_name AS vna_os_name, 
  vna.is_online AS vna_is_online, 
  port.id AS port_id,
  port.port_type, 
  port."name" AS port_name, 
  port.uuid AS port_uuid, 
  port.admin_state AS port_admin_state, 
  port.is_online AS port_is_online, 
  port.is_cfg AS port_is_cfg, 
  port.is_consistency AS port_is_consistency
FROM 
  port, 
  vna
WHERE 
  port.vna_id = vna.id;
  


CREATE OR REPLACE VIEW v_net_physical_port_vna AS 
SELECT 
  port."name" AS phy_name, 
  port.uuid AS phy_uuid, 
  port.id AS phy_id, 
  vna.vna_uuid AS vna_uuid, 
  vna.id AS vna_id, 
  physical_port.is_linked,
  port.is_online AS phy_is_online,
  port.is_consistency AS phy_is_consistency,
  port.is_cfg AS phy_is_cfg,
  port.admin_state AS phy_admin_state
FROM 
  port, 
  vna, 
  physical_port
WHERE 
  port.vna_id = vna.id AND
  physical_port.port_id = port.id AND
  physical_port.vna_id = port.vna_id;

  
  
-- sriov port view  
CREATE OR REPLACE VIEW v_net_sriov_port_vna AS   
SELECT 
  vna.vna_uuid, 
  vna.vna_application, 
  vna.host_name, 
  vna.vnm_application, 
  vna.is_online AS vna_is_online, 
  port.id, 
  port."name", 
  port.uuid, 
  port.is_online AS port_is_online,  
  port.is_consistency, 
  physical_port.is_linked, 
  sriov_port.is_loop_report, 
  sriov_port.is_loop_cfg,
  port.is_cfg AS port_is_cfg,
  port.admin_state AS port_admin_state
FROM 
  port, 
  sriov_port, 
  physical_port, 
  vna
WHERE 
  port.vna_id = vna.id AND
  physical_port.port_id = port.id AND
  physical_port.port_id = sriov_port.physical_port_id;  
  
  
-- sriov vf port view
CREATE OR REPLACE VIEW v_net_sriov_vf_port_vna AS   
SELECT 
  vna.vna_uuid, 
  vna.vna_application, 
  vna.host_name, 
  vna.vnm_application, 
  vna.is_online AS vna_is_online, 
  port.id, 
  port."name", 
  port.uuid, 
  port.is_online AS port_is_online,  
  port.is_consistency, 
  sriov_vf_port.sriov_port_id, 
  sriov_vf_port.pci, 
  sriov_vf_port.vlan_num,
  port.is_consistency AS port_is_consistency,
  port.is_cfg AS port_is_cfg,
  port.admin_state AS port_admin_state,
  port.state AS port_state
FROM 
  port, 
  vna, 
  sriov_vf_port
WHERE 
  port.vna_id = vna.id AND
  sriov_vf_port.port_id = port.id;
  
  
-- free sriov vf port view 
CREATE OR REPLACE VIEW v_net_free_sriov_vf_port_vna AS   
SELECT 
  distinct 
  v_net_sriov_port_vna.vna_uuid, 
  v_net_sriov_port_vna.vna_is_online AS vna_is_online, 
  v_net_sriov_port_vna."name" AS sriov_name, 
  v_net_sriov_port_vna.uuid AS sriov_uuid, 
  v_net_sriov_port_vna.id AS sriov_id, 
  v_net_sriov_port_vna.port_is_online AS sriov_is_online,
  v_net_sriov_port_vna.is_consistency AS sriov_is_consistency,
  v_net_sriov_port_vna.port_admin_state AS sriov_admin_state,
  v_net_sriov_vf_port_vna."name" AS vf_name, 
  v_net_sriov_vf_port_vna.uuid AS vf_uuid, 
  v_net_sriov_vf_port_vna.id AS vf_id, 
  v_net_sriov_vf_port_vna.pci, 
  v_net_sriov_vf_port_vna.vlan_num,
  v_net_sriov_vf_port_vna.port_is_online AS vf_is_online,
  v_net_sriov_vf_port_vna.port_is_consistency AS vf_is_consistency,
  v_net_sriov_vf_port_vna.port_is_cfg AS vf_is_cfg,
  v_net_sriov_vf_port_vna.port_admin_state AS vf_admin_state 
FROM 
  public.v_net_sriov_port_vna, 
  public.v_net_sriov_vf_port_vna
WHERE 
  v_net_sriov_vf_port_vna.sriov_port_id = v_net_sriov_port_vna.id 
  AND   v_net_sriov_vf_port_vna.id not in (SELECT sriov_vf_port_id FROM vsi_sriov );
  
  

-- vna 获取 hc cluster 名称 
CREATE OR REPLACE VIEW v_net_vna_hc_cluster AS  
SELECT 
  vna.id AS vna_id,
  vna.vna_uuid, 
  regist_module.module_name, 
  regist_module.module_uuid, 
  regist_module."role" AS module_role, 
  regist_module.is_online AS module_is_online,
  hc_info.cluster_name, 
  vna.vna_application, 
  vna.vnm_application, 
  vna.host_name, 
  vna.is_online AS vna_is_online,
  CASE 
     WHEN ( (SELECT count(*) FROM regist_module b WHERE vna.id  = b.vna_id AND b.role = 5 ) > 0 )
	     THEN (1)
	 ELSE (0)
  END AS is_has_watchdog,
  CASE 
     WHEN (SELECT id is not null FROM regist_module b WHERE vna.id  = b.vna_id AND b.role = 5 AND b.is_online = 1 ) 
	     THEN (SELECT is_online FROM regist_module b WHERE vna.id  = b.vna_id AND b.role = 5 AND b.is_online = 1)
	 ELSE (0)
  END AS watchdog_is_online
  
FROM 
  vna, 
  regist_module, 
  hc_info
WHERE 
  regist_module.vna_id = vna.id AND
  hc_info.regist_module_id = regist_module.id AND 
  regist_module.is_online =1;
  
  
  

CREATE OR REPLACE VIEW v_net_uplinkpg_trunk_vlanrange AS 
SELECT 
  port_group."name" AS uplinkpg_name, 
  port_group.uuid AS uplinkpg_uuid, 
  port_group.pg_type, 
  pg_switchport_trunk.native_vlan_num, 
  pg_switchport_trunk_vlan_range.vlan_begin, 
  pg_switchport_trunk_vlan_range.vlan_end, 
  port_group.id AS uplinkpg_id
FROM 
  public.uplink_pg, 
  public.port_group, 
  public.pg_switchport_trunk, 
  public.pg_switchport_trunk_vlan_range
WHERE 
  uplink_pg.pg_id = port_group.id AND
  port_group.id = pg_switchport_trunk.pg_id AND
  pg_switchport_trunk.pg_id = pg_switchport_trunk_vlan_range.pg_switchport_trunk_id;


--- netplane portgroup
CREATE OR REPLACE VIEW v_net_netplane_uplinkpg AS 
 SELECT netplane.name AS "NP_name", 
        netplane.mtu AS "NP_mtu", 
        port_group.mtu AS "PG_mtu", 
        port_group.name AS "PG_name"
   FROM netplane, uplink_pg, port_group
  WHERE netplane.id = port_group.netplane_id AND port_group.id = uplink_pg.pg_id AND port_group.pg_type = uplink_pg.pg_type;

  
CREATE OR REPLACE VIEW v_net_netplane_vmpg AS   
  SELECT 
  netplane."name" AS netplane_name, 
  netplane.uuid AS netplane_uuid, 
  port_group.uuid AS pg_uuid, 
  port_group."name" AS pg_name, 
  netplane.id AS netplane_id, 
  port_group.id AS pg_id
FROM 
  public.netplane, 
  public.port_group, 
  public.vm_pg
WHERE 
  netplane.id = port_group.netplane_id AND
  port_group.id = vm_pg.pg_id AND
  port_group.netplane_id is not null;

  
  
--6 logicnetwork_portgroup_netplane

CREATE OR REPLACE VIEW v_net_logicnetwork_portgroup_netplane AS 
SELECT 
  netplane."name" AS netplane_name, 
  netplane.uuid AS netplane_uuid, 
  netplane.mtu AS netplane_mtu, 
  port_group."name" AS pg_name, 
  port_group.uuid AS pg_uuid, 
  port_group.mtu AS pg_mtu, 
  logic_network."name" AS logicnetwork_name, 
  logic_network.uuid AS logicnetwork_uuid,
  logic_network.ip_bootprotocolmode AS logicnetwork_ipmode,
  logic_network.id AS logicnetwork_id 
FROM 
  public.port_group, 
  public.vm_pg, 
  public.netplane, 
  public.logic_network
WHERE 
  port_group.id = vm_pg.pg_id AND
  vm_pg.pg_id = logic_network.vm_pg_id AND
  netplane.id = port_group.netplane_id AND 
  port_group.netplane_id is not null;
  
-- 7 switch_port_vna

CREATE OR REPLACE VIEW v_net_switch_port_vna AS 
SELECT 
  vna.vna_uuid, 
  vna.vna_application, 
  vna.host_name, 
  vna.vnm_application, 
  vna.is_online,
  port.uuid AS port_uuid, 
  port."name" AS port_name, 
  port.port_type, 
  switch.switch_type, 
  switch."name" AS switch_name, 
  switch.uuid AS switch_uuid, 
  virtual_switch.is_active, 
  virtual_switch.evb_mode, 
  virtual_switch.uplink_pg_id, 
  virtual_switch.nic_max_num_cfg,
  switch.id AS switch_id,
  port.admin_state AS port_admin_state,
  port.is_online AS port_is_online,
  port.is_cfg AS port_is_cfg,
  port.is_consistency AS port_is_consistency,
  virtual_switch_map.is_cfg AS switch_map_is_cfg,
  virtual_switch_map.is_selected AS switch_map_is_selected,
  virtual_switch_map.is_consistency AS switch_map_is_consistency
FROM 
  public.port, 
  public.vna, 
  public.switch, 
  public.virtual_switch, 
  public.virtual_switch_map
WHERE 
  port.vna_id = vna.id AND
  switch.id = virtual_switch.switch_id AND
  virtual_switch.switch_id = virtual_switch_map.virtual_switch_id AND
  virtual_switch_map.port_id = port.id;
  
CREATE OR REPLACE VIEW v_net_vswitch AS   
  SELECT 
  switch.id,
  switch."name", 
  switch.uuid, 
  switch.switch_type, 
  switch.state, 
  switch.max_mtu, 
  virtual_switch.is_active, 
  virtual_switch.evb_mode, 
  virtual_switch.uplink_pg_id, 
  virtual_switch.nic_max_num_cfg
FROM 
  public.switch, 
  public.virtual_switch
WHERE 
  switch.id = virtual_switch.switch_id;
  
  
-- 8 switch_vsi
-- 9 vlanmap_netplane_pg_vlan


CREATE OR REPLACE VIEW v_net_vlanmap_netplane_pg_vlan AS 
SELECT 
  port_group."name" AS pg_name, 
  port_group.uuid AS pg_uuid, 
  vlan_map.project_id AS project_id, 
  vlan_pool.vlan_num AS vlan_num,   
  netplane."name" AS netplane_name, 
  netplane.uuid AS netplane_uuid,
  vlan_map.isolate_no AS isolate_no
FROM 
  public.vlan_map, 
  public.vlan_pool, 
  public.vm_pg, 
  public.netplane, 
  public.port_group
WHERE 
  vlan_map.vm_pg_id = vm_pg.pg_id AND
  vlan_map.vlan_id = vlan_pool.id AND
  vlan_pool.netplane_id = netplane.id AND
  vm_pg.pg_id = port_group.id AND
  port_group.netplane_id is not null;

-- 10 bond

CREATE OR REPLACE VIEW v_net_bond_vna AS 
SELECT 
  port."name" AS bond_name, 
  port.uuid AS bond_uuid, 
  port.id AS bond_id, 
  bond.bond_mode AS bond_mode, 
  vna.vna_uuid AS vna_uuid, 
  vna.id AS vna_id,
  vna.is_online AS vna_is_online,  
  port.is_online AS bond_is_online,
  port.is_consistency AS bond_is_consistency,
  port.is_cfg AS bond_is_cfg,
  port.admin_state AS bond_admin_state,
  port.state AS bond_state
FROM 
  public.bond, 
  public.port, 
  public.vna
WHERE 
  bond.port_id = port.id AND
  bond.vna_id = port.vna_id AND
  port.vna_id = vna.id;
  
  
-- 11 bond_map


CREATE OR REPLACE VIEW v_net_bondmap_bond_phy AS 
SELECT 
  v_net_bond_vna.bond_name, 
  v_net_bond_vna.bond_uuid, 
  v_net_bond_vna.bond_mode, 
  v_net_bond_vna.vna_uuid, 
  v_net_bond_vna.vna_is_online,  
  v_net_physical_port_vna.phy_name, 
  v_net_physical_port_vna.phy_uuid, 
  v_net_physical_port_vna.phy_is_online,
  v_net_physical_port_vna.phy_is_consistency,
  v_net_physical_port_vna.phy_admin_state,
  bond_map.is_cfg, 
  bond_map.is_selected, 
  bond_map.id AS bond_map_id,
  bond_map.lacp_is_success,
  bond_map.backup_is_active,
  v_net_physical_port_vna.is_linked
FROM 
  public.bond_map, 
  public.v_net_bond_vna, 
  public.v_net_physical_port_vna
WHERE 
  bond_map.physical_port_id = v_net_physical_port_vna.phy_id AND
  bond_map.vna_id = v_net_physical_port_vna.vna_id AND
  v_net_bond_vna.bond_id = bond_map.bond_id AND
  v_net_bond_vna.vna_id = bond_map.vna_id;
  
  

-- bond sriov vf port view 
CREATE OR REPLACE VIEW v_net_bond_phy_free_sriov_vf_port_vna AS 
SELECT 
  v_net_bondmap_bond_phy.bond_name, 
  v_net_bondmap_bond_phy.bond_uuid, 
  v_net_bondmap_bond_phy.vna_uuid, 
  v_net_bondmap_bond_phy.vna_is_online, 
  v_net_bondmap_bond_phy.phy_uuid, 
  v_net_bondmap_bond_phy.phy_name, 
  v_net_bondmap_bond_phy.phy_is_online,
  v_net_bondmap_bond_phy.phy_is_consistency,
  v_net_bondmap_bond_phy.phy_admin_state,
  v_net_free_sriov_vf_port_vna.vf_uuid, 
  v_net_free_sriov_vf_port_vna.pci, 
  v_net_free_sriov_vf_port_vna.vlan_num, 
  v_net_free_sriov_vf_port_vna.vf_name,
  v_net_free_sriov_vf_port_vna.vf_is_online,
  v_net_free_sriov_vf_port_vna.vf_is_consistency,
  v_net_free_sriov_vf_port_vna.vf_is_cfg,
  v_net_free_sriov_vf_port_vna.vf_admin_state 
    
FROM 
  public.v_net_bondmap_bond_phy, 
  public.v_net_free_sriov_vf_port_vna
WHERE 
  v_net_free_sriov_vf_port_vna.sriov_uuid = v_net_bondmap_bond_phy.phy_uuid;  
  
  
-- 查看bond phy 成员口的sriov空闲情况  
CREATE OR REPLACE VIEW v_net_schedule_edvs_bond_phy_free_sriov_vf_statistic AS   
SELECT 
  a.bond_name, 
  a.bond_uuid, 
  a.bond_mode, 
  a.vna_uuid, 
  a.vna_is_online,  
  a.phy_name, 
  a.phy_uuid, 
  a.phy_is_online,
  a.phy_is_consistency,
  a.phy_admin_state,
  (SELECT count(*) FROM v_net_bond_phy_free_sriov_vf_port_vna b 
   WHERE  
     a.bond_uuid = b.bond_uuid AND a.phy_uuid = b.phy_uuid AND 
	 b.vf_is_consistency = 1 ) AS port_free_sriov_num 
FROM 
  v_net_bondmap_bond_phy a
WHERE 
  a.vna_is_online = 1 AND a.phy_is_consistency = 1;
 
  
  
-- 12 vsi


CREATE OR REPLACE VIEW v_net_vsi_vnic AS   
SELECT 
  vnic.vm_id AS vnic_vm_id, 
  vnic.nic_index AS vnic_nic_index, 
  vnic.is_sriov AS vnic_is_sriov, 
  vm.project_id AS vnic_project_id, 
  vnic.is_loop AS vnic_is_loop, 
  vnic.logic_network_uuid AS vnic_logic_network_uuid, 
  vnic.port_type AS vnic_port_type, 
  vm.host_name AS vnic_host_name, 
  vm.cluster_name AS vnic_cluster_name, 
  vnic.id AS vnic_id, 
  vsi_profile.id AS vsi_profile_id,
  vsi_profile.vsi_id_value AS vsi_id_value, 
  vsi_profile.vsi_id_format AS vsi_id_format, 
  vsi_profile.vm_pg_id AS vsi_vm_pg_id, 
  vsi_profile.mac_id AS vsi_mac_id, 
  vsi_profile.logic_network_ip_id AS vsi_logic_network_ip_id, 
  vsi_profile.virtual_switch_id AS vsi_virtual_switch_id 
FROM 
  vm,
  vnic, 
  vsi_profile
WHERE 
  vm.vm_id = vnic.vm_id AND 
  vnic.vsi_profile_id = vsi_profile.id ;
  

  
  




CREATE OR REPLACE VIEW v_net_vsi_deploy_vna_switch_port AS   
SELECT 
  distinct 
  c.cluster_name,
  a.vna_uuid,
  a.is_online AS vna_is_online, 
  b.module_name ,
  b.is_online AS    hc_is_online, 
  i.uuid AS switch_uuid,
  e.is_consistency AS switch_map_is_consistency,
    
  f.uuid AS port_uuid, 
  f.port_type AS port_type,
  f.is_consistency AS port_is_consistency ,
  
  o.project_id, 
  p.logic_network_uuid,
  p.is_sriov,
  p.is_loop

FROM 
  vna a,
  regist_module b,
  hc_info c,  
  virtual_switch d,
  virtual_switch_map e,  
  port f,       
/* regist_module g,*/
  switch i,
  
  vm o, 
  vnic p,
  vsi_profile q
  
WHERE
  a.is_online = 1 AND 
  b.is_online = 1 AND
  b.role = 1 AND 
  a.id = b.vna_id AND 
  b.id = c.regist_module_id AND 
  b.is_online = 1 AND 
  
/*
  g.is_online = 1 AND
  g.role = 5 AND 
  a.id = g.vna_id AND 
*/  
  i.id = d.switch_id AND 
  e.virtual_switch_id = d.switch_id AND 
  
  f.vna_id = a.id AND 
  f.is_consistency = 1 AND 
  e.is_consistency = 1 AND 
  e.port_id = f.id  AND 
  
  o.vm_id = p.vm_id AND 
  p.vsi_profile_id = q.id AND 
  q.virtual_switch_id = d.switch_id AND 
  b.module_name = o.host_name 
order by 
   vna_uuid  ;

   

-- 13 NEW_schedule.txt



-- 调度使用 查看vnic 视图（mac，ip，sriov，switch，vna， hc ，cluster）
-- 以后实现，作为调度的优化项 
-- vsi vnic 资源视图   

CREATE OR REPLACE VIEW v_net_vsi_vnic_resource AS    
SELECT    
  vnic_vm_id, 
  vnic_nic_index, 
  vnic_is_sriov, 
  vnic_project_id, 
  vnic_is_loop, 
  vnic_logic_network_uuid, 
  vnic_port_type, 
  vnic_host_name, 
  vnic_cluster_name, 
  vnic_id, 
  vsi_profile_id,
  vsi_id_value, 
  vsi_id_format, 
  vsi_vm_pg_id, 
  vsi_mac_id, 
  vsi_logic_network_ip_id, 
  vsi_virtual_switch_id,
  CASE 
     WHEN (SELECT b.sriov_vf_port_id  is not null  FROM v_net_sriov_vf_port_vna c, vsi_sriov b
               WHERE a.vsi_profile_id = b.vsi_profile_id AND b.sriov_vf_port_id = c.id )
       THEN (1)
     ELSE (0) 
  END AS  is_has_sriovvf,
  CASE 
     WHEN (SELECT b.sriov_vf_port_id  is not null  FROM v_net_sriov_vf_port_vna c, vsi_sriov b
               WHERE a.vsi_profile_id = b.vsi_profile_id AND b.sriov_vf_port_id = c.id )
       THEN (SELECT c.pci FROM v_net_sriov_vf_port_vna c, vsi_sriov b
               WHERE a.vsi_profile_id = b.vsi_profile_id AND b.sriov_vf_port_id = c.id )
     ELSE (null) 
  END AS  sriov_vf_pci,
  CASE 
     WHEN (SELECT b.sriov_vf_port_id  is not null  FROM v_net_sriov_vf_port_vna c, vsi_sriov b
               WHERE a.vsi_profile_id = b.vsi_profile_id AND b.sriov_vf_port_id = c.id )
       THEN (SELECT c.uuid FROM v_net_sriov_vf_port_vna c, vsi_sriov b
               WHERE a.vsi_profile_id = b.vsi_profile_id AND b.sriov_vf_port_id = c.id )
     ELSE (null) 
  END AS  sriov_vf_uuid,
  CASE 
     WHEN (SELECT b.sriov_vf_port_id  is not null  FROM v_net_sriov_vf_port_vna c, vsi_sriov b
               WHERE a.vsi_profile_id = b.vsi_profile_id AND b.sriov_vf_port_id = c.id )
       THEN (SELECT c.name FROM v_net_sriov_vf_port_vna c, vsi_sriov b
               WHERE a.vsi_profile_id = b.vsi_profile_id AND b.sriov_vf_port_id = c.id )
     ELSE (null) 
  END AS  sriov_vf_name,
  
  CASE 
     WHEN (SELECT a.vsi_vm_pg_id  is not null  FROM port_group b
               WHERE a.vsi_vm_pg_id = b.id )
       THEN (1)
     ELSE (0) 
  END AS  is_has_vmpg,
  
  CASE 
     WHEN (SELECT a.vsi_vm_pg_id  is not null  FROM port_group b
               WHERE a.vsi_vm_pg_id = b.id )
       THEN (SELECT b.uuid FROM port_group b
               WHERE a.vsi_vm_pg_id = b.id)
     ELSE (null) 
  END AS  vmpg_uuid,
  CASE 
     WHEN (SELECT a.vsi_vm_pg_id  is not null  FROM port_group b
               WHERE a.vsi_vm_pg_id = b.id )
       THEN (SELECT b.name FROM port_group b
               WHERE a.vsi_vm_pg_id = b.id)
     ELSE (null) 
  END AS  vmpg_name,
  CASE 
     WHEN (SELECT a.vsi_vm_pg_id  is not null  FROM port_group b
               WHERE a.vsi_vm_pg_id = b.id )
       THEN (SELECT b.pg_type FROM port_group b
               WHERE a.vsi_vm_pg_id = b.id)
     ELSE (null) 
  END AS  vmpg_type,
  
  CASE 
     WHEN (SELECT a.vsi_mac_id  is not null  FROM mac_pool b
               WHERE a.vsi_mac_id = b.id )
       THEN (SELECT b.mac FROM mac_pool b
               WHERE a.vsi_mac_id = b.id )
     ELSE (null) 
  END AS mac,
  
  CASE 
     WHEN (SELECT a.vsi_logic_network_ip_id  is not null  FROM logic_network_ip_pool b
               WHERE a.vsi_logic_network_ip_id = b.id )
       THEN (1)
     ELSE (0) 
  END AS  is_has_ip,  
  CASE 
     WHEN (SELECT a.vsi_logic_network_ip_id  is not null  FROM logic_network_ip_pool b
               WHERE a.vsi_logic_network_ip_id = b.id )
       THEN (SELECT b.ip FROM logic_network_ip_pool b
               WHERE a.vsi_logic_network_ip_id = b.id )
     ELSE (null) 
  END AS ip,
  
  CASE 
     WHEN (SELECT a.vsi_virtual_switch_id  is not null  FROM v_net_vswitch b
               WHERE a.vsi_virtual_switch_id = b.id )
       THEN (1)
     ELSE (0) 
  END AS  is_has_switch,  
  CASE 
     WHEN (SELECT a.vsi_virtual_switch_id  is not null  FROM v_net_vswitch b
               WHERE a.vsi_virtual_switch_id = b.id )
         THEN ( SELECT b.uuid  FROM v_net_vswitch b
               WHERE a.vsi_virtual_switch_id = b.id)
     ELSE (null) 
  END AS  switch_uuid,  
  CASE 
     WHEN (SELECT a.vsi_virtual_switch_id  is not null  FROM v_net_vswitch b
               WHERE a.vsi_virtual_switch_id = b.id )
       THEN (SELECT b.name  FROM v_net_vswitch b
               WHERE a.vsi_virtual_switch_id = b.id)
     ELSE (null) 
  END AS  switch_name,
  CASE 
     WHEN (SELECT a.vsi_virtual_switch_id  is not null  FROM v_net_vswitch b
               WHERE a.vsi_virtual_switch_id = b.id )
       THEN (SELECT b.switch_type  FROM v_net_vswitch b
               WHERE a.vsi_virtual_switch_id = b.id)
     ELSE (null) 
  END AS  switch_type
  
FROM
   v_net_vsi_vnic a ;
   
 
-- port 是否能调度  
-- 仅将port = 0,1,4 physcial port, bond port, loop port 过滤显示
-- port = 4 时 不用查看 state状态 
CREATE OR REPLACE VIEW v_net_port_state AS 
SELECT 
  distinct 
  vna_id,
  id AS port_id,
  name AS port_name,
  uuid AS port_uuid,
  port_type, 
  CASE 
     WHEN (port_type = 4) 
         THEN (1)
     WHEN (port_type =0 AND 
	     (SELECT state is not null FROM physical_port WHERE  physical_port.port_id = port.id 
		     AND physical_port.is_linked = 0 ))
         THEN (0)         
     ELSE (port.state ) 
  END AS  port_is_up
FROM 
  port 
WHERE 
  port.is_consistency = 1 AND 
  ( port.port_type = 4 OR 
    (port.state = 1 AND ( port.port_type=0 OR port.port_type = 1) ))
ORDER BY port.vna_id;
 
-- SDVS 
----- TC 调度 （调度cc， 没有wdg) 
CREATE OR REPLACE VIEW v_net_sdvs_schedule_no_wdg AS 
SELECT 
  distinct 
  c.regist_module_id AS cluster_id,
  a.id AS  vna_id,
  c.cluster_name,
  a.vna_uuid,
  b.module_name,
  i.uuid AS switch_uuid,
  i.name AS switch_name,
  
  (d.nic_max_num_cfg - d.deploy_vsi_num ) AS switch_sriov_free_num,  -- sdvs 和switch_free_num相同 
  (d.nic_max_num_cfg - d.deploy_vsi_num ) AS switch_free_num, 
  d.deploy_vsi_num,
  
  h.mtu AS pg_mtu,
  h.netplane_id, 
  f.name AS port_name,
  f.id AS port_id,
  f.uuid AS port_uuid, 
  f.port_type ,
  k.port_is_up,
  CASE 
     WHEN (f.port_type =4 )
         THEN ( 1)
     WHEN (f.port_type =0 AND (SELECT is_loop_report is not null FROM sriov_port WHERE physical_port_id = f.id AND is_loop_report = 1 ))
         THEN (1)
     ELSE (0 ) 
  END AS  port_is_loop, 
  CASE 
     WHEN ( f.port_type = 0 AND (SELECT x.physical_port_id is not null FROM sriov_port x WHERE x.physical_port_id = f.id ) )
        THEN (1)
     WHEN ( f.port_type = 1 AND ( (SELECT count(*) FROM sriov_port z, bond_map y WHERE z.physical_port_id = y.physical_port_id AND y.bond_id = f.id ) > 0 ) )
        THEN (1)
     ELSE (0)
  END AS  port_is_sriov,
  j.isolate_type,
  CASE 
     WHEN( (isolate_type > 0 ) AND 
	   (SELECT x.port_id is not null FROM vtep_port x,port y WHERE x.port_id = y.id AND x.vna_id = a.id AND x.virtual_switch_id = d.switch_id AND y.is_consistency = 1 ))
	   THEN (1)
	 ELSE (0)	 
  END AS is_support_vxlan, 
 j.is_sdn
    
FROM 
  vna a,
  regist_module b,
  hc_info c,  
  virtual_switch d,
  virtual_switch_map e,  
  port f, 
  port_group h,
  switch i,
  uplink_pg j,
  v_net_port_state k
WHERE 
  a.is_online = 1 AND 
  b.is_online = 1 AND
  b.role = 1 AND 
  a.id = b.vna_id AND 
  b.id = c.regist_module_id AND 
  
  d.switch_type = 1 AND 
  d.uplink_pg_id is not null AND 
  d.nic_max_num_cfg > d.deploy_vsi_num AND 
  
  e.is_consistency = 1 AND 
  e.virtual_switch_id = d.switch_id AND 
  
  f.vna_id = a.id AND
  f.is_consistency = 1 AND 
  e.port_id = f.id AND 

  h.id = d.uplink_pg_id AND 
  j.pg_id = h.id AND 
  h.netplane_id is not null AND 

  d.switch_id = i.id AND 
  
  f.id = k.port_id AND 
  k.port_is_up = 1
ORDER BY 
  pg_mtu ,port_is_sriov , switch_free_num DESC, deploy_vsi_num; 
  


----- TC 调度 （调度cc  带wdg)
CREATE OR REPLACE VIEW v_net_sdvs_schedule_has_wdg  AS 
SELECT 
  distinct 
  c.regist_module_id AS cluster_id,
  a.id AS  vna_id,
  c.cluster_name,
  a.vna_uuid,
  b.module_name,
  i.uuid AS switch_uuid,
  i.name AS switch_name,
  
  (d.nic_max_num_cfg - d.deploy_vsi_num ) AS switch_sriov_free_num,  -- sdvs 和switch_free_num相同 
  (d.nic_max_num_cfg - d.deploy_vsi_num ) AS switch_free_num, 
  d.deploy_vsi_num,
  
  h.mtu AS pg_mtu,  
  h.netplane_id,   
  f.name AS port_name,
  f.id AS port_id,
  f.uuid AS port_uuid, 
  f.port_type ,
  
  k.port_is_up, 
  
  CASE 
     WHEN (f.port_type =4 )
         THEN ( 1)
     WHEN (f.port_type =0 AND (SELECT is_loop_report is not null FROM sriov_port WHERE physical_port_id = f.id AND is_loop_report = 1 ))
         THEN (1)
     ELSE (0 ) 
  END AS  port_is_loop,
  
  CASE 
     WHEN ( f.port_type = 0 AND (SELECT x.physical_port_id is not null FROM sriov_port x WHERE x.physical_port_id = f.id ) )
        THEN (1)
     WHEN ( f.port_type = 1 AND ( (SELECT count(*) FROM sriov_port z, bond_map y WHERE z.physical_port_id = y.physical_port_id AND y.bond_id = f.id ) > 0 ) )
        THEN (1)
     ELSE (0)
  END AS  port_is_sriov,
  
  j.isolate_type,
  CASE 
     WHEN( (isolate_type > 0 ) AND 
	   (SELECT x.port_id is not null FROM vtep_port x,port y WHERE x.port_id = y.id AND x.vna_id = a.id AND x.virtual_switch_id = d.switch_id AND y.is_consistency = 1 ))
	   THEN (1)
	 ELSE (0)	 
  END AS is_support_vxlan, 
  j.is_sdn
  
FROM 
  vna a,
  regist_module b,
  hc_info c,  
  virtual_switch d,
  virtual_switch_map e,  
  port f ,
  regist_module g ,
  port_group h,
  switch i,
  uplink_pg j,
  v_net_port_state k
WHERE 
  a.is_online = 1 AND 
  b.is_online = 1 AND
  b.role = 1 AND 
  a.id = b.vna_id AND 
  b.id = c.regist_module_id AND 
  
  g.is_online = 1 AND
  g.role = 5 AND 
  a.id = g.vna_id AND 
  
  d.switch_type = 1 AND 
  d.uplink_pg_id is not null AND 
  d.nic_max_num_cfg > d.deploy_vsi_num AND 
  
  e.is_consistency = 1 AND 
  e.virtual_switch_id = d.switch_id AND 
  
  f.vna_id = a.id AND
  f.is_consistency = 1 AND 
  e.port_id = f.id AND 
  
  h.id = d.uplink_pg_id AND 
  j.pg_id = h.id AND 
  h.netplane_id is not null AND 

  d.switch_id = i.id AND 
  
  f.id = k.port_id AND 
  k.port_is_up = 1
ORDER BY 
  pg_mtu , port_is_sriov , switch_free_num DESC, deploy_vsi_num;   
  
  
 CREATE OR REPLACE VIEW v_net_sdn_switch AS 	
 SELECT a.name AS switch_name
   FROM switch a, virtual_switch b, uplink_pg c
  WHERE a.id = b.switch_id AND b.uplink_pg_id = c.pg_id AND c.is_sdn = 1;
    
-------------------------------------------------------------------------------------------------------------------------
-- EDVS 
-- 首先将bond 端口和成员口port信息显示出来
CREATE OR REPLACE VIEW v_net_bond_sriovport AS  
SELECT 
   a.vna_id, 
   a.id                 AS bond_id,
   a.uuid               AS bond_uuid, 
--   a.total_sriovvf_num  AS bond_total_sriovvf_num,
   a.free_sriovvf_num   AS bond_free_sriovvf_num,
   e.total_sriovport_num AS bond_total_phy_num,
   e.valid_sriovport_num AS bond_valid_phy_num,
   
   c.id                 AS phy_port_id,
   c.uuid               AS phy_port_uuid, 
--   c.total_sriovvf_num  AS phy_port_total_sriovvf_num,
   c.free_sriovvf_num   AS phy_port_free_sriovvf_num
FROM 
    port a,       -- bond port
    bond_map b,   
    port c,       -- phy port
    sriov_port d,
    bond e
WHERE 
    a.is_consistency = 1 AND     
    a.free_sriovvf_num > 0 AND 
    a.id = b.bond_id  AND 
    d.physical_port_id = c.id AND --- 成员口必须是SRIOV口
    a.id = e.port_id AND
    b.physical_port_id = c.id AND 
    c.is_consistency = 1 AND 
    c.free_sriovvf_num > 0 ;  -- 成员口必须存在free sriovvf

    
        


----- TC 调度CC LIST (不带is_bond_nics = 0, 不带wdg) 
CREATE OR REPLACE VIEW v_net_edvs_schedule_no_bond_no_wdg AS 
SELECT 
  distinct 
  c.regist_module_id AS cluster_id,
  a.id AS  vna_id,
  c.cluster_name,
  a.vna_uuid,
  b.module_name,
  i.uuid AS switch_uuid,
  i.name AS switch_name,
  
  (d.nic_max_num_cfg ) AS switch_sriov_free_num,  -- edvs 和switch_free_num相同 
  (d.nic_max_num_cfg ) AS switch_free_num, 
  d.deploy_vsi_num,
  
  h.mtu AS pg_mtu,
  f.name AS port_name,
  f.id AS port_id,
  f.uuid AS port_uuid, 
  f.port_type AS port_type,
  f.free_sriovvf_num AS port_free_sriovvf_num,
  h.netplane_id,   
  k.port_is_up,
  
  CASE 
     WHEN ( SELECT x.is_loop_report is not null FROM sriov_port x WHERE x.physical_port_id = f.id AND x.is_loop_report = 1 )
         THEN ( 1)
     ELSE (0 ) 
  END AS  port_is_loop ,
  j.isolate_type,
  CASE 
     WHEN( (isolate_type > 0 ) AND 
	   (SELECT x.port_id is not null FROM vtep_port x,port y WHERE x.port_id = y.id AND x.vna_id = a.id AND x.virtual_switch_id = d.switch_id AND y.is_consistency = 1 ))
	   THEN (1)
	 ELSE (0)	 
  END AS is_support_vxlan, 
  j.is_sdn  
FROM 
  vna a,
  regist_module b,
  hc_info c,  
  virtual_switch d,
  virtual_switch_map e,  
  port f,        ---- sirov phy  
  port_group h,
  switch i,
  uplink_pg j,
  v_net_port_state k
  
WHERE 
  a.is_online = 1 AND 
  b.is_online = 1 AND
  b.role = 1 AND 
  a.id = b.vna_id AND 
  b.id = c.regist_module_id AND 
  
  d.switch_type = 2 AND 
  d.uplink_pg_id is not null AND 
  d.nic_max_num_cfg > 0 AND 
  
  e.is_consistency = 1 AND 
  e.virtual_switch_id = d.switch_id AND 
  
  (f.port_type = 0 ) AND --- 只能为sriov phy port 
  f.vna_id = a.id AND
  f.is_consistency = 1 AND 
  e.port_id = f.id AND 

  h.id = d.uplink_pg_id AND 
  j.pg_id = h.id AND 
  h.netplane_id is not null AND 

  d.switch_id = i.id AND 
  f.id = k.port_id AND 
  k.port_is_up = 1
ORDER BY   
   pg_mtu , 
   port_free_sriovvf_num DESC, switch_free_num DESC, deploy_vsi_num,
   vna_uuid, switch_uuid, port_uuid;  


----- TC 调度CC LIST (不带is_bond_nics = 0, 带wdg) 
CREATE OR REPLACE VIEW v_net_edvs_schedule_no_bond_has_wdg AS 
SELECT 
  distinct 
  c.regist_module_id AS cluster_id,
  a.id AS  vna_id,
  c.cluster_name,
  a.vna_uuid,
  b.module_name,
  i.uuid AS switch_uuid,
  i.name AS switch_name,
  
  (d.nic_max_num_cfg ) AS switch_sriov_free_num,  -- edvs 和switch_free_num相同 
  (d.nic_max_num_cfg ) AS switch_free_num, 
  d.deploy_vsi_num,
  
  h.mtu AS pg_mtu,
  f.name AS port_name,
  f.id AS port_id,
  f.uuid AS port_uuid, 
  f.port_type AS port_type,
  f.free_sriovvf_num AS port_free_sriovvf_num,
  h.netplane_id,
  k.port_is_up,
  
  CASE 
     WHEN ( SELECT x.is_loop_report is not null FROM sriov_port x WHERE x.physical_port_id = f.id AND x.is_loop_report = 1 )
         THEN ( 1)
     ELSE (0 ) 
  END AS  port_is_loop,
  j.isolate_type,
  CASE 
     WHEN( (isolate_type > 0 ) AND 
	   (SELECT x.port_id is not null FROM vtep_port x,port y WHERE x.port_id = y.id AND x.vna_id = a.id AND x.virtual_switch_id = d.switch_id AND y.is_consistency = 1 ))
	   THEN (1)
	 ELSE (0)	 
  END AS is_support_vxlan, 
  j.is_sdn    
  
FROM 
  vna a,
  regist_module b,
  hc_info c,  
  virtual_switch d,
  virtual_switch_map e,  
  port f,        ---- sirov phy  
  regist_module g,
  port_group h,
  switch i,
  uplink_pg j,
  v_net_port_state k
WHERE 
  a.is_online = 1 AND 
  b.is_online = 1 AND
  b.role = 1 AND 
  a.id = b.vna_id AND 
  b.id = c.regist_module_id AND 
  
  g.is_online = 1 AND
  g.role = 5 AND 
  a.id = g.vna_id AND 
  
  d.switch_type = 2 AND 
  d.uplink_pg_id is not null AND 
  d.nic_max_num_cfg > 0 AND 
  
  e.is_consistency = 1 AND 
  e.virtual_switch_id = d.switch_id AND 
  
  f.port_type = 0 AND --- 只能为sriov phy port
  f.vna_id = a.id AND
  f.is_consistency = 1 AND 
  e.port_id = f.id AND 

  h.id = d.uplink_pg_id AND 
  j.pg_id = h.id AND 
  h.netplane_id is not null AND 

  d.switch_id = i.id  AND 
  f.id = k.port_id AND 
  k.port_is_up = 1
ORDER BY   
   pg_mtu , 
   port_free_sriovvf_num DESC, switch_free_num DESC, deploy_vsi_num,
   vna_uuid, switch_uuid, port_uuid;    
   
  
  
----- TC 调度CC LIST (带is_bond_nics = 1, 不带wdg)
CREATE OR REPLACE VIEW v_net_edvs_schedule_has_bond_no_wdg  AS 
SELECT 
  distinct 
  c.regist_module_id AS cluster_id,
  a.id AS  vna_id,
  c.cluster_name,
  a.vna_uuid,
  b.module_name,
  i.uuid AS switch_uuid,
  i.name AS switch_name,
  
  (d.nic_max_num_cfg ) AS switch_sriov_free_num,  -- edvs 和switch_free_num相同 
  (d.nic_max_num_cfg ) AS switch_free_num, 
  d.deploy_vsi_num,
  
  h.mtu AS pg_mtu,
  f.name AS port_name,
  f.uuid AS port_uuid, 
  f.port_type AS port_type,
  h.netplane_id,
  0 AS port_is_loop, --- bond口只能为非环回 
  
  m.bond_total_phy_num,
  m.bond_valid_phy_num,
  m.bond_free_sriovvf_num ,
  m.phy_port_uuid,
  m.phy_port_free_sriovvf_num,
  
  j.isolate_type,
  k.port_is_up,	 
  CASE 
     WHEN( (isolate_type > 0 ) AND 
	   (SELECT x.port_id is not null FROM vtep_port x,port y WHERE x.port_id = y.id AND x.vna_id = a.id AND x.virtual_switch_id = d.switch_id AND y.is_consistency = 1 ))
	   THEN (1)
	 ELSE (0)	 
  END AS is_support_vxlan, 
  j.is_sdn 
  
FROM 
  vna a,
  regist_module b,
  hc_info c,  
  virtual_switch d,
  virtual_switch_map e,  
  port f,        ---- bond 
  port_group h,
  switch i,
  uplink_pg j,
  v_net_bond_sriovport m,
  v_net_port_state k
WHERE 
  a.is_online = 1 AND 
  b.is_online = 1 AND
  b.role = 1 AND 
  a.id = b.vna_id AND 
  b.id = c.regist_module_id AND 
  
  d.switch_type = 2 AND 
  d.uplink_pg_id is not null AND 
  d.nic_max_num_cfg > 0 AND 
  
  e.is_consistency = 1 AND 
  e.virtual_switch_id = d.switch_id AND 
  
  f.vna_id = a.id AND
  f.is_consistency = 1 AND 
  e.port_id = f.id AND 

  h.id = d.uplink_pg_id AND 
  j.pg_id = h.id  AND 
  h.netplane_id is not null AND 

  d.switch_id = i.id AND 
  
  m.bond_id = f.id AND 
  m.vna_id = a.id AND 
  f.id = k.port_id AND 
  k.port_is_up = 1
ORDER BY 
    pg_mtu , 
    bond_valid_phy_num DESC, bond_free_sriovvf_num DESC, phy_port_free_sriovvf_num DESC,  switch_free_num DESC, deploy_vsi_num,
    vna_uuid, switch_uuid, phy_port_uuid;  
  
  
----- TC 调度CC LIST (带is_bond_nics = 1, 带wdg)
CREATE OR REPLACE VIEW v_net_edvs_schedule_has_bond_has_wdg AS 
SELECT 
  distinct 
  c.regist_module_id AS cluster_id,
  a.id AS  vna_id,
  c.cluster_name,
  a.vna_uuid,
  b.module_name,
  i.uuid AS switch_uuid,
  i.name AS switch_name,
  
  (d.nic_max_num_cfg ) AS switch_sriov_free_num,  -- edvs 和switch_free_num相同 
  (d.nic_max_num_cfg ) AS switch_free_num, 
  d.deploy_vsi_num,
  
  h.mtu AS pg_mtu,
  f.name AS port_name,
  f.uuid AS port_uuid, 
  f.port_type AS port_type,
  h.netplane_id,
  0 AS port_is_loop, --- bond口只能为非环回 
  
  m.bond_total_phy_num,
  m.bond_valid_phy_num,
  m.bond_free_sriovvf_num,
  m.phy_port_uuid,
  m.phy_port_free_sriovvf_num,
  j.isolate_type,
  k.port_is_up,
  
  CASE 
     WHEN( (isolate_type > 0 ) AND 
	   (SELECT x.port_id is not null FROM vtep_port x,port y WHERE x.port_id = y.id AND x.vna_id = a.id AND x.virtual_switch_id = d.switch_id AND y.is_consistency = 1 ))
	   THEN (1)
	 ELSE (0)	 
  END AS is_support_vxlan, 
  j.is_sdn 
    
FROM 
  vna a,
  regist_module b,
  hc_info c,  
  virtual_switch d,
  virtual_switch_map e,  
  port f,        ---- bond 
  regist_module g,
  port_group h,
  switch i,
  uplink_pg j,
  v_net_bond_sriovport m,
  v_net_port_state k
  
WHERE 
  a.is_online = 1 AND 
  b.is_online = 1 AND
  b.role = 1 AND 
  a.id = b.vna_id AND 
  b.id = c.regist_module_id AND 
  
  g.is_online = 1 AND
  g.role = 5 AND 
  a.id = g.vna_id AND 
  
  d.switch_type = 2 AND 
  d.uplink_pg_id is not null AND 
  d.nic_max_num_cfg > 0 AND 
  
  e.is_consistency = 1 AND 
  e.virtual_switch_id = d.switch_id AND 
  
  f.vna_id = a.id AND
  f.is_consistency = 1 AND 
  e.port_id = f.id AND 

  h.id = d.uplink_pg_id AND 
  j.pg_id = h.id AND 
  h.netplane_id is not null AND 

  d.switch_id = i.id AND 
  
  m.bond_id = f.id AND 
  m.vna_id = a.id AND 
  f.id = k.port_id AND 
  k.port_is_up = 1
ORDER BY 
    pg_mtu , 
    bond_valid_phy_num DESC, bond_free_sriovvf_num DESC, phy_port_free_sriovvf_num DESC,  switch_free_num DESC, deploy_vsi_num,
    vna_uuid, switch_uuid, phy_port_uuid;  
    
	
-- 14 kernel_report
CREATE OR REPLACE VIEW v_net_kernelreport_bondmap AS  
SELECT 
  kernel_cfg_report.id, 
  kernel_cfg_report.uuid, 
  kernel_cfg_report.vna_id, 
  vna.vna_uuid, 
  kernel_cfg_report.switch_name, 
  kernel_cfg_report.uplink_port_name, 
  kernel_cfg_report.ip, 
  kernel_cfg_report.mask, 
  kernel_cfg_report.is_online, 
  kernel_cfg_report.kernel_port_name, 
  kernel_cfg_report.uplink_port_type, 
  kernel_cfg_report_bond.bond_mode AS uplink_port_bond_mode, 
  kernel_cfg_report_bond_map.phy_port_name
FROM 
  kernel_cfg_report, 
  kernel_cfg_report_bond, 
  kernel_cfg_report_bond_map, 
  vna
WHERE 
  kernel_cfg_report.vna_id = vna.id AND
  kernel_cfg_report_bond.kernel_cfg_report_id = kernel_cfg_report.id AND
  kernel_cfg_report_bond_map.kernel_cfg_report_bond_id = kernel_cfg_report_bond.kernel_cfg_report_id;

--   15 wildcast_create_vport.txt



CREATE OR REPLACE VIEW v_net_wildcast_create_vport AS   
SELECT 
  distinct
  id,
  uuid, 
  port_name,
  port_type,   
  CASE 
     WHEN (SELECT kernel_type  IS NOT NULL FROM wildcast_cfg_create_kernel_port b WHERE a.id = b.wildcast_create_vport_id  )
         THEN (1)
     ELSE (0 ) 
  END AS  is_has_kernel_type,

  CASE 
     WHEN (SELECT kernel_type  IS NOT NULL FROM wildcast_cfg_create_kernel_port b WHERE a.id = b.wildcast_create_vport_id  )
         THEN (SELECT kernel_type FROM wildcast_cfg_create_kernel_port b WHERE a.id = b.wildcast_create_vport_id )
     ELSE (0 ) 
  END AS  kernel_type,
  
  (SELECT c.uuid  FROM wildcast_cfg_create_kernel_port b, port_group c WHERE a.id = b.wildcast_create_vport_id AND c.id = b.kernel_pg_id)
      AS  kernel_pg_uuid,
  (SELECT c.switch_type FROM wildcast_cfg_create_kernel_port b, switch c WHERE a.id = b.wildcast_create_vport_id AND b.switch_id = c.id )
      AS switch_type,
  (SELECT c.name FROM wildcast_cfg_create_kernel_port b, switch c WHERE a.id = b.wildcast_create_vport_id AND b.switch_id = c.id )
      AS switch_name,
  (SELECT c.uuid FROM wildcast_cfg_create_kernel_port b, switch c WHERE a.id = b.wildcast_create_vport_id AND b.switch_id = c.id )
      AS switch_uuid,
   CASE 
     WHEN (SELECT is_dhcp  IS NOT NULL FROM wildcast_cfg_create_kernel_port b WHERE a.id = b.wildcast_create_vport_id  )
         THEN (SELECT is_dhcp FROM wildcast_cfg_create_kernel_port b WHERE a.id = b.wildcast_create_vport_id )
     ELSE (0 ) 
   END AS  is_dhcp
FROM 
  wildcast_cfg_create_virtual_port a;
  
  
CREATE OR REPLACE VIEW v_net_wildcast_task_create_vport AS     
SELECT 
  wildcast_task_create_vport.id AS task_id, 
  wildcast_task_create_vport.uuid AS task_uuid, 
  wildcast_task_create_vport.state AS task_state, 
  wildcast_task_create_vport.send_num AS task_send_num, 
  vna.vna_uuid, 
  vna.is_online AS vna_is_online, 
  v_net_wildcast_create_vport.uuid AS create_vport_uuid, 
  v_net_wildcast_create_vport.port_name, 
  v_net_wildcast_create_vport.port_type, 
  v_net_wildcast_create_vport.is_has_kernel_type, 
  v_net_wildcast_create_vport.kernel_type,
  v_net_wildcast_create_vport.kernel_pg_uuid,
  v_net_wildcast_create_vport.switch_type,
  v_net_wildcast_create_vport.switch_name,
  v_net_wildcast_create_vport.switch_uuid,
  v_net_wildcast_create_vport.is_dhcp
FROM 
  public.v_net_wildcast_create_vport, 
  public.wildcast_task_create_vport, 
  public.vna
WHERE 
  v_net_wildcast_create_vport.id = wildcast_task_create_vport.wildcast_cfg_create_vport_id AND
  vna.id = wildcast_task_create_vport.vna_id;
  
  
-- 16 wildcast_switch.txt


CREATE OR REPLACE VIEW v_net_wildcast_switch AS   
SELECT 
  distinct
  a.id,
  a.uuid, 
  a.switch_name,
  a.switch_type,
  a.state,
  a.evb_mode,
  b.uuid AS pg_uuid,
  a.mtu_max,
  a.nic_max_num,
  port_name,
  port_type,   
  CASE 
     WHEN (SELECT bond_mode IS NOT NULL FROM wildcast_cfg_switch_bond c WHERE a.id = c.wildcast_cfg_switch_id  )
         THEN (SELECT bond_mode FROM wildcast_cfg_switch_bond c WHERE a.id = c.wildcast_cfg_switch_id )
     ELSE (NULL ) ::integer
  END AS  bond_mode
  
FROM 
  wildcast_cfg_switch a,
  port_group  b
WHERE 
  a.upg_id = b.id;  
  
  
CREATE OR REPLACE VIEW v_net_wildcast_switch_bondmap AS  
SELECT 
  wildcast_cfg_switch.uuid AS switch_uuid, 
  wildcast_cfg_switch.switch_name, 
  wildcast_cfg_switch.switch_type, 
  wildcast_cfg_switch.port_name AS uplink_port_name, 
  wildcast_cfg_switch.port_type AS uplink_port_type, 
  wildcast_cfg_switch_bond.bond_mode, 
  wildcast_cfg_switch_bond_map.phy_port_name, 
  port_group.uuid AS pg_uuid, 
  port_group."name" AS pg_name
FROM 
  public.wildcast_cfg_switch, 
  public.wildcast_cfg_switch_bond, 
  public.wildcast_cfg_switch_bond_map, 
  public.port_group
WHERE 
  wildcast_cfg_switch.upg_id = port_group.id AND
  wildcast_cfg_switch_bond.wildcast_cfg_switch_id = wildcast_cfg_switch.id AND
  wildcast_cfg_switch_bond_map.wildcast_cfg_bond_id = wildcast_cfg_switch_bond.wildcast_cfg_switch_id;
    
  
---- task view   
CREATE OR REPLACE VIEW v_net_wildcast_task_switch AS    
SELECT 
  wildcast_task_switch.uuid AS task_uuid, 
  wildcast_task_switch.state AS task_state, 
  wildcast_task_switch.send_num AS task_send_num, 
  wildcast_task_switch.id AS task_id, 
  v_net_wildcast_switch.uuid AS switch_uuid, 
  v_net_wildcast_switch.switch_name, 
  v_net_wildcast_switch.switch_type, 
  v_net_wildcast_switch.state, 
  v_net_wildcast_switch.evb_mode, 
  v_net_wildcast_switch.pg_uuid, 
  v_net_wildcast_switch.mtu_max, 
  v_net_wildcast_switch.nic_max_num, 
  v_net_wildcast_switch.port_name, 
  v_net_wildcast_switch.port_type, 
  v_net_wildcast_switch.bond_mode, 
  vna.vna_uuid, 
  vna.is_online AS vna_is_online
FROM 
  public.wildcast_task_switch, 
  public.vna, 
  public.v_net_wildcast_switch
WHERE 
  vna.id = wildcast_task_switch.vna_id AND
  v_net_wildcast_switch.id = wildcast_task_switch.wildcast_cfg_switch_id;  

--   17 wildcast_loopback


CREATE OR REPLACE VIEW v_net_wildcast_task_loopback AS    
SELECT 
  wildcast_task_loopback.uuid AS task_uuid, 
  wildcast_task_loopback.state AS task_state, 
  wildcast_task_loopback.send_num AS task_send_num, 
  wildcast_cfg_loopback_port.uuid AS loopback_uuid, 
  wildcast_cfg_loopback_port.port_name, 
  wildcast_cfg_loopback_port.is_loop, 
  vna.vna_uuid, 
  vna.is_online AS vna_is_online
FROM 
  public.wildcast_cfg_loopback_port, 
  public.wildcast_task_loopback, 
  public.vna
WHERE 
  wildcast_task_loopback.wildcast_cfg_loopback_port_id = wildcast_cfg_loopback_port.id AND
  vna.id = wildcast_task_loopback.vna_id;

  
-- 20 web_netplane.txt


CREATE OR REPLACE VIEW web_view_netplane AS 
SELECT 
  a."name", 
  a.uuid, 
  a.description, 
  a.mtu,
  (SELECT count(*) FROM ip_range b WHERE a.id = b.netplane_id) AS iprange_count,
  (SELECT count(*) FROM mac_range c WHERE a.id = c.netplane_id) AS macrange_count,
  (SELECT count(*) FROM vlan_range d WHERE a.id = d.netplane_id) AS vlanrange_count
FROM 
  netplane a;
  

CREATE OR REPLACE VIEW web_view_netplane_iprange AS 
SELECT 
  netplane."name" AS netplane_name, 
  netplane.uuid AS netplane_uuid, 
  ip_range.ip_begin, 
  ip_range.ip_end, 
  ip_range.mask
FROM 
  netplane, 
  ip_range
WHERE 
  netplane.id = ip_range.netplane_id;  
  
  
CREATE OR REPLACE VIEW web_view_netplane_macrange AS   
  SELECT 
  netplane."name" AS netplane_name, 
  netplane.uuid AS netplane_uuid, 
  mac_range.mac_begin, 
  mac_range.mac_end
FROM 
  public.netplane, 
  public.mac_range
WHERE 
  netplane.id = mac_range.netplane_id;
  

CREATE OR REPLACE VIEW web_view_netplane_vlanrange AS   
SELECT 
  netplane."name" AS netplane_name, 
  netplane.uuid AS netplane_uuid, 
  vlan_range.vlan_begin, 
  vlan_range.vlan_end
FROM 
  netplane, 
  vlan_range
WHERE 
  netplane.id = vlan_range.netplane_id;
  
CREATE OR REPLACE VIEW web_view_sdn AS 
  SELECT
  a.uuid AS sdn_uuid,  
  a.protocol_type AS sdn_conntype, 
  a.port AS sdn_connport, 
  a.ip AS sdn_connip,
  b.uuid AS quantum_uuid,  
  b.ip AS quantum_serverip
  FROM sdn_ctrl_cfg a, quantum_restfull_cfg b;
  
  
--- 已分配的资源   
CREATE OR REPLACE VIEW web_view_netplane_logicnetwork_ipalloc AS 
SELECT 
  a.netplane_name, 
  a.netplane_uuid, 
  a.logicnetwork_name,
  a.logicnetwork_uuid,
  b.ip,
  b.mask,
  x.vm_id, 
  y.nic_index
FROM 
  v_net_logicnetwork_portgroup_netplane a,
  logic_network_ip_pool b ,
  vm x,
  vnic y,
  vsi_profile z
WHERE 
  a.logicnetwork_id = b.logic_network_id AND 
  x.vm_id = y.vm_id AND 
  y.vsi_profile_id = z.id AND 
  z.logic_network_ip_id = b.id ;
  
  
CREATE OR REPLACE VIEW web_view_netplane_macalloc AS   
  SELECT 
  netplane."name" AS netplane_name, 
  netplane.uuid AS netplane_uuid, 
  mac_pool.mac,
  x.vm_id, 
  y.nic_index
FROM 
  netplane, 
  mac_pool,
  vm x,
  vnic y,
  vsi_profile z
WHERE 
  netplane.id = mac_pool.netplane_id AND 
  x.vm_id = y.vm_id AND 
  y.vsi_profile_id = z.id AND 
  z.mac_id = mac_pool.id ;
  
CREATE OR REPLACE VIEW web_view_netplane_vlanalloc AS   
SELECT 
  netplane."name" AS netplane_name, 
  netplane.uuid AS netplane_uuid, 
  vlan_pool.vlan_num,
  x.project_id,
  x.vm_id, 
  y.nic_index,
  t.isolate_no
FROM 
  netplane, 
  vlan_pool,
  vlan_map t,
  vm x,
  vnic y,
  vsi_profile z
WHERE 
  netplane.id = vlan_pool.netplane_id AND 
  t.vlan_id = vlan_pool.id AND 
  x.vm_id = y.vm_id AND 
  y.vsi_profile_id = z.id AND 
  z.vm_pg_id = t.vm_pg_id AND 
  x.project_id = t.project_id 
ORDER BY  
  netplane_uuid, project_id, vlan_num ;
  


-- 方便统计视图 (ln (logicnetwork))
CREATE OR REPLACE  VIEW web_view_logicnetwork_ip_total_num AS  
SELECT 
  b.id AS ln_id, 
  sum(a.ip_end_num - a.ip_begin_num + 1) AS ln_total_num
FROM 
  logic_network_ip_range a,
  logic_network b
WHERE 
  a.logic_network_id = b.id 
GROUP BY  b.id ;


-- (logicnetwork) 使用的ip alloc num
CREATE OR REPLACE  VIEW web_view_logicnetwork_ip_alloc_num AS  
SELECT 
  b.id AS ln_id, 
  count(*) AS ln_alloc_num
FROM 
  logic_network_ip_pool a,
  logic_network b
WHERE 
  a.logic_network_id = b.id 
GROUP BY  b.id ;


-- 提供给web的视图(logicnetwork)
CREATE OR REPLACE VIEW web_view_logicnetwork_res_statatic_ip AS  
SELECT 
  c.id AS ln_id,
  c.name AS ln_name,
  c.uuid AS ln_uuid,
  a.ln_total_num,
  b.ln_alloc_num

FROM 
  web_view_logicnetwork_ip_total_num a,
  web_view_logicnetwork_ip_alloc_num b,
  logic_network c
WHERE 
  a.ln_id = b.ln_id AND 
  b.ln_id = c.id ;




-- 方便统计 np ip 使用情况 
CREATE OR REPLACE  VIEW web_view_netplane_ip_total_num AS  
SELECT 
  d.id AS netplane_id,
  sum(a.ln_total_num) AS np_total_num
FROM 
  web_view_logicnetwork_ip_total_num a,
  logic_network b,
  port_group c,
  netplane d 
WHERE 
  a.ln_id = b.id AND   
  b.vm_pg_id = c.id AND 
  c.netplane_id = d.id 
GROUP BY  d.id ;  

CREATE OR REPLACE  VIEW web_view_netplane_ip_alloc_num AS  
SELECT 
  d.id AS netplane_id,
  sum(a.ln_alloc_num) AS np_alloc_num
FROM 
  web_view_logicnetwork_ip_alloc_num a,
  logic_network b,
  port_group c,
  netplane d 
WHERE 
  a.ln_id = b.id AND   
  b.vm_pg_id = c.id AND 
  c.netplane_id = d.id 
GROUP BY  d.id ; 





-- 提供给web使用的统计 IP 
CREATE OR REPLACE VIEW web_view_netplane_res_statatic_ip AS  
 SELECT c.id AS np_id, c.name AS np_name, c.uuid AS np_uuid, a.np_total_num, 
 CASE 
    WHEN (SELECT np_alloc_num is not null FROM web_view_netplane_ip_alloc_num b WHERE a.netplane_id = b.netplane_id AND b.netplane_id = c.id )
    THEN (SELECT np_alloc_num  FROM web_view_netplane_ip_alloc_num b WHERE a.netplane_id = b.netplane_id AND b.netplane_id = c.id )
    ELSE (0)
 END AS np_alloc_num  
   FROM web_view_netplane_ip_total_num a, 
  netplane c
  WHERE a.netplane_id = c.id;



-- 方便统计netplane MAC 使用情况 

CREATE OR REPLACE  VIEW web_view_netplane_mac_total_num AS  
SELECT 
  b.id AS np_id, 
  sum(a.mac_end_num - a.mac_begin_num + 1) AS np_total_num
FROM 
  mac_range a,
  netplane b
WHERE 
  a.netplane_id = b.id 
GROUP BY  b.id ;

-- (netplane) 使用的mac alloc num
CREATE OR REPLACE  VIEW web_view_netplane_mac_alloc_num AS  
SELECT 
  b.id AS np_id, 
  count(*) AS np_alloc_num
FROM 
  mac_pool a,
  netplane b
WHERE 
  a.netplane_id = b.id 
GROUP BY  b.id ;  

-- 提供给web使用的统计 MAC 
CREATE OR REPLACE VIEW web_view_netplane_res_statatic_mac AS  
  SELECT c.id AS np_id, c.name AS np_name, c.uuid AS np_uuid, a.np_total_num, 
 CASE 
    WHEN (SELECT np_alloc_num is not null FROM web_view_netplane_mac_alloc_num b WHERE a.np_id = b.np_id AND b.np_id = c.id )
    THEN (SELECT np_alloc_num  FROM web_view_netplane_mac_alloc_num b WHERE a.np_id = b.np_id AND b.np_id = c.id )
    ELSE (0)
 END AS np_alloc_num 
   FROM web_view_netplane_mac_total_num a, 
  netplane c
  WHERE a.np_id = c.id;  




-- 方便统计netplane vlan 使用情况 

CREATE OR REPLACE  VIEW web_view_netplane_vlan_total_num AS  
SELECT 
  b.id AS np_id, 
  sum(a.vlan_end - a.vlan_begin + 1) AS np_total_num
FROM 
  vlan_range a,
  netplane b
WHERE 
  a.netplane_id = b.id 
GROUP BY  b.id ;

-- (netplane) 使用的vlan alloc num
CREATE OR REPLACE  VIEW web_view_netplane_vlan_alloc_num AS  
SELECT 
  b.id AS np_id, 
  count(*) AS np_alloc_num
FROM 
  vlan_pool a,
  netplane b
WHERE 
  a.netplane_id = b.id 
GROUP BY  b.id ;  


-- 提供给web使用的统计 vlan 
CREATE OR REPLACE VIEW web_view_netplane_res_statatic_vlan AS  
 SELECT c.id AS np_id, c.name AS np_name, c.uuid AS np_uuid, a.np_total_num,
 CASE 
    WHEN (SELECT np_alloc_num is not null FROM web_view_netplane_vlan_alloc_num b WHERE a.np_id = b.np_id AND b.np_id = c.id )
    THEN (SELECT np_alloc_num  FROM web_view_netplane_vlan_alloc_num b WHERE a.np_id = b.np_id AND b.np_id = c.id )
    ELSE (0)
 END AS np_alloc_num 
FROM 
  web_view_netplane_vlan_total_num a,
  netplane c
  WHERE a.np_id  = c.id ; 
  
  

-- 21 web_portgroup.txt

CREATE OR REPLACE VIEW web_view_portgroup AS 
SELECT a.id, a.name, a.pg_type, a.uuid, a.acl, a.qos, a.allowed_priorities, a.receive_bandwidth_limit, 
        a.receive_bandwidth_reserve, a.default_vlan_id, a.promiscuous, a.macvlan_filter_enable, a.allowed_transmit_macs,
        a.allowed_transmit_macvlans, a.policy_block_override, a.policy_vlan_override, a.version, a.mgr_id, a.type_id,
        a.allow_mac_change, a.switchport_mode, a.netplane_id, a.mtu, 
        ( SELECT count(*) FROM v_net_uplinkpg_trunk_vlanrange b WHERE a.id = b.uplinkpg_id ) AS vlan_count,
        ( SELECT name FROM netplane n WHERE a.netplane_id = n.id ) AS netplane_name,
		
	  CASE 
         WHEN ( switchport_mode = 0) 
	    THEN ( SELECT native_vlan_num FROM pg_switchport_trunk c WHERE a.id = c.pg_id )
	  ELSE (null)	  
      END AS native_vlan_num, 
	  
	  CASE 
         WHEN ( (switchport_mode = 1) AND (pg_type != 21) )
	    THEN ( SELECT vlan_num FROM pg_switchport_access_vlan d WHERE a.id = d.pg_id )
	  ELSE (null) 
      END AS access_vlan_num, 
	  
     a.description,
     CASE
            WHEN a.pg_type = 0 THEN ( SELECT uppg.isolate_type
               FROM uplink_pg uppg
              WHERE a.id = uppg.pg_id)
            WHEN a.pg_type = 20 OR a.pg_type = 21 THEN ( SELECT vmpg.isolate_type
               FROM vm_pg vmpg
              WHERE a.id = vmpg.pg_id)
            ELSE 0
        END AS isolate_type, 
		
	  CASE
            WHEN ( SELECT vmpg.isolate_type is not null FROM vm_pg vmpg WHERE a.id = vmpg.pg_id AND vmpg.isolate_type = 0) 
			   AND a.switchport_mode = 1 AND a.pg_type = 21 THEN 
			   ( SELECT e.isolate_no
               FROM pg_switchport_access_isolate e
              WHERE a.id = e.pg_id)
            WHEN ( SELECT vmpg.isolate_type is not null FROM vm_pg vmpg WHERE a.id = vmpg.pg_id AND vmpg.isolate_type = 1)  AND
     			a.pg_type = 21 THEN 
				( SELECT f.isolate_no
               FROM vm_pg_private_vxlan_segment f
              WHERE a.id = f.vmpg_id)              
            ELSE NULL::integer
        END AS isolate_no, 
		
    ( SELECT vx.segment_num AS segment_id_num
           FROM vm_pg_share_vxlan_segment vx
          WHERE vx.vmpg_id = a.id) AS segment_id_num,
    CASE
            WHEN a.pg_type = 0 THEN ( SELECT uppg.is_sdn
               FROM uplink_pg uppg
              WHERE a.id = uppg.pg_id)
            WHEN a.pg_type = 20 OR a.pg_type = 21 THEN ( SELECT vmpg.is_sdn
               FROM vm_pg vmpg
              WHERE a.id = vmpg.pg_id)
            ELSE 0
    END AS is_sdn 
    FROM port_group a;
	
CREATE OR REPLACE VIEW web_view_portgroup_vlanrange AS 
SELECT 
  a.id AS pg_id,
  a.uuid AS pg_uuid,
  a.pg_type,
  a.switchport_mode,
  b.native_vlan_num,
  c.vlan_begin,
  c.vlan_end
FROM 
   port_group a,
   pg_switchport_trunk b,
   pg_switchport_trunk_vlan_range c
WHERE
   a.id = b.pg_id AND 
   b.pg_id = c.pg_switchport_trunk_id
ORDER BY 
   a.id, c.vlan_begin ;
   

-- 22 web_logicnet
 
 
 CREATE OR REPLACE VIEW web_view_logicnet AS 
 SELECT a.name, a.uuid, a.vm_pg_id, a.ip_bootprotocolmode , a.description,  
    ( SELECT count(*) FROM logic_network_ip_range b WHERE a.id = b.logic_network_id ) AS iprange_count,
    ( SELECT name From port_group c WHERE c.id=a.vm_pg_id ) AS vm_pg_name, 
    ( SELECT uuid From port_group c WHERE c.id=a.vm_pg_id ) AS vm_pg_uuid
    FROM logic_network a;

CREATE OR REPLACE VIEW web_view_logicnet_iprange AS 
SELECT 
d.name AS netplane_name,
d.uuid AS netplane_uuid,
a.name AS logic_network_name, 
a.uuid AS logic_network_uuid, 
b.ip_begin, 
b.ip_end, 
b.mask
FROM 
logic_network a, 
logic_network_ip_range b,
port_group c, 
netplane d
WHERE 
a.id = b.logic_network_id AND 
a.vm_pg_id = c.id AND 
c.netplane_id = d.id 
ORDER BY 
  netplane_uuid, ip_begin; 
  
	
-- 23 web_vna
CREATE OR REPLACE VIEW web_view_vna AS   
SELECT 
  vna.id AS vna_id,
  vna.vna_uuid, 
  vna.vna_application, 
  vna.is_online AS vna_is_online, 
  CASE 
      WHEN( SELECT b.is_online is not null FROM regist_module b WHERE b.vna_id 
      = vna.id AND b.role = 1 AND b.is_online = 1 ) 
      THEN ( 1) 
      ELSE (0)
  END AS hc_is_online,

  CASE 
      WHEN( SELECT b.is_online is not null FROM regist_module b WHERE b.vna_id 
      = vna.id AND b.role = 1 AND b.is_online = 1  ) 
      THEN ( SELECT b.module_name FROM regist_module b WHERE b.vna_id 
      = vna.id AND b.role = 1 AND b.is_online = 1 ) 
  END AS hc_application,
  
  (SELECT count(*) FROM port WHERE port.vna_id = vna.id AND port.port_type <> 3 ) AS port_total_num
FROM 
  vna; 
  
  
  
-- 24 web_switch
 CREATE OR REPLACE VIEW web_view_switch AS 
 SELECT a.id, a.name, a.uuid, a.switch_type, a.state, a.max_mtu, a.is_active, a.evb_mode, a.uplink_pg_id, a.nic_max_num_cfg, 
        ( SELECT count(*) FROM virtual_switch_map b WHERE a.id = b.virtual_switch_id ) AS port_count,
        ( SELECT name From port_group c WHERE c.id=a.uplink_pg_id ) AS uplink_pg_name, 
        ( SELECT uuid From port_group c WHERE c.id=a.uplink_pg_id ) AS uplink_pg_uuid        
    FROM v_net_vswitch a;


 CREATE OR REPLACE VIEW web_view_switch_vna_port AS 
 SELECT a.id, 
  a.name AS switch_name, 
  a.uuid AS switch_uuid, 
  a.switch_type, 
  a.state, 
  a.max_mtu, 
  a.is_active, 
  a.evb_mode, 
  a.uplink_pg_id,
  a.nic_max_num_cfg, 
  ( 1) AS port_count, /* switch uplink port 一个VNA 目前只可能一个 */

  ( SELECT name From port_group c WHERE c.id=a.uplink_pg_id ) AS 
uplink_pg_name, 

  ( SELECT uuid From port_group c WHERE c.id=a.uplink_pg_id ) AS 
uplink_pg_uuid,

  b.vna_uuid,
  b.id AS vna_id,
  b.is_online AS vna_is_online,
  d.name AS port_name,
  d.port_type,
  CASE WHEN (port_type = 4 ) THEN (1)
     WHEN (port_type = 1) THEN (0)
     WHEN (port_type = 0 AND ( SELECT x.is_loop_report is not null FROM 
sriov_port x WHERE x.physical_port_id = d.id AND x.is_loop_report = 1 ))

        THEN (1) ELSE (0) END AS is_loop 
 FROM v_net_vswitch a,
  vna b,
  virtual_switch_map c,
  port d WHERE a.id = c.virtual_switch_id AND
  c.port_id = d.id AND
  d.vna_id = b.id ; 




-- 25 web_port.txt

CREATE OR REPLACE VIEW web_view_port_physical AS    
SELECT 
  vna.vna_uuid, 
  vna.is_online as vna_is_online, 
  port."name", 
  port.uuid, 
  port.state, 
  port.is_online, 
  port.is_cfg, 
  port.is_consistency, 
  port.is_broadcast, 
  port.is_running, 
  port.is_multicast, 
  port.promiscuous, 
  port.mtu, 
  port.is_master, 
  port.port_type, 
  physical_port.supported_ports, 
  physical_port.supported_link_modes, 
  physical_port.is_support_auto_negotiation, 
  physical_port.advertised_link_modes, 
  physical_port.advertised_pause_frame_use, 
  physical_port.is_advertised_auto_negotiation, 
  physical_port.speed, 
  physical_port.duplex, 
  physical_port.port, 
  physical_port.phyad, 
  physical_port.transceiver, 
  physical_port.auto_negotiate, 
  physical_port.supports_wake_on, 
  physical_port.wake_on, 
  physical_port.current_msg_level, 
  physical_port.is_linked, 
  physical_port_extend.is_rx_checksumming, 
  physical_port_extend.is_tx_checksumming, 
  physical_port_extend.is_scatter_gather, 
  physical_port_extend.is_tcp_segmentation_offload, 
  physical_port_extend.is_udp_fragmentation_offload, 
  physical_port_extend.is_generic_segmentation_offload, 
  physical_port_extend.is_generic_receive_offload, 
  physical_port_extend.is_large_receive_offload,
  
  CASE 
      WHEN (SELECT physical_port_id is not null FROM sriov_port WHERE port.id = sriov_port.physical_port_id )
	    THEN (1)
	  ELSE (0)	  
  END AS is_sriov_port, 
  
  CASE 
      WHEN (SELECT physical_port_id is not null FROM sriov_port WHERE port.id = sriov_port.physical_port_id )
	    THEN (SELECT is_loop_report FROM sriov_port WHERE port.id = sriov_port.physical_port_id)
	  ELSE (NULL)	  
  END AS sriov_port_is_loop_report, 
  
  CASE 
      WHEN (SELECT physical_port_id is not null FROM sriov_port WHERE port.id = sriov_port.physical_port_id )
	    THEN (SELECT is_loop_cfg FROM sriov_port WHERE port.id = sriov_port.physical_port_id)
	  ELSE (NULL)	  
  END AS sriov_port_is_loop_cfg
  
FROM 
  port, 
  physical_port, 
  physical_port_extend, 
  vna
WHERE 
  port.vna_id = vna.id AND
  physical_port.port_id = port.id AND
  physical_port.vna_id = port.vna_id AND
  physical_port_extend.physical_port_id = physical_port.port_id;
  
  
CREATE OR REPLACE VIEW web_view_port_bond AS   
SELECT 
 v_net_bond_vna.bond_name, 
 v_net_bond_vna.bond_uuid, 
 v_net_bond_vna.bond_mode, 
 v_net_bond_vna.bond_is_online, 
 v_net_bond_vna.bond_is_cfg, 
 v_net_bond_vna.bond_is_consistency, 
 v_net_bond_vna.bond_state, 
 v_net_bond_vna.vna_uuid, 
 v_net_bond_vna.vna_is_online, 
 v_net_physical_port_vna.phy_name, 
 v_net_physical_port_vna.phy_uuid, 
 bond_map.is_cfg AS bond_map_is_cfg, 
 bond_map.is_selected AS bond_map_is_selected, 
 bond_map.lacp_is_success, 
 bond_map.backup_is_active, 
 v_net_physical_port_vna.is_linked
FROM 
  bond_map, v_net_bond_vna, v_net_physical_port_vna
WHERE 
  bond_map.physical_port_id = v_net_physical_port_vna.phy_id 
  AND bond_map.vna_id = v_net_physical_port_vna.vna_id 
  AND v_net_bond_vna.bond_id = bond_map.bond_id 
  AND v_net_bond_vna.vna_id = bond_map.vna_id;
  
  
CREATE OR REPLACE VIEW web_view_port_kernel AS   
SELECT 
  vna.vna_uuid, 
  vna.is_online AS vna_is_online, 
  port.uuid AS port_uuid, 
  port."name" as port_name, 
  port.state AS port_state, 
  port.is_online AS port_is_online, 
  port.is_cfg AS port_is_cfg, 
  port.is_consistency AS port_is_consistency, 
  kernel_port."type" AS kernel_type,
  CASE 
     WHEN (kernel_port.kernel_pg_id is not null )
	     THEN (1)
	 ELSE (0)
  END is_has_pg ,
  CASE 
     WHEN (kernel_port.kernel_pg_id is not null )
	     THEN (SELECT port_group.uuid FROM port_group WHERE port_group.id = kernel_port.kernel_pg_id)
	 ELSE (NULL)
  END pg_uuid ,
  
  CASE 
     WHEN (kernel_port.virtual_switch_id is not null )
	     THEN (1)
	 ELSE (0)
  END is_has_vswitch ,  
  CASE 
     WHEN (kernel_port.virtual_switch_id is not null )
	     THEN (SELECT uuid FROM switch WHERE switch.id = kernel_port.virtual_switch_id)
	 ELSE (NULL)
  END vswitch_uuid 
  
FROM 
  kernel_port, 
  port, 
  vna
WHERE 
  port.vna_id = vna.id AND
  port.id = kernel_port.port_id;

CREATE OR REPLACE VIEW web_view_port_uplinkloop AS     
SELECT 
  vna.vna_uuid, 
  vna.is_online AS vna_is_online, 
  port.uuid AS port_uuid, 
  port."name" as port_name, 
  port.state AS port_state, 
  port.is_online AS port_is_online, 
  port.is_cfg AS port_is_cfg, 
  port.is_consistency AS port_is_consistency 
FROM 
  port, 
  vna, 
  uplink_loop_port
WHERE 
  port.vna_id = vna.id AND
  port.id = uplink_loop_port.port_id;
  
CREATE OR REPLACE VIEW web_view_port_vswitchport AS     
SELECT 
  vna.vna_uuid, 
  vna.is_online, 
  vna.is_online AS vna_is_online, 
  port.uuid AS port_uuid, 
  port."name" as port_name, 
  port.state AS port_state, 
  port.is_online AS port_is_online, 
  port.is_cfg AS port_is_cfg, 
  port.is_consistency AS port_is_consistency
FROM 
  port, 
  vna, 
  vswitch_port
WHERE 
  port.vna_id = vna.id AND
  port.id = vswitch_port.port_id;
  
  
CREATE OR REPLACE VIEW web_view_port_sriov_vf AS   
SELECT 
  v_net_sriov_vf_port_vna.vna_uuid, 
  v_net_sriov_vf_port_vna.vna_is_online, 
  v_net_sriov_vf_port_vna."name" AS sriov_vf_port_name, 
  v_net_sriov_vf_port_vna.id AS sriov_vf_port_id,
  v_net_sriov_vf_port_vna.uuid AS sriov_vf_port_uuid, 
  v_net_sriov_vf_port_vna.pci, 
  v_net_sriov_vf_port_vna.vlan_num, 
  v_net_sriov_vf_port_vna.port_is_consistency AS sriov_vf_port_is_consistency, 
  v_net_sriov_vf_port_vna.port_is_cfg AS sriov_vf_port_is_cfg, 
  v_net_sriov_vf_port_vna.port_is_online AS sriov_vf_port_is_online, 
  v_net_sriov_vf_port_vna.port_admin_state AS sriov_vf_port_is_admin_state, 
  v_net_sriov_vf_port_vna.port_state AS sriov_vf_port_is_state, 
  v_net_sriov_port_vna."name" AS phy_port_name, 
  v_net_sriov_port_vna.uuid AS phy_port_uuid,
  CASE 
     WHEN (SELECT sriov_vf_port_id is not null FROM vsi_sriov WHERE v_net_sriov_vf_port_vna.id = vsi_sriov.sriov_vf_port_id )
     THEN (0)
     ELSE (1) 
  END AS sriov_vf_is_free,

  CASE 
     WHEN (SELECT sriov_vf_port_id is not null FROM vsi_sriov WHERE v_net_sriov_vf_port_vna.id = vsi_sriov.sriov_vf_port_id )
     THEN (SELECT vnic.vm_id FROM vsi_sriov, vsi_profile, vnic WHERE v_net_sriov_vf_port_vna.id = vsi_sriov.sriov_vf_port_id AND 
            vsi_sriov.vsi_profile_id = vsi_profile.id AND vnic.vsi_profile_id  = vsi_profile.id )
     ELSE (NULL) 
  END AS vm_id
  
FROM 
  public.v_net_sriov_vf_port_vna, 
  public.v_net_sriov_port_vna
WHERE 
  v_net_sriov_vf_port_vna.sriov_port_id = v_net_sriov_port_vna.id;
  
  
  
CREATE OR REPLACE VIEW web_view_port_ip AS     
SELECT 
  port."name" AS port_name, 
  port.uuid AS port_uuid, 
  port.port_type AS port_type, 
  vna.vna_uuid, 
  port_ip.port_name AS port_ip_name, 
  port_ip.ip, 
  port_ip.mask, 
  port_ip.is_cfg AS port_ip_is_cfg, 
  port_ip.is_online AS port_ip_is_online
FROM 
  public.port, 
  public.port_ip, 
  public.vna
WHERE 
  port.vna_id = vna.id AND
  port_ip.port_id = port.id;
  
  
/*
CREATE OR REPLACE VIEW web_view_vna AS   
SELECT 
  vna.id AS vna_id, 
  vna.vna_uuid, 
  vna.vna_application, 
  vna.is_online AS vna_is_online, 
  regist_module.is_online AS hc_is_online , 
  regist_module.module_name AS hc_application, 
  (SELECT count(*) FROM port WHERE port.vna_id = vna.id ) AS port_total_num
FROM 
  vna, 
  regist_module
WHERE 
  regist_module.vna_id = vna.id AND 
  regist_module.role = 1;  
*/    
   

-- 26 web_vm.txt
CREATE OR REPLACE VIEW web_view_vm_netinfo AS 
 SELECT 
 v_net_vsi_vnic_resource.vnic_vm_id      AS vm_id, 
 v_net_vsi_vnic_resource.vnic_nic_index  AS nic_index, 
 v_net_vsi_vnic_resource.vsi_id_format, 
 v_net_vsi_vnic_resource.vsi_id_value, 
 v_net_vsi_vnic_resource.mac, 
 v_net_vsi_vnic_resource.is_has_ip, 
 v_net_vsi_vnic_resource.ip
   FROM 
 v_net_vsi_vnic_resource;
  

-- 27 web_wildcast_create_vport.txt
CREATE OR REPLACE VIEW web_view_wildcast_cfg_uplinkloopport AS    
SELECT 
   id, 
   uuid, 
   port_type,
   port_name
FROM wildcast_cfg_create_virtual_port
WHERE 
    port_type = 4;
    
    

CREATE OR REPLACE VIEW web_view_wildcast_cfg_kernelport AS    
SELECT 
   a.id, 
   a.uuid, 
   a.port_type,
   a.port_name,
   b.kernel_type,
   c.uuid AS kernel_pg_uuid,
   c.name AS kernel_pg_name,
   d.uuid AS switch_uuid,
   d.name AS switch_name,
   d.switch_type
FROM 
    wildcast_cfg_create_virtual_port a,
    wildcast_cfg_create_kernel_port b,
    port_group  c,
    switch d
WHERE 
    a.id = b.wildcast_create_vport_id AND 
    b.kernel_pg_id = c.id AND
    b.switch_id = d.id AND 
    a.port_type = 3    ;
    
    
    
CREATE OR REPLACE VIEW web_view_wildcast_task_uplinkloopport AS    
SELECT 
   a.uuid,    
   vna.vna_uuid ,
   
   CASE
       WHEN (SELECT y.cluster_name is not null FROM regist_module x,hc_info y WHERE y.regist_module_id = x.id AND vna.id  = x.vna_id AND x.role = 1 AND x.is_online = 1 )  
	   THEN (SELECT y.cluster_name FROM regist_module x,hc_info y  WHERE y.regist_module_id = x.id AND vna.id  = x.vna_id AND x.role = 1 AND x.is_online = 1 )
	   ELSE ('')
   END AS cluster_name,
   
   CASE
       WHEN (SELECT module_name is not null FROM regist_module x WHERE vna.id  = x.vna_id AND x.role = 1 AND x.is_online = 1 )  
	   THEN (SELECT module_name FROM regist_module x WHERE vna.id  = x.vna_id AND x.role = 1 AND x.is_online = 1 )
	   ELSE ('')
   END AS host_name,
/*   
   c.cluster_name,
   c.host_name,
*/   
   b.uuid AS wildcast_cfg_uplinkloopport_uuid,
   b.port_type,
   b.port_name,
   a.send_num,
   CASE 
      WHEN (a.state = 0 OR a.state = 2 OR a.send_num > 5) 
      THEN (1)
      ELSE (0)
   END AS is_can_delete 
FROM     
    wildcast_task_create_vport a,
    wildcast_cfg_create_virtual_port b,
    vna
WHERE 
    a.wildcast_cfg_create_vport_id = b.id AND 
    a.vna_id = vna.id AND 
    b.port_type = 4    ;
    
CREATE OR REPLACE VIEW web_view_wildcast_task_kernelport AS    
SELECT 
   a.uuid,    
   vna.vna_uuid,
   
   CASE
       WHEN (SELECT y.cluster_name is not null FROM regist_module x,hc_info y WHERE y.regist_module_id = x.id AND vna.id  = x.vna_id AND x.role = 1 AND x.is_online = 1 )  
	   THEN (SELECT y.cluster_name FROM regist_module x,hc_info y  WHERE y.regist_module_id = x.id AND vna.id  = x.vna_id AND x.role = 1 AND x.is_online = 1 )
	   ELSE ('')
   END AS cluster_name,
   
   CASE
       WHEN (SELECT module_name is not null FROM regist_module x WHERE vna.id  = x.vna_id AND x.role = 1 AND x.is_online = 1 )  
	   THEN (SELECT module_name FROM regist_module x WHERE vna.id  = x.vna_id AND x.role = 1 AND x.is_online = 1 )
	   ELSE ('')
   END AS host_name,
   
   
   b.uuid AS wildcast_cfg_kernelport_uuid,
   b.port_type,
   b.port_name,
   b.kernel_type,   
   b.kernel_pg_uuid,
   b.kernel_pg_name,
   b.switch_uuid,
   b.switch_name,
   b.switch_type,
   CASE 
      WHEN (a.state = 0 OR a.state = 2 OR (send_num > 5)) 
      THEN (1)
      ELSE (0)
   END AS is_can_delete 
FROM     
    wildcast_task_create_vport a,
    web_view_wildcast_cfg_kernelport b,
	vna 
	/*,
    v_net_vna_hc_cluster c */
WHERE 
    a.wildcast_cfg_create_vport_id = b.id AND 
    a.vna_id = vna.id AND  
    b.port_type = 3;    
    
-- 28 web_wildcast_loopback_port

CREATE OR REPLACE VIEW web_view_wildcast_cfg_loopback AS    
SELECT 
   id, 
   uuid, 
   port_name,
   is_loop
FROM wildcast_cfg_loopback_port ;


	
CREATE OR REPLACE VIEW web_view_wildcast_task_loopback AS    
SELECT 
   a.uuid,    
   vna.vna_uuid ,
   CASE
       WHEN (SELECT y.cluster_name is not null FROM regist_module x,hc_info y WHERE y.regist_module_id = x.id AND vna.id  = x.vna_id AND x.role = 1 AND x.is_online = 1 )  
	   THEN (SELECT y.cluster_name FROM regist_module x,hc_info y  WHERE y.regist_module_id = x.id AND vna.id  = x.vna_id AND x.role = 1 AND x.is_online = 1 )
	   ELSE ('')
   END AS cluster_name,
   
   CASE
       WHEN (SELECT module_name is not null FROM regist_module x WHERE vna.id  = x.vna_id AND x.role = 1 AND x.is_online = 1 )  
	   THEN (SELECT module_name FROM regist_module x WHERE vna.id  = x.vna_id AND x.role = 1 AND x.is_online = 1 )
	   ELSE ('')
   END AS host_name,
/*   
   c.cluster_name,
   c.host_name,
*/   
   b.uuid AS wildcast_cfg_uplinkloopport_uuid,
   b.port_name,
   a.send_num,
   CASE 
      WHEN (a.state = 0 OR a.state = 2 OR (send_num > 5)) 
	  THEN (1)
	  ELSE (0)
   END AS is_can_delete 
FROM 	
	wildcast_task_loopback a,
    wildcast_cfg_loopback_port b,
	vna
WHERE 
    a.wildcast_cfg_loopback_port_id = b.id AND 
	a.vna_id = vna.id ;

-- 29 web_wildcast_switch
/*
CREATE OR REPLACE VIEW web_view_wildcast_cfg_switch AS    
SELECT 
   a.id, 
   a.uuid, 
   a.switch_name,
   a.switch_type,
   a.state,
   a.evb_mode,
   a.pg_uuid,
   a.mtu_max,
   a.nic_max_num,
   a.port_type,
   a.port_name,
   a.bond_mode
FROM v_net_wildcast_switch a ;
*/

CREATE OR REPLACE VIEW web_view_wildcast_cfg_switch_bondmap AS  
SELECT 
   switch_uuid,
   switch_name,
   switch_type,
   uplink_port_name,
   uplink_port_type,
   bond_mode,
   phy_port_name,
   pg_uuid,
   pg_name   
FROM v_net_wildcast_switch_bondmap;


CREATE OR REPLACE VIEW web_view_wildcast_cfg_switch AS 
 SELECT 
 a.id, 
 a.uuid, 
 a.switch_name, 
 a.switch_type, 
 a.state, 
 a.evb_mode, 
 a.pg_uuid, 
 c.name AS pg_name,
 a.mtu_max, 
 a.nic_max_num,
 a.port_type, 
 a.port_name, 
 a.bond_mode,
 CASE
     WHEN ( (port_type = 0 OR port_type = 4) AND port_name <> '') THEN 1
     WHEN ( port_type = 1 ) THEN
     (SELECT count(*) FROM web_view_wildcast_cfg_switch_bondmap b WHERE b.switch_uuid = 
     a.uuid)
     ELSE 0
  END AS port_num
 FROM v_net_wildcast_switch a, port_group c
    WHERE c.uuid = a.pg_uuid;    


CREATE OR REPLACE VIEW web_view_wildcast_task_switch AS    
SELECT 
   a.uuid,    
   vna.vna_uuid ,
   CASE
       WHEN (SELECT y.cluster_name is not null FROM regist_module x,hc_info y WHERE y.regist_module_id = x.id AND vna.id  = x.vna_id AND x.role = 1 AND x.is_online = 1 )  
	   THEN (SELECT y.cluster_name FROM regist_module x,hc_info y  WHERE y.regist_module_id = x.id AND vna.id  = x.vna_id AND x.role = 1 AND x.is_online = 1 )
	   ELSE ('')
   END AS cluster_name,
   
   CASE
       WHEN (SELECT module_name is not null FROM regist_module x WHERE vna.id  = x.vna_id AND x.role = 1 AND x.is_online = 1 )  
	   THEN (SELECT module_name FROM regist_module x WHERE vna.id  = x.vna_id AND x.role = 1 AND x.is_online = 1 )
	   ELSE ('')
   END AS host_name,
/*   
   c.cluster_name,
   c.host_name,
*/   
   b.uuid AS wildcast_cfg_swtich_uuid,
   b.switch_name,
   b.switch_type,
   b.state AS switch_state,
   b.evb_mode AS switch_evb_mode,
   b.pg_uuid AS switch_pg_uuid,
   b.pg_name AS switch_pg_name,
   b.mtu_max AS switch_mut_max,
   b.nic_max_num AS switch_nic_max_num,
   b.port_type AS switch_uplinkport_type,
   b.port_name AS switch_uplinkport_name,
   b.bond_mode AS switch_uplinkport_bondmode,   
   a.send_num,
   CASE 
      WHEN (a.state = 0 OR a.state = 2 OR (send_num > 5)) 
	  THEN (1)
	  ELSE (0)
   END AS is_can_delete 
FROM 	
	wildcast_task_switch a,
    web_view_wildcast_cfg_switch b,
	vna
WHERE 
    a.wildcast_cfg_switch_id = b.id AND 
	a.vna_id = vna.id ;
	

CREATE VIEW v_net_quantum_network_logicnetwork AS 
SELECT 
  b.id as logicnetwork_id,
  b.uuid as logicnetwork_uuid, 
  a.id as quantumnetwork_id,
  a.uuid as quantumnetwork_uuid,
  
  CASE 
     WHEN (select c.project_id is not null from quantum_network_map c where a.id = c.quantum_network_id  ) THEN 
	 (select c.project_id from quantum_network_map c where a.id = c.quantum_network_id  )
	 ELSE (0)	 
  END as project_id
FROM 
  quantum_network a,
  logic_network b
WHERE 
  b.id = a.logic_network_id;


CREATE OR REPLACE VIEW web_view_netplane_segment_total_num AS 
 SELECT b.id AS np_id, sum(a.segment_end - a.segment_begin + 1) AS np_total_num
   FROM segment_range a, netplane b
  WHERE a.netplane_id = b.id
  GROUP BY b.id;

CREATE OR REPLACE VIEW web_view_netplane_segmentalloc AS 
 SELECT netplane.name AS netplane_name, netplane.uuid AS netplane_uuid, segment_pool.segment_num, x.project_id, x.vm_id, y.nic_index, t.isolate_no
   FROM netplane, segment_pool, segment_map t, vm x, vnic y, vsi_profile z
  WHERE netplane.id = segment_pool.netplane_id AND 
  t.segment_id = segment_pool.id AND
  x.vm_id = y.vm_id AND 
  y.vsi_profile_id = z.id AND 
  z.vm_pg_id = t.vm_pg_id AND 
  x.project_id = t.project_id
  ORDER BY netplane.uuid, x.project_id, segment_pool.segment_num;
  
CREATE OR REPLACE VIEW web_view_netplane_segment_alloc_num AS 
 SELECT b.id AS np_id, count(*) AS np_alloc_num
   FROM segment_pool a, netplane b
  WHERE a.netplane_id = b.id
  GROUP BY b.id;  
  
CREATE OR REPLACE VIEW web_view_netplane_res_statatic_segment AS 
 SELECT c.id AS np_id, c.name AS np_name, c.uuid AS np_uuid, a.np_total_num, 
        CASE
            WHEN ( SELECT b.np_alloc_num IS NOT NULL
               FROM web_view_netplane_segment_alloc_num b
              WHERE a.np_id = b.np_id AND b.np_id = c.id) THEN ( SELECT b.np_alloc_num
               FROM web_view_netplane_segment_alloc_num b
              WHERE a.np_id = b.np_id AND b.np_id = c.id)
            ELSE 0::bigint
        END AS np_alloc_num
   FROM web_view_netplane_segment_total_num a, netplane c
  WHERE a.np_id = c.id;

CREATE OR REPLACE VIEW web_view_netplane_res_statatic_segment AS 
 SELECT c.id AS np_id, c.name AS np_name, c.uuid AS np_uuid, a.np_total_num, 
        CASE
            WHEN ( SELECT b.np_alloc_num IS NOT NULL
               FROM web_view_netplane_segment_alloc_num b
              WHERE a.np_id = b.np_id AND b.np_id = c.id) THEN ( SELECT b.np_alloc_num
               FROM web_view_netplane_segment_alloc_num b
              WHERE a.np_id = b.np_id AND b.np_id = c.id)
            ELSE 0::bigint
        END AS np_alloc_num
   FROM web_view_netplane_segment_total_num a, netplane c
  WHERE a.np_id = c.id;



CREATE OR REPLACE VIEW web_view_netplane_segmentrange AS 
 SELECT netplane.name AS netplane_name, netplane.uuid AS netplane_uuid, segment_range.segment_begin, segment_range.segment_end
   FROM netplane, segment_range
  WHERE netplane.id = segment_range.netplane_id;


CREATE VIEW v_net_vna_vtep_switch_pg_share_segment AS 
SELECT 
    DISTINCT  
    c.id AS vna_id,
    c.vna_uuid, 
	c.is_online AS vna_is_online,
	b.id AS vtep_id,
	b.name AS vtep_name,
    b.uuid AS vtep_uuid,  
	b.is_cfg AS vtep_is_cfg,
	b.is_consistency AS vtep_is_consistency,
	b.is_online  AS vtep_is_online,
	i.id AS netplane_id,
	i.name AS netplane_name,
	i.uuid AS netplane_uuid,
/*		
	e.id AS switch_id,
	e.name AS switch_name,
	e.uuid AS switch_uuid,

	g.id AS uplink_pg_id,
	g.name AS uplink_pg_name,
	g.uuid AS uplink_pg_uuid,
	
	j.id AS vm_pg_id,
	j.name AS vm_pg_name,
	j.uuid AS vm_pg_uuid,
*/	
	o.segment_num,
	x.id AS ip_pool_id,
	x.ip,
	x.mask
from 
vtep_port a,
port b,
vna c,
virtual_switch d,
switch e,
uplink_pg f,
port_group g,
netplane i,
vm_pg h,
port_group j,
netplane k,
vm_pg_share_vxlan_segment o,
ip_pool x,
logic_network y,
vnic z
where 
a.port_id = b.id AND 
a.vna_id = c.id AND 
c.is_online = 1 AND 
d.switch_id = e.id AND 
a.virtual_switch_id = e.id AND 
f.pg_id = g.id AND 
d.uplink_pg_id = f.pg_id AND 
g.netplane_id = i.id AND 
h.pg_id = j.id AND 
j.netplane_id = k.id AND 
k.id = i.id AND 
o.vmpg_id = h.pg_id  AND
a.ip_pool_id = x.id  AND 
h.pg_id = y.vm_pg_id  AND 
y.uuid = z.logic_network_uuid 
ORDER BY vna_id, netplane_id,segment_num;



CREATE VIEW v_net_vna_vtep_switch_pg_private_segment AS 
SELECT 
    DISTINCT
    c.id AS vna_id,
    c.vna_uuid, 
	c.is_online AS vna_is_online,
	b.id AS vtep_id,
	b.name AS vtep_name,
    b.uuid AS vtep_uuid, 
	b.is_cfg AS vtep_is_cfg,
	b.is_consistency AS vtep_is_consistency,
	b.is_online  AS vtep_is_online,
	i.id AS netplane_id,
	i.name AS netplane_name,
	i.uuid AS netplane_uuid,
/*		
	e.id AS switch_id,
	e.name AS switch_name,
	e.uuid AS switch_uuid,

	g.id AS uplink_pg_id,
	g.name AS uplink_pg_name,
	g.uuid AS uplink_pg_uuid,
	
	j.id AS vm_pg_id,
	j.name AS vm_pg_name,
	j.uuid AS vm_pg_uuid,
*/	
	v.project_id,
	p.segment_num,
	x.id AS ip_pool_id,
	x.ip,
	x.mask
from 
vtep_port a,
port b,
vna c,
virtual_switch d,
switch e,
uplink_pg f,
port_group g,
netplane i,
vm_pg h,
port_group j,
netplane k,
vm_pg_private_vxlan_segment o,
segment_map v,
segment_pool p,
ip_pool x,
logic_network y,
vnic z
where 
a.port_id = b.id AND 
a.vna_id = c.id AND 
c.is_online = 1 AND 
d.switch_id = e.id AND 
a.virtual_switch_id = e.id AND 
f.pg_id = g.id AND 
d.uplink_pg_id = f.pg_id AND 
g.netplane_id = i.id AND 
h.pg_id = j.id AND 
j.netplane_id = k.id AND 
k.id = i.id AND 
o.vmpg_id = h.pg_id AND 
v.vm_pg_id = o.vmpg_id AND 
v.segment_id = p.id AND 
a.ip_pool_id = x.id AND 
h.pg_id = y.vm_pg_id  AND 
y.uuid = z.logic_network_uuid 
ORDER BY vna_id, netplane_id,segment_num;


CREATE OR REPLACE VIEW v_net_segmentmap_netplane_pg_segment AS 
SELECT 
  port_group."name" AS pg_name, 
  port_group.uuid AS pg_uuid, 
  segment_map.project_id AS project_id, 
  segment_pool.segment_num AS segment_num,   
  netplane."name" AS netplane_name, 
  netplane.uuid AS netplane_uuid,
  segment_map.isolate_no AS isolate_no
FROM 
  public.segment_map, 
  public.segment_pool, 
  public.vm_pg, 
  public.netplane, 
  public.port_group
WHERE 
  segment_map.vm_pg_id = vm_pg.pg_id AND
  segment_map.segment_id = segment_pool.id AND
  segment_pool.netplane_id = netplane.id AND
  vm_pg.pg_id = port_group.id AND
  port_group.netplane_id is not null;





  
	
	