
//var xmlrpc_session = "admin:22";
//var xmlrpc_addr = "http://10.43.178.200:8080/RPC2";

var xmlrpc_addr = "";

/*************************************   Global Var  ***********************************/

var tableLanguage = {
      ch:{
            "sProcessing":   "处理中...",
            "sLengthMenu":   "显示 _MENU_ 项结果",
            "sZeroRecords":  "没有匹配结果",
            "sInfo":         "共 _TOTAL_ 项",
            "sInfoEmpty":    "共 0 项",
            "sInfoFiltered": "(由 _MAX_ 项结果过滤)",
            "sInfoPostFix":  "",
            "sSearch":       "<i class='icon-search'></i>",
            "sUrl":          "",
            "oPaginate": { "sFirst":    "首页",    "sPrevious": "上页",    "sNext":     "下页","sLast":     "末页"}
        },
    en:{
        "sLengthMenu": "Display _MENU_ records per page",
        "sZeroRecords": "Nothing found",
        "sInfo": "Total _TOTAL_ entries",
        "sInfoEmpty": "Total 0 entries",
        "sInfoFiltered": "(filtered from _MAX_ total records)",
        "sSearch":       "<i class='icon-search'></i>",        
    }
};

var dhcpstatus = [ {ch:"初始态",en:"init"},
                   {ch:"停止",en:"stop"},
                   {ch:"运行",en:"running"}];

var cc_subnode = [ {ch:"计算",en:"Compute"},
                   {ch:"网络",en:"Network"},
                   {ch:"存储",en:"Storage"}];

var ip_types = [ {ch:"", en:""},
                 {ch:"DHCP分配", en:"DHCP"},
                 {ch:"网关", en:"Gateway"},
                 {ch:"本地接口", en:"Interface"} ];

var usertype = [ {ch:"未知",en:"unknown"},
                 {ch:"云管理员",en:"cloud_admin"},
                 {ch:"帐户管理员",en:"account_admin"},
                 {ch:"普通用户",en:"account"}];

var userstatus = [ {ch:"未知",en:"unknown"},
                   {ch:"启用",en:"Enable"},
                   {ch:"禁用",en:"Disable"}];

var runtime_res = [ {id:"pid", ch:"进程ID", en:"pid"},
                    {id:"tid", ch:"线程ID", en:"tid"},
                    {id:"running_seconds", ch:"运行时间", en:"runtime"},
                    {id:"asserts", ch:"断言", en:"Asserts"},
                    {id:"mu", ch:"消息单元", en:"MU"},
                    {id:"state", ch:"状态", en:"State"},
                    {id:"push_count", ch:"收到消息数", en:"push_count"},
                    {id:"pop_count", ch:"处理消息数", en:"pop_count"},
                    {id:"queued_count", ch:"积压消息数", en:"queued_count"},
                    {id:"last_message", ch:"最后消息", en:"last_message"},
                    {id:"exceptions", ch:"异常", en:"Exceptions"}];

var mu_state = [   {ch:"未知",en:"none"},
                   {ch:"初始态",en:"startup"},
                   {ch:"工作中",en:"work"},
                   {ch:"退出中",en:"exit"}];

var ok_cancel = [  {ch:"确定",en:"OK"},
                   {ch:"取消",en:"Cancel"},
                   {ch:"关闭",en:"Close"}];

var yes_no = [  {ch:"否",en:"No"},
                {ch:"是",en:"Yes"}];


/*************************************   Global Function  ***********************************/

