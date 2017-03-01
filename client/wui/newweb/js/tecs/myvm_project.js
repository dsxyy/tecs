var oMyVmProjectDbt;
var project_pg_id = null;
var myvm_project_timer_id = null;

function RetrieveVrProjectData( sSource, aoData, fnCallback ){
    //$("#tbProject input[type=checkbox]").attr("checked", false);
    aoData.push( { "name": "user_name", "value": userdata.name} );

    $.getJSON( sSource, aoData, function (json) {
        var prodata = json.data;
        var projs_name = [];
        var vmcounts   = []; 
        if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
        }
        for(var i = 0;i < prodata.aaData.length;i++){
            projs_name[i] = prodata.aaData[i][1] + "&" + prodata.aaData[i][4];
            vmcounts[i]   = prodata.aaData[i][3];
        }
        //绑定checkbox
        for(var i = 0;i < prodata.aaData.length;i++){
           // prodata.aaData[i][0] = '<input type="checkbox" value=' + '"' + projs_name[i] + '" >';
            prodata.aaData[i][0] = GetItemCheckbox(projs_name[i] , currMyvmProjectState);
            var project_name = projs_name[i].split("&")[0];
            var user_name    = projs_name[i].split("&")[1];
            var vmcountlink  = vmcounts[i];
            if(vmcounts[i] != "0"){
                var urldata = "myvm_vm.php?user_name=" + user_name + "&project_name=" + project_name;
                vmcountlink = '<a href="javascript:void(0);" onclick="Switch_Vr_Page(' + "'" + urldata + "'"  + ')">' + vmcounts[i] + '</a>';
            }
            prodata.aaData[i][3] = vmcountlink;
            var oplink = '';
            oplink = '<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["modify"] +'" onclick="OnModifyClick('+ "'" + project_name + "'" +')"><i class="icon-edit"></i><span></span></a>';
            oplink += '&nbsp;|&nbsp;' + '<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["saveasimage"] +'" onclick="SaveProjectToImage('+ "'" + project_name + "'" + ')"><i class="icon icon-save"></i><span></span></a>';
            oplink += '&nbsp;|&nbsp;' + '<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["clone"] +'" onclick="CloneProj('+"'"+ project_name +"'"+')"><i class="icon icon-blue icon-copy"></i><span></span></a>';

            prodata.aaData[i][4] = oplink;
        }
        
        fnCallback(prodata);
       
        BindProjectData(prodata.aaData);

        $(".tooltip").hide();
        $('[rel="tooltip"],[data-rel="tooltip"]').tooltip({"placement":"bottom"});

        ShowLastUpdateTime($("#tbProject").parent());
        
    });
    
}

function BindProjectData(projectdata){
    for(var i = 0;i < projectdata.length;i++){
        var cb = $("#tbProject>tbody").find(":checkbox").eq(i);
        cb.data("item", projectdata[i]);
    }
}

function ShowProjectProgress(){
    if($('#tbProject').length == 0){
        clearInterval(project_pg_id);        
        return;
    }

    ShowWorkflowProgress(oMyVmProjectDbt, Refresh_MyVM_Project_Data);    
}

function RemoveProjectCallback(vCheckedbox){
    ClearAlertInfo();
     vCheckedbox.each(function(){
        var itemindex = $(this).attr("value"); 
        var projname = itemindex.split("&")[0];
        var username = itemindex.split("&")[1];
        var result = xmlrpc_service.tecs.project["delete"](xmlrpc_session,projname,username);
        ShowSynRpcInfo(result);
    }); 
    oMyVmProjectDbt.fnDraw();
}

function remove(){
    ShowBatchOpConfirmDiag($("#idx_confirmDlg"), "tbProject", page.res[page.pagelan].type.err_info["delete_confirm_info"], RemoveProjectCallback);
}

function create(){ 
    Create_Project(Refresh_MyVM_Project_Data);
}

