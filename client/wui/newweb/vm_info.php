<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <title>TECS</title>
    
 <?php
        $action       =  $_GET['action'];
        $project_name =  $_GET['project_name'];
        $vid          =  $_GET['vid'];
        $tab_flag     =  $_GET['tab_flag'];
	?> 
</head>

<body>
    <input type="hidden" id="txAction" value="<?php echo $action?>">
    <input type="hidden" id="txProjectName" value="<?php echo $project_name?>">
    <input type="hidden" id="txVid" value="<?php echo $vid?>">
    <input type="hidden" id="txTabFlag" value="<?php echo $tab_flag?>">
    <div>
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
            <a class="btn" id="preview"><i class="icon-zoom-in"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="preview"/></span></a>
            <a class="btn" id="submit_data"><i class="icon-check"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="submit"/></span></a>
        </p>
    </div>
    <div class="row-fluid">
        <div class="box span3">
            <div class="box-header well">
                <h3><OamStaticUI bindattr="text" maptype="display" bindvalue="device_area"/></h3>
            </div>
            <div class="box-content">
                <div class="accordion" id="accordion2">
                    <div class="accordion-group">
                        <div class="accordion-heading">
                            <a class="accordion-toggle" data-toggle="collapse" data-parent="#accordion2" href="#collapseImage">
                                <OamStaticUI bindattr="text" maptype="field_name" bindvalue="image"/>
                            </a>
                        </div>
                        <div id="collapseImage" class="accordion-body collapse in">
                            <div class="accordion-inner" style="height:400px;overflow:auto;">
                            </div>
                        </div>
                    </div>
                    <div class="accordion-group">
                        <div class="accordion-heading">
                            <a class="accordion-toggle" data-toggle="collapse" data-parent="#accordion2" href="#collapseSotrageDevice">
                                <OamStaticUI bindattr="text" maptype="display" bindvalue="sotrage_device"/>
                            </a>
                        </div>
                        <div id="collapseSotrageDevice" class="accordion-body collapse">
                            <div class="accordion-inner" style="height:400px;overflow:auto;">
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
                            <a class="accordion-toggle" data-toggle="collapse" data-parent="#accordion2" href="#collapseNic">
                                <OamStaticUI bindattr="text" maptype="display" bindvalue="nic"/>
                            </a>
                        </div>
                        <div id="collapseNic" class="accordion-body collapse">
                            <div class="accordion-inner">
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
                            <a class="accordion-toggle" data-toggle="collapse" data-parent="#accordion2" href="#collapsePCI">
                                <OamStaticUI bindattr="text" maptype="display" bindvalue="pci_device"/>
                            </a>
                        </div>
                        <div id="collapsePCI" class="accordion-body collapse">
                            <div class="accordion-inner">
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
            <div class="box-content" id="para_content">
            </div>
        </div> 
    </div>    

    <div class="modal hide" id="idx_confirmDlg">
        <div class="modal-header">
            <h5><OamStaticUI bindattr="text" maptype="display" bindvalue="confirm_op"/></h5>
        </div>
        <div class="modal-body">
            <div class="confirmStatus"></div>
        </div>
        <div class="modal-footer">
            <button id="idx_confirmDlg_confirm" class="btn btn-primary"><OamStaticUI bindattr="text" maptype="opr" bindvalue="confirm"/></button>
            <button class="btn" data-dismiss="modal" aria-hidden="true"><OamStaticUI bindattr="text" maptype="opr" bindvalue="cancel"/></button>
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
    <script src="js/tecs/vm_info.js"></script>
</body>
</html>
