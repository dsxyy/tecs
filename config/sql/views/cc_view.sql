--*********************************************************************
-- 版权所有 (C)2012, 深圳市中兴通讯股份有限公司。
--
-- 文件名称： cc_view.sql
-- 文件标识：
-- 内容摘要：
-- 其它说明：
-- 当前版本：
-- 作    者： zte
-- 完成日期： 2012-8-8
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
a.timeout, a.history_repeat_count,a.synchronous, a.skipped, a.finished, a.create_time AS action_create_time, a.last_repeat_time, a.finish_time,a.description, 
a.label_int64_1, a.label_int64_2, a.label_int64_3, a.label_int64_4, a.label_int64_5, a.label_int64_6, a.label_int64_7, a.label_int64_8, a.label_int64_9, 
a.label_string_1, a.label_string_2, a.label_string_3, a.label_string_4, a.label_string_5, a.label_string_6, a.label_string_7, a.label_string_8, a.label_string_9
FROM view_actions a, view_workflows w WHERE ((a.workflow_id)::text = (w.workflow_id)::text);;

CREATE VIEW valid_action_sequences AS SELECT action_sequences.action_prev, action_sequences.action_next FROM action_sequences WHERE ((action_sequences.action_prev)::text IN (SELECT action_pool.action_id FROM action_pool WHERE (action_pool.finished = 0)));;

CREATE VIEW paused_actions AS SELECT a.action_id, a.workflow_id, a.engine, a.application, a.process FROM (workflow_actions a LEFT JOIN valid_action_sequences s ON (((a.action_id)::text = (s.action_next)::text))) WHERE (((s.action_prev IS NULL) AND (a.workflow_paused = 1)) AND (a.finished = 0));;

CREATE VIEW running_actions AS SELECT a.action_id, a.workflow_id, a.engine, a.application, a.process FROM (workflow_actions a LEFT JOIN valid_action_sequences s ON (((a.action_id)::text = (s.action_next)::text))) WHERE (((s.action_prev IS NULL) AND (a.workflow_paused = 0)) AND (a.finished = 0));;

CREATE VIEW workflow_progress AS SELECT action_pool.workflow_id, (sum((action_pool.progress * action_pool.progress_weight)) / sum(action_pool.progress_weight)) AS progress FROM action_pool GROUP BY action_pool.workflow_id;;

CREATE VIEW view_workflow_history AS SELECT w.workflow_id, w.rollback_workflow_id, w.category, w.name, w.application, w.process, w.state, w.detail, w.failed_action_category, w.failed_action_name, w.create_time, w.delete_time, w.description, l.label_int64_1, l.label_int64_2, l.label_int64_3, l.label_int64_4, l.label_int64_5, l.label_int64_6, l.label_int64_7, l.label_int64_8, l.label_int64_9, l.label_string_1, l.label_string_2, l.label_string_3, l.label_string_4, l.label_string_5, l.label_string_6, l.label_string_7, l.label_string_8, l.label_string_9 FROM workflow_history w, workflow_labels_history l WHERE ((w.workflow_id)::text = (l.workflow_id)::text);;

CREATE VIEW webview_workflows AS SELECT w.workflow_id, w.rollback_workflow_id, w.category, w.name, w.application, w.process, w.engine, w.paused, w.create_time, w.pause_time, w.description, w.label_int64_1, w.label_int64_2, w.label_int64_3, w.label_int64_4, w.label_int64_5, w.label_int64_6, w.label_int64_7, w.label_int64_8, w.label_int64_9, w.label_string_1, w.label_string_2, w.label_string_3, w.label_string_4, w.label_string_5, w.label_string_6, w.label_string_7, w.label_string_8, w.label_string_9, p.progress, w.originator FROM view_workflows w, workflow_progress p WHERE ((w.workflow_id)::text = (p.workflow_id)::text);;

--然后才是cc上各个业务模块的视图

CREATE VIEW dhcp_all_gateway AS SELECT dhcpd_subnet.id AS subnet_id, dhcpd_subnet.type AS subnet_type, dhcpd_subnet.net, dhcpd_subnet.mask, dhcpd_option.routers, dhcpd_option.subnet_mask FROM dhcpd_map, dhcpd_subnet, dhcpd_option WHERE ((((dhcpd_subnet.id = dhcpd_map.owner_id) AND (dhcpd_subnet.type = dhcpd_map.owner_type)) AND (dhcpd_option.id = dhcpd_map.id)) AND (dhcpd_option.type = dhcpd_map.type));;

