function ShowDiskDiag(diag, droper, vm_data, action, drag_elem_obj){
    var info = '';
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["type"] +'</label>'
    info += '           <div class="controls">';
    info += '               <label class="OamUi" bindattr="text" bind="type"></label>';
    info += '            </div>';
    info += '       </div>';
     
    info += '       <div class="control-group cdrom hide">';
    info += '           <label class="control-label">ID</label>'
    info += '           <div class="controls">';
    info += '               <select id="slimageid" class="OamUi" bindattr="val" bind="id"></select>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '       <div class="control-group disk hide disk_size">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["size"] + '<font color="red">*</font></label>'
    info += '           <div class="controls">';
    info += '               <input id="txSize" type="text" class="OamUi" bindattr="val" bind="size">';
    info += '               <span class="help-inline">' + page.res[page.pagelan].type.err_info["vm_disksize_desc"] + '</span>';
    info += '            </div>';
    info += '       </div>';
    
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["bus"] + '</label>'
    info += '           <div class="controls">';
    info += '               <label class="OamUi" bindattr="text" bind="bus"></label>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["position"] + '</label>'
    info += '           <div class="controls">';
    info += '               <select id="slposition" class="OamUi" bindattr="val" bind="position"></select>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '       <div class="control-group disk fstype hide">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["fstype"] + '</label>'
    info += '           <div class="controls">';
    info += '               <select id="slftype" class="OamUi" bindattr="val" bind="fstype"></select>';
    info += '            </div>';
    info += '       </div>';
    
    info += '   </fieldset>';
    info += '</form>'; 
    SetVmParaHeader(drag_elem_obj.diag_img, droper.find("div h5").text(), diag);
    diag.find('div.modal-body').html(info);
    InitSlPosition($("#slposition"));
    InitDiskFsType($("#slftype")); 
    drag_elem_obj.initdata_callback(diag, vm_data, droper.find("div h5").text());
    //if (userdata.role == 1){
    //    diag.find('.fstype').show();
    //}
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        UpdateDiskData(diag, droper, vm_data, drag_elem_obj);
    });
    $("input:checkbox, input:radio, input:file").not('[data-no-uniform="true"],#uniform-is-ajax').uniform();
    diag.modal("show");
}

function ShowDisksDetail(idx_pos,vm_data){
    var info = '';
    info += '<table class="table table-condensed">';
    info += '   <tbody>';
    for(var i = 0;i < vm_data["cfg_info"]["base_info"]["disks"].length;i++){
        var disk = vm_data["cfg_info"]["base_info"]["disks"][i];
        info += '    <tr>';
        info += '       <td width="30%">' + disk["target"] + '</td>'; 
        info += '       <td width="70%">';
        info += '           <table class="table table-condensed">'; 
        info += '               <tbody>';
        info += '                   <tr>';
        info += '                       <td width="40%">' + page.res[page.pagelan].type.field_name["type"] + '</td>';
        info += '                       <td width="60%">' + page.res[page.pagelan].type.DiskType[disk["type"]] + '</td>';
        info += '                   </tr>';
        info += '                   <tr>'; 
        if(disk["type"] == "disk"){
            info += '                   <td width="40%">' + page.res[page.pagelan].type.field_name["size"] + '</td>';
            info += '                   <td width="60%">' + getformatsize(disk["size"]) + '</td>';
        }
        else if(disk["type"] == "cdrom"){
            info += '                   <td width="40%">' + page.res[page.pagelan].type.field_name["image"] + '</td>';
            info += '                   <td width="60%">' + GetImageFullName(disk["id"]) + '</td>';
        }
        info += '                   </tr>'; 
        info += '                   <tr>';
        info += '                       <td width="40%">' + page.res[page.pagelan].type.field_name["bus"] + '</td>';
        info += '                       <td width="60%">' + disk["bus"] + '</td>';
        info += '                   </tr>'; 
        info += '                   <tr>';
        info += '                       <td width="40%">' + page.res[page.pagelan].type.field_name["position"] + '</td>';
        info += '                       <td width="60%">' + page.res[page.pagelan].type.DiskPosition[disk["position"]] + '</td>';
        info += '                   </tr>';
        if (disk["type"] != "cdrom"){
            info += '               <tr>';
            info += '                   <td width="40%">' + page.res[page.pagelan].type.field_name["fstype"] + '</td>';
            info += '                   <td width="60%">' + page.res[page.pagelan].type.DiskFormatType[disk["fstype"]] + '</td>';
            info += '               </tr>';
        }
        info += '               </tbody>';
        info += '           </table>';
        info += '       </td>';
        info += '   </tr>';
    } 
    info += '   </tbody>';
    info += '</table>';
    idx_pos.html(info);
}

