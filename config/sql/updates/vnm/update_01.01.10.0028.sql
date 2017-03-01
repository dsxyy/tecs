﻿------------------------------------------------------------------------------------
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

--对sriov 计算重新梳理下, port状态变化时，total_sriov_num不改变，free_sriov_num随之变化
--涉及sriov vf modify , add;  sriov modify add, bond map add modify, edvs map add modify 
-- 存在switch多计数问题，需要加于解决
-- port 中记录的free必须是有效的计数， total 可以是全部的 
-- bond 中记录的free 必须是有效的计数
-- edvs 中记录的数目 必须也是有效的计数 
-- 
------------------------------------------------------------------------------------
 
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
  VALUES ('01.01.10.0028', 'tecs-vnm_test',now(),'first version');

  

--添加SRIOV VF 时触发 流程 
-- sriovvf is_consistency != 1 时，仅对total_num 进行计数，不对free_num计数； bond,switch相同逻辑
--
--
--
CREATE OR REPLACE FUNCTION pf_net_tr_add_sriovvfport(in_sriovvfport_id numeric(19,0), in_port_id numeric(19,0))
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  oldtotalnum integer := 0;
  oldfreenum integer := 0;
  bondid numeric(19,0);
  edvsid numeric(19,0);
  oldedvsnicmaxnum integer :=0;
  sriovisok integer := 0;
  sriovvfisok integer := 0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0; 
  
  SELECT count(*) INTO is_exist FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
  SELECT a.total_sriovvf_num INTO oldtotalnum FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
  SELECT a.free_sriovvf_num INTO oldfreenum FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
  -- update port total_num / free_num 
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
  END IF;

  SELECT count(*) INTO sriovisok FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id AND a.is_consistency = 1;
  /* sriov vf add，一定是isok的 */
  --SELECT count(*) INTO sriovvfisok FROM port a, sriov_vf_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id AND a.is_consistency = 1;
  sriovvfisok = 1;
  BEGIN 
        
        UPDATE port SET total_sriovvf_num = (oldtotalnum +1)  WHERE id = in_port_id;

        IF (sriovisok > 0) AND (sriovvfisok > 0) THEN 
            UPDATE port SET free_sriovvf_num = (oldfreenum + 1) WHERE id = in_port_id;
        END IF;
                
        -- 判断该SRIOV 网口是否加入了EDVS， 如时，需更新EDVS nic_max_num_cfg
        SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id AND a.is_consistency = 1;
        SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id;
        SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id;
        IF (is_exist > 0) AND (sriovisok > 0) AND (sriovvfisok > 0) THEN 		    
            UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum + 1) WHERE switch_id = edvsid;
        END IF;

        -- 判断该SRIOV 网口是否加入了BOND        
		SELECT count(*) INTO is_exist FROM bond_map a, port b WHERE a.bond_id = b.id AND a.physical_port_id = sriovportid AND b.is_consistency = 1;
        SELECT bond_id INTO bondid FROM bond_map WHERE physical_port_id = in_port_id;
        IF is_exist > 0 THEN 
            /* TOTAL NUM */
            SELECT total_sriovvf_num INTO oldtotalnum FROM port WHERE id = bondid ;
            UPDATE port SET total_sriovvf_num = (oldtotalnum +1) WHERE id = bondid;

            /* FREE NUM */
            SELECT count(*) INTO is_exist FROM bond_map a, port b WHERE a.physical_port_id = in_port_id AND a.physical_port_id = b.id AND b.is_consistency = 1 ;
            SELECT free_sriovvf_num INTO oldfreenum FROM port WHERE id = bondid ;
            IF (is_exist > 0) AND (sriovisok > 0) AND (sriovvfisok > 0) THEN 
            UPDATE port SET free_sriovvf_num = (oldfreenum + 1) WHERE id = bondid;
            END IF;
            
            -- bond 口的total_sriov_num, valid_sriov_num 
            result := pf_net_tr_calc_bond_sriovport_num(bondid);
            IF result > 0 THEN 
            
            END IF;
            
            -- 判断该BOND口是否加入了EDVS ， 如时，需更新EDVS nic_max_num_cfg
            SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid AND a.is_consistency = 1;
            SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid;
            SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid;
            IF (is_exist > 0) AND (sriovisok > 0) AND (sriovvfisok > 0) THEN 
                UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum + 1) WHERE switch_id = edvsid;
            END IF;            
            
        END IF;
                
  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- SRIOV VF 修改时触发 流程 (online -> offline or offline -> online ) 主要考虑换后插卡 
