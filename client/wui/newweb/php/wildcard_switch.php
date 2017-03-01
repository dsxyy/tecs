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
    $query = 'select   uuid, switch_name, switch_type,  pg_name,  evb_mode,  nic_max_num, port_num, state, port_name, port_type,bond_mode,pg_uuid';
    $query = $query . ' from web_view_wildcast_cfg_switch';

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

        $one[] = $rs->fields[0]; //uuid
        $one[] = $rs->fields[1]; //switch_name
        $one[] = $rs->fields[2]; //switch_type   
        $one[] = $rs->fields[3]; //pg_name
        $one[] = $rs->fields[4]; //evb_mode
        $one[] = $rs->fields[5]; //nic_max_num
        $one[] = $rs->fields[6]; //port_num
        $one[] = $rs->fields[7]; //
        //$one[] = $rs->fields[8];
        if($rs->fields[9] == 0 ||$rs->fields[9] == 4){
            $one[] = $rs->fields[8];//port_name
        }
        else if($rs->fields[9] == 1 ){
            $inner_query = "select phy_port_name from web_view_wildcast_cfg_switch_bondmap where switch_uuid = '" .$rs->fields[0] ."'";
            $inner_rs = $conn->Execute($inner_query);
            if (!$inner_rs){
                $conn->Close();
                ExitWithError($inner_query . " execute failed!");
            }
             $names ="";
            while(!$inner_rs->EOF){

                $names =$names . $inner_rs->fields[0] .",";
                $inner_rs->MoveNext();
                //$one[] = $inner_rs->fields[0];
            }
            $one[] = $names;
        }
        $one[] = $rs->fields[9];//port_type
        $one[] = $rs->fields[10];//bond_mode
        $one[] = $rs->fields[11];//bond_mode		
        $data[1][] = $one;

        $rs->MoveNext();
    }

    $rs->Close();

    print json_encode($data);    
?>