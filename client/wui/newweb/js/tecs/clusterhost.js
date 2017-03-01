/**
*  page begin
**/
var page = {
    pageid: "clusterhost",
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

var allcluster;

$(function() {
    page.init();    	
    var tc_html = '<li><a href="#" onclick="tc_click()" style="word-wrap:break-word;word-break:break-all"> \
                        <span><img src="img/tc.png"></img></span><span>TecsCloud</span> \
                   </a></li>';
    $("#cluster_list").append(tc_html);
    
    $.getJSON("../php/cluster.php",function(json){
        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["11", json[1]]);
            return;
        }

        allcluster = json[1];

        for(var i=0; i<json[1].length; i++){
            var cc_html =  '<li><a href="#" onclick="cc_click(this)" style="word-wrap:break-word;word-break:break-all">';
                cc_html += '<span><img src="img/server16.png"></img></span><span>';
                cc_html += json[1][i].name;
                cc_html += '</span></a></li>';
            $("#cluster_list").append(cc_html);
        }

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

/**
* All cluster overview in table
*/
var cclist_html =  
   '<div style="display:none;"><ul class="breadcrumb"></ul></div> \
    <ul class="nav nav-tabs" id="cclistTab"><li class="active"><a href="#cclistSummary" id="cclistSummaryNav"></a></li></ul> \
    <div class="tab-content"> <div class="tab-pane active" id="cclistSummary">\
        <div align="right"> \
            <button class="btn"><i class="icon-wrench"></i><span id="ccDisable"></span></button> \
            <button class="btn"><i class="icon-play"></i><span id="ccEnable"></span></button> \
            <button class="btn"><i class="icon-trash"></i><span id="ccForget"></span></button> \
            <button class="btn"><i class="icon-refresh"></i><span id="ccRefresh"></span></button> \
        </div> \
        <table id="cc_list" class="table table-striped table-bordered table-condensed"> \
            <thead> <tr> \
                <th align="left" width="25px;"><INPUT id="ccSelecltAll" type="checkbox" /></th> \
                <th align="left" width="100px;"><span id="cc_t1"></span></th> \
                <th align="left" width="100px;"><span id="cc_t2"></span></th> \
                <th align="left" width="50px;"><span id="cc_t3"></span></th> \
                <th align="left" width="75px;"><span id="cc_t4"></span></th> \
                <th align="left" width="200px;"><span id="cc_t5"></span></th> \
                <th align="left" width="100px;"><span id="cc_t6"></span></th> \
                <th align="left" width="25px;"></th> \
            </tr> </thead> \
            <tbody style="word-wrap:break-word;word-break:break-all;"> \
            </tbody> \
        </table> \
        <div id="cclist_last_refresh_time"></div> \
    </div></div>';

function tc_click() {
    $('ul.main-menu li').removeClass("active");
    $('ul.main-menu li').eq(0).addClass("active");

    $("#right_content").html(cclist_html);  
    c_changeLangue(page);  
   
    ccTable = $("#cc_list").dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] }, 
                         { "bSortable": false, "aTargets": [ 3 ] },
                         { "bSortable": false, "aTargets": [ 5 ] },
                         { "bSortable": false, "aTargets": [ 7 ] } ],
        "alternatively" : "destory"
    });  

    $("#ccSelecltAll").click(cc_select_all_click);
    $("#ccRefresh").parent().click(refresh_ccinfo);
    $("#ccDisable").parent().click(disable_cc);
    $("#ccEnable").parent().click(enable_cc);
    $("#ccForget").parent().click(forget_cc);

    refresh_cctable();

    clear_all_timer();
    curr_timer_id = setInterval(refresh_ccinfo, 30*1000);
}

function refresh_ccinfo() {
    $.getJSON("../php/cluster.php",function(json){
        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["11", json[1]]);
            return;
        }

        allcluster = json[1];

        refresh_cctable();
    });
}

function disable_cc() {
    ClearAlertInfo();    
    var pageVal = page.res[page.pagelan]["xid"]["val"];
    var vCheckedbox = $(ccTable.fnGetNodes()).find(":checkbox:checked");

    var vresultSucceeded = [];
    var vresultFailed    = [];

    if(vCheckedbox.size() > 0){
        ShowDiaglog("disable_cc_dlg", pageVal["disable_cc_confirm"], {
            show:function(){
                var h5 = $("#disable_cc_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-wrench"></i>' + h5.text());    
            },
            ok:function(){
                $("#disable_cc_dlg").modal('hide');    

                vCheckedbox.each(function(){
                    var tr = $(this).parent().parent();      

                    var ccname = tr.find("td").eq(1).text();

                    var rs = xmlrpc_service.tecs.cluster.disable(xmlrpc_session, ccname);

                    ShowSynRpcInfo(rs);
                });    

                refresh_ccinfo();
            }
        });
    }
    else{
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]);
    }
}

function enable_cc() {
    ClearAlertInfo();     
    var vCheckedbox = $(ccTable.fnGetNodes()).find(":checkbox:checked");

    var vresultSucceeded = [];
    var vresultFailed    = [];

    if(vCheckedbox.size() > 0){
        /*ShowDiaglog("disable_cc_dlg", "disable_cc_confirm", {
            ok:function(){
                $("#disable_cc_dlg").modal('hide'); */

                vCheckedbox.each(function(){
                    var tr = $(this).parent().parent();      

                    var ccname = tr.find("td").eq(1).text();

                    var rs = xmlrpc_service.tecs.cluster.enable(xmlrpc_session, ccname);

                    ShowSynRpcInfo(rs);
                });    

                refresh_ccinfo();
            /*}
        });*/
    }
    else{
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]);
    }
}

function forget_cc() {
    ClearAlertInfo();
    var pageVal = page.res[page.pagelan]["xid"]["val"];    
    var vCheckedbox = $(ccTable.fnGetNodes()).find(":checkbox:checked");

    var vresultSucceeded = [];
    var vresultFailed    = [];

    if(vCheckedbox.size() > 0){
        ShowDiaglog("forget_cc_dlg", pageVal["forget_cc_confirm"], {
            show:function(){
                var h5 = $("#forget_cc_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-trash"></i>' + h5.text());                    
            },
            ok:function(){
                $("#forget_cc_dlg").modal('hide');    

                vCheckedbox.each(function(){
                    var tr = $(this).parent().parent();      

                    var ccname = tr.find("td").eq(1).text();

                    var rs = xmlrpc_service.tecs.cluster.forget(xmlrpc_session, ccname);

                    ShowSynRpcInfo(rs);
                });    

                //refresh_ccinfo();
                $('ul.mainnav li.active a').eq(0).click();
            }
        });
    }
    else{
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]);
    }
}

function cc_state(cc_info) {
    var cc_state = "";
    var map_info = page.res[page.pagelan]["type"]["ClusterRunState"];

    if(cc_info.online == 0) {
        cc_state = '<img src="img/hc_offline.png" data-rel="tooltip" data-original-title="' 
                   + map_info[0] +'"/>'           
    }
    else {
        cc_state = '<img src="img/hc_ok.png" data-rel="tooltip" data-original-title="'
                   + map_info[1] +'"/>'
    }

    if(cc_info.enabled == 0){
        cc_state += '<img src="img/hc_mantaince.png" data-rel="tooltip" data-original-title="' 
                    + map_info[2] +'"/>'
    }   

    return cc_state;     
}

var currCcState = new Object();

function refresh_cctable() {
    RecordSelItem(ccTable, "cc_list", currCcState);    
    ccTable.fnClearTable();

    var cc_data = [];

    for (var i = 0; i < allcluster.length; i++) {
        var one_cc = [];

        one_cc[0] = GetItemCheckbox(allcluster[i]["name"], currCcState); //"<INPUT type='checkbox' />";
        one_cc[1] = "<a href='#' onclick='goto_cluster(this)'>" + allcluster[i]["name"] + "</a>";
        one_cc[2] = allcluster[i]["ip_address"];
        one_cc[3] = cc_state(allcluster[i]);
        one_cc[4] = "<a href='#' onclick='goto_cluster_hc(this)'>" + allcluster[i]["host_count"] + "</a>";   
        one_cc[5] = allcluster[i]["desc"].length < 32 ? allcluster[i]["desc"] : (allcluster[i]["desc"].substring(0, 32) + "...");
        var rt = new Date(Number(allcluster[i]["register_time"])*1000);
        one_cc[6] = getLocalTime(rt.toString()); 
        one_cc[7] = "<a href='javascript:void(0);' onclick='modify_cclist_click(this)' data-rel='tooltip' data-original-title='"
                    + page.res[page.pagelan]["type"]["opr"]["modify"] + "'><i class='icon-edit'></i></a>";

        cc_data[i] = one_cc;
    };

    ccTable.fnAddData(cc_data);

    ClearCurSelItem(currCcState);

    $('.tooltip').hide();
    $('[data-rel="tooltip"]').tooltip({"placement":"bottom"});

    $("#cclist_last_refresh_time").html(GetLastUpdateTime());    
}

function goto_cluster(a){
    var ccname = $(a).text();
    $("#cluster_list").find("a").each(function(){
        if($(this).text() == ccname){
            $(this).trigger("click");
            return;
        }
    });
}

function goto_cluster_hc(a){
    var ccname = $(a).parent().parent().find("td").eq(1).text();
    $("#cluster_list").find("a").each(function(){
        if($(this).text() == ccname){
            $(this).trigger("click");
            $("#ccHostNav").trigger("click");
            return;
        }
    });    
}

function modify_cclist_click(cc){
    //$(".tooltip").hide();
    curr_cluster = $(cc).parent().parent().find("td").eq(1).text();
    modify_cc_desc_click();    
}

function cc_select_all_click() {
    if($(this).attr("checked") == "checked") {
        $("#cc_list tbody input[type=checkbox]").attr("checked", true);
    }
    else {
        $("#cc_list tbody input[type=checkbox]").attr("checked", false);
    }    
}

