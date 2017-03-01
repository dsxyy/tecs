--*********************************************************************
-- 版权所有 (C)2012, 深圳市中兴通讯股份有限公司。
--
-- 文件名称： tc.sql
-- 文件标识：
-- 内容摘要：
-- 其它说明：
-- 当前版本：
-- 作    者： zte
-- 完成日期： 2012-2-8
--
--    修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
--    修改日期：
--    版 本 号：
--    修 改 人：
--    修改内容：
-- 修改记录2：…
-- 修 改 人：
-- 修改日期：
-- 修改内容：
--*********************************************************************
--首先创建的必须是sky模块的公共视图
CREATE VIEW view_workflows AS SELECT w.workflow_id,w.rollback_workflow_id,w.category,w.name,w.application,w.process,w.engine,w.paused,w.create_time,w.pause_time,w.description, w.upstream_sender, w.upstream_action_id,
  l.label_int64_1,l.label_int64_2,l.label_int64_3,l.label_int64_4,l.label_int64_5,l.label_int64_6,l.label_int64_7,l.label_int64_8,l.label_int64_9, 
  l.label_string_1,l.label_string_2,l.label_string_3,l.label_string_4,l.label_string_5,l.label_string_6,l.label_string_7,l.label_string_8,l.label_string_9,w.originator
FROM public.workflow w,public.workflow_labels l
WHERE w.workflow_id = l.workflow_id;

CREATE VIEW view_actions AS SELECT a.action_id,a.workflow_id,a.rollback_action_id,a.category,a.name,a.sender,a.receiver,
a.message_id,a.message_priority,a.message_version,a.message_req,
a.message_ack_id,a.message_ack_priority,a.message_ack_version,a.message_ack,a.success_feedback,a.failure_feedback,a.timeout_feedback,
a.state,a.progress,a.progress_weight,a.timeout,a.history_repeat_count,a.synchronous,a.skipped,a.finished,a.finish_time,a.last_repeat_time,a.create_time,a.description,
  l.label_int64_1,l.label_int64_2,l.label_int64_3,l.label_int64_4,l.label_int64_5,l.label_int64_6,l.label_int64_7,l.label_int64_8,l.label_int64_9, 
  l.label_string_1,l.label_string_2,l.label_string_3,l.label_string_4,l.label_string_5,l.label_string_6,l.label_string_7,l.label_string_8,l.label_string_9
FROM public.action_pool a,public.action_labels l
WHERE a.action_id = l.action_id;

CREATE VIEW workflow_actions AS SELECT a.action_id, a.workflow_id, a.rollback_action_id, 
w.application, w.process, w.engine, w.paused AS workflow_paused, w.create_time AS workflow_create_time, w.pause_time AS workflow_pause_time, 
a.category, a.name, a.sender, a.receiver, a.message_id, a.message_priority, a.message_version, a.message_req, 
a.message_ack_id, a.message_ack_priority, a.message_ack_version, a.message_ack, 
a.success_feedback, a.failure_feedback, a.timeout_feedback, a.state, a.progress, a.progress_weight, 
a.timeout, a.history_repeat_count, a.synchronous,a.skipped, a.finished, a.create_time AS action_create_time, a.last_repeat_time, a.finish_time,a.description, 
a.label_int64_1, a.label_int64_2, a.label_int64_3, a.label_int64_4, a.label_int64_5, a.label_int64_6, a.label_int64_7, a.label_int64_8, a.label_int64_9, 
a.label_string_1, a.label_string_2, a.label_string_3, a.label_string_4, a.label_string_5, a.label_string_6, a.label_string_7, a.label_string_8, a.label_string_9
FROM view_actions a, view_workflows w WHERE ((a.workflow_id)::text = (w.workflow_id)::text);;

CREATE VIEW valid_action_sequences AS SELECT action_sequences.action_prev, action_sequences.action_next FROM action_sequences WHERE ((action_sequences.action_prev)::text IN (SELECT action_pool.action_id FROM action_pool WHERE (action_pool.finished = 0)));;

CREATE VIEW paused_actions AS SELECT a.action_id, a.workflow_id, a.engine, a.application, a.process FROM (workflow_actions a LEFT JOIN valid_action_sequences s ON (((a.action_id)::text = (s.action_next)::text))) WHERE (((s.action_prev IS NULL) AND (a.workflow_paused = 1)) AND (a.finished = 0));;

CREATE VIEW running_actions AS SELECT a.action_id, a.workflow_id, a.engine, a.application, a.process FROM (workflow_actions a LEFT JOIN valid_action_sequences s ON (((a.action_id)::text = (s.action_next)::text))) WHERE (((s.action_prev IS NULL) AND (a.workflow_paused = 0)) AND (a.finished = 0));;

CREATE VIEW workflow_progress AS SELECT action_pool.workflow_id, (sum((action_pool.progress * action_pool.progress_weight)) / sum(action_pool.progress_weight)) AS progress FROM action_pool GROUP BY action_pool.workflow_id;;