-- online -> offline    如果sriov port 已经处于offline ，则不需要进行其他更新
-- offline -> online    如果sriov port 还是处于offline，则不需要进行其他更新 
-- in_status =1 offline->online; =0 online -> offline
CREATE OR REPLACE FUNCTION pf_net_tr_modify_sriovvfport(in_sriovvfport_id numeric(19,0), in_port_id numeric(19,0), in_status integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  oldtotalnum integer := 0;
  oldfreenum integer := 0;
  bondid numeric(19,0);
  edvsid numeric(19,0);
  oldedvsnicmaxnum integer :=0;
  sriovisok integer :=0;
  edvsmapisok integer := 0;
  oldsriovvfisconsistency integer :=0;
  ADJUSTVALUE integer :=0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0;
  
  SELECT count(*) INTO is_exist FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
  SELECT a.total_sriovvf_num INTO oldtotalnum FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
  SELECT a.free_sriovvf_num INTO oldfreenum FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
  SELECT a.is_consistency INTO sriovisok FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
  -- update port total_num / free_num 
  IF is_exist <= 0 THEN 
    result := RTN_ITEM_NO_EXIST;
    return result;
  END IF; 
  
  SELECT count(*)INTO is_exist FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.id = in_sriovvfport_id;
  SELECT a.is_consistency INTO oldsriovvfisconsistency FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.id = in_sriovvfport_id;
  IF is_exist <= 0 THEN 
    result := RTN_ITEM_NO_EXIST;
    return result;
  END IF;
 
  IF in_status <= 0 THEN 
      ADJUSTVALUE = -1;
  ELSE
      ADJUSTVALUE = 1;
  END IF;
  
  BEGIN     
        /* ȷ��sriov port ���� */
        IF sriovisok > 0 THEN
           UPDATE port SET free_sriovvf_num = (oldfreenum + ADJUSTVALUE) WHERE id = in_port_id;
        ELSE 
            result := 0;
            return result;
        END IF;
        
        -- 判断该SRIOV 网口是否加入了EDVS， 如时，需更新EDVS nic_max_num_cfg
        SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id AND a.is_consistency = 1;
        SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id;
        SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id;
        IF is_exist > 0 THEN                         
            UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum + ADJUSTVALUE) WHERE switch_id = edvsid;
        END IF;
        
        -- 判断该SRIOV 网口是否加入了BOND
        SELECT count(*) INTO is_exist FROM bond_map a, port b WHERE a.physical_port_id = in_port_id AND a.bond_id = b.id AND b.is_consistency = 1;
        SELECT bond_id INTO bondid FROM bond_map WHERE physical_port_id = in_port_id;
        IF is_exist > 0 THEN 
            SELECT total_sriovvf_num INTO oldtotalnum FROM port WHERE id = bondid ;
            SELECT free_sriovvf_num INTO oldfreenum FROM port WHERE id = bondid ;
            UPDATE port SET free_sriovvf_num = (oldfreenum + ADJUSTVALUE) WHERE id = bondid;
            
            -- bond 口的total_sriov_num, valid_sriov_num 
            result := pf_net_tr_calc_bond_sriovport_num(bondid);
            IF result > 0 THEN 
            
            END IF;
            
            -- 判断该BOND口是否加入了EDVS ， 如时，需更新EDVS nic_max_num_cfg
            SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid AND a.is_consistency = 1;
            SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid;
            SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid;
            IF is_exist > 0 THEN 
                UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum + ADJUSTVALUE) WHERE switch_id = edvsid;
            END IF;    

        END IF;
  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 



