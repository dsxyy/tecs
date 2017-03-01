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
    $query = 'select uuid, name,is_sdn ';
    $query = $query . ' from web_view_portgroup';
    $query = $query . ' where pg_type=0';

    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        ExitWithError($query . " execute failed!");
    }

    $pg = Array();
    $pg[0] = 0;
    $pg[1] = Array();    

    while (!$rs->EOF) {
        $onePg = Array();

        $onePg["uuid"] = $rs->fields[0];
        $onePg["name"] = $rs->fields[1];        
        $onePg["is_sdn"] = $rs->fields[2];

        $pg[1][] = $onePg;

        $rs->MoveNext();
    }

    $rs->Close();

    print json_encode($pg);
?>