function init_xmlrpc(addr)
{
        var xmlrpc = null;
        var xmlrpc_service = null;

        if(addr != "") {
            xmlrpc_addr = addr;
        }

        if( xmlrpc_addr == "") {
            xmlrpc_addr = "https://" + window.location.hostname + ":443/RPC2";
        }

        var xmlrpc_methods = [
            "tecs.clustercfg.query",
            "tecs.clustercfg.delete",
            "tecs.clustercfg.set",
            //"tecs.cluster.preserve.start",
            //"tecs.cluster.preserve.stop",
            "tecs.cluster.query",
            "tecs.cluster.register",
            "tecs.cluster.unregister",
            "tecs.cluster.set",
            "tecs.cluster.forget",
            "tecs.cluster.disable",
            "tecs.cluster.enable",
            "tecs.cluster.query_netplane",
            "tecs.cluster.query_core_tcu",
            "tecs.cpuinfo.query",
            "tecs.currentalarm.query",
            "tecs.historyalarm.query",
            "tecs.host.disable",
            "tecs.host.enable",
            "tecs.host.forget",
            "tecs.host.query",
            "tecs.host.reboot",
            "tecs.host.register",
            "tecs.host.set",
            "tecs.host.shutdown",
            "tecs.host.unregister",
            "tecs.cmm.query",            
            "tecs.cmm.add",            
            "tecs.cmm.delete",            
            "tecs.cmm.set",             
            "tecs.host.extern_ctrl_cmd",
            "tecs.host.extern_ctrl.query",
            "tecs.host.extern_ctrl_cfg_get",
            "tecs.host.extern_ctrl_cfg_set",
            "tecs.image.unregister",
            "tecs.image.enable",
            //"tecs.image.modify",
            "tecs.image.publish",
            "tecs.image.query",
            "tecs.image.register",
            "tecs.image.set",
            "tecs.image.delete",
            "tecs.image.deploy", 
            "tecs.image.cancel",
            "tecs.image.ftp_svr_info_get",
            "tecs.image.getspace",
              
            "tecs.file.query",
            "tecs.port.query",
            "tecs.port.set",
            "tecs.port_loopback.set",
            "tecs.port_netplane.set",
            "tecs.port.delete",
            "tecs.tcu.delete",
            "tecs.tcu.query",
            "tecs.tcu.set",
            "tecs.trunk.delete",
            "tecs.trunk.query",
            "tecs.trunk.set",
            "tecs.trunk_netplane.set",
            "tecs.trunk.add",
            "tecs.project.query",
            "tecs.project.allocate",
            "tecs.project.delete",
            //"tecs.project.modify",
            "tecs.project.query_vm_by_project",
            "tecs.project.set",
            "tecs.project.save_as_image",
            "tecs.project.create_by_image",
             "tecs.project.clone",
             "tecs.project.action",

            "tecs.usergroup.allocate",
            "tecs.usergroup.delete",
            "tecs.usergroup.query",
            "tecs.usergroup.set",
            "tecs.user.allocate",
            "tecs.user.delete",
            "tecs.user.query",
            "tecs.user.set",

            "tecs.session.login",
            "tecs.session.query",
            "tecs.session.logout",

            "tecs.system.runtime.query",
            "tecs.system.license.query",
            "tecs.system.license.hardware_fingerprint.query",

            "tecs.snmpconfiguration.query",
            "tecs.snmpconfiguration.set",
            "tecs.snmpuser.query",
            "tecs.snmpuser.set",
            "tecs.trapaddress.delete",
            "tecs.trapaddress.query",
            "tecs.trapaddress.set",

            "tecs.version.install",
            "tecs.version.uninstall",
            "tecs.version.query",
            "tecs.version.getresult",
            "tecs.version.update",
            "tecs.version.cancel",
            "tecs.version.get_repo_addr",
            "tecs.version.set_repo_addr",

            "tecs.sa.enable",
            "tecs.sa.disable",
            "tecs.sa.forgetbyid",
            "tecs.sa.clustermap",            

            "tecs.dhcp.global.set",
            "tecs.dhcp.query",
            "tecs.dhcp.start",
            "tecs.dhcp.stop",
            "tecs.dhcp_state.query",
            "tecs.dhcp_subnet.delete",
            "tecs.dhcp_subnet.set",
            "tecs.dhcp_pxe.set",
            "tecs.dhcp_range.set",
            "tecs.dhcp.ip.query",
            "tecs.vlan.query",
            "tecs.vlan_range.query",
            "tecs.vlan_range.delete",
            "tecs.vlan_range.set",

            "tecs.netplane.query",
            "tecs.netplane.add",
            "tecs.netplane.delete",
            "tecs.netplane.set",
            "tecs.netplane_map.set",
            "tecs.netplane_map.query",
            "tecs.netplane_map.delete",

            "tecs.netplane_default.set",
            "tecs.netplane_default.delete",
            "tecs.netplane_default.query",

            "tecs.vnet.dhcp.subnet.set",
            "tecs.vnet.netplane.delete",
            "tecs.vnet.netplane.map.delete",
            "tecs.vnet.netplane.map.set",
            "tecs.vnet.netplane.map.show",
            "tecs.vnet.netplane.set",
            "tecs.vnet.netplane.show",
            "tecs.vnet.vlan.show",

            //add netplane
            "tecs.vnet.netplane.cfg",
            "tecs.vnet.netplaneip.cfg",
            "tecs.vnet.netplanemac.cfg",
            "tecs.vnet.netplanevlan.cfg",
            "tecs.vnet.netplanesegment.cfg",

            "tecs.vnet.logicnet.cfg",
            "tecs.vnet.logicnetip.cfg",

            "tecs.vnet.portgroup.cfg",
            "tecs.vnet.portgroupvlan.cfg",

            "tecs.vnet.switch.cfg",
            "tecs.vnet.switchport.cfg",
            "tecs.vnet.wcswitch.base.cfg",
            "tecs.vnet.wcswitch.port.cfg",
            "tecs.vnet.wcloopport.cfg",
            "tecs.vnet.wcdeltask.cfg",
            "tecs.vnet.clearvna.cfg",
			
            "tecs.vnet.loopport.cfg",
            "tecs.vnet.kernalport.cfg",
            
            "tecs.vnet.wcvport.cfg",
            "tecs.vnet.sdn.cfg",

            "tecs.vmtemplate.query",
            "tecs.vmtemplate.allocate",
            "tecs.vmtemplate.delete",
            "tecs.vmtemplate.set",
            "tecs.vmtemplate.publish",
			
            "tecs.portabledisk.create"	,	
            "tecs.portabledisk.delete",
			"tecs.vmcfg.portable_disk.attach",
			"tecs.vmcfg.portable_disk.detach",
            "tecs.portabledisk.attachhost",
            "tecs.portabledisk.detachhost",
            "tecs.portabledisk.blockinfo.get",
			
			"tecs.save_energy.set",
			"tecs.save_energy.query",
			
            "tecs.scheduler.set_policy",
            "tecs.scheduler.query_policy",
            
            "tecs.vm_ha.set_policy",
            "tecs.vm_ha.query_policy",
			
            "tecs.vmcfg.affinity_region.allocate",
            "tecs.vmcfg.affinity_region.delete",
            "tecs.vmcfg.affinity_region.query",
            "tecs.vmcfg.affinity_region.set",
            "tecs.vmcfg.affinity_region.member.add",
            "tecs.vmcfg.affinity_region.member.delete",
			
            "tecs.vmcfg.allocate",
            "tecs.vmcfg.allocate_by_vt",
            "tecs.vmcfg.query",
            "tecs.vmcfg.staticinfo.query",
            "tecs.vmcfg.runninginfo.query",
            "tecs.vmcfg.delete",
            "tecs.vmcfg.set",
            "tecs.vmcfg.modify",
            "tecs.vmcfg.action",
            "tecs.vmcfg.query_relation",
            "tecs.vmcfg.relation.set",
            "tecs.vmcfg.migrate",
            "tecs.vmcfg.full.query",
            "tecs.vmcfg.query_vm_syn_state",
            "tecs.vmcfg.query_usb_device",
            "tecs.vmcfg.operate_usb_device",
            "tecs.vmcfg.image_backup.create",	
            "tecs.vmcfg.image_backup.modify",	
            "tecs.vmcfg.image_backup.restore",	
            "tecs.vmcfg.image_backup.delete",	
			
            "tecs.user.full.query",
            "tecs.vnic.map.query"
        ];

    try
    {
        xmlrpc = importModule("xmlrpc");
        xmlrpc_service = new xmlrpc.ServiceProxy(xmlrpc_addr, xmlrpc_methods);

        // 功能映射
        xmlrpc_service.tecs.project_vm = {};
        xmlrpc_service.tecs.project_vm.query = xmlrpc_service.tecs.project.query_vm_by_project;

        xmlrpc_session = $.cookie(window.name + '_session') == null ? "" : $.cookie(window.name + '_session').split("_")[0];
    }
    catch(e)
    {
          reportException(e);
          throw "import of xmlrpc module failed.";
    }

    return xmlrpc_service;
}

