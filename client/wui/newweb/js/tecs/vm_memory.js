function ShowMemoryDiag(diag, droper, vm_data, vm_action, drag_elem_obj){
    var info = '';
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    info += '       <div class="control-group memory">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["memory"] + '<font color="red">*</font></label>';
    info += '           <div class="controls">';
    info += '               <input id="txMemory" type="text" class="OamUi" bindattr="val" bind="base_info.memory">';
    info += '               <span class="help-inline">' + page.res[page.pagelan].type.err_info["vm_memory_desc"] + '</span>';
    info += '            </div>';
    info += '       </div>';
    
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.display["unit"] + '</label>';
    info += '           <div class="controls">';
    info += '               <label class="radio">';
    info += '                   <input type="radio" name="rdunit" id="rdGB" value="option1" checked="">GB';
    info += '               </label>';
    info += '               <div style="clear:both"></div>';
    info += '               <label class="radio">';
    info += '                   <input type="radio" name="rdunit" id="rdMB" value="option2">MB';
    info += '               </label>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '   </fieldset>';
    info += '</form>';
    SetVmParaHeader(drag_elem_obj.diag_img, droper.find("div h5").text(), diag);
    diag.find('div.modal-body').html(info);
    InitMemoryData(diag,vm_data);
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        UpdateMemoryData(diag, droper, vm_data);
    });
    $("input:checkbox, input:radio, input:file").not('[data-no-uniform="true"],#uniform-is-ajax').uniform();
    diag.modal("show");
}

function ShowMemoryDetail(idx_pos,vm_data){
    var info = '';
    info += '<table class="table table-condensed">';
    info += '   <tbody>';
    info += '       <tr>';
    info += '           <td width="30%">' + page.res[page.pagelan].type.field_name["memory"] + '</td>';
    info += '           <td>' + getformatsize(vm_data["cfg_info"]["base_info"]["memory"]) + '</td>';
    info += '       </tr>';
    
    info += '   </tbody>';
    info += '</table>';
    
    idx_pos.html(info);
}

function InitMemoryData(diag,vm_data){
    var unit = "";
    if(vm_data["cfg_info"]["base_info"]["memory"] >= 1024 * 1024 * 1024){
        $("#rdGB").attr("checked", "checked");
    }
    else{
        $("#rdMB").attr("checked", "checked");
    }
    if($("#rdGB").attr("checked") == "checked"){
        unit = "GB";
    }
    else{
        unit = "MB";
    }
    var size = Get_Assignedformat_Size(vm_data["cfg_info"]["base_info"]["memory"], unit);
    $("#txMemory").val(size);
}

function GetMemoryDataByForm(frm,vm_data){
    var form_data                     = GetModifyForm(frm);
    if($("#rdGB").attr("checked") == "checked"){
        form_data["base_info.memory"] = form_data["base_info.memory"] * 1024 * 1024 * 1024;
    }
    else{
        form_data["base_info.memory"] = form_data["base_info.memory"] * 1024 * 1024;
    }
    form_data["base_info.memory"] = form_data["base_info.memory"].toString().split(".")[0];
    vm_data.cfg_info.base_info.memory          = new I8(form_data["base_info.memory"]);

    var UpdateMemAction        = new Object();
    UpdateMemAction.vid        = vm_data["vid"];
    UpdateMemAction.operation  = VM_UPDATE_ITEM;
    UpdateMemAction.element    = VM_ITEM_MEM;
    UpdateMemAction.args       = new Object();
    UpdateMemAction.args.memory = vm_data.cfg_info.base_info.memory.toString();
    
    if(vm_data.Actions != null)
        vm_data.Actions.push(UpdateMemAction);    
}

function CheckVmMemoryDataValid(frm){    
    var form_data = GetModifyForm(frm);    
    var size;
    frm.find("fieldset div").removeClass("error");
    if(form_data["base_info.memory"] == ""){
        frm.find(".memory").addClass("error");
        return false;
    }
    
    if($("#rdGB").attr("checked") == "checked"){
        size = form_data["base_info.memory"] * 1024 * 1024 * 1024;
    }
    else{
        size = form_data["base_info.memory"] * 1024 * 1024;
    }
    if(size < 4 * 1024 * 1024 || size > 64 * 1024 * 1024 * 1024){
        frm.find(".memory").addClass("error");
        return false;
    }
    return true;
}

function UpdateMemoryData(diag, droper, vm_data){
    if(!CheckVmMemoryDataValid(diag.find(".form-horizontal"))){
        return true;
    }
    GetMemoryDataByForm(diag.find(".form-horizontal"), vm_data);
    diag.modal("hide");
}

function AddMemoryData(droper, drag_elem_obj, vm_data, vm_action){
    vm_data.cfg_info.base_info.memory  = new I8(1024 * 1024 * 1024);
}

function ShowMemoryImage(droper, drag_elem_obj, vm_data){
    var info = '<img src="img/' + drag_elem_obj.img + '"/>'; 
    var class_name = drag_elem_obj.clname;
    droper.find(".box-content").find(".span12").html(info);
    droper.addClass(class_name).addClass("configed");
    droper.data("data", drag_elem_obj);
    return;
}
