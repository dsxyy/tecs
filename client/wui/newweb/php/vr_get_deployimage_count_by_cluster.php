<?php
    include('adodb_connect.php');
    
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");
    
    $cluster_name     = $_GET["cluster_name"];
    $image_id         = $_GET["image_id"];
    $sEcho            = "";
    $iTotalRecords    = "0";
    $conn = GetTCDbConnect($errcode,$errinfo);
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }
    
    //查表数据
    $query = ' SELECT count(*) ';
    $query = $query . ' FROM web_view_deployedimage';
    $query = $query . ' WHERE';
    $filter = " cluster_name = '" . $cluster_name . "'";
    $filter = $filter . " and image_id = '" . $image_id . "'";
    $query = $query . $filter;
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
    $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
    while (!$rs->EOF) {
        $arrData = Array();
        $arrData[] = $rs->fields[0];//count
        $ackdata["data"]["aaData"][] = $arrData;
        $rs->MoveNext();
    }
    $rs->Close();
    $conn->Close();
    print json_encode($ackdata);
    
?>
