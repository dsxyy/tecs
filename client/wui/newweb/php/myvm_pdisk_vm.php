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
    $iSortCol       = $_GET["iSortCol_0"];
    $sSortDir       = $_GET["sSortDir_0"];
    $sSearch        = $_GET["sSearch"];
    $iTotalRecords = 0;
    $filed = array("name","size","vm");
    $orderfield = "";
    $orderdir = "";
    $order = "";
    
    $conn = GetTCDbConnect($errcode,$errinfo);
    
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }
    
    /* $filter = " 1=1";
    
    if($user_name != ""){
        $filter = $filter . " and username = " . "'" . $user_name . "'";
    }
    
    if($sSearch != ""){
        $filter = $filter . " and ( ";
        $filter = $filter . " name like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or description like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(vmcount,'99999999999999999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or username like " . "'%" . $sSearch . "%'";
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
	
    //查询表记录数
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
    $totalnumrs->Close(); */
	
	
    //查表数据
    $query = ' select vid,deployed_cluster from web_view_vm';
    $query = $query . " WHERE user_name='" . $user_name . "' and deployed_cluster is not null"; 
    //$query = $query . ' ORDER by' . $order;
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
     
    $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
    while (!$rs->EOF) {
        $arrProjData = Array();
        $arrProjData["vid"]   = $rs->fields[0]; 
        $arrProjData["disks"] = Array(); 
        
        if( GetVmDiskInfo($conn, $rs->fields[1], $rs->fields[0], $arrProjData["disks"], $errcode, $errinfo) ){
            $ackdata["data"]["aaData"][] = $arrProjData;    
        }    
        
        $rs->MoveNext();
    }
	
	//查询是不是cdrom
    $query = "select vid from vmcfg_image where target='hdc'";
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }	
	if($rs->RecordCount()!= 0){
	    $arrTypeData = Array();
        while (!$rs->EOF) {
	        $arrTypeData[]["vid"] = $rs->fields[0];
            $rs->MoveNext();
        }
		
	    for($i=0; $i<count($ackdata["data"]["aaData"]);$i++){	    
	        for($j=0; $j<count($arrTypeData);$j++){
	            if($ackdata["data"]["aaData"][$i]['vid']==$arrTypeData[$j]['vid']){
			        array_push($ackdata["data"]["aaData"][$i]['disks'],'hdc');			    
			    }	     
	        }	
	    }		
	}
	
    $rs->Close();
    $conn->Close();

    print json_encode($ackdata);    
?>