--修改SRIOV网口时 时触发 流程 
-- 流程 (online -> offline or offline -> online ) 
CREATE OR REPLACE FUNCTION pf_net_tr_modify_sriovport( in_port_id numeric(19,0), in_status integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  oldPortTotalnum integer := 0;
  oldPortFreenum integer := 0;
  oldtotalnum integer := 0;
  oldfreenum integer := 0;
  bondid numeric(19,0);
  edvsid numeric(19,0);
  oldedvsnicmaxnum integer :=0;
  oldsriovisok integer :=0;
  ADJUSTVALUE integer :=0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0;
  
  SELECT count(*) INTO is_exist FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
  SELECT a.total_sriovvf_num INTO oldPortTotalnum FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
  SELECT a.free_sriovvf_num INTO oldPortFreenum FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
  
  IF is_exist <= 0 THEN 
    result := RTN_ITEM_NO_EXIST;
    return result;
  END IF; 
  
  /* ����Ǵ�offline-->online ��Ҫ���¼���sriov vf ��Ŀ */
  IF in_status <= 0 THEN 
      ADJUSTVALUE = (0-oldPortFreenum);
  ELSE
      SELECT count(*) INTO oldPortFreenum FROM port a, sriov_port b, sriov_vf_port c ,vsi_sriov d 
       WHERE c.port_id = a.id AND c.sriov_port_id = b.physical_port_id AND b.physical_port_id = in_port_id AND a.is_consistency = 1 
	         AND (a.id <> d.sriov_vf_port_id );
      ADJUSTVALUE = oldPortFreenum;
  END IF;
  
  BEGIN     
        IF in_status > 0 THEN 
            UPDATE port SET free_sriovvf_num = (oldfreenum + ADJUSTVALUE) WHERE id = in_port_id;
        ELSE 
            UPDATE port SET free_sriovvf_num = (0) WHERE id = in_port_id;
        END IF;
                
        -- 判断该SRIOV 网口是否加入了EDVS， 如时，需更新EDVS nic_max_num_cfg
        SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id AND a.is_consistency = 1;
        SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id;
        SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id;
        IF is_exist > 0 THEN 
            UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum + ADJUSTVALUE) WHERE switch_id = edvsid;
        END IF;
        
        -- 判断该SRIOV 网口是否加入了BOND
        SELECT count(*) INTO is_exist FROM bond_map a, port b WHERE a.physical_port_id = in_port_id AND a.bond_id = b.id AND b.is_consistency = 1;
        SELECT bond_id INTO bondid FROM bond_map WHERE physical_port_id = in_port_id;
        IF is_exist > 0 THEN 
            SELECT total_sriovvf_num INTO oldtotalnum FROM port WHERE id = bondid ;
            SELECT free_sriovvf_num INTO oldfreenum FROM port WHERE id = bondid ;
            IF in_status > 0 THEN 
                UPDATE port SET free_sriovvf_num = (oldfreenum + ADJUSTVALUE) WHERE id = bondid;
            ELSE 
                UPDATE port SET free_sriovvf_num = (oldfreenum + ADJUSTVALUE) WHERE id = bondid;
            END IF;
            
            -- bond 口的total_sriov_num, valid_sriov_num 
            result := pf_net_tr_calc_bond_sriovport_num(bondid);
            IF result > 0 THEN 
            
            END IF;
            
            -- 判断该BOND口是否加入了EDVS ， 如时，需更新EDVS nic_max_num_cfg
            SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid AND a.is_consistency = 1;
            SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid;
            SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid;
            IF is_exist > 0 THEN 
                UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum + ADJUSTVALUE) WHERE switch_id = edvsid;
            END IF;    

        END IF;
  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