function InitDiskData(diag, vm_data, target){
    var index = GetIndexOfDiskByTarget(vm_data, target);
    if(index != null){
        SetModifyForm(diag.find(".form-horizontal"), vm_data["cfg_info"]["base_info"]["disks"][index]);
    }
}

function InitCDROMData(diag, vm_data, target){
    for(var j = 0;j < vm_data["cfg_info"]["base_info"]["disks"].length;j++){
        var disk = vm_data["cfg_info"]["base_info"]["disks"][j];
        if(disk.target == target && disk.type == "cdrom" ){
            diag.find('div.modal-body').find(".cdrom").show();
            diag.find('div.modal-body').find(".disk").hide();
            InitSlCDROMImage($("#slimageid"));
            InitDiskData(diag, vm_data, target);
        }
    }
}

function InitHardDiskData(diag, vm_data, target){ 
    diag.find('div.modal-body').find(".cdrom").hide();
    diag.find('div.modal-body').find(".disk").show();
    InitDiskData(diag, vm_data, target); 
    var size = Get_Assignedformat_Size(diag.find("#txSize").val(), "GB");
    diag.find("#txSize").val(size);
}

function InitSlCDROMImage(slimage){ 
    slimage.empty();
    var rs = QueryImages(-3,"",-1);
    if(rs == null || rs.length == 0){
        return;
    } 
    for(var i = 0;i < rs.length;i++){
        var imageinfo = rs[i];
        if(imageinfo["is_enabled"] && imageinfo["type"] == "cdrom"){  
            var title = imageinfo["file"] + "(" + page.res[page.pagelan].type.field_name["creater"] + ":" + imageinfo["user_name"] + ")";
            slimage.append("<option value=" + imageinfo["id"].toString() + ">" + title + "</option>");
        }
    }
    return;
}

function GetFirstCDROMImage(){
    var rs = QueryImages(-3,"",-1);
    if(rs == null || rs.length == 0){
        return null;
    } 
    for(var i = 0;i < rs.length;i++){
        var imageinfo = rs[i];
        if(imageinfo["is_enabled"] && imageinfo["type"] == "cdrom"){  
            return imageinfo;
        }
    }
    return null;
}

function UpdateDiskData(diag, droper, vm_data, drag_elem_obj){
    var ret = drag_elem_obj.updatedatabyformdata_callback(diag.find(".form-horizontal"), vm_data, droper.find("div h5").text());
    if(ret){
        diag.modal("hide");
    }
}

function CheckHardDiskValid(frm){
    var form_data = GetModifyForm(frm);
    frm.find("fieldset div").removeClass("error");
    if(form_data["size"] == "" || isNaN(form_data["size"])){
        frm.find(".disk_size").addClass("error");
        return false;
    }
    return true;
}

function UpdateHardDiskDataByForm(frm, vm_data, target){
    if(!CheckHardDiskValid(frm)){
        return false;
    }
    var index = GetIndexOfDiskByTarget(vm_data, target); 
    if(index != null){
        var form_data                                    = GetModifyForm(frm);
        var size=form_data["size"] * 1024 * 1024 * 1024;
        size = String(size);
        size = parseInt(size);
        vm_data["cfg_info"]["base_info"]["disks"][index]["size"]     = new I8(size);  
        vm_data["cfg_info"]["base_info"]["disks"][index]["id"]       = new I8(-1);
        vm_data["cfg_info"]["base_info"]["disks"][index]["position"] = parseInt(form_data["position"]);
        vm_data["cfg_info"]["base_info"]["disks"][index]["fstype"]   = form_data["fstype"];
        return true;
    }
    return false;
}

function UpdateCDROMDataByForm(frm, vm_data, target){
    var index = GetIndexOfDiskByTarget(vm_data, target); 
    if(index != null){
        var form_data                                    = GetModifyForm(frm);
        vm_data["cfg_info"]["base_info"]["disks"][index]["size"]     = new I8(0);
        vm_data["cfg_info"]["base_info"]["disks"][index]["id"]       = new I8(form_data["id"]);
        vm_data["cfg_info"]["base_info"]["disks"][index]["position"] = parseInt(form_data["position"]);
        vm_data["cfg_info"]["base_info"]["disks"][index]["fstype"]   = form_data["fstype"];
        return true;
    }
    return false;
}

function GetIndexOfDiskByTarget(vm_data,target){
    for(var i = 0;i < vm_data["cfg_info"]["base_info"]["disks"].length;i++){
        var disk = vm_data["cfg_info"]["base_info"]["disks"][i];
        if(disk["target"] == target){
            return i;
        }
    }
    return null;
} 

