function ShowVmSummaryDiag(diag, droper, vm_data, vm_action, drag_elem_obj){
    var info = '';
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    
    info += '       <div class="control-group vm_name hide">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["name"] + '<font color="red">*</font></label>';
    info += '           <div class="controls">';
    info += '               <input id="txVmName" type="text" maxlength="32" class="OamUi" bindattr="val" bind="cfg_info.vm_name">';
    info += '               <span class="help-inline">' + page.res[page.pagelan].type.err_info["text_input_error"] + '</span>';
    info += '            </div>';
    info += '       </div>';
    
    info += '       <div class="control-group vt_name hide">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["name"] + '</label>'
    info += '           <div class="controls">';
    info += '               <input id="txVtName" type="text" class="OamUi" bindattr="val" bind="vt_name">';
    info += '               <span class="help-inline">' + page.res[page.pagelan].type.err_info["text_input_error"] + '</span>';
    info += '            </div>';
    info += '       </div>';
     
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["virt_type"] + '</label>';
    info += '           <div class="controls">';
    info += '               <input data-no-uniform="true" checked type="checkbox" class="iphone-toggle rdvirtype">';
    info += '            </div>';
    info += '       </div>';
      
    info += '       <div class="control-group deploy_pos">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["assign_cluster"] + '</label>'
    info += '           <div class="controls">';
    info += '               <select id="slccname" class="OamUi" bindattr="val" bind="cfg_info.base_info.cluster"></select>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '       <div class="control-group deploy_pos">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["assign_host"] + '</label>'
    info += '           <div class="controls">';
    info += '               <select id="slhcname" class="OamUi" bindattr="val" bind="cfg_info.base_info.host"></select>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '       <div class="control-group vm_enable_serial">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["enable_serial"] + '</label>'
    info += '           <div class="controls">';
    info += '               <input data-no-uniform="true" checked type="checkbox" class="iphone-toggle enable_serial">';
    info += '            </div>';
    info += '       </div>'; 

    info += '       <div class="control-group vm_num hide">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["num"] + '<font color="red">*</font></label>'
    info += '           <div class="controls">';
    info += '               <input type="text" id="txVmNum" class="OamUi" bindattr="val" bind="vm_num">';
    info += '               <span class="help-inline">' + page.res[page.pagelan].type.err_info["vm_num_desc"] + '</span>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '       <div class="control-group vm_project hide">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["project"] + '</label>'
    info += '           <div class="controls">';
    info += '               <select id="slproject" class="OamUi" bindattr="val" bind="cfg_info.project_name"></select>';
    info += '            </div>';
    info += '       </div>';
    
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + 'Hypervisor' + '</label>'
    info += '           <div class="controls">';
    info += '               <select id="slhypervisor"><option value="xen">xen<option value="kvm">kvm<option value="any">any</select>';
    info += '            </div>';
    info += '       </div>';    
    
    info += '       <div class="control-group vt_is_public hide">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["ispublish"] + '</label>'
    info += '           <div class="controls">';
    info += '               <input data-no-uniform="true" checked type="checkbox" class="iphone-toggle rdispublic">';
    info += '            </div>';
    info += '       </div>'; 
     
    info += '       <div class="control-group vm_enable_coredump">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["enable_coredump"] + '</label>'
    info += '           <div class="controls">';
    info += '               <input data-no-uniform="true" type="checkbox" class="iphone-toggle enable_coredump">';
    info += '            </div>';
    info += '       </div>'; 

    info += '       <div class="control-group vm_enable_livemigrate">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["enable_livemigrate"] + '</label>'
    info += '           <div class="controls">';
    info += '               <input data-no-uniform="true" type="checkbox" class="iphone-toggle enable_livemigrate">';
    info += '            </div>';
    info += '       </div>';         
     
    info += '       <div class="control-group vm_backup">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["backupspace"] + '</label>'
    info += '           <div class="controls">';
    info += '               <input type="text" id="txVmBackup" class="OamUi" bindattr="val" bind="vm_backup">';	
    info += '            </div>';
    info += '       </div>'; 
	
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["desc"] + '</label>'
    info += '           <div class="controls">';
    info += '               <textarea id="taDesc" rows=8 cols=50 maxlength="128" class="OamUi autogrow" bindattr="val" bind="cfg_info.base_info.description"></textarea><p id="pSetDescInfo"></p>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '   </fieldset>';
    info += '</form>';

 
    SetVmParaHeader(drag_elem_obj.diag_img, droper.find("div h5").text(), diag);
    diag.find('div.modal-body').html(info);
    InitSlPorject($("#slproject"));
    $("#slccname").change(CCNameChange);
    z_strInput("txVmName",255);
    z_strInput("txVtName",255);
    z_numInput("txVmNum",1,10000);
    InitSummaryData(diag, vm_data, vm_action);
    LimitCharLenInput($("#taDesc"),$("#pSetDescInfo"));
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        UpdateSummaryData(diag, droper, vm_data, vm_action);
    });
    diag.find('#txVtName').unbind('blur').bind('blur',function(){
        CheckVtNameIsExist(diag.find('#txVtName').val(), diag.find(".form-horizontal"));
    });
    
    diag.modal("show");

    //iOS / iPhone style toggle switch
    diag.find('.rdvirtype').iphoneStyle({
        checkedLabel: page.res[page.pagelan].type.VirtType["hvm"],
        uncheckedLabel: page.res[page.pagelan].type.VirtType["pvm"],
        onChange:function(){
            $("#slhypervisor").empty();
            if(diag.find('.rdvirtype').eq(0).attr("checked")){
                $("#slhypervisor").append("<option value='xen'>xen");
                $("#slhypervisor").append("<option value='kvm'>kvm");
                $("#slhypervisor").append("<option value='any'>any");
            }
            else {
                $("#slhypervisor").append("<option value='xen'>xen");
            }
        }
    });

    $("#slhypervisor").unbind("change").bind("change", function(){
        if($("#slhypervisor").val() == "kvm"|| $("#slhypervisor").val() == "any"){
            $(".vm_enable_coredump").hide();
        }
        else {
            $(".vm_enable_coredump").show();
        }

        UpdateImageList($("#slhypervisor").val());
    });

    //iOS / iPhone style toggle switch
    diag.find('.rdispublic').iphoneStyle({
        checkedLabel: page.res[page.pagelan].type.YesOrNo["1"],
        uncheckedLabel: page.res[page.pagelan].type.YesOrNo["0"]
    });
    diag.find('.enable_coredump').iphoneStyle({
        checkedLabel: page.res[page.pagelan].type.CoreDump["1"],
        uncheckedLabel: page.res[page.pagelan].type.CoreDump["0"]
    }); 
    diag.find('.enable_serial').iphoneStyle({
        checkedLabel: page.res[page.pagelan].type.Serial["1"],
        uncheckedLabel: page.res[page.pagelan].type.Serial["0"]
    });
    diag.find('.enable_livemigrate').iphoneStyle({
        checkedLabel: page.res[page.pagelan].type.YesOrNo["1"],
        uncheckedLabel: page.res[page.pagelan].type.YesOrNo["0"]
    });          

    $("#slhypervisor").trigger("change");    
}

