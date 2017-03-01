<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $pg_uuid = $_GET['pg_uuid'];

    $conn = GetVNMDbConnect($errcode,$errinfo);
    
    if(!$conn){
        ExitWithError($err_info);
    }

    //查表数据
    $query = 'select pg_type, acl, qos, allowed_priorities, version, mgr_id, pg_type, switchport_mode, ';
    $query = $query . ' (select n.uuid from netplane n where n.id=web_view_portgroup.netplane_id) as netplane_uuid,';
    $query = $query . ' promiscuous, default_vlan_id, mtu, native_vlan_num, access_vlan_num, isolate_no, name, description,';
    $query = $query . ' (select n.name from netplane n where n.id=web_view_portgroup.netplane_id) as netplane_name,';  
    $query = $query . 'is_sdn,isolate_type,segment_id_num';
    $query = $query . " from web_view_portgroup where uuid='" . $pg_uuid . "'";

    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        ExitWithError($query . " execute failed!");
    }

    $portgroup = Array();
    $portgroup[0] = 0;
    $portgroup[1] = Array();    

    while (!$rs->EOF) {
        $portgroup[1]["uuid"]    = $pg_uuid;
        $portgroup[1]["pg_type"] = $rs->fields[0]; 
        $portgroup[1]["acl"]     = $rs->fields[1];  
        $portgroup[1]["qos"]     = $rs->fields[2];         
        $portgroup[1]["prior"]   = $rs->fields[3];  
        $portgroup[1]["ver"]     = $rs->fields[4];  
        $portgroup[1]["mgr"]     = $rs->fields[5];  
        $portgroup[1]["pg_type"] = $rs->fields[6];  
        $portgroup[1]["switchport_mode"] = $rs->fields[7];  
        $portgroup[1]["netplane_uuid"] = $rs->fields[8];  
        $portgroup[1]["promiscuous"] = $rs->fields[9];  
        $portgroup[1]["default_vlan_id"] = $rs->fields[10]; 
        $portgroup[1]["mtu"] = $rs->fields[11]; 
        $portgroup[1]["native_vlan_num"] = $rs->fields[12]; 
        $portgroup[1]["access_vlan_num"] = $rs->fields[13]; 
        $portgroup[1]["isolate_no"] = $rs->fields[14]; 
        $portgroup[1]["pg_name"] = $rs->fields[15]; 
        $portgroup[1]["desc"] = $rs->fields[16];
        $portgroup[1]["netplane_name"] = $rs->fields[17] == null ? "" : $rs->fields[17];      
        if($rs->fields[18] == 1){
            $portgroup[1]["is_sdn"] = "yes";
        }
        else{
            $portgroup[1]["is_sdn"] = "no";
        }
        $portgroup[1]["isolate_type"] = $rs->fields[19];
        $portgroup[1]["segment"] = $rs->fields[20] == null ? "" : $rs->fields[20];
        $rs->MoveNext();
    }

    $rs->Close();

    $portgroup[1]["vlan_range"] = Array();

    //查表数据
    $query = "select vlan_begin,vlan_end from web_view_portgroup_vlanrange where pg_uuid='" . $pg_uuid . "'";

    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        ExitWithError($query . " execute failed!");
    }

    while (!$rs->EOF) {
        $one = Array();

        $one[] = $rs->fields[0];
        $one[] = $rs->fields[1];

        $portgroup[1]["vlan_range"][] = $one;

        $rs->MoveNext();
    }

    $rs->Close();    

    print json_encode($portgroup);
?>