/**
*   One cc detail information.
*/
var ccdetail_html = 
   '<div style="display:none;"><ul class="breadcrumb"></ul></div> \
    <div> \
        <ul class="nav nav-tabs" id="ccInfoTab"> \
            <li class="active"><a href="#ccSummary" id="ccSummaryNav"></a></li> \
            <li><a href="#ccCmm" id="ccCmmNav"></a></li> \
            <li><a href="#ccHost" id="ccHostNav"></a></li> \
            <li><a href="#ccVm" id="ccVmNav"></a></li> \
            <li><a href="#ccTcu" id="ccTcuNav"></a></li> \
            <li><a href="#ccSa" id="ccSaNav"></a></li> \
            <li><a href="#ccStrategy" id="ccStrategyNav"></a></li> \
        </ul> \
        <div class="tab-content"> \
            <div class="tab-pane active" id="ccSummary"> \
                <div class="row-fluid"> \
                    <div class="box span3"> \
                        <div class="box-header well"><h3 id="tcuHeader"></h3></div> \
                        <div class="box-content"><div id="tcuPie" style="height:150px;"></div> \
                        <div style="margin-left:35px;"></div></div> \
                    </div> \
                    <div class="box span3"> \
                        <div class="box-header well"><h3 id="memHeader"></h3></div> \
                        <div class="box-content"><div id="memPie" style="height:150px;"></div> \
                        <div style="margin-left:35px;"></div></div> \
                    </div> \
                    <div class="box span3"> \
                        <div class="box-header well"><h3 id="diskHeader"></h3></div>  \
                        <div class="box-content"><div id="diskPie" style="height:150px;"></div> \
                        <div style="margin-left:35px;"></div></div> \
                    </div> \
                    <div class="box span3"> \
                        <div class="box-header well"><h3 id="shareDiskHeader"></h3></div> \
                        <div class="box-content"><div id="shareDiskPie" style="height:150px;"></div> \
                        <div style="margin-left:35px;"></div></div> \
                    </div> \
                </div> \
                <div class="row-fluid"> \
                    <div class="box span4" style="height:175px"> \
                        <div class="box-header well"><h3><span id="cc_state"></span><span>:</span></h3></div> \
                        <div class="box-content"> \
                            <div style="margin-left:20px;"> \
                                <div><strong><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Cluster.enabled"></OamStaticUI></strong>:</div> \
                                <div style="margin-left:20px;"><input data-no-uniform="true" type="checkbox" class="iphone-toggle" id="cc_enable_toggle"></div> \
                                <div><strong><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Cluster.online"></OamStaticUI></strong>:</div> \
                                <div style="margin-left:20px;"><span class="OamUi ccdata" bindattr="text" bind="online" maptype="ClusterRunState"></span></div> \
                            </div> \
                        </div> \
                    </div> \
                    <div class="box span4" style="height:175px"> \
                        <div class="box-header well"><h3><span id="cc_ip">IP</span><span>:</span></h3></div> \
                        <div class="box-content"> \
                            <div style="margin-left:20px;"> \
                                <span class="OamUi ccdata" bindattr="text" bind="ip_address"></span> \
                            </div> \
                        </div> \
                    </div> \
                    <div class="box span4" style="height:175px"> \
                        <div class="box-header well"><h3><span id="cc_desc"></span><span>:</span></h3></div>\
                        <div class="box-content"> \
                            <div style="margin-left:20px;word-wrap:break-word;word-break:break-all;"> \
                                <span class="OamUi ccdata" bindattr="text" bind="desc"></span> \
                                (<a id="modify_cc_desc" href="#" onclick="modify_cc_desc_click()"></a>) \
                            </div> \
                        </div> \
                    </div> \
                </div> \
            </div> \
            <div class="tab-pane" id="ccCmm"> \
                <div align="right"> \
                    <button class="btn"><i class="icon-plus"></i><span id="cmmAdd"></span></button> \
                    <button class="btn"><i class="icon-trash"></i><span id="cmmRemove"></span></button> \
                    <button class="btn"><i class="icon-refresh"></i><span id="cmmRefresh"></span></button> \
                </div> \
                <table id="cmm_list" class="table table-striped table-bordered table-condensed"> \
                    <thead> <tr> \
                            <th width="25px;" align="left"><INPUT id="cmmSelecltAll" type="checkbox" /></th> \
                            <th width="50px;" align="left"><span id="cmm_t1"></span></th> \
                            <th width="50px;" align="left"><span id="cmm_t2"></span></th> \
                            <th width="50px;" align="left"><span id="cmm_t3"></span></th> \
                            <th width="75px;" align="left"><span id="cmm_t4"></span></th> \
                            <th width="125px;" align="left"><span id="cmm_t5"></span></th> \
                            <th width="175px;" align="left"><span id="cmm_t6"></span></th> \
                            <th width="25px;" align="left"></th> \
                    </tr></thead> \
                    <tbody style="word-wrap:break-word;word-break:break-all;"></tbody> \
                </table> \
            </div> \
            <div class="tab-pane" id="ccHost"> \
                <div align="right"> \
                    <button class="btn"><i class="icon-wrench"></i><span id="hostDisable"></button> \
                    <button class="btn"><i class="icon-play"></i><span id="hostEnable"></button> \
                    <button class="btn"><i class="icon-repeat"></i><span id="hostReboot"></button> \
                    <button class="btn"><i class="icon-off"></i><span id="hostShutdown"></button> \
                    <button class="btn"><i class="icon-trash"></i><span id="hostForget"></button> \
                    <button class="btn"><i class="icon-refresh"></i><span id="hostRefresh"></button> \
                </div> \
                <table id="host_list" class="table table-striped table-bordered table-condensed"> \
                    <thead><tr> \
                        <th aligh="left" width="25px;"><INPUT id="hostSelectAll" type="checkbox" /></th> \
                        <th aligh="left" width="100px;"><span id="host_t1"></span></th> \
                        <th aligh="left" width="75px;"><span id="host_t2"></span></th> \
                        <th aligh="left" width="50px;"><span id="host_t3"></span></th> \
                        <th aligh="left" width="75px;"><span id="host_t4"></span></th> \
                        <th aligh="left" width="175px;"><span id="host_t5"></span></th> \
                        <th aligh="left" width="75px;"><span id="host_t6"></span></th> \
                        <th aligh="left" width="25px;"></th> \
                    </tr></thead> \
                    <tbody style="word-wrap:break-word;word-break:break-all;"></tbody> \
                </table> \
            </div> \
            <div class="tab-pane" id="ccVm"> \
                <!--div align="right"> \
                    <button class="btn"><i class="icon-refresh"></i><span id="vmRefresh"></span></button> \
                </div> \
                <table id="vm_list" class="table table-striped table-bordered table-condensed"> \
                    <thead> \
                        <tr> \
                            <th width="75px;" align="left"><span id="vm_t1"></span></th> \
                            <th width="75px;" align="left"><span id="vm_t2"></span></th> \
                            <th width="100px;" align="left"><span id="vm_t3"></span></th> \
                            <th width="75px;" align="left"><span id="vm_t4"></span></th> \
                            <th width="75px;" align="left"><span id="vm_t5"></span></th> \
                            <th width="75px;" align="left"><span id="vm_t6"></span></th> \
                        </tr> \
                    </thead> \
                    <tbody style="word-wrap:break-word;word-break:break-all;"></tbody> \
                </table--> \
                <div class="row-fluid"> \
                    <div class="span8"> \
                        <div class="row-fluid" style="table-layout:fixed;word-break:break-all">  \
                            <div class="span4">  \
                                <select data-placeholder="" id="selectHost" multiple data-rel="chosen"> </select> \
                            </div> \
                        </div> \
                    </div> \
                    <div class="span4" align="right"> \
                        <button class="btn" id="cancel_deploy"><i class="icon-step-backward"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="cancel_deploy"/></span></button> \
                        <button class="btn" id="remove"><i class="icon-trash"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="remove"/></span></button> \
                        <button class="btn" id="refresh"><i class="icon-refresh"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="refresh"/></span></button> \
                    </div> \
                </div> \
                <div> \
                    <table class="table table-striped table-bordered table-condensed" id="vmInfoTb"> \
                        <thead>  \
                              <tr> \
                                 <th align="left" width="25px;"><INPUT id="SelecltAll" type="checkbox" value="" /></th> \
                                 <th align="left" width="50px;">ID</th>  \
                                 <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="name"/></th> \
                                 <th align="left" width="50px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="status"/></th> \
                                 <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="vcpu"/></th> \
                                 <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="tcu"/></th> \
                                 <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="memory_size"/></th> \
                                 <th align="left" width="75px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="disk_size"/></th> \
                                 <th align="left" width="100px;">IP</th> \
                                 <th align="left" width="50px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="user"/></th> \
                                 <th align="left" width="50px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="project"/></th> \
                                 <th align="left" width="50px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="host"/></th> \
                                 <th align="left" width="50px;"></th> \
                              </tr> \
                        </thead> \
                        <tbody style="word-wrap:break-word;word-break:break-all;"></tbody> \
                    </table> \
                </div> \
            </div> \
            <div class="tab-pane" id="ccTcu"> \
                <div align="right"> \
                    <button class="btn"><i class="icon-refresh"></i><span id="tcuRefresh"></span></button> \
                </div> \
                <table id="tcu_list" class="table table-striped table-bordered table-condensed"> \
                    <thead> \
                        <tr> \
                            <th width="75px;" align="left"><span id="tcu_t1"></span></th> \
                            <th width="200px;" align="left"><span id="tcu_t2"></span></th> \
                            <th width="75px;" align="left"><span id="tcu_t3"></span></th> \
                            <th width="100px;" align="left"><span id="tcu_t4"></span></th> \
                            <th width="75px;" align="left"><span id="tcu_t5"></span></th> \
                            <th width="200px;" align="left"><span id="tcu_t6"></span></th> \
                            <th width="25px;" align="left"></th> \
                        </tr> \
                    </thead> \
                    <tbody style="word-wrap:break-word;word-break:break-all;"></tbody> \
                </table> \
            </div> \
            <div class="tab-pane" id="ccSa"> \
                <div align="right"> \
                    <button class="btn"><i class="icon-plus"></i><span id="saAdd"></span></button> \
                    <button class="btn"><i class="icon-trash"></i><span id="saRemove"></span></button> \
                    <button class="btn"><i class="icon-refresh"></i><span id="saRefresh"></span></button> \
                </div> \
                <table id="sa_list" class="table table-striped table-bordered table-condensed">  \
                    <thead> \
                        <tr> \
                            <th width="25px;" align="left"><INPUT id="saSelectAll" type="checkbox" /></th> \
                            <th width="50px;" align="left"><span id="sa_t1"></span></th> \
                            <th width="150px;" align="left"><span id="sa_t2"></span></th> \
                            <!--th width="100px;" align="left"><span id="sa_t3"></span></th--> \
                            <th width="50px;" align="left"><span id="sa_t4"></span></th> \
                            <th width="75px;" align="left"><span id="sa_t5"></span></th> \
                            <th width="100px;" align="left"><span id="sa_t6"></span></th> \
                            <!--th width="" align="left"><span id="sa_t7"></span></th--> \
                            <th width="150px;" align="left"><span id="sa_t8"></span></th> \
                        </tr> \
                    </thead> \
                    <tbody style="word-wrap:break-word;word-break:break-all;"></tbody> \
                </table> \
            </div> \
            <div class="tab-pane" id="ccStrategy"> \
                <div class="box box-content" > \
			        <form class="form-horizontal"> <fieldset>\
                    <div class="control-group">  \
                        <label class="control-label"><span id="is_save_energy"></span><span></span></label> \
                        <div class="controls" > \
                            <input data-no-uniform="true" type="checkbox" class="iphone-toggle" id="energy_toggle"> \
                        </div> \
                    </div>  \
                    <div class="control-group max">   \
                        <label class="control-label"><span id="max_host_num"></span><span></span></label> \
                        <div class="controls"><input id="max_input" type="text" class="inputrge OamUi" bindattr="val" bind="max"></input>\
					    </div> \
                    </div> \
                    <div class="control-group min">   \
                        <label class="control-label"><span id="min_host_num"></span><span></span></label> \
                        <div class="controls"><input id="min_input" type="text" class="inputrge OamUi"  bindattr="val" bind="min"></input></div> \
                    </div> \
                    <div class="control-group percent">    \
                        <label class="control-label"><span id="host_percent"></span><span></span></label> \
                        <div class="controls"><input id="host_percent_input" type="text" class="inputrge OamUi" bindattr="val" bind="percent"></input><span >%</span></div> \
                    </div> \
                    <div class="control-group interval">    \
                        <label class="control-label"><span id="check_interval"></span><span></span></label> \
                        <div class="controls"><input id="host_check_interval" popover="5~43200"  type="text" class="inputrge OamUi" bindattr="val" bind="interval"></input></div> \
                    </div> \
                    <div class="control-group">    \
                        <label class="control-label"></label> \
                        <div class="controls"> \
                           <a class="btn" id="strategy_config"><i class="icon-ok"></i><span id="modify_strategy_config"></span></a>\
                        </div> \
                    </div> \
			        </fieldset> </form>\
                </div> \
                <div class="box box-content" > \
                    <form class="form-horizontal"> <fieldset>\
                    <div class="control-group">  \
                        <label class="control-label"><span id="schedule_policy"></span><span></span></label> \
                        <div class="controls" > <select id="slSchedulePolicy"></select> </div> \
                    </div>  \
                    <div class="control-group">   \
                        <label class="control-label"><span id="cycle_time"></span><span></span></label> \
                        <div class="controls"><input id="inputCycleTime" popover="5~1500" type="text" maxlength="7"></input></div> \
                    </div> \
                    <div class="control-group">   \
                        <label class="control-label"><span id="influence_vms"></span><span></span></label> \
                        <div class="controls"><input id="inputInfluenceVMs" type="text" maxlength="7"></input></div> \
                    </div> \
                    <div class="control-group">    \
                        <label class="control-label"></label> \
                        <div class="controls"> \
                           <a class="btn" id="schedulepolicy_config"><i class="icon-ok"></i><span id="modify_schedulepolicy_config"></span></a>\
                        </div> \
                    </div> \
                    </fieldset> </form>\
                </div> \
                <div class="box box-content" > \
                    <form class="form-horizontal"> <fieldset>\
                    <div class="control-group">  \
                        <label class="control-label"><span id="HostDown"></span><span></span></label> \
                        <div class="controls" > <select id="slHostDown"></select> </div> \
                    </div>  \
                    <div class="control-group">   \
                        <label class="control-label"><span id="HostFault"></span><span></span></label> \
                        <div class="controls"> <select id="slHostFault"></select> </div> \
                    </div> \
                    <div class="control-group">   \
                        <label class="control-label"><span id="HostStorageFault"></span><span></span></label> \
                        <div class="controls"> <select id="slHostStorageFault"></select> </div> \
                    </div> \
                    <div class="control-group">    \
                        <label class="control-label"></label> \
                        <div class="controls"> \
                           <a class="btn" id="hapolicy_config"><i class="icon-ok"></i><span id="modify_hapolicy_config"></span></a>\
                        </div> \
                    </div> \
                    </fieldset> </form>\
                </div> \
            </div> \
            <div id="last_refresh_time"></div><br /> \
        </div> \
    </div>';

function cc_click(a) {
    $('ul.main-menu li').removeClass("active");
    $(a).parent().addClass("active");
 
    $("#right_content").html(ccdetail_html); 
    c_changeLangue(page);  

    $('#ccInfoTab a:first').tab('show');
    $('#ccInfoTab a').click(function (e) {
        e.preventDefault();
        $(this).tab('show');

        clear_all_timer();

        if($(this).attr("id") == "ccSummaryNav"){
            refresh_summary();
            curr_timer_id = setInterval(refresh_summary, 30*1000);
        }

        if($(this).attr("id") == "ccCmmNav"){
            refresh_cmm();  
        }

        if($(this).attr("id") == "ccHostNav"){
            curr_timer_id = setInterval(refresh_host_table, 30*1000);
        }

        if($(this).attr("id") == "ccVmNav"){
            refresh_vm();
            curr_timer_id = setInterval(refresh_vm, 30*1000);
        }

        if($(this).attr("id") == "ccTcuNav"){
            refresh_tcu();  
        }

        if($(this).attr("id") == "ccSaNav"){
            refresh_sa();
            curr_timer_id = setInterval(refresh_sa, 30*1000);
        }  

        if($(this).attr("id") == "ccStrategyNav"){
            refresh_energy();  
            refresh_schedulepolicy();  
            refresh_hapolicy();  
        }                   	
        $("#last_refresh_time").html(GetLastUpdateTime());                               
    });

    curr_cluster = $(a).text();
 
    init_summary();
    init_cmm();
    init_host();
    init_tcu();
    init_vm();
    init_sa();
    
    TogglePopover($("#ccStrategy"));

    clear_all_timer();
    curr_timer_id = setInterval(refresh_summary, 30*1000);    
}

$(window).unbind("resize").resize(function(){
    if($("#tcuPie").is(":visible")){
       init_summary();
    }
});

//------------ summary ---------------------
function get_cc_obj(cluster_name) {
    for (var i = 0; i < allcluster.length; i++) {
        if(allcluster[i].name == cluster_name){
            return allcluster[i];
        }
    };

    return null;      
}

function init_summary(){
    var cluster_info = get_cc_obj(curr_cluster);
    
    if(cluster_info == null){
        ShowNotyRpcErrorInfo(["1", "no such cluster"]);
        return;
    }

    var mapinfo = page.res[page.pagelan]["type"]["usage"];    

    cluster_info.tcu_used_total = 
        Number(cluster_info.tcu_max_total) - Number(cluster_info.tcu_free_total);
    var tcuData = [ {label:mapinfo["used"], data:parseInt(cluster_info.tcu_used_total)},
                    {label:mapinfo["free"], data:parseInt(cluster_info.tcu_free_total)}];
    
    var extra = {label:mapinfo["free"], data:mapinfo["max"] + ' : ' + parseInt(cluster_info.tcu_free_max)};
    init_piechart("tcuPie", tcuData, extra);

    cluster_info.mem_used_total = 
        Number(cluster_info.mem_max_total) - Number(cluster_info.mem_free_total);
    var memData = [{label:mapinfo["used"], data:parseInt(cluster_info.mem_used_total)}, 
                   {label:mapinfo["free"], data:parseInt(cluster_info.mem_free_total)}];

    var extra = {label:mapinfo["free"], data: mapinfo["max"] + ' : ' + Capacity(parseInt(cluster_info.mem_free_max))};                   
    init_piechart("memPie", memData, extra);

    cluster_info.disk_used_total = 
        Number(cluster_info.disk_max_total) - Number(cluster_info.disk_free_total);
    var diskData = [{label:mapinfo["used"], data:parseInt(cluster_info.disk_used_total)}, 
                    {label:mapinfo["free"], data:parseInt(cluster_info.disk_free_total)}];

    var extra = {label:mapinfo["free"], data: mapinfo["max"] + ' : ' + Capacity(parseInt(cluster_info.disk_free_max))};
    init_piechart("diskPie", diskData, extra);   

    cluster_info.share_disk_used_total = 
        Number(cluster_info.share_disk_max_total) - Number(cluster_info.share_disk_free_total);
    var diskData = [{label:mapinfo["used"], data:parseInt(cluster_info.share_disk_used_total)}, 
                    {label:mapinfo["free"], data:parseInt(cluster_info.share_disk_free_total)}];
    init_piechart("shareDiskPie", diskData);

    $("#ccSummary").find("OamStaticUI").each(function(){
        SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
    });            

    if(cluster_info["online"] == 0) {
        cluster_info["run_state"] = cluster_info["online"];
    }
    else {
        if(cluster_info["enabled"] == 0){
           cluster_info["run_state"]  = 2;
           $("#cc_enable_toggle").attr("checked", true);
        }
        else {
           cluster_info["run_state"]  = 1;
           $("#cc_enable_toggle").attr("checked", false);           
        }        
    }
    SetModifyForm($("#ccSummary"), cluster_info);

    $("#cc_enable_toggle").iphoneStyle({onChange:operate_cc});     
}

function init_piechart(id, data, extra) {
    $.plot($("#"+id), data, {
        series: { pie: { show: true } },    
        legend: { 
            container: $("#"+id).next(),
            labelFormatter: function(label, series){
                var formatLabel;

                if(isNaN(series.percent)){
                    series.percent = 0;
                }

                if(id == "tcuPie"){
                    formatLabel = label + ' : ' + series.data[0][1] + 
                                  ' (' + Math.floor(series.percent) + '%)';  
                }
                else {
                    formatLabel = label + ' : ' + Capacity(series.data[0][1]) + 
                                  ' (' + Math.floor(series.percent) + '%)';
                }

                if(extra != null && label == extra.label){  
                    formatLabel += '<br />' + extra.data 
                }  

                return formatLabel;
            }
        }
    });    
}

function refresh_summary(){
    $.getJSON("../php/cluster.php",function(json){
        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["11", json[1]]);
            return;
        }

        allcluster = json[1];

        init_summary();

        $("#last_refresh_time").html(GetLastUpdateTime());         
    });
}

