/**
* page begin
*/
var page = {
    pageid: "storage",
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
    },
    eixt: function(){
    }
}; 

$(function(){
    page.init();
    var tc_html = '<li><a href="#" onclick="tc_click()" style="word-wrap:break-word;word-break:break-all"> \
                       <span><img src="img/tc.png"></img></span><span>TecsCloud</span> \
                   </a></li>';
    $("#sa_ul").append(tc_html);

    var sid_clicked = getUrlArgs(suburl).sid;

    $.getJSON("../php/storage.php",function(json){
        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["12",json[1]]);
            return;
        }

        for(var i=0; i<json[1].length; i++){
            var cc_html =  '<li><a href="#" onclick="sa_click($(this))" style="word-wrap:break-word;word-break:break-all">'
                cc_html += '<span><img src="img/storage16.png"></img></span><span>';
                cc_html += json[1][i].name + ' - ' + json[1][i].sa_name;
                cc_html += '</span></a></li>';
            $("#sa_ul").append(cc_html);
            $("#sa_ul li a:last").data("item", json[1][i]);
        }
         
        if(sid_clicked == null){
            $('ul.main-menu li a').eq(0).trigger("click");
        }
        else {
            $("ul.main-menu li a").each(function(){
                var item = $(this).data("item");
                if(item != null && item.sid == sid_clicked){
                    $(this).trigger("click");
                }
            });
        }

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
* All sa overview in table
*/
var salist_html =
   '<div style="display:none;"><ul class="breadcrumb"></ul></div> \
    <ul class="nav nav-tabs"><li class="active"><a href="#salistSummary" id="salistSummaryNav"></a></li></ul> \
    <div class="tab-content"><div class="tab-pane active" id="salistSummary"> \
        <div align="right"> \
            <button class="btn"><i class="icon-wrench"></i><span id="saDisable"></span></button> \
            <button class="btn"><i class="icon-play"></i><span id="saEnable"></span></button> \
            <button class="btn"><i class="icon-trash"></i><span id="saForget"></span></button> \
            <button class="btn"><i class="icon-refresh"></i><span id="saRefresh"></span></button> \
        </div> \
        <table class="table table-striped table-bordered table-condensed" id="sa_list"> \
            <thead> <tr> \
                <th width="25px;" align="left"><INPUT id="saSelectAll" type="checkbox" /></th> \
                <th width="50px;" align="left"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="sa.sid"></OamStaticUI></th> \
                <th width="150px;" align="left"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="sa.name"></OamStaticUI></th> \
                <!--th width="100px;" align="left"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="sa.sa_name"></OamStaticUI></th--> \
                <th width="75px;" align="left"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="sa.state"></OamStaticUI></th> \
                <th width="100px;" align="left"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="sa.type"></OamStaticUI></th> \
                <th width="125px;" align="left"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="sa.ctrl_addr"></OamStaticUI></th> \
                <th width="175px;" align="left"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="sa.cluster"></OamStaticUI></th> \
                <th width="100px;" align="left"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="sa.desc"></OamStaticUI></th> \
            </tr> </thead> \
            <tbody style="word-wrap:break-word;word-break:break-all"> </tbody> \
        </table> \
        <div id="salist_last_refresh_time"></div> \
    </div></div>';

function tc_click() {
    $('ul.main-menu li').removeClass("active");
    $('ul.main-menu li').eq(0).addClass("active");

    $("#right_content").html(salist_html);  
    c_changeLangue(page);  

    $("#sa_list").find("OamStaticUI").each(function(){
        SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
    });          

    saTable = $("#sa_list").dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] },
                         { "bSortable": false, "aTargets": [ 3 ] },
                         { "bSortable": false, "aTargets": [ 7 ] } ],
        "alternatively" : "destory"
    });    

    $("#saSelectAll").click(sa_select_all_click);
    $("#saRefresh").parent().click(refresh_sa_info);
    $("#saDisable").parent().click(disable_sa);
    $("#saEnable").parent().click(enable_sa);
    $("#saForget").parent().click(forget_sa);

    refresh_sa_info();

    clear_all_timer();
    curr_timer_id = setInterval(refresh_sa_info, 30*1000);
}

function sa_select_all_click() {
    if($(this).attr("checked") == "checked") {
        $("#sa_list tbody input[type=checkbox]").attr("checked", true);
    }
    else {
        $("#sa_list tbody input[type=checkbox]").attr("checked", false);
    }
}

var currSaState = new Object();

