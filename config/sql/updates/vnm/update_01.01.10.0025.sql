

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

-- 解决 611003932854 【TECS】网络平面配置private类型vxlan pg还允许删除其下的segmentid未作限制 
------------------------------------------------------------------------------------
 
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
  VALUES ('01.01.10.0025', 'tecs-vnm_test',now(),'first version');


CREATE OR REPLACE FUNCTION pf_net_query_vm_vxlan_private_pg_isolate_no_list_by_netplane(in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    netplaneid numeric(19,0);
BEGIN
     -- netplane uuid 
    SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
    SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
       --result := RTN_ITEM_NO_EXIST; 
       return ref;
    END IF;   
    
    OPEN ref FOR 
    SELECT isolate_no
    FROM port_group a, vm_pg_private_vxlan_segment b 
    WHERE a.id = b.vmpg_id AND a.pg_type = 21 AND a.netplane_id = netplaneid;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  

