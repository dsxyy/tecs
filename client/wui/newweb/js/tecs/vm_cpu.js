function ShowCPUDiag(diag, droper, vm_data, vm_action, drag_elem_obj){
    var info = '';
    var rs = query_core_tcu();
    if(rs == null){
        return;
    }
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    info += '       <div class="control-group vcpu">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["vcpu"] + '<font color="red">*</font></label>'
    info += '           <div class="controls">';
    info += '               <input id="txVCpu" type="text" class="OamUi" bindattr="val" bind="cfg_info.base_info.vcpu">';
    info += '               <span class="help-inline">1~' + rs[1] + page.res[page.pagelan].type.err_info["no_empty"] + '</span>';
    info += '            </div>';
    info += '       </div>';
    
    info += '       <div class="control-group tcu">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["tcu"] + '<font color="red">*</font></label>';
    info += '           <div class="controls">';
    info += '               <input id="txTcu" type="text" class="OamUi" bindattr="val" bind="cfg_info.base_info.tcu">';
    info += '               <span class="help-inline">1~' + rs[2] + page.res[page.pagelan].type.err_info["no_empty"] + '</span>';
    info += '            </div>';
    info += '       </div>';
    
    info += '   </fieldset>';
    info += '</form>';
    SetVmParaHeader(drag_elem_obj.diag_img, droper.find("div h5").text(), diag);
    diag.find('div.modal-body').html(info);
    z_numInput("txVCpu",1,rs[1]);
    z_numInput("txTcu",1,rs[2]);
    InitCPUData(diag,vm_data);
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        UpdateCPUData(diag, droper, vm_data);
    });
    $("input:checkbox, input:radio, input:file").not('[data-no-uniform="true"],#uniform-is-ajax').uniform();;

    diag.modal("show");
}

function ShowCPUDetail(idx_pos,vm_data){
    var info = '';
    info += '<table class="table table-condensed">';
    info += '   <tbody>';
    info += '       <tr>';
    info += '           <td width="30%">' + page.res[page.pagelan].type.field_name["vcpu"] + '</td>';
    info += '           <td>' + vm_data["cfg_info"]["base_info"]["vcpu"] + '</td>';
    info += '       </tr>';
     
    info += '       <tr>';
    info += '           <td width="30%">' + page.res[page.pagelan].type.field_name["tcu"] + '</td>';
    info += '           <td>' + vm_data["cfg_info"]["base_info"]["tcu"] + '</td>';
    info += '       </tr>';
    
    info += '   </tbody>';
    info += '</table>';
    
    idx_pos.html(info);
}

function query_core_tcu(){
    var rs = null;
    try{
        rs = xmlrpc_service.tecs.cluster.query_core_tcu(xmlrpc_session);
        if(rs[0] != 0){
            ShowNotyRpcErrorInfo(rs);
            rs=null;
        }
    }
    catch(e){
        var em;
        if (e.toTraceString){
            em = e.toTraceString;
        }
        else{
            em = e.message;
        }
        rs = "Error Trace:\n\n" + em;
        alert(rs);
        rs = null;
    }
    return rs;
}

function InitCoreTcu(coredesc,txvcpu,tcudesc,txtcu){
    var rs = query_core_tcu();
    if(rs == null){
        return;
    }
    if(rs[1] > 0){
        var lbcpucoredesc = "1~"+rs[1].toString() + "(" +page.res[page.pagelan].type.display["individual"] + ")";
        coredesc.text(lbcpucoredesc);
        z_numInput(txvcpu,1,rs[1]);
    }
    if(rs[2] > 0){
        var lbtcudesc = "1~" + rs[2].toString() + "(TCU)";
        tcudesc.text(lbtcudesc);
        z_numInput(txtcu,1,rs[2]);
    }
}

function InitCPUData(diag,vm_data){
    SetModifyForm(diag.find(".form-horizontal"), vm_data);
}

function GetCPUDataByForm(frm,vm_data){
    var form_data                     = GetModifyForm(frm);
    vm_data.cfg_info.base_info.vcpu            = parseInt(form_data["cfg_info.base_info.vcpu"]);
    vm_data.cfg_info.base_info.tcu             = parseInt(form_data["cfg_info.base_info.tcu"]);

    var UpdateCPUAction = new Object();
    UpdateCPUAction.vid     = vm_data["vid"];
    UpdateCPUAction.operation  = VM_UPDATE_ITEM;
    UpdateCPUAction.element    = VM_ITEM_CPU;
    UpdateCPUAction.args       = new Object();
    UpdateCPUAction.args.vcpu  = vm_data.cfg_info.base_info.vcpu.toString();
    UpdateCPUAction.args.tcu   = vm_data.cfg_info.base_info.tcu.toString();

    if(vm_data.Actions != null)
        vm_data.Actions.push(UpdateCPUAction);
}

function CheckVmCPUDataValid(frm){
    var form_data = GetModifyForm(frm);
    frm.find("fieldset div").removeClass("error");
    if(form_data["base_info.vcpu"] == ""){
        frm.find(".vcpu").addClass("error");
        return false;
    }
    if(form_data["base_info.tcu"] == ""){
        frm.find(".tcu").addClass("error");
        return false;
    }
    return true;
}

function UpdateCPUData(diag, droper, vm_data){
    if(!CheckVmCPUDataValid(diag.find(".form-horizontal"))){
        return true;
    }
    GetCPUDataByForm(diag.find(".form-horizontal"), vm_data);
    diag.modal("hide");
}

function AddCPUData(droper, drag_elem_obj, vm_data, vm_action){
    vm_data.cfg_info.base_info.vcpu            = 1;
    vm_data.cfg_info.base_info.tcu             = 1;
}

function ShowCPUImage(droper, drag_elem_obj, vm_data){
    var info = '<img src="img/' + drag_elem_obj.img + '"/>'; 
    var class_name = drag_elem_obj.clname;
    droper.find(".box-content").find(".span12").html(info);
    droper.addClass(class_name).addClass("configed");
    droper.data("data", drag_elem_obj);
    return;
}