--删除SRIOV网口时 时触发 流程 
-- 流程 ( 网口从sriov-->非sriov) 
-- 流程 ( 网口从非sriov-->sriov)  
CREATE OR REPLACE FUNCTION pf_net_tr_sriovport_change_phyport( in_port_id numeric(19,0))
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  oldPortTotalnum integer := 0;
  oldPortFreenum integer := 0;
  oldtotalnum integer := 0;
  oldfreenum integer := 0;
  bondid numeric(19,0);
  edvsid numeric(19,0);
  oldedvsnicmaxnum integer :=0;
  oldsriovvfisconsistency integer :=0;
  sriovisok integer := 0;
  ADJUSTVALUE integer :=0;
  bondisok integer := 0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0;
  
  SELECT count(*) INTO is_exist FROM port  WHERE id = in_port_id;
  SELECT total_sriovvf_num INTO oldPortTotalnum FROM port  WHERE id = in_port_id ;
  SELECT free_sriovvf_num INTO oldPortFreenum FROM port  WHERE id = in_port_id ;
  SELECT is_consistency INTO sriovisok FROM port  WHERE id = in_port_id ;
  -- update port total_num / free_num 
  IF is_exist <= 0 THEN 
    result := RTN_ITEM_NO_EXIST;
    return result;
  END IF; 
    
  IF (oldPortTotalnum =0 AND oldPortFreenum = 0 ) THEN 
      result := 0;
      return result;
  END IF;
  
  BEGIN    
        UPDATE port SET free_sriovvf_num = 0, total_sriovvf_num = 0  WHERE id = in_port_id;
                
        -- 判断该SRIOV 网口是否加入了EDVS， 如时，需更新EDVS nic_max_num_cfg
        SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id AND a.is_consistency = 1;
        SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id;
        SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id;
        IF is_exist > 0 THEN    
            IF sriovisok > 0 THEN 
                UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum - oldPortFreenum) WHERE switch_id = edvsid;            
            END IF;
        END IF;
        
        -- 判断该SRIOV 网口是否加入了BOND        
		SELECT count(*) INTO is_exist FROM bond_map a, port b WHERE a.bond_id = b.id AND a.physical_port_id = in_port_id AND b.is_consistency = 1;
        SELECT bond_id INTO bondid FROM bond_map WHERE physical_port_id = in_port_id;
        IF is_exist > 0 THEN 
            SELECT total_sriovvf_num INTO oldtotalnum FROM port WHERE id = bondid ;
            SELECT free_sriovvf_num INTO oldfreenum FROM port WHERE id = bondid ;
            UPDATE port SET total_sriovvf_num = (oldtotalnum - oldPortTotalnum) WHERE id = bondid;

            SELECT count(*) INTO bondisok FROM bond_map a, port b 
               WHERE a.physical_port_id = in_port_id AND a.bond_id = b.id AND b.is_consistency = 1;
            IF (bondisok > 0 ) AND (sriovisok > 0) THEN 
                UPDATE port SET free_sriovvf_num = (oldfreenum - oldPortFreenum) WHERE id = bondid;
            END IF;
            
            -- bond 口的total_sriov_num, valid_sriov_num 
            result := pf_net_tr_calc_bond_sriovport_num(bondid);
            IF result > 0 THEN 
            
            END IF;
                        
            -- 判断该BOND口是否加入了EDVS ， 如时，需更新EDVS nic_max_num_cfg
            SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid AND a.is_consistency = 1;
            SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid;
            SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid;
            IF (is_exist > 0) AND (bondisok > 0 ) AND (sriovisok > 0) THEN                 
                UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum - oldPortFreenum) WHERE switch_id = edvsid;                
            END IF;    

        END IF;
  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

--修改BOND网口时 时触发 流程 
-- 流程 (online -> offline or offline -> online ) 
CREATE OR REPLACE FUNCTION pf_net_tr_modify_bondport( in_port_id numeric(19,0), in_status integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  oldPortTotalnum integer := 0;
  oldPortFreenum integer := 0;
  oldtotalnum integer := 0;
  oldfreenum integer := 0;
  bondid numeric(19,0);
  edvsid numeric(19,0);
  oldedvsnicmaxnum integer :=0;
  oldsriovvfisconsistency integer :=0;
  ADJUSTVALUE integer :=0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0;
  
  SELECT count(*) INTO is_exist FROM port a, bond b WHERE a.id = in_port_id AND a.id = b.port_id ;
  SELECT a.total_sriovvf_num INTO oldPortTotalnum FROM port a, bond b WHERE a.id = in_port_id AND a.id = b.port_id ;
  SELECT a.free_sriovvf_num INTO oldPortFreenum FROM port a, bond b WHERE a.id = in_port_id AND a.id = b.port_id ;
  -- update port total_num / free_num 
  IF is_exist <= 0 THEN 
    result := RTN_ITEM_NO_EXIST;
    return result;
  END IF; 
  
  -- 该BOND为非SRIOV 口bond，无需更新EDVS 
  IF (oldPortTotalnum = 0 AND oldPortFreenum = 0) THEN 
      result := 0;
      return result;
  END IF;
    
  BEGIN     
        IF in_status <= 0 THEN           
            ADJUSTVALUE = (0 - oldPortFreenum );
            UPDATE port SET free_sriovvf_num = 0 WHERE id = in_port_id;
        ELSE 
            ADJUSTVALUE = 0;
            SELECT sum(b.free_sriovvf_num) INTO ADJUSTVALUE FROM bond_map a, port b 
              WHERE a.physical_port_id = b.id AND b.is_consistency = 1 AND a.bond_id = in_port_id;  
            UPDATE port SET free_sriovvf_num = ADJUSTVALUE WHERE id = in_port_id;
        END IF;
                
        -- 判断该bond 网口是否加入了EDVS， 如时，需更新EDVS nic_max_num_cfg
        SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id AND a.is_consistency = 1;
        SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id;
        SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_port_id;
        IF is_exist > 0 THEN 
            UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum + ADJUSTVALUE) WHERE switch_id = edvsid;            
        END IF;
        
  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 添加 bond_map 时触发