CREATE VIEW view_workflow_history AS SELECT w.workflow_id, w.rollback_workflow_id, w.category, w.name, w.application, w.process, w.state, w.detail, w.failed_action_category, w.failed_action_name, w.create_time, w.delete_time, w.description, l.label_int64_1, l.label_int64_2, l.label_int64_3, l.label_int64_4, l.label_int64_5, l.label_int64_6, l.label_int64_7, l.label_int64_8, l.label_int64_9, l.label_string_1, l.label_string_2, l.label_string_3, l.label_string_4, l.label_string_5, l.label_string_6, l.label_string_7, l.label_string_8, l.label_string_9 FROM workflow_history w, workflow_labels_history l WHERE ((w.workflow_id)::text = (l.workflow_id)::text);;

CREATE VIEW webview_workflows AS SELECT w.workflow_id, w.rollback_workflow_id, w.category, w.name, w.application, w.process, w.engine, w.paused, w.create_time, w.pause_time, w.description, w.label_int64_1, w.label_int64_2, w.label_int64_3, w.label_int64_4, w.label_int64_5, w.label_int64_6, w.label_int64_7, w.label_int64_8, w.label_int64_9, w.label_string_1, w.label_string_2, w.label_string_3, w.label_string_4, w.label_string_5, w.label_string_6, w.label_string_7, w.label_string_8, w.label_string_9, p.progress,w.originator FROM view_workflows w, workflow_progress p WHERE ((w.workflow_id)::text = (p.workflow_id)::text);;

--然后才是tc上各个业务模块的视图

CREATE VIEW image_ref_by_private_vt AS SELECT i.image_id, (count(list.image_id))::integer AS ref_count FROM (image_pool i LEFT JOIN (SELECT DISTINCT vt_image.image_id, vt_image.vmtemplate_id FROM vmtemplate_image vt_image, vmtemplate_pool vt WHERE ((vt_image.vmtemplate_id = vt.oid) AND (vt.is_public = 0))) list ON ((i.image_id = list.image_id))) GROUP BY i.image_id;;

CREATE VIEW image_ref_by_public_vt AS SELECT i.image_id, (count(list.image_id))::integer AS ref_count FROM (image_pool i LEFT JOIN (SELECT DISTINCT vt_image.image_id, vt_image.vmtemplate_id FROM vmtemplate_image vt_image, vmtemplate_pool vt WHERE ((vt_image.vmtemplate_id = vt.oid) AND (vt.is_public = 1))) list ON ((i.image_id = list.image_id))) GROUP BY i.image_id;;

CREATE VIEW image_vmcfg_count AS SELECT i.image_id, (count(list.image_id))::integer AS count FROM (image_pool i LEFT JOIN (SELECT DISTINCT vmcfg_image.image_id, vmcfg_image.vid FROM vmcfg_image) list ON ((i.image_id = list.image_id))) GROUP BY i.image_id;;

CREATE VIEW image_vmtemplate_count AS SELECT pub_vt.image_id, pub_vt.ref_count AS public_vt_count, pri_vt.ref_count AS private_vt_count FROM (image_ref_by_public_vt pub_vt LEFT JOIN image_ref_by_private_vt pri_vt ON ((pub_vt.image_id = pri_vt.image_id)));;

CREATE VIEW view_vmcfg_with_project AS SELECT vm.oid, vm.name, vm.description, vm.create_time, vm.vcpu, vm.tcu, vm.memory, vm.virt_type, vm.root_device, vm.kernel_command, vm.bootloader, vm.raw_data, vm.vnc_password, vm.qualification, vm.watchdog, vm.appointed_cluster, vm.appointed_host, vm.project_id, vm.deployed_cluster, vm.config_version, vm.enable_coredump, vm.enable_serial,  vm.hypervisor, vm.enable_livemigrate, p.name AS project_name, u.oid AS uid, u.name AS user_name FROM vmcfg_pool vm, project_pool p, user_pool u WHERE ((vm.project_id = p.oid) AND (p.uid = u.oid));;


CREATE OR REPLACE VIEW view_project AS 
 SELECT p.oid, p.name, p.description, u.name AS user_name, u.oid AS uid
   FROM project_pool p, user_pool u
  WHERE p.uid  = u.oid; 

CREATE VIEW view_project_with_cluster AS SELECT DISTINCT p.oid AS project_id, p.name AS project_name, cluster.name AS cluster_name, p.uid AS user_id FROM vmcfg_pool vm, project_pool p, cluster_pool cluster WHERE ((vm.project_id = p.oid) AND ((vm.deployed_cluster)::text = (cluster.name)::text));;

CREATE OR REPLACE VIEW view_vmcfg_image_no_image_id AS SELECT * FROM vmcfg_image WHERE image_id IS NULL;

CREATE OR REPLACE VIEW view_vmcfg_image_has_image_id AS SELECT * FROM vmcfg_image WHERE image_id IS NOT NULL;

CREATE OR REPLACE VIEW view_vmcfg_image_size AS SELECT * 
FROM (SELECT vin.vid, 0::numeric(19,0) AS total_size
           FROM view_vmcfg_image_no_image_id vin
          GROUP BY vin.vid
   UNION ALL  SELECT vi.vid, sum(img.size)::numeric(19,0) AS total_size
           FROM view_vmcfg_image_has_image_id vi, image_pool img
          WHERE vi.image_id = img.image_id
          GROUP BY vi.vid) tmp ;

CREATE OR REPLACE VIEW view_vmcfg_image_total_size AS 
 SELECT vis.vid,SUM(vis.total_size) AS total_size FROM view_vmcfg_image_size vis GROUP BY vis.vid;

