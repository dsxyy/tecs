var oVrImageDtb;
var oVrDeployImageDtb;
var vrprogressBarID = null;

function ShowDetail(img_id){
    var diag = $("#idx_detailDlg");
    ShowImageDetail(diag, img_id);
}

function OnModifyClick(imgid){
    var diag = $("#idx_confirmDlg");
    ShowModifyImageDiag(diag, imgid, RefreshData);
}

function ClusterNameChange(cluster, img_id, diag){
    var sSource = "../php/vr_get_deployimage_count_by_cluster.php"; 
    var aoData  = { cluster_name: cluster,image_id:img_id};
    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data; 
        if(json.errcode != ""){ 
            ShowNotyQueryDBErrorInfo(json);
        }
        if(ackdata.aaData.length > 0){
            var count = parseInt(ackdata.aaData[0][0]);
            if(count > 0){
                diag.find(".duplicate").show();
            }
            else{
                diag.find(".duplicate").hide();
            }
        }
    });
}

function BindDeployCluster(sl){
    sl.empty();
    var mode = 0;
    var rs = TecsObjList("cluster", 1, mode);
    if(rs == null || rs.length == 0){
        return;
    }
    for(var i = 0;i < rs.length;i++){
        var sSource = "../php/vr_storage_cluster_count.php"; 
        var aoData  = { cluster_name: rs[i]["cluster_name"]};
        $.getJSON( sSource, aoData, function (json) {
            var ackdata = json.data; 
            if(json.errcode != ""){
                ShowErrorInfo(json.errcode, GetErrStrOfRes(json.errinfo,top.userData.language,top.errData));
            }
            var count = parseInt(ackdata.aaData[0][0])
            if(count > 0){
                sl.append("<option value="+ ackdata.aaData[0][1] +">"+ ackdata.aaData[0][1]);// +"</option>");
            }
        });
    }
}

function OnDeployClick(imgid){
    var diag = $("#idx_confirmDlg"); 
    var info = '';
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    
    info += '       <div class="control-group cluster">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["cluster"] +  '</label>'
    info += '           <div class="controls">';
    info += '               <Select id="slDeployClustername" class="OamUi" bindattr="val" bind="cluster"></Select>';
    info += '               <span class="help-inline">' + page.res[page.pagelan].type.err_info["image_deploy_cluster_desc"] + '</span>';
    info += '            </div>';
    info += '       </div>';
    
    /*
    info += '       <div class="control-group duplicate ">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["duplicate"] + '</label>'
    info += '           <div class="controls">';
    info += '               <input data-no-uniform="true" checked type="checkbox" class="iphone-toggle rdisduplicate">';
    info += '            </div>';
    info += '       </div>'; 
    */
    
    info += '   </fieldset>';
    info += '</form>';
    
    var header = '<i class="icon-edit"></i>&nbsp;' + page.res[page.pagelan].type.opr["deploy"] + '-' + imgid;
    diag.find('div.modal-header h5').html(header);
    diag.find('div.modal-body').html(info);
    diag.modal("show");
    //iOS / iPhone style toggle switch
    /*diag.find('.rdisduplicate').iphoneStyle({
        checkedLabel: page.res[page.pagelan].type.YesOrNo["1"],
        uncheckedLabel: page.res[page.pagelan].type.YesOrNo["0"]
    });*/
    diag.find(".duplicate").hide(); 
    BindDeployCluster($("#slDeployClustername"));
    diag.find('#slDeployClustername').unbind('change').bind('change',function(){
        ClusterNameChange(diag.find("#slDeployClustername").attr("value"), imgid, diag);
    });
    ClusterNameChange(diag.find("#slDeployClustername").attr("value"), imgid, diag);
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        diag.find(".cluster").removeClass("error");
        var form_data = GetModifyForm(diag.find("form.form-horizontal")); 
        if(form_data["cluster"]==null || form_data["cluster"] == ""){
            diag.find(".cluster").addClass("error");
            return false;
        }
        //var isduplicate = diag.find('.rdisduplicate').attr("checked") == "checked" ? true : false;
        var rs = xmlrpc_service.tecs.image["deploy"](xmlrpc_session, new I8(imgid), form_data["cluster"]/*, isduplicate*/);
        if(rs[0] != 0 && rs[0] != 4005){
            ShowNotyRpcErrorInfo(rs);
            return;
        }
        else{
            /*$(oVrImageDtb.fnGetNodes()).find(":checkbox").each(function(){
                var obj = $(this).data("item");
                if(obj!=null && obj.img_id == imgid){
                    obj.workflow = rs[1];
                    $(this).data("item", obj);
                }
            });*/
        }
        diag.modal("hide");
        
        RefreshDeployImageData();
        return true;
    });
    
}

