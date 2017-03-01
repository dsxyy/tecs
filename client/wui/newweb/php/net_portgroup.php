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
    $netplane       = $_GET["netplane"];
    
 	$iTotalRecords = 0;
	$filed = array("name","pg_type", "mtu", "isolate_type", "netplane_name");
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
        $filter = $filter . " name like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(pg_type,'99999999999999999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . ")";
    }

    if($netplane != ""){
        $filter = $filter . " and netplane_name in (" . $netplane . ")";
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
    $query = "SELECT COUNT(*) FROM web_view_portgroup WHERE " . $filter;
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
    $query = ' select uuid, name, pg_type, switchport_mode, netplane_name, netplane_id, description, mtu, native_vlan_num,isolate_type ';
    $query = $query . ' from web_view_portgroup';
    $query = $query . ' WHERE' . $filter;
    $query = $query . ' ORDER by' . $order;
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
     
    $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
    while (!$rs->EOF) {
        $onePg = Array();
        $onePg["uuid"]    = $rs->fields[0];//checkbox
        $onePg["name"]    = $rs->fields[1];//name
        $onePg["pg_type"] = $rs->fields[2];//type
        $onePg["switchport_mode"] = $rs->fields[3];//version          
        $onePg["netplane"] = $rs->fields[4];//netplane_name
        $onePg["netplane_id"] = $rs->fields[5];//netplane_id
        $onePg["desc"]    = $rs->fields[6];//description           
        $onePg["mtu"] = $rs->fields[7];//mtu
        $onePg["native_vlan_num"] = $rs->fields[8];//native_vlan_num
        $onePg["isolateType"] = $rs->fields[9];//isolate_type

        $onePg[] = "";
        if($netplane == "" || strstr($netplane, $onePg["netplane"])){
            $ackdata["data"]["aaData"][] = $onePg;
        }
        
        $rs->MoveNext();
    }
    $rs->Close();
    $conn->Close();

	print json_encode($ackdata);   
?>