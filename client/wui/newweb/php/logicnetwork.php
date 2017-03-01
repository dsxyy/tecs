<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $vnm_db_conn = GetVNMDbConnect($err_code, $err_info);
    if(!$vnm_db_conn){
        ExitWithError($err_info);
    }

    $logicnetwork_query = "select * from logic_network";

    $rs = $vnm_db_conn->Execute($logicnetwork_query);
    if (!$rs){
        ExitWithError($logicnetwork_query . " execute_failed");
    }

    $logicnetwork = Array();
    $logicnetwork[0] = 0;
    $logicnetwork[1] = Array();

    while(!$rs->EOF){
        $one = Array();
        $one["id"]                    = $rs->fields[0];
        $one["name"]                  = $rs->fields[1];
        $one["uuid"]                  = $rs->fields[2];
        $one["vm_pg_id"]              = $rs->fields[3];
        $one["description"]           = $rs->fields[4];
        $one["ip_bootprotocolmode"]   = $rs->fields[5];
     

        $logicnetwork[1][] = $one;

        $rs->MoveNext();
    }

    $rs->Close();

    print json_encode($logicnetwork);
?>