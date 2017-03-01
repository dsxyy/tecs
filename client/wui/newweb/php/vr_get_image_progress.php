<?php
    include('adodb_connect.php');
    
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");
    
    $image_ids = $_GET["image_ids"];
    $sEcho              = "";
    $iTotalRecords      = "0";
    
    $tc_conn = GetTCDbConnect($errcode,$errinfo);
    if(!$tc_conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }
	//暂时直接从TC中读取web_view_image_workflow数据
	$imageArray = explode(",",$image_ids);
    $query = "select image_id,base_uuid,cluster_name,progress,workflow_name,workflow_id from web_view_image_workflow ";
	$query = $query . " where image_id in ( " . $image_ids . " )";
    
	$rs = $tc_conn->Execute($query);
	if (!$rs){
		$tc_conn->Close();
		die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
	}
	
	$ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
	 while (!$rs->EOF) {
	    
	    $arrData = Array();
		$arrData[]     = $rs->fields[0];//image_id
		$arrData[]     = $rs->fields[1];//base_uuid
		$arrData[]     = $rs->fields[2];//cluster_name
		$arrData[]     = $rs->fields[3];//progress
		$arrData[]     = $rs->fields[4];//workflow_name
		$arrData[]     = $rs->fields[5];//workflow_id
		
        $ackdata["data"]["aaData"][] = $arrData;		
		$rs->MoveNext();
     }	 
	$rs->Close();
	$tc_conn->Close();
    print json_encode($ackdata);
?>
