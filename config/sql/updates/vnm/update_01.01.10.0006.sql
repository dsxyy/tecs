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

-- 增加vswitch_map 查询接口（通过port查询switch）
--
------------------------------------------------------------------------------------
 
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
  VALUES ('01.01.10.0006', 'tecs-vnm_test',now(),'first version');


--- 通过port 查询switch 
CREATE OR REPLACE FUNCTION pf_net_query_vswitchmap_by_port(in_port_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer := 0; 
	porttype integer := 0;
    RTN_ITEM_NO_EXIST integer := 4;
	RTN_PARAM_INVALID integer := 7;
BEGIN
    -- port uuid 
    SELECT count(*) INTO is_exist FROM port WHERE uuid = in_port_uuid;
	SELECT port_type INTO porttype FROM port WHERE uuid = in_port_uuid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF;  
	        
    -- 
    OPEN ref FOR SELECT switch_uuid,switch_name,switch_type,
        vna_uuid, vna_application,vnm_application,host_name,
        port_uuid,port_name,port_type ,switch_map_is_cfg,switch_map_is_selected,switch_map_is_consistency 
        FROM v_net_switch_port_vna WHERE port_uuid = in_port_uuid;
        
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;  
  
