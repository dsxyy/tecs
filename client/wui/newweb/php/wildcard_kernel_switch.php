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
    //$query = "select name, uuid from web_view_switch a where switch_type ='1'";
    $query = "select distinct switch_name, switch_uuid from web_view_switch_vna_port where switch_type = '1' and is_loop = '0'";
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
     
    $switch_data = Array();
    $switch_data[0] = 0;
    $switch_data[1] = Array();    

    while (!$rs->EOF) {        
		$one_switch = Array();
        if($rs->fields[1] != null){
            $one_switch["name"] = $rs->fields[0];
            $one_switch["uuid"] = $rs->fields[1];                          
            $switch_data[1][] = $one_switch;    
        }
        $rs->MoveNext();
    }
    $rs->Close();

    $conn->Close(); 	

    print json_encode($switch_data);  
?>