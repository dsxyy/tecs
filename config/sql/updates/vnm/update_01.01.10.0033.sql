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

-- EDVS SRIOV 计数优化，全部采用触发器类实现 
--
-- 
------------------------------------------------------------------------------------


/*
  不再简单地加减-处理，而是通过触发重新计数 
  
   计算sriov vf port 
   计算sriov port 
   计算bond 
   计算bond validnum
   计算switch 
   
   由下面几种event触发
   port 
   1 add port 
     sriov vf port ---> 由 sriov_vf_port 表触发器触发
     sriov port ---> 第一次添加肯定为0，无需操作
     bond       ---> 无需计算，由bond_map来触发， tr_calc_bond_port() tr_calc_bond_validnum_port()
　　
　　 update port
     如果 is_consistency，total_sriov_num, free_sriov_num 没有变化 将直接返回。
     sriov vf port ---> tr_calc_sriov_port()
     sriov port    ---> tr_calc_sriov_port()  ?  是否存在死循环触发 
                        exist(bond)  tr_calc_bond_port()  tr_calc_bond_validnum_port()
                        exist(switch)   tr_calc_switch()
     bond          ---> tr_calc_bond_port() tr_calc_bond_validnum_port()
                        exist(switch)   tr_calc_switch()
     delete port 
     sriov vf port ---> 由sriov vf port 表触发器触发
     sriov port    --->　if exist(switch)  tr_calc_switch()  由switch_map表
                   --->  if exist(bond) 由 bond_map 触发器触发 
     bond          --->  tr_calc_switch() bond_map也存在触发，是否存在问题?
     
  2 sriov vf port
    add port  ----> tr_calc_sriov_port()
    del port  ----> tr_calc_sriov_port()
    
  3 bond_map 
    add   ---->  tr_calc_bond_port() tr_calc_bond_validnum_port()
                 这些由bond表来触发  if exist(switch)   tr_calc_switch()
    update --->  tr_calc_bond_port() tr_calc_bond_validnum_port() 
    
    del   ---->  tr_calc_bond_port() tr_calc_bond_validnum_port()
                 由bond表来触发 if exist(switch)   tr_calc_switch()  
   
  4 virtual_switch_map
    add    ----->　   tr_calc_switch()  
    del    ----->     tr_calc_switch() 
    update ----->     tr_calc_switch() 
    
  5 update vna ---> tr_calc_switch()    

  6 vsi_sriov 
    add   ----—->  tr_calc_sriovport()
    
    del  -----> tr_calc_sriovport()
*/


 
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
  VALUES ('01.01.10.0033', 'tecs-vnm_test',now(),'first version');
  


CREATE OR REPLACE FUNCTION pf_net_tr_add_bondmap(in_bond_id numeric, in_port_id numeric)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
BEGIN
  result := 0; 
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


CREATE OR REPLACE FUNCTION pf_net_tr_add_edvsmap(in_evds_id numeric, in_port_id numeric)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
BEGIN
  result := 0; 
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



CREATE OR REPLACE FUNCTION pf_net_tr_add_sriovvfport(in_sriovvfport_id numeric, in_port_id numeric)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
BEGIN
  result := 0; 
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


CREATE OR REPLACE FUNCTION pf_net_tr_add_vsisriov(in_vsi_id numeric, in_port_id numeric)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
BEGIN
  result := 0; 
    
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



CREATE OR REPLACE FUNCTION pf_net_tr_calc_bond_sriovport_num(in_port_id numeric)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
BEGIN
  result := 0; 
      
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;




CREATE OR REPLACE FUNCTION pf_net_tr_del_bondmap(in_bond_id numeric, in_port_id numeric)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
BEGIN
  result := 0; 
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- Function: pf_net_tr_del_edvsmap(numeric, numeric)

-- DROP FUNCTION pf_net_tr_del_edvsmap(numeric, numeric);

CREATE OR REPLACE FUNCTION pf_net_tr_del_edvsmap(in_evds_id numeric, in_port_id numeric)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
BEGIN
  result := 0; 
  
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



