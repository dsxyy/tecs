<?php
    include('adodb_connect.php');
    
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");
    
    $project_name   = $_GET["project_name"];
    $user_name      = $_GET["user_name"];
    $sEcho          = $_GET['sEcho'];
    $iDisplayStart  = $_GET['iDisplayStart'];
    $iDisplayLength = $_GET['iDisplayLength'];
    $iSortCol       = $_GET["iSortCol_0"];
    $sSortDir       = $_GET["sSortDir_0"];
    $sSearch        = $_GET["sSearch"];
    
    $iTotalRecords = 0;
    $filed = array("vid","vm_name","vm_create_time","vcpu","tcu","memory","total_size","project_name");
    $orderfield = "";
    $orderdir = "";
    $order = "";
    $conn = GetTCDbConnect($errcode,$errinfo);
    
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }
    $filter = " 1=1";
    if($project_name != ""){
        $filter = $filter . " and project_name in (" . $project_name . ")";
    }
    if($user_name != ""){
        $filter = $filter . " and user_name = " . "'" . $user_name ."'";
    }
    $filter = $filter . " and deployed_cluster is null";
    if($sSearch != ""){
        $filter = $filter . " and ( ";
        $filter = $filter . " to_char(vid,'99999999999999999999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or vm_name like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or vm_create_time like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(vcpu,'999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(tcu,'999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(memory,'99999.99') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or (case when total_size  < 1 then '0' || trim(to_char(total_size,'9.99')) else to_char(total_size,'9999999999999.99') end) like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or project_name like " . "'%" . $sSearch . "%'";
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
    //表记录数
    $query = "SELECT COUNT(*) FROM web_view_vm WHERE " . $filter;
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
    $query = 'select v.vid,vm_name,vm_create_time,vcpu,tcu,memory,total_size,project_name, ';
    $query = $query . '(select workflow_id from web_view_vm_workflow w where w.vid=v.vid limit 1) as workflow';
    $query = $query . ' FROM web_view_vm v';
    $query = $query . ' WHERE' . $filter;
    $query = $query . ' ORDER by' . $order;
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
    $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
    while (!$rs->EOF) {
        $arrData = Array();
        $arrData[] = "";//第一列checkbox
        $arrData[] = $rs->fields[0];//vid
        $arrData[] = $rs->fields[1];//name
        $arrData[] = $rs->fields[2];//create_time
        $arrData[] = $rs->fields[3];//vcpu
        $arrData[] = $rs->fields[4];//tcu
        $arrData[] = $rs->fields[5];//memory
        $arrData[] = $rs->fields[6];//disk
        $arrData[] = $rs->fields[7];//project_name
        $arrData[] = "";//op
        $arrData["workflow"] = $rs->fields[8];//workflow
        $ackdata["data"]["aaData"][] = $arrData;
        $rs->MoveNext();
    }
    $rs->Close();
    $conn->Close();
    print json_encode($ackdata);
?>
