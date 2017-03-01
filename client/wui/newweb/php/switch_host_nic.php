<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");
    
    $conn = GetVNMDbConnect($errcode,$errinfo);
    
    if(!$conn){
        ExitWithError($err_info);
    }

    //查表数据
    $query = 'select a.vna_uuid, (select b.cluster_name from v_net_vna_hc_cluster b where a.vna_uuid=b.vna_uuid and b.vna_is_online=1) as cluster_name, ';
    $query = $query . 'a.hc_application from web_view_vna a where a.vna_is_online=1';

    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        ExitWithError($query . " execute failed!");
    }     

    $hosts = Array();
    $hosts[0] = 0;
    $hosts[1] = Array();

    while (!$rs->EOF) {
        $oneHost = Array();
        $oneHost["vna_uuid"]  = $rs->fields[0];      
        $oneHost["cluster_name"] = $rs->fields[1]; 
        $oneHost["host_name"] = $rs->fields[2];      
        $oneHost["nics"] = Array();

        $port_query = "select a.port_name, a.port_uuid, a.port_type, ";
        $port_query = $port_query . "(select switch_uuid from v_net_switch_port_vna b where a.port_uuid=b.port_uuid and switch_type=1) as switch_uuid, ";
        $port_query = $port_query . "(select switch_uuid from v_net_switch_port_vna b where a.port_uuid=b.port_uuid and switch_type=2) as switch_uuid2, ";
        $port_query = $port_query . "(select count(*) from v_net_sriov_port_vna c where a.port_uuid=c.uuid) as is_sriov ";
        $port_query = $port_query . " from v_net_vna_port a where vna_uuid='" . $oneHost["vna_uuid"] . "' and a.port_is_consistency=1";
        $port_query = $port_query . " order by a.port_name asc";

        $port_rs = $conn->Execute($port_query);
        if ($port_rs){
            while (!$port_rs->EOF) {
                $onePort = Array();
                $onePort["name"] = $port_rs->fields[0];
                $onePort["uuid"] = $port_rs->fields[1];
                $onePort["port_type"] = $port_rs->fields[2];
                $onePort["switch_uuid"] = $port_rs->fields[3];
                $onePort["switch_uuid2"] =  $port_rs->fields[4];
                $onePort["is_sriov"] = $port_rs->fields[5];

                $onePort["members"] = Array();
                if($onePort["port_type"] == 1){
                    $members_query = "select phy_name,bond_mode from v_net_bondmap_bond_phy where bond_uuid='" . $onePort["uuid"] . "'";
                    $members_rs = $conn->Execute($members_query);
                    if($members_rs){
                        while (!$members_rs->EOF) {
                            $onePort["members"][] = $members_rs->fields[0];
                            $onePort["bond_mode"] = $members_rs->fields[1];
                            $members_rs->MoveNext(); 
                        }
                        $members_rs->Close();
                    }
                }

                $oneHost["nics"][] = $onePort;    

                $port_rs->MoveNext();
            }

            $port_rs->Close();
        }
        
        $rs->MoveNext();

        $hosts[1][] = $oneHost; 
    }
    $rs->Close();

    $conn->Close();

    print json_encode($hosts);
?>