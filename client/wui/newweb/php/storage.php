<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $tc_db_conn = GetTCDbConnect($err_code, $err_info);
    if(!$tc_db_conn){
        ExitWithError($err_info);
    }

    $storage_query = "select * from storage_adaptor";

    $rs = $tc_db_conn->Execute($storage_query);
    if (!$rs){
        ExitWithError($storage_query . " execute_failed");
    }

    $storage = Array();
    $storage[0] = 0;
    $storage[1] = Array();

    while(!$rs->EOF){
        $oneSc = Array();
        $sid = $oneSc["sid"]     = $rs->fields[0];
        $oneSc["name"]           = $rs->fields[1];
        $oneSc["sa_name"]        = $rs->fields[2];
        $oneSc["ctrl_address"]   = $rs->fields[3];
        $oneSc["type"]           = $rs->fields[4];
        $oneSc["is_online"]      = $rs->fields[5];
        $oneSc["enabled"]        = $rs->fields[6];
        $oneSc["description"]    = $rs->fields[7];
        $oneSc["register_time"]  = $rs->fields[8];

        $storage_cluter_map_query = "select sid, cluster_name from storage_cluster where sid = " . $sid;
        $storage_cluter_map_query = $storage_cluter_map_query . " and exists(select name from cluster_pool where storage_cluster.cluster_name = cluster_pool.name)";
        $rs1 = $tc_db_conn->Execute($storage_cluter_map_query); 
        
        if (!$rs1){
            ExitWithError($storage_cluter_map_query . " execute_failed");
        }

        $clusters = Array();
        while(!$rs1->EOF){
            $clusters[] = $rs1->fields[1];

            $rs1->MoveNext();
        }
        $oneSc["clusters"] = $clusters; 

        $storage[1][] = $oneSc;

        $rs->MoveNext();
    }

    $rs->Close();

    print json_encode($storage);
?>