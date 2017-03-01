<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $pg_uuid = $_GET["pg_uuid"];

    $conn = GetVNMDbConnect($errcode,$errinfo);
    
    if(!$conn){
        ExitWithError($err_info);
    }

    //查表数据
    $query = "select vlan_begin,vlan_end from web_view_portgroup_vlanrange where pg_uuid='" . $pg_uuid . "'";

    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        ExitWithError($query . " execute failed!");
    }

    $data = Array();
    $data[0] = 0;
    $data[1] = Array();    

    while (!$rs->EOF) {
        $one = Array();

        $one[] = $rs->fields[0];
        $one[] = $rs->fields[1];
        $one[] = "<a href='javascript:void(0);' onclick='delete_one_vlanrange(this)'><i class='icon-trash'></i></a>";

        $data[1][] = $one;

        $rs->MoveNext();
    }
    $rs->Close();

    $query = "select * from pf_net_query_netplane_vmshare_vlans((select n.uuid from netplane n, web_view_portgroup p where n.id=p.netplane_id and p.uuid='" . $pg_uuid . "'))";
    $rs = $conn->Execute($query);
    if (!$rs){
        ExitWithError($query . " execute failed!");
    }
    if (!$rs->EOF) {
        $data[2] = $rs->fields[0];
        if($data[2] != "" && substr($data[2], -1) == ","){
            $data[2] = substr($data[2], 0, strlen($data[2]) - 1);
        }
    }
    $rs->Close();    

    $conn->Close();

    print json_encode($data);    
?>