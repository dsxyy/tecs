

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

-- 解决 v_net_vna_hc_cluster 中如果hc存在垃圾数据 数据显示问题  
------------------------------------------------------------------------------------
 
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
  VALUES ('01.01.10.0022', 'tecs-vnm_test',now(),'first version');
  
  

-- Function: pf_net_del_netplane_iprange(character varying, numeric, numeric, numeric, character varying, character varying, character varying)

-- DROP FUNCTION pf_net_del_netplane_iprange(character varying, numeric, numeric, numeric, character varying, character varying, character varying);

CREATE OR REPLACE FUNCTION pf_net_del_netplane_iprange(in_uuid character varying, in_num_begin numeric, in_num_end numeric, in_num_mask numeric, in_str_begin character varying, in_str_end character varying, in_str_mask character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  netplaneid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- begin end 
  IF in_num_begin > in_num_end THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  
  -- del 不作检查，由应用保准
      
  SELECT count(*) INTO is_exist FROM ip_range WHERE netplane_id = netplaneid 
                AND ip_begin_num = in_num_begin AND ip_end_num = in_num_end AND mask_num = in_num_mask 
                AND ip_begin = in_str_begin AND ip_end = in_str_end AND mask = in_str_mask;
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;                
        
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM ip_range WHERE netplane_id = netplaneid 
                AND ip_begin_num = in_num_begin AND ip_end_num = in_num_end AND mask_num = in_num_mask 
                AND ip_begin = in_str_begin AND ip_end = in_str_end AND mask = in_str_mask;
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