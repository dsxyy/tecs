<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="utf-8">
	<title>TECS</title> 
    <?php 
        if(isset($_GET['user_name'])){
            $user_name =  $_GET['user_name'];
        }
        else{
            $user_name = "";
        }
	?>
</head>

<body>
    <input type="hidden" id="txUsername" value="<?php echo $user_name?>">
    <div style="display:none;">
        <ul class="breadcrumb">
            <li>
                <a href="#" data-value="vr_overview.html"><OamStaticUI bindattr="text" maptype="display" bindvalue="vr_app"/></a> <span class="divider">/</span>
            </li>
            <li class="active">
                <a href="#" data-value="vr_user_resource.php"><OamStaticUI bindattr="text" maptype="display" bindvalue="user_resource"/></a>
            </li>
        </ul>
    </div>
    <div class="box-content">
        <ul class="nav nav-tabs" id="tabsUserresource">
            <li class="active"><a href="#tabUserresource"><OamStaticUI bindattr="text" maptype="display" bindvalue="user_resource_list"/></a></li>
        </ul> 
        <div class="tab-content">
            <div class="tab-pane active" id="tabUserresource">
                <div align="right">
                    <button class="btn" id="refresh"><i class="icon-refresh"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="refresh"/></span></button>
                </div> 
                <div>
                    <table class="table table-striped table-bordered table-condensed bootstrap-datatable datatable" id="tbUserResource" style="width:100%;word-break:break-all">
                        <thead> 
                            <tr> 
                                <th align="left"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="user"/></th> 
                                <th align="left"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="project"/></th>
                                <th align="left"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="vm"/></th> 
                                <th align="left"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="pdisk"/></th>                                 
                                <th align="left"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="vt"/></th>
                                <th align="left"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="image"/></th> 
                            </tr>
                        </thead>
                    </table>
                </div>
            </div>
        </div>
    </div>
	<!-- external javascript
	================================================== -->
	<!-- Placed at the end of the document so the pages load faster -->
    <script src="js/tecs/vr_user_resource.js"></script>
    <script src="js/tecs/vm_common.js"></script>
</body>
</html>
