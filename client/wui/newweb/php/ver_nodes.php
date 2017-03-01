<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $tc_db_conn = GetTCDbConnect($err_code, $err_info);
    if(!$tc_db_conn){
        ExitWithError($err_info);
    }

    $node_ip = Array();

    $tc_ip_query = "select * from cloud_node where is_master=1";

    $tc_node = Array();
    $tc_node["type"]    = "tc";
    $tc_node["tc"]      = "tc";
    $tc_node["tc_ip"]   = "127.0.0.1";
    $tc_node["cc"]      = "";
    $tc_node["cc_ip"]   = "";
    $tc_node["hc"]      = "";
    $tc_node["hc_ip"]   = "";
    $node_ip[] = $tc_node;

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

        $cc_node = Array();
        $cc_node["type"]    = "cc";
        $cc_node["tc"]      = $tc_node["tc"];
        $cc_node["tc_ip"]   = $tc_node["tc_ip"];
        $cc_node["cc"]      = $rs->fields[5];
        $cc_node["cc_ip"]   = $cluster_ip;
        $cc_node["hc"]      = "";
        $cc_node["hc_ip"]   = "";
        $node_ip[] = $cc_node;

        $ccdb_conn  = GetClusterDbConnect($cluster_ip,$cluster_port,$cluster_user,$cluster_pwd,$cluster_dbname);
        if($ccdb_conn){
            $query = "select * from host_pool_in_use";
            $rs1 = $ccdb_conn->Execute($query);
            while (!$rs1->EOF) {
                $hc_node = Array();
                $hc_node["type"]     = "hc";
                $hc_node["tc"]       = $tc_node["tc"];
                $hc_node["tc_ip"]    = $tc_node["tc_ip"];
                $hc_node["cc"]       = $cc_node["cc"];
                $hc_node["cc_ip"]    = $cc_node["cc_ip"];
                $hc_node["hc"]       = $rs1->fields[1];
                $hc_node["hc_ip"]    = $rs1->fields[2];
                $node_ip[]  = $hc_node;

                $rs1->MoveNext();
            }
            $rs1->Close();
        }

        $rs->MoveNext();
    }
    $rs->Close();

    print json_encode($node_ip);
?>