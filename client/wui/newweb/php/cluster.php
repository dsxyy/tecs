<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $tc_db_conn = GetTCDbConnect($err_code, $err_info);
    if(!$tc_db_conn){
        ExitWithError($err_info);
    }

    $cluster_query = "select * from cluster_pool order by name asc";
    $rs = $tc_db_conn->Execute($cluster_query);
    if (!$rs){
        ExitWithError($cluster_query . " excute failed!");
    }

    $cc = Array();
    $cc[0] = 0;
    $cc[1] = Array(); 
    while (!$rs->EOF) {
        $one_cc = Array();
        $one_cc["name"] = $rs->fields[1];
        $one_cc["desc"] = $rs->fields[2];
        $one_cc["register_time"] = $rs->fields[3];
        $one_cc["enabled"] = $rs->fields[4];
        $one_cc["online"] = $rs->fields[5]; 
        $one_cc["core_free_max"] = $rs->fields[6];
        $one_cc["tcu_unit_free"] = $rs->fields[7];
        $one_cc["tcu_max_total"] = $rs->fields[8];
        $one_cc["tcu_free_total"] = $rs->fields[9];
        $one_cc["tcu_free_max"] = $rs->fields[10];
        $one_cc["disk_max_total"] = $rs->fields[11];
        $one_cc["disk_free_total"] = $rs->fields[12];
        $one_cc["disk_free_max"] = $rs->fields[13];
        $one_cc["mem_max_total"] = $rs->fields[14];
        $one_cc["mem_free_total"] = $rs->fields[15]; 
        $one_cc["mem_free_max"] = $rs->fields[16];
        $one_cc["host_count"] = $rs->fields[25];
        $one_cc["ip_address"] = "";
        $one_cc["share_disk_max_total"] = 0;
        $one_cc["share_disk_free_total"] = 0;
        
        $cc_ip_query = "select ip_address from cluster_node where is_master=1 and cluster_name='" . $one_cc["name"] . "'";
        $ip_rs = $tc_db_conn->Execute($cc_ip_query);
        if($ip_rs && $ip_rs->RecordCount() > 0){
            $one_cc["ip_address"] = $ip_rs->fields[0];
        }
        $ip_rs->Close();

        $cc_sharedisk_total_query = "SELECT sum(sv.size) FROM storage_vg sv, storage_cluster sc, storage_adaptor sa WHERE sv.sid=sc.sid AND sa.sid=sv.sid AND sa.is_online = 1 AND sc.cluster_name='" . $one_cc["name"] . "'";
        $total_rs = $tc_db_conn->Execute($cc_sharedisk_total_query);
        if($total_rs && !$total_rs->EOF){
            $one_cc["share_disk_max_total"] = $total_rs->fields[0] * 1024 * 1024;
        }
        $total_rs->Close();

        $cc_sharedisk_free_query = "select * from storage_cluster_total_free_size_view where cluster_name='" . $one_cc["name"] . "'";
        $free_rs = $tc_db_conn->Execute($cc_sharedisk_free_query);
        if($free_rs && !$free_rs->EOF){
            $one_cc["share_disk_free_total"] = $free_rs->fields[1] * 1024 * 1024;
        }
        $free_rs->Close();

        $cc[1][] = $one_cc;
        $rs->MoveNext();
    }
    $rs->Close();

    print json_encode($cc);
?>