<?php
    include('adodb_connect.php');
      
    function GetVmState($conn,$running_cluster,$vid){
        //查询集群表
        $query = "select db_name,db_server_url,db_server_port,db_user,db_passwd from cluster_pool where cluster_name = " . "'" . $running_cluster ."'";
        $rs = $conn->Execute($query);
        if (!$rs || $rs->RecordCount() == 0){
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
        $clusterdb_conn  = GetClusterDbConnect($cluster_ip,$cluster_port,$cluster_user,$cluster_pwd,$cluster_dbname);
        $rs->Close();
        if(!$clusterdb_conn){
            return false;
        }
        $query = "select to_char(state,'99') as vm_state from vm_pool where vid = ". "'" . $vid . "'";
        $rs = $clusterdb_conn->Execute($query);
        if (!$rs || $rs->RecordCount() == 0){
            $clusterdb_conn->Close();
            return false;
        }
        while (!$rs->EOF) {
            $state = $rs->fields[0];
            break;
        }
        $rs->Close();
        $clusterdb_conn->Close();
        return $state;
    }
     
    $conn = GetDbConnect();
    if(!$conn){
        print 'GetDbConnect failed!';
        return;
    }
    // Performing SQL query
    $query = 'SELECT vid,name,user_id,user_name,project_id,project_name,vcpu,tcu,memory,disk,running_cluster,create_time  FROM vmcfg_full_info WHERE 1=1 ORDER by vid LIMIT 10 OFFSET 0';
    //$query = 'SELECT vid  FROM vmcfg_full_info';
    $rs = $conn->Execute($query);
    
    if (!$rs){
        print $conn->ErrorMsg();
    }
    else{
        while (!$rs->EOF) {
            print $rs->fields[0].' '.$rs->fields[1].' '.$rs->fields[2].' '.$rs->fields[3].' '.$rs->fields[4].' '.$rs->fields[5].' '.$rs->fields[6].' '.$rs->fields[7].' '.$rs->fields[8].' '.$rs->fields[9].' '.$rs->fields[10].' '.$rs->fields[11].' '.$rs->fields[12].' '.'<BR>';
            $rs->MoveNext();
        }
    }
    $running_cluster = "tecscluster";
    $vid = "7";
    GetVmState($conn,$running_cluster,$vid);
?> 