CREATE VIEW dhcp_all_host AS SELECT dhcpd_host.id AS host_id, dhcpd_host.type AS host_type, dhcpd_param.fixed_ip, dhcpd_param.hardware_mac, dhcpd_host.name FROM dhcpd_host, dhcpd_map, dhcpd_param WHERE ((((dhcpd_host.id = dhcpd_map.owner_id) AND (dhcpd_host.type = dhcpd_map.owner_type)) AND (dhcpd_map.id = dhcpd_param.id)) AND (dhcpd_map.type = dhcpd_param.type));;

CREATE VIEW dhcp_all_range AS SELECT dhcpd_range.id AS range_id, dhcpd_range.type AS range_type, dhcpd_subnet.id AS subnet_id, dhcpd_subnet.type AS subnet_type, dhcpd_range.ip_start, dhcpd_range.ip_end, dhcpd_subnet.net, dhcpd_subnet.mask FROM dhcpd_map, dhcpd_range, dhcpd_subnet WHERE ((((dhcpd_range.id = dhcpd_map.id) AND (dhcpd_range.type = dhcpd_map.type)) AND (dhcpd_subnet.id = dhcpd_map.owner_id)) AND (dhcpd_subnet.type = dhcpd_map.owner_type));;

CREATE VIEW dhcp_all_subnet AS SELECT dhcpd_subnet.id AS subnet_id, dhcpd_subnet.type AS subnet_type, dhcpd_subnet.net, dhcpd_subnet.mask, dhcpd_sharenet.vlan_id FROM dhcpd_map, dhcpd_sharenet, dhcpd_subnet WHERE ((((dhcpd_map.owner_id = dhcpd_sharenet.id) AND (dhcpd_map.owner_type = dhcpd_sharenet.type)) AND (dhcpd_subnet.id = dhcpd_map.id)) AND (dhcpd_subnet.type = dhcpd_map.type));;

CREATE VIEW dhcp_all_subnet_host AS SELECT dhcp_all_subnet.subnet_id, dhcp_all_subnet.subnet_type, dhcp_all_host.host_id, dhcp_all_host.host_type, dhcp_all_subnet.vlan_id, dhcp_all_subnet.net, dhcp_all_subnet.mask, dhcp_all_host.fixed_ip, dhcp_all_host.hardware_mac, dhcp_all_host.name FROM dhcp_all_host, dhcp_all_subnet, dhcpd_map WHERE ((((dhcp_all_subnet.subnet_id = dhcpd_map.owner_id) AND (dhcp_all_subnet.subnet_type = dhcpd_map.owner_type)) AND (dhcpd_map.id = dhcp_all_host.host_id)) AND (dhcpd_map.type = dhcp_all_host.host_type));;

CREATE VIEW dhcp_all_subnet_range AS SELECT dhcp_all_subnet.vlan_id, dhcp_all_subnet.net, dhcp_all_subnet.mask, dhcp_all_subnet.subnet_id, dhcp_all_subnet.subnet_type, dhcp_all_gateway.routers AS gateway, dhcp_all_gateway.subnet_mask AS client_mask, dhcp_all_range.range_id, dhcp_all_range.range_type, dhcp_all_range.ip_start, dhcp_all_range.ip_end FROM dhcp_all_subnet, dhcp_all_gateway, dhcp_all_range WHERE ((((dhcp_all_subnet.subnet_id = dhcp_all_gateway.subnet_id) AND (dhcp_all_subnet.subnet_type = dhcp_all_gateway.subnet_type)) AND (dhcp_all_subnet.subnet_id = dhcp_all_range.subnet_id)) AND (dhcp_all_subnet.subnet_type = dhcp_all_range.subnet_type)) ORDER BY dhcp_all_subnet.vlan_id, dhcp_all_subnet.net, dhcp_all_subnet.mask;;

