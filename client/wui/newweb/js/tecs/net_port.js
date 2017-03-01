var EN_ADD_LOOPPORT   = 1;
var EN_SET_LOOPPORT   = 2;
var EN_UNSET_LOOPPORT = 3;
var EN_DEL_LOOPPORT   = 4;
var EN_ADD_WC_LOOPBACK_PORT = 1;
var EN_DEL_WC_LOOPBACK_PORT = 2;

var PORT_TYPE_UPLINKLOOP = 4;
var EN_ADD_WC_PORT = 1;
var  EN_DEL_WC_PORT =2;

var VNET_LIB_WC_TASK_UPLINK = 1;
var VNET_LIB_WC_TASK_LOOP = 2;

PortView = Backbone.View.extend({

    initialize:function(){

    },

    render:function(eventName){
        var portTableHtml = 
            '<div align="right">  \
                <button class="btn" id="portRefresh"> \
                    <i class="icon-refresh"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="refresh"/></span> \
                </button> \
            </div> \
            <table id="portList" class="table table-striped table-bordered table-condensed"> \
                <thead> \
                    <tr> \
                        <th align="left" width="75px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="port.name"/></span> </th> \
                        <th align="left" width="75px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="port.loop"/></span></th> \
                        <th align="left" width="75px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="duplux"/></span></th> \
                        <th align="left" width="50px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="port.speed"/></span></th> \
                        <th align="left" width="50px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="port.state"/></span></th> \
                        <th align="left" width="50px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="port.link"/></span></th> \
                        <th align="left" width="50px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="port.sriov"/></span></th> \
						<th align="left" width="50px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="is_consistency"/></span></th> \
						<th align="left" width="50px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="IP"/></span></th> \
                    </tr> \
                </thead> \
                <tbody style="table-layout:fixed;word-break:break-all"> </tbody> \
            </table> \
            <div></div> <br />';

        $(this.el).html(portTableHtml);
        
        return this;
    },   

    events:{
        "click #portRefresh":"RefreshPort" 
    },

    RefreshPort:function(){
        portTable.fnDraw();
    }
	
});



TrunkView = Backbone.View.extend({
    initialize:function(){

    },

    render:function(eventName){
        var trunkTableHtml = 
            '<div align="right">  \
                <button class="btn" id="trunkRefresh"> \
                    <i class="icon-refresh"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="refresh"/></span> \
                </button> \
            </div> \
            <table id="trunkList" class="table table-striped table-bordered table-condensed"> \
                <thead> \
                    <tr> \
                        <th align="left" width="75px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="bond.name"/></span></th> \
                        <th align="left" width="50px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="bond.type"/></span></th> \
                        <th align="left" width="75px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="members.linked"/></span></th> \
                        <th align="left" width="50px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="members.bond"/></span></th> \
                    </tr> \
                </thead> \
                <tbody style="table-layout:fixed;word-break:break-all"> </tbody> \
            </table> \
            <div></div><br/>';

        $(this.el).html(trunkTableHtml);

        return this;
    },   

    events:{
        "click #trunkRefresh":"RefreshTrunk" 
    },

    RefreshTrunk:function(){
        trunkTable.fnDraw();
    }       
});

LoopView = Backbone.View.extend({
    initialize:function(){

    },

    render:function(eventName){
        var loopTableHtml = 
            '<div align="right">  \
                <button class="btn" id="loopCreate"> \
                    <i class="icon-plus"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="create"/></span> \
                </button> \
                <button class="btn" id="loopRemove"> \
                    <i class="icon-trash"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="remove"/></span> \
                </button> \
                <button class="btn" id="loopRefresh"> \
                    <i class="icon-refresh"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="refresh"/></span> \
                </button> \
            </div> \
            <table id="loopList" class="table table-striped table-bordered table-condensed"> \
                <thead> \
                    <tr> \
                        <th align="left" width="25px;"><INPUT id="loopSelecltAll" type="checkbox" /></th> \
                        <th align="left" width="250px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="loop.name"/></th> \
                    </tr> \
                </thead> \
                <tbody style="table-layout:fixed;word-break:break-all"> </tbody> \
            </table> \
            <div></div><br/>';

        $(this.el).html(loopTableHtml);

        return this;
    },

    events:{
        "click #loopCreate":"CreateLoop", 
        "click #loopRemove":"RemoveLoop", 
        "click #loopRefresh":"RefreshLoop", 
        "click #loopSelecltAll":"LoopSelecltAll"
    }, 

    CreateLoop:function(){
        createLoop();
    },

    RemoveLoop:function(){
        removeLoop();
    },    

    RefreshLoop:function(){
        loopTable.fnDraw();
        $("#loopList  input[type=checkbox]").attr("checked", false);
    },

    LoopSelecltAll:function(){
        if($("#loopSelecltAll").attr("checked")=="checked") {
            $("#loopList tbody input[type=checkbox]").attr("checked", true);    
		}
	    else {
		    $("#loopList tbody input[type=checkbox]").attr("checked", false);
		}
    }
});

