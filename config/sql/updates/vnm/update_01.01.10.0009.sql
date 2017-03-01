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

-- 解决 611003904475 【TECS】端口组mtu修改某种条件下判断缺失 
--
------------------------------------------------------------------------------------
 
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
  VALUES ('01.01.10.0009', 'tecs-vnm_test',now(),'first version');

  

CREATE OR REPLACE FUNCTION pf_net_modify_portgroup(in_name character varying, in_pg_type integer,
        in_uuid character varying, in_acl character varying, in_qos character varying,in_allowed_priorities character varying,
        in_receive_bandwidth_limit character varying, in_receive_bandwidth_reserve character varying, 
        in_default_vlan_id integer, in_promiscuous integer, in_macvlan_filter_enable integer, 
        in_allowed_transmit_macs character varying, in_allowed_transmit_macvlans character varying, 
        in_policy_block_override character varying, in_policy_vlan_override character varying, 
        in_version integer,in_mgr_id character varying, in_type_id character varying,
        in_allow_mac_change integer, in_switchport_mode integer,
        in_is_cfg_netplane integer, in_netplane_uuid character varying,        
        in_mtu integer,
        in_trunk_native_vlan_num integer,in_access_vlan_num integer,in_access_isolate integer, in_desc character varying  )
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  is_modify_netplane integer;
  npmtu integer;
  netplaneid numeric(19,0);
  oldnetplaneid numeric(19,0);
  pgid numeric(19,0);
  db_pg_type integer;
  ismodifyswitchportmode integer;
  oldswitchportmode integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG integer := 5;
  RTN_PARAM_INVALID integer := 7;
