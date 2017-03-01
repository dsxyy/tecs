var highlight_class_name = "fc-state-highlight";
var global_vm_data = null;
var vm_config_breadcrumb_list = new Array();
//action 虚拟机：0-创建 1-修改 2-查看  模板：3-创建
function InitCreateVmBreadCrumb(vm_action, project_name, vid, tabindex, level, breadcrumb_list){ 
    var breadcrumbobj = new Object();
    breadcrumbobj.name = page.res[page.pagelan].type.display["my_vm"];
    breadcrumbobj.datavalue = "myvm.php";
    breadcrumbobj.showurl_callback = ShowLevel1;
    breadcrumb_list.push(breadcrumbobj);
    
    breadcrumbobj = new Object();
    breadcrumbobj.name = page.res[page.pagelan].type.field_name["vm"];
    breadcrumbobj.datavalue = "myvm.php?flag=1";
    breadcrumbobj.datavalue1 = "myvm_vm.php?tabindex=" + tabindex + "&project_name=" + project_name;
    breadcrumbobj.datavalue2 = "myvm_vm.php?tabindex=1&project_name=" + project_name;//正确情况下跳转页面
    breadcrumbobj.showurl_callback = ShowLevel2;
    breadcrumb_list.push(breadcrumbobj);
    
    breadcrumbobj = new Object();
    breadcrumbobj.name = page.res[page.pagelan].type.opr["create_vm"];
    breadcrumbobj.datavalue = "vm_config.php?action=" + vm_action + "&project_name=" + project_name + "&level=" + level;
    breadcrumbobj.showurl_callback = ShowLevel3;
    breadcrumb_list.push(breadcrumbobj);
}

function InitModifyVmBreadCrumb(vm_action, project_name, vid, tabindex, level, breadcrumb_list){ 
    var breadcrumbobj = new Object();
    breadcrumbobj.name = page.res[page.pagelan].type.display["my_vm"];
    breadcrumbobj.datavalue = "myvm.php";
    breadcrumbobj.showurl_callback = ShowLevel1;
    breadcrumb_list.push(breadcrumbobj);
        
    breadcrumbobj = new Object();
    breadcrumbobj.name = page.res[page.pagelan].type.field_name["vm"];
    breadcrumbobj.datavalue = "myvm.php?flag=1";
    breadcrumbobj.datavalue1 = "myvm_vm.php?tabindex=" + tabindex + "&project_name=" + project_name;
    breadcrumbobj.datavalue2 = "myvm_vm.php?tabindex=" + tabindex + "&project_name=" + project_name;//正确情况下跳转页面
    breadcrumbobj.showurl_callback = ShowLevel2;
    breadcrumb_list.push(breadcrumbobj);
    
    breadcrumbobj = new Object();
    breadcrumbobj.name = page.res[page.pagelan].type.opr["modify"] + "-" + vid;
    breadcrumbobj.datavalue = "vm_config.php?action=" + vm_action + "&vid=" + vid + "&tabindex=" + tabindex + "&project_name=" + project_name + "&level=" + level;
    breadcrumbobj.showurl_callback = ShowLevel3;
    breadcrumb_list.push(breadcrumbobj);
}

function InitCreateVtBreadCrumb(vm_action, project_name, vid, tabindex, level, breadcrumb_list){ 
    var breadcrumbobj = new Object();
    breadcrumbobj.name = page.res[page.pagelan].type.display["vr_app"];
    breadcrumbobj.datavalue = "vr.php";
    breadcrumbobj.showurl_callback = ShowLevel1;
    breadcrumb_list.push(breadcrumbobj);
    
    breadcrumbobj = new Object();
    breadcrumbobj.name = page.res[page.pagelan].type.display["vt_manager"];
    breadcrumbobj.datavalue = "vr.php?flag=1";
    breadcrumbobj.datavalue1 = "vr_vt.php";
    breadcrumbobj.datavalue2 = "vr_vt.php";//正确情况下跳转页面
    breadcrumbobj.showurl_callback = ShowLevel2;
    breadcrumb_list.push(breadcrumbobj);
    
    breadcrumbobj = new Object();
    breadcrumbobj.name = page.res[page.pagelan].type.opr["create_vt"];
    breadcrumbobj.datavalue = "vm_config.php?action=" + vm_action + "&level=" + level;
    breadcrumbobj.showurl_callback = ShowLevel3;
    breadcrumb_list.push(breadcrumbobj);
}

function InitModifyVtBreadCrumb(vm_action, project_name, vid, tabindex, level, breadcrumb_list){ 
    var breadcrumbobj = new Object();
    breadcrumbobj.name = page.res[page.pagelan].type.display["vr_app"];
    breadcrumbobj.datavalue = "vr.php";
    breadcrumbobj.showurl_callback = ShowLevel1;
    breadcrumb_list.push(breadcrumbobj);
    
    breadcrumbobj = new Object();
    breadcrumbobj.name = page.res[page.pagelan].type.display["vt_manager"];
    breadcrumbobj.datavalue = "vr.php?flag=1";
    breadcrumbobj.datavalue1 = "vr_vt.php";
    breadcrumbobj.datavalue2 = "vr_vt.php";//正确情况下跳转页面
    breadcrumbobj.showurl_callback = ShowLevel2;
    breadcrumb_list.push(breadcrumbobj);
    
    breadcrumbobj = new Object();
    breadcrumbobj.name = page.res[page.pagelan].type.opr["modify"] + "-" + vid;
    breadcrumbobj.datavalue = "vm_config.php?action=4&vid=" + vid + "&level=" + level;
    breadcrumbobj.showurl_callback = ShowLevel3;
    breadcrumb_list.push(breadcrumbobj);
}

