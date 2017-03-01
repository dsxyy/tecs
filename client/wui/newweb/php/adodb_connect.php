<?php
    include('adodb/adodb.inc.php'); //包含adodb类库文件
    
    /*该函数要废弃，使用GetQueryAckData*/
    function ProcExecInfo($sEcho, $error){
        $execinfo = '{"sEcho": ' . $sEcho . ',';
        $execinfo = $execinfo . '"iTotalRecords": "0"' . ',';
        $execinfo = $execinfo . '"iTotalDisplayRecords": "0"' . ',';
        $execinfo = $execinfo . '"err_info":' . $error . ',';
        $execinfo = $execinfo . '"aaData": [ ';
        $execinfo = $execinfo . ']}';
        print $execinfo;
    }
    
    function ExitWithError($errinfo){
        $rs = Array();
        $rs[] = -1;
        $rs[] = $errinfo;
        exit(json_encode($rs));
    } 

    function GetQueryAckData($errcode,$errinfo,$sEcho,$iTotalRecords){
        $ackdata = Array();
        $ackdata["errcode"] = $errcode;
        $ackdata["errinfo"] = $errinfo;
        $data = Array();
        $data["sEcho"] = $sEcho;
        $data["iTotalRecords"] = $iTotalRecords;
        $data["iTotalDisplayRecords"] = $iTotalRecords;
        $data["aaData"] = Array();
        $ackdata["data"] = $data;
        return $ackdata;
    }
    
    function GetTCDbConnect(&$errcode,&$errinfo){
        $dbcfg = parse_ini_file('db_connect.ini');
        $dbdriver = $dbcfg["driver"];
        $tcdb_connect = ADONewConnection($dbdriver);
        if(!$tcdb_connect){
            $errcode = "loadfailed_dbdriver";
            $errinfo = "errinfo_loadfailed_dbdriver";
            return false;
        }
        $db_host = $dbcfg["host"];
        $db_port = $dbcfg["port"];
        $db_user = $dbcfg["user"];
        $db_pass = $dbcfg["password"];
        $db_name = $dbcfg["dbname"];
        $db_host = $db_host . ":" . $db_port; 
        
        $result = $tcdb_connect->Connect($db_host,$db_user,$db_pass,$db_name);
        if(!$result){
            $errcode = "connectfailed_tcdb";
            $errinfo = "errinfo_connectfailed_tcdb";
            return false;
        }
        $tcdb_connect->debug = false;
        return $tcdb_connect;
    }
    
    function GetClusterDbConnect($cluster_ip,$cluster_port,$cluster_user,$cluster_pwd,$cluster_dbname,&$errcode,&$errinfo){
        $dbcfg = parse_ini_file('db_connect.ini');
        $dbdriver = $dbcfg["driver"];
        $clusterdb_connect = ADONewConnection($dbdriver);
        if(!$clusterdb_connect){
            $errcode = "loadfailed_dbdriver";
            $errinfo = "errinfo_loadfailed_dbdriver";
            return false;
        }
        $clusterdb_host = $cluster_ip . ":" . $cluster_port; 
            
        $result = $clusterdb_connect->Connect($clusterdb_host,$cluster_user,$cluster_pwd,$cluster_dbname);
        if(!$result){
            $errcode = "connectfailed_ccdb";
            $errinfo = "errinfo_connectfailed_ccdb";
            return false;
        }
        $clusterdb_connect->debug = false;
        return $clusterdb_connect;
    }
    
    function GetClusterDbConnect_By_QueryCluster($tc_conn,$clustername,&$errcode,&$errinfo){
        $tmp_errcode    = "";
        $tmp_errinfo    = "";
        $cluster_dbname = "";
        $cluster_ip     = "";
        $cluster_port   = "";
        $cluster_user   = "";
        $cluster_pwd    = "";
        
        //查询集群表
        $query = "select db_name,db_server_url,db_server_port,db_user,db_passwd from cluster_pool where name = " . "'" . $clustername ."'";
        $rs = $tc_conn->Execute($query);
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
            $rs->MoveNext();
        }
        $rs->Close();
        $cc_conn = GetClusterDbConnect($cluster_ip,$cluster_port,$cluster_user,$cluster_pwd,$cluster_dbname,$tmp_errcode,$tmp_errinfo);
        if(!$cc_conn){
            $errcode = $tmp_errcode;
            $errinfo = $tmp_errinfo;
            return false;
        }
        return $cc_conn;
    }
    
    function GetClusterDbConnect_By_ClusterName($clustername,&$errcode,&$errinfo){
        $tmp_errcode     = "";
        $tmp_errinfo     = "";
        $cluster_dbname = "";
        $cluster_ip     = "";
        $cluster_port   = "";
        $cluster_user   = "";
        $cluster_pwd    = "";
        
        //获取TC连接
        $tc_conn = GetTCDbConnect($tmp_errcode,$tmp_errinfo);
        if(!$tc_conn){
            $errcode = $tmp_errcode;
            $errinfo = $tmp_errinfo;
            return false;
        }
        //查询集群表获取CC连接
        $cc_conn = GetClusterDbConnect_By_QueryCluster($tc_conn,$clustername,$tmp_errcode,$tmp_errinfo);
        $tc_conn->Close();
        if(!$cc_conn){
            $errcode = $tmp_errcode;
            $errinfo = $tmp_errinfo;
            return false;
        }
        return $cc_conn;
    }
    
    function GetVNMDbConnect(&$errcode,&$errinfo){
        $vnm_dbcfg = parse_ini_file('db_connect.ini');
        $dbdriver = $vnm_dbcfg["driver"];
        $vnmdb_connect = ADONewConnection($dbdriver);
        if(!$vnmdb_connect){
            $errcode = "loadfailed_dbdriver";
            $errinfo = "errinfo_loadfailed_dbdriver";
            return false;
        }

        $db_host = $vnm_dbcfg["vnm_host"];
        $db_port = $vnm_dbcfg["vnm_port"];
        $db_user = $vnm_dbcfg["vnm_user"];
        $db_pass = $vnm_dbcfg["vnm_password"];
        $db_name = $vnm_dbcfg["vnm_dbname"];
        $db_host = $db_host . ":" . $db_port; 
        
        $result = $vnmdb_connect->Connect($db_host,$db_user,$db_pass,$db_name);
        if(!$result){
            $errcode = "connectfailed_ccdb";
            $errinfo = "errinfo_connectfailed_vnmdb";
            return false;
        }

        $vnmdb_connect->debug = false;
        
        return $vnmdb_connect;
		
    	/*	
    	$vnm_ip = "127.0.0.1";
    	$vnm_port = "5432";
    	$vnm_user = "tecs";
    	$vnm_pwd = "tecs";
    	$vnm_dbname = "vnm_new";
    	
    	$vnmdb_host = $vnm_ip . ":" . $vnm_port; 
    	
        $dbcfg = parse_ini_file('db_connect.ini');
        $dbdriver = $dbcfg["driver"];
        $vnmdb_connect = ADONewConnection($dbdriver);
        if(!$vnmdb_connect){
            $errcode = "loadfailed_dbdriver";
            $errinfo = "errinfo_loadfailed_dbdriver";
            return false;
        }
            
        $result = $vnmdb_connect->Connect($vnmdb_host, $vnm_user, $vnm_pwd, $vnm_dbname);
        if(!$result){
            $errcode = "connectfailed_ccdb";
            $errinfo = "errinfo_connectfailed_vnmdb";
            return false;
        }    

        $vnmdb_connect->debug = false;    

        return $vnmdb_connect;
        */
    }	    
	
?> 