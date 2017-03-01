<?php
    include('adodb_connect.php');
    
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");
    
    $sEcho  = "";
    $uuid   = $_GET["uuid"];
    
    $iTotalRecords = 1;
    $conn = GetVNMDbConnect($errcode,$errinfo);
    
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }

    $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);

    //查表数据
    $query = "select vlan_begin,vlan_end from web_view_netplane_vlanrange where netplane_uuid = '" . $uuid ."'";

    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }

    while (!$rs->EOF) {
        $arrData = Array();
        $arrData[] = $rs->fields[0];//vlan_begin
        $arrData[] = $rs->fields[1];//vlan_end
        $arrData[] = "<a href='javascript:void(0);' onclick='delete_one_vlanrange(this)'><i class='icon-trash'></i></a>";
        $ackdata["data"]["aaData"][] = $arrData;
        $rs->MoveNext();
    }
    $rs->Close();

    $query = "select * from pf_net_query_netplane_vmshare_vlans('" . $uuid . "')";
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
    if (!$rs->EOF) {
        $ackdata["data"]["sharevlan"] = $rs->fields[0];
        if($ackdata["data"]["sharevlan"] != "" && substr($ackdata["data"]["sharevlan"], -1) == ","){            
            $ackdata["data"]["sharevlan"] = substr($ackdata["data"]["sharevlan"], 0, strlen($ackdata["data"]["sharevlan"]) - 1);
        }
    }
    $rs->Close();    

    $conn->Close();
    print json_encode($ackdata);
?>
