<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $tc_db_conn = GetTCDbConnect($err_code, $err_info);
    if(!$tc_db_conn){
        ExitWithError($err_info);
    }

    $num = $_GET["num"];

    $update_progress = Array();
    $ver_update_query = "select oid,percent,result,repo_version from upgrade_log";// order by oid desc limit " . $num;

    $rs = $tc_db_conn->Execute($ver_update_query);
    if (!$rs || $rs->RecordCount() == 0){
        ExitWithError($ver_update_query . " excute failed!");
    }

    while (!$rs->EOF) {
        $oneNode = Array();
        $oneNode["oid"]     = $rs->fields[0];
        $oneNode["percent"] = $rs->fields[1];
        $oneNode["result"]  = $rs->fields[2];
        $oneNode["repo_ver"]  = $rs->fields[3];
        $update_progress[]  = $oneNode;

        $rs->MoveNext();
    }
    $rs->Close();

    print json_encode($update_progress);
?>