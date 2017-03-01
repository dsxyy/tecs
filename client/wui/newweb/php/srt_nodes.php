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

    $cc_ip_query = "select db_name,db_server_url,db_server_port,db_user,db_passwd,name from cluster_pool";
    $rs = $tc_db_conn->Execute($cc_ip_query);
    if (!$rs || $rs->RecordCount() == 0){
        ExitWithError($cc_ip_query . " execute failed!");
    }

    while (!$rs->EOF) {
        $cluster_dbname = $rs->fields[0];
        $cluster_ip = $rs->fields[1];
        $cluster_port = $rs->fields[2];
        $cluster_user = $rs->fields[3];
        $cluster_pwd = $rs->fields[4];

        $oneNode["name"] = $rs->fields[5];
        $oneNode["hosts"] = Array();

        $ccdb_conn  = GetClusterDbConnect($cluster_ip,$cluster_port,$cluster_user,$cluster_pwd,$cluster_dbname, $err_code, $err_info);
        if($ccdb_conn){
            $query = "select * from host_pool_in_use";
            $rs1 = $ccdb_conn->Execute($query);
            while (!$rs1->EOF) {
                $oneNode["hosts"][]     = $rs1->fields[1];

                $rs1->MoveNext();
            }
            $rs1->Close();
        }

        $node_list[] = $oneNode;

        $rs->MoveNext();
    }
    $rs->Close();

    print json_encode($node_list);
?>