function refresh_sa_info() {
    RecordSelItem(saTable, "sa_list", currSaState);    

    $.getJSON("../php/storage.php", function(json){
        saTable.fnClearTable();

        if(json[0] != 0){
            alert(json[1]);
        }

        var saData = [];

        for(var i=0; i<json[1].length; i++){
            var oneSa = []
            oneSa[0] = GetItemCheckbox(json[1][i]["sid"], currSaState);//"<INPUT type='Checkbox'/>";
            oneSa[1] = json[1][i]["sid"];
            oneSa[2] = "<a href='#' onclick='goto_sa(this)'>" + json[1][i]["name"] + " - " + json[1][i]["sa_name"] + "</a>";
            oneSa[3] = sa_state(json[1][i]);
            oneSa[4] = json[1][i]["type"];
            if(oneSa[4] == "IPSAN"){
                oneSa[5] = '<a href="#" onclick=window.open("http://' + json[1][i]["ctrl_address"] + '")>' + json[1][i]["ctrl_address"] + '</a>';
            }
            else {
                oneSa[5] = json[1][i]["ctrl_address"];
            }
            oneSa[6] = json[1][i]["clusters"] + "(<a href='javascript:void(0)' onclick='add_cluster_to_sa(this)'>"
                            + page.res[page.pagelan]["type"]["opr"]["setting"] + "</a>)";
            oneSa[7] = json[1][i]["description"];
            saData.push(oneSa);
        }    

        saTable.fnAddData(saData);   

        ClearCurSelItem(currSaState);

        $(".tooltip").hide();
        $('[data-rel="tooltip"]').tooltip({"placement":"bottom"});

        $("#salist_last_refresh_time").html(GetLastUpdateTime());         
    });
}

function sa_state(a) {
    var ret = "";
    var map_info = page.res[page.pagelan]["type"]["SaRunState"];    

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

function enable_sa() {
    ClearAlertInfo();    
    var vCheckedbox = $("#sa_list>tbody").find(":checkbox:checked");
    if(vCheckedbox.size() == 0) {
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]);
        return;
    }

    vCheckedbox.each(function() {
        var sid = $(this).parent().parent().find("td").eq(1).text();

        var result = xmlrpc_service.tecs.sa.enable(xmlrpc_session, Number(sid));

        ShowSynRpcInfo(result);
    });

    refresh_sa_info();
}

function disable_sa() {
    ClearAlertInfo();    
    var vCheckedbox = $("#sa_list>tbody").find(":checkbox:checked");
    if(vCheckedbox.size() == 0) {
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]);
        return;
    }

    ShowDiaglog("disable_sa_dlg", page.res[page.pagelan]["xid"]["val"]["disable_sa_confirm"], {
        show:function(){
            var h5 = $("#disable_sa_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-wrench"></i>' + h5.text());                    
        },
        ok:function(){   
            $("#disable_sa_dlg").modal("hide");

            vCheckedbox.each(function(){
                var sid = $(this).parent().parent().find("td").eq(1).text();        

                var result = xmlrpc_service.tecs.sa.disable(xmlrpc_session, Number(sid));        

                ShowSynRpcInfo(result);        
            });        

            refresh_sa_info();
        }
    });
}

function forget_sa() {
    ClearAlertInfo();    
    var vCheckedbox = $("#sa_list>tbody").find(":checkbox:checked");

    if(vCheckedbox.size() == 0) {
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]);
        return;
    }

    ShowDiaglog("forget_sa_dlg", page.res[page.pagelan]["xid"]["val"]["forget_sa_confirm"], {
        show:function(){
            var h5 = $("#forget_sa_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-trash"></i>' + h5.text());                    
        },
        ok:function(){  
            $("#forget_sa_dlg").modal("hide");

            vCheckedbox.each(function(){
                var sid = $(this).parent().parent().find("td").eq(1).text();        

                var result = xmlrpc_service.tecs.sa.forgetbyid(xmlrpc_session, Number(sid));

                ShowSynRpcInfo(result); 
            });
       
            $('ul.mainnav li.active a').eq(0).click();
        }
    });
}

function goto_sa(sa){
   var sid = $(sa).parent().parent().find("td").eq(1).text();     

    $("#sa_ul").find("a").each(function(){
        var sainfo = $(this).data("item");
        if(sainfo != null && sainfo.sid == sid){
            $(this).trigger("click");
            return;
        }
    });  
}

