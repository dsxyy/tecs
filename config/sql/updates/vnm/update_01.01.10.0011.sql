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

-- 解决 调度模块，对nic_num 在非loop情况不用考虑
--
------------------------------------------------------------------------------------
 
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
  VALUES ('01.01.10.0011', 'tecs-vnm_test',now(),'first version');

  
  
CREATE OR REPLACE FUNCTION pf_net_schedule(in_netplane_uuid character varying, 
                           in_is_sriov integer, in_is_loop integer, in_nic_num integer, in_is_bond_nic integer,
                           in_mtu integer , in_is_watchdog integer)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    check_ret integer;
    netplaneid numeric(19,0);
    
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
   
    SELECT count(*) INTO is_exist  FROM netplane WHERE uuid = in_netplane_uuid;
    SELECT id INTO netplaneid  FROM netplane WHERE uuid = in_netplane_uuid;
    IF is_exist <= 0 THEN 
        return ref;
    END IF;
	
	IF (  ((in_is_sriov = 0 ) AND (in_is_loop = 0 )) OR ((in_is_sriov = 1 ) AND ((in_is_loop = 0 ) AND (in_is_bond_nic = 0)) ) ) THEN 
	    in_nic_num = 1;
	END IF;
    
    -- switch chioce 
IF     in_is_watchdog <= 0 THEN     
    IF in_is_sriov > 0 THEN       
        IF in_is_bond_nic > 0 THEN 
            return pf_net_edvs_schedule_has_bond_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',0);             
        ELSE        
            return pf_net_edvs_schedule_no_bond_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',0);             
        END IF;
    ELSE
        return pf_net_sdvs_schedule_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',0);    
    END IF;
ELSE
    IF in_is_sriov > 0 THEN       
        IF in_is_bond_nic > 0 THEN 
            return pf_net_edvs_schedule_has_bond_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',0);             
        ELSE        
            return pf_net_edvs_schedule_no_bond_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',0);             
        END IF;
    ELSE
        return pf_net_sdvs_schedule_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',0);    
    END IF;
END IF;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  

  
CREATE OR REPLACE FUNCTION pf_net_schedule_by_cluster(in_netplane_uuid character varying, 
                           in_is_sriov integer, in_is_loop integer, in_nic_num integer, in_is_bond_nic integer,
                           in_mtu integer, in_cluster_name character varying, in_is_watchdog integer)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    check_ret integer;
    netplaneid numeric(19,0);
    clusterid  numeric(19,0);
    
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
   
    SELECT count(*) INTO is_exist  FROM netplane WHERE uuid = in_netplane_uuid;
    SELECT id INTO netplaneid  FROM netplane WHERE uuid = in_netplane_uuid;
    IF is_exist <= 0 THEN 
        -- 鐩存帴杩斿洖 鍙傛暟鏈夎
        return ref;
    END IF;
	
	IF (  ((in_is_sriov = 0 ) AND (in_is_loop = 0 )) OR ((in_is_sriov = 1 ) AND ((in_is_loop = 0 ) AND (in_is_bond_nic = 0)) ) ) THEN 
	    in_nic_num = 1;
	END IF;
	                
    -- switch chioce 
IF     in_is_watchdog <= 0 THEN     
    IF in_is_sriov > 0 THEN       
        IF in_is_bond_nic > 0 THEN 
            return pf_net_edvs_schedule_has_bond_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,1,in_cluster_name,0);             
        ELSE        
            return pf_net_edvs_schedule_no_bond_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,1,in_cluster_name,0);             
        END IF;
    ELSE
        return pf_net_sdvs_schedule_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,1,in_cluster_name,0);    
    END IF;
ELSE
    IF in_is_sriov > 0 THEN       
        IF in_is_bond_nic > 0 THEN 
            return pf_net_edvs_schedule_has_bond_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,1,in_cluster_name,0);             
        ELSE        
            return pf_net_edvs_schedule_no_bond_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,1,in_cluster_name,0);             
        END IF;
    ELSE
        return pf_net_sdvs_schedule_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,1,in_cluster_name,0);    
    END IF;
END IF;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
    
  
  

CREATE OR REPLACE FUNCTION pf_net_schedule_by_vna(in_netplane_uuid character varying, 
                           in_is_sriov integer, in_is_loop integer, in_nic_num integer, in_is_bond_nic integer,
                           in_mtu integer, in_vna_uuid character varying, in_is_watchdog integer)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    check_ret integer;
    netplaneid numeric(19,0);
    vnaid numeric(19,0);
    
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
   
    SELECT count(*) INTO is_exist  FROM netplane WHERE uuid = in_netplane_uuid;
    SELECT id INTO netplaneid  FROM netplane WHERE uuid = in_netplane_uuid;
    IF is_exist <= 0 THEN 
        -- 鐩存帴杩斿洖 鍙傛暟鏈夎
        return ref;
    END IF;

    SELECT count(*) INTO is_exist  FROM vna WHERE vna_uuid = in_vna_uuid;
    SELECT id INTO vnaid  FROM vna WHERE vna_uuid = in_vna_uuid;
    IF is_exist <= 0 THEN 
        -- 鐩存帴杩斿洖 鍙傛暟鏈夎
        return ref;
    END IF;
	
	IF (  ((in_is_sriov = 0 ) AND (in_is_loop = 0 )) OR ((in_is_sriov = 1 ) AND ((in_is_loop = 0 ) AND (in_is_bond_nic = 0)) ) ) THEN 
	    in_nic_num = 1;
	END IF;
        
    -- switch chioce 
IF     in_is_watchdog <= 0 THEN     
    IF in_is_sriov > 0 THEN       
        IF in_is_bond_nic > 0 THEN 
            return pf_net_edvs_schedule_has_bond_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',vnaid);             
        ELSE        
            return pf_net_edvs_schedule_no_bond_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',vnaid);             
        END IF;
    ELSE
        return pf_net_sdvs_schedule_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',vnaid);    
    END IF;
ELSE
    IF in_is_sriov > 0 THEN       
        IF in_is_bond_nic > 0 THEN 
            return pf_net_edvs_schedule_has_bond_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',vnaid);             
        ELSE        
            return pf_net_edvs_schedule_no_bond_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',vnaid);             
        END IF;
    ELSE
        return pf_net_sdvs_schedule_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',vnaid);    
    END IF;
END IF;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
     