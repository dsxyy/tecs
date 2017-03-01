------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：刘毅
--修改时间：2012.11.30
--修改补丁号：0013
--修改内容说明：增加cmm_pool
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
CREATE TABLE cmm_pool ( 
	bureau               int4 NOT NULL,
	rack                 int4 NOT NULL,
	shelf                int4 NOT NULL,
	type                 varchar( 32 ),
	ip_address           varchar( 64 ),
	description          varchar( 512 ),
	CONSTRAINT idx_cmm_pool PRIMARY KEY ( bureau, rack, shelf )
 );

ALTER TABLE public.cmm_pool ADD CONSTRAINT ck_7 CHECK ( bureau > 0 );

ALTER TABLE public.cmm_pool ADD CONSTRAINT ck_8 CHECK ( rack > 0 );

ALTER TABLE public.cmm_pool ADD CONSTRAINT ck_9 CHECK ( shelf > 0 );

--DROP VIEW  host_ctrl_info_view;
DROP TABLE host_ctrl_info;

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0016', 'tecs',now(),'add table cmm_pool');
------------------------------------------------------------------------------------