function ShowSummaryDetail(idx_pos, vm_data, vm_action){
    var info = '';
    info += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
    info += '   <tbody>';
    info += '       <tr class="vm_id hide">';
    info += '           <td width="40%">ID</td>';
    info += '           <td>' + vm_data["vid"] + '</td>';
    info += '       </tr>';
    
    info += '       <tr class="vm_name hide">';
    info += '           <td width="40%">' + page.res[page.pagelan].type.field_name["name"] + '</td>';
    info += '           <td >' + vm_data["cfg_info"]["vm_name"] + '</td>';
    info += '       </tr>';
    
    info += '       <tr class="vt_name hide">';
    info += '           <td width="40%">' + page.res[page.pagelan].type.field_name["name"] + '</td>';
    info += '           <td >' + vm_data["vt_name"] + '</td>';
    info += '       </tr>';
    if(vm_data["computationinfo"] != null){
        if(vm_data["computationinfo"]["state"] != null){  
    info += '       <tr class="vm_state hide">';
    info += '           <td width="40%">' + page.res[page.pagelan].type.field_name["status"] + '</td>';
    info += '           <td >' + page.res[page.pagelan].type.VmState[vm_data["computationinfo"]["state"]] + '</td>';
    info += '       </tr>';
        }
   }
 
    info += '       <tr class="vm_create_time hide">';
    info += '           <td width="40%">' + page.res[page.pagelan].type.field_name["create_time"] + '</td>';
    info += '           <td >' + getLocalTime( vm_data["create_time"] )+ '</td>';
    info += '       </tr>';
    
    info += '       <tr>';
    info += '           <td width="40%">' + page.res[page.pagelan].type.field_name["virt_type"] + '</td>';
    info += '           <td >' + GetVirtTypeByRes(vm_data["cfg_info"]["base_info"]["virt_type"]) + '</td>';
    info += '       </tr>'; 
    
    info += '       <tr class="deploy_pos">';
    info += '           <td width="40%">' + page.res[page.pagelan].type.field_name["assign_cluster"] + '</td>';
    info += '           <td >' + GetDeployPosByRes(vm_data["cfg_info"]["base_info"]["cluster"]) + '</td>';
    info += '       </tr>'; 
    
    info += '       <tr class="deploy_pos">';
    info += '           <td width="40%">' + page.res[page.pagelan].type.field_name["assign_host"] + '</td>';
    info += '           <td >' + GetDeployPosByRes(vm_data["cfg_info"]["base_info"]["host"]) + '</td>';
    info += '       </tr>';  
    
    info += '       <tr class=vm_enable_serial >';
    info += '           <td width="40%">' + page.res[page.pagelan].type.field_name["enable_serial"] + '</td>';
    info += '           <td >' + GetSerialByRes(vm_data["cfg_info"]["base_info"]["enable_serial"]) + '</td>';
    info += '       </tr>'; 

    if(vm_data["computationinfo"] != null){
        if(vm_data["computationinfo"]["deployed_cluster"] != null){
    info += '       <tr class="running_pos hide">';
    info += '           <td width="40%">' + page.res[page.pagelan].type.field_name["running_cluster"] + '</td>';
    info += '           <td >' + GetDeployPosByRes(vm_data["computationinfo"]["deployed_cluster"]) + '</td>';
    info += '       </tr>'; 
        }
        if(vm_data["computationinfo"]["deployed_hc"] != null){
    info += '       <tr class="running_pos hide">';
    info += '           <td width="40%">' + page.res[page.pagelan].type.field_name["running_hc"] + '</td>';
    info += '           <td >' + GetDeployPosByRes(vm_data["computationinfo"]["deployed_hc"]) + '</td>';
    info += '       </tr>'; 
        }
    }
    
    info += '       <tr>';
    info += '           <td width="40%">' + page.res[page.pagelan].type.field_name["desc"] + '</td>';
    info += '           <td >' + vm_data["cfg_info"]["base_info"]["description"] + '</td>';
    info += '       </tr>'; 
    
    info += '       <tr class="vm_num hide">';
    info += '           <td width="40%">' + page.res[page.pagelan].type.field_name["num"] + '</td>';
    info += '           <td >' + GetVmNumByRes(vm_data["vm_num"]) + '</td>';
    info += '       </tr>';  
    
    info += '       <tr class="vm_project hide">';
    info += '           <td width="40%">' + page.res[page.pagelan].type.field_name["project"] + '</td>';
    info += '           <td >' + vm_data["cfg_info"]["project_name"] + '</td>';
    info += '       </tr>';   
    
    info += '       <tr>';
    info += '           <td width="40%">' + 'Hypervisor' + '</td>';
    info += '           <td >' + vm_data["cfg_info"]["base_info"]["hypervisor"] + '</td>';
    info += '       </tr>';  
	
    info += '       <tr>';
    info += '           <td width="40%">' + page.res[page.pagelan].type.field_name["backupspace"] + '</td>';
    if(vm_data["cfg_info"]["base_info"]["machine"]["reserved_backup"] != null){
    info += '           <td >' + vm_data["cfg_info"]["base_info"]["machine"]["reserved_backup"] + '</td>';
    }
    else if(vm_data["cfg_info"]["base_info"]["reserved_backup"] != null){
        info += '           <td >' + vm_data["cfg_info"]["base_info"]["reserved_backup"] + '</td>';
    }
    info += '       </tr>'; 
	
    info += '       <tr class="vt_is_public hide">';
    info += '           <td width="40%">' + page.res[page.pagelan].type.field_name["ispublish"] + '</td>';
    info += '           <td >' + GetIsPublicByRes(vm_data["is_public"]) + '</td>';
    info += '       </tr>'; 
    
    if(vm_data["cfg_info"]["base_info"]["hypervisor"] != "kvm" && vm_data["cfg_info"]["base_info"]["hypervisor"] != "any"){
        info += '       <tr class="vm_enable_coredump">';
        info += '           <td width="40%">' + page.res[page.pagelan].type.field_name["enable_coredump"] + '</td>';
        info += '           <td >' + GetCoreDumpByRes(vm_data["cfg_info"]["base_info"]["enable_coredump"]) + '</td>';
        info += '       </tr>';     
    }

    info += '       <tr class="vm_enable_livemigrate">';
    info += '           <td width="40%">' + page.res[page.pagelan].type.field_name["enable_livemigrate"] + '</td>';
    info += '           <td >' + GetLiveMigrateByRes(vm_data["cfg_info"]["base_info"]["enable_livemigrate"]) + '</td>';
    info += '       </tr>';      
    
    info += '   </tbody>';
    info += '</table>';
    
    idx_pos.html(info); 
    ControlSummaryShow(vm_action);
}

