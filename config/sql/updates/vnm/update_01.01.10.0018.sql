

------------------------------------------------------------------------------------
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

-- 解决 查询portgroup时，vxlan的隔离号 获取的不对
--
------------------------------------------------------------------------------------
 
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
  VALUES ('01.01.10.0018', 'tecs-vnm_test',now(),'first version');

  

CREATE OR REPLACE FUNCTION pf_net_query_portgroup(in_uuid character varying)
  RETURNS t_portgroup_query AS
$BODY$
DECLARE
  result  t_portgroup_query;
  is_exist integer;
  pgid numeric(19,0);
  iscfgnetplane integer;
  pgtype integer;
  swithmode integer;
  RTN_ITEM_NO_EXIST integer :=4;
BEGIN

  result.ret := 0;
  SELECT count(*) INTO is_exist FROM port_group WHERE uuid = in_uuid;
  SELECT id INTO pgid FROM port_group WHERE uuid = in_uuid;
  SELECT pg_type INTO pgtype FROM port_group WHERE uuid = in_uuid;
  SELECT switchport_mode INTO swithmode FROM port_group WHERE uuid = in_uuid; 
  IF is_exist > 0 THEN
     result.ret := 0;
  ELSE
     result.ret := RTN_ITEM_NO_EXIST; -- no exist item
     return result;     
  END IF;
   
   result.uuid := in_uuid;

   -- port_group 
   SELECT pg_type into result.pg_type  FROM port_group WHERE uuid = in_uuid;
   SELECT name into result.name  FROM port_group WHERE uuid = in_uuid;
   SELECT acl into result.acl FROM port_group WHERE uuid = in_uuid;
   SELECT qos into result.qos  FROM port_group WHERE uuid = in_uuid;
   SELECT allowed_priorities into result.allowed_priorities  FROM port_group WHERE uuid = in_uuid;
   SELECT receive_bandwidth_limit into result.receive_bandwidth_limit  FROM port_group WHERE uuid = in_uuid;
   SELECT receive_bandwidth_reserve into result.receive_bandwidth_reserve  FROM port_group WHERE uuid = in_uuid;
      
   SELECT default_vlan_id into result.default_vlan_id  FROM port_group WHERE uuid = in_uuid;
   SELECT promiscuous into result.promiscuous  FROM port_group WHERE uuid = in_uuid;
   SELECT macvlan_filter_enable into result.macvlan_filter_enable  FROM port_group WHERE uuid = in_uuid;
   SELECT allowed_transmit_macs into result.allowed_transmit_macs  FROM port_group WHERE uuid = in_uuid;
   SELECT allowed_transmit_macvlans into result.allowed_transmit_macvlans  FROM port_group WHERE uuid = in_uuid;
   SELECT policy_block_override into result.policy_block_override  FROM port_group WHERE uuid = in_uuid;
   SELECT policy_vlan_override into result.policy_vlan_override  FROM port_group WHERE uuid = in_uuid;
   SELECT version into result.version  FROM port_group WHERE uuid = in_uuid;
   SELECT mgr_id into result.mgr_id  FROM port_group WHERE uuid = in_uuid;
   SELECT type_id into result.type_id  FROM port_group WHERE uuid = in_uuid;
   SELECT allow_mac_change into result.allow_mac_change  FROM port_group WHERE uuid = in_uuid;
   SELECT switchport_mode into result.switchport_mode  FROM port_group WHERE uuid = in_uuid;
   
   SELECT count(*) INTO is_exist FROM port_group WHERE  uuid = in_uuid AND netplane_id is not null;
   SELECT count(*) INTO is_exist FROM port_group WHERE  uuid = in_uuid AND netplane_id is not null;
   IF is_exist > 0 THEN 
       result.is_cfg_netplane := 1;
       SELECT a.uuid INTO result.netplane_uuid FROM netplane a, port_group b WHERE a.id = b.netplane_id AND b.uuid = in_uuid;
   ELSE 
       result.is_cfg_netplane := 0;
   END IF;
   
   SELECT mtu into result.mtu FROM port_group WHERE uuid = in_uuid;
   SELECT description into result.description FROM port_group WHERE uuid = in_uuid;
    
   -- trunk 
   IF swithmode = 0 THEN 
      SELECT native_vlan_num INTO result.trunk_native_vlan_num FROM pg_switchport_trunk WHERE pg_id = pgid;   
   END IF;
   
   -- access 
   IF swithmode = 1 THEN 
      IF pgtype = 21 THEN 
          SELECT isolate_no INTO result.access_isolate_no FROM pg_switchport_access_isolate WHERE pg_id = pgid;   
      ELSE 
          SELECT vlan_num INTO result.access_vlan_num FROM pg_switchport_access_vlan WHERE pg_id = pgid;   
      END IF;
   END IF;

   /*vxlan sdn */
   IF pgtype = 0 THEN 
      
	   SELECT is_sdn into result.is_sdn FROM uplink_pg WHERE pg_id = pgid AND pg_type = result.pg_type;
	   SELECT isolate_type into result.isolate_type FROM uplink_pg WHERE pg_id = pgid AND pg_type = result.pg_type;
	   
   END IF;
   
   IF pgtype = 21 OR pgtype = 20 THEN 
       SELECT is_sdn into result.is_sdn FROM vm_pg WHERE pg_id = pgid AND pg_type = result.pg_type;
       SELECT isolate_type into result.isolate_type FROM vm_pg WHERE pg_id = pgid AND pg_type = result.pg_type;
       -- RAISE WARNING '------- in_uuid:%   pg_type:%   isolatetype:%', in_uuid, result.pg_type, result.isolate_type;
       IF result.isolate_type = 1 THEN 
          IF pgtype = 21 THEN 
              SELECT isolate_no INTO result.vxlan_isolate_no FROM vm_pg_private_vxlan_segment WHERE vmpg_id = pgid;   
          ELSE 
              SELECT segment_num INTO result.segment_num FROM vm_pg_share_vxlan_segment WHERE vmpg_id = pgid;   
          END IF;
       END IF;
   END IF;

  return result;
END$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  