CREATE OR REPLACE FUNCTION pf_net_tr_add_bondmap(in_bond_id numeric(19,0), in_port_id numeric(19,0))
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  oldtotalnum integer := 0;
  oldfreenum integer := 0;
  porttotalnum integer := 0;
  portfreenum integer := 0;
  edvsid numeric(19,0);
  oldedvsnicmaxnum integer :=0;
  bondisok integer := 0;
  phyisok integer := 0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0; 
  
    SELECT count(*) INTO is_exist FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
    SELECT a.total_sriovvf_num INTO porttotalnum FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
    SELECT a.free_sriovvf_num INTO portfreenum FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
    -- update port total_num / free_num 
    IF is_exist <= 0 THEN 
      --result := RTN_ITEM_NO_EXIST;  -- 存在非SRIOV也能触发此流程
      return result;
    END IF;

    SELECT count(*) INTO phyisok FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id AND a.is_consistency = 1 ;
    
    SELECT count(*) INTO is_exist FROM port  WHERE id = in_bond_id ;
    SELECT total_sriovvf_num INTO oldtotalnum FROM port  WHERE id = in_bond_id ;
    SELECT free_sriovvf_num INTO oldfreenum FROM port  WHERE id = in_bond_id ;
    IF is_exist <= 0 THEN 
      --result := RTN_ITEM_NO_EXIST; -- 存在非SRIOV也能触发此流程
      return result;
    END IF;

    SELECT count(*) INTO bondisok FROM port  WHERE id = in_bond_id  AND is_consistency = 1;
    
  BEGIN 
        UPDATE port SET total_sriovvf_num = (oldtotalnum +porttotalnum) WHERE id = in_bond_id;

        IF (phyisok > 0) AND (bondisok > 0 ) THEN 
            UPDATE port SET free_sriovvf_num = (oldfreenum + portfreenum) WHERE id = in_bond_id;
        END IF;
        
        -- bond 口的total_sriov_num, valid_sriov_num 
        result := pf_net_tr_calc_bond_sriovport_num(in_bond_id);
        IF result > 0 THEN 
            
        END IF;
        
        -- 判断该BOND 网口是否加入了EDVS， 如时，需更新EDVS nic_max_num_cfg
        SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_bond_id AND a.is_consistency = 1;
        SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_bond_id;
        SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_bond_id;
        IF (is_exist > 0) AND (phyisok > 0) AND (bondisok > 0 ) THEN 
            UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum + portfreenum) WHERE switch_id = edvsid;
        END IF;

  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 删除 bond_map 时触发
