<?php
    include('adodb_connect.php');
    include('myvm_pdisk_common.php');
	
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache"); 
     
    $sEcho          = $_GET['sEcho'];
    $iDisplayStart  = $_GET['iDisplayStart'];
    $iDisplayLength = $_GET['iDisplayLength'];
    $user_name      = $_GET["user_name"];
    $iSortCol       = $_GET["iSortCol_0"];
    $sSortDir       = $_GET["sSortDir_0"];
    $sSearch        = $_GET["sSearch"];
	
    $iTotalRecords = 0;
    $filed = array("name", "size", "description", "username", "username", "username");
    $orderfield = "";
    $orderdir = "";
    $order = "";
	
    $conn = GetTCDbConnect($errcode,$errinfo);  
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }
    $filter = " 1=1";

    if($user_name != ""){
        $filter = $filter . " and username in (" . $user_name . ")";
    }
	
    if($sSearch != ""){
        $filter = $filter . " and ( ";
        $filter = $filter . " name like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(size,'99999999999999999') like " . "'%" . $sSearch . "%'";		
        $filter = $filter . " or username like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or description like " . "'%" . $sSearch . "%'";		
        $filter = $filter . ")";
    }
	

    $orderfield = $filed[intval($iSortCol)];

    if($sSortDir == ""){
        $orderdir = "asc";
    }
    else{
        $orderdir = $sSortDir;
    } 
     
    $order = " " . $orderfield . " " . $orderdir;
    $order = $order . " LIMIT " . $iDisplayLength . " OFFSET " . $iDisplayStart;
	
		
    //查询表记录数
    $query = "SELECT COUNT(*) FROM web_view_pdisk WHERE " . $filter;
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
	  
    //查询表数据
    $query = 'select p.request_id, p.name, p.size, p.sid, p.description, p.username,';
    $query = $query . ' (select usage from storage_user_volume v where v.request_id=p.request_id) as usage';    
    $query = $query . ' from web_view_pdisk p';
    $query = $query . ' WHERE' . $filter;
    $query = $query . ' ORDER by' . $order;
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }	 
    $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
	
    while (!$rs->EOF) {	
		//查询cluster	
		/*$tsid = $rs->fields[3];				
        $cluster_query = "select cluster_name from storage_cluster where sid='" . $tsid . "'";
        $cluster_rs = $conn->Execute($cluster_query);	
        if (!$cluster_rs){
            $conn->Close();
            die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
        }   		
        $cluster = Array();
        while (!$cluster_rs->EOF) {
            $cluster[] = $cluster_rs->fields[0];
            $cluster_rs->MoveNext();
        }

        $vidshow = "";	
        $curRequestid = $rs->fields[0];		
        //每个cluster的vm_disk表中挂载在哪个虚拟机下		
        foreach($cluster as $clustername){	
            $dynamicinfos = '';
            $ret = GetDiskShareInfo($conn,$clustername,$curRequestid,$errcode,$errinfo,$dynamicinfos);			
			
            if(!$ret){
                $rs->Close();
                $clusterdb_conn->Close();
                return false;
            }
            $vidshow = $dynamicinfos;		
        }*/     
		
        $arrData = Array();
        $arrData["request_id"] = $rs->fields[0];        
        $arrData[] = $rs->fields[1];            //name
        $arrData[] = $rs->fields[2]*1024*1024;  //size
        $arrData[] = $rs->fields[4];            //description

        $vid   = "";
        $drive = "";
        if($rs->fields[6] != null) {
           $vid   = strtok($rs->fields[6], ":");
           $drive = strtok(":");
        }     
        $arrData[] = $vid;
        $arrData[] = $drive;
		
        $arrData[] = $rs->fields[5];            //user		
				
        $ackdata["data"]["aaData"][] = $arrData;
        $rs->MoveNext();
    }	
	
    $rs->Close();
    $conn->Close();
    print json_encode($ackdata);    
?>