//-------------------------------------
function SetOamUiData(elem_ref, bind_ref, raw_value)
{
    var maptype = bind_ref.attr("maptype");
    var bindattr = bind_ref.attr("bindattr");

    var item_value;
    if (maptype == null)
    {
        item_value = raw_value;
    }
    else
    {
        elem_ref.attr("uvalue", raw_value);
        var map_info = page.res[page.pagelan].type[maptype];
        item_value = map_info[raw_value];
    }

    if (bindattr == "html")
    {
        elem_ref.html(item_value);
    }
    else if (bindattr == "text")
    {
        var text_value = String(item_value);
        /*if (text_value == "")
        {
            text_value = " ";
        }*/
        elem_ref.text(text_value);
    }
    else if (bindattr == "data")
    {
        elem_ref.data("item", item_value);
    }
    else if (bindattr == "val")
    {
        elem_ref.val(String(item_value));
    }
    else if (bindattr == "progress_max")    // 进度条处理
    {
        var progress_val = elem_ref.data("progress_val");
        var progress_max = item_value;
        elem_ref.data("progress_max", progress_max);

        var progress_value = 0;
        if ((progress_val != null)  && (progress_max != null)&& (progress_max != 0) && (progress_val >= 0)/* && (progress_val <= progress_max)*/)
        {
            progress_value = progress_val / progress_max * 100;
        }
        elem_ref.progressbar( "option", "value", progress_value );
    }
    else if (bindattr == "progress_val")    // 进度条处理
    {
        var progress_val = item_value;
        var progress_max = elem_ref.data("progress_max");
        elem_ref.data("progress_val", progress_val);

        var progress_value = 0;
        if ((progress_val != null)  && (progress_max != null)&& (progress_max != 0) && (progress_val >= 0)/* && (progress_val <= progress_max)*/)
        {
            progress_value = progress_val / progress_max * 100;
        }
        elem_ref.progressbar( "option", "value", progress_value );
    }
    else if (bindattr == "list")    // select特殊处理
    {
        elem_ref.html("");
        item_value = eval(item_value);
        for (var key in item_value)
        {
            if (key == "toXmlRpc")
            {
            }
            else
            {
                elem_ref.append("<option value='"+key+"'>"+item_value[key]+"</option>");
            }
        }
    }
    else
    {
        elem_ref.attr(bindattr, item_value);
    }
}
function SetOamUiDefault(elem_ref, bind_ref)
{
    var maptype = bind_ref.attr("maptype");
    var bindattr = bind_ref.attr("bindattr");

    if (bindattr == "html")
    {
        elem_ref.html("");
    }
    else if (bindattr == "text")
    {
        elem_ref.text(String(""));
    }
    else if (bindattr == "data")
    {
        elem_ref.data("item", null);
    }
    else if (bindattr == "val")
    {
        elem_ref.val(String(""));
    }
    else if (bindattr == "list")    // select特殊处理
    {
        var first_value = bind_ref.find(":first").val();
        bind_ref.val(first_value);
    }
    else
    {
        elem_ref.attr(bindattr, null);
    }
}
function GetOamUiBindData(elem_ref, bind_ref)
{
    var item_value;

    var maptype = bind_ref.attr("maptype");
    if (maptype == null)
    {
        var bindattr = bind_ref.attr("bindattr");

        if (bindattr == "html")
        {
            item_value = elem_ref.html();
        }
        else if (bindattr == "text")
        {
            item_value = elem_ref.text();
        }
        else if (bindattr == "data")
        {
            item_value = elem_ref.data("item");
        }
        else if (bindattr == "val")
        {
            item_value =  elem_ref.val();
        }
        else if (bindattr == "list")    // select特殊处理
        {
            item_value = null;
        }
        else
        {
            item_value = elem_ref.attr(bindattr);
        }
    }
    else
    {
        item_value = elem_ref.attr("uvalue");
    }

    var bind = bind_ref.attr("bind");

    if (bind == ".")
    {
        return {bind_value:item_value};
    }
    else
    {
        return {bind_name:bind, bind_value:item_value};
    }
}
function SetOamUiBindData(elem_ref, bind_ref, item)
{
    var bind = bind_ref.attr("bind");

    var raw_value;
    if (bind == ".")
    {
        raw_value = item;
    }
    else
    {
        var arrattr = bind.split(".");
        var tmp = item ;
        for(var i = 0;i < arrattr.length;i++)
        {
            var attr = arrattr[i];
            raw_value = tmp[attr];
            tmp = tmp[attr];
        }
    }
    SetOamUiData(elem_ref, bind_ref, raw_value);
}
//-------------------------------------
// table操作
function ClearTable(table)
{
    $(table).find(".user_tr").remove();
}
function SetTableData(table, data_list)
{
    ClearTable(table);
    for(var i=0; i<data_list.length; i++)
    {
        // 数据
        var item = data_list[i];
        // 界面元素
        var newtr = $(table).find(".tmp").clone().removeClass("tmp").addClass("user_tr").css("display", "");
        // 关联
        newtr.find(".OamUi").each(function(){
            SetOamUiBindData($(this), $(this), item);
        });
        newtr.find("OamUi").each(function(){
            SetOamUiBindData($(this).parent(), $(this), item);
        });
        newtr.find("td:first").find("Input").data("item", item);
        newtr.find(".OamButton").each(function(){
            var fn = eval($(this).attr("fn"));
            $(this).unbind("click", fn);
            $(this).bind("click", fn);
        });

        newtr.appendTo(table);
    }
    if (data_list.length == 0)
    {
        $(table).find("tfoot").show();
    }
    else
    {
        $(table).find("tfoot").hide();
    }

    // 交替样式
    $(table).find("tr.user_tr:odd").addClass("odd");
    $(table).find("tr.user_tr:even").addClass("even");
}
function OnSelectAllClick()
{
    var table_ref = $(this).parentsUntil("table").parent();
    var checked_value = $(this).attr("checked");
    if (checked_value)
    {
        SelectAllItem(table_ref);
    }
    else
    {
        SelectNoneItem(table_ref);
    }
}
function SelectAllItem(table_ref)
{
    table_ref.find("tbody input[type=checkbox]").attr("checked", true);
    // clear tmp
    $(".tmp").find(":checkbox").attr("checked", false);
}
function SelectNoneItem(table_ref)
{
    table_ref.find("tbody input[type=checkbox]").attr("checked", false);
    // clear tmp
    $(".tmp").find(":checkbox").attr("checked", false);
}
//-------------------------------------
// 从table的一行，生成修改界面
function SetModifyForm(form_ref, item)
{
    form_ref.find("OamUi").each(function(){
        SetOamUiBindData($(this).parent(), $(this), item);
        });
    form_ref.find(".OamUi").each(function(){
        SetOamUiBindData($(this), $(this), item);
        });
    form_ref.find(".OamButton").each(function(){
        var fn = eval($(this).attr("fn"));
        $(this).unbind("click", fn);
        $(this).bind("click", fn);
      });
}
function GetModifyForm(form_ref)
{
    var item = {};
    form_ref.find(".OamUi").each(function (){
            var bind_data = GetOamUiBindData($(this), $(this));
            item[bind_data.bind_name] = bind_data.bind_value;
        });
    return item;
}
function ClearModifyForm(form_ref)
{
    form_ref.find(".OamUi").each(function (){
            var var_name = $(this).attr("name");
            if (var_name != null)
            {
                SetOamUiDefault($(this), $(this));
            }
        });
}
//-------------------------------------------
function GetKeyList(list, key_field_name, value_field_name)
{
    if (value_field_name == null)
    {
        value_field_name = key_field_name;
    }

    var result = {};
    for(var i=0; i<list.length; i++)
    {
        var item = list[i];
        result[item[key_field_name]] = item[value_field_name];
    }
    return result;
}