--
-- Name: config_tcu_select; Type: view; 
--
CREATE VIEW config_tcu_select AS SELECT 
    h.cpu_info_id, h.processor_num, 
    floor(cast(cpu_bench as numeric(18,2))/10000/h.processor_num) as system_tcu_num , 
    COALESCE(m.tcu_num,0) as config_tcu_num ,m.description
    from host_cpuinfo h left join config_tcu m on h.cpu_info_id = m.cpu_info_id where h.processor_num !=0;


--
-- Name: config_tcu_useful; Type: VIEW; 
--
CREATE VIEW config_tcu_useful AS SELECT 
    cpu_info_id,processor_num,(case when  config_tcu_num >0 then config_tcu_num else system_tcu_num end ) 
    as tcu_useful from config_tcu_select ;

    
--
-- Name: host_in_use_list; Type: VIEW; 
--
--CREATE OR REPLACE VIEW host_in_use_list AS 
  --  SELECT hid,vcpu,tcu,(memory+9*1024*1024) as memory,localdisk,vcpu*tcu as used_tcu FROM vm_pool UNION ALL 
  --  SELECT hid,vcpu,tcu,(memory+9*1024*1024) as memory,localdisk,vcpu*tcu as used_tcu FROM vm_cancel_pool;
    
--
-- Name: vms_used; Type: VIEW; 
-- 罗列vm占用的 计算、内存
--log_state=8, 表示冻结
CREATE OR REPLACE VIEW vms_used AS 
    SELECT hid,(memory+9*1024*1024)*cast((log_state != 8) as int) as memory,vcpu*tcu*cast((log_state != 8) as int) as used_tcu FROM vm_pool 
    UNION ALL 
    SELECT hid,(memory+9*1024*1024) as memory,vcpu*tcu as used_tcu FROM vm_cancel_pool
    UNION ALL 
    SELECT hid_next as hid,(memory+9*1024*1024) as memory,vcpu*tcu as used_tcu FROM vm_pool where hid_next is not null;


-- Name: all_vm_local_image_real_backups; Type: VIEW; 
--
 CREATE OR REPLACE VIEW all_vm_local_image_real_backups AS 
     SELECT vi.vid, vi.target, 
         CASE WHEN list.real_backup IS NULL THEN 0 
         ELSE list.real_backup
         END
         AS real_backup   
     FROM vm_image vi
     LEFT JOIN ( SELECT count(*) AS real_backup, vib.vid, vib.target
             FROM vm_image_backup vib GROUP BY vib.vid, vib.target) list 
     ON vi.vid = list.vid AND vi.target::text = list.target::text 
     WHERE vi."position" = 1;

-- Name: all_vm_local_image_backups; Type: VIEW; 
--
CREATE OR REPLACE VIEW all_vm_local_image_backups AS 
    SELECT vi.vid,vi.target,vi.reserved_backup,vrb.real_backup
    FROM vm_image vi,all_vm_local_image_real_backups vrb WHERE vi.vid = vrb.vid AND vi.target = vrb.target;

-- Name: all_vm_images_in_host; Type: VIEW; 
--

CREATE OR REPLACE VIEW all_vm_local_images_in_host AS 
	SELECT * FROM
	(
	    SELECT vi.vid,-1 AS image_id,
	    (CASE WHEN (vrb.reserved_backup > vrb.real_backup) THEN ((vi.occupied_size * vrb.reserved_backup))::numeric(19,0) 
	    ELSE ((vi.occupied_size * vrb.real_backup))::numeric(19,0) 
	    END) AS occupied_size,vi.position,vp.hid
	    FROM vm_image vi,all_vm_local_image_backups vrb,vm_pool vp WHERE vi.vid = vrb.vid AND vi.target = vrb.target AND vi.vid = vp.vid
	    UNION ALL
	    SELECT vm_image.vid,vm_image.image_id,vm_image.occupied_size,vm_image.position,vm_pool.hid
	    FROM vm_image LEFT JOIN vm_pool ON vm_image.vid = vm_pool.vid 
	    UNION ALL
	    SELECT vm_image.vid,vm_image.image_id,vm_image.occupied_size,vm_image.position,vm_pool.hid_next as hid
	    FROM vm_image LEFT JOIN vm_pool ON vm_image.vid = vm_pool.vid where vm_pool.hid_next is not null
	    UNION ALL
	    SELECT vm_image_cancel.vid,vm_image_cancel.image_id,vm_image_cancel.occupied_size,vm_image_cancel.position,vm_image_cancel.hid 
	    FROM vm_image_cancel
	    UNION ALL        
	    SELECT vm_disk.vid,-1,vm_disk.size as occupied_size,vm_disk.position,vm_pool.hid
	    FROM vm_disk LEFT JOIN vm_pool ON vm_disk.vid = vm_pool.vid 
	    UNION ALL
	    SELECT vm_disk.vid,-1,vm_disk.size as occupied_size,vm_disk.position,vm_pool.hid_next as hid
	    FROM vm_disk LEFT JOIN vm_pool ON vm_disk.vid = vm_pool.vid where vm_pool.hid_next is not null
	    UNION ALL
	    SELECT vm_disk_cancel.vid,-1,vm_disk_cancel.size as occupied_size,vm_disk_cancel.position,vm_disk_cancel.hid 
	    FROM vm_disk_cancel        
	    )tmp where position = 1;
	    