CREATE OR REPLACE VIEW view_vmcfg_disk_total_size AS SELECT vid, (sum(vd.size))::numeric(19,0) AS total_size FROM vmcfg_disk vd GROUP BY vd.vid;

CREATE OR REPLACE VIEW view_vmcfg_disk_image_size AS SELECT vm_image.vid, vm_image.total_size::numeric(19,0) AS total_image_size, CASE WHEN (vm_disk.total_size IS NULL) THEN (0)::numeric(19,0) ELSE vm_disk.total_size END AS total_disk_size, CASE WHEN (vm_disk.total_size IS NULL) THEN vm_image.total_size::numeric(19,0) ELSE ((vm_image.total_size::numeric(19,0) + vm_disk.total_size::numeric(19,0)))::numeric(19,0) END AS total_size FROM ((SELECT * FROM view_vmcfg_image_total_size) vm_image LEFT JOIN (SELECT * FROM view_vmcfg_disk_total_size) vm_disk ON ((vm_image.vid = vm_disk.vid)));;

CREATE VIEW view_vmcfg_image AS SELECT vm.oid AS vid, vi.target, vi.image_id, vi."position", vi.reserved_backup, img.type, img.size, img.bus, img.fileurl, img.location, img.uid AS image_owner, img.os_type, img.disk_size, img.disk_format, img.os_arch FROM vmcfg_pool vm, vmcfg_image vi, image_pool img WHERE ((vm.oid = vi.vid) AND (vi.image_id = img.image_id));;


CREATE VIEW view_vmcfg_repel AS SELECT r.vid1, vm1.uid AS uid1, r.vid2, vm2.uid AS uid2 FROM vmcfg_repel r, view_vmcfg_with_project vm1, view_vmcfg_with_project vm2 WHERE ((vm1.oid = r.vid1) AND (vm2.oid = r.vid2));;


CREATE VIEW view_vmtemplate_disk_image_size AS SELECT vt_image.vmtemplate_id, vt_image.total_size AS total_image_size, vt_disk.total_size AS total_disk_size, CASE WHEN (vt_disk.total_size IS NULL) THEN vt_image.total_size ELSE ((vt_image.total_size + vt_disk.total_size))::numeric(19,0) END AS total_size FROM ((SELECT vt.oid AS vmtemplate_id, (sum(img.size))::numeric(19,0) AS total_size FROM vmtemplate_pool vt, vmtemplate_image vi, image_pool img WHERE ((vt.oid = vi.vmtemplate_id) AND (vi.image_id = img.image_id)) GROUP BY vt.oid) vt_image LEFT JOIN (SELECT vt.oid AS vmtemplate_id, (sum(vd.size))::numeric(19,0) AS total_size FROM vmtemplate_pool vt, vmtemplate_disk vd WHERE (vt.oid = vd.vmtemplate_id) GROUP BY vt.oid) vt_disk ON ((vt_image.vmtemplate_id = vt_disk.vmtemplate_id)));;

CREATE VIEW view_vmtemplate_image AS SELECT vt.oid AS vmtemplate_id, vti.target, vti.image_id, vti."position", vti.reserved_backup, img.type, img.size, img.bus, img.fileurl, img.location, img.uid AS image_owner, img.os_type , img.disk_size, img.disk_format, img.os_arch FROM vmtemplate_pool vt, vmtemplate_image vti, image_pool img WHERE ((vt.oid = vti.vmtemplate_id) AND (vti.image_id = img.image_id));;

CREATE VIEW user_full_info AS SELECT u.oid AS user_id, u.name AS user_name, u.password, u.enabled AS user_enabled, u.role, u.email, u.phone, u.location, u.group_id, g.name AS group_name, g.type AS group_type, g.enabled AS group_enabled, g.description AS group_description, COALESCE(p.project_count, (0)::bigint) AS project_count, COALESCE(v.vmcfg_count, (0)::bigint) AS vmcfg_count, COALESCE(t.vmtpl_count, (0)::bigint) AS vmtpl_count, COALESCE(i.image_count, (0)::bigint) AS image_count FROM (((((user_pool u LEFT JOIN (SELECT view_vmcfg_with_project.uid, count(view_vmcfg_with_project.oid) AS vmcfg_count FROM view_vmcfg_with_project GROUP BY view_vmcfg_with_project.uid) v ON ((u.oid = v.uid))) LEFT JOIN (SELECT vmtemplate_pool.uid, count(vmtemplate_pool.oid) AS vmtpl_count FROM vmtemplate_pool GROUP BY vmtemplate_pool.uid) t ON ((u.oid = t.uid))) LEFT JOIN (SELECT image_pool.uid, count(image_pool.image_id) AS image_count FROM image_pool GROUP BY image_pool.uid) i ON ((u.oid = i.uid))) LEFT JOIN (SELECT project_pool.uid, count(project_pool.oid) AS project_count FROM project_pool GROUP BY project_pool.uid) p ON ((u.oid = p.uid))) LEFT JOIN usergroup_pool g ON ((u.oid = g.oid)));;

