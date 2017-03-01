/*--page begin--*/
var page = {
    pageid: "system",
    pagelan: "ch",
    init: function(){
        page.pagelan = $.cookie('current_language') == null ? 'ch' :$.cookie('current_language');
        c_changeLangue(page);
        if(indexpage.pagelan != page.pagelan){         
            $('#languages a[data-value="'+page.pagelan+'"]').find('i').addClass('icon-ok');
            $('#languages a[data-value="'+indexpage.pagelan+'"]').find('i').removeClass('icon-ok');
            indexpage.pagelan = language;    
            indexpage.init();
        }
    }
}; 

var system_menu_html = 
   '<li> \
        <a href="#" onclick="runstate_click()"><span><img src="img/runstate16.png"></img></span><span id="runstate"/></span></a> \
    </li> \
    <li> \
        <a href="#" onclick="workflow_click()"><span><img src="img/workflow16.png"></img></span><span id="workflow"/></span></a> \
    </li> \
    <li style="display:none;"> \
        <a href="#" onclick="version_click()"><span><img src="img/version16.png"></img></span><span id="version"/></span></a> \
    </li> \
    <li> \
        <a href="#" onclick="alarm_click()"><span><img src="img/alarm16.png"></img></span><span id="alarm"/></span></a> \
    </li> \
    <li> \
        <a href="#" onclick="session_click()"><span><img src="img/session16.png"></img></span><span id="session"/></span></a> \
    </li> \
    <li> \
        <a href="#" onclick="license_click()"><span><img src="img/license16.png"></img></span><span id="license"/></span></a> \
    </li>';

$(function(){
    page.init();

    $("#system_list").append(system_menu_html);
    c_changeLangue(page); 

    $.getJSON("../php/srt_nodes.php", function (json){
        all_nodes = json;

        $('ul.main-menu li a').eq(0).trigger("click");
        
        //animating menus on hover
        $('ul.main-menu li:not(.nav-header)').hover(function(){
            $(this).animate({'margin-left':'+=5'},300);
        },
        function(){
            $(this).animate({'margin-left':'-=5'},300);
        });
    });
});

function active_mainmenu_item(index){
    $('ul.main-menu li').removeClass("active");
    $('ul.main-menu li').eq(index).addClass("active");
}

/**
*   系统运行状态
**/
var all_nodes = [];
var sys_infos = [];
var mu_infos = [];
var cb_count = 0;
var activeIndex = 0;
var auto_refresh = false;
var last_click_proc = "";
var runType = 'caculate';
var runTypeOld = false;
var sys_runstate_html = 
   '<div> \
        <ul class="nav nav-tabs" id="sysRunTab"> \
            <li class="active"><a href="#sysRunstateSummary" id="sysRunstateNav"></a></li> \
			<li ><a href="#sysRunstateSummary" id="sysRunstateNet"></a></li> \
			<li ><a href="#sysRunstateSummary" id="sysRunstateSa"></a></li> \
        </ul> \
        <div class="tab-content">\
		  <div class="tab-pane active" id="sysRunstateSummary"> \
            <div class="row-fluid"> \
                <div class="span4 box" style="height:300px;overflow:auto;margin-top:0px;"> \
                    <ul id="tecs_tree" style="list-style:none"></ul> \
                </div> \
                <div class="span8 box" style="height:300px;overflow:auto;margin-top:0px;"> \
                    <div id="proc_list"></div> \
                </div> \
            </div> \
            <div class="row-fluid"><div class="span12 box" id="proc_state"></div></div> \
          </div>\
		</div> \
        <div id="sys_runstate_last_refresh_time"></div> \
    </div>';

function runstate_click(){
    active_mainmenu_item(0);

    $("#right_content").html(sys_runstate_html); 
    c_changeLangue(page);
    	
    $('#sysRunTab a:first').tab('show');
    $('#sysRunTab a').click(function (e) {
        if(cb_count > 0) return;
		if(runTypeOld ==false) return;
        e.preventDefault();
        $(this).tab('show');

        if($(this).attr("id") == "sysRunstateNav"){
			runType = 'caculate';
        }
        if($(this).attr("id") == "sysRunstateNet"){
			runType = 'network';
        }
        if($(this).attr("id") == "sysRunstateSa"){
			runType = 'storage';
        } 
		
        sys_infos = [];
        $("#tecs_tree").empty();
        $("#proc_list").empty();
        $("#proc_state").empty();
        last_click_proc = "";			
		refresh_sys_all_runstate();	
        clear_all_timer();
        curr_timer_id = setInterval(refresh_sys_all_runstate, 30*1000);		
    });
	
	runType = 'caculate';
	runTypeOld = false;   //这样记录一下
	
    sys_infos = [];
    $("#tecs_tree").empty();
    $("#proc_list").empty();
    $("#proc_state").empty();
    last_click_proc = "";		
    refresh_sys_all_runstate();	
    clear_all_timer();
    curr_timer_id = setInterval(refresh_sys_all_runstate, 30*1000);
}


function refresh_sys_all_runstate(){
    if(cb_count > 0) return;

    //获取网络信息
    xmlrpc_service.tecs.system.runtime.query(xmlrpc_session, runType, "", all_runtime_callback);
    cb_count++;
}

function getCurNodes(runType){ 	
	var getnodes = "";
	if(runType =='network'){
	    getnodes = "../php/vnm_nodes.php";
	}else if(runType =='storage'){
	    getnodes = "../php/sa_nodes.php";
	}else{
	    getnodes = "../php/srt_nodes.php";
	}  
	return getnodes;
}

function all_runtime_callback(rs, ex) {
    cb_count--;

    if(rs[0] != 0){
        ShowNotyRpcErrorInfo(rs);
        return;
    }
    sys_infos["tc"] = rs;
	
    var nodephp = getCurNodes(runType);	
    $.getJSON(nodephp, function (json){
        all_nodes = json;
		if(json[0] == -1){
		    return;		
		}
		
        for(var i=0; i<all_nodes.length; i++)
        {
			xmlrpc_service.tecs.system.runtime.query(xmlrpc_session, runType, all_nodes[i].name, nodes_runtime_callback);
            cb_count++;
        }
    });
	
    update_tree_view();
}

function nodes_runtime_callback(rs, ex) {
    cb_count--;

    if(rs[0] != 0){
       ShowNotyRpcErrorInfo(rs);
       return;
    }

    sys_infos[rs[1].target] = rs;

    var hosts = get_hosts_by_clustername(rs[1].name);
	
    for (var i=0;i<hosts.length;i++)
    {
        var hc_item = rs[1].name+","+hosts[i];		 
        xmlrpc_service.tecs.system.runtime.query(xmlrpc_session, runType, hc_item, nodesub_runtime_callback);
        if( i == hosts.length){
		    runTypeOld = true;
		}		
        cb_count++;
    }
    
    //没有时把节点清掉
    if(hosts.length == 0){
        $("#cc_info_list").find("#"+rs[1].name).find("ul").text("");
    }
    
    update_tree_view();
}

function nodesub_runtime_callback(rs, ex) {
    cb_count--;
    if(rs[0] != 0){
       ShowNotyRpcErrorInfo(rs);
       return;
    }
	runTypeOld = true;
    sys_infos[rs[1].target] = rs;
    update_tree_view();
}

function refresh_sys_runstate(){
    if(cb_count > 0) return;

    //获取TC的信息
    xmlrpc_service.tecs.system.runtime.query(xmlrpc_session, "caculate", "", tc_runtime_callback);
    cb_count++;
}

function time_string(sec) {
   var hour = String(sec / 3600).split(".")[0];
   var minute = String((sec % 3600) / 60).split(".")[0];
   var second  = String(sec % 60).split(".")[0];

   var time = "";
   if(hour != "0"){
       time += hour + "h ";
   }

   if(minute != "0"){
       time += minute + "m ";
   }

   if(second != "0"){
       time += second + "s ";
   }

   return time;
}

function get_state_gif(state) {
    if(state == 0){
        return "runtime_ok.png";
    }
    if(state == 1){
        return "runtime_notice.png";
    }
    if(state == 2){
        return "runtime_warning.png";
    }
    if(state == 3){
        return "runtime_error.png";
    }
    return "";
}

function get_state_color(state) {
    if(state == 0){
        return "green";
    }
    if(state == 1){
        return "blue";
    }
    if(state == 2){
        return "orange";
    }
    if(state == 3){
        return "red";
    }
}

function tc_runtime_callback(rs, ex) {
    cb_count--;

    if(rs[0] != 0){
        ShowNotyRpcErrorInfo(rs);
        return;
    }

    sys_infos["tc"] = rs;

    $.getJSON("../php/srt_nodes.php", function (json){
        all_nodes = json;
        for(var i=0; i<all_nodes.length; i++)
        {
            xmlrpc_service.tecs.system.runtime.query(xmlrpc_session, "caculate", all_nodes[i].name, cc_runtime_callback);
            cb_count++;
        }
    });

    update_tree_view();
}

function get_hosts_by_clustername(cluster_name) {
    for(var i=0; i<all_nodes.length; i++) {
        if(all_nodes[i].name == cluster_name) {
            return all_nodes[i].hosts;
        }
    }

    return [];
}

function cc_runtime_callback(rs, ex) {
    cb_count--;

    if(rs[0] != 0){
       ShowNotyRpcErrorInfo(rs);
       return;
    }

    sys_infos[rs[1].target] = rs;

    var hosts = get_hosts_by_clustername(rs[1].name);
    for (var i=0;i<hosts.length;i++)
    {
         var hc_item = rs[1].name+","+hosts[i];
         xmlrpc_service.tecs.system.runtime.query(xmlrpc_session, "caculate", hc_item, hc_runtime_callback);
         cb_count++;
    }
    
    //没有hc时把cc下的节点清掉
    if(hosts.length == 0){
        $("#cc_info_list").find("#"+rs[1].name).find("ul").text("");
    }
    
    update_tree_view();
}

function hc_runtime_callback(rs, ex) {
    cb_count--;

    if(rs[0] != 0){
       ShowNotyRpcErrorInfo(rs);
       return;
    }

    sys_infos[rs[1].target] = rs;

    update_tree_view();
}