function removeLoop(){
    var vCheckedbox = $(loopTable.fnGetNodes()).find(":checkbox:checked");

    if(vCheckedbox.size() > 0) {
        ShowDiaglog("delete_loop_dlg", page.res[page.pagelan]["xid"]["val"]["delete_loop_confirm"], {
            show:function(){
                var h5 = $("#delete_loop_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-trash"></i>' + page.res[page.pagelan].type.opr["remove"]);
            },
            ok:function(){
                vCheckedbox.each(function(){
                    var uuid = $(this).attr("value");
                    var loopname = $(this).parent().next().text();
                    var rs = xmlrpc_service.tecs.vnet["loopport"].cfg(xmlrpc_session, vna_uuid, uuid, loopname, EN_DEL_LOOPPORT);  
                    if(rs[0] == 0){ 
                        $("#delete_loop_dlg").modal("hide");
                        loopTable.fnDraw();
                    }
                    else {
                        ShowNotyRpcErrorInfo(rs);
                    }
                });
            }
        });
    }
    else {
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]); 
    }
}

function checkLoopName(str){
    if (str === ""){
        return false;
    }
    var reg = new RegExp("^loop[A-Za-z0-9_]+$");
    if (!reg.test(str)){
        return false;
    }
    return true;
}

function createLoop(){
    var add_loop_dlg_html = 
        '<form class="form-horizontal"><fieldset>  \
            <div class="control-group">   \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="loop.name"></OamStaticUI></label> \
                <div class="controls"> \
                    <input type="text" id="loop_name" popover="loop_name_format" value="loop" maxlength="15"/> \
                    <span class="help-inline">' + page.res[page.pagelan].type.display["loop_name_format"] + '</span>\
                </div> \
            </div> \
        </fieldset></form>';    

    ShowDiaglog("add_loop_dlg", add_loop_dlg_html, {
    	show:function(){
            $("#loop_name").val("loop");
            var h5 = $("#add_loop_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-plus"></i>' + page.res[page.pagelan].type.opr["create"]); 

            $("#add_loop_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });     

            $("#port_info_dlg").modal("hide");

            TogglePopover($("#add_loop_dlg"));
    	},
    	ok:function(){
            var name = $("#loop_name").val();

            $(".control-group").removeClass("error");
            if(!checkLoopName(name)){
                $("#loop_name").parent().parent().addClass("error");
                return;
            }		

            var rs = xmlrpc_service.tecs.vnet["loopport"].cfg(xmlrpc_session, vna_uuid, "", name, EN_ADD_LOOPPORT);

            if(rs[0] == 0){ 
                $("#add_loop_dlg").modal("hide");
                $("#port_info_dlg").modal("show");
                loopTable.fnDraw();
            }
            else {
                ShowNotyRpcErrorInfo(rs);
            }
    	},
        cancel:function(){
            $("#add_loop_dlg").modal("hide");
            $("#port_info_dlg").modal("show");            
        }
    });	
}

SriovView = Backbone.View.extend({
    initialize:function(){
    },

    render:function(eventName){
        var sriovTableHtml = 
            '<div align="right">  \
                <button class="btn" id="sriovRefresh"> \
                    <i class="icon-refresh"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="refresh"/></span> \
                </button> \
            </div> \
            <table id="sriovList" class="table table-striped table-bordered table-condensed"> \
                <thead> \
                    <tr> \
                        <th align="left" width="75px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="sriov_vf_port_name"/></th> \
                        <th align="left" width="75px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="pci"/></th> \
                        <th align="left" width="75px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="vlan_num"/></th> \
                        <th align="left" width="50px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="sriov_vf_is_free"/></th> \
                        <th align="left" width="50px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="phy_port_name"/></th> \
                    </tr> \
                </thead> \
                <tbody style="table-layout:fixed;word-break:break-all"> </tbody> \
            </table> \
            <div></div><br/>';

        $(this.el).html(sriovTableHtml);
		
        return this;
    },   

    events:{
        "click #sriovRefresh":"RefreshSriov" 
    },

    RefreshSriov:function(){
        sriovTable.fnDraw();
    }       
});

function RetrieveSriovData(sSource, aoData, fnCallback){
    aoData.push({"name":"sVNAid","value":vna_uuid});
    $.getJSON(sSource,aoData,function(json){
	    
	    var sriovInfo = json.data;

		if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
            return;
        }
		
		fnCallback(sriovInfo);
	});
}

function RefreshVnaData(){
    clearInterval(vna_timer_id);
    if($('#vnaList').length == 0) {
        return;
    }

    RecordSelItem(vnaTable, "vnaList", currVnaState);    
    GotoCurrentPageOfDataTable(vnaTable);
}

function VnaSelecltAllClick(){
    if($(this).attr("checked") == "checked") {
        $("#vnaList tbody input[type=checkbox]").attr("checked", true);
    }
    else {
        $("#vnaList tbody input[type=checkbox]").attr("checked", false);
    }      
}

function CleanVnaData(){
    var vCheckedbox = $(vnaTable.fnGetNodes()).find(":checkbox:checked");

    var vresultSucceeded = [];
    var vresultFailed    = [];

    if(vCheckedbox.size() > 0){
        ShowDiaglog("forget_vna_dlg", page.res[page.pagelan]["xid"]["val"]["forget_vna_confirm"], {
            show:function(){
                var h5 = $("#forget_vna_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-trash"></i>' + h5.text());                    
            },
            ok:function(){
                $("#forget_vna_dlg").modal('hide');    

                vCheckedbox.each(function(){
                    var vnaUuid = $(this).attr("value");
   
                    var rs = xmlrpc_service.tecs.vnet.clearvna.cfg(xmlrpc_session, vnaUuid);

                    ShowSynRpcInfo(rs);
                });    

                RefreshVnaData();
            }
        });
    }
    else{
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]);
    }    
}