function InitCreateVmByVtBreadCrumb(vm_action, project_name, vid, tabindex, level, breadcrumb_list){ 
    var breadcrumbobj = new Object();
    breadcrumbobj.name = page.res[page.pagelan].type.display["my_vm"];
    breadcrumbobj.datavalue = "myvm.php";
    breadcrumbobj.showurl_callback = ShowLevel1;
    breadcrumb_list.push(breadcrumbobj);
    
    breadcrumbobj = new Object();
    breadcrumbobj.name = page.res[page.pagelan].type.field_name["vt"];
    breadcrumbobj.datavalue = "myvm.php?flag=1";
    breadcrumbobj.datavalue1 = "myvm_vt.html";
    breadcrumbobj.datavalue2 = "myvm_vm.php?tabindex=1&project_name=" + project_name;;//正确情况下跳转页面
    breadcrumbobj.showurl_callback = ShowLevel2;
    breadcrumb_list.push(breadcrumbobj);
    
    breadcrumbobj = new Object();
    breadcrumbobj.name = page.res[page.pagelan].type.opr["use_template"] + "(" + vid + ")" + page.res[page.pagelan].type.opr["create_vm"];
    breadcrumbobj.datavalue = "vm_config.php?action=6&vid=" + vid + "&level=" + level;
    breadcrumbobj.showurl_callback = ShowLevel3;
    breadcrumb_list.push(breadcrumbobj);
}

function ShowBreadCrumb(){
    var vm_action    = parseInt($("#txAction").val());
    var project_name = $("#txProjectName").val();
    var vid          = $("#txVid").val(); 
    var tabindex     = $("#txTabIndex").val();
    var level        = $("#txLevel").val();
    var init_breadcrumb_callback_list = [
        [global_createvm_acton,InitCreateVmBreadCrumb],
        [global_modifyvm_acton,InitModifyVmBreadCrumb],
        [global_createvt_acton,InitCreateVtBreadCrumb],
        [global_modifyvt_acton,InitModifyVtBreadCrumb],
        [global_createvmbyvt_acton,InitCreateVmByVtBreadCrumb],
        [global_modifydeplofyvm_acton,InitModifyVmBreadCrumb]
    ]; 
    //初始化
    for(var i = 0;i < init_breadcrumb_callback_list.length;i++){
        var init_breadcrumb_callback = init_breadcrumb_callback_list[i];
        if(init_breadcrumb_callback[0] == vm_action){
            init_breadcrumb_callback[1](vm_action, project_name, vid, tabindex, level, vm_config_breadcrumb_list);
        }
    }
    //显示
    for(var i = 0;i < vm_config_breadcrumb_list.length;i++){
        var breadcrumb_obj = vm_config_breadcrumb_list[i];
        $('ul.breadcrumb li a').eq(i).html(breadcrumb_obj.name);
        $('ul.breadcrumb li a').eq(i).attr("href",breadcrumb_obj.href);
    }
    $('ul.breadcrumb li a').click(function(e){
        var index = $(this).parent().index();
        vm_config_breadcrumb_list[index].showurl_callback(vm_config_breadcrumb_list[index]);
    });
}

function ShowLevel1(breadcrumb_obj){
    Load_Page($('#content'), breadcrumb_obj.datavalue);
}

function ShowLevel2(breadcrumb_obj){
    Load_Page($('#content'), breadcrumb_obj.datavalue); 
    Switch_Vr_Page(breadcrumb_obj.datavalue1);
}

function ShowLevel3(breadcrumb_obj){
     Load_Page($('#content'), breadcrumb_obj.datavalue);
}


function ShowImage(){
    var total_running_vms = 0;
    var sort_image = [];
    var index = 0;
    var rs = QueryImages(-3, "", -1, "", "");
    if(rs == null || rs.length == 0){
        return;
    }
    
    //按引用次数大小排序
    for(var i = 0;i < rs.length;i++){
        var onedata = rs[i];
        if(onedata["type"] != "machine"){
            continue;
        }
        if(!onedata["is_enabled"]){
            continue;
        }
        if(onedata["container_format"] != ""){
            continue;
        }
        var running_vms = onedata["running_vms"];
        total_running_vms += running_vms;
        sort_image[index++] = onedata;
    } 
    
    for(var i = 0;i < sort_image.length;i++){
        for(var j = i + 1;j < sort_image.length;j++){
            if(sort_image[i]["running_vms"] < sort_image[j]["running_vms"]){
                var tmp = sort_image[i];
                sort_image[i] = sort_image[j];
                sort_image[j] = tmp;
            }
        }
    }
    
    for(var i=0; i< sort_image.length; i++){
        var onedata = sort_image[i];
        onedata["reserved_backup"] = 0;
        var raty_cl = "raty" + "_" + onedata["id"].toString(); 
        var score = parseInt(((onedata["running_vms"] / total_running_vms) * 10 + 1)/2);
        var dv_id = "image_" + onedata["id"].toString();
        var dv = '<div class="box span12 box_image" id="' + dv_id + '">'; 
        dv += '<div class="box-content center" style="overflow:auto;">';
        dv += ShowOSIcon(onedata["os_type"]);
        //dv += '<div class="raty ' + raty_cl + ' center"></div>';
        dv += '<span class="clearfix">' + page.res[page.pagelan].type.field_name["filename"] + ':' + onedata["file"] + '</span>';
        //dv += '<span class="clearfix">' + page.res[page.pagelan].type.field_name["os_type"] + ':' + onedata["os_type"] + '</span>';                
        dv += '<span class="clearfix">' + page.res[page.pagelan].type.field_name["size"] + ':' + getformatsize(onedata["size"]) + '</span>';
        dv += '<span class="clearfix">' + page.res[page.pagelan].type.field_name["creater"] + ':' + onedata["user_name"] + '</span>';
        dv += '<span class="clearfix">' + page.res[page.pagelan].type.field_name["disk_format"] + ':' + onedata["disk_format"] + '</span>';        
        dv += '</div>';
        dv += '</div>';
        $("#collapseImage .accordion-inner").append(dv);
        $('.' + raty_cl).raty({
		    score : score //default stars
	    });
        $("#" + dv_id).data("data",onedata);
    }

    $("#image_tooltips").attr("title", page.res[page.pagelan].type.err_info["vm_image_format_error"]);
    $(".tooltip").hide();
    $('[data-rel="tooltip"]').tooltip({"placement":"bottom"});

    UpdateImageList(global_vm_data.cfg_info.base_info.hypervisor);
} 

function UpdateImageList(hypervisor){
    $("#image_tooltips").show();
    $("#collapseImage").find(".box_image").each(function(){
        $(this).hide();
        var image = $(this).data("data");
        if(hypervisor == "kvm"){    
            if(image["disk_format"] == "raw" || image["disk_format"] == "qcow2"){
                $(this).show();
                $("#image_tooltips").hide();
            }
        }
        else if(hypervisor = "xen"){    
            if(image["disk_format"] == "raw" || image["disk_format"] == "vhd"){
                $(this).show();
                $("#image_tooltips").hide();
            }
        }
        else if(hypervisor = "any"){
            if(image["disk_format"] == "raw"){
                $(this).show();
                $("#image_tooltips").hide();
            }
        }
    });
} 

