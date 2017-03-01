<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <title>TECS</title>
    
 <?php
        if(isset($_GET['action'])){
            $action =  $_GET['action'];
        }
        else{
            $action = "";
        }
        if(isset($_GET['project_name'])){
            $project_name =  $_GET['project_name'];
        }
        else{
            $project_name = "";
        }
        if(isset($_GET['vid'])){
            $vid =  $_GET['vid'];
        }
        else{
            $vid = "";
        }
        if(isset($_GET['tabindex'])){
            $tabindex =  $_GET['tabindex'];
        }
        else{
            $tabindex = "";
        }
        if(isset($_GET['level'])){
            $level =  $_GET['level'];
        }
        else{
            $level = "";
        }
	?> 
</head>

<body>
    <input type="hidden" id="txAction" value="<?php echo $action?>">
    <input type="hidden" id="txProjectName" value="<?php echo $project_name?>">
    <input type="hidden" id="txVid" value="<?php echo $vid?>">
    <input type="hidden" id="txTabIndex" value="<?php echo $tabindex?>">
    <input type="hidden" id="txLevel" value="<?php echo $level?>">
    <div style="display:none;">
        <ul class="breadcrumb">
            <li>
                <a href="#"></a> <span class="divider">/</span>
            </li>
            <li>
                <a href="#"></a> <span class="divider">/</span>
            </li>
            <li class="active">
                <a href="#"></a>
            </li>
        </ul>
    </div>
    <div>
        <p align="right">
            <button class="btn" id="cancel"><i class="icon-remove-sign"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="cancel"/></span></button>
            <button class="btn" id="preview"><i class="icon-zoom-in"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="preview"/></span></button>
            <button class="btn btn-primary" id="submit_data"><i class="icon-check"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="submit"/></span></button>
        </p>
    </div>
    <div class="row-fluid">
        <div class="box span3">
            <div class="box-header well">
                <h3><OamStaticUI bindattr="text" maptype="display" bindvalue="device_area"/></h3>
            </div>
            <div class="box-content">
                <div class="accordion" id="accordionDevice">
                    <div class="accordion-group">
                        <div class="accordion-heading">
                            <a class="accordion-toggle" data-toggle="collapse" data-parent="#accordionDevice" href="#collapseImage">
                                <span><OamStaticUI bindattr="text" maptype="field_name" bindvalue="image"/></span>
                                <span class="pull-right" data-rel="tooltip" id="image_tooltips"><i class="icon icon-color icon-info"></i></span>
                            </a>
                        </div>
                        <div id="collapseImage" class="accordion-body collapse in">
                            <div class="accordion-inner" style="height:300px;overflow:auto;">
                            </div>
                        </div>
                    </div>
                    <div class="accordion-group">
                        <div class="accordion-heading">
                            <a class="accordion-toggle" data-toggle="collapse" data-parent="#accordionDevice" href="#collapseSotrageDevice">
                                <OamStaticUI bindattr="text" maptype="display" bindvalue="sotrage_device"/>
                            </a>
                        </div>
                        <div id="collapseSotrageDevice" class="accordion-body collapse">
                            <div class="accordion-inner" style="height:300px;overflow:auto;">
                                <div class="box span12 box_ide">
                                    <div class="box-header well">
	                                    <h2>IDE</h2>
	                                </div>
                                    <div class="box-content center" style="overflow:auto;">
                                         <img  src="./img/config_harddisk.png">
                                    </div>
                                </div>
                                <div class="box span12 box_scsi">
                                    <div class="box-header well">
	                                    <h2>SCSI</h2>
	                                </div>
                                    <div class="box-content center" style="overflow:auto;">
                                         <img  src="./img/config_harddisk.png">
                                    </div>
                                </div>
                                <div class="box span12 box_cdrom">
                                    <div class="box-header well">
	                                    <h2>CD-ROM</h2>
	                                </div>
                                    <div class="box-content center" style="overflow:auto;">
                                         <img  src="./img/config_cd_drive.png">
                                    </div>
                                </div>
                                <div class="box span12 box_noramlcontext">
                                    <div class="box-header well">
	                                    <h2><OamStaticUI bindattr="text" maptype="display" bindvalue="normal_context"/></h2>
	                                </div>
                                    <div class="box-content center" style="overflow:auto;">
                                         <img  src="./img/config_context.png">
                                    </div>
                                </div> 
                                <div class="box span12 box_v4context">
                                    <div class="box-header well">
	                                    <h2><OamStaticUI bindattr="text" maptype="display" bindvalue="v4_context"/></h2>
	                                </div>
                                    <div class="box-content center" style="overflow:auto;">
                                         <img  src="./img/config_context.png">
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                    <div class="accordion-group">
                        <div class="accordion-heading">
                            <a class="accordion-toggle" data-toggle="collapse" data-parent="#accordionDevice" href="#collapseNic">
                                <OamStaticUI bindattr="text" maptype="display" bindvalue="nic"/>
                            </a>
                        </div>
                        <div id="collapseNic" class="accordion-body collapse">
                            <div class="accordion-inner" style="height:300px;overflow:auto;">
                                <div class="box span12 box_normalnic">
                                    <div class="box-header well">
	                                    <h2><OamStaticUI bindattr="text" maptype="display" bindvalue="noraml_nic"/></h2>
	                                </div>
                                    <div class="box-content center" style="overflow:auto;">
                                         <img  src="./img/config_nic.png">
                                    </div>
                                </div>
                                <div class="box span12 box_sriovnic">
                                    <div class="box-header well">
	                                    <h2><OamStaticUI bindattr="text" maptype="display" bindvalue="sriov_nic"/></h2>
	                                </div>
                                    <div class="box-content center" style="overflow:auto;">
                                         <img  src="./img/config_nic.png">
                                    </div>
                                </div>
                                <div class="box span12 box_watchdog">
                                    <div class="box-header well">
	                                    <h2><OamStaticUI bindattr="text" maptype="display" bindvalue="watch_dog"/></h2>
	                                </div>
                                    <div class="box-content center" style="overflow:auto;">
                                         <img  src="./img/config_watch_dog.png">
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                    <div class="accordion-group">
                        <div class="accordion-heading">
                            <a class="accordion-toggle" data-toggle="collapse" data-parent="#accordionDevice" href="#collapsePCI">
                                <OamStaticUI bindattr="text" maptype="display" bindvalue="pci_device"/>
                            </a>
                        </div>
                        <div id="collapsePCI" class="accordion-body collapse">
                            <div class="accordion-inner" style="height:300px;overflow:auto;">
                                <div class="box span12 box_pdh">
                                    <div class="box-header well">
	                                    <h2>PDH</h2>
	                                </div>
                                    <div class="box-content center" style="overflow:auto;">
                                         <img  src="./img/config_pdh.png">
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div> 
        <div class="box span9">
            <div class="box-header well">
                <h3><OamStaticUI bindattr="text" maptype="display" bindvalue="param_area"/></h3>
            </div>
            <div class="box-content">
                <div class="accordion" id="accordionPara">
                    <!-- base/cpu/memory/vnc -->
                    <div class="accordion-group">
                        <div class="accordion-heading">
                            <a class="accordion-toggle" data-toggle="collapse" data-parent="#accordionPara" href="#collapseParaBase"></a>
                        </div>
                        <div id="collapseParaBase" class="accordion-body collapse in">
                            <div class="accordion-inner collapse-para">
                            </div>
                        </div>
                    </div>
                     
                    <!-- image/ide/cdrom/context -->
                    <div class="accordion-group">
                        <div class="accordion-heading">
                            <a class="accordion-toggle" data-toggle="collapse" data-parent="#accordionPara" href="#collapseParaIDE"></a>
                        </div>
                        <div id="collapseParaIDE" class="accordion-body collapse">
                            <div class="accordion-inner  collapse-para">
                            </div>
                        </div>
                    </div> 
                    
                    <!-- SCSI -->
                    <div class="accordion-group">
                        <div class="accordion-heading">
                            <a class="accordion-toggle" data-toggle="collapse" data-parent="#accordionPara" href="#collapseParaSCSI"></a>
                        </div>
                        <div id="collapseParaSCSI" class="accordion-body collapse">
                            <div class="accordion-inner collapse-para">
                            </div>
                        </div>
                    </div>  
                    
                    <!-- Normal NIC/Watch dog -->
                    <div class="accordion-group">
                        <div class="accordion-heading">
                            <a class="accordion-toggle" data-toggle="collapse" data-parent="#accordionPara" href="#collapseParaNormalNic"></a>
                        </div>
                        <div id="collapseParaNormalNic" class="accordion-body collapse">
                            <div class="accordion-inner collapse-para">
                            </div>
                        </div>
                    </div> 
                    
                    <!-- SRIOV NIC -->
                    <div class="accordion-group">
                        <div class="accordion-heading">
                            <a class="accordion-toggle" data-toggle="collapse" data-parent="#accordionPara" href="#collapseParaSRIOVNic"></a>
                        </div>
                        <div id="collapseParaSRIOVNic" class="accordion-body collapse">
                            <div class="accordion-inner collapse-para">
                            </div>
                        </div>
                    </div>
                     
                    <!-- PCI -->
                    <div class="accordion-group">
                        <div class="accordion-heading">
                            <a class="accordion-toggle" data-toggle="collapse" data-parent="#accordionPara" href="#collapseParaPCI"></a>
                        </div>
                        <div id="collapseParaPCI" class="accordion-body collapse">
                            <div class="accordion-inner collapse-para">
                            </div>
                        </div>
                    </div>
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
    <script src="js/tecs/vm_remote_desktop.js"></script>
    <script src="js/tecs/vm_image.js"></script>
    <script src="js/tecs/vm_disk.js"></script>
    <script src="js/tecs/vm_context.js"></script>
    <script src="js/tecs/vm_nic.js"></script>
    <script src="js/tecs/vm_watch_dog.js"></script>
    <script src="js/tecs/vm_pdh.js"></script>
    <script src="js/tecs/vm_info.js"></script>
    <script src="js/tecs/vm_config.js"></script>
</body>
</html>