CREATE OR REPLACE FUNCTION pf_net_tr_del_bondmap(in_bond_id numeric(19,0), in_port_id numeric(19,0))
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  oldtotalnum integer := 0;
  oldfreenum integer := 0;
  porttotalnum integer := 0;
  portfreenum integer := 0;
  edvsid numeric(19,0);
  oldedvsnicmaxnum integer :=0;
  bondisok integer := 0;
  phyisok integer := 0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0; 
  
    SELECT count(*) INTO is_exist FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
    SELECT a.total_sriovvf_num INTO porttotalnum FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
    SELECT a.free_sriovvf_num INTO portfreenum FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id ;
    -- update port total_num / free_num 
    IF is_exist <= 0 THEN 
      --result := RTN_ITEM_NO_EXIST;  -- 存在非SRIOV也能触发此流程
      return result;
    END IF;
    
    SELECT count(*) INTO phyisok FROM port a, sriov_port b WHERE a.id = in_port_id AND a.id = b.physical_port_id AND a.is_consistency = 1 ;
    
    SELECT count(*) INTO is_exist FROM port  WHERE id = in_bond_id ;
    SELECT total_sriovvf_num INTO oldtotalnum FROM port  WHERE id = in_bond_id ;
    SELECT free_sriovvf_num INTO oldfreenum FROM port  WHERE id = in_bond_id ;
    IF is_exist <= 0 THEN 
      --result := RTN_ITEM_NO_EXIST;  -- 存在非SRIOV也能触发此流程
      return result;
    END IF;

    SELECT count(*) INTO bondisok FROM port  WHERE id = in_bond_id  AND is_consistency = 1;
    
  BEGIN 
        UPDATE port SET total_sriovvf_num = (oldtotalnum - porttotalnum) WHERE id = in_bond_id;

        IF (phyisok > 0) AND (bondisok > 0 ) THEN 
            UPDATE port SET free_sriovvf_num = (oldfreenum - portfreenum) WHERE id = in_bond_id;
        END IF;

        -- bond 口的total_sriov_num, valid_sriov_num 
        result := pf_net_tr_calc_bond_sriovport_num(in_bond_id);
        IF result > 0 THEN 
            
        END IF;
        
        -- 判断网口是否加入了SWITCH
        -- 判断该BOND 网口是否加入了EDVS， 如时，需更新EDVS nic_max_num_cfg
        SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_bond_id AND a.is_consistency = 1;
        SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_bond_id;
        SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = in_bond_id;
        IF (is_exist > 0) AND (phyisok > 0) AND (bondisok > 0 ) THEN 
            UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum - portfreenum) WHERE switch_id = edvsid;
        END IF;
        
  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 添加 edvs_map 时触发
CREATE OR REPLACE FUNCTION pf_net_tr_add_edvsmap(in_evds_id numeric(19,0), in_port_id numeric(19,0))
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  oldtotalnum integer := 0;
  oldfreenum integer := 0;
  porttotalnum integer := 0;
  portfreenum integer := 0;
  edvsid numeric(19,0);
  oldedvsnicmaxnum integer :=0;
  portisok integer := 0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0; 
  
    SELECT count(*) INTO is_exist FROM virtual_switch WHERE switch_id = in_evds_id AND switch_type = 2;
    SELECT nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch WHERE switch_id = in_evds_id AND switch_type = 2;
    IF is_exist <= 0 THEN 
      --sdvs 也会调用 
      --result := RTN_ITEM_NO_EXIST;
      return result;
    END IF;
  
    SELECT count(*) INTO is_exist FROM port WHERE id = in_port_id ;
    SELECT total_sriovvf_num INTO porttotalnum FROM port WHERE id = in_port_id ;
    SELECT free_sriovvf_num INTO portfreenum FROM port WHERE id = in_port_id ;
    -- update port total_num / free_num 
    IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
    END IF;

    SELECT count(*) INTO portisok FROM port WHERE id = in_port_id AND is_consistency = 1 ;
        
  BEGIN 
      IF portisok > 0 THEN 
          UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum +portfreenum)  WHERE switch_id = in_evds_id;    
      END IF;
        
  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 删除 edvs_map 时触发
CREATE OR REPLACE FUNCTION pf_net_tr_del_edvsmap(in_evds_id numeric(19,0), in_port_id numeric(19,0))
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  oldtotalnum integer := 0;
  oldfreenum integer := 0;
  porttotalnum integer := 0;
  portfreenum integer := 0;
  edvsid numeric(19,0);
  oldedvsnicmaxnum integer :=0;
  portisok integer := 0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