var vm_obj = new Object();
function RemoveAllItemHighlight(){
    for(var i = 0;i < vm_obj.drag_elem_list.length;i++){
        var drag_obj = vm_obj.drag_elem_list[i];
        if(drag_obj.collapse){
            $( drag_obj.collapse ).find(".box").removeClass(highlight_class_name);
        }
    }
    for(var i = 0;i < vm_obj.drop_elem_list.length;i++){
        var drop_obj = vm_obj.drop_elem_list[i];
        if(drop_obj.collapse){
            $( drop_obj.collapse ).find(".box").removeClass(highlight_class_name);
        }
    }
}

function SetItemHighlight(){
    RemoveAllItemHighlight();
    $(this).addClass(highlight_class_name);
    var drag_item = GetHighLightDragItem($(this));
    if(!drag_item){
        return;
    }
    for(var i = 0;i < vm_obj.drop_elem_list.length;i++){
        var drop_obj = vm_obj.drop_elem_list[i];
        var arr_clname = drop_obj.clname.split(" ");
        for(var j = 0;j < arr_clname.length;j++){
            if(arr_clname[j] == drag_item.clname){
                $( drop_obj.collapse ).find("div." + arr_clname[j]).addClass(highlight_class_name);
                if(!$(drop_obj.collapse).hasClass("in")){
                    $('a[href=' + drop_obj.collapse + ']').click();
                }
            }
        }
    }
}

function GetHighLightDragItem(item){ 
    for(var i = 0;i < vm_obj.drag_elem_list.length;i++){
        var drag_obj = vm_obj.drag_elem_list[i]; 
        if(item.hasClass(drag_obj.clname)){
            return drag_obj;
        }
    } 
    return null;
}



function Show_Drop_Elem(){
    var count = 0;
    var type  = "";
     for(var i = 0;i < vm_obj.drop_elem_list.length;i++){
        var obj = vm_obj.drop_elem_list[i];
        if(type != obj.collapse){
            count = 0;
            type = obj.collapse;
        }
        var info = '<div class="box span3 ' + obj.clname +'" style="cursor:pointer;';
        if((count > 0) && (count % 4 == 0)){
            info += 'margin-left:0px;';
        }
        info += '">';
        info +=    '    <div class="box-header well center">'; 
        info +=    '        <h5>' + obj.title + '</h5>';
        info +=    '    </div>';
        info +=    '    <div class="box-content center" style="overflow:auto;">';
        info +=    '        <div class="span12" style="height:50px;">';
        info +=    '        </div">';
        info +=    '    </div">';
        info +=    '</div">';
        $(obj.collapse).find(".accordion-inner").append(info);
        count++;
    }
}

//function SetDragable(dd_conect_obj){
//    $( dd_conect_obj.drager ).draggable({
//        helper: "clone",
//        opacity:0.35,
//        revert:"invalid",
//        cursor: "move",
//        start: function() {
//            SetItemHighlight();
//        },
//        stop: function() {
//        }
//    });
//}
//
//function SetDropable(dd_conect_obj){
//    $( dd_conect_obj.droper ).droppable({
//        accept:$( dd_conect_obj.drager ),
//        drop: function( event, ui ){
//            var index = $( this ).index();
//            AddData(ui.draggable, $( this ));
//        }
//    });
//}

function AddData(drager,droper){
    for(var i =0;i < vm_obj.drag_elem_list.length;i++){
        var drag_elem_obj = vm_obj.drag_elem_list[i];
        if(drager.hasClass(drag_elem_obj.clname)){
            var vm_action = parseInt($("#txAction").val());
            drag_elem_obj.adddata_callback(droper, drag_elem_obj, global_vm_data, vm_action, $("#idx_confirmDlg"), drager);
        }
    }
}

//在高亮方式下增加V4上下文
function AddContextByHighlight($item){
    //如果V4上下文高亮,且没有配置V4上下文-先增加
    var drag = $("#accordionDevice").find("."+highlight_class_name);
    if(drag == null){
        return;
    }
    if(drag.hasClass("box_v4context")){
        AddData(drag,$item);
    }
    return;
}

function Init_DragDrop_Connect(){
    for(var i = 0;i < vm_obj.drag_elem_list.length;i++){
        var drag_obj = vm_obj.drag_elem_list[i];
        if(drag_obj.collapse == null || drag_obj.collapse == null){
            continue;
        }
        //增加高亮
        $( drag_obj.collapse).find("."+drag_obj.clname).hover(SetItemHighlight).click(SetItemHighlight);
        //拖
        //SetDragable(drag_obj);
        //放
        //SetDropable(drag_obj);
    }
    $( ".collapse-para div.box" ).click(function( event ) {
        var $item = $( this ),
            $target = $( event.target );
            
        if ( $target.is( "span.icon-trash" ) ) {
            var clname = $target[0].className.split(" ")[2];
            DelVmDataDiag(clname,$item);
        }
        else{
            if($item.hasClass("configed")){
                AddContextByHighlight($item);
                ShowVmDataDiag($item);
            } 
            else{
                //高亮
                if($item.hasClass(highlight_class_name)){
                    var drag = $("#accordionDevice").find("."+highlight_class_name);
                    if(drag == null){
                        return;
                    }
                    AddData(drag,$item);
                }
            }
        }
    });
}

function ControlModifyDeployVmShow(){
    for(var i = 0;i < vm_obj.drag_elem_list.length;i++){
        var drag_elem_obj = vm_obj.drag_elem_list[i];
        $("."+drag_elem_obj.clname).hide();
    }
    $(".collapse-para").find(".box").hide(); 
    $(".box_memory").show();
    $(".box_cpu").show();
    //$(".box_image").show();
}

function GetDroperByClname(clname){
    for(var i = 0;i < vm_obj.drop_elem_list.length;i++){
        var drop_obj = vm_obj.drop_elem_list[i];
        var arrclname = drop_obj.clname.split(" ");
        for(var j = 0;j < arrclname.length;j++){
            if(arrclname[j] == clname){
                return drop_obj;
            }
        }
    } 
    return null;
}

