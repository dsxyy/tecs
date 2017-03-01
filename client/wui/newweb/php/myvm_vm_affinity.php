<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $user        = $_GET["user"]; 
    $vid         = $_GET["vid"];   

    $tc_db_conn = GetTCDbConnect($err_code, $err_info);
    if(!$tc_db_conn){
        ExitWithError($err_info);
    }

    $query = "select oid, name, description, level, is_forced from view_affinity_region where user_name='" . $user . "' order by name";
    $rs = $tc_db_conn->Execute($query);
    if (!$rs){
        ExitWithError($query . " excute failed!");
    }

    $vm_affinity = Array();
    $vm_affinity[0] = 0;
    $vm_affinity[1] = Array(); 
    $vm_affinity["added"] = null;     

    while (!$rs->EOF) {
        $one = Array();
        
        $one["id"]           = $rs->fields[0];
        $one["name"]         = $rs->fields[1]; 
        $one["description"]  = $rs->fields[2]; 
        $one["level"]        = $rs->fields[3]; 
        $one["is_forced"]    = $rs->fields[4]; 
        $one["vms"]          = Array();

        $query_vm = "select vid from view_vmcfg_with_affinity_region where affinity_region_id=" . $one["id"];
        $vm_rs = $tc_db_conn->Execute($query_vm);

        if(!$vm_rs){
        }
        else {
            while (!$vm_rs->EOF) {
                $one["vms"][] = $vm_rs->fields[0];
                if($vm_rs->fields[0] == $vid){
                    $vm_affinity["added"] = $one["id"];
                }
                $vm_rs->MoveNext();
            }
        }

        $vm_affinity[1][] = $one;
 
        $rs->MoveNext();
    }
    $rs->Close();

    print json_encode($vm_affinity);
?>