function operate_cc() {
    var item = get_cc_obj(curr_cluster);
    var run_state = item.run_state;

    var result;
    if (run_state == 1) {
        //if(confirm(page.res[page.pagelan]["xid"]["val"]["disable_cc_confirm"])){ 
            result = xmlrpc_service.tecs.cluster.disable(xmlrpc_session, item.name);               

            if (result[0] != 0) {
                ShowNotyRpcErrorInfo(result);
            } 
        //}    
    }
    else if (run_state == 2) {
        result = xmlrpc_service.tecs.cluster.enable(xmlrpc_session, item.name);

        if (result[0] != 0) {
            ShowNotyRpcErrorInfo(result);
        }  
    }

    refresh_summary(); 
}

function modify_cc_desc_click(){
    var hc_desc_dlg_html =
       '<form class="form-horizontal"> <fieldset>  \
            <div class="control-group">   \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Host.description"></OamStaticUI></label> \
                <div class="controls"><textarea type="text" maxlength="128" rows="6" id="cc_desc_in"></textarea><p id="cc_desc_tip"></p></div> \
            </div>    \
        </fieldset></form>';

    ShowDiaglog("cc_desc_dlg", hc_desc_dlg_html, {
        show:function() {
            $("#cc_desc_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });            

            var h5 = $("#cc_desc_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-edit"></i>' + h5.text() + " - " + curr_cluster);  

            LimitCharLenInput($("#cc_desc_in"),$("#cc_desc_tip"));

            var cc = get_cc_obj(curr_cluster);
            $("#cc_desc_in").val(cc.desc);
        },
        ok: function(){
            var ccDesc = $("#cc_desc_in").val();

            if(ccDesc.length > 128){
                ShowNotyRpcErrorInfo(["desc_too_long", ""]);
                return;
            }

            var rs = xmlrpc_service.tecs.cluster["set"](xmlrpc_session, curr_cluster, ccDesc);
            if (rs[0] == 0) {
                if($('ul.main-menu li').eq(0).hasClass("active")){
                    refresh_ccinfo();
                }
                else {
                    refresh_summary();
                }

                $("#cc_desc_dlg").modal("hide");
            }
            else {
                ShowNotyRpcErrorInfo(rs);
            } 
        }
    });       
}

//------------- cmm ---------------
function init_cmm() {
    cmmTable = $("#cmm_list").dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] }, 
                         { "bSortable": false, "aTargets": [ 6 ] },
                         { "bSortable": false, "aTargets": [ 7 ] } ],
        "alternatively" : "destory"
    }); 

    $("#cmmSelecltAll").click(cmm_select_all_click);
    $("#cmmAdd").parent().click(add_cmm);
    $("#cmmRemove").parent().click(remove_cmm); 
    $("#cmmRefresh").parent().click(refresh_cmm);   
}

//query cmm infomation by async xmlrpc call
function refresh_cmm() {
    cmmTable.fnClearTable();
    cmmData = [];
    xmlrpc_service.tecs.cmm.query(xmlrpc_session, new I8(0), new I8(50), curr_cluster, query_cmm_callback);
}

function query_cmm_callback(rs, ex) {
    if(rs[0] == 0){
        cmmData = cmmData.concat(rs[3]);
        if(rs[1] != -1){
            xmlrpc_service.tecs.cmm.query(xmlrpc_session, rs[1], new I8(50), curr_cluster, query_cmm_callback);
        }
        else{
            var map_info = page.res[page.pagelan]["type"]["opr"];

            var cmmTableData = [];
            for(var i=0; i<cmmData.length; i++){
                var oneCmm = [];
                oneCmm[0] = "<input type=checkbox />";
                oneCmm[1] = cmmData[i]["bureau"];
                oneCmm[2] = cmmData[i]["rack"];
                oneCmm[3] = cmmData[i]["shelf"];
                oneCmm[4] = cmmData[i]["type"];
                oneCmm[5] = '<a href="#" onclick=window.open("http://' + cmmData[i]["ip_address"] + '")>' + cmmData[i]["ip_address"] + '</a>';   
                oneCmm[6] = "<label>" + cmmData[i]["description"] + "</label>";
                oneCmm[7] = "<a href='javascript:void(0);' onclick='set_cmm(this)' data-rel='tooltip' data-original-title='" 
                              + map_info["setting"] + "'><i class='icon-edit'></i></a>";
                cmmTableData[cmmTableData.length] = oneCmm;                   
            }

            cmmTable.fnAddData(cmmTableData);

            $('.tooltip').hide();            
            $('[data-rel="tooltip"]').tooltip({"placement":"bottom"});

            $("#last_refresh_time").html(GetLastUpdateTime());             
        }
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }
}

function add_cmm() {
    var add_cmm_dlg_html = 
       '<form class="form-horizontal"><fieldset> \
            <div class="control-group"> \
                <label class="control-label"><span><OamStaticUI bindattr="text" maptype="field_name" bindvalue="cmm.bureau"></OamStaticUI></span><span><font color="red">*</font></span></label> \
                <div class="controls"><input type="text" id="cmm_bur_input" style="width:144px;" maxlength="3"/></div> \
            </div> \
            <div class="control-group"> \
                <label class="control-label"><span><OamStaticUI bindattr="text" maptype="field_name" bindvalue="cmm.rack"></OamStaticUI></span><span><font color="red">*</font></span></label> \
                <div class="controls"><input type="text" id="cmm_rack_input" style="width:144px;" maxlength="3" /></div> \
            </div> \
            <div class="control-group"> \
                <label class="control-label"><span><OamStaticUI bindattr="text" maptype="field_name" bindvalue="cmm.shelf"></OamStaticUI></span><span><font color="red">*</font></span></label> \
                <div class="controls"><input type="text" id="cmm_shelf_input" style="width:144px;" maxlength="3" /></div> \
            </div> \
            <div class="control-group"> \
                <label class="control-label"><span><OamStaticUI bindattr="text" maptype="field_name" bindvalue="cmm.type"></OamStaticUI></span><span><font color="red">*</font></span></label> \
                <div class="controls"> \
                    <select id="cmm_type_input" style="width:144px;"><option value="ATCA">ATCA<option value="ETCA">ETCA</select> \
                </div> \
            </div> \
            <div class="control-group"> \
                <label class="control-label"><span><OamStaticUI bindattr="text" maptype="field_name" bindvalue="cmm.ipaddr"></OamStaticUI></span><span><font color="red">*</font></span></label> \
                <div class="controls"><input type="text" id="cmm_ipaddr_input" style="width:144px;" /></div> \
            </div> \
            <div class="control-group"> \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="cmm.description"></OamStaticUI></label> \
                <div class="controls"><textarea id="cmm_desc_input" maxlength="128" style="width:144px;height:50px;"></textarea><p id="cmm_desc_tip"></p></div> \
            </div> \
        </fieldset></form>';

    ShowDiaglog("add_cmm_dlg", add_cmm_dlg_html, {
        show:function(){
            $("#add_cmm_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });

            var h5 = $("#add_cmm_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-plus"></i>' + h5.text());  

            LimitCharLenInput($("#cmm_desc_input"),$("#cmm_desc_tip"));
            $("#cmm_bur_input").val("");
            $("#cmm_rack_input").val("");
            $("#cmm_shelf_input").val("");
            $("#cmm_type_input").val("ATCA");              
            $("#cmm_ipaddr_input").val("");  
            $("#cmm_desc_input").val("");  
            
            $(".control-group").removeClass("error");
        },
        ok:function(){
            var bur = $("#cmm_bur_input").val();
            var rack = $("#cmm_rack_input").val();
            var shelf = $("#cmm_shelf_input").val();
            var type = $("#cmm_type_input").val();              
            var ipaddr = $("#cmm_ipaddr_input").val();  
            ipaddr = $.trim(ipaddr); 
            var desc = $("#cmm_desc_input").val(); 

            $(".control-group").removeClass("error");

            if(bur == "" || isNaN(bur)) {
                $("#cmm_bur_input").parent().parent().addClass("error");
                return;
            }

            if(rack == "" || isNaN(rack)) {
                $("#cmm_rack_input").parent().parent().addClass("error");
                return;
            } 

            if(shelf == "" || isNaN(shelf)) {
                $("#cmm_shelf_input").parent().parent().addClass("error");
                return;
            } 

            if(!checkIP(ipaddr)) {
                $("#cmm_ipaddr_input").parent().parent().addClass("error");
                return;
            }

            if(desc.length > 128){
                ShowNotyRpcErrorInfo(["desc_too_long", ""]);
                return;
            }            

            var rs = xmlrpc_service.tecs.cmm.add(xmlrpc_session, curr_cluster, 
                                                Number(bur), Number(rack),
                                                Number(shelf), type, ipaddr, desc);
            if (rs[0] == 0) {
                $("#add_cmm_dlg").modal('hide');
                refresh_cmm();
            }
            else {
                ShowNotyRpcErrorInfo(rs);
            }
        }
    });
}

function remove_cmm(){
    ClearAlertInfo();
    var pageVal = page.res[page.pagelan]["xid"]["val"];    
    var vCheckedbox = $(cmmTable.fnGetNodes()).find(":checkbox:checked");

    var vresultSucceeded = [];
    var vresultFailed    = [];

    if(vCheckedbox.size() > 0){
        ShowDiaglog("delete_cmm_dlg", pageVal["delete_cmm_confirm"], {
            show:function(){
                var h5 = $("#delete_cmm_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-trash"></i>' + h5.text());   
            },
            ok:function(){
                $("#delete_cmm_dlg").modal('hide');    

                vCheckedbox.each(function(){
                    var cmm_tr = $(this).parent().parent();      

                    var bur = cmm_tr.find("td").eq(1).text();
                    var rack = cmm_tr.find("td").eq(2).text();
                    var shelf = cmm_tr.find("td").eq(3).text();      

                    var rs = xmlrpc_service.tecs.cmm["delete"](xmlrpc_session, curr_cluster, 
                                                               Number(bur), Number(rack), Number(shelf));
                    ShowSynRpcInfo(rs);
                });    

                refresh_cmm();
            }
        });
    }
    else{
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]);
    }
}

function set_cmm(a){
    var set_cmm_form_html = 
       '<form class="form-horizontal"><fieldset> \
            <div class="control-group"> \
                <label class="control-label"><span><OamStaticUI bindattr="text" maptype="field_name" bindvalue="cmm.type"></OamStaticUI></span><span><font color="red">*</font></span></label> \
                <div class="controls"><select id="set_cmm_type" style="width:144px;"><option value="ATCA">ATCA<option value="ETCA">ETCA</select></div> \
            </div> \
            <div class="control-group"> \
                <label class="control-label"><span><OamStaticUI bindattr="text" maptype="field_name" bindvalue="cmm.ipaddr"></OamStaticUI></span><span><font color="red">*</font></span></label> \
                <div class="controls"><input type="text" id="set_cmm_ipaddr" style="width:144px;" /></div> \
            </div> \
            <div class="control-group"> \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="cmm.description"></OamStaticUI></label> \
                <div class="controls"><textarea id="set_cmm_desc" maxlength="128" style="width:144px;height:50px;"></textarea><p id="set_cmm_tip"></p></div> \
            </div> \
        </fieldset></form>';

    var cmm_tr = $(a).parent().parent();        

    var bur = cmm_tr.find("td").eq(1).text();
    var rack = cmm_tr.find("td").eq(2).text();
    var shelf = cmm_tr.find("td").eq(3).text();
    var type =  cmm_tr.find("td").eq(4).text();
    var ipaddr = cmm_tr.find("td").eq(5).text();
    var desc = cmm_tr.find("td").eq(6).text();  

    ShowDiaglog("set_cmm_dlg", set_cmm_form_html, {
        show:function(){
            $("#set_cmm_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });

            var h5 = $("#set_cmm_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-edit"></i>' + h5.text());  

            LimitCharLenInput($("#set_cmm_desc"), $("#set_cmm_tip"));

            $("#set_cmm_type").val(type);
            $("#set_cmm_ipaddr").val(ipaddr);
            $("#set_cmm_desc").val(desc);            

            $(".control-group").removeClass("error");
        },
        ok:function(){
            type = $("#set_cmm_type").val();

            $(".control-group").removeClass("error");

            var ipaddr = $("#set_cmm_ipaddr").val();  
            if(!checkIP(ipaddr)) {
                $("#set_cmm_ipaddr").parent().parent().addClass("error");
                return;
            }            

            var cmm_desc = $("#set_cmm_desc").val();

            if(cmm_desc.length > 128){
                ShowNotyRpcErrorInfo(["desc_too_long", ""]);
                return;
            }

            var rs = xmlrpc_service.tecs.cmm.set(xmlrpc_session, curr_cluster, 
                                                 Number(bur), Number(rack), Number(shelf),
                                                 type, ipaddr, cmm_desc);
            if (rs[0] == 0) {
                $("#set_cmm_dlg").modal('hide');
                refresh_cmm();
            }
            else {
                ShowNotyRpcErrorInfo(rs);
            }
        }
    });    
}

function cmm_select_all_click() {
    if($(this).attr("checked") == "checked") {
        $("#cmm_list tbody input[type=checkbox]").attr("checked", true);
    }
    else {
        $("#cmm_list tbody input[type=checkbox]").attr("checked", false);
    }
}

//---------------- host -----------------
function init_host(){
    hcTable = $("#host_list").dataTable({
        "sPaginationType": "bootstrap",        
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] }, 
                         { "bSortable": false, "aTargets": [ 3 ] },
                         { "bSortable": false, "aTargets": [ 5 ] },
                         { "bSortable": false, "aTargets": [ 6 ] },
                         { "bSortable": false, "aTargets": [ 7 ] } ],
        "alternatively" : "destory",
        "bProcessing": false,
        "bServerSide": true,
        "sAjaxSource": "../php/host.php",
        "fnServerParams": host_extra_param,
        "fnServerData": host_retrieve_data
    });    

    $("#hostSelectAll").click(host_select_all_click);   
    $("#hostDisable").parent().click(diable_host);
    $("#hostEnable").parent().click(enable_host);
    $("#hostReboot").parent().click(reboot_host);
    $("#hostShutdown").parent().click(shutdown_host);
    $("#hostForget").parent().click(forget_host);  
    $("#hostRefresh").parent().click(refresh_host_table);
}

var currHcState = new Object();

function refresh_host_table() {
    RecordSelItem(hcTable, "host_list", currHcState);
    GotoCurrentPageOfDataTable(hcTable);
}

function host_extra_param(aoData) {
    aoData.push({"name":"cluster_name", "value": curr_cluster });
}