function ShowVmDataDiag(droper){
    var drag_elem_obj = droper.data("data");
    var vm_action = parseInt($("#txAction").val());
    drag_elem_obj.showdata_callback($("#idx_confirmDlg"), droper, global_vm_data, vm_action, drag_elem_obj);
} 

function DelVmDataDiag(clname,droper){
    for(var i = 0;i < vm_obj.drag_elem_list.length;i++){
        var drag_elem_obj = vm_obj.drag_elem_list[i];
        if(drag_elem_obj.clname == clname){
            drag_elem_obj.deldata_callback($("#idx_confirmDlg"), droper, global_vm_data, drag_elem_obj);
        }
    }
} 

function Init_Drag_Elem_Data(){
    for(var i = 0;i < vm_obj.drag_elem_list.length;i++){
        var drag_elem_obj = vm_obj.drag_elem_list[i];
        var vm_action = parseInt($("#txAction").val());
        var clname = drag_elem_obj.clname;
        var drop_obj = GetDroperByClname(clname);
        if(drop_obj == null){
            continue;
        }
        var droper = $(drop_obj.collapse).find("div." + clname);
        
        if((drag_elem_obj.collapse == null) && 
            (vm_action == global_createvm_acton || vm_action == global_createvt_acton)){//新增默认参数
            drag_elem_obj.adddata_callback(droper, drag_elem_obj, global_vm_data, vm_action);
        }
        if(drag_elem_obj.showimg_callback != null){
            drag_elem_obj.showimg_callback(droper, drag_elem_obj, global_vm_data);
        }
    }
    if(vm_action == global_modifydeplofyvm_acton){
        ControlModifyDeployVmShow();
    }
}

