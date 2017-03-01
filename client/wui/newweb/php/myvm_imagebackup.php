<?php
    include('adodb_connect.php');
    
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");
	
    $vmid      = $_GET["vmid"]; 
    $clustername      = $_GET["cluster"];
   
    $conn = NULL;
    $conn = GetClusterDbConnect_By_ClusterName($clustername, $errcode, $errinfo);
    
    if(!$conn){
        ExitWithError($errinfo);
    }

    $vmData = Array();
    $vmData[] = 0;  	
	
    $query = 'select request_id, state, create_time, target,description ,parent_request_id FROM vm_image_backup';
    $query = $query . " WHERE vid='" . $vmid ."'";

    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        ExitWithError($query . " execute failed!");
    } 

	
    $ackdata = Array();
    while(!$rs->EOF) {	
        $arrData = Array();
        $arrData[] = $rs->fields[0];//request_id
        $arrData[] = $rs->fields[1];//state
        $arrData[] = $rs->fields[2];//create_time
        $arrData[] = $rs->fields[3];//target		
        $arrData[] = $rs->fields[4];//description				
        $arrData[] = $rs->fields[5];;					
        $ackdata[] = $arrData;
	    $rs->MoveNext();
    } 
    $vmData[] = $ackdata;
    $rs->Close();
    $conn->Close();

    print json_encode($vmData);
?>