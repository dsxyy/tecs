------------------------------------------------------------------------------------
--数据库schema升级脚本
--修改人: xiett
--修改时间：2013.7.30
--修改补丁号：0045
--修改说明：创建新表 host_statistics, host_statistics_nics
--                   vm_statistics, vm_statistics_nics
------------------------------------------------------------------------------------
--请在此加入tc/cc表结构修改的语句，视图的修改请直接编辑tc/cc_view.sql：
--
CREATE TABLE host_statistics
(
  hid numeric(19,0) NOT NULL,
  statistics_time character varying(32) NOT NULL,
  save_time character varying(32) NOT NULL,
  cpu_used_rate integer NOT NULL,
  mem_used_rate integer NOT NULL,
  CONSTRAINT host_statistics_pkey PRIMARY KEY (hid , statistics_time),
  CONSTRAINT host_statistics_fkey FOREIGN KEY (hid)
      REFERENCES host_pool (oid) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE CASCADE
);

CREATE TABLE host_statistics_nics
(
  hid numeric(19,0) NOT NULL,
  name varchar(32) NOT NULL,
  statistics_time character varying(32) NOT NULL,
  save_time character varying(32) NOT NULL,
  used_rate integer NOT NULL,
  CONSTRAINT host_statistics_nics_pkey PRIMARY KEY (hid, name, statistics_time),
  CONSTRAINT host_statistics_nics_fkey FOREIGN KEY (hid, statistics_time)
      REFERENCES host_statistics (hid, statistics_time)  MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE CASCADE
);

CREATE TABLE vm_statistics
(
  vid numeric(19,0) NOT NULL,
  statistics_time character varying(32) NOT NULL,
  save_time character varying(32) NOT NULL,
  cpu_used_rate integer NOT NULL,
  mem_used_rate integer NOT NULL,
  CONSTRAINT vm_statistics_pkey PRIMARY KEY (vid , statistics_time),
  CONSTRAINT vm_statistics_fkey FOREIGN KEY (vid)
      REFERENCES vm_pool (vid) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE CASCADE
);

CREATE TABLE vm_statistics_nics
(
  vid numeric(19,0) NOT NULL,
  name varchar(32) NOT NULL,
  statistics_time character varying(32) NOT NULL,
  save_time character varying(32) NOT NULL,
  used_rate integer NOT NULL,
  CONSTRAINT vm_statistics_nics_pkey PRIMARY KEY (vid, name, statistics_time),
  CONSTRAINT vm_statistics_nics_fkey FOREIGN KEY (vid, statistics_time)
      REFERENCES vm_statistics (vid, statistics_time)  MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE CASCADE
);

------------------------------------------------------------------------------------
INSERT INTO _schema_log(version_number, update_by, update_at, description) 
VALUES ('01.01.10.0045', 'tecs', now(), 'create new table host_statistics, host_statistics_nics, vm_statistics, vm_statistics_nics');