function add_cluster_to_sa(a) {
    var add_cluster_to_sa_dlg_html = 
    '<table id="add_cluster_to_sa_table"> \
        <tr> \
            <td width="220px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="sa.unmaped"/></td>  \
            <td></td>  \
            <td width="220px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="sa.maped"/></td>   \
        </tr>   \
        <tr> \
            <td>  \
                <select id="sa_unmaped_cluster" multiple="multiple" style="height: 180px;"></select> \
            </td> \
            <td style="width:0px;padding:20px;">  \
                <a class="btn" onclick="sa_map_cluster()"> > </a> \
                <a class="btn" onclick="sa_demap_cluster()"> < </a> \
            </td> \
            <td> \
                <select id="sa_maped_cluster" multiple="multiple" style="height: 180px;"></select> \
            </td> \
        </tr>   \
    </table>';

    ShowDiaglog("add_cluster_to_sa_dlg", add_cluster_to_sa_dlg_html, {
        show:function(){
            $("#add_cluster_to_sa_table").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });       
            
            sid = $(a).parent().parent().find("td").eq(1).text();

            var h5 = $("#add_cluster_to_sa_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-hdd"></i>' + h5.text() + " - " + sid);            

            refresh_sa_cluster(sid);
        },
        close:function(){
            $("#add_cluster_to_sa_dlg").modal("hide");
            refresh_sa_info();
        }
    });
}

function sa_map_cluster() {
    $("#sa_unmaped_cluster").find("option:selected").each(function() {   
        var rs = xmlrpc_service.tecs.sa.clustermap(xmlrpc_session, Number(sid), $(this).val(), true);
        if(rs[0] == 0){
            refresh_sa_cluster(sid);
        }
        else {
            ShowNotyRpcErrorInfo(rs[0]);
        }
    });
}

function sa_demap_cluster() {
    $("#sa_maped_cluster").find("option:selected").each(function() {   
        var rs = xmlrpc_service.tecs.sa.clustermap(xmlrpc_session, Number(sid), $(this).val(), false);
        if(rs[0] == 0){
            refresh_sa_cluster(sid);
        }
        else {
            ShowNotyRpcErrorInfo(rs[0]);
        }  
    });      
}

function refresh_sa_cluster(sid) {
    var param = [];
    param.push({ "name": "sid",  "value": sid});

    $.getJSON("../php/storage_sa_cc.php", param, function (json){  
        $("#sa_maped_cluster").empty();
        $("#sa_unmaped_cluster").empty();

        for(var i=0; i<json.length; i++){
            if(json[i]["maped"] == "1"){
                $("#sa_maped_cluster").append("<option value="+ json[i].name +">" + json[i].name);
            }
            else {
                $("#sa_unmaped_cluster").append("<option value="+ json[i].name +">" + json[i].name);
            }
        }        
    });  
}


/**
*   One sa detail information.
*/

