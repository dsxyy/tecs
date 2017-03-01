<?php
    /**
    *   Query the port and trunk info from cluster database.
    *
    *   write by wangmh
    */

    include ("adodb_connect.php");

    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Cache-Control: no-cache");
    header("Pragma: no-cache");

    $cc_name = $_GET["cc"];
    $hc_name = $_GET["hc"];

    $cc_db_conn = GetClusterDbConnect_By_ClusterName($cc_name, $err_code, $err_info);
    if(!$cc_db_conn){
        ExitWithError($err_info);
    }

    $port_query  = "select p.* from host_port p, host_pool h where p.hid=h.oid and h.name='" . $hc_name . "' order by p.name asc";
    $trunk_query = "select t.* from host_trunk t, host_pool h where t.hid=h.oid and h.name='" . $hc_name . "' order by t.name asc";

    $port_rs = $cc_db_conn->Execute($port_query);
    if(!$port_rs){
    	ExitWithError($port_query . " execute failed!");
    }

    $trunk_rs = $cc_db_conn->Execute($trunk_query);
    if(!$trunk_rs){
    	ExitWithError($trunk_query . " execute failed!");
    }

    $trunk_info = array();
    while(!$trunk_rs->EOF){
    	$one_trunk = array();
    	$one_trunk["trunk_name"]  = $trunk_rs->fields[1];
    	$one_trunk["trunk_type"]  = $trunk_rs->fields[2];
    	$one_trunk["trunk_state"] = $trunk_rs->fields[3];
    	$one_trunk["trunk_ports"] = array();
    	$one_trunk["netplane"]    = $trunk_rs->fields[4];
        if($one_trunk["netplane"] == null) $one_trunk["netplane"] = "";
    	$trunk_info[] = $one_trunk;

        $trunk_rs->MoveNext();
    }
    $trunk_rs->Close();

    $port_info = array();
    while(!$port_rs->EOF){
        $one_port = array();
        $one_port["name"]        = $port_rs->fields[1];
        $one_port["negotiate"]   = $port_rs->fields[2];
        $one_port["speed"]       = $port_rs->fields[3];
        $one_port["duplex"]      = $port_rs->fields[4];
        $one_port["master"]      = $port_rs->fields[5];
        $one_port["sriov_num"]   = $port_rs->fields[6];
        $one_port["sriov"]       = $one_port["sriov_num"] > 0 ? 1 : 0;
        $one_port["state"]       = $port_rs->fields[7];
        $one_port["linked"]      = $port_rs->fields[8];
        $one_port["netplane"]    = $port_rs->fields[9];
        if($one_port["netplane"] == null) $one_port["netplane"] = "";
        $one_port["used"]        = $port_rs->fields[10];
        $one_port["loopback_mode"] = $port_rs->fields[11];        
        $port_info[] = $one_port;

        $port_rs->MoveNext();
    }
    $port_rs->Close();

    $port_trunk_map_query = "select tp.* from trunk_port tp, host_pool h where tp.hid=h.oid and h.name='" . $hc_name . "'";
    $port_trunk_map_rs = $cc_db_conn->Execute($port_trunk_map_query);
    if(!$port_trunk_map_rs){
        ExitWithError($port_trunk_map_query . " execute failed!");
    }

    while(!$port_trunk_map_rs->EOF){
        $trunk_name = $port_trunk_map_rs->fields[1];
        $port_name = $port_trunk_map_rs->fields[2];

        foreach ($port_info as &$p) {
            if($p["name"] == $port_name){
                $p["trunk"] = $trunk_name;
            }
        }

        foreach ($trunk_info as &$t) {
            if($t["trunk_name"] == $trunk_name){
                $t["trunk_ports"][] = $port_name;
            }
        }

        $port_trunk_map_rs->MoveNext();
    }
    $port_trunk_map_rs->Close();

    $port_trunk = array();
    $port_trunk["port"] = $port_info;
    $port_trunk["trunk"] = $trunk_info;

    print json_encode($port_trunk);    
?>