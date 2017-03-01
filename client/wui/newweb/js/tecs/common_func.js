
//----------------------------------------------
function Load_ResFile(spath){
    var ret = null;
    $.ajax(spath, {
        cache: false,
        global: false,
        async:false,
        dataType: "json",
        success: function(s){
            ret = eval(s);
        },
        error: function(r, s, t){
            alert("res error:" + spath);
        }
    });
    return ret;
}

//----------------------------------------------
function Load_Page(div,urldata){
    $('#loading').length > 0 ? $('#loading').remove():"";
    div.html("");
    //div.fadeOut();
    suburl = urldata;
    div.parent().append('<div id="loading" class="center">Loading...<div class="center"></div></div>');
    $.ajax({
        url:urldata,
        async:false,
        success:function(msg){
            div.html($(msg));
            $('#loading').remove();
            //div.fadeIn();
        }
    });
}

function getUrlArgs(url) {
    var args = new Object();
    var search = url.split("?");
    if(search[1] == null) return args;
    var pairs = search[1].split("&");
    for(var i = 0; i < pairs.length; i++) {
        var pos = pairs[i].indexOf('=');           // Look for "name=value"
        if (pos == -1) continue;                   // If not found, skip
        var argname = pairs[i].substring(0,pos);   // Extract the name
        var value = pairs[i].substring(pos+1);     // Extract the value
        value = decodeURIComponent(value);         // Decode it, if needed
        args[argname] = value;                     // Store as a property
    }
    return args;                                   // Return the object    
}

//----------------------------------------------
function Switch_Page(ul_classname,a_link,div_content,url_data){
    $('ul.' + ul_classname +' li.active').removeClass('active');
    a_link.parent('li').addClass('active');
    Load_Page(div_content,url_data);
}

//----------------------------------------------
function GetLastUpdateTime(){
    var myDate = new Date();
    //var mytime=myDate.toLocaleTimeString();
    var mytime=myDate.toTimeString().slice(0,8);
    var lastupdatetime = GetErrStr("web_last_updatetime") + ' ' + '<strong>' + mytime + '</strong>';
    return lastupdatetime;
}

//----------------------------------------------
function ShowLastUpdateTime(div){
    if(div.find(".last_update_time").length > 0){
        div.find(".last_update_time").remove();
    }
    var info = '<div class="last_update_time">' + GetLastUpdateTime() + '</div>'; 
    div.append(info);
}

//----------------------------------------------
function LimitCharLenInput(_area,_info,maxlenth){
    var _max;
    if(maxlenth == null){
        _max = _area.attr('maxlength');
    }
    else{
        _max = maxlenth;
    }
    var _val,_cur,_count,_warn;
    _area.bind('keyup change',function(){ //绑定keyup和change事件
        if(_info.find('span').size()<1){//避免每次弹起都会插入一条提示信息
            _info.append('<span>' + GetErrStr("you_can_enter") + '<em class="clem">' + _max + '</em>' + GetErrStr("count_charater") + '<font class="clfont"></font></span>');
        }
        _val=$(this).val();
        _cur=_val.length;
        _count=_info.find('em');
        _warn=_info.find('font');
        _warn.text(GetErrStr("no_distinguish_charater"));
        if(_cur==0){//当默认值长度为0时,可输入数为默认maxlength值,此时不可提交
            _count.text(_max);
        }else if(_cur<_max){//当默认值小于限制数时,可输入数为max-cur
            _count.text(_max-_cur);
        }else{//当默认值大于等于限制数时,插入一条提示信息并截取限制数内的值
            _count.text(0);
            _warn.text(GetErrStr("no_input_again"));
            $(this).val(_val.substring(0,_max));
        }
    });
}

/**
 * [idx_showDlg description]显示提示对话框
 * @param  {json} 
 *     paras [{type : "sucess" ,"failure","confirm",
 *             msg  : "显示在对话框内的文字",
 *             confrim : "选择确定后回调函数，只对确认对话框有效"}]
 * @return {void}
 */
function idx_showDlg(paras){
    var oParas = paras?paras:{};
    var dlgType = oParas.type?$.trim(oParas.type):"message";
    var msg;
    var confirmFunc;

    if(dlgType === "confirm"){
        var header = GetErrStr("confirm_op");
        $('#idx_'+dlgType+'Dlg .modal-header h5').html(header);
        msg = oParas.msg?oParas.msg:"确定要进行此操作吗?";
        $('#idx_'+dlgType+'Dlg .modal-body').html(msg);
    }
    confirmFunc = oParas.confirm?oParas.confirm:function(){};
    
    $('#idx_'+dlgType+'Dlg').modal('show');
    $('#idx_'+dlgType+'Dlg_confirm').unbind('click').bind('click',function(){
        if(dlgType === "confirm"){
            $('#idx_'+dlgType+'Dlg').modal('hide');
            confirmFunc();
        }
        else{
            if(confirmFunc()){
                $('#idx_'+dlgType+'Dlg').modal('hide');
            }
        }
    });
}

