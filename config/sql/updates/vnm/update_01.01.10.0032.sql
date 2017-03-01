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

-- 指定MAC功能 
--
-- 
------------------------------------------------------------------------------------
 
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
  VALUES ('01.01.10.0032', 'tecs-vnm_test',now(),'first version');
  
alter table vsi_profile drop CONSTRAINT idx_vsi_profile;
alter table vsi_profile alter column mac_id DROP NOT NULL;
alter table vnic add column ass_mac character varying( 64 );

drop FUNCTION pf_net_query_vsi_by_vmnic(in_vm_id numeric(19,0),in_nic_index integer);
drop FUNCTION pf_net_query_vsi(in_vsi_id_value character varying);
drop TYPE t_query_vsi;
CREATE TYPE t_query_vsi  AS(
    ret integer, -- proc ret
    vm_id numeric(19),
    nic_index integer, 
    is_sriov integer, 
    is_loop integer, 
    logic_network_uuid character varying(64),
    ass_ip character varying(64),
    ass_mask character varying(64),
    ass_gateway character varying(64),
    ass_mac character varying(64),
    port_type character varying(64),
	
    vsi_id_value character varying(64),
    vsi_id_format integer,
    vm_pg_uuid character varying(64),
    mac character varying(64),
    is_has_ip integer, 
    ip  character varying(64),
    mask character varying(64),
    is_has_vswitch integer,  
    vswitch_uuid character varying(64),
    is_has_sriovvf integer,
    sriovvfport_uuid character varying(64),
	qn_uuid character varying(64),
	qp_uuid character varying(64)
);

CREATE OR REPLACE FUNCTION pf_net_check_vsi_param(in_vm_id numeric, in_nic_index integer, in_is_sriov integer, in_is_loop integer, in_logicnetwork_uuid character varying, in_port_type character varying, in_vsi_id_value character varying, in_vsi_id_format integer, in_vm_pg_uuid character varying, in_ass_mac character varying, in_mac character varying, in_ass_ip character varying, in_is_has_ip integer, in_ip character varying, in_is_has_vswitch integer, in_vswitch_uuid character varying, in_is_has_sriovvf integer, in_sriov_vf_port_uuid character varying)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  switchportmode integer;
  pgid numeric(19,0);
  logicnetworkid numeric(19,0);  
  netplaneid numeric(19,0);
  vswitchid numeric(19,0);
  sriovportid numeric(19,0);
  phyid numeric(19,0);
  phyuuid character varying(64);
  bonduuid character varying(64);
  bondid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE integer := 13;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
  
  -- in_vsi_id_format 必须为5 
  IF in_vsi_id_format = 5 THEN 
      result := 0;
  ELSE
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  
  -- logicnetwork uuid 
  SELECT count(*) INTO is_exist FROM logic_network WHERE  uuid = in_logicnetwork_uuid;
  SELECT id INTO logicnetworkid FROM logic_network WHERE  uuid = in_logicnetwork_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 

  -- vm pg uuid  
  -- vm pg 
  SELECT count(*) INTO is_exist FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_vm_pg_uuid;
  SELECT a.id INTO pgid FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_vm_pg_uuid;
  SELECT a.switchport_mode INTO switchportmode FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_vm_pg_uuid;  
  SELECT netplane_id INTO netplaneid FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_vm_pg_uuid AND a.netplane_id is not null;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
    
  -- logicnetwork 必须 属于vm pg uuid 
  SELECT count(*) INTO is_exist FROM v_net_logicnetwork_portgroup_netplane WHERE pg_uuid = in_vm_pg_uuid AND logicnetwork_uuid = in_logicnetwork_uuid ;
  IF is_exist <= 0 THEN 
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;  
  
  -- mac 
  IF in_ass_mac = '' THEN
      SELECT count(*) INTO is_exist FROM  mac_pool WHERE mac = in_mac AND netplane_id = netplaneid;
      IF is_exist <= 0 THEN 
         result := RTN_ITEM_NO_EXIST; 
         return result;
      END IF; 
  END IF;
  
  -- ip 
  IF in_ass_ip = '' THEN
      IF in_is_has_ip > 0 THEN 
         SELECT count(*) INTO is_exist FROM logic_network_ip_pool WHERE logic_network_id = logicnetworkid AND ip = in_ip;
         IF is_exist <= 0 THEN 
             result := RTN_ITEM_NO_EXIST; 
             return result;
         END IF; 
      END IF;
  END IF;
  
  -- switch uuid 
  IF in_is_has_vswitch > 0 THEN 
      SELECT count(*) INTO is_exist FROM switch a,virtual_switch b WHERE a.id = b.switch_id AND a.uuid = in_vswitch_uuid;
      SELECT a.id INTO vswitchid FROM switch a,virtual_switch b WHERE a.id = b.switch_id AND a.uuid = in_vswitch_uuid;      
      IF is_exist <= 0 THEN 
         result := RTN_ITEM_NO_EXIST; 
         return result;
      END IF;
            
      -- sriov 时，switch 必须是edvs
      IF in_is_sriov > 0 THEN 
          SELECT count(*) INTO is_exist FROM switch WHERE id = vswitchid AND switch_type = 2;
          IF is_exist <= 0 THEN 
             result := RTN_PARAM_INVALID; 
             return result;
          END IF;     
      ELSE 
          SELECT count(*) INTO is_exist FROM switch WHERE id = vswitchid AND switch_type = 1;
          IF is_exist <= 0 THEN 
             result := RTN_PARAM_INVALID; 
             return result;
          END IF;
      END IF;
  END IF;
  
  -- sriov uuid 
  IF in_is_sriov > 0 THEN 
      IF in_is_has_sriovvf > 0 THEN 
          SELECT count(*) INTO is_exist FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.uuid = in_sriov_vf_port_uuid;
          IF is_exist <= 0 THEN 
             result := RTN_ITEM_NO_EXIST; 
             return result;
          END IF;
      
          SELECT b.port_id INTO sriovportid FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.uuid = in_sriov_vf_port_uuid;
          SELECT b.sriov_port_id INTO phyid FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.uuid = in_sriov_vf_port_uuid;      
          SELECT uuid INTO phyuuid FROM port WHERE id = phyid;
          IF is_exist <= 0 THEN 
             result := RTN_ITEM_NO_EXIST; 
             return result;
          END IF;
      END IF;      
  END IF;
  
  -- switch uuid, sriov uuid 
  IF in_is_sriov > 0 THEN 
     IF in_is_has_vswitch > 0 THEN 
        IF in_is_has_sriovvf > 0 THEN 
            -- 需要判断是否是switch的uplink口(bond成员口)    
            -- 物理端口是否在bond中 
            SELECT count(*) INTO is_exist FROM v_net_bondmap_bond_phy WHERE phy_uuid = phyuuid;
            SELECT bond_uuid INTO bonduuid FROM v_net_bondmap_bond_phy WHERE phy_uuid = phyuuid;
            IF is_exist > 0 THEN 
                -- 已经做了bond
                SELECT count(*) INTO is_exist FROM v_net_switch_port_vna WHERE switch_uuid = in_vswitch_uuid AND port_uuid = bonduuid;
                IF is_exist <= 0 THEN 
                    -- bond口不是switch uplink
                    result := RTN_PARAM_INVALID; 
                    return result;
                END IF;
            ELSE 
                -- PHY 并没有加入bond
                SELECT count(*) INTO is_exist FROM v_net_switch_port_vna WHERE switch_uuid = in_vswitch_uuid AND port_uuid = phyuuid;
                IF is_exist <= 0 THEN 
                    -- phy口不是switch uplink
                    result := RTN_PARAM_INVALID; 
                    return result;
                END IF;
            END IF;
        ELSE
            -- switch 和 sriov 成对出现
            result := RTN_PARAM_INVALID; 
            return result;
        END IF;
     END IF;
  END IF;
  
  --   
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION pf_net_check_vsi_param(numeric, integer, integer, integer, character varying, character varying, character varying, integer, character varying, character varying, integer, character varying, integer, character varying, integer, character varying) OWNER TO tecs;
  

