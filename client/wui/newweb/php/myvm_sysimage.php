﻿<?php
    include('adodb_connect.php');
    
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");
    
    $sEcho          = $_GET['sEcho'];
    $iDisplayStart  = $_GET['iDisplayStart'];
    $iDisplayLength = $_GET['iDisplayLength'];
    $language       = $_GET["language"];
    $iSortCol       = $_GET["iSortCol_0"];
    $sSortDir       = $_GET["sSortDir_0"];
    $sSearch        = $_GET["sSearch"];
    
    $iTotalRecords = 0;
    $enabled_type   = "";
    $container_format_type = "";
    if($language == 'ch'){
        $enabled_type   = "enabled_ch";
        $container_format_type = "container_format_ch";
    }
    else{
        $enabled_type   = "enabled_en";
        $container_format_type = "container_format_en";
    }
    
    $filed = array("image_id","file_name","register_time", $enabled_type,"running_vms",$container_format_type,"size", "os_type", "description");
    $orderfield = "";
    $orderdir = "";
    $order = "";
    $conn = GetTCDbConnect($errcode,$errinfo);
    
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }
    $filter = " 1=1 and is_public = 1 ";
    if($sSearch != ""){
        $filter = $filter . " and ( ";
        $filter = $filter . " to_char(image_id,'99999999999999999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or file_name like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or register_time like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or " . $enabled_type ." like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(running_vms,'9999999999999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or " . $container_format_type . "  like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or (case when size  < 1 then '0' || trim(to_char(size,'9.99')) else to_char(size,'9999999999999.99') end) like " . "'%" . $sSearch . "%'";
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
    $query = ' SELECT image_id,file_name,register_time,' . $enabled_type . ',running_vms,' . $container_format_type . ', size, os_type, description, container_format, ';
    $query = $query . " (select workflow_id from webview_workflows w where w.name='project_import' and w.label_int64_2=web_view_image.image_id) as workflow";    
    $query = $query . ' FROM web_view_image';
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
        $arrData[] = $rs->fields[0];//image_id
        $arrData[] = $rs->fields[1];//file_name
        $arrData[] = $rs->fields[2];//register_time
        $arrData[] = $rs->fields[3];//enabled
        $arrData[] = $rs->fields[4];//running_vms
        $arrData[] = $rs->fields[5];//container_format_type
        $arrData[] = $rs->fields[6];//size
        if($rs->fields[9] == "tpi"){
            $arrData[] = "";//os_type
        }
        else if($rs->fields[9] == ""){
            $arrData[] =$rs->fields[7];//os_type
        }
        $arrData[] = $rs->fields[8];//description
        $arrData[] = $rs->fields[9];//container_format
        $arrData["workflow"] = $rs->fields[10];
        $ackdata["data"]["aaData"][] = $arrData;
        $rs->MoveNext();
    }
    $rs->Close();
    $conn->Close();
    print json_encode($ackdata);
?>