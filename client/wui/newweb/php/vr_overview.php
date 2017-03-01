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
    $sSearch        = $_GET["sSearch"];
    
    $iTotalRecords = 0;
    $filed = array("user_name","project_count","vmcfg_count","pdisk_count","vmtpl_count","image_count");
    $orderfield = "";
    $orderdir = "";
    $order = "";
    $conn = GetTCDbConnect($errcode,$errinfo);
    
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }
    $filter = " 1=1";
    if($sSearch != ""){
        $filter = $filter . " and ( ";
        $filter = $filter . " user_name like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(project_count,'999999999999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(vmcfg_count,'999999999999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(vmtpl_count,'999999999999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(image_count,'999999999999') like " . "'%" . $sSearch . "%'";	
        $filter = $filter . " or to_char(pdisk_count,'999999999999') like " . "'%" . $sSearch . "%'";			
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
    $query = "SELECT COUNT(*) FROM web_view_vm_overview WHERE " . $filter;			
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
    $query = ' select * ';
    $query = $query . ' FROM web_view_vm_overview';	
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
        $arrData[] = $rs->fields[0];
        $arrData[] = $rs->fields[1];
        $arrData[] = $rs->fields[2];
        $arrData[] = $rs->fields[3];
        $arrData[] = $rs->fields[4];
        $arrData[] = $rs->fields[5];
        $ackdata["data"]["aaData"][] = $arrData;
        $rs->MoveNext();
    }
    $rs->Close();
    $conn->Close();
    print json_encode($ackdata);
?>
