var OP_ADD = 1;
var OP_DEL = 2;
var OP_MOD = 3;
var OP_MOD_NATIVEVLAN = 6;

var netplane_selected = "";

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

function RetrievePortGroupData( sSource, aoData, fnCallback ){
    $("#portgroup_list input[type=checkbox]").attr("checked", false);
    aoData.push({"name":"netplane","value":GetFormatString(netplane_selected)});

    $.getJSON( sSource, aoData, function (json) {
        if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
            return;
        }		

        var ackdata = json.data;

        for(var i = 0;i < ackdata.aaData.length;i++){ //ackdata.aaData[i][0] = GetItemCheckbox(uuids[i], currNetplaneState);
            //ackdata.aaData[i][0] = '<input type="checkbox" value=' + '"' + ackdata.aaData[i]["uuid"] + '" >';
            ackdata.aaData[i][0] = GetItemCheckbox(ackdata.aaData[i]["uuid"], currPortgroupState);
            ackdata.aaData[i][1] = '<a href="javascript:void(0);" onclick="ShowOnePgInfo(this)">'+ ackdata.aaData[i]["name"] + '</a>';
            ackdata.aaData[i][2] = page.res[page.pagelan].type.PortgroupType[ackdata.aaData[i]["pg_type"]]; 
            ackdata.aaData[i][3] = ackdata.aaData[i]["mtu"];
            ackdata.aaData[i][4] = page.res[page.pagelan].type.IsolateType[ackdata.aaData[i]["isolateType"]];     
            //ackdata.aaData[i][5] = page.res[page.pagelan].type.SwitchPortMode[ackdata.aaData[i]["switchport_mode"]];
            ackdata.aaData[i][5] = ackdata.aaData[i]["netplane"];
            ackdata.aaData[i][6] = ackdata.aaData[i]["desc"];            
			var oplink = '<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["modify"] 
                         + '" onclick="OnModifyPgClick(this)"><i class="icon-edit"></i></a>';
            
            if(ackdata.aaData[i]["pg_type"] == 20 && ackdata.aaData[i]["switchport_mode"]==0 && ackdata.aaData[i]["isolateType"]==0 ){
                oplink += '| <a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.display["VLANrange"] 
                         +'" native_vlan_num = ' +ackdata.aaData[i]["native_vlan_num"] +'  onclick="OnModifyPgVlan(this)"><i class="icon-cog"></i></a>'
            }

            ackdata.aaData[i][7] = oplink;
        }
				
        fnCallback(ackdata);
        AvoidGotoBlankPage(pgTable);
        $(".tooltip").hide();
        $('[data-rel="tooltip"]').tooltip({"placement":"bottom"});
        
        $("#portgroup_last_refresh_time").html(GetLastUpdateTime());
    });
}

function InitSelectNetplane(sl, netplane, cb){
    sl.empty();
    sl.attr("data-placeholder", page.res[page.pagelan].type.display["select_netplane"]);

    $.getJSON("../php/portgroup_netplane.php", function (json) {
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

/*--main--*/
$(function(){
    /*--table--*/
    pgTable = $("#portgroup_list").dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] },
                         { "bSortable": false, "aTargets": [ 6 ] },
                         { "bSortable": false, "aTargets": [ 7 ] }
                         ],
        "alternatively" : "destory",
        "bProcessing": false,
        "bServerSide": true,
        "sAjaxSource": "../php/net_portgroup.php",
        "fnServerData": RetrievePortGroupData
    });

    $.getJSON("../php/portgroup_records.php",function(json){
        if(json[0] > 0){
            $('#tabsPortgroup a:eq(1)').tab('show');
        }
        else{
    $('#tabsPortgroup a:first').tab('show');
        }
    });
    
    //$('#tabsPortgroup a:first').tab('show');
    $('#tabsPortgroup a').click(function (e) {
        e.preventDefault();
        $(this).tab('show');
    });

    $("#create_portgroup_link").click(function(e){
        $('#tabsPortgroup a').eq(1).click();
        $("#pgCreate").click();
    });

    $('#create_switch_link').click(function(e){
        mainmenu_item_click(e,$(this));
    });      

    $('#create_logicnet_link').click(function(e){
        mainmenu_item_click(e,$(this));
    });  
    $('#create_switch_link').click(function(e){
        mainmenu_item_click(e,$(this));
    });  
    
    $("#pgRefresh").click(ShowPortgroup);
    $("#pgCreate").click(CreatePortgroup);
    $("#pgRemove").click(RemovePortgroup);
    $("#pgSelecltAll").click(OnPgSelectAll);
  
	// OamUi静态显示部分初始化
    $("OamStaticUI").each(function(){
  	    SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
    });

    InitSelectNetplane($("#selectNetplane"), "", SelectNeplaneChange);
    
    portgroup_timer_id    = setInterval(ShowPortgroup, 30000);
});

function SelectNeplaneChange(){
    netplane_selected = $("#selectNetplane").val();
    if(netplane_selected == null) netplane_selected = "";

    ShowPortgroup();
}

var currPortgroupState = new Object();
function ShowPortgroup(){
    //pgTable.fnDraw();
    if($('#portgroup_list').length == 0) {
        clearInterval(portgroup_timer_id);
        return;
    }
    ClearCurSelItem(currPortgroupState);  
    RecordSelItem(pgTable, "portgroup_list", currPortgroupState);   
    GotoCurrentPageOfDataTable(pgTable);
}

var create_portgroup_dlg_html = 
    '<ul class="nav nav-tabs" id="portgroupInfoTab"> \
         <li><a href="#tabBaseinfo" id="basenav"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt1_baseinfo"/></a></li> \
         <li><a href="#tabAdvance" id="advnav"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt3_advance"/></a></li> \
     </ul> \
     <div class="tab-content"> \
        <div class="tab-pane active" id="tabBaseinfo"> \
            <form class="form-horizontal"> <fieldset>  \
                <div class="control-group"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt1_name"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"><input id="pgName" popover="name_format" maxlength="64" type="text"/></div> \
                </div> \
                <div class="control-group"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt1_type"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"> \
                        <select name="select" id="pgType"></select>\
                    </div> \
                </div> \
                <div class="control-group"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt1_netplan"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"><select name="select" id="pgNetplane"></select></div> \
                </div> \
                <div class="control-group"> \
                    <label class="control-label">SDN</label> \
                    <div class="controls"><select name="select" id="SDN"></select></div> \
                </div> \
                <div class="control-group" style="display:none;"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt3_promisc"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"> \
                        <select name="select" id="pgPromisc"></select> \
                    </div> \
                </div> \
                <div class="control-group"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt3_mtu"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"><input id="pgMtu" popover="mtu_format" maxlength="6" type="text" value="1500"/></div> \
                </div> \
                <div class="control-group"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt3_isolate"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"> \
                        <select name="select" id="pgIsolate"></select> \
                    </div> \
                </div> \
                <div class="control-group"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt2_switch_port_mode"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"> \
                        <select name="select" id="pgSwitchPortMode"></select> \
                    </div> \
                </div> \
                <div class="control-group"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt2_vlan_id"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"><input id="pgNativeVlan" popover="native_vlan_format" maxlength="4" type="text"></input></div> \
                </div> \
                <div class="control-group"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt2_access_vlan"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"><input id="pgAccessVlan" popover="access_vlan_format" maxlength="4" type="text"/></div> \
                </div> \
                <div class="control-group"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt2_isolate_vlan"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"><input id="pgIsolateVlan" popover="isolate_vlan_format" maxlength="4" type="text"/></div> \
                </div> \
                <div class="control-group"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt2_vxlan_segment"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"><input id="pgVxlanSegment" popover="isolate_vxlan_segment_format" maxlength="8" type="text" /></div> \
                </div> \
            </fieldset> </form> \
        </div> \
        <div class="tab-pane" id="tabAdvance">   \
            <form class="form-horizontal"> <fieldset>  \
                <div class="control-group" style="display:none;"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt3_acl"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"><input id="pgAcl" maxlength="6" type="text"/></div> \
                </div> \
                <div class="control-group" style="display:none;"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt3_qos"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"><input id="pgQos" maxlength="6" type="text"/></div> \
                </div>  \
                <div class="control-group" style="display:none;"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt3_prior"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"><input id="pgPrior" maxlength="6" type="text"/></div> \
                </div>  \
                <div class="control-group" style="display:none;"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt1_version"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"> \
                    <select name="select" id="pgVersion"> \
                        <option value="1">1 \
                    </select> \
                    </div> \
                </div> \
                <div class="control-group" style="display:none;"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt3_mgr_id"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"><input id="pgMgrId" maxlength="6" type="text"/></div> \
                </div> \
                <div class="control-group"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt1_description"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"><textarea type="text" maxlength="128" rows="6" id="pgDesc"></textarea><p id="pgDescTip"></p></div> \
                </div> \
            </fieldset> </form> \
        </div> \
     </div> '; 