-- check add vsi 
CREATE OR REPLACE FUNCTION pf_net_check_add_vsi(in_vm_id numeric(19,0),in_nic_index integer,
                           in_is_sriov integer, in_is_loop integer, in_logicnetwork_uuid character varying,
                           in_ass_ip character varying, in_ass_mask character varying,
                           in_ass_gateway character varying, in_ass_mac character varying,                          
                           in_port_type character varying, 
                           in_vsi_id_value character varying,  in_vsi_id_format integer,
                           in_vm_pg_uuid character varying, in_mac character varying,  in_is_has_ip integer, in_ip character varying, 
                           in_is_has_vswitch integer, in_vswitch_uuid character varying,
                           in_is_has_sriovvf integer, in_sriov_vf_port_uuid character varying,
						   in_qn_uuid  character varying,in_qp_uuid  character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  
  logicnetworkid numeric(19,0);
  netplaneid numeric(19,0);
  pgid numeric(19,0);
  macid numeric(19,0);
  ipid  numeric(19,0);
  sriovvfportid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE integer := 13;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
  
  result := pf_net_check_vsi_param(in_vm_id,in_nic_index, in_is_sriov, in_is_loop, in_logicnetwork_uuid,
                      in_port_type,
                      in_vsi_id_value,in_vsi_id_format,in_vm_pg_uuid,in_ass_mac,in_mac, in_ass_ip, in_is_has_ip,in_ip,
                      in_is_has_vswitch,in_vswitch_uuid,in_is_has_sriovvf,in_sriov_vf_port_uuid);
  IF  result > 0 THEN 
      return result;
  END IF;  
  
  IF in_qn_uuid <> '' THEN 
     SELECT count(*) INTO is_exist FROM quantum_network WHERE uuid = in_qn_uuid;
	 IF is_exist <= 0 THEN 
	     result := RTN_ITEM_NO_EXIST; 
         return result;
	 END IF;
  END IF;
  
   IF in_qp_uuid <> '' THEN 
     SELECT count(*) INTO is_exist FROM quantum_port WHERE uuid = in_qp_uuid;
	 IF is_exist <= 0 THEN 
	     result := RTN_ITEM_NO_EXIST; 
         return result;
	 END IF;
  END IF;
  
  --vm 必须存在
  SELECT count(*) INTO is_exist FROM vm WHERE  vm_id = in_vm_id;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
  
  -- logicnetwork uuid 
  SELECT count(*) INTO is_exist FROM logic_network WHERE  uuid = in_logicnetwork_uuid;
  SELECT id INTO logicnetworkid FROM logic_network WHERE  uuid = in_logicnetwork_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
  -- vm pg uuid  
  -- vm pg 
  SELECT count(*) INTO is_exist FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_vm_pg_uuid;
  SELECT a.id INTO pgid FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_vm_pg_uuid;
  SELECT netplane_id INTO netplaneid FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_vm_pg_uuid AND a.netplane_id is not null;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
  
  -- vm_id, vnic_index 检查
  SELECT count(*) INTO is_exist FROM vnic WHERE vm_id = in_vm_id AND nic_index = in_nic_index;
  IF is_exist > 0 THEN 
      result := RTN_ITEM_CONFLICT; 
      return result;
  END IF;
  
  --mac 
  IF in_ass_mac = '' THEN
      SELECT count(*) INTO is_exist FROM  mac_pool WHERE mac = in_mac AND netplane_id = netplaneid;
      SELECT id INTO  macid  FROM  mac_pool WHERE mac = in_mac AND netplane_id = netplaneid;
      IF is_exist <= 0 THEN 
         result := RTN_ITEM_NO_EXIST; 
         return result;
      END IF; 
       
      -- mac conflict check
      SELECT count(*) INTO is_exist FROM vsi_profile WHERE mac_id = macid;
      IF is_exist > 0 THEN 
         result := RTN_ITEM_CONFLICT; 
         return result;
      END IF;
  END IF;

  -- ass ip conflict check
  IF in_ass_ip <> '' THEN
     SELECT count(*) INTO is_exist FROM vnic WHERE ass_ip = in_ass_ip;
     IF is_exist > 0 THEN
       result := RTN_ITEM_CONFLICT;
       return result;
     END IF;
  END IF; 

  -- ass mac conflict check
  IF in_ass_mac <> '' THEN
     SELECT count(*) INTO is_exist FROM vnic WHERE ass_mac = in_ass_mac;
     IF is_exist > 0 THEN
       result := RTN_ITEM_CONFLICT;
       return result;
     END IF;
  END IF;    
  
  -- ip 
  IF in_is_has_ip > 0 THEN 
     SELECT count(*) INTO is_exist FROM logic_network_ip_pool WHERE logic_network_id = logicnetworkid AND ip = in_ip;
     SELECT id INTO ipid FROM logic_network_ip_pool WHERE logic_network_id = logicnetworkid AND ip = in_ip;
     IF is_exist <= 0 THEN 
         result := RTN_ITEM_NO_EXIST; 
         return result;
     END IF; 
     
     -- ip conflict check 
     SELECT count(*) INTO is_exist FROM vsi_profile WHERE logic_network_ip_id = ipid;
     IF is_exist > 0 THEN 
         result := RTN_ITEM_CONFLICT; 
         return result;
     END IF; 
  END IF;
  
  -- sriov 
  -- sriov uuid 
  IF in_is_has_sriovvf > 0 THEN 
      SELECT count(*) INTO is_exist FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.uuid = in_sriov_vf_port_uuid;
      IF is_exist <= 0 THEN 
         result := RTN_ITEM_NO_EXIST; 
         return result;
      END IF;
      
      SELECT b.port_id INTO sriovvfportid FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.uuid = in_sriov_vf_port_uuid;
      
      --sriov vf port  conflict check
      SELECT count(*) INTO is_exist FROM vsi_sriov WHERE sriov_vf_port_id = sriovvfportid;      
      IF is_exist > 0 THEN 
         result := RTN_ITEM_CONFLICT; 
         return result;
      END IF;
  END IF;
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 
                           

                           

-- vsi 添加 
-- Function: pf_net_add_vsi(character varying, character varying, character varying, integer)

-- DROP FUNCTION pf_net_add_vsi(character varying, character varying, character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_vsi(in_vm_id numeric(19,0), in_nic_index integer,
                           in_is_sriov integer, in_is_loop integer, in_logicnetwork_uuid character varying,
                           in_ass_ip character varying, in_ass_mask character varying,
                           in_ass_gateway character varying, in_ass_mac character varying,                          
                           in_port_type character varying, 
                           in_vsi_id_value character varying,  in_vsi_id_format integer,
                           in_vm_pg_uuid character varying, in_mac character varying,  in_is_has_ip integer, in_ip character varying, 
                           in_is_has_vswitch integer, in_vswitch_uuid character varying,
                           in_is_has_sriovvf integer, in_sriov_vf_port_uuid character varying,
                           in_qn_uuid  character varying,in_qp_uuid  character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  
  vnicid numeric(19,0);
  ipid numeric(19,0);
  macid numeric(19,0);
  pgid numeric(19,0);
  logicnetworkid numeric(19,0);  
  netplaneid numeric(19,0);
  vswitchid numeric(19,0);
  sriovportid numeric(19,0);
  phyid numeric(19,0);
  phyuuid character varying(64);
  bonduuid character varying(64);
  bondid numeric(19,0);
  qnid numeric(19,0);
  qpid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_add_vsi(in_vm_id,in_nic_index, in_is_sriov, in_is_loop, in_logicnetwork_uuid,
                      in_ass_ip, in_ass_mask, in_ass_gateway, in_ass_mac,
                      in_port_type,
                      in_vsi_id_value,in_vsi_id_format,in_vm_pg_uuid,in_mac,in_is_has_ip,in_ip,
                      in_is_has_vswitch,in_vswitch_uuid,in_is_has_sriovvf,in_sriov_vf_port_uuid,
					  in_qn_uuid,in_qp_uuid);
  IF result.ret > 0 THEN      
     return result;
  END IF;
  
   IF in_qn_uuid <> '' THEN 
     SELECT count(*) INTO is_exist FROM quantum_network WHERE uuid = in_qn_uuid;
	 SELECT id INTO qnid FROM quantum_network WHERE uuid = in_qn_uuid;
	 IF is_exist <= 0 THEN 
	     result := RTN_ITEM_NO_EXIST; 
         return result;
	 END IF;
  END IF;
  
   IF in_qp_uuid <> '' THEN 
     SELECT count(*) INTO is_exist FROM quantum_port WHERE uuid = in_qp_uuid;
	 SELECT id INTO qpid FROM quantum_port WHERE uuid = in_qp_uuid;
	 IF is_exist <= 0 THEN 
	     result := RTN_ITEM_NO_EXIST; 
         return result;
	 END IF;
  END IF;
  
  -- logicnetwork uuid 
  SELECT count(*) INTO is_exist FROM logic_network WHERE  uuid = in_logicnetwork_uuid;
  SELECT id INTO logicnetworkid FROM logic_network WHERE  uuid = in_logicnetwork_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 

  -- vm pg uuid   
  SELECT count(*) INTO is_exist FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_vm_pg_uuid;
  SELECT a.id INTO pgid FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_vm_pg_uuid;  
  SELECT netplane_id INTO netplaneid FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_vm_pg_uuid AND a.netplane_id is not null;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
    
  
  -- mac
  IF in_ass_mac = '' THEN  
      SELECT count(*) INTO is_exist FROM  mac_pool WHERE mac = in_mac AND netplane_id = netplaneid;
      SELECT id INTO macid FROM  mac_pool WHERE mac = in_mac AND netplane_id = netplaneid;
      IF is_exist <= 0 THEN 
         result := RTN_ITEM_NO_EXIST; 
         return result;
      END IF;
  END IF;  
  
  -- ip 
  IF in_is_has_ip > 0 THEN 
     SELECT count(*) INTO is_exist FROM logic_network_ip_pool WHERE logic_network_id = logicnetworkid AND ip = in_ip;
     SELECT id INTO ipid FROM logic_network_ip_pool WHERE logic_network_id = logicnetworkid AND ip = in_ip;
     IF is_exist <= 0 THEN 
         result := RTN_ITEM_NO_EXIST; 
         return result;
     END IF; 
  END IF;
  
  -- switch uuid 
  IF in_is_has_vswitch > 0 THEN 
      SELECT count(*) INTO is_exist FROM switch a,virtual_switch b WHERE a.id = b.switch_id AND a.uuid = in_vswitch_uuid;
      SELECT a.id INTO vswitchid FROM switch a,virtual_switch b WHERE a.id = b.switch_id AND a.uuid = in_vswitch_uuid;      
      IF is_exist <= 0 THEN 
         result := RTN_ITEM_NO_EXIST; 
         return result;
      END IF;
  END IF;
  
  -- sriov uuid 
  IF in_is_has_sriovvf > 0 THEN 
      SELECT count(*) INTO is_exist FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.uuid = in_sriov_vf_port_uuid;
      SELECT b.port_id INTO sriovportid FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.uuid = in_sriov_vf_port_uuid;
      SELECT b.sriov_port_id INTO phyid FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.uuid = in_sriov_vf_port_uuid;      
      SELECT uuid INTO phyuuid FROM port WHERE id = phyid;
      IF is_exist <= 0 THEN 
         result := RTN_ITEM_NO_EXIST; 
         return result;
      END IF;
  END IF;
  
  
  -- insert item 
  result.id := nextval('serial_vsi_profile');
  vnicid := nextval('serial_vnic');
  result.uuid := in_vsi_id_value;
  BEGIN 
      -- INSERT vsi profile 
      IF in_is_has_ip > 0 THEN           
          IF in_is_has_vswitch > 0 THEN 
              INSERT INTO vsi_profile(id,vsi_id_value, vsi_id_format, vm_pg_id, mac_id, logic_network_ip_id,virtual_switch_id) 
                values(result.id,in_vsi_id_value,in_vsi_id_format, pgid,macid, ipid, vswitchid);
          ELSE 
              INSERT INTO vsi_profile(id,vsi_id_value, vsi_id_format, vm_pg_id, mac_id, logic_network_ip_id) 
                values(result.id,in_vsi_id_value,in_vsi_id_format, pgid,macid, ipid);
          END IF;
      ELSE 
          IF in_is_has_vswitch > 0 THEN 
              INSERT INTO vsi_profile(id,vsi_id_value, vsi_id_format, vm_pg_id, mac_id, virtual_switch_id) 
                values(result.id,in_vsi_id_value,in_vsi_id_format, pgid,macid,  vswitchid);
                
             -- trigger
            result.ret :=  pf_net_tr_vsi_attach_vswitch(result.id, vswitchid);
             
          ELSE 
              INSERT INTO vsi_profile(id,vsi_id_value, vsi_id_format, vm_pg_id, mac_id) 
                values(result.id,in_vsi_id_value,in_vsi_id_format, pgid,macid);                
          END IF;                   
      END IF;
	  
	  IF in_qn_uuid <> '' THEN 
	      UPDATE vsi_profile SET quantum_network_id = qnid WHERE id = result.id;
	  END IF;
	  
	  IF in_qp_uuid <> '' THEN 
	      UPDATE vsi_profile SET quantum_port_id = qpid WHERE id = result.id;
	  END IF;
      
      -- INSERT VNIC 
     INSERT INTO vnic(id, vm_id, nic_index, is_sriov ,is_loop, logic_network_uuid, ass_ip, ass_mask, ass_gateway, ass_mac, port_type, 
             vsi_profile_id )
      VALUES(vnicid,in_vm_id,in_nic_index,in_is_sriov,in_is_loop,in_logicnetwork_uuid, in_ass_ip, in_ass_mask, in_ass_gateway, in_ass_mac, in_port_type, 
             result.id);
                 
      -- INSERT VSI SRIOV 
      IF in_is_has_sriovvf > 0 THEN 
         INSERT INTO vsi_sriov(vsi_profile_id,sriov_vf_port_id) values(result.id,sriovportid);
         
         -- trigger
         result.ret :=  pf_net_tr_add_vsisriov(result.id,sriovportid);
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
 

-- 判断是否可以进行修改                  

CREATE OR REPLACE FUNCTION pf_net_check_modify_vsi(in_vm_id numeric(19,0),in_nic_index integer, 
                           in_is_sriov integer, in_is_loop integer, in_logicnetwork_uuid character varying,
                           in_ass_ip character varying, in_ass_mask character varying,
                           in_ass_gateway character varying, in_ass_mac character varying,                          
                           in_port_type character varying, 
                           in_vsi_id_value character varying,  in_vsi_id_format integer,
                           in_vm_pg_uuid character varying, in_mac character varying,  in_is_has_ip integer, in_ip character varying, 
                           in_is_has_vswitch integer, in_vswitch_uuid character varying,
                           in_is_has_sriovvf integer, in_sriov_vf_port_uuid character varying,
						   in_qn_uuid  character varying,in_qp_uuid  character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  vsiid numeric(19,0);
  sriovvfportid numeric(19,0);
  vswitchid numeric(19,0);
  
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE integer := 13;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
  
  result := pf_net_check_vsi_param(in_vm_id,in_nic_index, in_is_sriov, in_is_loop, in_logicnetwork_uuid,
                      in_port_type,
                      in_vsi_id_value,in_vsi_id_format,in_vm_pg_uuid,in_ass_mac,in_mac,in_ass_ip,in_is_has_ip,in_ip,
                      in_is_has_vswitch,in_vswitch_uuid,in_is_has_sriovvf,in_sriov_vf_port_uuid);                      
  IF  result > 0 THEN 
      return result;
  END IF;  
  
  IF in_qn_uuid <> '' THEN 
     SELECT count(*) INTO is_exist FROM quantum_network WHERE uuid = in_qn_uuid;
	 IF is_exist <= 0 THEN 
	     result := RTN_ITEM_NO_EXIST; 
         return result;
	 END IF;
  END IF;
  
   IF in_qp_uuid <> '' THEN 
     SELECT count(*) INTO is_exist FROM quantum_port WHERE uuid = in_qp_uuid;
	 IF is_exist <= 0 THEN 
	     result := RTN_ITEM_NO_EXIST; 
         return result;
	 END IF;
  END IF;
  
  -- check vsi_id 是否存在
  SELECT count(*) INTO is_exist FROM vsi_profile WHERE vsi_id_value = in_vsi_id_value;
  SELECT id INTO vsiid FROM vsi_profile WHERE vsi_id_value = in_vsi_id_value;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST; 
      return result;
  END IF; 
  
  -- vm_id, nic_index, is_sriov, is_loop, logicnetwork_uuid, port_type 不能修改
  SELECT count(*) INTO is_exist FROM vnic WHERE vm_id = in_vm_id AND nic_index = in_nic_index 
          AND is_sriov = in_is_sriov AND is_loop = in_is_loop AND logic_network_uuid = in_logicnetwork_uuid AND port_type = in_port_type 
          AND vsi_profile_id = vsiid;
  IF is_exist <= 0 THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF; 
  
  -- mac 不能改变
  IF in_ass_mac = '' THEN
      SELECT count(*) INTO is_exist FROM vsi_profile a, mac_pool b WHERE a.mac_id = b.id AND b.mac = in_mac AND a.id = vsiid;
      IF is_exist <= 0 THEN 
          result := RTN_PARAM_INVALID; 
          return result;
      END IF;
  END IF;
  
  -- ip 不能改变
  IF in_ass_ip = '' THEN
      IF in_is_has_ip > 0 THEN 
         SELECT count(*) INTO is_exist FROM vsi_profile a, logic_network_ip_pool b WHERE a.logic_network_ip_id = b.id 
                 AND b.ip = in_ip AND a.id = vsiid;
         IF is_exist <= 0 THEN 
            result := RTN_PARAM_INVALID; 
            return result;
         END IF;
      ELSE
         SELECT count(*) INTO is_exist FROM vsi_profile WHERE id = vsiid AND logic_network_ip_id is NULL;
         IF is_exist <= 0 THEN 
            result := RTN_PARAM_INVALID; 
            return result;
         END IF;
      END IF;
  END IF;
  
  -- ass ip check
  IF in_ass_ip <> '' THEN
     SELECT count(*) INTO is_exist FROM vnic WHERE vm_id = in_vm_id AND ass_ip = in_ass_ip;
     IF is_exist <= 0 THEN
       result := RTN_PARAM_INVALID;
       return result;
     END IF;
  END IF;
  
  -- ass mac check
  IF in_ass_mac <> '' THEN
     SELECT count(*) INTO is_exist FROM vnic WHERE vm_id = in_vm_id AND ass_mac = in_ass_mac;
     IF is_exist <= 0 THEN
       result := RTN_PARAM_INVALID;
       return result;
     END IF;
  END IF;
  
  -- sriov conflict check 
  -- sriov uuid 
  IF in_is_has_sriovvf > 0 THEN 
      SELECT count(*) INTO is_exist FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.uuid = in_sriov_vf_port_uuid;
      IF is_exist <= 0 THEN 
         result := RTN_ITEM_NO_EXIST; 
         return result;
      END IF;
      
      SELECT b.port_id INTO sriovvfportid FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.uuid = in_sriov_vf_port_uuid;
      
      --sriov vf port  conflict check
      SELECT count(*) INTO is_exist FROM vsi_sriov WHERE sriov_vf_port_id = sriovvfportid AND vsi_profile_id = vsiid ;      
      IF is_exist <= 0 THEN 
          SELECT count(*) INTO is_exist FROM vsi_sriov WHERE sriov_vf_port_id = sriovvfportid;      
          IF is_exist > 0 THEN 
             result := RTN_ITEM_CONFLICT; 
             return result;
          END IF;
          
          -- 已分配sriov，不能再修改
          SELECT count(*) INTO is_exist FROM vsi_sriov WHERE vsi_profile_id = vsiid ;      
          IF is_exist > 0 THEN 
             result := RTN_ITEM_CONFLICT; 
             return result;
          END IF;
      END IF;
  END IF;
  
  -- 考虑实际情况，部署vnic, Switch，sriov确定后，将不能再进行修改
  -- switch uuid 
  IF in_is_has_vswitch > 0 THEN 
      SELECT count(*) INTO is_exist FROM switch a,virtual_switch b WHERE a.id = b.switch_id AND a.uuid = in_vswitch_uuid;
      SELECT a.id INTO vswitchid FROM switch a,virtual_switch b WHERE a.id = b.switch_id AND a.uuid = in_vswitch_uuid;      
      IF is_exist <= 0 THEN 
         result := RTN_ITEM_NO_EXIST; 
         return result;
      END IF;
      
      -- switch can not change
      SELECT count(*) INTO is_exist FROM vsi_profile WHERE id = vsiid AND virtual_switch_id is not null;
      IF is_exist > 0 THEN 
          SELECT count(*) INTO is_exist FROM vsi_profile WHERE id = vsiid AND virtual_switch_id = vswitchid;
          IF is_exist <= 0 THEN 
              result := RTN_ITEM_CONFLICT; 
              return result;
          END IF;
      END IF;      
  END IF;
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

                           
-- vsi 修改 
CREATE OR REPLACE FUNCTION pf_net_modify_vsi(in_vm_id numeric(19,0),in_nic_index integer,
                           in_is_sriov integer, in_is_loop integer, in_logicnetwork_uuid character varying, 
                           in_ass_ip character varying, in_ass_mask character varying,
                           in_ass_gateway character varying, in_ass_mac character varying,
                           in_port_type character varying, 
                           in_vsi_id_value character varying,  in_vsi_id_format integer,
                           in_vm_pg_uuid character varying, in_mac character varying,  in_is_has_ip integer, in_ip character varying, 
                           in_is_has_vswitch integer, in_vswitch_uuid character varying,
                           in_is_has_sriovvf integer, in_sriov_vf_port_uuid character varying,
						   in_qn_uuid  character varying,in_qp_uuid  character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  
  vsiid numeric(19,0);
  vnicid numeric(19,0);
  ipid numeric(19,0);
  macid numeric(19,0);
  pgid numeric(19,0);
  logicnetworkid numeric(19,0);  
  netplaneid numeric(19,0);
  vswitchid numeric(19,0);
  oldvswitchid numeric(19,0);
  sriovportid numeric(19,0);
  phyid numeric(19,0);
  phyuuid character varying(64);
  bonduuid character varying(64);
  bondid numeric(19,0);
  qnid numeric(19,0);
  qpid numeric(19,0);
  oldsriovvfportid numeric(19,0);
  oldexistswitch integer := 0;
  oldexistsriovvfport integer := 0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_modify_vsi(in_vm_id,in_nic_index, in_is_sriov, in_is_loop, in_logicnetwork_uuid,
                      in_ass_ip, in_ass_mask, in_ass_gateway, in_ass_mac,
                      in_port_type,
                      in_vsi_id_value,in_vsi_id_format,in_vm_pg_uuid,in_mac,in_is_has_ip,in_ip,
                      in_is_has_vswitch,in_vswitch_uuid,in_is_has_sriovvf,in_sriov_vf_port_uuid,
					  in_qn_uuid,in_qp_uuid);
  IF result.ret > 0 THEN      
     return result;
  END IF;
    
  IF in_qn_uuid <> '' THEN 
     SELECT count(*) INTO is_exist FROM quantum_network WHERE uuid = in_qn_uuid;
	 SELECT id INTO qnid FROM quantum_network WHERE uuid = in_qn_uuid;
	 IF is_exist <= 0 THEN 
	     result := RTN_ITEM_NO_EXIST; 
         return result;
	 END IF;
  END IF;
  
   IF in_qp_uuid <> '' THEN 
     SELECT count(*) INTO is_exist FROM quantum_port WHERE uuid = in_qp_uuid;
	 SELECT id INTO qpid FROM quantum_port WHERE uuid = in_qp_uuid;
	 IF is_exist <= 0 THEN 
	     result := RTN_ITEM_NO_EXIST; 
         return result;
	 END IF;
  END IF;
  
  -- check vsi_id 是否存在
  SELECT count(*) INTO is_exist FROM vsi_profile WHERE vsi_id_value = in_vsi_id_value;
  SELECT id INTO vsiid FROM vsi_profile WHERE vsi_id_value = in_vsi_id_value;
  SELECT id INTO result.id FROM vsi_profile WHERE vsi_id_value = in_vsi_id_value;  
  
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST; 
      return result;
  END IF; 
  
  -- switch uuid 
  IF in_is_has_vswitch > 0 THEN 
      SELECT count(*) INTO is_exist FROM switch a,virtual_switch b WHERE a.id = b.switch_id AND a.uuid = in_vswitch_uuid;
      SELECT a.id INTO vswitchid FROM switch a,virtual_switch b WHERE a.id = b.switch_id AND a.uuid = in_vswitch_uuid;      
      IF is_exist <= 0 THEN 
         result := RTN_ITEM_NO_EXIST; 
         return result;
      END IF;
  END IF;
  
  -- sriov uuid 
  IF in_is_has_sriovvf > 0 THEN 
      SELECT count(*) INTO is_exist FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.uuid = in_sriov_vf_port_uuid;
      SELECT b.port_id INTO sriovportid FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.uuid = in_sriov_vf_port_uuid;
      IF is_exist <= 0 THEN 
         result := RTN_ITEM_NO_EXIST; 
         return result;
      END IF;
  END IF;
  
  
  SELECT count(*) INTO oldexistswitch FROM vsi_profile WHERE virtual_switch_id is not null  AND vsi_id_value = in_vsi_id_value;
  SELECT virtual_switch_id INTO oldvswitchid FROM vsi_profile WHERE virtual_switch_id is not null  AND vsi_id_value = in_vsi_id_value;
  SELECT sriov_vf_port_id INTO oldexistsriovvfport FROM vsi_sriov WHERE sriov_vf_port_id is not null  AND vsi_profile_id = vsiid;
  
      
  -- update item 
  --result.id := nextval('serial_vsi_profile');
  --vnicid := nextval('serial_vnic');
  --result.uuid := in_uuid;
  result.uuid := in_vsi_id_value;
  BEGIN 
      -- UPDATE vsi profile      
      IF in_is_has_vswitch > 0 THEN 
          UPDATE vsi_profile SET virtual_switch_id = vswitchid WHERE id = vsiid;
          
          -- trigger
          IF oldexistswitch <= 0 THEN
              result.ret :=  pf_net_tr_vsi_attach_vswitch(vsiid, vswitchid);
          END IF;
          
      ELSE           
          -- trigger
          IF oldexistswitch > 0 THEN
              result.ret :=  pf_net_tr_vsi_unattach_vswitch(vsiid,oldvswitchid);
          END IF;
          
          UPDATE vsi_profile SET virtual_switch_id = NULL WHERE id = vsiid;
          
      END IF;
	  
	  IF in_qn_uuid <> '' THEN 
	      UPDATE  vsi_profile SET quantum_network_id = qnid WHERE id = vsiid;
	  ELSE
	      UPDATE  vsi_profile SET quantum_network_id = null WHERE id = vsiid;
	  END IF;
	  
	  IF in_qp_uuid <> '' THEN 
	      UPDATE  vsi_profile SET quantum_port_id = qpid WHERE id = vsiid;
	  ELSE
	      UPDATE  vsi_profile SET quantum_port_id = null WHERE id = vsiid;
	  END IF;
            
      -- UPDATE VNIC 
     
      -- UPDATE vsi_sriov       
      -- 考虑sriov没有修改， 以及sriov vf port 修改~~ 
      -- sriov vf 本身不能修改，只能uuid->null, null->uuid
      IF in_is_has_sriovvf > 0 THEN 
          SELECT count(*) INTO is_exist FROM vsi_sriov WHERE vsi_profile_id = vsiid;
          IF is_exist <= 0 THEN 
              INSERT INTO vsi_sriov(vsi_profile_id,sriov_vf_port_id) values(vsiid,sriovportid);
			  
              -- trigger
              result.ret :=  pf_net_tr_add_vsisriov(vsiid,sriovportid);
          ELSE           
              SELECT count(*) INTO is_exist FROM vsi_sriov WHERE vsi_profile_id = vsiid AND sriov_vf_port_id = sriovportid;
              IF  is_exist <= 0 THEN 
                  DELETE FROM vsi_sriov WHERE vsi_profile_id = vsiid;
                  INSERT INTO vsi_sriov(vsi_profile_id,sriov_vf_port_id) values(vsiid,sriovportid);
				  
                  -- trigger
                  result.ret :=  pf_net_tr_add_vsisriov(vsiid,sriovportid);
              END IF;          
          END IF;
      ELSE      
          SELECT count(*) INTO is_exist FROM vsi_sriov WHERE vsi_profile_id = vsiid;
          SELECT sriov_vf_port_id INTO oldsriovvfportid FROM vsi_sriov WHERE vsi_profile_id = vsiid;

          -- 之前配置了sriov vf 现在freeOnHc后将此sriov vf 释放了，需更新tr
          IF is_exist > 0 THEN               
              -- trigger
              result.ret :=  pf_net_tr_del_vsisriov(vsiid,oldsriovvfportid);
          END IF; 
          
          DELETE FROM vsi_sriov WHERE vsi_profile_id = vsiid;
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
 
 -- 查询 (通过vsi_id_value)
CREATE OR REPLACE FUNCTION pf_net_query_vsi(in_vsi_id_value character varying)
  RETURNS t_query_vsi AS
$BODY$
DECLARE
    result t_query_vsi;
    is_exist integer; 
    vsiid numeric(19,0);
    logicnetworkid numeric(19,0);
    vswitchid numeric(19,0);
    sriovvfportid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN  
    result.ret := 0;
    SELECT count(*) INTO is_exist FROM vsi_profile WHERE vsi_id_value = in_vsi_id_value;
    SELECT id INTO vsiid FROM vsi_profile WHERE vsi_id_value = in_vsi_id_value;
    IF is_exist <= 0 THEN 
        result.ret := RTN_ITEM_NO_EXIST; 
        return result;
    END IF; 

    -- vnic 
    SELECT vm_id INTO result.vm_id FROM vnic WHERE vsi_profile_id = vsiid;
        
    SELECT nic_index INTO result.nic_index FROM vnic WHERE vsi_profile_id = vsiid;
    SELECT is_sriov INTO result.is_sriov FROM vnic WHERE vsi_profile_id = vsiid;
    SELECT is_loop INTO result.is_loop FROM vnic WHERE vsi_profile_id = vsiid;
    SELECT logic_network_uuid INTO result.logic_network_uuid FROM vnic WHERE vsi_profile_id = vsiid;
    SELECT ass_ip INTO result.ass_ip FROM vnic WHERE vsi_profile_id = vsiid;
    SELECT ass_mask INTO result.ass_mask FROM vnic WHERE vsi_profile_id = vsiid;
    SELECT ass_gateway INTO result.ass_gateway FROM vnic WHERE vsi_profile_id = vsiid;
    SELECT ass_mac INTO result.ass_mac FROM vnic WHERE vsi_profile_id = vsiid;
    SELECT port_type INTO result.port_type FROM vnic WHERE vsi_profile_id = vsiid;
        
    -- vsi profile 
    result.vsi_id_value := in_vsi_id_value;
    SELECT vsi_id_format INTO result.vsi_id_format FROM vsi_profile WHERE vsi_id_value = in_vsi_id_value;
    
    -- vm pg uuid 
    SELECT b.uuid INTO result.vm_pg_uuid FROM vsi_profile a, port_group b WHERE a.vm_pg_id = b.id AND a.id = vsiid;
    
    -- mac 
    SELECT b.mac INTO result.mac FROM vsi_profile a, mac_pool b WHERE a.mac_id = b.id AND a.id = vsiid;
    
    -- ip 
    SELECT count(*) INTO is_exist FROM vsi_profile  WHERE id = vsiid AND logic_network_ip_id is not null;
    SELECT logic_network_ip_id INTO logicnetworkid FROM vsi_profile  WHERE id = vsiid AND logic_network_ip_id is not null;    
    IF is_exist <= 0 THEN 
        result.is_has_ip := 0;
    ELSE 
        result.is_has_ip := 1;
        SELECT ip INTO result.ip FROM logic_network_ip_pool WHERE id = logicnetworkid;
        SELECT mask INTO result.mask FROM logic_network_ip_pool WHERE id = logicnetworkid;
    END IF;
    
    -- vswitch 
    SELECT count(*) INTO is_exist FROM vsi_profile  WHERE id = vsiid AND virtual_switch_id is not null;
    SELECT virtual_switch_id INTO vswitchid FROM vsi_profile  WHERE id = vsiid AND virtual_switch_id is not null;    
    IF is_exist <= 0 THEN 
        result.is_has_vswitch := 0;
    ELSE 
        result.is_has_vswitch := 1;
        SELECT uuid INTO result.vswitch_uuid FROM switch WHERE id = vswitchid;
    END IF;
    
    -- sriov 
    SELECT count(*) INTO is_exist FROM vsi_sriov  WHERE vsi_profile_id = vsiid;
    SELECT sriov_vf_port_id INTO sriovvfportid FROM vsi_sriov  WHERE vsi_profile_id = vsiid;    
    IF is_exist <= 0 THEN 
        result.is_has_sriovvf := 0;
    ELSE 
        result.is_has_sriovvf := 1;
        SELECT uuid INTO result.sriovvfport_uuid FROM port WHERE id = sriovvfportid;
    END IF;
    
	/* qn qp uuid */
	SELECT count(*) INTO is_exist FROM vsi_profile a, quantum_network b WHERE a.quantum_network_id = b.id; 
	IF is_exist > 0 THEN 
	    SELECT b.uuid INTO result.qn_uuid FROM vsi_profile a, quantum_network b WHERE a.quantum_network_id = b.id; 
	ELSE
	    result.qn_uuid := '';
	END IF;
	
	SELECT count(*) INTO is_exist FROM vsi_profile a, quantum_port b WHERE a.quantum_port_id = b.id; 
	IF is_exist > 0 THEN 
	    SELECT b.uuid INTO result.qp_uuid FROM vsi_profile a, quantum_port b WHERE a.quantum_port_id = b.id; 
	ELSE
	    result.qp_uuid := '';
	END IF;
	
    return result;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 查询 (通过vm_id nic_indxe)

CREATE OR REPLACE FUNCTION pf_net_query_vsi_by_vmnic(in_vm_id numeric(19,0),in_nic_index integer)
  RETURNS t_query_vsi AS
$BODY$
DECLARE
    result t_query_vsi;
    is_exist integer; 
    vsiid numeric(19,0);
    logicnetworkid numeric(19,0);
    vswitchid numeric(19,0);
    sriovvfportid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN  
    result.ret := 0;
    SELECT count(*) INTO is_exist FROM vnic WHERE vm_id = in_vm_id AND nic_index = in_nic_index;
    SELECT vsi_profile_id INTO vsiid FROM vnic WHERE vm_id = in_vm_id AND nic_index = in_nic_index;
    IF is_exist <= 0 THEN 
        result.ret := RTN_ITEM_NO_EXIST; 
        return result;
    END IF; 

    -- vnic 
    SELECT vm_id INTO result.vm_id FROM vnic WHERE vsi_profile_id = vsiid;
    
    SELECT nic_index INTO result.nic_index FROM vnic WHERE vsi_profile_id = vsiid;
    SELECT is_sriov INTO result.is_sriov FROM vnic WHERE vsi_profile_id = vsiid;
    SELECT is_loop INTO result.is_loop FROM vnic WHERE vsi_profile_id = vsiid;
    SELECT logic_network_uuid INTO result.logic_network_uuid FROM vnic WHERE vsi_profile_id = vsiid;
    SELECT ass_ip INTO result.ass_ip FROM vnic WHERE vsi_profile_id = vsiid;
    SELECT ass_mask INTO result.ass_mask FROM vnic WHERE vsi_profile_id = vsiid;
    SELECT ass_gateway INTO result.ass_gateway FROM vnic WHERE vsi_profile_id = vsiid;
    SELECT ass_mac INTO result.ass_mac FROM vnic WHERE vsi_profile_id = vsiid;
    SELECT port_type INTO result.port_type FROM vnic WHERE vsi_profile_id = vsiid;
            
    -- vsi profile 
    SELECT vsi_id_value INTO result.vsi_id_value FROM vsi_profile WHERE id = vsiid;
    SELECT vsi_id_format INTO result.vsi_id_format FROM vsi_profile WHERE id = vsiid;
    
    -- vm pg uuid 
    SELECT b.uuid INTO result.vm_pg_uuid FROM vsi_profile a, port_group b WHERE a.vm_pg_id = b.id AND a.id = vsiid;
    
    -- mac 
    SELECT b.mac INTO result.mac FROM vsi_profile a, mac_pool b WHERE a.mac_id = b.id AND a.id = vsiid;
    
    -- ip 
    SELECT count(*) INTO is_exist FROM vsi_profile  WHERE id = vsiid AND logic_network_ip_id is not null;
    SELECT logic_network_ip_id INTO logicnetworkid FROM vsi_profile  WHERE id = vsiid AND logic_network_ip_id is not null;    
    IF is_exist <= 0 THEN 
        result.is_has_ip := 0;
    ELSE 
        result.is_has_ip := 1;
        SELECT ip INTO result.ip FROM logic_network_ip_pool WHERE id = logicnetworkid;
        SELECT mask INTO result.mask FROM logic_network_ip_pool WHERE id = logicnetworkid;
    END IF;
    
    -- vswitch 
    SELECT count(*) INTO is_exist FROM vsi_profile  WHERE id = vsiid AND virtual_switch_id is not null;
    SELECT virtual_switch_id INTO vswitchid FROM vsi_profile  WHERE id = vsiid AND virtual_switch_id is not null;    
    IF is_exist <= 0 THEN 
        result.is_has_vswitch := 0;
    ELSE 
        result.is_has_vswitch := 1;
        SELECT uuid INTO result.vswitch_uuid FROM switch WHERE id = vswitchid;
    END IF;
    
    -- sriov 
    SELECT count(*) INTO is_exist FROM vsi_sriov  WHERE vsi_profile_id = vsiid;
    SELECT sriov_vf_port_id INTO sriovvfportid FROM vsi_sriov  WHERE vsi_profile_id = vsiid;    
    IF is_exist <= 0 THEN 
        result.is_has_sriovvf := 0;
    ELSE 
        result.is_has_sriovvf := 1;
        SELECT uuid INTO result.sriovvfport_uuid FROM port WHERE id = sriovvfportid;
    END IF;
	
	/* qn qp uuid */
	SELECT count(*) INTO is_exist FROM vsi_profile a, quantum_network b WHERE a.quantum_network_id = b.id; 
	IF is_exist > 0 THEN 
	    SELECT b.uuid INTO result.qn_uuid FROM vsi_profile a, quantum_network b WHERE a.quantum_network_id = b.id; 
	ELSE
	    result.qn_uuid := '';
	END IF;
	
	SELECT count(*) INTO is_exist FROM vsi_profile a, quantum_port b WHERE a.quantum_port_id = b.id; 
	IF is_exist > 0 THEN 
	    SELECT b.uuid INTO result.qp_uuid FROM vsi_profile a, quantum_port b WHERE a.quantum_port_id = b.id; 
	ELSE
	    result.qp_uuid := '';
	END IF;
            
    return result;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;