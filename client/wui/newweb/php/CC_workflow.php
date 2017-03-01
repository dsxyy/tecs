<?php
    include('adodb_connect.php');
    
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $clusterName    = $_GET["clusterName"];

    $iTotalRecords = 0;

    $filed = array("workflow_id","category","name","process","engine","create_time","description","progress","paused");
    $orderfield = "";
    $orderdir = "";
    $order = "";

    $conn = GetClusterDbConnect_By_ClusterName($clusterName,&$errcode,&$errinfo);
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }
    $filter = " 1=1 " ;

    //���¼��
    $query = "SELECT COUNT(*) FROM webview_workflows WHERE " . $filter;
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
    //�������
    $query = ' select workflow_id,category,name,process,engine,create_time,description,progress,paused,label_int64_1,label_int64_2,label_int64_3,label_int64_4,label_int64_5,label_int64_6,label_int64_7,label_int64_8,label_int64_9,label_string_1,label_string_2,label_string_3,label_string_4,label_string_5,label_string_6,label_string_7,label_string_8,label_string_9 '; 
    $query = $query . ' FROM webview_workflows';
    $query = $query . ' WHERE' . $filter;
    $query = $query . ' ORDER by create_time desc';
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
    $ackdata = GetQueryAckData("","",0,$iTotalRecords);
    while (!$rs->EOF) {
        $arrWorkflowsData = Array();
		$arrWorkflowsData[] = '';
        $arrWorkflowsData[] = $rs->fields[0];
		$arrWorkflowsData[] = $rs->fields[1];
		$arrWorkflowsData[] = $rs->fields[2];
		$arrWorkflowsData[] = $rs->fields[3];
		$arrWorkflowsData[] = $rs->fields[4];
		$arrWorkflowsData[] = $rs->fields[5];
    
        $description = $rs->fields[6];
        $description = str_replace("\n","<br/>",htmlspecialchars($description));
        $arrWorkflowsData[] = $description;
		
        $arrWorkflowsData[] = $rs->fields[7];  
        $arrWorkflowsData["paused"] = $rs->fields[8];
		$arrWorkflowsData["label_int64_1"] = $rs->fields[9];
		$arrWorkflowsData["label_int64_2"] = $rs->fields[10];
		$arrWorkflowsData["label_int64_3"] = $rs->fields[11];
		$arrWorkflowsData["label_int64_4"] = $rs->fields[12];
		$arrWorkflowsData["label_int64_5"] = $rs->fields[13];
		$arrWorkflowsData["label_int64_6"] = $rs->fields[14];
		$arrWorkflowsData["label_int64_7"] = $rs->fields[15];
		$arrWorkflowsData["label_int64_8"] = $rs->fields[16];
		$arrWorkflowsData["label_int64_9"] = $rs->fields[17];
		$arrWorkflowsData["label_string_1"] = $rs->fields[18];
		$arrWorkflowsData["label_string_2"] = $rs->fields[19];
		$arrWorkflowsData["label_string_3"] = $rs->fields[20];
		$arrWorkflowsData["label_string_4"] = $rs->fields[21];
		$arrWorkflowsData["label_string_5"] = $rs->fields[22];
		$arrWorkflowsData["label_string_6"] = $rs->fields[23];
		$arrWorkflowsData["label_string_7"] = $rs->fields[24];
		$arrWorkflowsData["label_string_8"] = $rs->fields[25];
		$arrWorkflowsData["label_string_9"] = $rs->fields[26];
        
        if($arrWorkflowsData["paused"] == 0){
            $ackdata["data"]["aaData"][] = $arrWorkflowsData;
        }
        
        $rs->MoveNext();
    }
    $rs->Close();
    $conn->Close();
    print json_encode($ackdata);
?>