function update_node_gif(node, state) {
   var gif = "<img src='../img/" + get_state_gif(state) + "'/>";
   $(node).find("img").replaceWith(gif);
}

function update_tc_node(node_name, node_info) {
    var node = $("#tecs_tree").find("#"+node_name);
    if(node.length == 0){
        one_node_html = "<li id='tc'  style='word-wrap:break-word;word-break:break-all'><img /><a class='selected' onclick='tc_runtime_click(this)'>" + node_info[1].name
                      + "</a><ul id='cc_info_list' style='list-style:none'></ul></li>";

        $("#tecs_tree").append(one_node_html);
        node = $("#tecs_tree").find("#"+node_name);
    }
    $(node).data("proc_info", node_info);
    update_node_gif(node, node_info[1].runtime_state);
}

function update_cc_node(node_name, node_info) {
    var node = $("#cc_info_list").find("#"+node_name);

    if(node.length == 0){
        var cc_node = "<li id=" + node_name + "  style='word-wrap:break-word;word-break:break-all'><img /><a onclick='cc_runtime_click(this)'>" + node_info[1].name
                      + "</a><ul style='list-style:none'></ul></li>";

        $("#cc_info_list").append(cc_node);
        node = $("#cc_info_list").find("#"+node_name);
    }
    $(node).data("proc_info", node_info);
    update_node_gif(node, node_info[1].runtime_state);
}

function update_hc_node(node_name, node_info) {
    var node = document.getElementById(node_name);

    if(node == null){
        var hc_node = "<li id=" + node_name + "  style='word-wrap:break-word;word-break:break-all'><img /><a onclick='hc_runtime_click(this)'>" + node_info[1].name
                      + "</a></li>";

        var idx = node_name.indexOf(",");
        var cc_name = node_name.substring(0, idx);
        var ccnode = $("#cc_info_list").find("#"+cc_name);

        ccnode.find("ul").append(hc_node);
        node = document.getElementById(node_name);
    }
    $(node).data("proc_info", node_info);
    update_node_gif(node, node_info[1].runtime_state);
}

function update_tree_view() {
    for(var info in sys_infos)
    {
        var one_node = sys_infos[info];
        var one_node_html = "";

        if(info == "toXmlRpc") continue;

        if(info == "tc") {
            update_tc_node(info, one_node);
        }
        else {
            var idx = info.indexOf(",");
            if(idx == -1){
                update_cc_node(info, one_node);
            }
            else {
                update_hc_node(info, one_node);
            }
        }
    }

    if(cb_count > 0) return;

    //$("#tecs_tree").treeview();

    var sel_item = $("#tecs_tree").find(".selected");

    if($("#proc_list").text() != ""){
        activeIndex = $("#proc_list").accordion("option", "active");
    }

    auto_refresh = true;
    if(sel_item == null){
        $("#tc a").click();
    }
    else {
        $(sel_item).click();
    }
    auto_refresh = false;

    $("#sys_runstate_last_refresh_time").html(GetLastUpdateTime());
}

/*function init_sys_runtime_page() {
    if(cb_count > 0) return;

    sys_infos = [];
    $("#tecs_tree").empty();
    $("#proc_list").empty();
    $("#proc_state").empty();
    last_click_proc = "";
    refresh_sys_runtime_info();
}*/

function tc_runtime_click(a) {
    $("#tecs_tree a").removeClass("selected");
    $(a).addClass("selected");
    if(!auto_refresh) activeIndex = 0;

    show_proc_info($(a).parent().data("proc_info"));
}

function cc_runtime_click(a) {
    $("#tecs_tree a").removeClass("selected");
    $(a).addClass("selected");
    if(!auto_refresh) activeIndex = 0;

    show_proc_info($(a).parent().data("proc_info"));
}

function hc_runtime_click(a) {
    $("#tecs_tree a").removeClass("selected");
    $(a).addClass("selected");
    if(!auto_refresh) activeIndex = 0;

    var cc = $(a).parent().parent().parent().find("a").eq(0).text();
    var hc = $(a).text();

    show_proc_info($(a).parent().data("proc_info"));
}

function get_name_by_id(a) {
    for(var i=0; i<runtime_res.length; i++){
        if(runtime_res[i]["id"] == a){
            return runtime_res[i][page.pagelan];
        }
    }

    return "unknown";
}

function show_proc_info(rs) {
    $("#proc_list").empty();
    $("#proc_state").empty();

    if(rs[0] == 0){
        var procs = rs[1];
        var first_proc = "";
        var proc_html = "";

        for(var i=0; i<procs.procinfo.length; i++){
            var one_proc = procs.procinfo[i];
            var one_proc_html;

            var proc_id = "proc_" + rs[1].target + "_" + one_proc.name;
            if(first_proc == "") first_proc = proc_id;
            if(one_proc.exist == true) {
                one_proc_html = "<h3><a id=" + proc_id + " onclick=show_mu_info(this) style='color:" + get_state_color(one_proc.runtime_state) + ";'>"
                               +  one_proc.name  + "</a></h3>"
                               + "<div><table><tbody>"
                               + "<tr><td> " + get_name_by_id("running_seconds") + " :" + time_string(one_proc.running_seconds) + "</td></tr>"
                               + "<tr><td> " + get_name_by_id("pid") + " :" + one_proc.pid  + "</td></tr>"
                               + "<tr><td> " + get_name_by_id("exceptions") + " :" + one_proc.exceptions + "</td></tr>"
                               + "<tr><td> " + get_name_by_id("asserts") + " :" + one_proc.asserts.length + "</td></tr>";

                for(var j=0; j<one_proc.asserts.length; j++) {
                    one_proc_html += "    <tr><td>" + one_proc.asserts[j] + "</td></tr>";
                }

                one_proc_html += "</tbody></table></div>";

                mu_infos[proc_id] = one_proc.muinfo;
            }
            else {
                one_proc_html = "<h3><a id=" + proc_id + " onclick=show_mu_info(this) style='color:" + get_state_color(one_proc.runtime_state) + ";'>"
                               + one_proc.name  + "</a></h3>"
                               + "<div><label style='color:red'>no information!</label>"

                one_proc_html += "</div>";

                mu_infos[proc_id] = null;
            }

            proc_html += one_proc_html;
        }

        $("#proc_list").append(proc_html);
        $("#proc_list").accordion( "destroy" )
        $("#proc_list").accordion({ autoHeight: false, active: activeIndex });

        var last_proc = document.getElementById(last_click_proc)
        if(last_proc == null){
            last_proc = document.getElementById(first_proc);
        }

        show_mu_info(last_proc);
    }
}

function show_mu_info(a) {
    $("#proc_state").empty();

    last_click_proc = $(a).attr("id");

    var mu_info = mu_infos[last_click_proc];

    if(mu_info == null) {
        return;
    }

    var mu_html = "<table class='table table-striped table-bordered table-condensed'><thead><tr>";
        mu_html += "<th>" + get_name_by_id("mu") + "</th>";
        mu_html += "<th>" + get_name_by_id("tid") + "</th>";
        mu_html += "<th>" + get_name_by_id("state") + "</th>";
        mu_html += "<th>" + get_name_by_id("push_count") + "</th>";
        mu_html += "<th>" + get_name_by_id("pop_count") + "</th>";
        mu_html += "<th>" + get_name_by_id("queued_count") + "</th>";
        mu_html += "<th>" + get_name_by_id("last_message") + "</th>";
        //mu_html += "<th>" + get_name_by_id("mu") + "age</th>";
        mu_html += "</tr></thead><tbody>";

    for(var j=0; j<mu_info.length; j++){
        var one_mu = mu_info[j];

        var one_mu_html = "<tr style='color:" + get_state_color(one_mu.runtime_state) + ";'>"
                               + "<td class='td_r'>" + one_mu.name
                               + "</td><td class='td_c'>" + one_mu.pid
                               + "</td><td class='td_c'>" + mu_state[one_mu.state][page.pagelan]
                               + "</td><td class='td_c'>" + one_mu.push_count
                               + "</td><td class='td_c'>" + one_mu.pop_count
                               + "</td><td class='td_c'>" + one_mu.queued_count
                               + "</td><td class='td_c'>" + one_mu.last_message;

        one_mu_html += "</td></tr>";

        mu_html += one_mu_html;
    }
    mu_html += "</tbody></table>";

    $("#proc_state").append(mu_html);
}


/**
*  版本管理
**/
var sys_version_html = 
   '<div> \
        <ul class="nav nav-tabs"> \
            <li class="active"><a href="#sysVerSummary" id="sysVerNav"></a><li> \
        </ul> \
        <div class="tab-content"> <div class="tab-pane active" id="sysVerSummary">\
            <div class="row-fluid"> \
                <div class="span6" align="left"> \
                    <span><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Ver.repo"></OamStaticUI></span><span id="repo_url"></span> \
                    (<a href="#" onclick="set_repo_url()"><OamStaticUI class="OamUi" bindattr="text" maptype="opr" bindvalue="setting"></OamStaticUI></a>) \
                </div> \
                <div class="span6" align="right"> \
                    <button class="btn"><i class="icon-check"></i><span id="seletVernode"></span></button> \
                    <button class="btn"><i class="icon-circle-arrow-up"></i><span id="upgradeVersion"></span></button> \
                    <button class="btn"><i class="icon-refresh"></i><span id="refeshVersion"></span></button> \
                </div> \
            </div> \
            <table id="version_list" class="table table-striped table-bordered table-condensed"> \
                <thead> <tr> \
                    <th width="25px;"><INPUT id="version_select_all" type="checkbox" /></th> \
                    <th width="150px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Ver.position"></OamStaticUI></th> \
                    <th width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Ver.package"></OamStaticUI></th> \
                    <th width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Ver.crr_ver"></OamStaticUI></th> \
                    <th width="100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Ver.latest_ver"></OamStaticUI></th> \
                    <th width="75px;"></th> \
                </tr> </thead> \
                <tbody style="word-wrap:break-word;word-break:break-all"> </tbody> \
            </table> \
            <div id="divWaiting" style="background:transparent; z-index:1001; position:absolute; display:none; top:50%; left:50%; margin:-50px 0px 0px -50px;"> \
                <div style="padding:3px 15px 3px 15px;text-align:center;vertical-align:middle;" > \
                    <div><IMG SRC="img/wait.gif" /> </div> \
                </div> \
            </div> \
        </div></div> \
    </div>';

