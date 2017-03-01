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
    $query = ' select sdn_uuid,sdn_conntype, sdn_connport, sdn_connip, quantum_serverip ,quantum_uuid';
    $query = $query . ' from web_view_sdn';
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
    $sdn_data = Array();
    $sdn_data[0] = 0;
    $sdn_data[1] = Array();    

    while (!$rs->EOF) {        
		$oneSDN = Array();
        $oneSDN[] = '<input type="checkbox" >';
        $oneSDN[] = $rs->fields[1];//sdn_conntype
        $oneSDN[] = $rs->fields[2];//sdn_connport
        $oneSDN[] = $rs->fields[3];//sdn_connip     
        $oneSDN[] = $rs->fields[4];//quantum_serverip  
        $oneSDN[] = $rs->fields[0];//sdn_uuid
        $oneSDN[] = $rs->fields[5];//quantum_uuid       
                        
        $sdn_data[1][] = $oneSDN;    
        
        $rs->MoveNext();
    }
    $rs->Close();


    $conn->Close();
	print json_encode($sdn_data);   
?>