function DeployProjectCallback(vCheckedbox){
    ClearAlertInfo();
     vCheckedbox.each(function(){
        var itemindex = $(this).attr("value"); 
        var projname = itemindex.split("&")[0];
        var username = itemindex.split("&")[1];
        var rs = xmlrpc_service.tecs.project.action(xmlrpc_session,projname, "project_deploy");
        if(rs[0] != 0){
            ShowNotyRpcErrorInfo(rs);
        }
        else {        
            $(oMyVmProjectDbt.fnGetNodes()).find(":checkbox").each(function(){
                var obj = $(this).data("item");
                if(obj!=null && obj.project == projname){
                    obj.workflow = rs[1];
                    $(this).data("item", obj);
                }
            });    

            ShowProjectProgress();      
        }

    }); 
}

function deployProj(){
    ShowBatchOpConfirmDiag($("#idx_confirmDlg"), "tbProject", page.res[page.pagelan].type.err_info["deploy_confirm_info"], DeployProjectCallback);
}


function CancelDeployProjectCallback(vCheckedbox){
    ClearAlertInfo();
     vCheckedbox.each(function(){
        var itemindex = $(this).attr("value"); 
        var projname = itemindex.split("&")[0];
        var username = itemindex.split("&")[1];
       var rs = xmlrpc_service.tecs.project.action(xmlrpc_session,projname, "project_cancel");
        if(rs[0] != 0){
            ShowNotyRpcErrorInfo(rs);
        }
        else {
            $(oMyVmProjectDbt.fnGetNodes()).find(":checkbox").each(function(){
                var obj = $(this).data("item");
                if(obj!=null && obj.project == projname){
                    obj.workflow = rs[1];
                    $(this).data("item", obj);
                }
            });    
        }
        ShowProjectProgress();    
    }); 
}

function cancelDeployProj(){
    ShowBatchOpConfirmDiag($("#idx_confirmDlg"), "tbProject", page.res[page.pagelan].type.err_info["cancelDeployProj_confirm_info"], CancelDeployProjectCallback);
}

function RebootProjectCallback(vCheckedbox){
    ClearAlertInfo();
     vCheckedbox.each(function(){
        var itemindex = $(this).attr("value"); 
        var projname = itemindex.split("&")[0];
        var username = itemindex.split("&")[1];
        var rs = xmlrpc_service.tecs.project.action(xmlrpc_session,projname, "project_reboot");
        if(rs[0] != 0){
            ShowNotyRpcErrorInfo(rs);
        }
        else {
            $(oMyVmProjectDbt.fnGetNodes()).find(":checkbox").each(function(){
                var obj = $(this).data("item");
                if(obj!=null && obj.project == projname){
                    obj.workflow = rs[1];
                    $(this).data("item", obj);
                }
            });    
        }
            ShowProjectProgress();    

    }); 
    oMyVmProjectDbt.fnDraw();
}

function rebootProj(){
    ShowBatchOpConfirmDiag($("#idx_confirmDlg"), "tbProject", page.res[page.pagelan].type.err_info["rebootProj_confirm_info"], RebootProjectCallback);
}

function ForceRebootProjectCallback(vCheckedbox){
    ClearAlertInfo();
     vCheckedbox.each(function(){
        var itemindex = $(this).attr("value"); 
        var projname = itemindex.split("&")[0];
        var username = itemindex.split("&")[1];
        var rs = xmlrpc_service.tecs.project.action(xmlrpc_session,projname, "project_reset");
        if(rs[0] != 0){
            ShowNotyRpcErrorInfo(rs);
        }
        else {
            $(oMyVmProjectDbt.fnGetNodes()).find(":checkbox").each(function(){
                var obj = $(this).data("item");
                if(obj!=null && obj.project == projname){
                    obj.workflow = rs[1];
                    $(this).data("item", obj);
                }
            });    
        }
            ShowProjectProgress();    

    }); 
    oMyVmProjectDbt.fnDraw();
}

function forceRebootProj(){
    ShowBatchOpConfirmDiag($("#idx_confirmDlg"), "tbProject", page.res[page.pagelan].type.err_info["rebootProj_confirm_info"], ForceRebootProjectCallback);
}
function modifycallback(frm ,projectname){
    var rs = null;
    var item = GetModifyForm(frm); 
    try{
        rs = xmlrpc_service.tecs.project.set(xmlrpc_session,projectname,item.des);
        if(rs[0] != 0){
            ShowNotyRpcErrorInfo(rs);
            return false;
        }
    }catch(e){
      var em;
      if (e.toTraceString){
            em = e.toTraceString;
      }else{
            em = e.message;
      }
      rs = "Error Trace:\n\n" + em;
      alert(rs);
      return false;
    } 
    Refresh_MyVM_Project_Data();
    return true;
}

