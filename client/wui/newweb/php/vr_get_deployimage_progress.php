<?php
    include('adodb_connect.php');
    
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");
    
    $cluster_caches = $_GET["cluster_caches"];
    $language       = $_GET["language"];
    if($language == 'ch'){
        $state = "state_ch";
    }
    else{
        $state = "state_en";
    }
    $sEcho              = "";
    $iTotalRecords      = "0";
    $cache_ids          = "";
    $arr_cluster_cache  = Array();
    $arr_cluster_cacheids = Array();
    $cluster_cache_list = explode(";",$cluster_caches);
    
    $tc_conn = GetTCDbConnect($errcode,$errinfo);
    if(!$tc_conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }
    //初始化cluster_cache 关键字: clustername:cache_id
    foreach($cluster_cache_list as $cluster_cache){
        $arr_cluster_cache[$cluster_cache]["progress"]   = "";
        $arr_cluster_cache[$cluster_cache]["state"]      = "";
        $arr_cluster_cache[$cluster_cache]["state_lang"] = "";
    }
    //获取集群下cache_id 关键字: clustername
    foreach($arr_cluster_cache as $key => $value){
        $cluster_cache_map = explode(":",$key);
        $arr_cluster_cacheids[$cluster_cache_map[0]]["cache_id"] = $arr_cluster_cacheids[$cluster_cache_map[0]]["cache_id"] . "," . $cluster_cache_map[1];
    }
    //删除第一个逗号
    foreach($arr_cluster_cacheids as $key => $value){
        $arr_cluster_cacheids[$key]["cache_id"] = substr($value["cache_id"],1);
    }
    foreach($arr_cluster_cacheids as $key => $value){
        $clustername = $key;
        $cache_ids   = $value["cache_id"]; 
        //查询TC数据库,获取状态
        $query = "select cluster_name,cache_id,state," . $state . " from web_view_deployimage";
        $query = $query . " where cluster_name = '" . $clustername . "'";
        $query = $query . " and cache_id in (". $cache_ids .")";
        $rs = $tc_conn->Execute($query);
        if (!$rs){
            $tc_conn->Close();
            die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
        }
        while (!$rs->EOF) {
            foreach($arr_cluster_cache as $key => $value){                
                $cluster_cache_index = $rs->fields[0] . ":" . $rs->fields[1];
                $arr_cluster_cache[$cluster_cache_index]["state"]      = $rs->fields[2];
                $arr_cluster_cache[$cluster_cache_index]["state_lang"] = $rs->fields[3];
                break;
            }
            $rs->MoveNext();
        }
        $rs->Close();
        //根据集群名获取CC数据库连接
        $cc_conn = GetClusterDbConnect_By_QueryCluster($tc_conn,$clustername,$errcode,$errinfo);
        if(!$cc_conn){
            $tc_conn->Close();
            die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
        }
        //查询CC数据库获取部署镜像进度
        $query = " SELECT cache_id,progress ";
        $query = $query . " FROM web_view_deploy_image";
        $query = $query . " WHERE";
        $query = $query . " cache_id in (". $cache_ids .")"; 
        $rs = $cc_conn->Execute($query);
        if (!$rs){
            $tc_conn->Close();
            $cc_conn->Close();
            die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
        }
        while (!$rs->EOF) {
            foreach($arr_cluster_cache as $key => $value){
                $cluster_cache_index = $clustername . ":" . $rs->fields[0];
                $arr_cluster_cache[$cluster_cache_index]["progress"] = $rs->fields[1];
                break;
            }
            $rs->MoveNext();
        }
        $rs->Close();
        $tc_conn->Close();
        $cc_conn->Close();
        $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
        foreach($arr_cluster_cache as $key => $value){
             $arrData = Array();
             $cluster_cache = explode(":",$key);
             $cluster_name  = $cluster_cache [0];
             $cache_id      = $cluster_cache [1];
             $arrData[]     = $cluster_name;//cluster_name 
             $arrData[]     = $cache_id;//cache_id 
             $arrData[]     = $value["progress"];//progress
             $arrData[]     = $value["state"];//state
             $arrData[]     = $value["state_lang"];//state_lang
             $ackdata["data"]["aaData"][] = $arrData;
        }
        print json_encode($ackdata);
    }
?>
