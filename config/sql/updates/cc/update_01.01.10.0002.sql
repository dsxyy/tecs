------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：龚协丰170393	
--修改时间：2012.8.29
--修改补丁号：0002
--修改内容说明：增加MAC_IP表
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
CREATE TABLE public.mac_ip ( 
	mac                  varchar( 64 ) NOT NULL,
	ip                   varchar( 64 ) NOT NULL,
	CONSTRAINT idx_macip PRIMARY KEY ( mac ),
	CONSTRAINT pk_macip_ip UNIQUE ( ip )
 );
------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0002', 'tecs',now(),'change all_host_images');
------------------------------------------------------------------------------------