BEGIN
  result := 0; 
  
    SELECT count(*) INTO is_exist FROM virtual_switch WHERE switch_id = in_evds_id AND switch_type = 2;
    SELECT nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch WHERE switch_id = in_evds_id AND switch_type = 2;
    IF is_exist <= 0 THEN 
      --sdvs 也会调用 
      --result := RTN_ITEM_NO_EXIST;
      return result;
    END IF;
  
    SELECT count(*) INTO is_exist FROM port WHERE id = in_port_id ;
    SELECT total_sriovvf_num INTO porttotalnum FROM port WHERE id = in_port_id ;
    SELECT free_sriovvf_num INTO portfreenum FROM port WHERE id = in_port_id ;
    -- update port total_num / free_num 
    IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
    END IF;

    SELECT count(*) INTO portisok FROM port WHERE id = in_port_id AND is_consistency = 1 ;
        
  BEGIN 
  
    IF portisok > 0 THEN 
        UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum - portfreenum)  WHERE switch_id = in_evds_id; 
    END IF;
      
  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 修改 edvs_map 时触发 
-- in_status = 1, is_consistency 0-->1
-- in_status = 0, is_consistency 1-->0
CREATE OR REPLACE FUNCTION pf_net_tr_modify_edvsmap(in_evds_id numeric(19,0), in_port_id numeric(19,0), in_status integer)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  oldtotalnum integer := 0;
  oldfreenum integer := 0;
  porttotalnum integer := 0;
  portfreenum integer := 0;
  edvsid numeric(19,0);
  oldedvsnicmaxnum integer :=0;
  portisok integer := 0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0; 
  
    SELECT count(*) INTO is_exist FROM virtual_switch WHERE switch_id = in_evds_id AND switch_type = 2;
    SELECT nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch WHERE switch_id = in_evds_id AND switch_type = 2;
    IF is_exist <= 0 THEN 
      --sdvs 也会调用 
      --result := RTN_ITEM_NO_EXIST;
      return result;
    END IF;
  
    SELECT count(*) INTO is_exist FROM port WHERE id = in_port_id ;
    SELECT total_sriovvf_num INTO porttotalnum FROM port WHERE id = in_port_id ;
    SELECT free_sriovvf_num INTO portfreenum FROM port WHERE id = in_port_id ;
    -- update port total_num / free_num 
    IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
    END IF;

    SELECT count(*) INTO portisok FROM port WHERE id = in_port_id AND is_consistency = 1 ;
        
  BEGIN 
      IF (in_status > 0) AND (portisok > 0 ) THEN 
          UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum + portfreenum)  WHERE switch_id = in_evds_id; 
      END IF;
      
      IF (in_status <= 0) AND (portisok > 0 ) THEN 
          UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum - portfreenum)  WHERE switch_id = in_evds_id;            
      END IF;
  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 


-- 添加 vsi_sriov 时触发
CREATE OR REPLACE FUNCTION pf_net_tr_add_vsisriov(in_vsi_id numeric(19,0), in_port_id numeric(19,0))
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  oldtotalnum integer := 0;
  oldfreenum integer := 0;
  porttotalnum integer := 0;
  portfreenum integer := 0;
  sriovportid numeric(19,0);
  bondid numeric(19,0);
  edvsid numeric(19,0);
  oldedvsnicmaxnum integer :=0;
  sriovisok integer := 0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0; 
    
    SELECT count(*) INTO is_exist FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.id = in_port_id ;    
    SELECT b.sriov_port_id INTO sriovportid FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.id = in_port_id ;   
    IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
    END IF;
    
    SELECT total_sriovvf_num INTO porttotalnum FROM port WHERE id = sriovportid ;
    SELECT free_sriovvf_num INTO portfreenum FROM port WHERE id = sriovportid ;
    -- update port total_num / free_num 
    
    SELECT is_consistency INTO sriovisok FROM port WHERE id = sriovportid AND is_consistency =1 ;

    IF (sriovisok <= 0 ) THEN 
       result := 0;
       return result;
    END IF;       
  BEGIN 
      IF (sriovisok > 0 ) THEN 
          UPDATE port SET free_sriovvf_num = (portfreenum - 1) WHERE id = sriovportid ;
      END IF;
      
      -- 判断该SRIOV 网口是否加入了EDVS， 如时，需更新EDVS nic_max_num_cfg
        SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = sriovportid AND a.is_consistency = 1;
        SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = sriovportid;
        SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = sriovportid;
        IF is_exist > 0 THEN 
            UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum - 1) WHERE switch_id = edvsid;
        END IF;
        
        -- 判断该SRIOV 网口是否加入了BOND
		SELECT count(*) INTO is_exist FROM bond_map a, port b WHERE a.bond_id = b.id AND a.physical_port_id = sriovportid AND b.is_consistency = 1;
        SELECT bond_id INTO bondid FROM bond_map WHERE physical_port_id = sriovportid;
        IF is_exist > 0 THEN 
            SELECT free_sriovvf_num INTO oldfreenum FROM port WHERE id = bondid ;
            UPDATE port SET free_sriovvf_num = (oldfreenum - 1) WHERE id = bondid;
            
            -- bond 口的total_sriov_num, valid_sriov_num 
            result := pf_net_tr_calc_bond_sriovport_num(bondid);
            IF result > 0 THEN 
            
            END IF;
            
            -- 判断该BOND口是否加入了EDVS ， 如时，需更新EDVS nic_max_num_cfg
            SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid AND a.is_consistency = 1;
            SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid;
            SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid;
            IF is_exist > 0 THEN 
                UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum - 1) WHERE switch_id = edvsid;
            END IF;            
        END IF;
        
  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 删除 vsi_sriov 时触发
