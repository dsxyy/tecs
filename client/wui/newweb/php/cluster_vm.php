<?php
    include('adodb_connect.php');
    include('vm_common.php');

    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $cluster_name   = $_GET["cluster_name"];

    $tcdb_conn = GetTCDbConnect($err_code, $err_info);
    if(!$tcdb_conn){
        ExitWithError($err_info);
    }

    $ccdb_conn = GetClusterDbConnect_By_QueryCluster($tcdb_conn, $cluster_name, $err_code, $err_info);
    if(!$ccdb_conn){
        ExitWithError($err_info);
    }

    $vmData = Array();
    $vmData[] = 0;  

    $tc_query = "select vid,vm_name,vm_create_time,vcpu,tcu,memory,total_size,user_name,project_name,deployed_cluster, ";
    $tc_query = $tc_query . ' (select workflow_id from web_view_vm_workflow w where w.vid=web_view_vm.vid limit 1) as workflow '; 
    $tc_query = $tc_query . " from web_view_vm where deployed_cluster='" . $cluster_name . "'";   

    $tc_rs = $tcdb_conn->Execute($tc_query);

    if ($tc_rs){
        $rawData = Array();
        while (!$tc_rs->EOF) {
            $oneRow = Array();
            $oneRow["vid"]            = $tc_rs->fields[0];
            $oneRow["vm_name"]        = $tc_rs->fields[1];
            $oneRow["create_time"]    = $tc_rs->fields[2];
            $oneRow["vcpu"]           = $tc_rs->fields[3];
            $oneRow["tcu"]            = $tc_rs->fields[4];
            $oneRow["memory"]         = $tc_rs->fields[5];
            $oneRow["disk"]           = $tc_rs->fields[6];
            $oneRow["owner"]          = $tc_rs->fields[7];
            $oneRow["project"]        = $tc_rs->fields[8];            
            $oneRow["cluster"]        = $tc_rs->fields[9];
            $oneRow["workflow"]       = $tc_rs->fields[10];
            $oneRow["host"]           = ""; 
            $oneRow["state"]          = 255;

            $cc_query = "select v.vid,h.name,w.state from web_view_vmstate w, vm_pool v, host_pool h where v.hid=h.oid and w.vid=v.vid and v.vid=" . $oneRow["vid"];
            $cc_rs = $ccdb_conn->Execute($cc_query);
            if ($cc_rs && !$cc_rs->EOF){
                 $oneRow["host"]  = $cc_rs->fields[1];
                 $oneRow["state"] = $cc_rs->fields[2];
            }
            $cc_rs->Close();

            $oneRow["ip"] = "";
            $ip           = "";
            $ret = GetVmIp($oneRow["vid"], $err_code, $err_info, $ip);
            if(!$ret){
                $ip = $errinfo;               
            }
            else {
                $oneRow["ip"] = $ip;
            }
         
            $rawData[] = $oneRow;

            $tc_rs->MoveNext();
        }
        $vmData[] = $rawData;
        $tc_rs->Close();
        print json_encode($vmData);
    }
    else{
        ExitWithError($tc_query . " execute failed!");
    }
?>