function InitDropObj(){
    //---------------drop---------------
    var _title = page.res[page.pagelan].type.display["baseinfo"]
                          + '&' 
                          + 'CPU'
                          + '&'
                          + page.res[page.pagelan].type.display["memory"]
                          + '&'
                          + page.res[page.pagelan].type.display["remote_control"];
    $('a[href="#collapseParaBase"]').html(_title);
     
    _title = page.res[page.pagelan].type.field_name["image"]
                          + '&' 
                          + 'IDE Disk'
                          + '&'
                          + page.res[page.pagelan].type.display["cdrom"]
                          + '&'
                          + page.res[page.pagelan].type.display["context"];
    $('a[href="#collapseParaIDE"]').html(_title);
     
     _title = 'SCSI Disk';
    $('a[href="#collapseParaSCSI"]').html(_title);
     
     _title = page.res[page.pagelan].type.display["noraml_nic"]
                          + '&' 
                          + page.res[page.pagelan].type.display["watch_dog"];
    $('a[href="#collapseParaNormalNic"]').html(_title); 
     
    _title = page.res[page.pagelan].type.display["sriov_nic"];
    $('a[href="#collapseParaSRIOVNic"]').html(_title);
     
    _title = page.res[page.pagelan].type.display["pci_device"];
    $('a[href="#collapseParaPCI"]').html(_title); 
    
    var drop_elem_list = [];
    //summary  
    var drop_elem_obj      = new Object();
    drop_elem_obj.title    = page.res[page.pagelan].type.display["baseinfo"];
    drop_elem_obj.clname   = "box_summary";
    drop_elem_obj.collapse = "#collapseParaBase";
    drop_elem_list.push(drop_elem_obj);
    
    //CPU
    drop_elem_obj          = new Object();
    drop_elem_obj.title    = "CPU";
    drop_elem_obj.clname   = "box_cpu";
    drop_elem_obj.collapse = "#collapseParaBase";
    drop_elem_list.push(drop_elem_obj);
     
    //memory
    drop_elem_obj            = new Object();
    drop_elem_obj.title      = page.res[page.pagelan].type.display["memory"];
    drop_elem_obj.clname     = "box_memory";
    drop_elem_obj.collapse   = "#collapseParaBase";
    drop_elem_list.push(drop_elem_obj);
    
    //remote control
    drop_elem_obj            = new Object();
    drop_elem_obj.title      = page.res[page.pagelan].type.display["remote_control"];
    drop_elem_obj.clname     = "box_remotecontrol";
    drop_elem_obj.collapse   = "#collapseParaBase";
    drop_elem_list.push(drop_elem_obj);
    
    //hda
    drop_elem_obj          = new Object(); 
    drop_elem_obj.title    = page.res[page.pagelan].type.field_name["image"];
    drop_elem_obj.clname     = "box_image";
    drop_elem_obj.collapse   = "#collapseParaIDE";
    drop_elem_list.push(drop_elem_obj);
     
    //hdb
    drop_elem_obj          = new Object(); 
    drop_elem_obj.title    = "hdb";
    drop_elem_obj.clname     = "box_ide";
    drop_elem_obj.collapse   = "#collapseParaIDE";
    drop_elem_list.push(drop_elem_obj);
     
    //hdc
    drop_elem_obj          = new Object(); 
    drop_elem_obj.title    = "hdc" ;
    drop_elem_obj.clname     = "box_ide box_cdrom";
    drop_elem_obj.collapse   = "#collapseParaIDE";
    drop_elem_list.push(drop_elem_obj); 
    
    //hdd
    drop_elem_obj          = new Object(); 
    drop_elem_obj.title    = page.res[page.pagelan].type.field_name["context"];
    drop_elem_obj.clname     = "box_noramlcontext box_v4context";
    drop_elem_obj.collapse   = "#collapseParaIDE";
    drop_elem_list.push(drop_elem_obj);
     
    //sda~sdh
    for(var i = 97; i <= 104;i++){
        drop_elem_obj          = new Object(); 
        drop_elem_obj.title    = "sd" + String.fromCharCode(i);
        drop_elem_obj.clname   = "box_scsi";
        drop_elem_obj.collapse = "#collapseParaSCSI";
        drop_elem_list.push(drop_elem_obj);
    }
     
    //normal nic 1~8 
    for(var i = 1; i <= 7;i++){
        drop_elem_obj         = new Object(); 
        drop_elem_obj.title   = i;
        drop_elem_obj.clname     = "box_normalnic";
        drop_elem_obj.collapse   = "#collapseParaNormalNic";
        drop_elem_list.push(drop_elem_obj);
    }
    
    drop_elem_obj            = new Object(); 
    drop_elem_obj.title      = "8";
    drop_elem_obj.clname     = "box_normalnic box_watchdog";
    drop_elem_obj.collapse   = "#collapseParaNormalNic";
    drop_elem_list.push(drop_elem_obj);
    
    
     //sriov nic 1~16
    for(var i = 1; i <= 16;i++){
        drop_elem_obj         = new Object(); 
        drop_elem_obj.title   = i;
        drop_elem_obj.clname     = "box_sriovnic";
        drop_elem_obj.collapse   = "#collapseParaSRIOVNic";
        drop_elem_list.push(drop_elem_obj);
    }
    drop_elem_obj         = new Object(); 
    drop_elem_obj.title   = "1";
    drop_elem_obj.clname     = "box_pdh";
    drop_elem_obj.collapse   = "#collapseParaPCI";
    drop_elem_list.push(drop_elem_obj);
    
    vm_obj.drop_elem_list    = drop_elem_list;
    vm_obj.showelem_callback = Show_Drop_Elem;
     
    //---------------drag---------------
    var drag_elem_list = [];
    //summary  
    var drag_elem_obj               = new Object();
    drag_elem_obj.clname            = "box_summary";
    drag_elem_obj.img               = "home.png";
    drag_elem_obj.diag_img          = "home16.png";
    drag_elem_obj.collapse          = null;
    drag_elem_obj.adddata_callback  = AddSummaryData;
    drag_elem_obj.showimg_callback  = ShowSummaryImage;
    drag_elem_obj.showdata_callback = ShowVmSummaryDiag;
    drag_elem_list.push(drag_elem_obj);
     
    //CPU  
    drag_elem_obj                   = new Object();
    drag_elem_obj.clname            = "box_cpu";
    drag_elem_obj.img               = "cpu.png";
    drag_elem_obj.diag_img          = "cpu_16.png";
    drag_elem_obj.collapse          = null;
    drag_elem_obj.adddata_callback  = AddCPUData; 
    drag_elem_obj.showimg_callback  = ShowCPUImage;
    drag_elem_obj.showdata_callback = ShowCPUDiag; 
    drag_elem_list.push(drag_elem_obj);
     
    //Memory  
    drag_elem_obj                   = new Object();
    drag_elem_obj.clname            = "box_memory";
    drag_elem_obj.img               = "memory.png";
    drag_elem_obj.diag_img          = "memory_16.png";
    drag_elem_obj.collapse          = null;
    drag_elem_obj.adddata_callback  = AddMemoryData; 
    drag_elem_obj.showimg_callback  = ShowMemoryImage;
    drag_elem_obj.showdata_callback = ShowMemoryDiag; 
    drag_elem_list.push(drag_elem_obj);
    
    //remote control
    drag_elem_obj                   = new Object();
    drag_elem_obj.clname            = "box_remotecontrol";
    drag_elem_obj.img               = "remote_desktop.png";
    drag_elem_obj.diag_img          = "remote_desktop_16.png";
    drag_elem_obj.collapse          = null;
    drag_elem_obj.adddata_callback  = AddRemoteDesktopData; 
    drag_elem_obj.showimg_callback  = ShowRemoteDesktopImage;
    drag_elem_obj.showdata_callback = ShowRemoteDesktopDiag; 
    drag_elem_list.push(drag_elem_obj);
    
    //image
    drag_elem_obj                   = new Object();
    drag_elem_obj.clname            = "box_image";
    drag_elem_obj.img               = "windows.png";
    drag_elem_obj.diag_img          = "windows_16.png";
    drag_elem_obj.collapse          = "#collapseImage";
    drag_elem_obj.adddata_callback  = AddMachineImageData;
    drag_elem_obj.showimg_callback  = ShowMachineImageImage;
    drag_elem_obj.showdata_callback = ShowMachineImageDiag;
    drag_elem_obj.deldata_callback  = DeleteImageData;
    drag_elem_list.push(drag_elem_obj);
    
    //IDE
    drag_elem_obj                   = new Object();
    drag_elem_obj.clname            = "box_ide";
    drag_elem_obj.img               = "param_harddisk.png";
    drag_elem_obj.diag_img          = "disk_16.png";
    drag_elem_obj.collapse          = "#collapseSotrageDevice";
    drag_elem_obj.adddata_callback  = AddDiskData;
    drag_elem_obj.showimg_callback  = ShowDiskImage;
    drag_elem_obj.adddefaultdata_callback = AddDefaultIDEHardDiskData;
    drag_elem_obj.showdata_callback = ShowDiskDiag;
    drag_elem_obj.initdata_callback = InitHardDiskData;
    drag_elem_obj.updatedatabyformdata_callback = UpdateHardDiskDataByForm;
    drag_elem_obj.deldata_callback  = DeleteDiskData;
    drag_elem_list.push(drag_elem_obj);
    
    //SCSI
    drag_elem_obj                   = new Object();
    drag_elem_obj.clname            = "box_scsi";
    drag_elem_obj.img               = "param_harddisk.png";
    drag_elem_obj.diag_img          = "disk_16.png";
    drag_elem_obj.collapse          = "#collapseSotrageDevice";
    drag_elem_obj.adddata_callback  = AddDiskData;
    drag_elem_obj.showimg_callback  = ShowDiskImage;
    drag_elem_obj.adddefaultdata_callback = AddDefaultSCSIHardDiskData;
    drag_elem_obj.showdata_callback = ShowDiskDiag;
    drag_elem_obj.initdata_callback = InitHardDiskData;
    drag_elem_obj.updatedatabyformdata_callback = UpdateHardDiskDataByForm;
    drag_elem_obj.deldata_callback  = DeleteDiskData;
    drag_elem_list.push(drag_elem_obj); 
     
    //CDROM
    drag_elem_obj                   = new Object();
    drag_elem_obj.clname            = "box_cdrom";
    drag_elem_obj.img               = "param_cd_drive.png";
    drag_elem_obj.diag_img          = "cdrom_16.png";
    drag_elem_obj.collapse          = "#collapseSotrageDevice";
    drag_elem_obj.adddata_callback  = AddDiskData;
    drag_elem_obj.showimg_callback  = ShowCDRomImage;
    drag_elem_obj.adddefaultdata_callback = AddDefaultCDROMData;
    drag_elem_obj.showdata_callback = ShowDiskDiag;
    drag_elem_obj.initdata_callback = InitCDROMData;
    drag_elem_obj.updatedatabyformdata_callback = UpdateCDROMDataByForm;
    drag_elem_obj.deldata_callback  = DeleteDiskData;
    drag_elem_list.push(drag_elem_obj);
   
    //Normal CONTEXT
    drag_elem_obj                   = new Object();
    drag_elem_obj.clname            = "box_noramlcontext";
    drag_elem_obj.img               = "directory.png";
    drag_elem_obj.diag_img          = "directory_16.png";
    drag_elem_obj.collapse          = "#collapseSotrageDevice";
    drag_elem_obj.adddata_callback  = AddNormalContextData;
    drag_elem_obj.showimg_callback  = ShowContextImage;
    drag_elem_obj.showdata_callback = ShowContextsDiag;
    drag_elem_obj.deldata_callback  = DeleteContextsData;
    drag_elem_list.push(drag_elem_obj);
    
    //V4 CONTEXT
    drag_elem_obj                   = new Object();
    drag_elem_obj.clname            = "box_v4context";
    drag_elem_obj.img               = "directory.png";
    drag_elem_obj.diag_img          = "directory_16.png";
    drag_elem_obj.collapse          = "#collapseSotrageDevice";
    drag_elem_obj.adddata_callback  = AddV4ContextData;
    drag_elem_obj.showimg_callback  = ShowContextImage;
    drag_elem_obj.showdata_callback = ShowContextsDiag;
    drag_elem_obj.deldata_callback  = DeleteContextsData;
    drag_elem_list.push(drag_elem_obj);
    
    //NORMAL NIC
    drag_elem_obj                   = new Object();
    drag_elem_obj.clname            = "box_normalnic";
    drag_elem_obj.img               = "param_nic.png";
    drag_elem_obj.diag_img          = "nic_16.png";
    drag_elem_obj.collapse          = "#collapseNic";
    drag_elem_obj.adddata_callback  = AddNicData;
    drag_elem_obj.showimg_callback  = ShowNormalNicImage;
    drag_elem_obj.adddefaultdata_callback = AddDefaultNormalNicData;
    drag_elem_obj.getindex_callback = GetIndexOfNormalNic;
    drag_elem_obj.showdata_callback = ShowNicDiag;
    drag_elem_obj.showdiagheader_callback = ShowNormalNicHeader;
    drag_elem_obj.initdata_callback = InitNoarmNicData;
    drag_elem_obj.updatedatabyformdata_callback = UpdateNormalNicDataByForm;
    drag_elem_obj.deldata_callback  = DeleteNicsData;
    drag_elem_list.push(drag_elem_obj); 
    
    //SRIOV NIC
    drag_elem_obj                   = new Object();
    drag_elem_obj.clname            = "box_sriovnic";
    drag_elem_obj.img               = "param_nic.png";
    drag_elem_obj.diag_img          = "nic_16.png";
    drag_elem_obj.collapse          = "#collapseNic";
    drag_elem_obj.adddata_callback  = AddNicData;
    drag_elem_obj.showimg_callback  = ShowSRIOVNicImage;
    drag_elem_obj.adddefaultdata_callback = AddDefaultSRIOVNicData;
    drag_elem_obj.getindex_callback = GetIndexOfSRIOVNic;
    drag_elem_obj.showdata_callback = ShowNicDiag; 
    drag_elem_obj.showdiagheader_callback = ShowSRIOVNicHeader;
    drag_elem_obj.initdata_callback = InitSRIOVNicData;
    drag_elem_obj.updatedatabyformdata_callback = UpdateSRIOVNicDataByForm;
    drag_elem_obj.deldata_callback  = DeleteNicsData;
    drag_elem_list.push(drag_elem_obj);
    
    //WATCH DOG
    drag_elem_obj                   = new Object();
    drag_elem_obj.clname            = "box_watchdog";
    drag_elem_obj.img               = "param_watch_dog.png";
    drag_elem_obj.diag_img          = "watch_dog_16.png";
    drag_elem_obj.collapse          = "#collapseNic";
    drag_elem_obj.adddata_callback  = AddWatchDogData;
    drag_elem_obj.showimg_callback  = ShowWatchDogImage;
    drag_elem_obj.showdata_callback = ShowWatchDogDiag;
    drag_elem_obj.deldata_callback  = DeleteWatchDogData;
    drag_elem_list.push(drag_elem_obj); 
    
    //PCI
    drag_elem_obj                   = new Object();
    drag_elem_obj.clname            = "box_pdh";
    drag_elem_obj.img               = "param_pdh.png";
    drag_elem_obj.diag_img          = "pdh_16.png";
    drag_elem_obj.collapse          = "#collapsePCI";
    drag_elem_obj.adddata_callback  = AddPDHData;
    drag_elem_obj.showimg_callback  = ShowPDHImage;
    drag_elem_obj.showdata_callback = ShowPDHDiag;
    drag_elem_obj.deldata_callback  = DeletePDHData;
    drag_elem_list.push(drag_elem_obj);
    
    vm_obj.drag_elem_list           = drag_elem_list;
    vm_obj.initdd_conn_callback     = Init_DragDrop_Connect;
    vm_obj.init_drag_elem_data_callback = Init_Drag_Elem_Data;
} 

