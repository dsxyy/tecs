var global_createvm_acton      = 0;
var global_modifyvm_acton      = 1;
var global_queryvm_acton       = 2;
var global_createvt_acton      = 3;
var global_modifyvt_acton      = 4;
var global_queryvt_acton       = 5;
var global_createvmbyvt_acton  = 6;
var global_modifydeplofyvm_acton = 7;

var VM_INSERT_ITEM = 1;
var VM_DELETE_ITEM = 2;
var VM_UPDATE_ITEM = 3;

var VM_ITEM_NAME    = 1;
var VM_ITEM_DESC    = 2;
var VM_ITEM_PROJECT = 3;
var VM_ITEM_CPU     = 11;
var VM_ITEM_MEM     = 12;
var VM_ITEM_IMAGE   = 14;
var VM_ITEM_WDOG    = 15;
var VM_ITEM_MUTEX   = 16;


//查询镜像
function QueryImages(flag,user_name,image_id,file_name){
    if(file_name == null){
        return TecsObjList("image", 1, flag, user_name,new I8(image_id),"");
    }
    else{
        return TecsObjList("image", 1, flag,  user_name,new I8(image_id),file_name);
    }
}
//查询工程
function QueryPorjects(flag,projectname,username){
    return TecsObjList("project", 1, new I8(flag),projectname,username);
}

//查询虚拟机
function QueryVms(flag){
    if(flag == -5){
        return TecsObjList("vmcfg.staticinfo", 1, "query_all_self", new I8(flag));
    }
    else {
        var staticInfo = TecsObjList("vmcfg.staticinfo", 1, "query_self_appointed", new I8(flag));  
        var dynicInfo = TecsObjList("vmcfg.runninginfo", 1, "query_self_appointed", new I8(flag));  
        var s = staticInfo[0];
        var d = dynicInfo[0] ;
        return jQuery.extend(s,d);        
    }
}

//查询当前虚拟机
function GetCurrentVmInfo(vmid){
    var vms = QueryVms(vmid);
    if (vms==null || vms.length==0){
        return null;
    }
    return vms;
}

function GetVmRunningInfo(vmid){
    var start_index = new I8(0);
    var query_count = new I8(50);
    var vmid = new I8(vmid);    
    var rs = xmlrpc_service.tecs.vmcfg.runninginfo.query(xmlrpc_session, start_index, query_count, "query_self_appointed", vmid);    
    if(rs[0] == 0){
        return rs[3][0];
    }
    else {
        return null;
    }
}

//查询模板
function QueryTemplates(flag,vt_name){
    return TecsObjList("vmtemplate", 1, flag,vt_name);
}

//查询当前模板
function GetCurrentVtInfo(vt_name){
    var vts = QueryTemplates(3,vt_name);
    if (vts==null || vts.length==0)
    {
        return null;
    }
    var vt = vts[0];
    vt["computationinfo"] = [];
    vt["netinfo"] = [];
    vt["storageinfo"] = [];
    return vt;
}

//获取第一个工程名
function QueryFirstPorjectsName(flag,projectname,username){
    var start_index = new I8(0);
    var query_count = new I8(1);
    var rs = null;
    try{
        rs = xmlrpc_service.tecs["project"].query(xmlrpc_session,  start_index, query_count, new I8(flag), projectname, username);
        if(rs[0] != 0){
            ShowNotyRpcErrorInfo(rs);
            return null;
        } 
        return rs[3][0]["name"];
    }
    catch(e){
        var em;
        if (e.toTraceString){
            em = e.toTraceString;
        }
        else{
            em = e.message;
        }
    }
    return "";
}

function InitSlPorject(slproject){
    var flag = new I8(-5);
    var rs;
    rs = TecsObjList("project", 1, flag,"","");
    if (rs!=null || rs.length>0){
        for(var i=0;i<rs.length;i++){
            var project = rs[i];
            slproject.append("<option value="+project["name"]+">"+project["name"]+"</option>");
        }
    }
} 