function ControlCreateVmShow(){
    $(".vm_name").show();
    $(".vm_num").show(); 
    $(".vm_project").show();
    //$(".vm_enable_coredump").show();
}

function ControlModifyVmShow(){
    $(".vm_id").show();
    $(".vm_name").show();
    $(".vm_project").show();
    //$(".vm_enable_coredump").show();    
} 

function ControlQueryVmShow(){
    $(".vm_id").show();
    $(".vm_name").show();
    $(".vm_state").show(); 
    $(".vm_create_time").show();
    $(".vm_project").show();
    //$(".vm_enable_coredump").show();        
    if(userdata.role == 1){
        $(".running_pos").show();
    }
} 

function ControlCreateVtShow(){
    $(".vt_name").show();
    $(".vt_is_public").show();
}  

function ControlModifyVtShow(){
    $(".vt_is_public").show();
}

function ControlSummaryShow(vm_atcion){
    var control_list = [
        [global_createvm_acton,ControlCreateVmShow],
        [global_modifyvm_acton,ControlModifyVmShow],
        [global_queryvm_acton,ControlQueryVmShow], 
        [global_createvt_acton,ControlCreateVtShow],
        [global_modifyvt_acton,ControlModifyVtShow],
        [global_queryvt_acton,ControlCreateVtShow],
        [global_createvmbyvt_acton,ControlCreateVmShow],
        [global_modifydeplofyvm_acton,ControlModifyVmShow]
    ];
    for(var i = 0;i < control_list.length;i++){
        var control = control_list[i];
        if(control[0] == vm_atcion){
            control[1]();
        }
    }
    //租户
    if(userdata.role != 1){
        $(".deploy_pos").hide();
    }
}

