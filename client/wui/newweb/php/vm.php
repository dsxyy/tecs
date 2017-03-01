<?php
    include("xmlrpc.inc");
    $sEcho = $_GET['sEcho'];
    $iDisplayStart = $_GET['iDisplayStart'];
    $iDisplayLength = $_GET['iDisplayLength'];
    
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Pragma: no-cache");
    
    $session      = php_xmlrpc_encode($_GET["session"]);
    $start_index  = php_xmlrpc_encode(new xmlrpcval($iDisplayStart,"i8"));
    $query_count  = php_xmlrpc_encode(new xmlrpcval($iDisplayLength,"i8"));
    $flag         = php_xmlrpc_encode(new xmlrpcval($_GET["flag"],"i8"));
    $user_name    = php_xmlrpc_encode($_GET["user_name"]);
    $project_name = php_xmlrpc_encode($_GET["project_name"]);
    $vm_state     = php_xmlrpc_encode(new xmlrpcval($_GET["vm_state"],"int"));
    
    $ps = array($session,$start_index,$query_count,$flag,$user_name,$project_name,$vm_state);
    
    $f=new xmlrpcmsg('tecs.vmcfg.fullquery',$ps);
    $ip = $_GET['ip'];
    $port = $_GET['port'];
    
    $c = new xmlrpc_client('/RPC2', $ip, $port);
    $r=&$c->send($f);
    if(!$r->faultCode())
    {
        $xmldata = $r->serialize();
        //print htmlentities($xmldata);
        $xmlDoc = new DOMDocument("1.0");
        $xmlDoc->loadXML($xmldata);
        $xpath = new domxpath($xmlDoc);
        $strvalue = "/methodResponse/params/param/value/array/data/value";        
        $query = $strvalue . "[1]/i4";//错误码
        $entries = $xpath->query($query);
        if($entries->item(0)->nodeValue != "0")
        {
            //to do
        }
        else
        {
            $vmdatas = '{"sEcho": ' . $sEcho . ',';
            $query = $strvalue . "[3]/i8";//总记录数
            $iTotalRecords = $xpath->query($query)->item(0)->nodeValue;
            $vmdatas = $vmdatas . '"iTotalRecords": ' . '"' . $iTotalRecords . '",';
            $vmdatas = $vmdatas . '"iTotalDisplayRecords": ' . '"' . $iTotalRecords . '",';
            $vmdatas = $vmdatas . '"aaData": [ ';
            $strcurrec = $strvalue . "[4]/array/data/value";//当前记录数
            $iCurRecords = $xpath->query($strcurrec)->length;
            
            for($i = 1;$i <= $iCurRecords;$i++)
            {
                $vmdata = '[';
                $vmdata = $vmdata . '"' . '",';//第一列checkbox
                if($_GET["vm_state"] == "-1")
                {
                    $query = $strcurrec . "[".strval($i)."]/struct/member[name='vid']/value";
                    $entries = $xpath->query($query);
                    $vid = $entries->item(0)->nodeValue;
                    $vmdata = $vmdata . '"' . $vid . '",';
                    
                    $query = $strcurrec . "[".strval($i)."]/struct/member[name='name']/value";
                    $entries = $xpath->query($query);
                    $name = $entries->item(0)->nodeValue;
                    $vmdata = $vmdata . '"' . $name . '",';
                    
                    $vmdata = $vmdata . '"' . '",';//状态不查
                    
                    $query = $strcurrec . "[".strval($i)."]/struct/member[name='vcpu']/value";
                    $entries = $xpath->query($query);
                    $vcpu = $entries->item(0)->nodeValue;
                    $vmdata = $vmdata . '"' . $vcpu . '",';
                
                    $query = $strcurrec . "[".strval($i)."]/struct/member[name='tcu']/value";
                    $entries = $xpath->query($query);
                    $tcu = $entries->item(0)->nodeValue;
                    $vmdata = $vmdata . '"' . $tcu . '",';
                
                    $query = $strcurrec . "[".strval($i)."]/struct/member[name='memory']/value";
                    $entries = $xpath->query($query);
                    $memory = $entries->item(0)->nodeValue;
                    $vmdata = $vmdata . '"' . $memory . '",';
                    
                    $query = $strcurrec . "[".strval($i)."]/struct/member[name='disk']/value";
                    $entries = $xpath->query($query);
                    $disk = $entries->item(0)->nodeValue;
                    $vmdata = $vmdata . '"' . $disk . '",';
                    
                    $query = $strcurrec . "[".strval($i)."]/struct/member[name='user_name']/value";
                    $entries = $xpath->query($query);
                    $user_name = $entries->item(0)->nodeValue;
                    $vmdata = $vmdata . '"' . $user_name . '",';
                    
                    $query = $strcurrec . "[".strval($i)."]/struct/member[name='project_name']/value";
                    $entries = $xpath->query($query);
                    $project_name = $entries->item(0)->nodeValue;
                    $vmdata = $vmdata . '"' . $project_name . '"';
                }
                
                if($_GET["vm_state"] == "0")
                {
                    $query = $strcurrec . "[".strval($i)."]/struct/member[name='vid']/value";
                    $entries = $xpath->query($query);
                    $vid = $entries->item(0)->nodeValue;
                    $vmdata = $vmdata . '"' . $vid . '",';
                    
                    $query = $strcurrec . "[".strval($i)."]/struct/member[name='name']/value";
                    $entries = $xpath->query($query);
                    $name = $entries->item(0)->nodeValue;
                    $vmdata = $vmdata . '"' . $name . '",';
                    
                    $query = $strcurrec . "[".strval($i)."]/struct/member[name='create_time']/value";
                    $entries = $xpath->query($query);
                    $create_time = $entries->item(0)->nodeValue;
                    $vmdata = $vmdata . '"' . $create_time . '",';
                    
                    $query = $strcurrec . "[".strval($i)."]/struct/member[name='vcpu']/value";
                    $entries = $xpath->query($query);
                    $vcpu = $entries->item(0)->nodeValue;
                    $vmdata = $vmdata . '"' . $vcpu . '",';
                
                    $query = $strcurrec . "[".strval($i)."]/struct/member[name='tcu']/value";
                    $entries = $xpath->query($query);
                    $tcu = $entries->item(0)->nodeValue;
                    $vmdata = $vmdata . '"' . $tcu . '",';
                
                    $query = $strcurrec . "[".strval($i)."]/struct/member[name='memory']/value";
                    $entries = $xpath->query($query);
                    $memory = $entries->item(0)->nodeValue;
                    $vmdata = $vmdata . '"' . $memory . '",';
                    
                    $query = $strcurrec . "[".strval($i)."]/struct/member[name='disk']/value";
                    $entries = $xpath->query($query);
                    $disk = $entries->item(0)->nodeValue;
                    $vmdata = $vmdata . '"' . $disk . '"';
                }
                
                if($_GET["vm_state"] == "1")
                {
                    $vmdata = $vmdata . '"' . '",';//状态不查
                    
                    $query = $strcurrec . "[".strval($i)."]/struct/member[name='vid']/value";
                    $entries = $xpath->query($query);
                    $vid = $entries->item(0)->nodeValue;
                    $vmdata = $vmdata . '"' . $vid . '",';
                    
                    $query = $strcurrec . "[".strval($i)."]/struct/member[name='name']/value";
                    $entries = $xpath->query($query);
                    $name = $entries->item(0)->nodeValue;
                    $vmdata = $vmdata . '"' . $name . '",';
                    
                    $query = $strcurrec . "[".strval($i)."]/struct/member[name='create_time']/value";
                    $entries = $xpath->query($query);
                    $create_time = $entries->item(0)->nodeValue;
                    $vmdata = $vmdata . '"' . $create_time . '",';
                    
                    $query = $strcurrec . "[".strval($i)."]/struct/member[name='vcpu']/value";
                    $entries = $xpath->query($query);
                    $vcpu = $entries->item(0)->nodeValue;
                    $vmdata = $vmdata . '"' . $vcpu . '",';
                
                    $query = $strcurrec . "[".strval($i)."]/struct/member[name='tcu']/value";
                    $entries = $xpath->query($query);
                    $tcu = $entries->item(0)->nodeValue;
                    $vmdata = $vmdata . '"' . $tcu . '",';
                
                    $query = $strcurrec . "[".strval($i)."]/struct/member[name='memory']/value";
                    $entries = $xpath->query($query);
                    $memory = $entries->item(0)->nodeValue;
                    $vmdata = $vmdata . '"' . $memory . '",';
                    
                    $query = $strcurrec . "[".strval($i)."]/struct/member[name='disk']/value";
                    $entries = $xpath->query($query);
                    $disk = $entries->item(0)->nodeValue;
                    $vmdata = $vmdata . '"' . $disk . '",';
                    
                    $vmdata = $vmdata . '"' . '"';//状态不查
                }
                
                if($i < $iCurRecords)
                {
                    $vmdata = $vmdata . "],";
                }
                else
                {
                    $vmdata = $vmdata . "]";
                }
                $vmdatas = $vmdatas.$vmdata;
            }
            $vmdatas = $vmdatas . ']}';
            print $vmdatas;
            
        }
        
        //print $xmldata;
    }
    else
    {
	    print "An error occurred: ";
	    print "Code: " . htmlspecialchars($r->faultCode())
	    . " Reason: '" . htmlspecialchars($r->faultString()) . "'</pre><br/>";
    }
    
?>
