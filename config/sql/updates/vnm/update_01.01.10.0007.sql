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

-- 解决在检查switch 混用的情况，pg不能修改的检查有无
--
------------------------------------------------------------------------------------
 
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
  VALUES ('01.01.10.0007', 'tecs-vnm_test',now(),'first version');

-- Function: pf_net_can_modify_vswitch(integer, character varying, character varying, integer, integer, integer, character varying, integer, integer)

-- DROP FUNCTION pf_net_can_modify_vswitch(integer, character varying, character varying, integer, integer, integer, character varying, integer, integer);

CREATE OR REPLACE FUNCTION pf_net_can_modify_vswitch(in_switch_type integer, in_name character varying, in_uuid character varying, in_state integer, 
in_max_mtu integer, in_evb_mode integer, in_pg_uuid character varying, in_max_nic_num integer, in_is_active integer)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  switchid numeric(19,0);
  switchtype integer;
  is_exist integer; 
  uplinkportid numeric(19,0);
  oldpgid numeric(19,0);
  pgid numeric(19,0);
  pg_mtu integer;
  maxkernelmtu integer;
  muplinkportlists RECORD;
  /*错误返回码定义*/
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_UPLINKPGMTU_LESS_KERNELPG integer := 18;
BEGIN
  result := 0;
  -- 入参检查 后面做


  -- 检查pg uuid 是否有效
  SELECT count(*) INTO is_exist FROM port_group a, uplink_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type AND a.uuid = in_pg_uuid;
  SELECT a.id INTO pgid FROM port_group a, uplink_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type AND a.uuid = in_pg_uuid;  
  SELECT a.mtu INTO pg_mtu FROM port_group a, uplink_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type AND a.uuid = in_pg_uuid;  
  IF is_exist <= 0 THEN 
    result := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  -- switch type
  IF in_switch_type < 1 THEN 
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;
  
  IF in_switch_type > 2 THEN 
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;
  
  -- swtich uuid 
  SELECT count(*) INTO is_exist FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_uuid;
  SELECT a.switch_type INTO switchtype FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_uuid;    
  SELECT b.uplink_pg_id INTO oldpgid FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_uuid;    
  SELECT a.id INTO switchid FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_uuid;            
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
  
  -- switch type 不能修改 
  IF switchtype = in_switch_type THEN 
      result := 0;
  ELSE
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  
  -- 检查switch name 是否冲突 
  SELECT count(*) INTO is_exist FROM switch WHERE uuid = in_uuid AND name = in_name;
  IF is_exist <= 0 THEN 
     SELECT count(*) INTO is_exist FROM switch WHERE name = in_name;
     IF is_exist > 0 THEN 
        result := RTN_ITEM_CONFLICT;   
        return result;
     END IF;
  END IF;
  
  /* check max_mtu 
  IF in_max_mtu > pg_mtu THEN 
     result := RTN_SWITCH_MTU_LESS_PG;   
     return result;
  END IF;
  */

  
  
  --如果swtich存在虚拟nic的情况下，不能修改pg_id 
  SELECT count(*) INTO is_exist FROM virtual_switch a, vsi_profile b WHERE a.switch_id = b.virtual_switch_id AND  a.switch_id = switchid;

  IF is_exist > 0 THEN 
       IF oldpgid = pgid THEN 
           result :=0;
       ELSE
           result := RTN_ITEM_CONFLICT;   
           return result;
       END IF;
  END IF;


  --如果swtich的uplink 在SDVS EDVS混用的情况下，不能修改pg_id 
  SELECT count(*) INTO is_exist FROM virtual_switch_map WHERE virtual_switch_id = switchid;
  IF is_exist > 0 THEN 
 
      FOR muplinkportlists IN SELECT port_id , virtual_switch_id FROM virtual_switch_map 
                 WHERE virtual_switch_id = switchid ORDER BY port_id  LOOP
         SELECT count(*) INTO is_exist FROM virtual_switch_map WHERE port_id  = muplinkportlists.port_id;
         -- is_exist > 1 说明存在混用情况 
        
         IF is_exist > 1 THEN 
             IF oldpgid = pgid THEN 
                 result :=0;
             ELSE
                 result := RTN_ITEM_CONFLICT;   
                 return result;
             END IF;
         END IF;
        
      END LOOP;       
  END IF;
  
  /*
  如果switch存在kernel port的情况下，uplink pg mtu 必须大于 kernel pg mtu 
  */
  SELECT MAX(b.mtu) INTO maxkernelmtu 
  FROM kernel_port a, port_group b 
  WHERE a.virtual_switch_id = switchid AND a.kernel_pg_id = b.id ;
  
  SELECT count(*) INTO is_exist 
  FROM kernel_port a, port_group b 
  WHERE a.virtual_switch_id = switchid AND a.kernel_pg_id = b.id ;
  
  IF is_exist > 0 THEN 
      IF maxkernelmtu > pg_mtu THEN        
          result := RTN_UPLINKPGMTU_LESS_KERNELPG;   
          return result;
      END IF;
  END IF;
  
  /*看看是否需要判断virtual_switch_map表*/
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
