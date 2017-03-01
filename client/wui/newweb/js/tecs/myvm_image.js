var oSystemImageTable;
var oMyImageTable;

function ShowDetail(img_id){
    var diag = $("#idx_detailDlg");
    ShowImageDetail(diag, img_id);
}

function OnModifyClick(imgid){
    var diag = $("#idx_confirmDlg");
    ShowModifyImageDiag(diag, imgid, RefreshMyImageData); 
}

function AddProjectByImage(imgid){
    var info = '';
    var diag = $("#idx_confirmDlg");
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    
    //工程名称    
    info += '       <div class="control-group dvproject_name">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["name"] + '</label>'
    info += '           <div class="controls">';
    info += '               <input id="txProjectname" type="text" class="OamUi" bindattr="val" bind="project_name">';
    info += '               <span class="help-inline">' + page.res[page.pagelan].type.err_info["text_input_error"] + '</span>';
    info += '            </div>';
    info += '       </div>';
    
    info += '   </fieldset>';
    info += '</form>';
    
    var header = '<i class="icon-edit"></i>&nbsp;' + page.res[page.pagelan].type.opr["create_project"] + '-' + imgid;
    diag.find('div.modal-header h5').html(header);
    diag.find('div.modal-body').html(info);
    z_strInput("txProjectname",255);
    diag.modal("show");
     
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){ 
        diag.find(".form-horizontal").find(".dvproject_name").removeClass("error");
        if($("#txProjectname").val() == ""){
            diag.find(".form-horizontal").find(".dvproject_name").addClass("error");
            diag.find(".form-horizontal").find(".dvproject_name .help-inline").html(page.res[page.pagelan].type.err_info["text_input_error"]);
            return;
        }
        var obj_addproject_data = new Object();
        obj_addproject_data.image_id = new I8(imgid);
        obj_addproject_data.project_name = $("#txProjectname").val();
        //调用RPC接口
        var rs = xmlrpc_service.tecs.project.create_by_image(xmlrpc_session, obj_addproject_data);
        if(rs[0] != 0){
            ShowNotyRpcErrorInfo(rs);
            return false;
        }
        diag.modal("hide");
    });
}

function RemoveMyImage_Callback(vCheckedbox){
    ClearAlertInfo();
    vCheckedbox.each(function(){
        var id = new I8($(this).attr("value"));
        var name = $(this).closest("tr").find("td").eq(2).text();
        var result = xmlrpc_service.tecs.image["delete"](xmlrpc_session, id);
        ShowSynRpcInfo(result);
    });
    RefreshMyImageData();
}

function remove(){
    ClearAlertInfo();
    var vCheckedbox = $("#tbMyImage>tbody").find(":checkbox:checked");
    if(vCheckedbox.size() == 0){
        ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["must_select_one_error"]);
        return;
    }
    ShowBatchOpConfirmDiag($("#idx_confirmDlg"), "tbMyImage", page.res[page.pagelan].type.err_info["delete_confirm_info"], RemoveMyImage_Callback);
}

function RefreshSystemImageData(){
    if($('#tbSystemImage').length == 0) {
        clearInterval(sysimage_timer_id);
        return;
    }
    oSystemImageTable.fnDraw();
}

function RetrieveSystemImageData( sSource, aoData, fnCallback ){
    var language = page.pagelan;
    aoData.push( { "name": "language",  "value": language} ); 
    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
            return;
        }

        var imgs = [];        
        for(var i = 0;i < ackdata.aaData.length;i++){
            var oneimage = [];
            oneimage["image_id"] = ackdata.aaData[i][0];
            oneimage["workflow"] = ackdata.aaData[i]["workflow"];
            imgs.push(oneimage);
        }

        for(var i = 0;i < ackdata.aaData.length;i++){
            var detaillink = '<a href="javascript:void(0);" onclick=ShowDetail('+"'"+ imgs[i].image_id + "'" + ')>' + imgs[i].image_id +'</a>';
            ackdata.aaData[i][0] = detaillink;
            ackdata.aaData[i][0] += '<input type="checkbox" style="display:none;" />';
            ackdata.aaData[i][2] = getLocalTime(json.data.aaData[i][2]);
            ackdata.aaData[i][5] = page.res[page.pagelan].type.ContainerFormat[ackdata.aaData[i][9]];
            var oprlink = '';
            //oprlink = '<a href="javascript:void(0);">'+ page.res[page.pagelan].type.opr["create_vm"] + '</a>';
            if(ackdata.aaData[i][9] != ""){
                oprlink += '<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["create_project"] +'" onclick="AddProjectByImage('+ "'" + imgs[i].image_id + "'" +')"><i class="icon-plus"></i><span></span></a>';
            }
            ackdata.aaData[i][9] = oprlink;
        }
        fnCallback(ackdata);

        BindTableCheckboxData($("#tbSystemImage"), imgs);

        ShowLastUpdateTime($("#tbSystemImage").parent());
    });
}