//虚拟机相关操作
function action(id,action_name){
    var vmid = new I8(id);
    var rs = null;
    try{
        rs = xmlrpc_service.tecs.vmcfg["action"](xmlrpc_session, vmid,action_name);
    }
    catch(e){
        var em;
        if (e.toTraceString){
            em = e.toTraceString;
        }
        else{
            em = e.message;
        }
    }
    return rs;
}

//----------------------------------------------
function Switch_Vr_Page(urldata){
    var a_link = null;
    var urldata_tmp = "";
    if(typeof urldata == "string"){ 
        a_link = $('ul.main-menu li a[href="' + urldata.split("?")[0] +'"]');
        urldata_tmp = urldata;
    }
    else{ 
        a_link = $('ul.main-menu li a[href="' + $(this).attr('data-value') +'"]');
        urldata_tmp = $(this).attr('data-value');
    }
    Switch_Page("main-menu",a_link,$('#right_content'),urldata_tmp);
}

//----------------------------------------------
function QueryVmCallBack(vmid,fnCallback){
    var rs = null;
    var start_index = new I8(0);
    var query_count = new I8(50);
    var vmid = new I8(vmid);
    xmlrpc_service.tecs.vmcfg.runninginfo.query(xmlrpc_session,start_index, query_count,"query_self_appointed", vmid,fnCallback);
}

//----------------------------------------------
function QueryVmsCallBack(vmsid,fnCallback){
    for(var i = 0;i < vmsid.length;i++){
        QueryVmCallBack(vmsid[i],fnCallback);
    }
}

//----------------------------------------------
function BindCCSelect(sl){
    sl.empty();
    var mode = 0;
    var rs = TecsObjList("cluster", 1, mode);
    if(rs == null || rs.length == 0){
        return;
    }
    for(var i = 0;i < rs.length;i++){
        sl.append("<option value="+ rs[i]["cluster_name"] +">"+ rs[i]["cluster_name"] +"</option>");
    }
}

//----------------------------------------------
function InitSlPosition(sl)
{
    sl.empty();
    for(var p in page.res[page.pagelan].type.DiskPosition){
        sl.append("<option value="+p+">"+ page.res[page.pagelan].type.DiskPosition[p] +"</option>");
    }
}

//----------------------------------------------
function InitDiskFsType(sl){
    sl.empty();
    for(var p in page.res[page.pagelan].type.DiskFormatType){
        sl.append("<option value="+p+">"+ page.res[page.pagelan].type.DiskFormatType[p] +"</option>");
    }
}

//----------------------------------------------
//获取镜像全称
function GetImageFullName(imageid){
    var fullname = "";
    var imagelist = QueryImages(1,"",imageid.toString(),"");
    if (imagelist!=null || imagelist.length>0)
    {
        var image = imagelist[0];
        fullname = image["file"] + "(" + page.res[page.pagelan].type.field_name["creater"] + ":" + image["user_name"] + ")";
    }
    return fullname;
}

//----------------------------------------------虚拟机详细
function onMoreVmClick(diag,vid){
    var rs = GetCurrentVmInfo(vid);
    if(rs == null){
        return;
    }
    var title = '<i class="icon-zoom-in"></i> <span>' + page.res[page.pagelan].type.display["detail"] + '</span>'; 
    $("#"+diag).find('div.modal-header h5').html(title);
    ShowVmInfoDiag($("#"+diag) ,rs, 2);
}

//----------------------------------------------虚拟机模板详细
function onMoreVtClick(diag,vt_name){
    var rs = GetCurrentVtInfo(vt_name);
    if(rs == null){
        return;
    }
    var title = '<i class="icon-zoom-in"></i> <span>' + page.res[page.pagelan].type.display["detail"] + '</span>'; 
    $("#"+diag).find('div.modal-header h5').html(title);
    ShowVmInfoDiag($("#"+diag) ,rs, 5);
}