--
-- Name: all_host_images; Type: VIEW; 
--
CREATE OR REPLACE VIEW all_host_local_images AS     
    SELECT all_vm_local_images_in_host.hid, all_vm_local_images_in_host.image_id, 
    count(all_vm_local_images_in_host.occupied_size) AS image_count, 
    sum(all_vm_local_images_in_host.occupied_size)::numeric(19,0) AS sum_of_basesize, 
    (sum(all_vm_local_images_in_host.occupied_size) + (select count(1) from config_cluster where config_name='img_usage' and config_value='snapshot')
							* cast((all_vm_local_images_in_host.image_id != -1) as int) * sum(all_vm_local_images_in_host.occupied_size) / count(all_vm_local_images_in_host.occupied_size))::numeric(19,0) AS used
    FROM all_vm_local_images_in_host
    GROUP BY all_vm_local_images_in_host.hid, all_vm_local_images_in_host.image_id
    ORDER BY all_vm_local_images_in_host.hid, all_vm_local_images_in_host.image_id;  
           
--
-- Name: host_using_image; Type: VIEW; 
-- 这个是让调度的时候，优先选择HC上已有对应的image文件，disk就没不要考虑了；SUMZ字段算法是错的，暂时不管了
CREATE OR REPLACE VIEW host_using_image as
    (select COALESCE(a.hid,0)  as hid, b.image_id, SUM(b.occupied_size)  from vm_image b
    LEFT JOIN  vm_pool a on a.vid = b.vid GROUP BY hid, image_id)
    UNION
    (select COALESCE(a.hid_next,0)  as hid, b.image_id, SUM(b.occupied_size)  from vm_image b
    LEFT JOIN  vm_pool a on a.vid = b.vid WHERE a.hid_next != -1 GROUP BY a.hid_next, image_id);

--
-- Name: vm_target_hc; Type: VIEW; 
-- 这个是计算虚拟机最终预计会落到哪个HC上，如果hid_next为空，那就是落到hid_next上。
CREATE OR REPLACE VIEW vm_target_hc AS 
select vid, COALESCE(hid_next, hid) as hid from vm_pool order by vid;

--
-- Name: host_in_use_stat; Type: VIEW; 
--
CREATE OR REPLACE VIEW host_in_use_stat AS 
    SELECT list.hid, list.tcu_used, list.memory_used, COALESCE(image.localdisk_used,0) as localdisk_used, list.running_vms, hc_vms.target_vms
    from(
        SELECT hid, SUM(used_tcu)::integer AS tcu_used,SUM(memory)::numeric(19,0) AS memory_used,COUNT(hid)::integer AS running_vms
        FROM vms_used GROUP BY hid
    )list
    LEFT JOIN (
    SELECT hid, sum(used)::numeric(19,0) AS localdisk_used  FROM all_host_local_images GROUP BY hid
    )image
    on list.hid = image.hid
    LEFT JOIN (
    SELECT hid, count(hid)::integer AS target_vms  FROM vm_target_hc GROUP BY hid
    )hc_vms
    on list.hid = hc_vms.hid;

