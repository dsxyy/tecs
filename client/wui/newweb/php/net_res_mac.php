<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $conn = GetVNMDbConnect($errcode,$errinfo);
  
    if(!$conn){
        ExitWithError($err_info);
    }

    //查表IP
    $query = ' select netplane_name, netplane_uuid, mac, vm_id, nic_index from web_view_netplane_macalloc';

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
        
        $one["netplane_name"]   = $rs->fields[0];
        $one["netplane_uuid"]   = $rs->fields[1];
        $one["mac"]             = $rs->fields[2];
        $one["vid"]             = $rs->fields[3];
        $one["nic"]             = $rs->fields[4];

        $info[1][] = $one;

        $rs->MoveNext();
    }
    $rs->Close();

    $conn->Close(); 

    print json_encode($info);  
?>