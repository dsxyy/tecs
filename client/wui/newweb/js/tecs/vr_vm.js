var oVrVmDtb;
var vmmigrate_updatepg_id = null;

function ShowVmState(rs){
    if(rs[0] != 0){
        ShowNotyRpcErrorInfo(rs);
        return;
    }
    var vm = rs[3][0];
    var vm_state = page.res[page.pagelan].type.VmState[vm["state"]];
    for(var i = 1;i < $("#tbvms").find("tr").length;i++){ 
        if($("#tbvms").find("tr").eq(i).find("td").eq(1).text() == vm["base_info"]["id"].toString()){ 
            $("#tbvms").find("tr").eq(i).find("td").eq(3).text(vm_state);
        }
    }
}

function GetSelectProjectVal(){
    var ret = '';
    if($("#selectProject").val() == null){
        return ret;
    }
    var _options = $("#selectProject option:selected");
    for(var i = 0;i < _options.length;i++){
        var _option = _options.eq(i);
        ret += _option.attr('user_name') + ":" + _option.attr('value');
        if(i < _options.length - 1){
            ret += ';';
        }
    }
    return ret;
}

function RetrieveVrVmData( sSource, aoData, fnCallback ){
    //$("#tbVm input[type=checkbox]").attr("checked", false);
    var user_name = $("#selectUser").val() == null ? "" : $("#selectUser").val();
    user_name = GetFrmtStrOfMultiSelect(user_name);
    var project_name = GetSelectProjectVal();
    
    aoData.push( { "name": "user_name", "value": user_name} );
    aoData.push( { "name": "project_name", "value": project_name} );

    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        var vmsid = [];
        var nostatevmid = []; 
        var vmdata = [];
        if(json.errcode != ""){ 
            ShowNotyQueryDBErrorInfo(json);
        }
        for(var i = 0;i < ackdata.aaData.length;i++){ 
            vmsid[i] = new I8(ackdata.aaData[i][1]);
        } 
        //绑定checkbox
        for(var i = 0;i < ackdata.aaData.length;i++){
        
            var vmobj = new Object();
            vmobj.id       = ackdata.aaData[i][1];
            vmobj.workflow = ackdata.aaData[i]["workflow"];
            vmobj.cluster  = ackdata.aaData[i]["deployed_cluster"];
            vmdata.push(vmobj);

            //var chklink = '<input type="checkbox" value=' + '"' + ackdata.aaData[i][1] + '" >';
            ackdata.aaData[i][0] = GetItemCheckbox(ackdata.aaData[i][1], currVrVmState);
            var state = ackdata.aaData[i][3];
            var enable_livemigrate = ackdata.aaData[i]["enable_livemigrate"];
            //ackdata.aaData[i][0] = chklink;
            var morelink = '<a href="javascript:void(0);"  onclick="onMoreVmClick(';
            morelink     += "'" + "idx_detailDlg" + "',";
            morelink     += "'" + ackdata.aaData[i][1] + "'";
            morelink     += ')">'+ackdata.aaData[i][1]+'</a>';
            ackdata.aaData[i][1] = morelink;
            if(ackdata.aaData[i][3] == "-1"){
                ackdata.aaData[i][3] = '<img src="./images/waiting.gif" />';
                nostatevmid.push(vmsid[i]);
            }
            else{
                ackdata.aaData[i][3] = page.res[page.pagelan].type.VmState[parseInt(ackdata.aaData[i][3])];
            }
            var oplink = ''; 
            oplink += '<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["saveasvt"] +'" onclick="SaveVtByVm('+ "'" + vmsid[i] + "'" + ')"><i class="icon icon-blue icon-save"></i><span></span></a>';
            if(state == 2 && enable_livemigrate == 1){
                oplink += '&nbsp;|&nbsp;' +'<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["livemigrate"] +'" onclick="LivemigrateVm('+ "'" + vmsid[i] + "'" + ')"><i class="icon icon-orange icon-arrow-nesw"></i><span></span></a>';   
            }
            else if(state == 4){
                oplink += '&nbsp;|&nbsp;' +'<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["coldmigrate"] +'" onclick="ColdMigrateVm('+ "'" + vmsid[i] + "'" + ')"><i class="icon icon-blue icon-arrow-nesw"></i><span></span></a>';   
            }

            if(ackdata.aaData[i]["deployed_cluster"] != null){
                oplink += "&nbsp;|&nbsp;" + '<a href="javascript:void(0);" onclick="vm_statistics($(this))" data-rel="tooltip" data-original-title="' + page.res[page.pagelan].type.opr["statistics"] + '"><i class="icon icon-image"></i></a>';             
            }
            ackdata.aaData[i][11] = oplink;
        }
        fnCallback(ackdata);

        if(nostatevmid.length > 0){
            QueryVmsCallBack(nostatevmid,ShowVmState);
        }
       
        BindTableCheckboxData($("#tbVm"),vmdata);
       
        ShowLastUpdateTime($("#tbVm").parent());

        $(".tooltip").hide();
        $('[rel="tooltip"],[data-rel="tooltip"]').tooltip({"placement":"bottom"});
    });    
}

