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

    if(isset($_GET['query_length'])){
        $iDisplayLength =  $_GET['query_length'];
    }

    $state = "255";
    
    $iTotalRecords = 0;
    $filed = array("vid","vm_name","vid","vcpu","tcu","memory","total_size", "vid","user_name","project_name");
    $orderfield = "";
    $orderdir = "";
    $order = "";
    $conn = GetTCDbConnect($errcode,$errinfo);
    
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }
    $filter = " 1=1";
    
    //$cluster_cache_list = explode(";",$cluster_caches);
    if($project_name != ""){
        $filter = $filter . " and ";
        $user_project_list = explode(";",$project_name);
        foreach($user_project_list as $user_project){
            $arr_user_project = explode(":",$user_project);
            $filter = $filter . "(user_name = " . "'" . $arr_user_project[0] . "'" . " and project_name = " . "'" . $arr_user_project[1] . "'" . ") or ";
        }
        if(strlen($filter) > 0){
            $filter = substr($filter,0,strlen($filter) - strlen(" or "));
        }
        
    }
    else{
        if($user_name != ""){
            $filter = " user_name in (" . $user_name . ")";
        }
    }
    if($sSearch != ""){
        $filter = $filter . " and ( ";
        $filter = $filter . " to_char(vid,'99999999999999999999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or vm_name like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(vcpu,'999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(tcu,'999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(memory,'9999999999999.99') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or (case when total_size  < 1 then '0' || trim(to_char(total_size,'9.99')) else to_char(total_size,'9999999999999.99') end) like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or user_name like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or project_name like " . "'%" . $sSearch . "%'";
        $filter = $filter . ")";
    }
    
    if($iSortCol == "0"){
        $orderfield = $filed[0];
    }
    else{
        $orderfield = $filed[intval($iSortCol) - 1];
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
    //表数据
    $query = 'SELECT vid,vm_name,vcpu,tcu,memory,total_size,user_name,project_name,deployed_cluster, ';
    $query = $query . '(select workflow_id from web_view_vm_workflow w where w.vid=web_view_vm.vid limit 1) as workflow, ';
    $query = $query . '(select enable_livemigrate from view_vmcfg_with_project v where v.oid=web_view_vm.vid limit 1) as enable_livemigrate ';
    $query = $query . ' FROM web_view_vm';
    $query = $query . ' WHERE' . $filter;
    $query = $query . ' ORDER by' . $order;
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
    $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
    $clusterlist = Array();
    while (!$rs->EOF) {
        $arrData = Array();
        $arrData[] = "";
        $arrData[] = $rs->fields[0];//vid
        $arrData[] = $rs->fields[1];//name
        $deployed_cluster = $rs->fields[8];
        if($deployed_cluster == ''){
            $state = "7";
        }
        else{
            $state = "255";
            $clusterlist[$deployed_cluster][] = $rs->fields[0];//vid
        }
        $arrData[] = $state;//state
        $arrData[] = $rs->fields[2];//vcpu
        $arrData[] = $rs->fields[3];//tcu
        $arrData[] = $rs->fields[4];//memory
        $arrData[] = $rs->fields[5];//disk
        $arrData[] = "";//IP
        $arrData[] = $rs->fields[6];//user_name
        $arrData[] = $rs->fields[7];//project_name
        $arrData[] = "";
        $arrData["deployed_cluster"] = $rs->fields[8];//deployed_cluster
        $arrData["workflow"]         = $rs->fields[9];//workflow
        $arrData["enable_livemigrate"] = $rs->fields[10];
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
                        if($dynamicindex == $data[1]){
                            $ackdata["data"]["aaData"][$ackindex][3] = $dynamicinfo["state"];
                            $ackdata["data"]["aaData"][$ackindex][8] = $dynamicinfo["ip"];
                            if($ackdata["data"]["aaData"][$ackindex]["workflow"] == null){
                                $ackdata["data"]["aaData"][$ackindex]["workflow"] = $dynamicinfo["workflow"];
                            }
                            else {
                                $ackdata["data"]["aaData"][$ackindex]["deployed_cluster"] = "";
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
