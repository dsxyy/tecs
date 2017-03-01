<?php
    include('adodb_connect.php');
    include('vm_common.php');
    
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");
    $vmsid          = $_GET["vmsid"];
    $sEcho          = $_GET['sEcho'];
    $vid              = "0";
    $workflow_name    = "0";
    $progress         = "0";
    $workflow_id      = "0";
    $iTotalRecords    = 0;
    
    $conn = GetTCDbConnect($errcode,$errinfo);
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }
    
    //查表数据
    $query = ' SELECT vid,deployed_cluster,config_version ';
    $query = $query . ' FROM web_view_vm WHERE deployed_cluster is not null ';
    $query = $query . " and vid in (" . $vmsid . ")";

    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
    
    $clusterlist = Array();
    while (!$rs->EOF) {
        $arrData = Array();        

        $vid               = $rs->fields[0];
        $deployed_cluster  = $rs->fields[1];
        $tc_config_version = $rs->fields[2];

        $clusterlist[$deployed_cluster][] = $vid;
        
        $rs->MoveNext();
    }
    $rs->Close();    

    $wfdata = GetQueryAckData("","",$sEcho,$iTotalRecords); 

    $query = "select vid,workflow_name,progress,workflow_id ";
    $query = $query . "from web_view_vm_workflow ";
    $query = $query . "where vid in (". $vmsid . ")";
    
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb", $query, $sEcho, 0)));
    }
    
    while (!$rs->EOF) {
        $arrData = Array();

        $arrData[]      = $rs->fields[0]; //vid
        $arrData[]      = $rs->fields[1]; //workflow_name
        $arrData[]      = $rs->fields[2]; //progress
        $arrData[]      = $rs->fields[3]; //workflow_id
        $arrData[]      = "";             //cluster, tc is blank space
        
        $wfdata["data"]["aaData"][] = $arrData;        
        $rs->MoveNext();
    }
    $rs->Close();

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
            $ret = GetVmDynamicWorkflow($conn,$clustername,$vids,$errcode,$errinfo,$dynamicinfos);
            if(!$ret){
                $conn->Close();
                die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
            }
            foreach($dynamicinfos as $dynamicindex => $dynamicinfo){
				$workflowData = Array();

				$workflowData[]      = $dynamicinfo["vid"];           //vid
				$workflowData[]      = $dynamicinfo["workflow_name"]; //workflow_name
				$workflowData[]      = $dynamicinfo["progress"];      //progress
				$workflowData[]      = $dynamicinfo["workflow_id"];   //workflow_id
                $workflowData[]      = $clustername;                  //cluster
				$wfdata["data"]["aaData"][] = $workflowData;					
            }
        }
    }
    $rs->Close();
    $conn->Close();
    print json_encode($wfdata);
?>