function host_retrieve_data(sSource, aoData, fnCallback) {
    $.getJSON( sSource, aoData, function (json){
        hosts = json.rawData;

        InitSelectHost($("#selectHost"), refresh_vm);

        if(hosts != null){
            for (var i=0;i<hosts.length;i++) {
                //转换内存和硬盘的单位
                hosts[i].mem_max_p    = Capacity(hosts[i].mem_max);
                hosts[i].mem_total_p  = Capacity(hosts[i].mem_total);
                hosts[i].mem_free_p   = Capacity(hosts[i].mem_free);
                hosts[i].disk_max_p   = Capacity(hosts[i].disk_max);
                hosts[i].disk_total_p = Capacity(hosts[i].disk_total);
                hosts[i].disk_free_p  = Capacity(hosts[i].disk_free);    

                hosts[i].running_time = sToDate(hosts[i].running_time);
                
                var location_xml = "<pos>" + hosts[i].location +"</pos>";
                hosts[i].location = "";    

                /*var xmldoc = LoadXMLStr(location_xml);    

                var root = xmldoc.firstChild;    
                for(var f = root.firstChild; f!=null; f=f.nextSibling){
                    var tag = f.tagName;
                    var content = f.text;
                    hosts[i].location += tag + ":" + content + ";";
                }*/
                if(hosts[i].bureau != null){
                    hosts[i].location = "Bureau:" + hosts[i].bureau + "; ";
                }
                if(hosts[i].rack != null){
                    hosts[i].location += "Rack:" + hosts[i].rack + "; ";
                }
                if(hosts[i].shelf != null){
                    hosts[i].location += "Shelf:" + hosts[i].shelf + "; ";
                }
                if(hosts[i].slot != null){
                    hosts[i].location += "Slot:" + hosts[i].slot;
                }                                                    

                var onehost = [];
                onehost[0] = "";
                onehost[1] = hosts[i].host_name;
                onehost[2] = hosts[i].ip_address;
                onehost[3] = "";
                onehost[4] = hosts[i].running_vms;
                onehost[5] = hosts[i].description;
                onehost[6] = "";
                onehost[7] = "";
                json.aaData[i] = onehost;
            }    

            //为过滤条件添加颜色
            SetFilterFlag(aoData, json);    

            for (var i=0; i<hosts.length; i++) {
                //物理机状态
                var hc_state = "";
                var map_info = page.res[page.pagelan]["type"]["HostRunState"];
                if(hosts[i].run_state == 2){
                    hc_state = '<img src="img/hc_ok.png" data-rel="tooltip" data-original-title="'
                               + map_info[hosts[i].run_state] +'"/>'
                }
                else if(hosts[i].run_state == 1){
                    hc_state = '<img src="img/hc_offline.png" data-rel="tooltip" data-original-title="' 
                               + map_info[hosts[i].run_state] +'"/>'
                }    

                var map_info = page.res[page.pagelan]["type"]["HostDisabled"];
                if(hosts[i].is_disabled){
                    hc_state += '<img src="img/hc_mantaince.png" data-rel="tooltip" data-original-title="' 
                                + map_info[hosts[i].is_disabled] +'"/>'
                }    

                var map_info = page.res[page.pagelan]["type"]["HardwareState"];
                if(hosts[i].hardware_state != "0"){
                    hc_state += '<img src="img/hc_error.png" data-rel="tooltip" data-original-title="'
                                + map_info[hosts[i].hardware_state] +'"/>'
                }    

                var map_info = page.res[page.pagelan]["type"]["opr"];    

                json.aaData[i][0] = GetItemCheckbox(hosts[i].host_name,currHcState);
                json.aaData[i][1] = '<a href="#" onclick="host_detail($(this))">'
                                    + json.aaData[i][1] + '</a>';
                json.aaData[i][3] = hc_state;
                json.aaData[i][4] = '<a href="javascript:void(0);" onclick="goto_vms($(this))">'
                                    + json.aaData[i][4] +'</a>';
                json.aaData[i][6] = '<a href="#" onclick="extern_ctrl($(this))">'
                                    + map_info["config"] +'</a>';
                json.aaData[i][7] = '<a href="javascript:void(0);" onclick="modify_hc_desc($(this))" data-rel="tooltip" data-original-title="'
                                    + map_info["modify"] + '"><i class="icon-edit"></i></a>' + " | " + 
                                    
                                    '<a href="javascript:void(0);" onclick="hostHwmonInfo($(this) ,'+ hosts[i].host_id +')" data-rel="tooltip" data-original-title="'
                                    + map_info["sensorInfo"] + '"><i class="icon-info-sign"></i></a>' ;

                json.aaData[i][7] += "|" + '<a href="javascript:void(0);" onclick="host_statistics($(this))" data-rel="tooltip" data-original-title="'
                                    + map_info["statistics"] + '"><i class="icon icon-image"></i></a>';                    
            }
        }

        if(json.err_info != null) {
           ShowErrorInfo(["", json.err_info]);
        }

        fnCallback(json);

        $('.tooltip').hide();
        $('[data-rel="tooltip"]').tooltip({"placement":"bottom"});    

        ClearCurSelItem(currHcState);
        $("#last_refresh_time").html(GetLastUpdateTime());
    });
}

function hostHwmonInfo(hc,hid) {
	var sSource = "../php/hwmon_query.php";
	var aoData = {
			       "host_id"         :   hid,
			       "cluster_name"    :   curr_cluster
			      };
	$.getJSON( sSource, aoData, function (json){
		var hwmonData = json.rawData[0];
		var sensorDetailHtml = '<table class="table table-condensed" id="sensorDetailTable"><tbody> ';
		var tempStart = 1;
		var cpuStart = 0;
		var fanStart = 1;
		var inStart = 0;
		
		for(var i = 0 ; i < 33 ; i++ ){
			if(i < 8){
				var key = "temp" + tempStart + "_name";
				var valueKey = "temp" + tempStart + "_value";;
				var keyName = hwmonData[key];
				var displayStr = getDisplayHTML(keyName,hwmonData[valueKey]);
				if("" != displayStr){
					sensorDetailHtml += displayStr;
				}
				++tempStart; 
			} else if(i < 10){
				var key = "cpu" + cpuStart + "_vid_name";
				var valueKey = "cpu" + cpuStart + "_vid_value";;
				var keyName = hwmonData[key];
				var displayStr = getDisplayHTML(keyName,hwmonData[valueKey]);
				if("" != displayStr){
					sensorDetailHtml += displayStr;
				}
				++cpuStart; 
			}else if(i < 18) {
				var key = "fan" + fanStart + "_rpm_name";
				var valueKey = "fan" + fanStart + "_rpm_value";;
				var keyName = hwmonData[key];
				var displayStr = getDisplayHTML(keyName,hwmonData[valueKey]);
				if("" != displayStr){
					sensorDetailHtml += displayStr;
				}
				++fanStart; 
			}else {
				var key = "in" + inStart + "_name";
				var valueKey = "in" + inStart + "_value";;
				var keyName = hwmonData[key];
				var displayStr = getDisplayHTML(keyName,hwmonData[valueKey]);
				if("" != displayStr){
					sensorDetailHtml += displayStr;
				}
				++inStart; 
			}
		}
		sensorDetailHtml += "</tbody></table>";
			
			
		ShowDiaglog("sensorDetailDLG", sensorDetailHtml, {
	        show:function() {
	            var h5 = $("#sensorDetailDLG").find('div.modal-header h5');
	            h5.html('<i class="icon-zoom-in"></i>' + h5.text() );  

	        },
	        close: function(){
	            $("#host_detail_dlg").modal("hide");
	        }
	    });	
		
    });    
}

function isKeyEmpty(key){
	return "" == key || null == key || "undefined" == typeof(key);
}

function getDisplayHTML(keyName,value) {
	if(!isKeyEmpty(keyName)){
		return '<tr><td width="30%">'+ keyName +'</td><td> '+ value +' </td></tr>'
	} else {
		return "";
	}
}

function host_select_all_click() {
    if($(this).attr("checked") == "checked") {
        $("#host_list tbody input[type=checkbox]").attr("checked", true);
    }
    else {
        $("#host_list tbody input[type=checkbox]").attr("checked", false);
    }
}

function diable_host() {
    ClearAlertInfo();
    var pageVal = page.res[page.pagelan]["xid"]["val"];
    var vCheckedbox = $(hcTable.fnGetNodes()).find(":checkbox:checked");

    var vresultSucceeded = [];
    var vresultFailed    = [];

    if(vCheckedbox.size() > 0){
        for (var i = 0; i < vCheckedbox.length; i++) {
            var tr = $(vCheckedbox[i]).parent().parent();  
            var hostname = tr.find("td").eq(1).text();            
            var hostObj = get_hc_obj(hostname);
            if(hostObj.is_disabled){
                var diag = $("#idx_detailDlg");                                     
                diag.find('div.modal-header h5').html(pageVal["tip"]);
                diag.find('div.modal-body').html(hostname + pageVal["already_disabled"]);
                diag.modal("show");                        
                return;
            }
        };

        ShowDiaglog("disable_host_dlg", pageVal["disable_host_confirm"], {
            show:function(){
                var h5 = $("#disable_host_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-wrench"></i>' + h5.text()); 
            },
            ok:function(){
                $("#disable_host_dlg").modal('hide');    
    
                vCheckedbox.each(function(){
                    var tr = $(this).parent().parent();      

                    var hostname = tr.find("td").eq(1).text();
                    var running_vms = $(this).parent().parent().find("td").eq(4).text();

                    var rs;
                    if(running_vms == "0"){
                        rs = xmlrpc_service.tecs["host"].disable(xmlrpc_session, curr_cluster, hostname, false);
                        ShowSynRpcInfo(rs);
                        refresh_host_table();
                    }
                    else {
                        var message = page.res[page.pagelan]["type"]["force_msg"]["disable"];
                        ShowDiaglog("force_disable_dlg" + hostname, hostname + message, {
                            show:function(){
                                var h5 = $("#force_disable_dlg" + hostname).find('div.modal-header h5');
                                h5.html('<i class="icon-exclamation-sign"></i>' + page.res[page.pagelan]["xid"]["dialog"]["force_disable_dlg"]); 
                            },
                            ok:function(){
                                $("#force_disable_dlg" + hostname).modal("hide");
                                rs = xmlrpc_service.tecs["host"].disable(xmlrpc_session, curr_cluster, hostname, true); 
                                ShowSynRpcInfo(rs);
                                refresh_host_table();                               
                            }
                        });                        
                    }
                });
            }
        });
    }
    else{
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]);
    }
}

function enable_host() {
    ClearAlertInfo();
    var vCheckedbox = $(hcTable.fnGetNodes()).find(":checkbox:checked");

    var vresultSucceeded = [];
    var vresultFailed    = [];

    if(vCheckedbox.size() > 0){
        /*ShowDiaglog("enable_host_dlg", "enable_host_confirm", {
            ok:function(){
                $("#enable_host_dlg").modal('hide');    */

                vCheckedbox.each(function(){
                    var tr = $(this).parent().parent();      

                    var hostname = tr.find("td").eq(1).text();

                    var rs = xmlrpc_service.tecs["host"].enable(xmlrpc_session, curr_cluster, hostname);

                    ShowSynRpcInfo(rs);
                });    

                refresh_host_table();
        /*    }
        });*/
    }
    else{
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]);
    }
}

function reboot_host() {
    ClearAlertInfo();
    var pageVal = page.res[page.pagelan]["xid"]["val"];
    var vCheckedbox = $(hcTable.fnGetNodes()).find(":checkbox:checked");

    var vresultSucceeded = [];
    var vresultFailed    = [];

    if(vCheckedbox.size() > 0){
        ShowDiaglog("reboot_host_dlg", pageVal["reboot_host_confirm"], {
            show:function(){
                var h5 = $("#reboot_host_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-repeat"></i>' + h5.text()); 
            },
            ok:function(){
                $("#reboot_host_dlg").modal('hide');    

                vCheckedbox.each(function(){
                    var tr = $(this).parent().parent();      

                    var hostname = tr.find("td").eq(1).text();
                    var running_vms = $(this).parent().parent().find("td").eq(4).text();

                    var rs;
                    if(running_vms == "0"){
                        rs = xmlrpc_service.tecs["host"].reboot(xmlrpc_session, curr_cluster, hostname, false);
                        ShowSynRpcInfo(rs); 
                    }
                    else {
                        var message = page.res[page.pagelan]["type"]["force_msg"]["reboot"];
                        ShowDiaglog("force_reboot_dlg", hostname + message, {
                            show:function(){
                                var h5 = $("#force_reboot_dlg").find('div.modal-header h5');
                                h5.html('<i class="icon-exclamation-sign"></i>' + h5.text()); 
                            },                            
                            ok:function(){
                                $("#force_reboot_dlg").modal("hide");
                                rs = xmlrpc_service.tecs["host"].reboot(xmlrpc_session, curr_cluster, hostname, true); 
                                ShowSynRpcInfo(rs);                               
                            }
                        });
                    }
                });    

                refresh_host_table();
            }
        });
    }
    else{
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]);
    }
}

function shutdown_host() {
    ClearAlertInfo();
    var pageVal = page.res[page.pagelan]["xid"]["val"];
    var vCheckedbox = $(hcTable.fnGetNodes()).find(":checkbox:checked");

    var vresultSucceeded = [];
    var vresultFailed    = [];

    if(vCheckedbox.size() > 0){
        ShowDiaglog("shutdown_host_dlg", pageVal["shutdown_host_confirm"], {
            show:function(){
                var h5 = $("#shutdown_host_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-off"></i>' + h5.text()); 
            },            
            ok:function(){
                $("#shutdown_host_dlg").modal('hide');  

                vCheckedbox.each(function(){
                    var tr = $(this).parent().parent();      

                    var hostname = tr.find("td").eq(1).text();

                    var rs = xmlrpc_service.tecs["host"].shutdown(xmlrpc_session, curr_cluster, hostname);

                    ShowSynRpcInfo(rs);
                });    

                refresh_host_table();
            }
        });
    }
    else{
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]);
    }
}

function forget_host() {
    ClearAlertInfo();
    var pageVal = page.res[page.pagelan]["xid"]["val"];
    var vCheckedbox = $(hcTable.fnGetNodes()).find(":checkbox:checked");

    var vresultSucceeded = [];
    var vresultFailed    = [];

    if(vCheckedbox.size() > 0){
        ShowDiaglog("forget_host_dlg", pageVal["forget_host_confirm"], {
            show:function(){
                var h5 = $("#forget_host_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-trash"></i>' + h5.text()); 
            },            
            ok:function(){
                $("#forget_host_dlg").modal('hide');  

                vCheckedbox.each(function(){
                    var tr = $(this).parent().parent();      

                    var hostname = tr.find("td").eq(1).text();

                    var rs = xmlrpc_service.tecs["host"].forget(xmlrpc_session, curr_cluster, hostname);

                    ShowSynRpcInfo(rs);
                });    

                refresh_host_table();
            }
        });
    }
    else{
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]);
    }
}

function get_hc_obj(hcname) {
    for (var i=0;i<hosts.length;i++) {
        if(hosts[i].host_name == hcname)
           return hosts[i];
    }
    return null;
}

