<?php
    include('adodb_connect.php');
    include('vm_common.php');
    
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");
    $vmsid          = $_GET["vmsid"];
    $sEcho          = $_GET['sEcho'];
    $vid              = "0";
    $workflow_name    = "0";
    $progress         = "0";
    $workflow_id      = "0";
    $iTotalRecords    = 0;
    
    $tc_conn = GetTCDbConnect($errcode,$errinfo);
    if(!$tc_conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }

	 $query = "select vid,workflow_name,progress,workflow_id ";
     $query = $query . "from web_view_vm_workflow ";
     $query = $query . "where vid in (". $vmsid . ")";
    
	$rs = $tc_conn->Execute($query);
	if (!$rs){
		$tc_conn->Close();
		die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
	}
	
	$ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
	 while (!$rs->EOF) {
	    $arrData = Array();

		$arrData[]      = $rs->fields[0]; //vid
        $arrData[]      = $rs->fields[1]; //workflow_name
        $arrData[]      = $rs->fields[2]; //progress
        $arrData[]      = $rs->fields[3]; //workflow_id
		$arrData[]      = "";             //cluster, tc is blank space
		
        $ackdata["data"]["aaData"][] = $arrData;		
		$rs->MoveNext();
     }	 
	$rs->Close();
	$tc_conn->Close();
    print json_encode($ackdata);
?>