CREATE or replace VIEW vmcfg_full_info AS
SELECT vmcfg_pool.oid AS vid, vmcfg_pool.name AS vm_name, vmcfg_pool.description AS vm_description, 
vmcfg_pool.create_time AS vm_create_time, vmcfg_pool.vcpu, vmcfg_pool.tcu, vmcfg_pool.memory, view_vmcfg_disk_image_size.total_size, 
vmcfg_pool.virt_type, vmcfg_pool.root_device, vmcfg_pool.kernel_command, vmcfg_pool.bootloader, vmcfg_pool.raw_data, 
vmcfg_pool.qualification, vmcfg_pool.watchdog, vmcfg_pool.appointed_cluster, vmcfg_pool.appointed_host, vmcfg_pool.deployed_cluster, 
project_pool.oid AS project_id, project_pool.name AS project_name, project_pool.description AS project_description, 
project_pool.create_time AS project_create_time, user_pool.oid AS uid, user_pool.name AS user_name, 
user_pool.description AS user_description, user_pool.create_time AS user_create_time, user_pool.enabled AS user_enabled, 
user_pool.role, user_pool.group_id, user_pool.email, user_pool.phone, user_pool.location, usergroup_pool.name AS usergroup_name, 
usergroup_pool.description AS usergroup_description, usergroup_pool.create_time AS usergroup_create_time, 
usergroup_pool.enabled AS usergroup_enabled, usergroup_pool.type AS usergroup_type 
FROM project_pool, user_pool, usergroup_pool, vmcfg_pool
left join view_vmcfg_disk_image_size on view_vmcfg_disk_image_size.vid = vmcfg_pool.oid
WHERE ((((vmcfg_pool.project_id = project_pool.oid)) 
AND (project_pool.uid = user_pool.oid)) AND (user_pool.group_id = usergroup_pool.oid));

CREATE OR REPLACE VIEW image_all_info AS SELECT i.image_id, i.name, i.uid, i.type, i.bus, i.size, i.is_public,i.enabled, i.location, i.fileurl, i.disk_format, i.container_format, i.os_type, i.os_version, i.os_arch, i.register_time, i.description,image_count.count AS running_vms, private.ref_count AS ref_count_by_private_vt,public.ref_count AS ref_count_by_public_vt, i.disk_size FROM image_pool i, image_vmcfg_count image_count, image_ref_by_private_vt private, image_ref_by_public_vt public WHERE (i.image_id = image_count.image_id AND i.image_id = private.image_id AND i.image_id = public.image_id);;

CREATE OR REPLACE VIEW web_view_image AS 
 SELECT vi.image_id, split_part(vi.location::text, '/'::text, 7) AS file_name, vi.register_time, vi.enabled, 
        CASE
            WHEN vi.enabled = 1 THEN '可用'::text
            ELSE '不可用'::text
        END AS enabled_ch, 
        CASE
            WHEN vi.enabled = 1 THEN 'Enable'::text
            ELSE 'Disable'::text
        END AS enabled_en, vi.is_public, 
        CASE
            WHEN vi.is_public = 0 THEN '否'::text
            ELSE '是'::text
        END AS is_public_ch, 
        CASE
            WHEN vi.is_public = 0 THEN 'No'::text
            ELSE 'Yes'::text
        END AS is_public_en, vi.running_vms + vi.ref_count_by_private_vt + vi.ref_count_by_public_vt AS running_vms, vi.type, round(vi.size / 1024::numeric / 1024::numeric, 2) AS size, u.name AS user_name, vi.os_type, vi.disk_size, vi.disk_format, vi.description, vi.container_format, 
        CASE
            WHEN vi.container_format::text = ''::text THEN '虚拟机镜像'::text
            ELSE '工程镜像'::text
        END AS container_format_ch, 
        CASE
            WHEN vi.container_format::text = ''::text THEN 'VM Image'::text
            ELSE 'Project Image'::text
        END AS container_format_en
   FROM image_all_info vi, user_pool u
  WHERE vi.uid = u.oid;


CREATE OR REPLACE VIEW web_view_project AS SELECT project.name, project.description, project.username, CASE WHEN vm.vmcount IS NULL THEN 0::numeric ELSE vm.vmcount END AS vmcount FROM ( SELECT p.oid, p.name, p.description, u.name AS username FROM project_pool p, user_pool u WHERE p.uid = u.oid) project LEFT JOIN ( SELECT vmcfg_full_info.project_id, count(vmcfg_full_info.vid)::numeric(19,0) AS vmcount FROM vmcfg_full_info GROUP BY vmcfg_full_info.project_id) vm ON vm.project_id = project.oid;;

CREATE OR REPLACE VIEW web_view_vm AS 
 SELECT vmfull.vid, vmfull.vm_name, vmfull.vm_create_time, vmfull.vcpu, vmfull.tcu, round(vmfull.memory / 1024::numeric / 1024::numeric, 2) AS memory, round(vmfull.total_size / 1024::numeric / 1024::numeric / 1024::numeric, 2) AS total_size, vmfull.user_name, vmfull.project_name, vmfull.deployed_cluster, vmcfg.config_version
   FROM vmcfg_full_info vmfull, vmcfg_pool vmcfg
  WHERE vmcfg.oid = vmfull.vid;