function preview_click(){
    var diag = $("#idx_detailDlg");
    var title = '<i class="icon-zoom-in"></i> <span>' + page.res[page.pagelan].type.display["preview"] + '</span>'; 
    diag.find('div.modal-header h5').html(title);
    ShowVmInfoDiag(diag, global_vm_data, parseInt($("#txAction").val()));
}

function cancel_click(){
    if($("txLevel").val() == ""){
        return;
    } 
    var level = $("#txLevel").val();
    $('ul.breadcrumb li a:eq(' + level +')').click();
}

function CheckVmDataValid(vm_data){
    if(vm_data["cfg_info"]["base_info"]["machine"] == null){
        ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["vm_image_input_error"]);
        return false;
    }

    if( vm_data["cfg_info"]["base_info"]["hypervisor"] == "kvm" && vm_data["cfg_info"]["base_info"]["vnc_passwd"] == ""){
        ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["kvm_vnc_password_error"]);
        return false;
    }
    if( vm_data["cfg_info"]["base_info"]["hypervisor"] == "any" && vm_data["cfg_info"]["base_info"]["vnc_passwd"] == ""){
        ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["any_vnc_password_error"]);
        return false;
    }

    if(!CheckNicModelValid(vm_data["cfg_info"]["base_info"]["virt_type"], vm_data["cfg_info"]["base_info"]["nics"])){
        return false;
    }
    else{
        vm_data["cfg_info"]["base_info"]["nics"] = ReorderNics(vm_data["cfg_info"]["base_info"]["nics"]);
        
        //重新设置网卡index
    }
    return true;
}