function TecsObjList(obj_name, rs_index, param1, param2, param3, param4)
{
    var fn;

    if(obj_name == "dhcp.ip"){
        fn = xmlrpc_service.tecs.dhcp.ip.query;
    }
    else if(obj_name == "vmcfg.staticinfo"){
        fn = xmlrpc_service.tecs.vmcfg.staticinfo.query;
    }
    else if(obj_name == "vmcfg.runninginfo")
    {
        fn =  xmlrpc_service.tecs.vmcfg.runninginfo.query;
    }
    else {
        fn = xmlrpc_service.tecs[obj_name].query;
    }

    var result = new Array();
    var rs;

    if(obj_name == "clustercfg") {

        if (param1 == null) {
            rs = fn(xmlrpc_session);
        }
        else if (param2 == null) {
            rs = fn(xmlrpc_session, param1);
        }
        else if (param3 == null) {
            rs = fn(xmlrpc_session, param1, param2);
        }
        else if (param4 == null) {
            rs = fn(xmlrpc_session, param1, param2, param3);
        }
        else {
            rs = fn(xmlrpc_session, param1, param2, param3, param4);
        }
        
        if (rs[0] == 0) {
            result = rs[rs_index];
        }
        else {
            ShowNotyRpcErrorInfo(rs);
            result = null;
        }
    }
    else {

            var start_index = new I8(0);
            var query_count = new I8(50);

            do{

                if (param1 == null) {
                    rs = fn(xmlrpc_session, start_index, query_count);
                }
                else if (param2 == null) {
                    rs = fn(xmlrpc_session, start_index, query_count, param1);
                }
                else if (param3 == null)  {
                    rs = fn(xmlrpc_session, start_index, query_count, param1, param2);
                }
                else if (param4 == null)  {
                    rs = fn(xmlrpc_session,  start_index, query_count, param1, param2, param3);
                }
                else {
                    rs = fn(xmlrpc_session,  start_index, query_count, param1, param2, param3,param4);
                }

                if (rs[0] == 0) {
                    result = result.concat(rs[rs_index + 2]);
                }
                else if(rs[0] == 2038){
                    break;
                }
                else{ 
                    ShowNotyRpcErrorInfo(rs);
                    result = null;
                    break;
                }

                start_index = rs[1];

            }while(rs[1] != -1);

    }

    return result;
}
//---------------------------------------------
function SetFilterItemFlag(sFilter, sData){
    var pos = -1;
    var rtn = sData;
    if(sData == "" || sData == null){
        return rtn;
    }
    if(sFilter == "" || sFilter == null){
        return rtn;
    }
    pos = sData.indexOf(sFilter);
    if(pos < 0){
        return rtn;
    }
    rtn = sData.substr(0,pos);
    rtn += '<span style="background-color: #FFFF00;">';
    rtn += sData.substr(pos,sFilter.length);
    rtn += '</span>';
    if(pos + sFilter.length < sData.length){
        rtn += sData.substr(pos + sFilter.length);
    }
    return rtn;
}