function version_click(){
    active_mainmenu_item(2);

    $("#right_content").html(sys_version_html); 
    c_changeLangue(page);
    $("#right_content").find("OamStaticUI").each(function(){
        SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
    });    

    verTable = $("#version_list").dataTable({
        "bPaginate": false,
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] } ,{ "bSortable": false, "aTargets": [ 1 ] } ,
            { "bSortable": false, "aTargets": [ 2 ] } , { "bSortable": false, "aTargets": [ 3 ] } ,
            { "bSortable": false, "aTargets": [ 4 ] } , { "bSortable": false, "aTargets": [ 5 ] }],
        "sDom":"t",
        "alternatively" : "destory"
    });   

    $("#seletVernode").parent().click(init_version);
    $("#refeshVersion").parent().click(refresh_version);
    $("#upgradeVersion").parent().click(upgrade_version);

    init_version(); 
}

var ver_cb_count = 0;
var ver_node_selected = [];
var ver_progress_timer;

//刷新版本信息
function init_version() {
    if(verTable.find("tr .details").length > 0){
        alert("some version is upgrading...");
        return;
    }

    if(ver_cb_count > 0){
        return;
    }

    var rs = xmlrpc_service.tecs.version.get_repo_addr(xmlrpc_session);
    if(rs[0] == 0){
        $("#repo_url").text(rs[2]);
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }

    var select_vernode_dlg_html = 
       '<table class="table table-striped table-bordered table-condensed" id="node_select_list"> \
            <thead> <tr> \
                <th width="20px"><input id="node_select_all" type="checkbox" /></th> \
                <th><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Ver.node"></OamStaticUI></th> \
            </tr> </thead> \
            <tbody style="table-layout:fixed;word-break:break-all;"> <tr class="tmp"> \
                <td width="20px"><input type="checkbox" class="OamUi" bindattr="data" bind="." /></td> \
                <td><Label type="text" class="OamUi" bindattr="html" bind="vernode"></label></td> \
            </tr> </tbody> \
        </table>';

    $.getJSON("../php/ver_nodes.php", function (json){
        var versionData = [];
        for(var i=0; i<json.length; i++){
            json[i]["vernode"] = node_name(json[i]);
        }

        ShowDiaglog("select_vernode_dlg", select_vernode_dlg_html, {
            show:function(){
                var h5 = $("#select_vernode_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-check"></i>' + h5.text()); 

                $("#node_select_list").find("OamStaticUI").each(function(){
                    SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
                }); 

                SetTableData("#node_select_list", json);
                $("#node_select_list").trigger("update");

                $("#node_select_all").click(node_select_all_click);
            },
            ok:function(){
                var vCheckedbox = $("#node_select_list tbody input[type=checkbox]:checked");
                if(vCheckedbox.size() > 0){
                    ver_node_selected = [];
                    vCheckedbox.each(function(){
                        ver_node_selected[ver_node_selected.length] = $(this).data("item");
                    });
                    $("#select_vernode_dlg").modal("hide");
                    refresh_version();
                }
                else{
                    ShowNotyRpcErrorInfo(["no_operate_obj", ""]);
                }                
            }
        });
    });
}

function node_checkbox(a) {
   var cb  = "<INPUT type='Checkbox'";
   cb += " id=" + a["type"] + "_"  + a["cc"] + "_" + a["hc"];
   cb += " nodetype='" + a["type"]  + "'";
   cb += " tc='" + a["tc"] + "' tc_ip='" + a["tc_ip"] + "'";
   cb += " cc='" + a["cc"] + "' cc_ip='" + a["cc_ip"] + "'";
   cb += " hc='" + a["hc"] + "' hc_ip='" + a["hc_ip"] + "'";
   cb += " />";

   return cb;
}

function node_name(a) {
    var node;

    if(a["type"] == "tc"){
        node = "<div class='tb_space'></div>" + a["tc"];
    }
    else if(a["type"] == "sa"){
        node = "<div class='tb_space'></div><div class='tb_space'></div>" + a["sa"];
    }     
    else if(a["type"] == "cc"){
        node = "<div class='tb_space'></div><div class='tb_space'></div>" + a["cc"];
    }
    else if(a["type"] == "hc"){
        node = "<div class='tb_space'></div><div class='tb_space'></div><div class='tb_space'></div>" + a["hc"];
    }
    else{
        alert("error node type!!!");
        node = "";
    }

    return node;
}

function refresh_version() {
    if(verTable.find("tr .details").length > 0){
        alert("some version is upgrading...");
        return;
    }

    if(ver_cb_count > 0){
        return;
    }

    verTable.fnClearTable();

    for(var i=0; i<ver_node_selected.length; i++){
        var param = {};
        param._node_type    = ver_node_selected[i]["type"];
        param._cloud_name   = ver_node_selected[i]["tc"];
        param._cloud_ip     = ver_node_selected[i]["tc_ip"];
        param._cluster_name = ver_node_selected[i]["cc"];
        param._cluster_ip   = ver_node_selected[i]["cc_ip"];
        param._host_name    = ver_node_selected[i]["hc"];
        param._host_ip      = ver_node_selected[i]["hc_ip"];
        param._storage_adapter_name    = ver_node_selected[i]["sa"];
        param._storage_adapter_ip      = ver_node_selected[i]["sa_ip"];            
        param._package      = "";        

        xmlrpc_service.tecs.version.query(xmlrpc_session, param, ver_query_callback);
        ver_cb_count++;
    }    

    if(ver_cb_count > 0) {
        $("#divWaiting").show();
    }

    clear_all_timer();
    curr_timer_id = setInterval(refresh_ver_progress, 3000);
}

function ver_checkbox(a) {
   var cb  = "<INPUT type='Checkbox'";

   if(a["type"] != "hc") {
        cb += " disabled='disabled'";
   }

   cb += " nodetype='" + a["type"]  + "'";
   cb += " tc='" + a["tc"] + "' tc_ip='" + a["tc_ip"] + "'";
   cb += " cc='" + a["cc"] + "' cc_ip='" + a["cc_ip"] + "'";
   cb += " hc='" + a["hc"] + "' hc_ip='" + a["hc_ip"] + "'";
   cb += " />";

   return cb;
}

function ver_position(a) {
    var pos = a["tc"];
    if(a["cc"] != "")
        pos += " / " + a["cc"];
    if(a["hc"] != "")
        pos += " / " + a["hc"];

    return pos;
}

function ver_query_callback(rs, ex) {
    ver_cb_count--;

    var node = {};
    node["type"]  = rs[2]._node_type;
    node["tc"]    = rs[2]._cloud_name;
    node["tc_ip"] = rs[2]._cloud_ip;
    node["cc"]    = rs[2]._cluster_name;
    node["cc_ip"] = rs[2]._cluster_ip;
    node["hc"]    = rs[2]._host_name;
    node["hc_ip"] = rs[2]._host_ip;

    if(rs[0] == 0){
        var versionData = [];

        for(var j=0; j<rs[2]._version_info.length; j++){
            var oneVersion = [];
            oneVersion[0] = ver_checkbox(node);
            oneVersion[1] = ver_position(node);
            oneVersion[2] = rs[2]._version_info[j]._package_name;
            oneVersion[3] = rs[2]._version_info[j]._local_version;
            oneVersion[4] = rs[2]._version_info[j]._repo_version;
            oneVersion[5] = "";
            versionData.push(oneVersion);
        }

        verTable.fnAddData(versionData);
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }

    if(ver_cb_count == 0){
        $("#divWaiting").hide();
    }
}

function refresh_ver_progress() {
    var param = [];
    param.push({ "name": "num",  "value": 10000});

    $.getJSON("../php/ver_update.php", param, function (json){
         for(var i=0; i<json.length; i++){
            if(json[0] != 0){
                alert(json[1]);
                return;
            }

            $("#ver_result" + json[i]["oid"]).text(get_result_string(json[i]["result"]));

            if(json[i]["percent"] == 100){
                var ver_tr = $("#"+json[i]["oid"]).parent().parent().prev();
                if(verTable.fnIsOpen(ver_tr[0])){
                    verTable.fnClose(ver_tr[0]);
                }
                if(json[i]["result"] == 0){
                    $("#ver_result" + json[i]["oid"]).parent().parent().find("td").eq(4).text(json[i]["repo_ver"]);
                }
            }
            else {
                $("#"+json[i]["oid"]).progressBar(json[i]["percent"], {showText: true});
            }
         }
    });
}

//升级版本
function upgrade_version() {
    if(ver_cb_count > 0){
        return;
    }

    var vCheckedbox = $("#version_list tbody input[type=checkbox]:checked:not('[class=vernode]')");

    if(vCheckedbox.size() > 0){
        vCheckedbox.each(function(){
            var param = {};
            param._node_type    = $(this).attr("nodetype");
            param._cluster_name = $(this).attr("cc");
            param._cluster_ip   = $(this).attr("cc_ip");
            param._host_name    = $(this).attr("hc");
            param._host_ip      = $(this).attr("hc_ip");
            param._package      = $(this).parent().parent().find("td").eq(2).text();

            var rs = xmlrpc_service.tecs.version.update(xmlrpc_session, param);

            if(rs[0] == 0){
                var progressbar_id = rs[2];//rs[1];

                var ver_tr = $(this).parent().parent();
                var progressbar = '<span class="progressBar" id="' + progressbar_id + '"></span>';
                    progressbar += '<button onclick="cancel_ver_upgrade(' + progressbar_id + ')">取消</button>';
                verTable.fnOpen(ver_tr[0], progressbar, 'details');
                $("#"+progressbar_id).progressBar(1, {showText: true});

                var detail_html = "<span id=ver_result" + progressbar_id + "></span>";
                    detail_html += "(<a href='javascript:void(0)' onclick=ver_upgrade_detail(" + progressbar_id + ")>详情</a>)";
                $(this).parent().parent().find("td").eq(5).html(detail_html);
            }
            else{
                ShowNotyRpcErrorInfo(rs);
            }
        })
    }
    else {
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]);
    }
}

