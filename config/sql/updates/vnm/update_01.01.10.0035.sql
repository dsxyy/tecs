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

-- SDVS的空闲个数 也需要计算（通过触发器）
--
-- 
------------------------------------------------------------------------------------


/*
  不再简单地加减-处理，而是通过触发重新计数 
   硬件EDVS 计算方式见 update_01.01.10.0033.sql 
   软件SDVS 计算方式如下 
   
   由下面几种event触发
  
   
  4 vsi_profile
    add    ----->　   tr_calc_sdvs_switch()  
    del    ----->     tr_calc_sdvs_switch() 
    update ----->     tr_calc_sdvs_switch() 
    
*/


 
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
  VALUES ('01.01.10.0035', 'tecs-vnm_test',now(),'first version');
  


/* 计算 sdvs swtich, deploy_vsi_num*/
CREATE OR REPLACE FUNCTION pf_tr_calc_sdvs_switch(in_sw_id numeric(19,0))
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  totalNum integer := 0;
  deployNum integer := 0; 
  
  isOk integer  := 0; 
  isSdvs integer := 0;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1+100;
  RTN_HAS_SAME_ITEM integer := 2+100;
  RTN_ITEM_CONFLICT integer := 3+100;
  RTN_ITEM_NO_EXIST integer := 4+100;
BEGIN
  result := 0; 
    
  BEGIN      
     /* sdvs 才需要更新，edvs 将直接返回 
     */
     SELECT count(*) INTO isSdvs FROM virtual_switch WHERE switch_id = in_sw_id AND switch_type = 1; 
     SELECT nic_max_num_cfg INTO totalNum FROM virtual_switch WHERE switch_id = in_sw_id AND switch_type = 1; 
     IF isSdvs = 0 THEN 
         RETURN result;
     END IF;
          
     SELECT count(*) INTO deployNum 
     FROM 
        vsi_profile 
     WHERE 
        virtual_switch_id = in_sw_id ;
             
     IF ( totalNum - deployNum ) < 0 THEN 
         /* 有异常 */
         RAISE WARNING 'pf_tr_calc_sdvs_switch faild. sw:%,totalNum:%,deployNum:%',in_sw_id,totalNum,deployNum;
         UPDATE virtual_switch SET deploy_vsi_num = deployNum  WHERE switch_id = in_sw_id;
     ELSE 
         UPDATE virtual_switch SET deploy_vsi_num = deployNum  WHERE switch_id = in_sw_id;
     END IF;
     
  EXCEPTION 
    WHEN others
    THEN
    RAISE WARNING 'pf_tr_calc_sdvs_switch(%) failed.',in_sw_id;
    result := RTN_OPERATOR_FAILED;
  END;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  
  

/* 以上是触发器 调用函数 */  

/* vsi_profile 触发器 */



CREATE OR REPLACE FUNCTION tr_insert_vsi_profile () RETURNS trigger AS $tr_insert_vsi_profile$
DECLARE
  isExist integer := 0;
  bondid numeric(19,0) := 0;
  swid numeric(19,0) := 0;
  retvalue integer := 0;
    BEGIN
        /* 
           vsi_profile 增加记录 
        */            
        retvalue = pf_tr_calc_sdvs_switch(NEW.virtual_switch_id);
        IF retvalue <> 0 THEN 
            RAISE WARNING 'tr_insert_vsi_profile call pf_tr_calc_sdvs_switch(%) failed. ret=%',NEW.virtual_switch_id,retvalue;
            RETURN NULL;
        END IF;
                
        RETURN NULL;
    END;
$tr_insert_vsi_profile$ 
LANGUAGE plpgsql;

CREATE TRIGGER tr_insert_vsi_profile AFTER INSERT ON vsi_profile
    FOR EACH ROW EXECUTE PROCEDURE tr_insert_vsi_profile();    
    
    


CREATE OR REPLACE FUNCTION tr_update_vsi_profile () RETURNS trigger AS $tr_update_vsi_profile$
DECLARE
  bondid numeric(19,0) := 0;
  swid numeric(19,0) := 0;
  retvalue integer := 0;
    BEGIN
        /* 
           vsi_profile update记录 
        */            
        retvalue = pf_tr_calc_sdvs_switch(OLD.virtual_switch_id);
        IF retvalue <> 0 THEN 
            RAISE WARNING 'tr_insert_vsi_profile call pf_tr_calc_sdvs_switch(%) failed. ret=%',OLD.virtual_switch_id,retvalue;
            RETURN NULL;
        END IF;
        
        retvalue = pf_tr_calc_sdvs_switch(NEW.virtual_switch_id);
        IF retvalue <> 0 THEN 
            RAISE WARNING 'tr_insert_vsi_profile call pf_tr_calc_sdvs_switch(%) failed. ret=%',NEW.virtual_switch_id,retvalue;
            RETURN NULL;
        END IF;
        
        RETURN NULL;
        
    END;
$tr_update_vsi_profile$ 
LANGUAGE plpgsql;

CREATE TRIGGER tr_update_vsi_profile AFTER UPDATE  ON vsi_profile
    FOR EACH ROW EXECUTE PROCEDURE tr_update_vsi_profile();
        
        

CREATE OR REPLACE FUNCTION tr_delete_vsi_profile () RETURNS trigger AS $tr_delete_vsi_profile$
DECLARE
  sriov_is_ok integer := 0;
  bondid numeric(19,0) := 0;
  retvalue integer := 0;
    BEGIN
        /* 
           vsi_profile DELETE 记录 
        */            
        retvalue = pf_tr_calc_sdvs_switch(OLD.virtual_switch_id);
        IF retvalue <> 0 THEN 
             RAISE WARNING 'tr_insert_vsi_profile call pf_tr_calc_sdvs_switch(%) failed. ret=%',OLD.virtual_switch_id,retvalue;
            RETURN NULL;
        END IF;
        RETURN NULL;
    END;
$tr_delete_vsi_profile$ 
LANGUAGE plpgsql;

CREATE TRIGGER tr_delete_vsi_profile AFTER DELETE ON vsi_profile
    FOR EACH ROW EXECUTE PROCEDURE tr_delete_vsi_profile();
            
        
            