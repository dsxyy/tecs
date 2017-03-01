<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $oid = $_GET["oid"];

    $tc_db_conn = GetTCDbConnect($err_code, $err_info);
    if(!$tc_db_conn){
        ExitWithError($err_info);
    }

    $ver_detail_query = "select repo_version,detail from upgrade_log where oid=" . $oid;

    $rs = $tc_db_conn->Execute($ver_detail_query);
    if (!$rs || $rs->RecordCount() == 0){
        ExitWithError($ver_detail_query . " execute failed!");
    }

    $detail = Array();
    $detail[0] = $rs->fields[0];
    $detail[1] = $rs->fields[1];
    $rs->close();

    print json_encode($detail);
?>