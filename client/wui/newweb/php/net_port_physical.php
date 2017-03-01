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

    $filed = array("name", "loop", "duplex", "speed", "state", "is_linked", "is_sriov_port", "is_consistency");

    $orderfield = "";
    $orderdir = "";
    $order = "";
 
    $conn = GetVNMDbConnect($errcode,$errinfo);
    
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }

    $filter = " 1 = 1"; 
	$filter = $filter . " and vna_uuid = '" . $sVNAid ."'";
    
    if($sSearch != ""){
        $filter = $filter . " and ( ";
        $filter = $filter . " name like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(state,'99999999999999999') like " . "'%" . $sSearch . "%'";
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
    $query = "SELECT COUNT(distinct uuid) FROM web_view_port_physical WHERE " . $filter;
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
    $query = ' select name, sriov_port_is_loop_report,sriov_port_is_loop_cfg, duplex, speed, state, is_linked, is_sriov_port, is_consistency,uuid,';
	$query = $query . ' (select ip from web_view_port_ip where web_view_port_ip.port_uuid = web_view_port_physical.uuid ) as ip';
    $query = $query . ' from web_view_port_physical';
    $query = $query . ' WHERE' . $filter;
    $query = $query . ' ORDER by' . $order;
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
     
    $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
    while (!$rs->EOF) {
        $one = Array();
        $one[] = $rs->fields[0];
        if($rs->fields[7] == 1){//sriov类型的网卡可设置loop
            /*if($rs->fields[1] == 1){//上报类型为loop,不可再设置
                $one[] = "";
            }
            else {*/
                $one[] = $rs->fields[2];
           // }
        }
        else{
            $one[] = "";
        }

        $one[] = $rs->fields[3];
        $one[] = $rs->fields[4];

        if($rs->fields[5] ==1){//state
            $one[] = "up";
        }
        else{
            $one[] = "down";
        }
       
        if($rs->fields[6] ==1){
            $one[] = "yes";
        }
        else{
            $one[] = "no";
        }
        if($rs->fields[7] ==1){
            $one[] = "yes";
        }
        else{
            $one[] = "no";
        }
       // $one[] = $rs->fields[7];
  
        if($rs->fields[8] ==0){//is_consistency
            $one[] = "yes";
        }
        else{
            $one[] = "no";//不冲突
        }
        $one[] = $rs->fields[9];
        $one[] = $rs->fields[10];//ip

        $ackdata["data"]["aaData"][] = $one;
        
        $rs->MoveNext();
    }
    $rs->Close();

    $conn->Close(); 

    print json_encode($ackdata);  
?>