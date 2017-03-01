var OP_ADD = 1;
var OP_DEL = 2;
var OP_MOD = 3;

var portgroup_selected = "";

$(function(){
    logicnetTable = $("#logicnet_list").dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] }, 
                         { "bSortable": false, "aTargets": [ 3 ] },
                         { "bSortable": false, "aTargets": [ 4 ] },
                         { "bSortable": false, "aTargets": [ 5 ] },
                         { "bSortable": false, "aTargets": [ 6 ] } ],
        "bServerSide": true,
        "bProcessing": false,
        "sAjaxSource": "../php/net_logicnet.php",
        "fnServerData": RetrieveLogicnetData,
        "alternatively" : "destory"
    });      

    $.getJSON("../php/logicnet_records.php",function(json){
        if(json[0] > 0){
            $('#tabsLogicnet a:eq(1)').tab('show');
        }
        else{
            $('#tabsLogicnet a:first').tab('show');
        }
    });

    $('#tabsLogicnet a').click(function (e) {
        e.preventDefault();
        $(this).tab('show');
    });

    $("#create_logicnet_link").click(function(e){
        $('#tabsLogicnet a').eq(1).click();
        $("#logicnetAdd").click();
    });
      
    $("#logicnetAdd").parent().click(add_logicnet);
    $("#logicnetDel").parent().click(del_logicnet);
    $("#logicnetRefresh").parent().click(refresh_logicnet);
    $("#logicnetSelecltAll").click(logicnet_select_all_click);

    InitSelectPortgroup($("#selectPortgroup"), "", SelectPortgroupChange);

    //refresh_logicnet();
    loginnet_timer_id    = setInterval(refresh_logicnet, 30000);
});

function InitSelectPortgroup(sl, netplane, cb){
    sl.empty();
    sl.attr("data-placeholder", page.res[page.pagelan].type.display["select_portgroup"]);

    $.getJSON("../php/logicnet_portgroup.php", function (json) {
        if(json[0] != 0){ 
            ShowNotyRpcErrorInfo(["11", json[1]]);
            return;
        }        

        for(var i = 0;i < json[1].length;i++){ 
            sl.append("<option value='" + json[1][i].name + "'>" + json[1][i].name);
        }

        sl.chosen();
    });

    if(cb != null){
        sl.unbind('change').bind('change',function(){
            cb();
        });
    }
}

function SelectPortgroupChange(){
    portgroup_selected = $("#selectPortgroup").val();
    if(portgroup_selected == null) portgroup_selected = "";

    refresh_logicnet();
}

function ShowIPUsageDetail(uuid, name){
    $.getJSON("../php/net_res_ip.php", function(json){
        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["11", json[1]]);
            return;
        } 

        var diag = $("#idx_detailDlg");

        var info = '';
        info += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
        info += '   <tbody>';
        info += '       <tr>';
        info += '           <td>' + 'IP' + '</td>';
        info += '           <td>' + page.res[page.pagelan].xid.html["mask"] + '</td>';
        info += '           <td>' + page.res[page.pagelan].xid.html["vid"] + '</td>';
        info += '       </tr>';

        for (var i = 0; i < json[1].length; i++) {
            if(json[1][i].logicnet_uuid == uuid){                
                info += '       <tr>';
                info += '           <td>' + json[1][i].ip + '</td>';
                info += '           <td>' + json[1][i].mask + '</td>';
                info += '           <td>' + json[1][i].vid + '</td>';
                info += '       </tr>';
            }
        };

        info += '   </tbody>';
        info += '</table>';

        var title = '<i class="icon-zoom-in"></i> <span>' + page.res[page.pagelan].xid.html["ipUseDetail"] + '</span>' + ' - ' + name; 
        diag.find('div.modal-header h5').html(title); 
        diag.find('div.modal-body').html(info); 

        diag.modal('show');       
    });
}

