function ShowMachineImageDiag(diag, droper, vm_data, vm_action, drag_elem_obj){
    var info = '';
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    info += '       <div class="control-group">';
    info += '           <label class="control-label">ID</label>'
    info += '           <div class="controls">';
    info += '               <label class="OamUi" bindattr="text" bind="id"></label>';
    info += '            </div>';
    info += '       </div>';
     
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["filename"] + '</label>'
    info += '           <div class="controls">';
    info += '               <label class="OamUi" bindattr="text" bind="file"></label>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["register_time"] + '</label>'
    info += '           <div class="controls">';
    info += '               <label class="OamUi" bindattr="text" bind="register_time"></label>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["type"] + '</label>'
    info += '           <div class="controls">';
    info += '               <label class="OamUi" bindattr="text" bind="type"></label>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["os_type"] + '</label>'
    info += '           <div class="controls">';
    info +=             ShowOSIcon(vm_data.cfg_info.base_info.machine.os_type);
    info += '           <label class="OamUi" bindattr="text" bind="os_type"></label>';
    info += '            </div>';
    info += '       </div>'; 

    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["size"] + '</label>'
    info += '           <div class="controls">';
    info += '               <label class="OamUi" bindattr="text" bind="size" id="lbSize"></label>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["running_vms"] + '</label>'
    info += '           <div class="controls">';
    info += '               <label class="OamUi" bindattr="text" bind="running_vms"></label>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["arch"] + '</label>'
    info += '           <div class="controls">';
    info += '               <label class="OamUi" bindattr="text" bind="arch"></label>';
    info += '            </div>';
    info += '       </div>';  
    
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["bus"] + '</label>'
    info += '           <div class="controls">';
    info += '               <label class="OamUi" bindattr="text" bind="bus"></label>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["MD5"] + '</label>'
    info += '           <div class="controls">';
    info += '               <label class="OamUi" bindattr="text" bind="md5sum"></label>';
    info += '            </div>';
    info += '       </div>';
     
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["desc"] + '</label>'
    info += '           <div class="controls">';
    info += '               <label class="OamUi" bindattr="text" bind="description"></label>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["disk_format"] + '</label>'
    info += '           <div class="controls">';
    info += '               <label class="OamUi" bindattr="text" bind="disk_format"></label>';
    info += '            </div>';
    info += '       </div>'; 

    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["position"] + '</label>'
    info += '           <div class="controls">';
    info += '               <select id="slposition" class="OamUi" bindattr="val" bind="position"></select>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '   </fieldset>';
    info += '</form>';
    SetVmParaHeader(drag_elem_obj.diag_img, droper.find("div h5").text(), diag);
    diag.find('div.modal-body').html(info);
    InitSlPosition($("#slposition"));
    InitImageData(diag,vm_data);
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        UpdateImageData(diag, droper, vm_data);
    });
    $("input:checkbox, input:radio, input:file").not('[data-no-uniform="true"],#uniform-is-ajax').uniform();
    diag.modal("show");
}