function SetFilterFlag(aoData,json,nofilter){
    var filter = "";
    var filterflag = true;
    for(var i = 0;i < aoData.length;i++){
        if(aoData[i]["name"] == "sSearch"){
            filter = aoData[i]["value"];
        }
    }
    if(filter == ""){
        return;
    }
    for(var i = 0;i < json.aaData.length;i++)
    {
        for(var j = 0;j < json.aaData[i].length;j++)
        {
            filterflag = true;
            if(nofilter != null){
                for(var k = 0;k < nofilter.length;k++){
                    if(j == nofilter[k]){
                        filterflag = false;
                        break;
                    }
                }
            }
            if(filterflag){
                json.aaData[i][j] = SetFilterItemFlag(filter,json.aaData[i][j]);
            }
        }
    }
}

LoadXMLStr = function(xmlString){
    var xmlDoc=null; 
    if($.browser.msie){//window.DOMParser 判断是否是非ie浏览器
        var xmlDomVersions = ['MSXML.2.DOMDocument.6.0','MSXML.2.DOMDocument.3.0','Microsoft.XMLDOM'];
        for(var i=0;i<xmlDomVersions.length;i++){
            try{
                xmlDoc = new ActiveXObject(xmlDomVersions[i]);
                xmlDoc.async = false;
                xmlDoc.loadXML(xmlString); //loadXML方法载入xml字符串
                break;
            }catch(e){
                
            }
        }
    }
    else{
        try{//Firefox, Mozilla, Opera, etc.
            parser = new DOMParser();
            xmlDoc = parser.parseFromString(xmlString,"text/xml");
        }
        catch(e){
            alert(e.message);
        }   
    }

    if(xmlDoc == null){
        alert("xmlparser not work.");
    }
    
    return xmlDoc;
}



