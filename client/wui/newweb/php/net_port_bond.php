<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $sEcho          = $_GET['sEcho'];
    $iDisplayStart  = $_GET['iDisplayStart'];
    $iDisplayLength = $_GET['iDisplayLength'];
    $iSortCol       = $_GET["iSortCol_0"];
    $sSortDir       = $_GET["sSortDir_0"];
    $sSearch        = "";//$_GET["sSearch"];
    $sVNAid         = $_GET["sVNAid"];

    $iTotalRecords = 0;

    $filed = array("bond_name", "bond_mode","is_linked");

    $orderfield = "";
    $orderdir = "";
    $order = "";
   
    $conn = GetVNMDbConnect($errcode,$errinfo);
    
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }

    $filter = " 1 = 1"; 
    $filter = $filter . " and  vna_uuid = '" . $sVNAid . "'";
    
    if($sSearch != ""){
        $filter = $filter . " and ( ";
        $filter = $filter . " bond_name like " . "'%" . $sSearch . "%'";
        $filter = $filter . " or to_char(bond_mode,'99999999999999999') like " . "'%" . $sSearch . "%'";
        $filter = $filter . ")";
    }


    if($iSortCol == "0"){
        $orderfield = $filed[0];
    }
    else{
        $orderfield = $filed[intval($iSortCol)];
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
    $query = "SELECT COUNT(distinct bond_uuid) FROM web_view_port_bond WHERE " . $filter;
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
   // $query = ' select distinct bond_name, bond_mode, is_linked, bond_uuid,lacp_is_success,backup_is_active ';
    $query = ' select distinct bond_name, bond_mode,bond_uuid ';
    $query = $query . ' from web_view_port_bond';
    $query = $query . ' WHERE' . $filter;
    $query = $query . ' ORDER by' . $order;
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
     
    $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
    while (!$rs->EOF) {
        $one = Array();
		
        $one[] = $rs->fields[0];//bond_name
        $one[] = $rs->fields[1];//bond_mode

        $membersLinkQuery = "SELECT phy_name,is_linked FROM web_view_port_bond WHERE bond_uuid ='" . $rs->fields[2] . "'";
	    $membersLinked = $conn->Execute( $membersLinkQuery);
	    $memberLinked = Array();	
	    while(!$membersLinked->EOF){
            if( $membersLinked->fields[1] ==1){
                $memberLinked[] = $membersLinked->fields[0] .":". "up" ;
            }
            else if( $membersLinked->fields[1] ==0){
                $memberLinked[] = $membersLinked->fields[0] .":". "down";
            }               
            $membersLinked->MoveNext();
	    }
	    $one[] = $memberLinked;
        
		if($rs->fields[1] == 1){//主备模式
            $membersBondQuery = "SELECT phy_name,backup_is_active FROM web_view_port_bond WHERE bond_uuid ='" . $rs->fields[2] . "'";
            $membersBond = $conn->Execute( $membersBondQuery);
            $memberBond = Array();	
            while(!$membersBond->EOF){
                //$memberBond[] = $membersBond->fields[0];
                if( $membersBond->fields[1] ==1){
                    $memberBond[] = $membersBond->fields[0] .":" ."Active" ;
                }
                else if( $membersBond->fields[1] ==0){
                    $memberBond[] = $membersBond->fields[0] .":" ."Other";
                }               
                $membersBond->MoveNext();
            }
            $one[] = $memberBond;
        }
        else if($rs->fields[1] == 4){//协议模式
            $membersBondQuery = "SELECT phy_name,lacp_is_success FROM web_view_port_bond WHERE bond_uuid ='" . $rs->fields[2] . "'";
            $membersBond = $conn->Execute( $membersBondQuery);
            $memberBond = Array();	
            while(!$membersBond->EOF){
                //$memberBond[] = $membersBond->fields[0];
                if( $membersBond->fields[1] ==1){
                    $memberBond[] = $membersBond->fields[0] .":" ."Succ" ;
                }
                else if( $membersBond->fields[1] ==0){
                    $memberBond[] = $membersBond->fields[0] .":" . "Fail";
                }               
                $membersBond->MoveNext();
            }
            $one[] = $memberBond;
        }
		/* 
	    $ipQuery = "select ip from web_view_port_ip where web_view_port_ip.port_uuid ='" . $rs->fields[2] . "'";
	    $ips = $conn->Execute($ipQuery);
	    $ip = Array();	
	    while(!$ips->EOF){
                $ip[] = $ips->fields[0];
                $ips->MoveNext();
	    }
	    $one[] = $ip;		
		*/
        $ackdata["data"]["aaData"][] = $one;
        
        $rs->MoveNext();
    }
    $rs->Close();

    $conn->Close(); 

    print json_encode($ackdata);  
?>