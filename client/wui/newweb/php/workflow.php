<?php
    include('adodb_connect.php');
    
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");
    /*
    $sEcho          = $_GET['sEcho'];
    $iDisplayStart  = $_GET['iDisplayStart'];
    $iDisplayLength = $_GET['iDisplayLength'];
    $user_name      = $_GET["user_name"];
    $language       = $_GET["language"];
    $iSortCol       = $_GET["iSortCol_0"];
    $sSortDir       = $_GET["sSortDir_0"];
    $sSearch        = $_GET["sSearch"];
    */
    $iTotalRecords = 0;

    $filed = array("workflow_id","category","name","process","engine","create_time","progress");
    $orderfield = "";
    $orderdir = "";
    $order = "";
    $conn = GetTCDbConnect($errcode,$errinfo);
    
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }
    $filter = " 1=1 " ;
/*
    if($sSearch != ""){
        $filter = $filter . " and ( ";
        $filter = $filter . " to_char(workflow_id,'99999999999999999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or category like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or name like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or process like " . "'%" . $sSearch . "%'";
        $filter = $filter . " to_char(engine,'99999999999999999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or create_time like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or description like " . "'%" . $sSearch . "%'";
        $filter = $filter . " to_char(progress,'99999999999999999') like " . "'%" . $sSearch . "%'";
       // $filter = $filter . " or " . $enabled_type ." like " . "'%" . $sSearch . "%'";
        //$filter = $filter . " or " . $is_public_type ." like " . "'%" . $sSearch . "%'";
        //$filter = $filter . " or to_char(running_vms,'9999999999999') like " . "'%" . $sSearch . "%'";
		
        $filter = $filter . " or paused like " . "'%" . $sSearch . "%'";
        //$filter = $filter . " or (case when size  < 1 then '0' || trim(to_char(size,'9.99')) else to_char(size,'9999999999999.99') end) like " . "'%" . $sSearch . "%'";
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
    */
    //表记录数
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
    //查表数据
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