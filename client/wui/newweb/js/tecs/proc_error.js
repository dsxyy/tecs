function LoadErrorFile(){
    var spath = "./res/errcode.res";
    return Load_ResFile(spath);
}

function GetErrStr(err_code){
    var err_info = "";
    /*if(err_code == ""){
        return err_info;
    }*/
    for(var i=0;i<errData.length;i++){
    	if((err_code) == errData[i].id){
    		if(page.pagelan == "ch")
    			err_info = errData[i].des_ch;
    		else
    			err_info = errData[i].des_en;
    		break;
    	}
    }
    if(err_info == ""){
        if(page.pagelan == "ch")
    	    err_info = "未知错误:" + err_code;
        else
    	    err_info = "Unknown error:" + err_code;
    }
    return err_info;
}

function GetErrStrByLang(lang, err_code, err_data){
    var err_info = "";
    if(err_code == ""){
        return err_info;
    }
    for(var i=0;i<err_data.length;i++){
    	if((err_code) == err_data[i].id){
    		if(lang == "ch")
    			err_info = err_data[i].des_ch;
    		else
    			err_info = err_data[i].des_en;
    		break;
    	}
    }
    if(err_info == ""){
        if(lang == "ch")
    	    err_info = "未知错误:" + err_code;
        else
    	    err_info = "Unknown error:" + err_code;
    }
    return err_info;
}

//----------------------------------------------
function ShowErrorInfo(err_id, err_msg, idx_pos){
    var desc = GetErrStr(err_id);
    ShowErrorAlertInfo(desc, err_msg, idx_pos);
}

//----------------------------------------------
function ShowErrorInfoNoClose(err_id, err_msg, idx_alert){
    var desc = GetErrStr(err_id);
    var alertinfo = "<strong>" + desc + "</strong> " + err_msg;
    $(".alert").removeClass("alert-info").addClass("alert-error").html(alertinfo);
}

//----------------------------------------------
function ShowAlertInfo(isSuccess,err_msg1, err_msg2, idx_pos){
    var info = "";
    if(!isSuccess){
        info = '<div class="alert alert-error">';
    }
    else{
        info = '<div class="alert alert-success">';
    }
    info += '<button type="button" class="close" data-dismiss="alert">×</button><strong>' + err_msg1 + '</strong> ' + err_msg2 + '</div>';
    idx_pos.after(info);
}

//----------------------------------------------
function ShowErrorAlertInfo(err_msg1, err_msg2, idx_pos){
    $(".alert-error").remove();
    ShowAlertInfo(false,err_msg1, err_msg2, idx_pos);
}

//----------------------------------------------
function ShowSuccessAlertInfo(err_msg1, err_msg2, idx_pos){
    $(".alert-success").remove();
    ShowAlertInfo(true,err_msg1, err_msg2, idx_pos);
}

//----------------------------------------------
function ShowBatchAlertInfo(isSuccess,err_msg, idx_pos){
    var info = "";
    if(err_msg == ""){
        return;
    }
    if(!isSuccess){
        info = '<div class="alert alert-error">';
    }
    else{
        info = '<div class="alert alert-success">';
    }
    info += '<button type="button" class="close" data-dismiss="alert">×</button>' + err_msg + '</div>';
    idx_pos.after(info);
} 

//----------------------------------------------
function ShowBatchErrorAlertInfo(rs, idx_pos){
    var err_msg = "";
    for(var i = 0;i < rs.length;i++){
        var rsobj = rs[i];
        err_msg += '<strong>' + rsobj.code + '</strong> ' + rsobj.detail;
        if(i < rs.length - 1){ 
            err_msg += '<br>';
        }
    }  
    ShowBatchAlertInfo(false,err_msg,idx_pos);
}

//---------------------------------------------- 
function ShowBatchErrorAlertInfoAfterBreadCrumb(rs){
    ShowBatchErrorAlertInfo(rs,$(".breadcrumb").parent());
}

//----------------------------------------------
function ShowBatchSuccessAlertInfo(rs, idx_pos){
    var err_msg = "";
    for(var i = 0;i < rs.length;i++){
        var rsobj = rs[i];
        err_msg += '<strong>' + rsobj.code + '</strong> ' + rsobj.detail;
        if(i < rs.length - 1){ 
            err_msg += '</br>';
        }
    }  
    ShowBatchAlertInfo(true,err_msg,idx_pos);
}

//---------------------------------------------- 
function ShowBatchSuccessAlertInfoAfterBreadCrumb(rs){
    ShowBatchSuccessAlertInfo(rs,$(".breadcrumb").parent());
}

//---------------------------------------------- 
function ShowRpcErrorInfo(result,idx_pos){
    ShowErrorAlertInfo(GetErrStr(result[0]),result[1],idx_pos);
    return;
}

//---------------------------------------------- 
function ShowRpcErrorInfoAfterBreadCrumb(result){
    ShowRpcErrorInfo(result,$(".breadcrumb").parent());
    return;
} 

function GetRpcErrInfo(result){
    var info = '<strong>' + GetErrStr(result[0]) + '</strong> ';
    //if(result[0] != 0){
        if(result[1] != null){
        info += result[1];
        }
   // }
    info += '<br>';
    
    return info;
}