function RetrieveVrImageData( sSource, aoData, fnCallback ){
    var user_name = $("#selectAllImageUser").val() == null ? "" : $("#selectAllImageUser").val();
    user_name = GetFrmtStrOfMultiSelect(user_name);
    var language  = page.pagelan;
    //$("#tbImage input[type=checkbox]").attr("checked", false);
    aoData.push( { "name": "user_name", "value": user_name} );
    aoData.push( { "name": "language",  "value": language} );

    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
		var vrData = [];
        var imgs_id = []; 
        if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
        }
        for(var i = 0;i < ackdata.aaData.length;i++){
            imgs_id[i]    = ackdata.aaData[i][1];			
			var vrobj     = new Object();
            vrobj.img_id  = ackdata.aaData[i][1];
            vrobj.workflow = ackdata.aaData[i]["workflow"];
            vrData.push(vrobj);
        }
        for(var i = 0;i < ackdata.aaData.length;i++){
            //var chklink = '<input type="checkbox" value=' + '"' + imgs_id[i] + '" >';
            ackdata.aaData[i][0] = GetItemCheckbox(imgs_id[i], currImageState);
            var detaillink = '<a href="javascript:void(0);" onclick="ShowDetail(' + "'" + imgs_id[i] + "'" + ')">' + imgs_id[i] + '</a>';
            ackdata.aaData[i][1] = detaillink;
            ackdata.aaData[i][3] = getLocalTime(json.data.aaData[i][3]);
            ackdata.aaData[i][7] = page.res[page.pagelan].type.ContainerFormat[ackdata.aaData[i][14]];
            var oplink = '';
            if(userdata.name == ackdata.aaData[i][9]){
                oplink += '<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["modify"] +'" onclick="OnModifyClick('+ "'" + imgs_id[i] + "'" +')"><i class="icon-edit"></i><span></span></a>';
            }
            //已发布+可用+无格式
            if(ackdata.aaData[i][12] == "1" && ackdata.aaData[i][13] == "1" && ackdata.aaData[i][14] == ""){
                if(userdata.name == ackdata.aaData[i][9]){ 
                    oplink += '&nbsp;|&nbsp;';
                }
                oplink += '<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["deploy"] +'" onclick="OnDeployClick('+ "'" + imgs_id[i] + "'" +')"><i class="icon-step-forward"></i><span></span></a>';
            }
            ackdata.aaData[i][11] = oplink;
        }
        fnCallback(ackdata); 
        BindTBImageData(vrData);

        ShowImageDeployProcess();

        ShowLastUpdateTime($("#tbImage").parent());

        $(".tooltip").hide();
        $('[rel="tooltip"],[data-rel="tooltip"]').tooltip({"placement":"bottom"});
        all_image_timer_id=setInterval(RefreshData, 10000);
    });

    
}

function BindDeployImageData(deployimagedata){
    for(var i = 0;i < deployimagedata.aaData.length;i++){
        var cb = $("#tbDeployImage>tbody").find(":checkbox").eq(i);
        cb.data("item", deployimagedata.aaData[i]);
    }
}

function BindTBImageData(imagedata){
    for(var i = 0;i < imagedata.length;i++){
        var cb = $("#tbImage>tbody").find(":checkbox").eq(i);
        cb.data("item", imagedata[i]);
    }
}

function ShowImageDeployProcess() {
    if($('#tbImage').length == 0){
        clearInterval(vrprogressBarID);    
        return;
    }

    ShowWorkflowProgress(oVrImageDtb, RefreshData);
}