CREATE OR REPLACE FUNCTION pf_net_tr_del_vsisriov(in_vsi_id numeric, in_port_id numeric)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
BEGIN
  result := 0; 
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



CREATE OR REPLACE FUNCTION pf_net_tr_modify_bondport(in_port_id numeric, in_status integer)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
BEGIN
  result := 0;
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



CREATE OR REPLACE FUNCTION pf_net_tr_modify_edvsmap(in_evds_id numeric, in_port_id numeric, in_status integer)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
BEGIN
  result := 0; 
  
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



CREATE OR REPLACE FUNCTION pf_net_tr_modify_sriovport(in_port_id numeric, in_status integer)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
BEGIN
  result := 0;
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



CREATE OR REPLACE FUNCTION pf_net_tr_modify_sriovvfport(in_sriovvfport_id numeric, in_port_id numeric, in_status integer)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
BEGIN
  result := 0;
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



CREATE OR REPLACE FUNCTION pf_net_tr_sriovport_change_phyport(in_port_id numeric)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
BEGIN
  result := 0;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- Function: pf_net_tr_vsi_attach_vswitch(numeric, numeric)

-- DROP FUNCTION pf_net_tr_vsi_attach_vswitch(numeric, numeric);

CREATE OR REPLACE FUNCTION pf_net_tr_vsi_attach_vswitch(in_vsi_id numeric, in_switch_id numeric)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
BEGIN
  result := 0; 
  
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


CREATE OR REPLACE FUNCTION pf_net_tr_vsi_unattach_vswitch(in_vsi_id numeric, in_switch_id numeric)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
BEGIN
  result := 0; 
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



/*   以上是清除old计数    */



/* 计算 bond， sriovvf num */
CREATE OR REPLACE FUNCTION pf_tr_calc_bondport(in_port_id numeric(19,0))
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  totalNum integer := 0;
  freeNum integer := 0; 
  
  isOk integer  := 0; 
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0; 
    
  BEGIN 
      SELECT is_consistency INTO isOk FROM port WHERE id = in_port_id;
      
      IF isOk = 1 THEN       
          SELECT COALESCE(sum(b.total_sriovvf_num),0) INTO totalNum  FROM bond_map a,port b WHERE a.bond_id = in_port_id AND a.physical_port_id = b.id AND b.is_consistency =1 
                AND a.is_cfg = 1 AND a.is_selected =1  ;
          SELECT COALESCE(sum(b.free_sriovvf_num),0) INTO freeNum  FROM bond_map a,port b WHERE a.bond_id = in_port_id AND a.physical_port_id = b.id AND b.is_consistency =1 
                AND a.is_cfg = 1 AND a.is_selected =1  ;
          UPDATE port SET total_sriovvf_num = totalNum, free_sriovvf_num = freeNum  WHERE id = in_port_id;
      ELSE 
          UPDATE port SET total_sriovvf_num = 0, free_sriovvf_num = 0 WHERE id = in_port_id;
      END IF;
      
  EXCEPTION 
    WHEN others
    THEN
    RAISE WARNING 'pf_tr_calc_bondport(%) failed.',in_port_id;
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
    
    

/* 计算 bond， valid num */
CREATE OR REPLACE FUNCTION pf_tr_calc_bondport_validnum(in_port_id numeric(19,0))
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  totalNum integer := 0;
  validNum integer := 0; 
  
  isOk integer  := 0; 
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0; 
    
  BEGIN 
     SELECT count(*) INTO totalNum  FROM bond_map a,port b WHERE a.bond_id = in_port_id AND a.physical_port_id = b.id ;
     SELECT count(*) INTO validNum  FROM bond_map a,port b WHERE a.bond_id = in_port_id AND a.physical_port_id = b.id AND b.is_consistency =1 
            AND a.is_cfg = 1 AND a.is_selected =1  AND b.free_sriovvf_num > 0 ;
     UPDATE bond SET total_sriovport_num = totalNum, valid_sriovport_num = validNum  WHERE port_id = in_port_id;
     
  EXCEPTION 
    WHEN others
    THEN
    RAISE WARNING 'pf_tr_calc_bondport_validnum(%) failed.',in_port_id;
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