function InitSummaryData(diag, vm_data, vm_action){
    InitAssignedDeploy();
    SetModifyForm(diag.find(".form-horizontal"), vm_data);
    CCNameChange();
    $("#slhcname").attr("value",vm_data["cfg_info"]["base_info"]["host"]);
    SetVirtType(vm_data["cfg_info"]["base_info"]["virt_type"], diag);
    SetIsPublic(vm_data["is_public"], diag);
    SetCoreDump(vm_data["cfg_info"]["base_info"]["enable_coredump"], diag);
    SetLiveMigrate(vm_data["cfg_info"]["base_info"]["enable_livemigrate"], diag);
    SetSerial(vm_data["cfg_info"]["base_info"]["enable_serial"], diag);
    SetHypervisor(vm_data["cfg_info"]["base_info"]["hypervisor"], diag);
    SetReservedBackup(vm_data["cfg_info"]["base_info"]["reserved_backup"], diag);	
    ControlSummaryShow(vm_action);
}

function SetVirtType(virt_type, diag){
    if(virt_type == "hvm"){
        diag.find(".rdvirtype").attr("checked",true);
    }
    else{
        diag.find(".rdvirtype").attr("checked",false);

        $("#slhypervisor").empty();
        if(diag.find('.rdvirtype').eq(0).attr("checked")){
            $("#slhypervisor").append("<option value='xen'>xen");
            $("#slhypervisor").append("<option value='kvm'>kvm");
        }
        else {
            $("#slhypervisor").append("<option value='xen'>xen");
        }
    }
}