--
-- Name: host_pool_with_tcu; Type: VIEW; 
--
CREATE VIEW host_pool_with_tcu AS SELECT 
    h.oid, h.name, h.ip_address, h.run_state, 
    h.enabled, h.create_time, h.cpu_info_id, h.vmm_info, h.os_info,     
    h.mem_total, h.mem_max, h.disk_total, h.disk_max, 
    h.cpu_usage_1m, h.cpu_usage_5m, h.cpu_usage_30m, h.hardware_state, 
    h.description,  h.location, h.is_support_hvm, h.running_time, 
    COALESCE(s.tcu_useful, 0)::INTEGER AS tcu_num, 
    h.cpus*COALESCE(s.tcu_useful, 0) as all_tcu,h.cpus 
    FROM host_pool h LEFT JOIN config_tcu_useful s 
    ON h.cpu_info_id = s.cpu_info_id ;

--
-- Name: host_pool_in_use; Type: VIEW; 
--

CREATE VIEW host_pool_in_use AS SELECT 
    h.oid, h.name, h.ip_address, h.run_state, 
    h.enabled, h.create_time, h.cpu_info_id, h.vmm_info, h.os_info,      
    h.mem_total, h.mem_max, h.disk_total, h.disk_max, 
    h.cpu_usage_1m, h.cpu_usage_5m, h.cpu_usage_30m, h.hardware_state, h.tcu_num,h.all_tcu,
    h.description,  h.location, h.is_support_hvm, h.running_time,
    COALESCE(u.running_vms, 0)::INTEGER AS running_vms, 
    COALESCE(u.target_vms, 0)::INTEGER AS target_vms, 
    (h.all_tcu - COALESCE(u.tcu_used, 0))::INTEGER AS all_free_tcu,  
    (h.mem_max - COALESCE(u.memory_used, 0))::NUMERIC(19) AS mem_free,
    (h.disk_max - COALESCE(u.localdisk_used, 0))::NUMERIC(19) AS disk_free,h.cpus 
    FROM host_pool_with_tcu h LEFT JOIN host_in_use_stat u ON h.oid = u.hid ;

--
-- Name: host_pool_in_use_with_cpuinfo; Type: VIEW; 
--
CREATE VIEW host_pool_in_use_with_cpuinfo AS SELECT 
    h.oid, h.name, h.ip_address, h.run_state, 
    h.enabled, h.create_time, h.cpu_info_id, h.vmm_info, h.os_info,     
    h.running_vms, h.target_vms,h.tcu_num, h.all_free_tcu, h.mem_total, h.mem_max, 
    h.mem_free, h.disk_total, h.disk_max, h.disk_free, h.cpu_usage_1m, 
    h.cpu_usage_5m, h.cpu_usage_30m, h.hardware_state, h.description, 
    h.location, h.is_support_hvm, h.running_time,
    c.physical_id_num, c.cpu_cores, c.vendor_id, 
    c.cpu_family, c.model, c.model_name, c.stepping, c.cache_size, 
    c.fpu, c.fpu_exception, c.cpuid_level, c.wp, c.flags, h.cpus
    FROM host_pool_in_use h, host_cpuinfo c 
    WHERE h.cpu_info_id = c.cpu_info_id ;
    
CREATE OR REPLACE VIEW web_view_vmstate AS 
select vid,
	case when log_state = 8 then 8
        when last_moni_at::varchar(19) < (now() - interval '1 minute')::varchar(19) then 6
	    when state = 0 then 4
	else state
	end,
 	case when last_op = 15 or last_op = 16 or last_op = 19 then 16
 	else last_op
 	end,
 	last_op_result,
 	case when last_op = 15 then (last_op_progress * 0.3)::integer
		when last_op = 16 then (last_op_progress * 0.4 + 30)::integer
		when  last_op = 19 then (last_op_progress * 0.3 + 70)::integer
 	else last_op_progress
 	end,
 	last_op_detail,vm_pool.config_version, vm_pool.run_version 
	from vm_pool;

CREATE OR REPLACE VIEW web_view_mac_ip AS 
 SELECT mac_ip.mac, mac_ip.ip
   FROM mac_ip;
   
