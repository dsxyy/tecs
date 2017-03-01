<!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="utf-8">
        <title>TECS</title>
        <META http-equiv="X-UA-Compatible" content="IE=9" > </META>
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <meta name="description" content="TECS">
        <meta name="author" content="TECS">
    
        <link rel="stylesheet" type="text/css" href="../css/Shojumaru.css"/>
        <link rel="stylesheet" type="text/css" href="../css/Karla_Ubuntu.css"/>
        <link rel="stylesheet" type="text/css" href="../css/Droid_Sans.css"/>
        <link rel="stylesheet" type="text/css" href="../css/Open_Sans.css"/>
        <link rel="stylesheet" type="text/css" href="../css/Ubuntu.css"/>
        <!-- The styles -->
        <link id="bs-css" href="../css/bootstrap-cerulean.css" rel="stylesheet">
        <style type="text/css">
          body {
            padding-bottom: 40px;
          }
          .sidebar-nav {
            padding: 9px 0;
          }
        </style>
        <link href="../css/bootstrap-responsive.css" rel="stylesheet">
        <link href="../css/charisma-app.css" rel="stylesheet">
        <link href="../css/jquery-ui-1.8.21.custom.css" rel="stylesheet">
        <link href="../css/fullcalendar.css" rel="stylesheet">
        <link href="../css/fullcalendar.print.css" rel="stylesheet"  media="print">
        <link href="../css/chosen.css" rel="stylesheet">
        <link href="../css/uniform.default.css" rel="stylesheet">
        <link href="../css/colorbox.css" rel="stylesheet">
        <link href="../css/jquery.cleditor.css" rel="stylesheet">
        <link href="../css/jquery.noty.css" rel="stylesheet">
        <link href="../css/noty_theme_default.css" rel="stylesheet">
        <link href="../css/elfinder.min.css" rel="stylesheet">
        <link href="../css/elfinder.theme.css" rel="stylesheet">
        <link href="../css/jquery.iphone.toggle.css" rel="stylesheet">
        <link href="../css/opa-icons.css" rel="stylesheet">
        <link href="../css/uploadify.css" rel="stylesheet">
        <link rel="shortcut icon" href="../img/favicon.ico">
    </head>
    
    <body>
        <script type="text/javascript">
            function getCookie(objName){
                var arrStr = document.cookie.split("; ");  
                for(var i = 0;i < arrStr.length;i ++){  
                    var temp = arrStr[i].split("=");  
                    if(temp[0] == objName) return unescape(temp[1]);  
                }   
                return null;
            }  

            var current_theme = getCookie("current_theme")==null ? "cerulean" : getCookie("current_theme");
            document.getElementById('bs-css').href = '../css/bootstrap-' + current_theme + '.css';   

            var login_page  = 'https://' + window.location.hostname + '/login.html';
            var count = 15;
            function timeout()
            {
                if(count-- > 0){                
                    setTimeout(timeout, 1000);
                    document.getElementById("countdown").innerHTML = "Go to login page after " + count + " seconds!"; 
                }
                else {
                    window.location.href=login_page;
                }
            }
            setTimeout(timeout, 1000);  
        </script>

        <!-- topbar starts -->
        <div class="navbar">
            <div class="navbar-inner">
                <div class="container-fluid">
                    <a class="brand" href="../login.html"> <img alt="Charisma Logo" src="../img/logo20.png" /> <span>TECS</span></a>
                </div>
            </div>
        </div>
        <!-- topbar ends -->
        <div class="container-fluid" style="font-size:18px;">
        <h3 class='alert alert-block' id="countdown">Go to login page after 15 seconds!</h3>
<?php
    if ($_FILES["license_fullname"]["error"] > 0)
    {
        echo "Error: " . $_FILES["license_fullname"]["error"] . "<br />";
    }
    else
    {
        $license_tmp_dir = "/tmp/tecs_license";
        $license_ini     = $license_tmp_dir . "/license.ini";
        $license_sig     = $license_tmp_dir . "/license.sig";
        $license_md5     = $license_tmp_dir . "/license.md5";
        $license_tar     = $license_tmp_dir . "/" . $_FILES["license_fullname"]["name"];

        if(!is_dir($license_tmp_dir))   {  
            mkdir($license_tmp_dir);   //创建目录  
            chmod($license_tmp_dir, 0777);//改变权限  
        }  

        if(move_uploaded_file($_FILES["license_fullname"]["tmp_name"], $license_tar))
        {
        	echo "<h3 class='alert alert-success'>Upload " . $_FILES["license_fullname"]["name"] . " success!";
            echo "<br />";
        }
        else
        {
        	echo "<h3 class='alert alert-error'>Upload " . $_FILES["license_fullname"]["name"] . " failed!</h3>";
        	exit();
        }

        //upzip
        chmod($license_tar, 0777);
        chdir($license_tmp_dir);
        exec("tar -zxvf " . $license_tar);
        exec("rm -f ". $license_tar);

        //check file context
        if(!is_file($license_ini) || !is_file($license_sig) || !is_file($license_md5)){
            echo "<h3 class='alert alert-error'>Invaild license file!</h3>";
            exec("rm -f *");
            exit();
        }

        $host_fingerprint = $_POST["license_fingerprint_upload"];
        $ini_data = parse_ini_file($license_ini, true);
        if($ini_data == FALSE || $ini_data["limitation"]["hardware_fingerprints"] != $host_fingerprint){
            echo "<h3 class='alert alert-error'>The fingerprint in license file is invaild!</h3>";
            exec("rm -f *");
            exit();
        }

        $handle = @fopen($license_md5, "r");
        if (!$handle) {
        	echo "<h3 class='alert alert-error'>Open license md5 file failed!</h3>";
            exec("rm -f *");
        	exit();
        } 

        if(($buffer = fgets($handle, 4096)) == false) {
           	echo "<h3 class='alert alert-error'>Read license.ini md5 failed!</h3>";
            exec("rm -f *");
           	exit();
        }
        $ini_md5 = substr($buffer, 0, 32);            
        if(md5_file($license_ini) !== $ini_md5) {
            echo "<h3 class='alert alert-error'>license.ini md5(" . $ini_md5 . ") is not match!</h3>";
            exec("rm -f *");
            exit();
        } 
     
        if(($buffer = fgets($handle, 4096)) == false) {
           	echo "<h3 class='alert alert-error'>Read license.sig md5 failed!</h3>";
            exec("rm -f *");
           	exit();        	
        }
        $sig_md5 = substr($buffer, 0, 32);                    
        if(md5_file($license_sig) != $sig_md5) {
        	echo "<h3 class='alert alert-error'>license.sig md5(" . $sig_md5 . ") is not match!</h3>";
            exec("rm -f *");
        	exit();
        }       

        fclose($handle);               

        //cp to license dir and restart tc
        exec("/opt/tecs/wui/htdocs/copy_license");

        echo "Update license success!";
        echo "<br />";
        echo "The System will restart, Please login after some minutes!</h3>";
        echo "<br />";

        exec("rm -f *");
    }
?>

    </div><!--/.fluid-container-->
</body>
</html>