function host_detail(hc) {
    var host_detail_dlg_html = 
       '<table class="table table-condensed" id="host_detail_table"><tbody> \
            <tr> \
                <td width="30%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Host.location"></OamStaticUI></td> \
                <td><span type="text" class="OamUi" bindattr="text" bind="location"></span></td> \
            </tr> <!--tr> \
                <td width="30%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Host.description"></OamStaticUI></td> \
                <td><span type="text" class="OamUi" bindattr="text" bind="description"></span> \
                   (<a href="javascript:void(0);" class="OamUi OamButton" bindattr="data" bind="." fn="modify_hc_desc"> \
                    <OamStaticUI class="OamUi" bindattr="text" maptype="opr" bindvalue="modify"></OamStaticUI></a>) \
                </td> \
            </tr--> <tr> \
                <td width="30%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Host.create_time"></OamStaticUI></td> \
                <td><span type="text" class="OamUi" bindattr="text" bind="create_time"></span></td> \
            </tr> <tr> \
                <td width="30%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Host.runtime"></OamStaticUI></td> \
                <td><span type="text" class="OamUi" bindattr="text" bind="running_time"/></span></td> \
            </tr> <tr> \
                <td width="30%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Host.vmm_info"></OamStaticUI></td> \
                <td><span type="text" class="OamUi" bindattr="text" bind="vmm_info"></span></td> \
            </tr> <tr> \
                <td width="30%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Host.os_info"></OamStaticUI></td> \
                <td><span type="text" class="OamUi" bindattr="text" bind="os_info"></span></td> \
            </tr> <tr> \
                <td width="30%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Host.is_support_hvm"></OamStaticUI></td> \
                <td><span type="text" class="OamUi" bindattr="text" bind="is_support_hvm" maptype="Host_hvm"></span></td> \
            </tr> <tr> \
                <td width="30%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="tcu.cpu_info_id"></OamStaticUI></td> \
                <td> \
                    <a href="javascript:void(0);" class="OamUi OamButton" bindattr="data" bind="." fn="goto_tcu"> \
                    <span type="text" class="OamUi" bindattr="text" bind="cpu_info_id"></span></a> \
                </td> \
            </tr> <tr> \
                <td width="30%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Host.pdh"></OamStaticUI></td> \
                <td><span type="text" class="OamUi" bindattr="text" bind="pdh"></span></td> \
            </tr> <tr> \
            <tr> \
                <td width="30%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Host.cpus"></OamStaticUI></td> \
                <td><span type="text" class="OamUi" bindattr="text" bind="cpus"/></span></td> \
            </tr> \
                <td width="30%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Host.svcpu_max"></OamStaticUI></td> \
                <td><span type="text" class="OamUi" bindattr="text" bind="tcu_max"></span></td> \
            </tr> <tr> \
                <td width="30%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Host.svcpu_free"></OamStaticUI></td> \
                <td><span type="text" class="OamUi" bindattr="text" bind="tcu_free"></span></td> \
            </tr> <tr> \
                <td width="30%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Host.mem_max"></OamStaticUI></td> \
                <td><span type="text" class="OamUi" bindattr="text" bind="mem_max_p"></span></td> \
            </tr> <tr> \
                <td width="30%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Host.mem_total"></OamStaticUI></td> \
                <td><span type="text" class="OamUi" bindattr="text" bind="mem_total_p"></span></td> \
            </tr><tr> \
                <td width="30%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Host.mem_free"></OamStaticUI></td> \
                <td><span type="text" class="OamUi" bindattr="text" bind="mem_free_p"></span></td> \
            </tr><tr> \
                <td width="30%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Host.disk_max"></OamStaticUI></td> \
                <td><span type="text" class="OamUi" bindattr="text" bind="disk_max_p"></span></td> \
            </tr><tr> \
                <td width="30%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Host.disk_total"></OamStaticUI></td> \
                <td><span type="text" class="OamUi" bindattr="text" bind="disk_total_p"></span></td> \
            </tr><tr> \
                <td width="30%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Host.disk_free"></OamStaticUI></td> \
                <td><span type="text" class="OamUi" bindattr="text" bind="disk_free_p"></span></td> \
            </tr><tr> \
                <td width="30%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Host.cpu_usage_1m"></OamStaticUI></td> \
                <td><span type="text" class="OamUi" bindattr="text" bind="cpu_usage_1m"></span><span>‰</span></td> \
            </tr><tr> \
                <td width="30%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Host.cpu_usage_5m"></OamStaticUI></td> \
                <td><span type="text" class="OamUi" bindattr="text" bind="cpu_usage_5m"/></span><span>‰</span></td> \
            </tr><tr> \
                <td width="30%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Host.cpu_usage_30m"></OamStaticUI></td> \
                <td><span type="text" class="OamUi" bindattr="text" bind="cpu_usage_30m"/></span><span>‰</span></td> \
            </tr><tr class="rate_power"> \
                <td width="30%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Host.rate_power"></OamStaticUI></td> \
                <td><span type="text" class="OamUi" bindattr="text" bind="rate_power"/></span>&nbsp;W</td> \
            </tr><tr> \
                <td width="30%"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Host.total_poweroff_time"></OamStaticUI></td> \
                <td><span type="text" class="OamUi" bindattr="text" bind="total_poweroff_time"/></span></td> \
            </tr> \
        </tbody> </table>';

    ShowDiaglog("host_detail_dlg", host_detail_dlg_html, {
        show:function() {
            $("#host_detail_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });

            var h5 = $("#host_detail_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-zoom-in"></i>' + h5.text() + " - " + hc.text());  

            var item = get_hc_obj(hc.text());
            if(item == null) {
                ShowNotyRpcErrorInfo(["1", "no such host object"]);
                return;
            }
            if(item["pdh"] == null) item["pdh"] = "Not Support";
            SetModifyForm($("#host_detail_dlg"), item);

            if(item["rate_power"] == null) {
                $(".rate_power").hide();
            }
            else {
                $(".rate_power").show();
            }
        },
        close: function(){
            $("#host_detail_dlg").modal("hide");
        }
    });
}

function modify_hc_desc(hc) {
    var hc_desc_dlg_html =
       '<form class="form-horizontal"> <fieldset>    \
            <div class="control-group">     \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Host.description"></OamStaticUI></label>  \
                <div class="controls"><textarea type="text" rows="6" maxlength="128" id="hc_desc_in"></textarea><p id="hc_desc_tip"></p></div>   \
            </div>    \
        </fieldset></form>';
    
    var hcName = hc.parent().parent().find("td").eq(1).text(); 

    var item = get_hc_obj(hcName);

    ShowDiaglog("host_desc_dlg", hc_desc_dlg_html, {
        show:function() {
            $("#host_desc_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });            

            var h5 = $("#host_desc_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-edit"></i>' + h5.text() + " - " + item.host_name);  

            LimitCharLenInput($("#hc_desc_in"), $("#hc_desc_tip"));

            $("#hc_desc_in").val(item.description);
        },
        ok: function(){
            var hc_desc = $("#hc_desc_in").val();

            if(hc_desc.length > 128){
                ShowNotyRpcErrorInfo(["desc_too_long", ""]);
                return;
            }

            var rs = xmlrpc_service.tecs.host["set"](xmlrpc_session, curr_cluster, item.host_name, hc_desc);
            if (rs[0] == 0) {
                $("#host_desc_dlg").modal("hide");
                refresh_host_table();
            }
            else {
                ShowNotyRpcErrorInfo(rs);
            } 
        }
    });    
}

function goto_tcu() {
    $("#host_detail_dlg").modal("hide");
    $("#ccTcuNav").tab("show");
    refresh_tcu(); 
}

function goto_vms(hc){
    var hcName = hc.parent().parent().find("td").eq(1).text(); 
    
    //$("#ccVmNav").tab("show");      

    $("#selectHost").val(hcName);
    $("#selectHost").trigger("liszt:updated");  

    //refresh_vm(); 
    $("#ccVmNav").trigger("click");     
}

function show_port(hc) {
    var port_trunk_dlg_html = 
       '<div> \
            <ul class="nav nav-tabs" id="port_trunk_tab"> \
                <li class="active"><a id="port_nav" href="#port_page">Port</a></li> \
                <li><a id="trunk_nav" href="#trunk_page">Trunk</a></li> \
            </ul> \
            <div class="tab-content"><div class="tab-pane active" id="port_page"> \
                <table id="port_list" class="table table-striped table-bordered table-condensed"> \
                    <thead><tr> \
                          <th><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Port.name"></OamStaticUI></th> \
                          <th><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Port.used"></OamStaticUI></th> \
                          <th><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Port.loop"></OamStaticUI></th> \
                          <th><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Port.negotiate"></OamStaticUI></th> \
                          <th><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Port.speed"></OamStaticUI></th> \
                          <th><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Port.duplex"></OamStaticUI></th> \
                          <th><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Port.master"></OamStaticUI></th> \
                          <th><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Port.state"></OamStaticUI></th> \
                          <th><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Port.linked"></OamStaticUI></th> \
                          <th><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Port.netplane"></OamStaticUI></th> \
                          <th><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Port.sriov"></OamStaticUI></th> \
                    </tr></thead><tbody></tbody> \
                </table> <br />\
            </div> \
            <div class="tab-pane" id="trunk_page"> \
                <div align="right"> \
                    <button class="btn"><i class="icon-plus"></i><span id="add_trunk"></span></button> \
                </div> \
                <table id="trunk_list" class="table table-striped table-bordered table-condensed"> \
                    <thead><tr> \
                          <th><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Trunk.name"></OamStaticUI></th> \
                          <th><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Trunk.type"></OamStaticUI></th> \
                          <th><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Trunk.state"></OamStaticUI></th> \
                          <th><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Trunk.members"></OamStaticUI></th> \
                          <th><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Trunk.netplane"></OamStaticUI></th> \
                          <th></th> \
                    </tr></thead> <tbody></tbody> \
                </table> <br />\
            </div></div>\
        </div>';

    hc_clicked = hc.parent().parent().find("td").eq(1).text();    

    ShowDiaglog("port_trunk_dlg", port_trunk_dlg_html, {
        init: function() {
            portTable = $("#port_list").dataTable({
                "oLanguage": tableLanguage[page.pagelan],
                "sDom":"t",
                "iDisplayLength": 50,
                "alternatively" : "destory"
            });         

            trunkTable = $("#trunk_list").dataTable({
                "oLanguage": tableLanguage[page.pagelan],
                "sDom":"t", 
                "iDisplayLength": 50,               
                "alternatively" : "destory"
            });    

            $("#add_trunk").parent().click(add_turnk_click);

            $("#port_trunk_dlg").attr("style", "width:800px;");
        },
        show: function() {
            $("#port_page").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });

            $("#trunk_page").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });

            var h5 = $("#port_trunk_dlg").find('div.modal-header h5');
            h5.html(h5.text() + " - " + hc_clicked);  

            $('#port_trunk_tab a:first').tab('show');
            $('#port_trunk_tab a').click(function (e) {
                e.preventDefault();
                $(this).tab('show');                          
            });

            refresh_host_port();

            port_timer_id = setInterval(refresh_host_port, 10000);         
        },
        close: function() {
            $("#port_trunk_dlg").modal("hide");
            clearInterval(port_timer_id);
        }
    });
}

function refresh_host_port() {
    var hc_name = hc_clicked;
    unbond_ports = [];

    var params = [];
    params.push({"name":"cc", "value":curr_cluster});
    params.push({"name":"hc", "value":hc_name});    

    $.getJSON("../php/port_trunk.php", params, function (json){
        portTable.fnClearTable();
        trunkTable.fnClearTable();

        var record_port = json["port"];
        var record_trunk = json["trunk"];

        if (record_port.length > 0) {
            for(var k=0; k<record_port.length; k++){
               if(record_port[k]["trunk"] == null){
                   record_port[k]["trunk"] = "";
                   unbond_ports[unbond_ports.length] = record_port[k].name;
               }
            }
        }

        var mapType = page.res[page.pagelan]["type"];

        var portData = [];
        for (var i = 0; i < record_port.length; i++) {
            var onePort = [];
            onePort[0] = record_port[i].name + '(<a href="#" onclick="set_port(this)">' + mapType["opr"]["setting"] + '</a>)';
            onePort[1] = mapType["PortUsed"][record_port[i].used];
            onePort[2] = mapType["PortLoop"][record_port[i].loopback_mode] + '<a href="#" onclick="loop_port(this)">' + mapType["port_loop_opr"][record_port[i].loopback_mode] + '</a>';
            onePort[3] = mapType["PortNegotiate"][record_port[i].negotiate];
            onePort[4] = mapType["PortSpeed"][record_port[i].speed];
            onePort[5] = mapType["PortDuplex"][record_port[i].duplex];
            onePort[6] = mapType["PortMaster"][record_port[i].master];
            onePort[7] = mapType["PortState"][record_port[i].state];
            onePort[8] = mapType["PortLinked"][record_port[i].linked];
            onePort[9] = record_port[i].netplane + '(<a href="#" onclick="set_port_netplane(this)">' + mapType["opr"]["setting"] + '</a>)';
            onePort[10] = mapType["PortSriov"][record_port[i].sriov] + '(' + record_port[i].sriov_num + ')';
            portData.push(onePort);
        };       
        portTable.fnAddData(portData);
        set_node_data(portTable, record_port);

        var trunkData = [];
        for (var i = 0; i < record_trunk.length; i++) {
            record_trunk[i]
            var oneTrunk = [];
            oneTrunk[0] = record_trunk[i].trunk_name + '(<a href="#" onclick="set_trunk(this)">' + mapType["opr"]["setting"] + '</a>)';
            oneTrunk[1] = mapType["TrunkType"][record_trunk[i].trunk_type];
            oneTrunk[2] = mapType["TrunkState"][record_trunk[i].trunk_state];
            oneTrunk[3] = record_trunk[i].trunk_ports;
            oneTrunk[4] = record_trunk[i].netplane + '(<a href="#" onclick="set_trunk_netplane(this)">' + mapType["opr"]["setting"] + '</a>)';
            oneTrunk[5] = '<a href="#" onclick="del_trunk(this)">' + mapType["opr"]["remove"] + '</a>';
            trunkData.push(oneTrunk);            
        };
        trunkTable.fnAddData(trunkData);
        set_node_data(trunkTable, record_trunk);
    });
}

