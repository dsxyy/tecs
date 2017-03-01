<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $sid = $_GET["sid"];

    $tc_db_conn = GetTCDbConnect($err_code, $err_info);
    if(!$tc_db_conn){
        ExitWithError($err_info);
    }

    $storage_cluter_map_query = "select cluster_name from storage_cluster where sid = " . $sid;

    $rs = $tc_db_conn->Execute($storage_cluter_map_query);
    if (!$rs){
        ExitWithError($storage_cluter_map_query . " execute failed!");
    }

    $sa_cluster = Array();
    while(!$rs->EOF){
        $sa_cluster[] = $rs->fields[0];
        $rs->MoveNext();
    }
    $rs->Close();

    $cc_query = "select name from cluster_pool";
    $rs = $tc_db_conn->Execute($cc_query);
    if (!$rs){
        ExitWithError($cc_query . " execute failed!");
    }

    $allcc = Array();
    while(!$rs->EOF){
        $onecc = Array();
        $onecc["name"] = $rs->fields[0];
        if(in_array($onecc["name"], $sa_cluster)){
           $onecc["maped"] = 1;
        }
        else {
           $onecc["maped"] = 0;
        }
        $allcc[] = $onecc;

        $rs->MoveNext();
    }
    $rs->Close();

    print json_encode($allcc);
?>