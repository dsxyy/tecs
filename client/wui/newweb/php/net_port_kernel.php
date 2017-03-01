<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $iTotalRecords = 0;
 
    $conn = GetVNMDbConnect($errcode,$errinfo);
    
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }

    $filter = " 1 = 1"; 

    //表记录数
    $query = "SELECT COUNT(distinct port_uuid) FROM web_view_port_kernel WHERE " . $filter;
    $totalnumrs = $conn->Execute($query);
    if (!$totalnumrs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
    while (!$totalnumrs->EOF) {
        $iTotalRecords = $totalnumrs->fields[0];
        break;
    }
    $totalnumrs->Close(); 
   
    //查表数据
    $query = ' select vna_uuid,port_uuid,port_name,port_state,kernel_type,is_has_pg,pg_uuid,is_has_vswitch,vswitch_uuid,vna_is_online,';
    $query = $query . '( select distinct cluster_name from v_net_vna_hc_cluster where v_net_vna_hc_cluster.vna_uuid =web_view_port_kernel.vna_uuid) as cluster_name,';
    $query = $query . '( select distinct host_name from v_net_vna_hc_cluster where v_net_vna_hc_cluster.vna_uuid =web_view_port_kernel.vna_uuid) as host_name,';
    $query = $query . '( select ip from web_view_port_ip where web_view_port_ip.port_uuid =web_view_port_kernel.port_uuid) as ip,';
    $query = $query . '( select mask from web_view_port_ip where web_view_port_ip.port_uuid =web_view_port_kernel.port_uuid) as mask'; 
    $query = $query . ' from web_view_port_kernel';
    $query = $query . ' WHERE' . $filter;
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
     
    $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
    while (!$rs->EOF) {
        $one = Array();
        $one[] = "";

        $one[] = $rs->fields[2];//port_name
        $one[] = $rs->fields[10];//cluster_name
        $one[] = $rs->fields[11];//host_name
        $one[] = $rs->fields[9];//vna_is_online
        $one[] = $rs->fields[12];//ip
        $one[] = $rs->fields[13];//mask
       
		
        if($rs->fields[3] == 1){//port_state
            $one[] = "up";
        }
        else if($rs->fields[3] == 0){
            $one[] = "down";
        }
		/*
        if($rs->fields[4] == 10){
            $one[] = "管理通道";
        }
        else if($rs->fields[4] == 11){
            $one[] = "存储通道";
        }
        else{*/
            $one[] = $rs->fields[4];//kernel_type
       // }
		if($rs->fields[5] ==1){	//端口组	
		    $pg_uuid = $rs->fields[6];
		    $pg_nameQuery = "select name from web_view_portgroup where uuid=" ."'". $rs->fields[6] ."'";
		    $pg_name = $conn->Execute($pg_nameQuery);			
			if(!$pg_name){
			    $conn->Close();
				die(json_encode(GetQueryAckData("failed_querydb",$pg_nameQuery,$sEcho,0)));
			}	  
			while(!$pg_name->EOF){
			    $one[] = $pg_name->fields[0];
                break;			  
			}			     
            $pg_name->Close();            
		}
		else{
		    $one[] = $rs->fields[5];
		}
		
		if($rs->fields[7] ==1){	//switch    
		    $s_uuid = $rs->fields[8];
		    $s_nameQuery = "select name from web_view_switch where uuid=" ."'". $rs->fields[8] ."'";
		    $s_name = $conn->Execute($s_nameQuery);			
			if(!$s_name){
			    $conn->Close();
				die(json_encode(GetQueryAckData("failed_querydb",$s_nameQuery,$sEcho,0)));
			}
			while(!$s_name->EOF){
			    $one[] = $s_name->fields[0];
			    break;
			}			          
            $s_name->Close();            
		}
		else{
		    $one[] = $rs->fields[7];
		}
		
		$one["vna_uuid"] = $rs->fields[0];//vna_uuid
        $one["uuid"] = $rs->fields[1];//port_uuid
        $one["pg_uuid"] = $rs->fields[6];//pg_uuid		
        $ackdata["data"]["aaData"][] = $one;
        
        $rs->MoveNext();
    }
    $rs->Close();

    $conn->Close(); 


    print json_encode($ackdata);  
?>