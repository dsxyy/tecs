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
    $query = ' select uuid, name, pg_type, switchport_mode, netplane_name, netplane_id ';
    $query = $query . ' from web_view_portgroup';
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
    $pg_data = Array();
    $pg_data[0] = 0;
    $pg_data[1] = Array();    

    while (!$rs->EOF) {        
		$onePg = Array();
        $onePg["uuid"]    = $rs->fields[0];//checkbox
        $onePg["name"]    = $rs->fields[1];//name
        $onePg["pg_type"] = $rs->fields[2];//type
        $onePg["desc"]    = "";      
        $onePg["switchport_mode"] = $rs->fields[3];//version          
        $onePg["netplane"] = $rs->fields[4];//netplane_name
        $onePg["netplane_id"] = $rs->fields[5];//netplane_id
                        
        $pg_data[1][] = $onePg;    
        
        $rs->MoveNext();
    }
    $rs->Close();


    $conn->Close();
	print json_encode($pg_data);   
?>