CREATE OR REPLACE VIEW vms_devices AS 
    SELECT vm_device.vid,vm_device.type,vm_device.count,vm_pool.hid 
    FROM vm_device left join vm_pool on vm_device.vid = vm_pool.vid
    UNION ALL 
    SELECT vm_device.vid,vm_device.type,vm_device.count,vm_pool.hid_next as hid
    FROM vm_device left join vm_pool on vm_device.vid = vm_pool.vid where vm_pool.hid_next is not null
    UNION ALL 
    SELECT vid,type,count,hid 
    FROM vm_device_cancel;
        
CREATE OR REPLACE VIEW host_device_used AS 
    SELECT hid,type,sum(count) as count from vms_devices group by hid,type;
    
CREATE OR REPLACE VIEW host_device_free AS 
    select host_device.hid, host_device.type, ( host_device.number - COALESCE(host_device_used.count,0) ) as free from host_device 
    LEFT JOIN  host_device_used on host_device.hid = host_device_used.hid and  host_device.type = host_device_used.type; 
	
CREATE OR REPLACE VIEW web_view_deploy_image AS 
 SELECT image_cache.cache_id, image_cache.progress
   FROM image_cache;
   
CREATE OR REPLACE VIEW base_image_with_snapshot_count_view AS 
 SELECT cache_id, count(*) AS snapshot_count
   FROM vm_disk_share WHERE cache_id != -1 GROUP BY cache_id;

CREATE OR REPLACE VIEW vmm_workflow AS 
    SELECT workflow_id, rollback_workflow_id, category, name, application, 
       process, engine, paused, create_time, pause_time, description, 
       label_int64_1 as vid, label_int64_2 as operation
  FROM view_workflows
  WHERE category = 'vm' and paused = 0;

CREATE OR REPLACE VIEW host_atca_with_host_pool AS 
SELECT h.hid, h.bureau, h.rack, h.shelf, h.slot, h.board_type, h.power_state, h.power_operate, h.rate_power, h.poweroff_time, h.total_poweroff_time,h.description, 	
p.run_state, p.running_vms
from host_atca h, host_pool_in_use p where h.hid = p.oid and p.enabled = 0; 

CREATE OR REPLACE VIEW host_position_add_cmmip AS
SELECT h.hid, h.bureau, h.rack, h.shelf, h.slot, h.board_type, h.power_state, h.power_operate, 
h.rate_power, h.poweroff_time, h.total_poweroff_time,h.description,h.run_state,h.running_vms,p.ip_address 
from  host_atca_with_host_pool h 
LEFT JOIN cmm_pool p on h.bureau = p.bureau and h.rack  = p.rack and h.shelf = p.shelf;

CREATE OR REPLACE VIEW host_position_group AS 
SELECT bureau,rack,shelf,slot, count(bureau) as repeat_num from host_position_add_cmmip 
group by bureau,rack,shelf,slot;

CREATE OR REPLACE VIEW host_position_delete_repeat AS SELECT 
h.hid, h.bureau, h.rack, h.shelf, h.slot, h.board_type, h.power_state, h.power_operate, 
h.rate_power, h.poweroff_time, h.total_poweroff_time,h.description,h.run_state,h.running_vms,h.ip_address
 from host_position_add_cmmip h 
LEFT JOIN host_position_group p on h.bureau = p.bureau and h.rack  = p.rack and h.shelf = p.shelf and h.slot = p.slot
where  p.repeat_num = 1;

CREATE OR REPLACE VIEW host_position_cmmip_valid AS SELECT 
h.hid, h.bureau, h.rack, h.shelf, h.slot, h.board_type, h.power_state, h.power_operate, 
h.rate_power, h.poweroff_time, h.total_poweroff_time,h.description,h.run_state,h.running_vms,h.ip_address
from host_position_delete_repeat h where  h.ip_address IS NOT NULL;

CREATE OR REPLACE VIEW web_view_vm_workflow AS
  SELECT  wl.label_int64_1 as vid, w.name as workflow_name, p.progress as progress, w.workflow_id as workflow_id  
  FROM workflow w, workflow_progress p, workflow_labels wl
  WHERE w.category ='vm' and w.paused = 0 and w.workflow_id = p.workflow_id and w.workflow_id = wl.workflow_id;