//----------------------------------------------获取指定格式大小
function Get_Assignedformat_Size(memorysize,memunit){
    var rtn = memorysize;
    var GLevel = 1024*1024*1024;
    var MLevel = 1024*1024;
    var KLevel = 1024;
    var arrsize = new Array();
    var size = new Object();
    size.name = "GB";
    size.unit = GLevel;
    arrsize.push(size);
    size = new Object();
    size.name = "MB";
    size.unit = MLevel;
    arrsize.push(size);
    size = new Object();
    size.name = "KB";
    size.unit = KLevel;
    arrsize.push(size);
    for(var i = 0;i < arrsize.length;i++){
        if(memunit == arrsize[i].name){
            rtn = memorysize/(arrsize[i].unit);
            rtn = rtn.toFixed(2);
            break;
        }
    }
    return rtn;
} 

//----------------------------------------------获取虚拟机/模板基本信息-Machine
function GetMachineOfVmBaseInfo(aMachine){
    var machine                 = new Object(); 
    machine["id"]               = aMachine["id"];
    machine["position"]         = aMachine["position"]; 
    machine["size"]             = aMachine["size"]; 
    machine["bus"]              = aMachine["bus"]; 
    machine["type"]             = aMachine["type"];
    machine["target"]           = aMachine["target"];
    machine["reserved_backup"]  = aMachine["reserved_backup"];	
    return machine;
}

//----------------------------------------------获取虚拟机/模板基本信息-NIC
function GetNicsOfVmBaseInfo(aNics){
    var nics                 = new Array(); 
    for(var i = 0;i < aNics.length;i++){
        var aNic = aNics[i];
        var nic = new Object();
        nic.nic_index = aNic.nic_index;
        nic.sriov     = aNic.sriov; 
        nic.ip        = aNic.ip; 
        nic.netmask   = aNic.netmask; 
        nic.gateway   = aNic.gateway; 
        nic.mac       = aNic.mac;
/*
        nic.plane     = aNic.plane; 
        nic.vlan      = aNic.vlan; 
        nic.vlantrunk = aNic.vlantrunk; 
        nic.dhcp      = aNic.dhcp;
        nic.model      = aNic.model; 
*/
        nic.logic_network_id     = aNic.logic_network_id; 
        nic.loop = aNic.loop; 
        nic.model      = aNic.model; 
        nics.push(nic);
    }
    return nics;
}

//----------------------------------------------获取虚拟机/模板基本信息部分
function GetVmBaseInfo(VmData){
    var VmBaseInfo = new Object();
    VmBaseInfo.description      = VmData["cfg_info"]["base_info"]["description"];
    VmBaseInfo.vcpu             = VmData["cfg_info"]["base_info"]["vcpu"];
    VmBaseInfo.tcu              = VmData["cfg_info"]["base_info"]["tcu"];
    VmBaseInfo.memory           = VmData["cfg_info"]["base_info"]["memory"];
    VmBaseInfo.machine          = GetMachineOfVmBaseInfo(VmData["cfg_info"]["base_info"]["machine"]);
    VmBaseInfo.root_device      = "";
    VmBaseInfo.kernel_command   = "";
    VmBaseInfo.bootloader       = "";
    VmBaseInfo.virt_type        = VmData["cfg_info"]["base_info"]["virt_type"];
    VmBaseInfo.rawdata          = "";
    VmBaseInfo.context          = VmData.cfg_info.base_info.context;    
    VmBaseInfo.qualifications   = new Array();
    VmBaseInfo.disks            = VmData["cfg_info"]["base_info"]["disks"];
    VmBaseInfo.nics             = GetNicsOfVmBaseInfo(VmData["cfg_info"]["base_info"]["nics"]);
    VmBaseInfo.devices          = VmData["cfg_info"]["base_info"]["devices"];
    VmBaseInfo.cluster          = VmData["cfg_info"]["base_info"]["cluster"];
    VmBaseInfo.host             = VmData["cfg_info"]["base_info"]["host"];
    VmBaseInfo.vm_wdtime        = VmData["cfg_info"]["base_info"]["vm_wdtime"];
    VmBaseInfo.vnc_passwd       = VmData["cfg_info"]["base_info"]["vnc_passwd"];
    VmBaseInfo.enable_serial    = VmData["cfg_info"]["base_info"]["enable_serial"];
    VmBaseInfo.enable_coredump  = VmData["cfg_info"]["base_info"]["enable_coredump"];
    VmBaseInfo.enable_livemigrate  = VmData["cfg_info"]["base_info"]["enable_livemigrate"];
    VmBaseInfo.hypervisor       = VmData["cfg_info"]["base_info"]["hypervisor"];
    //VmBaseInfo.machine.reserved_backup  = parseInt(VmData["cfg_info"]["base_info"]["reserved_backup"]);			
    return VmBaseInfo;
}