function RecordSelItem(datatable,table_name,curstat){
    if(curstat!=null){
        if(curstat.itemlist == null){
            curstat.itemlist = new Array();
        }
        curstat.itemlist = [];
        var vCheckedbox = $("#" + table_name + ">tbody").find(":checkbox:checked");
        vCheckedbox.each(function(){
            curstat.itemlist.push($(this).attr("value"));
        });
    }
    var oSettings = datatable.fnSettings();
    curstat.pageno = oSettings._iDisplayStart / oSettings._iDisplayLength;
}

function GetItemCheckbox(itemindex,curstat) {
    var chkbox = '<input type="checkbox" value=' + '"' + itemindex + '"';
    if(curstat != null && curstat.itemlist != null){
        for(var i = 0;i < curstat.itemlist.length;i++){
            if(itemindex == curstat.itemlist[i]){
                chkbox += ' checked';
            }
        }
    }
    chkbox += ' />';
    return chkbox;
}
function GetItemCheckboxWithFunc(itemindex,curstat,keys,values) {
    var chkbox = '<input type="checkbox" value=' + '"' + itemindex + '" ';
    for(var i=0;i<keys.length;i++){
        if(keys[i] != null && values[i] != null){
            chkbox += "  ";
            chkbox += keys[i] ;
            chkbox += ' = ';
            chkbox += values[i] ;
        }
    }
    if(curstat != null && curstat.itemlist != null){
        for(var i = 0;i < curstat.itemlist.length;i++){
            if(itemindex == curstat.itemlist[i]){
                chkbox += ' checked';
            }
        }
    }
    chkbox += ' />';
    return chkbox;
}
function CheckboxIsChecked(itemindex, curstat){
    if(curstat != null && curstat.itemlist != null){
        for(var i = 0;i < curstat.itemlist.length;i++){
            if(itemindex == curstat.itemlist[i]){
                return " checked ";
            }
        }
    }
    return "";
}

