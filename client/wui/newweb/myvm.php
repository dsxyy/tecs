﻿<!DOCTYPE html>
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
                    <li class="active"><a href="myvm_overview.html"><span><img src="img/home16.png"></img></span> <span class="hidden-tablet"><OamStaticUI bindattr="text" maptype="display" bindvalue="overview"/></span></a></li>
                    <li><a href="myvm_project.html"><span><img src="img/project16.png"></img></span> <span class="hidden-tablet"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="project"/></span></a></li>
                    <li><a href="myvm_vm.php"><span><img src="img/computer16.png"></img></span> <span class="hidden-tablet"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="vm"/></span></a></li>
                    <li><a href="myvm_portabledisk.php"><span><img src="img/portable_disk16.png"></img></span> <span class="hidden-tablet"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="pdisk"/></span></a></li>					
                    <li><a href="myvm_vt.html"><span><img src="img/template16.png"></img></span> <span class="hidden-tablet"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="vt"/></span></a></li>
                    <li><a href="myvm_image.html"><span><img src="img/image16.png"></img></span> <span class="hidden-tablet"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="image"/></span></a></li>
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
    <script src="js/tecs/myvm.js"></script>
</body>
</html>