function ShowModifyImageDiag(diag, img_id, func_callback){
    var info = '';
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["container_format"] +  '</label>'
    info += '           <div class="controls">';
    info += '               <Select id="slContainerFormat" class="OamUi" bindattr="val" bind="container_format"></Select>';
    info += '            </div>';
    info += '       </div>'; 

    info += '       <div class="control-group dv_vm_image dv_image_type">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["type"] +  '</label>'
    info += '           <div class="controls">';
    info += '               <Select id="slImageType" class="OamUi" bindattr="val" bind="type"></Select>';
    info += '            </div>';
    info += '       </div>';
      
    info += '       <div class="control-group dv_vm_image dv_image_os_type">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["os_type"] +  '</label>'
    info += '           <div class="controls">';
    info += '               <Select id="slImageOSType" class="OamUi" bindattr="val" bind="os_type"></Select>';
    info += '            </div>';
    info += '       </div>';

    info += '       <div class="control-group dv_vm_image dv_image_bus">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["bus"] +  '</label>'
    info += '           <div class="controls">';
    info += '               <Select id="slImageBus" class="OamUi" bindattr="val" bind="bus"></Select>';
    info += '            </div>';
    info += '       </div>';
     
    info += '       <div class="control-group dv_vm_image dv_image_arch">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["arch"] +  '</label>'
    info += '           <div class="controls">';
    info += '               <Select id="slImageArch" class="OamUi" bindattr="val" bind="arch"></Select>';
    info += '            </div>';
    info += '       </div>';
    
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["desc"] +  '</label>'
    info += '           <div class="controls">';
    info += '               <textarea class="cleditor OamUi" id="taSetDesc" maxlength="128" rows="6" bindattr="val" bind="description"></textarea><p id="pSetImageInfo"></p>';
    info += '            </div>';
    info += '       </div>';
    
    info += '   </fieldset>';
    info += '</form>';
    
    var header = '<i class="icon-edit"></i>&nbsp;' + page.res[page.pagelan].type.opr["modify"] + '-' + img_id;
    diag.find('div.modal-header h5').html(header);
    diag.find('div.modal-body').html(info);
    
    for(var p in page.res[page.pagelan].type.ContainerFormat){
        $("#slContainerFormat").append("<option value=" + p + ">" + page.res[page.pagelan].type.ContainerFormat[p] + "</option>");
    }

    for(var p in page.res[page.pagelan].type.ImageType){
        $("#slImageType").append("<option value=" + p + ">" + page.res[page.pagelan].type.ImageType[p] + "</option>");
    } 
    
    for(var p in page.res[page.pagelan].type.ImageOSType){
        $("#slImageOSType").append("<option value='" + p + "'>" + page.res[page.pagelan].type.ImageOSType[p] + "</option>");
    } 

    for(var p in page.res[page.pagelan].type.BusType){
        $("#slImageBus").append("<option value=" + p + ">" + page.res[page.pagelan].type.BusType[p] + "</option>");
    }
     
    for(var p in page.res[page.pagelan].type.ArchType){
        $("#slImageArch").append("<option value=" + p + ">" + page.res[page.pagelan].type.ArchType[p] + "</option>");
    }
    LimitCharLenInput($("#taSetDesc"),$("#pSetImageInfo")); 
    
    var rs = QueryImages(1, "", img_id, "");
    if(rs == null && rs.length == 0){
        return;
    }
    imageinfo = rs[0];
    SetModifyForm(diag.find("form.form-horizontal"), imageinfo);
    
    $('#slContainerFormat').unbind('change').bind('change',function(){
        var attr = $("#slContainerFormat").attr("value");
        if(attr == ""){
            $(".dv_vm_image").show();
        }
        else{
            $(".dv_vm_image").hide();
        }
    });
    $('#slContainerFormat').change();

    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        var item = GetModifyForm(diag.find("form.form-horizontal"));
        var type = "";
        var arch = "" ;        
        var bus = ""; 
        var os_type = "";
        if(item.container_format == ""){
            type = item.type;
            diag.find(".dv_image_type").removeClass("error");
            if(type == null || type == ""){
                diag.find(".dv_image_type").addClass("error");
                return;
            }
            os_type = item.os_type;
            diag.find(".dv_image_os_type").removeClass("error");
            if(os_type == null || os_type == ""){
                diag.find(".dv_image_os_type").addClass("error");
                return;
            }
            bus = item.bus;
            diag.find(".dv_image_bus").removeClass("error");
            if(bus == null || bus == ""){
                diag.find(".dv_image_bus").addClass("error");
                return;
            }
            arch = item.arch;
            diag.find(".dv_image_arch").removeClass("error");
            if(arch == null || arch == ""){
                diag.find(".dv_image_arch").addClass("error");
                return;
            }
        } 
        else{
            type = imageinfo["type"];
            arch = imageinfo["arch"];
            bus = imageinfo["bus"];
            os_type = imageinfo["os_type"];
        }
        var rs = xmlrpc_service.tecs.image["set"](xmlrpc_session, 
                                                    imageinfo["id"],
                                                    imageinfo["name"],
                                                    type,
                                                    arch,
                                                    bus,
                                                    item.description,
                                                    os_type,
                                                    item.container_format);        
        if(rs[0] != 0){
            ShowNotyRpcErrorInfo(rs);  
            return false;
        }
        diag.modal("hide");
        func_callback();
        return true;
    });
    diag.modal("show");
}