function GotoCurrentPageOfDataTable(datatable){
    var oSettings = datatable.fnSettings();
    var pageno = oSettings._iDisplayStart / oSettings._iDisplayLength;
    datatable.fnPageChange(parseInt(pageno));
    return;
}

function AvoidGotoBlankPage(datatable){
    var oSettings = datatable.fnSettings();
    var wholeNO = oSettings._iRecordsTotal /oSettings._iDisplayLength;
    var orinigalNo = oSettings._iDisplayStart / oSettings._iDisplayLength;
    if(wholeNO == 0){
        return;
    }
    if(wholeNO <= orinigalNo){
        datatable.fnPageChange("last");
    }
}

function ClearCurSelItem(curstat){
    if(curstat != null && curstat.itemlist != null){
        curstat.itemlist = [];
    }
}

//--------------- MISC -------------------
function Capacity(cap) {
    var c = cap;
    var unit_level = 0;
    while (c > 1023) {
        c = parseInt(c);
        c = c / 1024;
        unit_level++;
    }

    var unit_map = [
        "BYTE",
        "K",
        "M",
        "G",
        "T",
        "P"
    ];

    var decimal = unit_level < 2 ? unit_level : 2;

    return Number(c).toFixed(decimal) + " " + unit_map[unit_level];
}

