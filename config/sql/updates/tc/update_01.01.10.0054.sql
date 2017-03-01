------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人:杜文超
--修改时间：2013.8.26
--修改补丁号：0054
--修改说明：创建affinity_region表、affinity_region_vmcfg表和视图view_affinity_region_with_cluster、
--          view_vmcfg_with_affinity_region、view_affinity_region和view_repel_with_affinity_region
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
CREATE TABLE affinity_region (
    oid         numeric(19,0) NOT NULL,
        name     character varying(32) NOT NULL,
        description  character varying(512),
        create_time  character varying(32) NOT NULL,
        uid         numeric(19,0),
        level       character varying(32) NOT NULL,
        is_forced     integer DEFAULT 0,
        CONSTRAINT affinity_region_pkey PRIMARY KEY (oid),
        CONSTRAINT ck_1 CHECK ( level::text = 'shelf'::text OR level::text = 'rack'::text OR level::text = 'board'::text),
        CONSTRAINT fk_affinity_region_user_pool FOREIGN KEY (uid) REFERENCES user_pool (oid) MATCH SIMPLE ON UPDATE NO ACTION ON DELETE RESTRICT
 );
 
 CREATE TABLE affinity_region_vmcfg (
     affinity_region_id   numeric(19,0) NOT NULL,
         vid                numeric(19,0) NOT NULL,
         CONSTRAINT idx_affinity_region_vmcfg PRIMARY KEY (vid),
         CONSTRAINT fk_affinity_region FOREIGN KEY (affinity_region_id) REFERENCES affinity_region(oid) MATCH SIMPLE ON UPDATE NO ACTION ON DELETE RESTRICT,
         CONSTRAINT fk_vmcfg_pool FOREIGN KEY (vid) REFERENCES vmcfg_pool (oid) MATCH SIMPLE ON UPDATE NO ACTION ON DELETE CASCADE
);


------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0054', 'duwch', now(), 'add affinity_region and affinity_region_vmcfg table and some views');
