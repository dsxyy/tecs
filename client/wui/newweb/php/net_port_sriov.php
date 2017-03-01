<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $sEcho          = $_GET['sEcho'];
    $iDisplayStart  = $_GET['iDisplayStart'];
    $iDisplayLength = $_GET['iDisplayLength'];
    $iSortCol       = $_GET["iSortCol_0"];
    $sSortDir       = $_GET["sSortDir_0"];
    $sSearch        = "";//$_GET["sSearch"];
    $sVNAid         = $_GET["sVNAid"];

    $iTotalRecords = 0;

    $filed = array("sriov_vf_port_name",  "pci", "vlan_num","sriov_vf_is_free","phy_port_name");

    $orderfield = "";
    $orderdir = "";
    $order = "";
 
    $conn = GetVNMDbConnect($errcode,$errinfo);
    
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }

    $filter = " 1 = 1"; 
	$filter = $filter . " and vna_uuid = '" . $sVNAid ."'";
    
    if($sSearch != ""){
        $filter = $filter . " and ( ";
        $filter = $filter . " sriov_vf_port_name like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or pci like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or phy_port_name like " . "'%" . $sSearch . "%'";
        $filter = $filter . ")";
    }


    if($iSortCol == "0"){
        $orderfield = $filed[0];
    }
    else{
        $orderfield = $filed[intval($iSortCol)];
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
    $query = "SELECT COUNT(*) FROM web_view_port_sriov_vf WHERE " . $filter;
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
    $query = ' select sriov_vf_port_name, pci, vlan_num,sriov_vf_is_free,phy_port_name';
    $query = $query . ' from web_view_port_sriov_vf';
    $query = $query . ' WHERE' . $filter;
    $query = $query . ' ORDER by' . $order;
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
     
    $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
    while (!$rs->EOF) {
        $one = Array();
        $one[] = $rs->fields[0];//sriov_vf_port_name	
        $one[] = $rs->fields[1];//pci
        $one[] = $rs->fields[2];//vlan_num
        if($rs->fields[3] == 1){
            $one[] = "be free";
        }
        elseif($rs->fields[3] == 0){
            $one[] = "in use";
        }
        $one[] = $rs->fields[4];//phy_port_name

        $ackdata["data"]["aaData"][] = $one;
        
        $rs->MoveNext();
    }
    $rs->Close();

    $conn->Close(); 	

    print json_encode($ackdata);  
?>