var currVnaState = Object();

function RetrieveVnaData(sSource, aoData, fnCallback){
    $.getJSON(sSource, aoData, function(json){
        var ackdata = json.data;

        for (var i = 0; i < ackdata.aaData.length; i++) {
            var oneVna = [];
            oneVna[0] = GetItemCheckbox(ackdata.aaData[i][0], currVnaState);
            oneVna[1] = ackdata.aaData[i][0];
            oneVna[2] = ackdata.aaData[i][2];
            oneVna[3] = page.res[page.pagelan]["type"]["VNAIsOnline"][json.data.aaData[i][3]];
            oneVna[4] = "<a href='javascript:void(0)' onclick='vna_portinfo_click(this)'" 
                        + "value='"+ackdata.aaData[i][0]+"'>" + ackdata.aaData[i][4] + "</a>";

            ackdata.aaData[i] = oneVna;
        };

        fnCallback(ackdata); 
        AvoidGotoBlankPage(vnaTable);
        ClearCurSelItem(currVnaState);

        $("#vna_last_refresh_time").html(GetLastUpdateTime());       
    });
    
    vna_timer_id=setInterval(RefreshVnaData, 10000);
}

$(function(){
    $('#vnaPortTab a:first').tab('show');
    $('#vnaPortTab a').click(function (e){
        e.preventDefault();
        $(this).tab('show');
    }); 

    $('#wildcardPortCfgTab a:first').tab('show');
    $('#wildcardPortCfgTab a').click(function (e) {
        e.preventDefault();
        $(this).tab('show');
        if( $(this).attr("id") == "loopWildcardNav"){
            RefreshLoopBackTask();
        }
        if( $(this).attr("id") == "uplinkWildcardNav"){
            RefreshLoopUplinkTask();
        }
        
    });         

    vnaTable = $("#vnaList").dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] },
                         { "bSortable": false, "aTargets": [ 3 ] },
                         { "bSortable": false, "aTargets": [ 4 ] }],
        "bServerSide": true,
        "sAjaxSource": "../php/net_vna.php",
        "fnServerData": RetrieveVnaData,
        "alternatively" : "destory"
    }); 

    $("#vnaRefresh").parent().click(RefreshVnaData);
    $("#vnaClean").parent().click(CleanVnaData);
    $("#vnaSelecltAll").click(VnaSelecltAllClick);

    loopWildcardTable = $("#loop_wildcard_list").dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] }]          
    });

    $("#loopWildcardAdd").parent().click(AddLoopWildcardClick);
    $("#loopWildcardDel").parent().click(DelLoopWildcardClick);
    $("#loopWildcardRefresh").parent().click(RefreshLoopWildcardClick); 
    $("#loopWildcardSelectAll").click(LoopSelectAllClick);

    uplinkWildcardTable = $("#uplink_wildcard_list").dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] }]        
    });

    $("#uplinkWildcardAdd").parent().click(AddUplinkWildcardClick);
    $("#uplinkWildcardDel").parent().click(DelUplinkWildcardClick);
    $("#uplinkWildcardRefresh").parent().click(RefreshUplinkWildcardClick);  
    $("#uplinkWildcardSelectAll").click(UplinkSelectAllClick);

    wildcardTaskTable = $("#wildcardtask_list").dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "sDom":"tip",
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 5 ] }]  
    });

    RefreshLoopWildcardClick();
    RefreshUplinkWildcardClick();
    RefreshLoopBackTask();
});