function InitDtHost(vm_data){
    var rs = xmlrpc_service.tecs.vmcfg.runninginfo.query(xmlrpc_session, new I8(0), new I8(50),"query_self_appointed", new I8(vm_data["vid"]));
    var vm_runinfo = rs[3][0];

    $("#sldthost").empty();
    $("#sldthost").append('<option value="-1">'+ page.res[page.pagelan].type.display["noassign"] +'</option>');
    var rs = TecsObjList("host",1,vm_runinfo["computationinfo"]["deployed_cluster"],"");
    if(rs == null){
        return;
    }
    for(var i = 0;i < rs.length;i++){
        if(rs[i]["host_name"] == vm_runinfo["computationinfo"]["deployed_hc"]){
            continue;
        }
        if(rs[i]["run_state"]==2 && !rs[i].is_disabled){//已经在线且非维护
            $("#sldthost").append('<option value="'+rs[i]["oid"].toString()+'">'+ rs[i]["host_name"] +'</option>');
        }
    }
}

function showMigratingProgressInfo() {
    if($('#tbVm').length == 0){
        clearInterval(vmmigrate_updatepg_id);
        return;
    }
    
    ShowWorkflowProgress(oVrVmDtb, RefreshData);
}

function LivemigrateVm(vmid){
    var info = '';
    var diag = $("#idx_confirmDlg");
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    
    info += '       <div class="control-group vt_name">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["target_host"] + '</label>'
    info += '           <div class="controls">';
    info += '               <select id="sldthost"></select>';
    info += '               <span id="wait_migrate" class="help-inline hide"><img src="img/spinner-mini.gif"/></span>';
    info += '            </div>';
    info += '       </div>';
     
    info += '       <div class="control-group vt_is_public">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.display["isforcemigrate"] + '</label>'
    info += '           <div class="controls">';
    info += '               <input data-no-uniform="true" checked type="checkbox" class="iphone-toggle rdisforcemigrate">';
    info += '            </div>';
    info += '       </div>';
    
    info += '   </fieldset>';
    info += '</form>';
    var header = '<i class="icon icon-blue icon-arrow-nesw"></i>&nbsp;' + page.res[page.pagelan].type.opr["livemigrate"] + "-" + vmid;
    diag.find('div.modal-header h5').html(header);
    diag.find('div.modal-body').html(info);
    var vm_data = GetCurrentVmInfo(vmid); 
    if(vm_data == null){
        return;
    }
    InitDtHost(vm_data);
    diag.modal("show");
    //iOS / iPhone style toggle switch
    diag.find('.rdisforcemigrate').iphoneStyle({
        checkedLabel: page.res[page.pagelan].type.YesOrNo["1"],
        uncheckedLabel: page.res[page.pagelan].type.YesOrNo["0"]
    });
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        diag.modal("hide");
        var migrate     = new Object();
        migrate.vid     = vm_data["vid"];
        migrate.hid     = new I8($("#sldthost").attr("value"));
        migrate.bForced = diag.find('.rdisforcemigrate').attr("checked") == "checked" ? true : false;
        migrate.bLive   = true;
        //$("#wait_migrate").show();
        var rs = xmlrpc_service.tecs.vmcfg.migrate(xmlrpc_session, migrate);
        if(rs[0] == 0){
            if(rs[1] != ""){
                $("#tbVm tbody input[type=checkbox]").each(function(){
                    var id = new I8($(this).attr("value"));
                    if(id == vmid){
                        var obj = $(this).data("item");
                        obj.workflow = rs[1];

                        $(this).data("item", obj);  
                    }
                });
                showMigratingProgressInfo();                
            }
        }
        else{
            ShowSynRpcInfo(rs);
        }
    });
}

