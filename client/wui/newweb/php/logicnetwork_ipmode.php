<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $logicnet_uuid   = $_GET["logicnet_uuid"];  

    $conn = GetVNMDbConnect($errcode,$errinfo);

    if(!$conn){
        ExitWithError($err_info);
    }

    //查表数据
    $query = "select ip_bootprotocolmode from web_view_logicnet where uuid='" . $logicnet_uuid . "'";
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        ExitWithError($query . " execute failed!");
    }
     
    $logicnet_iprange = Array();
    $logicnet_iprange[0] = 0;

    if (!$rs->EOF) {
        $logicnet_iprange[1] = $rs->fields[0];
    }
    $rs->Close();

    $conn->Close();

    print json_encode($logicnet_iprange);  
?>