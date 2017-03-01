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
                <a href="#"  data-value="vr.php"><OamStaticUI bindattr="text" maptype="display" bindvalue="vr_app"/></a> <span class="divider">/</span>
            </li>
            <li class="active">
                <a href="#" data-value="vr_portabledisk.php"><OamStaticUI bindattr="text" maptype="display" bindvalue="pdisk_manager"/></a>
            </li>
        </ul>
    </div>
    <div class="box-content">
        <ul class="nav nav-tabs" id="tabsDisk">
            <li class="active"><a href="#removeable_disk"><OamStaticUI bindattr="text" maptype="display" bindvalue="pdisk_manager"/></a></li>
        </ul> 
        <div id="diskTabContent" class="tab-content">
            <div class="tab-pane active" id="removeable_disk">
                <div class="row-fluid" style="table-layout:fixed;word-break:break-all">
                    <div class="span4">
                        <select data-placeholder="" id="selectUser" multiple data-rel="chosen">
                        </select>
                    </div>                    
                    <div class="span8" align="right">
                        <div class="btn-toolbar" style="margin: 0;">                            
                            <div class="btn-group">
                                <button class="btn" id="pdisk_refresh"><i class="icon-refresh"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="refresh"/></span></button>
                            </div>
                        </div>
                    </div>
                </div>
               				
                <div>
                    <table class="table table-striped table-bordered table-condensed bootstrap-datatable datatable" id="tbPotableDisk" style="width:100%;word-break:break-all">
                        <thead> 
                              <tr> 
                                 <th align="left" width="15%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="name"/></th> 
                                 <th align="left" width="15%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="size"/></th>
                                 <th align="left" width="20%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="desc"/></th>								 
                                 <th align="left" width="15%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="vm"/></th> 
                                 <th align="left" width="10%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="drive"/></th> 
                                 <th align="left" width="15%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="block"/></th>
                                 <th align="left" width="10%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="user"/></th>
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
    <script src="js/tecs/vm_portabledisk.js"></script>
</body>
</html>