function ColdMigrateVm (vmid) {
    var info = '';
    var diag = $("#idx_confirmDlg");
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    
    info += '       <div class="control-group vt_name">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["target_host"] + '</label>'
    info += '           <div class="controls">';
    info += '               <select id="sldthost"></select>';
    info += '               <span id="wait_migrate" class="help-inline hide"><img src="img/spinner-mini.gif"/></span>';
    info += '            </div>';
    info += '       </div>';
    
    info += '   </fieldset>';
    info += '</form>';
    var header = '<i class="icon icon-blue icon-arrow-nesw"></i>&nbsp;' + page.res[page.pagelan].type.opr["coldmigrate"] + "-" + vmid;
    diag.find('div.modal-header h5').html(header);
    diag.find('div.modal-body').html(info);
    var vm_data = GetCurrentVmInfo(vmid); 
    if(vm_data == null){
        return;
    }
    InitDtHost(vm_data);
    diag.modal("show");

    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        diag.modal("hide");
        var migrate     = new Object();
        migrate.vid     = vm_data["vid"];
        migrate.hid     = new I8($("#sldthost").attr("value"));
        migrate.bForced = false;
        migrate.bLive   = false;
        //$("#wait_migrate").show();
        var rs = xmlrpc_service.tecs.vmcfg.migrate(xmlrpc_session, migrate);
         if(rs[0] == 0){
            if(rs[1] != ""){
                $("#tbVm tbody input[type=checkbox]").each(function(){
                    var id = new I8($(this).attr("value"));
                    if(id == vmid){
                        var obj = $(this).data("item");
                        obj.workflow = rs[1];
                        $(this).data("item", obj);  
                    }
                });
                showMigratingProgressInfo();                
            }
        }
        else{
            ShowSynRpcInfo(rs);
        }
    });    
}

function SaveVtByVm(vmid){
    var info = '';
    var diag = $("#idx_confirmDlg");
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    
    info += '       <div class="control-group vt_name">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["template_name"] + '</label>'
    info += '           <div class="controls">';
    info += '               <input id="txVtName" type="text" class="OamUi" bindattr="val" bind="vt_name">';
    info += '               <span class="help-inline">' + page.res[page.pagelan].type.err_info["text_input_error"] + '</span>';
    info += '            </div>';
    info += '       </div>';
     
    info += '       <div class="control-group vt_is_public">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["ispublish"] + '</label>'
    info += '           <div class="controls">';
    info += '               <input data-no-uniform="true" checked type="checkbox" class="iphone-toggle rdispublic">';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '   </fieldset>';
    info += '</form>';
    var header = '<i class="icon icon-blue icon-save"></i>&nbsp;' + page.res[page.pagelan].type.opr["saveasvt"] + "-" + vmid;
    diag.find('div.modal-header h5').html(header);
    diag.find('div.modal-body').html(info);
    z_strInput("txVtName",255);
    diag.modal("show");
    //iOS / iPhone style toggle switch
    diag.find('.rdispublic').iphoneStyle({
        checkedLabel: page.res[page.pagelan].type.YesOrNo["1"],
        uncheckedLabel: page.res[page.pagelan].type.YesOrNo["0"]
    });
    
    /*diag.find('#txVtName').unbind('blur').bind('blur',function(){
        CheckVtNameIsExist(diag.find('#txVtName').val(), diag.find(".form-horizontal"));
    });*/
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        diag.find(".vt_name").removeClass("error");
        if(!CheckVtNameIsExist(diag.find('#txVtName').val(), diag.find(".form-horizontal"))){
            return;
        } 
        
        var vtdata = new Object();
        vtdata.cfg_info = new Object();

        var vm_data = GetCurrentVmInfo(vmid);
        if(vm_data == null){
            return;
        }
        vtdata.cfg_info.base_info = GetVmBaseInfo(vm_data);
        vtdata.vt_name   = $("#txVtName").val();
        vtdata.is_public = diag.find('.rdispublic').attr("checked") == "checked" ? true : false;
        var rs = xmlrpc_service.tecs.vmtemplate.allocate(xmlrpc_session, vtdata);
        if(rs[0]!=0){
            diag.modal("hide");
            ShowNotyRpcErrorInfo(rs);
            return;
        }
        diag.modal("hide");
        $('.main-menu li').removeClass("active");
        $('.main-menu li a[href="vr_vt.php"]').parent().addClass("active");
        $('.main-menu li a[href="vr_vt.php"]').click();
    });
}

function RemoveVmCallback(vCheckedbox){
    ClearAlertInfo();
    vCheckedbox.each(function(){
        var id = new I8($(this).attr("value"));
        var name = $(this).closest("tr").find("td").eq(2).text();
        var result = action(id,"delete");
        ShowSynRpcInfo(result);
    });
    oVrVmDtb.fnDraw();
}

function remove(){
    ShowBatchOpConfirmDiag($("#idx_confirmDlg"), "tbVm", page.res[page.pagelan].type.err_info["delete_confirm_info"], RemoveVmCallback);
} 