/*function UpdateDeployImageProgress(ackdata){
    var vCheckedbox = $("#tbDeployImage>tbody").find(":checkbox");
    vCheckedbox.each(function(){
        var item = $(this).data("item");
        //找到 
        if(item[2] == ackdata[0] &&
           item[1] == ackdata[1]){
            var tr_flag = $(this).parent().parent();
            var nTr = tr_flag[0];
            //非部署状态
            if(ackdata[3] != "0"){
                if (oVrDeployImageDtb.fnIsOpen(nTr)){
                    oVrDeployImageDtb.fnClose(nTr);
                }
                var td_flag = tr_flag.find("td").eq(4);
                td_flag.html(ackdata[4]);
                item[7] = ackdata[3];  
            }
            else{ 
                var progress_id = "pb" + ackdata[0] + ackdata[1];
                var progress = ackdata[2];
                if (!(oVrDeployImageDtb.fnIsOpen(nTr))){
                    var progressbar = GetOpProcessURL(progress_id,progress);
                    oVrDeployImageDtb.fnOpen( nTr, progressbar, 'details' );
                }
                else{//更新进度
                    var old_progress_flag = $("#"+progress_id)[0].className.split(" ")[1];
                    var new_progress_flag = GetOpProgressFlag(progress); 
                    if(old_progress_flag != new_progress_flag){
                        $("#"+progress_id).removeClass();
                        $("#"+progress_id).addClass("progress").addClass(new_progress_flag).addClass("span6").addClass("active");
                    }
                    $("#"+progress_id+" .bar").attr("style","width: " + progress + "%");
                    $("#"+progress_id+" .bar").html(progress + "%");
                }
            }
            return;
        }
    });    
}*/

/*function ShowImageDeployProcess(){
    var vCheckedbox = $("#tbDeployImage>tbody").find(":checkbox");
    var cluster_caches = "";
    var sSource = "../php/vr_get_deployimage_progress.php";
    //获取部署中镜像 
    vCheckedbox.each(function(){
        var item = $(this).data("item");
        if(item[7] == "0"){
            cluster_caches += item[2] + ":" + item[1] + ";";
        }
    }); 
    if(cluster_caches == ""){
        return;
    }
    var lang = $.cookie('current_language') == null ? 'ch' :$.cookie('current_language');
    cluster_caches = cluster_caches.substring(0,cluster_caches.length - 1); 
    var aoData  = { cluster_caches: cluster_caches,language:lang};
    $.getJSON( sSource, aoData, function (json){
        var ackdata = json.data;
        if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
            return;
        }
        for(var i = 0;i < ackdata.aaData.length;i++){
            UpdateDeployImageProgress(ackdata.aaData[i]);
        } 
    });
}*/

function RetrieveDeployImageData( sSource, aoData, fnCallback ){
    var user_name = $("#selectDeployImageUser").val() == null ? "" : $("#selectDeployImageUser").val();
    user_name = GetFrmtStrOfMultiSelect(user_name);
    var language = page.pagelan;
    $("#tbDeployImage input[type=checkbox]").attr("checked", false);
    aoData.push( { "name": "user_name", "value": user_name} ); 
    aoData.push( { "name": "language",  "value": language} ); 
    $.getJSON( sSource, aoData, function (json){
        var ackdata = json.data;
        var cluster_cache = [];
        if(json.errcode != ""){ 
            ShowNotyQueryDBErrorInfo(json);
        }
        for(var i = 0;i < ackdata.aaData.length;i++){
            cluster_cache[i] = ackdata.aaData[i][1] + "$#" + ackdata.aaData[i][2];
        }   
        for(var i = 0;i < ackdata.aaData.length;i++)
        {
            var chklink = '<input type="checkbox" value=' + '"' + cluster_cache[i] + '" uuid="' + ackdata.aaData[i][7] + '">';
            ackdata.aaData[i][0] = chklink;
            var detaillink = '<a href="javascript:void(0);" onclick="ShowDetail(' + "'" + ackdata.aaData[i][3] + "'" + ')">' + ackdata.aaData[i][3] + '</a>';
            ackdata.aaData[i][3] = detaillink;
        }
        fnCallback(ackdata);
        BindDeployImageData(ackdata);
        //ShowImageDeployProcess();
        ShowLastUpdateTime($("#tbDeployImage").parent());
    });    
}

function OnSelectAll(){
    if($(this).attr("checked") == "checked"){
        $("#tbImage tbody input[type=checkbox]").attr("checked", true);
    }
    else{
        $("#tbImage tbody input[type=checkbox]").attr("checked", false);
    }
}

function OnSelectAllDeployImage(){
    if($(this).attr("checked") == "checked"){
        $("#tbDeployImage tbody input[type=checkbox]").attr("checked", true); 
    }
    else{
        $("#tbDeployImage tbody input[type=checkbox]").attr("checked", false);
    }
}

var currImageState = new Object();