//---------------------------------------------- 
function ShowNetSynRpcInfo(result){
    var info = ""; 
    var alert_type = "";
    //var rpc_info = GetRpcErrInfo(result);
    
    var rpc_info = '<strong>' + GetErrStr(result[0]) + '</strong> ';
    if(result[0] != 0){
        if(result[1] != null){
        rpc_info += result[1];
        }
    }
    rpc_info += '<br>';
    
    
    if(result[0] == 0){//成功 
        alert_type = "alert-success";
    }
    else{
        alert_type = "alert-error";
    }
    if($("."+alert_type).length > 0){//追加
        $("."+alert_type).append(rpc_info);
    }
    else{
        info = '<div class="alert ' + alert_type +'">';
        info += '<button type="button" class="close" data-dismiss="alert">×</button>' + rpc_info + '</div>'; 
        $(".breadcrumb").parent().after(info);
    }
    return;
}

function ShowSynRpcInfo(result){
    var info = ""; 
    var alert_type = "";
    var rpc_info = GetRpcErrInfo(result);
    if(result[0] == 0){//成功 
        alert_type = "alert-success";
    }
    else{
        alert_type = "alert-error";
    }
    if($("."+alert_type).length > 0){//追加
        $("."+alert_type).append(rpc_info);
    }
    else{
        info = '<div class="alert ' + alert_type +'">';
        info += '<button type="button" class="close" data-dismiss="alert">×</button>' + rpc_info + '</div>'; 
        $(".breadcrumb").parent().after(info);
    }
    return;
} 
//---------------------------------------------- 

function ShowQueryDBErrorInfo(json,idx_pos){ 
    ShowErrorAlertInfo(GetErrStr(json.errcode),GetErrStr(json.errinfo),idx_pos);
    return;
}

//----------------------------------------------  
function ShowQueryDBErrorInfoAfterBreadCrumb(json){
    ShowQueryDBErrorInfo(json,$(".breadcrumb").parent());
    return;
} 

//----------------------------------------------  
function ShowErrorAlertInfoAfterBreadCrumb(err_msg1, err_msg2){
    ShowErrorAlertInfo(err_msg1, err_msg2, $(".breadcrumb").parent());
}

//----------------------------------------------
function ClearAlertInfo(){
    $(".alert-error").remove(); 
    $(".alert-success").remove();
}

function ShowNotyInfo(err_msg,layout,type,closeButton){
     var options = {
        "text":err_msg,
        "layout":layout,
        "type":type,
        "closeButton":closeButton
    }; 
    noty(options);
}

//----------------------------------------------
function ShowBottomNotyErrorInfo(err_msg1,err_msg2){
    var err_msg = err_msg1
    if(err_msg2 != null){
        err_msg += " " + err_msg2;
    }
    ShowNotyInfo(err_msg,"bottom","error","true");
}

//----------------------------------------------
function ShowBottomNotySuccessInfo(err_msg1,err_msg2){
    var err_msg = err_msg1 + " " + err_msg2;
    ShowNotyInfo(err_msg, "bottom", "success", "true");
}

//----------------------------------------------  
function ShowNotyQueryDBErrorInfo(json){
    ShowBottomNotyErrorInfo(GetErrStr(json.errcode),GetErrStr(json.errinfo));
    return;
}

//----------------------------------------------
function ShowNotyBatchSuccessInfo(rs){
    var err_msg = "";
    if(rs.length == 0){
        return;
    }
    for(var i = 0;i < rs.length;i++){
        var rsobj = rs[i];
        err_msg += rsobj.code + ' ' + rsobj.detail;
        if(i < rs.length - 1){ 
            err_msg += '</br>';
        }
    }
    ShowBottomNotySuccessInfo(err_msg,"");
}

//----------------------------------------------
function ShowNotyBatchErrorInfo(rs){
    var err_msg = "";
    if(rs.length == 0){
        return;
    }
    for(var i = 0;i < rs.length;i++){
        var rsobj = rs[i];
        err_msg += rsobj.code + ' ' + rsobj.detail;
        if(i < rs.length - 1){ 
            err_msg += '</br>';
        }
    }
    ShowBottomNotyErrorInfo(err_msg,"");
}

function ShowAlertBatchResultInfo(rs){
    var err_success_msg = "</br>";
    var err_failed_msg  = "</br>";
    var err_msg = '';
    if(rs.length == 0){
        return;
    } 
    for(var i = 0;i < rs.length;i++){
        var rsobj = rs[i]; 
        if(rsobj.ret == 0){
            err_success_msg += rsobj.code + ' ' + rsobj.detail + '</br>';
        }
        else{
            err_failed_msg  += rsobj.code + ' ' + rsobj.detail + '</br>';
        }
    }
    if(err_success_msg != "</br>"){
        err_msg += err_success_msg;
    }
    if(err_failed_msg != "</br>"){
        err_msg += err_failed_msg;
    }
    ShowNotyInfo(err_msg,"bottom","alert","true");
}

//---------------------------------------------- 
function ShowNotyRpcErrorInfo(result){
    ShowBottomNotyErrorInfo(GetErrStr(result[0]),result[1]);
}

function ShowNotyRpcSuccessInfo(result){
    ShowBottomNotySuccessInfo(GetErrStr(result[0]), "");
}
