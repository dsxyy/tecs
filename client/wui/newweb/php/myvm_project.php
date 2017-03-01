<?php
    include('adodb_connect.php');
     
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
    $filed = array("name","description","vmcount","username");
    $orderfield = "";
    $orderdir = "";
    $order = "";
    
    $conn = GetTCDbConnect($errcode,$errinfo);
    
    if(!$conn){
        die(json_encode(GetQueryAckData($errcode,$errinfo,$sEcho,0)));
    }
    $filter = " 1 = 1"; 
    
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
    //表记录数
    $query = "SELECT COUNT(*) FROM web_view_project WHERE " . $filter;
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
    $query = ' select p.name,p.description,vmcount,username, ';
    $query = $query . "(select workflow_id from webview_workflows w where category='project' and w.label_string_2=p.name and w.originator=p.username limit 1) as workflow ";
    $query = $query . ' from web_view_project p';
    $query = $query . ' WHERE' . $filter;
    $query = $query . ' ORDER by' . $order;
    $rs = $conn->Execute($query);
    if (!$rs){
        $conn->Close();
        die(json_encode(GetQueryAckData("failed_querydb",$query,$sEcho,0)));
    }
     
    $ackdata = GetQueryAckData("","",$sEcho,$iTotalRecords);
    while (!$rs->EOF) {
        $arrProjData = Array();
        $arrProjData[] = "";
        $arrProjData[] = $rs->fields[0];
    
        $description = $rs->fields[1];
        $description = str_replace("\n","<br/>",htmlspecialchars($description));
        $arrProjData[] = $description;
         
        $arrProjData[] = $rs->fields[2];
          
        $arrProjData[] = $rs->fields[3];
        
        $arrProjData["workflow"] = $rs->fields[4];
        $arrProjData["project"]  = $rs->fields[0];

        $arrProjData[] = "";
        $ackdata["data"]["aaData"][] = $arrProjData;
        
        $rs->MoveNext();
    }
    $rs->Close();
    $conn->Close();
    print json_encode($ackdata);    
?>
