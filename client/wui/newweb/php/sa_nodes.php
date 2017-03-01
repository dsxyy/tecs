<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $tc_db_conn = GetTCDbConnect($err_code, $err_info);
    if(!$tc_db_conn){
        ExitWithError($err_info);
    }

    $node_list = Array();
    $oneNode = Array();

    $cc_ip_query = "select application from storage_adaptor";
    $rs = $tc_db_conn->Execute($cc_ip_query);
    if (!$rs || $rs->RecordCount() == 0){
        ExitWithError($cc_ip_query . " execute failed!");
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