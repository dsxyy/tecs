

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

-- 解决 删除netplane时，要判断下segmentid是否已分配 在使用 
-- vxlan segment id range 显示有问题 
------------------------------------------------------------------------------------
 
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
  VALUES ('01.01.10.0019', 'tecs-vnm_test',now(),'first version');

  

CREATE OR REPLACE FUNCTION pf_net_integer_to_char(in_int integer)
  RETURNS character varying AS
$BODY$ 
DECLARE is_exist integer; 
    retchar character varying;    
    tmpchar character varying(8);
BEGIN 
    retchar := '';

    /*int32 最大值为999999999 */
    IF in_int > 999999999 THEN 
       retchar := to_char(in_int,'9999999999');
       return retchar;
    END IF;

    IF in_int > 99999999 THEN 
       retchar := to_char(in_int,'999999999');
       return retchar;
    END IF;

    IF in_int > 9999999 THEN 
       retchar := to_char(in_int,'99999999');
       return retchar;
    END IF;

    IF in_int > 999999 THEN 
       retchar := to_char(in_int,'9999999');
       return retchar;
    END IF;

    IF in_int > 99999 THEN 
       retchar := to_char(in_int,'999999');
       return retchar;
    END IF;

    IF in_int > 9999 THEN 
       retchar := to_char(in_int,'99999');
       return retchar;
    END IF;

    IF in_int > 999 THEN 
       retchar := to_char(in_int,'9999');
       return retchar;
    END IF;

    IF in_int > 99 THEN 
       retchar := to_char(in_int,'999');
       return retchar;
    END IF;

    IF in_int > 9 THEN 
       retchar := to_char(in_int,'99');
       return retchar;
    END IF;

    IF in_int > 0 THEN 
       retchar := to_char(in_int,'9');
       return retchar;
    END IF;
    
    return retchar;
END; 
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  
  