//----------------------------------------------将BOOL型转成资源文件
function GetBool2Res(para){
    var ret = "";
    if(para){
        ret = page.res[page.pagelan].type.YesOrNo["1"];
    }
    else{
        ret = page.res[page.pagelan].type.YesOrNo["0"];
    }
    return ret;
}

//----------------------------------------------检查是否可以创建虚拟机

function Add_Project_Callback(frm,func_callback){
    var rs = null;    
    var item = GetModifyForm(frm);
    frm.find(".name").removeClass("error");
    if(item.name == ""){
        frm.find(".name").addClass("error");
        return false;
    }
    try{
        rs = xmlrpc_service.tecs.project.allocate(xmlrpc_session,item.name,item.des);
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
    if(func_callback != null){
        func_callback();
    }
    return true;
}

function Create_Project(func_callback){
    var diag = $('#idx_confirmDlg');
    var info = '';
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    
    info += '       <div class="control-group name">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["name"] + '<font color="red">*</font></label>';
    info += '           <div class="controls">';
    info += '               <input id="txProName" type="text" maxlength="32" class="OamUi" bindattr="val" bind="name">';
    info += '               <span class="help-inline">' + page.res[page.pagelan].type.err_info["text_input_error"] + '</span>';
    info += '            </div>';
    info += '       </div>';
    
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["desc"] + '</label>';
    info += '           <div class="controls">';
    info += '               <textarea id="taDesc" class="autogrow OamUi" id="taDesc" maxlength="128" rows="6" bindattr="val" bind="des"></textarea><p id="pSetDescInfo"></p>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '   </fieldset>';
    info += '</form>';
    
    var header = '<i class="icon-plus"></i>&nbsp;' + page.res[page.pagelan].type.opr["create"];
    diag.find('div.modal-header h5').html(header);
    diag.find('div.modal-body').html(info); 
    z_strInput("txProName",255);
    LimitCharLenInput($("#taDesc"),$("#pSetDescInfo"));
    diag.modal("show"); 
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        if(!Add_Project_Callback(diag.find(".form-horizontal"), func_callback)){
            return;
        }
        diag.modal("hide");
    });
}

function Upload_Image(){
    var rs = xmlrpc_service.tecs.image.ftp_svr_info_get(xmlrpc_session);
    if (rs[0] != 0){
        ShowNotyRpcErrorInfo(rs); 
        return;
    }
    var ftp_server_info = rs[1];
    var ftp_ip = ftp_server_info["ftp_svr_ip"] == "*"?window.location.host:ftp_server_info["ftp_svr_ip"];
    var lang = $.cookie('current_language') == null ? 'ch' :$.cookie('current_language');
    var strTargetUrl = "upload_image.htm";
    strTargetUrl    += "?host=" + ftp_ip; 
    strTargetUrl    += "&password=" + xmlrpc_session;
    strTargetUrl    += "&port="+ftp_server_info["ftp_svr_port"];
    strTargetUrl    += "&lang=" + lang;
    return OpenWinToCenter("700","500",strTargetUrl);
}

