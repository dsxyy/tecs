<?php
    include('adodb_connect.php');
    
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");
    
    $sEcho          = $_GET['sEcho'];
    $iDisplayStart  = $_GET['iDisplayStart'];
    $iDisplayLength = $_GET['iDisplayLength'];
    $user_name      = $_GET["user_name"];
    $language       = $_GET["language"];
    $iSortCol       = $_GET["iSortCol_0"];
    $sSortDir       = $_GET["sSortDir_0"];
    $sSearch        = $_GET["sSearch"];
    
    $iTotalRecords = 0;
    $state = "";
    if($language == 'ch'){
        $state = "state_ch";
    }
    else{
        $state = "state_en";
    }
    
    $filed = array("base_id","cluster_name","base_id","image_id", "file_name","user_name");
    $orderfield = "";
    $orderdir = "";
    $order = "";
    $conn = GetTCDbConnect($errcode,$errinfo);
    
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }
    $filter = " 1=1";
    if($user_name != ""){
        $filter = " user_name in (" . $user_name . ")";
    }
    if($sSearch != ""){
        $filter = $filter . " and ( ";
        $filter = $filter . " to_char(base_uuid,'99999999999999999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or cluster_name like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or base_id like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(image_id,'99999999999999999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or file_name like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or user_name like " . "'%" . $sSearch . "%'";
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
    $query = "SELECT COUNT(*) FROM web_view_deployedimage WHERE " . $filter;
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
    $query = ' select base_id,cluster_name,image_id,file_name,user_name,description,w.base_uuid, ';
    $query = $query . ' (select count(s.snapshot_uuid) from storage_snapshot s where s.base_uuid = w.base_uuid)  ';
    $query = $query . ' FROM web_view_deployedimage w';
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
        $arrData[] = $rs->fields[0];//base_id
        $arrData[] = $rs->fields[1];//cluster_name
        $arrData[] = $rs->fields[2];//image_id
        $arrData[] = $rs->fields[3];//file_name
        $arrData[] = $rs->fields[4];//user_name
        $arrData[] = $rs->fields[5];//description
        $arrData[] = $rs->fields[6];//base_uuid
        $arrData["snapshot_count"] = $rs->fields[7]; 
        $ackdata["data"]["aaData"][] = $arrData;
        $rs->MoveNext();
    }
    $rs->Close();
    $conn->Close();
    print json_encode($ackdata);
?>
