<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $conn = GetVNMDbConnect($errcode,$errinfo);
  
    if(!$conn){
        ExitWithError($err_info);
    }

    //²é±íIP
    $query = ' select netplane_name, netplane_uuid, segment_num, project_id, vm_id, nic_index, isolate_no from web_view_netplane_segmentalloc';

    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        ExitWithError($query . " execute failed!");          
    }

    $info = Array();
    $info[0] = 0;
    $info[1] = Array();

    while (!$rs->EOF) {
        $one = Array();
		
        $one["netplane_name"] = $rs->fields[0];
        $one["netplane_uuid"] = $rs->fields[1];
        $one["segment_num"] = $rs->fields[2];
        $one["project_id"] = $rs->fields[3];
        $one["vm_id"] = $rs->fields[4];
        $one["nic_index"] = $rs->fields[5];
        $one["isolate_no"] = $rs->fields[6];

        $info[1][] = $one;

        $rs->MoveNext();
    }
    $rs->Close();

    $conn->Close(); 

    print json_encode($info);  
?>