<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $conn = GetVNMDbConnect($errcode,$errinfo);
  
    if(!$conn){
        ExitWithError($err_info);
    }

    $ipquery    = 'select sum(a.np_total_num), sum(a.np_alloc_num) from web_view_netplane_res_statatic_ip a';
    $macquery   = 'select sum(b.np_total_num), sum(b.np_alloc_num) from web_view_netplane_res_statatic_mac b';
    $vlanquery  = 'select sum(c.np_total_num), sum(c.np_alloc_num) from web_view_netplane_res_statatic_vlan c';
    $segmentquery = 'select sum(d.np_total_num),sum(d.np_alloc_num) from web_view_netplane_res_statatic_segment d';

    $info = Array();
    $info[0] = 0;
    $info[1] = Array();

    $rs = $conn->Execute($ipquery);
    if (!$rs){
        $conn->Close();
        ExitWithError($ipquery . " execute failed!");          
    }
    if (!$rs->EOF) {
        $info[1]["ip_total"] = $rs->fields[0] == null ? 0 : $rs->fields[0];
        $info[1]["ip_alloc"] = $rs->fields[1] == null ? 0 : $rs->fields[1];
    }
    $rs->Close();
		
    $rs = $conn->Execute($macquery);
    if (!$rs){
        $conn->Close();
        ExitWithError($macquery . " execute failed!");          
    }
    if (!$rs->EOF) {
        $info[1]["mac_total"] = $rs->fields[0] == null ? 0 : $rs->fields[0];
        $info[1]["mac_alloc"] = $rs->fields[1] == null ? 0 : $rs->fields[1];
    }
    $rs->Close();

    $rs = $conn->Execute($vlanquery);
    if (!$rs){
        $conn->Close();
        ExitWithError($vlanquery . " execute failed!");          
    }
    if (!$rs->EOF) {
        $info[1]["vlan_total"] = $rs->fields[0] == null ? 0 : $rs->fields[0];
        $info[1]["vlan_alloc"] = $rs->fields[1] == null ? 0 : $rs->fields[1];
    }
    $rs->Close();

    $rs = $conn->Execute($segmentquery);
    if (!$rs){
        $conn->Close();
        ExitWithError($segmentquery . " execute failed!");          
    }
    if (!$rs->EOF) {
        $info[1]["segment_total"] = $rs->fields[0] == null ? 0 : $rs->fields[0];
        $info[1]["segment_alloc"] = $rs->fields[1] == null ? 0 : $rs->fields[1];
    }
    $rs->Close();
    
    $conn->Close(); 

    print json_encode($info);  
?>