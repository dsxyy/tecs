<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $conn = GetVNMDbConnect($errcode,$errinfo);
    
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }

    //查表数据
    $query = ' select vna_uuid';
    
    //$query = $query .'(select switch_uuid from v_net_switch_port_vna where web_view_switch.uuid = v_net_switch_port_vna.switch_uuid and  ( port_type =0 or port_type= 1))as uuid';
    $query = $query . ' from web_view_vna';
    $query = $query . '  where vna_is_online = 1 ';

    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
     
    $vna_data = Array();
    $vna_data[0] = 0;
    $vna_data[1] = Array();    

    while (!$rs->EOF) {        
		$one_vna = Array();
       
        $one_vna["uuid"] = $rs->fields[0];
            //$one_switch["uuid"] = $rs->fields[1];                          
        $vna_data[1][] = $one_vna;    
        
        $rs->MoveNext();
    }
    $rs->Close();

    $conn->Close(); 	

    print json_encode($vna_data);  
?>