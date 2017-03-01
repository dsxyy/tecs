

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

-- 解决 logicname -> uuid 存贮过程没有合入 
-- 
------------------------------------------------------------------------------------
 
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
  VALUES ('01.01.10.0026', 'tecs-vnm_test',now(),'first version');

  
CREATE OR REPLACE FUNCTION pf_net_query_logicnetwork_by_name(in_ln_name character varying)
  RETURNS t_logicnetwork_query AS
$BODY$
DECLARE
    result t_logicnetwork_query;
    is_exist integer; 
    pgid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    result.ret := 0;
    
    -- logic_network uuid 
    SELECT count(*) INTO is_exist FROM logic_network WHERE name = in_ln_name;
    SELECT vm_pg_id INTO pgid FROM logic_network WHERE name = in_ln_name;
    IF is_exist <= 0 THEN 
         result.ret := RTN_ITEM_NO_EXIST; 
         return result;
    END IF;  
    
    result.name  := in_ln_name;
    -- SELECT name INTO result.name FROM  logic_network WHERE uuid = in_uuid;
	SELECT uuid INTO result.uuid FROM  logic_network WHERE name = in_ln_name;

    
    -- pg uuid 
    SELECT uuid INTO result.pg_uuid FROM port_group WHERE id = pgid;
  
    SELECT description INTO result.des FROM  logic_network WHERE name = in_ln_name;
    
    SELECT ip_bootprotocolmode INTO result.ipmode FROM  logic_network WHERE name = in_ln_name;
    
    return result;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;