function set_port(a) {
    var set_port_dlg_html  = 
       '<table id="set_port_table"><tbody> \
            <tr> \
                <td class="td_l"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Port.negotiate"></OamStaticUI></td> \
                <td class="td_r"><select id="select_negotiate" maptype="PortNegotiate" style="width:144px;"><option value="0"><option value="1"></select></td> \
            </tr> \
            <tr> \
                <td class="td_l"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Port.speed"></OamStaticUI></td> \
                <td class="td_r"><select id="select_speed" maptype="PortSpeed" style="width:144px;"><option value="0"><option value="1"><option value="2"><option value="3"><option value="4"></select></td> \
            </tr> \
            <tr> \
                <td class="td_l"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Port.duplex"></OamStaticUI></td> \
                <td class="td_r"><select id="select_duplex" maptype="PortDuplex" style="width:144px;"><option value="0"><option value="1"><option value="2"></select></td> \
            </tr> \
            <tr> \
                <td class="td_l"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Port.master"></OamStaticUI></td> \
                <td class="td_r"><select id="select_master" maptype="PortMaster" style="width:144px;"><option value="0"><option value="1"><option value="2"><option value="3"></select></td> \
            </tr> \
            <!--tr> \
                <td class="td_l"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Port.delay_up_time"></OamStaticUI></td> \
                <td class="td_r"><input type="text" id="input_delay_up_time" maxlength="5"></input></td> \
            </tr--> \
        </tbody></table>';

    var item = $(a).parent().parent().data("item");
    var hcName = hc_clicked;

    ShowDiaglog("set_port_dlg", set_port_dlg_html, {
        show : function(){
            $("#port_trunk_dlg").modal("hide");

            $("#set_port_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });
            $("#set_port_dlg").find("option").each(function(){
                var maptype = $(this).parent().attr("maptype");
                var mapkey = $(this).val();        
                $(this).text(page.res[page.pagelan].type[maptype][mapkey]);
            });    

            var h5 = $("#set_port_dlg").find('div.modal-header h5');
            h5.html(h5.text() + " - " + hcName + " - " + item.name);             

            $("#select_negotiate").attr("value",item.negotiate);
            $("#select_speed").attr("value",item.speed);
            $("#select_duplex").attr("value",item.duplex);
            $("#select_master").attr("value",item.master);
        },
        ok : function(){
            var portParam = {};
            portParam.used = parseInt(item.used);
            portParam.negotiate  = parseInt($("#select_negotiate").val());
            portParam.speed  = parseInt($("#select_speed").val());
            portParam.duplex  = parseInt($("#select_duplex").val());
            portParam.master  = parseInt($("#select_master").val());

            var rs = xmlrpc_service.tecs["port"].set(xmlrpc_session, curr_cluster, hcName, item.name, portParam);
            if(rs[0] != 0) {
                ShowNotyRpcErrorInfo(rs);
                return;
            }

            $("#set_port_dlg").modal("hide");
            $("#port_trunk_dlg").modal("show");

            refresh_host_port();
        },
        cancel: function(){
            $("#set_port_dlg").modal("hide");
            $("#port_trunk_dlg").modal("show");            
        }
    });
}

function set_port_netplane(a) {
    var set_port_netplane_dlg_html = 
       '<table id="set_port_netplane_table"><tbody> \
            <tr> \
                <td class="td_l"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Port.netplane"></OamStaticUI></td> \
                <td class="td_r"><select id="port_netplane" style="width:144px;"></select></td> \
            </tr> \
        </tbody></table>';

    var item = $(a).parent().parent().data("item");
    var hcName = hc_clicked;

    ShowDiaglog("set_port_netplane_dlg", set_port_netplane_dlg_html, {
        show: function(){
            $("#port_trunk_dlg").modal("hide");              
            $("#set_port_netplane_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });

            var h5 = $("#set_port_netplane_dlg").find('div.modal-header h5');
            h5.html(h5.text() + " - " + hcName + " - " + item.name); 

            var rs = xmlrpc_service.tecs.cluster.query_netplane(xmlrpc_session, curr_cluster);        

            if(rs[0] == 0){
                $("#port_netplane").empty();
                $("#port_netplane").append("<option value=''>");
                for(var i=0; i<rs[1].length; i++) {
                    $("#port_netplane").append("<option value='" + rs[1][i] + "'>" + rs[1][i]);
                }
                $("#port_netplane").val(item.netplane);
            }
        },
        ok: function(){
            var netplane = $("#port_netplane").val();
            var rs = xmlrpc_service.tecs["port_netplane"].set(xmlrpc_session, curr_cluster, hcName, item.name, netplane);
            if(rs[0] != 0) {
                ShowNotyRpcErrorInfo(rs);
                return;
            }
            $("#set_port_netplane_dlg").modal("hide");
            $("#port_trunk_dlg").modal("show");  

            refresh_host_port();
        },
        cancel: function(){
            $("#set_port_netplane_dlg").modal("hide");
            $("#port_trunk_dlg").modal("show");               
        }
    });
}

function set_trunk(a) {
    if($("#trunk_page").attr("disabled")) return;

    var set_trunk_dlg_html  = 
       '<table><tbody> \
            <tr> \
                <td class="td_l"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Trunk.type"></OamStaticUI></td> \
                <td class="td_r"><select id="select_trunk_type" maptype="TrunkSetType" style="width:144px;"><option value="0"><option value="1"><option value="4"></select></td> \
            </tr> \
        </tbody></table>'; 

    var item = $(a).parent().parent().data("item");
    var hcName = hc_clicked;

    ShowDiaglog("set_trunk_dlg", set_trunk_dlg_html, {
        show: function(){
            $("#port_trunk_dlg").modal("hide");

            $("#set_trunk_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });
            $("#set_trunk_dlg").find("option").each(function(){
                var maptype = $(this).parent().attr("maptype");
                var mapkey = $(this).val(); 
                $(this).text(page.res[page.pagelan].type[maptype][mapkey]);
            });

            var h5 = $("#set_trunk_dlg").find('div.modal-header h5');
            h5.html(h5.text() + " - " + hcName + " - " + item.trunk_name);             

            $("#select_trunk_type").val(item.trunk_type);            
        },
        ok: function(){
            var trunkType = parseInt($("#select_trunk_type").val());

            var rs = xmlrpc_service.tecs["trunk"].set(xmlrpc_session, curr_cluster, hcName, item.trunk_name, trunkType);
            if(rs[0] != 0) {
                ShowNotyRpcErrorInfo(rs);
                return;
            }
            $("#set_trunk_dlg").modal("hide");
            $("#port_trunk_dlg").modal("show");

            refresh_host_port();
        },
        cancel: function(){
            $("#set_trunk_dlg").modal("hide");
            $("#port_trunk_dlg").modal("show");            
        }
    });   
}

function set_trunk_netplane(a) {
    if($("#trunk_page").attr("disabled")) return;

    var set_trunk_netplane_dlg_html = 
       '<table id="set_trunk_netplane_table"><tbody> \
            <tr> \
                <td class="td_l"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Port.netplane"></OamStaticUI></td> \
                <td class="td_r"><select id="trunk_netplane" style="width:144px;"></select></td> \
            </tr> \
        </tbody></table>';

    var item = $(a).parent().parent().data("item");
    var hcName = hc_clicked;

    ShowDiaglog("set_trunk_netplane_dlg", set_trunk_netplane_dlg_html, {
        show: function(){
            $("#port_trunk_dlg").modal("hide");

            $("#set_trunk_netplane_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });

            var h5 = $("#set_trunk_netplane_dlg").find('div.modal-header h5');
            h5.html(h5.text() + " - " + hcName + " - " + item.trunk_name);     

            var rs = xmlrpc_service.tecs.cluster.query_netplane(xmlrpc_session, curr_cluster);        

            if(rs[0] == 0){
                $("#trunk_netplane").empty();
                $("#trunk_netplane").append("<option value=''>");
                for(var i=0; i<rs[1].length; i++) {
                    $("#trunk_netplane").append("<option value='" + rs[1][i] + "'>" + rs[1][i]);
                }
                $("#trunk_netplane").val(item.netplane);
            }
        },
        ok: function() {
            var netplane = $("#trunk_netplane").val();
            var rs = xmlrpc_service.tecs["trunk_netplane"].set(xmlrpc_session, curr_cluster, hcName, item.trunk_name, netplane);
            if(rs[0] != 0) {
                ShowNotyRpcErrorInfo(rs[0]);
                return;
            }
            $("#set_trunk_netplane_dlg").modal("hide");
            $("#port_trunk_dlg").modal("show");

            refresh_host_port();
        },
        cancel: function(){
            $("#set_trunk_netplane_dlg").modal("hide");
            $("#port_trunk_dlg").modal("show");            
        }
    });    
}

function add_turnk_click() {
    if($("#trunk_page").attr("disabled")) return;

    var add_trunk_dlg_html  = 
       '<table id="add_trunk_table"><tbody> \
            <tr> \
                <td class="td_l"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Trunk.name"></OamStaticUI></td> \
                <td class="td_r"><select style="width: 144px;" id="add_trunk_name"></select></td> \
            </tr> \
            <tr> \
                <td class="td_l"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Trunk.type"></OamStaticUI></td> \
                <td class="td_r"><select maptype="TrunkSetType" style="width: 144px;" id="add_select_trunk_type"><!--option value="0"><option value="1"--><option value="4"></select></td> \
            </tr> \
            <tr> \
                <td class="td_l"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Trunk.members"></OamStaticUI></td> \
                <td class="td_r"><select class="OamUi" bindattr="list" multiple="multiple" style="width: 144px;" id="add_trunk_members"></select></td> \
            </tr> \
        </tbody></table>';

    var hcName = hc_clicked;

    ShowDiaglog("add_trunk_dlg", add_trunk_dlg_html, {
        show: function(){
            $("#add_trunk_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });
            $("#add_trunk_dlg").find("option").each(function(){
                var maptype = $(this).parent().attr("maptype");
                if(maptype == null) {
                    return; 
                }
                var mapkey = $(this).val();
                $(this).text(page.res[page.pagelan].type[maptype][mapkey]);
            });        

            var h5 = $("#add_trunk_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-plus"></i>' + h5.text());                

            $("#add_trunk_name").empty();
            var bond_name_html = '<option value="bond0">bond0<option value="bond1">bond1<option value="bond2">bond2<option value="bond3">bond3<option value="bond4">bond4<option value="bond5">bond5<option value="bond6">bond6<option value="bond7">bond7<option value="bond8">bond8<option value="bond9">bond9';
            $("#add_trunk_name").append(bond_name_html);        

            var bond_exists = $("#trunk_list>tbody").find("tr");
            bond_exists.each(function(){
                if(!$(this).hasClass("tmp")){
                    var trunk_name_text = $(this).find("td").eq(0).text();
                    var trunk_name = trunk_name_text.substring(0, 5);
                    var remove_option = "#add_trunk_name option[value='" + trunk_name + "']";
                    $(remove_option).remove();
                }
            });        

            SetOamUiData($("#add_trunk_members"), $("#add_trunk_members"), unbond_ports);

            $("#port_trunk_dlg").modal("hide");
        },
        ok: function(){
            var trunk_name = $("#add_trunk_name").val();
            var trunk_type = parseInt($("#add_select_trunk_type").val());
            var trunk_members = [];
            $("#add_trunk_members").find("option:selected").each(function(){
                trunk_members[trunk_members.length] = $(this).text();
            })

            var rs = xmlrpc_service.tecs["trunk"].add(xmlrpc_session, curr_cluster, hcName, trunk_name, trunk_type, trunk_members);
            if(rs[0] != 0) {
                ShowNotyRpcErrorInfo(rs);
                return;
            }
            $("#add_trunk_dlg").modal("hide");
            $("#port_trunk_dlg").modal("show");
            disable_ui();
        },
        cancel: function(){
            $("#add_trunk_dlg").modal("hide");
            $("#port_trunk_dlg").modal("show");            
        }
    });
}

function use_port() {
    var item = $(this).data("item");

    if(item.used == 0) {
       item.used = 1;
    }
    else {
       item.used = 0;
    }

    var port_param = {};
    port_param.used = item.used;
    port_param.negotiate  = item.negotiate;
    port_param.speed  = item.speed;
    port_param.duplex  = item.duplex;
    port_param.master  = item.master;

    var rs = xmlrpc_service.tecs["port"].set(xmlrpc_session, curr_cluster, hc_clicked, item.name,  port_param);

    if(rs[0] != 0) {
        ShowNotyRpcErrorInfo(rs);
    }

    refresh_host_port();
}

function loop_port(a) {
    var item = $(a).parent().parent().data("item");

    if(item.sriov_num == 0){
        ShowNotyRpcErrorInfo(["web_not_sirov", ""]);
        return;
    }

    if(item.loopback_mode == 1){
       item.loopback_mode = 0;
    }
    else{
       item.loopback_mode = 1;
    }

    var rs = xmlrpc_service.tecs["port_loopback"].set(xmlrpc_session, curr_cluster, hc_clicked, item.name, item.loopback_mode);

    if(rs[0] == 0) {
        refresh_host_port();
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }
}

function del_trunk(a) {
    if($("#trunk_page").attr("disabled")) return;

    var item = $(a).parent().parent().data("item");
    var rs = xmlrpc_service.tecs["trunk"]["delete"](xmlrpc_session, curr_cluster, hc_clicked, item.trunk_name);
    if(rs[0] == 0){
        disable_ui();
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }
}

function enable_ui() {
    $("#trunk_page").attr("disabled", false);
    $("#add_trunk").attr("disabled", false);
    clearInterval(disui_timer_id);

    port_timer_id = setInterval(refresh_host_port, 10000);
}

function disable_ui() {
    $("#trunk_page").attr("disabled", true);
    $("#add_trunk").attr("disabled", true);
    disui_timer_id = setInterval(enable_ui, 1*60000);

    clearInterval(port_timer_id);
}

function extern_ctrl(hc) {
    var extern_ctrl_dlg_html =
       '<table id="extern_ctrl_table"> <tr> \
            <td class="td_l"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="extern_ctrl.state"></OamStaticUI></td> \
            <td class="td_r" ><span type="text" class="OamUi" bindattr="text" bind="state" maptype="Power_State"></span> \
              (<a href="javascript:void(0);" class="OamUi OamButton" bindattr="data" bind="." fn="extern_ctrl_query"> \
                  <OamStaticUI class="OamUi" bindattr="text" maptype="field_name" bindvalue="extern_ctrl.query"></OamStaticUI> \
              </a> \
              |<a href="javascript:void(0);" class="OamUi OamButton" bindattr="data" bind="." fn="extern_ctrl_poweron"> \
                  <OamStaticUI class="OamUi" bindattr="text" maptype="field_name" bindvalue="extern_ctrl.poweron"></OamStaticUI> \
              </a> \
              |<a href="javascript:void(0);" class="OamUi OamButton" bindattr="data" bind="." fn="extern_ctrl_poweroff"> \
                  <OamStaticUI class="OamUi" bindattr="text" maptype="field_name" bindvalue="extern_ctrl.poweroff"></OamStaticUI> \
              </a> \
              <!--|<a href="javascript:void(0);" class="OamUi OamButton" bindattr="data" bind="." fn="extern_ctrl_reboot"> \
                  <OamStaticUI class="OamUi" bindattr="text" maptype="field_name" bindvalue="extern_ctrl.reboot"></OamStaticUI> \
              </a>-->) \
            </td> \
        </tr> </table>';

    hc_clicked = hc.parent().parent().find("td").eq(1).text();

    ShowDiaglog("extern_ctrl_dlg", extern_ctrl_dlg_html, {
        show:function() {
            $("#extern_ctrl_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });            

            var h5 = $("#extern_ctrl_dlg").find('div.modal-header h5');
            h5.html(h5.text() + " - " + hc_clicked);    

            extern_ctrl_query();
        },
        close: function(){
            $("#extern_ctrl_dlg").modal("hide");      
        }
    });     
}

function extern_ctrl_query() {
    var hc_extern_info = [];

    hc_extern_info["state"] = 8;
    hc_extern_info["node_type"] = "";
    hc_extern_info["node_manager"] = "";
    hc_extern_info["node_address"] = "";    
    var rs = xmlrpc_service.tecs["host"].extern_ctrl.query(xmlrpc_session, curr_cluster, hc_clicked, "");
    if(rs[0] == 0) {
        hc_extern_info["state"] = rs[1];
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }

    SetModifyForm($("#extern_ctrl_dlg"), hc_extern_info);
}

function extern_ctrl_poweron() {
    var rs = xmlrpc_service.tecs["host"].extern_ctrl_cmd(xmlrpc_session, curr_cluster, hc_clicked, 2, "");
    if(rs[0] == 0){
        ShowNotyRpcSuccessInfo(rs);
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }
}

function extern_ctrl_poweroff() {
    var rs = xmlrpc_service.tecs["host"].extern_ctrl_cmd(xmlrpc_session, curr_cluster, hc_clicked, 1, "");
    if(rs[0] == 0){
        ShowNotyRpcSuccessInfo(rs);
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }
}

function extern_ctrl_reboot() {
    var rs = xmlrpc_service.tecs["host"].extern_ctrl_cmd(xmlrpc_session, curr_cluster, hc_clicked, 3, "");
    if(rs[0] == 0){
        ShowNotyRpcSuccessInfo(rs);
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }
}

var totalPoints = 300;

function getInitData(){
    var res  = [];
    for (var i = 0; i < totalPoints; i++) {
        res.push([i, 0])
    };
    return res;
}

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

var host_statistics_timer = null

function host_statistics(hc){
    var host_statistics_dlg_html = 
        '<div class="box"> \
            <div class="box-header well" data-original-title> \
                <h2><i class="icon-list-alt"></i> CPU </h2> \
            </div> \
            <div class="box-content"> \
                <div class="box-content"><div id="cpuchart" style="height:190px;"></div></div> \
            </div>  \
        </div> \
        <div class="box"> \
            <div class="box-header well" data-original-title> \
                <h2><i class="icon-list-alt"></i> ' + page.res[page.pagelan].type.field_name["memory"] + ' </h2> \
            </div> \
            <div class="box-content"> \
                <div class="box-content"><div id="ramchart" style="height:190px;"></div></div>  \
            </div> \
        </div> \
        <div class="box"> \
            <div class="box-header well" data-original-title> \
                <h2><i class="icon-list-alt"></i> ' + page.res[page.pagelan].type.display["nic"] + ' </h2> \
            </div> \
            <div class="box-content"> \
                <div class="box-content"><div id="nicchart" style="height:190px;"></div></div> \
            </div> \
        </div>';
   
    ShowDiaglog("host_statistics_dlg", host_statistics_dlg_html, {
        show: function(){  
            var options = {
                yaxis: { min: 0, max: 100 },
                xaxis: { show: false }
            };   

            var hcName = hc.parent().parent().find("td").eq(1).text(); 

            var h5 = $("#host_statistics_dlg").find('div.modal-header h5');
            h5.html(h5.text() + " - " + hcName);                       

            var params = [];
            params.push({"name":"cluster", "value":curr_cluster});        
            params.push({"name":"host",    "value":hcName}); 

            function update() {
                if($("#cpuchart").length)
                {           
                    $.getJSON("../php/host_statistics.php", params, function(info){
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

            $.getJSON("../php/host_statistics.php", params, function(info){
                cpuPlot = $.plot($("#cpuchart"), [getStatisticsData(info.cpu)], options); 
   
                ramPlot = $.plot($("#ramchart"), [getStatisticsData(info.mem)], options);                

                var nicData = [];
                for (x in info.nic) {
                    nicData.push({label: x, data: getStatisticsData(info.nic[x])});
                }; 
                nicPlot = $.plot($("#nicchart"), nicData, options);  

                host_statistics_timer = setInterval(update, 5000);                 
            }); 
        },
        close: function(){
            clearInterval(host_statistics_timer);
            $("#host_statistics_dlg").modal("hide");
        }
    });
}

//--------------- VM -------------------
function init_vm(){
    /*vmTable = $("#vm_list").dataTable({
        "sPaginationType": "bootstrap",        
        "oLanguage": tableLanguage[page.pagelan],
        "alternatively" : "destory"
    });  

    $("#vmRefresh").parent().click(refresh_vm);  */

    oVrVmDtb = $('#vmInfoTb').dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] },
                         { "bSortable": false, "aTargets": [ 3 ] },
                         { "bSortable": false, "aTargets": [ 10 ] },
                         { "bSortable": false, "aTargets": [ 11 ] },
                         { "bSortable": false, "aTargets": [ 12 ] }],
        "alternatively" : "destory",
        "bProcessing": false
        //"bServerSide": true,
        //"sAjaxSource": "../php/vr_vm.php",
        //"fnServerData": RetrieveVmInfoData
    } );

    $('.modal').modal({
        backdrop:"static",
        keyboard:false,
        "show":false
    });

    // OamUi静态显示部分初始化
    $("OamStaticUI").each(function(){
        SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
    }); 
    $("#SelecltAll").click(OnVmInfoSelectAll);
    $("#refresh").click(refresh_vm);
    $("#remove").click(remove_vm);
    $("#cancel_deploy").click(cancel_deploy_vm); 
}

function InitSelectHost(sl, cb){
    sl.empty();
    sl.attr("data-placeholder", page.res[page.pagelan].type.field_name["select_host"]);

    for (var i=0;i<hosts.length;i++) {
        sl.append("<option value='" + hosts[i].host_name + "'>" + hosts[i].host_name);
    }

    sl.chosen();

    if(cb != null){
        sl.unbind('change').bind('change',function(){
            cb();
        });
    }
}

var currVmState = new Object();

function RetrieveVmInfoData( sSource, aoData, fnCallback ){
    //$("#vmInfoTb input[type=checkbox]").attr("checked", false);

    var params = [];
    params.push({"name":"cluster_name", "value":curr_cluster});   

    var oSettings = oVrVmDtb.fnSettings();
    var pageno = oSettings._iDisplayStart / oSettings._iDisplayLength;

    var vmInHc = [];
    $.getJSON("../php/cluster_vm.php", params, function(vminfo){
        oVrVmDtb.fnClearTable()

        if(vminfo[0] != 0){
            ShowNotyRpcErrorInfo(["11", vminfo[1]]);
            return;
        } 

        var nostatevmid = []; 
        var vmInfoData = [];

        var hc_selected = $("#selectHost").val();
        for (var i = 0; i < vminfo[1].length; i++) {
            var oneVm = vminfo[1][i];

            if(hc_selected == null || hc_selected == "" || hc_selected.toString().indexOf(oneVm.host)!=-1){
                var oneVmInfo = [];

                var vid = oneVm["vid"];
                //var chklink = '<input type="checkbox" value=' + '"' + vid + '" >';
                var chklink  = GetItemCheckbox(vid, currVmState);
                oneVmInfo[0] = chklink;

                var morelink = '<a href="javascript:void(0);"  onclick="onMoreVmClick(';
                morelink     += "'" + "idx_detailDlg" + "'," + "'" + vid + "'" + ')">'+ vid +'</a>';
                oneVmInfo[1] = morelink;

                oneVmInfo[2] = oneVm["vm_name"];

                var state = oneVm["state"];                
                if(state == "-1"){
                    oneVmInfo[3] = '<img src="./images/waiting.gif" />';
                    nostatevmid.push(vid);
                }
                else{
                    oneVmInfo[3] = page.res[page.pagelan].type.VmState[parseInt(state)];
                }

                oneVmInfo[4] = oneVm["vcpu"];
                oneVmInfo[5] = oneVm["tcu"];
                oneVmInfo[6] = oneVm["memory"];
                oneVmInfo[7] = oneVm["disk"];
                oneVmInfo[8] = oneVm["ip"];
                oneVmInfo[9] = oneVm["owner"];
                oneVmInfo[10] = oneVm["project"];

                oneVmInfo[11] = oneVm["host"];

                var oplink = ''; 
                oplink += '<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["saveasvt"] +'" onclick="SaveVtByVm('+ "'" + vid + "'" + ')"><i class="icon icon-blue icon-save"></i><span></span></a>';
                if(state == 2){
                    oplink += '&nbsp;|&nbsp;' +'<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["livemigrate"] +'" onclick="LivemigrateVm('+ "'" + vid + "'" + ')"><i class="icon icon-orange icon-arrow-nesw"></i><span></span></a>';   
                }
                else if(state == 4){
                    oplink += '&nbsp;|&nbsp;' +'<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["coldmigrate"] +'" onclick="ColdMigrateVm('+ "'" + vid + "'" + ')"><i class="icon icon-blue icon-arrow-nesw"></i><span></span></a>';   
                }
                oplink += "&nbsp;|&nbsp;" + '<a href="javascript:void(0);" onclick="vm_statistics($(this))" data-rel="tooltip" data-original-title="' + page.res[page.pagelan].type.opr["statistics"] + '"><i class="icon icon-image"></i></a>';             
                oneVmInfo[12] = oplink;    

                vmInfoData.push(oneVmInfo);                    
            }
        };

        oVrVmDtb.fnAddData(vmInfoData);                       

        oVrVmDtb.fnPageChange(parseInt(pageno)); 

        ClearCurSelItem(currVmState);                    

        $("#last_refresh_time").html(GetLastUpdateTime());              

        if(nostatevmid.length > 0){
            QueryVmsCallBack(nostatevmid, ShowVmState);
        }        

        $('.tooltip').hide();            
        $('[rel="tooltip"],[data-rel="tooltip"]').tooltip({"placement":"bottom"});         

        /*aoData.push( { "name": "user_name", "value": ""} );
        aoData.push( { "name": "project_name", "value": ""} );
        aoData.push( { "name": "query_length", "value": "100000"} );    

        $.getJSON( sSource, aoData, function (json) {
            var ackdata = json.data;
            var vmsid = [];
            var nostatevmid = []; 
            if(json.errcode != ""){ 
                ShowNotyQueryDBErrorInfo(json);
            }    

            for(var i = 0;i < ackdata.aaData.length;i++){ 
                vmsid[i] = new I8(ackdata.aaData[i][1]);
            }

            var vmInfoData = [];
            //绑定checkbox
            for(var i = 0;i < ackdata.aaData.length;i++){
                var vid = ackdata.aaData[i][1];
                var chklink = '<input type="checkbox" value=' + '"' + ackdata.aaData[i][1] + '" >';
                var state = ackdata.aaData[i][3];
                ackdata.aaData[i][0] = chklink;
                var morelink = '<a href="javascript:void(0);"  onclick="onMoreVmClick(';
                morelink     += "'" + "idx_detailDlg" + "',";
                morelink     += "'" + ackdata.aaData[i][1] + "'";
                morelink     += ')">'+ackdata.aaData[i][1]+'</a>';
                ackdata.aaData[i][1] = morelink;
                if(ackdata.aaData[i][3] == "-1"){
                    ackdata.aaData[i][3] = '<img src="./images/waiting.gif" />';
                    nostatevmid.push(vmsid[i]);
                }
                else{
                    ackdata.aaData[i][3] = page.res[page.pagelan].type.VmState[parseInt(ackdata.aaData[i][3])];
                }
                var oplink = ''; 
                oplink += '<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["saveasvt"] +'" onclick="SaveVtByVm('+ "'" + vmsid[i] + "'" + ')"><i class="icon icon-blue icon-save"></i><span></span></a>';
                if(state == 2){
                    oplink += '&nbsp;|&nbsp;' +'<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["livemigrate"] +'" onclick="LivemigrateVm('+ "'" + vmsid[i] + "'" + ')"><i class="icon icon-orange icon-arrow-nesw"></i><span></span></a>';   
                }
                else if(state == 4){
                    oplink += '&nbsp;|&nbsp;' +'<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["coldmigrate"] +'" onclick="ColdMigrateVm('+ "'" + vmsid[i] + "'" + ')"><i class="icon icon-blue icon-arrow-nesw"></i><span></span></a>';   
                }
                oplink += "&nbsp;|&nbsp;" + '<a href="javascript:void(0);" onclick="vm_statistics($(this))" data-rel="tooltip" data-original-title="' + page.res[page.pagelan].type.opr["statistics"] + '"><i class="icon icon-image"></i></a>';             
                ackdata.aaData[i][12] = oplink;    

                ackdata.aaData[i][11] = "";
                for (var j = 0; j < vmInHc.length; j++) {
                    if(vmInHc[j].vid == vid){
                        ackdata.aaData[i][11] = vmInHc[j].host;
                        vmInfoData.push(ackdata.aaData[i]);                    
                        break;
                    }
                };
            }     
         
            ackdata.aaData = vmInfoData;
            ackdata.iTotalRecords = vmInfoData.length;
            ackdata.iTotalDisplayRecords = vmInfoData.length;    

            debugger;

            fnCallback(ackdata);                

            $("#last_refresh_time").html(GetLastUpdateTime());      

            if(nostatevmid.length > 0){
                QueryVmsCallBack(nostatevmid,ShowVmState);
            }            

            $('.tooltip').hide();            
            $('[rel="tooltip"],[data-rel="tooltip"]').tooltip({"placement":"bottom"}); 
        });*/

    });
}

function refresh_vm(){
    /*var params = [];
    params.push({"name":"cluster_name", "value":curr_cluster});

    $.getJSON("../php/cluster_vm.php", params, function(json){
        vmTable.fnClearTable();

        if(json[0] != 0){
            alert(json[1]);
            return;
        }

        var vmData = [];
        for (var i = 0; i < json[1].length; i++) {
            var oneVm = [];
            oneVm[0] = json[1][i].vid;
            oneVm[1] = json[1][i].vm_name;
            oneVm[2] = json[1][i].create_time;
            oneVm[3] = json[1][i].owner;
            oneVm[4] = json[1][i].project;  
            oneVm[5] = json[1][i].host; 

            vmData.push(oneVm);  
        };

        vmTable.fnAddData(vmData);
    });*/
    RecordSelItem(oVrVmDtb, "vmInfoTb", currVmState);
    RetrieveVmInfoData();
}

function remove_vm(){
    ShowBatchOpConfirmDiag($("#idx_confirmDlg"), "vmInfoTb", page.res[page.pagelan].type.err_info["delete_confirm_info"], RemoveVmCallback);
} 

function cancel_deploy_vm(){
    ShowBatchOpConfirmDiag($("#idx_confirmDlg"), "vmInfoTb", page.res[page.pagelan].type.err_info["cancel_deploy_confirm_info"], Cancel_DeployVm_Callback);
}

function OnVmInfoSelectAll(){
    if($(this).attr("checked") == "checked"){
        $("#vmInfoTb tbody input[type=checkbox]").attr("checked", true);
    }
    else{
        $("#vmInfoTb tbody input[type=checkbox]").attr("checked", false);
    }
}

//--------------- TCU -------------------
function init_tcu(){
    tcuTable = $("#tcu_list").dataTable({
        "sPaginationType": "bootstrap",        
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] }, 
                         { "bSortable": false, "aTargets": [ 5 ] },
                         { "bSortable": false, "aTargets": [ 6 ] } ],
        "alternatively" : "destory"
    });  

    $("#tcuRefresh").parent().click(refresh_tcu);
}

function refresh_tcu(){
    tcuTable.fnClearTable();

    var tcus = TecsObjList("tcu", 1, curr_cluster, "", new I8(-1));
    var tcuData = [];

    if(tcus == null){
        return;
    }

    for(var i=0; i<tcus.length; i++){
        var cpu_xmlinfo = tcus[i].cpu_info;
        var model_begin = cpu_xmlinfo.indexOf("<model_name>");
        var model_end = cpu_xmlinfo.indexOf("</model_name>");
        var model_name = cpu_xmlinfo.substring(model_begin+12, model_end);
        tcus[i].model_name = model_name; 

        var core_num_begin = cpu_xmlinfo.indexOf("<processor_num>");
        var core_num_end = cpu_xmlinfo.indexOf("</processor_num>");
        var core_num = cpu_xmlinfo.substring(core_num_begin+15, core_num_end);
        tcus[i].core_num = core_num; 

        if(tcus[i].tcu_num == 0) tcus[i].tcu_num = tcus[i].commend_tcu_num;

        var onetcu = [];
        onetcu[0] = tcus[i].cpu_info_id;
        onetcu[1] = tcus[i].model_name;
        onetcu[2] = tcus[i].core_num;
        onetcu[3] = tcus[i].commend_tcu_num;
        onetcu[4] = tcus[i].tcu_num;
        onetcu[5] = tcus[i].description; 
        onetcu[6] = '<a href="javascript:void(0)" onclick="set_tcu(this)" data-rel="tooltip" data-original-title="' 
                    + page.res[page.pagelan]["type"]["opr"]["setting"] + '"><i class="icon-edit"></i></a>';
 
        tcuData[i] = onetcu;                                       
    }    

    tcuTable.fnAddData(tcuData);

    $('.tooltip').hide();
    $('[data-rel="tooltip"]').tooltip({"placement":"bottom"});        

    $("#last_refresh_time").html(GetLastUpdateTime());  

    set_node_data(tcuTable, tcus);   
}

function set_node_data(table, data){
    var trlist = table.fnGetNodes();
    for (var i = 0; i < trlist.length; i++) {
        $(trlist[i]).data("item", data[i]);
    };
}

function set_tcu(a) {
    var set_tcu_dlg_html =
       '<form class="form-horizontal"> <fieldset> \
            <div class="control-group"> \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="tcu.num"/></label> \
                <div class="controls"><input type="text" id="tcu_num" style="width:144px;" maxlength="6"/></div> \
            </div> \
            <div class="control-group"> \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="tcu.desc"></OamStaticUI></label> \
                <div class="controls"><textarea id="tcu_desc" maxlength="128" style="height:100px;"></textarea><p id="tcu_desc_tip"></p></div> \
            </div> \
            <div class="control-group"> \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="tcu.cpu_info"></OamStaticUI></label> \
                <div class="controls"><label id="tcu_cpu_info" style="width:144px;"></label></div> \
            </div> \
        </fieldset> </form>';

    var rowitem = $(a).parent().parent().data("item");

    ShowDiaglog("set_tcu_dlg", set_tcu_dlg_html, {
        show:function(){
            $("#set_tcu_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });

            var h5 = $("#set_tcu_dlg").find('div.modal-header h5');
            h5.html("<i class='icon-edit'></i>" + h5.text() + " - " + rowitem.cpu_info_id);    

            LimitCharLenInput($("#tcu_desc"), $("#tcu_desc_tip"));

            $("#tcu_num").val(rowitem.tcu_num);
            $("#tcu_desc").val(rowitem.description);
            $("#tcu_cpu_info").text(rowitem.cpu_info);
            
            $(".control-group").removeClass("error");
        },
        ok:function(){
            var tcu_num = $("#tcu_num").val();
            var tcu_desc = $("#tcu_desc").val();

            if(rowitem.tcu_num == tcu_num && rowitem["description"] == tcu_desc){
                $("#set_tcu_dlg").modal("hide");
                return;
            }

            $(".control-group").removeClass("error");

            if(isNaN(tcu_num) || Number(tcu_num) > 128 || Number(tcu_num) < 1){
                $("#tcu_num").parent().parent().addClass("error");
                ShowNotyRpcErrorInfo(["tcu_range_tip", ""]);
                return;
            }

            if(tcu_desc.length > 128){
                ShowNotyRpcErrorInfo(["desc_too_long", ""]);
                return;
            }

            rs = xmlrpc_service.tecs.tcu.set(xmlrpc_session, curr_cluster, 
                     new I8(rowitem.cpu_info_id), Number(tcu_num), tcu_desc);

            if(rs[0] == 0) {
                refresh_tcu();
            }
            else {
                ShowNotyRpcErrorInfo(rs);
            }
            $("#set_tcu_dlg").modal("hide");
        }
    });
}

//--------------- sa -------------------
function init_sa() {
    storageTable = $("#sa_list").dataTable({
        "sPaginationType": "bootstrap",          
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] },
                         { "bSortable": false, "aTargets": [ 6 ] } ],
        "alternatively" : "destory"
    });

    $("#saSelectAll").click(sa_select_all_click);
    $("#saRefresh").parent().click(refresh_sa);
    $("#saAdd").parent().click(add_sa);
    $("#saRemove").parent().click(remove_sa);   
}

function sa_state(a) {
    var ret = "";
    var map_info = page.res[page.pagelan]["type"]["ClusterRunState"];    

    if(a["is_online"] == "0"){
        ret += '<img src="img/hc_offline.png" data-rel="tooltip" data-original-title="'
               + map_info[0] + '"/>';
    }
    else {
        ret += '<img src="img/hc_ok.png" data-rel="tooltip" data-original-title="'
               + map_info[1] + '"/>';
    }

    if(a["enabled"] != "1"){
        ret += '<img src="img/hc_mantaince.png" data-rel="tooltip" data-original-title="'
               +  map_info[2]  + '"/>';
    }

    return ret;
}

function is_contains(arr, val){
    for (var i = 0; i < arr.length; i++) {
        if(arr[i] == val){
            return true;
        }
    }

    return false;
}

function refresh_sa() {
    storageTable.fnClearTable();

    $.getJSON("../php/storage.php", function(json){
        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["11", json[1]]);
            return;
        }
        var sa = json[1];
        var saData = [];
        for(var i=0; i<sa.length; i++){
            var oneSa = []
            oneSa[0] = "<INPUT type='Checkbox'/>";
            oneSa[1] = sa[i]["sid"];
            oneSa[2] = "<a href='#' onclick='goto_storage_sa(this)'>" + sa[i]["name"] + " - " + sa[i]["sa_name"] + "</a>";
            oneSa[3] = sa_state(sa[i]);
            oneSa[4] = sa[i]["type"];
            oneSa[5] = sa[i]["ctrl_address"];
            oneSa[6] = sa[i]["description"];

            if(is_contains(sa[i]["clusters"], curr_cluster)){
                saData[saData.length] = oneSa;                
            }
        }

        storageTable.fnAddData(saData);  

        $('.tooltip').hide();
        $('[data-rel="tooltip"]').tooltip({"placement":"bottom"});    

        $("#last_refresh_time").html(GetLastUpdateTime());       
    });
}