var sa_detail_html =
   '<div style="display:none;"><ul class="breadcrumb"></ul></div> \
    <div> \
        <ul class="nav nav-tabs" id="ccSaTab"> \
            <li class="active"><a href="#saSummary" id="saSummaryNav"></a></li> \
            <li><a href="#saVg" id="saVgNav"></a></li> \
            <li><a href="#saVolume" id="saVolumeNav"></a></li> \
        </ul> \
        <div class="tab-content"> \
            <div id="saSummary" class="tab-pane active"> \
                <div class="row-fluid"> \
                    <div class="box span4"> \
                        <div class="box-header well"><h3 id="saUseage"></h3></div> \
                        <div class="box-content"><div id="saUsePie" style="height:160px;"></div> \
                        <div style="margin-left:75px;"></div></div> \
                    </div> \
                    <div class="box span4"> \
                        <div class="box-header well"><h3 id="saInfo"></h3></div> \
                        <div class="box-content" style="height:200px;"> \
                            <div><strong><OamStaticUI bindattr="text" maptype="field_name" bindvalue="sa.state"></OamStaticUI></strong>:</div> \
                            <div style="margin-left:20px;"> \
                                <input data-no-uniform="true" type="checkbox" class="iphone-toggle" id="sa_enable_toggle">  \
                            </div> \
                            <div><strong><OamStaticUI bindattr="text" maptype="field_name" bindvalue="sa.online"></OamStaticUI></strong>:</div> \
                            <div style="margin-left:20px;"> \
                                <span class="OamUi ccdata" bindattr="text" bind="is_online" maptype="SaOnline"></span> \
                            </div> \
                            <div><strong><OamStaticUI bindattr="text" maptype="field_name" bindvalue="sa.register_time"></OamStaticUI></strong>:</div> \
                            <div style="margin-left:20px;"> \
                                <span class="OamUi ccdata" bindattr="text" bind="register_time"></span> \
                            </div> \
                            <div><strong><OamStaticUI bindattr="text" maptype="field_name" bindvalue="sa.ctrl_addr"></OamStaticUI></strong>:</div> \
                            <div style="margin-left:20px;"> \
                                <span class="OamUi ccdata" bindattr="text" bind="ctrl_address"></span> \
                            </div> \
                        </div> \
                    </div> \
                    <div class="box span4"> \
                        <div class="box-header well"><h3 id="saDesc"></h3></div> \
                        <div class="box-content" style="height:200px;"> \
                            <div style="margin-left:20px;"> \
                                <span class="OamUi ccdata" bindattr="text" bind="description"></span> \
                            </div> \
                        </div> \
                    </div> \
                </div> \
            </div> \
            <div id="saVg" class="tab-pane"> \
                <div align="right"> \
                    <button class="btn"><i class="icon-refresh"></i><span id="vgRefresh"></span></button> \
                </div> \
                <table id="vg_list" class="table table-striped table-bordered table-condensed"> \
                    <thead> \
                        <tr> \
                            <th width="125px;" align="left"><span id="vg_t1"></span></th> \
                            <th width="75px;" align="left"><span id="vg_t2"></span></th> \
                            <th width="75px;" align="left"><span id="vg_t3"></span></th> \
                            <th width="75px;" align="left"><span id="vg_t4"></span></th> \
                        </tr> \
                    </thead> \
                    <tbody style="word-wrap:break-word;word-break:break-all;"></tbody> \
                </table> \
            </div> \
            <div id="saVolume" class="tab-pane"> \
                <div class="span8"> \
                    <div class="row-fluid" style="table-layout:fixed;word-break:break-all">  \
                        <div class="span4">  \
                            <select data-placeholder="" id="selectVg" multiple data-rel="chosen"> </select> \
                        </div> \
                    </div> \
                </div> \
                <div class="span4" align="right"> \
                    <button class="btn"><i class="icon-refresh"></i><span id="volumeRefresh"></span></button> \
                </div> \
                <table id="volume_list" class="table table-striped table-bordered table-condensed"> \
                    <thead> \
                        <tr> \
                            <th width="125px;" align="left"><span id="volume_t1"></span></th> \
                            <th width="75px;" align="left"><span id="volume_t2"></span></th> \
                            <th width="50px;" align="left"><span id="volume_t3"></span></th> \
                            <th width="150px;" align="left"><span id="volume_t4"></span></th> \
                            <th width="50px;" align="left"><span id="volume_t5"></span></th> \
                            <th width="50px;" align="left"><span id="volume_t6"></span></th> \
                        </tr> \
                    </thead> \
                    <tbody style="word-wrap:break-word;word-break:break-all;"></tbody> \
                </table> \
            </div> \
            <div id="sa_last_refresh_time"></div><br /> \
        </div> \
    </div>';

function sa_click(a) {
    $('ul.main-menu li').removeClass("active");
    a.parent().addClass("active");

    $("#right_content").html(sa_detail_html);  
    c_changeLangue(page); 

    $("#right_content").find("OamStaticUI").each(function(){
        SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
    }); 

    sa_clicked = a.data("item");

    $('#ccSaTab a:first').tab('show');
    $('#ccSaTab a').click(function (e) {
        e.preventDefault();
        $(this).tab('show');

        if($(this).attr("id") == "saSummaryNav"){
            refresh_usage();  
        }     
    });

    vgTable = $("#vg_list").dataTable({
        "sPaginationType": "bootstrap",        
        "oLanguage": tableLanguage[page.pagelan],
        "alternatively" : "destory"
    });  

    volumeTable = $("#volume_list").dataTable({
        "sPaginationType": "bootstrap",        
        "oLanguage": tableLanguage[page.pagelan],
        "alternatively" : "destory"
    });  

    $("#vgRefresh").parent().click(function(){
        var param = [];
        param.push({ "name": "sid",  "value": sa_clicked.sid});        
        $.getJSON("../php/storage_info.php", param, function (json){  
            sa_clicked = json;  
            refresh_vg();
        });
    });
    $("#volumeRefresh").parent().click(function(){
        var param = [];
        param.push({ "name": "sid",  "value": sa_clicked.sid});
        $.getJSON("../php/storage_info.php", param, function (json){  
            sa_clicked = json;  
            refresh_volume();
        });
    });

    refresh_sa();  
}


$(window).unbind("resize").resize(function(){
    if($("#saUsePie").is(":visible")){    
        refresh_usage();
    }
});

function InitSelectVg(sl, cb){
    sl.empty();
    sl.attr("data-placeholder", page.res[page.pagelan].type.field_name["select_vg"]);

    for(var i = 0;i < sa_clicked["vg"].length;i++){ 
        sl.append("<option value='" + sa_clicked["vg"][i].name + "'>" + sa_clicked["vg"][i].name);
    }

    sl.chosen();

    if(cb != null){
        sl.unbind('change').bind('change',function(){
            cb();
        });
    }
}

