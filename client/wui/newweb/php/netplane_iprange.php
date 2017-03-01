<?php
    include('adodb_connect.php');
    
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");
    
    $sEcho          = "";
    $uuid   = $_GET["uuid"];
    
    $iTotalRecords = 1;
    $conn = GetVNMDbConnect($errcode,$errinfo);
    
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }
    //查表数据
    $query = "select ip_begin,ip_end,mask from web_view_netplane_iprange where netplane_uuid = '" . $uuid ."'";
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
    $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
    while (!$rs->EOF) {
        $arrData = Array();
        $arrData[] = $rs->fields[0];//ip_begin
        $arrData[] = $rs->fields[1];//ip_end
        $arrData[] = $rs->fields[2];//mask
        $arrData[] = "<a href='javascript:void(0);' onclick='delete_one_iprange(this)'><i class='icon-trash'></i></a>";
        $ackdata["data"]["aaData"][] = $arrData;
        $rs->MoveNext();
    }
    $rs->Close();
    $conn->Close();
    print json_encode($ackdata);
?>