function GetFormatString(list){
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

function RetrieveLogicnetData(sSource, aoData, fnCallback){
    aoData.push({"name":"portgroup","value":GetFormatString(portgroup_selected)});

    $.getJSON(sSource, aoData, function(json){
        if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
            return;
        }        

        var ackdata = json.data;

        for (var i = 0; i < ackdata.aaData.length; i++) {
            var oneLogicnet = [];
        
            //oneLogicnet[0] = "<input type='checkbox' value='" + ackdata.aaData[i][0] + "' pg_uuid='" + ackdata.aaData[i][6] + "'/ >";
            oneLogicnet[0] = GetItemCheckbox(ackdata.aaData[i][0], currLogicnetState);
            oneLogicnet[1] = ackdata.aaData[i][1];
           //oneLogicnet[2] = ackdata.aaData[i][2];
          //  oneLogicnet[2] = "<span pg_uuid='" + ackdata.aaData[i][6] + "'>" + ackdata.aaData[i][2] + "</span>";
            oneLogicnet[2] =  "<span pg_uuid='" + ackdata.aaData[i][6] + "'>" +'<a href="javascript:void(0);" onclick="logicnet_portgroupinfo_click(this)">'+ ackdata.aaData[i][2] +'</a>'+ "</span>";
            oneLogicnet[3] = "<span value='" + ackdata.aaData[i][3] + "'>" + page.res[page.pagelan].type.IPMode[ackdata.aaData[i][3]] + "</span>";
            oneLogicnet[4] = '<a href="javascript:void(0);" onclick="ShowIPUsageDetail(' + "'" + ackdata.aaData[i][0] + "', '" + ackdata.aaData[i][1] + "'" + ')">' + ackdata.aaData[i][4] + '</a>';;
            oneLogicnet[5] = ackdata.aaData[i][5];
            oneLogicnet[6] = "<a href='javascript:void(0);' onclick='modify_logicnet_click(this)' data-rel='tooltip' data-original-title='"
                              + page.res[page.pagelan]["type"]["opr"]["modify"] + "'><i class='icon-edit'></i></a>";

            if(ackdata.aaData[i][3] != 2){
                oneLogicnet[6] += " | <a href='javascript:void(0);' onclick='modify_logicnet_iprange(this)' data-rel='tooltip' data-original-title='"
                              + page.res[page.pagelan]["type"]["display"]["IPrange"] + "'><i class='icon-cog'></i></a>";                              
            }    

            ackdata.aaData[i] = oneLogicnet;
        };

        fnCallback(ackdata);
        AvoidGotoBlankPage(logicnetTable);
        $(".tooltip").hide();
        $('[data-rel="tooltip"]').tooltip({"placement":"bottom"});
        
        $("#logicnetlist_last_refresh_time").html(GetLastUpdateTime());
    });
}

function logicnet_portgroupinfo_click(pg){
    var pg_name = $(pg).text();
    var pg_uuid = $(pg).parent().parent().parent().find("td span").eq(0).attr("pg_uuid");
    portgroup_info_common(pg_uuid,pg_name);
}

