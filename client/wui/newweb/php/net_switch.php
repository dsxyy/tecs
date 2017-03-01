<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $conn = GetVNMDbConnect($errcode,$errinfo);
    
    if(!$conn){
        ExitWithError($err_info);
    }

    //查表数据
    $query = 'select s.uuid, s.name, s.switch_type, s.uplink_pg_name, s.max_mtu, s.evb_mode, s.nic_max_num_cfg, ';
    $query = $query . '(select count(*) FROM web_view_switch_vna_port p WHERE p.switch_uuid = s.uuid and p.vna_is_online = 1) as port_count, ';
    $query = $query . 's.uplink_pg_uuid from web_view_switch s';

    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        ExitWithError($query . " execute failed!");
    }

    $switch_data = Array();
    $switch_data[0] = 0;
    $switch_data[1] = Array();    

    while (!$rs->EOF) {
        $one_switch = Array();

        $one_switch[] = $rs->fields[0];
        $one_switch[] = $rs->fields[1];
        $one_switch[] = $rs->fields[2];        
        $one_switch[] = $rs->fields[3];
        $one_switch[] = $rs->fields[4];        
        $one_switch[] = $rs->fields[5];
        $one_switch[] = $rs->fields[6];        
        $one_switch[] = $rs->fields[7];
        $one_switch[] = $rs->fields[8];

        $switch_data[1][] = $one_switch;

        $rs->MoveNext();
    }

    $rs->Close();

    print json_encode($switch_data);    
?>