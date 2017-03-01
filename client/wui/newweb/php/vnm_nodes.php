<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $conn = GetVNMDbConnect($errcode,$errinfo);
    
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }

    $node_list = Array();
    $oneNode = Array();

    $query = "select vna_application from vna";
    $rs = $conn->Execute($query);
    if (!$rs || $rs->RecordCount() == 0){
        ExitWithError($query . " execute failed!");
    }

    while (!$rs->EOF) {
        $oneNode["name"] = $rs->fields[0];
        $oneNode["hosts"] = Array();		
        $node_list[] = $oneNode;

        $rs->MoveNext();
    }
    $rs->Close();

    print json_encode($node_list);
?>