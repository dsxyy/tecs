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
    $query = 'select   vna_uuid, cluster_name, host_name, kernel_type, kernel_pg_name, switch_name, is_can_delete,uuid';
    $query = $query . ' from web_view_wildcast_task_kernelport';

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

        $one[] = $rs->fields[0];
        $one[] = $rs->fields[1];
        $one[] = $rs->fields[2];
        $one[] = $rs->fields[3];
        $one[] = $rs->fields[4];
        $one[] = $rs->fields[5]; 
        $one[] = $rs->fields[6]; 
        $one[] = $rs->fields[7];

        $data[1][] = $one;

        $rs->MoveNext();
    }

    $rs->Close();

    print json_encode($data);    
?>