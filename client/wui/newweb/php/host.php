<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $sEcho          = $_GET['sEcho'];
    $iDisplayStart  = $_GET['iDisplayStart'];
    $iDisplayLength = $_GET['iDisplayLength'];
    $cluster_name   = $_GET["cluster_name"];
    $iSortCol       = $_GET["iSortCol_0"];
    $sSortDir       = $_GET["sSortDir_0"];
    $sSearch        = $_GET["sSearch"];

    $ccdb_conn = GetClusterDbConnect_By_ClusterName($cluster_name, $err_code, $err_info);
    if(!$ccdb_conn){
        ProcExecInfo($sEcho, $err_info);
        return;
    }

    $where = " WHERE 1=1";
    if($sSearch != ""){
        $where = " WHERE name like " . "'%" . $sSearch . "%'";
        $where = $where . " OR ip_address like " . "'%" . $sSearch . "%'";
    }

    $iTotalRecords = 0;

    $query = "SELECT COUNT(*) FROM host_pool" . $where;
    $totalnumrs = $ccdb_conn->Execute($query);
    if (!$totalnumrs){
        ProcExecInfo($sEcho, "totalnum get failed!");
        return;
    }
    if (!$totalnumrs->EOF) {
        $iTotalRecords = $totalnumrs->fields[0];
    }
    $totalnumrs->Close();

    $filed = array("oid", "name", "ip_address", "name", "running_vms");
    $orderfield = $filed[intval($iSortCol)];

    if($sSortDir == ""){
        $orderdir = "asc";
    }
    else{
        $orderdir = $sSortDir;
    }
    $order = " ORDER BY " . $orderfield . " " . $orderdir;

    $limit = " LIMIT " . $iDisplayLength . " OFFSET " . $iDisplayStart;

    $param =          "oid, name, ip_address, run_state, enabled, create_time, cpu_info_id, vmm_info, os_info, mem_total, mem_max, disk_total, disk_max, ";
    $param = $param . " cpu_usage_1m, cpu_usage_5m, cpu_usage_30m, hardware_state, tcu_num, all_tcu, host_pool_in_use.description, location, is_support_hvm,";
    $param = $param . " running_vms, all_free_tcu, mem_free, disk_free, b.number, b.description, cpus, c.bureau, c.rack, c.shelf, c.slot, c.rate_power, running_time";

    $query = "select " . $param . " from host_pool_in_use FULL OUTER JOIN host_device b ON oid = b.hid FULL OUTER JOIN host_atca c ON oid=c.hid " . $where . $order . $limit;

    $rs = $ccdb_conn->Execute($query);

    $hostData = Array();
    $hostData["sEcho"] = $sEcho;
    $hostData["iTotalRecords"] = $iTotalRecords;
    $hostData["iTotalDisplayRecords"] = $iTotalRecords;
    $hostData["aaData"] = Array();

    if ($rs){
        $rawData = Array();
        while (!$rs->EOF) {
            $oneRow = Array();
            $oneRow["host_id"]       = $rs->fields[0];
            $oneRow["host_name"]     = $rs->fields[1];
            $oneRow["ip_address"]    = $rs->fields[2];
            $oneRow["run_state"]     = $rs->fields[3];
            $oneRow["is_disabled"]   = ($rs->fields[4] == 1)?true:false;
            $oneRow["create_time"]   = gmdate("Y-m-d H:i:s", $rs->fields[5]+8*60*60);
            $oneRow["cpu_info_id"]   = $rs->fields[6];
            $oneRow["vmm_info"]      = $rs->fields[7];
            $oneRow["os_info"]       = $rs->fields[8];
            $oneRow["mem_total"]     = $rs->fields[9];
            $oneRow["mem_max"]       = $rs->fields[10];
            $oneRow["disk_total"]    = $rs->fields[11];
            $oneRow["disk_max"]      = $rs->fields[12];
            $oneRow["cpu_usage_1m"]  = $rs->fields[13];
            $oneRow["cpu_usage_5m"]  = $rs->fields[14];
            $oneRow["cpu_usage_30m"] = $rs->fields[15];
            $oneRow["hardware_state"] = $rs->fields[16];
            //$oneRow["processor_num"]  = $rs->fields[17];
            $oneRow["tcu_num"]        = $rs->fields[17];
            $oneRow["tcu_max"]        = $rs->fields[18];  //all_tcu
            $oneRow["description"]    = $rs->fields[19];
            $oneRow["location"]       = $rs->fields[20];
            $oneRow["is_support_hvm"] = ($rs->fields[21] == 1)?true:false;
            $oneRow["running_vms"]    = $rs->fields[22];
            $oneRow["tcu_free"]       = $rs->fields[23];  //all_free_tcu
            $oneRow["mem_free"]       = $rs->fields[24];
            $oneRow["disk_free"]      = $rs->fields[25];
            $oneRow["pdh"]            = $rs->fields[26];
            $oneRow["pdh_desc"]       = $rs->fields[27];
            $oneRow["cpus"]           = $rs->fields[28];
            $oneRow["bureau"]         = $rs->fields[29];
            $oneRow["rack"]           = $rs->fields[30];
            $oneRow["shelf"]          = $rs->fields[31];
            $oneRow["slot"]           = $rs->fields[32];
            $oneRow["rate_power"]     = $rs->fields[33];    
            $oneRow["running_time"]   = $rs->fields[34];        
            $oneRow["total_poweroff_time"] = "";
            $rawData[] = $oneRow;

            $rs->MoveNext();
        }
        $hostData["rawData"] = $rawData;
        $rs->Close();
        print json_encode($hostData);
    }
    else{
        ProcExecInfo($sEcho, $query);
    }
?>