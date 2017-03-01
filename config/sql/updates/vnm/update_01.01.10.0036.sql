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

-- tr_delete_bond_map 触发器中必须使用OLD
--
-- 
------------------------------------------------------------------------------------



 
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
  VALUES ('01.01.10.0036', 'tecs-vnm_test',now(),'first version');
  


  
CREATE OR REPLACE FUNCTION tr_delete_bond_map () RETURNS trigger AS $tr_delete_bond_map$
DECLARE
  sriov_is_ok integer := 0;
  bondid numeric(19,0) := 0;
  retvalue integer := 0;
    BEGIN
        /* 
           bond_map DELETE 记录 
        */            
        bondid = OLD.bond_id;
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