function GetImageDetailHtmlInfo(img){
    var info = '';
    info += '       <tr>';
    info += '           <td width="30%">ID</td>';
    info += '           <td>' + img["id"] + '</td>';
    info += '       </tr>';
    
    info += '       <tr>';
    info += '           <td width="30%">' + page.res[page.pagelan].type.field_name["filename"] + '</td>';
    info += '           <td>' + img["file"] + '</td>';
    info += '       </tr>'; 
    
    info += '       <tr>';
    info += '           <td width="30%">' + page.res[page.pagelan].type.field_name["register_time"] + '</td>';
    info += '           <td>' + getLocalTime(img["register_time"] )+ '</td>';
    info += '       </tr>';
    
    info += '       <tr>';
    info += '           <td width="30%">' + page.res[page.pagelan].type.field_name["status"] + '</td>';
    info += '           <td>' + GetStatusOfRes(img["is_enabled"]) + '</td>';
    info += '       </tr>';  
    
    info += '       <tr>';
    info += '           <td width="30%">' + page.res[page.pagelan].type.field_name["ispublish"] + '</td>';
    info += '           <td>' + GetBool2Res(img["is_public"]) + '</td>';
    info += '       </tr>'; 
    
    info += '       <tr>';
    info += '           <td width="30%">' + page.res[page.pagelan].type.field_name["container_format"] + '</td>';
    info += '           <td>' + page.res[page.pagelan].type.ContainerFormat[img["container_format"]] + '</td>';
    info += '       </tr>';
     
    if(img["container_format"] == ""){
        info += '       <tr>';
        info += '           <td width="30%">' + page.res[page.pagelan].type.field_name["type"] + '</td>';
        info += '           <td>' + img["type"] + '</td>';
        info += '       </tr>';
         
        info += '       <tr>';
        info += '           <td width="30%">' + page.res[page.pagelan].type.field_name["os_type"] + '</td>';
        info += '           <td>'  +ShowOSIcon(img["os_type"])+ img["os_type"] + '</td>';
        info += '       </tr>';
         
        info += '       <tr>';
        info += '           <td width="30%">' + page.res[page.pagelan].type.field_name["bus"] + '</td>';
        info += '           <td>' + img["bus"] + '</td>';
        info += '       </tr>';
    
        info += '       <tr>';
        info += '           <td width="30%">' + page.res[page.pagelan].type.field_name["arch"] + '</td>';
        info += '           <td>' + img["arch"] + '</td>';
        info += '       </tr>';
    }

    info += '       <tr>';
    info += '           <td width="30%">' + page.res[page.pagelan].type.field_name["size"] + '</td>';
    info += '           <td>' + getformatsize(img["size"]) + '</td>';
    info += '       </tr>'; 
    
    info += '       <tr>';
    info += '           <td width="30%">' + page.res[page.pagelan].type.field_name["running_vms"] + '</td>';
    info += '           <td>' + img["running_vms"] + '</td>';
    info += '       </tr>'; 
    
    info += '       <tr>';
    info += '           <td width="30%">' + page.res[page.pagelan].type.field_name["MD5"] + '</td>';
    info += '           <td>' + img["md5sum"] + '</td>';
    info += '       </tr>';    
    
    info += '       <tr>';
    info += '           <td width="30%">' + page.res[page.pagelan].type.field_name["disk_size"] + '</td>';
    info += '           <td>' +getformatsize( img["disk_size"] )+ '</td>';
    info += '       </tr>';  

    info += '       <tr>';
    info += '           <td width="30%">' + page.res[page.pagelan].type.field_name["disk_format"] + '</td>';
    info += '           <td>' + img["disk_format"] + '</td>';
    info += '       </tr>'; 
    
    info += '       <tr>';
    info += '           <td width="30%">' + page.res[page.pagelan].type.field_name["desc"] + '</td>';
    info += '           <td>' + img["description"] + '</td>';
    info += '       </tr>';
     
    return info;
}