function GetAddVmParam(VmData){
    var ConfigInfo          = new Object();
    ConfigInfo.base_info    = GetVmBaseInfo(VmData);
    ConfigInfo.vm_name      = VmData.cfg_info.vm_name; 
    ConfigInfo.project_name = VmData.cfg_info.project_name;
    ConfigInfo.base_info.machine.reserved_backup = VmData.cfg_info.base_info.reserved_backup;

    var AddVmData           = new Object();    
    AddVmData.cfg_info      = ConfigInfo;
    AddVmData.vm_num        = VmData.vm_num;

    return AddVmData;
}

function GetModifyVmParam(VmData){
    var ConfigInfo            = new Object();
    ConfigInfo.base_info    = GetVmBaseInfo(VmData);
    ConfigInfo.vm_name      = VmData.cfg_info.vm_name; 
    ConfigInfo.project_name = VmData.cfg_info.project_name;
    ConfigInfo.base_info.machine.reserved_backup = VmData.cfg_info.base_info.reserved_backup; 

    var ModifyVmData          = new Object();
    ModifyVmData.cfg_info     = ConfigInfo;
    ModifyVmData.vid          = VmData["vid"]; 

    return ModifyVmData;
}

function GetModifyDeployedVmParam(VmData){
    var VmCfgModifyElements     = new Object();
    VmCfgModifyElements.modifications = [];

    for (var i=0; i<VmData.Actions.length; i++){
        VmCfgModifyElements.modifications.push(VmData.Actions[i]);
    }

    return VmCfgModifyElements;
}

function GetAddVtParam(VtData){
    var ConfigInfo           = new Object();
    ConfigInfo.vm_name       = ""; //无用的参数,为了兼容vm
    ConfigInfo.base_info     = GetVmBaseInfo(VtData);
    ConfigInfo.base_info.machine.reserved_backup = VtData.cfg_info.base_info.reserved_backup;

    var AddVtData           = new Object();
    AddVtData.cfg_info      = ConfigInfo;
    AddVtData.vt_name       = VtData.vt_name;
    AddVtData.is_public     = VtData.is_public;

    return AddVtData;
}

function GetSetVtParam(VtData){
    var ConfigInfo       = new Object();
    ConfigInfo.vt_id     = VtData["vt_id"];
    ConfigInfo.base_info = GetVmBaseInfo(VtData);
    ConfigInfo.vm_name   = "";    //无用的参数,为了兼容vm
    ConfigInfo.base_info.machine.reserved_backup = VtData.cfg_info.base_info.reserved_backup;

    var SetVtData       = new Object();
    SetVtData.cfg_info  = ConfigInfo;
    SetVtData.vt_name   = VtData.vt_name;
    SetVtData.is_public = VtData.is_public;

    return SetVtData;
}

function Submit_CreateVm_Data(){
    return xmlrpc_service.tecs.vmcfg.allocate(xmlrpc_session, GetAddVmParam(global_vm_data));
}

function Submit_ModifyVm_Data(){
    return xmlrpc_service.tecs.vmcfg.set(xmlrpc_session,GetModifyVmParam(global_vm_data));
}

function Submit_CreateVt_Data(){
    return xmlrpc_service.tecs.vmtemplate.allocate(xmlrpc_session, GetAddVtParam(global_vm_data));
} 

function Submit_ModifyVt_Data(){
    return xmlrpc_service.tecs.vmtemplate.set(xmlrpc_session, GetSetVtParam(global_vm_data));
} 

function Submit_ModifyDeployedVm_Data(){
    //return xmlrpc_service.tecs.vmcfg.set(xmlrpc_session,GetModifyVmParam(global_vm_data));   

    return xmlrpc_service.tecs.vmcfg.modify(xmlrpc_session, GetModifyDeployedVmParam(global_vm_data));
}

function Submit_Data(vm_action){ 
    var submit_data_callback_list = [
        [global_createvm_acton,Submit_CreateVm_Data],
        [global_modifyvm_acton,Submit_ModifyVm_Data],
        [global_createvt_acton,Submit_CreateVt_Data], 
        [global_modifyvt_acton,Submit_ModifyVt_Data],
        [global_createvmbyvt_acton,Submit_CreateVm_Data],
        [global_modifydeplofyvm_acton,Submit_ModifyDeployedVm_Data]
    ]; 
    for(var i = 0;i < submit_data_callback_list.length;i++){
        var submit_data_callback = submit_data_callback_list[i];
        if(submit_data_callback[0] == vm_action){
            return submit_data_callback[1]();
        }
    }
} 

function submit_data_click(){
    var ret = false;
    //检查参数有效性
    ret = CheckVmDataValid(global_vm_data);
    if(!ret){
        return;
    } 
    var rs = null;
    var vm_action = parseInt($("#txAction").val());
    rs = Submit_Data(vm_action);
    
    if(rs[0] != 0){
        ShowNotyRpcErrorInfo(rs);
        return;
    }
    var breadcrumb_obj = vm_config_breadcrumb_list[1];
    Load_Page($('#content'), breadcrumb_obj.datavalue); 
    Switch_Vr_Page(breadcrumb_obj.datavalue2);
    //$('ul.breadcrumb li a:eq(1)').click();
    return;
}