function set_repo_url() {
    var set_repo_dlg_html = 
       '<form class="form-horizontal"><fieldset> \
            <div class="control-group"> \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Ver.repo"></OamStaticUI></label> \
                <div class="controls"><input type="text" id="repo_url_input" style="width:144px;" /></div> \
            </div> \
        </fieldset></form>'; 

    ShowDiaglog("set_repo_dlg", set_repo_dlg_html, {
        show:function(){
            var h5 = $("#set_repo_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-edit"></i>' + h5.text()); 

            $("#set_repo_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            }); 

            var repo = $("#repo_url").text();
            $("#repo_url_input").val(repo);

            $(".control-group").removeClass("error");
        },
        ok: function(){
            $(".control-group").removeClass("error");

            var repo_url_str = $("#repo_url_input").val();
            if(repo_url_str == ""){
                $("#repo_url_input").parent().parent().addClass("error");
                return;
            }

            var rs = xmlrpc_service.tecs.version.set_repo_addr(xmlrpc_session, repo_url_str);
            if(rs[0] == 0){
                $("#repo_url").text(rs[2]);
                $("#set_repo_dlg").modal("hide");
            }
            else {
                ShowNotyRpcErrorInfo(rs);
            }
        }
    });
}

function cancel_ver_upgrade(upgrade_id) {
    var ver_tr = $("#"+upgrade_id).parent().parent().prev();
    var ver_cb = ver_tr.find("input[type=checkbox]");

    var param = {};
    param._node_type    = ver_cb.attr("nodetype");
    param._cluster_name = ver_cb.attr("cc");
    param._cluster_ip   = ver_cb.attr("cc_ip");
    param._host_name    = ver_cb.attr("hc");
    param._host_ip      = ver_cb.attr("hc_ip");
    param._package      = ver_cb.parent().parent().find("td").eq(2).text();

    var rs = xmlrpc_service.tecs.version.cancel(xmlrpc_session, param);

    if(rs[0] == 0){
        var ver_tr = $("#"+upgrade_id).parent().parent().prev();
        if(verTable.fnIsOpen(ver_tr[0])){
            verTable.fnClose(ver_tr[0]);
        }
    }
    else{
        ShowNotyRpcErrorInfo(rs[0]);
    }
}

function get_result_string(err_code) {
    var ver_update_result = {
        ch:{0:"成功",
            1:"失败",
            25:"运行中",
            9001: "取消",
            9002: "没有可升级的版本",
            9003: "没有安装版本",
            9004: "升级失败",
            9005: "异常退出"},
        en:{0:"success",
            1:"failed",
            25:"running",
            9001: "cancel",
            9002: "no package",
            9003: "not install",
            9004: "operate failed",
            9005: "abnormal exit"}
    };

    if(ver_update_result[page.pagelan][err_code] == null) {
        return err_code;
    }
    else {
        return ver_update_result[page.pagelan][err_code];
    }
}

function ReplaceEnterFlag(str) {
    var newstr = "";
    var reg=new RegExp("\n","g"); //创建正则RegExp对象
    newstr=str.replace(reg,"<br/>");
    return newstr;
}

function ver_upgrade_detail(upgrade_id) {
    var param = [];
    param.push( { "name": "oid",  "value": upgrade_id} );

    var upgrade_detail_dlg_html =
       '<table> <tbody style="table-layout:auto;word-break:break-all;"> <tr> \
            <td class="td_l"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Ver.detail"></OamStaticUI></td> \
            <td class="td_r"><div id="ver_detail"></div></td> \
        </tr> </tbody> </table>';

    $.getJSON("../php/ver_detail.php", param, function (json){
        ShowDiaglog("upgrade_detail_dlg", upgrade_detail_dlg_html, {
            show: function(){
                var h5 = $("#upgrade_detail_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-zoom-in"></i>' + h5.text()); 

                $("#upgrade_detail_dlg").find("OamStaticUI").each(function(){
                    SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
                }); 

                $("#ver_detail").html(ReplaceEnterFlag(json[1]));
            },
            close:function(){
                $("#upgrade_detail_dlg").modal("hide");
            }
        });
    });
}

function version_select_all_click() {
    if($(this).attr("checked") == "checked") {
        $("#version_list tbody input[type=checkbox]:not(:disabled)").attr("checked", true);
    }
    else {
        $("#version_list tbody input[type=checkbox]:not(:disabled)").attr("checked", false);
    }
}

function node_select_all_click() {
    if($(this).attr("checked") == "checked") {
        $("#node_select_list tbody input[type=checkbox]").attr("checked", true);
    }
    else {
        $("#node_select_list tbody input[type=checkbox]").attr("checked", false);
    }

    $("#node_select_list .tmp").find(":checkbox").attr("checked", false);
}


/**
*  告警
**/
var alarm_html = 
   '<div style="display:none;"><ul class="breadcrumb"></ul></div> \
    <ul class="nav nav-tabs" id="sysAlarm"> \
        <li class="active"><a href="#sysCurrAlarm" id="sysCurrAlarmNav"></a></li> \
        <li><a href="#sysHisAlarm" id="sysHisAlarmNav"></a></li> \
        <li><a href="#sysSnmp" id="sysSnmpNav"></a></li> \
    </ul> \
    <div class="tab-content"> \
        <div id="sysCurrAlarm" class="tab-pane active"> \
            <div align="right"> \
                <button class="btn"><i class="icon-refresh"></i><span id="currAlarmRefresh"></span></button> \
            </div> \
            <table id="tbCurrAlarm" class="table table-striped table-bordered table-condensed"> \
                <thead> <tr> \
                    <th align="left" width="40px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Alarm.alarm_id"/></th> \
                    <th align="left" width="60px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Alarm.alarm_code"/></th> \
                    <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Alarm.alarm_time"/></th> \
                    <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Alarm.logic_address"/></th> \
                    <th align="left" width="100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Alarm.alarm_obj"/></th> \
                    <th align="left" width="150px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Alarm.alarm_reason"/></th> \
                    <th align="left" width="150px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Alarm.alarm_advice"/></th> \
                </tr> </thead> \
                <tbody style="table-layout:fixed;word-break:break-all"></tbody> \
            </table>  \
        </div> \
        <div id="sysHisAlarm" class="tab-pane"> \
            <div align="right"> \
                <button class="btn"><i class="icon-refresh"></i><span id="hisAlarmRefresh"></span></button> \
            </div> \
            <table id="tbHisAlarm" class="table table-striped table-bordered table-condensed"> \
                <thead> <tr> \
                    <th align="left" width="60px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Alarm.alarm_code"/></th> \
                    <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Alarm.alarm_time"/></th> \
                    <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Alarm.restore_time"/></th> \
                    <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Alarm.logic_address"/></th> \
                    <th align="left" width="100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Alarm.alarm_obj"/></th> \
                    <th align="left" width="150px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Alarm.alarm_reason"/></th> \
                    <th align="left" width="150px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Alarm.alarm_advice"/></th> \
                </tr> </thead> \
                <tbody style="table-layout:fixed;word-break:break-all"></tbody> \
            </table> \
        </div> \
        <div id="sysSnmp" class="tab-pane"> \
            <div class="row-fluid"> <div class="span12 box"> \
                <table> \
                    <tr> \
                        <td class="td_r"><span id="community"></span><span><font color="red">*</font></span></td> \
                        <td><input type="text" id="community_val" /></td> \
                    </tr> \
                    <tr> \
                        <td class="td_r"><span id="trap_community"></span><span><font color="red">*</font></span></td> \
                        <td><input type="text" id="trap_community_val" /></td> \
                    </tr> \
                    <tr> \
                        <td class="td_r"><span id="trap_version"></span><span><font color="red">*</font></span></td> \
                        <td> \
                            <INPUT type="radio" class="trap_ver" name="trap_ver" value=0 checked="true">V1 \
                            <INPUT type="radio" class="trap_ver" name="trap_ver" value=1>V2c \
                        </td> \
                    </tr> \
                    <tr> \
                        <td class="td_r"></td> \
                        <td><button class="btn"><i class="icon-ok"></i><span id="modify_snmp_config"></span></button></td> \
                    </tr> \
                </table> \
            </div> </div> \
            <div class="row-fluid"> <div class="span12 box"> \
                <table> <tr> \
                    <td class="td_r"><span id="snmp_user"></span><span><font color="red">*</font></span></td> \
                    <td><input type="text" id="snmp_user_val" /></td> \
                </tr> <tr> \
                    <td class="td_r"></td> \
                    <td><button class="btn"><i class="icon-ok"></i><span id="modify_snmp_user"></span></button></td> \
                </tr></table> \
            </div> </div> \
            <div class="row-fluid"> <div class="span12 box"> \
                <div align="right">  \
                    <button class="btn"><i class="icon-plus"></i><span id="add_trapaddr"></span></button> \
                    <button class="btn"><i class="icon-trash"></i><span id="delete_trapaddr"></span></button> \
                </div> \
                <table id="snmp_trap_list" class="table table-striped table-bordered table-condensed"> \
                    <thead> <tr> \
                        <th align="left" width="25px;"><INPUT id="snmp_trap_select_all" type="checkbox" /></th> \
                        <th align="left" width="175px;"><span id="snmp_trap_t1">IP</span></th> \
                        <th align="left" width="100px;"><span id="snmp_trap_t2">Port</span></th> \
                    </tr> </thead> \
                    <tbody> </tbody> \
                </table> \
            </div> </div> \
        </div> \
        <div id="alarm_last_refresh_time"></div><br /> \
    </div>'; 

function alarm_click(){
    active_mainmenu_item(3);
    $("#right_content").html(alarm_html);  

    c_changeLangue(page);

    $("#sysCurrAlarm, #sysHisAlarm").find("OamStaticUI").each(function(){
        SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
    });     

    $('#sysAlarm a:first').tab('show');
    $('#sysAlarm a').click(function (e) {
        e.preventDefault();
        $(this).tab('show');

        if($(this).attr("id") == "sysCurrAlarm"){
            RefreshCurAlarmData();
        }
        if($(this).attr("id") == "sysHisAlarm"){
            RefreshHisAlarmData();
        }
        if($(this).attr("id") == "sysSnmpNav"){
            refresh_snmp_info();
        }  
    });

    oCurTable = $('#tbCurrAlarm').dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 4 ] } ,
                         { "bSortable": false, "aTargets": [ 5 ] } ,
                         { "bSortable": false, "aTargets": [ 6 ] } ],
        "bRetrieve" : true,
        "bDestroy" : true,
        "bProcessing": false,
        "bServerSide": true,
        "sAjaxSource": "../php/current_alarm.php",
        "fnServerData": CurrentAlarmRetrieveData
    });

    oHosTable = $('#tbHisAlarm').dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 4 ] } ,
                         { "bSortable": false, "aTargets": [ 5 ] } ,
                         { "bSortable": false, "aTargets": [ 6 ] }],
        "bRetrieve" : true,
        "alternatively" : "destory",
        "bDestroy" : true,
        "bProcessing": false,
        "bServerSide": true,
        "sAjaxSource": "../php/history_alarm.php",
        "fnServerData": HistoryAlarmRetrieveData
    });

    snmpTrapTable = $("#snmp_trap_list").dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "sDom":'t',
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] }]
    });   

    $("#currAlarmRefresh").parent().click(RefreshCurAlarmData);
    $("#hisAlarmRefresh").parent().click(RefreshHisAlarmData); 
    $("#modify_snmp_config").parent().click(modify_snmp_config_click);
    $("#modify_snmp_user").parent().click(modify_snmp_user_click);
    $("#add_trapaddr").parent().click(add_trapaddr_click);
    $("#delete_trapaddr").parent().click(delete_trapaddr_click); 
    $("#snmp_trap_select_all").click(snmp_trap_select_all); 

    clear_all_timer();          
    curr_timer_id = setInterval(TimerUpDate, 30*1000);
}