function GetVirtType(diag){
    if(diag.find(".rdvirtype").attr("checked") == "checked"){
        return "hvm";
    }
    else{
        return "pvm";
    }
}

function SetIsPublic(is_public, diag){
    if(is_public == null){
        return;
    }
    
    if(is_public){
        diag.find(".rdispublic").attr("checked", true);
    }
    else{
        diag.find(".rdispublic").attr("checked",false);
    }
} 

function GetIsPublic(diag){
    if(diag.find(".rdispublic").attr("checked") == "checked"){
        return true;
    }
    else{
        return false;
    }
}

function SetCoreDump(coredump, diag){
    if(coredump == null){
        return;
    }
    
    if(coredump){
        diag.find(".enable_coredump").attr("checked", true);
    }
    else{
        diag.find(".enable_coredump").attr("checked",false);
    }
}

function GetCoreDump(diag){
    if(diag.find(".enable_coredump").attr("checked") == "checked"){
        return true;
    }
    else{
        return false;
    }
}

function SetLiveMigrate(livemigrate, diag){
    if(livemigrate == null){
        return;
    }
    
    if(livemigrate){
        diag.find(".enable_livemigrate").attr("checked", true);
    }
    else{
        diag.find(".enable_livemigrate").attr("checked",false);
    }    
}

function GetLiveMigrate(diag){
    if(diag.find(".enable_livemigrate").attr("checked") == "checked"){
        return true;
    }
    else{
        return false;
    }
}

function SetSerial(enable_serial, diag){
    if(enable_serial == null){
        return;
    }
    
    if(enable_serial){
        diag.find(".enable_serial").attr("checked", true);
    }
    else{
        diag.find(".enable_serial").attr("checked",false);
    }
} 

function GetSerial(diag){
    if(diag.find(".enable_serial").attr("checked") == "checked"){
        return true;
    }
    else{
        return false;
    }
}

function SetHypervisor (hypervisor, diag) {
    diag.find("#slhypervisor").val(hypervisor);
}

function GetHypervisor (diag) {
    return diag.find("#slhypervisor").val();
}

function SetReservedBackup (backup, diag) {
    diag.find("#txVmBackup").val(backup);
}

function GetDeployPosByRes(pos){
    if(pos == null){
        return "";
    }
    if(pos == ""){
        return page.res[page.pagelan].type.display["noassign"];
    } 
    return pos;
}

function GetIsPublicByRes(is_public){
    if(is_public == null){
        return "";
    }
    if(!is_public){
        return page.res[page.pagelan].type.YesOrNo["0"];
    }
    return page.res[page.pagelan].type.YesOrNo["1"];
}

function GetCoreDumpByRes(coredump){
    if(coredump == null){
        return "";
    }
    if(!coredump){
        return page.res[page.pagelan].type.CoreDump["0"];
    }
    return page.res[page.pagelan].type.CoreDump["1"];
}

function GetLiveMigrateByRes(livemigrate){
    if(livemigrate == null){
        return "";
    }
    if(!livemigrate){
        return page.res[page.pagelan].type.YesOrNo["0"];
    }
    return page.res[page.pagelan].type.YesOrNo["1"];    
}