function OnModifyClick(projectname){ 
    var info = '';
    var diag = $('#idx_confirmDlg');
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["desc"] + '</label>';
    info += '           <div class="controls">';
    info += '               <textarea id="taDesc" class="autogrow OamUi" id="taDesc" maxlength="128" rows="6" bindattr="val" bind="des"></textarea><p id="pSetDescInfo"></p>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '   </fieldset>';
    info += '</form>';
    
    var header = '<i class="icon-edit"></i>&nbsp;' + page.res[page.pagelan].type.opr["modify"] + '-' + projectname;
    diag.find('div.modal-header h5').html(header);
    diag.find('div.modal-body').html(info);
    LimitCharLenInput($("#taDesc"),$("#pSetDescInfo"));
    diag.modal("show");
    var rs = QueryPorjects(0,projectname,""); 
    if(rs==null || rs.length == 0){ 
        return;
    }
    SetModifyForm(diag.find(".form-horizontal"), rs[0]);
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        if(!modifycallback(diag.find(".form-horizontal"), projectname)){
            return;
        }
        diag.modal("hide");
    });
}

function SaveProjectToImage(projectname) 
{ 
    var info = '';
    var diag = $("#idx_confirmDlg");
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    
    //是否只包含已经部署的虚拟机    
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.display["vm_type"] + '</label>'
    info += '           <div class="controls">';
    info += '               <input data-no-uniform="true" checked type="checkbox" class="iphone-toggle rdis_deployed_only">';
    info += '            </div>';
    info += '       </div>';
    
    //是否包含虚拟机镜像
    info += '       <div class="control-group vt_is_public">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.display["include_vm_image"] + '</label>'
    info += '           <div class="controls">';
    info += '               <input data-no-uniform="true" checked type="checkbox" class="iphone-toggle rdis_image_save">';
    info += '            </div>';
    info += '       </div>'; 
    
    //工程镜像描述信息
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["desc"] + '</label>';
    info += '           <div class="controls">';
    info += '               <textarea id="taDesc" class="autogrow OamUi" id="taDesc" maxlength="128" rows="6" bindattr="val" bind="description"></textarea><p id="pSetDescInfo"></p>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '   </fieldset>';
    info += '</form>';
    
    var header = '<i class="icon icon-blue icon-save"></i>&nbsp;' + page.res[page.pagelan].type.opr["saveasimage"] + '-' + projectname;
    diag.find('div.modal-header h5').html(header);
    diag.find('div.modal-body').html(info);
    diag.modal("show"); 
    
    LimitCharLenInput($("#taDesc"),$("#pSetDescInfo"));
    //iOS / iPhone style toggle switch
    diag.find('.rdis_deployed_only').iphoneStyle({
        checkedLabel: page.res[page.pagelan].type.ExportVMType["0"],
        uncheckedLabel: page.res[page.pagelan].type.ExportVMType["1"]
    });
     
    //iOS / iPhone style toggle switch
    diag.find('.rdis_image_save').iphoneStyle({
        checkedLabel: page.res[page.pagelan].type.YesOrNo["1"],
        uncheckedLabel: page.res[page.pagelan].type.YesOrNo["0"]
    });
     
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){ 
        var obj_saveprojectdata = new Object();
        var is_deployed_only = diag.find('.rdis_deployed_only').attr("checked") == "checked" ? false : true;
        var is_image_save    = diag.find('.rdis_image_save').attr("checked") == "checked" ? true : false;
        
        obj_saveprojectdata.project_name = projectname;
        obj_saveprojectdata.is_deployed_only = is_deployed_only; 
        obj_saveprojectdata.is_image_save = is_image_save; 
        obj_saveprojectdata.description = $("#taDesc").val();
        //调用RPC接口
        var rs = xmlrpc_service.tecs.project.save_as_image(xmlrpc_session, obj_saveprojectdata);
        if(rs[0] != 0){
            ShowNotyRpcErrorInfo(rs);
            return false;
        }
        else {
            $(oMyVmProjectDbt.fnGetNodes()).find(":checkbox").each(function(){
                var obj = $(this).data("item");
                if(obj!=null && obj.project == projectname){
                    obj.workflow = rs[1];
                    $(this).data("item", obj);
                }
            });    

            ShowProjectProgress();      
        }
        diag.modal("hide");
    });
}

