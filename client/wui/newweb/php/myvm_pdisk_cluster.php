<?php
    include('adodb_connect.php');
     
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache"); 
     
    $sEcho          = $_GET['sEcho'];
    $iDisplayStart  = $_GET['iDisplayStart'];
    $iDisplayLength = $_GET['iDisplayLength'];
    $user_name      = $_GET["user_name"];
    $iSortCol       = $_GET["iSortCol_0"];
    $sSortDir       = $_GET["sSortDir_0"];
    $sSearch        = $_GET["sSearch"];
    $iTotalRecords = 0;
   
    $conn = GetTCDbConnect($errcode,$errinfo);    
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }
    $filter = " 1=1";
    //表记录数
    $query = "SELECT COUNT(*) FROM storage_cluster_total_free_size_view WHERE " . $filter;
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
    $query = ' select cluster_name as clustername ';
    $query = $query . ' from storage_cluster_total_free_size_view';	
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
     
    $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
    while (!$rs->EOF) {
        $arrProjData = Array();
        $arrProjData[] = "";
        $arrProjData[] = $rs->fields[0];     
        
        $arrProjData[] = "";
        $ackdata["data"]["aaData"][] = $arrProjData;
        
        $rs->MoveNext();
    }
    $rs->Close();
    $conn->Close();
    print json_encode($ackdata);    
?>