function AddDefaultCDROMData(target,vm_data){
    var disk           = new Object(); 
    var imageinfo      = GetFirstCDROMImage();
    if(!imageinfo){
        var err1 = page.res[page.pagelan].type.err_info["vm_nocd-romimage_forcreatevm"];
        var err2 = '<button class="btn" onclick="Upload_Image()"><i class="icon-circle-arrow-up"></i><span>' + page.res[page.pagelan].type.opr["upload"] + '</span></button>';
        ShowBottomNotyErrorInfo(err1, err2);
        return false;
    } 
    disk.id            = imageinfo["id"];
    disk.size          = new I8(0);
    disk.type          = "cdrom";
    disk.bus           = imageinfo["bus"];
    disk.position      = 1;
    disk.target        = target;
    disk.fstype        = "";
    vm_data["cfg_info"]["base_info"]["disks"].push(disk);
    return true;
}

function AddDefaultIDEHardDiskData(target,vm_data){
    var disk           = new Object();
    disk.id            = new I8(-1);
    disk.size          = new I8(10 * 1024 * 1024 * 1024);
    disk.type          = "disk";
    disk.bus           = "ide";
    disk.position      = 1;
    disk.target        = target;
    disk.fstype        = "";
    vm_data["cfg_info"]["base_info"]["disks"].push(disk);
    return true;
}

function AddDefaultSCSIHardDiskData(target,vm_data){
    var disk           = new Object();
    disk.id            = new I8(-1);
    disk.size          = new I8(10 * 1024 * 1024 * 1024);
    disk.type          = "disk";
    disk.bus           = "scsi";
    disk.position      = 1;
    disk.target        = target;
    disk.fstype        = "";
    vm_data["cfg_info"]["base_info"]["disks"].push(disk);
    return true;
}

function AddDiskData(droper, drag_elem_obj, vm_data, vm_action, diag, drager){
    var ret = drag_elem_obj.adddefaultdata_callback(droper.find("div h5").text(), vm_data);
    if(!ret){
        return;
    }
    drag_elem_obj.showimg_callback(droper, drag_elem_obj, vm_data);
    return;
} 

function ShowDiskImage(droper, drag_elem_obj, vm_data){
    for(var i = 0;i < droper.length;i++){
        var droper_pos = droper.eq(i);
        var target = droper_pos.find("div h5").text();
        for(var j = 0;j < vm_data["cfg_info"]["base_info"]["disks"].length;j++){
            var disk = vm_data["cfg_info"]["base_info"]["disks"][j];
            if(disk.target == target && disk.type == "disk" ){
                var info = '<span class="span12"><img src="img/' + drag_elem_obj.img + '"/></span>';  
                var class_name = drag_elem_obj.clname; 
                info += '<span class="pull-right icon-trash ' + class_name + '" style="cursor:pointer;"></span>'; 
                droper_pos.find(".box-content").find(".span12").html(info); 
                droper_pos.addClass(class_name).addClass("configed");
                droper.data("data", drag_elem_obj);
            }
        }
    }
}

function ShowCDRomImage(droper, drag_elem_obj, vm_data){
    for(var i = 0;i < droper.length;i++){
        var droper_pos = droper.eq(i);
        var target = droper_pos.find("div h5").text();
        for(var j = 0;j < vm_data["cfg_info"]["base_info"]["disks"].length;j++){
            var disk = vm_data["cfg_info"]["base_info"]["disks"][j];
            if(disk.target == target && disk.type == "cdrom"){
                var info = '<span class="span12"><img src="img/' + drag_elem_obj.img + '"/></span>';  
                var class_name = drag_elem_obj.clname; 
                info += '<span class="pull-right icon-trash ' + class_name + '" style="cursor:pointer;"></span>'; 
                droper_pos.find(".box-content").find(".span12").html(info); 
                droper_pos.addClass(class_name).addClass("configed");
                droper.data("data", drag_elem_obj);
            }
        }
    }
}

function DeleteDiskDataCallBack(diag, droper, vm_data){ 
    var index = GetIndexOfDiskByTarget(vm_data, droper.find("div h5").text());
    if(index != null){
        vm_data["cfg_info"]["base_info"]["disks"].splice(index,1);
    }
    droper.find(".box-content").find(".span12").html("");
    droper.removeClass("configed");
    diag.modal("hide");
}

function DeleteDiskData(diag, droper, vm_data){
    diag.find('div.modal-header h5').html(page.res[page.pagelan].type.display["confirm_op"]);
    diag.find('div.modal-body').html(page.res[page.pagelan].type.err_info["delete_confirm_info"]);
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        DeleteDiskDataCallBack(diag, droper, vm_data);
    });
    diag.modal("show");
}
