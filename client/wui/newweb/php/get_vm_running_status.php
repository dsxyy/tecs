<?php
    include('adodb_connect.php');
    include('vm_common.php');
    
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");
    
    $sEcho          = "";
    $user_name      = $_GET["user_name"];
    
    $running_cnt = 0;
    $paused_cnt   = 0;
    $shutoff_cnt = 0;
    $crashed_cnt = 0;
    $unknown_cnt = 0;
    $freeze_cnt = 0;
    
    $iTotalRecords = 1;
    $conn = GetTCDbConnect($errcode,$errinfo);
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }

    $filter = " 1=1";
    if($user_name != ""){
        $filter = " user_name = " . "'" . $user_name . "'";
    }
    
    //获取虚拟机(已部署)记录
    $deploy_filter = $filter . " and deployed_cluster is not null";
    $query = ' SELECT vid,deployed_cluster ';
    $query = $query . ' FROM web_view_vm';
    $query = $query . ' WHERE' . $deploy_filter;
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
    
    $clusterlist = Array();
    while (!$rs->EOF) {
        $vid = $rs->fields[0];
        $deployed_cluster = $rs->fields[1];
        $clusterlist[$deployed_cluster][] = $vid;
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
                if($dynamicinfo["state"] == "2"){
                    $running_cnt = $running_cnt + 1;
                }
                else if($dynamicinfo["state"] == "3"){
                    $paused_cnt = $paused_cnt + 1;
                }
                else if($dynamicinfo["state"] == "4"){
                    $shutoff_cnt = $shutoff_cnt + 1;
                }
                else if($dynamicinfo["state"] == "5"){
                    $crashed_cnt = $crashed_cnt + 1;
                }
                else if($dynamicinfo["state"] == "6"){
                    $unknown_cnt = $unknown_cnt + 1;
                }
                else if($dynamicinfo["state"] == "8"){
                    $freeze_cnt = $freeze_cnt + 1;
                }
            }
        }
    }
    $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
    $arrData = Array();
    $arrData[] = $running_cnt;
    $arrData[] = $paused_cnt;
    $arrData[] = $shutoff_cnt;
    $arrData[] = $crashed_cnt;
    $arrData[] = $unknown_cnt;
    $arrData[] = $freeze_cnt;
    $ackdata["data"]["aaData"][] = $arrData;
    $rs->Close();
    $conn->Close();
    print json_encode($ackdata);
?>