/* 计算 sroiv port， sriovvf num*/
CREATE OR REPLACE FUNCTION pf_tr_calc_sriovport(in_port_id numeric(19,0))
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  totalNum integer := 0;
  freeNum integer := 0; 
  
  isOk integer  := 0; 
  retvalue integer := 0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0; 
    
  BEGIN 
      SELECT is_consistency INTO isOk FROM port WHERE id = in_port_id;
      
      IF isOk = 1 THEN       
          SELECT count(*) INTO totalNum  FROM sriov_vf_port a,port b WHERE a.port_id = b.id AND b.is_consistency = 1 AND a.sriov_port_id = in_port_id ;
          SELECT count(*) INTO freeNum  FROM sriov_vf_port a,port b WHERE a.port_id = b.id AND b.is_consistency = 1 AND a.sriov_port_id = in_port_id 
             AND NOT EXISTS (SELECT sriov_vf_port_id FROM vsi_sriov WHERE sriov_vf_port_id = b.id );
          UPDATE port SET total_sriovvf_num = totalNum, free_sriovvf_num = freeNum  WHERE id = in_port_id;
      ELSE 
          UPDATE port SET total_sriovvf_num = 0, free_sriovvf_num = 0 WHERE id = in_port_id;
      END IF;
      
  EXCEPTION 
    WHEN others
    THEN
    RAISE WARNING 'pf_tr_calc_sriovport(%) failed.',in_port_id;
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;

  





/* 计算 swtich, sriov vf num */
CREATE OR REPLACE FUNCTION pf_tr_calc_switch(in_sw_id numeric(19,0))
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  totalNum integer := 0;
  freeNum integer := 0; 
  
  isOk integer  := 0; 
  isEdvs integer := 0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0; 
    
  BEGIN 
     /*virtual_switch_map.is_consistency = 1 
       port.is_consistency = 1
       port.vna.is_consistency = 1 
     */
     
     /* edvs 才需要更新，sdvs 将直接返回 
     */
     SELECT count(*) INTO isEdvs FROM switch WHERE id = in_sw_id AND switch_type = 2; 
     IF isEdvs = 0 THEN 
         RETURN result;
     END IF;
          
     SELECT COALESCE(sum(total_sriovvf_num),0) INTO totalNum 
     FROM 
        switch a,
        virtual_switch_map b,
        port c,
        vna d
     WHERE 
        a.id =     in_sw_id AND 
        a.id = b.virtual_switch_id AND 
        b.is_consistency  = 1 AND 
        b.port_id = c.id AND 
        c.is_consistency = 1 AND 
        c.vna_id = d.id AND 
        d.is_online = 1 ;
        
     SELECT COALESCE(sum(free_sriovvf_num),0) INTO freeNum 
     FROM 
        switch a,
        virtual_switch_map b,
        port c,
        vna d
     WHERE 
        a.id =     in_sw_id AND 
        a.id = b.virtual_switch_id AND 
        b.is_consistency  = 1 AND 
        b.port_id = c.id AND 
        c.is_consistency = 1 AND 
        c.vna_id = d.id AND 
        d.is_online = 1 ;        
       
     IF ( totalNum - freeNum ) < 0 THEN 
         /* 有异常 */
         RAISE WARNING 'pf_tr_calc_switch faild. sw:%,totalNum:%,freeNum:%',in_sw_id,totalNum,freeNum;
         UPDATE virtual_switch SET nic_max_num_cfg = freeNum, deploy_vsi_num = (totalNum - freeNum)  WHERE switch_id = in_sw_id;
     ELSE 
         UPDATE virtual_switch SET nic_max_num_cfg = freeNum, deploy_vsi_num = (totalNum - freeNum)  WHERE switch_id = in_sw_id;
     END IF;
     
  EXCEPTION 
    WHEN others
    THEN
    RAISE WARNING 'pf_tr_calc_switch(%) failed.',in_sw_id;
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  
  