function GetSerialByRes(Serial){
    if(Serial == null){
        return "";
    }
    if(!Serial){
        return page.res[page.pagelan].type.Serial["0"];
    }
    return page.res[page.pagelan].type.Serial["1"];
}

function GetVmNumByRes(vm_num){
    if(vm_num == null){
        return "";
    } 
    return vm_num.toString();
}

function GetVirtTypeByRes(virt_type){
    if(virt_type == "hvm"){
        return page.res[page.pagelan].type.VirtType["hvm"];
    }
    else{
        return page.res[page.pagelan].type.VirtType["pvm"];
    }
}

function GetCreateVmSummaryDataByForm(frm,vm_data){
    var form_data                     = GetModifyForm(frm);
    vm_data.cfg_info.vm_name                   = form_data["cfg_info.vm_name"];
    vm_data.cfg_info.project_name              = form_data["cfg_info.project_name"]; 
    vm_data.cfg_info.base_info.virt_type       = GetVirtType(frm);
    vm_data.cfg_info.base_info.enable_serial   = GetSerial(frm);
    vm_data.cfg_info.base_info.cluster         = form_data["cfg_info.base_info.cluster"];
    vm_data.cfg_info.base_info.host            = form_data["cfg_info.base_info.host"];
    vm_data.cfg_info.base_info.description     = form_data["cfg_info.base_info.description"];
    vm_data.cfg_info.base_info.enable_coredump = GetCoreDump(frm);
    vm_data.cfg_info.base_info.enable_livemigrate = GetLiveMigrate(frm);
    vm_data.vm_num                             = parseInt(form_data["vm_num"]);
    vm_data.cfg_info.base_info.hypervisor      = GetHypervisor(frm);    
    vm_data.cfg_info.base_info.reserved_backup = parseInt(form_data.vm_backup); 
}

function GetModifyVmSummaryDataByForm(frm,vm_data){
    var form_data                     = GetModifyForm(frm);
    vm_data.cfg_info.vm_name                   = form_data["cfg_info.vm_name"];
    vm_data.cfg_info.project_name              = form_data["cfg_info.project_name"]; 
    vm_data.cfg_info.base_info.virt_type       = GetVirtType(frm);
    vm_data.cfg_info.base_info.enable_serial   = GetSerial(frm);
    vm_data.cfg_info.base_info.cluster         = form_data["cfg_info.base_info.cluster"];
    vm_data.cfg_info.base_info.host            = form_data["cfg_info.base_info.host"];
    vm_data.cfg_info.base_info.description     = form_data["cfg_info.base_info.description"];
    vm_data.cfg_info.base_info.enable_coredump = GetCoreDump(frm);   
    vm_data.cfg_info.base_info.enable_livemigrate = GetLiveMigrate(frm);     
    vm_data.cfg_info.base_info.hypervisor      = GetHypervisor(frm);   
    vm_data.cfg_info.base_info.reserved_backup = parseInt(form_data.vm_backup); 
}

function GetCreateVtSummaryDataByForm(frm,vm_data){
    var form_data                     = GetModifyForm(frm);
    vm_data.vt_name                   = form_data.vt_name;
    vm_data.cfg_info.base_info.virt_type       = GetVirtType(frm);
    vm_data.cfg_info.base_info.enable_serial   = GetSerial(frm);
    vm_data.cfg_info.base_info.cluster         = form_data["cfg_info.base_info.cluster"];
    vm_data.cfg_info.base_info.host            = form_data["cfg_info.base_info.host"];
    vm_data.cfg_info.base_info.description     = form_data["cfg_info.base_info.description"];
    vm_data.is_public                          = GetIsPublic(frm);
    vm_data.cfg_info.base_info.enable_coredump = GetCoreDump(frm);  
    vm_data.cfg_info.base_info.enable_livemigrate = GetLiveMigrate(frm);       
    vm_data.cfg_info.base_info.hypervisor      = GetHypervisor(frm);
    vm_data.cfg_info.base_info.reserved_backup = parseInt(form_data.vm_backup); 
}