function RefreshData(){
    clearInterval(all_image_timer_id);
    if($("#tbImage").length == 0){
        return;
    }
    ClearCurSelItem(currImageState);
    RecordSelItem(oVrImageDtb, "tbImage", currImageState);     
    GotoCurrentPageOfDataTable(oVrImageDtb);
}

function RefreshDeployImageData(){
    oVrDeployImageDtb.fnDraw();
}

function RemoveImageCallback(vCheckedbox){
    ClearAlertInfo();
    vCheckedbox.each(function(){
        var id = new I8($(this).attr("value"));
        var name = $(this).closest("tr").find("td").eq(2).text();
        var result = xmlrpc_service.tecs.image["delete"](xmlrpc_session, id);
        ShowSynRpcInfo(result);
    });
    RefreshData();
}

function remove(){
    ShowBatchOpConfirmDiag($("#idx_confirmDlg"), "tbImage", page.res[page.pagelan].type.err_info["delete_confirm_info"], RemoveImageCallback);
}

function enable(){
    ClearAlertInfo();
    var vCheckedbox = $("#tbImage>tbody").find(":checkbox:checked");
    if(vCheckedbox.size() == 0){
        ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["must_select_one_error"]);
        return;
    }
    vCheckedbox.each(function(){
        var id = new I8($(this).attr("value"));
        var result = xmlrpc_service.tecs.image.enable(xmlrpc_session, id, true);
        ShowSynRpcInfo(result);
    });
    RefreshData();
}

function Disable_Image_Callback(vCheckedbox){
    ClearAlertInfo();
    vCheckedbox.each(function(){
        var id = new I8($(this).attr("value"));
        var result = xmlrpc_service.tecs.image.enable(xmlrpc_session, id, false);
        ShowSynRpcInfo(result);
    });
    RefreshData();
}

function disable(){
    ShowBatchOpConfirmDiag($("#idx_confirmDlg"), "tbImage", page.res[page.pagelan].type.err_info["disable_confirm_info"], Disable_Image_Callback);
}

function publish(){
    ClearAlertInfo();
    var vCheckedbox = $("#tbImage>tbody").find(":checkbox:checked");
    if(vCheckedbox.size() == 0){
        ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["must_select_one_error"]);
        return;
    }
    vCheckedbox.each(function(){
        var id = new I8($(this).attr("value"));
        var name = $(this).closest("tr").find("td").eq(2).text();
        var result = xmlrpc_service.tecs.image.publish(xmlrpc_session, id, true);
        ShowSynRpcInfo(result);
    });
    RefreshData();
}

function Private_Vm_Callback(vCheckedbox){
    ClearAlertInfo();
    vCheckedbox.each(function(){
        var id = new I8($(this).attr("value"));
        var name = $(this).closest("tr").find("td").eq(2).text();
        var result = xmlrpc_service.tecs.image.publish(xmlrpc_session, id, false);
        ShowSynRpcInfo(result);
    });
    RefreshData();
}

function OnPrivate(){
    ShowBatchOpConfirmDiag($("#idx_confirmDlg"), "tbImage", page.res[page.pagelan].type.err_info["nopublish_confirm_info"], Private_Vm_Callback);
}

function Cancel_DeployImage(cluster_name,cache_id, uuid){
    var sSource = "../php/vr_get_imagesnapshot_count_by_cluster.php"; 
    var aoData  = { cluster_name: cluster_name,cache_id:cache_id};
    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        var info = "Base ID" + ":" + cache_id + "," + page.res[page.pagelan].type.field_name["cluster"] + ":" + cluster_name;
        var rsObj = new Object();
        if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
            return;
        }
        var count = 0;
        if(parseInt(ackdata.iTotalRecords) > 0){
            count = parseInt(ackdata.aaData[0][0]);
        }
        if(count == 0){ 
            var rs = xmlrpc_service.tecs.image.cancel(xmlrpc_session, uuid, cluster_name);
            if(rs[0] !=0 && rs[0] != 4005){
                ShowSynRpcInfo(rs);
            }
        } 
        else{
            ;
        }
        RefreshDeployImageData();
    });
}

function CancelDeploy_Vm_Callback(vCheckedbox){
    ClearAlertInfo();
    vCheckedbox.each(function(){
        var item = $(this).data("item");
        var cache_id     = item[1];
        var cluster_name = item[2];
	    var uuid = $(this).attr("uuid");
        if(item["snapshot_count"] == 0){
            Cancel_DeployImage(cluster_name, cache_id, uuid);    
        }
        else {
            ShowSynRpcInfo(["2", "Image(" + uuid + ") has snapshot!"]);
        }
    });
}