function GetAlarmInfo(alarm_code,add_info) {
    var ret = [];

    ret["name"]     = "";
    ret["reason"]   = "";
    ret["solution"] = "";

    if(add_info != "") {
        var alarm_doc = LoadXMLStr(add_info);
        ret["name"] = alarm_doc.firstChild.firstChild.text + " ";
    }

    for(var i=0;i<alarmData.length;i++) {
        if(alarm_code == alarmData[i].id) {
            if(page.pagelan == "ch") {
               ret["name"] += alarmData[i].name_ch + "<br />";
               ret["reason"] = alarmData[i].reason_ch + "<br />";
               ret["solution"] = alarmData[i].op_ch + "<br />";
            } else {
               ret["name"] += alarmData[i].name_en + "<br />";
               ret["reason"] = alarmData[i].reason_en + "<br />";
               ret["solution"] = alarmData[i].op_en + "<br />";
            }
            break;
        }
    }

    return ret;
}

var curalarmstat  = new Object();
var hisalarmstat = new Object();
function TimerUpDate(){
    RefreshCurAlarmData();
    RefreshHisAlarmData();
}

function RefreshCurAlarmData(){
    RecordSelItem(oCurTable,"tbAlarm",curalarmstat);
    GotoCurrentPageOfDataTable(oCurTable);
}

function RefreshHisAlarmData(){
    RecordSelItem(oHosTable,"tbHosAlarm",hisalarmstat);
    GotoCurrentPageOfDataTable(oHosTable);
}

function CurrentAlarmRetrieveData( sSource, aoData, fnCallback ){
    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        var alarm_code    = [];
        var alarm_addinfo = [];
        var oSettings = oCurTable.fnSettings();
        if(oSettings._iDisplayStart != 0 && ackdata.aaData.length == 0){
            oCurTable.fnPageChange(0);
            return;
        }
        if(json.errcode != ""){
            ShowNotyRpcErrorInfo([json.errcode, GetErrStrOfRes(json.errinfo, page.pagelan, errData)]);
        }
        for(var i = 0;i < ackdata.aaData.length;i++){
            alarm_code[i]    = ackdata.aaData[i][1];
            alarm_addinfo[i] = ackdata.aaData[i][4];
        }
        //为过滤条件添加颜色
        SetFilterFlag(aoData,ackdata);

        for(var i = 0;i < ackdata.aaData.length;i++){
            ackdata.aaData[i][2] = getLocalTime( json.data.aaData[i][2]);
            var alarm_info = GetAlarmInfo(alarm_code[i],alarm_addinfo[i]);
            ackdata.aaData[i][4] = alarm_info["name"];
            ackdata.aaData[i][5] = alarm_info["reason"];
            ackdata.aaData[i][6] = alarm_info["solution"];
        }
        fnCallback(ackdata);
        ClearCurSelItem(curalarmstat);
        $("#alarm_last_refresh_time").html(GetLastUpdateTime());
    });
}

function HistoryAlarmRetrieveData( sSource, aoData, fnCallback ){
    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        var alarm_code    = [];
        var alarm_addinfo = [];
        var oSettings = oHosTable.fnSettings();
        if(oSettings._iDisplayStart != 0 && ackdata.aaData.length == 0){
            oHosTable.fnPageChange(0);
            return;
        }
        if(json.errcode != ""){
            ShowNotyRpcErrorInfo([json.errcode, GetErrStrOfRes(json.errinfo, page.pagelan, errData)]);
        }
        for(var i = 0;i < ackdata.aaData.length;i++){
            alarm_code[i] = ackdata.aaData[i][0];
            alarm_addinfo[i] = ackdata.aaData[i][5];
        }
        //为过滤条件添加颜色
        SetFilterFlag(aoData,ackdata);

        for(var i = 0;i < ackdata.aaData.length;i++){
            ackdata.aaData[i][1] = getLocalTime(json.data.aaData[i][1]);
            ackdata.aaData[i][2] = getLocalTime(json.data.aaData[i][2]);
            var alarm_info = GetAlarmInfo(alarm_code[i],alarm_addinfo[i]);
            ackdata.aaData[i][4] = alarm_info["name"];
            ackdata.aaData[i][5] = alarm_info["reason"];
            ackdata.aaData[i][6] = alarm_info["solution"];
        }
        fnCallback(ackdata);
        ClearCurSelItem(hisalarmstat);
        $("#alarm_last_refresh_time").html(GetLastUpdateTime());
    });
}


function snmp_trap_select_all() {
    if($(this).attr("checked") == "checked") {
        $("#snmp_trap_list tbody input[type=checkbox]").attr("checked", true);
    }
    else {
        $("#snmp_trap_list tbody input[type=checkbox]").attr("checked", false);
    }
}

function refresh_snmp_info() {
    var rs = xmlrpc_service.tecs.snmpconfiguration.query(xmlrpc_session, new I8(0), new I8(50));
    if(rs[0] == 0) {
        $("#community_val").val(rs[4][0].community);
        $("#trap_community_val").val(rs[4][0].trap_community);
        if(rs[4][0].trap_version == 1){
            $(".trap_ver")[1].checked = true;
        }
        else {
            $(".trap_ver")[0].checked = true;
        }
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }

    rs = xmlrpc_service.tecs.snmpuser.query(xmlrpc_session, new I8(0), new I8(50));
    if(rs[0] == 0) {
       if(rs[4].length > 0) {
           $("#snmp_user_val").val(rs[4][0].user_name);
       }
    }
    else {
        ShowNotyRpcErrorInfo(rs);        
    }

    snmpTrapTable.fnClearTable();
    var rs = xmlrpc_service.tecs.trapaddress.query(xmlrpc_session, new I8(0), new I8(50));
    if(rs[0] == 0) {
        var trap_addrs = rs[4];
        for(var i=0; i<trap_addrs.length; i++){
            var trap_ip = trap_addrs[i].ip_address/*num2ip(trap_addrs[i].ipaddress)*/;
            var trap_port = trap_addrs[i].port;

            snmpTrapTable.fnAddData([
                '<input type="checkbox"/>',
                trap_ip,
                trap_port
            ]);
        }
    }
    else {
        ShowNotyRpcErrorInfo(rs);  
    }

    $("#alarm_last_refresh_time").html(GetLastUpdateTime());    
}

function modify_snmp_config_click() {
    var community = $("#community_val").val();
    var trap_community = $("#trap_community_val").val();
    var trap_var = parseInt($(".trap_ver:checked").val());

    if(community == "" || trap_community == ""){
        ShowNotyRpcErrorInfo(["invalid_param",""]); 
        return;
    }

    var rs = xmlrpc_service.tecs.snmpconfiguration["set"](xmlrpc_session, community, trap_community, trap_var);
    
    if(rs[0] != 0){
        ShowNotyRpcErrorInfo(rs);
    }
    else {
        ShowNotyRpcSuccessInfo(rs);
    }
}

function modify_snmp_user_click() {
    var snmp_user = $("#snmp_user_val").val();
    //var snmp_user_pass = $("#snmp_user_pass_val").val();
    if(snmp_user == "" /*|| snmp_user_pass == ""*/){
        ShowNotyRpcErrorInfo(["invalid_param",""]); 
        return;
    }

    var rs = xmlrpc_service.tecs.snmpuser["set"](xmlrpc_session, snmp_user/*, snmp_user_pass*/);
    
    if(rs[0] != 0){
        ShowNotyRpcErrorInfo(rs);
    }
    else {
        ShowNotyRpcSuccessInfo(rs);
    }
}

