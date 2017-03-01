function ShowWatchDogDiag(diag, droper, vm_data, action, drag_elem_obj){
    var info = '';
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    info += '       <div class="control-group vm_wdtime">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["watch_dog"] +'<font color="red">*</font></label>'
    info += '           <div class="controls">';
    info += '               <input id="txWdTime" type="text" class="OamUi" bindattr="val" bind="vm_wdtime">';
    info += '               <span class="help-inline">' + page.res[page.pagelan].type.err_info["vm_watch_dog_desc"] + '</span>';
    info += '            </div>';
    info += '       </div>';
    
    info += '   </fieldset>';
    info += '</form>';
    SetVmParaHeader(drag_elem_obj.diag_img, page.res[page.pagelan].type.field_name["watch_dog"], diag);
    diag.find('div.modal-body').html(info);
    z_numInput("txWdTime",10,120);
    InitWatchDogData(diag, vm_data);
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        UpdateWatchDogData(diag, droper, vm_data, drag_elem_obj);
    });
    $("input:checkbox, input:radio, input:file").not('[data-no-uniform="true"],#uniform-is-ajax').uniform();
    diag.modal("show");
}

function ShowWatchDogDetail(idx_pos,vm_data){
    var info = '';
    info += '<table class="table table-condensed">';
    info += '   <tbody>';
    info += '       <tr>';
    info += '           <td width="30%">' + page.res[page.pagelan].type.field_name["watch_dog"] +'</td>';
    if(vm_data["cfg_info"]["base_info"]["vm_wdtime"] == 0){ 
        info += '           <td>' + page.res[page.pagelan].type.opr["disable"] + '</td>';
    }
    else{
        info += '           <td>' + GetFrmWdTime(vm_data["cfg_info"]["base_info"]["vm_wdtime"]) + '</td>';
    }
    info += '       </tr>'; 
    
    info += '   </tbody>';
    info += '</table>';
    
    idx_pos.html(info);
}

function GetFrmWdTime(vm_wdtime){
    var ret = "";
    ret = vm_wdtime / 60 + page.res[page.pagelan].type.display["minute"];
    return ret;
}

function InitWatchDogData(diag, vm_data, drag_elem_obj){
    SetModifyForm(diag.find(".form-horizontal"), vm_data["cfg_info"]["base_info"]);
    var wd_time = diag.find("#txWdTime").val() / 60;
    diag.find("#txWdTime").val(wd_time);
}

function CheckWatchDogValid(frm){
    frm.removeClass("error");
    var form_data = GetModifyForm(frm); 
    if(form_data["vm_wdtime"] == ""){
        frm.find(".vm_wdtime").addClass("error");
        return false;
    } 
    return true;
}

function UpdateWatchDogData(diag, droper, vm_data, drag_elem_obj){
    if(!CheckWatchDogValid(diag)){
        return;
    }
    var form_data                     = GetModifyForm(diag);
    vm_data["cfg_info"]["base_info"]["vm_wdtime"] = parseInt(form_data["vm_wdtime"]) * 60;
    diag.modal("hide");
}

function AddWatchDogData(droper, drag_elem_obj, vm_data, vm_action, diag, drager){
    vm_data["cfg_info"]["base_info"]["vm_wdtime"] = 10 * 60;
    ShowWatchDogImage(droper, drag_elem_obj, vm_data);
    return;
}

function ShowWatchDogImage(droper, drag_elem_obj, vm_data){
    if(vm_data["cfg_info"]["base_info"]["vm_wdtime"] == 0){
        return;
    } 
    var info = '<span class="span12"><img src="img/' + drag_elem_obj.img + '"/></span>'; 
    var class_name = drag_elem_obj.clname;
    info += '<span class="pull-right icon-trash ' + class_name + '" style="cursor:pointer;"></span>';
    droper.find(".box-content").find(".span12").html(info);
    droper.addClass(class_name).addClass("configed");
    droper.data("data", drag_elem_obj);
    return;
}

function DeleteWatchDogDataCallBack(diag, droper, vm_data, drag_elem_obj){ 
    vm_data["cfg_info"]["base_info"]["vm_wdtime"] = 0;
    droper.find(".box-content").find(".span12").html("");
    droper.removeClass("configed");
    diag.modal("hide");
}

function DeleteWatchDogData(diag, droper, vm_data, drag_elem_obj){
    diag.find('div.modal-header h5').html(page.res[page.pagelan].type.display["confirm_op"]);
    diag.find('div.modal-body').html(page.res[page.pagelan].type.err_info["delete_confirm_info"]);
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        DeleteWatchDogDataCallBack(diag, droper, vm_data, drag_elem_obj);
    });
    diag.modal("show");
}