/* 以上是触发器 调用函数 */  

/* bond_map 触发器 */



CREATE OR REPLACE FUNCTION tr_insert_bond_map () RETURNS trigger AS $tr_insert_bond_map$
DECLARE
  isExist integer := 0;
  bondid numeric(19,0) := 0;
  swid numeric(19,0) := 0;
  retvalue integer := 0;
    BEGIN
        /* 
           bond_map 增加记录 
        */            
        bondid = NEW.bond_id;
        retvalue = pf_tr_calc_bondport(bondid);
        IF retvalue <> 0 THEN 
            RAISE WARNING 'tr_insert_bond_map call pf_tr_calc_bondport(%) failed. ret=%',bondid,retvalue;
            RETURN NULL;
        END IF;
        
        retvalue = pf_tr_calc_bondport_validnum(bondid);
        IF retvalue <> 0 THEN 
            RAISE WARNING 'tr_insert_bond_map call pf_tr_calc_bondport_validnum(%) failed. ret=%',bondid,retvalue;
            RETURN NULL;
        END IF;
        
        RETURN NULL;
    END;
$tr_insert_bond_map$ 
LANGUAGE plpgsql;

CREATE TRIGGER tr_insert_bond_map AFTER INSERT ON bond_map
    FOR EACH ROW EXECUTE PROCEDURE tr_insert_bond_map();    
    
    


CREATE OR REPLACE FUNCTION tr_update_bond_map () RETURNS trigger AS $tr_update_bond_map$
DECLARE
  bondid numeric(19,0) := 0;
  swid numeric(19,0) := 0;
  retvalue integer := 0;
    BEGIN
        /* 
           bond_map update记录 
        */            
        bondid = NEW.bond_id;
        retvalue = pf_tr_calc_bondport(bondid);
        IF retvalue <> 0 THEN 
            RAISE WARNING 'tr_update_bond_map call pf_tr_calc_bondport(%) failed. ret=%',bondid,retvalue;
            RETURN NULL;
        END IF;
        
        retvalue = pf_tr_calc_bondport_validnum(bondid);
        IF retvalue <> 0 THEN 
            RAISE WARNING 'tr_update_bond_map call pf_tr_calc_bondport_validnum(%) failed. ret=%',bondid,retvalue;
            RETURN NULL;
        END IF;
        
        RETURN NULL;
        
    END;
$tr_update_bond_map$ 
LANGUAGE plpgsql;

CREATE TRIGGER tr_update_bond_map AFTER UPDATE  ON bond_map
    FOR EACH ROW EXECUTE PROCEDURE tr_update_bond_map();
        
        

CREATE OR REPLACE FUNCTION tr_delete_bond_map () RETURNS trigger AS $tr_delete_bond_map$
DECLARE
  sriov_is_ok integer := 0;
  bondid numeric(19,0) := 0;
  retvalue integer := 0;
    BEGIN
        /* 
           bond_map DELETE 记录 
        */            
        bondid = NEW.bond_id;
        retvalue = pf_tr_calc_bondport(bondid);
        IF retvalue <> 0 THEN 
            RAISE WARNING 'tr_delete_bond_map call pf_tr_calc_bondport(%) failed. ret=%',bondid,retvalue;
            RETURN NULL;
        END IF;
        
        retvalue = pf_tr_calc_bondport_validnum(bondid);
        IF retvalue <> 0 THEN 
            RAISE WARNING 'tr_delete_bond_map call pf_tr_calc_bondport_validnum(%) failed. ret=%',bondid,retvalue;
            RETURN NULL;
        END IF;
        RETURN NULL;
    END;
$tr_delete_bond_map$ 
LANGUAGE plpgsql;

CREATE TRIGGER tr_delete_bond_map AFTER DELETE ON bond_map
    FOR EACH ROW EXECUTE PROCEDURE tr_delete_bond_map();
            
/* port 表触发器*/