function GetModifyVtSummaryDataByForm(frm,vm_data){
    var form_data                              = GetModifyForm(frm);
    vm_data.cfg_info.base_info.virt_type       = GetVirtType(frm);
    vm_data.cfg_info.base_info.enable_serial   = GetSerial(frm);
    vm_data.cfg_info.base_info.cluster         = form_data["cfg_info.base_info.cluster"];
    vm_data.cfg_info.base_info.host            = form_data["cfg_info.base_info.host"];
    vm_data.cfg_info.base_info.description     = form_data["cfg_info.base_info.description"];
    vm_data.is_public                          = GetIsPublic(frm);
    vm_data.cfg_info.base_info.enable_coredump = GetCoreDump(frm);    
    vm_data.cfg_info.base_info.enable_livemigrate = GetLiveMigrate(frm);     
    vm_data.cfg_info.base_info.hypervisor      = GetHypervisor(frm); 
    vm_data.cfg_info.base_info.reserved_backup = parseInt(form_data.vm_backup); 
}

function CheckVmNameValid(vm_name, frm){
    if(vm_name == ""){
        frm.find(".vm_name").addClass("error");
        return false;
    }
    return true;
}

function CheckVtNameValid(vt_name, frm){
    if(vt_name == ""){
        frm.find(".vt_name").addClass("error");
        frm.find(".vt_name .help-inline").html(page.res[page.pagelan].type.err_info["text_input_error"]);
        return false;
    }
    return true;
}

function CheckVmNumValid(vm_num, frm){
    if(vm_num == ""){
        frm.find(".vm_num").addClass("error");
        return false;
    }
    return true;
}


function CheckCreateVmSummary(frm){
    var form_data = GetModifyForm(frm);
    if(!CheckVmNameValid(form_data["vm_name"], frm)){
        return false;
    }
    if(!CheckVmNumValid(form_data["vm_num"], frm)){
        return false;
    }
    return true;
}

function CheckModifyVmSummary(frm){
    var form_data = GetModifyForm(frm);
    if(!CheckVmNameValid(form_data["vm_name"], frm)){
        return false;
    }
    return true;
}

function CheckCreateVtSummary(frm, droper){
    var form_data = GetModifyForm(frm);
    if(!CheckVtNameIsExist(form_data["vt_name"], frm)){
        return false;
    }
    return true;
}

function CheckModifyVtSummary(frm){
    var form_data = GetModifyForm(frm);
    return true;
}

function CheckVmSummaryDataValid(frm, vm_action, droper){
    var checksummary_callback_list = [
        [global_createvm_acton,CheckCreateVmSummary],
        [global_modifyvm_acton,CheckModifyVmSummary],
        [global_createvt_acton,CheckCreateVtSummary],
        [global_modifyvt_acton,CheckModifyVtSummary],
        [global_createvmbyvt_acton,CheckCreateVmSummary]
    ];
    for(var i = 0;i < checksummary_callback_list.length;i++){
        var checksummary_callback = checksummary_callback_list[i];
        if(checksummary_callback[0] == vm_action){
            return checksummary_callback[1](frm, droper);
        }
    }
    return false;
}

function GetSummaryDataByForm(frm, vm_data, vm_action){
    var getsummary_callback_list = [
        [global_createvm_acton,GetCreateVmSummaryDataByForm],
        [global_modifyvm_acton,GetModifyVmSummaryDataByForm],
        [global_createvt_acton,GetCreateVtSummaryDataByForm],
        [global_modifyvt_acton,GetModifyVtSummaryDataByForm],
        [global_createvmbyvt_acton,GetCreateVmSummaryDataByForm]
    ];
    for(var i = 0;i < getsummary_callback_list.length;i++){
        var getsummary_callback = getsummary_callback_list[i];
        if(getsummary_callback[0] == vm_action){
            return getsummary_callback[1](frm, vm_data);
        }
    }
    return;
}

function UpdateSummaryData(diag, droper, vm_data, vm_action){
    if(!CheckVmSummaryDataValid(diag.find(".form-horizontal"), vm_action, droper)){
        return true;
    }
    GetSummaryDataByForm(diag.find(".form-horizontal"), vm_data, vm_action);
    diag.modal("hide");
}