function add_trapaddr_click() {
   var add_trapaddr_dlg_html =
       '<form class="form-horizontal"> <fieldset>  \
            <div class="control-group">     \
                <label class="control-label"><span id="trap_addr_ip"></span><span><font color="red">*</font></span></label> \
                <div class="controls"><Input type="text" id="trap_addr_ip_in"/></div>   \
            </div>    \
            <div class="control-group">     \
                <label class="control-label"><span id="trap_addr_port"></span><span><font color="red">*</font></span></label>  \
                <div class="controls"><Input type="text" maxlength="6" id="trap_addr_port_in"/></div> \
            </div>    \
        </fieldset> </form>';

    ShowDiaglog("add_trapaddr_dlg", add_trapaddr_dlg_html, {
        show:function(){
            $(".control-group").removeClass("error");

            var h5 = $("#add_trapaddr_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-plus"></i>' + h5.text());             
        },
        ok:function(){
            var trap_addr = $("#trap_addr_ip_in").val();
            var trap_port = $("#trap_addr_port_in").val();

            $(".control-group").removeClass("error");

            if(trap_addr == "") {
                $("#trap_addr_ip").parent().parent().addClass("error"); 
                return;
            }

            if(trap_port == "" || isNaN(trap_port)){
                $("#trap_addr_port_in").parent().parent().addClass("error");                 
                return;
            }

            var rs = xmlrpc_service.tecs.trapaddress["set"](xmlrpc_session, trap_addr/*ip2num(trap_addr)*/, Number(trap_port));
            if (rs[0] == 0) {
                $("#add_trapaddr_dlg").modal("hide");
            }
            else {
                ShowNotyRpcErrorInfo(rs);
            }

            refresh_snmp_info();            
        }
    });
}

function delete_trapaddr_click() {
    ClearAlertInfo();
    var pageVal = page.res[page.pagelan]["xid"]["val"];

    var vCheckedbox = $(snmpTrapTable.fnGetNodes()).find(":checkbox:checked");

    if(vCheckedbox.size() > 0){
        ShowDiaglog("delete_trapaddr_dlg", pageVal["delete_trapaddr_confirm"], {
            show:function(){
                var h5 = $("#delete_trapaddr_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-trash"></i>' + h5.text());                  
            },
            ok:function(){
                $("#delete_trapaddr_dlg").modal("hide");

                var vresultSucceeded = [];
                var vresultFailed    = [];

                vCheckedbox.each(function(){
                    var trap_addr = $(this).parent().parent().find("td").eq(1).text();
                    var trap_port = $(this).parent().parent().find("td").eq(2).text();

                    var rs = xmlrpc_service.tecs.trapaddress["delete"](xmlrpc_session, trap_addr/*ip2num(trap_addr)*/, Number(trap_port));
                    if (rs[0] == 0) {
                        vresultSucceeded.push(trap_addr + " : " + trap_port);
                    }
                    else {
                        var r = trap_addr + " : " + trap_port;// + "(" +ErrorCodeToInfo(rs[0]) +")";
                        vresultFailed.push(r);
                    }

                    ShowSynRpcInfo(rs);
                });

                //ShowNotyBatchSuccessInfo(vresultSucceeded);
                //ShowNotyBatchErrorInfo(vresultFailed);

                refresh_snmp_info();                
            }
        });
    }
    else {
        ShowNotyRpcErrorInfo(["no_operate_obj",""]); 
    }
}

/**
*   会话信息
*/
var session_html =     
   '<div> \
        <ul class="nav nav-tabs"> \
            <li class="active"><a href="#sessionSummary" id="sessionSummaryNav"></a></li> \
        </ul> \
        <div class="tab-content"><div class="tab-pane active" id="sessionSummary"> \
            <div align="right"> \
                <button class="btn"><i class="icon-refresh"></i><span id="sessionRefresh"></span></button> \
            </div> \
            <table id="session_list" class="table table-striped table-bordered table-condensed"> \
                <thead> <tr> \
                    <th align="left"><span id="session_t1"></span></th> \
                    <th align="left"><span id="session_t2"></span></th> \
                </tr> </thead> \
                <tbody> </tbody> \
            </table> \
            <div id="session_last_refresh_time"></div> \
        </div></div> \
    </div>';

function session_click(){
    active_mainmenu_item(4);

    $("#right_content").html(session_html);  
    c_changeLangue(page);

    sessionTable = $("#session_list").dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "alternatively" : "destory"
    });

    $("#sessionRefresh").parent().click( function(){
        get_session_info();
    });

    $("#sessionRefresh").trigger("click");

    clear_all_timer();
    curr_timer_id = setInterval(get_session_info, 30*1000);
}

function get_session_info(){
    sessionTable.fnClearTable();

    var rs = xmlrpc_service.tecs.session.query(xmlrpc_session);
    if(rs[0] == 0){
        var sessionData = [];

        for(var i=0; i<rs[1].length; i++){
           var oneSession = [];
           oneSession[0] = rs[1][i].user;
           oneSession[1] = getLocalTime(rs[1][i].login_time);
           sessionData[i] = oneSession;
        }

        sessionTable.fnAddData(sessionData);

        $("#session_last_refresh_time").html(GetLastUpdateTime());
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }
}

/**
*  授权信息
*/
function license_num(num){
    if(num == 0){
       return "unlimited";
    }

    return num;
}

function license_date(d){
    var a = new Date();
    a.setTime(0);

    if(d.getTime() == a.getTime()){
       return "unlimited";
    }

    return d;
}

function license_finger(finger){
    var finger_html = "<table>";

    for(var i=0; i<finger.length; i++){
        finger_html += "<tr><td>" + finger[i] + "</td></tr>"
    }

    finger_html += "</table>";

    return finger_html;
}

function license_color(a, b){
    if(b == 0){
        return "";
    }

    if(Number(a) == Number(b)){
        return " style='color:blue;'";
    }

    if(a > b){
        return " style='color:red;'";
    }

    return "";
}

var license_html = 
    '<div> \
        <ul class="nav nav-tabs" id="licenseNav"> \
            <li class="active"><a href="#licenseSummary" id="licenseSummaryNav"></a></li> \
            <li><a href="#licenseUpdate" id="licenseUpdateNav"></a></li> \
        </ul> \
        <div class="tab-content"> \
            <div class="tab-pane active" id="licenseSummary"></div> \
            <div class="tab-pane" id="licenseUpdate"> \
            <div class="row-fluid"> \
                <div class="span12 box"> \
                    <div class="form-horizontal"><fieldset> \
                        <div class="control-group"> \
                            <label class="control-label"><span id="license_user"></span></label> \
                            <div class="controls"><input type="text" id="license_user_val"  maxlength=32 /></div> \
                        </div> \
                        <div class="control-group"> \
                            <label class="control-label"><span id="license_pass"></span></label> \
                            <div class="controls"><input type="password" id="license_pass_val" maxlength=32 /></div> \
                        </div> \
                        <div class="control-group"> \
                            <label class="control-label"><span id="license_desc"></span></label> \
                            <div class="controls"><input type="text" id="license_desc_val" maxlength=1024 /></div> \
                        </div> \
                        <div class="control-group"> \
                            <label class="control-label"><span id="license_fingerprint"></span></label> \
                            <div class="controls"><input type="text" readonly id="license_fingerprint_val" /></div> \
                        </div> \
                        <div class="control-group"> \
                            <label class="control-label"></label> \
                            <div class="controls"><button class="btn"><i class="icon-download"></i><span id="license_download"></span></button></div> \
                        </div> \
                    </fieldset></div> \
                </div> \
            </div> \
           <div class="row-fluid"> \
                <div class="span12 box"> \
                    <form action="php/upload_license.php" method="post" enctype="multipart/form-data"> \
                    <div class="form-horizontal"><fieldset> \
                        <div class="control-group"> \
                            <label class="control-label"><span id="license_upload"></span></label> \
                            <div class="controls"><input type="file" name="license_fullname" id="license_fullname"> </div>  \
                        </div> \
                        <div class="control-group hide"> \
                            <label class="control-label"><span>license fingerprint</span></label> \
                            <div class="controls"><input type="text" readonly name="license_fingerprint_upload" id="license_fingerprint_upload"> </div>  \
                        </div> \
                        <div class="control-group"> \
                            <label class="control-label"></label> \
                            <div class="controls"><button class="btn" type="summit"><i class="icon-upload"></i><span id="upload"></span></button></div> \
                        </div> \
                    </fieldset></div> \
                    </form> \
                </div> \
            </div> \
            </div> \
         </div> \
     </div>';

function license_click(){
    active_mainmenu_item(5);

    $("#right_content").html(license_html);
    c_changeLangue(page);

    $('#licenseNav a:first').tab('show');
    $('#licenseNav a').click(function (e) {
        e.preventDefault();
        $(this).tab('show');
    });    

    $("#license_download").parent().unbind("click").bind("click", license_download);

    xmlrpc_service.tecs.system.license.hardware_fingerprint.query(xmlrpc_session, fingerprint_query_callback);

    var license = xmlrpc_service.tecs.system.license.query(xmlrpc_session);

    if(license[0] != 0){
        var license_alert = "";
        if(page.pagelan == "ch"){
            license_alert = "您使用的TECS系统授权不合法?";
        }
        else {
            license_alert = "Your TECS license is invalid.";
        }
        $("#licenseSummary").html(license_alert);
        return;
    }
    else {
        var license_info =  "<div style='margin-left:20px;margin-top:20px;'><table style='table-layout:fixed;word-break:break-all'>";
            license_info += "<tr><td class='td_r'><label id='name'></label></td><td>"             + license[1].customer_name + "</td></tr>";
            license_info += "<tr><td class='td_r'><label id='company'></label></td><td>"          + license[1].customer_company + "</td></tr>";
            license_info += "<tr><td class='td_r'><label id='address'></label></td><td>"          + license[1].customer_address + "</td></tr>";
            license_info += "<tr><td class='td_r'><label id='phone'></label></td><td>"            + license[1].customer_phone + "</td></tr>";
            license_info += "<tr><td class='td_r'><label id='email'></label></td><td>"            + license[1].customer_email + "</td></tr>";
            license_info += "<tr><td class='td_r'><label id='description'></label></td><td>"      + license[1].customer_description + "</td></tr>";
            license_info += "<tr><td class='td_r'><label id='days'></label></td><td>"             + license_num(license[1].remaining_days) + "/" + license_num(license[1].effective_days) + "</td></tr>";
            license_info += "<tr><td class='td_r'><label id='expire'></label></td><td>"           + license_date(license[1].expired_date) + "</td></tr>";
            license_info += "<tr" + license_color(license[1].current_created_vms, license[1].max_created_vms) +"><td class='td_r'><label id='max_created_vms'></label></td><td>"  + license[1].current_created_vms + "/" + license_num(license[1].max_created_vms) + "</td></tr>";
            license_info += "<tr" + license_color(license[1].current_deployed_vms, license[1].max_deployed_vms) +"><td class='td_r'><label id='max_deployed_vms'></label></td><td>" + license[1].current_deployed_vms + "/" + license_num(license[1].max_deployed_vms) + "</td></tr>";
            license_info += "<tr" + license_color(license[1].current_images, license[1].max_images) +"><td class='td_r'><label id='max_images'></label></td><td>"       + license[1].current_images + "/" + license_num(license[1].max_images) + "</td></tr>";
            license_info += "<tr" + license_color(license[1].current_users, license[1].max_users) +"><td class='td_r'><label id='max_users'></label></td><td>"        + license[1].current_users + "/" + license_num(license[1].max_users) + "</td></tr>";
            license_info += "<tr" + license_color(license[1].current_clusters, license[1].max_clusters) +"><td class='td_r'><label id='max_clusters'></label></td><td>"     + license[1].current_clusters + "/" + license_num(license[1].max_clusters) + "</td></tr>";
            //license_info += "<tr><td width='40%'><label id='max_hosts'></label></td><td>"        + license_num(license[1].max_hosts) + "</td></tr>";
            license_info += "<tr><td class='td_r'><label id='fingerprints'></label></td><td>"     + license_finger(license[1].hardware_fingerprints) + "</td></tr></table></div>";    

        $("#licenseSummary").html(license_info);        
    }

    c_changeLangue(page);
}