function ShowDiaglog(id, diag_html, diag_option){
    var diag_title = ""
    if(page.res[page.pagelan]["xid"] != null && page.res[page.pagelan]["xid"]["dialog"] != null){
        diag_title = page.res[page.pagelan]["xid"]["dialog"][id];
    }

    var need_init = false;

    if($("#" + id).length == 0) {
        var dialog_html  = '<div id="' + id + '" class="modal hide">';
            dialog_html += '    <div class="modal-header"><h5></h5></div>';
            dialog_html += '    <div class="modal-body">' + diag_html + '</div>';
            dialog_html += '    <div class="modal-footer">';
            dialog_html += '        <button class="btn btn-primary" id="' + id + '_ok_btn">';
            dialog_html +=               ok_cancel[0][page.pagelan] + '</button>';
            dialog_html += '        <button class="btn" data-dismiss="modal" aria-hidden="true" id="' + id + '_cancel_btn">';
            dialog_html +=               ok_cancel[1][page.pagelan] + '</button>';
            dialog_html += '        <button class="btn btn-primary" data-dismiss="modal" aria-hidden="true" id="' + id + '_close_btn">';
            dialog_html +=               ok_cancel[2][page.pagelan] + '</button>';            
            dialog_html += '    </div>'; 
            dialog_html += '</div>';    

        $("#right_content").append(dialog_html);
        c_changeLangue(page);

        need_init = true;

        $("#" + id).modal({
            backdrop:"static",
            keyboard:false,
            "show":false
        });  
    }

    $("#" + id).find('div.modal-header h5').html(diag_title);

    if(diag_option.ok != null){
        $("#" + id + "_ok_btn").unbind("click").bind("click", diag_option.ok);
        $("#" + id + "_close_btn").hide();
        $("#" + id + "_ok_btn").show();
        $("#" + id + "_cancel_btn").show();         
    }

    if(diag_option.cancel != null){
        $("#" + id + "_cancel_btn").unbind("click").bind("click", diag_option.cancel);
    }

    if(diag_option.close != null){
        $("#" + id + "_ok_btn").hide();
        $("#" + id + "_cancel_btn").hide();  
        $("#" + id + "_close_btn").show();                      

        $("#" + id + "_close_btn").unbind("click").bind("click", diag_option.close);        
    }

    $("#" + id).modal('show');              

    if(need_init && diag_option.init != null) {
        diag_option.init();
    }

    if(diag_option.show != null) {
        diag_option.show();
    }    
}

function TogglePopover(item, res){
    var pop = $;
    
    if(item != null){
        pop = item;
    }

    $(pop.find("[popover]")).each(function(){
        var content, idx;
        content = idx = $(this).attr("popover");

        if(res != null && res[idx] != null){
            content = res[idx];
        }
        else if(page.res[page.pagelan].type.display != null && page.res[page.pagelan].type.display[idx] != null){
            content = page.res[page.pagelan].type.display[idx];
        }

        $(this).attr("data-rel", "popover");
        $(this).attr("data-content", content);
    });    

    $('[data-rel="popover"]').popover({"trigger":"click"});
}

function sToDate(s){

    var time ;
    if(s != null){
        time = parseInt(s);
    }
    var date = "";
    var days = parseInt( (time/(24*60*60)).toString() );
    var hours =parseInt( (( time%(24*60*60) )/(60*60)).toString() ); 
    var minutes =parseInt (( ((time%(24*60*60))% (60*60) )/60 ).toString() );
    var seconds= ((time%(24*60*60))%(60*60) )%60 ;
    
    date += days +"days  ";

    date += hours + ": ";

    date += minutes +": ";

    date += seconds ;

            
    return date;  
    
}