function CheckLoopWildcardName(name){
    if ( (new RegExp("^[A-Za-z0-9_]+$")).test(name)) {
        return true;
    }
    return false; 
}
function AddLoopWildcardClick(){
    var add_loopwildcard_dlg_html = 
    '<form class="form-horizontal"> <fieldset> \
        <div class="control-group">  \
            <label class="control-label"><span id="loop_wildcard_portname"></span><span><font color="red">*</font></span></label> \
            <div class = "controls">\
            <input id="loopWildcardportname" type="text" maxlength="15"></input> \
            <span class="help-inline">' + page.res[page.pagelan].type.display["loop_wildcard_name_format"] + '</span>\
            </div>\
        </div>  \
        <div class="control-group">   \
            <label class="control-label"><span id="loop_wildcard_isloop"></span><span><font color="red">*</font></span></label> \
            <div class="controls"> \
                <input data-no-uniform="true" type="checkbox" class="iphone-toggle" id="loopWildcardisloop"> \
            </div> \
        </div> \
    </fieldset> </form>';

    ShowDiaglog("add_loopwildcard_dlg", add_loopwildcard_dlg_html, {
        show: function(){
            var h5 = $("#add_loopwildcard_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-plus"></i>' + h5.text()); 
            $("#loopWildcardportname").parent().parent().removeClass("error");
            $("#loopWildcardportname").val("");
            $("#loopWildcardportname").unbind("keypress").bind("keypress",function(event){
                if(event.keyCode == 13){
                    event.preventDefault();
                }
            });
            $("#loopWildcardisloop").iphoneStyle({
                duration:100,
                checkedLabel: page.res[page.pagelan].type.LoopOpr["0"],
                uncheckedLabel: page.res[page.pagelan].type.LoopOpr["1"]
            }).data("iphoneStyle").refresh();
        },
        ok: function(){
            var name = $("#loopWildcardportname").val();
          
            if(!CheckLoopWildcardName(name)){
                $("#loopWildcardportname").parent().parent().addClass("error");
                return;
            }
            var isLoop = 0;
            if($("#loopWildcardisloop").attr("checked") == "checked"){
                isLoop = 1;
            }

            var rs = xmlrpc_service.tecs.vnet["wcloopport"].cfg(xmlrpc_session,"",name,isLoop,EN_ADD_WC_LOOPBACK_PORT);
            if(rs[0] == 0){
                $("#add_loopwildcard_dlg").modal("hide");
                RefreshLoopWildcardClick();
            }
            else{
                ShowNotyRpcErrorInfo(rs);
            }
        }
    });
}

function DelLoopWildcardClick(){
    var vCheckedbox = $(loopWildcardTable.fnGetNodes()).find(":checkbox:checked");

    if(vCheckedbox.size() > 0){
        ShowDiaglog("del_loopwildcard_dlg", page.res[page.pagelan]["xid"]["val"]["del_loopwildcard_confirm"], {
            show:function(){
                var h5 = $("#del_loopwildcard_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-trash"></i>' + h5.text());    
            },
            ok:function(){
                $("#del_loopwildcard_dlg").modal("hide");

                vCheckedbox.each(function(){
                    var uuid = $(this).attr("value");
                    var rs = xmlrpc_service.tecs.vnet["wcloopport"].cfg(xmlrpc_session, uuid, "", 3, EN_DEL_WC_LOOPBACK_PORT );
                    if(rs[0] == 0){
                        $("#add_loopwildcard_dlg").modal("hide");
                        RefreshLoopWildcardClick();
                    }
                    else{
                        ShowNotyRpcErrorInfo(rs);
                    }
                });
            }
        });
    }
    else {
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]);       
    }
}

 
function RefreshLoopWildcardClick(){
    var currLoopWildcardState = new Object();
    ClearCurSelItem(currLoopWildcardState);  
    RecordSelItem(loopWildcardTable, "loop_wildcard_list", currLoopWildcardState);   
    pageno = currLoopWildcardState.pageno;
    
    $.getJSON("../php/wildcard_loop.php", function (json){
        loopWildcardTable.fnClearTable();

        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["2", json[1]]);
            return;
        }

        var loopData = [];

        for (var i = 0; i < json[1].length; i++) {
            var oneLoop = [];
            
            oneLoop[0] = "<input type='checkbox' value='" + json[1][i][0] + "' />"; 
            oneLoop[1] = json[1][i][1];
            oneLoop[2] = json[1][i][2];

            loopData.push(oneLoop);
        };

        loopWildcardTable.fnAddData(loopData);
        
        var oSettings = loopWildcardTable.fnSettings();
        var wholeNO = json[1].length /oSettings._iDisplayLength;

        if(wholeNO <= 1){
        }
        else if(wholeNO < (pageno +1) ){
            loopWildcardTable.fnPageChange("last");
        }
        else{
            loopWildcardTable.fnPageChange(parseInt(pageno));
        }
        
        $("#loop_wildcard_list  input[type=checkbox]").attr("checked", false);
    });
    RefreshLoopBackTask();
}