function ShowOSIcon(os_type){
	var img='';
    if("" == os_type){
	
	img += '<img  src="./img/unknown.png">';
    }
    else{
	img += '<img  src="./img/' + os_type + '.png">';
    }
	return img;
}

function ShowImageDetail(diag, img_id){
    var info = '';
    var rs = QueryImages(1, "",new I8(img_id), "");
    if(rs == null || rs.length == 0){
        return;
    } 
    info += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
    info += '   <tbody>';
    info += GetImageDetailHtmlInfo(rs[0]);
    
    info += '   </tbody>';
    info += '</table>';
    
    diag.find("div.modal-body").html(info);
    diag.modal("show"); 
    var header = '<i class="icon-zoom-in"></i>&nbsp;' + page.res[page.pagelan].type.display["detail"];
    diag.find('div.modal-header h5').html(header);
}

function ShowMachineImageDetail(idx_pos,vm_data){
    var info = '';
    if(vm_data["cfg_info"]["base_info"]["machine"] == null){
        return;
    } 
    var rs = QueryImages(1 ,"", new I8(vm_data["cfg_info"]["base_info"]["machine"]["id"].toString()), "");
    if(rs == null || rs.length == 0){
         return;
    }
    var machine = rs[0];
    machine["position"] = vm_data["cfg_info"]["base_info"]["machine"]["position"];
    machine["target"]   = vm_data["cfg_info"]["base_info"]["machine"]["target"];
    info += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
    info += '   <tbody>';
    info += GetImageDetailHtmlInfo(machine);
     
    info += '       <tr>';
    info += '           <td width="30%">' + page.res[page.pagelan].type.field_name["target"] + '</td>';
    info += '           <td>' + machine["target"] + '</td>';
    info += '       </tr>';
    
    info += '       <tr>';
    info += '           <td width="30%">' + page.res[page.pagelan].type.field_name["position"] + '</td>';
    info += '           <td>' + page.res[page.pagelan].type.DiskPosition[machine["position"]] + '</td>';
    info += '       </tr>';  
    
    info += '   </tbody>';
    info += '</table>';
    
    idx_pos.html(info);
}

function GetStatusOfRes(status){
    if(status){
         return page.res[page.pagelan].type.opr["enable"];
    }
    return page.res[page.pagelan].type.opr["disable"];
}

function InitImageData(diag,vm_data){
    SetModifyForm(diag.find(".form-horizontal"), vm_data.cfg_info.base_info.machine);
    $("#lbSize").text(getformatsize($("#lbSize").text())); 
}

function GetImageDataByForm(frm,vm_data){
    var form_data                               = GetModifyForm(frm);
    vm_data["cfg_info"]["base_info"]["machine"]["position"] = parseInt(form_data["position"]);
}

function UpdateImageData(diag, droper, vm_data){
    GetImageDataByForm(diag.find(".form-horizontal"), vm_data);
    diag.modal("hide");
}

function GetDefaultImageData(vm_data, droper){
    vm_data["cfg_info"]["base_info"]["machine"]["position"] = 1;
    vm_data["cfg_info"]["base_info"]["machine"]["target"]   = droper.find("div h5").text();
} 

