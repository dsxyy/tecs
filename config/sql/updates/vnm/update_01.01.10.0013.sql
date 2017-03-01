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

--vxlan sdn 功能，比较多 
--
------------------------------------------------------------------------------------
 
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
  VALUES ('01.01.10.0013', 'tecs-vnm_test',now(),'first version');

--sdn 
-- Sdn Ctrl Cfg SERIAL
CREATE SEQUENCE SERIAL_SDN_CTRL_CFG START 1;
CREATE SEQUENCE SERIAL_QUANTUM_RESTFUL_CFG START 1;
CREATE SEQUENCE SERIAL_QUANTUM_NETWORK START 1;
CREATE SEQUENCE SERIAL_QUANTUM_PORT START 1;


-- 创建 TYPE
drop function pf_net_query_vswitch(character varying);
DROP type t_vswtich_query;
create TYPE t_vswtich_query AS (
    ret integer, -- proc ret
    name character varying(64), -- port name 
    uuid character varying(64),  -- port uuid
    switch_type integer, --- switch type
    state integer, 
    max_mtu integer,
    evb_mode integer,
    pg_uuid character varying(64), 
    nic_max_num_cfg integer,        
    is_active integer,
	sdn_ctrl_cfg_uuid character varying(64)
);

create TYPE t_quantumnetwork_query AS (
    ret integer, -- proc ret
    uuid character varying(64),  
	logicnetwork_uuid character varying(64),  
	project_id numeric(19)
);

create TYPE t_quantumport_query AS (
    ret integer, -- proc ret
    qn_uuid character varying(64),  
	qp_uuid character varying(64)
);


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

-- table 

