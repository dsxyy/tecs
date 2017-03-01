<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $logicnet_name   = $_GET["logicnet"];
    $logicnet_uuid   = $_GET["uuid"];  

    $conn = GetVNMDbConnect($errcode,$errinfo);

    if(!$conn){
        ExitWithError($err_info);
    }

    //查表数据
    $query = ' select a.ip_begin, a.ip_end, a.mask, a.logic_network_uuid ';
    $query = $query . ' from web_view_logicnet_iprange a, v_net_logicnetwork_portgroup_netplane b';
    $query = $query . " where a.netplane_uuid=b.netplane_uuid and b.logicnetwork_uuid='" . $logicnet_uuid . "'";
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        ExitWithError($query . " execute failed!");
    }
     
    $logicnet_iprange = Array();
    $logicnet_iprange[0] = 0;
    $logicnet_iprange[1] = Array();

    while (!$rs->EOF) {
        $oneRange = Array();

        $oneRange["ip_begin"] = $rs->fields[0];
        $oneRange["ip_end"] = $rs->fields[1];
        $oneRange["ip_mask"] = $rs->fields[2];
        $oneRange["uuid"] = $rs->fields[3];

        $logicnet_iprange[1][] = $oneRange;
        
        $rs->MoveNext();
    }
    $rs->Close();

    $conn->Close();

    print json_encode($logicnet_iprange);  
?>