CREATE OR REPLACE VIEW web_view_pdisk AS 
 SELECT storage_volume.request_id, storage_volume.name, storage_volume.size, storage_volume.sid, user_pool.name AS username, storage_user_volume.description
   FROM storage_volume, storage_user_volume, user_pool
  WHERE storage_user_volume.uid <> (-1)::numeric AND storage_volume.request_id::text = storage_user_volume.request_id::text AND user_pool.oid = storage_user_volume.uid;

CREATE OR REPLACE VIEW web_view_vm_overview AS 
 SELECT user_full_info.user_name, user_full_info.project_count, user_full_info.vmcfg_count, user_full_info.vmtpl_count, user_full_info.image_count, COALESCE(v.pdisk_count, 0::bigint) AS pdisk_count
   FROM user_full_info
   LEFT JOIN ( SELECT web_view_pdisk.username, count(web_view_pdisk.request_id) AS pdisk_count
           FROM web_view_pdisk
          GROUP BY web_view_pdisk.username) v ON user_full_info.user_name::text = v.username::text;
		  
CREATE OR REPLACE VIEW web_view_vt AS SELECT vt.oid AS id, vt.name AS vt_name, vt.vcpu, vt.tcu, round(vt.memory / 1024::numeric / 1024::numeric, 2) AS memory, round(disk.total_size / 1024::numeric / 1024::numeric / 1024::numeric, 2) AS total_size,vt.is_public,  CASE WHEN vt.is_public = 0 THEN '否'::text ELSE '是'::text END AS is_public_ch, CASE WHEN vt.is_public = 0 THEN 'No'::text ELSE 'Yes'::text END AS is_public_en, u.name AS user_name FROM vmtemplate_pool vt, view_vmtemplate_disk_image_size disk, user_pool u WHERE vt.oid = disk.vmtemplate_id AND u.oid = vt.uid;;

CREATE OR REPLACE VIEW web_view_current_alarm AS SELECT current_alarm.alarm_id, current_alarm.alarm_code, to_char(to_timestamp(current_alarm.alarm_time::double precision) at time zone 'UTC', 'YYYY-MM-DD HH24:MI:SS'::text) AS alarm_time, current_alarm.subsystem, CASE WHEN char_length(current_alarm.level2::text) <> 0 AND char_length(current_alarm.level3::text) <> 0 THEN ((((current_alarm.level1::text || '|'::text) || current_alarm.level2::text) || '|'::text) || current_alarm.level3::text)::character varying WHEN char_length(current_alarm.level2::text) = 0 AND char_length(current_alarm.level3::text) <> 0 THEN ((current_alarm.level1::text || '|'::text) || current_alarm.level3::text)::character varying WHEN char_length(current_alarm.level2::text) <> 0 AND char_length(current_alarm.level3::text) = 0 THEN ((current_alarm.level1::text || '|'::text) || current_alarm.level2::text)::character varying ELSE current_alarm.level1 END AS lgclocation, current_alarm.addinfo FROM current_alarm;;

CREATE OR REPLACE VIEW web_view_history_alarm AS SELECT history_alarm.alarm_code, to_char(to_timestamp(history_alarm.alarm_time::double precision) at time zone 'UTC', 'YYYY-MM-DD HH24:MI:SS'::text) AS alarm_time, to_char(to_timestamp(history_alarm.restore_time::double precision) at time zone 'UTC', 'YYYY-MM-DD HH24:MI:SS'::text) AS restore_time, history_alarm.restore_type, history_alarm.subsystem, CASE WHEN char_length(history_alarm.level2::text) <> 0 AND char_length(history_alarm.level3::text) <> 0 THEN ((((history_alarm.level1::text || '|'::text) || history_alarm.level2::text) || '|'::text) || history_alarm.level3::text)::character varying WHEN char_length(history_alarm.level2::text) = 0 AND char_length(history_alarm.level3::text) <> 0 THEN ((history_alarm.level1::text || '|'::text) || history_alarm.level3::text)::character varying WHEN char_length(history_alarm.level2::text) <> 0 AND char_length(history_alarm.level3::text) = 0 THEN ((history_alarm.level1::text || '|'::text) || history_alarm.level2::text)::character varying ELSE history_alarm.level1 END AS lgclocation, history_alarm.addinfo FROM history_alarm;;


CREATE OR REPLACE VIEW storage_target_action_view AS 
SELECT al.label_int64_1 AS sid ,al.label_int64_2 AS target_id 
FROM action_pool ap,action_labels al
WHERE (ap.name::text = 'create_target'::text OR ap.name::text = 'delete_target'::text) AND ap.action_id=al.action_id ;


CREATE OR REPLACE VIEW storage_volume_action_view AS 
SELECT al.label_int64_1 AS sid , al.label_int64_2 AS size , al.label_int64_3 AS volume_id , al.label_string_1 AS vg_name , al.label_string_2 AS name
FROM action_pool ap,action_labels al
WHERE (ap.name::text = 'create_volume'::text OR ap.name::text = 'delete_volume'::text) AND ap.action_id=al.action_id ;

CREATE OR REPLACE VIEW storage_lun_action_view AS 
SELECT al.label_int64_1 AS sid , al.label_string_1 AS target_name ,al.label_int64_2 AS lunid
FROM action_pool ap,action_labels al
WHERE (ap.name::text = 'add_volume_to_target'::text OR ap.name::text = 'delete_volume_from_target'::text) AND ap.action_id=al.action_id ;

