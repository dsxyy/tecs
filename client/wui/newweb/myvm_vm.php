<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <title>TECS</title>
    
    <?php
        if(isset($_GET['project_name'])){
            $project_name =  $_GET['project_name'];
        }
        else{
            $project_name = "";
        }
        if(isset($_GET['tabindex'])){
            $tabindex =  $_GET['tabindex'];
        }
        else{
            $tabindex =  "";
        }
    ?>
</head>

<body>
    <input type="hidden" id="txProject_name" value="<?php echo $project_name?>">
    <input type="hidden" id="txTabIndex" value="<?php echo $tabindex?>">
    <div style="display:none;">
        <ul class="breadcrumb">
            <li>
                <a href="#"  data-value="myvm_overview.html"><OamStaticUI bindattr="text" maptype="display" bindvalue="my_vm"/></a> <span class="divider">/</span>
            </li>
            <li class="active">
                <a href="#" data-value="myvm_vm.php"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="vm"/></a>
            </li>
        </ul>
    </div>
    <div class="box-content">
        <ul class="nav nav-tabs" id="vmTab">
            <li class="active"><a href="#deployed_vm"><OamStaticUI bindattr="text" maptype="display" bindvalue="deployed"/></a></li>
            <li><a href="#fiction_vm"><OamStaticUI bindattr="text" maptype="display" bindvalue="fiction"/></a></li>
        </ul> 
        <div id="vmTabContent" class="tab-content">
            <div class="tab-pane active" id="deployed_vm">
                <div class="row-fluid">
                    <div class="span12" align="right">
                        <div class="btn-toolbar" style="margin: 0;">
                            <div class="btn-group">
                                <button class="btn" id="deployedvm_create"><i class="icon-plus"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="create"/></span></button>
                                <button class="btn dropdown-toggle" data-toggle="dropdown"><span class="caret"></span></button>
                                <ul class="dropdown-menu pull-right">
                                    <li><button class="btn uservt span12" data-value="myvm_vt.html"><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="use_template"/></span></button></li>
                                </ul>
                            </div>
                            <div class="btn-group">
                                <button class="btn" id="deployedvm_canceldeploy"><i class="icon-step-backward"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="cancel_deploy"/></span></button>
                            </div>
                            <div class="btn-group">
                                <button class="btn" id="deployedvm_poweron"><i class="icon-play"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="poweron"/></span></button>
                            </div>
                            <div class="btn-group">
                                <button class="btn" id="deployedvm_poweroff"><i class="icon-off"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="poweroff"/></span></button>
                                <button class="btn dropdown-toggle" data-toggle="dropdown"><span class="caret"></span></button>
                                <ul class="dropdown-menu pull-right">
                                    <li><button class="btn span12" id="deployedvm_forcepoweroff"><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="force_poweroff"/></span></button></li>
                                </ul>
                            </div>
                            <div class="btn-group">
                                <button class="btn" id="deployedvm_pause"><i class="icon-pause"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="pause"/></span></button>
                            </div>
                            <div class="btn-group">
                                <button class="btn" id="deployedvm_resume"><i class="icon-check"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="resume"/></span></button>
                            </div>

                            <div class="btn-group">
                                <button class="btn" id="deployedvm_reboot"><i class="icon-repeat"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="reboot_vm"/></span></button>
                                <button class="btn dropdown-toggle" data-toggle="dropdown"><span class="caret"></span></button>
                                <ul class="dropdown-menu pull-right">
                                    <li><button class="btn span12" id="deployedvm_forcereboot"><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="force_reboot"/></span></button></li>
                                </ul>
                            </div>
                            <div class="btn-group">
                                <button class="btn" id="deployedvm_saveasimage"><i class="icon icon-darkgray icon-save"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="saveasimage"/></span></button>
                            </div>
                            <div class="btn-group">
                                <button class="btn" id="deployedvm_recoverimage"><i class="icon-retweet"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="recoverimage"/></span></button>
                            </div>                             
                            <div class="btn-group">
                                <button class="btn" id="deployedvm_freeze"><i class="icon-ban-circle"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="freeze"/></span></button>
                            </div>
                            <div class="btn-group">
                                <button class="btn" id="deployedvm_unfreeze"><i class="icon-ok-circle"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="unfreeze"/></span></button>
                            </div>                                                    
                            <div class="btn-group">
                                <button class="btn" id="deployedvm_refresh"><i class="icon-refresh"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="refresh"/></span></button>
                            </div>
                        </div>
                    </div>
                </div>
                <div class="row-fluid">
                    <div class="span12">
                        <div class="span4" style="table-layout:fixed;word-break:break-all">
                            <select data-placeholder="" id="selectDeployProject" multiple data-rel="chosen">
                            </select>
                        </div>    
                    </div>
                </div>
                
                <div>
                    <table class="table table-striped table-bordered table-condensed" id="tbDeployVm">
                        <thead> 
                            <tr>
                                <th align="left" width="25px;"><INPUT id="SelecltDeployedVmAll" type="checkbox" value="" /></th>
                                <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="status"/></th>
                                <th align="left" width="50px;">ID</th>
                                <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="name"/></th>
                                <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="vcpu"/></th>
                                <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="tcu"/></th>
                                <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="memory_size"/></th>
                                <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="disk_size"/></th>
                                <th align="left" width="100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="ip"/></th>
                                <th align="left" width="50px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="project"/></th>
                                <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="blank"/></th>
                            </tr>
                        </thead>
                        <tbody style="word-wrap:break-word;word-break:break-all;"></tbody>
                    </table>
                </div>
            </div>
            <div class="tab-pane" id="fiction_vm">
                <div class="row-fluid" style="table-layout:fixed;word-break:break-all">
                    <div class="span4">
                        <select data-placeholder=""  id="selectFictionProject" multiple data-rel="chosen">
                        </select>
                    </div>                    
                    <div class="span8" align="right">
                        <div class="btn-toolbar" style="margin: 0;">
                            <div class="btn-group">
                                <button class="btn" href="javascript:void(0);" id="fictionvm_create"><i class="icon-plus"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="create"/></span></button>
                                <button class="btn dropdown-toggle" data-toggle="dropdown"><span class="caret"></span></button>
                                <ul class="dropdown-menu pull-right">
                                    <li><button class="btn span12 uservt" data-value="myvm_vt.html"><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="use_template"/></span></button></li>
                                </ul>
                            </div>
                            <div class="btn-group">
                                <button class="btn" href="javascript:void(0);" id="fictionvm_deploy"><i class="icon-step-forward"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="deploy"/></span></button>
                            </div>
                            
                            <div class="btn-group">
                                <button class="btn" href="javascript:void(0);" id="fictionvm_remove"><i class="icon-trash"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="remove"/></span></button>
                            </div>
                            <div class="btn-group">
                                <button class="btn" href="javascript:void(0);" id="fictionvm_refresh"><i class="icon-refresh"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="refresh"/></span></button>
                            </div>
                        </div>
                    </div>
                </div>
                <div>
                    <table class="table table-striped table-bordered table-condensed" id="tbFictonVm">
                        <thead> 
                            <tr>
                                <th align="left" width="25px;"><INPUT id="SelecltFictionVmAll" type="checkbox" value="" /></th>
                                <th align="left" width="50px;">ID</th>
                                <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="name"/></th>
                                <th align="left" width="100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="create_time"/></th>
                                <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="vcpu"/></th>
                                <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="tcu"/></th>
                                <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="memory_size"/></th>
                                <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="disk_size"/></th>
                                <th align="left" width="50px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="project"/></th>
                                <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="blank"/></th>
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
    <script src="js/tecs/myvm_backup_common.js"></script>	
    <script src="js/tecs/myvm_vm.js"></script>
</body>
</html>