CREATE OR REPLACE FUNCTION tr_update_port () RETURNS trigger AS $tr_update_port$
DECLARE
  isSriovPort integer := 0;
  isBondMember integer := 0;
  isswitchMember integer := 0;
  bondid numeric(19,0) := 0;
  sriovid numeric(19,0) := 0;
  switchid numeric(19,0) := 0;
  retvalue integer := 0;
    BEGIN
        /* 
           SRIOV VF PORT 
           只处理 =0 physical port;n=1 bond port;n=2 sriov vf port ,
           其他直接忽略退出 
        */        
        IF NEW.port_type > 2 THEN 
            RETURN NULL;
        END IF;
        
        /* 如果 is_consistency, total_sriovvf_num, free_sriovvf_num 没有变化的情况下，无需更新 */
        IF NEW.is_consistency = OLD.is_consistency AND NEW.total_sriovvf_num = OLD.total_sriovvf_num AND NEW.free_sriovvf_num = OLD.free_sriovvf_num THEN 
            RETURN NULL;
        END IF;
        
        IF NEW.port_type = 0 THEN 
            SELECT count(*) INTO isSriovPort FROM sriov_port WHERE physical_port_id = NEW.id;
            /*  不是sriov 直接退出 */
            IF isSriovPort = 1 THEN                
                retvalue = pf_tr_calc_sriovport(NEW.id);
                IF retvalue <> 0 THEN 
                    RAISE WARNING 'tr_update_port call pf_tr_calc_sriovport(%) failed. ret=%',NEW.id,retvalue;
                    RETURN NULL;
                END IF;
                
                SELECT count(*) INTO isBondMember FROM bond_map WHERE physical_port_id = NEW.id;
                IF isBondMember > 0 THEN 
                    SELECT bond_id INTO bondid FROM bond_map WHERE physical_port_id = NEW.id;
                    retvalue = pf_tr_calc_bondport(bondid);
                    IF retvalue <> 0 THEN 
                        RAISE WARNING 'tr_update_port(%) call pf_tr_calc_bondport(%) failed. ret=%',NEW.id,bondid,retvalue;
                        RETURN NULL;
                    END IF;

                    retvalue =  pf_tr_calc_bondport_validnum(bondid);
                    IF retvalue <> 0 THEN 
                        RAISE WARNING 'tr_update_port(%) call pf_tr_calc_bondport_validnum(%,%) failed. ret=%',NEW.id,bondid,retvalue;
                        RETURN NULL;
                    END IF;
                END IF;
                
                SELECT count(*) INTO isswitchMember FROM virtual_switch_map a, switch b WHERE a.virtual_switch_id = b.id  AND b.switch_type = 2 AND a.port_id = NEW.id; 
                SELECT b.id INTO switchid FROM virtual_switch_map a, switch b WHERE a.virtual_switch_id = b.id  AND b.switch_type = 2 AND a.port_id = NEW.id; 
                IF isswitchMember > 0 THEN 
                    retvalue = pf_tr_calc_switch(switchid);
                    IF retvalue <> 0 THEN 
                        RAISE WARNING 'tr_update_port(%) call pf_tr_calc_switch(%) failed. ret=%',NEW.id,switchid,retvalue;
                        RETURN NULL;
                    END IF;
                END IF;
                
            END IF;
            RETURN NULL; 
        END IF;
        
        IF NEW.port_type = 1 THEN 
            /* BOND  */        
            retvalue = pf_tr_calc_bondport(NEW.id);
            IF retvalue  <> 0THEN 
                RAISE WARNING 'tr_update_port(%) call pf_tr_calc_bondport(%) failed. ret=%',NEW.id,NEW.id,retvalue;
                RETURN NULL;
            END IF;
            retvalue = pf_tr_calc_bondport_validnum(NEW.id);
            IF retvalue <> 0 THEN 
                RAISE WARNING 'tr_update_port(%) call pf_tr_calc_bondport_validnum(%) failed. ret=%',NEW.id,NEW.id,retvalue;
                RETURN NULL;
            END IF;
            
            SELECT count(*) INTO isswitchMember FROM virtual_switch_map a, switch b WHERE a.virtual_switch_id = b.id  AND b.switch_type = 2 AND a.port_id = NEW.id; 
            SELECT b.id INTO switchid FROM virtual_switch_map a, switch b WHERE a.virtual_switch_id = b.id  AND b.switch_type = 2 AND a.port_id = NEW.id; 
            IF isswitchMember > 0 THEN 
                retvalue = pf_tr_calc_switch(switchid);
                IF retvalue <> 0 THEN 
                    RAISE WARNING 'tr_update_port(%) call pf_tr_calc_switch(%) failed. ret=%',NEW.id,switchid,retvalue;
                    RETURN NULL;
                END IF;
            END IF;
            
            RETURN NULL;
        END IF;
        
        IF NEW.port_type = 2 THEN 
            /* sriov vf port  */
            SELECT sriov_port_id INTO sriovid FROM sriov_vf_port WHERE port_id = NEW.id;
            retvalue = pf_tr_calc_sriovport(sriovid);    
            IF retvalue <> 0 THEN 
                RAISE WARNING 'tr_update_port(%) call pf_tr_calc_sriovport(%) failed. ret=%',NEW.id,sriovid,retvalue;
                RETURN NULL;
            END IF;            
            RETURN NULL;
        END IF;
        
        RETURN NULL;
        
    END;
