<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $vid          = $_GET["vid"];
    
    $cluster_name = "";

    $res          = Array(); 

    $tc_db_conn = GetTCDbConnect($errcode, $errinfo);
    if(!$tc_db_conn){
    	ExitWithError($errinfo);
    }

    $query_cc = "select deployed_cluster from web_view_vm where vid=" . $vid;

    $res_cc = $tc_db_conn->Execute($query_cc);
    if(!$res_cc){
        ExitWithError($query_cc . "execute failed!");
    }
    else{
    	if(!$res_cc->EOF){
            $cluster_name = $res_cc->fields[0];
    	}
    	$res_cc->Close();    	
    }

    $cc_db_conn = GetClusterDbConnect_By_QueryCluster($tc_db_conn, $cluster_name, $errcode, $errinfo);

    if(!$cc_db_conn){
        ExitWithError($errinfo);
    }

    $order = " order by statistics_time asc";

    $query_cpu_mem = "select statistics_time,cpu_used_rate,mem_used_rate from vm_statistics where vid=" . $vid . $order;

    $res_cpu_mem = $cc_db_conn->Execute($query_cpu_mem);

    if(!$res_cpu_mem){
    	ExitWithError($query_cpu_mem . " execute failed!");
    }
    else{
    	$res["cpu"] = Array();
    	$res["mem"] = Array();

        while (!$res_cpu_mem->EOF) {
        	$res["cpu"][] = $res_cpu_mem->fields[1];
        	$res["mem"][] = $res_cpu_mem->fields[2];
           
            $res_cpu_mem->MoveNext();        	
        }

        $res_cpu_mem->Close();
    }

    $query_nic = "select statistics_time,name,used_rate from vm_statistics_nics where vid=" . $vid . $order;

    $res_nic = $cc_db_conn->Execute($query_nic);

    if(!$res_nic){
    	ExitWithError($query_nic . " execute failed!");
    }
    else{
    	$res["nic"] = Array();

        while (!$res_nic->EOF) {
            if($res["nic"][$res_nic->fields[1]] == null){
            	$res["nic"][$res_nic->fields[1]] = Array();
            }

        	$res["nic"][$res_nic->fields[1]][] = $res_nic->fields[2];

            $res_nic->MoveNext();        	
        }

        $res_nic->Close();
    }

    $cc_db_conn->Close();
    $tc_db_conn->Close();

    print json_encode($res);    
?>