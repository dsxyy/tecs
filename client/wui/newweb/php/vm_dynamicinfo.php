<?php
    include('adodb_connect.php');
    include('vm_common.php');
    
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");
    $vmsid            = $_GET["vmsid"];
    $sEcho            = $_GET['sEcho'];
    $vid              = "0";
    $state            = "255";
    $last_op          = "0";
    $last_op_result   = "0";
    $last_op_progress = "0";
    $ip               = "";
    $tc_config_version = "0";
    $cc_config_version = "0";
    $run_version       = "0";
    
    $iTotalRecords     = 0;
    
    $conn = GetTCDbConnect($errcode,$errinfo);
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }
    
    //查表数据
    $query = ' SELECT vid,deployed_cluster,config_version ';
    $query = $query . ' FROM web_view_vm';
    $query = $query . ' WHERE';
    $filter = " deployed_cluster is not null";
    $filter = $filter . " and vid in (" . $vmsid . ")";
    $query = $query . $filter;
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
    
    $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
    $clusterlist = Array();
    while (!$rs->EOF) {
        $arrData = Array();        
        $vid              = $rs->fields[0];
        $deployed_cluster = $rs->fields[1];
        $tc_config_version = $rs->fields[2];
        $clusterlist[$deployed_cluster][] = $vid;
        $arrData[] = $vid;
        $arrData[] = $state;
        $arrData[] = $last_op;
        $arrData[] = $last_op_result;
        $arrData[] = $last_op_progress;
        $arrData[] = $ip;
        $arrData[] = $tc_config_version;
        $arrData[] = $cc_config_version;
        $arrData[] = $run_version;
        $ackdata["data"]["aaData"][] = $arrData;
        
        $ret = GetVmDynamicInfo($conn,$deployed_cluster,$vid,$errcode,$errinfo);
        if(!$ret){
            $conn->Close();
            die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
        }
        
        $rs->MoveNext();
    }
    $cnt = count($clusterlist);
    if($cnt > 0){//查CC数据库，获取VM动态信息
        foreach($clusterlist as $clustername => $cluster){
            $vids = "";
            foreach($cluster as $vid){
                $vids = $vids . $vid . ",";
            }
            if(strlen($vids) > 0){
                $vids = substr($vids,0,strlen($vids) - 1);
            }
            $dynamicinfos = array();
            $ret = GetVmDynamicInfo($conn,$clustername,$vids,$errcode,$errinfo,$dynamicinfos);
            if(!$ret){
                $rs->Close();
                $conn->Close();
                die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
            }
            foreach($dynamicinfos as $dynamicindex => $dynamicinfo){
                foreach($ackdata["data"]["aaData"] as $ackindex => $data){
                    if($dynamicindex == $data[0]){
                        $ackdata["data"]["aaData"][$ackindex][1] = $dynamicinfo["state"];
                        $ackdata["data"]["aaData"][$ackindex][2] = $dynamicinfo["last_op"];
                        $ackdata["data"]["aaData"][$ackindex][3] = $dynamicinfo["last_op_result"];
                        $ackdata["data"]["aaData"][$ackindex][4] = $dynamicinfo["last_op_progress"];
                        $ackdata["data"]["aaData"][$ackindex][5] = $dynamicinfo["ip"];
                        $ackdata["data"]["aaData"][$ackindex][7] = $dynamicinfo["config_version"];
                        $ackdata["data"]["aaData"][$ackindex][8] = $dynamicinfo["run_version"];
                    }
                }
            }
        }
    }
    $rs->Close();
    $conn->Close();
    print json_encode($ackdata);
?>
