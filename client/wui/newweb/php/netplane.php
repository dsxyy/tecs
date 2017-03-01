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

    $ccdb_conn  = GetClusterDbConnect_By_ClusterName($cluster_name, $err_code, $err_info);
    if(!$ccdb_conn){
        ProcExecInfo($sEcho, $err_info);
        return;
    }

    $where = " WHERE 1=1";
    if($sSearch != ""){
        $where = " WHERE name like " . "'%" . $sSearch . "%'";
        $where = $where . " OR description like " . "'%" . $sSearch . "%'";
    }

    $iTotalRecords = 0;

    $query = "SELECT COUNT(*) FROM netplane " . $where;
    $totalnumrs = $ccdb_conn->Execute($query);
    if (!$totalnumrs){
        ProcExecInfo($sEcho, "totalnum get failed!");
        return;
    }
    if (!$totalnumrs->EOF) {
        $iTotalRecords = $totalnumrs->fields[0];
    }
    $totalnumrs->Close();

    $filed = array("name", "name", "description");
    $orderfield = $filed[intval($iSortCol)];

    if($sSortDir == ""){
        $orderdir = "asc";
    }
    else{
        $orderdir = $sSortDir;
    }
    $order = " ORDER BY " . $orderfield . " " . $orderdir;

    $limit = " LIMIT " . $iDisplayLength . " OFFSET " . $iDisplayStart;

    $query = "select * from netplane ". $where . $order . $limit;

    $rs = $ccdb_conn->Execute($query);

    $hostData = Array();
    $hostData["sEcho"] = $sEcho;
    $hostData["iTotalRecords"] = $iTotalRecords;
    $hostData["iTotalDisplayRecords"] = $iTotalRecords;
    $hostData["aaData"] = Array();

    if ($rs){
        while (!$rs->EOF) {
            $oneRow = Array();
            $oneRow[]        = "";
            $oneRow[]        = $rs->fields[0];
            $oneRow[]        = $rs->fields[1];
            $oneRow[]        = "";
            $hostData["aaData"][]     = $oneRow;

            $rs->MoveNext();
        }
        $rs->Close();
        print json_encode($hostData);
    }
    else{
        ProcExecInfo($sEcho, $query);
    }
?>