BEGIN
  result.ret := 0;
  
  -- 鍏ュ弬妫€鏌?鍚庨潰鍋? 
  -- netplane check
  IF in_is_cfg_netplane > 0 THEN 
     SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_netplane_uuid;
     SELECT id INTO netplaneid FROM netplane WHERE uuid = in_netplane_uuid;
     IF is_exist <= 0 THEN 
        result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
        return result;
     END IF;  
  END IF;
    
  -- uuid check
  SELECT count(*) INTO is_exist FROM port_group WHERE uuid = in_uuid;
  SELECT pg_type INTO db_pg_type FROM port_group WHERE uuid = in_uuid;  
  SELECT id INTO pgid FROM port_group WHERE uuid = in_uuid;  
  SELECT switchport_mode INTO oldswitchportmode FROM port_group WHERE uuid = in_uuid;   
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST; -- conflict 
     return result;
  END IF;
  
  -- pg_type check 
  SELECT count(*) INTO is_exist FROM port_group WHERE uuid = in_uuid AND db_pg_type = in_pg_type;
  IF is_exist <= 0 THEN 
     result.ret := RTN_PARAM_INVALID; -- param invalid 
     return result;
  END IF;
    
  -- 鍒ゆ柇name鏄惁鍐茬獊
  SELECT count(*) INTO is_exist FROM port_group WHERE name = in_name AND  uuid = in_uuid;
  IF is_exist <= 0 THEN 
     SELECT count(*) INTO is_exist FROM port_group WHERE name = in_name;
     IF is_exist > 0 THEN 
        result.ret := RTN_ITEM_CONFLICT; -- conflict 
        return result;
     END IF;
  END IF;
  
  -- 妫€鏌witch_mode
  IF in_switchport_mode < 0 THEN 
    result.ret := RTN_PARAM_INVALID; -- param invalid 
    return result;   
  END IF;
  
  IF in_switchport_mode > 1  THEN 
    result.ret := RTN_PARAM_INVALID; -- param invalid  
    return result;   
  END IF;
  
  -- 妫€鏌?switch_mode 鏄惁鍙樺寲
  IF in_switchport_mode = oldswitchportmode THEN 
      ismodifyswitchportmode := 0;
  ELSE
      ismodifyswitchportmode := 1;
  END IF;
  
  -- 妫€鏌g_type 
  
  -- 妫€鏌etplane modify锛屼箣鍓嶅凡缁忛厤缃簡netplane锛岃兘灏嗕箣淇敼锛屼絾蹇呴』mtu >= pg_mtu
  is_modify_netplane := 0;
  SELECT count(*) INTO is_exist FROM port_group WHERE uuid = in_uuid AND netplane_id is not null;
  SELECT netplane_id INTO oldnetplaneid FROM port_group WHERE uuid = in_uuid AND netplane_id is not null; 
  IF is_exist > 0 THEN 
     IF in_is_cfg_netplane <= 0 THEN 
        result.ret := RTN_PARAM_INVALID; -- param invalid  鍘熷厛閰嶇疆netplane锛屼笉鑳藉皢涔嬩慨鏀逛负NULL
        return result; 
     ELSE        
		SELECT mtu INTO npmtu FROM netplane WHERE id = netplaneid;
        IF npmtu < in_mtu THEN 
            result.ret := RTN_NETPLANE_MTU_LESS_PG; -- param invalid  netplane mtu 灏忎簬pg_mtu锛屼笉鑳藉皢涔嬩慨鏀?
            return result;
        END IF;    
     END IF;
  END IF; 
  
  IF in_is_cfg_netplane > 0 THEN 
     SELECT mtu INTO npmtu FROM netplane WHERE id = netplaneid;
     IF npmtu < in_mtu THEN 
          result.ret := RTN_NETPLANE_MTU_LESS_PG; -- param invalid  netplane mtu 
          return result;
     END IF; 
  END IF;
  
          
  -- modify item
  result.id  := pgid;
  result.uuid := in_uuid;
  BEGIN 
    
    -- 灏嗚€佺殑switch mode 鏁版嵁鍒犻櫎
    IF ismodifyswitchportmode > 0 THEN 
       -- 淇敼switchport mode闇€瑕佸垹闄や互鍓嶈褰?
       IF oldswitchportmode = 0 THEN 
            DELETE FROM pg_switchport_trunk WHERE pg_id = pgid;            
       ELSE 
          IF in_pg_type = 21 THEN 
              DELETE FROM pg_switchport_access_isolate WHERE pg_id = pgid;
          ELSE 
              DELETE FROM pg_switchport_access_vlan WHERE pg_id = pgid;
          END IF;
       END IF;    
    END IF;
    
    IF in_is_cfg_netplane > 0 THEN     
       UPDATE port_group SET name = in_name, pg_type = in_pg_type,acl = in_acl,qos = in_qos,allowed_priorities = in_allowed_priorities,
            receive_bandwidth_limit = in_receive_bandwidth_limit,receive_bandwidth_reserve = in_receive_bandwidth_reserve ,default_vlan_id = in_default_vlan_id,
            promiscuous = in_promiscuous,macvlan_filter_enable = in_macvlan_filter_enable,allowed_transmit_macs = in_allowed_transmit_macs,
            allowed_transmit_macvlans = in_allowed_transmit_macvlans,
            policy_block_override = in_policy_block_override,policy_vlan_override = in_policy_vlan_override,version = in_version,mgr_id = in_mgr_id,type_id = in_type_id,
            allow_mac_change = in_allow_mac_change ,switchport_mode = in_switchport_mode, netplane_id = netplaneid, mtu = in_mtu, description = in_desc    
          WHERE uuid = in_uuid ;    
    ELSE 
       UPDATE port_group SET name = in_name, pg_type = in_pg_type,acl = in_acl,qos = in_qos,allowed_priorities = in_allowed_priorities,
            receive_bandwidth_limit = in_receive_bandwidth_limit,receive_bandwidth_reserve = in_receive_bandwidth_reserve ,default_vlan_id = in_default_vlan_id,
            promiscuous = in_promiscuous,macvlan_filter_enable = in_macvlan_filter_enable,allowed_transmit_macs = in_allowed_transmit_macs,
            allowed_transmit_macvlans = in_allowed_transmit_macvlans,
            policy_block_override = in_policy_block_override,policy_vlan_override = in_policy_vlan_override,version = in_version,mgr_id = in_mgr_id,type_id = in_type_id,
            allow_mac_change = in_allow_mac_change ,switchport_mode = in_switchport_mode, mtu = in_mtu, description = in_desc 
          WHERE uuid = in_uuid ;  
    END IF;
    
    
    -- 
    -- pg_switchport_trunk 
    IF in_switchport_mode = 0 THEN 
        IF ismodifyswitchportmode > 0 THEN 
            INSERT INTO pg_switchport_trunk values(pgid, in_switchport_mode, in_trunk_native_vlan_num );
        ELSE 
            UPDATE pg_switchport_trunk SET native_vlan_num = in_trunk_native_vlan_num WHERE pg_id = pgid;
        END IF;
    END IF;
    
    -- PG access
    IF in_switchport_mode = 1 THEN 
      IF in_pg_type = 21 THEN 
          IF ismodifyswitchportmode > 0 THEN 
              INSERT INTO pg_switchport_access_isolate values(pgid, in_pg_type,in_switchport_mode, in_access_isolate );      
          ELSE
              UPDATE pg_switchport_access_isolate SET isolate_no = in_access_isolate  WHERE pg_id = pgid;  
          END IF;
      ELSE
          IF ismodifyswitchportmode > 0 THEN 
              INSERT INTO pg_switchport_access_vlan values(pgid, in_pg_type,in_switchport_mode, in_access_vlan_num );    
          ELSE
              UPDATE pg_switchport_access_vlan SET vlan_num = in_access_vlan_num  WHERE pg_id = pgid; 
          END IF;          
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
  