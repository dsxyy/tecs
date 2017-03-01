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
        if(isset($_GET['project_name'])){
            $project_name =  $_GET['project_name'];
        }
        else{
            $project_name = "";
        }
    ?>
</head>

<body>
    <input type="hidden" id="txUsername" value="<?php echo $user_name?>"> 
    <input type="hidden" id="txProject_name" value="<?php echo $project_name?>">
    <div style="display:none;">
        <ul class="breadcrumb">
            <li>
                <a href="#" data-value="vr_overview.html"><OamStaticUI bindattr="text" maptype="display" bindvalue="vr_app"/></a> <span class="divider">/</span>
            </li>
            <li class="active">
                <a href="#" data-value="vr_vm.php"><OamStaticUI bindattr="text" maptype="display" bindvalue="vm_manager"/></a>
            </li>
        </ul>
    </div>
    <div class="box-content">
        <ul class="nav nav-tabs" id="tabsVrvm">
            <li class="active"><a href="#tabVrvm"><OamStaticUI bindattr="text" maptype="display" bindvalue="vm_list"/></a></li>
        </ul> 
        <div class="tab-content">
            <div class="tab-pane active" id="tabVrvm">
                <div class="row-fluid" style="table-layout:fixed;word-break:break-all">
                    <div class="span3">
                        <select data-placeholder="" id="selectUser" multiple data-rel="chosen">
                        </select>
                    </div>
                    <div class="span3">
                        <select data-placeholder="" id="selectProject" multiple data-rel="chosen">
                        </select>
                    </div>                    
                    <div class="span6" align="right">
                        <button class="btn" id="cancel_deploy"><i class="icon-step-backward"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="cancel_deploy"/></span></button>
                        <button class="btn" id="remove"><i class="icon-trash"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="remove"/></span></button>
                        <button class="btn" id="refresh"><i class="icon-refresh"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="refresh"/></span></button>
                    </div>
                </div>
                <div>
                    <table class="table table-striped table-bordered table-condensed bootstrap-datatable datatable" id="tbVm">
                        <thead> 
                              <tr>
                                 <th align="left" width="25px;"><INPUT id="SelecltAll" type="checkbox" value="" /></th>
                                 <th align="left" width="50px;">ID</th> 
                                 <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="name"/></th>
                                 <th align="left" width="50px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="status"/></th> 
                                 <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="vcpu"/></th>
                                 <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="tcu"/></th>
                                 <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="memory_size"/></th>
                                 <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="disk_size"/></th>
                                 <th align="left" width="100px;">IP</th>
                                 <th align="left" width="50px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="user"/></th>
                                 <th align="left" width="50px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="project"/></th>
                                 <th align="left" width="50px;"></th>
                              </tr>
                        </thead>
                        <tbody style="word-wrap:break-word;word-break:break-all;"></tbody>
                    </table>
                </div> 
            </div>
        </div>
    </div>
    
    
    <!-- external javascript
    ================================================== -->
    <!-- Placed at the end of the document so the pages load faster -->
    <script src="js/tecs/vm_summary.js"></script>
    <script src="js/tecs/vm_cpu.js"></script>
    <script src="js/tecs/vm_memory.js"></script>
    <script src="js/tecs/vm_image.js"></script>
    <script src="js/tecs/vm_disk.js"></script>
    <script src="js/tecs/vm_context.js"></script>
    <script src="js/tecs/vm_nic.js"></script>
    <script src="js/tecs/vm_watch_dog.js"></script>
    <script src="js/tecs/vm_pdh.js"></script>
    <script src="js/tecs/vm_remote_desktop.js"></script>
    <script src="js/tecs/vm_mutex.js"></script>
    <script src="js/tecs/vm_info.js"></script>
    <script src="js/tecs/vr_vm.js"></script>
</body>
</html>
