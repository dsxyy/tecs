<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $conn = GetVNMDbConnect($errcode,$errinfo);
    
    if(!$conn){
        ExitWithError($err_info);
    }

    //查表数据
    $query = 'select   vna_uuid, cluster_name, host_name, switch_name, send_num, is_can_delete, uuid';
    $query = $query . ' from web_view_wildcast_task_switch';

    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        ExitWithError($query . " execute failed!");
    }

    $data = Array();
    $data[0] = 0;
    $data[1] = Array();    

    while (!$rs->EOF) {
        $one = Array();
      
        $one[] = $rs->fields[0]; //vna_uuid
        $one[] = $rs->fields[1]; //cluster_name
        $one[] = $rs->fields[2]; //host_name   
        $one[] = $rs->fields[3]; //switch_name
        $one[] = $rs->fields[4]; //send_num
        $one[] = $rs->fields[5]; //is_can_delete
        $one[] = $rs->fields[6];//uuid

        $data[1][] = $one;

        $rs->MoveNext();
    }

    $rs->Close();

    print json_encode($data);    
?>