CREATE OR REPLACE VIEW storage_volume_with_action_view AS 
SELECT sv.sid,sv.vg_name,
   CASE  
       WHEN sv.name IS NULL THEN svav.name
       ELSE sv.name
   END AS name,
   CASE
        WHEN sv.size IS NULL THEN svav.size        
        ELSE sv.size 
   END AS size
FROM storage_volume sv FULL JOIN storage_volume_action_view svav
ON sv.sid=svav.sid AND sv.vg_name::text = svav.vg_name::text AND sv.name::text = svav.name::text;    
   
CREATE OR REPLACE VIEW storage_vg_alloc_view AS
  SELECT svwav.sid AS sid, svwav.vg_name AS vg_name, SUM(svwav.size) AS alloc_size 
  FROM storage_volume_with_action_view svwav GROUP BY svwav.sid, svwav.vg_name;

CREATE OR REPLACE VIEW storage_vg_free_view AS
  SELECT svg.sid AS sid, svg.name AS vg_name, (CASE WHEN (svav.alloc_size IS NULL) THEN svg.size ELSE (svg.size-svav.alloc_size)::numeric(19,0) END) AS free_size 
  FROM storage_vg svg LEFT JOIN storage_vg_alloc_view svav ON svg.sid = svav.sid AND svg.name = svav.vg_name;
  
CREATE OR REPLACE VIEW storage_adaptor_max_free_view AS
    SELECT sid, MAX(storage_vg_free_view.free_size) as free_size 
	FROM storage_vg_free_view 
	GROUP BY storage_vg_free_view.sid;

CREATE OR REPLACE VIEW storage_vg_single_free_view AS
    SELECT sid,vg_name,free_size 
	FROM storage_vg_free_view svfv 
	WHERE NOT EXISTS(SELECT 1 FROM storage_vg_free_view WHERE svfv.sid = sid AND svfv.free_size = free_size AND svfv.vg_name < vg_name);
	
CREATE OR REPLACE VIEW storage_adaptor_max_free_with_vg_name_view AS	
    SELECT samfv.sid, vg_name, samfv.free_size 
	FROM storage_adaptor_max_free_view samfv,storage_vg_single_free_view svsfv 
	WHERE samfv.sid = svsfv.sid AND samfv.free_size = svsfv.free_size;

CREATE OR REPLACE VIEW storage_adaptor_sum_free_view AS
    SELECT sid, SUM(storage_vg_free_view.free_size) AS free_size
	FROM storage_vg_free_view 
	GROUP BY storage_vg_free_view.sid;
	
CREATE OR REPLACE VIEW storage_adaptor_free_view AS
  SELECT sasfv.sid AS sid, vg_name, sasfv.free_size AS sa_free_size, samfwvnv.free_size AS vg_free_size 
  FROM storage_adaptor_sum_free_view sasfv, storage_adaptor_max_free_with_vg_name_view samfwvnv 
  WHERE sasfv.sid = samfwvnv.sid;

CREATE OR REPLACE VIEW storage_adaptor_with_vg_and_cluster_view AS 
  SELECT sa.sid AS sid, cp.name AS cluster_name, sa.application AS application, sa.name AS unit,sa.type AS type, safv.vg_name AS vg_name,safv.sa_free_size AS sa_total_free_size, safv.vg_free_size AS max_vg_free_size  
  FROM cluster_pool cp, storage_cluster sc, storage_adaptor sa, storage_adaptor_free_view safv 
  WHERE (safv.sid = sa.sid) AND (cp.name = sc.cluster_name) AND (sc.sid = sa.sid) AND (sa.enabled = 1) AND (sa.is_online  = 1); 

CREATE OR REPLACE VIEW storage_adaptor_with_multivg_and_cluster_view AS
    SELECT sa.sid, cp.name AS cluster_name, sa.application, sa.name AS unit, sa.type, svfv.vg_name, safv.sa_free_size AS sa_total_free_size, svfv.free_size AS vg_free_size 
	FROM cluster_pool cp, storage_cluster sc, storage_adaptor sa, storage_adaptor_free_view safv, storage_vg_free_view svfv 
	WHERE safv.sid = sa.sid AND cp.name::text = sc.cluster_name::text AND sc.sid = sa.sid AND sa.enabled = 1 AND sa.is_online = 1 AND  svfv.sid = sa.sid;

CREATE OR REPLACE VIEW storage_volume_with_lun_and_auth_and_adaptor_view AS 
  SELECT sv.volume_uuid AS vol_uuid, sl.target_name AS target_name,sl.lun_id AS lun_id,sau.ip_address AS ip_address,sau.iscsi_initiator AS iscsi_initiator,sa.application AS application,sa.name AS unit,sa.type AS type 
  FROM storage_adaptor sa,storage_volume sv,storage_lun sl,storage_authorize_lun sal,storage_authorize sau
  WHERE (sv.sid = sa.sid) AND (sl.volume_uuid = sv.volume_uuid) AND (sal.lun_uuid = sl.lun_uuid) AND (sau.auth_uuid = sal.auth_uuid);

CREATE OR REPLACE VIEW storage_volume_with_lun_and_address_view AS 
  SELECT sv.volume_uuid AS vol_uuid, sl.lun_id AS lun_id, sa.media_address AS media_address 
  FROM storage_address sa,storage_volume sv,storage_lun sl
  WHERE sv.sid = sa.sid AND sl.volume_uuid = sv.volume_uuid; 

