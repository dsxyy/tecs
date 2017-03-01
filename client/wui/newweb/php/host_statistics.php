<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $cluster_name = $_GET["cluster"];
    $host_name    = $_GET["host"];

    $res          = Array(); 

    $cc_db_conn = GetClusterDbConnect_By_ClusterName($cluster_name, $errcode, $errinfo);

    if(!$cc_db_conn){
        ExitWithError($errinfo);
    }

    $query_hid = "select oid from host_pool_in_use where name='" . $host_name . "'";

    $hid = -1;

    $res_hid = $cc_db_conn->Execute($query_hid);
    if(!$res_hid){
    	ExitWithError($query_hid . " execute failed!");
    }
    else{
    	if(!$res_hid->EOF){
    	    $hid = $res_hid->fields[0];
    	}
    	$res_hid->Close();
    }

    $order = " order by statistics_time asc";

    $query_cpu_mem = "select statistics_time,cpu_used_rate,mem_used_rate from host_statistics where hid=" . $hid . $order;

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


    $query_nic = "select statistics_time,name,used_rate from host_statistics_nics where hid=" . $hid . $order;

    $res_nic = $cc_db_conn->Execute($query_nic);

    if(!$res_nic){
    	ExitWithError($query_cpu_mem . " execute failed!");
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

    print json_encode($res);    
?>