function LoopSelectAllClick(){
    if($(this).attr("checked") == "checked") {
        $("#loop_wildcard_list tbody input[type=checkbox]").attr("checked", true);
    }
    else {
        $("#loop_wildcard_list tbody input[type=checkbox]").attr("checked", false);
    }     
}

function CheckUplinkWildcardName(name){
    if ( (new RegExp("^loopw[A-Za-z0-9_]+$")).test(name)) {
        return true;
    }
    return false;
}
 
function AddUplinkWildcardClick(){
    var add_uplinkwildcard_dlg_html =     
    '<form class="form-horizontal"> <fieldset> \
        <div class="control-group">  \
            <label class="control-label"><span id="uplink_wildcard_portname"></span><span><font color="red">*</font></span></label> \
            <div class="controls">\
            <input id="uplinkWildcardportname"  type="text" maxlength="15"></input>\
            <span class="help-inline">' + page.res[page.pagelan].type.display["uplink_wildcard_name_format"] + '</span>\
            </div> \
        </div>  \
    </fieldset> </form>';

    ShowDiaglog("add_uplinkwildcard_dlg", add_uplinkwildcard_dlg_html, {
        show: function(){
            var h5 = $("#add_uplinkwildcard_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-plus"></i>' + h5.text()); 
            $("#uplinkWildcardportname").parent().parent().removeClass("error");
            $("#uplinkWildcardportname").val("loopw");
            $("#loopWildcardisloop").empty();
            $("#uplinkWildcardportname").unbind("keypress").bind("keypress",function(event){
                if(event.keyCode == 13){
                    event.preventDefault();
                }
            });
            //TogglePopover($("#add_uplinkwildcard_dlg"));
        },
        ok: function(){
            var name = $("#uplinkWildcardportname").val();
            if(!CheckUplinkWildcardName(name)){
                $("#uplinkWildcardportname").parent().parent().addClass("error");
                return;
            }
            var rs = xmlrpc_service.tecs.vnet["wcvport"].cfg(xmlrpc_session, name, "", PORT_TYPE_UPLINKLOOP, 0, "", "",EN_ADD_WC_PORT );
            if(rs[0] == 0){
                $("#add_uplinkwildcard_dlg").modal("hide");
                RefreshUplinkWildcardClick();
            }
            else{
                ShowNotyRpcErrorInfo(rs);
            }
        }
    });
}

function DelUplinkWildcardClick(){
    var vCheckedbox = $(uplinkWildcardTable.fnGetNodes()).find(":checkbox:checked");

    if(vCheckedbox.size() > 0){
        ShowDiaglog("del_uplinkwildcard_dlg", page.res[page.pagelan]["xid"]["val"]["del_uplinkwildcard_confirm"], {
            show:function(){
                var h5 = $("#del_uplinkwildcard_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-trash"></i>' + h5.text());    
            },
            ok:function(){
                $("#del_uplinkwildcard_dlg").modal("hide");

                vCheckedbox.each(function(){
                    var uuid = $(this).attr("value");
                    var rs = xmlrpc_service.tecs.vnet["wcvport"].cfg(xmlrpc_session, "", uuid, PORT_TYPE_UPLINKLOOP, 0, "", "",EN_DEL_WC_PORT );
                    if(rs[0] == 0){
                        $("#add_uplinkwildcard_dlg").modal("hide");
                        //RefreshUplinkWildcardClick();
                    }
                    else{
                        ShowNotyRpcErrorInfo(rs);
                    }
                    //ShowSynRpcInfo(rs);
                });
                RefreshUplinkWildcardClick();
                
            }
        });
    }
    else {
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]);       
    }
}


