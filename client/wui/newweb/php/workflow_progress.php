<?php
    include('adodb_connect.php');
    
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");
 
    $workflowid       = $_GET["workflow"];
    $clustername      = $_GET["cluster"];
   
    $conn = NULL;

    if($clustername != ""){
        $conn = GetClusterDbConnect_By_ClusterName($clustername, $errcode, $errinfo);
    }
    else {
        $conn = GetTCDbConnect($errcode,$errinfo);
    }
    
    if(!$conn){
        ExitWithError($errinfo);
    }

    $workflowResult = Array();

    $workflowResult[0] = 0;
    $workflowResult[1] = Array();

    $query = ' select name, -1, progress, category FROM webview_workflows';
    $query = $query . " WHERE workflow_id='" . $workflowid ."'";

    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        ExitWithError($query . " execute failed!");
    }  

    if(!$rs->EOF) {
        $oneData = Array();

        $oneData[0] = $rs->fields[0];
        $oneData[1] = $rs->fields[1];
        $oneData[2] = $rs->fields[2];
        $oneData[3] = $workflowid;
        $oneData[4] = $rs->fields[3];
    
        $workflowResult[1] = $oneData;
    } 
    else {    
        $rs->Close(); 

        $query = ' select name, state, detail, category FROM workflow_history';
        $query = $query . " WHERE workflow_id='" . $workflowid ."'";
      
        $rs = $conn->Execute($query);
        if (!$rs){
            $conn->Close();
    		ExitWithError($query . " execute failed!");
        }    

        if (!$rs->EOF) {
            $oneData = Array();    

            $oneData[0] = $rs->fields[0];
    		$oneData[1] = $rs->fields[1];
    		$oneData[2] = $rs->fields[2];
            $oneData[3] = $workflowid;
            $oneData[4] = $rs->fields[3];
        
            $workflowResult[1] = $oneData;
        }
    }

    $rs->Close();
    $conn->Close();

    print json_encode($workflowResult);
?>