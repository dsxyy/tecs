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
    $portgroup      = $_GET["portgroup"];

    $iTotalRecords = 0;

    $filed = array("name", "uuid", "vm_pg_name", "ip_bootprotocolmode", "description");

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
        $filter = $filter . " or description like " . "'%" . $sSearch . "%'";
        $filter = $filter . ")";
    }

    if($portgroup != ""){
        $filter = $filter . " and vm_pg_name in (" . $portgroup . ")";
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
    $query = "SELECT COUNT(*) FROM web_view_logicnet WHERE " . $filter;
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
    $query = ' select uuid, name, vm_pg_name, ip_bootprotocolmode, description, ';
    $query = $query . '(select a.ln_total_num from web_view_logicnetwork_res_statatic_ip a where a.ln_uuid = uuid) as ln_ip_total, ';
    $query = $query . '(select a.ln_alloc_num from web_view_logicnetwork_res_statatic_ip a where a.ln_uuid = uuid) as ln_ip_alloc, vm_pg_uuid';
    $query = $query . ' from web_view_logicnet WHERE' . $filter . ' ORDER by' . $order;
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
     
    $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
    while (!$rs->EOF) {
        $oneLogicnet = Array();
        $oneLogicnet[] = $rs->fields[0];//uuid
        $oneLogicnet[] = $rs->fields[1];//name
        $oneLogicnet[] = $rs->fields[2];//vm_pg_name
        $oneLogicnet[] = $rs->fields[3];//ip_bootprotocolmode
        if($rs->fields[5] != 0){       
            $oneLogicnet[] = round(($rs->fields[6] / $rs->fields[5]) * 100, 1) . '%';
        }
        else {
            $oneLogicnet[] = "0%";
        }        
        $description   = $rs->fields[4];//description
        $description   = str_replace("\n", "<br/>", htmlspecialchars($description));
        $oneLogicnet[] = $description;
        $oneLogicnet[] = $rs->fields[7]; 

        if($portgroup == "" || strstr($portgroup, $oneLogicnet[2]))
        {
            $ackdata["data"]["aaData"][] = $oneLogicnet;
        }
        
        $rs->MoveNext();
    }
    $rs->Close();

    $conn->Close();

    print json_encode($ackdata);  
?>