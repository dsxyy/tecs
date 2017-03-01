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
    $query = 'select uuid, name ';
    $query = $query . ' from web_view_netplane';

    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        ExitWithError($query . " execute failed!");
    }

    $netplane = Array();
    $netplane[0] = 0;
    $netplane[1] = Array();    

    while (!$rs->EOF) {
        $onenetplane = Array();

        $onenetplane["uuid"] = $rs->fields[0];
        $onenetplane["name"] = $rs->fields[1];        

        $netplane[1][] = $onenetplane;

        $rs->MoveNext();
    }

    $rs->Close();

    print json_encode($netplane);
?>