CREATE OR REPLACE FUNCTION pf_net_del_netplane(netplane_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  max_pg_mtu integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否存在记录
  SELECT count(*) INTO is_same FROM netplane WHERE uuid = $1 ;  
  IF is_same <= 0 THEN
     result.ret := RTN_ITEM_NO_EXIST;-- does not exist item
     return result;
  END IF;

  -- netplane 是否被占用  
  -- PG
  SELECT count(*) INTO is_exist FROM netplane,port_group WHERE netplane.uuid = $1 AND netplane.id = port_group.netplane_id;
  IF is_exist > 0 THEN
     result.ret := RTN_RESOURCE_USING;-- resource has been using
     return result;
  END IF;

  -- vlan_range 
  SELECT count(*) INTO is_exist FROM netplane,vlan_range WHERE netplane.uuid = $1 AND netplane.id = vlan_range.netplane_id;
  IF is_exist > 0 THEN
     result.ret := RTN_RESOURCE_USING;-- resource has been using
     return result;
  END IF;

  -- mac_pool 
  SELECT count(*) INTO is_exist FROM netplane,mac_pool WHERE netplane.uuid = $1 AND netplane.id = mac_pool.netplane_id;
  IF is_exist > 0 THEN
     result.ret := RTN_RESOURCE_USING;-- resource has been using
     return result;
  END IF;

  -- mac_range 
  SELECT count(*) INTO is_exist FROM netplane,mac_range WHERE netplane.uuid = $1 AND netplane.id = mac_range.netplane_id;
  IF is_exist > 0 THEN
     result.ret := RTN_RESOURCE_USING;-- resource has been using
     return result;
  END IF;

  -- vlan_pool 
  SELECT count(*) INTO is_exist FROM netplane,vlan_pool WHERE netplane.uuid = $1 AND netplane.id = vlan_pool.netplane_id;
  IF is_exist > 0 THEN
     result.ret := RTN_RESOURCE_USING;-- resource has been using
     return result;
  END IF;

  -- ip_range 
  SELECT count(*) INTO is_exist FROM netplane,ip_range WHERE netplane.uuid = $1 AND netplane.id = ip_range.netplane_id;
  IF is_exist > 0 THEN
     result.ret := RTN_RESOURCE_USING;-- resource has been using
     return result;
  END IF;
  
  /* segment range */  
  SELECT count(*) INTO is_exist FROM netplane,segment_range WHERE netplane.uuid = $1 AND netplane.id = segment_range.netplane_id;
  IF is_exist > 0 THEN
     result.ret := RTN_RESOURCE_USING;-- resource has been using
     return result;
  END IF;
  
  /* segment pool */  
  SELECT count(*) INTO is_exist FROM netplane,segment_pool WHERE netplane.uuid = $1 AND netplane.id = segment_pool.netplane_id;
  IF is_exist > 0 THEN
     result.ret := RTN_RESOURCE_USING;-- resource has been using
     return result;
  END IF;
   
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM netplane WHERE uuid = $1 ;
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;

  
  
CREATE OR REPLACE FUNCTION pf_net_add_portgroup(in_name character varying, in_pg_type integer, in_uuid character varying, in_acl character varying, in_qos character varying, in_allowed_priorities character varying, in_receive_bandwidth_limit character varying, 
in_receive_bandwidth_reserve character varying, in_default_vlan_id integer, in_promiscuous integer, in_macvlan_filter_enable integer, 
in_allowed_transmit_macs character varying, in_allowed_transmit_macvlans character varying, in_policy_block_override character varying,
 in_policy_vlan_override character varying, in_version integer, in_mgr_id character varying, in_type_id character varying, in_allow_mac_change integer, 
 in_switchport_mode integer, in_is_cfg_netplane integer, in_netplane_uuid character varying, in_mtu integer, in_trunk_native_vlan_num integer, 
 in_access_vlan_num integer, in_access_isolate integer, in_desc character varying, in_isolate_type integer, in_segment_id_num integer, 
 in_vxlan_isolate integer, in_is_sdn integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_same_pg integer;
  is_conflict integer;
  npmtu integer;
  pgid numeric(19,0);
  netplaneid numeric(19,0);
  is_exist integer; 
  db_pg_uuid character varying;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG integer := 5;
  RTN_PARAM_INVALID integer := 7;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  IF in_is_cfg_netplane > 0 THEN 
     SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_netplane_uuid;
     SELECT id INTO netplaneid FROM netplane WHERE uuid = in_netplane_uuid;
	 SELECT mtu INTO npmtu FROM netplane WHERE uuid = in_netplane_uuid;
     IF is_exist <= 0 THEN 
        result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
        return result;
     END IF;  
	 IF npmtu < in_mtu THEN 
	    result.ret := RTN_NETPLANE_MTU_LESS_PG;   -- PG MTU 不能大于 netplane mtu 
        return result;
	 END IF;
  END IF;
    
  -- 不支持重复添加 
  -- 判断name是否冲突
  SELECT count(*) INTO is_exist FROM port_group WHERE name = in_name;
  IF is_exist > 0 THEN 
     result.ret := RTN_ITEM_CONFLICT; -- conflict 
     return result;
  END IF;
  
  -- 检查switch_mode
  IF in_switchport_mode < 0 THEN 
    result.ret := RTN_PARAM_INVALID; -- param invalid 
    return result;   
  END IF;
  
  IF in_switchport_mode > 1  THEN 
    result.ret := RTN_PARAM_INVALID; -- param invalid  
    return result;   
  END IF;
  
  -- 检查pg_type 
  
  -- insert item
  result.id := nextval('serial_port_group');
  result.uuid := in_uuid;
    
  BEGIN 
    IF in_is_cfg_netplane > 0 THEN     
        INSERT INTO port_group(id,name,pg_type,uuid,acl,qos,allowed_priorities,
            receive_bandwidth_limit,receive_bandwidth_reserve,default_vlan_id,
            promiscuous,macvlan_filter_enable,allowed_transmit_macs,allowed_transmit_macvlans,
            policy_block_override,policy_vlan_override,version,mgr_id,type_id,
            allow_mac_change,switchport_mode,netplane_id, mtu, description) 
        values(result.id,in_name,in_pg_type,in_uuid,in_acl,in_qos,in_allowed_priorities,
            in_receive_bandwidth_limit,in_receive_bandwidth_reserve,in_default_vlan_id,
            in_promiscuous,in_macvlan_filter_enable,in_allowed_transmit_macs,in_allowed_transmit_macvlans,
            in_policy_block_override,in_policy_vlan_override,in_version,in_mgr_id,in_type_id,
            in_allow_mac_change,in_switchport_mode,netplaneid, in_mtu,in_desc);     
    ELSE 
       INSERT INTO port_group(id,name,pg_type,uuid,acl,qos,allowed_priorities,
            receive_bandwidth_limit,receive_bandwidth_reserve,default_vlan_id,
            promiscuous,macvlan_filter_enable,allowed_transmit_macs,allowed_transmit_macvlans,
            policy_block_override,policy_vlan_override,version,mgr_id,type_id,
            allow_mac_change,switchport_mode, mtu,description) 
        values(result.id,in_name,in_pg_type,in_uuid,in_acl,in_qos,in_allowed_priorities,
            in_receive_bandwidth_limit,in_receive_bandwidth_reserve,in_default_vlan_id,
            in_promiscuous,in_macvlan_filter_enable,in_allowed_transmit_macs,in_allowed_transmit_macvlans,
            in_policy_block_override,in_policy_vlan_override,in_version,in_mgr_id,in_type_id,
            in_allow_mac_change,in_switchport_mode, in_mtu,in_desc);    
    END IF;
    
    -- uplink pg
    IF in_pg_type = 0 THEN 
        INSERT INTO uplink_pg(pg_id,pg_type,isolate_type,is_sdn) values(result.id, in_pg_type, in_isolate_type,in_is_sdn);
    END IF;
    
    -- kernel_pg
    IF in_pg_type = 10 OR in_pg_type = 11 THEN 
      INSERT INTO kernel_pg values(result.id, in_pg_type);
    END IF;
    
    -- vm_pg
    IF in_pg_type = 21 THEN 
      INSERT INTO vm_pg values(result.id, in_pg_type, in_isolate_type,in_is_sdn);
	  IF in_isolate_type > 0 THEN 
	      INSERT INTO vm_pg_private_vxlan_segment values(result.id, in_vxlan_isolate);
	  END IF;
    END IF;

    IF in_pg_type = 20 THEN 
      INSERT INTO vm_pg values(result.id, in_pg_type, in_isolate_type);
	  IF in_isolate_type > 0 THEN 
          INSERT INTO vm_pg_share_vxlan_segment values(result.id, in_segment_id_num);
	  END IF;
    END IF;
    
    -- pg_switchport_trunk 
    IF in_switchport_mode = 0 THEN 
        INSERT INTO pg_switchport_trunk values(result.id, in_switchport_mode, in_trunk_native_vlan_num );
    END IF;
    
    IF (in_switchport_mode = 1) AND (in_isolate_type = 0 ) THEN 
      IF in_pg_type = 21 THEN 
          INSERT INTO pg_switchport_access_isolate values(result.id, in_pg_type,in_switchport_mode, in_access_isolate );      
      ELSE
          INSERT INTO pg_switchport_access_vlan values(result.id, in_pg_type,in_switchport_mode, in_access_vlan_num );      
      END IF;
    END IF; 
    
  EXCEPTION 
    WHEN others
    THEN
    result.ret := RTN_OPERATOR_FAILED;
  END;
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;

  