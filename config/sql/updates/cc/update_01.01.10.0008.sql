------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：袁进坤
--修改时间：2012.10.22
--修改补丁号：0008
--修改内容说明：netplane_map_public调整为netplane_map_default
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

--将老数据直接拷贝到host_port和host_trunk表中
UPDATE host_port
   SET netplane = netplane_map_public.netplane
   FROM netplane_map_public netplane_map_public
 WHERE host_port.netplane IS NULL AND host_port.name = netplane_map_public.if_name;

UPDATE host_trunk
   SET netplane = netplane_map_public.netplane
   FROM netplane_map_public netplane_map_public
 WHERE host_trunk.netplane IS NULL AND host_trunk.name = netplane_map_public.if_name;
 
--更改表名和主键、外键名称
ALTER TABLE netplane_map_public DROP CONSTRAINT fk_netplane_map_public;
ALTER TABLE netplane_map_public DROP CONSTRAINT idx_netplane_map_public;
ALTER TABLE netplane_map_public ADD CONSTRAINT idx_netplane_map_default PRIMARY KEY (if_name);
ALTER TABLE netplane_map_public ADD CONSTRAINT fk_netplane_map_default FOREIGN KEY ( netplane ) REFERENCES netplane( name ) ON DELETE RESTRICT;
ALTER TABLE netplane_map_public RENAME TO netplane_map_default;

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0008', 'yuanjk',now(),'modify netplane_map_public to netplane_map_default');
------------------------------------------------------------------------------------