function CheckIsProjectOfCreateVm(){
    //检查是否有工程
    var rs = xmlrpc_service.tecs.project.query(xmlrpc_session, new I8(0), new I8(50), new I8(-5),"","");
    if(rs == null){
        return false;
    } 
    if(rs[0] != 0){
        ShowNotyRpcErrorInfo(rs);
        return false;
    }
    if(rs[3].length == 0){
        var err1 = page.res[page.pagelan].type.err_info["vm_noproject_forcreatevm"];
        var err2 = '<button class="btn" onclick="Create_Project()"><i class="icon-plus"></i><span>' + page.res[page.pagelan].type.opr["create_project"] + '</span></button>';
        ShowBottomNotyErrorInfo(err1, err2);
        return false;
    } 
    return true;
}

function CheckIsImageOfCreateVm(){
    var image_count = 0; 
    //检查是否有镜像
    var rs = xmlrpc_service.tecs.image.query(xmlrpc_session, new I8(0), new I8(50), -3,"",new I8(-1),"");
    if(rs == null){
        return false;
    } 
    if(rs[0] != 0){
        ShowNotyRpcErrorInfo(rs);
        return false;
    }
    if(rs[3].length == 0){
        var err1 = page.res[page.pagelan].type.err_info["vm_noimage_forcreatevm"];
        var err2 = '<button class="btn" onclick="Upload_Image()"><i class="icon-circle-arrow-up"></i><span>' + page.res[page.pagelan].type.opr["upload"] + '</span></button>';
        ShowBottomNotyErrorInfo(err1, err2);
        return false;
    }
    for(var i = 0;i < rs[3].length;i++){
        var onedata = rs[3][i];
        if(onedata["type"] != "machine"){
            continue;
        }
        if(!onedata["is_enabled"]){
            continue;
        } 
        image_count++;
    }
    if(image_count == 0){
        var err1 = page.res[page.pagelan].type.err_info["vm_noimage_forcreatevm"];
        var err2 = '<button class="btn" onclick="Upload_Image()"><i class="icon-circle-arrow-up"></i><span>' + page.res[page.pagelan].type.opr["upload"] + '</span></button>';
        ShowBottomNotyErrorInfo(err1, err2);
        return false;
    }
    return true;
}

function CheckIsCreateVm(){
    if(!CheckIsProjectOfCreateVm()){
        return false;
    }
    if(!CheckIsImageOfCreateVm()){
        return false;
    }
    return true;
}

function CheckVtNameIsExist(vt_name, frm){
    frm.find(".vt_name .help-inline").html(page.res[page.pagelan].type.err_info["text_input_error"]);
    if(!CheckVtNameValid(vt_name, frm)){
        return false;
    } 
    var rs = GetCurrentVtInfo(vt_name);
    if(rs != null){ 
        var err_msg = '<span class="icon icon-color icon-close"></span>';
        err_msg += '<span class="label label-warning">' + page.res[page.pagelan].type.err_info["vm_vtname_same"] + '</span>';
        frm.find(".vt_name").addClass("error");
        frm.find(".vt_name .help-inline").html(err_msg);
        return false;
    }
    /*else{
        var err_msg = '<span class="icon icon-color icon-check"></span>';
        frm.find(".vt_name .help-inline").html(err_msg);
        return true;
    }*/
    return true;
}