$tr_update_port$ 
LANGUAGE plpgsql;

CREATE TRIGGER tr_update_port AFTER UPDATE  ON port
    FOR EACH ROW EXECUTE PROCEDURE tr_update_port();
        

    
    
/* sriov vf port 触发器 */



CREATE OR REPLACE FUNCTION tr_insert_sriov_vf_port () RETURNS trigger AS $tr_insert_sriov_vf_port$
DECLARE
  sriov_is_ok integer := 0;
  sriovid numeric(19,0) := 0;
  retvalue integer := 0;
    BEGIN
        /* 
           SRIOV VF PORT 变化，需更新sriov port 
           
        */
        sriovid = NEW.sriov_port_id ;    
        retvalue = pf_tr_calc_sriovport(sriovid);
        IF retvalue <> 0  THEN 
            RAISE WARNING 'tr_insert_sriov_vf_port call pf_tr_calc_sriovport(%) failed. ret=%',sriovid,retvalue;
        END IF;
        
        RETURN NULL;
    END;
$tr_insert_sriov_vf_port$ 
LANGUAGE plpgsql;

CREATE TRIGGER tr_insert_sriov_vf_port AFTER INSERT ON sriov_vf_port
    FOR EACH ROW EXECUTE PROCEDURE tr_insert_sriov_vf_port();

    

CREATE OR REPLACE FUNCTION tr_delete_sriov_vf_port () RETURNS trigger AS $tr_delete_sriov_vf_port$
DECLARE
  sriov_is_ok integer := 0;
  sriovid numeric(19,0) := 0;
  retvalue integer := 0;
    BEGIN
        /* 
           SRIOV VF PORT 变化，需更新sriov port 
           
        */
        sriovid = OLD.sriov_port_id ;        
        retvalue = pf_tr_calc_sriovport(sriovid);
        IF retvalue <> 0 THEN 
            RAISE WARNING 'tr_delete_sriov_vf_port call pf_tr_calc_sriovport(%) failed. ret=%',sriovid,retvalue;
        END IF;
        
        RETURN NULL;
    END;
$tr_delete_sriov_vf_port$ 
LANGUAGE plpgsql;

CREATE TRIGGER tr_delete_sriov_vf_port AFTER DELETE ON sriov_vf_port
    FOR EACH ROW EXECUTE PROCEDURE tr_delete_sriov_vf_port();
    
    
/*switch map 触发器 */    