function RefreshUplinkWildcardClick(){
    currUplinkWildcardState = new Object();
    ClearCurSelItem(currUplinkWildcardState);  
    RecordSelItem(uplinkWildcardTable, "uplink_wildcard_list", currUplinkWildcardState);   
    pageno = currUplinkWildcardState.pageno;
    
    $.getJSON("../php/wildcard_uplink.php", function (json){
        uplinkWildcardTable.fnClearTable();

        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["2", json[1]]);
            return;
        }

        var uplinkData = [];

        for (var i = 0; i < json[1].length; i++) {
            var oneUplink = [];
            
            oneUplink[0] = "<input type='checkbox' value='" + json[1][i][0] + "' />"; 
            oneUplink[1] = json[1][i][1];

            uplinkData.push(oneUplink);
        };

        uplinkWildcardTable.fnAddData(uplinkData);
        
        var oSettings = uplinkWildcardTable.fnSettings();
        var wholeNO = json[1].length /oSettings._iDisplayLength;

        if(wholeNO <= 1){
        }
        else if(wholeNO < (pageno +1) ){
            uplinkWildcardTable.fnPageChange("last");
        }
        else{
            uplinkWildcardTable.fnPageChange(parseInt(pageno));
        }
        
        $("#uplink_wildcard_list  input[type=checkbox]").attr("checked", false);
    });
    RefreshLoopUplinkTask();
}

function UplinkSelectAllClick(){
    if($(this).attr("checked") == "checked") {
        $("#uplink_wildcard_list tbody input[type=checkbox]").attr("checked", true);
    }
    else {
        $("#uplink_wildcard_list tbody input[type=checkbox]").attr("checked", false);
    }     
}

