------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：张业兵
--修改时间：2012.8.9
--修改补丁号：0002
--修改内容说明：
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

CREATE TABLE cloud_node ( 
	ip_address           varchar( 64 ) NOT NULL,
	is_master            int4 NOT NULL,
	CONSTRAINT pk_cloud_node PRIMARY KEY ( ip_address )
 );

ALTER TABLE cloud_node ADD CONSTRAINT ck_28 CHECK ( is_master = 0 OR is_master = 1 );


CREATE TABLE cluster_node ( 
	cluster_name         varchar( 64 ) NOT NULL,
	ip_address           varchar( 64 ) NOT NULL,
	is_master            int4 NOT NULL,
	CONSTRAINT pk_cluster_node PRIMARY KEY ( cluster_name, ip_address )
 );

ALTER TABLE public.cluster_node ADD CONSTRAINT ck_37 CHECK ( is_master = 0 OR is_master = 1 );

ALTER TABLE public.cluster_node ADD CONSTRAINT fk_cluster_node_cluster_pool FOREIGN KEY ( cluster_name ) REFERENCES cluster_pool( name ) ON DELETE CASCADE;

INSERT INTO cluster_node(cluster_name, ip_address, is_master) select name,ip_address,1 from cluster_pool;

ALTER TABLE cluster_pool DROP COLUMN ip_address;


------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0002', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0002', 'tecs',now(), 'create table cloud_node and cluster_node,delete ip_address from cluster_pool');