function AddMachineImageData(droper, drag_elem_obj, vm_data, vm_action, diag, drager){
    var machine = new Object(); 
    machine = drager.data("data");
    vm_data["cfg_info"]["base_info"]["machine"] = machine;
    vm_data["cfg_info"]["base_info"]["machine"]["position"] = 1;
    vm_data["cfg_info"]["base_info"]["machine"]["target"]   = "hda";
    ShowMachineImageImage(droper, drag_elem_obj, vm_data);

    var AddImageAction        = new Object();
    AddImageAction.vid        = vm_data["vid"];
    AddImageAction.operation  = VM_INSERT_ITEM;
    AddImageAction.element    = VM_ITEM_IMAGE;
    AddImageAction.args       = new Object();
    AddImageAction.args.id    = vm_data["cfg_info"]["base_info"]["machine"]["id"].toString();
    AddImageAction.args.position = vm_data["cfg_info"]["base_info"]["machine"]["position"].toString();
    AddImageAction.args.size     = vm_data["cfg_info"]["base_info"]["machine"]["size"].toString();
    AddImageAction.args.bus      = vm_data["cfg_info"]["base_info"]["machine"]["bus"].toString();
    AddImageAction.args.type     = vm_data["cfg_info"]["base_info"]["machine"]["type"].toString();
    AddImageAction.args.target   = vm_data["cfg_info"]["base_info"]["machine"]["target"].toString();
    AddImageAction.args.reserved_backup = vm_data["cfg_info"]["base_info"]["machine"]["reserved_backup"].toString();

    if(vm_data.Actions != null)
        vm_data.Actions.push(AddImageAction);    

    return;
}

function ShowMachineImageImage(droper, drag_elem_obj, vm_data){
    var class_name = drag_elem_obj.clname; 
    if(vm_data["cfg_info"]["base_info"]["machine"] == null){
        return;
    }
    var info = '<span class="span12">' + ShowOSIcon(vm_data.cfg_info.base_info.machine.os_type) + '</span>';   
    info += '<span class="pull-right icon-trash ' + class_name + '" style="cursor:pointer;"></span>';
    droper.find(".box-content").find(".span12").html(info);
    droper.addClass(class_name).addClass("configed");
    droper.data("data", drag_elem_obj);
    return;
}

function DeleteImageDataCallBack(diag, droper, vm_data){
    var DelImageAction        = new Object();
    DelImageAction.vid        = vm_data["vid"];
    DelImageAction.operation  = VM_INSERT_ITEM;
    DelImageAction.element    = VM_ITEM_IMAGE;
    DelImageAction.args       = new Object();
    DelImageAction.args.id    =  vm_data["cfg_info"]["base_info"]["machine"]["id"].toString();
    DelImageAction.args.position = vm_data["cfg_info"]["base_info"]["machine"]["position"].toString();
    DelImageAction.args.size     = vm_data["cfg_info"]["base_info"]["machine"]["size"].toString();
    DelImageAction.args.bus      = vm_data["cfg_info"]["base_info"]["machine"]["bus"].toString();
    DelImageAction.args.type     = vm_data["cfg_info"]["base_info"]["machine"]["type"].toString();
    DelImageAction.args.target   = vm_data["cfg_info"]["base_info"]["machine"]["target"].toString();
    DelImageAction.args.reserved_backup = vm_data["cfg_info"]["base_info"]["machine"]["reserved_backup"].toString();

    if(vm_data.Actions != null)
        vm_data.Actions.push(DelImageAction);   

    if(vm_data["cfg_info"]["base_info"]["machine"] != null){
        delete vm_data["cfg_info"]["base_info"]["machine"];
    } 
    droper.find(".box-content").find(".span12").html("");
    droper.removeClass("configed");
    diag.modal("hide");
}

function DeleteImageData(diag, droper, vm_data){
    diag.find('div.modal-header h5').html(page.res[page.pagelan].type.display["confirm_op"]);
    diag.find('div.modal-body').html(page.res[page.pagelan].type.err_info["delete_confirm_info"]);
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        DeleteImageDataCallBack(diag, droper, vm_data);
    });
    diag.modal("show");
}