function add_sa(){
    var add_sa_dig_html =
    '<form class="form-horizontal"> <filedset> \
        <div class="control-group"> \
            <Label class="control-label"><span id="saId"></span><span><font color="red">*</font></span></Label> \
            <div class="controls"><select id="upmaped_sa"></select></div> \
        </div> \
    </filedset> </form>';

    ShowDiaglog("add_sa_dlg", add_sa_dig_html, {
        show:function(){
            var h5 = $("#add_sa_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-plus"></i>' + h5.text());  

            $.getJSON("../php/storage.php", function(json){
                if(json[0] != 0){
                    ShowNotyRpcErrorInfo(["11", json[1]]);
                    return;
                }
                var sa = json[1];
                $("#upmaped_sa").empty();
                for(var i=0; i<sa.length; i++){
                    if(!is_contains(sa[i]["clusters"], curr_cluster)){
                        $("#upmaped_sa").append("<option value="+ sa[i]["sid"]+">" + sa[i]["name"] + " - " + sa[i]["sa_name"]);              
                    }
                }
            });
        },
        ok:function(){
            var sid = $("#upmaped_sa").val();
            
            if(sid == null){
                ShowNotyRpcErrorInfo(["invalid_param", ""]);
                return;
            }

            var rs = xmlrpc_service.tecs.sa.clustermap(xmlrpc_session, Number(sid), curr_cluster, true);
            if(rs[0] == 0){
                refresh_sa();

                $("#add_sa_dlg").modal("hide");                
            }
            else {
                ShowNotyRpcErrorInfo(rs[0]);
            }
        }
    })
}

function remove_sa(){
    var pageVal = page.res[page.pagelan]["xid"]["val"];
    var vCheckedbox = $(storageTable.fnGetNodes()).find(":checkbox:checked");

    var vresultSucceeded = [];
    var vresultFailed    = [];

    if(vCheckedbox.size() > 0){
        ShowDiaglog("remove_sa_dlg", pageVal["remove_sa_confirm"], {
            show:function(){
                var h5 = $("#remove_sa_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-trash"></i>' + h5.text()); 
            },            
            ok:function(){
                $("#remove_sa_dlg").modal('hide');    

                vCheckedbox.each(function(){
                    var tr = $(this).parent().parent();      

                    var saId = tr.find("td").eq(1).text();

                    var rs =  xmlrpc_service.tecs.sa.clustermap(xmlrpc_session, Number(saId), curr_cluster, false);

                    ShowSynRpcInfo(rs);
                });    

                refresh_sa();
            }
        });
    }
    else{
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]);
    }
}