function CloneProj(projectname){
     var diag = $('#idx_confirmDlg');
    var info = '';
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    
    info += '       <div class="control-group name">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["name"] + '<font color="red">*</font></label>';
    info += '           <div class="controls">';
    info += '               <input id="txCloneProName" type="text" maxlength="32" class="OamUi" bindattr="val" bind="name">';
    info += '               <span class="help-inline">' + page.res[page.pagelan].type.err_info["text_input_error"] + '</span>';
    info += '            </div>';
    info += '       </div>';
    
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["desc"] + '</label>';
    info += '           <div class="controls">';
    info += '               <textarea id="taCloneDesc" class="autogrow OamUi"  maxlength="128" rows="6" bindattr="val" bind="des"></textarea><p id="pSetDescInfo"></p>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '   </fieldset>';
    info += '</form>';
    
    var header = '<i class="icon icon-blue icon-copy"></i>&nbsp;' + page.res[page.pagelan].type.opr["clone"];
    diag.find('div.modal-header h5').html(header);
    diag.find('div.modal-body').html(info); 
    z_strInput("txCloneProName",255);
    LimitCharLenInput($("#taCloneDesc"),$("#pSetDescInfo"));
    diag.modal("show"); 
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        var projName = $("#txCloneProName").val();
        var projDesc = $("#taCloneDesc").val();
        var rs = xmlrpc_service.tecs.project.clone(xmlrpc_session, projectname, projName,projDesc );
        diag.modal("hide");
        if(rs[0] == 0){
            ShowNetSynRpcInfo(rs);
            Refresh_MyVM_Project_Data();
        }
        else {
            ShowNotyRpcErrorInfo(rs);
            return false;
        }             
    });
}

var currMyvmProjectState = new Object();
function Refresh_MyVM_Project_Data(){
    if($('#tbProject').length == 0) {
        clearInterval(myvm_project_timer_id);
        return;
    }
    ClearCurSelItem(currMyvmProjectState);
    RecordSelItem(oMyVmProjectDbt, "tbProject", currMyvmProjectState);  
    GotoCurrentPageOfDataTable(oMyVmProjectDbt);

}

function OnSelectAll(){
    if($(this).attr("checked") == "checked"){
        $("#tbProject tbody input[type=checkbox]").attr("checked", true);
    }
    else{
        $("#tbProject tbody input[type=checkbox]").attr("checked", false);
    }
}

$(function(){
    oMyVmProjectDbt = $('#tbProject').dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] },{ "bSortable": false, "aTargets": [ 2 ] },{ "bSortable": false, "aTargets": [ 4 ] }],
        "alternatively" : "destory",
        "bProcessing": false,
        "bServerSide": true,
        "sAjaxSource": "../php/myvm_project.php",
        "fnServerData": RetrieveVrProjectData
    } );
    //tabs
    $('#tabsProject a:first').tab('show');
    $('#tabsProject a').click(function (e) {
      e.preventDefault();
      $(this).tab('show');
    });
    $('.modal').modal({
        backdrop:"static",
        keyboard:false,
        "show":false
    });
    LimitCharLenInput($("#taDesc"),$("#pSetProjInfo"));
    // OamUi静态显示部分初始化
    $("OamStaticUI").each(function(){
        SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
    });

    if(userdata.role != 1){
        $(".breadcrumb").hide();
    }
    if(project_pg_id == null){
        project_pg_id = setInterval(ShowProjectProgress,1000);
    } 
    if(myvm_project_timer_id == null){    
        myvm_project_timer_id = setInterval(Refresh_MyVM_Project_Data, 6000);
    }    
    $("#SelecltAll").click(OnSelectAll);
    $("#refresh").click(Refresh_MyVM_Project_Data); 
    $("#remove").click(remove);
    $("#create").click(create);
    $("#deployProj").click(deployProj);
    $("#cancelDeployProj").click(cancelDeployProj);
    $("#rebootProj").click(rebootProj);
    $("#force_rebootProj").click(forceRebootProj);

    $('ul.breadcrumb li a').click(Switch_Vr_Page);
});