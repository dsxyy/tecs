 <?php   
    function GetDiskShareInfo($conn,$running_cluster,$requestid,&$errcode,&$errinfo,&$vmdynamicinfo){
	
        //查询集群表
        $query = "select db_name,db_server_url,db_server_port,db_user,db_passwd from cluster_pool where name = " . "'" . $running_cluster ."'";
        $rs = $conn->Execute($query);		
          if(!$rs){
            $errcode = "failed_querydb";
            $errinfo = $query;
			
            return false;
        }
        /*集群不存在*/
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
			
        $query = "select vid ";
        $query = $query . "from vm_disk ";
		$query = $query . "where request_id='" . $requestid . "'";
		
        $rs_vid = $clusterdb_conn->Execute($query);	
        if($rs_vid && !$rs_vid->EOF){
            $vmdynamicinfo = $rs_vid->fields[0];
        }
        $rs_vid->Close();	
        $clusterdb_conn->Close();
        return true;   
    }
?> 