function sa_select_all_click() {
    if($(this).attr("checked") == "checked") {
        $("#sa_list tbody input[type=checkbox]").attr("checked", true);
    }
    else {
        $("#sa_list tbody input[type=checkbox]").attr("checked", false);
    }
}

function goto_storage_sa(sa){
    var sid = $(sa).parent().parent().find("td").eq(1).text();     

    Switch_Page("nav", $("#storage").parent(), $('#content'), "storage.html?sid="+sid);
}

//--------------- energy -------------------
function init_energy(){
	//查询当前配置
	energy_query_func();
	z_numInput("host_percent_input", 0, 1000);	
    z_numInput("host_check_interval", 0, 43200);
	
    $("#strategy_config").unbind("click").bind("click", modify_strategy_config_click);		
}


function refresh_energy(){
    init_energy();
}


function energy_query_func() {	
    xmlrpc_service.tecs.save_energy.query(xmlrpc_session, curr_cluster, energy_query_callback);
}

function energy_query_callback(rs, ex) {
    if(rs[0] == 0){	
	    //获取使能状态
	    if(0 ==  rs[1]){
		    $("#energy_toggle").attr("checked",false);

            $("#max_input").attr("disabled", true);
            $("#min_input").attr("disabled", true);
            $("#host_percent_input").attr("disabled", true);    
            $("#host_check_interval").attr("disabled", true);            
		}else {
	
		    $("#energy_toggle").attr("checked",true);
		}   
		
		$("#max_input").val(rs[2]);
		$("#min_input").val(rs[3]);
		$("#host_percent_input").val(rs[4]);	
        $("#host_check_interval").val(rs[5]);

        $("#energy_toggle").iphoneStyle({
            checkedLabel: userstatus[1][page.pagelan],
            uncheckedLabel: userstatus[2][page.pagelan],
	    	onChange:onoff_strategy
        }).data("iphoneStyle").refresh();
				
        $("#last_refresh_time").html(GetLastUpdateTime());             
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }
}


function onoff_strategy(){
    //启用或者禁用节能
    if($("#energy_toggle").attr("checked") != "checked"){
	    $("#energy_toggle").attr("checked",false);

        $("#max_input").val(0);
        $("#min_input").val(0);
        $("#host_percent_input").val(0);    
        $("#host_check_interval").val(0);

        $("#max_input").attr("disabled", true);
        $("#min_input").attr("disabled", true);
        $("#host_percent_input").attr("disabled", true);    
        $("#host_check_interval").attr("disabled", true);

	}else {
	    $("#energy_toggle").attr("checked",true);

        $("#max_input").attr("disabled", false);
        $("#min_input").attr("disabled", false);
        $("#host_percent_input").attr("disabled", false);    
        $("#host_check_interval").attr("disabled", false);     
	}
}

function modify_strategy_config_click() {

    var item = GetModifyForm($("#ccStrategy").find(".form-horizontal"));
    $("#ccStrategy").find(".control-group").removeClass("error");
    //空校验
    if(item.max == "" || isNaN(item.max)){
       $("#ccStrategy").find(".max").addClass("error");		
        return false;
    }
    if(item.min == "" || isNaN(item.min)){
        $("#ccStrategy").find(".min").addClass("error");		
        return false;
    }	
    if(item.percent == ""|| isNaN(item.percent)){
       $("#ccStrategy").find(".percent").addClass("error");		
        return false;
    }	
    if(item.interval == ""|| isNaN(item.interval)){
       $("#ccStrategy").find(".interval").addClass("error");     
        return false;
    }   
	
	if(parseInt(item.max) < parseInt(item.min)){
        $("#ccStrategy").find(".max").addClass("error");		
        $("#ccStrategy").find(".min").addClass("error");	
        return false;		
	}
	
	var save_energy = 0;
    if($("#energy_toggle").attr("checked") == "checked"){
        save_energy = 1;
    } else {
        save_energy = 0;
    }  

    if(save_energy == 1 && item.interval < 5){
       $("#ccStrategy").find(".interval").addClass("error");  
       return false;        
    }

    var rs = xmlrpc_service.tecs.save_energy.set(xmlrpc_session, curr_cluster,save_energy,parseInt(item.max),parseInt(item.min),parseInt(item.percent), parseInt(item.interval));
    	
	if (rs[0] == 0) {
        ShowNotyRpcSuccessInfo([0, ""]);
        energy_query_func();
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }
}

//-----------------------------schedule policy-------------------------
function refresh_schedulepolicy(){
    $("#schedulepolicy_config").unbind("click").bind("click", modify_schedulepolicy_click); 

    $(".control-group").removeClass("error");

    $("#slSchedulePolicy").empty();
    $("#slSchedulePolicy").append('<option value="none">' + page.res[page.pagelan].type.SchedulePolicy["none"]);
    $("#slSchedulePolicy").append('<option value="packing">' + page.res[page.pagelan].type.SchedulePolicy["packing"]);
    $("#slSchedulePolicy").append('<option value="striping">' + page.res[page.pagelan].type.SchedulePolicy["striping"]);

    $("#slSchedulePolicy").unbind("change").bind("change", function(){  
        if($("#slSchedulePolicy").val() == "none"){
            $("#inputCycleTime").val(0);
            $("#inputInfluenceVMs").val(0); 

            $("#inputCycleTime").attr("disabled", true);
            $("#inputInfluenceVMs").attr("disabled", true);
        }   
        else{
            $("#inputCycleTime").attr("disabled", false);
            $("#inputInfluenceVMs").attr("disabled", false);            
        }    
    });

    xmlrpc_service.tecs.scheduler.query_policy(xmlrpc_session, curr_cluster, query_schedulepolicy_callback);
}

function query_schedulepolicy_callback(rs, ex){
    if(rs[0] == 0){
        $("#slSchedulePolicy").val(rs[1]);
        $("#inputCycleTime").val(rs[2]/60);
        $("#inputInfluenceVMs").val(rs[3]);

        $("#slSchedulePolicy").trigger("change");
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }
}

function modify_schedulepolicy_click(){
    var sp = $("#slSchedulePolicy").val();
    var ct = $("#inputCycleTime").val();
    var iv = $("#inputInfluenceVMs").val();

    $(".control-group").removeClass("error");
    if(sp == null){
        $("#slSchedulePolicy").parent().parent().addClass("error");
        return;
    }

    if(sp != "none"){
        if(ct == "" || isNaN(ct) || Number(ct) < 5 || Number(ct) > 1500){
            $("#inputCycleTime").parent().parent().addClass("error");
            return;
        }
    }
    else {
        if(ct == ""){
            $("#inputCycleTime").parent().parent().addClass("error");
            return;
        }        
    }

    if(iv == "" || isNaN(iv)){
        $("#inputInfluenceVMs").parent().parent().addClass("error");
        return;
    }     

    var rs = xmlrpc_service.tecs.scheduler.set_policy(xmlrpc_session, curr_cluster, sp, parseInt(ct)*60, parseInt(iv));
    if(rs[0] == 0){
        ShowNotyRpcSuccessInfo(rs);
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }    
}

//-----------------------------   hc policy   -------------------------
function refresh_hapolicy(){
    $("#hapolicy_config").unbind("click").bind("click", modify_hapolicy_click);  

    $(".control-group").removeClass("error");

    $("#slHostDown").empty();
    $("#slHostDown").append('<option value="none">' + page.res[page.pagelan].type.HaPolicy["none"]);
    $("#slHostDown").append('<option value="migrate">' + page.res[page.pagelan].type.HaPolicy["migrate"]);

    $("#slHostFault").empty();
    $("#slHostFault").append('<option value="none">' + page.res[page.pagelan].type.HaPolicy["none"]);
    $("#slHostFault").append('<option value="migrate">' + page.res[page.pagelan].type.HaPolicy["migrate"]);

    $("#slHostStorageFault").empty();
    $("#slHostStorageFault").append('<option value="none">' + page.res[page.pagelan].type.HaPolicy["none"]);
    $("#slHostStorageFault").append('<option value="migrate">' + page.res[page.pagelan].type.HaPolicy["migrate"]);    

    xmlrpc_service.tecs.vm_ha.query_policy(xmlrpc_session, curr_cluster, query_hapolicy_callback);
}

function query_hapolicy_callback(rs, ex){
    if(rs[0] == 0){
        $("#slHostDown").val(rs[2]);
        $("#slHostFault").val(rs[3]);
        $("#slHostStorageFault").val(rs[5]);
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }
}

function modify_hapolicy_click(){
    var hd = $("#slHostDown").val();
    var hf = $("#slHostFault").val();
    var hsf = $("#slHostStorageFault").val();

    $(".control-group").removeClass("error");
    if(hd == null){
        $("#slHostDown").parent().parent().addClass("error");
        return;
    }

    if(hf == null){
        $("#slHostFault").parent().parent().addClass("error");
        return;
    }

    if(hsf == null){
        $("#slHostStorageFault").parent().parent().addClass("error");
        return;
    }        

    var rs = xmlrpc_service.tecs.vm_ha.set_policy(xmlrpc_session, curr_cluster, "", hd, hf, "", hsf);
    if(rs[0] == 0){
        ShowNotyRpcSuccessInfo(rs);
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }
}