function fingerprint_query_callback(rs, ex){
    if(rs[0] == 0){
        $("#license_fingerprint_val").val(rs[1]);
        $("#license_fingerprint_upload").val(rs[1]);
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }
}

function license_download(){
    var user = $("#license_user_val").val();
    var pass = $("#license_pass_val").val();
    var desc = $("#license_desc_val").val();
    var fingerprint = $("#license_fingerprint_val").val();

    var down_url = "https://tecs_license.zte.com.cn/download_license.html";
        down_url += "?user=" + user;
        down_url += "&pass=" + pass;
        down_url += "&desc=" + desc;
        down_url += "&figerprint=" + fingerprint;

    window.open(down_url);
}

/**
 * 工作流
*/

var workflow_html =     
   '<div> \
        <ul class="nav nav-tabs" id="workflow"> \
            <li class="active"><a href="#currentWorkflow" id="currentWorkflowNav"></a></li> \
            <li><a href="#historyWorkflow" id="historyWorkflowNav"></a></li>\
        </ul> \
        <div class="tab-content">\
            <div class="tab-pane active" id="currentWorkflow"> \
                <div class="row-fluid"> \
                <div class="span8"> \
                            <div class="span4" style="table-layout:fixed;word-break:break-all">  \
                                <select id="selectCC" > </select> \
                            </div> \
                        </div> \
                    <div class="span4" align="right"> \
                    <button class="btn"><i class="icon-refresh"></i><span id="workflowRefresh"></span></button> \
                </div> \
                </div>\
                <div>\
                <table id="workflow_list" class="table table-striped table-bordered table-condensed"> \
                    <thead> <tr> \
                        <th align="left"></th> \
                        <th align="left" width="23%"><span id="workflow_t1"></span></th> \
                        <th align="left"><span id="workflow_t2"></span></th> \
                        <th align="left"><span id="workflow_t3"></span></th> \
                        <th align="left"><span id="workflow_t4"></span></th> \
                        <th align="left"><span id="workflow_t5"></span></th> \
                        <th align="left"><span id="workflow_t6"></span></th> \
                        <th align="left"><span id="workflow_t7"></span></th> \
                        <th align="left"><span id="workflow_t8"></span></th> \
                        <!--th align="left"><span id="workflow_t9"></span></th--> \
                        <th align="left"><span id="workflow_t10"></span></th> \
                    </tr> </thead> \
                    <tbody style="word-wrap:break-word;word-break:break-all;"> </tbody> \
                </table> \
                <!--div id="workflow_last_refresh_time"></div--> \
            </div>\
            </div>\
            <div class="tab-pane" id="historyWorkflow"> \
                    <div class="span8"> \
                        <div class="span4" style="table-layout:fixed;word-break:break-all">  \
                                <select id="selectCCHisWorkflow" > </select> \
                        </div> \
                    </div> \
                <div align="right"> \
                    <button class="btn"><i class="icon-refresh"></i><span id="historyWorkflowRefresh"></span></button> \
                </div> \
                <table id="history_workflow_list" class="table table-striped table-bordered table-condensed"> \
                    <thead> <tr> \
                        <th align="left"><span id="history_workflow_t0"></span></th> \
                        <th align="left"><span id="history_workflow_t1"></span></th> \
                        <th align="left"><span id="history_workflow_t2"></span></th> \
                        <th align="left"><span id="history_workflow_t3"></span></th> \
                        <th align="left"><span id="history_workflow_t4"></span></th> \
                        <th align="left"><span id="history_workflow_t5"></span></th> \
                        <th align="left"><span id="history_workflow_t6"></span></th> \
                        <th align="left"><span id="history_workflow_t7"></span></th> \
                        <!--th align="left"><span id="history_workflow_t8"></span></th--> \
                        <th align="left"><span id="history_workflow_t9"></span></th> \
                        <th align="left"><span id="history_workflow_t10"></span></th> \
                        <th align="left"><span id="history_workflow_t11"></span></th> \
                        <th align="left"><span id="history_workflow_t12"></span></th> \
                        <th align="left"><span id="history_workflow_t13"></span></th> \
                    </tr> </thead> \
                    <tbody style="word-wrap:break-word;word-break:break-all;"> </tbody> \
                </table> \
                <!--div id="workflow_last_refresh_time"></div--> \
            </div>\
            <div id="workflow_last_refresh_time"></div> \
        </div>\
    </div>';

function workflow_click(){
    active_mainmenu_item(1);

    $("#right_content").html(workflow_html);  
    c_changeLangue(page);

    $('#workflow a:first').tab('show');
    $('#workflow a').click(function (e) {
        e.preventDefault();
        $(this).tab('show');

        if($(this).attr("id") == "currentWorkflow"){
            SelectRefreshCCOrTC();
        }
        if($(this).attr("id") == "historyWorkflow"){
            InitSelectCCorTC($("#selectCCHisWorkflow"));
            RefreshHistoryWorkflowData();
        } 
    });

    InitSelectCCorTC($("#selectCC")); 
    $("#selectCC").unbind('change').bind('change',function(){
        SelectRefreshCCOrTC();
        });    
    InitSelectCCorTC($("#selectCCHisWorkflow"));
    $("#selectCCHisWorkflow").unbind('change').bind('change',function(){
        RefreshHistoryWorkflowData();
        });    
    workflowTable = $("#workflow_list").dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
		"aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] },
                         { "bSortable": false, "aTargets": [ 7 ] },
                         { "bSortable": false, "aTargets": [ 9 ] }],
        "alternatively" : "destory"
    });

	$('.modal').modal({
        backdrop:"static",
        keyboard:false,
        "show":false
    });

    historyWorkflowTable = $("#history_workflow_list").dataTable({
        "sPaginationType":"bootstrap",
        "oLanguage":tableLanguage[page.pagelan],
        "aoColumnDefs":[{"bSortable":false,"aTargets":[1]},
                        {"bSortable":false,"aTargets":[11]}],
        "bRetrieve" : true,
        "bDestroy" : true,
        "alternatively" : "destory",
        "bProcessing": false,
        "bServerSide": true,
        "sAjaxSource": "../php/history_workflow.php",
        "fnServerData": history_retrieve_data
    });
    
   $("#workflowRefresh").parent().click(SelectRefreshCCOrTC);
   $("#workflowRefresh").trigger("click");

    $("#historyWorkflowRefresh").parent().click(RefreshHistoryWorkflowData);   
    //$("#historyWorkflowRefresh").trigger("click");
    
    clear_all_timer();
    //curr_timer_id = setInterval(UpdataWorkflow, 30*1000);
}

function InitSelectCCorTC(sl, cb){
    sl.empty();
    sl.attr("data-placeholder", page.res[page.pagelan].type.field_name["select_CCorTC"]);
    
    var allCC;
    $.getJSON("../php/cluster.php",function(json){
        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["11", json[1]]);
            return;
        }

        allCC = json[1];
        sl.append("<option value='tecscloud'>tecscloud");
        for (var i=0;i<allCC.length;i++) {
            sl.append("<option value='" + allCC[i].name + "'>" + allCC[i].name);
        }
        sl.chosen();
    });

    if(cb != null){
            cb();
    }
}

function UpdataWorkflow(){
    RefreshWorkflowData();
    RefreshHistoryWorkflowData();
}
function set_node_data(table, data){
    var trlist = table.fnGetNodes();
    for (var i = 0; i < trlist.length; i++) {
        $(trlist[i]).data("item", data[i]);
    };
}

function SelectRefreshCCOrTC(){
    var ccName = $("#selectCC").val();
    if(ccName == null){
        RefreshWorkflowData();
    }
    else if( ccName.toString() == "tecscloud"){
        RefreshWorkflowData();
    }
    else{
        RefreshCCWorkflowData();
    }
}

function RefreshWorkflowData(){
    var more = [];
    $.getJSON( "../php/workflow.php",  function (json) {
        workflowTable.fnClearTable();        
     
        if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
            return;
        }

        var workflows = json.data.aaData;        

        for(var i = 0;i < workflows.length;i++){
		    workflows[i][0] = '<a href="#" onclick="workflow_action_info(this)"><span class="icon icon-green icon-add"></span></a>';
           
			workflows[i][8] = '<span>'+json.data.aaData[i][8]+'<span>%</span></span>';

            var morelink = '<a  href="#" onclick="workflow_more_click(this)"><span>more</span></a>';
            workflows[i][9] = morelink; 
        }		 
        
		workflowTable.fnAddData(workflows);
        set_node_data(workflowTable, json.data.aaData);	 
    });
    
    $("#workflow_last_refresh_time").html(GetLastUpdateTime());    
}

