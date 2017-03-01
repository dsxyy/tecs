<?php
    include('adodb_connect.php');
     
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache"); 
     
    $sEcho          = $_GET['sEcho'];
    $iDisplayStart  = $_GET['iDisplayStart'];
    $iDisplayLength = $_GET['iDisplayLength'];
    //$user_name      = $_GET["user_name"];
    $iSortCol       = $_GET["iSortCol_0"];
    $sSortDir       = $_GET["sSortDir_0"];
    $sSearch        = $_GET["sSearch"];
    $iTotalRecords = 0;
    $filed = array("name","mtu","description");
    $orderfield = "";
    $orderdir = "";
    $order = "";
    
    $conn = GetVNMDbConnect($errcode,$errinfo);
    
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }
    $filter = " 1 = 1"; 
    
    if($sSearch != ""){
        $filter = $filter . " and ( ";
        $filter = $filter . " name like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(mtu,'99999999999999999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or description like " . "'%" . $sSearch . "%'";
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
    $query = "SELECT COUNT(*) FROM web_view_netplane WHERE " . $filter;
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
    $query = ' select uuid, name, mtu, description, ';
    $query = $query . '(select b.np_total_num from web_view_netplane_res_statatic_mac b where b.np_uuid = uuid) as np_mac_total, ';
    $query = $query . '(select b.np_alloc_num from web_view_netplane_res_statatic_mac b where b.np_uuid = uuid) as np_mac_alloc, ';
    $query = $query . '(select c.np_total_num from web_view_netplane_res_statatic_vlan c where c.np_uuid = uuid) as np_vlan_total, ';
    $query = $query . '(select c.np_alloc_num from web_view_netplane_res_statatic_vlan c where c.np_uuid = uuid) as np_vlan_alloc, '; 
    $query = $query . '(select d.np_total_num from web_view_netplane_res_statatic_segment d where d.np_uuid = uuid) as np_segment_total, ';
    $query = $query . '(select d.np_alloc_num from web_view_netplane_res_statatic_segment d where d.np_uuid = uuid) as np_segment_alloc '; 
    $query = $query . ' from web_view_netplane WHERE ' . $filter . ' ORDER by' . $order;

    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
     
    $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
    while (!$rs->EOF) {
        $arrProjData = Array();
        $arrProjData[] = $rs->fields[0];//checkbox
        $arrProjData[] = $rs->fields[1];//name
        $arrProjData[] = $rs->fields[2];//mtu
        $description   = $rs->fields[3];//description
        $description   = str_replace("\n","<br/>",htmlspecialchars($description));
        $arrProjData[] = $description;  
        if($rs->fields[4] != 0){       
            $arrProjData[] = round(($rs->fields[5] / $rs->fields[4]) * 100, 1) . '%';
        }
        else {
            $arrProjData[] = "0%";
        }

        if($rs->fields[6] != 0){
            $arrProjData[] = round(($rs->fields[7] / $rs->fields[6]) * 100, 1) . '%';
        }
        else {
            $arrProjData[] = "0%";
        }
         

        if($rs->fields[8] != 0){
            $arrProjData[] = round(($rs->fields[9] / $rs->fields[8]) * 100, 1) . '%';
        }
        else {
            $arrProjData[] = "0%";
        }

        $arrProjData[] = "";
        $ackdata["data"]["aaData"][] = $arrProjData;
        
        $rs->MoveNext();
    }
    $rs->Close();
    $conn->Close();
    print json_encode($ackdata);    
?>
