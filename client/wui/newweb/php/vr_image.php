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
    $is_public_type = "";
    $enabled_type   = "";
    $container_format_type = "";
    if($language == 'ch'){
        $is_public_type = "is_public_ch";
        $enabled_type   = "enabled_ch";
        $container_format_type = "container_format_ch";
    }
    else{
        $is_public_type = "is_public_en";
        $enabled_type   = "enabled_en";
        $container_format_type = "container_format_ch";
    }
    
    $filed = array("image_id","file_name","register_time",$enabled_type,$is_public_type,"running_vms",$container_format_type,"size","user_name");
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
        $filter = $filter . " to_char(image_id,'99999999999999999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or file_name like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or register_time like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or " . $enabled_type ." like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or " . $is_public_type ." like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(running_vms,'9999999999999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or " . $container_format_type . " like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or (case when size  < 1 then '0' || trim(to_char(size,'9.99')) else to_char(size,'9999999999999.99') end) like " . "'%" . $sSearch . "%'";
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
    $query = "SELECT COUNT(*) FROM web_view_image WHERE " . $filter;
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
    $query = ' select i.image_id,file_name,register_time,' . $enabled_type . ',' . $is_public_type . ',running_vms,' . $container_format_type . ',size,user_name,description,enabled,is_public,';
    $query = $query . ' (select workflow_id from web_view_image_workflow w where w.image_id=i.image_id limit 1) as workflow, container_format';
    $query = $query . ' FROM web_view_image i';
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
        $arrData[] = $rs->fields[0];//image_id
        $arrData[] = $rs->fields[1];//file_name
        $arrData[] = $rs->fields[2];//register_time
        $arrData[] = $rs->fields[3];//enabled
        $arrData[] = $rs->fields[4];//is_public
        $arrData[] = $rs->fields[5];//running_vms
        $arrData[] = $rs->fields[6];//container_format_type
        $arrData[] = $rs->fields[7];//size
        $arrData[] = $rs->fields[8];//user_name
        $arrData[] = $rs->fields[9];//description
        $arrData[] = "";
        $arrData[] = $rs->fields[10];//enabled
        $arrData[] = $rs->fields[11];//is_public
        $arrData[] = $rs->fields[13];
        $arrData["workflow"] = $rs->fields[12];
        $ackdata["data"]["aaData"][] = $arrData;
        $rs->MoveNext();
    }
    $rs->Close();
    $conn->Close();
    print json_encode($ackdata);
?>
