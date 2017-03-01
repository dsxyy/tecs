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
    $query = 'select ( SELECT count(*) FROM netplane ) AS netplane_count, ( SELECT count(*) FROM port_group ) AS pg_count, ';
    $query = $query . '( SELECT count(*) FROM logic_network ) AS logicnet_count, ( SELECT count(*) FROM  switch) AS switch_count';

    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        ExitWithError($query . " execute failed!");
    }

    $info = Array();
    $info[0] = 0;
    $info[1] = Array();    

    while (!$rs->EOF) {
        $info[1][] = $rs->fields[0];
        $info[1][] = $rs->fields[1];        
        $info[1][] = $rs->fields[2];
        $info[1][] = $rs->fields[3]; 

        $rs->MoveNext();
    }

    $rs->Close();

    print json_encode($info);
?>