CREATE OR REPLACE VIEW storage_volume_with_adaptor_view AS 
  SELECT sv.volume_uuid AS vol_uuid, sa.sid AS sid, sv.vg_name AS vg_name, sv.name AS vol_name, sa.application AS application, sa.name AS unit, sa.type AS type 
  FROM storage_volume sv,storage_adaptor sa 
  WHERE sv.sid = sa.sid; 

CREATE OR REPLACE VIEW storage_authorize_with_target_id_view AS 
  SELECT st.target_id AS target_id, sa.sid AS sid, sa.target_name AS target_name, sa.ip_address AS ip_address, sa.iscsi_initiator AS iscsi_initiator, sa.auth_uuid AS auth_uuid 
  FROM storage_target st,storage_authorize sa 
  WHERE (sa.sid = st.sid) AND (sa.target_name = st.name); 
  
CREATE OR REPLACE VIEW storage_volume_with_lun_view AS 
  SELECT sv.sid AS sid, sv.vg_name AS vg_name, sv.name AS volume_name, sv.volume_uuid AS volume_uuid, sl.target_name AS target_name, sl.lun_id AS lun_id, sl.lun_uuid AS lun_uuid 
  FROM storage_volume sv,storage_lun sl 
  WHERE (sv.sid = sl.sid) AND (sv.volume_uuid = sl.volume_uuid); 
  
CREATE OR REPLACE VIEW storage_cluster_total_free_size_view AS 
  SELECT cluster_name, SUM(sa_total_free_size) AS total_free_size
  FROM storage_adaptor_with_vg_and_cluster_view 
  GROUP BY cluster_name; 
  
CREATE OR REPLACE VIEW storage_cluster_max_free_size_view AS
  SELECT cluster_name, max(sa_total_free_size) AS max_free_size 
  FROM storage_adaptor_with_vg_and_cluster_view 
  GROUP BY cluster_name;

  
CREATE OR REPLACE VIEW base_image_cluster_view AS 
  SELECT bic.base_uuid AS base_uuid, bic.cluster_name AS cluster_name, bi.base_id AS base_id, bi.image_id AS image_id
  FROM base_image_cluster bic,base_image bi 
  WHERE bic.base_uuid = bi.base_uuid;   
  
 
CREATE OR REPLACE VIEW web_view_deployedimage AS 
 SELECT deployimage.base_uuid, deployimage.cluster_name, deployimage.base_id, deployimage.image_id, 
        web_image.file_name, web_image.user_name, web_image.description
   FROM base_image_cluster_view deployimage, web_view_image web_image
  WHERE deployimage.image_id = web_image.image_id;
  
CREATE OR REPLACE VIEW view_image_other_user AS 
 SELECT list.image_id AS image_id, list.uid  AS uid
  FROM (SELECT vmcfg_image.image_id, vmcfg_full_info.uid 
         FROM vmcfg_image, vmcfg_full_info
         WHERE vmcfg_image.vid = vmcfg_full_info.vid) list, image_pool
 WHERE image_pool.image_id = list.image_id and image_pool.uid != list.uid;

CREATE OR REPLACE VIEW web_view_image_workflow AS
  SELECT  wl.label_int64_1 as image_id, wl.label_string_1 as base_uuid, wl.label_string_2 as cluster_name, p.progress as progress, w.name as workflow_name, w.workflow_id as workflow_id
  FROM workflow w, workflow_progress p, workflow_labels wl
  WHERE w.category ='image' and w.paused = 0 and w.workflow_id = p.workflow_id and w.workflow_id = wl.workflow_id;

CREATE OR REPLACE VIEW web_view_vm_workflow AS
  SELECT  wl.label_int64_1 as vid, w.name as workflow_name, p.progress as progress, w.workflow_id as workflow_id  
  FROM workflow w, workflow_progress p, workflow_labels wl
  WHERE w.category ='vm' and w.paused = 0 and w.workflow_id = p.workflow_id and w.workflow_id = wl.workflow_id;

CREATE OR REPLACE VIEW view_affinity_region_with_cluster AS
  SELECT  arv.affinity_region_id, arv.vid, ar.name,  vmcfg.deployed_cluster
  FROM  affinity_region ar, affinity_region_vmcfg  arv, vmcfg_pool  vmcfg
  WHERE  arv.vid = vmcfg.oid and ar.oid = arv.affinity_region_id and vmcfg.deployed_cluster is NOT NULL;

CREATE OR REPLACE VIEW view_vmcfg_with_affinity_region AS
  SELECT  arv.vid, arv.affinity_region_id, ar.name, ar.description, ar.create_time, ar.uid, ar.level, ar.is_forced
  FROM  affinity_region_vmcfg  arv, affinity_region ar
  WHERE arv.affinity_region_id = ar.oid;

CREATE OR REPLACE VIEW view_affinity_region AS
  SELECT  ar.oid, ar.name, ar.description, u.name as user_name, ar.level, ar.is_forced, ar.uid
  FROM  affinity_region  ar, user_pool u
  WHERE ar.uid = u.oid;

