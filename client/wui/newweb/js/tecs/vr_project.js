var oVrProjectDbt;

function RetrieveVrProjectData( sSource, aoData, fnCallback ){
    var user_name = $("#selectUser").val() == null ? "" : $("#selectUser").val();
    user_name = GetFrmtStrOfMultiSelect(user_name);
    //$("#tbVrProject input[type=checkbox]").attr("checked", false);
    aoData.push( { "name": "user_name", "value": user_name} );

    $.getJSON( sSource, aoData, function (json) {
        var prodata = json.data;
        var projs_name = [];
        var vmcounts   = [];
        if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
            return;
        }
        for(var i = 0;i < prodata.aaData.length;i++){
            projs_name[i] = prodata.aaData[i][1] + "&" + prodata.aaData[i][4];
            vmcounts[i]   = prodata.aaData[i][3];
        }
        //绑定checkbox
        for(var i = 0;i < prodata.aaData.length;i++){
            //prodata.aaData[i][0] = '<input type="checkbox" value=' + '"' + projs_name[i] + '" >';
            prodata.aaData[i][0] = GetItemCheckbox(projs_name[i] , currVrProjectState);
            var project_name = projs_name[i].split("&")[0];
            var user_name    = projs_name[i].split("&")[1];
            var vmcountlink  = vmcounts[i];
            if(vmcounts[i] != "0"){
                var urldata = "vr_vm.php?user_name=" + user_name + "&project_name=" + project_name;
                vmcountlink = '<a href="javascript:void(0);" onclick="Switch_Vr_Page(' + "'" + urldata + "'"  + ')">' + vmcounts[i] + '</a>';
            }
            prodata.aaData[i][3] = vmcountlink;
        }
        fnCallback(prodata);
        ClearCurSelItem(currVrProjectState);
        ShowLastUpdateTime($("#tbVrProject").parent());
    });
    vr_project_timer_id=setInterval(RefreshData, 6000);
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
    oVrProjectDbt.fnDraw();
}

function remove(){
    ShowBatchOpConfirmDiag($("#idx_confirmDlg"), "tbVrProject", page.res[page.pagelan].type.err_info["delete_confirm_info"], RemoveProjectCallback);
}

var currVrProjectState = new Object();
function RefreshData(){
    clearInterval(vr_project_timer_id);
    if($('#tbVrProject').length == 0) {
        return;
    }
    RecordSelItem(oVrProjectDbt, "tbVrProject", currVrProjectState);  
    GotoCurrentPageOfDataTable(oVrProjectDbt);
} 

function test(){
    for(var i = 0;i < $("#selectError1").find('option').length;i++){
        $("#selectError1").find('option').eq(i).removeAttr("selected");
    }
    $("#selectError1").trigger("liszt:updated");
}

function OnSelectAll(){
    if($(this).attr("checked") == "checked"){
        $("#tbVrProject tbody input[type=checkbox]").attr("checked", true);
    }
    else{
        $("#tbVrProject tbody input[type=checkbox]").attr("checked", false);
    }
} 

$(function(){
    InitSelectUser($("#selectUser"), $("#txUsername").val(), RefreshData);
    //datatable
    oVrProjectDbt = $('.datatable').dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] },{ "bSortable": false, "aTargets": [ 2 ] }],
        "alternatively" : "destory",
        "bProcessing": false,
        "bServerSide": true,
        "sAjaxSource": "../php/project.php",
        "fnServerData": RetrieveVrProjectData
    } );
    //tabs
    $('#tabsProject a:first').tab('show');
    $('#tabsProject a').click(function (e) {
      e.preventDefault();
      $(this).tab('show');
    });
    // OamUi静态显示部分初始化
    $("OamStaticUI").each(function(){
        SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
    });  
    
    $("#SelecltAll").click(OnSelectAll);
    $("#remove").click(remove);
    $("#refresh").click(RefreshData);
    $('ul.breadcrumb li a').click(Switch_Vr_Page);
});
