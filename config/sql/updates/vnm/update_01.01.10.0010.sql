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

-- 解决 611003904470 【TECS】设置为自环模式的SR-IOV网口在加入交换后，仍能取消自环设置 
--
------------------------------------------------------------------------------------
 
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
  VALUES ('01.01.10.0010', 'tecs-vnm_test',now(),'first version');

  
CREATE OR REPLACE FUNCTION pf_net_check_sriovport_loopback(in_uuid character 
varying, in_is_loop integer)

  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  portid numeric(19,0);
  bondid numeric(19,0);  
  switchid numeric(19,0);   
  vnaid numeric(19,0);
  pg_mtu integer;
  portname character varying;
  -- 閿欒杩斿洖鐮佸畾涔?
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
BEGIN
  result := 0;
  -- 鍏ュ弬妫€鏌?鍚庨潰鍋?
  
  -- 妫€鏌ort uuid 鏄惁鏈夋晥
  SELECT count(*) INTO is_exist FROM port a, sriov_port b WHERE a.uuid = 
in_uuid AND a.id = b.physical_port_id;

  SELECT a.vna_id INTO vnaid FROM port a, sriov_port b WHERE a.uuid = in_uuid 
AND a.id = b.physical_port_id;  

  SELECT a.id INTO portid FROM port a, sriov_port b WHERE a.uuid = in_uuid 
AND a.id = b.physical_port_id;

  SELECT a.name INTO portname FROM port a, sriov_port b WHERE a.uuid = 
in_uuid AND a.id = b.physical_port_id;

  SELECT a.id INTO portid FROM port a, sriov_port b WHERE a.uuid = in_uuid 
AND a.id = b.physical_port_id;

  IF is_exist <= 0 THEN 
    result := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  

  -- 妫€鏌ヨ缃戝彛涓嬫槸鍚﹀瓨鍦╒M 浣跨敤VF
  SELECT count(*) INTO is_exist FROM sriov_vf_port a, vsi_sriov b WHERE 
  a.port_id  = b.sriov_vf_port_id AND a.sriov_port_id = portid;
  IF is_exist > 0 THEN 
      result := RTN_RESOURCE_USING;   -- vm using
      return result;
  END IF;

  -- 妫€鏌ort鏄惁鍔犲叆浜哹ond锛屽姞鍏ond鍚庯紝涓嶈兘淇敼鍏剁幆鍥炲睘鎬?
  SELECT count(*) INTO is_exist FROM bond_map WHERE physical_port_id = portid; 
  IF is_exist > 0 THEN 
    result := RTN_PARAM_INVALID;   -- item has not exist.
    return result;
  END IF; 

  -- 濡傛灉璇ョ綉鍙ｅ凡缁忓姞鍏DVS EDVS 灏变笉鑳藉啀璁剧疆涓簂oop
 
  SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
WHERE a.virtual_switch_id = b.switch_id AND a.port_id = portid;

  IF is_exist > 0 THEN 
    result := RTN_PARAM_INVALID;   
    return result;
  END IF;
  
  /*濡傛灉璇ョ綉鍙ｅ凡缁忓姞鍏?wildcast loop task, 
  涔熷皢涓嶈兘杩涜loop璁剧疆 */

  SELECT count(*) INTO is_exist 
  FROM wildcast_task_loopback a, 
  wildcast_cfg_loopback_port b 
  WHERE 
     a.wildcast_cfg_loopback_port_id = b.id AND 
     a.vna_id = vnaid AND 
     b.port_name = portname; 
  IF is_exist > 0 THEN      
     result := RTN_ITEM_CONFLICT;   
    return result;
  END IF;

  /* 濡傛灉璇ョ綉鍙ｅ凡缁忓姞鍏?wildcast switch task,  switch port name 瀛樺湪涓や釜鍦版柟 
浠呭BOND鍙ｉ渶闄愬埗 */

  SELECT count(*) INTO is_exist 
  FROM wildcast_task_switch a, 
  wildcast_cfg_switch_bond_map b 
  WHERE 
     a.wildcast_cfg_switch_id = b.wildcast_cfg_bond_id AND 
     a.vna_id = vnaid AND 
     b.phy_port_name = portname; 
  IF is_exist > 0 THEN      
     result := RTN_ITEM_CONFLICT;   
    return result;
  END IF;

  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;