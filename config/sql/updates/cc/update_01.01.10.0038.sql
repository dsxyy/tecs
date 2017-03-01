------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人:wangmh
--修改时间：2013.6.6
--修改补丁号：0038
--修改说明：host_pool增加字段cpus
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
ALTER TABLE host_pool ADD COLUMN cpus int4 DEFAULT 0 NOT NULL;

UPDATE  host_pool h SET cpus = cpu_info.processor_num From host_cpuinfo cpu_info WHERE cpu_info.cpu_info_id = h.cpu_info_id;

------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0038', 'tecs', now(), 'add column cpus to host_pool');