function CancelDeployImageClick(){
    ShowBatchOpConfirmDiag($("#idx_confirmDlg"), "tbDeployImage", page.res[page.pagelan].type.err_info["cancel_deploy_confirm_info"], CancelDeploy_Vm_Callback);
} 

function Get_Image_Space_Status_Count(auser_name, piechart, pieinfo){
    var rs = xmlrpc_service.tecs["image"].getspace(xmlrpc_session, 0);

    if(rs[0] == 0){   
        var restSpace = parseInt(rs[1]);
        var usedSpace = parseInt(rs[2]) - restSpace;
            
        var data = [
            { label: page.res[page.pagelan].type.ImageSpace[1],  data: usedSpace },
            { label: page.res[page.pagelan].type.ImageSpace[2],  data: restSpace }
        ];
        if(piechart.length){
            $.plot(piechart, data,
            {
                series: {
                    pie: {
                    show: true
                    }
                },
                legend: { 
                    container: pieinfo,
                    labelFormatter: function(label, series){
                        var formatLabel;
                        var percent = 0;
                        if(!isNaN(series.percent)){
                            percent = Math.floor(series.percent);
                        }
                        formatLabel = label + ' - ' + getformatsize(series.data[0][1]*1024) + 
                            ' (' + percent + '%)';
                        return formatLabel;
                    }
                }
            });
        }       
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }
}

$(window).unbind("resize").resize(function(){
    if($("#image_piechart").is(":visible")){ 
        Get_Image_Space_Status_Count("", $("#image_piechart"), $("#image_space_info"));
    }
});

$(function(){
    Get_Image_Space_Status_Count("", $("#image_piechart"), $("#image_space_info"));
    
    InitSelectUser($("#selectAllImageUser"), $("#txUsername").val(), RefreshData);
    InitSelectUser($("#selectDeployImageUser"), $("#txUsername").val(), RefreshDeployImageData);
    //datatable

    oVrImageDtb = $('#tbImage').dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] }, { "bSortable": false, "aTargets": [ 10 ] }, { "bSortable": false, "aTargets": [ 11 ] }],
        "alternatively" : "destory",
        "bProcessing": false,
        "bServerSide": true,
        "sAjaxSource": "../php/vr_image.php",
        "fnServerData": RetrieveVrImageData
    } );
   
    oVrDeployImageDtb = $('#tbDeployImage').dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] }, { "bSortable": false, "aTargets": [ 6 ] }],
        "alternatively" : "destory",
        "bProcessing": false,
        "bServerSide": true,
        "sAjaxSource": "../php/vr_deployimage.php",
        "fnServerData": RetrieveDeployImageData
    }); 
    
    
    // OamUi静态显示部分初始化
    $("OamStaticUI").each(function(){
        SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
    });
    $(".OamStaticUI").each(function(){
        SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
    });
    
    //tabs
    if($("#txUsername").val() != ""){
        $('#imageTab a:eq(1)').tab('show');
    }
    else{
        $('#imageTab a:eq(0)').tab('show');
    }
    $('#imageTab a').click(function (e) {
        e.preventDefault(); 
        $(this).tab('show');
        RefreshDeployImageData();
    });
    //Get_Image_Space_Status_Count("", $("#image_piechart"), $("#image_space_info"));
    $('#imageTab a:first').click(function(e){
        Get_Image_Space_Status_Count("", $("#image_piechart"), $("#image_space_info"));
    });
    $("#SelecltAll").click(OnSelectAll);
    $("#SelecltDeployImageAll").click(OnSelectAllDeployImage);
    $("#refresh").click(RefreshData);
    $("#remove").click(remove);
    $("#enable").click(enable);
    $("#disable").click(disable);
    $("#public").click(publish);
    $("#private").click(OnPrivate);
    $("#upload").click(Upload_Image);
    $("#cancel_deployimage").click(CancelDeployImageClick); 
    $("#refresh_deployimage").click(RefreshDeployImageData);
    
	if(vrprogressBarID == null){
    	vrprogressBarID = setInterval(ShowImageDeployProcess, 1000);
    }
    $('ul.breadcrumb li a').click(Switch_Vr_Page);
});
