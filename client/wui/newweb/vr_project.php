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
                <a href="#" data-value="vr_project.php"><OamStaticUI bindattr="text" maptype="display" bindvalue="project_manager"/></a>
            </li>
        </ul>
    </div>
    <div class="box-content">
        <ul class="nav nav-tabs" id="tabsProject">
            <li class="active"><a href="#tabProject"><OamStaticUI bindattr="text" maptype="display" bindvalue="project_list"/></a></li>
        </ul> 
        <div class="tab-content">
            <div class="tab-pane active" id="tabProject">
                <div class="row-fluid" style="table-layout:fixed;word-break:break-all">
                    <div class="span4">
                        <select data-placeholder=""  id="selectUser" multiple data-rel="chosen">
                        </select>
                    </div>                    
                    <div class="span8" align="right">
                        <button class="btn" id="remove"><i class="icon-trash"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="remove"/></span></button>
                        <button class="btn" id="refresh"><i class="icon-refresh"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="refresh"/></span></button>
                    </div>
                </div>
                <div>
                    <table class="table table-striped table-bordered table-condensed bootstrap-datatable datatable" id="tbVrProject" style="width:100%;word-break:break-all">
                        <thead> 
                              <tr> 
                                 <th align="left" width="5%"><INPUT id="SelecltAll" type="checkbox" value="" /></th>
                                 <th align="left" width="20%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="name"/></th> 
                                 <th align="left" width="35%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="desc"/></th>
                                 <th align="left" width="20%"><OamStaticUI bindattr="text" maptype="display" bindvalue="vm_cnt"/></th> 
                                 <th align="left" width="20%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="user"/></th>
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
    <script src="js/tecs/vr_project.js"></script>
    <script src="js/tecs/vm_common.js"></script>
</body>
</html>