function ShowCancelVmCallBack(rs,ex){
    if(rs == null || rs.length == 0){
        return;
    }

    if(rs[0] == 0){
        RefreshData();
    }
    else {
        ShowSynRpcInfo(rs);        
    }
}

function Cancel_DeployVm_Callback(vCheckedbox){
    ClearAlertInfo();    
    vCheckedbox.each(function(){
        var vid = new I8($(this).attr("value"));
        xmlrpc_service.tecs.vmcfg["action"](xmlrpc_session, vid,"cancel",ShowCancelVmCallBack);
    });
}

function cancel_deploy(){
    ShowBatchOpConfirmDiag($("#idx_confirmDlg"), "tbVm", page.res[page.pagelan].type.err_info["cancel_deploy_confirm_info"], Cancel_DeployVm_Callback);
}

var currVrVmState = new Object();
function RefreshData(){
    if($('#tbVm').length == 0) {
        clearInterval(vr_vm_timer_id);
        return;
    }
    ClearCurSelItem(currVrVmState);
    RecordSelItem(oVrVmDtb, "tbVm", currVrVmState);     
    GotoCurrentPageOfDataTable(oVrVmDtb);
} 

function OnSelectAll(){
    if($(this).attr("checked") == "checked"){
        $("#tbVm tbody input[type=checkbox]").attr("checked", true);
    }
    else{
        $("#tbVm tbody input[type=checkbox]").attr("checked", false);
    }
}

function UpdateSelectProject(sl, user_name_list){
    var flag = false;
    if(user_name_list == null){
        sl.attr("disabled", "disabled");
        sl.empty();
        sl.trigger("liszt:updated"); 
        return;
    }

    sl.removeAttr("disabled");

    //根据用户名增加工程选项
    for(var i = 0;i < user_name_list.length;i++){
        var user_name = user_name_list[i];
        var _options = sl.find('option');
        flag = false;
        for(var j = 0;j < _options.length;j++){
            var _option = _options.eq(j);
            if(_option.attr("user_name") == user_name){
                flag = true;
                break;
            }
        }
        if(!flag){
            AppendOption2SelectProject(sl, user_name, userdata.role, true);
        }
    }
    //根据用户名删除工程选项
    for(var i = 0;i < sl.find('option').length;i++){
        var _option = sl.find('option').eq(i);
        flag = false;
        for(var j = 0;j < user_name_list.length;j++){
            var user_name = user_name_list[j];
            if(_option.attr("user_name") == user_name){
                flag = true;
                break;
            }
        }
        if(!flag){
            sl.find('option[user_name="'+ _option.attr("user_name") +'"]').remove();
        }
    }
    sl.trigger("liszt:updated");
}

function SelectUserChange(){
    UpdateSelectProject($("#selectProject"), $("#selectUser").val());
    RefreshData();
}

$(function(){
    InitSelectUser($("#selectUser"), $("#txUsername").val(), SelectUserChange);
    var user_name_list = $("#selectUser").val();
    var user_name = user_name_list == null ? null : user_name_list[0];
    if(user_name == null) $("#selectProject").attr("disabled", "disabled");
    InitSelectProject($("#selectProject"), user_name, $("#txProject_name").val(), userdata.role, true, RefreshData)
    //datatable
    oVrVmDtb = $('#tbVm').dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] },{ "bSortable": false, "aTargets": [ 3 ] },{ "bSortable": false, "aTargets": [ 8 ] },{ "bSortable": false, "aTargets": [ 11 ] }],
        "alternatively" : "destory",
        "bProcessing": false,
        "bServerSide": true,
        "sAjaxSource": "../php/vr_vm.php",
        "fnServerData": RetrieveVrVmData
    } );
    $('.modal').modal({
        backdrop:"static",
        keyboard:false,
        "show":false
    });
    if(vmmigrate_updatepg_id == null){
    	vmmigrate_updatepg_id = setInterval(showMigratingProgressInfo,1000);
    }

    vr_vm_timer_id = setInterval(RefreshData, 10000);

    //tabs
    $('#tabsVrvm a:first').tab('show');
    $('#tabsVrvm a').click(function (e) {
      e.preventDefault();
      $(this).tab('show');
    });
    // OamUi静态显示部分初始化
    $("OamStaticUI").each(function(){
        SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
    }); 
    $("#SelecltAll").click(OnSelectAll);
    $("#refresh").click(RefreshData);
    $("#remove").click(remove);
    $("#cancel_deploy").click(cancel_deploy);
    $('ul.breadcrumb li a').click(Switch_Vr_Page);
});