function ShowBatchOpConfirmDiag(diag, table, confirm_info, func_callback){
    var vCheckedbox = $("#" + table + ">tbody").find(":checkbox:checked");
    if(vCheckedbox.size() == 0){
        ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["must_select_one_error"]);
        return;
    }
    diag.find('div.modal-header h5').html(GetErrStr("confirm_op"));
    diag.find('div.modal-body').html(confirm_info);
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        diag.modal("hide");
        func_callback(vCheckedbox);
    });
    diag.modal("show");
}

//----------------------------------------------
function OutNoJDKTip(lang,errdata,div){
    var info = '<p>';
    info    += GetErrStrByLang(lang, "no_jdk_tip", errdata);
    info    += '<a href="http://www.java.com">';
    info    += GetErrStrByLang(lang, "download", errdata);
    info    += '</a>';
    info    += ' | ';
    info    += '<a href="javascript:void(0);" onclick="document.location.reload();">';
    info    += GetErrStrByLang(lang, "refresh", errdata);
    info    += '</a>';
    $("#"+div).html(info);
}

//----------------------------------------------
function CheckJdkIsInstall(lang,errdata,div){
    var ret = false;
    var versions = deployJava.getJREs();
    if(versions == null || versions.length == 0){
        OutNoJDKTip(lang,errdata,div);
        return false;
    }
    for(var i = 0;i < versions.length;i++){
        var version = versions[i];
        if(parseFloat(version) >= 1.6){
            ret = true;
            break;
        }
    }
    if(!ret){
        OutNoJDKTip(lang,errdata,div);
    }
    return ret;
}

//----------------------------------------------
//获取大小格式字符串
function getformatsize(size){
    var rtn;
    var GLevel = 1024*1024*1024;
    var MLevel = 1024*1024;
    var KLevel = 1024;
    if(size >= GLevel)
    {
        rtn = size/GLevel;
        rtn = rtn.toFixed(2);
        return rtn.toString()+"GB";
    }
    else if(size >= MLevel)
    {
        rtn = size/MLevel;
        rtn = rtn.toFixed(2);
        return rtn.toString()+"MB";
    }
    else if(size >= KLevel)
    {
        rtn = size/KLevel;
        rtn = rtn.toFixed(2);
        return rtn.toString()+"KB";
    }
    else
    {
        rtn = size;
        return rtn.toString()+"B";
    }
    return rtn;
}

function GetXmlNodeStr(node){
    if($.browser.msie){
        return node.text;
    }
    return node.textContent;
}

function SetXmlNodeStr(node,str){
    if($.browser.msie){
        node.text = str;
    }
    else{
        node.textContent = str;
    }
}

//----------------------------------------------
//IP地址转换成十进制
function IP2Decimal(ipv4)
{
    var aIPsec=ipv4.split("."); 
    for(var i  =0;i < aIPsec.length;i++)
    {
        if(parseInt(aIPsec[i])<16)
        {
            aIPsec[i]="0"+parseInt(aIPsec[i]).toString(16);
        }
        else
        {
            aIPsec[i]=parseInt(aIPsec[i]).toString(16);
        }
    }
    var nIPaddr=parseInt("0x"+aIPsec[0]+aIPsec[1]+aIPsec[2]+aIPsec[3]);
    return nIPaddr;
}  

//MAC地址转换成十进制
function MAC2Decimal(mac)
{
    var aMACsec=mac.split(":");
    var nIPaddr=parseInt("0x"+aMACsec[0]+aMACsec[1]+aMACsec[2]+aMACsec[3]+aMACsec[4]+aMACsec[5]).toString(16);
    return nIPaddr;
}  

//workflow result
function GetWorkflowResult(workflowid, callback, cluster){
    var params = [];
    params.push({"name":"workflow", "value":workflowid});
    params.push({"name":"cluster",  "value": (cluster==null? "" : cluster)});
    
    $.getJSON("../php/workflow_result.php", params, function(json){
         if(json[0] != 0){
            ShowNotyRpcErrorInfo("12", json[1]);
            return;
         }

         var rs = [];
         if(json[1][1] == 0){
            rs[0] = 0;
            rs[1] = "";
         }
         else {
            rs[0] = 2;
            rs[1] = "workflow " + json[1][0] + " failed, reason " + json[1][2];
         }

         callback(rs);
    });
}

//workflow progress
function GetWorkflowProgress(workflowid, callback, cluster){
    var params = [];
    params.push({"name":"workflow", "value":workflowid});
    params.push({"name":"cluster",  "value": (cluster==null? "" : cluster)});
    
    $.getJSON("../php/workflow_progress.php", params, function(json){
         if(json[0] != 0){
            ShowNotyRpcErrorInfo(["12", json[1]]);
            return;
         }

         callback(json[1]);
    });
}

//----------------------------------------------进度条
function GetOpProgressFlag(progress){
    var progress_flag = "";
    if(progress >= 0 && progress < 25){
        progress_flag = "progress-danger";
    }  
    else if(progress >= 25 && progress < 50){
        progress_flag = "progress-warning";
    }
    else if(progress >= 50 && progress < 75){
        progress_flag = "progress-info";
    }
    else{
        progress_flag = "progress-success";
    }
    return "progress-info";//progress_flag;
}

