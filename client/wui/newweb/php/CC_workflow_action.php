<?php
    include('adodb_connect.php');
    
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");
    
    $clusterName    = $_GET["clusterName"];
    //$sEcho          = $_GET['sEcho'];   
    $workflowid      = $_GET["workflowid"];
   
    $conn = GetClusterDbConnect_By_ClusterName($clusterName,&$errcode,&$errinfo);
    
    if(!$conn){
        ExitWithError($errinfo);
    }
    $filter = " 1=1 " ;
	$filter = $filter . " and ";
	$filter = $filter . "workflow_id='" . $workflowid ."'";


    $query = ' select action_id,category,name,progress'; 
    $query = $query . ' FROM workflow_actions';
    $query = $query . ' WHERE' . $filter.' order by progress desc';
  
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
		ExitWithError($query . " execute failed!");
    }
    $workflowActions = Array();
	$workflowActions[0] = 0;
	$workflowActions[1] = Array();
    while (!$rs->EOF) {
        $oneData = Array();
        $oneData[0] = $rs->fields[0];
		$oneData[1] = $rs->fields[1];
		$oneData[2] = $rs->fields[2];
		$oneData[3] = $rs->fields[3];         
        
        $workflowActions[1][] = $oneData;
        
        $rs->MoveNext();
    }
    $rs->Close();
    $conn->Close();
    print json_encode($workflowActions);
?>