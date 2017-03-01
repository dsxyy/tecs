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

-- 611003957571 【tecs】261版本，edvs的sriov网卡资源没上报 
--涉及pf_net_tr_add_sriovvfport 中在bond判断时，入参错误
-- 
------------------------------------------------------------------------------------
 
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
  VALUES ('01.01.10.0029', 'tecs-vnm_test',now(),'first version');

  

--添加SRIOV VF 时触发 流程 
-- sriovvf is_consistency != 1 时，仅对total_num 进行计数，不对free_num计数； bond,switch相同逻辑
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
		SELECT count(*) INTO is_exist FROM bond_map a, port b WHERE a.bond_id = b.id AND a.physical_port_id = in_port_id AND b.is_consistency = 1;
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
 
 