function GetOpProcessURL(progress_id, progress){
    var progress_flag = GetOpProgressFlag(progress);
    var progress_info = '<div class="span8 active"><div class="progress ' + progress_flag + '" id="' + progress_id + '">';
    //progress_info +=     '<div class="span2" align="right"></div>';
    progress_info +=     '<div class="bar" style="width: ' + progress +'%;">' + progress +'%</div>';
    progress_info += '</div></div>';
    progress_info += '<div class="span2" style="display:none;"><button class="btn btn-mini btn-danger"><span>' + page.res[page.pagelan].type.opr["cancel"] + '</span></button></div>';
    return progress_info;
}

function UpdateProgressByID(progress_id, progress, action) {
    /*var old_progress_flag = $("#"+progress_id)[0].className.split(" ")[1];
    var new_progress_flag = GetOpProgressFlag(progress); 
    if(old_progress_flag != new_progress_flag){
        $("#"+progress_id).removeClass();
        $("#"+progress_id).addClass("progress").addClass(new_progress_flag).addClass("span6").addClass("active");
    }*/

    $("#"+progress_id+" .bar").attr("style","width: " + progress + "%");
    $("#"+progress_id+" .bar").html(/*action + ":" +*/ progress + "%");
}

function ShowWorkflowProgress(oTable, cb) {
    var vCheckedbox = $(oTable.fnGetNodes()).find(":checkbox");

    vCheckedbox.each(function(){
        var item = $(this).data("item");        

        if(item != null && item["workflow"] != null){ 
            var tr = $(this).parent().parent();
            var nTr = tr[0];
            var progress_id = "pb_" + item["workflow"];

            var opLink = tr.children().last();
            opLink.html("");
            
            if (!oTable.fnIsOpen(nTr)){
                var progressbar = GetOpProcessURL(progress_id, 0);
                oTable.fnOpen( nTr, progressbar, 'details');
            } 

            if($("#" + progress_id).attr("fetching") == "true"){
                return;
            }
            $("#" + progress_id).attr("fetching", "true");

            GetWorkflowProgress(item["workflow"], function(pi){
                var progress_id = "pb_" + pi[3];

                if(pi[1] == -1){
                    UpdateProgressByID(progress_id, pi[2], pi[0]);
                } 
                else {
                    var rs = [];
                    
                    if(pi[4] == "vm"){
                        rs[1] = pi[4] + "(" + item["id"] + ")";
                    }
                    else if(pi[4] == "project"){
                        if(pi[0] == "project_import"){
                            rs[1] =  "image(" + item["image_id"] + ")";
                        }
                        else {
                            rs[1] = pi[4] + "(" + item["project"] + ")";
                        }
                    }
                    else if(pi[4] == "image"){
                        rs[1] = pi[4] + "(" + item["img_id"] + ")";
                    }
                    else {
                        rs[1] = "";
                    }

                    if(pi[1] == 0){
                        rs[0] = 0; rs[1] += " workflow " + pi[0] + " success";
                    }
                    else if(pi[1] == 994){
                        rs[0] = 0; rs[1] += " workflow " + pi[0] + " is cancelled";
                    }
                    else{
                       rs[0] = 2; rs[1] += " workflow " + pi[0] + " failed, reason " + pi[2];
                    } 
                    ShowSynRpcInfo(rs);

                    cb();
                }                

                $("#" + progress_id).attr("fetching", "false");                                
            }, item["cluster"]);
        }
    });   
}

function BindTableCheckboxData(table, data){
    for(var i = 0; i < data.length && i < table.find("tbody").find(":checkbox").length;i++){
        var cb = table.find("tbody").find(":checkbox").eq(i);
        cb.data("item", data[i]);
    }    
}
function TableHeadCheckboxClick(a){
    var cb = $(a);
    if(cb.attr("checked") == "checked"){
        cb.parents("table").find("tbody input[type=checkbox]").attr("checked", true);
    }
    else{
        cb.parents("table").find("tbody input[type=checkbox]").attr("checked", false);
    }    
}

function TableHeadCheckbox(extra_attr){
    var cb =  '<input type="checkbox" ' 
    if(extra_attr != null){
        cb += extra_attr;  
    }
    cb += ' onclick=TableHeadCheckboxClick(this)';
    cb += ' />';

    return cb;
}

function TableBodyCheckboxClick(a){
    var cb = $(a);    
    if(cb.attr("checked") == "checked"){
        var unchecked = cb.parents("table tbody").find("input:checkbox").not("input:checked");
        if(unchecked.length > 0){
            cb.parents("table").find("thead input[type=checkbox]").attr("checked", false);    
        }   
        else {
            cb.parents("table").find("thead input[type=checkbox]").attr("checked", true);            
        }     
    }
    else{
        cb.parents("table").find("thead input[type=checkbox]").attr("checked", false);
    }      
}

function TableBodyCheckbox(extra_attr){
    var cb =  '<input type="checkbox" ' 
    if(extra_attr != null){
        cb += extra_attr;  
    }
    cb += ' onclick=TableBodyCheckboxClick(this)';
    cb += ' />';

    return cb;
}