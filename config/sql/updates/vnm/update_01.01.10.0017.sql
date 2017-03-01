

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

-- 解决 vtep 占用 ip地址的情况下，ip range 应该显示已经在使用
--
------------------------------------------------------------------------------------
 
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
  VALUES ('01.01.10.0017', 'tecs-vnm_test',now(),'first version');

  
  
CREATE OR REPLACE FUNCTION pf_net_query_netplane_iprange_is_using(in_netplane_uuid character varying, in_num_begin numeric, in_num_end numeric)
  RETURNS t_query_total AS
$BODY$
DECLARE
  result  t_query_total;
  is_conflict integer;
  netplaneid numeric(19,0);
  is_exist integer; 
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
    
  -- 查询vtep ip range 是否已有分配的
  
   -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_netplane_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_netplane_uuid;
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
  
  -- begin end 
  IF in_num_begin > in_num_end THEN 
      result.ret  := RTN_PARAM_INVALID; 
      return result;
  END IF;
    
  SELECT count(*) INTO result.total_num FROM ip_pool WHERE netplane_id = netplaneid 
                AND (ip_num >= in_num_begin AND ip_num <= in_num_end );
				
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  
  