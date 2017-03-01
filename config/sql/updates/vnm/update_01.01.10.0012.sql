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

-- 解决 没有配置logicnetwork ip range 的情况下，判断ip地址有效范围
--
------------------------------------------------------------------------------------
 
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
  VALUES ('01.01.10.0012', 'tecs-vnm_test',now(),'first version');


CREATE OR REPLACE FUNCTION pf_net_check_schedule_resource_ip(in_logicnetwork_uuid character varying, in_ip_num integer)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  total_num numeric(19,0) := 0;
  alloc_num numeric(19,0) := 0;
  free_num numeric(19,0) := 0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE integer := 13;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
  RTN_NO_MAC_RESOURCE  integer := 15;
  RTN_NO_IP_RESOURCE  integer := 16;
BEGIN
  result := 0;

  SELECT count(*) INTO is_exist FROM logic_network b WHERE b.uuid = in_logicnetwork_uuid AND b.ip_bootprotocolmode = 0;
  IF is_exist <= 0 THEN 
      /* result := RTN_NO_MAC_RESOURCE;*/
      return result;
  END IF;

  SELECT count(*) INTO is_exist FROM logic_network_ip_range a, logic_network b WHERE a.logic_network_id = b.id AND b.uuid = in_logicnetwork_uuid AND b.ip_bootprotocolmode = 0;
  IF is_exist <= 0 THEN 
      result := RTN_NO_IP_RESOURCE;
      return result;
  END IF;
  
  SELECT count(*) INTO is_exist FROM logic_network_ip_range a, logic_network b WHERE a.logic_network_id = b.id AND b.uuid = in_logicnetwork_uuid;
  SELECT sum(ip_end_num - ip_begin_num +1) INTO total_num  FROM logic_network_ip_range a, logic_network b WHERE a.logic_network_id = b.id AND b.uuid = in_logicnetwork_uuid;
  IF is_exist <= 0 THEN 
      result := RTN_NO_IP_RESOURCE;
      return result;
  END IF;
  
  SELECT count(*) INTO alloc_num FROM logic_network_ip_pool a, logic_network b WHERE a.logic_network_id = b.id AND b.uuid = in_logicnetwork_uuid;
  free_num := (total_num - alloc_num);
  free_num := (free_num - in_ip_num);
  IF free_num < 0 THEN 
      result := RTN_NO_IP_RESOURCE;
      return result;
  END IF;
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;

