<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $sid = $_GET["sid"];

    $tc_db_conn = GetTCDbConnect($err_code, $err_info);
    if(!$tc_db_conn){
        ExitWithError($err_info);
    }

    $storage_query = "select * from storage_adaptor where sid=" . $sid;

    $rs = $tc_db_conn->Execute($storage_query);
    if (!$rs){
        ExitWithError($storage_query . " execute failed!");
    }

    $sa_info = Array();
    if(!$rs->EOF){
        $sa_info["sid"]     = $rs->fields[0];
        $sa_info["name"]           = $rs->fields[1];
        $sa_info["sa_name"]        = $rs->fields[2];
        $sa_info["ctrl_address"]   = $rs->fields[3];
        $sa_info["type"]           = $rs->fields[4];
        $sa_info["is_online"]      = $rs->fields[5];
        $sa_info["enabled"]        = $rs->fields[6];
        $sa_info["description"]    = $rs->fields[7];
        $sa_info["register_time"]  = $rs->fields[8];//gmdate("Y-m-d H:i:s", $rs->fields[8]);
    }
    $rs->Close();

    $sa_info["free"] = 0;
    $safree_query = "select * from storage_adaptor_sum_free_view where sid=" . $sid;
    $rs = $tc_db_conn->Execute($safree_query);
    if($rs && !$rs->EOF){
        $sa_info["free"] = $rs->fields[1]*1024*1024;    
    }
    $rs->Close();
    
    $sa_info["used"] = 0;
    $satotal_query = "select sum(size) from storage_vg where sid=" . $sid;
    $rs = $tc_db_conn->Execute($satotal_query);
    if($rs && !$rs->EOF){
        $sa_info["used"] = $rs->fields[0]*1024*1024 - $sa_info["free"];    
    }
    $rs->Close();

    $vg_query = "select vg.name,vg.size,f.free_size from storage_vg vg, storage_vg_free_view f";
    $vg_query = $vg_query . " where vg.name=f.vg_name and vg.sid=f.sid and vg.sid=" . $sid;
    $rs = $tc_db_conn->Execute($vg_query);
    if (!$rs){
        ExitWithError($vg_query . " execute failed!");
    }

    $sa_info["vg"] = Array();
    while(!$rs->EOF){
        $onevg["name"] = $rs->fields[0];
        $onevg["size"] = $rs->fields[1]*1024*1024;
        $onevg["free"] = $rs->fields[2]*1024*1024;
        $onevg["count"] = -1;
        $volcount_query = "select count(*) from storage_volume where vg_name='" . $onevg["name"] . "' and sid=" . $sid;
        $rs1 = $tc_db_conn->Execute($volcount_query);
        if($rs1 && !$rs1->EOF){
            $onevg["count"] = $rs1->fields[0];
        }
        $rs1->Close();
        $sa_info["vg"][] = $onevg;

        $rs->MoveNext();
    }
    $rs->Close();

    $volume_query = "select v.sid, v.vg_name, v.name, v.size, v.volume_id, v.volume_uuid, v.request_id, ";
    $volume_query = $volume_query . " (select u.usage from storage_user_volume u where u.request_id = v.request_id) as usage ";
    $volume_query = $volume_query . " from storage_volume v where v.sid=" . $sid;
    $rs = $tc_db_conn->Execute($volume_query);
    if (!$rs){
        ExitWithError($volume_query . " execute failed!");
    }

    $sa_info["volume"] = Array();
    while(!$rs->EOF){
        $onevolume["vg"]          = $rs->fields[1];
        $onevolume["name"]        = $rs->fields[2];
        $onevolume["size"]        = $rs->fields[3]*1024*1024;
        $onevolume["volume_id"]   = $rs->fields[4];
        $onevolume["volume_uuid"] = $rs->fields[5];
        $onevolume["request_id"]  = $rs->fields[6];
        $onevolume["usage"]       = $rs->fields[7];

        $sa_info["volume"][] = $onevolume;
        $rs->MoveNext();
    }
    $rs->Close();    

    print json_encode($sa_info);
?>