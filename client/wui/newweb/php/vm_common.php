<?php
    
    function GetVmIp($vid,&$errcode,&$errinfo,&$ip){
        /*
        $tcmac_query = "select mac from web_view_vmmac where vid = " . "'" . $vid ."'";
        $tcmac_rs = $conn->Execute($tcmac_query);
        if(!$tcmac_rs){
            $errcode = "failed_querydb";
            $errinfo = $tcmac_query;
            return false;
        }
        if ($tcmac_rs->RecordCount() == 0){
            $tcmac_rs->Close();
            return true;
        }
        while (!$tcmac_rs->EOF) {
            $tcmac = $tcmac_rs->fields[0];
            $tcmac = strtoupper($tcmac);
            $ccip_query = "select ip from web_view_mac_ip where mac = " . "'" . $tcmac ."'";
            $ccip_rs = $clusterdb_conn->Execute($ccip_query);
            if(!$ccip_rs){
                $errcode = "failed_querydb";
                $errinfo = $ccip_query;
                $tcmac_rs->Close();
                return false;
            }
            if ($ccip_rs->RecordCount() == 0){
                $tcmac_rs->MoveNext();
            }
            else{
                while (!$ccip_rs->EOF) {
                    if($ip != ""){
                        $ip = $ip . '<br>' .$ccip_rs->fields[0];
                    }
                    else{
                        $ip = $ip . $ccip_rs->fields[0];
                    }
                    $ccip_rs->MoveNext();
                }
                $tcmac_rs->MoveNext();
            }
            $ccip_rs->Close();
        }
        $tcmac_rs->Close();
        return true;
        */
        $conn = GetVNMDbConnect($errcode,$errinfo);
    
        if(!$conn){
            $errcode = "failed_querydb";
            $errinfo = "connect vnm db failed!";
            return false;
        }
        $vnmnetinfo_query = "select ip from web_view_vm_netinfo where vm_id = " . "'" . $vid ."'";
        $vnmnetinfo_rs = $conn->Execute($vnmnetinfo_query);
        if(!$vnmnetinfo_rs){
            $errcode = "failed_querydb";
            $errinfo = $vnmnetinfo_query;
            return false;
        }
        if ($vnmnetinfo_rs->RecordCount() == 0){
            $vnmnetinfo_rs->Close();
            return true;
        }		
	    while (!$vnmnetinfo_rs->EOF) {
            if($vnmnetinfo_rs->fields[0] != ""){
                if($ip != ""){
                    $ip = $ip . '<br>' .$vnmnetinfo_rs->fields[0];
                }
                else{
                    $ip = $ip . $vnmnetinfo_rs->fields[0];
                }  
            }
            
            $vnmnetinfo_rs->MoveNext();					
        }

    	$vnmnetinfo_rs->Close();
    	$conn->Close();
    	return true;
    }
    
    function GetVmDynamicInfo($conn,$running_cluster,$vids,&$errcode,&$errinfo,&$vmdynamicinfo){
      
        $query = "select db_name,db_server_url,db_server_port,db_user,db_passwd from cluster_pool where name = " . "'" . $running_cluster ."'";
        $rs = $conn->Execute($query);
        if(!$rs){
            $errcode = "failed_querydb";
            $errinfo = $query;
            return false;
        }
        
        if ($rs->RecordCount() == 0){
            $errcode = "noexist_record";
            $errinfo = $query;
            return false;
        }
        while (!$rs->EOF) {
            $cluster_dbname = $rs->fields[0];
            $cluster_ip = $rs->fields[1];
            $cluster_port = $rs->fields[2];
            $cluster_user = $rs->fields[3];
            $cluster_pwd = $rs->fields[4];
            break;
        }
        $clusterdb_conn  = GetClusterDbConnect($cluster_ip,$cluster_port,$cluster_user,$cluster_pwd,$cluster_dbname,$errcode,$errinfo);
        $rs->Close();
        if(!$clusterdb_conn){
            $errinfo = "cc lost";
            return false;
        }
        $query = "select v.vid,state,last_op,last_op_result,last_op_progress,config_version,run_version, ";
        $query = $query . "(select workflow_id from web_view_vm_workflow w where w.vid=v.vid limit 1) as workflow, ";
        $query = $query . "(select operation from vmm_workflow w1 where w1.vid = v.vid limit 1) as vm_op ";
        $query = $query . "from web_view_vmstate v ";
        $query = $query . "where vid in (". $vids . ")";
        $rs = $clusterdb_conn->Execute($query);
        if(!$rs){
            $errcode = "failed_querydb";
            $errinfo = $query;
            $clusterdb_conn->Close();
            return false;
        }
        while (!$rs->EOF) {
            $vid                                     = $rs->fields[0];
            $vmdynamicinfo[$vid]["state"]            = $rs->fields[1];
            $vmdynamicinfo[$vid]["last_op"]          = $rs->fields[2];
            $vmdynamicinfo[$vid]["last_op_result"]   = $rs->fields[3];
            $vmdynamicinfo[$vid]["last_op_progress"] = $rs->fields[4];
            $vmdynamicinfo[$vid]["config_version"]   = $rs->fields[5];
            $vmdynamicinfo[$vid]["run_version"]      = $rs->fields[6];
            $vmdynamicinfo[$vid]["workflow"]         = $rs->fields[7];
            
            if($rs->fields[8] != null){
                $vmdynamicinfo[$vid]["last_op"] = $rs->fields[8];
                $vmdynamicinfo[$vid]["last_op_result"] = 33;
            }

            $ip = "";
            $ret = GetVmIp($vid,$errcode,$errinfo,$ip);
            if(!$ret){
                $ip = $errinfo;               
            }
            else {
                $vmdynamicinfo[$vid]["ip"] = $ip;
            }
            $rs->MoveNext();
        }
        $rs->Close();
        $clusterdb_conn->Close();
        return true;   
    }
	
	function GetVmDynamicWorkflow($conn,$running_cluster,$vids,&$errcode,&$errinfo,&$vmdynworkflow){
     
        $query = "select db_name,db_server_url,db_server_port,db_user,db_passwd from cluster_pool where name = " . "'" . $running_cluster ."'";
        $rs = $conn->Execute($query);
        if(!$rs){
            $errcode = "failed_querydb";
            $errinfo = $query;
            return false;
        }
       
        if ($rs->RecordCount() == 0){
            $errcode = "noexist_record";
            $errinfo = $query;
            return false;
        }
        while (!$rs->EOF) {
            $cluster_dbname = $rs->fields[0];
            $cluster_ip = $rs->fields[1];
            $cluster_port = $rs->fields[2];
            $cluster_user = $rs->fields[3];
            $cluster_pwd = $rs->fields[4];
            break;
        }
        $clusterdb_conn  = GetClusterDbConnect($cluster_ip,$cluster_port,$cluster_user,$cluster_pwd,$cluster_dbname,$errcode,$errinfo);
        $rs->Close();
        if(!$clusterdb_conn){
            return false;
        }
        $query = "select vid,workflow_name,progress,workflow_id ";
        $query = $query . "from web_view_vm_workflow ";
        $query = $query . "where vid in (". $vids . ")";
        $rs = $clusterdb_conn->Execute($query);
        if(!$rs){
            $errcode = "failed_querydb";
            $errinfo = $query;
            $clusterdb_conn->Close();
            return false;
        }
        while (!$rs->EOF) {
            $vid                                     = $rs->fields[0];
			$vmdynworkflow[$vid]["vid"]              = $rs->fields[0];
            $vmdynworkflow[$vid]["workflow_name"]    = $rs->fields[1];
            $vmdynworkflow[$vid]["progress"]         = $rs->fields[2];
            $vmdynworkflow[$vid]["workflow_id"]      = $rs->fields[3];            
            $rs->MoveNext();
        }
        $rs->Close();
        $clusterdb_conn->Close();
        return true;   
    }

    function GetVmDiskInfo($conn, $running_cluster, $vid, &$disks, &$errcode, &$errinfo){
      
        $query = "select db_name,db_server_url,db_server_port,db_user,db_passwd from cluster_pool where name = " . "'" . $running_cluster ."'";
        $rs = $conn->Execute($query);
        if(!$rs){
            $errcode = "failed_querydb";
            $errinfo = $query;
            return false;
        }
        if ($rs->RecordCount() == 0){
            $errcode = "noexist_record";
            $errinfo = $query;
            return false;
        }
        while (!$rs->EOF) {
            $cluster_dbname = $rs->fields[0];
            $cluster_ip = $rs->fields[1];
            $cluster_port = $rs->fields[2];
            $cluster_user = $rs->fields[3];
            $cluster_pwd = $rs->fields[4];
            break;
        }

        $clusterdb_conn  = GetClusterDbConnect($cluster_ip,$cluster_port,$cluster_user,$cluster_pwd,$cluster_dbname,$errcode,$errinfo);
        $rs->Close();
        if(!$clusterdb_conn){
            return false;
        }

      
        $query = "select state from web_view_vmstate where vid=" . $vid;
        $rs = $clusterdb_conn->Execute($query);
        if(!$rs){
            $errcode = "failed_querydb";
            $errinfo = $query;
            $clusterdb_conn->Close();
            return false;
        }
        if (!$rs->EOF) {
            $vmstate = $rs->fields[0];
            $rs->Close();

            if($vmstate != 4/*shutoff_state*/){
                $clusterdb_conn->Close();                
                return false;
            }
        }

      
        $query = "select target from vm_disk where vid=" . $vid;
        $rs = $clusterdb_conn->Execute($query);
        if(!$rs){
            $errcode = "failed_querydb";
            $errinfo = $query;
            $clusterdb_conn->Close();
            return false;
        }
        while (!$rs->EOF) {
            $disks[] = $rs->fields[0];
            $rs->MoveNext();
        }

        $rs->Close();
        $clusterdb_conn->Close();
        return true;   
    }

?> 