//----------------------------------------------获取虚拟机运行状态图
function Get_Vm_Running_Status_Count(auser_name, piechart, pieinfo){
    var sSource = "../php/get_vm_running_status.php"; 
    var aoData  = {user_name: auser_name};
    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
        }
        for(var i = 0;i < ackdata.aaData.length;i++){
            var myvm_status_count = ackdata.aaData[i];
            var data = [
                { label: page.res[page.pagelan].type.VmState[3],  data: parseInt(myvm_status_count[1])},
                { label: page.res[page.pagelan].type.VmState[4],  data: parseInt(myvm_status_count[2])},
                { label: page.res[page.pagelan].type.VmState[5],  data: parseInt(myvm_status_count[3])}, 
                { label: page.res[page.pagelan].type.VmState[2],  data: parseInt(myvm_status_count[0])},
                { label: page.res[page.pagelan].type.VmState[8],  data: parseInt(myvm_status_count[5])},
                { label: page.res[page.pagelan].type.VmState[6],  data: parseInt(myvm_status_count[4])}
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
                            formatLabel = label + ' - ' + series.data[0][1] + 
                                ' (' + percent + '%)';
                            return formatLabel;
                        }
                    }
                });
            }
            break;
        }
    });
} 

//----------------------------------------------获取虚拟机部署状态图
function Get_Vm_Depoly_Status_Count(auser_name, piechart, pieinfo){
    var sSource = "../php/get_vm_deploy_status.php"; 
    var aoData  = {user_name: auser_name};
    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
        }
        for(var i = 0;i < ackdata.aaData.length;i++){
            var myvm_status_count = ackdata.aaData[i];
            var data = [
                { label: page.res[page.pagelan].type.display["fiction"],  data: parseInt(myvm_status_count[0])},
                { label: page.res[page.pagelan].type.display["deployed"],  data: parseInt(myvm_status_count[1])}
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
                            formatLabel = label + ' - ' + series.data[0][1] + 
                                ' (' + percent + '%)';
                            return formatLabel;
                        }
                    }
                });
            }
            break;
        }
    });
}

//-------------------------------初始化用户过滤
function InitSelectUser(sl, user_name, fnCallback){
    sl.empty();
    sl.attr("data-placeholder", page.res[page.pagelan].type.display["select_user"]);
    //查询用户
    var rs = TecsObjList("user", 1, "" ,"");
    if(rs == null || rs.length == 0){
        return;
    }
    for(var i = 0; i < rs.length; i++){
        var onedata = rs[i]; 
        var info = '';
        if(user_name == onedata["username"]){
            info = '<option selected>' + onedata["username"] + '</option>';
        }
        else{
            info = '<option>' + onedata["username"] + '</option>';
        }
        sl.append(info);
    }
    if(fnCallback != null){
        sl.unbind('change').bind('change',function(){
            fnCallback();
        });
    }
    sl.chosen();
}

//-------------------------------初始化工程过滤
function AppendOption2SelectProject(sl, user_name, role, isshowusername, project_name){
    //查找工程
    var flag;
    if(role == 1){
        flag = -4;
    } 
    else{
        flag = -5;
        user_name = "";
    }
    var rs = TecsObjList("project", 1, new I8(flag),"",user_name);
    for(var i = 0; i < rs.length; i++){
        var onedata = rs[i]; 
        var info = '';
        var title = '';
        if(isshowusername){ 
            title = onedata["name"] + "(" + user_name + ")";
        }
        else{
            title = onedata["name"];
        }
        
        if(project_name == onedata["name"]){
            info = '<option selected user_name=' + user_name + ' value=' + onedata["name"] + '>' +  title + '</option>';
        }
        else{
            info = '<option user_name=' + user_name + ' value=' + onedata["name"] + '>' + title + '</option>';
        }
        sl.append(info);
    }
}

function InitSelectProject(sl, user_name, project_name, role ,isshowusername, fnCallback){
    sl.attr("data-placeholder", page.res[page.pagelan].type.display["select_project"]);
    if(fnCallback != null){
        sl.unbind('change').bind('change',function(){
            fnCallback();
        });
    }
    sl.chosen();
    if(user_name == null || user_name == ""){
        return;
    } 
    
    AppendOption2SelectProject(sl, user_name, role, isshowusername, project_name);
    sl.trigger("liszt:updated");
}