CREATE OR REPLACE FUNCTION tr_insert_virtual_switch_map () RETURNS trigger AS $tr_insert_virtual_switch_map$
DECLARE
  isExist integer := 0;
  bondid numeric(19,0) := 0;
  swid numeric(19,0) := 0;
  retvalue integer := 0;
    BEGIN
        /* 
           switch_map 增加记录 
        */            
        SELECT count(*) INTO isExist FROM virtual_switch_map a, switch b WHERE a.virtual_switch_id = b.id  AND b.switch_type = 2 AND a.virtual_switch_id = NEW.virtual_switch_id; 
        IF isExist > 0 THEN 
            retvalue = pf_tr_calc_switch(NEW.virtual_switch_id);
            IF retvalue <> 0 THEN 
                RAISE WARNING 'tr_insert_virtual_switch_map call pf_tr_calc_switch(%) failed. ret=%',NEW.virtual_switch_id,retvalue;
                RETURN NULL;
            END IF;
        END IF;
        
        RETURN NULL;
    END;
$tr_insert_virtual_switch_map$ 
LANGUAGE plpgsql;

CREATE TRIGGER tr_insert_virtual_switch_map AFTER INSERT ON virtual_switch_map
    FOR EACH ROW EXECUTE PROCEDURE tr_insert_virtual_switch_map();    
    
    


CREATE OR REPLACE FUNCTION tr_update_virtual_switch_map () RETURNS trigger AS $tr_update_virtual_switch_map$
DECLARE
  isExist integer := 0;
  bondid numeric(19,0) := 0;
  swid numeric(19,0) := 0;
  retvalue integer := 0;
    BEGIN
        /* 
           switch_map update 
        */    
        SELECT count(*) INTO isExist FROM virtual_switch_map a, switch b WHERE a.virtual_switch_id = b.id  AND b.switch_type = 2 AND a.virtual_switch_id = NEW.virtual_switch_id; 
        IF isExist > 0 THEN 
            retvalue = pf_tr_calc_switch(NEW.virtual_switch_id);
            IF retvalue <> 0  THEN 
                RAISE WARNING 'tr_update_virtual_switch_map call pf_tr_calc_switch(%) failed. ret=%',NEW.virtual_switch_id,retvalue;
                RETURN NULL;
            END IF;
        END IF;
        
        RETURN NULL;
        
    END;
$tr_update_virtual_switch_map$ 
LANGUAGE plpgsql;

CREATE TRIGGER tr_update_virtual_switch_map AFTER UPDATE  ON virtual_switch_map
    FOR EACH ROW EXECUTE PROCEDURE tr_update_virtual_switch_map();
        
        

CREATE OR REPLACE FUNCTION tr_delete_virtual_switch_map () RETURNS trigger AS $tr_delete_virtual_switch_map$
DECLARE
  isExist integer := 0;
  sriov_is_ok integer := 0;
  swid numeric(19,0) := 0;
  retvalue integer := 0;
    BEGIN
        /* 
           bond_map 删除 
        */        
        
        SELECT count(*) INTO isExist FROM virtual_switch_map a, switch b WHERE a.virtual_switch_id = b.id  AND b.switch_type = 2 AND a.virtual_switch_id = OLD.virtual_switch_id; 
        IF isExist > 0 THEN 
            retvalue = pf_tr_calc_switch(OLD.virtual_switch_id);
            IF retvalue  <> 0 THEN 
                RAISE WARNING 'tr_delete_virtual_switch_map call pf_tr_calc_switch(%) failed. ret=%',OLD.virtual_switch_id,retvalue;
                RETURN NULL;
            END IF;
        END IF;
        
        RETURN NULL;
    END;
$tr_delete_virtual_switch_map$ 
LANGUAGE plpgsql;

CREATE TRIGGER tr_delete_virtual_switch_map AFTER DELETE ON virtual_switch_map
    FOR EACH ROW EXECUTE PROCEDURE tr_delete_virtual_switch_map();
            

/* vna触发器 */            


