<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <title>TECS</title>
    <?php 
        if(isset($_GET['flag'])){
            $flag =  $_GET['flag'];
        }
        else{
            $flag = "";
        } 
    ?>
</head>

<body>
    <input type="hidden" id="txFlag" value="<?php echo $flag?>">
    <div class="row-fluid">
        <!-- left menu starts -->
        <div class="span2 main-menu-span">
            <div class="well nav-collapse sidebar-nav">
                <ul class="nav nav-tabs nav-stacked main-menu">
                    <li class="active"><a href="vr_overview.html"><span><img src="img/home16.png"></img></span>&nbsp;<span class="hidden-tablet"><OamStaticUI bindattr="text" maptype="display" bindvalue="overview"/></span></a></li>
                    <li><a href="vr_user_resource.php"><span><i class="icon icon-color icon-user"></i></span>&nbsp;<span class="hidden-tablet"><OamStaticUI bindattr="text" maptype="display" bindvalue="user_resource"/></span></a></li>
                    <li><a href="vr_project.php"><span><img src="img/project16.png"></img></span>&nbsp;<span class="hidden-tablet"><OamStaticUI bindattr="text" maptype="display" bindvalue="project_manager"/></span></a></li>
                    <li><a href="vr_vm.php"><span><img src="img/computer16.png"></img></span>&nbsp;<span class="hidden-tablet"><OamStaticUI bindattr="text" maptype="display" bindvalue="vm_manager"/></span></a></li>
                    <li><a href="vr_portabledisk.php"><span><img src="img/portable_disk16.png"></img></span>&nbsp;<span class="hidden-tablet"><OamStaticUI bindattr="text" maptype="display" bindvalue="pdisk_manager"/></span></a></li>															
                    <li><a href="vr_vt.php"><span><img src="img/template16.png"></img></span>&nbsp;<span class="hidden-tablet"><OamStaticUI bindattr="text" maptype="display" bindvalue="vt_manager"/></span></a></li>
                    <li><a href="vr_image.php"><span><img src="img/image16.png"></img></span>&nbsp;<span class="hidden-tablet"><OamStaticUI bindattr="text" maptype="display" bindvalue="image_manager"/></span></a></li>

                </ul>
            </div><!--/.well -->
        </div><!--/span-->
        <!-- left menu ends -->
        
        <div id="right_content" class="span10">
        <!-- content starts -->
        <!-- content ends -->
        </div><!--/#content.span10-->
    </div><!--/fluid-row-->

    <!-- external javascript
    ================================================== -->
    <!-- Placed at the end of the document so the pages load faster -->
    <script src="js/tecs/vm_common.js"></script>
    <script src="js/tecs/vr.js"></script>
</body>
</html>