var currMyImageState = new Object();
function RefreshMyImageData(){
    if($('#tbMyImage').length == 0) {
        clearInterval(image_timer_id);
        return;
    }
    ClearCurSelItem(currMyImageState);  
    RecordSelItem(oMyImageTable, "tbMyImage", currMyImageState);   
    GotoCurrentPageOfDataTable(oMyImageTable);
}

function RetrieveMyImageData( sSource, aoData, fnCallback ){
    var language = page.pagelan;
    aoData.push( { "name": "user_name",  "value": userdata.name} );
    aoData.push( { "name": "language",  "value": language} ); 
    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
            return;
        }

        var imgs = [];
        for(var i = 0;i < ackdata.aaData.length;i++){
            var oneimage = [];
            oneimage["image_id"] = ackdata.aaData[i][1];
            oneimage["workflow"] = ackdata.aaData[i]["workflow"];
            imgs.push(oneimage);
        }
        
        for(var i = 0;i < ackdata.aaData.length;i++){
            ackdata.aaData[i][0] = GetItemCheckbox(imgs[i].image_id , currMyImageState);
            var detaillink = '<a href="javascript:void(0);" onclick=ShowDetail('+"'"+ imgs[i].image_id + "'" + ')>' + imgs[i].image_id +'</a>';
            ackdata.aaData[i][1] = detaillink;
            ackdata.aaData[i][3] = getLocalTime(json.data.aaData[i][3]);
            var oplink = '<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["modify"] +'" onclick="OnModifyClick('+ "'" + imgs[i].image_id + "'" +')"><i class="icon-edit"></i><span></span></a>';
            //container_format不为空
            ackdata.aaData[i][7] = page.res[page.pagelan].type.ContainerFormat[ackdata.aaData[i][11]];
            if(ackdata.aaData[i][11] != ""){
                oplink += '&nbsp;|&nbsp;' + '<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["create_project"] +'" onclick="AddProjectByImage('+ "'" + imgs[i].image_id + "'" +')"><i class="icon-plus"></i><span></span></a>';
            }
            ackdata.aaData[i][11] = oplink;
        }
        fnCallback(ackdata); 

        BindTableCheckboxData($("#tbMyImage"), imgs);

        ShowLastUpdateTime($("#tbMyImage").parent());

        $(".tooltip").hide();
        $('[rel="tooltip"],[data-rel="tooltip"]').tooltip({"placement":"bottom"});
    });
}

function OnSelectMyImageAll(){
    if($(this).attr("checked") == "checked"){
        $("#tbMyImage tbody input[type=checkbox]").attr("checked", true);
    }
    else{
        $("#tbMyImage tbody input[type=checkbox]").attr("checked", false);
    }
}

function ShowImageProcess(){
    if($("#tbMyImage").length == 0){
        clearInterval(image_process_timer);
        return;
    }

    ShowWorkflowProgress(oMyImageTable, RefreshMyImageData);
    ShowWorkflowProgress(oSystemImageTable, RefreshSystemImageData);
}

$(function(){
    //datatable
    oMyImageTable = $('#tbMyImage').dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] },{ "bSortable": false, "aTargets": [ 10 ] },{ "bSortable": false, "aTargets": [ 11 ] } ],
        "alternatively" : "destory",
        "bProcessing": false,
        "bServerSide": true,
        "sAjaxSource": "../php/myvm_myimage.php",
        "fnServerData": RetrieveMyImageData
        
    });
    oSystemImageTable = $('#tbSystemImage').dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 8 ] },{ "bSortable": false, "aTargets": [ 9 ] }],
        "alternatively" : "destory",
        "bProcessing": false,
        "bServerSide": true,
        "sAjaxSource": "../php/myvm_sysimage.php",
        "fnServerData": RetrieveSystemImageData
        
    });
    $('.modal').modal({
        backdrop:"static",
        keyboard:false,
        "show":false
    });

    if(userdata.role != 1){
        $(".breadcrumb").hide();
    }

    // OamUi静态显示部分初始化
    $("OamStaticUI").each(function(){
        SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
    });
    $(".OamStaticUI").each(function(){
        SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
    }); 

    //tabs
    $('#imageTab a:first').tab('show');
    $('#imageTab a').click(function (e) {
      e.preventDefault();
      $(this).tab('show');
    });

    image_process_timer = setInterval(ShowImageProcess,1000);

    image_timer_id    = setInterval(RefreshMyImageData, 10000);
    sysimage_timer_id = setInterval(RefreshSystemImageData, 10000);

    LimitCharLenInput($("#taSetDesc"),$("#pSetImageInfo"));

    $("#systemimage_refresh").click(RefreshSystemImageData);
    $("#myimage_refresh").click(RefreshMyImageData);
    $("#SelecltMyImageAll").click(OnSelectMyImageAll);
    $("#myimage_remove").click(remove);
    $("#myimage_upload").click(Upload_Image);
    $('ul.breadcrumb li a').click(Switch_Vr_Page);
});

