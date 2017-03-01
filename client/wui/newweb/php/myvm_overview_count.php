<?php
    include('adodb_connect.php');
    
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");
    
    $sEcho          = "";
    $user_name      = $_GET["user_name"];
    $project_cnt    = 0;
    $vm_cnt         = 0;
    $portable_disk_cnt = 0;
    $vt_cnt            = 0;
    $image_cnt         = 0;
    
    $iTotalRecords = 1;
    $conn = GetTCDbConnect($errcode,$errinfo);
    
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }
    
    //查表数据
    $query = "SELECT project_count,vmcfg_count,image_count,vmtpl_count FROM web_view_vm_overview";
    $query = $query . " where user_name = " . "'" . $user_name . "'";
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
    $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
    while (!$rs->EOF) {
        $project_cnt += $rs->fields[0];
        $vm_cnt      += $rs->fields[1];
        $image_cnt   += $rs->fields[2];
        $vt_cnt      += $rs->fields[3]; 
        $rs->MoveNext();
    }
	
	
    //从数据库查询移动盘
    $querypdisk = "SELECT COUNT(*) FROM web_view_pdisk WHERE username = " . "'" . $user_name . "'";
    $portable_disk= $conn->Execute($querypdisk);
    if (!$portable_disk){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$querypdisk,$sEcho,0)));
    }
    while (!$portable_disk->EOF) {
        $portable_disk_cnt = $portable_disk->fields[0];
        break;
    }

	
    $arrData = Array();
    $arrData[] = $project_cnt;
    $arrData[] = $vm_cnt;
    $arrData[] = $portable_disk_cnt;	
    $arrData[] = $vt_cnt;
    $arrData[] = $image_cnt;
    $ackdata["data"]["aaData"][] = $arrData;
    $rs->Close();
    $portable_disk->Close(); 	
    $conn->Close();

    print json_encode($ackdata);
?>