function add_logicnet(){
	var add_logicnet_dlg_html = 
	    '<div class="span8" style="border-right:1px solid #dddddd;padding-right:10px;"> \
            <div class="row-fluid"> \
            <form class="form-horizontal"> <fieldset>  \
                <div class="control-group">   \
                    <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="logicnet.name"></OamStaticUI></label> \
                    <div class="controls" style="margin-left:110px;"><input type="text" popover="name_format" id="logicnet_name" maxlength="32" /></div> \
                </div>    \
                <div class="control-group">   \
                    <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="logicnet.portgroup"></OamStaticUI></label> \
                    <div class="controls" style="margin-left:110px;"> \
                        <select id="logicnet_pg" ></select> \
                    </div> \
                </div>    \
                <div class="control-group">   \
                    <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="logicnet.ip_mode"></OamStaticUI></label> \
                    <div class="controls" style="margin-left:110px;"> \
                        <select id="logicnet_ip_mode"></select> \
                    </div> \
                </div>    \
                <div class="control-group">   \
                    <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="logicnet.description"></OamStaticUI></label> \
                    <div class="controls" style="margin-left:110px;"><textarea type="text" maxlength="128" rows="6" id="logicnet_desc"></textarea><p id="logicnet_desc_tip"></p></div> \
                </div>    \
            </fieldset></form> \
            </div> \
        </div> \
        <div class="span4" id="logicnet_pg_tips"> \
        </div>';

    ShowDiaglog("add_logicnet_dlg", add_logicnet_dlg_html, {
    	show:function(){
            var h5 = $("#add_logicnet_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-plus"></i>' + h5.text());    

            $("#add_logicnet_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });  

            z_strInput("logicnet_name", 64);

            $("#logicnet_pg").unbind('change').bind('change', function(){
                AppendPortGroupInfo($("#logicnet_pg").val(), $("#logicnet_pg_tips"));
            });  
           
            $("#logicnet_pg").empty();
            $.getJSON("../php/logicnet_portgroup.php", function(json){
                if(json[0] != 0){
                    ShowNotyRpcErrorInfo(["11", json[1]]);
                    return;
                }

                for (var i = 0; i < json[1].length; i++) {
                    $("#logicnet_pg").append("<option value='" + json[1][i].uuid +"'>" + json[1][i].name);
                };  

                $("#logicnet_pg").trigger("change");      
            });

            $("#logicnet_ip_mode").empty();
            $("#logicnet_ip_mode").append('<option value="0">' + page.res[page.pagelan].type.IPMode["0"]);
            $("#logicnet_ip_mode").append('<option value="1">' + page.res[page.pagelan].type.IPMode["1"]);
            $("#logicnet_ip_mode").append('<option value="2">' + page.res[page.pagelan].type.IPMode["2"]);

            TogglePopover($("#add_logicnet_dlg"));
            LimitCharLenInput($("#logicnet_desc"),$("#logicnet_desc_tip"));            
    	},
    	ok:function(){
            var name = $("#logicnet_name").val();
            var pg = $("#logicnet_pg").val();
            var ip_mode = $("#logicnet_ip_mode").val();
            var desc = $("#logicnet_desc").val();

            $(".control-group").removeClass("error");
            if(name == ""){
                $("#logicnet_name").parent().parent().addClass("error");
                return;
            }

            var rs = xmlrpc_service.tecs.vnet.logicnet.cfg(xmlrpc_session, name, "", pg, Number(ip_mode), desc, OP_ADD);

            if(rs[0] == 0){ 
                $("#add_logicnet_dlg").modal("hide");
                refresh_logicnet();
            }
            else {
                ShowNotyRpcErrorInfo(rs);
            }
    	}
    });
}

function del_logicnet(){
    var vCheckedbox = $(logicnetTable.fnGetNodes()).find(":checkbox:checked");

    if(vCheckedbox.size() > 0){
        ShowDiaglog("del_logicnet_dlg", page.res[page.pagelan]["xid"]["val"]["del_logicnet_confirm"], {
        	show:function(){
                var h5 = $("#del_logicnet_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-trash"></i>' + h5.text());    
        	},
        	ok:function(){
                $("#del_logicnet_dlg").modal("hide");

                vCheckedbox.each(function(){
                    var uuid = $(this).attr("value");
                    var tr = $(this).parent().parent();                      
                    var logicnet = tr.find("td").eq(1).text();                    

                    var rs = xmlrpc_service.tecs.vnet.logicnet.cfg(xmlrpc_session, logicnet, uuid, "", 0, "", OP_DEL);     
                    ShowNetSynRpcInfo(rs);
                });

                refresh_logicnet();
        	}
        });
    }
    else {
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]);    	
    }
}

function modify_logicnet_click(logicnet){
    var modify_logicnet_dlg_html = 
        '<div class="span8" style="border-right:1px solid #dddddd;padding-right:10px;"> \
            <div class="row-fluid"> \
            <form class="form-horizontal"> <fieldset>  \
            <div class="control-group">   \
                <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="logicnet.portgroup"></OamStaticUI></label> \
                <div class="controls" style="margin-left:110px;"> \
                    <select id="logicnet_pg_set"></select> \
                </div> \
            </div>    \
            <div class="control-group">   \
                <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="logicnet.ip_mode"></OamStaticUI></label> \
                <div class="controls" style="margin-left:110px;"> \
                    <select id="logicnet_ip_mode_set"></select> \
                </div> \
            </div>    \
            <div class="control-group">   \
                <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="logicnet.description"></OamStaticUI></label> \
                <div class="controls" style="margin-left:110px;"><textarea type="text" maxlength="128" rows="6" id="logicnet_desc_set"></textarea><p id="logicnet_desc_set_tip"></p></div> \
            </div>    \
            </fieldset></form> \
            </div> \
        </div> \
        <div class="span4" id="logicnet_pg_set_tips"> \
        </div>';    

    var logicnet_name = $(logicnet).parent().parent().find("td").eq(1).text();
    var ip_mode = $(logicnet).parent().parent().find("td span").eq(1).attr("value");
    var desc = $(logicnet).parent().parent().find("td").eq(5).text();
    var uuid = $(logicnet).parent().parent().find(":checkbox").eq(0).attr("value");
    //var pg_uuid = $(logicnet).parent().parent().find(":checkbox").eq(0).attr("pg_uuid");
    var pg_uuid = $(logicnet).parent().parent().find("td span").eq(0).attr("pg_uuid");
    
    ShowDiaglog("modify_logicnet_dlg", modify_logicnet_dlg_html, {
    	show:function(){
            var h5 = $("#modify_logicnet_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-edit"></i>' + h5.text() + ' - ' + logicnet_name);

            $("#modify_logicnet_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });   
            
            $("#logicnet_pg_set").unbind('change').bind('change', function(){
                AppendPortGroupInfo($("#logicnet_pg_set").val(), $("#logicnet_pg_set_tips"));
            });  

            $.getJSON("../php/logicnet_portgroup.php", function(json){
                if(json[0] != 0){
                    alert(json[1]);
                    return;
                }

                $("#logicnet_pg_set").empty();
                for (var i = 0; i < json[1].length; i++) {
                    $("#logicnet_pg_set").append("<option value='" + json[1][i].uuid +"'>" + json[1][i].name);
                };  

                $("#logicnet_pg_set").val(pg_uuid); 
                $("#logicnet_pg_set").trigger("change");             
            });

            $("#logicnet_ip_mode_set").empty();
            $("#logicnet_ip_mode_set").append('<option value="0">' + page.res[page.pagelan].type.IPMode["0"]);
            $("#logicnet_ip_mode_set").append('<option value="1">' + page.res[page.pagelan].type.IPMode["1"]);
            $("#logicnet_ip_mode_set").append('<option value="2">' + page.res[page.pagelan].type.IPMode["2"]);

            $("#logicnet_ip_mode_set").val(ip_mode);
            $("#logicnet_desc_set").val(desc);

            TogglePopover($("#add_logicnet_dlg"));
            LimitCharLenInput($("#logicnet_desc_set"),$("#logicnet_desc_set_tip"));                  
    	},
    	ok:function(){
            var pg = $("#logicnet_pg_set").val();
            var ip_mode = parseInt($("#logicnet_ip_mode_set").val());
            var desc = $("#logicnet_desc_set").val();

            var rs = xmlrpc_service.tecs.vnet.logicnet.cfg(xmlrpc_session, logicnet_name, uuid, pg, ip_mode, desc, OP_MOD);

            if(rs[0] == 0){ 
                $("#modify_logicnet_dlg").modal("hide");
                refresh_logicnet();
            }
            else {
                ShowNotyRpcErrorInfo(rs);
            }
    	}
    });
}

var currLogicnetState = new Object();
function refresh_logicnet(){
    if($('#logicnet_list').length == 0) {
        clearInterval(loginnet_timer_id);
        return;
    }
   // logicnetTable.fnDraw();
    ClearCurSelItem(currLogicnetState);  
    RecordSelItem(logicnetTable, "logicnet_list", currLogicnetState);   
    GotoCurrentPageOfDataTable(logicnetTable);
}

function logicnet_select_all_click(){
    if($(this).attr("checked") == "checked") {
        $("#logicnet_list tbody input[type=checkbox]").attr("checked", true);
    }
    else {
        $("#logicnet_list tbody input[type=checkbox]").attr("checked", false);
    }    	
}

function modify_logicnet_iprange(logicnet){
    var modify_logicnet_iprange_dlg_html =
    '<div class="span8" style="border-right:1px solid #dddddd;padding-right:10px;"> \
        <div class="row-fluid"> \
            <form class="form-horizontal span8"> \
                <div class="control-group">   \
                    <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="logicnet.ip_begin"></OamStaticUI></label> \
                    <div class="controls" style="margin-left:110px;"><input type="text" popover="ip_format" id="logicnet_ip_begin" maxlength="15" /></div> \
                </div>    \
                <div class="control-group">   \
                    <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="logicnet.ip_end"></OamStaticUI></label> \
                    <div class="controls" style="margin-left:110px;"><input type="text" popover="ip_format" id="logicnet_ip_end" maxlength="15" /></div> \
                </div>    \
                <div class="control-group">   \
                    <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="logicnet.ip_mask"></OamStaticUI></label> \
                    <div class="controls" style="margin-left:110px;"><input type="text" popover="mask_format" id="logicnet_ip_mask" maxlength="15" /></div> \
                </div>    \
            </form> \
        </div> \
        <div class="row-fluid"> \
            <div align="right"> \
                <button class="btn"><i class="icon-plus"></i><span id="logicnetIpAdd"></span></button>  \
                <button class="btn"><i class="icon-trash"></i><span id="logicnetIpDel"></span></button>  \
                <button class="btn"><i class="icon-refresh"></i><span id="logicnetIpRefresh"></span></button> \
            </div> \
            <table id="logicnet_ip_list" class="table table-striped table-bordered table-condensed">  \
                <thead>  \
                    <tr>  \
                        <th style="width:30%;" align="left"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="logicnet.ip_begin"></OamStaticUI></th>  \
                        <th style="width:30%;" align="left"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="logicnet.ip_end"></OamStaticUI></th>  \
                        <th style="width:30%;" align="left"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="logicnet.ip_mask"></OamStaticUI></th>  \
                        <th style="width:10%;"></th> \
                    </tr>  \
                </thead>  \
                <tbody style="word-wrap:break-word;word-break:break-all;">  \
                </tbody>  \
            </table> \
        </div> \
    </div> \
    <div class="span4" id="logicnetip_tips"> \
    </div>';

    logicnet_name = $(logicnet).parent().parent().find("td").eq(1).text();
    logicnet_uuid = $(logicnet).parent().parent().find(":checkbox").eq(0).attr("value");

    ShowDiaglog("modify_logicnet_iprange_dlg", modify_logicnet_iprange_dlg_html, {
        init: function(){
            logicnetIpTable = $("#logicnet_ip_list").dataTable({
                "sPaginationType": "bootstrap",
                "bFilter":false,
                "bInfo": false,
                "bPaginate": false,
                "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 3 ] }],
                "oLanguage": tableLanguage[page.pagelan]
            }); 

            $("#logicnetIpAdd").parent().bind("click", AddLogicnetIp);
            $("#logicnetIpDel").parent().bind("click", DelLogicnetIp);
            $("#logicnetIpRefresh").parent().click(RefreshLogicnetIp);

            TogglePopover($("#modify_logicnet_iprange_dlg"));                         
        },
        show:function(){
            var h5 = $("#modify_logicnet_iprange_dlg").find('div.modal-header h5');
            h5.html(h5.text() + ' - ' + logicnet_name); 

            $("#modify_logicnet_iprange_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });       

            RefreshLogicnetIp();
        },
        close:function(){
            $("#modify_logicnet_iprange_dlg").modal("hide");
        }
    }); 
}

function AddLogicnetIp(){
    $(".control-group").removeClass("error");

    var ipbegin = $("#logicnet_ip_begin").val();
    var ipend = $("#logicnet_ip_end").val();
    var mask =  $("#logicnet_ip_mask").val();    

    if(ipbegin == "" || !checkIP(ipbegin)){
        $("#logicnet_ip_begin").parent().parent().addClass("error");
        return false;
    }    

    if(ipend == "" || !checkIP(ipend)){
        $("#logicnet_ip_end").parent().parent().addClass("error");
        return false;
    }    

    if(mask == "" || !checkIP(mask)){
        $("#logicnet_ip_mask").parent().parent().addClass("error");
        return false;
    } 
    
    if(IP2Decimal(ipbegin) > IP2Decimal(ipend)){
        $("#logicnet_ip_end").parent().parent().addClass("error");
        return false;
    }    

    var rs = xmlrpc_service.tecs.vnet.logicnetip.cfg(xmlrpc_session, logicnet_uuid, ipbegin, mask, ipend, mask, OP_ADD);

    if(rs[0] == 0){
        RefreshLogicnetIp();
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }
}

function DelLogicnetIp(){
    var ipbegin = $("#logicnet_ip_begin").val();
    var ipend = $("#logicnet_ip_end").val();
    var mask =  $("#logicnet_ip_mask").val();    

    if(ipbegin == "" || !checkIP(ipbegin)){
        $("#logicnet_ip_begin").parent().parent().addClass("error");
        return false;
    }    

    if(ipend == "" || !checkIP(ipend)){
        $("#logicnet_ip_end").parent().parent().addClass("error");
        return false;
    }    

    if(mask == "" || !checkIP(mask)){
        $("#logicnet_ip_mask").parent().parent().addClass("error");
        return false;
    } 
    
    if(IP2Decimal(ipbegin) > IP2Decimal(ipend)){
        $("#logicnet_ip_end").parent().parent().addClass("error");
        return false;
    }    

    var rs = xmlrpc_service.tecs.vnet.logicnetip.cfg(xmlrpc_session, logicnet_uuid, ipbegin, mask, ipend, mask, OP_DEL);

    if(rs[0] == 0){
        RefreshLogicnetIp();
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }
}

function delete_one_iprange(iprange){
    var ipbegin = $(iprange).parent().parent().find("td").eq(0).text();
    var ipend = $(iprange).parent().parent().find("td").eq(1).text();
    var mask = $(iprange).parent().parent().find("td").eq(2).text();  

    var rs = xmlrpc_service.tecs.vnet.logicnetip.cfg(xmlrpc_session, logicnet_uuid, ipbegin, mask, ipend, mask, OP_DEL);

    if(rs[0] == 0){
        RefreshLogicnetIp();
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }       
}

function RefreshLogicnetIp(){
    var param = [];
    param.push({"name":"logicnet", "value":logicnet_name});
    param.push({"name":"uuid", "value":logicnet_uuid});    

    $.getJSON("../php/logicnet_iprange.php", param, function(json){
        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["11", json[1]]);
            return;
        }        

        var tips = "";
        tips += '<div><strong>' + page.res[page.pagelan].type.display["logicnet_ip_tips"] + '</strong></div>';
        tips += '<br />';
        tips += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
        tips += '   <tbody>';
        tips += '       <tr>';
        tips += '           <td>' + page.res[page.pagelan].type.field_name["logicnet.ip_begin"] + '</td>';
        tips += '           <td>' + page.res[page.pagelan].type.field_name["logicnet.ip_end"] + '</td>';
        tips += '           <td>' + page.res[page.pagelan].type.field_name["logicnet.ip_mask"] + '</td>';
        tips += '       </tr>';

        var iprange = [];
        for (var i = 0; i < json[1].length; i++) {
            var oneRange = [];
            
            if(json[1][i].uuid == logicnet_uuid){
                oneRange[0] = json[1][i].ip_begin;
                oneRange[1] = json[1][i].ip_end;
                oneRange[2] = json[1][i].ip_mask;    
                oneRange[3] = "<a href='javascript:void(0);' onclick='delete_one_iprange(this)'><i class='icon-trash'></i></a>";

                iprange.push(oneRange);
            }
            else {
                tips += '       <tr>';
                tips += '           <td>' + json[1][i].ip_begin + '</td>';
                tips += '           <td>' + json[1][i].ip_end   + '</td>';
                tips += '           <td>' + json[1][i].ip_mask  + '</td>';                
                tips += '       </tr>';                
            }
        };

        tips += '   </tbody>';
        tips += '</table>';

        $("#logicnetip_tips").html(tips); 

        logicnetIpTable.fnClearTable();
        logicnetIpTable.fnAddData(iprange);
    });    
}

function AppendPortGroupInfo(pg_uuid, ref){
    var param = [];
    param.push({"name":"pg_uuid", "value":pg_uuid});                

    $.getJSON("../php/portgroup_info.php", param, function (json) {
        if(json[0] != 0){ 
            ShowNotyRpcErrorInfo(["11", json[1]]);
            return;
        }        

        var pginfo = '';

        pginfo += '<strong>' + page.res[page.pagelan].type.display.pg + " : " + json[1]["pg_name"] + '</strong>';

        pginfo += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
        pginfo += '   <tbody>';

        pginfo += '       <tr>';
        pginfo += '           <td>' + page.res[page.pagelan].type.display.dt1_netplan + '</td>';
        pginfo += '           <td>' + json[1]["netplane_name"] + '</td>';
        pginfo += '       </tr>';
        
        pginfo += '       <tr>';
        pginfo += '           <td>' + page.res[page.pagelan].type.display.dt1_type + '</td>';
        pginfo += '           <td>' + page.res[page.pagelan].type.PortgroupType[json[1]["pg_type"]] + '</td>';
        pginfo += '       </tr>';
        
        pginfo += '       <tr>';
        pginfo += '           <td>' + page.res[page.pagelan].type.display.dt1_isolateType + '</td>';
        pginfo += '           <td>' + page.res[page.pagelan].type.IsolateType[json[1]["isolate_type"]] + '</td>';
        pginfo += '       </tr>';
        /*
        pginfo += '       <tr>';
        pginfo += '           <td>' + page.res[page.pagelan].type.display.dt3_promisc + '</td>';
        pginfo += '           <td>' + page.res[page.pagelan].type.PortgroupPromisc[json[1]["promiscuous"]] + '</td>';
        pginfo += '       </tr>';        
*/
        pginfo += '       <tr>';
        pginfo += '           <td>' + page.res[page.pagelan].type.display.dt3_mtu + '</td>';
        pginfo += '           <td>' + json[1]["mtu"] + '</td>';
        pginfo += '       </tr>';                       
      
        pginfo += '       <tr>';
        pginfo += '           <td>' + page.res[page.pagelan].type.display.dt1_description + '</td>';
        pginfo += '           <td>' + json[1]["desc"] + '</td>';
        pginfo += '       </tr>'; 

        if(json[1]["vlan_range"].length > 0){
            pginfo += '       <tr>';
            pginfo += '           <td>' + page.res[page.pagelan].type.display["VLANrange"] + '</td>';
            pginfo += '           <td>';
                for(var i = 0;i < json[1]["vlan_range"].length;i++){ 
                    pginfo +=  json[1]["vlan_range"][i][0] + "~" + json[1]["vlan_range"][i][1] + '<br />';
                }   
            pginfo += '           </td>'
            pginfo += '       </tr>';
        }      

        if(json[1]["pg_type"] == 21){
            pginfo += '       <tr>';
            pginfo += '           <td>' + page.res[page.pagelan].type.display.dt2_isolate_vlan + '</td>';
            pginfo += '           <td>' + json[1]["isolate_no"] + '</td>';
            pginfo += '       </tr>';
        }
        else if(json[1]["pg_type"] == 20 &&json[1]["isolate_type"] == 1){
            pginfo += '       <tr>';
            pginfo += '           <td>' + page.res[page.pagelan].type.display.dt2_vxlan_segment + '</td>';
            pginfo += '           <td>' + json[1]["segment"] + '</td>';
            pginfo += '       </tr>';
        }
        else if(json[1]["switchport_mode"] == 0){
            pginfo += '       <tr>';
            pginfo += '           <td>' + page.res[page.pagelan].type.display.dt2_switch_port_mode + '</td>';
            pginfo += '           <td>' + page.res[page.pagelan].type.SwitchPortMode[json[1]["switchport_mode"]] + '</td>';
            pginfo += '       </tr>'; 
            
            pginfo += '       <tr>';
            pginfo += '           <td>' + page.res[page.pagelan].type.display.dt2_vlan_id + '</td>';
            pginfo += '           <td>' + json[1]["native_vlan_num"] + '</td>';
            pginfo += '       </tr>';
        }  
        else if(json[1]["switchport_mode"] == 1){
            pginfo += '       <tr>';
            pginfo += '           <td>' + page.res[page.pagelan].type.display.dt2_switch_port_mode + '</td>';
            pginfo += '           <td>' + page.res[page.pagelan].type.SwitchPortMode[json[1]["switchport_mode"]] + '</td>';
            pginfo += '       </tr>';
            
            pginfo += '       <tr>';
            pginfo += '           <td>' + page.res[page.pagelan].type.display.dt2_access_vlan + '</td>';
            pginfo += '           <td>' + json[1]["access_vlan_num"] + '</td>';
            pginfo += '       </tr>';
        }
        pginfo += '       <tr>';
        pginfo += '           <td>SDN</td>';
        pginfo += '           <td>' + json[1]["is_sdn"] + '</td>';
        pginfo += '       </tr>'; 
        
        pginfo += '   </tbody>';
        pginfo += '</table>'; 

        ref.html(pginfo);
    });
}