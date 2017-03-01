<?php
    include('adodb_connect.php');
    
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");
    
    $clusterName    = $_GET["clusterName"];
    $sEcho          = $_GET['sEcho'];
    $iDisplayStart  = $_GET['iDisplayStart'];
    $iDisplayLength = $_GET['iDisplayLength'];
    $iSortCol       = $_GET["iSortCol_0"];
    $sSortDir       = $_GET["sSortDir_0"];
    $sSearch        = $_GET["sSearch"];
    
    $iTotalRecords = 0;
    $filed = array("workflow_id","rollback_workflow_id","category","name","application","process","state", "detail",  "failed_action_name","create_time","delete_time","description","progress");
    $orderfield = "";
    $orderdir = "";
    $order = "";
    $conn = NULL;

    $conn = GetClusterDbConnect_By_ClusterName($clusterName,&$errcode,&$errinfo); 
    if(!$conn){
        ExitWithError($errinfo);
    }
    $filter = " 1=1";
        if($sSearch != ""){
        $filter = $filter . " and ( ";
        $filter = $filter . " to_char(state,'99999999999999999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(progress,'99999999999999999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or category like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or name like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or application like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or process like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or failed_action_category like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or failed_action_name like " . "'%" . $sSearch . "%'";
        $filter = $filter . ")";
    }
        
    $orderfield = $filed[intval($iSortCol)]; 
    if($sSortDir == ""){
        $orderdir = "asc";
    }
    else{
        $orderdir = $sSortDir;
    }
    $order = " " . $orderfield . " " . $orderdir;
    $limit = " LIMIT " . $iDisplayLength . " OFFSET " . $iDisplayStart;
    /******************/
    $query = "SELECT COUNT(*) FROM workflow_history WHERE" .filter;
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
    
  
  /************************************/  
    $query = 'select workflow_id,rollback_workflow_id,category,name,application,process,state, detail, failed_action_category, failed_action_name,create_time,delete_time,description,progress FROM workflow_history';  
    $query = $query . ' WHERE' . $filter;
    $query = $query . ' ORDER by' . $order;
    $query = $query. $limit;
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
		ExitWithError($query . " execute failed!");
    }
    $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);

    while (!$rs->EOF) {
        $oneData = Array();

        $oneData[] = $rs->fields[0];//workflow_id
		$oneData[] = $rs->fields[1];//rollback_workflow_id
		$oneData[] = $rs->fields[2];//category
        $oneData[] = $rs->fields[3];//name
		$oneData[] = $rs->fields[4];//application
		$oneData[] = $rs->fields[5];//process
        if($rs->fields[6] ==0){
            $oneData[] ="Success" ;//state
        }
        else {
            $oneData[] ="Fail";
        }
		$oneData[] = $rs->fields[7];//detail
		//$oneData[8] = $rs->fields[8];//failed_action_category
        $oneData[] = $rs->fields[9];//failed_action_name
        $oneData[] = $rs->fields[10];//create_time
        $oneData[] = $rs->fields[11];//delete_time
        
        
        $description = $rs->fields[12];//description
        $description = str_replace("\n","<br/>",htmlspecialchars($description));
        $oneData[] = $description;
        $oneData[] = $rs->fields[13]."%";//progress
        
        $ackdata["data"]["aaData"][] = $oneData;        
        $rs->MoveNext();
    }
    $rs->Close();
    $conn->Close();

    print json_encode($ackdata);
?>