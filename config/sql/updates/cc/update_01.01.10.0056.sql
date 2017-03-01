------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人：xiett
--修改时间：2013.12.16
--修改补丁号：0056
--修改内容说明：host_cpuinfo表中删除字段bogomips
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--

ALTER TABLE host_cpuinfo DROP CONSTRAINT idx_host_cpuinfo;
ALTER TABLE host_cpuinfo ADD CONSTRAINT idx_host_cpuinfo UNIQUE (processor_num , physical_id_num , cpu_cores , vendor_id , cpu_family , model , model_name , stepping , cache_size , fpu , fpu_exception , cpuid_level , wp , flags );
ALTER TABLE host_cpuinfo DROP COLUMN bogomips;

------------------------------------------------------------------------------------
--所有升级脚本的最后一步都必须是升级Schema版本号，
--version_number字段的值必须和文件名update_*.sql的星号部分完全相同！！！
INSERT INTO _schema_log(version_number, update_by,update_at,description) 
VALUES ('01.01.10.0056', 'xiett',now(),'delete bogomips field of host_cpuinfo table');
------------------------------------------------------------------------------------