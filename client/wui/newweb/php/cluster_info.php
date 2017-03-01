<?php
    include("adodb_connect.php");
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $cluster_name = $_GET["cluster"];

    $tc_db_conn = GetTCDbConnect($errcode, $errinfo);
    if(!$tc_db_conn){
        ExitWithError($errinfo);
    }

    $query_cluster_total_share_disk = "SELECT sum(sv.size) FROM storage_vg sv, storage_cluster sc WHERE sv.sid=sc.sid AND sc.cluster_name='" . $cluster_name . "'";

    $rs = $tc_db_conn->Execute($query_cluster_total_share_disk);
    if(!$rs){
        ExitWithError($query_cluster_total_share_disk . " execute failed!");
    }
    else{
        $total = $rs->fields[0];
        if($total == null) $total = 0;
        print json_encode($total);
    }    
    $rs->Close();
?>