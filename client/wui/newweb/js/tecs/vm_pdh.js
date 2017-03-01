function ShowPDHDiag(diag, droper, vm_data, action, drag_elem_obj){
    var info = '';
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["num"] +'</label>'
    info += '           <div class="controls">';
    info += '               <input type="text" class="OamUi" bindattr="val" bind="count" disabled>';
    info += '            </div>';
    info += '       </div>';
    
    info += '   </fieldset>';
    info += '</form>';
    SetVmParaHeader(drag_elem_obj.diag_img, "PDH", diag);
    diag.find('div.modal-body').html(info);
    InitPDHData(diag, vm_data);
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        UpdatePDHData(diag, droper, vm_data, drag_elem_obj);
    });
    $("input:checkbox, input:radio, input:file").not('[data-no-uniform="true"],#uniform-is-ajax').uniform();
    diag.modal("show");
}

function ShowPCIDeviceDetail(idx_pos,vm_data){
    var info = '';
    info += '<table class="table table-condensed">';
    info += '   <thead>';
    info += '       <tr>';
    info += '           <th width="30%">' + page.res[page.pagelan].type.field_name["name"] + '</th>';
    info += '           <th>' + page.res[page.pagelan].type.field_name["num"] + '</th>';
    info += '       </tr>';
    info += '   </thead>';
    info += '   <tbody>';
    for(var i = 0;i < vm_data["cfg_info"]["base_info"]["devices"].length;i++){
        var device = vm_data["cfg_info"]["base_info"]["devices"][i];
        info += '       <tr>';
        info += '           <td>PDH</td>';
        info += '           <td>' + device["count"] + '</td>';
        info += '       </tr>';
    } 
    info += '   </tbody>';
    info += '</table>';
    idx_pos.html(info);
}

function InitPDHData(diag, vm_data, drag_elem_obj){
    var index = GetIndexOfPDH(vm_data);
    SetModifyForm(diag.find(".form-horizontal"), vm_data["cfg_info"]["base_info"]["devices"][index]);
}

function UpdatePDHData(diag, droper, vm_data, drag_elem_obj){
    var index                              = GetIndexOfPDH(vm_data);
    var form_data                          = GetModifyForm(diag); 
    vm_data["cfg_info"]["base_info"]["devices"][index].count = parseInt(form_data["count"]);
    diag.modal("hide");
}

function GetIndexOfPDH(vm_data){
    for(var i = 0;i < vm_data["cfg_info"]["base_info"]["devices"].length;i++){
        var device = vm_data["cfg_info"]["base_info"]["devices"][i];
        if(device.type == 1){
            return i;
        }
    } 
    return null;
}

function AddPDHData(droper, drag_elem_obj, vm_data, action, diag, drager){
    var index = GetIndexOfPDH(vm_data);
    if(index != null){
        ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["vm_devicetype_same_exist"],"");
        return;
    }
    else{
        var device   = new Object();
        device.type  = 1;
        device.count = 1;
        vm_data["cfg_info"]["base_info"]["devices"].push(device);
        ShowPDHImage(droper, drag_elem_obj, vm_data);
        return;
    }
} 

function ShowPDHImage(droper, drag_elem_obj, vm_data){
    for(var i = 0;i < droper.length;i++){
        var droper_pos = droper.eq(i);
        var type = droper_pos.find("div h5").text();
        for(var j = 0;j < vm_data["cfg_info"]["base_info"]["devices"].length;j++){
            var device = vm_data["cfg_info"]["base_info"]["devices"][j];
            if(device.type == type){
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

function DeletePDHDataCallBack(diag, droper, vm_data, drag_elem_obj){
    var index = GetIndexOfPDH(vm_data); 
    vm_data["cfg_info"]["base_info"]["devices"].splice(index,1);
    droper.find(".box-content").find(".span12").html("");
    diag.modal("hide");
    droper.removeClass("configed");
}

function DeletePDHData(diag, droper, vm_data, drag_elem_obj){
    diag.find('div.modal-header h5').html(page.res[page.pagelan].type.display["confirm_op"]);
    diag.find('div.modal-body').html(page.res[page.pagelan].type.err_info["delete_confirm_info"]);
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        DeletePDHDataCallBack(diag, droper, vm_data, drag_elem_obj);
    });
    diag.modal("show");
}
