<?php
    include('adodb_connect.php');
    
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");
    
    $cluster_name     = $_GET["cluster_name"];
    $cache_id         = $_GET["cache_id"];
    $sEcho            = "";
    $iTotalRecords    = "0";
    $cc_conn = GetClusterDbConnect_By_ClusterName($cluster_name,$errcode,$errinfo);
    if(!$cc_conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }
    
    //查表数据
    $query = ' SELECT snapshot_count ';
    $query = $query . ' FROM base_image_with_snapshot_count_view';
    $query = $query . ' WHERE';
    $filter = " cache_id = '" . $cache_id . "'";
    $query = $query . $filter;
    $rs = $cc_conn->Execute($query);
    if (!$rs){
        $cc_conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
    $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
    while (!$rs->EOF) {
        $arrData = Array();
        $arrData[] = $rs->fields[0];//snapshot_count
        $ackdata["data"]["aaData"][] = $arrData;
        $rs->MoveNext();
    }
    $rs->Close();
    $cc_conn->Close();
    print json_encode($ackdata);
    
?>
