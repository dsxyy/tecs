function ShowRemoteDesktopDiag(diag, droper, vm_data, vm_action, drag_elem_obj){
    var info = '';
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    
    info += '       <div class="control-group vnc_passwd">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["remotepwd"] + '</label>'
    info += '           <div class="controls">';
    info += '               <input type="password" id="txVnc_Passwd" class="OamUi" bindattr="val" bind="cfg_info.base_info.vnc_passwd">';
    info += '               <span class="help-inline">' + page.res[page.pagelan].type.err_info["vm_remotepwd_desc"] + '</span>';
    info += '            </div>';
    info += '       </div>';
     
    info += '       <div class="control-group confirm_passwd">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["confirmpwd"] + '</label>'
    info += '           <div class="controls">';
    info += '               <input type="password" id="txConfirm_Passwd" class="OamUi" bindattr="val" bind="cfg_info.base_info.vnc_passwd">';
    info += '               <span class="help-inline">' + page.res[page.pagelan].type.err_info["vm_remotepwd_noeq"] + '</span>';
    info += '            </div>';
    info += '       </div>';
    
    info += '   </fieldset>';
    info += '</form>';
    SetVmParaHeader(drag_elem_obj.diag_img, droper.find("div h5").text(), diag);
    diag.find('div.modal-body').html(info);
    SetModifyForm(diag.find(".form-horizontal"), vm_data);
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        UpdateRemoteDesktopData(diag, droper, vm_data, vm_action);
    });

    $("#txVnc_Passwd,#txConfirm_Passwd").unbind("change").bind("change", function(){
        CheckVmPwdValid(diag.find("#txVnc_Passwd").val(), diag.find("#txConfirm_Passwd").val(), diag.find(".form-horizontal"))
    });
    
    diag.modal("show");
}

function ShowRemoteDesktopDetail(idx_pos, vm_data, vm_action){
    if(vm_data["computationinfo"]["vnc_port"] == -1)
    {
        vm_data["computationinfo"]["vnc_port"] = "";
    }

    var info = '';
    info += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
    info += '   <tbody>';

    info += '       <tr>';
    info += '           <td COLSPAN="2" width="30%" bgcolor="whitesmoke" ><B>VNC:</B></td>';
    info += '       </tr>';
    
    info += '       <tr>';
    info += '           <td width="30%">IP</td>';
    info += '           <td>' + vm_data["computationinfo"]["vnc_ip"] + '</td>';
    info += '       </tr>';
    
    info += '       <tr>';
    info += '           <td width="30%">Port</td>';
    info += '           <td>' + vm_data["computationinfo"]["vnc_port"] + '</td>';
    info += '       </tr>';
    
    info += '   </tbody>';
    info += '</table>';
        
    info +=' <br><br>';
  
    info += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
    info += '   <tbody>';
    info += '       <tr>';
    info += '           <td class="enableTelnetips" COLSPAN="2" width="30%" bgcolor="whitesmoke"><B>Telnet:</B></td>';
    info += '       </tr>';

    if(1 == vm_data["cfg_info"]["base_info"]["enable_serial"]){
        if(vm_data["computationinfo"]["serial_port"] == -1)
        {
            vm_data["computationinfo"]["serial_port"] = "";
        }
        info += '       <tr>';
        info += '           <td width="30%">IP</td>';
        info += '           <td>' + vm_data["computationinfo"]["vnc_ip"] + '</td>';
        info += '       </tr>';
    
        info += '       <tr>';
        info += '           <td width="30%">Port</td>';
        info += '           <td>' + vm_data["computationinfo"]["serial_port"] + '</td>';
        info += '       </tr>';
    }
    else {
	    info += '       <tr>';
        info += '           <td  COLSPAN="2" width="30%" bgcolor="whitesmoke"><B>'+page.res[page.pagelan].type.err_info["enableSerialTips"]+'</B></td>';    
        info += '       </tr>';
    }

    info += '   </tbody>';
    info += '</table>';
    
    idx_pos.html(info);
}

function CheckVmPwdValid(pwd, confirm_pwd, frm){
    frm.find(".error").removeClass("error");
    if(pwd != ""){
        if(pwd.length < 8 || pwd.length > 20){
            frm.find(".vnc_passwd").addClass("error");
            return false;
        }
        if($.trim(pwd).length == 0){
            frm.find(".vnc_passwd").addClass("error");
            return false;
        }
        if(pwd != confirm_pwd){
            frm.find(".confirm_passwd").addClass("error");
            return false;
        }
    }
    return true;
}

function UpdateRemoteDesktopData(diag, droper, vm_data, vm_action){
    if(!CheckVmPwdValid(diag.find("#txVnc_Passwd").val(), diag.find("#txConfirm_Passwd").val(), diag.find(".form-horizontal"))){
        return false;
    }
    vm_data.cfg_info.base_info.vnc_passwd = diag.find("#txVnc_Passwd").val();
    diag.modal("hide");
}

function AddRemoteDesktopData(droper, drag_elem_obj, vm_data, vm_action){
    vm_data.cfg_info.base_info.vnc_passwd = "";
    return;
}

function ShowRemoteDesktopImage(droper, drag_elem_obj, vm_data){
    var info = '<img src="img/' + drag_elem_obj.img + '"/>'; 
    var class_name = drag_elem_obj.clname;
    droper.find(".box-content").find(".span12").html(info);
    droper.addClass(class_name).addClass("configed");
    droper.data("data", drag_elem_obj);
    return;
}