function refresh_sa(){
    var param = [];
    param.push({ "name": "sid",  "value": sa_clicked.sid});

    $.getJSON("../php/storage_info.php", param, function (json){  
        sa_clicked = json;  
 
        refresh_usage();
        refresh_vg();
        refresh_volume();	
	json.register_time = getLocalTime(json.register_time);

        SetModifyForm($("#saSummary"), json); 

        InitSelectVg($("#selectVg"), refresh_volume); 

        if(sa_clicked.enabled == 0){
            $("#sa_enable_toggle").attr("checked", true);
        }
        else{
            $("#sa_enable_toggle").attr("checked", false);
        }    

        $("#sa_enable_toggle").iphoneStyle({onChange:operate_sa});             
    }); 
}

function refresh_usage(){
    var mapinfo = page.res[page.pagelan]["type"]["usage"]; 

    var saData = [{label:mapinfo["used"], data:parseInt(sa_clicked.used)}, 
                  {label:mapinfo["free"], data:parseInt(sa_clicked.free)}];

    $.plot($("#saUsePie"), saData, {
        series: { pie: { show: true } },    
        legend: { 
            container: $("#saUsePie").next(),
            labelFormatter: function(label, series){
                var formatLabel;                    

                if(isNaN(series.percent)){
                    series.percent = 0;
                }

                formatLabel = label + ' : ' + Capacity(series.data[0][1]) + 
                              ' (' + Math.floor(series.percent) + '%)';                

                return formatLabel;
            }
        }
    });

    $("#sa_last_refresh_time").html(GetLastUpdateTime());          
}

function refresh_vg(){
    vgTable.fnClearTable();

    var vgData = [];
    for (var i = 0; i < sa_clicked["vg"].length; i++) {
        var onevg = [];
        onevg[0] = sa_clicked["vg"][i]["name"];
        onevg[1] = Capacity(sa_clicked["vg"][i]["free"]);
        onevg[2] = Capacity(sa_clicked["vg"][i]["size"]);
        onevg[3] = "<a href='#' onclick='goto_volume($(this))'>" + sa_clicked["vg"][i]["count"] + "</a>";

        vgData.push(onevg);     
    };
    vgTable.fnAddData(vgData);

    $("#sa_last_refresh_time").html(GetLastUpdateTime());  
}

function goto_volume (vg) {
    var vg_name = vg.parent().parent().find("td").eq(0).text();

    $("#saVolumeNav").tab("show");
    $("#selectVg").val(vg_name);
    $("#selectVg").trigger("liszt:updated"); 

    refresh_volume();
}

function refresh_volume(){
    volumeTable.fnClearTable();

    var vg_selected = $("#selectVg").val();

    var volumeData = [];
    for (var i = 0; i < sa_clicked["volume"].length; i++) {
        var onevolume = [];
        onevolume[0] = sa_clicked["volume"][i]["vg"];
        onevolume[1] = sa_clicked["volume"][i]["name"];
        onevolume[2] = Capacity(sa_clicked["volume"][i]["size"]);
        //onevolume[3] = sa_clicked["volume"][i]["volume_uuid"];
        //onevolume[4] = sa_clicked["volume"][i]["volume_id"];
        onevolume[3] = sa_clicked["volume"][i]["request_id"];
        onevolume[4] = "";
        onevolume[5] = "";

        var volUsage = sa_clicked["volume"][i]["usage"];
        if(volUsage != null) {
            var usage = volUsage.split(":");
            if(usage[0] != null){
                onevolume[4] = usage[0];    
            }
            
            if(usage[1] != null){
                onevolume[5] = usage[1];                
            }
        }

        if(vg_selected == null || vg_selected == "" || vg_selected.toString().indexOf(onevolume[0]) != -1){        
            volumeData.push(onevolume);
        }
    };
    volumeTable.fnAddData(volumeData);
    
    $("#sa_last_refresh_time").html(GetLastUpdateTime());  
}

function operate_sa(){
    var rs;

    if(sa_clicked.enabled == 0){
        rs = xmlrpc_service.tecs.sa.enable(xmlrpc_session, Number(sa_clicked.sid));
    }
    else {
        rs = xmlrpc_service.tecs.sa.disable(xmlrpc_session, Number(sa_clicked.sid));        
    }

    if(rs[0] != 0){
        ShowNotyRpcErrorInfo(rs);
    }
    else {
        refresh_sa();
    }
}