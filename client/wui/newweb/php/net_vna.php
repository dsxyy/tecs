<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $sEcho          = $_GET['sEcho'];
    $iDisplayStart  = $_GET['iDisplayStart'];
    $iDisplayLength = $_GET['iDisplayLength'];
    $iSortCol       = $_GET["iSortCol_0"];
    $sSortDir       = $_GET["sSortDir_0"];
    $sSearch        = $_GET["sSearch"];

    $iTotalRecords = 0;

    $filed = array("vna_uuid", "vna_application", "hc_application");

    $orderfield = "";
    $orderdir = "";
    $order = "";
    
    $conn = GetVNMDbConnect($errcode,$errinfo);
    
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }

    $filter = " 1 = 1"; 
    
    if($sSearch != ""){
        $filter = $filter . " and ( ";
        $filter = $filter . " vna_uuid like " . "'%" . $sSearch . "%'";        
        $filter = $filter . " or vna_application like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or hc_application like " . "'%" . $sSearch . "%'";
        $filter = $filter . ")";
    }

    if($iSortCol == "0"){
        $orderfield = $filed[0];
    }
    else{
        $orderfield = $filed[intval($iSortCol)];
    }

    if($sSortDir == ""){
        $orderdir = "asc";
    }
    else{
        $orderdir = $sSortDir;
    } 
     
    $order = " " . $orderfield . " " . $orderdir;
    $order = $order . " LIMIT " . $iDisplayLength . " OFFSET " . $iDisplayStart;

    //表记录数
    $query = "SELECT COUNT(distinct vna_uuid) FROM web_view_vna WHERE " . $filter;
    $totalnumrs = $conn->Execute($query);
    if (!$totalnumrs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
    while (!$totalnumrs->EOF) {
        $iTotalRecords = $totalnumrs->fields[0];
        break;
    }
    $totalnumrs->Close(); 
     
    //查表数据
    $query = ' select vna_uuid, vna_application, hc_application, vna_is_online, port_total_num';
    $query = $query . ' from web_view_vna ';
    $query = $query . ' WHERE' . $filter;
    $query = $query . ' ORDER by' . $order;
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
     
    $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
    while (!$rs->EOF) {
        $oneVna = Array();
        $oneVna[] = $rs->fields[0];//vna_uuid
        $oneVna[] = $rs->fields[1];//vna_application
        $oneVna[] = $rs->fields[2];//host_name
        $oneVna[] = $rs->fields[3];//vna_is_online
        $oneVna[] = $rs->fields[4];//port_total_num        

        $ackdata["data"]["aaData"][] = $oneVna;
        
        $rs->MoveNext();
    }
    $rs->Close();

    $conn->Close();

    /* 
    $oneVna = Array();

    $oneVna[] = "testdata";
    $oneVna[] = "testdata";
    $oneVna[] = "testdata";
    $oneVna[] = 22;

    $ackdata["data"]["aaData"][] = $oneVna;
    */
    print json_encode($ackdata);  
?>