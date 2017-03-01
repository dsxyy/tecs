------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人:谢涛涛
--修改时间：2013.7.1
--修改补丁号：0041
--修改说明：创建表host_hwmon
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
CREATE TABLE host_hwmon ( 
	hid                  numeric(19,0) NOT NULL,
	temp1_name           varchar( 128 ),
	temp1_value          varchar( 128 ),
	temp2_name           varchar( 128 ),
	temp2_value          varchar( 128 ),
	temp3_name           varchar( 128 ),
	temp3_value          varchar( 128 ),
	temp4_name           varchar( 128 ),
	temp4_value          varchar( 128 ),
	temp5_name           varchar( 128 ),
	temp5_value          varchar( 128 ),
	temp6_name           varchar( 128 ),
	temp6_value          varchar( 128 ),
	temp7_name           varchar( 128 ),
	temp7_value          varchar( 128 ),
	temp8_name           varchar( 128 ),
	temp8_value          varchar( 128 ),
	cpu0_vid_name        varchar( 128 ),
	cpu0_vid_value       varchar( 128 ),
	cpu1_vid_name        varchar( 128 ),
	cpu1_vid_value       varchar( 128 ),
	fan1_rpm_name        varchar( 128 ),
	fan1_rpm_value       varchar( 128 ),
	fan2_rpm_name        varchar( 128 ),
	fan2_rpm_value       varchar( 128 ),
	fan3_rpm_name        varchar( 128 ),
	fan3_rpm_value       varchar( 128 ),
	fan4_rpm_name        varchar( 128 ),
	fan4_rpm_value       varchar( 128 ),
	fan5_rpm_name        varchar( 128 ),
	fan5_rpm_value       varchar( 128 ),
	fan6_rpm_name        varchar( 128 ),
	fan6_rpm_value       varchar( 128 ),
	fan7_rpm_name        varchar( 128 ),
	fan7_rpm_value       varchar( 128 ),
	fan8_rpm_name        varchar( 128 ),
	fan8_rpm_value       varchar( 128 ),
	in0_name             varchar( 128 ),
	in0_value            varchar( 128 ),
	in1_name             varchar( 128 ),
	in1_value            varchar( 128 ),
	in2_name             varchar( 128 ),
	in2_value            varchar( 128 ),
	in3_name             varchar( 128 ),
	in3_value            varchar( 128 ),
	in4_name             varchar( 128 ),
	in4_value            varchar( 128 ),
	in5_name             varchar( 128 ),
	in5_value            varchar( 128 ),
	in6_name             varchar( 128 ),
	in6_value            varchar( 128 ),
	in7_name             varchar( 128 ),
	in7_value            varchar( 128 ),
	in8_name             varchar( 128 ),
	in8_value            varchar( 128 ),
	in9_name             varchar( 128 ),
	in9_value            varchar( 128 ),
	in10_name            varchar( 128 ),
	in10_value           varchar( 128 ),
	in11_name            varchar( 128 ),
	in11_value           varchar( 128 ),
	in12_name            varchar( 128 ),
	in12_value           varchar( 128 ),
	in13_name            varchar( 128 ),
	in13_value           varchar( 128 ),
	in14_name            varchar( 128 ),
	in14_value           varchar( 128 ),
	CONSTRAINT pk_host_hwmon PRIMARY KEY ( hid ),
	CONSTRAINT fk_atca_host_pool FOREIGN KEY (hid)
      REFERENCES host_pool (oid) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE CASCADE
 );

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
--INSERT INTO _schema_log(version_number, update_by,update_at,description) 
--VALUES ('01.01.10.0004', 'tecs',now(),'first version');
------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0041', 'xiett', now(), 'create new table host_hwmon');
