

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

-- 解决 sriov vf 存在垃圾数据情况下，将不可用的sriov vf 分配出去   
-- dpdk vf 存在类似问题 
------------------------------------------------------------------------------------
 
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
  VALUES ('01.01.10.0023', 'tecs-vnm_test',now(),'first version');
  
-- Function: pf_net_query_sriovvfport_free(character varying)

-- DROP FUNCTION pf_net_query_sriovvfport_free(character varying);

CREATE OR REPLACE FUNCTION pf_net_query_sriovvfport_free(in_sriov_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
  ref refcursor;
  is_same integer;
  is_exist integer;
  vnaid numeric(19,0);
  phyportid numeric(19,0);
BEGIN            
      -- port 
      SELECT count(*) INTO is_exist FROM port a, physical_port b, sriov_port c WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
          AND b.port_id = c.physical_port_id AND b.vna_id = c.vna_id 
          AND a.uuid = in_sriov_uuid;
      
      SELECT a.id INTO phyportid FROM port a, physical_port b, sriov_port c WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
          AND b.port_id = c.physical_port_id AND b.vna_id = c.vna_id 
          AND a.uuid = in_sriov_uuid;
      
      IF  is_exist <= 0 THEN 
          return ref;
      END IF;
    
      -- 
      OPEN ref FOR SELECT in_sriov_uuid, a.uuid, b.pci,b.vlan_num FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
           AND b.sriov_port_id = phyportid 
		   AND a.is_consistency = 1 
           AND b.port_id not in (SELECT sriov_vf_port_id FROM vsi_sriov );
      return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;