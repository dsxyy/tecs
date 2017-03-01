<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <title>TECS</title>
    
    <?php 
        if(isset($_GET['user_name'])){
            $user_name = $_GET['user_name'];
        }
        else {
            $user_name = "";
        }
    ?>
</head>

<body>
    <input type="hidden" id="txUsername" value="<?php echo $user_name?>"> 
    <div style="display:none;">
        <ul class="breadcrumb">
            <li>
                <a href="#"  data-value="myvm_overview.html"><OamStaticUI bindattr="text" maptype="display" bindvalue="my_vm"/></a> <span class="divider">/</span>
            </li>
            <li class="active">
                <a href="#" data-value="myvm_portabledisk.php"><OamStaticUI bindattr="text" maptype="display" bindvalue="portabledisk_cnt"/></a>
            </li>
        </ul>
    </div>
    <div class="box-content">
        <ul class="nav nav-tabs" id="tabsDisk">
            <li class="active"><a href="#removeable_disk"><OamStaticUI bindattr="text" maptype="display" bindvalue="portabledisk_cnt"/></a></li>
        </ul> 
        <div id="diskTabContent" class="tab-content">
            <div class="tab-pane active" id="removeable_disk">
                <div class="row-fluid">
                    <div class="span12" align="right">
                        <div class="btn-toolbar" style="margin: 0;">
                            <div class="btn-group">
                                <button class="btn" id="create"><i class="icon-plus"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="adddisk"/></span></button>
                            </div>
                            <div class="btn-group">
                                <button class="btn" id="remove"><i class="icon-trash"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="remove"/></span></button>
                            </div>
                             
                            <div class="btn-group">
                                <button class="btn" id="myportable_refresh"><i class="icon-refresh"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="refresh"/></span></button>
                            </div>
                        </div>
                    </div>
                </div>

                <div>
                    <table class="table table-striped table-bordered table-condensed bootstrap-datatable datatable" id="tbMyPDisk" style="width:100%;word-break:break-all">
                        <thead> 
                              <tr> 
                                 <th align="left" width="5%"><INPUT id="SelecltAll" type="checkbox" value="" /></th>
                                 <th align="left" width="15%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="name"/></th> 
                                 <th align="left" width="15%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="size"/></th>
                                 <th align="left" width="20%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="desc"/></th>								 
                                 <th align="left" width="15%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="vm"/></th> 
                                 <th align="left" width="10%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="drive"/></th> 
                                 <th align="left" width="15%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="block"/></th> 
                                 <th align="left" width="5%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="blank"/></th>
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
    <script src="js/tecs/myvm_portabledisk.js"></script>
</body>
</html>
