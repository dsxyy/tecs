<?php
    include('adodb_connect.php');
    include('vm_common.php');
    
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");
    
    $sEcho          = "";
    $user_name      = $_GET["user_name"];
    
    $deploy_cnt        = 0;
    $configuration_cnt = 0;
    //$cluster_abnormal_cnt = 0;
    $iTotalRecords = 1;
    $conn = GetTCDbConnect($errcode,$errinfo);
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }

    $filter = " 1=1";
    if($user_name != ""){
        $filter = " user_name = " . "'" . $user_name . "'";
    }
    $config_filter = $filter . " and deployed_cluster is null";
    //获取虚拟机(拟制)个数
    $query = "SELECT COUNT(*) FROM web_view_vm WHERE " . $config_filter;
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
    while (!$rs->EOF) {
        $configuration_cnt = $rs->fields[0];
        $rs->MoveNext();
    }
    $rs->Close();
    //获取虚拟机(已部署)记录
    $deploy_filter = $filter . " and deployed_cluster is not null";
    $query = ' SELECT COUNT(*) ';
    $query = $query . ' FROM web_view_vm';
    $query = $query . ' WHERE' . $deploy_filter;
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
    
    while (!$rs->EOF) {
        $deploy_cnt = $rs->fields[0];
        $rs->MoveNext();
    }
    
    $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
    $arrData = Array();
    
    $arrData[] = $configuration_cnt;
    $arrData[] = $deploy_cnt;
    $ackdata["data"]["aaData"][] = $arrData;
    $rs->Close();
    $conn->Close();
    print json_encode($ackdata);
?>
