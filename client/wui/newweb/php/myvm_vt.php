<?php
    include('adodb_connect.php');
    
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");
    
    $sEcho          = $_GET['sEcho'];
    $iDisplayStart  = $_GET['iDisplayStart'];
    $iDisplayLength = $_GET['iDisplayLength'];
    $role           = $_GET["role"];
    $user_name      = $_GET["user_name"];
    $language       = $_GET["language"];
    $iSortCol       = $_GET["iSortCol_0"];
    $sSortDir       = $_GET["sSortDir_0"];
    $sSearch        = $_GET["sSearch"];
    
    $iTotalRecords = 0;
    $filed = array("vt_name","vcpu","tcu","memory","total_size");
    $orderfield = "";
    $orderdir = "";
    $order = "";
    $conn = GetTCDbConnect($errcode,$errinfo);
    
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }
    $filter = " 1=1 ";
    if($role != "1"){//租户
        $filter = $filter. "and is_public = 1";
    }
    else{
        $filter = $filter. "and (is_public = 1 or user_name = " . "'" . $user_name . "'" .")";
    }
    if($sSearch != ""){
        $filter = $filter . " and ( ";
        $filter = $filter . " vt_name like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(vcpu,'999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(tcu,'999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(memory,'9999999999999.99') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or (case when total_size  < 1 then '0' || trim(to_char(total_size,'9.99')) else to_char(total_size,'9999999999999.99') end) like " . "'%" . $sSearch . "%'";
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
    $query = "SELECT COUNT(*) FROM web_view_vt WHERE " . $filter;
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
    $query = ' SELECT vt_name,vcpu,tcu,memory,total_size ';
    $query = $query . ' FROM web_view_vt';
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
        $arrData[] = $rs->fields[0];//vt_name
        $arrData[] = $rs->fields[1];//vcpu
        $arrData[] = $rs->fields[2];//tcu
        $arrData[] = $rs->fields[3];//memory
        $arrData[] = $rs->fields[4];//total_size
        $ackdata["data"]["aaData"][] = $arrData;
        $rs->MoveNext();
    }
    $rs->Close();
    $conn->Close();
    print json_encode($ackdata);
?>