function InitAssignedDeploy(){
    InitCCName($("#slccname"));
    CCNameChange();
}

//----------------------------------------------
function InitCCName(sl){
    sl.empty();
    sl.append("<option value=''>"+ page.res[page.pagelan].type.display["noassign"] +"</option>");
    if(userdata.role == 1){//管理员
        var mode = 0;
        var rs = TecsObjList("cluster", 1, mode);
        if(rs == null || rs.length == 0){
            return;
        }
        for(var i = 0;i < rs.length;i++){
            sl.append("<option value="+ rs[i]["cluster_name"] +">"+ rs[i]["cluster_name"] +"</option>");
        }
    }
} 

function CCNameChange(){
    InitHCName($("#slccname").val(),$("#slhcname"));
}

function InitHCName(ccname,sl){
    sl.empty();
    sl.append("<option value=''>"+ page.res[page.pagelan].type.display["noassign"] +"</option>");
    if(ccname==null || ccname==""){
        sl.trigger("change");
        return;
    }
    if(userdata.role == 1){//管理员
        var rs = TecsObjList("host", 1, ccname,"");
        if(rs==null || rs.length==0){
            return;
        }
        for(var i = 0;i < rs.length;i++){
            sl.append("<option value="+rs[i]["host_name"]+">"+ rs[i]["host_name"] +"</option>");
        }

        sl.trigger("change");
    }

    sl.unbind("change").bind("change", function(){
        var selected_hc = sl.val();
        if(selected_hc != ""){
            $(".enable_livemigrate").attr("checked", false); 
            $('.enable_livemigrate').iphoneStyle({
                checkedLabel: page.res[page.pagelan].type.YesOrNo["1"],
                uncheckedLabel: page.res[page.pagelan].type.YesOrNo["0"]
            }).data("iphoneStyle").refresh();  

            $(".enable_livemigrate").attr("disabled", true);            
        }
        else {
            $(".enable_livemigrate").attr("disabled", false);
        }

        $('.enable_livemigrate').iphoneStyle({
            checkedLabel: page.res[page.pagelan].type.YesOrNo["1"],
            uncheckedLabel: page.res[page.pagelan].type.YesOrNo["0"]
        }).data("iphoneStyle").refresh();          
    });
}

function AddVmOfSummaryData(vm_data){
    var project_name = QueryFirstPorjectsName(-5,"","");
    vm_data.cfg_info.vm_name = "vm";
    vm_data.cfg_info.project_name = project_name; 
    vm_data.cfg_info.base_info.virt_type = "hvm";
    //vm_data.base_info.enable_serial    = true;
    vm_data.cfg_info.base_info.cluster = "";
    vm_data.cfg_info.base_info.host = "";
    vm_data.cfg_info.base_info.description = "";
    vm_data.vm_num = 1;
}

function AddVtOfSummaryData(vm_data){
    vm_data.vt_name = "vt";
    vm_data.cfg_info.base_info.virt_type = "hvm";
   // vm_data.base_info.enable_serial    = true;
    vm_data.cfg_info.base_info.cluster = "";
    vm_data.cfg_info.base_info.host = "";
    vm_data.cfg_info.base_info.description = "";
    vm_data.is_public = false;
}

function AddSummaryData(droper, drag_elem_obj, vm_data, vm_action){
    var addsummary_callback_list = [
        [global_createvm_acton,AddVmOfSummaryData],
        [global_createvt_acton,AddVtOfSummaryData]
    ];
    for(var i = 0;i < addsummary_callback_list.length;i++){
        var addsummary_callback = addsummary_callback_list[i];
        if(addsummary_callback[0] == vm_action){
            addsummary_callback[1](vm_data);
        }
    }
    return;
}

function ShowSummaryImage(droper, drag_elem_obj, vm_data){
    var info = '<img src="img/' + drag_elem_obj.img + '"/>'; 
    var class_name = drag_elem_obj.clname;
    droper.find(".box-content").find(".span12").html(info);
    droper.addClass(class_name).addClass("configed");
    droper.data("data", drag_elem_obj);
    return;
}