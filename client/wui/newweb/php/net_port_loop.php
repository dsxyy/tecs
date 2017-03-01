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
    $sSearch        = "";//$_GET["sSearch"];
    $sVNAid         = $_GET["sVNAid"];
    
 	$iTotalRecords = 0;
	$filed = array("port_name");
    $orderfield = "";
    $orderdir = "";
    $order = "";
    
    $conn = GetVNMDbConnect($errcode,$errinfo);
    
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }
    $filter = " 1 = 1"; 
    $filter = $filter . " and  vna_uuid = '" . $sVNAid . "'";
    
    if($sSearch != ""){
        $filter = $filter . " and ( ";
        $filter = $filter . " port_name like " . "'%" . $sSearch . "%'";
        //$filter = $filter . " or to_char(pg_type,'99999999999999999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . ")";
    }
    if($iSortCol == "0"){
        $orderfield = $filed[0];
    }
    else{
        $orderfield = $filed[intval($iSortCol) - 1];
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
    $query = "SELECT COUNT(*) FROM web_view_port_uplinkloop WHERE " . $filter;
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
    $query = ' select vna_uuid,port_name,port_uuid';
    $query = $query . ' from web_view_port_uplinkloop';
    $query = $query . ' WHERE' . $filter;
    $query = $query . ' ORDER by' . $order;
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
     
    $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
    while (!$rs->EOF) {
        $oneport = Array();
		$oneport["vna_uuid"]     = $rs->fields[0];
        $oneport["port_name"]    = $rs->fields[1];
        $oneport["port_uuid"]    = $rs->fields[2];
        $ackdata["data"]["aaData"][] = $oneport;
        
        $rs->MoveNext();
    }
    $rs->Close();
    $conn->Close();
	
	print json_encode($ackdata);   
?>