CREATE OR REPLACE VIEW view_repel_with_affinity_region AS
  SELECT arv.affinity_region_id,  r.vid1, r.vid2, ar.level 
  FROM affinity_region_vmcfg arv left join vmcfg_repel r ON arv.vid = r.vid1, affinity_region ar WHERE ar.oid = arv.affinity_region_id  
  INTERSECT SELECT arv.affinity_region_id,  r.vid1, r.vid2, ar.level 
  FROM affinity_region_vmcfg arv right join vmcfg_repel r ON arv.vid = r.vid2, affinity_region ar WHERE ar.oid = arv.affinity_region_id;  

  CREATE OR REPLACE VIEW storage_vid_volume AS 
SELECT substring(suv.usage::text FROM '(.*):' )::integer AS vid ,suv.request_id,sv.size
FROM storage_user_volume suv
LEFT JOIN storage_volume sv on suv.request_id::text = sv.request_id::text;   

CREATE OR REPLACE VIEW view_local_share_disk AS 
SELECT 
    CASE
    WHEN list1.vid IS NOT NULL THEN list1.vid
    ELSE list2.vid
    END  AS vid,list1.local_disk,list2.share_disk
FROM
(SELECT list.vid,list.disk_size AS local_disk
FROM(
SELECT  disk.vid,disk.position,sum(disk.size) AS disk_size                           
       FROM   vmcfg_disk disk GROUP BY disk.vid,disk.position)list  WHERE list."position"=1)list1
FULL JOIN       
(SELECT lists.vid,lists.disk_size AS share_disk
FROM(
SELECT  disk.vid,disk.position,sum(disk.size) AS disk_size                           
       FROM   vmcfg_disk disk GROUP BY disk.vid,disk.position)lists  WHERE lists."position"=2)list2
ON list1.vid = list2.vid ;         

CREATE OR REPLACE VIEW view_local_share_image AS 
SELECT
    CASE
    WHEN list1.vid IS NOT NULL THEN list1.vid
    ELSE list2.vid
    END AS vid,list1.local_img,list2.share_img
FROM
(SELECT listA.vid,listA.img_size AS local_img
FROM(SELECT list.vid,list.position,sum(list.disk_size) AS img_size
            FROM (SELECT img.vid,img.position,ipool.disk_size 
                  FROM vmcfg_image img LEFT JOIN  image_pool ipool ON img.image_id = ipool.image_id) list 
                  GROUP BY list.vid,list.position) listA WHERE listA."position"=1)list1
FULL JOIN
(SELECT listB.vid,listB.img_size AS share_img
FROM(SELECT list.vid,list.position,sum(list.disk_size) AS img_size
            FROM (SELECT img.vid,img.position,ipool.disk_size 
                  FROM vmcfg_image img LEFT JOIN  image_pool ipool ON img.image_id = ipool.image_id) list 
                  GROUP BY list.vid,list.position) listB WHERE listB."position"=2 ) list2 
ON list1.vid = list2.vid ;

CREATE OR REPLACE VIEW view_local_share_storage AS 
SELECT 
    CASE
    WHEN list1.vid IS NOT NULL THEN list1.vid
    ELSE list2.vid
    END  AS vid, (COALESCE(list1.local_disk,0) + COALESCE(list2.local_img,0)) AS local_storage ,
    (COALESCE(list1.share_disk,0) + COALESCE(list2.share_img,0)) AS share_storage   
FROM
view_local_share_disk list1
FULL JOIN       
view_local_share_image list2
ON list1.vid = list2.vid ;      
                  
CREATE OR REPLACE VIEW view_project_statistics_static AS 
 SELECT vproject.project_id,  vproject.oid AS vid, vproject.tcu, vproject.memory, vproject.local_storage, vproject.share_storage, vproject.nic_number
   FROM ( SELECT *
           FROM vmcfg_pool vpool
      LEFT JOIN (
        SELECT  svv.local_storage,svv.share_storage,list.nic_number,
        CASE  
        WHEN svv.vid IS NULL THEN list.vid
        ELSE svv.vid
        END AS vid
        FROM view_local_share_storage  svv
            FULL JOIN ( SELECT  count(vnic.vid)::integer AS nic_number, vnic.vid
                   FROM vmcfg_nic vnic
                  GROUP BY vnic.vid) list ON svv.vid = list.vid)snlist ON vpool.oid = snlist.vid) vproject
  WHERE vproject.deployed_cluster IS NOT NULL;

CREATE OR REPLACE VIEW view_project_statistics_static_sum  AS 
 SELECT ppool.project_id, ppool.project_name, ppool.user_id,list.vm_sum, list.tcu_sum, list.memory_sum, list.nic_number_sum, list.storage_share_sum,list.storage_local_sum
   FROM ( SELECT vproject.project_id, count(vproject.vid)::integer AS vm_sum, sum(vproject.tcu)::integer AS tcu_sum, sum(vproject.memory)::numeric(19,0) AS memory_sum,
          sum(vproject.nic_number)::numeric(19,0) AS nic_number_sum, sum(vproject.share_storage)::numeric(19,0) AS storage_share_sum,
          sum(vproject.local_storage)::numeric(19,0) AS storage_local_sum
          FROM view_project_statistics_static vproject
          GROUP BY vproject.project_id) list
   LEFT JOIN view_project_with_cluster ppool 
   ON list.project_id = ppool.project_id;