CREATE OR REPLACE FUNCTION pf_net_tr_del_vsisriov(in_vsi_id numeric(19,0), in_port_id numeric(19,0))
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  oldtotalnum integer := 0;
  oldfreenum integer := 0;
  porttotalnum integer := 0;
  portfreenum integer := 0;
  sriovportid numeric(19,0);
  bondid numeric(19,0);
  edvsid numeric(19,0);
  oldedvsnicmaxnum integer :=0;
  sriovisok integer := 0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0; 
    
    SELECT count(*) INTO is_exist FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.id = in_port_id ;    
    SELECT b.sriov_port_id INTO sriovportid FROM port a, sriov_vf_port b WHERE a.id = b.port_id AND a.id = in_port_id ;   
    IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
      return result;
    END IF;
    
    SELECT total_sriovvf_num INTO porttotalnum FROM port WHERE id = sriovportid ;
    SELECT free_sriovvf_num INTO portfreenum FROM port WHERE id = sriovportid ;
    -- update port total_num / free_num 

    SELECT is_consistency INTO sriovisok FROM port WHERE id = sriovportid AND is_consistency =1 ;

    IF (sriovisok <= 0 ) THEN 
       result := 0;
       return result;
    END IF;        
        
  BEGIN 
      UPDATE port SET free_sriovvf_num = (portfreenum + 1) WHERE id = sriovportid ;
      
      -- 判断该SRIOV 网口是否加入了EDVS， 如时，需更新EDVS nic_max_num_cfg
        SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = sriovportid AND a.is_consistency = 1;
        SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = sriovportid;
        SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
            WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = sriovportid;
        IF is_exist > 0 THEN 
            UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum + 1) WHERE switch_id = edvsid;
        END IF;
        
        -- 判断该SRIOV 网口是否加入了BOND
		SELECT count(*) INTO is_exist FROM bond_map a, port b WHERE a.bond_id = b.id AND a.physical_port_id = sriovportid AND b.is_consistency = 1;
        SELECT bond_id INTO bondid FROM bond_map WHERE physical_port_id = sriovportid;
        IF is_exist > 0 THEN 
            SELECT free_sriovvf_num INTO oldfreenum FROM port WHERE id = bondid ;
            UPDATE port SET free_sriovvf_num = (oldfreenum + 1) WHERE id = bondid;
            
            -- bond 口的total_sriov_num, valid_sriov_num 
            result := pf_net_tr_calc_bond_sriovport_num(bondid);
            IF result > 0 THEN 
            
            END IF;
            
            -- 判断该BOND口是否加入了EDVS ， 如时，需更新EDVS nic_max_num_cfg
            SELECT count(*) INTO is_exist FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid AND a.is_consistency = 1;
            SELECT b.switch_id INTO edvsid FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid;
            SELECT b.nic_max_num_cfg INTO oldedvsnicmaxnum FROM virtual_switch_map a, virtual_switch b 
                WHERE a.virtual_switch_id = b.switch_id AND b.switch_type = 2 AND a.port_id = bondid;
            IF is_exist > 0 THEN 
                UPDATE virtual_switch SET nic_max_num_cfg = (oldedvsnicmaxnum + 1) WHERE switch_id = edvsid;
            END IF;            
        END IF;
        
  EXCEPTION 
    WHEN others
    THEN
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
   
