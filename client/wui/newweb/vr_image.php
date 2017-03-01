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
                <a href="#"  data-value="vr_overview.html"><OamStaticUI bindattr="text" maptype="display" bindvalue="vr_app"/></a> <span class="divider">/</span>
            </li>
            <li class="active">
                <a href="#" data-value="vr_image.php"><OamStaticUI bindattr="text" maptype="display" bindvalue="image_manager"/></a>
            </li>
        </ul>
    </div>
    
    <div class="box-content">
        <ul class="nav nav-tabs" id="imageTab">
            <li class="active"><a href="#imageOverview" ><OamStaticUI bindattr="text" maptype="display" bindvalue="imageServer"/></a></li>
            <li ><a href="#allImage"><OamStaticUI bindattr="text" maptype="display" bindvalue="all_image"/></a></li>
            <li><a href="#deployImage"><OamStaticUI bindattr="text" maptype="display" bindvalue="deploy_image"/></a></li>
        </ul>
        <div id="imageTabContent" class="tab-content">
            <div class="tab-pane active" id="imageOverview"> 
                <div class="tab-pane box span6"  style="height:400px;">
               
                    <div class="box-header well" data-original-title>
                        <h2><i class="icon-list-alt"></i> <span><OamStaticUI bindattr="text" maptype="display" bindvalue="image_space_info"/></span></h2>
                    </div>
                   
                    <div class="box-content">
                        <div id="image_piechart" style="height:200px;"></div>
                            <div class="row-fluid">
                                <div class="span5"></div>
                                <div id="image_space_info" class="span5"></div>
                            </div>
                        </div>
                    </div> 
            </div>            
            <div class="tab-pane" id="allImage"> 
                <div class="row-fluid"  style="table-layout:fixed;word-break:break-all">
                    <div class="span4">
                        <select data-placeholder=""  id="selectAllImageUser" multiple data-rel="chosen">
                        </select>
                    </div>                    
                    <div class="span8" align="right">
                        <button class="btn" id="upload"><i class="icon-circle-arrow-up"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="upload"/></span></button>                          
                        <button class="btn" id="remove"><i class="icon-trash"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="remove"/></span></button>
                        <button class="btn" id="enable"><i class="icon-ok-sign"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="enable"/></span></button>
                        <button class="btn" id="disable"><i class="icon-ban-circle"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="disable"/></span></button>
                        <button class="btn" id="public"><i class="icon-share"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="publish"/></span></button>
                        <button class="btn" id="private"><i class="icon-remove-circle"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="unpublish"/></span></button>
                        <button class="btn" id="refresh"><i class="icon-refresh"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="refresh"/></span></button>
                    </div>
                </div>
                <div>
                    <table class="table table-striped table-bordered table-condensed" id="tbImage">
                        <thead> 
                            <tr>
                                <th align="left" width="25px;"><INPUT id="SelecltAll" type="checkbox" value="" /></th>
                                <th align="left" width="25px;">ID</th> 
                                <th align="left" width="150px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="filename"/></th>
                                <th align="left" width="100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="register_time"/></th>
                                <th align="left" width="50px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="status"/></th>
                                <th align="left" width="50px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="ispublish"/></th> 
                                <th align="left" width="50px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="running_vms"/></th>
                                <th align="left" width="50px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="container_format"/></th>
                                <th align="left" width="50px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="image_size"/></th>
                                <th align="left" width="50px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="user"/></th>
                                <th align="left" width="50px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="desc"/></th>
                                <th align="left" width="50px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="blank"/></th>
                            </tr>
                        </thead>
                        <tbody style="word-wrap:break-word;word-break:break-all;"></tbody>
                    </table>
                </div>
            </div>
            <div class="tab-pane" id="deployImage">
                <div class="row-fluid" style="table-layout:fixed;word-break:break-all">
                    <div class="span4">
                        <select data-placeholder=""  id="selectDeployImageUser" multiple data-rel="chosen">
                        </select>
                    </div>                    
                    <div class="span8" align="right">
                        <button class="btn" id="cancel_deployimage"><i class="icon-step-backward"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="cancel_deploy"/></span></button>
                        <button class="btn" id="refresh_deployimage"><i class="icon-refresh"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="refresh"/></span></button>
                    </div>
                </div>
                <div>
                    <table class="table table-bordered table-condensed" id="tbDeployImage">
                        <thead>
                            <tr>
                                <th align="left" width="25px;"><INPUT id="SelecltDeployImageAll" type="checkbox" value="" /></th>
                                <th align="left" width="150px;">Base ID</th>
                                <th align="left" width="100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="cluster"/>集群</th>
                                <th align="left" width="100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="image_id"/>镜像ID</th>
                                <!--th align="left" width="100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="status"/>状态</th-->
                                <th align="left" width="150px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="filename"/>文件名</th>
                                <th align="left" width="100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="user"/>用户</th>
                                <th align="left" width="50px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="desc"/></th>
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
    <script src="js/tecs/vm_image.js"></script>
    <script src="js/tecs/vr_image.js"></script>
</body>
</html>