CREATE OR REPLACE FUNCTION tr_update_vna () RETURNS trigger AS $tr_update_vna$
DECLARE
  bondid numeric(19,0) := 0;
  swid numeric(19,0) := 0;
  mvswlists RECORD;
  retvalue integer := 0;
    BEGIN
        /* 
           vna update 
        */    
        
        /* 状态变化 才需更新 */
        IF NEW.is_online = OLD.is_online THEN 
            RETURN NULL;
        END IF;
                
        FOR mvswlists IN  SELECT  DISTINCT c.virtual_switch_id FROM vna a,port b,virtual_switch_map c,switch d
           WHERE a.id = b.vna_id AND b.id = c.port_id  AND c.virtual_switch_id = d.id AND d.switch_type = 2 LOOP 
           
           swid = mvswlists.virtual_switch_id;
           retvalue = pf_tr_calc_switch(swid);
           IF retvalue <> 0 THEN 
                RAISE WARNING 'tr_update_vna call pf_tr_calc_switch(%) failed. ret=%',swid,retvalue;
                RETURN NULL;
            END IF;           
        END LOOP;           
                
        RETURN NULL;
        
    END;
$tr_update_vna$ 
LANGUAGE plpgsql;

CREATE TRIGGER tr_update_vna AFTER UPDATE  ON vna
    FOR EACH ROW EXECUTE PROCEDURE tr_update_vna();
        
        
/* vsi sriov 触发器 */



    


CREATE OR REPLACE FUNCTION tr_insert_vsi_sriov () RETURNS trigger AS $tr_insert_vsi_sriov$
DECLARE
  isExist integer := 0;
  sriovportid numeric(19,0) := 0;
  swid numeric(19,0) := 0;
  retvalue integer := 0;
    BEGIN
        /* 
           insert vsi_sriov
        */    
                
        SELECT count(*) INTO isExist FROM sriov_vf_port WHERE port_id = NEW.sriov_vf_port_id;
        SELECT sriov_port_id INTO sriovportid FROM sriov_vf_port WHERE port_id = NEW.sriov_vf_port_id;
        IF isExist = 1 THEN 
            retvalue = pf_tr_calc_sriovport(sriovportid);    
            IF retvalue <> 0 THEN 
                RAISE WARNING 'tr_insert_vsi_sriov(%) call pf_tr_calc_sriovport(%) failed. ret=%',NEW.sriov_vf_port_id,sriovportid,retvalue;
                RETURN NULL;
            END IF;
        END IF;
                
        RETURN NULL;
    END;
$tr_insert_vsi_sriov$ 
LANGUAGE plpgsql;

CREATE TRIGGER tr_insert_vsi_sriov AFTER INSERT  ON vsi_sriov
    FOR EACH ROW EXECUTE PROCEDURE tr_insert_vsi_sriov();
        
        


CREATE OR REPLACE FUNCTION tr_delete_vsi_sriov () RETURNS trigger AS $tr_delete_vsi_sriov$
DECLARE
  isExist integer := 0;
  sriovportid numeric(19,0) := 0;
  swid numeric(19,0) := 0;
  retvalue integer := 0;
    BEGIN
        /* 
           delete vsi_sriov
        */    
        SELECT count(*) INTO isExist FROM sriov_vf_port WHERE port_id = OLD.sriov_vf_port_id;
        SELECT sriov_port_id INTO sriovportid FROM sriov_vf_port WHERE port_id = OLD.sriov_vf_port_id;
        IF isExist = 1 THEN 
            retvalue = pf_tr_calc_sriovport(sriovportid);    
            IF retvalue <> 0  THEN             
                RAISE WARNING 'tr_delete_vsi_sriov(%) call pf_tr_calc_sriovport(%) failed. ret=%',OLD.sriov_vf_port_id,sriovportid,retvalue;
                RETURN NULL;
            END IF;
        END IF;
                
        RETURN NULL;
        
    END;
$tr_delete_vsi_sriov$ 
LANGUAGE plpgsql;

CREATE TRIGGER tr_delete_vsi_sriov AFTER DELETE  ON vsi_sriov
    FOR EACH ROW EXECUTE PROCEDURE tr_delete_vsi_sriov();
                
        
        
            

        
        
        
            

            
            
    
    
            
            
            






  