var oVrVtDtb;

function RetrieveVrVtData( sSource, aoData, fnCallback ){
    var user_name = $("#selectUser").val() == null ? "" : $("#selectUser").val();
    user_name = GetFrmtStrOfMultiSelect(user_name);
    var language    = page.pagelan;
    //$("#tbVt input[type=checkbox]").attr("checked", false);
    aoData.push( { "name": "user_name", "value": user_name} );
    aoData.push( { "name": "language",  "value": language} );
    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        var vts_name = [];
        if(json.errcode != ""){ 
            ShowNotyQueryDBErrorInfo(json);
        }
        for(var i = 0;i < ackdata.aaData.length;i++){
            vts_name[i] = ackdata.aaData[i][1];
        }
        for(var i = 0;i < ackdata.aaData.length;i++){
           // var chklink = '<input type="checkbox" value=' + '"' + vts_name[i] + '" >';
            //ackdata.aaData[i][0] = chklink;
            ackdata.aaData[i][0] = GetItemCheckbox(vts_name[i] , currVrVtState);
            var morelink = '<a href="javascript:void(0);"  onclick="onMoreVtClick(';
            morelink     += "'" + "idx_detailDlg" + "',";
            morelink     += "'" + vts_name[i] + "'";
            morelink     += ')">' + vts_name[i] + '</a>';
            ackdata.aaData[i][1] = morelink;
            var oplink = '<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["modify"] +'" onclick="Modidy_Vt('+ "'" + vts_name[i] + "'" +')"><i class="icon-edit"></i><span></span></a>'; 
            ackdata.aaData[i][8] = oplink;
        }
        fnCallback(ackdata);
       
        ShowLastUpdateTime($("#tbVt").parent());

        $(".tooltip").hide();
        $('[rel="tooltip"],[data-rel="tooltip"]').tooltip({"placement":"bottom"});
    vr_vt_timer_id=setInterval(RefreshData, 6000);
    });
    
}

function Modidy_Vt(vid){
    $('[rel="tooltip"],[data-rel="tooltip"]').tooltip("hide");
    Load_Page($('#content'),"vm_config.php?action=4&vid=" + vid + "&level=1");
}

function RemoveVtCallback(vCheckedbox){
    ClearAlertInfo();
    vCheckedbox.each(function(){
        var vt_name = $(this).attr("value");
        var result = Delete(vt_name);
        ShowSynRpcInfo(result);
    });
    oVrVtDtb.fnDraw();
}

function remove(){
    ShowBatchOpConfirmDiag($("#idx_confirmDlg"), "tbVt", page.res[page.pagelan].type.err_info["delete_confirm_info"], RemoveVtCallback);
}

function PublishVt(){
    ClearAlertInfo();
    var vCheckedbox = $("#tbVt>tbody").find(":checkbox:checked");
    if(vCheckedbox.size() == 0){
        ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["must_select_one_error"]);
        return;
    }
    vCheckedbox.each(function(){
        var vt_name = $(this).attr("value");
        var result = Publish(vt_name);
        ShowSynRpcInfo(result);
    });
    oVrVtDtb.fnDraw();
}

function NoPublishVt_Callback(vCheckedbox){
    ClearAlertInfo();
    vCheckedbox.each(function(){
        var vt_name = $(this).attr("value");
        var result = NoPublish(vt_name);
        ShowSynRpcInfo(result);
    });
    oVrVtDtb.fnDraw();
}

function NoPublishVt(){
    ShowBatchOpConfirmDiag($("#idx_confirmDlg"), "tbVt", page.res[page.pagelan].type.err_info["nopublish_vt_confirm_info"], NoPublishVt_Callback);
}

function Delete(vt_name){ 
    var rs = xmlrpc_service.tecs.vmtemplate["delete"](xmlrpc_session,vt_name);
    return rs;
}

function Publish(vt_name)
{
    return xmlrpc_service.tecs.vmtemplate.publish(xmlrpc_session,vt_name,true);
}

function NoPublish(vt_name)
{
    return xmlrpc_service.tecs.vmtemplate.publish(xmlrpc_session,vt_name,false);
}

var currVrVtState = new Object();
function RefreshData(){
    clearInterval(vr_vt_timer_id);
    if($('#tbVt').length == 0) {
        return;
    }
    ClearCurSelItem(currVrVtState);
    RecordSelItem(oVrVtDtb, "tbVt", currVrVtState);  
    GotoCurrentPageOfDataTable(oVrVtDtb);
    //oVrVtDtb.fnDraw();
} 

function OnSelectAll(){
    if($(this).attr("checked") == "checked"){
        $("#tbVt tbody input[type=checkbox]").attr("checked", true);
    }
    else{
        $("#tbVt tbody input[type=checkbox]").attr("checked", false);
    }
}

function OnCreateVtClick(){
    if(!CheckIsImageOfCreateVm()){
        return false;
    }
    Load_Page($('#content'),"vm_config.php?action=3" + "&level=1");
}

$(function(){
    InitSelectUser($("#selectUser"), $("#txUsername").val(), RefreshData);
    //datatable
    oVrVtDtb = $('.datatable').dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] }, { "bSortable": false, "aTargets": [ 8 ] }],
        "alternatively" : "destory",
        "bProcessing": false,
        "bServerSide": true,
        "sAjaxSource": "../php/vr_vt.php",
        "fnServerData": RetrieveVrVtData
    } );
    $('.modal').modal({
        backdrop:"static",
        keyboard:false,
        "show":false
    });
    //tabs
    $('#tabsVrvt a:first').tab('show');
    $('#tabsVrvt a').click(function (e) {
      e.preventDefault();
      $(this).tab('show');
    });
    // OamUi静态显示部分初始化
    $("OamStaticUI").each(function(){
        SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
    }); 
    $("#SelecltAll").click(OnSelectAll);
    $("#create").click(OnCreateVtClick);
    $("#remove").click(remove);
    $("#public").click(PublishVt);
    $("#private").click(NoPublishVt);
    $("#refresh").click(RefreshData);
    $('ul.breadcrumb li a').click(Switch_Vr_Page);
});
