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
                <a href="#" data-value="vr_vt.php"><OamStaticUI bindattr="text" maptype="display" bindvalue="vt_manager"/></a>
            </li>
        </ul>
    </div>
    <div class="box-content">
        <ul class="nav nav-tabs" id="tabsVrvt">
            <li class="active"><a href="#tabVrvt"><OamStaticUI bindattr="text" maptype="display" bindvalue="vt_list"/></a></li>
        </ul> 
        <div class="tab-content">
            <div class="tab-pane active" id="tabVrvt">
                <div class="row-fluid" style="table-layout:fixed;word-break:break-all">
                    <div class="span4">
                        <select data-placeholder=""  id="selectUser" multiple data-rel="chosen">
                        </select>
                    </div>                    
                    <div class="span8" align="right">
                        <button class="btn" id="create"><i class="icon-plus"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="create"/></span></button>
                        <button class="btn" id="remove"><i class="icon-trash"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="remove"/></span></button>
                        <button class="btn" id="public"><i class="icon-share"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="publish"/></span></button>
                        <button class="btn" id="private"><i class="icon-remove-circle"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="unpublish"/></span></button>
                        <button class="btn" id="refresh"><i class="icon-refresh"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="refresh"/></span></button>
                    </div>
                </div>
                
                <div>
                    <table class="table table-striped table-bordered table-condensed bootstrap-datatable datatable" id="tbVt" style="width:100%;word-break:break-all">
                        <thead> 
                              <tr>
                                 <th align="left" width="5%"><INPUT id="SelecltAll" type="checkbox" value="" /></th>
                                 <th align="left" width="20%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="name"/></th> 
                                 <th align="left" width="12%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="vcpu"/></th>
                                 <th align="left" width="13%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="tcu"/></th>
                                 <th align="left" width="10%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="memory_size"/></th>
                                 <th align="left" width="15%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="disk_size"/></th>
                                 <th align="left" width="10%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="ispublish"/></th>
                                 <th align="left" width="10%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="user"/></th>
                                 <th align="left" width="5%"></th>
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
    <script src="js/tecs/vr_vt.js"></script>
</body>
</html>