function loop_wildcard_task_delete_click(a){
        ShowDiaglog("del_loop_task_dlg", page.res[page.pagelan]["xid"]["val"]["del_loop_task_confirm"], {
        	show:function(){
                var h5 = $("#del_loop_task_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-trash"></i>' + h5.text());    
        	},
        	ok:function(){
                $("#del_loop_task_dlg").modal("hide");
               
                var uuid = $(a).attr("value");
                var rs = xmlrpc_service.tecs.vnet["wcdeltask"].cfg(xmlrpc_session, VNET_LIB_WC_TASK_LOOP,uuid);                     

                ShowNetSynRpcInfo(rs);            
                RefreshLoopBackTask();
        	}
        });
}
function uplink_wildcard_task_delete_click(a){
        ShowDiaglog("del_uplink_task_dlg", page.res[page.pagelan]["xid"]["val"]["del_uplink_task_confirm"], {
        	show:function(){
                var h5 = $("#del_uplink_task_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-trash"></i>' + h5.text());    
        	},
        	ok:function(){
                $("#del_uplink_task_dlg").modal("hide");
               
                var uuid = $(a).attr("value");
                var rs = xmlrpc_service.tecs.vnet["wcdeltask"].cfg(xmlrpc_session,VNET_LIB_WC_TASK_UPLINK , uuid);                     

                ShowNetSynRpcInfo(rs);       
                RefreshLoopUplinkTask();
        	}
        });
}
function RefreshLoopBackTask(){
    $.getJSON("../php/wildcard_loop_task.php",function(json){
        wildcardTaskTable.fnClearTable();

        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["2",json[1]]);
            return;
        }
        var loopTaskData = json[1];
        for(var i=0; i<json[1].length; i++){
            if(json[1][i][5] == 1){
                loopTaskData[i][5] = "<a onclick='loop_wildcard_task_delete_click(this)' value='"+ json[1][i][6] +"'>delete</a>";
            }
            else if(json[1][i][5] == 0){
                 loopTaskData[i][5] = "";
            }
        }
        wildcardTaskTable.fnAddData(loopTaskData);
    });
}
function RefreshLoopUplinkTask(){
    $.getJSON("../php/wildcard_loop_uplink_task.php", function (json){
        wildcardTaskTable.fnClearTable();

        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["2", json[1]]);
            return;
        }

       var uplinkTaskData = json[1];
        for (var i = 0; i < json[1].length; i++) {
            if(json[1][i][5] == 1){
                uplinkTaskData[i][5] = "<a onclick='uplink_wildcard_task_delete_click(this)' value='"+ json[1][i][6] +"'>delete</a>";
            }
            else if(json[1][i][5] == 0){
                 uplinkTaskData[i][5] = "";
            }
            }

        wildcardTaskTable.fnAddData(uplinkTaskData);
    });
}

var vna_uuid;

function set_loop_click(a, i){
    if($(a).attr("value") == 1){
        var rs = xmlrpc_service.tecs.vnet["loopport"].cfg(xmlrpc_session, vna_uuid, loop[i]["uuid"], loop[i]["name"], EN_UNSET_LOOPPORT);
        if(rs[0] == 0){
            portTable.fnDraw();
        }
        else {
            ShowNotyRpcErrorInfo(rs);
        }
    }
    else if($(a).attr("value") == 0){
        var rs = xmlrpc_service.tecs.vnet["loopport"].cfg(xmlrpc_session, vna_uuid, loop[i]["uuid"], loop[i]["name"], EN_SET_LOOPPORT);
        if(rs[0] == 0){
            portTable.fnDraw();
        }
        else {
            ShowNotyRpcErrorInfo(rs);
        }
    }

    return;
}

var loop = Array();
function port_retrive_data(sSource,aoData,fnCallback){
	aoData.push({"name":"sVNAid","value":vna_uuid});

    $.getJSON(sSource,aoData,function(json){
	    
	    var portInfo = json.data;
		
		if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
            return;
        }
		
		for(var i = 0;i < portInfo.aaData.length;i++){
            loop[i] = Array();
            loop[i]["uuid"] = json.data.aaData[i][8];
            loop[i]["name"] = json.data.aaData[i][0];
            var setLoopLink = "";
            if(portInfo.aaData[i][1] != ""){
                setLoopLink = page.res[page.pagelan].type.YesOrNo[portInfo.aaData[i][1]]+ "(<a href='javascript:void(0)' value='" + portInfo.aaData[i][1] + "' onclick='set_loop_click(this," 
                    + i + ")'>" + page.res[page.pagelan].type.LoopOpr[portInfo.aaData[i][1]] + "</a>)";
            }
            portInfo.aaData[i][1] = setLoopLink;
            portInfo.aaData[i][8] = json.data.aaData[i][9]; 
        }
		
		fnCallback(portInfo);
	});
}

function trunk_retrive_data(sSource,aoData,fnCallback){
	aoData.push({"name":"sVNAid","value":vna_uuid});

    $.getJSON(sSource,aoData,function(json){
	    var trunkInfo = json.data;
		
		if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
            return;
        }
        for(var i=0; i<json.data.aaData.length; i++){
            trunkInfo["aaData"][i][1] = page.res[page.pagelan].type.BondMode[json.data.aaData[i][1]] ;
            trunkInfo["aaData"][i][2] = json.data.aaData[i][2].toString().replace(",","<br/>");
            trunkInfo["aaData"][i][3] = json.data.aaData[i][3].toString().replace(",","<br/>");
        }
		fnCallback(trunkInfo);
	});
}

