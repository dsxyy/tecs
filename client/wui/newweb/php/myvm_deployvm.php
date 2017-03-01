<?php
    include('adodb_connect.php');
    include('vm_common.php');
    
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");
    
    $sEcho          = $_GET['sEcho'];
    $iDisplayStart  = $_GET['iDisplayStart'];
    $iDisplayLength = $_GET['iDisplayLength'];
    $user_name      = $_GET["user_name"];
    $project_name   = $_GET["project_name"];
    $iSortCol       = $_GET["iSortCol_0"];
    $sSortDir       = $_GET["sSortDir_0"];
    $sSearch        = $_GET["sSearch"];
    
    $state             = "255";
    $last_op           = "0";
    $last_op_result    = "0";
    $last_op_progress  = "0";
    $ip                = "";
    $tc_config_version = "0";
    $cc_config_version = "0";
    $run_version       = "0";

    $iTotalRecords     = 0;
    $filed             = array("vid","vm_name","vcpu","tcu","memory","total_size","","project_name");
    $orderfield        = "";
    $orderdir          = "";
    $order             = "";
    
    $conn = GetTCDbConnect($errcode,$errinfo);
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }

    $filter = " 1=1";
    if($user_name != ""){
        $filter = " user_name = " . "'" . $user_name . "'";
    }
    if($project_name != ""){
        $filter = $filter . " and project_name in (" . $project_name . ")";
    }
    $filter = $filter . " and deployed_cluster is not null";
    
    if($sSearch != ""){
        $filter = $filter . " and ( ";
        $filter = $filter . " to_char(vid,'99999999999999999999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or vm_name like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(vcpu,'999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(tcu,'999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(memory,'9999999999999.99') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or (case when total_size  < 1 then '0' || trim(to_char(total_size,'9.99')) else to_char(total_size,'9999999999999.99') end) like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or project_name like " . "'%" . $sSearch . "%'";
        $filter = $filter . ")";
    }
    
    if($iSortCol == "0"){
        $orderfield = $filed[0];
    }
    else{
        $orderfield = $filed[intval($iSortCol) - 2];
    }    
    if($sSortDir == ""){
        $orderdir = "asc";
    }
    else{
        $orderdir = $sSortDir;
    }
    
    $order = " " . $orderfield . " " . $orderdir;
    $order = $order . " LIMIT " . $iDisplayLength . " OFFSET " . $iDisplayStart;
    //表记录数
    $query = "SELECT COUNT(*) FROM web_view_vm WHERE " . $filter;
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
    $query = ' select v.vid,vm_name,vcpu,tcu,memory,total_size,project_name,deployed_cluster,config_version, ';
    $query = $query . "(select workflow_id from web_view_vm_workflow w where w.vid=v.vid limit 1) as workflow ";    
    $query = $query . ' FROM web_view_vm v';
    $query = $query . ' WHERE' . $filter;
    $query = $query . ' ORDER by ' . $order;
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
    $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
    $clusterlist = Array();
    while (!$rs->EOF) {
        $arrData = Array();
        $vid = $rs->fields[0];
        $deployed_cluster = $rs->fields[7];
        $clusterlist[$deployed_cluster][] = $vid;
        $arrData[] = "";//第一列checkbox
        $arrData[] = $state;//state
        $arrData[] = $vid;//vid
        $arrData[] = $rs->fields[1];//name
        $arrData[] = $rs->fields[2];//vcpu
        $arrData[] = $rs->fields[3];//tcu
        $arrData[] = $rs->fields[4];//memory
        $arrData[] = $rs->fields[5];//disk
        $arrData[] = $ip;//ip
        $arrData[] = $rs->fields[6];//project_name
        $arrData[] = "";//VNC Access
        $arrData[] = $last_op;
        $arrData[] = $last_op_result;
        $arrData[] = $last_op_progress;
        $arrData[] = $rs->fields[8];//tc_config_version
        $arrData[] = $cc_config_version;
        $arrData[] = $run_version;
        $arrData["cluster"]  = $rs->fields[7];
        $arrData["workflow"] = $rs->fields[9];
        $ackdata["data"]["aaData"][] = $arrData;
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
            if($ret){
                foreach($dynamicinfos as $dynamicindex => $dynamicinfo){
                    foreach($ackdata["data"]["aaData"] as $ackindex => $data){
                        if($dynamicindex == $data[2]){
                            $ackdata["data"]["aaData"][$ackindex][1]  = $dynamicinfo["state"];
                            $ackdata["data"]["aaData"][$ackindex][11] = $dynamicinfo["last_op"];
                            $ackdata["data"]["aaData"][$ackindex][12] = $dynamicinfo["last_op_result"];
                            $ackdata["data"]["aaData"][$ackindex][13] = $dynamicinfo["last_op_progress"];
                            $ackdata["data"]["aaData"][$ackindex][8]  = $dynamicinfo["ip"];
                            $ackdata["data"]["aaData"][$ackindex][15] = $dynamicinfo["config_version"];
                            $ackdata["data"]["aaData"][$ackindex][16] = $dynamicinfo["run_version"];
                            if($ackdata["data"]["aaData"][$ackindex]["workflow"] == null){
                                $ackdata["data"]["aaData"][$ackindex]["workflow"] = $dynamicinfo["workflow"];
                            }
                            else {
                                $ackdata["data"]["aaData"][$ackindex]["cluster"] = "";
                            }
                        }
                    }
                }
            }
        }
    }
    $rs->Close();
    $conn->Close();
    print json_encode($ackdata);
?>
