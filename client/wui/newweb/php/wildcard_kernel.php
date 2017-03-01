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
    $query = 'select  uuid, port_name, kernel_type, kernel_pg_name, switch_name,kernel_pg_uuid';
    $query = $query . ' from web_view_wildcast_cfg_kernelport';

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

        $one[] = $rs->fields[0];//uuid
        $one[] = $rs->fields[1];//port_name
       // $one[] = $rs->fields[2];// kernel_type    
        if($rs->fields[2] == 10){//kernel_type
            $one[] = "Kernel Mgr";
        }
        else if($rs->fields[2] == 11){
            $one[] = "Storge Mgr";
        }        
        $one[] = $rs->fields[3];//kernel_pg_name
        $one[] = $rs->fields[4];//switch_name
        $one[] = $rs->fields[5];//switch_name
        $data[1][] = $one;

        $rs->MoveNext();
    }

    $rs->Close();

    print json_encode($data);    
?>