function loop_retrive_data(sSource,aoData,fnCallback){
    aoData.push({"name":"sVNAid","value":vna_uuid});
    $.getJSON(sSource,aoData,function(json){
        if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
            return;
        }

		var ackdata = json.data;

        for(var i = 0;i < ackdata.aaData.length;i++){ 
            ackdata.aaData[i][0] = '<input type="checkbox" value=' + '"' + ackdata.aaData[i]["port_uuid"] + '" />';
		//	ackdata.aaData[i][1] = ackdata.aaData[i]["vna_uuid"];
            ackdata.aaData[i][1] = ackdata.aaData[i]["port_name"];
        }	

		fnCallback(ackdata);
	});
}

function vna_portinfo_click(vna){
    var port_info_dlg_html = 
    '<ul class="nav nav-tabs" id="portInfoTab"> \
        <li class="active"><a href="#phyPort" id="phyPortNav"><OamStaticUI bindattr="text" maptype="display" bindvalue="Port"/></a></li> \
        <li><a href="#trunkPort" id="trunkPortNav"><OamStaticUI bindattr="text" maptype="display" bindvalue="Trunk"/></a></li> \
        <li><a href="#loopPort" id="loopPortNav"><OamStaticUI bindattr="text" maptype="display" bindvalue="Loop"/></a></li> \
        <li><a href="#sriovPort" id="sriovPortNav"><OamStaticUI bindattr="text" maptype="display" bindvalue="SRIOV_VF"/></a></li> \
    </ul>  \
    <div class="tab-content"> \
        <div class="tab-pane active" id="phyPort">1</div> \
        <div class="tab-pane" id="trunkPort">2</div> \
        <div class="tab-pane" id="loopPort">4</div> \
        <div class="tab-pane" id="sriovPort">5</div> \
    </div>';
    
    vna_uuid = $(vna).attr("value");
	
    ShowDiaglog("port_info_dlg", port_info_dlg_html, {
        init:function(){
            $('#portInfoTab a:first').tab('show');
            $('#portInfoTab a').click(function (e) {
                e.preventDefault();
                $(this).tab('show');
            });       

            $("#phyPort").html(new PortView().render().el);
            $("#trunkPort").html(new TrunkView().render().el);
            $("#loopPort").html(new LoopView().render().el);
            $("#sriovPort").html(new SriovView().render().el);        

            portTable = $("#portList").dataTable({
                "sPaginationType": "bootstrap",
                "oLanguage": tableLanguage[page.pagelan],
	            "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 1 ] },
                                 { "bSortable": false, "aTargets": [ 8 ] }],
                "bServerSide": true,
                "bFilter":false,
                "alternatively": "destory",
                "sAjaxSource": "../php/net_port_physical.php",
                "fnServerData": port_retrive_data
            });

            trunkTable = $("#trunkList").dataTable({
                "sPaginationType": "bootstrap",
                "oLanguage": tableLanguage[page.pagelan],
                "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 3 ] }
                                 ],
                "bServerSide": true,
                "bFilter":false,
                "alternatively": "destory",
                "sAjaxSource": "../php/net_port_bond.php",
                "fnServerData": trunk_retrive_data
            });
			
			loopTable = $("#loopList").dataTable({
                "sPaginationType": "bootstrap",
                "oLanguage": tableLanguage[page.pagelan],
	            "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] }],
                "bServerSide": true,
                "bFilter":false,
                "alternatively": "destory",
                "sAjaxSource": "../php/net_port_loop.php",
                "fnServerData": loop_retrive_data
            });
			
            sriovTable = $("#sriovList").dataTable({
                "sPaginationType": "bootstrap",
                "oLanguage": tableLanguage[page.pagelan],
                "bServerSide": true,
                "bFilter":false,
                "sAjaxSource": "../php/net_port_sriov.php",
                "fnServerData": RetrieveSriovData,
                "alternatively" : "destory"
            }); 

            $("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            }); 
        },
        show: function(){
            portTable.fnDraw();
            trunkTable.fnDraw();
            loopTable.fnDraw();
            sriovTable.fnDraw();
        },
        close:function(){
            $("#port_info_dlg").modal("hide");
        }
    });   
}