CREATE TABLE  quantum_network ( 
	id                   numeric( 19 ) NOT NULL,
	uuid                 varchar( 64 ) NOT NULL,
	logic_network_id     numeric(19 ) NOT NULL,
	CONSTRAINT pk_pg_sdn PRIMARY KEY ( id ),	
	CONSTRAINT fk_qn_ln_id FOREIGN KEY (logic_network_id)
      REFERENCES logic_network (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE RESTRICT	  
 );

CREATE INDEX idx_quantum_network ON  quantum_network ( logic_network_id );

CREATE TABLE  quantum_network_map ( 
	quantum_network_id   numeric( 19 ) NOT NULL,
	project_id           numeric( 19 ) NOT NULL,
	CONSTRAINT pk_pg_sdn_0 PRIMARY KEY ( quantum_network_id )
 );
 
CREATE TABLE  quantum_port ( 
	id                   numeric( 19 ) NOT NULL,
	quantum_network_id   numeric( 19 ) NOT NULL,
	uuid                 character varying(64) NOT NULL,
	CONSTRAINT pk_quantum_port PRIMARY KEY ( id ),
	CONSTRAINT fk_qp_qn_id FOREIGN KEY (quantum_network_id)
      REFERENCES quantum_network (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE CASCADE	
 );

CREATE INDEX idx_quantum_port ON  quantum_port ( quantum_network_id );
 

/* quantum */
CREATE TABLE  sdn_ctrl_cfg ( 
	id                   numeric( 19,0) NOT NULL,
	uuid                 varchar( 64 )  NOT NULL,
	protocol_type        varchar( 64 )  NOT NULL,
	port                 int4 NOT NULL,
	ip                   varchar( 64 ),
	CONSTRAINT pk_sdn_config PRIMARY KEY ( id )
 );
 
 COMMENT ON COLUMN sdn_ctrl_cfg.protocol_type IS '= udp = tcp';
 
CREATE TABLE  quantum_restfull_cfg ( 
	id                   numeric( 19,0) NOT NULL,
	uuid                 varchar( 64 )  NOT NULL,
	ip                   varchar( 64 ),
	CONSTRAINT pk_quantum_restfull_cfg PRIMARY KEY ( id )
 );
 
alter table virtual_switch add sdn_ctrl_cfg_id numeric(19,0);

alter table virtual_switch add CONSTRAINT fk_virtual_switch_sdn_ctrl_cfg FOREIGN KEY (sdn_ctrl_cfg_id)
      REFERENCES sdn_ctrl_cfg (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE RESTRICT;

alter table vsi_profile add column quantum_network_id numeric( 19 );
alter table vsi_profile add column quantum_port_id numeric( 19 );

ALTER TABLE  vsi_profile ADD CONSTRAINT fk_vsi_profile_quantum_port FOREIGN KEY ( quantum_port_id ) REFERENCES  quantum_port( id ) ON DELETE RESTRICT;
ALTER TABLE  vsi_profile ADD CONSTRAINT fk_vsi_profile_quantum_network FOREIGN KEY ( quantum_network_id ) REFERENCES  quantum_network( id ) ON DELETE CASCADE;
 
-- procedure 
                           
-- check add vsi 
CREATE OR REPLACE FUNCTION pf_net_check_add_quantum_network(in_logicnetwork_uuid character varying, 
                           in_quantumnetwork_uuid character varying, in_project_id numeric(19,0))
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
  lnid numeric(19,0);
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
   
  SELECT count(*) INTO is_exist FROM logic_network WHERE uuid = in_logicnetwork_uuid;
  SELECT id INTO lnid FROM logic_network WHERE uuid = in_logicnetwork_uuid;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
	  return result;
  END IF;
  
  SELECT count(*) INTO is_exist FROM quantum_network WHERE uuid = in_quantumnetwork_uuid;
  IF is_exist > 0 THEN 
      result := RTN_ITEM_CONFLICT;
	  return result;
  END IF;
  
  SELECT count(*) INTO is_exist FROM quantum_network a, quantum_network_map b 
   WHERE a.id = b.quantum_network_id AND b.project_id = in_project_id AND a.logic_network_id = lnid;
  IF is_exist > 0 THEN 
      result := RTN_ITEM_CONFLICT;
	  return result;
  END IF;
  
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 
                           

                           
CREATE OR REPLACE FUNCTION pf_net_add_quantum_network(in_logicnetwork_uuid character varying, 
                           in_quantumnetwork_uuid character varying, in_project_id numeric(19,0))
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
  lnid numeric(19,0);
  phyuuid character varying(64);
  bonduuid character varying(64);
  bondid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_add_quantum_network(in_logicnetwork_uuid,in_quantumnetwork_uuid, in_project_id);
  IF result.ret > 0 THEN      
     return result;
  END IF;
  
  -- logicnetwork uuid 
  SELECT count(*) INTO is_exist FROM logic_network WHERE uuid = in_logicnetwork_uuid;
  SELECT id INTO lnid FROM logic_network WHERE uuid = in_logicnetwork_uuid;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
	  return result;
  END IF;
  
  -- insert item 
  result.id := nextval('SERIAL_QUANTUM_NETWORK');
  result.uuid := in_quantumnetwork_uuid;
  BEGIN 
      -- INSERT quantum network 
	  INSERT INTO quantum_network(id,uuid,logic_network_id) values(result.id,in_quantumnetwork_uuid,lnid);
	  
	  IF in_project_id > 0 THEN 
	      INSERT INTO quantum_network_map(quantum_network_id,project_id) values(result.id, in_project_id);
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
 
                           
                           
-- 判断能否删除
CREATE OR REPLACE FUNCTION pf_net_check_del_quantum_network(in_uuid character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  vsiid numeric(19,0);
  
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
    
  -- check uuid 是否存在
  SELECT count(*) INTO is_exist FROM quantum_network WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST; 
      return result;
  END IF; 
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 删除 vsi 
CREATE OR REPLACE FUNCTION pf_net_del_quantum_network(in_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  vsiid numeric(19,0);
  sriovvfportid numeric(19,0);
  oldexistswitch integer := 0;
  oldvswitchid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_del_quantum_network(in_uuid);
  IF result.ret > 0 THEN      
     return result;
  END IF;
      
  -- delete item 
  --result.uuid := in_uuid;
  BEGIN          
      DELETE FROM quantum_network WHERE uuid =  in_uuid;   
      
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
 
 
 
CREATE OR REPLACE FUNCTION pf_net_query_quantumnetwork(in_uuid character varying)
  RETURNS t_quantumnetwork_query AS
$BODY$
DECLARE
    result t_quantumnetwork_query;
    is_exist integer; 
    qnid numeric(19,0);
	lnid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    result.ret := 0;
    
    -- logic_network uuid 
    SELECT count(*) INTO is_exist FROM quantum_network WHERE uuid = in_uuid;
    SELECT id INTO qnid FROM quantum_network WHERE uuid = in_uuid;
	SELECT logic_network_id INTO lnid FROM quantum_network WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
         result.ret := RTN_ITEM_NO_EXIST; 
         return result;
    END IF;  
    
    result.uuid  := in_uuid;
    SELECT uuid INTO result.logicnetwork_uuid FROM  logic_network WHERE id = lnid;
    
	SELECT count(*) INTO is_exist FROM quantum_network_map WHERE quantum_network_id = qnid;
	IF is_exist > 0 THEN
	    SELECT project_id INTO result.project_id FROM  quantum_network_map WHERE quantum_network_id = qnid;
	ELSE 
	    result.project_id := 0;
	END IF;
    
    return result;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


CREATE OR REPLACE FUNCTION pf_net_query_quantumnetwork_by_ln_projectid(in_uuid character varying,in_project_d numeric(19,0) )
  RETURNS t_quantumnetwork_query AS
$BODY$
DECLARE
    ref t_quantumnetwork_query;
    is_exist integer; 
	lnid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    ref.ret := 0;
	
    SELECT count(*) INTO is_exist FROM logic_network WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
         ref.ret := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF;  
	
	SELECT count(*) INTO is_exist FROM v_net_quantum_network_logicnetwork 
         WHERE logicnetwork_uuid = in_uuid AND project_id = in_project_d;
    IF is_exist <= 0 THEN 
         ref.ret := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF; 
  
    SELECT quantumnetwork_uuid INTO ref.uuid  FROM v_net_quantum_network_logicnetwork 
       WHERE logicnetwork_uuid = in_uuid AND project_id = in_project_d;
    
	SELECT logicnetwork_uuid INTO ref.logicnetwork_uuid  FROM v_net_quantum_network_logicnetwork 
       WHERE logicnetwork_uuid = in_uuid AND project_id = in_project_d;
	   
	SELECT project_id INTO ref.project_id  FROM v_net_quantum_network_logicnetwork 
       WHERE logicnetwork_uuid = in_uuid AND project_id = in_project_d;
	   
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;

-- 查询


CREATE OR REPLACE FUNCTION pf_net_query_quantumnetwork_all( )
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
	lnid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
  
    OPEN ref FOR SELECT logicnetwork_uuid, quantumnetwork_uuid, project_id 
      FROM v_net_quantum_network_logicnetwork;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;

  
CREATE OR REPLACE FUNCTION pf_net_query_quantumnetwork_by_ln( in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
	lnid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    
    SELECT count(*) INTO is_exist FROM logic_network WHERE uuid = in_uuid;
	SELECT id INTO lnid FROM logic_network WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF;  
  
    OPEN ref FOR SELECT logicnetwork_uuid, quantumnetwork_uuid, project_id 
      FROM v_net_quantum_network_logicnetwork WHERE logicnetwork_uuid = in_uuid;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  

 


                           
-- check add vsi  
                           
CREATE OR REPLACE FUNCTION pf_net_check_add_quantum_port(in_qn_uuid character varying, 
                           in_qp_uuid character varying)
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
  lnid numeric(19,0);
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
   
  SELECT count(*) INTO is_exist FROM quantum_network WHERE uuid = in_qn_uuid;
  SELECT id INTO lnid FROM quantum_network WHERE uuid = in_qn_uuid;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
	  return result;
  END IF;
  
  SELECT count(*) INTO is_exist FROM quantum_port WHERE uuid = in_qp_uuid;
  IF is_exist > 0 THEN 
      result := RTN_ITEM_CONFLICT;
	  return result;
  END IF;
  
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
                           
CREATE OR REPLACE FUNCTION pf_net_add_quantum_port(in_qn_uuid character varying, 
                           in_qp_uuid character varying)
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
  lnid numeric(19,0);
  phyuuid character varying(64);
  bonduuid character varying(64);
  qnid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_add_quantum_port(in_qn_uuid,in_qp_uuid);
  IF result.ret > 0 THEN      
     return result;
  END IF;
  
  -- qn uuid 
  SELECT count(*) INTO is_exist FROM quantum_network WHERE uuid = in_qn_uuid;
  SELECT id INTO qnid FROM quantum_network WHERE uuid = in_qn_uuid;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST;
	  return result;
  END IF;
  
  -- insert item 
  result.id := nextval('SERIAL_QUANTUM_PORT');
  result.uuid := in_qp_uuid;
  BEGIN 
      -- INSERT quantum network 
	  INSERT INTO quantum_port(id,quantum_network_id,uuid) values(result.id,qnid,in_qp_uuid);
	        
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
 
                           
                           
-- 判断能否删除
CREATE OR REPLACE FUNCTION pf_net_check_del_quantum_port(in_uuid character varying)
 RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  vsiid numeric(19,0);
  
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
    
  -- check uuid 是否存在
  SELECT count(*) INTO is_exist FROM quantum_port WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
      result := RTN_ITEM_NO_EXIST; 
      return result;
  END IF; 
  
  -- vsi 在用，也不能删除
  
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- 删除 vsi 
CREATE OR REPLACE FUNCTION pf_net_del_quantum_port(in_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  vsiid numeric(19,0);
  sriovvfportid numeric(19,0);
  oldexistswitch integer := 0;
  oldvswitchid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_del_quantum_port(in_uuid);
  IF result.ret > 0 THEN      
     return result;
  END IF;
      
  -- delete item 
  --result.uuid := in_uuid;
  BEGIN          
      DELETE FROM quantum_port WHERE uuid =  in_uuid;   
      
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
 
 
 
CREATE OR REPLACE FUNCTION pf_net_query_quantumport(in_uuid character varying)
  RETURNS t_quantumport_query AS
$BODY$
DECLARE
    result t_quantumport_query;
    is_exist integer; 
     qpid numeric(19,0);
	qnid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    result.ret := 0;
    
    -- logic_network uuid 
    SELECT count(*) INTO is_exist FROM quantum_port WHERE uuid = in_uuid;
	SELECT quantum_network_id INTO qnid FROM quantum_port WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
         result.ret := RTN_ITEM_NO_EXIST; 
         return result;
    END IF;  
    
    result.qp_uuid  := in_uuid;
    SELECT uuid INTO result.qn_uuid FROM  quantum_network WHERE id = qnid;
        
    return result;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;




CREATE OR REPLACE FUNCTION pf_net_query_quantumport_all( )
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
	lnid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
  
    OPEN ref FOR SELECT a.uuid, b.uuid
      FROM quantum_network a,quantum_port b
	  WHERE a.id = b.quantum_network_id;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;

  
CREATE OR REPLACE FUNCTION pf_net_query_quantumnetwork_by_ln( in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
	lnid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    
    SELECT count(*) INTO is_exist FROM logic_network WHERE uuid = in_uuid;
	SELECT id INTO lnid FROM logic_network WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF;  
  
    OPEN ref FOR SELECT logicnetwork_uuid, quantumnetwork_uuid, project_id 
      FROM v_net_quantum_network_logicnetwork WHERE logicnetwork_uuid = in_uuid;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  


CREATE OR REPLACE FUNCTION pf_net_add_quantum_restfull_cfg(in_uuid character varying, in_ip character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_conflict integer;

  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;

  /* 仅有一条记录 */
  SELECT count(*) INTO is_conflict FROM quantum_restfull_cfg ;
  IF is_conflict > 0 THEN
     result.ret := RTN_ITEM_CONFLICT; -- conflict 
     return result;
  END IF;

  -- insert item
  result.id := nextval('SERIAL_QUANTUM_RESTFUL_CFG');
  result.uuid := in_uuid;
  BEGIN 
    INSERT INTO quantum_restfull_cfg(id, uuid,ip) values(result.id,in_uuid,in_ip);
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


CREATE OR REPLACE FUNCTION pf_net_modify_quantum_restfull_cfg(in_uuid character varying, in_ip character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  sdnid  numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG  integer := 5;
BEGIN
  result.ret := 0;

  /*  */
  SELECT count(*) INTO is_exist FROM quantum_restfull_cfg WHERE uuid = in_uuid ;
  SELECT id INTO sdnid FROM quantum_restfull_cfg WHERE uuid = in_uuid ;
  IF is_exist <= 0 THEN
     result.ret := RTN_ITEM_NO_EXIST; /* don't exist item */
     return result;     
  END IF;

  -- modify item
  BEGIN 
    SELECT id INTO result.id FROM quantum_restfull_cfg WHERE uuid = in_uuid;
	result.uuid := in_uuid;
  
    UPDATE quantum_restfull_cfg SET ip = in_ip WHERE uuid = in_uuid ;
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

  
  
CREATE OR REPLACE FUNCTION pf_net_del_quantum_restfull_cfg(in_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  max_pg_mtu integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
   
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM quantum_restfull_cfg WHERE uuid = in_uuid ;
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
  
  
CREATE OR REPLACE FUNCTION pf_net_query_quatnum_restfull_cfg_all()
  RETURNS refcursor AS
$BODY$
DECLARE
        ref refcursor;
BEGIN
       OPEN ref FOR SELECT uuid,ip FROM quantum_restfull_cfg;
       RETURN ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  
  

CREATE OR REPLACE FUNCTION pf_net_add_sdn_ctrl_cfg(in_uuid character varying, in_protocol_type character varying, in_port integer,in_ip character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_conflict integer;

  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;

  /* 仅有一条记录 */
  SELECT count(*) INTO is_conflict FROM sdn_ctrl_cfg ;
  IF is_conflict > 0 THEN
     result.ret := RTN_ITEM_CONFLICT; -- conflict 
     return result;
  END IF;

  /* insert item */
  result.id := nextval('SERIAL_SDN_CTRL_CFG');
  result.uuid := in_uuid;
  BEGIN 
    INSERT INTO sdn_ctrl_cfg(id, uuid,protocol_type,port,ip) values(result.id,in_uuid,in_protocol_type,in_port,in_ip);
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


CREATE OR REPLACE FUNCTION pf_net_modify_sdn_ctrl_cfg(in_uuid character varying, in_protocol_type character varying, in_port integer,in_ip character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  sdnid  numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG  integer := 5;
BEGIN
  result.ret := 0;

  /*  */
  SELECT count(*) INTO is_exist FROM sdn_ctrl_cfg WHERE uuid = in_uuid ;
  SELECT id INTO sdnid FROM sdn_ctrl_cfg WHERE uuid = in_uuid ;
  IF is_exist <= 0 THEN
     result.ret := RTN_ITEM_NO_EXIST; /* don't exist item */
     return result;     
  END IF;

  -- modify item
  BEGIN 
    SELECT id INTO result.id FROM sdn_ctrl_cfg WHERE uuid = in_uuid;
	result.uuid := in_uuid;
  
    UPDATE sdn_ctrl_cfg SET protocol_type = in_protocol_type, port = in_port, ip = in_ip WHERE uuid = in_uuid ;
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

  
  
CREATE OR REPLACE FUNCTION pf_net_del_sdn_ctrl_cfg(in_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  max_pg_mtu integer;
  sdnid  numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
   
  SELECT count(*) INTO is_exist FROM sdn_ctrl_cfg WHERE uuid = in_uuid ;
  SELECT id INTO sdnid FROM sdn_ctrl_cfg WHERE uuid = in_uuid ;
  IF is_exist <= 0 THEN
     result.ret := RTN_ITEM_NO_EXIST; /* don't exist item */
     return result;     
  END IF;
  
  /*check switch 是否在使用*/
  SELECT count(*) INTO is_exist FROM virtual_switch WHERE sdn_ctrl_cfg_id = sdnid;
  IF is_exist > 0 THEN
     result.ret := RTN_RESOURCE_USING; /* don't exist item */
     return result;     
  END IF;
  
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM sdn_ctrl_cfg WHERE uuid = in_uuid ;
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
  
  
CREATE OR REPLACE FUNCTION pf_net_query_sdn_ctrl_cfg_all()
  RETURNS refcursor AS
$BODY$
DECLARE
        ref refcursor;
BEGIN
       OPEN ref FOR SELECT uuid,protocol_type,port,ip FROM sdn_ctrl_cfg;
       RETURN ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  
  


CREATE OR REPLACE FUNCTION pf_net_query_vswitch(in_uuid character varying)
  RETURNS t_vswtich_query AS
$BODY$
DECLARE
  result  t_vswtich_query;
  is_exist integer;
  switchid numeric(19,0);
  pgid numeric(19,0);
  RTN_ITEM_NO_EXIST integer :=4;
BEGIN
  result.ret := 0;
  
  SELECT count(*) INTO is_exist FROM switch a, virtual_switch b  WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
           AND a.uuid = in_uuid ;
  SELECT a.id INTO switchid  FROM switch a, virtual_switch b  WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
           AND a.uuid = in_uuid ;
  SELECT b.uplink_pg_id INTO pgid  FROM switch a, virtual_switch b  WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
           AND a.uuid = in_uuid ;           
  IF is_exist <= 0 THEN   
     result.ret := RTN_ITEM_NO_EXIST; -- no exist item
     return result; 
  END IF;
  
   result.uuid := in_uuid;
   -- swtich 
   SELECT switch_type into result.switch_type  FROM switch WHERE uuid = in_uuid;
   SELECT name into result.name  FROM switch WHERE uuid = in_uuid;
   SELECT state into result.state FROM switch WHERE uuid = in_uuid;
   SELECT max_mtu into result.max_mtu  FROM switch WHERE uuid = in_uuid;
   
   --uplink pg 
   SELECT a.uuid INTO result.pg_uuid FROM port_group a, uplink_pg b, virtual_switch c WHERE a.id = b.pg_id AND b.pg_id = c.uplink_pg_id 
          AND c.switch_id = switchid;
   
   -- virtual switch 
   SELECT evb_mode into result.evb_mode  FROM virtual_switch WHERE switch_id = switchid;
   SELECT nic_max_num_cfg into result.nic_max_num_cfg  FROM virtual_switch WHERE switch_id = switchid;
   SELECT is_active into result.is_active  FROM virtual_switch WHERE switch_id = switchid;
   
   result.sdn_ctrl_cfg_uuid = '';
   SELECT count(*) INTO is_exist FROM virtual_switch a, sdn_ctrl_cfg b WHERE a.sdn_ctrl_cfg_id = b.id AND a.switch_id = switchid AND b.id is not null;
   IF is_exist > 0 THEN 
       SELECT b.uuid INTO result.sdn_ctrl_cfg_uuid FROM virtual_switch a, sdn_ctrl_cfg b WHERE a.sdn_ctrl_cfg_id = b.id AND a.switch_id = switchid AND b.id is not null;
   ELSE
       result.sdn_ctrl_cfg_uuid = '';
   END IF;
      
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;




CREATE OR REPLACE FUNCTION pf_net_can_add_vswitch(in_switch_type integer, in_name character varying, in_uuid character varying, in_state integer, in_max_mtu integer, in_evb_mode integer, 
in_pg_uuid character varying, in_max_nic_num integer, in_is_active integer, in_sdn_cfg_uuid character varying)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  switchid numeric(19,0);
  is_exist integer; 
  pgid numeric(19,0);
  pg_mtu integer;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_SDN_CFG_INVALID integer := 21;
BEGIN
  result := 0;
  -- 入参检查 后面做
  
  -- 检查pg uuid 是否有效
  SELECT count(*) INTO is_exist FROM port_group a, uplink_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type AND a.uuid = in_pg_uuid;
  SELECT a.id INTO pgid FROM port_group a, uplink_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type AND a.uuid = in_pg_uuid;  
  SELECT a.mtu INTO pg_mtu FROM port_group a, uplink_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type AND a.uuid = in_pg_uuid;  
  IF is_exist <= 0 THEN 
    result := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  

  -- switch type
  IF in_switch_type < 1 THEN 
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;
  
  IF in_switch_type > 2 THEN 
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;
  
  -- 检查switch name 是否冲突 
  SELECT count(*) INTO is_exist FROM switch WHERE name = in_name;
  IF is_exist > 0 THEN 
    result := RTN_ITEM_CONFLICT;   
    return result;
  END IF;
  
  -- check max_mtu 
  /* switch mtu 将废弃 
  IF in_max_mtu > pg_mtu THEN 
     result := RTN_SWITCH_MTU_LESS_PG;   
     return result;
  END IF;
  */
  
  /*check sdn cfg */
  IF in_sdn_cfg_uuid <> '' THEN 
      SELECT count(*) INTO is_exist FROM sdn_ctrl_cfg WHERE uuid = in_sdn_cfg_uuid;
	  IF is_exist <= 0 THEN 
	      result := RTN_SDN_CFG_INVALID;
		  return result;
	  END IF;
  END IF;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  
  
CREATE OR REPLACE FUNCTION pf_net_add_vswitch(in_switch_type integer, in_name character varying, in_uuid character varying, in_state integer, in_max_mtu integer, in_evb_mode integer,
 in_pg_uuid character varying, in_max_nic_num integer, in_is_active integer, in_sdn_cfg_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_conflict integer;
  switchid numeric(19,0);
  is_exist integer; 
  pgid numeric(19,0);
  sdnid numeric(19,0); 
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SDN_CFG_INVALID integer := 21;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- 检查能否add
  result.ret := pf_net_can_add_vswitch(in_switch_type,in_name,in_uuid,in_state,in_max_mtu,in_evb_mode,in_pg_uuid,in_max_nic_num,in_is_active,in_sdn_cfg_uuid);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  SELECT count(*) INTO is_exist FROM port_group a, uplink_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type AND a.uuid = in_pg_uuid;
  SELECT a.id INTO pgid FROM port_group a, uplink_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type AND a.uuid = in_pg_uuid;  
  IF is_exist <= 0 THEN 
    result := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  /*判断是否配置了 sdn cfg, 在check中已经判断了*/
  IF in_sdn_cfg_uuid <> '' THEN 
      SELECT id INTO sdnid FROM sdn_ctrl_cfg WHERE uuid = in_sdn_cfg_uuid;
  END IF;
    
  -- insert item
  result.id := nextval('serial_switch');
  result.uuid := in_uuid;
  
  BEGIN 
    INSERT INTO switch(id,switch_type,name,uuid,state,max_mtu) values(result.id,in_switch_type,in_name,in_uuid,in_state,in_max_mtu);

    -- edvs nic_max_num_cfg 添加的时候 必须为0
    IF in_switch_type = 2 THEN 
	    IF in_sdn_cfg_uuid = '' THEN 
            INSERT INTO virtual_switch(switch_id,switch_type,evb_mode,uplink_pg_id,nic_max_num_cfg,is_active )
               values(result.id,in_switch_type,in_evb_mode,pgid,0,in_is_active); 
	    ELSE
		    INSERT INTO virtual_switch(switch_id,switch_type,evb_mode,uplink_pg_id,nic_max_num_cfg,is_active,sdn_ctrl_cfg_id )
               values(result.id,in_switch_type,in_evb_mode,pgid,0,in_is_active,sdnid); 
		END IF;
    ELSE 
	    IF in_sdn_cfg_uuid = '' THEN 
            INSERT INTO virtual_switch(switch_id,switch_type,evb_mode,uplink_pg_id,nic_max_num_cfg,is_active)
                values(result.id,in_switch_type,in_evb_mode,pgid,in_max_nic_num,in_is_active);   
        ELSE
		    INSERT INTO virtual_switch(switch_id,switch_type,evb_mode,uplink_pg_id,nic_max_num_cfg,is_active,sdn_ctrl_cfg_id )
                values(result.id,in_switch_type,in_evb_mode,pgid,in_max_nic_num,in_is_active,sdnid);  
        END IF;		
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

  
  
  
CREATE OR REPLACE FUNCTION pf_net_can_modify_vswitch(in_switch_type integer, in_name character varying, in_uuid character varying, in_state integer, in_max_mtu integer, in_evb_mode integer, 
in_pg_uuid character varying, in_max_nic_num integer, in_is_active integer, in_sdn_cfg_uuid character varying)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  switchid numeric(19,0);
  switchtype integer;
  is_exist integer; 
  uplinkportid numeric(19,0);
  oldpgid numeric(19,0);
  pgid numeric(19,0);
  pg_mtu integer;
  maxkernelmtu integer;
  muplinkportlists RECORD;
  /*错误返回码定义*/
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_UPLINKPGMTU_LESS_KERNELPG integer := 18;
  RTN_SDN_CFG_INVALID integer := 21;
BEGIN
  result := 0;
  -- 入参检查 后面做


  -- 检查pg uuid 是否有效
  SELECT count(*) INTO is_exist FROM port_group a, uplink_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type AND a.uuid = in_pg_uuid;
  SELECT a.id INTO pgid FROM port_group a, uplink_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type AND a.uuid = in_pg_uuid;  
  SELECT a.mtu INTO pg_mtu FROM port_group a, uplink_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type AND a.uuid = in_pg_uuid;  
  IF is_exist <= 0 THEN 
    result := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  -- switch type
  IF in_switch_type < 1 THEN 
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;
  
  IF in_switch_type > 2 THEN 
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;
  
  -- swtich uuid 
  SELECT count(*) INTO is_exist FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_uuid;
  SELECT a.switch_type INTO switchtype FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_uuid;    
  SELECT b.uplink_pg_id INTO oldpgid FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_uuid;    
  SELECT a.id INTO switchid FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_uuid;            
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
  
  -- switch type 不能修改 
  IF switchtype = in_switch_type THEN 
      result := 0;
  ELSE
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  
  -- 检查switch name 是否冲突 
  SELECT count(*) INTO is_exist FROM switch WHERE uuid = in_uuid AND name = in_name;
  IF is_exist <= 0 THEN 
     SELECT count(*) INTO is_exist FROM switch WHERE name = in_name;
     IF is_exist > 0 THEN 
        result := RTN_ITEM_CONFLICT;   
        return result;
     END IF;
  END IF;
  
  /* check max_mtu 
  IF in_max_mtu > pg_mtu THEN 
     result := RTN_SWITCH_MTU_LESS_PG;   
     return result;
  END IF;
  */

  /*check sdn cfg */
  IF in_sdn_cfg_uuid <> '' THEN 
      SELECT count(*) INTO is_exist FROM sdn_ctrl_cfg WHERE uuid = in_sdn_cfg_uuid;
	  IF is_exist <= 0 THEN 
	      result := RTN_SDN_CFG_INVALID;
		  return result;
	  END IF;
  END IF;
  
  --如果swtich存在虚拟nic的情况下，不能修改pg_id 
  SELECT count(*) INTO is_exist FROM virtual_switch a, vsi_profile b WHERE a.switch_id = b.virtual_switch_id AND  a.switch_id = switchid;

  IF is_exist > 0 THEN 
       IF oldpgid = pgid THEN 
           result :=0;
       ELSE
           result := RTN_ITEM_CONFLICT;   
           return result;
       END IF;
  END IF;


  --如果swtich的uplink 在SDVS EDVS混用的情况下，不能修改pg_id 
  SELECT count(*) INTO is_exist FROM virtual_switch_map WHERE virtual_switch_id = switchid;
  IF is_exist > 0 THEN 
 
      FOR muplinkportlists IN SELECT port_id , virtual_switch_id FROM virtual_switch_map 
                 WHERE virtual_switch_id = switchid ORDER BY port_id  LOOP
         SELECT count(*) INTO is_exist FROM virtual_switch_map WHERE port_id  = muplinkportlists.port_id;
         -- is_exist > 1 说明存在混用情况 
        
         IF is_exist > 1 THEN 
             IF oldpgid = pgid THEN 
                 result :=0;
             ELSE
                 result := RTN_ITEM_CONFLICT;   
                 return result;
             END IF;
         END IF;
        
      END LOOP;       
  END IF;
  
  /*
  如果switch存在kernel port的情况下，uplink pg mtu 必须大于 kernel pg mtu 
  */
  SELECT MAX(b.mtu) INTO maxkernelmtu 
  FROM kernel_port a, port_group b 
  WHERE a.virtual_switch_id = switchid AND a.kernel_pg_id = b.id ;
  
  SELECT count(*) INTO is_exist 
  FROM kernel_port a, port_group b 
  WHERE a.virtual_switch_id = switchid AND a.kernel_pg_id = b.id ;
  
  IF is_exist > 0 THEN 
      IF maxkernelmtu > pg_mtu THEN        
          result := RTN_UPLINKPGMTU_LESS_KERNELPG;   
          return result;
      END IF;
  END IF;
  
  /*看看是否需要判断virtual_switch_map表*/
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  
  
  
CREATE OR REPLACE FUNCTION pf_net_modify_vswitch(in_switch_type integer, in_name character varying, in_uuid character varying, in_state integer, in_max_mtu integer, 
in_evb_mode integer, in_pg_uuid character varying, in_max_nic_num integer, in_is_active integer, in_sdn_cfg_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  pgid numeric(19,0);
  switchid numeric(19,0);
  phyportid numeric(19,0);
  sdnid numeric(19,0);
  is_sriov_exist integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG  integer := 5;
  RTN_SDN_CFG_INVALID integer := 21;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  
  -- 检查能否add
  result.ret := pf_net_can_modify_vswitch(in_switch_type,in_name,in_uuid,in_state,in_max_mtu,in_evb_mode,in_pg_uuid,in_max_nic_num,in_is_active,in_sdn_cfg_uuid);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  SELECT count(*) INTO is_exist FROM port_group a, uplink_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type AND a.uuid = in_pg_uuid;
  SELECT a.id INTO pgid FROM port_group a, uplink_pg b WHERE a.id = b.pg_id AND a.pg_type = b.pg_type AND a.uuid = in_pg_uuid;  
  IF is_exist <= 0 THEN 
    result := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  SELECT count(*) INTO is_exist FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_uuid;
  SELECT a.id INTO switchid FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
        AND a.uuid = in_uuid;                
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
  
  /*判断是否配置了 sdn cfg, 在check中已经判断了*/
  IF in_sdn_cfg_uuid <> '' THEN 
      SELECT id INTO sdnid FROM sdn_ctrl_cfg WHERE uuid = in_sdn_cfg_uuid;
  END IF;
                   
  -- modify item
  result.id  := switchid;
  result.uuid := in_uuid;
  BEGIN 
    
    UPDATE switch SET name = in_name, state = in_state, max_mtu =in_max_mtu
       WHERE uuid = in_uuid;
    
    -- EDVS 的 nic_max_num_cfg 由db tr 来控制
    IF in_switch_type = 2 THEN 
        UPDATE virtual_switch SET evb_mode = in_evb_mode,uplink_pg_id = pgid, is_active = in_is_active 
           WHERE switch_id = switchid;            
    ELSE 
        UPDATE virtual_switch SET evb_mode = in_evb_mode,uplink_pg_id = pgid, nic_max_num_cfg = in_max_nic_num, is_active = in_is_active 
           WHERE switch_id = switchid;
    END IF;
	
	IF in_sdn_cfg_uuid = '' THEN 
	    UPDATE virtual_switch SET sdn_ctrl_cfg_id = null
           WHERE switch_id = switchid; 
    ELSE
        UPDATE virtual_switch SET sdn_ctrl_cfg_id = sdnid 
           WHERE switch_id = switchid;  	
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





-- check add vsi 
CREATE OR REPLACE FUNCTION pf_net_check_add_vsi(in_vm_id numeric(19,0),in_nic_index integer,
                           in_is_sriov integer, in_is_loop integer, in_logicnetwork_uuid character varying, 
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
                      in_vsi_id_value,in_vsi_id_format,in_vm_pg_uuid,in_mac,in_is_has_ip,in_ip,
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
  SELECT count(*) INTO is_exist FROM  mac_pool WHERE mac = in_mac AND netplane_id = netplaneid;
  SELECT id INTO macid FROM  mac_pool WHERE mac = in_mac AND netplane_id = netplaneid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
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
     INSERT INTO vnic(id, vm_id, nic_index, is_sriov ,is_loop, logic_network_uuid, port_type, 
             vsi_profile_id )
      VALUES(vnicid,in_vm_id,in_nic_index,in_is_sriov,in_is_loop,in_logicnetwork_uuid,in_port_type, 
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
                      in_vsi_id_value,in_vsi_id_format,in_vm_pg_uuid,in_mac,in_is_has_ip,in_ip,
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
  SELECT count(*) INTO is_exist FROM vsi_profile a, mac_pool b WHERE a.mac_id = b.id AND b.mac = in_mac AND a.id = vsiid;
  IF is_exist <= 0 THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  
  -- ip 不能改变
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


  
-- vxlan 
 CREATE SEQUENCE serial_netplane_ip_pool START 1;
 CREATE SEQUENCE serial_segment_map START 1;
 CREATE SEQUENCE serial_segment_pool START 1;
 CREATE SEQUENCE serial_segment_range START 1;
 
create TYPE t_vtepport_query AS (
    ret integer, -- proc ret
    vna_uuid  character varying(64),  -- vna uuid
    port_type integer, --- port type
    name character varying(64), -- port name 
    uuid character varying(64),  -- port uuid
    state integer, 
    is_broadcast integer,
    is_running integer,
    is_multicast integer,
    promiscuous integer,
    mtu integer,
    is_master integer,
    admin_state integer,
    is_online integer,    
    is_port_cfg integer, 
    is_consistency integer, 
        
    vswitch_uuid  character varying(64),
	ip  character varying(64),
	mask  character varying(64),
	ip_num  numeric(19,0),
	mask_num  numeric(19,0)
);

drop  function pf_net_query_netplane(character varying);

-- 创建 TYPE t_netplane_query
DROP TYPE t_netplane_query;
create TYPE t_netplane_query AS (
    ret integer, -- proc ret
    uuid character varying(64),  -- netplane uuid
    netplane_name character varying(32),  -- netplane name    
    description character varying(512),
    mtu integer,
	key integer
);



drop function pf_net_query_portgroup(character varying);
-- 创建 TYPE t_portgroup_query
DROP type t_portgroup_query;
create TYPE t_portgroup_query AS (
    ret integer, -- proc ret
    name  character varying(64),
    pg_type integer,
    uuid  character varying(64),
    acl character varying(64),
    qos character varying(64),
    allowed_priorities character varying(32),
    receive_bandwidth_limit character varying(32),
    receive_bandwidth_reserve character varying(32),
    default_vlan_id integer,
    promiscuous integer,
    macvlan_filter_enable integer,
    allowed_transmit_macs character varying(64),
    allowed_transmit_macvlans character varying(64),
    policy_block_override character varying(64),
    policy_vlan_override character varying(64),
    version integer,
    mgr_id character varying(64),
    type_id character varying(64),
    allow_mac_change integer,
    switchport_mode integer,    
    is_cfg_netplane integer,
    netplane_uuid character varying(64),
    mtu integer,    
    trunk_native_vlan_num integer,  --- if switchport_mode =0, valid.
    access_vlan_num integer,        --- if switchport_mode =1 & pg_type != 21, valid.
    access_isolate_no integer,      --- if switchport_mode =1 & pg_type = 21, valid.    
	description character varying(512),
	isolate_type integer,
	segment_num integer,
	vxlan_isolate_no integer,
	is_sdn integer
);

-- table 
CREATE TABLE ip_pool
(
  id numeric(19,0) NOT NULL,
  netplane_id numeric(19,0) NOT NULL,
  ip character varying(64) NOT NULL,
  mask character varying(64) NOT NULL,
  ip_num numeric(19,0) NOT NULL,
  mask_num numeric(19,0) NOT NULL,
  CONSTRAINT pk_index PRIMARY KEY (id),
  CONSTRAINT fk_npid_id FOREIGN KEY (netplane_id)
      REFERENCES netplane (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE RESTRICT
)
WITH (
  OIDS=FALSE
);

/*
CREATE TABLE ip_range
(
  id numeric(19,0) NOT NULL,
  netplane_id numeric(19,0) NOT NULL,
  ip_begin character varying(64) NOT NULL,
  ip_end character varying(64) NOT NULL,
  mask character varying(64) NOT NULL,
  ip_begin_num numeric(19,0) NOT NULL,
  ip_end_num numeric(19,0) NOT NULL,
  mask_num numeric(19,0) NOT NULL,
  CONSTRAINT pk_vlan_range_1 PRIMARY KEY (id),
  CONSTRAINT fk_ip_range_netplane FOREIGN KEY (netplane_id)
      REFERENCES netplane (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE RESTRICT
)
WITH (
  OIDS=FALSE
);


-- Index: idx_mac_range_1

-- DROP INDEX idx_mac_range_1;

CREATE INDEX idx_mac_range_1
  ON ip_range
  USING btree
  (netplane_id);
*/
  
CREATE TABLE segment_pool
(
  id numeric(19,0) NOT NULL,
  netplane_id numeric(19,0) NOT NULL,
  segment_num integer NOT NULL,
  CONSTRAINT pk_segment_pool PRIMARY KEY (id),
  CONSTRAINT fk_npid_np_id FOREIGN KEY (netplane_id)
      REFERENCES netplane (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE CASCADE
)
WITH (
  OIDS=FALSE
);



CREATE TABLE segment_map
(
  id numeric(19,0) NOT NULL,
  project_id numeric(19,0) NOT NULL,
  vm_pg_id numeric(19,0) NOT NULL,
  segment_id numeric(19,0) NOT NULL DEFAULT 0,
  isolate_no integer NOT NULL DEFAULT 0,
  CONSTRAINT pk_id PRIMARY KEY (id),
  CONSTRAINT fk_segment_id_segment_pool FOREIGN KEY (segment_id)
      REFERENCES segment_pool (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE RESTRICT,
  CONSTRAINT fk_segment_map_vm_pg FOREIGN KEY (vm_pg_id)
      REFERENCES vm_pg (pg_id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE RESTRICT,
  CONSTRAINT idx_segment_map UNIQUE (vm_pg_id, segment_id, isolate_no)
)
WITH (
  OIDS=FALSE
);


CREATE INDEX idx_segment_map_0
  ON segment_map
  USING btree
  (vm_pg_id);

CREATE TABLE segment_range
(
  id numeric(19,0) NOT NULL,
  netplane_id numeric(19,0),
  segment_begin integer NOT NULL,
  segment_end integer NOT NULL,
  CONSTRAINT pk_segment_range PRIMARY KEY (id),
  CONSTRAINT fk_npid_np_id FOREIGN KEY (netplane_id)
      REFERENCES netplane (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE CASCADE
)
WITH (
  OIDS=FALSE
);  
 


alter table uplink_pg add column isolate_type integer NOT NULL DEFAULT 0; -- =0 vlan =1 vxlan = 2 vlan vxlan
COMMENT ON COLUMN uplink_pg.isolate_type IS '=0 vlan =1 vxlan =2 vlan vxlan';

alter table uplink_pg add column is_sdn integer NOT NULL DEFAULT 0; -- =0 no =1 sdn
COMMENT ON COLUMN uplink_pg.is_sdn IS '=0 no =1 sdn';

alter table vm_pg add column isolate_type integer NOT NULL DEFAULT 0; -- =0 vlan =1 vxlan
COMMENT ON COLUMN vm_pg.isolate_type IS '=0 vlan =1 vxlan';

alter table vm_pg add column is_sdn integer NOT NULL DEFAULT 0; -- =0 no =1 sdn
COMMENT ON COLUMN vm_pg.is_sdn IS '=0 no =1 sdn';

CREATE TABLE vm_pg_private_vxlan_segment
(
  vmpg_id numeric(19,0) NOT NULL,
  isolate_no integer DEFAULT 0,
  CONSTRAINT vmpg_private_vxlan_key_id PRIMARY KEY (vmpg_id),
  CONSTRAINT fk_vm_pg_private_vxlan_vm_pg FOREIGN KEY (vmpg_id)
      REFERENCES vm_pg (pg_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE CASCADE
)
WITH (
  OIDS=FALSE
);


CREATE TABLE vm_pg_share_vxlan_segment
(
  vmpg_id numeric(19,0) NOT NULL,
  segment_num integer DEFAULT 0,
  CONSTRAINT pk_vmpg_id PRIMARY KEY (vmpg_id),
  CONSTRAINT fk_vm_pg_vxlan_vm_pg FOREIGN KEY (vmpg_id)
      REFERENCES vm_pg (pg_id) MATCH FULL
      ON UPDATE NO ACTION ON DELETE CASCADE
)
WITH (
  OIDS=FALSE
);



ALTER TABLE  port DROP CONSTRAINT ck_31;
ALTER TABLE  port ADD CONSTRAINT ck_31 CHECK ( port_type = 0 OR port_type = 1 OR port_type = 2 OR port_type = 3 OR port_type = 4 OR port_type = 5  OR port_type = 6 );

CREATE TABLE  vtep_port ( 
	port_id              numeric( 19 ) NOT NULL,
	port_type            int4 DEFAULT 6 NOT NULL,
	vna_id               numeric( 19 ) NOT NULL,
	virtual_switch_id    numeric( 19 ),
	ip_pool_id           numeric( 19 ) NOT NULL,
	CONSTRAINT pk_vtep_nic_0 UNIQUE ( port_id ),
	CONSTRAINT pk_vtep_port_0 PRIMARY KEY ( port_id ),	
	CONSTRAINT fk_vtep_nic_port FOREIGN KEY (port_id, port_type, vna_id)
      REFERENCES port (id, port_type, vna_id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE CASCADE,    
    CONSTRAINT fk_vtep_port_virtual_switch FOREIGN KEY (virtual_switch_id)
      REFERENCES virtual_switch (switch_id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE RESTRICT 
 );

ALTER TABLE  vtep_port ADD CONSTRAINT ck_93 CHECK ( port_type=6 );

CREATE INDEX idx_mnic_profile_1 ON  vtep_port ( virtual_switch_id );

CREATE INDEX idx_vtep_port_0 ON  vtep_port ( ip_pool_id );


CREATE TABLE vxlan_switch_key
(
  netplane_id numeric(19,0) NOT NULL,
  key_id integer DEFAULT 0,
  CONSTRAINT pk_netplane_id PRIMARY KEY (netplane_id),
  CONSTRAINT fk_netplane FOREIGN KEY (netplane_id)
      REFERENCES netplane (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE CASCADE
)
WITH (
  OIDS=FALSE
);



-- procedure 



CREATE OR REPLACE FUNCTION pf_net_check_add_netplane_ippool(in_uuid character varying, in_num_ip numeric, in_num_mask numeric, in_str_ip character varying, in_str_mask character varying)
  RETURNS integer AS
$BODY$DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
  
  -- ip range 必须已经配置
  SELECT count(*) INTO is_exist FROM ip_range WHERE netplane_id = netplaneid  
       AND ( in_num_ip >= ip_begin_num AND in_num_ip <= ip_end_num ) ;  
  IF is_exist <= 0 THEN 
     result := RTN_MUST_CFG_VALID_RANGE; 
     return result;
  END IF;  
  
  -- ip pool 不能冲突
  SELECT count(*) INTO is_exist FROM ip_pool WHERE netplane_id = netplaneid  
       AND ip_num = in_num_ip;
  IF is_exist > 0 THEN 
     result := RTN_ITEM_CONFLICT; 
     return result;
  END IF;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  
  
  

CREATE OR REPLACE FUNCTION pf_net_add_netplane_ippool(in_uuid character varying, in_num_ip numeric, in_num_mask numeric, in_str_ip character varying, in_str_mask character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_add_netplane_ippool(in_uuid,in_num_ip,in_num_mask,
                            in_str_ip,in_str_mask);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
    
  -- 不支持重复添加 

  -- insert item 
  result.id := nextval('serial_netplane_ip_pool');
  --result.uuid := in_uuid;
  BEGIN 
      INSERT INTO ip_pool(id, netplane_id, ip, mask, ip_num, mask_num) 
          values(result.id, netplaneid, in_str_ip, in_str_mask, in_num_ip, in_num_mask);
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
  
  
  


CREATE OR REPLACE FUNCTION pf_net_check_del_netplane_ippool(in_uuid character varying, in_num_ip numeric, in_num_mask numeric, in_str_ip character varying, in_str_mask character varying)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  netplaneid numeric(19,0);
  npippoolid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- ip pool 必须存在
  SELECT count(*) INTO is_exist FROM ip_pool WHERE netplane_id = netplaneid  
       AND ip_num = in_num_ip AND mask_num = in_num_mask 
       AND ip = in_str_ip AND mask = in_str_mask; 
  SELECT id INTO npippoolid FROM ip_pool WHERE netplane_id = netplaneid  
       AND ip_num = in_num_ip AND mask_num = in_num_mask 
       AND ip = in_str_ip AND mask = in_str_mask; 
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
  
/*
  -- 判断vsi是否在用 
  -- 查看该资源是否在使用
  SELECT count(*) INTO is_exist FROM vsi_profile WHERE logic_network_ip_id = lnippoolid;
  IF is_exist > 0 THEN 
     result := RTN_RESOURCE_USING; 
     return result;
  END IF;
*/  
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  
  
CREATE OR REPLACE FUNCTION pf_net_del_netplane_ippool(in_uuid character varying, in_num_ip numeric, in_num_mask numeric, in_str_ip character varying, in_str_mask character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  netplaneid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- del 不作检查，由应用保准
  
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
    
  SELECT count(*) INTO is_exist FROM ip_pool WHERE netplane_id = netplaneid 
                AND ip_num = in_num_ip AND mask_num = in_num_mask 
                AND ip = in_str_ip AND mask = in_str_mask;
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;                
    
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM ip_pool WHERE netplane_id = netplaneid 
                AND ip_num = in_num_ip AND mask_num = in_num_mask 
                AND ip = in_str_ip AND mask = in_str_mask;
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

  
-- Function: pf_net_query_netplane_ippool(character varying)

-- DROP FUNCTION pf_net_query_netplane_ippool(character varying);

CREATE OR REPLACE FUNCTION pf_net_query_netplane_ippool(in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    netplaneid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    -- netplane uuid 
    SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
    SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF;  
    
    SELECT count(*) INTO is_exist FROM ip_pool  
      WHERE netplane_id = netplaneid ;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF; 
  
    OPEN ref FOR SELECT in_uuid,  ip_num, mask_num, ip, mask 
      FROM ip_pool   
      WHERE netplane_id = netplaneid;
    
    return ref;
END;$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  
  
  -- Function: pf_net_query_netplane_ippool_by_nprange(character varying, numeric, numeric)

-- DROP FUNCTION pf_net_query_netplane_ippool_by_nprange(character varying, numeric, numeric);

CREATE OR REPLACE FUNCTION pf_net_query_netplane_ippool_by_nprange(in_uuid character varying, in_begin numeric, in_end numeric)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    netplaneid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    -- netplane uuid 
    SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
    SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF;  
    
    SELECT count(*) INTO is_exist FROM ip_pool  
      WHERE netplane_id = netplaneid ;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF; 
  
    OPEN ref FOR SELECT in_uuid,  ip_num, mask_num, ip, mask 
      FROM ip_pool   
      WHERE netplane_id = netplaneid
	      AND (ip_num >= in_begin AND ip_num <= in_end);
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;

  
-- Function: pf_net_check_add_netplane_iprange(character varying, numeric, numeric, numeric, character varying, character varying, character varying)

-- DROP FUNCTION pf_net_check_add_netplane_iprange(character varying, numeric, numeric, numeric, character varying, character varying, character varying);

CREATE OR REPLACE FUNCTION pf_net_check_add_netplane_iprange(in_uuid character varying, in_num_begin numeric, in_num_end numeric, in_num_mask numeric, in_str_begin character varying, in_str_end character varying, in_str_mask character varying)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_SWITCH_UPLINKPORT_INVALID integer := 9;
  RTN_PORT_HAS_JOIN_OTHER_SWITCH integer := 10;
  RTN_PG_MUST_CFG_NETPLANE   integer := 13;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- begin end 
  IF in_num_begin > in_num_end THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  
/* 由应用来保准  
  -- ip begin end mask 互相不能重叠  
  SELECT count(*) INTO is_exist FROM ip_range WHERE  netplane_id = netplaneid AND (( in_num_begin >= ip_begin_num AND in_num_begin <= ip_end_num ) OR 
           ( in_num_end >= ip_begin_num AND in_num_end <= ip_end_num ) OR 
           ( in_num_begin <= ip_begin_num AND in_num_end >= ip_end_num ) );  
  IF is_exist > 0 THEN 
     result := RTN_ITEM_CONFLICT; 
     return result;
  END IF;  
*/             
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  


-- Function: pf_net_add_netplane_iprange(character varying, numeric, numeric, numeric, character varying, character varying, character varying)

-- DROP FUNCTION pf_net_add_netplane_iprange(character varying, numeric, numeric, numeric, character varying, character varying, character varying);

CREATE OR REPLACE FUNCTION pf_net_add_netplane_iprange(in_uuid character varying, in_num_begin numeric, in_num_end numeric, in_num_mask numeric, in_str_begin character varying, in_str_end character varying, in_str_mask character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_add_netplane_iprange(in_uuid,in_num_begin,in_num_end,in_num_mask, 
                            in_str_begin,in_str_end,in_str_mask);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- 不支持重复添加 

  -- insert item 
  result.id := nextval('serial_ip_range');
  --result.uuid := in_uuid;
  BEGIN 
    INSERT INTO ip_range(id,netplane_id,ip_begin,ip_end,mask,ip_begin_num,ip_end_num,mask_num) 
            values(result.id,netplaneid,in_str_begin, in_str_end,in_str_mask,in_num_begin,in_num_end,in_num_mask);
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

-- Function: pf_net_del_netplane_iprange(character varying, numeric, numeric, numeric, character varying, character varying, character varying)

-- DROP FUNCTION pf_net_del_netplane_iprange(character varying, numeric, numeric, numeric, character varying, character varying, character varying);

CREATE OR REPLACE FUNCTION pf_net_del_netplane_iprange(in_uuid character varying, in_num_begin numeric, in_num_end numeric, in_num_mask numeric, in_str_begin character varying, in_str_end character varying, in_str_mask character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  netplaneid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- begin end 
  IF in_num_begin > in_num_end THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  
  -- del 不作检查，由应用保准
      
  SELECT count(*) INTO is_exist FROM ip_range WHERE netplane_id = netplaneid 
                AND ip_begin_num = in_num_begin AND ip_end_num = in_num_end AND mask_num = in_num_mask 
                AND ip_begin = in_str_begin AND ip_end = in_str_end AND mask = in_str_mask;
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;                
  
  -- 如果逻辑网络在用，不能删除 
  SELECT count(*) INTO is_exist FROM port_group a, logic_network b, logic_network_ip_range c
     WHERE a.netplane_id = netplaneid AND b.vm_pg_id = a.id AND c.logic_network_id = b.id;
  IF is_exist > 0 THEN 
     result.ret := RTN_RESOURCE_USING; 
     return result;
  END IF; 
      
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM ip_range WHERE netplane_id = netplaneid 
                AND ip_begin_num = in_num_begin AND ip_end_num = in_num_end AND mask_num = in_num_mask 
                AND ip_begin = in_str_begin AND ip_end = in_str_end AND mask = in_str_mask;
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
  
  
-- Function: pf_net_force_del_netplane_iprange(character varying, numeric, numeric, numeric, character varying, character varying, character varying)

-- DROP FUNCTION pf_net_force_del_netplane_iprange(character varying, numeric, numeric, numeric, character varying, character varying, character varying);

CREATE OR REPLACE FUNCTION pf_net_force_del_netplane_iprange(in_uuid character varying, in_num_begin numeric, in_num_end numeric, in_num_mask numeric, in_str_begin character varying, in_str_end character varying, in_str_mask character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  netplaneid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- begin end 
  IF in_num_begin > in_num_end THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  
  -- del 不作检查，由应用保正
      
  SELECT count(*) INTO is_exist FROM ip_range WHERE netplane_id = netplaneid 
                AND ip_begin_num = in_num_begin AND ip_end_num = in_num_end AND mask_num = in_num_mask 
                AND ip_begin = in_str_begin AND ip_end = in_str_end AND mask = in_str_mask;
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST; 
     return result;
  END IF; 
      
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM ip_range WHERE netplane_id = netplaneid 
                AND ip_begin_num = in_num_begin AND ip_end_num = in_num_end AND mask_num = in_num_mask 
                AND ip_begin = in_str_begin AND ip_end = in_str_end AND mask = in_str_mask;
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
  


-- Function: pf_net_query_netplane_iprange(character varying)

-- DROP FUNCTION pf_net_query_netplane_iprange(character varying);

CREATE OR REPLACE FUNCTION pf_net_query_netplane_iprange(in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    netplaneid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    -- logicnetwork uuid 
    SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
    SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF;  
    
    SELECT count(*) INTO is_exist FROM ip_range  
      WHERE netplane_id = netplaneid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF; 
  
    OPEN ref FOR SELECT in_uuid, ip_begin_num,ip_end_num, mask_num, ip_begin, ip_end, mask 
      FROM ip_range  
      WHERE netplane_id = netplaneid;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  


  -- Function: pf_net_query_netplane_iprange_is_using(character varying, numeric, numeric)

-- DROP FUNCTION pf_net_query_netplane_iprange_is_using(character varying, numeric, numeric);

CREATE OR REPLACE FUNCTION pf_net_query_netplane_iprange_is_using(in_netplane_uuid character varying, in_num_begin numeric, in_num_end numeric)
  RETURNS t_query_total AS
$BODY$
DECLARE
  result  t_query_total;
  is_conflict integer;
  netplaneid numeric(19,0);
  is_exist integer; 
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- 检查netplane uuid 是否有效
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_netplane_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_netplane_uuid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  -- 查询logic network ip range 是否已有分配的
  SELECT count(*)  INTO  result.total_num FROM  v_net_logicnetwork_portgroup_netplane a, logic_network_ip_range b WHERE a.logicnetwork_id = b.logic_network_id 
          AND a.netplane_uuid = in_netplane_uuid AND ( in_num_begin <= b.ip_begin_num AND in_num_end >= b.ip_end_num);
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 

-- Function: pf_net_query_netplane_iprange_alloc(character varying, numeric, numeric)

-- DROP FUNCTION pf_net_query_netplane_iprange_alloc(character varying, numeric, numeric);

CREATE OR REPLACE FUNCTION pf_net_query_netplane_iprange_alloc(in_uuid character varying, in_num_begin numeric, in_num_end numeric)
  RETURNS t_query_total AS
$BODY$
DECLARE
  result  t_query_total;
  is_same integer;
  is_exist integer;
  netplaneid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做

   -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
  
  -- begin end 
  IF in_num_begin > in_num_end THEN 
      result.ret  := RTN_PARAM_INVALID; 
      return result;
  END IF;
    
  SELECT count(*) INTO result.total_num FROM ip_pool WHERE netplane_id = netplaneid 
                AND (ip_num >= in_num_begin AND ip_num <= in_num_end );

  return result;

END

$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
 



CREATE OR REPLACE FUNCTION pf_net_add_netplane(in_netplane_name character varying, in_netplane_uuid character varying, in_des character varying, in_mtu integer, in_key integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_conflict integer;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否已存在相同记录
  SELECT count(*) INTO is_same FROM netplane WHERE name = in_netplane_name AND description = in_des  AND mtu = in_mtu;  
--  SELECT count(*) INTO is_same FROM netplane WHERE name = in_netplane_name  AND description = in_des AND mtu = in_mtu;    
  IF is_same > 0 THEN
     result.ret := RTN_HAS_SAME_ITEM;   -- has same item
     SELECT id INTO result.id FROM netplane WHERE name = in_netplane_name;
     SELECT uuid INTO result.uuid FROM netplane WHERE name = in_netplane_name;
     return result;
  END IF;

  -- netplane 名称是否冲突
  SELECT count(*) INTO is_conflict FROM netplane WHERE name = in_netplane_name ;
  IF is_conflict > 0 THEN
     result.ret := RTN_ITEM_CONFLICT; -- conflict 
     return result;
  END IF;

  -- insert item
  result.id := nextval('serial_netplane');
  result.uuid := $2;
  BEGIN 
    INSERT INTO netplane values(result.id,in_netplane_name,in_netplane_uuid,in_des,in_mtu);
    INSERT INTO vxlan_switch_key values(result.id,in_key);
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
  
  
-- Function: pf_net_query_netplane(character varying)

-- DROP FUNCTION pf_net_query_netplane(character varying);

CREATE OR REPLACE FUNCTION pf_net_query_netplane(netplane_uuid character varying)
  RETURNS t_netplane_query AS
$BODY$
DECLARE
  result  t_netplane_query;
  is_exist integer;
  netplaneid numeric(19,0);
  RTN_ITEM_NO_EXIST integer :=4;
BEGIN
--  SELECT uuid into result.uuid, name into result.name,description into result.description,mtu into result.mtu FROM netplane WHERE uuid = $1;
  result.ret := 0;
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = $1;
  IF is_exist > 0 THEN
     result.ret := 0;
  ELSE
     result.ret := RTN_ITEM_NO_EXIST; -- no exist item
     return result;     
  END IF;

   SELECT uuid into result.uuid  FROM netplane WHERE uuid = $1;
   SELECT id into netplaneid  FROM netplane WHERE uuid = $1;
   SELECT name into result.netplane_name  FROM netplane WHERE uuid = $1;
   SELECT description into result.description  FROM netplane WHERE uuid = $1;
   SELECT mtu into result.mtu  FROM netplane WHERE uuid = $1;   
   SELECT key_id into result.key FROM vxlan_switch_key WHERE netplane_id = netplaneid;   
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION pf_net_query_netplane(character varying) OWNER TO tecs;




CREATE OR REPLACE FUNCTION pf_net_query_portgroup(in_uuid character varying)
  RETURNS t_portgroup_query AS
$BODY$
DECLARE
  result  t_portgroup_query;
  is_exist integer;
  pgid numeric(19,0);
  iscfgnetplane integer;
  pgtype integer;
  swithmode integer;
  RTN_ITEM_NO_EXIST integer :=4;
BEGIN

  result.ret := 0;
  SELECT count(*) INTO is_exist FROM port_group WHERE uuid = in_uuid;
  SELECT id INTO pgid FROM port_group WHERE uuid = in_uuid;
  SELECT pg_type INTO pgtype FROM port_group WHERE uuid = in_uuid;
  SELECT switchport_mode INTO swithmode FROM port_group WHERE uuid = in_uuid; 
  IF is_exist > 0 THEN
     result.ret := 0;
  ELSE
     result.ret := RTN_ITEM_NO_EXIST; -- no exist item
     return result;     
  END IF;
   
   result.uuid := in_uuid;

   -- port_group 
   SELECT pg_type into result.pg_type  FROM port_group WHERE uuid = in_uuid;
   SELECT name into result.name  FROM port_group WHERE uuid = in_uuid;
   SELECT acl into result.acl FROM port_group WHERE uuid = in_uuid;
   SELECT qos into result.qos  FROM port_group WHERE uuid = in_uuid;
   SELECT allowed_priorities into result.allowed_priorities  FROM port_group WHERE uuid = in_uuid;
   SELECT receive_bandwidth_limit into result.receive_bandwidth_limit  FROM port_group WHERE uuid = in_uuid;
   SELECT receive_bandwidth_reserve into result.receive_bandwidth_reserve  FROM port_group WHERE uuid = in_uuid;
      
   SELECT default_vlan_id into result.default_vlan_id  FROM port_group WHERE uuid = in_uuid;
   SELECT promiscuous into result.promiscuous  FROM port_group WHERE uuid = in_uuid;
   SELECT macvlan_filter_enable into result.macvlan_filter_enable  FROM port_group WHERE uuid = in_uuid;
   SELECT allowed_transmit_macs into result.allowed_transmit_macs  FROM port_group WHERE uuid = in_uuid;
   SELECT allowed_transmit_macvlans into result.allowed_transmit_macvlans  FROM port_group WHERE uuid = in_uuid;
   SELECT policy_block_override into result.policy_block_override  FROM port_group WHERE uuid = in_uuid;
   SELECT policy_vlan_override into result.policy_vlan_override  FROM port_group WHERE uuid = in_uuid;
   SELECT version into result.version  FROM port_group WHERE uuid = in_uuid;
   SELECT mgr_id into result.mgr_id  FROM port_group WHERE uuid = in_uuid;
   SELECT type_id into result.type_id  FROM port_group WHERE uuid = in_uuid;
   SELECT allow_mac_change into result.allow_mac_change  FROM port_group WHERE uuid = in_uuid;
   SELECT switchport_mode into result.switchport_mode  FROM port_group WHERE uuid = in_uuid;
   
   SELECT count(*) INTO is_exist FROM port_group WHERE  uuid = in_uuid AND netplane_id is not null;
   SELECT count(*) INTO is_exist FROM port_group WHERE  uuid = in_uuid AND netplane_id is not null;
   IF is_exist > 0 THEN 
       result.is_cfg_netplane := 1;
       SELECT a.uuid INTO result.netplane_uuid FROM netplane a, port_group b WHERE a.id = b.netplane_id AND b.uuid = in_uuid;
   ELSE 
       result.is_cfg_netplane := 0;
   END IF;
   
   SELECT mtu into result.mtu FROM port_group WHERE uuid = in_uuid;
   SELECT description into result.description FROM port_group WHERE uuid = in_uuid;
    
   -- trunk 
   IF swithmode = 0 THEN 
      SELECT native_vlan_num INTO result.trunk_native_vlan_num FROM pg_switchport_trunk WHERE pg_id = pgid;   
   END IF;
   
   -- access 
   IF swithmode = 1 THEN 
      IF pgtype = 21 THEN 
          SELECT isolate_no INTO result.access_isolate_no FROM pg_switchport_access_isolate WHERE pg_id = pgid;   
      ELSE 
          SELECT vlan_num INTO result.access_vlan_num FROM pg_switchport_access_vlan WHERE pg_id = pgid;   
      END IF;
   END IF;

   /*vxlan sdn */
   IF pgtype = 0 THEN 
      
	   SELECT is_sdn into result.is_sdn FROM uplink_pg WHERE pg_id = pgid AND pg_type = result.pg_type;
	   SELECT isolate_type into result.isolate_type FROM uplink_pg WHERE pg_id = pgid AND pg_type = result.pg_type;
	   
   END IF;
   
   IF pgtype = 21 OR pgtype = 20 THEN 
       SELECT is_sdn into result.is_sdn FROM vm_pg WHERE pg_id = pgid AND pg_type = result.pg_type;
       SELECT isolate_type into result.isolate_type FROM vm_pg WHERE pg_id = pgid AND pg_type = result.pg_type;
       -- RAISE WARNING '------- in_uuid:%   pg_type:%   isolatetype:%', in_uuid, result.pg_type, result.isolate_type;
       IF result.isolate_type = 1 THEN 
          IF pgtype = 21 THEN 
              SELECT isolate_no INTO result.access_isolate_no FROM vm_pg_private_vxlan_segment WHERE vmpg_id = pgid;   
          ELSE 
              SELECT segment_num INTO result.segment_num FROM vm_pg_share_vxlan_segment WHERE vmpg_id = pgid;   
          END IF;
       END IF;
   END IF;

  return result;
END$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


  
DROP  FUNCTION pf_net_add_portgroup(in_name character varying, in_pg_type integer, in_uuid character varying, 
in_acl character varying, in_qos character varying, in_allowed_priorities character varying, 
in_receive_bandwidth_limit character varying, in_receive_bandwidth_reserve character varying, 
in_default_vlan_id integer, in_promiscuous integer, in_macvlan_filter_enable integer, in_allowed_transmit_macs character varying,
 in_allowed_transmit_macvlans character varying, in_policy_block_override character varying, in_policy_vlan_override character varying,
 in_version integer, in_mgr_id character varying, in_type_id character varying, in_allow_mac_change integer, in_switchport_mode integer, 
 in_is_cfg_netplane integer, in_netplane_uuid character varying, in_mtu integer, in_trunk_native_vlan_num integer, in_access_vlan_num integer, 
 in_access_isolate integer, in_desc character varying);
 
CREATE OR REPLACE FUNCTION pf_net_add_portgroup(in_name character varying, in_pg_type integer, in_uuid character varying, in_acl character varying, in_qos character varying, in_allowed_priorities character varying, in_receive_bandwidth_limit character varying, 
in_receive_bandwidth_reserve character varying, in_default_vlan_id integer, in_promiscuous integer, in_macvlan_filter_enable integer, 
in_allowed_transmit_macs character varying, in_allowed_transmit_macvlans character varying, in_policy_block_override character varying,
 in_policy_vlan_override character varying, in_version integer, in_mgr_id character varying, in_type_id character varying, in_allow_mac_change integer, 
 in_switchport_mode integer, in_is_cfg_netplane integer, in_netplane_uuid character varying, in_mtu integer, in_trunk_native_vlan_num integer, 
 in_access_vlan_num integer, in_access_isolate integer, in_desc character varying, in_isolate_type integer, in_segment_id_num integer, 
 in_vxlan_isolate integer, in_is_sdn integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_same_pg integer;
  is_conflict integer;
  npmtu integer;
  pgid numeric(19,0);
  netplaneid numeric(19,0);
  is_exist integer; 
  db_pg_uuid character varying;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG integer := 5;
  RTN_PARAM_INVALID integer := 7;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  IF in_is_cfg_netplane > 0 THEN 
     SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_netplane_uuid;
     SELECT id INTO netplaneid FROM netplane WHERE uuid = in_netplane_uuid;
	 SELECT mtu INTO npmtu FROM netplane WHERE uuid = in_netplane_uuid;
     IF is_exist <= 0 THEN 
        result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
        return result;
     END IF;  
	 IF npmtu < in_mtu THEN 
	    result.ret := RTN_NETPLANE_MTU_LESS_PG;   -- PG MTU 不能大于 netplane mtu 
        return result;
	 END IF;
  END IF;
    
  -- 不支持重复添加 
  -- 判断name是否冲突
  SELECT count(*) INTO is_exist FROM port_group WHERE name = in_name;
  IF is_exist > 0 THEN 
     result.ret := RTN_ITEM_CONFLICT; -- conflict 
     return result;
  END IF;
  
  -- 检查switch_mode
  IF in_switchport_mode < 0 THEN 
    result.ret := RTN_PARAM_INVALID; -- param invalid 
    return result;   
  END IF;
  
  IF in_switchport_mode > 1  THEN 
    result.ret := RTN_PARAM_INVALID; -- param invalid  
    return result;   
  END IF;
  
  -- 检查pg_type 
  
  -- insert item
  result.id := nextval('serial_port_group');
  result.uuid := in_uuid;
    
  BEGIN 
    IF in_is_cfg_netplane > 0 THEN     
        INSERT INTO port_group(id,name,pg_type,uuid,acl,qos,allowed_priorities,
            receive_bandwidth_limit,receive_bandwidth_reserve,default_vlan_id,
            promiscuous,macvlan_filter_enable,allowed_transmit_macs,allowed_transmit_macvlans,
            policy_block_override,policy_vlan_override,version,mgr_id,type_id,
            allow_mac_change,switchport_mode,netplane_id, mtu, description) 
        values(result.id,in_name,in_pg_type,in_uuid,in_acl,in_qos,in_allowed_priorities,
            in_receive_bandwidth_limit,in_receive_bandwidth_reserve,in_default_vlan_id,
            in_promiscuous,in_macvlan_filter_enable,in_allowed_transmit_macs,in_allowed_transmit_macvlans,
            in_policy_block_override,in_policy_vlan_override,in_version,in_mgr_id,in_type_id,
            in_allow_mac_change,in_switchport_mode,netplaneid, in_mtu,in_desc);     
    ELSE 
       INSERT INTO port_group(id,name,pg_type,uuid,acl,qos,allowed_priorities,
            receive_bandwidth_limit,receive_bandwidth_reserve,default_vlan_id,
            promiscuous,macvlan_filter_enable,allowed_transmit_macs,allowed_transmit_macvlans,
            policy_block_override,policy_vlan_override,version,mgr_id,type_id,
            allow_mac_change,switchport_mode, mtu,description) 
        values(result.id,in_name,in_pg_type,in_uuid,in_acl,in_qos,in_allowed_priorities,
            in_receive_bandwidth_limit,in_receive_bandwidth_reserve,in_default_vlan_id,
            in_promiscuous,in_macvlan_filter_enable,in_allowed_transmit_macs,in_allowed_transmit_macvlans,
            in_policy_block_override,in_policy_vlan_override,in_version,in_mgr_id,in_type_id,
            in_allow_mac_change,in_switchport_mode, in_mtu,in_desc);    
    END IF;
    
    -- uplink pg
    IF in_pg_type = 0 THEN 
        INSERT INTO uplink_pg(pg_id,pg_type,isolate_type,is_sdn) values(result.id, in_pg_type, in_isolate_type,in_is_sdn);
    END IF;
    
    -- kernel_pg
    IF in_pg_type = 10 OR in_pg_type = 11 THEN 
      INSERT INTO kernel_pg values(result.id, in_pg_type);
    END IF;
    
    -- vm_pg
    IF in_pg_type = 21 THEN 
      INSERT INTO vm_pg values(result.id, in_pg_type, in_isolate_type,in_is_sdn);
	  IF in_isolate_type > 0 THEN 
	      INSERT INTO vm_pg_private_vxlan_segment values(result.id, in_vxlan_isolate);
	  END IF;
    END IF;

    IF in_pg_type = 20 THEN 
      INSERT INTO vm_pg values(result.id, in_pg_type, in_isolate_type);
	  IF in_isolate_type > 0 THEN 
          INSERT INTO vm_pg_share_vxlan_segment values(result.id, in_segment_id_num);
	  END IF;
    END IF;
    
    -- pg_switchport_trunk 
    IF in_switchport_mode = 0 THEN 
        INSERT INTO pg_switchport_trunk values(result.id, in_switchport_mode, in_trunk_native_vlan_num );
    END IF;
    
    IF in_switchport_mode = 1 THEN 
      IF in_pg_type = 21 THEN 
          INSERT INTO pg_switchport_access_isolate values(result.id, in_pg_type,in_switchport_mode, in_access_isolate );      
      ELSE
          INSERT INTO pg_switchport_access_vlan values(result.id, in_pg_type,in_switchport_mode, in_access_vlan_num );      
      END IF;
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

  
CREATE OR REPLACE FUNCTION pf_net_modify_portgroup(in_name character varying, in_pg_type integer, in_uuid character varying, in_acl character varying, in_qos character varying, 
in_allowed_priorities character varying, in_receive_bandwidth_limit character varying, in_receive_bandwidth_reserve character varying, 
in_default_vlan_id integer, in_promiscuous integer, in_macvlan_filter_enable integer, in_allowed_transmit_macs character varying, 
in_allowed_transmit_macvlans character varying, in_policy_block_override character varying, in_policy_vlan_override character varying, 
in_version integer, in_mgr_id character varying, in_type_id character varying, in_allow_mac_change integer, in_switchport_mode integer, 
in_is_cfg_netplane integer, in_netplane_uuid character varying, in_mtu integer, in_trunk_native_vlan_num integer, in_access_vlan_num integer, 
in_access_isolate integer, in_desc character varying, in_isolate_type integer, in_segment_id_num integer,in_vxlan_isolate integer, in_is_sdn integer)
  RETURNS t_operate_result AS
$BODY$DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  is_modify_netplane integer;
  npmtu integer;
  netplaneid numeric(19,0);
  oldnetplaneid numeric(19,0);
  pgid numeric(19,0);
  db_pg_type integer;
  ismodifyswitchportmode integer;
  oldswitchportmode integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG integer := 5;
  RTN_PARAM_INVALID integer := 7;
BEGIN
  result.ret := 0;
  
  -- 入参检查 后面做  
  -- netplane check
  IF in_is_cfg_netplane > 0 THEN 
     SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_netplane_uuid;
     SELECT id INTO netplaneid FROM netplane WHERE uuid = in_netplane_uuid;
     IF is_exist <= 0 THEN 
        result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
        return result;
     END IF;  
  END IF;
    
  -- uuid check
  SELECT count(*) INTO is_exist FROM port_group WHERE uuid = in_uuid;
  SELECT pg_type INTO db_pg_type FROM port_group WHERE uuid = in_uuid;  
  SELECT id INTO pgid FROM port_group WHERE uuid = in_uuid;  
  SELECT switchport_mode INTO oldswitchportmode FROM port_group WHERE uuid = in_uuid;   
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST; -- conflict 
     return result;
  END IF;
  
  -- pg_type check 
  SELECT count(*) INTO is_exist FROM port_group WHERE uuid = in_uuid AND db_pg_type = in_pg_type;
  IF is_exist <= 0 THEN 
     result.ret := RTN_PARAM_INVALID; -- param invalid 
     return result;
  END IF;
    
  -- 判断name是否冲突
  SELECT count(*) INTO is_exist FROM port_group WHERE name = in_name AND  uuid = in_uuid;
  IF is_exist <= 0 THEN 
     SELECT count(*) INTO is_exist FROM port_group WHERE name = in_name;
     IF is_exist > 0 THEN 
        result.ret := RTN_ITEM_CONFLICT; -- conflict 
        return result;
     END IF;
  END IF;
  
  -- 检查switch_mode
  IF in_switchport_mode < 0 THEN 
    result.ret := RTN_PARAM_INVALID; -- param invalid 
    return result;   
  END IF;
  
  IF in_switchport_mode > 1  THEN 
    result.ret := RTN_PARAM_INVALID; -- param invalid  
    return result;   
  END IF;
  
  -- 检查 switch_mode 是否变化
  IF in_switchport_mode = oldswitchportmode THEN 
      ismodifyswitchportmode := 0;
  ELSE
      ismodifyswitchportmode := 1;
  END IF;
  
  -- 检查pg_type 
  
  -- 检查netplane modify，之前已经配置了netplane，能将之修改，但必须mtu >= pg_mtu
  is_modify_netplane := 0;
  SELECT count(*) INTO is_exist FROM port_group WHERE uuid = in_uuid AND netplane_id is not null;
  SELECT netplane_id INTO oldnetplaneid FROM port_group WHERE uuid = in_uuid AND netplane_id is not null; 
  IF is_exist > 0 THEN 
     IF in_is_cfg_netplane <= 0 THEN 
        result.ret := RTN_PARAM_INVALID; -- param invalid  原先配置netplane，不能将之修改为NULL
        return result; 
     ELSE        
		SELECT mtu INTO npmtu FROM netplane WHERE id = netplaneid;
        IF npmtu < in_mtu THEN 
            result.ret := RTN_NETPLANE_MTU_LESS_PG; -- param invalid  netplane mtu 小于pg_mtu，不能将之修改
            return result;
        END IF;    
     END IF;
  END IF; 
          
  -- modify item
  result.id  := pgid;
  result.uuid := in_uuid;
  BEGIN 
    
    -- 将老的switch mode 数据删除
    IF ismodifyswitchportmode > 0 THEN 
       -- 修改switchport mode需要删除以前记录
       IF oldswitchportmode = 0 THEN 
            DELETE FROM pg_switchport_trunk WHERE pg_id = pgid;            
       ELSE 
          IF in_pg_type = 21 THEN 
              DELETE FROM pg_switchport_access_isolate WHERE pg_id = pgid;
          ELSE 
              DELETE FROM pg_switchport_access_vlan WHERE pg_id = pgid;
          END IF;
       END IF;    
    END IF;
    
    IF in_is_cfg_netplane > 0 THEN     
       UPDATE port_group SET name = in_name, pg_type = in_pg_type,acl = in_acl,qos = in_qos,allowed_priorities = in_allowed_priorities,
            receive_bandwidth_limit = in_receive_bandwidth_limit,receive_bandwidth_reserve = in_receive_bandwidth_reserve ,default_vlan_id = in_default_vlan_id,
            promiscuous = in_promiscuous,macvlan_filter_enable = in_macvlan_filter_enable,allowed_transmit_macs = in_allowed_transmit_macs,
            allowed_transmit_macvlans = in_allowed_transmit_macvlans,
            policy_block_override = in_policy_block_override,policy_vlan_override = in_policy_vlan_override,version = in_version,mgr_id = in_mgr_id,type_id = in_type_id,
            allow_mac_change = in_allow_mac_change ,switchport_mode = in_switchport_mode, netplane_id = netplaneid, mtu = in_mtu, description = in_desc    
          WHERE uuid = in_uuid ;    
    ELSE 
       UPDATE port_group SET name = in_name, pg_type = in_pg_type,acl = in_acl,qos = in_qos,allowed_priorities = in_allowed_priorities,
            receive_bandwidth_limit = in_receive_bandwidth_limit,receive_bandwidth_reserve = in_receive_bandwidth_reserve ,default_vlan_id = in_default_vlan_id,
            promiscuous = in_promiscuous,macvlan_filter_enable = in_macvlan_filter_enable,allowed_transmit_macs = in_allowed_transmit_macs,
            allowed_transmit_macvlans = in_allowed_transmit_macvlans,
            policy_block_override = in_policy_block_override,policy_vlan_override = in_policy_vlan_override,version = in_version,mgr_id = in_mgr_id,type_id = in_type_id,
            allow_mac_change = in_allow_mac_change ,switchport_mode = in_switchport_mode, mtu = in_mtu, description = in_desc 
          WHERE uuid = in_uuid ;  
    END IF;
    
    -- uplink pg
	IF in_pg_type = 0 THEN 
	    UPDATE uplink_pg SET isolate_type = in_isolate_type,is_sdn = in_is_sdn WHERE pg_id = pgid;
	END IF;
	
	IF (in_pg_type = 20 ) OR (in_pg_type = 21) THEN 
	    UPDATE vm_pg SET isolate_type = in_isolate_type,is_sdn = in_is_sdn WHERE pg_id = pgid;
	END IF;
    -- 
    -- pg_switchport_trunk 
    IF in_switchport_mode = 0 THEN 
        IF ismodifyswitchportmode > 0 THEN 
            INSERT INTO pg_switchport_trunk values(pgid, in_switchport_mode, in_trunk_native_vlan_num );
        ELSE 
            UPDATE pg_switchport_trunk SET native_vlan_num = in_trunk_native_vlan_num WHERE pg_id = pgid;
        END IF;
    END IF;
    
    -- PG access
    IF in_switchport_mode = 1 THEN 
      IF in_pg_type = 21 THEN 
          IF ismodifyswitchportmode > 0 THEN 
              INSERT INTO pg_switchport_access_isolate values(pgid, in_pg_type,in_switchport_mode, in_access_isolate );      
          ELSE
              UPDATE pg_switchport_access_isolate SET isolate_no = in_access_isolate  WHERE pg_id = pgid;  
          END IF;
      ELSE
          IF ismodifyswitchportmode > 0 THEN 
              INSERT INTO pg_switchport_access_vlan values(pgid, in_pg_type,in_switchport_mode, in_access_vlan_num );    
          ELSE
              UPDATE pg_switchport_access_vlan SET vlan_num = in_access_vlan_num  WHERE pg_id = pgid; 
          END IF;          
      END IF;
    END IF; 

    IF in_isolate_type = 1 THEN	
        IF in_pg_type = 21 THEN 
		    SELECT count(*) INTO is_exist from vm_pg_private_vxlan_segment WHERE vmpg_id = pgid; 
			IF is_exist > 0 THEN 
                UPDATE vm_pg_private_vxlan_segment SET isolate_no = in_vxlan_isolate  WHERE vmpg_id = pgid; 
			ELSE
			    DELETE FROM vm_pg_share_vxlan_segment WHERE vmpg_id = pgid;
			    INSERT INTO vm_pg_private_vxlan_segment values( pgid, in_vxlan_isolate);
			END IF;
        END IF;    
        IF in_pg_type = 20 THEN 
		    SELECT count(*) INTO is_exist from vm_pg_share_vxlan_segment WHERE vmpg_id = pgid;
			IF is_exist > 0 THEN 
                UPDATE vm_pg_share_vxlan_segment SET segment_num = in_segment_id_num  WHERE vmpg_id = pgid; 
			ELSE
			    DELETE FROM vm_pg_private_vxlan_segment WHERE vmpg_id = pgid;
                INSERT INTO vm_pg_share_vxlan_segment values(pgid, in_segment_id_num);
			END IF;
        END IF;
		
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
  
  

  
drop FUNCTION pf_net_schedule(in_netplane_uuid character varying, 
                           in_is_sriov integer, in_is_loop integer, in_nic_num integer, in_is_bond_nic integer,
                           in_mtu integer , in_is_watchdog integer);
CREATE OR REPLACE FUNCTION pf_net_schedule(in_netplane_uuid character varying, 
                           in_is_sriov integer, in_is_loop integer, in_nic_num integer, in_is_bond_nic integer,
                           in_mtu integer , in_is_watchdog integer,
						   in_is_sdn integer, in_is_vxlan integer)
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
            return pf_net_edvs_schedule_has_bond_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',0, in_is_sdn,in_is_vxlan);             
        ELSE        
            return pf_net_edvs_schedule_no_bond_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',0, in_is_sdn,in_is_vxlan);             
        END IF;
    ELSE
        return pf_net_sdvs_schedule_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',0, in_is_sdn,in_is_vxlan);    
    END IF;
ELSE
    IF in_is_sriov > 0 THEN       
        IF in_is_bond_nic > 0 THEN 
            return pf_net_edvs_schedule_has_bond_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',0, in_is_sdn,in_is_vxlan);             
        ELSE        
            return pf_net_edvs_schedule_no_bond_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',0, in_is_sdn,in_is_vxlan);             
        END IF;
    ELSE
        return pf_net_sdvs_schedule_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',0, in_is_sdn,in_is_vxlan);    
    END IF;
END IF;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  

drop  FUNCTION pf_net_schedule_by_cluster(in_netplane_uuid character varying, 
                           in_is_sriov integer, in_is_loop integer, in_nic_num integer, in_is_bond_nic integer,
                           in_mtu integer, in_cluster_name character varying, in_is_watchdog integer);
CREATE OR REPLACE FUNCTION pf_net_schedule_by_cluster(in_netplane_uuid character varying, 
                           in_is_sriov integer, in_is_loop integer, in_nic_num integer, in_is_bond_nic integer,
                           in_mtu integer, in_cluster_name character varying, in_is_watchdog integer,
						   in_is_sdn integer, in_is_vxlan integer)
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
            return pf_net_edvs_schedule_has_bond_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,1,in_cluster_name,0, in_is_sdn,in_is_vxlan);             
        ELSE        
            return pf_net_edvs_schedule_no_bond_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,1,in_cluster_name,0, in_is_sdn,in_is_vxlan);             
        END IF;
    ELSE
        return pf_net_sdvs_schedule_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,1,in_cluster_name,0, in_is_sdn,in_is_vxlan);    
    END IF;
ELSE
    IF in_is_sriov > 0 THEN       
        IF in_is_bond_nic > 0 THEN 
            return pf_net_edvs_schedule_has_bond_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,1,in_cluster_name,0, in_is_sdn,in_is_vxlan);             
        ELSE        
            return pf_net_edvs_schedule_no_bond_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,1,in_cluster_name,0, in_is_sdn,in_is_vxlan);             
        END IF;
    ELSE
        return pf_net_sdvs_schedule_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,1,in_cluster_name,0, in_is_sdn,in_is_vxlan);    
    END IF;
END IF;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
    
  
  
drop FUNCTION pf_net_schedule_by_vna(in_netplane_uuid character varying, 
                           in_is_sriov integer, in_is_loop integer, in_nic_num integer, in_is_bond_nic integer,
                           in_mtu integer, in_vna_uuid character varying, in_is_watchdog integer);
CREATE OR REPLACE FUNCTION pf_net_schedule_by_vna(in_netplane_uuid character varying, 
                           in_is_sriov integer, in_is_loop integer, in_nic_num integer, in_is_bond_nic integer,
                           in_mtu integer, in_vna_uuid character varying, in_is_watchdog integer,
						   in_is_sdn integer, in_is_vxlan integer)
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
            return pf_net_edvs_schedule_has_bond_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',vnaid, in_is_sdn,in_is_vxlan);             
        ELSE        
            return pf_net_edvs_schedule_no_bond_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',vnaid, in_is_sdn,in_is_vxlan);             
        END IF;
    ELSE
        return pf_net_sdvs_schedule_no_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',vnaid, in_is_sdn,in_is_vxlan);    
    END IF;
ELSE
    IF in_is_sriov > 0 THEN       
        IF in_is_bond_nic > 0 THEN 
            return pf_net_edvs_schedule_has_bond_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',vnaid, in_is_sdn,in_is_vxlan);             
        ELSE        
            return pf_net_edvs_schedule_no_bond_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',vnaid, in_is_sdn,in_is_vxlan);             
        END IF;
    ELSE
        return pf_net_sdvs_schedule_has_wdg(netplaneid, in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num ,in_is_bond_nic,in_mtu,in_is_watchdog,0,'',vnaid, in_is_sdn,in_is_vxlan);    
    END IF;
END IF;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
     
	 

-- 调度模块 获取 sdvs has wdg 
CREATE OR REPLACE FUNCTION pf_net_sdvs_schedule_has_wdg(in_netplane_id numeric(19,0), in_netplane_uuid character varying, 
                           in_is_sriov integer, in_is_loop integer, in_nic_num integer, in_is_bond_nic integer,
                           in_mtu integer , in_is_watchdog integer,
                           in_cluster_id numeric(19,0), in_cluster_name character varying, 
                           in_vna_id numeric(19,0) ,
						   in_is_sdn integer, in_is_vxlan integer)                           
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    check_ret integer;
    netplaneid numeric(19,0);
    
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    /* input param valid: in_netplane_id, in_nic_num, in_mtu, in_is_loop          
     switch chioce  */   
    IF in_cluster_id > 0 THEN 
        IF in_vna_id > 0 THEN 
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,in_is_sdn , in_is_vxlan ,
                 cluster_name,vna_uuid,module_name, switch_uuid, switch_sriov_free_num, switch_free_num, 
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_sdvs_schedule_has_wdg 
            WHERE 
                 cluster_name = in_cluster_name AND vna_id = in_vna_id AND
                 /* cluster_id = in_cluster_id AND vna_id = in_vna_id AND      */
                 /*cluster vna  filter*/
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu 
				 AND is_support_vxlan >= in_is_vxlan 
				 AND is_sdn = in_is_sdn;
                 
        ELSE
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,in_is_sdn , in_is_vxlan ,
                 cluster_name,vna_uuid,module_name, switch_uuid, switch_sriov_free_num, switch_free_num, 
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_sdvs_schedule_has_wdg 
            WHERE
                 cluster_name = in_cluster_name AND 
                 /*cluster_id = in_cluster_id AND */   /* cluster filter   */
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu
                 AND is_support_vxlan >= in_is_vxlan 
				 AND is_sdn = in_is_sdn;				 
        END IF;             
    ELSE
         IF in_vna_id > 0 THEN 
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,in_is_sdn , in_is_vxlan ,
                 cluster_name,vna_uuid,module_name, switch_uuid, switch_sriov_free_num, switch_free_num, 
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_sdvs_schedule_has_wdg 
            WHERE 
                 vna_id = in_vna_id AND     /*  vna filter */
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu
                 AND is_support_vxlan >= in_is_vxlan 
				 AND is_sdn = in_is_sdn;				 
        ELSE
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,in_is_sdn , in_is_vxlan ,
                 cluster_name,vna_uuid,module_name, switch_uuid, switch_sriov_free_num, switch_free_num, 
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_sdvs_schedule_has_wdg 
            WHERE netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu
                 AND is_support_vxlan >= in_is_vxlan 
				 AND is_sdn = in_is_sdn;				 
        END IF;    
    END IF;         
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 调度模块 获取 sdvs no wdg 
CREATE OR REPLACE FUNCTION pf_net_sdvs_schedule_no_wdg(in_netplane_id numeric(19,0), in_netplane_uuid character varying, 
                           in_is_sriov integer, in_is_loop integer, in_nic_num integer, in_is_bond_nic integer,
                           in_mtu integer , in_is_watchdog integer,
                           in_cluster_id numeric(19,0), in_cluster_name character varying, 
                           in_vna_id numeric(19,0) ,
						   in_is_sdn integer, in_is_vxlan integer)                           
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    check_ret integer;
    netplaneid numeric(19,0);
    
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    /* input param valid: in_netplane_id, in_nic_num, in_mtu, in_is_loop          
     switch chioce   */
    IF in_cluster_id > 0 THEN 
        IF in_vna_id > 0 THEN     
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,in_is_sdn , in_is_vxlan ,
                 cluster_name,vna_uuid,module_name, switch_uuid, switch_sriov_free_num, switch_free_num, 
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_sdvs_schedule_no_wdg 
            WHERE 
                 cluster_name = in_cluster_name AND vna_id = in_vna_id AND 
                 /* cluster_id = in_cluster_id AND vna_id = in_vna_id AND */    /* cluster vna filter */
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu 
                 AND is_support_vxlan >= in_is_vxlan 
				 AND is_sdn = in_is_sdn;				 
        ELSE
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,in_is_sdn , in_is_vxlan ,
                 cluster_name,vna_uuid,module_name, switch_uuid, switch_sriov_free_num, switch_free_num, 
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_sdvs_schedule_no_wdg 
            WHERE 
                 cluster_name = in_cluster_name AND 
                 /* cluster_id = in_cluster_id AND */   /* cluster filter */
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu
                 AND is_support_vxlan >= in_is_vxlan 
				 AND is_sdn = in_is_sdn;				 
        END IF;
    ELSE
        IF in_vna_id > 0 THEN     
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,in_is_sdn , in_is_vxlan ,
                 cluster_name,vna_uuid,module_name, switch_uuid, switch_sriov_free_num, switch_free_num, 
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_sdvs_schedule_no_wdg 
            WHERE 
                 vna_id = in_vna_id AND     /*  vna filter */
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu
                 AND is_support_vxlan >= in_is_vxlan 
				 AND is_sdn = in_is_sdn;				 
        ELSE
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,in_is_sdn , in_is_vxlan ,
                 cluster_name,vna_uuid,module_name, switch_uuid, switch_sriov_free_num, switch_free_num, 
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_sdvs_schedule_no_wdg 
            WHERE 
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu
                 AND is_support_vxlan >= in_is_vxlan 
				 AND is_sdn = in_is_sdn;				 
        END IF;
    END IF;
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
       
                           


-- 调度模块 获取 edvs no bond no wdg 
CREATE OR REPLACE FUNCTION pf_net_edvs_schedule_no_bond_no_wdg(in_netplane_id numeric(19,0), in_netplane_uuid character varying, 
                           in_is_sriov integer, in_is_loop integer, in_nic_num integer, in_is_bond_nic integer,
                           in_mtu integer , in_is_watchdog integer,
                           in_cluster_id numeric(19,0),  in_cluster_name character varying, 
                           in_vna_id numeric(19,0) ,
						   in_is_sdn integer, in_is_vxlan integer)                           
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    check_ret integer;
    netplaneid numeric(19,0);
    
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    /* input param valid: in_netplane_id, in_nic_num, in_mtu, in_is_loop          
     switch chioce  */
    /*
       edvs physical, switch_sriov_free_num 存放的是 physical口剩余可用VF数目
    */
    IF in_cluster_id > 0 THEN 
        IF in_vna_id > 0 THEN     
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,in_is_sdn , in_is_vxlan ,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 port_free_sriovvf_num,port_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_edvs_schedule_no_bond_no_wdg 
            WHERE 
                 cluster_name = in_cluster_name AND vna_id = in_vna_id AND 
                 /* cluster_id = in_cluster_id AND vna_id = in_vna_id AND */     /* cluster vna filter */
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu
                 AND is_support_vxlan >= in_is_vxlan 
				 AND is_sdn = in_is_sdn;				 
        ELSE
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,in_is_sdn , in_is_vxlan ,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 port_free_sriovvf_num,port_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_edvs_schedule_no_bond_no_wdg 
            WHERE 
                 cluster_name = in_cluster_name AND
                 /* cluster_id = in_cluster_id AND  */   /* cluster filter */
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu
                 AND is_support_vxlan >= in_is_vxlan 
				 AND is_sdn = in_is_sdn;				 
        END IF;
    ELSE
        IF in_vna_id > 0 THEN     
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,in_is_sdn , in_is_vxlan ,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 port_free_sriovvf_num,port_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_edvs_schedule_no_bond_no_wdg 
            WHERE 
                 vna_id = in_vna_id AND     /* vna filter */
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu
                 AND is_support_vxlan >= in_is_vxlan 
				 AND is_sdn = in_is_sdn;				 
        ELSE
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,in_is_sdn , in_is_vxlan ,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 port_free_sriovvf_num,port_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_edvs_schedule_no_bond_no_wdg 
            WHERE 
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu
                 AND is_support_vxlan >= in_is_vxlan 
				 AND is_sdn = in_is_sdn;				 
        END IF;
    END IF;    
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
                                  

-- 调度模块 获取 edvs has bond no wdg 
CREATE OR REPLACE FUNCTION pf_net_edvs_schedule_has_bond_no_wdg(in_netplane_id numeric(19,0), in_netplane_uuid character varying, 
                           in_is_sriov integer, in_is_loop integer, in_nic_num integer, in_is_bond_nic integer,
                           in_mtu integer , in_is_watchdog integer,
                           in_cluster_id numeric(19,0), in_cluster_name character varying, 
                           in_vna_id numeric(19,0),
						   in_is_sdn integer, in_is_vxlan integer)                               
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    check_ret integer;
    netplaneid numeric(19,0);
    
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    /* input param valid: in_netplane_id, in_nic_num, in_mtu, in_is_loop          
     switch chioce     */
    /*
       edvs bond, switch_sriov_free_num 存放的是 bond口剩余可用VF数目
    */
    IF in_cluster_id > 0 THEN 
        IF in_vna_id > 0 THEN         
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,in_is_sdn , in_is_vxlan ,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 bond_free_sriovvf_num,bond_free_sriovvf_num, 
                 /*switch_sriov_free_num,switch_free_num, */ 
                 pg_mtu,  port_uuid,
                 port_type,bond_total_phy_num,bond_valid_phy_num,phy_port_uuid,phy_port_free_sriovvf_num
            FROM v_net_edvs_schedule_has_bond_no_wdg 
            WHERE             
                 cluster_name = in_cluster_name AND vna_id = in_vna_id AND 
                 /* cluster_id = in_cluster_id AND vna_id = in_vna_id AND */    /* cluster vna  filter */
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu 
                 AND bond_valid_phy_num > 0 
                 AND is_support_vxlan >= in_is_vxlan 
				 AND is_sdn = in_is_sdn;					 
        ELSE
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,in_is_sdn , in_is_vxlan ,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 bond_free_sriovvf_num,bond_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num,*/
                 pg_mtu,  port_uuid,
                 port_type,bond_total_phy_num,bond_valid_phy_num,phy_port_uuid,phy_port_free_sriovvf_num
            FROM v_net_edvs_schedule_has_bond_no_wdg 
            WHERE             
                 cluster_name = in_cluster_name AND 
                 /* cluster_id = in_cluster_id AND */    /* cluster vna filter */
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu 
                 AND bond_valid_phy_num > 0 
				 AND is_support_vxlan >= in_is_vxlan 
				 AND is_sdn = in_is_sdn;	
        END IF;
    ELSE
        IF in_vna_id > 0 THEN         
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,in_is_sdn , in_is_vxlan ,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 bond_free_sriovvf_num,bond_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,bond_total_phy_num,bond_valid_phy_num,phy_port_uuid,phy_port_free_sriovvf_num
            FROM v_net_edvs_schedule_has_bond_no_wdg 
            WHERE             
                 vna_id = in_vna_id AND     /* vna filter   */
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu 
                 AND bond_valid_phy_num > 0 
                 AND is_support_vxlan >= in_is_vxlan 
				 AND is_sdn = in_is_sdn;					 
        ELSE
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,in_is_sdn , in_is_vxlan ,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 bond_free_sriovvf_num,bond_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,bond_total_phy_num,bond_valid_phy_num,phy_port_uuid,phy_port_free_sriovvf_num
            FROM v_net_edvs_schedule_has_bond_no_wdg 
            WHERE             
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu 
                 AND bond_valid_phy_num > 0 
				 AND is_support_vxlan >= in_is_vxlan 
				 AND is_sdn = in_is_sdn;	
        END IF;
    END IF;    
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
        
                           

-- 调度模块 获取 edvs no bond has wdg 
CREATE OR REPLACE FUNCTION pf_net_edvs_schedule_no_bond_has_wdg(in_netplane_id numeric(19,0), in_netplane_uuid character varying, 
                           in_is_sriov integer, in_is_loop integer, in_nic_num integer, in_is_bond_nic integer,
                           in_mtu integer , in_is_watchdog integer,
                           in_cluster_id numeric(19,0), in_cluster_name character varying, 
                           in_vna_id numeric(19,0),
						   in_is_sdn integer, in_is_vxlan integer)                               
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    check_ret integer;
    netplaneid numeric(19,0);
    
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    /* input param valid: in_netplane_id, in_nic_num, in_mtu, in_is_loop          
     switch chioce  */ 
    /*
       edvs physical, switch_sriov_free_num 存放的是 phy口剩余可用VF数目
    */
    
    IF in_cluster_id > 0 THEN 
        IF in_vna_id > 0 THEN         
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,in_is_sdn , in_is_vxlan ,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 port_free_sriovvf_num,port_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num,  */
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_edvs_schedule_no_bond_has_wdg 
            WHERE 
                 cluster_name = in_cluster_name AND vna_id = in_vna_id AND 
                 -- cluster_id = in_cluster_id AND vna_id = in_vna_id AND     -- cluster vna filter
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu 
                 AND is_support_vxlan >= in_is_vxlan 
				 AND is_sdn = in_is_sdn;				 
        ELSE
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,in_is_sdn , in_is_vxlan ,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 port_free_sriovvf_num,port_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_edvs_schedule_no_bond_has_wdg 
            WHERE 
                 cluster_name = in_cluster_name AND 
                 -- cluster_id = in_cluster_id AND     -- cluster vna filter
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu 
				 AND is_support_vxlan >= in_is_vxlan 
				 AND is_sdn = in_is_sdn;
        END IF;
    ELSE
        IF in_vna_id > 0 THEN         
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,in_is_sdn , in_is_vxlan ,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 port_free_sriovvf_num,port_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_edvs_schedule_no_bond_has_wdg 
            WHERE 
                 vna_id = in_vna_id AND     -- cluster vna filter
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu 
                 AND is_support_vxlan >= in_is_vxlan 
				 AND is_sdn = in_is_sdn;				 
        ELSE
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,in_is_sdn , in_is_vxlan ,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 port_free_sriovvf_num,port_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,null,null,null,null
            FROM v_net_edvs_schedule_no_bond_has_wdg 
            WHERE 
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu 
				 AND is_support_vxlan >= in_is_vxlan 
				 AND is_sdn = in_is_sdn;
        END IF;
    END IF;    
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
            

-- 调度模块 获取 edvs has bond has wdg 
CREATE OR REPLACE FUNCTION pf_net_edvs_schedule_has_bond_has_wdg(in_netplane_id numeric(19,0), in_netplane_uuid character varying, 
                           in_is_sriov integer, in_is_loop integer, in_nic_num integer, in_is_bond_nic integer,
                           in_mtu integer , in_is_watchdog integer,
                           in_cluster_id numeric(19,0), in_cluster_name character varying, 
                           in_vna_id numeric(19,0),
						   in_is_sdn integer, in_is_vxlan integer)                               
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    check_ret integer;
    netplaneid numeric(19,0);
    
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    /* input param valid: in_netplane_id, in_nic_num, in_mtu, in_is_loop          
     switch chioce     */
    /*
       edvs bond, switch_sriov_free_num 存放的是 bond口剩余可用VF数目
    */
         
    IF in_cluster_id > 0 THEN 
        IF in_vna_id > 0 THEN     
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,in_is_sdn , in_is_vxlan ,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 bond_free_sriovvf_num,bond_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,bond_total_phy_num,bond_valid_phy_num,phy_port_uuid,phy_port_free_sriovvf_num
            FROM v_net_edvs_schedule_has_bond_has_wdg 
            WHERE 
                 cluster_name = in_cluster_name AND vna_id = in_vna_id AND 
                 /* cluster_id = in_cluster_id AND vna_id = in_vna_id AND */    /* cluster vna filter */
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu 
                 AND bond_valid_phy_num > 0 
				 AND is_support_vxlan >= in_is_vxlan 
				 AND is_sdn = in_is_sdn;
        ELSE
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,in_is_sdn , in_is_vxlan ,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 bond_free_sriovvf_num,bond_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,bond_total_phy_num,bond_valid_phy_num,phy_port_uuid,phy_port_free_sriovvf_num
            FROM v_net_edvs_schedule_has_bond_has_wdg 
            WHERE 
                 cluster_name = in_cluster_name AND 
                 /* cluster_id = in_cluster_id AND */    /*cluster vna filter*/
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu 
                 AND bond_valid_phy_num > 0 
				 AND is_support_vxlan >= in_is_vxlan 
				 AND is_sdn = in_is_sdn;
        END IF;
    ELSE
        IF in_vna_id > 0 THEN     
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,in_is_sdn , in_is_vxlan ,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 bond_free_sriovvf_num,bond_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,bond_total_phy_num,bond_valid_phy_num,phy_port_uuid,phy_port_free_sriovvf_num
            FROM v_net_edvs_schedule_has_bond_has_wdg 
            WHERE 
                 vna_id = in_vna_id AND     /* cluster vna filter*/
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu 
                 AND bond_valid_phy_num > 0 
				 AND is_support_vxlan >= in_is_vxlan 
				 AND is_sdn = in_is_sdn;
        ELSE
            OPEN ref FOR 
            SELECT  
                 in_netplane_uuid,in_is_sriov,in_is_loop,in_nic_num,in_is_bond_nic,in_mtu,in_is_watchdog,in_is_sdn , in_is_vxlan ,
                 cluster_name,vna_uuid,module_name, switch_uuid, 
                 bond_free_sriovvf_num,bond_free_sriovvf_num,
                 /* switch_sriov_free_num, switch_free_num, */
                 pg_mtu,  port_uuid,
                 port_type,bond_total_phy_num,bond_valid_phy_num,phy_port_uuid,phy_port_free_sriovvf_num
            FROM v_net_edvs_schedule_has_bond_has_wdg 
            WHERE 
                 netplane_id = in_netplane_id AND port_is_loop = in_is_loop  
                 AND switch_free_num >= in_nic_num 
                 AND pg_mtu >= in_mtu 
                 AND bond_valid_phy_num > 0 
				 AND is_support_vxlan >= in_is_vxlan 
				 AND is_sdn = in_is_sdn;
        END IF;
    END IF;    
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
          

		  


CREATE OR REPLACE FUNCTION pf_net_check_add_segmentmap(in_project_id numeric, in_pg_uuid character varying, in_segment_num integer, in_isolate_no integer)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  isolateno integer; 
  pgid numeric(19,0);
  segmentid numeric(19,0);
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE integer := 13;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- pg uuid  
  -- vm pg, 必须是vm_pg private 
  SELECT count(*) INTO is_exist FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_pg_uuid AND a.pg_type = 21;
  SELECT a.id INTO pgid FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_pg_uuid AND a.pg_type = 21;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
/*
  -- 查看隔离域是否一致 
  SELECT count(*) INTO is_exist FROM pg_switchport_access_isolate WHERE pg_id = pgid; 
  SELECT isolate_no INTO isolateno FROM pg_switchport_access_isolate WHERE pg_id = pgid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;

  IF isolateno = in_isolate_no THEN 
     result := 0;
  ELSE 
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;
*/

  -- VLAN RANGE (1,16000000)
  IF in_segment_num < 1 THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  IF in_segment_num > 16000000 THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
          
  -- 通过pd 获取pg的netplane id
  SELECT count(*) into is_exist FROM v_net_netplane_vmpg WHERE pg_uuid = in_pg_uuid;
  SELECT netplane_id into netplaneid FROM v_net_netplane_vmpg WHERE pg_uuid = in_pg_uuid;  
  IF is_exist <= 0 THEN 
     result := RTN_PG_MUST_CFG_NETPLANE; 
     return result;
  END IF;
      
  -- segment pool 
  SELECT count(*) INTO is_exist FROM segment_pool WHERE netplane_id = netplaneid  AND segment_num = in_segment_num ;
  SELECT id INTO segmentid FROM segment_pool WHERE netplane_id = netplaneid  AND segment_num = in_segment_num ;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
    
  -- segment map 是否已存在project_id, pg_id, isolate_no 
  SELECT count(*) INTO is_exist FROM segment_map WHERE project_id = in_project_id AND vm_pg_id = pgid AND isolate_no = in_isolate_no;
  IF is_exist > 0 THEN 
      SELECT count(*) INTO is_exist FROM segment_map WHERE project_id = in_project_id AND vm_pg_id = pgid AND isolate_no = in_isolate_no AND segment_id = segmentid;
      IF is_exist > 0 THEN 
          result := RTN_HAS_SAME_ITEM; 
          return result;
      ELSE          
          result := RTN_ITEM_CONFLICT; 
          return result;
      END IF;
  END IF;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;

  
  
-- Function: pf_net_add_segmentmap(numeric, character varying, integer, integer)

-- DROP FUNCTION pf_net_add_segmentmap(numeric, character varying, integer, integer);

CREATE OR REPLACE FUNCTION pf_net_add_segmentmap(in_project_id numeric, in_pg_uuid character varying, in_segment_num integer, in_isolate_no integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  pgid numeric(19,0);
  segmentid numeric(19,0);
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_add_segmentmap(in_project_id,in_pg_uuid,in_segment_num,in_isolate_no);  
  -- 重复添加 直接返回
  IF result.ret = RTN_HAS_SAME_ITEM THEN      
     result.ret := 0;
     return result;
  END IF;
  IF result.ret > 0 THEN      
     return result;
  END IF;
  
  -- pg uuid  
  SELECT count(*) INTO is_exist FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_pg_uuid;
  SELECT a.id INTO pgid FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_pg_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
  
  -- 通过pd 获取pg的netplane id
  SELECT count(*) into is_exist FROM v_net_netplane_vmpg WHERE pg_uuid = in_pg_uuid;
  SELECT netplane_id into netplaneid FROM v_net_netplane_vmpg WHERE pg_uuid = in_pg_uuid;  
  IF is_exist <= 0 THEN 
     result := RTN_PG_MUST_CFG_NETPLANE; 
     return result;
  END IF;
  
  -- segment pool 
  SELECT count(*) INTO is_exist FROM segment_pool WHERE netplane_id = netplaneid  AND segment_num = in_segment_num ;
  SELECT id INTO segmentid FROM segment_pool WHERE netplane_id = netplaneid  AND segment_num = in_segment_num ;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
  

  -- insert item 
  result.id := nextval('serial_segment_map');
  --result.uuid := in_uuid;
  BEGIN 
    INSERT INTO segment_map(id,project_id,vm_pg_id,segment_id,isolate_no) 
            values(result.id,in_project_id,pgid,segmentid,in_isolate_no);
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
  
  


-- Function: pf_net_check_del_segmentmap(numeric, character varying, integer, integer)

-- DROP FUNCTION pf_net_check_del_segmentmap(numeric, character varying, integer, integer);

CREATE OR REPLACE FUNCTION pf_net_check_del_segmentmap(in_project_id numeric, in_pg_uuid character varying, in_segment_num integer, in_isolate_no integer)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  isolateno integer; 
  switchportmode integer; 
  pgid numeric(19,0);
  segmentid numeric(19,0);
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
  RTN_PG_MUST_CFG_NETPLANE integer := 13;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- pg uuid  
  -- vm pg, 必须是vm_pg private 
  SELECT count(*) INTO is_exist FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_pg_uuid AND a.pg_type = 21;
  SELECT a.id INTO pgid FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_pg_uuid AND a.pg_type = 21;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   

  /*
  -- 查看隔离域是否一致 
  SELECT count(*) INTO is_exist FROM pg_switchport_access_isolate WHERE pg_id = pgid; 
  SELECT isolate_no INTO isolateno FROM pg_switchport_access_isolate WHERE pg_id = pgid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
  
  IF isolateno = in_isolate_no THEN 
     result := 0;
  ELSE 
     result := RTN_PARAM_INVALID; 
     return result;
  END IF;
  */

  
  -- SEGMENT RANGE (1,16000000)
  IF in_segment_num < 1 THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  IF in_segment_num > 16000000 THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
          
  -- 通过pd 获取pg的netplane id
  SELECT count(*) into is_exist FROM v_net_netplane_vmpg WHERE pg_uuid = in_pg_uuid;
  SELECT netplane_id into netplaneid FROM v_net_netplane_vmpg WHERE pg_uuid = in_pg_uuid;  
  IF is_exist <= 0 THEN 
     result := RTN_PG_MUST_CFG_NETPLANE; 
     return result;
  END IF;
  
      
  -- segment pool 
  SELECT count(*) INTO is_exist FROM segment_pool WHERE netplane_id = netplaneid  AND segment_num = in_segment_num ;
  SELECT id INTO segmentid FROM segment_pool WHERE netplane_id = netplaneid  AND segment_num = in_segment_num ;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
    
  -- segment map 是否已存在project_id,pg_id 条目  
  SELECT count(*) INTO is_exist FROM segment_map WHERE project_id = in_project_id  
                AND vm_pg_id = pgid AND segment_id = segmentid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
  
  -- 判断project id, vm_pg_id 是否还在使用  (vnic, vsi_profile)
  SELECT count(*) INTO is_exist FROM  vnic a, vsi_profile b, vm c WHERE 
          c.vm_id = a.vm_id AND a.vsi_profile_id = b.id 
         AND c.project_id = in_project_id AND vm_pg_id = pgid;
  IF is_exist > 0 THEN 
      result := RTN_RESOURCE_USING; 
      return result;
  END IF;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  
  

CREATE OR REPLACE FUNCTION pf_net_del_segmentmap(in_project_id numeric, in_pg_uuid character varying, in_segment_num integer, in_isolate_no integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  pgid numeric(19,0);
  segmentid numeric(19,0);
  netplaneid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- check del 
  result.ret := pf_net_check_del_segmentmap(in_project_id,in_pg_uuid,in_segment_num,in_isolate_no);  
  IF result.ret > 0 THEN      
     return result;
  END IF;
  
  -- pg uuid  
  SELECT count(*) INTO is_exist FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_pg_uuid;
  SELECT a.id INTO pgid FROM port_group a, vm_pg b WHERE a.id = b.pg_id AND a.uuid = in_pg_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
  
  -- 通过pd 获取pg的netplane id
  SELECT count(*) into is_exist FROM v_net_netplane_vmpg WHERE pg_uuid = in_pg_uuid;
  SELECT netplane_id into netplaneid FROM v_net_netplane_vmpg WHERE pg_uuid = in_pg_uuid;  
  IF is_exist <= 0 THEN 
     result := RTN_PG_MUST_CFG_NETPLANE; 
     return result;
  END IF;
  
  -- segment pool 
  SELECT count(*) INTO is_exist FROM segment_pool WHERE netplane_id = netplaneid  AND segment_num = in_segment_num ;
  SELECT id INTO segmentid FROM segment_pool WHERE netplane_id = netplaneid  AND segment_num = in_segment_num ;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;
  
    
  SELECT count(*) INTO is_exist FROM segment_map WHERE project_id = in_project_id  
                AND vm_pg_id = pgid AND segment_id = segmentid;
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;                
  
    
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM segment_map WHERE project_id = in_project_id  
                AND vm_pg_id = pgid AND segment_id = segmentid;
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
  
  
  
-- Function: pf_net_query_segmentmap_by_netplane(character varying)

-- DROP FUNCTION pf_net_query_segmentmap_by_netplane(character varying);

CREATE OR REPLACE FUNCTION pf_net_query_segmentmap_by_netplane(in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    
    OPEN ref FOR SELECT netplane_uuid,netplane_name,pg_uuid,pg_name,project_id, segment_num ,isolate_no
       FROM v_net_segmentmap_netplane_pg_segment   
       WHERE  netplane_uuid = in_uuid;
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  

  -- Function: pf_net_query_segmentmap_by_netplane_and_project_and_isolate(character varying, numeric, integer)

-- DROP FUNCTION pf_net_query_segmentmap_by_netplane_and_project_and_isolate(character varying, numeric, integer);

CREATE OR REPLACE FUNCTION pf_net_query_segmentmap_by_netplane_and_project_and_isolate(in_np_uuid character varying, in_project_id numeric, in_isolate_no integer)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    -- RAISE WARNING  ' retchar --2222'; 
    OPEN ref FOR SELECT netplane_uuid,netplane_name,pg_uuid,pg_name,project_id, segment_num ,isolate_no
       FROM v_net_segmentmap_netplane_pg_segment   
       WHERE  netplane_uuid = in_np_uuid AND 
	         project_id = in_project_id AND 
			 isolate_no = in_isolate_no;
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  
  
  -- Function: pf_net_query_segmentmap_by_portgroup(character varying)

-- DROP FUNCTION pf_net_query_segmentmap_by_portgroup(character varying);

CREATE OR REPLACE FUNCTION pf_net_query_segmentmap_by_portgroup(in_uuid character varying)
  RETURNS refcursor AS
$BODY$ 
DECLARE
    ref refcursor;
    is_exist integer; 
    netplaneid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    
    OPEN ref FOR SELECT netplane_uuid,netplane_name,pg_uuid,pg_name,project_id, segment_num ,isolate_no
       FROM v_net_segmentmap_netplane_pg_segment   
       WHERE pg_uuid = in_uuid;
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


  -- Function: pf_net_query_segmentmap_by_projectid(numeric)

-- DROP FUNCTION pf_net_query_segmentmap_by_projectid(numeric);

CREATE OR REPLACE FUNCTION pf_net_query_segmentmap_by_projectid(in_project_id numeric)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    netplaneid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN  
    OPEN ref FOR SELECT netplane_uuid,netplane_name,pg_uuid,pg_name,project_id, segment_num,isolate_no
       FROM v_net_segmentmap_netplane_pg_segment   
       WHERE project_id = in_project_id;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- Function: pf_net_query_segmentmap_by_projectid_and_isolate(numeric, integer)

-- DROP FUNCTION pf_net_query_segmentmap_by_projectid_and_isolate(numeric, integer);

CREATE OR REPLACE FUNCTION pf_net_query_segmentmap_by_projectid_and_isolate(in_project_id numeric, in_isolate_no integer)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    
    OPEN ref FOR SELECT netplane_uuid,netplane_name,pg_uuid,pg_name,project_id, segment_num ,isolate_no
       FROM v_net_segmentmap_netplane_pg_segment   
       WHERE project_id = in_project_id AND isolate_no = in_isolate_no;
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  
-- Function: pf_net_query_segmentmap_by_projectid_and_pg(numeric, character varying)

-- DROP FUNCTION pf_net_query_segmentmap_by_projectid_and_pg(numeric, character varying);

CREATE OR REPLACE FUNCTION pf_net_query_segmentmap_by_projectid_and_pg(in_project_id numeric, in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
    
    OPEN ref FOR SELECT netplane_uuid,netplane_name,pg_uuid,pg_name,project_id, segment_num ,isolate_no
       FROM v_net_segmentmap_netplane_pg_segment   
       WHERE project_id = in_project_id AND pg_uuid = in_uuid;
    return ref;
END;$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  

-- Function: pf_net_check_add_netplane_segmentpool(character varying, integer)

-- DROP FUNCTION pf_net_check_add_netplane_segmentpool(character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_check_add_netplane_segmentpool(in_uuid character varying, in_segment_num integer)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
  -- RAISE WARNING  ' retchar --';
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
  
  -- VLAN RANGE (2,4094)
  IF in_segment_num < 1 THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  IF in_segment_num > 16000000 THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
    
  -- segment range 必须已经配置
  SELECT count(*) INTO is_exist FROM segment_range WHERE netplane_id = netplaneid  
       AND ( in_segment_num >= segment_begin AND in_segment_num <= segment_end ) ;  
  IF is_exist <= 0 THEN 
     result := RTN_MUST_CFG_VALID_RANGE; 
     return result;
  END IF;  
  
  -- segment pool 不能冲突
  SELECT count(*) INTO is_exist FROM segment_pool WHERE netplane_id = netplaneid  AND segment_num = in_segment_num ;
  IF is_exist > 0 THEN 
     result := RTN_ITEM_CONFLICT; 
     return result;
  END IF;
  
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  
  
-- Function: pf_net_add_netplane_segmentpool(character varying, integer)

-- DROP FUNCTION pf_net_add_netplane_segmentpool(character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_add_netplane_segmentpool(in_uuid character varying, in_segment_num integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  result.ret := pf_net_check_add_netplane_segmentpool(in_uuid,in_segment_num);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
   -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
    
  -- 不支持重复添加 

  -- insert item 
  result.id := nextval('serial_segment_pool');
  --result.uuid := in_uuid;
  BEGIN 
    INSERT INTO segment_pool(id,netplane_id,segment_num) 
            values(result.id,netplaneid,in_segment_num);
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
  


-- Function: pf_net_check_del_netplane_segmentpool(character varying, integer)

-- DROP FUNCTION pf_net_check_del_netplane_segmentpool(character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_check_del_netplane_segmentpool(in_uuid character varying, in_segment_num integer)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  segmentpoolid numeric(19,0);   
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
  RTN_PARAM_INVALID integer := 7;
  RTN_MUST_CFG_VALID_RANGE   integer := 14;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;   
  
  -- SEGMENT RANGE (1,16000000)
  IF in_segment_num < 1 THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  IF in_segment_num > 16000000 THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
    
  SELECT count(*) INTO is_exist FROM segment_pool WHERE netplane_id = netplaneid 
                AND segment_num = in_segment_num;
  SELECT id INTO segmentpoolid FROM segment_pool WHERE netplane_id = netplaneid 
                AND segment_num = in_segment_num;            
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;                
  
  -- 查看该资源是否在使用
  SELECT count(*) INTO is_exist FROM segment_map WHERE segment_id = segmentpoolid;
  IF is_exist > 0 THEN 
     result := RTN_RESOURCE_USING; 
     return result;
  END IF;
      
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  

  -- Function: pf_net_del_netplane_segmentpool(character varying, integer)

-- DROP FUNCTION pf_net_del_netplane_segmentpool(character varying, integer);

CREATE OR REPLACE FUNCTION pf_net_del_netplane_segmentpool(in_uuid character varying, in_segment_num integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  netplaneid numeric(19,0);
  segmentpoolid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- del 不作检查，由应用保准  
  result.ret := pf_net_check_del_netplane_segmentpool(in_uuid,in_segment_num);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
   -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
  
    
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM segment_pool WHERE netplane_id = netplaneid 
                AND segment_num = in_segment_num;
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
  
-- Function: pf_net_query_netplane_segmentpool(character varying)

-- DROP FUNCTION pf_net_query_netplane_segmentpool(character varying);

CREATE OR REPLACE FUNCTION pf_net_query_netplane_segmentpool(in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    netplaneid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN
     -- netplane uuid 
    SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
    SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
       --result := RTN_ITEM_NO_EXIST; 
       return ref;
    END IF;  
    
    SELECT count(*) INTO is_exist FROM segment_pool  
      WHERE netplane_id = netplaneid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF; 
  
    OPEN ref FOR SELECT in_uuid, segment_num 
      FROM segment_pool  
      WHERE netplane_id = netplaneid;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  


-- Function: pf_net_query_vm_vxlan_share_pg_by_netplane(character varying)

-- DROP FUNCTION pf_net_query_vm_vxlan_share_pg_by_netplane(character varying);

CREATE OR REPLACE FUNCTION pf_net_query_vm_vxlan_share_pg_by_netplane(in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    netplaneid numeric(19,0);
BEGIN
     -- netplane uuid 
     RAISE WARNING 'pf_net_query_vm_vxlan_share_pg_by_netplane';
    SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
    SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
       --result := RTN_ITEM_NO_EXIST; 
       return ref;
    END IF;   
    
    OPEN ref FOR 
    SELECT segment_num
    FROM port_group a,vm_pg_share_vxlan_segment b 
    WHERE a.id = b.vmpg_id AND a.pg_type = 20 AND a.netplane_id = netplaneid;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  
-- Function: pf_net_query_vm_vxlan_private_pg_isolate_no_list_by_netplane(character varying)

-- DROP FUNCTION pf_net_query_vm_vxlan_private_pg_isolate_no_list_by_netplane(character varying);

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
    FROM port_group a, segment_map b 
    WHERE a.id = b.vm_pg_id AND a.pg_type = 21 AND a.netplane_id = netplaneid;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  
  

  


CREATE OR REPLACE FUNCTION pf_net_query_segmentid_share_by_vna_np_seg(in_vna_uuid character varying,in_np_uuid character varying, in_seg_num integer )
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    netplaneid numeric(19,0);
	vnaid numeric(19,0);
BEGIN
     -- netplane uuid 
	IF in_np_uuid <> '' THEN 
       SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_np_uuid;
       SELECT id INTO netplaneid FROM netplane WHERE uuid = in_np_uuid;
       IF is_exist <= 0 THEN 
          --result := RTN_ITEM_NO_EXIST; 
          return ref;
       END IF;   
	END IF;
	
	IF in_vna_uuid <> '' THEN
	    SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
        SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
        IF is_exist <= 0 THEN 
           --result := RTN_ITEM_NO_EXIST; 
           return ref;
        END IF;  
	END IF;
    
	-- np 
	IF in_np_uuid <> '' THEN 
	    IF in_vna_uuid <> '' THEN
	        IF in_seg_num = -1 THEN 
			    OPEN ref FOR 
                SELECT vna_uuid, netplane_uuid, segment_num, ip,mask
                FROM v_net_vna_vtep_switch_pg_share_segment
                WHERE vna_id = vnaid AND netplane_id = netplaneid;
			ELSE 
			    OPEN ref FOR 
                SELECT vna_uuid, netplane_uuid, segment_num, ip,mask
                FROM v_net_vna_vtep_switch_pg_share_segment
                WHERE vna_id = vnaid AND netplane_id = netplaneid AND segment_num = in_seg_num;
			END IF;        			
        ELSE
            IF in_seg_num = -1 THEN 
			    OPEN ref FOR 
                SELECT vna_uuid, netplane_uuid, segment_num, ip,mask
                FROM v_net_vna_vtep_switch_pg_share_segment
                WHERE netplane_id = netplaneid;
			ELSE 
			    OPEN ref FOR 
                SELECT vna_uuid, netplane_uuid, segment_num, ip,mask
                FROM v_net_vna_vtep_switch_pg_share_segment
                WHERE netplane_id = netplaneid AND segment_num = in_seg_num;
			END IF;      	
		END IF;
    ELSE 
	    IF in_vna_uuid <> '' THEN
	        IF in_seg_num = -1 THEN 
			    OPEN ref FOR 
                SELECT vna_uuid, netplane_uuid, segment_num, ip,mask
                FROM v_net_vna_vtep_switch_pg_share_segment
                WHERE vna_id = vnaid ;
			ELSE 
			    OPEN ref FOR 
                SELECT vna_uuid, netplane_uuid, segment_num, ip,mask
                FROM v_net_vna_vtep_switch_pg_share_segment
                WHERE vna_id = vnaid AND segment_num = in_seg_num;
			END IF;        			
        ELSE
            IF in_seg_num = -1 THEN 
			    OPEN ref FOR 
                SELECT vna_uuid, netplane_uuid, segment_num, ip,mask
                FROM v_net_vna_vtep_switch_pg_share_segment
                WHERE 1=1;
			ELSE 
			    OPEN ref FOR 
                SELECT vna_uuid, netplane_uuid, segment_num, ip,mask
                FROM v_net_vna_vtep_switch_pg_share_segment
                WHERE segment_num = in_seg_num;
			END IF;      	
		END IF;
	END IF;
	    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  
  
CREATE OR REPLACE FUNCTION pf_net_query_segmentid_private_by_vna_np_seg(in_vna_uuid character varying,in_np_uuid character varying,in_seg_num integer )
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    netplaneid numeric(19,0);
	vnaid numeric(19,0);
BEGIN
     -- netplane uuid 
	IF in_np_uuid <> '' THEN 
       SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_np_uuid;
       SELECT id INTO netplaneid FROM netplane WHERE uuid = in_np_uuid;
       IF is_exist <= 0 THEN 
          --result := RTN_ITEM_NO_EXIST; 
          return ref;
       END IF;   
	END IF;
		
	IF in_vna_uuid <> '' THEN
	    SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
        SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
        IF is_exist <= 0 THEN 
           --result := RTN_ITEM_NO_EXIST; 
           return ref;
        END IF;  
	END IF;
    
	IF in_np_uuid <> '' THEN 
	    IF in_vna_uuid <> '' THEN
	        IF in_seg_num = -1 THEN 			
               OPEN ref FOR 
               SELECT vna_uuid, netplane_uuid, segment_num, ip,mask
               FROM v_net_vna_vtep_switch_pg_private_segment
               WHERE vna_id = vnaid AND netplane_id = netplaneid;
			ELSE 
			    OPEN ref FOR 
               SELECT vna_uuid, netplane_uuid, segment_num, ip,mask
               FROM v_net_vna_vtep_switch_pg_private_segment
               WHERE vna_id = vnaid AND netplane_id = netplaneid AND segment_num = in_seg_num;
			END IF;
        ELSE 
		    IF in_seg_num = -1 THEN 			
               OPEN ref FOR 
               SELECT vna_uuid, netplane_uuid, segment_num, ip,mask
               FROM v_net_vna_vtep_switch_pg_private_segment
               WHERE netplane_id = netplaneid;
			ELSE 
			    OPEN ref FOR 
               SELECT vna_uuid, netplane_uuid, segment_num, ip,mask
               FROM v_net_vna_vtep_switch_pg_private_segment
               WHERE  netplane_id = netplaneid AND segment_num = in_seg_num;
			END IF;
		END IF;
    ELSE  
        IF in_vna_uuid <> '' THEN
	        IF in_seg_num = -1 THEN 			
               OPEN ref FOR 
               SELECT vna_uuid, netplane_uuid, segment_num, ip,mask
               FROM v_net_vna_vtep_switch_pg_private_segment
               WHERE vna_id = vnaid ;
			ELSE 
			    OPEN ref FOR 
               SELECT vna_uuid, netplane_uuid, segment_num, ip,mask
               FROM v_net_vna_vtep_switch_pg_private_segment
               WHERE vna_id = vnaid AND segment_num = in_seg_num;
			END IF;
        ELSE 
		    IF in_seg_num = -1 THEN 			
               OPEN ref FOR 
               SELECT vna_uuid, netplane_uuid, segment_num, ip,mask
               FROM v_net_vna_vtep_switch_pg_private_segment
               WHERE 1=1;
			ELSE 
			    OPEN ref FOR 
               SELECT vna_uuid, netplane_uuid, segment_num, ip,mask
               FROM v_net_vna_vtep_switch_pg_private_segment
               WHERE segment_num = in_seg_num;
			END IF;
		END IF;  	
	END IF;
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;




CREATE OR REPLACE FUNCTION pf_net_check_add_netplane_segmentrange(in_uuid character varying, in_num_begin integer, in_num_end integer)
  RETURNS integer AS
$BODY$
DECLARE
  result  integer;
  is_conflict integer;
  is_exist integer; 
  is_same integer; 
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_PARAM_INVALID integer := 7;
  RTN_SWITCH_MTU_LESS_PG integer := 8;
  RTN_SWITCH_UPLINKPORT_INVALID integer := 9;
  RTN_PORT_HAS_JOIN_OTHER_SWITCH integer := 10;
  RTN_PG_MUST_CFG_NETPLANE   integer := 13;
BEGIN
  result := 0;
  -- 入参检查 后面做
      
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- begin end 
  IF in_num_begin > in_num_end THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  
  -- SEGMENT RANGE (1,16000000)
  IF in_num_begin < 1 THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  IF in_num_end > 16000000 THEN 
      result := RTN_PARAM_INVALID; 
      return result;
  END IF;
  
  -- ip begin end 互相不能重叠  
/*  
  SELECT count(*) INTO is_exist FROM vlan_range WHERE  (( in_num_begin >= vlan_begin AND in_num_begin <= vlan_end ) OR 
           ( in_num_end >= vlan_begin AND in_num_end <= vlan_end ) OR 
           ( in_num_begin <= vlan_begin AND in_num_end >= vlan_end ) ) AND netplane_id = netplaneid;  
  IF is_exist > 0 THEN 
     result := RTN_ITEM_CONFLICT; 
     return result;
  END IF;  
*/
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- Function: pf_net_add_netplane_segmentrange(character varying, integer, integer)

-- DROP FUNCTION pf_net_add_netplane_segmentrange(character varying, integer, integer);

CREATE OR REPLACE FUNCTION pf_net_add_netplane_segmentrange(in_uuid character varying, in_num_begin integer, in_num_end integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_exist integer;
  is_conflict integer;
  netplaneid numeric(19,0);
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  RAISE WARNING 'pf_net_add_netplane_segmentrange';
  result.ret := pf_net_check_add_netplane_segmentrange(in_uuid,in_num_begin,in_num_end);
  IF result.ret > 0 THEN 
     return result;
  END IF;
  
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- 不支持重复添加 

  -- insert item 
  result.id := nextval('serial_segment_range');
  --result.uuid := in_uuid;
  BEGIN 
    INSERT INTO segment_range(id,netplane_id,segment_begin,segment_end) 
            values(result.id,netplaneid,in_num_begin,in_num_end);
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
  


CREATE OR REPLACE FUNCTION pf_net_del_netplane_segmentrange(in_uuid character varying, in_num_begin integer, in_num_end integer)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  netplaneid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result.ret  := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- begin end 
  IF in_num_begin > in_num_end THEN 
      result.ret  := RTN_PARAM_INVALID; 
      return result;
  END IF;
  
  -- del 不作检查，由应用保准
      
  SELECT count(*) INTO is_exist FROM segment_range WHERE netplane_id = netplaneid 
                AND segment_begin = in_num_begin AND segment_end = in_num_end ;
  IF is_exist <= 0 THEN 
     result.ret := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;                
    
  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM segment_range WHERE netplane_id = netplaneid 
                AND segment_begin = in_num_begin AND segment_end = in_num_end ;
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
  

-- Function: pf_net_query_netplane_segmentrange_alloc(character varying, integer, integer)

-- DROP FUNCTION pf_net_query_netplane_segmentrange_alloc(character varying, integer, integer);

CREATE OR REPLACE FUNCTION pf_net_query_netplane_segmentrange_alloc(in_uuid character varying, in_num_begin integer, in_num_end integer)
  RETURNS t_query_total AS
$BODY$
DECLARE
  result  t_query_total;
  is_same integer;
  is_exist integer;
  netplaneid numeric(19,0);
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- netplane uuid 
  SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
  SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
  IF is_exist <= 0 THEN 
     result.ret  := RTN_ITEM_NO_EXIST; 
     return result;
  END IF;  
    
  -- begin end 
  IF in_num_begin > in_num_end THEN 
      result.ret  := RTN_PARAM_INVALID; 
      return result;
  END IF;
        
  SELECT count(*) INTO result.total_num FROM segment_pool WHERE netplane_id = netplaneid 
                AND (segment_num >= in_num_begin AND segment_num <= in_num_end );
    
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  

-- Function: pf_net_query_netplane_segmentrange(character varying)

-- DROP FUNCTION pf_net_query_netplane_segmentrange(character varying);

CREATE OR REPLACE FUNCTION pf_net_query_netplane_segmentrange(in_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
    ref refcursor;
    is_exist integer; 
    netplaneid numeric(19,0);
    RTN_ITEM_NO_EXIST integer := 4;
BEGIN

    -- logicnetwork uuid 
    RAISE WARNING 'pf_net_query_netplane_segmentrange';
    SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid;
    SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF;  
    
    SELECT count(*) INTO is_exist FROM segment_range  
      WHERE netplane_id = netplaneid;
    IF is_exist <= 0 THEN 
         --result := RTN_ITEM_NO_EXIST; 
         return ref;
    END IF; 
  
    OPEN ref FOR SELECT in_uuid, segment_begin,segment_end 
      FROM segment_range  
      WHERE netplane_id = netplaneid;
    
    return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  
  

CREATE OR REPLACE FUNCTION pf_net_query_netplane_vmshare_segments(in_uuid character varying)
  RETURNS character varying AS
$BODY$ 
DECLARE is_exist integer; 
    retchar character varying;    
    tmpchar character varying;
    tmpcharB character varying;
    tmpcharE character varying;
    netplaneid numeric(19,0);
    msegmentrangelists RECORD;
    i integer := 0;
    aindex integer := 0; 
    index integer := 1;
    segmentpre integer := 1;
    segmentbegin integer := 1;
BEGIN retchar := '';
    /* netplane id*/
    SELECT count(*) INTO is_exist FROM netplane WHERE uuid = in_uuid; 
   SELECT id INTO netplaneid FROM netplane WHERE uuid = in_uuid; 
   IF is_exist <= 0 THEN 
       return retchar; 
   END IF; 
   i := 1;        

   FOR msegmentrangelists IN SELECT segment_begin,segment_end  FROM segment_range 
        WHERE netplane_id = netplaneid ORDER BY segment_begin LOOP
        
       segmentbegin = msegmentrangelists.segment_begin;
       
       /* 第一条记录 */
       IF segmentpre <= 1 THEN 
           IF segmentpre < segmentbegin THEN 
              IF segmentpre < (segmentbegin-1) THEN 
                  tmpcharB := pf_net_integer_to_char(1);
                  tmpcharE := pf_net_integer_to_char(segmentbegin-1);
                  retchar = retchar || tmpcharB || ' ~' || tmpcharE || ',' ;
              END IF;
              IF segmentpre = (segmentbegin-1) THEN 
                  tmpcharB := pf_net_integer_to_char(1);
                  retchar = retchar || tmpcharB || ',' ;
              END IF;
           END IF;
       END IF;
       IF segmentpre > 1 THEN 
           IF segmentpre < segmentbegin THEN 
              IF segmentpre < (segmentbegin-2) THEN 
                  tmpcharB := pf_net_integer_to_char(segmentpre+1);
                  tmpcharE := pf_net_integer_to_char(segmentbegin-1);
                  retchar = retchar || tmpcharB || ' ~' || tmpcharE || ',' ;
              END IF;
              IF segmentpre = (segmentbegin-2) THEN 
                  tmpcharB := pf_net_integer_to_char(segmentpre+1);
                  retchar = retchar || tmpcharB || ',' ;
              END IF;
           END IF; 
       END IF;

       segmentpre = msegmentrangelists.segment_end;
   END LOOP;

   /*判断segment range end 是否是16000000*/
   /* 当没有配置segment range 的时候，需考虑边界值1 */
   IF segmentpre = 1 THEN 
       segmentpre = 0;
   END IF;

   IF segmentpre < 16000000 THEN 
       IF segmentpre < (16000000-1) THEN 
           tmpcharB := pf_net_integer_to_char(segmentpre+1);
           tmpcharE := to_char((16000000),'99999999');
           retchar = retchar || tmpcharB || ' ~' || tmpcharE  ;
       END IF;
       IF segmentpre = (16000000-1) THEN 
           tmpcharB := pf_net_integer_to_char(segmentpre+1);
           retchar = retchar || tmpcharB  ;
       END IF;
   END IF;
        
   /* RAISE WARNING  ' retchar --> %', retchar; */
   return retchar;
END; 
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
  
  


CREATE OR REPLACE FUNCTION pf_net_add_vtepport(in_vna_uuid character varying, in_type integer,in_name character varying,
        in_uuid character varying, in_state integer, in_is_broadcast integer,in_is_running integer,in_is_multicast integer,
        in_promiscuous integer, in_mtu integer, in_is_master integer, in_admin_state integer, in_is_online integer,
        in_is_port_cfg integer, in_is_consistency integer, 
        in_vswitch_uuid character varying,
		in_num_ip numeric(19),in_num_mask numeric(19),
        in_str_ip character varying,in_str_mask character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_same_port integer;
  is_same_port_ex integer;
  is_same_port_sriov integer;
  is_conflict integer;
  vnaid numeric(19,0);
  vswitchid numeric(19,0);
  ipid numeric(19,0);
  portid numeric(19,0);
  kernelpgMtu integer;
  uplinkpgMtu integer;
  is_exist integer; 
  db_uuid character varying;
  -- 错误返回码定义
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_UPLINKPGMTU_LESS_KERNELPG integer := 18;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  
  -- 检查vna uuid 是否有效
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  /* switch */ 
  SELECT count(*) INTO is_exist FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type  
           AND a.uuid = in_vswitch_uuid;
  SELECT a.id INTO vswitchid FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type  
           AND a.uuid = in_vswitch_uuid;
  IF is_exist <= 0 THEN 
        result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
        return result;
  END IF;

  
  /* in_ip */ 
  SELECT count(*) INTO is_exist FROM ip_pool WHERE ip_num = in_num_ip AND mask_num = in_num_mask;
  SELECT id INTO ipid FROM ip_pool WHERE ip_num = in_num_ip AND mask_num = in_num_mask;
  IF is_exist <= 0 THEN 
        result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
        return result;
  END IF;
  
  /*ip 不能已经被占用*/
  SELECT count(*) INTO is_exist FROM vtep_port WHERE ip_pool_id = ipid;
  IF is_exist > 0 THEN 
        result.ret := RTN_ITEM_CONFLICT;   
        return result;
  END IF;
  
  /* port name 名称是否冲突 */
  SELECT count(*) INTO is_conflict FROM  port WHERE vna_id = vnaid  AND name = in_name ;
  IF is_conflict > 0 THEN
     result.ret := RTN_ITEM_CONFLICT; -- conflict 
     return result;
  END IF;
        
  -- insert item
  result.id := nextval('serial_port');
  result.uuid := in_uuid;
  
  BEGIN 
    INSERT INTO port values(result.id,in_type,vnaid,in_name,in_uuid,in_state,in_is_broadcast,in_is_running,in_is_multicast,in_promiscuous,in_mtu,in_is_master,in_admin_state,in_is_online,in_is_port_cfg, in_is_consistency);
    
    INSERT INTO vtep_port(port_id,port_type,vna_id,virtual_switch_id,ip_pool_id) values(result.id,in_type,vnaid,vswitchid,ipid); 
          
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


-- port修改

CREATE OR REPLACE FUNCTION pf_net_modify_vtepport(in_vna_uuid character varying, in_type integer,in_name character varying,
        in_uuid character varying, in_state integer, in_is_broadcast integer,in_is_running integer,in_is_multicast integer,
        in_promiscuous integer, in_mtu integer, in_is_master integer, in_admin_state integer, in_is_online integer,
        in_is_port_cfg integer, in_is_consistency integer, 
        in_vswitch_uuid character varying,
		in_num_ip numeric(19),in_num_mask numeric(19),
        in_str_ip character varying,in_str_mask character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  vnaid numeric(19,0);
  vswitchid numeric(19,0);
  pgid numeric(19,0);
  portid numeric(19,0);
  ipid numeric(19,0);
  kernelpgMtu integer;
  uplinkpgMtu integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_HAS_SAME_ITEM integer := 2;
  RTN_ITEM_CONFLICT integer := 3;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_NETPLANE_MTU_LESS_PG  integer := 5;
  RTN_UPLINKPGMTU_LESS_KERNELPG integer := 18;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做  
  -- 检查vna uuid 是否有效
  SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
  SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;  
  
  /* 检查port uuid 是否有效 */
  SELECT count(*) INTO is_exist FROM port WHERE uuid = in_uuid AND port_type = in_type;
  SELECT id INTO portid FROM port WHERE uuid = in_uuid AND port_type = in_type;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF;   
      
  SELECT count(*) INTO is_exist FROM vtep_port WHERE port_id = portid AND port_type = in_type AND vna_id = vnaid;
  IF is_exist <= 0 THEN 
    result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
    return result;
  END IF; 
    
  /* switch */ 
  SELECT count(*) INTO is_exist FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type  
           AND a.uuid = in_vswitch_uuid;
  SELECT a.id INTO vswitchid FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type  
           AND a.uuid = in_vswitch_uuid;
  IF is_exist <= 0 THEN 
        result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
        return result;
  END IF;

  
  /* in_ip */ 
  SELECT count(*) INTO is_exist FROM ip_pool WHERE ip_num = in_num_ip AND mask_num = in_num_mask;
  SELECT id INTO ipid FROM ip_pool WHERE ip_num = in_num_ip AND mask_num = in_num_mask;
  IF is_exist <= 0 THEN 
        result.ret := RTN_ITEM_NO_EXIST;   -- item has not exist.
        return result;
  END IF;
  
  /*ip 不能已经被占用*/
  SELECT count(*) INTO is_exist FROM vtep_port WHERE ip_pool_id = ipid AND port_id = portid ;
  IF is_exist <= 0 THEN 
       SELECT count(*) INTO is_exist FROM vtep_port WHERE ip_pool_id = ipid;
	   IF is_exist > 0 THEN 
           result.ret := RTN_ITEM_CONFLICT;   
           return result;
	   END IF;
  END IF;
    
  /* port name */
  SELECT count(*) INTO is_exist FROM port WHERE vna_id = vnaid AND name = in_name AND uuid = in_uuid;
  IF is_exist <= 0 THEN
     SELECT count(*) INTO is_exist FROM port WHERE vna_id = vnaid AND name = in_name ;
     IF is_exist > 0 THEN
       result.ret := RTN_ITEM_CONFLICT;-- 
       return result;          
     END IF;
  END IF;  
                                 
  -- modify item
  result.id  := portid;
  result.uuid := in_uuid;
  BEGIN 
    UPDATE port SET name = in_name, state = in_state, is_broadcast =in_is_broadcast, is_multicast = in_is_multicast,
        promiscuous = in_promiscuous, mtu = in_mtu, is_master = in_is_master, admin_state = in_admin_state, is_online = in_is_online ,
        is_cfg = in_is_port_cfg , is_consistency = in_is_consistency 
        WHERE uuid = in_uuid AND port_type = in_type;
		
	UPDATE vtep_port SET virtual_switch_id = vswitchid, ip_pool_id = ipid WHERE port_id = portid;
   
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



-- 删除port

CREATE OR REPLACE FUNCTION pf_net_del_vtepport(in_port_uuid character varying)
  RETURNS t_operate_result AS
$BODY$
DECLARE
  result  t_operate_result;
  is_same integer;
  is_exist integer;
  RTN_OPERATOR_FAILED integer := 1;
  RTN_ITEM_NO_EXIST integer := 4;
  RTN_RESOURCE_USING integer := 6;
BEGIN
  result.ret := 0;
  -- 入参检查 后面做
  -- 是否存在记录
  SELECT count(*) INTO is_exist FROM port a, vtep_port b WHERE a.vna_id = b.vna_id AND a.id = b.port_id AND a.port_type = b.port_type AND a.uuid = in_port_uuid ;  
  IF is_exist <= 0 THEN
     result.ret := RTN_ITEM_NO_EXIST;-- does not exist item
     return result;
  END IF;

  -- del item
  result.ret := 0;
  BEGIN 
    DELETE FROM port WHERE uuid = in_port_uuid ;
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


--- 查询phy port summary
--- 输出 name,uuid
-- Function: pf_net_query_kernelport_summary()

CREATE OR REPLACE FUNCTION pf_net_query_vtepport_summary(in_vna_uuid character varying)
  RETURNS refcursor AS
$BODY$
DECLARE
  ref refcursor;
  is_same integer;
  is_exist integer;
  vnaid numeric(19,0);
BEGIN
      -- vna 
      SELECT count(*) INTO is_exist FROM vna WHERE vna_uuid = in_vna_uuid;
      SELECT id INTO vnaid FROM vna WHERE vna_uuid = in_vna_uuid;
      IF is_exist <= 0 THEN 
        return ref;
      END IF;
          
      -- 
      OPEN ref FOR SELECT in_vna_uuid, a.uuid, a.name, a.port_type FROM port a, vtep_port b WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
          AND a.vna_id = vnaid  ;      
      return ref;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;


-- 查询port 
-- 输出 uuid,name,description,mtu
CREATE OR REPLACE FUNCTION pf_net_query_vtepport(in_port_uuid character varying)
  RETURNS t_vtepport_query AS
$BODY$
DECLARE
  result  t_vtepport_query;
  is_exist integer;
  vtepportid numeric(19,0);
  vswitchid numeric(19,0);  
  ipid numeric(19,0);
  pgid numeric(19,0);
  RTN_ITEM_NO_EXIST integer :=4;
BEGIN

  result.ret := 0;
  
  SELECT count(*) INTO is_exist FROM port a, vtep_port b WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
           AND a.uuid = in_port_uuid ;
  SELECT a.id INTO vtepportid FROM port a, vtep_port b WHERE a.id = b.port_id AND a.port_type = b.port_type AND a.vna_id = b.vna_id 
           AND a.uuid = in_port_uuid ;
  IF is_exist <= 0 THEN   
     result.ret := RTN_ITEM_NO_EXIST; -- no exist item
     return result; 
  END IF;

   SELECT count(*) INTO is_exist FROM port a,vna b WHERE a.vna_id = b.id AND a.id = vtepportid ;
   SELECT b.vna_uuid INTO result.vna_uuid FROM port a,vna b WHERE a.vna_id = b.id AND a.id = vtepportid ;
   IF is_exist <= 0 THEN   
     result.ret := RTN_ITEM_NO_EXIST; -- no exist item
     return result; 
   END IF;
  
   result.uuid := in_port_uuid;
   -- port 
   SELECT port_type into result.port_type  FROM port WHERE uuid = in_port_uuid;
   SELECT name into result.name  FROM port WHERE uuid = in_port_uuid;
   SELECT state into result.state FROM port WHERE uuid = in_port_uuid;
   SELECT is_broadcast into result.is_broadcast  FROM port WHERE uuid = in_port_uuid;
   SELECT is_running into result.is_running  FROM port WHERE uuid = in_port_uuid;
   SELECT is_multicast into result.is_multicast  FROM port WHERE uuid = in_port_uuid;
   SELECT promiscuous into result.promiscuous  FROM port WHERE uuid = in_port_uuid;
   SELECT mtu into result.mtu  FROM port WHERE uuid = in_port_uuid;
   SELECT is_master into result.is_master  FROM port WHERE uuid = in_port_uuid;
   SELECT admin_state into result.admin_state  FROM port WHERE uuid = in_port_uuid;
   SELECT is_online into result.is_online  FROM port WHERE uuid = in_port_uuid;
   SELECT is_cfg into result.is_port_cfg  FROM port WHERE uuid = in_port_uuid;
   SELECT is_consistency into result.is_consistency  FROM port WHERE uuid = in_port_uuid;
   
   -- vtep port    
   -- vswitch 
   SELECT count(*) INTO is_exist FROM vtep_port WHERE port_id = vtepportid AND virtual_switch_id is not null;
   SELECT virtual_switch_id INTO vswitchid FROM vtep_port WHERE port_id = vtepportid AND virtual_switch_id is not null;
   IF is_exist > 0 THEN 
      SELECT a.uuid INTO result.vswitch_uuid FROM switch a, virtual_switch b WHERE a.id = b.switch_id AND a.switch_type = b.switch_type 
             AND a.id = vswitchid;
   END IF;
   
   /* ip  */
   SELECT count(*) INTO is_exist FROM vtep_port WHERE port_id = vtepportid AND ip_pool_id is not null;
   SELECT ip_pool_id INTO ipid FROM vtep_port WHERE port_id = vtepportid AND ip_pool_id is not null;
   IF is_exist > 0 THEN 
      SELECT ip INTO result.ip FROM ip_pool WHERE id = ipid;
	  SELECT mask INTO result.mask FROM ip_pool WHERE id = ipid;
	  SELECT ip_num INTO result.ip_num FROM ip_pool WHERE id = ipid;
	  SELECT mask_num INTO result.mask_num FROM ip_pool WHERE id = ipid;
   END IF;
   
  return result;
END
$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;



  
  
  