//-------------------------------获取multipleselect格式化字符串
function GetFrmtStrOfMultiSelect(list){
    var ret = '';
    if(list == null || list == ''){
        return ret;
    }
    for(var i = 0;i < list.length;i++){
        ret += "'" + list[i] + "'";
        if(i < list.length - 1){
            ret += ",";
        }
    }
    return ret;
}

//-------------------------------设置虚拟机信息对话框标题------------------------------
function SetVmParaHeader(diag_img, diag_text, diag){
    diag.find('div.modal-header h5').html('<img src="img/' + diag_img + '"/>&nbsp;' + diag_text);
}

//-------------------------------虚拟机统计信息------------------------------
var totalPoints = 300;

function getStatisticsData(data) {
    if(data == null){
        return [];
    }

    var temp = [];
    temp.length = totalPoints;
    for (var i = 0; i < data.length; i++){
        temp.shift();
        temp.push(data[i]/10);
    }

    var res = [];
    for (var i = 0; i < temp.length; ++i)
        res.push([i, temp[i]])    
    return res;
}


var vm_statistics_timer = null

function vm_statistics(vm){
    var vm_statistics_dlg_html = 
        '<div class="box"> \
            <div class="box-header well" data-original-title> \
                <h2><i class="icon-list-alt"></i> CPU </h2> \
            </div> \
            <div class="box-content"> \
                <div class="box-content"><div id="vmcpuchart" style="height:190px;"></div></div> \
            </div>  \
        </div> \
        <div class="box"> \
            <div class="box-header well" data-original-title> \
                <h2><i class="icon-list-alt"></i>' + page.res[page.pagelan].type.field_name["memory"] + '</h2> \
            </div> \
            <div class="box-content"> \
                <div class="box-content"><div id="vmramchart" style="height:190px;"></div></div>  \
            </div> \
        </div> \
        <div class="box"> \
            <div class="box-header well" data-original-title> \
                <h2><i class="icon-list-alt"></i> ' + page.res[page.pagelan].type.display["nic"] + ' </h2> \
            </div> \
            <div class="box-content"> \
                <div class="box-content"><div id="vmnicchart" style="height:190px;"></div></div> \
            </div> \
        </div>';
    
    ShowDiaglog("vm_statistics_dlg", vm_statistics_dlg_html, {
        show: function(){   
            var options = {
                yaxis: { min: 0, max: 100 },
                xaxis: { show: false }
            };        
            var vid = vm.parent().parent().find(":checkbox").eq(0).val();  

            var h5 = $("#vm_statistics_dlg").find('div.modal-header h5');
            h5.html(h5.text() + " - " + vid); 

            var params = [];
            params.push({"name":"vid", "value":vid});        

            function vmupdate() {
                if($("#vmcpuchart").length)
                {           
                    $.getJSON("../php/vm_statistics.php", params, function(info){
                        cpuPlot.setData([getStatisticsData(info.cpu)]);
                        cpuPlot.draw();        

                        ramPlot.setData([getStatisticsData(info.mem)]);
                        ramPlot.draw();        

                        var nicData = [];
                        for (x in info.nic) {
                            nicData.push({label: x, data: getStatisticsData(info.nic[x])});
                        }; 
                        nicPlot.setData(nicData);
                        nicPlot.draw();
                    });
                } 
            } 

            $.getJSON("../php/vm_statistics.php", params, function(info){
                cpuPlot = $.plot($("#vmcpuchart"), [getStatisticsData(info.cpu)], options); 
   
                ramPlot = $.plot($("#vmramchart"), [getStatisticsData(info.mem)], options);                

                var nicData = [];
                for (x in info.nic) {
                    nicData.push({label: x, data: getStatisticsData(info.nic[x])});
                }; 
                nicPlot = $.plot($("#vmnicchart"), nicData, options);  

                vm_statistics_timer = setInterval(vmupdate, 5000);               
            });             
        },
        close: function(){
            clearInterval(vm_statistics_timer);
            $("#host_statistics_dlg").modal("hide");
        }
    });
}