function workflow_retrieve_data( sSource, aoData, fnCallback ){
  var more = [];
  $.getJSON( sSource, aoData, function (json) {
        var workflows = json.data;
       
        if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
            return;
        }

        for(var i = 0;i < workflows.aaData.length;i++){
		    workflows.aaData[i][0] = '<a href="#" onclick="workflow_action_info(this)"><span class="icon icon-green icon-add"></span></a>';	  
			workflows.aaData[i][8] = '<span>' + json.data.aaData[i][8] + '<span>%</span></span>';
            var morelink = '<a  href="#" onclick="workflow_more_click(this)"><span>more</span></a>';
            workflows.aaData[i][9] = morelink; 
        }
		 
        fnCallback(workflows);
		
        set_node_data(workflowTable, json.data.aaData);	 
    });

    $("#workflow_last_refresh_time").html(GetLastUpdateTime());  
}


function workflow_action_info(a){
    var rowitem = $(a).parent().parent().data("item");
    var workflowid = rowitem[1];

    //获取actions数据
    var sSource = "../php/workflow_action.php";
	var param = [];
    param.push( {"name":"workflowid", "value":workflowid});
	
	$.getJSON(sSource, param, function (json) {
        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["11", json[1]]);
            return;
        }

        var workflowActions = [];

        workflow_action_html = 
            '<div><h5>'+page.res[page.pagelan].type.field_name["workflow.action"]+':</h5></div> \
                <table class="table table-bordered" style="width:90%;word-break:break-all" align="right" > \
                    <tr > \
                        <th >'+page.res[page.pagelan].type.field_name["workflow.actionid"]+'</th> \
                        <th>'+page.res[page.pagelan].type.field_name["workflow.category"]+'</th> \
						<th>'+page.res[page.pagelan].type.field_name["workflow.name"]+'</th> \
						<th>'+page.res[page.pagelan].type.field_name["workflow.progress"]+'</th> \
                    </tr>';
					
            for(var i = 0;i < json[1].length;i++){
                workflow_action_html +=	    
                    '<tr > \
                        <td >'+json[1][i][0]+'</td> \
                        <td>'+json[1][i][1]+'</td> \
						<td>'+json[1][i][2]+'</td> \
						<td>'+json[1][i][3]+'%</td> \
                    </tr>';
            }
        
        workflow_action_html += '</table> ';

        var tr = $(a).parent().parent();
	
        if(workflowTable.fnIsOpen(tr[0])){
            workflowTable.fnClose(tr[0]);
			$(a).children().removeClass();
			$(a).children().addClass("icon icon-green icon-add");
        }
        else{
            workflowTable.fnOpen(tr[0],workflow_action_html,"action_info");            
            $(a).children().removeClass();
            $(a).children().addClass("icon icon-green icon-remove");
        }
	
        $("OamStaticUI").each(function(){SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));});
    });
}


function workflow_more_click(a){
    var rowitem = $(a).parent().parent().data("item");

    var workflow_more_dlg_html = 
        '<table class="table table-condensed"> <tbody> \
            <tr> \
                <td width="30%"><span id="label_int64_1"></span></td> \
                <td><span class="OamUi" bindattr="text" bind="label_int64_1"></span></td> \
            </tr> <tr> \
                <td width="30%"><span id="label_int64_2"></span></td> \
                <td>'+rowitem.label_int64_2+'</td> \
            </tr> <tr> \
			    <td width="30%"><span id="label_int64_3"></span></td> \
                <td>'+rowitem.label_int64_3+'</td> \
            </tr> <tr> \
			    <td width="30%"><span id="label_int64_4"></span></td> \
                <td>'+rowitem.label_int64_4+'</td> \
            </tr> <tr> \
			    <td width="30%"><span id="label_int64_5"></span></td> \
                <td>'+rowitem.label_int64_5+'</td> \
            </tr> <tr> \
			    <td width="30%"><span id="label_int64_6"></span></td> \
                <td>'+rowitem.label_int64_6+'</td> \
            </tr> <tr> \
			    <td width="30%"><span id="label_int64_7"></span></td> \
                <td>'+rowitem.label_int64_7+'</td> \
            </tr> <tr> \
			    <td width="30%"><span id="label_int64_8"></span></td> \
                <td>'+rowitem.label_int64_8+'</td> \
            </tr> <tr> \
			    <td width="30%"><span id="label_int64_9"></span></td> \
                <td>'+rowitem.label_int64_9+'</td> \
            </tr> <tr> \
			    <td width="30%"><span id="label_string_1"></span></td> \
                <td>'+rowitem.label_string_1+'</td> \
            </tr> <tr> \
			    <td width="30%"><span id="label_string_2"></span></td> \
                <td>'+rowitem.label_string_2+'</td> \
            </tr> <tr> \
			    <td width="30%"><span id="label_string_3"></span></td> \
                <td>'+rowitem.label_string_3+'</td> \
            </tr> <tr> \
			    <td width="30%"><span id="label_string_4"></span></td> \
                <td>'+rowitem.label_string_4+'</td> \
            </tr> <tr> \
			    <td width="30%"><span id="label_string_5"></span></td> \
                <td>'+rowitem.label_string_5+'</td> \
            </tr> <tr> \
			    <td width="30%"><span id="label_string_6"></span></td> \
                <td>'+rowitem.label_string_6+'</td> \
            </tr> <tr> \
			    <td width="30%"><span id="label_string_7"></span></td> \
                <td>'+rowitem.label_string_7+'</td> \
            </tr> <tr> \
			    <td width="30%"><span id="label_string_8"></span></td> \
                <td>'+rowitem.label_string_8+'</td> \
            </tr> <tr> \
			    <td width="30%"><span id="label_string_9"></span></td> \
                <td>'+rowitem.label_string_9+'</td> \
            </tr> \
        </tbody> </table>'
		
    

    ShowDiaglog("workflow_more_dlg", workflow_more_dlg_html, {
	 show:function(){
            var h5 = $("#workflow_more_dlg").find('div.modal-header h5');
            h5.html( "ID:" + rowitem[1]); 

            SetModifyForm($("#workflow_more_dlg"), rowitem);
        },
        close: function(){
            $("#workflow_more_dlg").modal("hide");
        }
    });
}
function RefreshCCWorkflowData(){
    var more = [];
    var aoData =[];
    var selectedCC = $("#selectCC").val();
    aoData.push( {"name":"clusterName", "value":selectedCC});
    $.getJSON( "../php/CC_workflow.php",aoData,  function (json) {
        workflowTable.fnClearTable();        
     
        if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
            return;
        }

        var workflows = json.data.aaData;        

        for(var i = 0;i < workflows.length;i++){
		    workflows[i][0] = '<a href="#" onclick="cc_workflow_action_info(this)"><span class="icon icon-green icon-add"></span></a>';

			workflows[i][8] = '<span>'+json.data.aaData[i][8]+'<span>%</span></span>';

            var morelink = '<a  href="#" onclick="workflow_more_click(this)"><span>more</span></a>';
            workflows[i][9] = morelink; 
        }		 
        
		workflowTable.fnAddData(workflows);
        set_node_data(workflowTable, json.data.aaData);	 
    });
    
    $("#last_refresh_time").html(GetLastUpdateTime());    
}
function cc_workflow_action_info(a){
    var rowitem = $(a).parent().parent().data("item");
    var workflowid = rowitem[1];
    var selectedCC = $("#selectCC").val();
    //获取actions数据

    var sSource = "../php/CC_workflow_action.php";
	var param = [];
    param.push( {"name":"clusterName", "value":selectedCC});
    param.push( {"name":"workflowid", "value":workflowid});
	
	$.getJSON(sSource, param, function (json) {
        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["11", json[1]]);
            return;
        }

        var workflowActions = [];

        workflow_action_html = 
            '<div><h5>'+page.res[page.pagelan].type.field_name["workflow.action"]+':</h5></div> \
                <table class="table table-bordered" style="width:90%;word-break:break-all" align="right" > \
                    <tr > \
                        <th >'+page.res[page.pagelan].type.field_name["workflow.actionid"]+'</th> \
                        <th>'+page.res[page.pagelan].type.field_name["workflow.category"]+'</th> \
						<th>'+page.res[page.pagelan].type.field_name["workflow.name"]+'</th> \
						<th>'+page.res[page.pagelan].type.field_name["workflow.progress"]+'</th> \
                    </tr>';
					
            for(var i = 0;i < json[1].length;i++){
                workflow_action_html +=	    
                    '<tr > \
                        <td >'+json[1][i][0]+'</td> \
                        <td>'+json[1][i][1]+'</td> \
						<td>'+json[1][i][2]+'</td> \
						<td>'+json[1][i][3]+'%</td> \
                    </tr>';
            }
        
        workflow_action_html += '</table> ';

        var tr = $(a).parent().parent();
	
        if(workflowTable.fnIsOpen(tr[0])){
            workflowTable.fnClose(tr[0]);
			$(a).children().removeClass();
			$(a).children().addClass("icon icon-green icon-add");
        }
        else{
            workflowTable.fnOpen(tr[0],workflow_action_html,"action_info");            
            $(a).children().removeClass();
            $(a).children().addClass("icon icon-green icon-remove");
        }
	
        $("OamStaticUI").each(function(){SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));});
    });
}

function RefreshHistoryWorkflowData(){
    GotoCurrentPageOfDataTable(historyWorkflowTable);
}  
function history_retrieve_data(sSource, aoData, fnCallback){
    var ccName = $("#selectCCHisWorkflow").val();
    if(ccName==null){
        ccName = "tecscloud";
    }
    aoData.push( {"name":"clusterName", "value":ccName});
 $.getJSON( sSource, aoData, function (json) {
        //historyWorkflowTable.fnClearTable();

        if(json.errcode != ""){ 
            ShowNotyQueryDBErrorInfo(json);
        }

        var historyWorkflows = json.data;

        for(var i = 0;i < historyWorkflows.length;i++){
           historyWorkflows[i][9] = getLocalTime(json.data.aaData[i][9]); 
           historyWorkflows[i][10] = getLocalTime(json.data.aaData[i][10]); 
        }		         
		//historyWorkflowTable.fnAddData(historyWorkflows);	 
        fnCallback(json.data);
    });
    
    $("#workflow_last_refresh_time").html(GetLastUpdateTime());    

}    

     
