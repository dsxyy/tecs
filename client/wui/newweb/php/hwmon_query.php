<?php
    include('adodb_connect.php');
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");

    $sEcho          = $_GET['sEcho'];
    $hid            = $_GET['host_id'];    
    $cluster_name   = $_GET["cluster_name"];
   
    

    $ccdb_conn = GetClusterDbConnect_By_ClusterName($cluster_name, $err_code, $err_info);
    if(!$ccdb_conn){
        ProcExecInfo($sEcho, $err_info);
        return;
    }

    $where = " WHERE hid = " . intval($hid);    

    $param =          " hid, temp1_name, temp1_value, temp2_name, temp2_value, temp3_name, temp3_value, temp4_name, temp4_value, temp5_name, temp5_value, temp6_name, temp6_value, ";
    $param = $param . " temp7_name, temp7_value, temp8_name, temp8_value, cpu0_vid_name, cpu0_vid_value, cpu1_vid_name, cpu1_vid_value, fan1_rpm_name, fan1_rpm_value,";
    $param = $param . " fan2_rpm_name, fan2_rpm_value, fan3_rpm_name, fan3_rpm_value, fan4_rpm_name, fan4_rpm_value, fan5_rpm_name, fan5_rpm_value, fan6_rpm_name, fan6_rpm_value, ";
	$param = $param . " fan7_rpm_name, fan7_rpm_value, fan8_rpm_name, fan8_rpm_value, in0_name, in0_value, in1_name, in1_value, in2_name, in2_value, ";
	$param = $param . " in3_name, in3_value, in4_name, in4_value, in5_name, in5_value, in6_name, in6_value, in7_name, in7_value, ";
	$param = $param . " in8_name, in8_value, in9_name, in9_value, in10_name, in10_value, in11_name, in11_value, in12_name, in12_value, ";
	$param = $param . " in13_name, in13_value, in14_name, in14_value  ";

    $query = "select " . $param . " from host_hwmon  " . $where ;

    $rs = $ccdb_conn->Execute($query);

    $hwmonData = Array();
    $hwmonData["sEcho"] = $sEcho;
   
    $hwmonData["aaData"] = Array();

    if ($rs){
        $rawData = Array();
        while (!$rs->EOF) {
            $oneRow = Array();
            $oneRow["host_id"]             = $rs->fields[0];
			
            $oneRow["temp1_name"]          = $rs->fields[1];
            $oneRow["temp1_value"]         = $rs->fields[2];
            $oneRow["temp2_name"]          = $rs->fields[3];
            $oneRow["temp2_value"]         = $rs->fields[4];
            $oneRow["temp3_name"]          = $rs->fields[5];
            $oneRow["temp3_value"]         = $rs->fields[6];
            $oneRow["temp4_name"]          = $rs->fields[7];
            $oneRow["temp4_value"]         = $rs->fields[8];
            $oneRow["temp5_name"]          = $rs->fields[9];
            $oneRow["temp5_value"]         = $rs->fields[10];
            $oneRow["temp6_name"]          = $rs->fields[11];
            $oneRow["temp6_value"]         = $rs->fields[12];
            $oneRow["temp7_name"]          = $rs->fields[13];
            $oneRow["temp7_value"]         = $rs->fields[14];
            $oneRow["temp8_name"]          = $rs->fields[15];
            $oneRow["temp8_value"]         = $rs->fields[16];
            $oneRow["cpu0_vid_name"]       = $rs->fields[17];
            $oneRow["cpu0_vid_value"]      = $rs->fields[18];
            $oneRow["cpu1_vid_name"]       = $rs->fields[19];  
            $oneRow["cpu1_vid_value"]      = $rs->fields[20];
            $oneRow["fan1_rpm_name"]       = $rs->fields[21];
            $oneRow["fan1_rpm_value"]      = $rs->fields[22];
            $oneRow["fan2_rpm_name"]       = $rs->fields[23];
            $oneRow["fan2_rpm_value"]      = $rs->fields[24];  
            $oneRow["fan3_rpm_name"]       = $rs->fields[25];
            $oneRow["fan3_rpm_value"]      = $rs->fields[26];
            $oneRow["fan4_rpm_name"]       = $rs->fields[27];
            $oneRow["fan4_rpm_value"]      = $rs->fields[28];
            $oneRow["fan5_rpm_name"]       = $rs->fields[29];
            $oneRow["fan5_rpm_value"]      = $rs->fields[30];
            $oneRow["fan6_rpm_name"]       = $rs->fields[31];
            $oneRow["fan6_rpm_value"]      = $rs->fields[32];
            $oneRow["fan7_rpm_name"]       = $rs->fields[33];
			$oneRow["fan7_rpm_value"]      = $rs->fields[34];
			$oneRow["fan8_rpm_name"]       = $rs->fields[35];
			$oneRow["fan8_rpm_value"]      = $rs->fields[36];
			$oneRow["in0_name"]            = $rs->fields[37];
			$oneRow["in0_value"]           = $rs->fields[38];
			$oneRow["in1_name"]            = $rs->fields[39];
			$oneRow["in1_value"]           = $rs->fields[40];
			$oneRow["in2_name"]            = $rs->fields[41];
			$oneRow["in2_value"]           = $rs->fields[42];
			$oneRow["in3_name"]            = $rs->fields[43];
			$oneRow["in3_value"]           = $rs->fields[44];
			$oneRow["in4_name"]            = $rs->fields[45];
			$oneRow["in4_value"]           = $rs->fields[46];
			$oneRow["in5_name"]            = $rs->fields[47];
			$oneRow["in5_value"]           = $rs->fields[48];
			$oneRow["in6_name"]            = $rs->fields[49];
			$oneRow["in6_value"]           = $rs->fields[50];
			$oneRow["in7_name"]            = $rs->fields[51];
			$oneRow["in7_value"]           = $rs->fields[52];
			$oneRow["in8_name"]            = $rs->fields[53];
			$oneRow["in8_value"]           = $rs->fields[54];
			$oneRow["in9_name"]            = $rs->fields[55];
			$oneRow["in9_value"]           = $rs->fields[56];
			$oneRow["in10_name"]           = $rs->fields[57];
			$oneRow["in10_value"]          = $rs->fields[58];
			$oneRow["in11_name"]           = $rs->fields[59];
			$oneRow["in11_value"]          = $rs->fields[60];
			$oneRow["in12_name"]           = $rs->fields[61];
			$oneRow["in12_value"]          = $rs->fields[62];
			$oneRow["in13_name"]           = $rs->fields[63];
			$oneRow["in13_value"]          = $rs->fields[64];
			$oneRow["in14_name"]           = $rs->fields[65];
			$oneRow["in14_value"]          = $rs->fields[66];
            $rawData[] = $oneRow;

            $rs->MoveNext();
        }
        $hwmonData["rawData"] = $rawData;
        $rs->Close();
        print json_encode($hwmonData);
    }
    else{
        ProcExecInfo($sEcho, $query);
    }
?>