function CreatePortgroup() {
    ShowDiaglog("create_portgroup_dlg", create_portgroup_dlg_html, {
        init:function(){
            $('#portgroupInfoTab a:first').tab('show');
            $('#portgroupInfoTab a').click(function (e) {
                e.preventDefault();
                $(this).tab('show');
            });

            $("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });   
            
            $("#pgType").append('<option value="0">' +  page.res[page.pagelan].type.PortgroupType["0"]);
            $("#pgType").append('<option value="10">' +  page.res[page.pagelan].type.PortgroupType["10"]);
            $("#pgType").append('<option value="11">' +  page.res[page.pagelan].type.PortgroupType["11"]);
            $("#pgType").append('<option value="20">' +  page.res[page.pagelan].type.PortgroupType["20"]);
            $("#pgType").append('<option value="21">' +  page.res[page.pagelan].type.PortgroupType["21"]);

            //$("#pgSwitchPortMode").append('<option value="0">' +  page.res[page.pagelan].type.SwitchPortMode["0"]);
            //$("#pgSwitchPortMode").append('<option value="1">' +  page.res[page.pagelan].type.SwitchPortMode["1"]);
            //$("#pgSwitchPortMode").append('<option value="2">' +  page.res[page.pagelan].type.SwitchPortMode["2"]);

            $("#SDN").append('<option value="0">'+page.res[page.pagelan].type.SDNType["0"]);
            $("#SDN").append('<option value="1">'+page.res[page.pagelan].type.SDNType["1"]);

            $("#pgPromisc").append('<option value="0">' +  page.res[page.pagelan].type.PortgroupPromisc["0"]);
            $("#pgPromisc").append('<option value="1">' +  page.res[page.pagelan].type.PortgroupPromisc["1"]);
            $("#pgPromisc").append('<option value="2">' +  page.res[page.pagelan].type.PortgroupPromisc["2"]);
            $("#pgPromisc").append('<option value="3">' +  page.res[page.pagelan].type.PortgroupPromisc["3"]);

            TogglePopover($("#create_portgroup_dlg"));
            LimitCharLenInput($("#pgDesc"),$("#pgDescTip"));   
        },
        show:function(){
            var h5 = $("#create_portgroup_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-plus"></i>' + h5.text());

            z_strInput("pgName", 64);
            z_numInput("pgMtu",1280,9000);

            z_numInput("pgNativeVlan",1,4094);
            z_numInput("pgAccessVlan",1,4094);
            z_numInput("pgIsolateVlan",1,100);
            z_numInput("pgVxlanSegment",1,16000000);


            $("#pgType").unbind("change").bind("change",function(){
                var pg_type = $("#pgType").val();
                $("#pgSwitchPortMode").empty();
                $("#pgIsolate").empty();

                if(pg_type == 0){
                    $("#pgSwitchPortMode").append('<option value="0">' +  page.res[page.pagelan].type.SwitchPortMode["0"]);
                    $("#pgIsolate").append('<option value="0">'+page.res[page.pagelan].type.IsolateType["0"]);
                    $("#pgIsolate").append('<option value="2">'+page.res[page.pagelan].type.IsolateType["2"]);
                }

                if( pg_type == 20){
                    $("#pgSwitchPortMode").append('<option value="0">' +  page.res[page.pagelan].type.SwitchPortMode["0"]);
                    $("#pgSwitchPortMode").append('<option value="1">' +  page.res[page.pagelan].type.SwitchPortMode["1"]);
                    $("#pgIsolate").append('<option value="0">'+page.res[page.pagelan].type.IsolateType["0"]);
                    $("#pgIsolate").append('<option value="1">'+page.res[page.pagelan].type.IsolateType["1"]);
                }

                if(pg_type == 10 || pg_type == 11 ){
                    $("#pgSwitchPortMode").append('<option value="1">' +  page.res[page.pagelan].type.SwitchPortMode["1"]);
                    $("#pgIsolate").append('<option value="0">'+page.res[page.pagelan].type.IsolateType["0"]);		   
                }
                if(pg_type == 21){
                    $("#pgSwitchPortMode").append('<option value="1">' +  page.res[page.pagelan].type.SwitchPortMode["1"]);
                    $("#pgIsolate").append('<option value="0">'+page.res[page.pagelan].type.IsolateType["0"]);
                    $("#pgIsolate").append('<option value="1">'+page.res[page.pagelan].type.IsolateType["1"]);
                }
                $("#pgIsolate").trigger("change");
                $("#pgSwitchPortMode").trigger("change");

                if($("#pgNetplane").val() == ""){
                    $("#pgIsolateVlan").attr("disabled", true);                       
                    $("#pgIsolateVlan").val(1);
                }
                else{
                    $("#pgIsolateVlan").attr("disabled", false);                         
                }                
            });

            $("#pgType").trigger("change");    

            $("#pgSwitchPortMode").unbind("change").bind("change", function(){
                var pg_type = $("#pgType").val();
                var switchPortMode = $("#pgSwitchPortMode").val();
                
                $("#pgNativeVlan").parent().parent().hide();
                $("#pgAccessVlan").parent().parent().hide();
                $("#pgIsolateVlan").parent().parent().hide();

                if(switchPortMode == 0){
                        $("#pgNativeVlan").parent().parent().show();
                    }
                    else{
                    if(pg_type ==21 && switchPortMode == 1){
                        $("#pgIsolateVlan").parent().parent().show();
                    }  
                    else {
                        $("#pgAccessVlan").parent().parent().show();
                    } 
                }             
            });    

            $("#pgSwitchPortMode").trigger("change");

            $("#pgIsolate").unbind("change").bind("change",function(){
                var isolateType = $("#pgIsolate").val();
                var pgType = $("#pgType").val();
		
                $("#pgVxlanSegment").parent().parent().hide();
                $("#pgIsolateVlan").parent().parent().hide();
		
                if(isolateType==0||isolateType==2){
                    if(isolateType ==0 && pgType == 21){
                        $("#pgSwitchPortMode").parent().parent().hide();
                        $("#pgSwitchPortMode").trigger("change");
                    }
                    else
                    {
                        $("#pgSwitchPortMode").parent().parent().show();
                        $("#pgSwitchPortMode").trigger("change");
                    }
                }
                else if(isolateType==1){
                    $("#pgSwitchPortMode").parent().parent().hide();
                    $("#pgNativeVlan").parent().parent().hide();
                    $("#pgAccessVlan").parent().parent().hide();
                    $("#pgIsolateVlan").parent().parent().hide();
                    if(pgType==20){
                        $("#pgVxlanSegment").parent().parent().show();
                    }
                    else if(pgType==21){
                        $("#pgIsolateVlan").parent().parent().show();
                    }
                }
            });
	    
            $("#pgIsolate").trigger("change");

            $.getJSON("../php/portgroup_netplane.php", function (json) {
                if(json[0] != 0){ 
                    ShowNotyRpcErrorInfo(["11", json[1]]);
                    return;
                }

                $("#pgNetplane").empty();
                $("#pgNetplane").append("<option value=''>");

                for(var i = 0;i < json[1].length;i++){ 
                    $("#pgNetplane").append("<option value='" + json[1][i].uuid + "'>" + json[1][i].name);
                }

                $("#pgNetplane").unbind("change").bind("change", function(){
                    if($("#pgNetplane").val() == ""){
                        $("#pgIsolateVlan").attr("disabled", true);                       
                        $("#pgIsolateVlan").val(1);                            
                    }
                    else{
                        $("#pgIsolateVlan").attr("disabled", false);                         
                    }
                });

                $("#pgNetplane").trigger("change");
            });
        },
        ok:function(){
            var name = $("#pgName").val();
            var type = parseInt($("#pgType").val());
            var desc = $("#pgDesc").val();
            var mode = parseInt($("#pgSwitchPortMode").val());
            var netplane = $("#pgNetplane").val();
            var sdn = parseInt($("#SDN").val());
            var is_netplane_cfg = 0;
            if(netplane != ""){
                is_netplane_cfg = 1;
            }
            var promisc = parseInt($("#pgPromisc").val());
            var native_vlan = parseInt($("#pgNativeVlan").val());
            var access_vlan = parseInt($("#pgAccessVlan").val());
            var isolate_vlan = parseInt($("#pgIsolateVlan").val());
            var mtu = parseInt($("#pgMtu").val());
            var acl = $("#pgAcl").val();
            var qos = $("#pgQos").val();
            var prior = $("#pgPrior").val();
            var ver = parseInt($("#pgVersion").val());
            var mgr = $("#pgMgrId").val();
            var isolateType = parseInt($("#pgIsolate").val());
            var segment = parseInt($("#pgVxlanSegment").val());

            if(isNaN(native_vlan)){
                native_vlan = 0;
            }            

            if(isNaN(access_vlan)){
                access_vlan = 0;
            }

            if(isNaN(isolate_vlan)){
                isolate_vlan = 0;
            }     

            if(isNaN(segment)){
                segment = 0;
            }    
	   
            $(".control-group").removeClass("error");
            if(name == ""){
                $("#pgName").parent().parent().addClass("error");                
                return;
            }
if(isolateType != 1){
            if(mode == 0) {
                if(isNaN(native_vlan) || native_vlan < 1 || native_vlan > 4094){
                    $("#pgNativeVlan").parent().parent().addClass("error");
                    return;
                }
            }
            else if(type == 21 && mode==1){
                if(isNaN(isolate_vlan) || isolate_vlan < 1 || isolate_vlan > 100){
                    $("#pgIsolateVlan").parent().parent().addClass("error");
                    return;
                }
            }  
            else {
                if(isNaN(access_vlan) || access_vlan < 1 || access_vlan > 4094){
                    $("#pgAccessVlan").parent().parent().addClass("error");
                    return;
                }
            }   
}            
            if(type==20 && isolateType==1 ){
                if(segment==0 ){
                    $("#pgVxlanSegment").parent().parent().addClass("error");
                    return;
                }	
            }

            var rs = xmlrpc_service.tecs.vnet.portgroup.cfg(xmlrpc_session, name, "", type, desc, mode, is_netplane_cfg, netplane,sdn,
                        promisc, 0, native_vlan, access_vlan, isolate_vlan, mtu, acl, qos, prior, "", "", 0, 0, "", "", "", "", 
                        ver, mgr, "", isolateType,segment,OP_ADD);

            if(rs[0] == 0){
                ShowPortgroup();
                $("#create_portgroup_dlg").modal("hide");
            }
            else {
                ShowNotyRpcErrorInfo(rs);
            }
        }
    });
}

function RemovePortgroup() {
    var vCheckedbox = $(pgTable.fnGetNodes()).find(":checkbox:checked");

    if(vCheckedbox.size() > 0) {
        ShowDiaglog("delete_portgroup_dlg", page.res[page.pagelan]["xid"]["val"]["delete_portgroup_confirm"], {
            show:function(){
                var h5 = $("#delete_portgroup_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-trash"></i>' + h5.text());
            },
            ok:function(){
                vCheckedbox.each(function(){
                    var uuid = $(this).attr("value");
                    var name = $(this).parent().parent().find("td").eq(1).text();

                    var rs = xmlrpc_service.tecs.vnet.portgroup.cfg(xmlrpc_session, name, uuid, 0, "", 0, 0, "", 0, 0,0, 2, 2, 2, 
                        0, "", "", "", "", "", 0, 0, "", "", "", "", 0, "", "", 0, 0, OP_DEL);   
                    //ShowSynRpcInfo(rs);
                    ShowNetSynRpcInfo(rs);
                });
                ShowPortgroup();
                $("#delete_portgroup_dlg").modal("hide");
            }
        });
    }
    else {
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]); 
    }
}

var portgroup_info_dlg_html = 
    '<div class="container-fluid">  \
        <div class="row-fluid">   \
            <div class="span4"> \
                <div class="well nav-collapse sidebar-nav"> \
                    <ul class="nav nav-tabs nav-stacked netplane-menu"> \
                        <li><a href="#" id="pgbase"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt1_baseinfo"/></a></li> \
                        <li><a href="#" id="pgIsolateInfo"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt2_isolateType"/></a></li> \
                        <li><a href="#" id="pgadv"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt3_advance"/></a></li> \
                    </ul> \
                </div> \
            </div> \
            <div class="span8"> \
                <div class="row-fluid"> \
                    <div class="span12" id="pg_right_content"> </div>\
                </div> \
            </div> \
        </div> \
    </div>';

function ShowOnePgInfo(pg){
    var pg_name = $(pg).text();
    var pg_desc = $(pg).parent().parent().find("td").eq(6).text();
    var pg_netplane = $(pg).parent().parent().find("td").eq(5).text();
    var pg_uuid = $(pg).parent().parent().find(":checkbox").eq(0).attr("value");

    ShowDiaglog("portgroup_info_dlg", portgroup_info_dlg_html, { 
        show:function(){
            $("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });  

            var h5 = $("#portgroup_info_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-zoom-in"></i>' + h5.text() + ' - ' + pg_name);

            var param = [];
            param.push({"name":"pg_uuid", "value":pg_uuid});            

            $.getJSON("../php/portgroup_info.php", param, function (json) {
                if(json[0] != 0){ 
                    ShowNotyRpcErrorInfo(["11", json[1]]);
                    return;
                }

                var baseinfo = '';
                baseinfo += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
                baseinfo += '   <tbody>';
                baseinfo += '       <tr>';
                baseinfo += '           <td width="30%">' + page.res[page.pagelan].type.display.dt1_name + '</td>';
                baseinfo += '           <td>' + pg_name + '</td>';
                baseinfo += '       </tr>';

                baseinfo += '       <tr>';
                baseinfo += '           <td width="30%">' + page.res[page.pagelan].type.display.dt1_netplan + '</td>';
                baseinfo += '           <td>' + pg_netplane + '</td>';
                baseinfo += '       </tr>';
                
                baseinfo += '       <tr>';
                baseinfo += '           <td width="30%">' + page.res[page.pagelan].type.display.dt1_type + '</td>';
                baseinfo += '           <td>' + page.res[page.pagelan].type.PortgroupType[json[1]["pg_type"]] + '</td>';
                baseinfo += '       </tr>';
/*
                baseinfo += '       <tr>';
                baseinfo += '           <td width="30%">' + page.res[page.pagelan].type.display.dt2_switch_port_mode + '</td>';
                baseinfo += '           <td>' + page.res[page.pagelan].type.SwitchPortMode[json[1]["switchport_mode"]] + '</td>';
                baseinfo += '       </tr>';
*/
 /*               baseinfo += '       <tr>';
                baseinfo += '           <td width="30%">' + page.res[page.pagelan].type.display.dt3_promisc + '</td>';
                baseinfo += '           <td>' + page.res[page.pagelan].type.PortgroupPromisc[json[1]["promiscuous"]] + '</td>';
                baseinfo += '       </tr>';
*/
                baseinfo += '       <tr>';
                baseinfo += '           <td width="30%">' + page.res[page.pagelan].type.display.dt3_mtu + '</td>';
                baseinfo += '           <td>' + json[1]["mtu"] + '</td>';
                baseinfo += '       </tr>';
                baseinfo += '   </tbody>';
                baseinfo += '</table>'; 
                               
                $("#pgbase").unbind("click").bind("click", function(){
                    $("#portgroup_info_dlg").find("ul").find("li").removeClass("active");
                    $("#pgbase").parent().addClass("active");

                    $("#pg_right_content").html(baseinfo);
                });        

                var isolateType = json[1]["isolate_type"];                
                var isolateInfo = '';
                if(isolateType ==1){
            		var vxlaninfo = 'VXLAN:';
                    vxlaninfo += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
                    vxlaninfo +=' <tbody>';
                    vxlaninfo +=' <tr>';
                    if(json[1]["pg_type"] == 20){
                        vxlaninfo +=' <td>' + "segmentID" + '</td>';
                        vxlaninfo +=' <td>' + json[1]["segment"] +'</td>';
                    }
                    else if(json[1]["pg_type"] == 21){
                        vxlaninfo +=' <td>' + page.res[page.pagelan].type.display.dt2_isolate_vlan + '</td>';
                        vxlaninfo +=' <td>' + json[1]["isolate_no"] +'</td>';
                    }
                    vxlaninfo +=' </tr>';
                    vxlaninfo +=' </tbody>';
                    vxlaninfo +=' </table>';
                    isolateInfo = vxlaninfo;
                }
                else{

                    var vlanifno = 'VLAN:';
                    vlanifno += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
                    vlanifno += '   <tbody>';

                    if(json[1]["vlan_range"].length > 0){
                        vlanifno += '       <tr>';
                        vlanifno += '           <td width="30%">' + page.res[page.pagelan].type.display["VLANrange"] + '</td>';
                        vlanifno += '           <td>'; 
                        for(var i = 0;i < json[1]["vlan_range"].length;i++){ 
                            vlanifno += json[1]["vlan_range"][i][0] + ' ~ ' + json[1]["vlan_range"][i][1] + '<br />';
                        }
                        vlanifno += '           </td>';
                        vlanifno += '       </tr>';
                    }
                    
                    vlanifno += '       <tr>';
                    vlanifno += '           <td width="30%">' + page.res[page.pagelan].type.display.dt2_switch_port_mode + '</td>';
                    vlanifno += '           <td>' + page.res[page.pagelan].type.SwitchPortMode[json[1]["switchport_mode"]] + '</td>';
                    vlanifno += '       </tr>';
                    
                    if(json[1]["switchport_mode"] == 0){
                        vlanifno += '       <tr>';
                        vlanifno += '           <td width="30%">' + page.res[page.pagelan].type.display.dt2_vlan_id + '</td>';
                        vlanifno += '           <td>' + json[1]["native_vlan_num"] + '</td>';
                        vlanifno += '       </tr>';
                    }  
                    else {
                        if(json[1]["pg_type"] == 21){
                            vlanifno += '       <tr>';
                            vlanifno += '           <td width="30%">' + page.res[page.pagelan].type.display.dt2_isolate_vlan + '</td>';
                            vlanifno += '           <td>' + json[1]["isolate_no"] + '</td>';
                            vlanifno += '       </tr>';
                        }
                        else {
                            vlanifno += '       <tr>';
                            vlanifno += '           <td width="30%">' + page.res[page.pagelan].type.display.dt2_access_vlan + '</td>';
                            vlanifno += '           <td>' + json[1]["access_vlan_num"] + '</td>';
                            vlanifno += '       </tr>';
                        }
                    } 
                    if(isolateType ==2){
                            
                            vlanifno += '       <tr>';
                            vlanifno += '           <td width="30%">VXLAN:</td>';
                            vlanifno += '           <td></td>';
                            vlanifno += '       </tr>';
                            
                            vlanifno += '       <tr>';
                            vlanifno += '           <td width="30%"> All SegmentId</td>';
                            vlanifno += '           <td></td>';
                            vlanifno += '       </tr>';
                    
                    } 
                    vlanifno += '   </tbody>';
                    vlanifno += '</table>';  
                   
                    isolateInfo += vlanifno;            
                }

                $("#pgIsolateInfo").unbind("click").bind("click", function(){
                    $("#portgroup_info_dlg").find("ul").find("li").removeClass("active");
                    $("#pgIsolateInfo").parent().addClass("active");
                    $("#pg_right_content").html(isolateInfo);
                });  
                
                var advinfo = '';
                advinfo += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
                advinfo += '   <tbody>';
                /*advinfo += '       <tr>';
                advinfo += '           <td>' + page.res[page.pagelan].type.display.dt3_acl + '</td>';
                advinfo += '           <td>' + json[1]["acl"] + '</td>';
                advinfo += '       </tr>';
                
                advinfo += '       <tr>';
                advinfo += '           <td>' + page.res[page.pagelan].type.display.dt3_qos + '</td>';
                advinfo += '           <td>' + json[1]["qos"] + '</td>';
                advinfo += '       </tr>';
                
                advinfo += '       <tr>';
                advinfo += '           <td>' + page.res[page.pagelan].type.display.dt3_prior + '</td>';
                advinfo += '           <td>' + json[1]["prior"] + '</td>';
                advinfo += '       </tr>';

                advinfo += '       <tr>';
                advinfo += '           <td>' + page.res[page.pagelan].type.display.dt1_version + '</td>';
                advinfo += '           <td>' + json[1]["ver"] + '</td>';
                advinfo += '       </tr>';

                advinfo += '       <tr>';
                advinfo += '           <td>' + page.res[page.pagelan].type.display.dt3_mgr_id + '</td>';
                advinfo += '           <td>' + json[1]["mgr"] + '</td>';
                advinfo += '       </tr>';*/

                advinfo += '       <tr>';
                advinfo += '           <td width="30%">' + page.res[page.pagelan].type.display.dt1_description + '</td>';
                advinfo += '           <td>' + pg_desc + '</td>';
                advinfo += '       </tr>';                
                advinfo += '   </tbody>';
                advinfo += '</table>';                 
                $("#pgadv").unbind("click").bind("click", function(){
                    $("#portgroup_info_dlg").find("ul").find("li").removeClass("active");
                    $("#pgadv").parent().addClass("active");

                    $("#pg_right_content").html(advinfo);
                });

                $("#pgbase").trigger("click");
            });
        },
        close:function(){
            $("#portgroup_info_dlg").modal("hide");
        }   
    });
}

var modify_portgroup_dlg_html = 
    '<ul class="nav nav-tabs" id="portgroupInfoTabSet"> \
         <li><a href="#tabBaseinfoSet" id="basenav"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt1_baseinfo"/></a></li> \
         <li><a href="#tabAdvanceSet" id="advnav"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt3_advance"/></a></li> \
     </ul> \
     <div class="tab-content"> \
        <div class="tab-pane active" id="tabBaseinfoSet"> \
            <form class="form-horizontal"> <fieldset>  \
                <!--div class="control-group"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt1_name"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"><input id="pgNameSet" maxlength="64" type="text"/></div> \
                </div--> \
                <div class="control-group" style="display:none;"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt1_type"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"> \
                        <select name="select" id="pgTypeSet"></select> \
                    </div> \
                </div> \
                <div class="control-group"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt1_netplan"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"><select name="select" id="pgNetplaneSet"></select></div> \
                </div> \
                <div class="control-group" style="display:none;"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt3_promisc"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"> \
                        <select name="select" id="pgPromiscSet"></select> \
                    </div> \
                </div> \
                <div class="control-group"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt3_mtu"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"><input id="pgMtuSet" popover="mtu_format" maxlength="6" type="text" value="1500"/></div> \
                </div> \
                <div class="control-group" style="display:none;"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt3_isolate"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"> \
                        <select name="select" id="pgIsolateSet"></select> \
                    </div> \
                </div> \
                <div class="control-group"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt2_switch_port_mode"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"> \
                        <select name="select" id="pgSwitchPortModeSet"> </select> \
                    </div> \
                </div> \
                <div class="control-group"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt2_vlan_id"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"><input id="pgNativeVlanSet" popover="native_vlan_format" maxlength="4" type="text"></input></div> \
                </div> \
                <div class="control-group"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt2_access_vlan"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"><input id="pgAccessVlanSet" popover="access_vlan_format" maxlength="4" type="text"/></div> \
                </div> \
                <div class="control-group"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt2_isolate_vlan"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"><input id="pgIsolateVlanSet" popover="isolate_vlan_format" maxlength="4" type="text"/></div> \
                </div> \
                <div class="control-group"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt2_vxlan_segment"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"><input id="pgVxlanSegmentSet" popover="isolate_vxlan_segment_format" maxlength="8" type="text" /></div> \
                </div> \
            </fieldset> </form> \
        </div> \
        <div class="tab-pane" id="tabAdvanceSet">   \
            <form class="form-horizontal"> <fieldset>  \
                <div class="control-group" style="display:none;"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt3_acl"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"><input id="pgAclSet" maxlength="6" type="text"/></div> \
                </div> \
                <div class="control-group" style="display:none;"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt3_qos"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"><input id="pgQosSet" maxlength="6" type="text"/></div> \
                </div>  \
                <div class="control-group" style="display:none;"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt3_prior"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"><input id="pgPriorSet" maxlength="6" type="text"/></div> \
                </div>  \
                <div class="control-group" style="display:none;"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt1_version"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"> \
                    <select name="select" id="pgVersionSet"> \
                        <option value="1">1 \
                    </select> \
                    </div> \
                </div> \
                <div class="control-group" style="display:none;"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt3_mgr_id"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"><input id="pgMgrIdSet" maxlength="6" type="text"/></div> \
                </div> \
                <div class="control-group"> \
                    <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt1_description"></OamStaticUI><span><font color="red">*</font></span></label> \
                    <div class="controls"><textarea type="text" maxlength="128" rows="6" id="pgDescSet"></textarea><p id="pgDescSetTip"></p></div> \
                </div> \
            </fieldset> </form> \
        </div> \
     </div> '; 

function OnModifyPgClick(pg){
    var pg_name = $(pg).parent().parent().find("td").eq(1).text();
    var pg_desc = $(pg).parent().parent().find("td").eq(6).text();
    var pg_uuid = $(pg).parent().parent().find(":checkbox").eq(0).attr("value");

    ShowDiaglog("modify_portgroup_dlg", modify_portgroup_dlg_html, {
        init:function(){
            $('#portgroupInfoTabSet a:first').tab('show');
            $('#portgroupInfoTabSet a').click(function (e) {
                e.preventDefault();
                $(this).tab('show');
            });

            $("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });  

            $("#pgTypeSet").append('<option value="0">' +  page.res[page.pagelan].type.PortgroupType["0"]);
            $("#pgTypeSet").append('<option value="10">' +  page.res[page.pagelan].type.PortgroupType["10"]);
            $("#pgTypeSet").append('<option value="11">' +  page.res[page.pagelan].type.PortgroupType["11"]);
            $("#pgTypeSet").append('<option value="20">' +  page.res[page.pagelan].type.PortgroupType["20"]);
            $("#pgTypeSet").append('<option value="21">' +  page.res[page.pagelan].type.PortgroupType["21"]);

            $("#pgSwitchPortModeSet").append('<option value="0">' +  page.res[page.pagelan].type.SwitchPortMode["0"]);
            $("#pgSwitchPortModeSet").append('<option value="1">' +  page.res[page.pagelan].type.SwitchPortMode["1"]);
            //$("#pgSwitchPortModeSet").append('<option value="2">' +  page.res[page.pagelan].type.SwitchPortMode["2"]);

            $("#pgPromiscSet").append('<option value="0">' +  page.res[page.pagelan].type.PortgroupPromisc["0"]);
            $("#pgPromiscSet").append('<option value="1">' +  page.res[page.pagelan].type.PortgroupPromisc["1"]);
            $("#pgPromiscSet").append('<option value="2">' +  page.res[page.pagelan].type.PortgroupPromisc["2"]);
            $("#pgPromiscSet").append('<option value="3">' +  page.res[page.pagelan].type.PortgroupPromisc["3"]);

            TogglePopover($("#modify_portgroup_dlg")); 

            LimitCharLenInput($("#pgDescSet"),$("#pgDescSetTip"));                      
        },        
        show:function(){
            var h5 = $("#modify_portgroup_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-edit"></i>' + h5.text() + ' - ' + pg_name);

            z_numInput("pgMtuSet",1280,9000);
            z_numInput("pgNativeVlanSet",1,4094);
            z_numInput("pgAccessVlanSet",1,4094);
            z_numInput("pgIsolateVlanSet",1,100);
            z_numInput("pgVxlanSegmentSet",1,16000000);

            $("#pgIsolateSet").append('<option value="0">'+page.res[page.pagelan].type.IsolateType["0"]);
            $("#pgIsolateSet").append('<option value="1">'+page.res[page.pagelan].type.IsolateType["1"]);
            $("#pgIsolateSet").append('<option value="2">'+page.res[page.pagelan].type.IsolateType["2"]);
            
            $.getJSON("../php/portgroup_netplane.php", function (json) {
                if(json[0] != 0){ 
                    ShowNotyRpcErrorInfo(["11", json[1]])
                    return;
                }

                $("#pgNetplaneSet").empty();
                $("#pgNetplaneSet").append("<option value=''>");  

                for(var i = 0;i < json[1].length;i++){ 
                    for(var i = 0;i < json[1].length;i++){ 
                        $("#pgNetplaneSet").append("<option value='" + json[1][i].uuid + "'>" + json[1][i].name);
                    }                    
                }

                $("#pgNetplaneSet").unbind("change").bind("change", function(){
                    if($("#pgNetplaneSet").val() == ""){
                        $("#pgIsolateVlanSet").attr("disabled", true);
                    }
                    else{
                        $("#pgIsolateVlanSet").attr("disabled", false);                         
                    }
                });
                
            var param = [];
            param.push({"name":"pg_uuid", "value":pg_uuid});    
            $.getJSON("../php/portgroup_info.php", param, function (json) {
                if(json[0] != 0){ 
                    ShowNotyRpcErrorInfo(["11", json[1]]);
                    return;
                }    

                $("#pgSwitchPortModeSet").val(json[1]["switchport_mode"]);
                $("#pgTypeSet").val(json[1]["pg_type"]);
                $("#pgNetplaneSet").val(json[1]["netplane_uuid"]);
                $("#pgPromiscSet").val(json[1]["promiscuous"]);
                $("#pgNativeVlanSet").val(json[1]["native_vlan_num"]);
                $("#pgAccessVlanSet").val(json[1]["access_vlan_num"]);
                $("#pgIsolateVlanSet").val(json[1]["isolate_no"]);
                $("#pgMtuSet").val(json[1]["mtu"]);
                $("#pgAclSet").val(json[1]["acl"]);
                $("#pgQosSet").val(json[1]["qos"]);
                $("#pgPriorSet").val(json[1]["prior"]);
                $("#pgVersionSet").val(json[1]["ver"]);   
                $("#pgMgrIdSet").val(json[1]["mgr"]); 
                $("#pgDescSet").val(pg_desc);
                $("#pgIsolateSet").val(json[1]["isolate_type"]);
                $("#pgVxlanSegmentSet").val(json[1]["segment"]);
                   
                $("#pgTypeSet").trigger("change");                     
                $("#pgNetplaneSet").trigger("change");              
            });  
                
            $("#pgTypeSet").unbind("change").bind("change",function(){
                var pg_type = $("#pgTypeSet").val();
                var switchPortMode = $("#pgSwitchPortModeSet").val();
                var pgIsolateType = $("#pgIsolateSet").val();
                $("#pgSwitchPortModeSet").empty();
                $("#pgIsolateSet").empty();

                if(pg_type == 0){
                    $("#pgSwitchPortModeSet").append('<option value="0">' +  page.res[page.pagelan].type.SwitchPortMode["0"]);
                    $("#pgIsolateSet").append('<option value="0">'+page.res[page.pagelan].type.IsolateType["0"]);
                    $("#pgIsolateSet").append('<option value="2">'+page.res[page.pagelan].type.IsolateType["2"]);
                }

                if(pg_type == 20){
                    $("#pgSwitchPortModeSet").append('<option value="0">' +  page.res[page.pagelan].type.SwitchPortMode["0"]);
                    $("#pgSwitchPortModeSet").append('<option value="1">' +  page.res[page.pagelan].type.SwitchPortMode["1"]);
                    $("#pgIsolateSet").append('<option value="0">'+page.res[page.pagelan].type.IsolateType["0"]);
                    $("#pgIsolateSet").append('<option value="1">'+page.res[page.pagelan].type.IsolateType["1"]);
                }

                if(pg_type == 10 || pg_type == 11 ){
                    $("#pgSwitchPortModeSet").append('<option value="1">' +  page.res[page.pagelan].type.SwitchPortMode["1"]);
                    $("#pgIsolateSet").append('<option value="0">'+page.res[page.pagelan].type.IsolateType["0"]);		   
                }
                if(pg_type == 21){
                    $("#pgSwitchPortModeSet").append('<option value="1">' +  page.res[page.pagelan].type.SwitchPortMode["1"]);
                    $("#pgIsolateSet").append('<option value="0">'+page.res[page.pagelan].type.IsolateType["0"]);
                    $("#pgIsolateSet").append('<option value="1">'+page.res[page.pagelan].type.IsolateType["1"]);
                }

                $("#pgSwitchPortModeSet").val(switchPortMode);
                $("#pgIsolateSet").val(pgIsolateType);
                $("#pgIsolateSet").trigger("change");
                $("#pgSwitchPortModeSet").trigger("change");   
            });
          
            $("#pgSwitchPortModeSet").unbind("change").bind("change", function(){
                var pg_type = $("#pgTypeSet").val();
                var switchPortMode = $("#pgSwitchPortModeSet").val();
                var isolateType = $("#pgIsolateSet").val();
                
                $("#pgNativeVlanSet").parent().parent().hide();
                $("#pgAccessVlanSet").parent().parent().hide();
                $("#pgIsolateVlanSet").parent().parent().hide();

                if(switchPortMode == 0){
                        $("#pgNativeVlanSet").parent().parent().show();
                    }
                else {
                    if(pg_type == 21 && switchPortMode == 1){
                        $("#pgIsolateVlanSet").parent().parent().show();
                    }
                    else if(isolateType == 1){
                    }
                    else {
                        $("#pgAccessVlanSet").parent().parent().show();
                    }     
                }           
            });                      

            $("#pgIsolateSet").unbind("change").bind("change",function(){
                var isolateType = $("#pgIsolateSet").val();
                var pgType = $("#pgTypeSet").val();
		
                $("#pgVxlanSegmentSet").parent().parent().hide();
                $("#pgIsolateVlanSet").parent().parent().hide();
		
                if(isolateType==0||isolateType==2){
                    if(isolateType ==0 && pgType == 21){
                        $("#pgSwitchPortModeSet").parent().parent().hide();
                        $("#pgSwitchPortModeSet").trigger("change");
                    }
                    else
                    {
                        $("#pgSwitchPortModeSet").parent().parent().show();
                        $("#pgSwitchPortModeSet").trigger("change");
                    }
                }
                else if(isolateType==1){
                    $("#pgSwitchPortModeSet").parent().parent().hide();
                    $("#pgNativeVlanSet").parent().parent().hide();
                    $("#pgAccessVlanSet").parent().parent().hide();
                    $("#pgIsolateVlanSet").parent().parent().hide();
                    if(pgType==20){
                        $("#pgVxlanSegmentSet").parent().parent().show();
                    }
                    else if(pgType==21){
                        $("#pgIsolateVlanSet").parent().parent().show();
                    }
                }
            });	    
/*
            $.getJSON("../php/portgroup_netplane.php", function (json) {
                if(json[0] != 0){ 
                    ShowNotyRpcErrorInfo(["11", json[1]])
                    return;
                }

                $("#pgNetplaneSet").empty();
                $("#pgNetplaneSet").append("<option value=''>");  

                for(var i = 0;i < json[1].length;i++){ 
                    for(var i = 0;i < json[1].length;i++){ 
                        $("#pgNetplaneSet").append("<option value='" + json[1][i].uuid + "'>" + json[1][i].name);
                    }                    
                }

                $("#pgNetplaneSet").unbind("change").bind("change", function(){
                    if($("#pgNetplaneSet").val() == ""){
                        $("#pgIsolateVlanSet").attr("disabled", true);
                    }
                    else{
                        $("#pgIsolateVlanSet").attr("disabled", false);                         
                    }
                });

                var param = [];
                param.push({"name":"pg_uuid", "value":pg_uuid});    

                $.getJSON("../php/portgroup_info.php", param, function (json) {
                    if(json[0] != 0){ 
                        ShowNotyRpcErrorInfo(["11", json[1]]);
                        return;
                    }    

                    $("#pgSwitchPortModeSet").val(json[1]["switchport_mode"]);
                    $("#pgTypeSet").val(json[1]["pg_type"]);
                    $("#pgNetplaneSet").val(json[1]["netplane_uuid"]);
                    $("#pgPromiscSet").val(json[1]["promiscuous"]);
                    $("#pgNativeVlanSet").val(json[1]["native_vlan_num"]);
                    $("#pgAccessVlanSet").val(json[1]["access_vlan_num"]);
                    $("#pgIsolateVlanSet").val(json[1]["isolate_no"]);
                    $("#pgMtuSet").val(json[1]["mtu"]);
                    $("#pgAclSet").val(json[1]["acl"]);
                    $("#pgQosSet").val(json[1]["qos"]);
                    $("#pgPriorSet").val(json[1]["prior"]);
                    $("#pgVersionSet").val(json[1]["ver"]);   
                    $("#pgMgrIdSet").val(json[1]["mgr"]); 
                    $("#pgDescSet").val(pg_desc);
                    $("#pgIsolateSet").val(json[1]["isolateType"]);
                    $("#pgVxlanSegmentSet").val(json[1]["segment"]);

                    alert($("#pgIsolateSet").val());
                    $("#pgIsolateSet").trigger("change"); 
                    $("#pgTypeSet").trigger("change");                     
                    $("#pgNetplaneSet").trigger("change");              
                });    */             
            });
        },
        ok:function(){
            var type = parseInt($("#pgTypeSet").val());
            var desc = $("#pgDescSet").val();
            var mode = parseInt($("#pgSwitchPortModeSet").val());
            var netplane = $("#pgNetplaneSet").val();
            var is_netplane_cfg = 0;
            if(netplane != ""){
                is_netplane_cfg = 1;
            }
            var promisc = parseInt($("#pgPromiscSet").val());
            var native_vlan = parseInt($("#pgNativeVlanSet").val());
            var access_vlan = parseInt($("#pgAccessVlanSet").val());
            var isolate_vlan = parseInt($("#pgIsolateVlanSet").val());
            var mtu = parseInt($("#pgMtuSet").val());
            var acl = $("#pgAclSet").val();
            var qos = $("#pgQosSet").val();
            var prior = $("#pgPriorSet").val();
            var ver = parseInt($("#pgVersionSet").val());
            var mgr = $("#pgMgrIdSet").val();
            var isolateType = parseInt($("#pgIsolateSet").val());
            var segmentID = parseInt($("#pgVxlanSegmentSet").val());


            if(isNaN(native_vlan)){
                native_vlan = 0;
            }

            if(isNaN(access_vlan)){
                access_vlan = 0;
            }

            if(isNaN(isolate_vlan)){
                isolate_vlan = 0;
            }            

            if(isNaN(segmentID)){
                segmentID = 0;
            }
	   
            $(".control-group").removeClass("error");
            if(mode == 0){
                
                if(isNaN(native_vlan) || native_vlan < 1 || native_vlan > 4094){
                    $("#pgNativeVlanSet").parent().parent().addClass("error");
                    return;
                }
            }
            else if(type == 21 && mode==1){
                if(isNaN(isolate_vlan) || isolate_vlan < 1 || isolate_vlan > 100){
                    $("#pgIsolateVlanSet").parent().parent().addClass("error");
                    return;
                }     
            }  
            else {
                if(isNaN(access_vlan) || access_vlan < 1 || access_vlan > 4094){
                    $("#pgAccessVlanSet").parent().parent().addClass("error");
                    return;
                }
            }                 

	    
	    if(type==20 && isolateType==1 ){
		if(segmentID==0 ){
			$("#pgVxlanSegmentSet").parent().parent().addClass("error");
			return;
		}	
	    }
	    
            var rs = xmlrpc_service.tecs.vnet.portgroup.cfg(xmlrpc_session, pg_name, pg_uuid, type, desc, mode, is_netplane_cfg, netplane,0,
                        promisc, 0, native_vlan, access_vlan, isolate_vlan, mtu, acl, qos, prior, "", "", 0, 0, "", "", "", "", 
                        ver, mgr, "", isolateType, segmentID, OP_MOD);

            if(rs[0] == 0){
                ShowPortgroup();
                $("#modify_portgroup_dlg").modal("hide");
            }
            else {
                ShowNotyRpcErrorInfo(rs);
            }
        }
    });
}


function OnModifyPgVlan(pg){
    var modify_pgvlan_dlg_html = 
    '<div class="span8" style="border-right:1px solid #dddddd;padding-right:10px;"> \
        <div class="row-fluid"> \
            <div class="form-horizontal"> \
                <div class="control-group"> \
                    <label class="control-label" style="width:100px;"><span id="pg_vlan_begin"></span></label> \
                    <div class="controls" style="margin-left:110px;"> \
                        <input type="text" popover="vlan_format" maxlength="4" id="pgVlanBegin" /> \
                    </div> \
                </div> \
                <div class="control-group"> \
                    <label class="control-label" style="width:100px;"><span id="pg_vlan_end"></span></label> \
                    <div class="controls" style="margin-left:110px;"> \
                        <input type="text" popover="vlan_format" maxlength="4" id="pgVlanEnd" /> \
                    </div> \
                </div> \
            </div> \
        </div> \
        <div class="row-fluid"> \
            <div align="right"> \
                <button class="btn"><i class="icon-plus"></i><span id="pgVlanAdd"></span></button>  \
                <button class="btn"><i class="icon-trash"></i><span id="pgVlanDel"></span></button>  \
                <button class="btn"><i class="icon-refresh"></i><span id="pgVlanRefresh"></span></button> \
            </div> \
            <table id="pg_vlan_list" class="table table-striped table-bordered table-condensed">  \
                <thead>  \
                    <tr>  \
                        <th style="width:45%;" align="left"><span id="pg_vlan_t1"></span></th>  \
                        <th style="width:45%;" align="left"><span id="pg_vlan_t2"></span></th>  \
                        <th style="width:10%;"></th> \
                    </tr>  \
                </thead>  \
                <tbody style="word-wrap:break-word;word-break:break-all;">  \
                </tbody>  \
            </table> \
        </div> \
        <!--div class="row-fluid"> \
            <div class="form-horizontal"> \
                <div class="control-group"> \
                    <label class="control-label" style="width:100px;"><span id="vmshare_native_vlan"></span></label> \
                    <div class="controls" style="margin-left:110px;"> \
                        <input type="text" popover="vlan_format" maxlength="4" id="vmshareNativeVlan" /> \
                    </div> \
                </div> \
            </div> \
        </div--> \
    </div> \
    <div class="span4" id="pgvlan_tips"> \
    </div>';

    pg_name = $(pg).parent().parent().find("td").eq(1).text();
    pg_desc = $(pg).parent().parent().find("td").eq(6).text();
    pg_uuid = $(pg).parent().parent().find(":checkbox").eq(0).attr("value");
    var native_vlan_num = $(pg).attr("native_vlan_num");

    ShowDiaglog("modify_pgvlan_dlg", modify_pgvlan_dlg_html, {
        init: function(){
            pgVlanTable = $("#pg_vlan_list").dataTable({
                "sPaginationType": "bootstrap",
                "bFilter":false,
                "bInfo": false,
                "bPaginate": false,
                "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 2 ] }],
                "oLanguage": tableLanguage[page.pagelan]
            }); 

            $("#pgVlanAdd").parent().bind("click", pgVlanAddClick);
            $("#pgVlanDel").parent().bind("click", pgVlanDelClick);
            $("#pgVlanRefresh").parent().click(pgVlanRefreshClick);

            TogglePopover($("#modify_pgvlan_dlg"));
        },
        show: function(){
            var h5 = $("#modify_pgvlan_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-cog"></i>' + h5.text() + " - " + pg_name);

            $("#pgVlanBegin").val("");
            $("#pgVlanEnd").val("");
            //$("#vmshareNativeVlan").val(native_vlan_num);

            z_numInput("pgVlanBegin", 1, 4094);
            z_numInput("pgVlanEnd", 1, 4094);
           // z_numInput("vmshareNativeVlan", 1, 4094);

            pgVlanRefreshClick();
        },
      /*  ok: function(){
            if(pgVlanTable.fnGetNodes().length == 0){
                ShowNotyRpcErrorInfo(["1", "Please configure vlan range!"])
                return;
            }

            var nativevlan = parseInt($("#vmshareNativeVlan").val());

            if(isNaN(nativevlan) || nativevlan < 1 || nativevlan > 4094){
                $("#vmshareNativeVlan").parent().parent().addClass("error");
                return;
            }

            $(".control-group").removeClass("error");

            var param = [];
            param.push({"name":"pg_uuid", "value":pg_uuid});                

            $.getJSON("../php/portgroup_info.php", param, function (json) {
                if(json[0] != 0){ 
                    ShowNotyRpcErrorInfo(["11", json[1]]);
                    return;
                }   

                var is_netplane_cfg = 0;
                if(json[1]["netplane_uuid"] != ""){
                    is_netplane_cfg = 1;
                }                

                var rs = xmlrpc_service.tecs.vnet.portgroup.cfg(xmlrpc_session, pg_name, pg_uuid, parseInt(json[1]["pg_type"]), 
                           pg_desc,  parseInt(json[1]["switchport_mode"]), is_netplane_cfg, "", 0,
                           parseInt(json[1]["promiscuous"]), 0, nativevlan, 0, 0, parseInt(json[1]["mtu"]), json[1]["acl"], json[1]["qos"], 
                           json[1]["prior"], "", "", 0, 0, "", "", "", "", parseInt(json[1]["ver"]), json[1]["mgr"],"", 0, 0, OP_MOD_NATIVEVLAN);    

                if(rs[0] == 0){
                    ShowPortgroup();
                    $("#modify_pgvlan_dlg").modal("hide");
                }
                else {
                    ShowNotyRpcErrorInfo(rs);
                }
            });
        }*/
        close:function(){
        }
    });
}

function pgVlanAddClick(){
    var vlanbegin = parseInt($("#pgVlanBegin").val());
    var vlanend = parseInt($("#pgVlanEnd").val());    

    $(".control-group").removeClass("error");
    if(isNaN(vlanbegin) || vlanbegin < 1 || vlanbegin > 4094){
        $("#pgVlanBegin").parent().parent().addClass("error");
        //ShowNotyRpcErrorInfo(["invalid_param", "vlan begin is invalid!"]);
        return;
    }    

    if(isNaN(vlanend) || vlanend < 1 || vlanend > 4094 || vlanend < vlanbegin){
        $("#pgVlanEnd").parent().parent().addClass("error");
        //ShowNotyRpcErrorInfo(["invalid_param", "vlan end is invalid!"]);
        return;
    }     

    var rs = xmlrpc_service.tecs.vnet.portgroupvlan.cfg(xmlrpc_session, pg_uuid, vlanbegin, vlanend, 4/*ADD*/); 
    if(rs[0] == 0){
        pgVlanRefreshClick();
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }
}

function pgVlanDelClick(){
    var vlanbegin = parseInt($("#pgVlanBegin").val());
    var vlanend = parseInt($("#pgVlanEnd").val());    

    $(".control-group").removeClass("error");
    if(isNaN(vlanbegin) || vlanbegin < 1 || vlanbegin > 4094){
        $("#pgVlanBegin").parent().parent().addClass("error");
        //ShowNotyRpcErrorInfo(["invalid_param", "vlan begin is invalid!"]);
        return;                    
    }    

    if(isNaN(vlanend) || vlanend < 1 || vlanend > 4094 || vlanend < vlanbegin){
        $("#pgVlanEnd").parent().parent().addClass("error");
        //ShowNotyRpcErrorInfo(["invalid_param", "vlan end is invalid!"]);
        return;
    }
     
    var rs = xmlrpc_service.tecs.vnet.portgroupvlan.cfg(xmlrpc_session, pg_uuid, vlanbegin, vlanend, 5/*DEL*/); 
    if(rs[0] == 0){
        pgVlanRefreshClick();
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }    
}

function delete_one_vlanrange(vlanrange){
    var vlanbegin = $(vlanrange).parent().parent().find("td").eq(0).text();
    var vlanend = $(vlanrange).parent().parent().find("td").eq(1).text();

    var rs = xmlrpc_service.tecs.vnet.portgroupvlan.cfg(xmlrpc_session, pg_uuid, parseInt(vlanbegin), parseInt(vlanend), 5/*DEL*/); 
    if(rs[0] == 0){
        pgVlanRefreshClick();
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }            
}

function pgVlanRefreshClick(){
    var params = [];
    params.push({"name":"pg_uuid", "value":pg_uuid});    

    $.getJSON("../php/portgroup_vlanrange.php", params, function (json){
        pgVlanTable.fnClearTable();

        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["11", json[1]]);
            return;
        }

        var vlanData = [];

        for (var i = 0; i < json[1].length; i++) {
            vlanData.push(json[1][i]);
        };

        pgVlanTable.fnAddData(vlanData);

        $("#pgvlan_tips").html(page.res[page.pagelan].type.display["pgvlan_tips"] + json[2]);        
    });
}

function OnPgSelectAll() {
    if($(this).attr("checked")=="checked") {
        $("#portgroup_list tbody input[type=checkbox]").attr("checked", true);
    }
    else {
        $("#portgroup_list tbody input[type=checkbox]").attr("checked", false);
    }
}