CREATE OR REPLACE VIEW view_host_hwmon AS SELECT 
h.hid, h.temp1_name, h.temp1_value, h.temp2_name, h.temp2_value, h.temp3_name, h.temp3_value,
h.temp4_name, h.temp4_value, h.temp5_name, h.temp5_value, h.temp6_name, h.temp6_value,
h.temp7_name, h.temp7_value, h.temp8_name, h.temp8_value, h.cpu0_vid_name, h.cpu0_vid_value,
h.cpu1_vid_name, h.cpu1_vid_value, h.fan1_rpm_name, h.fan1_rpm_value, h.fan2_rpm_name, h.fan2_rpm_value,
h.fan3_rpm_name, h.fan3_rpm_value, h.fan4_rpm_name, h.fan4_rpm_value, h.fan5_rpm_name, h.fan5_rpm_value,
h.fan6_rpm_name, h.fan6_rpm_value, h.fan7_rpm_name, h.fan7_rpm_value, h.fan8_rpm_name, h.fan8_rpm_value,
h.in0_name, h.in0_value, h.in1_name, h.in1_value, h.in2_name, h.in2_value, h.in3_name, h.in3_value,
h.in4_name, h.in4_value, h.in5_name, h.in5_value, h.in6_name, h.in6_value, h.in7_name, h.in7_value,
h.in8_name, h.in8_value, h.in9_name, h.in9_value, h.in10_name, h.in10_value, h.in11_name, h.in11_value,
h.in12_name, h.in12_value, h.in13_name, h.in13_value, h.in14_name, h.in14_value
from host_hwmon h where  h.hid > (-1);

CREATE OR REPLACE VIEW view_project_dynamic_data AS 
SELECT vms.vid,list.nic_used_rate,vms.cpu_used_rate,vms.mem_used_rate
FROM(SELECT nic.vid, avg(nic.used_rate)::integer AS nic_used_rate
FROM (SELECT DISTINCT ON (vm_statistics_nics.name) vm_statistics_nics.vid,vm_statistics_nics.name ,vm_statistics_nics.statistics_time, 
      vm_statistics_nics.save_time,vm_statistics_nics.used_rate    FROM vm_statistics_nics ORDER BY vm_statistics_nics.name, 
      vm_statistics_nics.statistics_time DESC) nic GROUP BY  nic.vid) list
RIGHT JOIN(SELECT DISTINCT ON (vm_statistics.vid) vm_statistics.vid, vm_statistics.statistics_time, 
           vm_statistics.save_time, vm_statistics.cpu_used_rate, vm_statistics.mem_used_rate
           FROM vm_statistics ORDER BY vm_statistics.vid, vm_statistics.statistics_time DESC) vms ON vms.vid = list.vid;

CREATE OR REPLACE VIEW view_image_storage_local  AS 
 SELECT vmi.vid, vmi.target, 
 CASE WHEN list.image_count >0 THEN ((list.image_count + 1)::numeric * vmi.occupied_size)::numeric(19,0) 
 ELSE vmi.occupied_size
 END
 AS image_storage   FROM vm_image vmi
 LEFT JOIN ( SELECT count(*) AS image_count, vmibk.vid, vmibk.target
             FROM vm_image_backup vmibk GROUP BY vmibk.vid, vmibk.target) 
 list ON vmi.vid = list.vid AND vmi.target::text = list.target::text WHERE vmi."position" = 1;
 
 
 CREATE OR REPLACE VIEW view_storage_local AS 
 SELECT 
        CASE
            WHEN vmd.vid IS NULL THEN vmi.vid
            ELSE vmd.vid
        END AS vid, vmi.image_storage AS image_size, vmd.size AS disk_size, 
        CASE
            WHEN vmd.size IS NULL THEN vmi.image_storage
            WHEN vmi.image_storage IS NULL THEN vmd.size
            ELSE vmd.size + vmi.image_storage
        END AS storage_size
   FROM vm_disk vmd
   FULL JOIN view_image_storage_local vmi ON vmi.vid = vmd.vid
  WHERE vmd."position" = 1;
  
   CREATE VIEW port_number_used AS 
   (SELECT vnc_port as portno FROM vm_pool WHERE vnc_port != 0)
 UNION
   (SELECT serial_port as portno FROM vm_pool WHERE vnc_port != 0);
 