function InitCreateVmData(){
    global_vm_data = new Object();
    global_vm_data.cfg_info = new Object();
    global_vm_data.cfg_info.vm_name = "vm";
    global_vm_data.cfg_info.base_info = new Object();
    global_vm_data.cfg_info.base_info.disks = new Array();
    global_vm_data.cfg_info.base_info.nics  = new Array();
    global_vm_data.cfg_info.base_info.qualifications  = new Array();
    global_vm_data.cfg_info.base_info.context  = new Object();
    global_vm_data.cfg_info.base_info.devices  = new Array();
    global_vm_data.cfg_info.base_info.vm_wdtime = 0;
    global_vm_data.cfg_info.base_info.enable_coredump = false;
    global_vm_data.cfg_info.base_info.enable_serial = false;
    global_vm_data.cfg_info.base_info.enable_livemigrate = true;
    global_vm_data.cfg_info.base_info.hypervisor = "xen";
    global_vm_data.cfg_info.base_info.reserved_backup  = 0;	
}

function InitModifyVmData(){
    var vid = $("#txVid").val();
    var vm_data = GetCurrentVmInfo(vid);
    if(vm_data == null){
        return;
    }

    var image_data = QueryImages(1 ,"", vm_data["cfg_info"]["base_info"]["machine"]["id"], "");
    if(image_data == null || image_data.length == 0){
         vm_data["cfg_info"]["base_info"]["machine"] = null;
    }
    else {
        var machine = image_data[0];
        machine["position"] = vm_data["cfg_info"]["base_info"]["machine"]["position"];
        machine["target"]   = vm_data["cfg_info"]["base_info"]["machine"]["target"];
		machine["reserved_backup"] = vm_data["cfg_info"]["base_info"]["machine"]["reserved_backup"];      	
        vm_data["cfg_info"]["base_info"]["machine"] = machine;        
    }
	
    vm_data["cfg_info"]["base_info"]["reserved_backup"] = vm_data["cfg_info"]["base_info"]["machine"]["reserved_backup"];  
    global_vm_data = vm_data;
    global_vm_data.Actions = [];
}

function InitCreateVtData(){
    global_vm_data = new Object();
    global_vm_data.cfg_info = new Object();
    global_vm_data.cfg_info.base_info = new Object();
    global_vm_data.cfg_info.base_info.disks = new Array();
    global_vm_data.cfg_info.base_info.nics  = new Array();
    global_vm_data.cfg_info.base_info.qualifications  = new Array();
    global_vm_data.cfg_info.base_info.context  = new Object();
    global_vm_data.cfg_info.base_info.devices  = new Array();
    global_vm_data.cfg_info.base_info.vm_wdtime = 0;
    global_vm_data.cfg_info.base_info.enable_coredump = false;
    global_vm_data.cfg_info.base_info.enable_serial = false;    
    global_vm_data.cfg_info.base_info.enable_livemigrate = true;
    global_vm_data.cfg_info.base_info.hypervisor = "xen";   
    global_vm_data.cfg_info.base_info.reserved_backup  = 0;	
}

function InitModifyVtData(){
    var vid = $("#txVid").val();
    var vm_data = GetCurrentVtInfo(vid);
    if(vm_data == null){
        return;
    }
    var image_data = QueryImages(1 ,"", vm_data["cfg_info"]["base_info"]["machine"]["id"], "");
    if(image_data == null || image_data.length == 0){
         vm_data["cfg_info"]["base_info"]["machine"] = null;
    }
    else {
        var machine = image_data[0];
        machine["position"] = vm_data["cfg_info"]["base_info"]["machine"]["position"];
        machine["target"]   = vm_data["cfg_info"]["base_info"]["machine"]["target"];
		machine["reserved_backup"] = vm_data["cfg_info"]["base_info"]["machine"]["reserved_backup"];  		
        vm_data["cfg_info"]["base_info"]["machine"] = machine;        
    }
    vm_data["cfg_info"]["base_info"]["reserved_backup"] = vm_data["cfg_info"]["base_info"]["machine"]["reserved_backup"];  
    global_vm_data = vm_data;
} 

function InitCreateVmByVtData(){
    var vid = $("#txVid").val();
    var vm_data = GetCurrentVtInfo(vid);
    if(vm_data == null){
        return;
    }
    var image_data = QueryImages(1 ,"", vm_data["cfg_info"]["base_info"]["machine"]["id"], "");
    if(image_data == null || image_data.length == 0){
         vm_data["cfg_info"]["base_info"]["machine"] = null;
    }
    else {
        var machine = image_data[0];
        machine["position"]  = vm_data["cfg_info"]["base_info"]["machine"]["position"];
        machine["target"]    = vm_data["cfg_info"]["base_info"]["machine"]["target"];
        machine["reserved_backup"]    = vm_data["cfg_info"]["base_info"]["machine"]["reserved_backup"];
        vm_data["cfg_info"]["base_info"]["machine"] = machine;
    }

    vm_data["cfg_info"]["vm_name"]   = "vm";
    var project_name     = QueryFirstPorjectsName(-5,"","");
    vm_data.cfg_info.project_name = project_name;
    vm_data.vm_num       = 1;
    vm_data.cfg_info.base_info.reserved_backup = machine["reserved_backup"];
    global_vm_data       = vm_data;
}

function Init_Vm_Data(vm_action){
    var init_vm_data_callback_list = [
        [global_createvm_acton,InitCreateVmData],
        [global_modifyvm_acton,InitModifyVmData],
        [global_createvt_acton,InitCreateVtData], 
        [global_modifyvt_acton,InitModifyVtData],
        [global_createvmbyvt_acton,InitCreateVmByVtData],
        [global_modifydeplofyvm_acton,InitModifyVmData]
    ]; 
    for(var i = 0;i < init_vm_data_callback_list.length;i++){
        var init_vm_data_callback = init_vm_data_callback_list[i];
        if(init_vm_data_callback[0] == vm_action){
            init_vm_data_callback[1]();
        }
    }
}

$(function(){
    var vm_action = parseInt($("#txAction").val());
    $('.modal').modal({
        backdrop:"static",
        keyboard:false,
        "show":false
    });
    Init_Vm_Data(vm_action);
    ShowBreadCrumb();
    ShowImage();
    InitDropObj();
    GetFirstLogicNetwork();
    vm_obj.showelem_callback();
    vm_obj.initdd_conn_callback();
    vm_obj.init_drag_elem_data_callback();
    $("#preview").click(preview_click);
    $("#cancel").click(cancel_click);
    $("#submit_data").click(submit_data_click);
    // OamUi静态显示部分初始化
    $("OamStaticUI").each(function(){
        SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
    });
});