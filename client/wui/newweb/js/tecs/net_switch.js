var OP_ADD = 1;
var OP_DEL = 2;
var OP_MOD = 3;
var OP_ADD_SWITCH_PORT = 4;
var OP_DEL_SWITCH_PORT = 5;
var VNET_LIB_WC_TASK_SWITCH = 0;

var portgroup_selected = "";

function add_switch(){
	var add_switch_dlg_html = 
        '<div class="span8" style="border-right:1px solid #dddddd;padding-right:10px;"> \
            <div class="row-fluid"> \
            <form class="form-horizontal"> <fieldset>  \
            <div class="control-group">   \
                <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="switch.name"></OamStaticUI></label> \
                <div class="controls" style="margin-left:110px;"><input type="text" popover="dvs_name_format" id="switch_name" maxlength="15" /></div> \
            </div>    \
            <div class="control-group">   \
                <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="switch.type"></OamStaticUI></label> \
                <div class="controls" style="margin-left:110px;"> \
                    <select id="switch_type"></select> \
                </div> \
            </div>    \
            <div class="control-group">   \
                <label class="control-label" style="width:100px;">SDN</label> \
                <div class="controls" style="margin-left:110px;"> \
                    <select id="switch_sdn"></select> \
                </div> \
            </div>    \
            <div class="control-group">   \
                <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="switch.portgroup"></OamStaticUI></label> \
                <div class="controls" style="margin-left:110px;"> \
                    <select id="switch_pg"></select> \
                </div> \
            </div>    \
            <!--div class="control-group">   \
                <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="switch.mtu"></OamStaticUI></label> \
                <div class="controls" style="margin-left:110px;"><input type="text" id="switch_mtu" maxlength="64" value="1500" /></div> \
            </div-->    \
            <div class="control-group">   \
                <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="switch.evb"></OamStaticUI></label> \
                <div class="controls" style="margin-left:110px;"> \
                    <select id="switch_evb"></select> \
                </div> \
            </div>    \
            <div class="control-group">   \
                <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="switch.max_nics"></OamStaticUI></label> \
                <div class="controls" style="margin-left:110px;"><input type="text" popover="1~1024" id="switch_max_nics" maxlength="4" /></div> \
            </div>    \
            </fieldset></form> \
            </div> \
        </div> \
        <div class="span4" id="switch_pg_tips"> \
        </div>';    

    ShowDiaglog("add_switch_dlg", add_switch_dlg_html, {
    	show:function(){
            var h5 = $("#add_switch_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-plus"></i>' + h5.text()); 

            $("#add_switch_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });  

            z_strInput("switch_name", 64);
            z_numInput("switch_max_nics", 1, 1024);

            $("#switch_pg").unbind('change').bind('change', function(){
                AppendPortGroupInfo($("#switch_pg").val(), $("#switch_pg_tips"));
            });  

            $("#switch_pg").empty();
            var pg = new Array();
            pg["sdn"] = new Array();
            pg["notSdn"] = new Array();
            
            $.getJSON("../php/switch_portgroup.php", function(json){
                if(json[0] != 0){
                    ShowNotyRpcErrorInfo(["11", json[1]]);
                    return;
                }

                for (var i = 0; i < json[1].length; i++) {
                    if(json[1][i]["is_sdn"] == 1){
                        pg["sdn"].push(json[1][i]);
                    }
                    else if(json[1][i]["is_sdn"] == 0){
                        pg["notSdn"].push(json[1][i]);
                    }
                }

                for (var i = 0; i < pg["notSdn"].length; i++) {
                    $("#switch_pg").append("<option value='" + pg["notSdn"][i].uuid +"'>" + pg["notSdn"][i].name);
                } 

                $("#switch_pg").trigger("change");
            });

            $("#switch_type").empty();  
            $("#switch_type").append('<option value="1">' + page.res[page.pagelan].type.SwitchType["1"]);
            $("#switch_type").append('<option value="2">' + page.res[page.pagelan].type.SwitchType["2"]); 

            $("#switch_evb").empty();
            $("#switch_evb").append('<option value="0">' + page.res[page.pagelan].type.EVB["0"]);
            $("#switch_evb").append('<option value="1">' + page.res[page.pagelan].type.EVB["1"]); 
            $("#switch_evb").append('<option value="2">' + page.res[page.pagelan].type.EVB["2"]);            

            $("#switch_sdn").empty();
            $("#switch_sdn").append('<option value="0">'+page.res[page.pagelan].type.SDNType["0"]);
            $("#switch_sdn").append('<option value="1">'+page.res[page.pagelan].type.SDNType["1"]);

            $("#switch_type").unbind("change").bind("change", function(){
                if($("#switch_type").val() == 2){
                    $("#switch_max_nics").attr("disabled", true);
                    $("#switch_name").val("edvs");
                    $("#switch_max_nics").val("");
                }
                else {
                    $("#switch_max_nics").attr("disabled", false);
                    $("#switch_name").val("sdvs");
                    $("#switch_max_nics").val(1024);
                }
            });

            $("#switch_type").trigger("change");

          // /* 
            $("#switch_sdn").unbind("change").bind("change",function(){
                if($("#switch_sdn").val() ==0){
                    $("#switch_pg").empty();
                   // if(pg["notSdn"] != null){
                        for (var i = 0; i < pg["notSdn"].length; i++) {
                            $("#switch_pg").append("<option value='" + pg["notSdn"][i].uuid +"'>" + pg["notSdn"][i].name);
                           
                        } 
                         $("#switch_pg").trigger("change");
                   // }
                }
                else if($("#switch_sdn").val() ==1){
                    $("#switch_pg").empty();
                    //if(pg["sdn"] != null){
                        for (var i = 0; i < pg["sdn"].length; i++) {
                            $("#switch_pg").append("<option value='" + pg["sdn"][i].uuid +"'>" + pg["sdn"][i].name);
                        }
                         $("#switch_pg").trigger("change");
                   // }
                }
            });
            $("#switch_sdn").trigger("change");
            $("#switch_pg").trigger("change");
           // */
            TogglePopover($("#add_switch_dlg"));
    	},
    	ok:function(){
            var sw = $("#switch_name").val();
            var sw_type = parseInt($("#switch_type").val());
            var pg   = $("#switch_pg").val();
            //var mtu  = parseInt($("#switch_mtu").val());
            var evb  = parseInt($("#switch_evb").val());
            var max_nics = parseInt($("#switch_max_nics").val());

            $(".control-group").removeClass("error");
            if(sw == ""){
                $("#switch_name").parent().parent().addClass("error");
                return;
            }

            if(!CheckSwitchName(sw, sw_type)){
                $("#switch_name").parent().parent().addClass("error");
                return; 
            }

            if(sw_type == 1 && (isNaN(max_nics) || max_nics < 1 || max_nics > 1024)){
                $("#switch_max_nics").parent().parent().addClass("error");
                return;                
            }
            else if(sw_type == 2) {
                max_nics = 0;
            }            

            var rs = xmlrpc_service.tecs.vnet["switch"].cfg(xmlrpc_session, sw, "", sw_type, /*mtu,*/ OP_ADD, evb, max_nics, pg);

            if(rs[0] == 0){ 
                $("#add_switch_dlg").modal("hide");
                refresh_switch();
            }
            else {
                ShowNotyRpcErrorInfo(rs);
            }
    	}
    });
}

function CheckSwitchName(name, type){
    if(type == 1 && (new RegExp("^sdvs[A-Za-z0-9_]+$")).test(name)){
        return true;
    }

    if (type == 2 && (new RegExp("^edvs[A-Za-z0-9_]+$")).test(name)) {
        return true;
    }

    return false;
}

function del_switch(){
    var vCheckedbox = $(switchTable.fnGetNodes()).find(":checkbox:checked");

    if(vCheckedbox.size() > 0){
        ShowDiaglog("del_switch_dlg", page.res[page.pagelan]["xid"]["val"]["del_switch_confirm"], {
        	show:function(){
                var h5 = $("#del_switch_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-trash"></i>' + h5.text());    
        	},
        	ok:function(){
                $("#del_switch_dlg").modal("hide");

                vCheckedbox.each(function(){
                    var uuid = $(this).attr("value");
                    var tr = $(this).parent().parent();                      
                    var name = tr.find("td").eq(1).text();                    

                    var rs = xmlrpc_service.tecs.vnet["switch"].cfg(xmlrpc_session, name, uuid, 0, OP_DEL, 0, 0, "");     

                    ShowNetSynRpcInfo(rs);
                });

                refresh_switch();
        	}
        });
    }
    else {
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]);    	
    }
}
	
function portgroup_info_click(pg){
    var pg_name = $(pg).text();
    var pg_uuid = $(pg).parent().parent().find("td span").eq(0).attr("pg_uuid");
    portgroup_info_common(pg_uuid,pg_name);
}

function modify_switch_click(sw){
    var modify_switch_dlg_html = 
        '<div class="span8" style="border-right:1px solid #dddddd;padding-right:10px;"> \
            <div class="row-fluid"> \
            <form class="form-horizontal"> <fieldset>  \
            <div class="control-group" style="display:none;">   \
                <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="switch.type"></OamStaticUI></label> \
                <div class="controls" style="margin-left:110px;"> \
                    <select id="switch_type_set"> </select> \
                </div> \
            </div>    \
            <div class="control-group">   \
                <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="switch.portgroup"></OamStaticUI></label> \
                <div class="controls" style="margin-left:110px;"> \
                    <select id="switch_pg_set"></select> \
                </div> \
            </div>    \
            <!--div class="control-group">   \
                <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="switch.mtu"></OamStaticUI></label> \
                <div class="controls" style="margin-left:110px;"><input type="text" id="switch_mtu_set" maxlength="64" /></div> \
            </div-->    \
            <div class="control-group">   \
                <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="switch.evb"></OamStaticUI></label> \
                <div class="controls" style="margin-left:110px;"> \
                    <select id="switch_evb_set"></select> \
                </div> \
            </div>    \
            <div class="control-group">   \
                <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="switch.max_nics"></OamStaticUI></label> \
                <div class="controls" style="margin-left:110px;"><input type="text" id="switch_max_nics_set" popover="1~1024" maxlength="64" /></div> \
            </div>    \
            </fieldset></form> \
            </div> \
        </div> \
        <div class="span4" id="switch_pg_set_tips"> \
        </div>';     

    var switch_uuid = $(sw).parent().parent().find(":checkbox").eq(0).attr("value");
    var switch_name = $(sw).parent().parent().find("td").eq(1).text();
    var switch_type = $(sw).parent().parent().find("td span").eq(1).attr("value");
    //var switch_pg_uuid = $(sw).parent().parent().find(":checkbox").eq(0).attr("pg_uuid");
    var switch_pg_uuid = $(sw).parent().parent().find("td span").eq(0).attr("pg_uuid");
    //var switch_mtu = $(sw).parent().parent().find("td").eq(4).text();
    var switch_evb = $(sw).parent().parent().find("td span").eq(2).attr("value");
    var switch_max_nics = $(sw).parent().parent().find("td").eq(5).text();

    ShowDiaglog("modify_switch_dlg", modify_switch_dlg_html, {
    	show:function(){
            var h5 = $("#modify_switch_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-edit"></i>' + h5.text() + ' - ' + switch_name);

            z_numInput("switch_max_nics_set", 1, 1024);

            $("#modify_switch_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });   

            $("#switch_pg_set").unbind('change').bind('change', function(){
                AppendPortGroupInfo($("#switch_pg_set").val(), $("#switch_pg_set_tips"));
            });              

            $("#switch_pg_set").empty();
            $.getJSON("../php/switch_portgroup.php", function(json){
                if(json[0] != 0){
                    ShowNotyRpcErrorInfo(["11", json[1]]);
                    return;
                }

                for (var i = 0; i < json[1].length; i++) {
                    $("#switch_pg_set").append("<option value='" + json[1][i].uuid +"'>" + json[1][i].name);
                };                

                $("#switch_pg_set").val(switch_pg_uuid);
                $("#switch_pg_set").trigger("change");
            });

            $("#switch_type_set").empty();
            $("#switch_type_set").append('<option value="1">' + page.res[page.pagelan].type.SwitchType["1"]);
            $("#switch_type_set").append('<option value="2">' + page.res[page.pagelan].type.SwitchType["2"]); 

            $("#switch_evb_set").empty();
            $("#switch_evb_set").append('<option value="0">' + page.res[page.pagelan].type.EVB["0"]);
            $("#switch_evb_set").append('<option value="1">' + page.res[page.pagelan].type.EVB["1"]); 
            $("#switch_evb_set").append('<option value="2">' + page.res[page.pagelan].type.EVB["2"]); 

            $("#switch_type_set").unbind("change").bind("change", function(){
                if($("#switch_type_set").val() == 2){
                    $("#switch_max_nics_set").attr("disabled", true);
                }
                else {
                    $("#switch_max_nics_set").attr("disabled", false);
                }
            });            

            $("#switch_type_set").val(switch_type);
            $("#switch_type_set").trigger("change");
            //$("#switch_mtu_set").val(switch_mtu);
            $("#switch_evb_set").val(switch_evb);
            $("#switch_max_nics_set").val(switch_max_nics);

            TogglePopover($("#modify_switch_dlg"));
    	},
    	ok:function(){
            var sw_type = parseInt($("#switch_type_set").val());
            var pg   = $("#switch_pg_set").val();
            //var mtu  = parseInt($("#switch_mtu_set").val());
            var evb  = parseInt($("#switch_evb_set").val());
            var max_nics = parseInt($("#switch_max_nics_set").val());

            $(".control-group").removeClass("error");
            if(sw_type == 1 && (isNaN(max_nics) || max_nics < 1 || max_nics > 1024)){
                $("#switch_max_nics_set").parent().parent().addClass("error");
                return;                
            }
            else if(sw_type == 2) {
                max_nics = 0;
            }

            var rs = xmlrpc_service.tecs.vnet["switch"].cfg(xmlrpc_session, switch_name, switch_uuid, sw_type, /*mtu,*/ OP_MOD, evb, max_nics, pg);

            if(rs[0] == 0){
                $("#modify_switch_dlg").modal("hide");
                refresh_switch();
            }
            else {
                ShowNotyRpcErrorInfo(rs);
            }
    	}
    });	
}

function modify_switch_ports(){
    var modify_switch_ports_dlg_html = 
    '<div> \
        <table> \
            <tr> \
                <td width="220px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="switch.unmap_ports"/></td>   \
                <td></td>  \
                <td width="220px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="switch.maped_ports"/></td>     \
            </tr>   \
            <tr> \
                <td valign="top">  \
                    <select id="switch_host"></select> \
                    <select id="switch_upmap_ports" multiple="multiple" style="height: 180px;"></select> \
                    <input id="switch_wildcard_port" type="text" /> \
                </td> \
                <td style="width: 0px; padding:20px;">  \
                    <a class="btn" onclick="port_map_to_switch()"> > </a> \
                    <a class="btn" onclick="port_demap_from_switch()"> < </a> \
                </td> \
                <td style="valign:top;"> \
                    <select id="switch_maped_ports" multiple="multiple" style="height: 210px;"></select> \
                </td> \
            </tr>   \
        </table> \
    </div>';     

    var switch_name = $("#selectSwitch").find("option:selected").text();
    var switch_uuid = $("#selectSwitch").val();
    var switch_type = $("#selectSwitch").find("option:selected").attr("type");  

    modify_switch_name = switch_name;
    modify_switch_uuid = switch_uuid;
    modify_switch_type = parseInt(switch_type);

    ShowDiaglog("modify_switch_ports_dlg", modify_switch_ports_dlg_html, {
        show:function(){
            var h5 = $("#modify_switch_ports_dlg").find('div.modal-header h5');
            h5.html(h5.text() + ' - ' + switch_name);

            $("#modify_switch_ports_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });    

            $("#switch_upmap_ports").hide();
            $("#switch_wildcard_port").hide();

            $.getJSON("../php/switch_host_nic.php", function(json){
                if(json[0] != 0){
                    ShowNotyRpcErrorInfo(["12", json[1]]);
                    return;
                }

                hosts = json[1];

                $("#switch_host").empty();
                //$("#switch_host").append("<option value='*'>*");
                for (var i = 0; i < hosts.length; i++) {
                    if(hosts[i].host_name == null){
                        $("#switch_host").append("<option value='" + hosts[i].vna_uuid +"'>" + hosts[i].vna_uuid);
                    }
                    else {
                        $("#switch_host").append("<option value='" + hosts[i].vna_uuid +"'>" + hosts[i].host_name);
                    }
                }; 

                $("#switch_host").unbind("change").bind("change", function(){
                    if($("#switch_host").val() == "*"){
                        $("#switch_upmap_ports").hide();
                        $("#switch_wildcard_port").show();
                    }
                    else {
                        $("#switch_upmap_ports").show();
                        $("#switch_wildcard_port").hide();
                    }
                    
                    refresh_map_unmap_port(hosts);
                });

                $("#switch_host").trigger("change");                                 
            }); 
        },
        close:function(){
            $("#modify_switch_ports_dlg").modal("hide");
            //refresh_switch();
        }
    });     
}

function check_nic(nic){
    if(modify_switch_type == 1){
        if(nic["switch_uuid"] == null && nic["port_type"] != 2 && nic["port_type"] != 3 && nic["port_type"] != 5 &&nic["port_type"] != 6){
            return true;
        }
    }

    if(modify_switch_type == 2){
        if(nic["switch_uuid2"] == null && nic["is_sriov"] == 1){
            return true;
        } 
    }

    return false;
}

function check_loop(port){
    var new_port = port.toString();
    var exist_port = $("#switch_maped_ports").text();

    if(port.length > 1 && new_port.match("loop") != null){
        return true;
    }

    if(exist_port.match("loop") != null){
        return true;
    }

    if($("#switch_maped_ports").find("option").length > 0 && new_port.match("loop") != null){
        return true;
    }   

    return false;
}

function check_bond_member(nics, index){
    if(nics[index].port_type == 1){
        return false;
    }

    var portname = nics[index].name;

    for (var i = 0; i < nics.length; i++) {
        if(nics[i].port_type == 1){
            for (var j = 0; j < nics[i].members.length; j++) {
                if(nics[i].members[j] == portname){
                    return true;
                }
            };
        }
    };
    return false;
}

function is_contains(arr, val){
    for (var i = 0; i < arr.length; i++) {
        if(arr[i] == val){
            return true;
        }
    }

    return false;
}

function check_bond(nics, index){
    if(nics[index].port_type == 1){
        for (var j = 0; j < nics.length; j++) {
            if(is_contains(nics[index].members, nics[j].name)){
                nics[j].switch_uuid = nics[index].switch_uuid;
                nics[j].switch_uuid2 = nics[index].switch_uuid2;
            }
        };
        return true;
    } 

    return false;
}

function refresh_map_unmap_port(hosts){
    $("#switch_upmap_ports").empty();
    $("#switch_maped_ports").empty();
    for (var i = 0; i < hosts.length; i++) {
        if(hosts[i].vna_uuid == $("#switch_host").val()){
            for (var j = 0; j < hosts[i].nics.length; j++) {
                if(check_bond(hosts[i].nics, j)){
                    continue;
                }

                if(check_nic(hosts[i].nics[j])){
                    /*if(hosts[i].nics[j].port_type == 1){
                        for (var k = 0; k < hosts[i].nics[j].members.length; k++) { 
                            $("#switch_upmap_ports").append("<option value='" + hosts[i].nics[j].members[k] +"'>" + hosts[i].nics[j].members[k]);                                
                        }
                    }
                    else {*/
                        $("#switch_upmap_ports").append("<option value='" + hosts[i].nics[j].name +"'>" + hosts[i].nics[j].name);
                    //}
                }
                else {
                    if(hosts[i].nics[j].switch_uuid == modify_switch_uuid || hosts[i].nics[j].switch_uuid2 == modify_switch_uuid){
                        /*if(hosts[i].nics[j].port_type == 1){
                            for (var k = 0; k < hosts[i].nics[j].members.length; k++) {
                                $("#switch_maped_ports").append("<option value='" + hosts[i].nics[j].members[k] +"'>" + hosts[i].nics[j].members[k]);                                     
                            };
                        }
                        else {*/
                            $("#switch_maped_ports").append("<option value='" + hosts[i].nics[j].name +"'>" + hosts[i].nics[j].name); 
                        //}
                    }                 
                }
            };  
            return;                      
        }
    } 
}

function refresh_switch_port(){
    $.getJSON("../php/switch_host_nic.php", function(json){
        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["12", json[1]]);
            return;
        }

        hosts = json[1];

        var switchPortData = [];

        for (var i = 0; i < hosts.length; i++) {
            for (var j = 0; j < hosts[i].nics.length; j++) {
                if(hosts[i].nics[j].switch_uuid == modify_switch_uuid || hosts[i].nics[j].switch_uuid2 == modify_switch_uuid){
                    var oneSwitchPort = [];
                    oneSwitchPort[0] = modify_switch_name;   
                    oneSwitchPort[1] = hosts[i].vna_uuid;
                    oneSwitchPort[2] = hosts[i].cluster_name;
                    oneSwitchPort[3] = hosts[i].host_name;
                    oneSwitchPort[4] = hosts[i].nics[j].name;
                    if(hosts[i].nics[j].port_type == 1){
                        oneSwitchPort[4] += "(" + hosts[i].nics[j].members.toString() + ")";   
                        oneSwitchPort[4] += "(" + page.res[page.pagelan].type.BondMode[hosts[i].nics[j].bond_mode] + ")";                       
                    }

                    switchPortData.push(oneSwitchPort);
                }                 
            };  
        }         

        switchPortTable.fnClearTable();
        switchPortTable.fnAddData(switchPortData);      

        refresh_map_unmap_port(hosts);        
        $(".tooltip").hide();
        $('[data-rel="tooltip"]').tooltip({"placement":"bottom"});
        $("#switchPort_last_refresh_time").html(GetLastUpdateTime());        
    });    
}

function enable_ui()
{
    $("#modify_switch_ports_dlg").find("a").attr("disabled", false);
    clearInterval(disui_timer_id);
}

function disable_ui()
{
    $("#modify_switch_ports_dlg").find("a").attr("disabled", true);
    disui_timer_id = setInterval(enable_ui, 30000);
}

function port_map_to_switch(){
    var vna_uuid = $("#switch_host").val();
    var host_name = $("#switch_host").text();

    if(vna_uuid == "*"){
        var wildcard_port = $("#switch_wildcard_port").val();
        if(wildcard_port == ""){
            ShowNotyRpcErrorInfo(["1", "please input a port name"]);
            return;
        }

        disable_ui();        
        xmlrpc_service.tecs.vnet.switchport.cfg(xmlrpc_session, modify_switch_name, modify_switch_uuid,
            modify_switch_type, vna_uuid, 1, OP_ADD_SWITCH_PORT, "", 1, wildcard_port, function(rs){
                if(rs[0] == 0){
                    refresh_switch_port();
                    enable_ui();
                }
                else {
                    ShowNotyRpcErrorInfo(rs);
                    enable_ui();
                }
            });
    }
    else {
        var select_bond_mode_dlg_html = 
        '<form class="form-horizontal"> <fieldset>  \
            <div class="control-group">   \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="switch.bond_mode"></OamStaticUI></label> \
                <div class="controls"> \
                    <select id="switch_bond_mode"></select> \
                </div> \
            </div> \
        </fieldset> </form>';

        var port = [];
        $("#switch_upmap_ports").find("option:selected").each(function(){
            port.push( $(this).val() );
        });

        if(port.length < 1){
            ShowNotyRpcErrorInfo(["1","please select a port name"]);
            return;
        }

        if(check_loop(port)){
            ShowNotyRpcErrorInfo(["1","loop can't bond with other port"]);
            return;
        }

        var port_num = port.length;

        if((port_num > 1 && $("#switch_maped_ports option").length < 1) || $("#switch_maped_ports option").length == 1){ 
            ShowDiaglog("select_bond_mode_dlg", select_bond_mode_dlg_html, {
                init: function(){
                    $("#switch_bond_mode").append('<option value="1">' +  page.res[page.pagelan].type.BondMode["1"]);
                    $("#switch_bond_mode").append('<option value="4">' +  page.res[page.pagelan].type.BondMode["4"]);                    
                },
                show: function(){
                    $("#select_bond_mode_dlg").modal("show");
                    $("#modify_switch_ports_dlg").modal("hide");                    

                    $("#select_bond_mode_dlg").find("OamStaticUI").each(function(){
                        SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
                    });                                      
                },
                ok: function(){
                    var bond_mode = parseInt($("#switch_bond_mode").val());

                    disable_ui();
                    xmlrpc_service.tecs.vnet.switchport.cfg(xmlrpc_session, modify_switch_name, modify_switch_uuid,
                        modify_switch_type, vna_uuid, bond_mode, OP_ADD_SWITCH_PORT, "", port_num, port.toString(),
                        function(rs) {
                            $("#select_bond_mode_dlg").modal("hide");
                            $("#modify_switch_ports_dlg").modal("show"); 
                            //enable_ui();  
                            if(rs[0] == 0){
                                refresh_switch_port();
                                enable_ui();
                            }
                            else {
                                ShowNotyRpcErrorInfo(rs);
                                enable_ui();
                            }
                        });
                },
                cancel: function(){
                    $("#select_bond_mode_dlg").modal("hide");
                    $("#modify_switch_ports_dlg").modal("show");
                    enable_ui();
                }
            });
        } 
        else {
            disable_ui();
            xmlrpc_service.tecs.vnet.switchport.cfg(xmlrpc_session, modify_switch_name, modify_switch_uuid,
                modify_switch_type, vna_uuid, 1, OP_ADD_SWITCH_PORT, "", 1, port.toString(), "", "", "", "", 
                "", "", "", function(rs){
                    //enable_ui();
                    if(rs[0] == 0){
                        refresh_switch_port();
                        enable_ui();
                    }
                    else {
                        ShowNotyRpcErrorInfo(rs);
                        enable_ui();
                    }                         
                });  
        }
    }
}

function port_demap_from_switch(){
    if($("#modify_switch_ports_dlg").find("a").attr("disabled") == "disabled"){
        //alert();
        return;
    }
    var vna_uuid = $("#switch_host").val();
    var host_name = $("#switch_host").text();

    var ports = [];

    if($("#switch_maped_ports").find("option:selected").size() == 0){
        ShowNotyRpcErrorInfo(["1", "please input a port name"]);
        return;
    }
    $("#switch_maped_ports").find("option:selected").each(function(){
        ports.push($(this).val())
    });

    disable_ui();
    xmlrpc_service.tecs.vnet.switchport.cfg(xmlrpc_session, modify_switch_name, modify_switch_uuid,
        modify_switch_type, vna_uuid, 1, OP_DEL_SWITCH_PORT, "", ports.length, ports.toString(), function(rs){
           // enable_ui();
            if(rs[0] != 0){
                ShowNotyRpcErrorInfo(rs);
                enable_ui();                 
            }
            else {
                refresh_switch_port();
                enable_ui();
            }                         
        });       
}

function InitSelectPortgroup(sl, netplane, cb){
    sl.empty();
    sl.attr("data-placeholder", page.res[page.pagelan].type.display["select_portgroup"]);

    $.getJSON("../php/switch_portgroup.php", function (json) {
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

    refresh_switch();
}

var currSwitchState = new Object();
var pageno;
function refresh_switch(){
    if($("#switch_list").is(":visible")){  
    }
    else {
        clearInterval(switch_timer_id);
        return;
    }
    ClearCurSelItem(currSwitchState);  
    RecordSelItem(switchTable, "switch_list", currSwitchState);   
   
     pageno = currSwitchState.pageno;    

    $.getJSON("../php/net_switch.php", function(json){
        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["1", json[1]]);
            return;
        }

    	switchTable.fnClearTable();
        $("#selectSwitch").empty();

        var switchData = [];

        for (var i = 0; i < json[1].length; i++) {
        	var oneSwitch = [];
       	
        	//oneSwitch[0] = "<input type='checkbox' value='" + json[1][i][0] + "' pg_uuid='" + json[1][i][8] + "' />";
        	oneSwitch[0] = GetItemCheckbox(json[1][i][0] , currSwitchState);
        	//oneSwitch[1] = json[1][i][1];
            oneSwitch[1] = "<span pg_uuid = '"+json[1][i][8] +"'>"+json[1][i][1] +"</span>";
        	oneSwitch[2] = "<span value='" + json[1][i][2] + "'>" + page.res[page.pagelan].type.SwitchType[json[1][i][2]] + "</span>";
            oneSwitch[3] = '<a href="javascript:void(0);" onclick="portgroup_info_click(this)">'+ json[1][i][3] + '</a>';       
	     //   oneSwitch[3] = json[1][i][3];
        	//oneSwitch[4] = json[1][i][4];
        	oneSwitch[4] = "<span value='" + json[1][i][5] + "'>" + page.res[page.pagelan].type.EVB[json[1][i][5]] + "</span>";
            oneSwitch[5] = json[1][i][6];
        	oneSwitch[6] = "<a href='javascript:void(0)' onclick='goto_port(this)'>" + json[1][i][7] + "</a>";        	        	        	        	        	
        	oneSwitch[7] = "<a href='javascript:void(0)' onclick='modify_switch_click(this)' data-rel='tooltip' data-original-title='"
        	                  + page.res[page.pagelan]["type"]["opr"]["modify"] + "'><i class='icon-edit'></i></a>";


            if(portgroup_selected == "" || portgroup_selected.toString().indexOf(oneSwitch[3]) != -1){
                switchData.push(oneSwitch);
            }
            $("#selectSwitch").append("<option value='" + json[1][i][0] + "' type=" + json[1][i][2] + ">" + json[1][i][1]);
        };

    	switchTable.fnAddData(switchData); 

            var oSettings = switchTable.fnSettings();
            var wholeNO = json[1].length /oSettings._iDisplayLength;

            if(wholeNO <= 1){
            }
            else if(wholeNO < (pageno +1) ){
                switchTable.fnPageChange("last");
            }
            else{
                switchTable.fnPageChange(parseInt(pageno));
            }

  

        $(".tooltip").hide();
        $('[data-rel="tooltip"]').tooltip({"placement":"bottom"});

        $("#switchlist_last_refresh_time").html(GetLastUpdateTime());
        $("#switchPort_last_refresh_time").html(GetLastUpdateTime());
    });
}

function goto_port(sw){
    $("#switchPortNav").tab("show");

    var switch_uuid = $(sw).parent().parent().find(":checkbox").eq(0).attr("value");

    $("#selectSwitch").val(switch_uuid);
    $("#selectSwitch").trigger("change");
}

function switch_select_all_click(){
    if($(this).attr("checked") == "checked") {
        $("#switch_list tbody input[type=checkbox]").attr("checked", true);
    }
    else {
        $("#switch_list tbody input[type=checkbox]").attr("checked", false);
    }    	
}

function SwitchWildcardAddClick(){
    var add_switch_dlg_html = 
        '<form class="form-horizontal"> <fieldset>  \
            <div class="control-group">   \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="switch.name"></OamStaticUI></label> \
                <div class="controls"><input type="text" popover = "switch_wildcard_name_format" id="switchwildcard_name" maxlength="15" /></div> \
            </div>    \
            <div class="control-group">   \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="switch.type"></OamStaticUI></label> \
                <div class="controls"> \
                    <select id="switchwildcard_type"></select> \
                </div> \
            </div>    \
            <div class="control-group">   \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="switch.portgroup"></OamStaticUI></label> \
                <div class="controls"> \
                    <select id="switchwildcard_pg"></select> \
                </div> \
            </div>    \
            <!--div class="control-group">   \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="switch.mtu"></OamStaticUI></label> \
                <div class="controls"><input type="text" id="switchwildcard_mtu" maxlength="64" value="1500" /></div> \
            </div-->    \
            <div class="control-group">   \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="switch.evb"></OamStaticUI></label> \
                <div class="controls"> \
                    <select id="switchwildcard_evb"></select> \
                </div> \
            </div>    \
            <div class="control-group">   \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="switch.max_nics"></OamStaticUI></label> \
                <div class="controls"><input type="text" id="switchwildcard_max_nics" popover="1~1024" maxlength="4" /></div> \
            </div>    \
        </fieldset></form>';    
 
    ShowDiaglog("add_switchwildcard_dlg", add_switch_dlg_html, {
        show:function(){
            var h5 = $("#add_switchwildcard_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-plus"></i>' + h5.text()); 

            $("#add_switchwildcard_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });  

            z_numInput("switchwildcard_max_nics", 1, 1024);

            $("#switchwildcard_pg").empty();
            $.getJSON("../php/switch_portgroup.php", function(json){
                if(json[0] != 0){
                    ShowNotyRpcErrorInfo(["11", json[1]]);
                    return;
                }

                for (var i = 0; i < json[1].length; i++) {
                    $("#switchwildcard_pg").append("<option value='" + json[1][i].uuid +"'>" + json[1][i].name);
                } 
            });

            $("#switchwildcard_type").empty();  
            $("#switchwildcard_type").append('<option value="1">' + page.res[page.pagelan].type.SwitchType["1"]);
            $("#switchwildcard_type").append('<option value="2">' + page.res[page.pagelan].type.SwitchType["2"]); 

            $("#switchwildcard_evb").empty();
            $("#switchwildcard_evb").append('<option value="0">' + page.res[page.pagelan].type.EVB["0"]);
            $("#switchwildcard_evb").append('<option value="1">' + page.res[page.pagelan].type.EVB["1"]);
            $("#switchwildcard_evb").append('<option value="2">' + page.res[page.pagelan].type.EVB["2"]);

            $("#switchwildcard_type").unbind("change").bind("change",function(){
                var type = parseInt( $("#switchwildcard_type").val() );
                if(type == 2){
                    $("#switchwildcard_max_nics").attr("disabled",true);
                    $("#switchwildcard_max_nics").attr("value", 0);
                    $("#switchwildcard_name").val("edvsw");
                }
                else{
                    $("#switchwildcard_max_nics").attr("disabled",false);
                    $("#switchwildcard_max_nics").attr("value", "1024");
                    $("#switchwildcard_name").val("sdvsw");
                }
            });
            $("#switchwildcard_type").trigger("change");
            TogglePopover($("#add_switchwildcard_dlg"));
        },
        ok:function(){
            var sw = $("#switchwildcard_name").val();
            var sw_type = parseInt($("#switchwildcard_type").val());
            var pg   = $("#switchwildcard_pg").val();
            //var mtu  = parseInt($("#switch_mtu").val());
            var evb  = parseInt($("#switchwildcard_evb").val());
            var max_nics = parseInt($("#switchwildcard_max_nics").val());

            $(".control-group").removeClass("error");
            if(sw == ""){
                $("#switchwildcard_name").parent().parent().addClass("error");
                return;
            }
            if(!CheckSwitchWildcardName(sw,sw_type)){
                $("#switchwildcard_name").parent().parent().addClass("error");
                return;               
            }
            if(isNaN(max_nics) || max_nics > 1024){
                $("#switchwildcard_max_nics").parent().parent().addClass("error");
                return;                
            }

            var rs = [];
            var rs = xmlrpc_service.tecs.vnet["wcswitch"].base.cfg(xmlrpc_session, sw, "", sw_type, /*mtu,*/ OP_ADD, evb, max_nics, pg);
            if(rs[0] == 0){ 
                $("#add_switchwildcard_dlg").modal("hide");
                SwitchWildcardRefreshClick();
            }
            else {
                ShowNotyRpcErrorInfo(rs);
            }
        }
    });
}

function CheckSwitchWildcardName(name, type){
    if(type == 1 && (new RegExp("^sdvsw[A-Za-z0-9_]+$")).test(name)){
        return true;
    }

    if (type == 2 && (new RegExp("^edvsw[A-Za-z0-9_]+$")).test(name)) {
        return true;
    }

    return false;
}

function SwitchWildcardDelClick(){
    var vCheckedbox = $(switchWildcardTable.fnGetNodes()).find(":checkbox:checked");

    if(vCheckedbox.size() > 0){
        ShowDiaglog("del_switchwildcard_dlg", page.res[page.pagelan]["xid"]["val"]["del_switchwildcard_confirm"], {
            show:function(){
                var h5 = $("#del_switchwildcard_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-trash"></i>' + h5.text());    
            },
            ok:function(){
                $("#del_switchwildcard_dlg").modal("hide");

                vCheckedbox.each(function(){
                    var uuid = $(this).attr("value");
                    var rs = xmlrpc_service.tecs.vnet["wcswitch"].base.cfg(xmlrpc_session, "", uuid, 1, /*mtu,*/ OP_DEL, 0, 0, "");
                    ShowNetSynRpcInfo(rs);
                });

                //refresh_switch();
                SwitchWildcardRefreshClick();
            }
        });
    }
    else {
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]);       
    }
}

function removePort(){
    var vCheckedbox = $(switchWildcardPortTable.fnGetNodes()).find(":checkbox:checked");

    if(vCheckedbox.size() > 0) {
        ShowDiaglog("delete_port_dlg", page.res[page.pagelan]["xid"]["val"]["delete_port_confirm"], {
            show:function(){
                var h5 = $("#delete_port_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-trash"></i>' + page.res[page.pagelan].type.opr["remove"]);
            },

            ok:function(){
                var names = new Array();
                var switch_portType = 0;
                var mode;
                if(switch_bondMode == "主备模式"){
                    mode = 1;
                }
                else if(switch_bondMode == "协议模式"){
                    mode = 4;
                }
                else {
                    mode =0;
                }
                vCheckedbox.each(function(){
                    var switch_uuid = $(this).attr("value");
                    var portname = $(this).parent().next().text();
                    if( NameBeginWithLoop(portname) ){
                        switch_portType = 4;
                    }
                    names.push(portname);
                }); 
                var rs = xmlrpc_service.tecs.vnet["wcswitch"].port.cfg(xmlrpc_session, switch_name, switch_uuid, switch_type, switch_portType, mode,OP_DEL_SWITCH_PORT,names.length,names.toString());
                if(rs[0] == 0){ 
                    $("#delete_port_dlg").modal("hide");
                    port_num =  port_num -names.length;
                    SwitchWildcardRefreshClick();
                }
                else {
                    ShowNotyRpcErrorInfo(rs);
                }
            }
        });
    }
    else {
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]);       
    }
}

function NameBeginWithEth(str){
    if (str === "")
        return false;
    var reg = new RegExp("^eth");
    if (!reg.test(str))
        return false;
    
    return true; 
}
function NameBeginWithLoop(str){
    if (str === "")
        return false;
    var reg = new RegExp("^loop");
    if (!reg.test(str))
        return false;
    
    return true; 
}
function CreatePortClick(){
    if( port_type == 4 ){//如果已经有一个loop类型的端口，不能再创建端口
        alert("已经有一个loop类型的端口，不能再创建端口");
    }
    else{
        CreatePort();
    }
}
var num ;
function CreatePort(){
    var add_port_dlg_html = 
            '<form class="form-horizontal add_switchwildcard_port"><fieldset>  \
            <div class="control-group" id="first_port" >   \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="port.name_one"></OamStaticUI></label> \
                <div class="controls"> \
                    <input type="text" id="switchWildcard_port_name1" popover="wildcard_port_name_format"  /> \
                    <span class="help-inline switchWildcardPortAdd_tip">' + page.res[page.pagelan].type.display["wildcard_port_name_format"] + '</span>\
                </div> \
            </div> \
            <div class="control-group hide" id="second_port" >   \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="port.name_two"></OamStaticUI></label> \
                <div class="controls"> \
                    <input type="text" id="switchWildcard_port_name2" popover="wildcard_eth_port_name_format"  /> \
                    <span class="help-inline">' + page.res[page.pagelan].type.display["wildcard_eth_port_name_format"] + '</span>\
                </div> \
            </div> \
            <div class="control-group hide" id="third_port" >   \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="port.name_three"></OamStaticUI></label> \
                <div class="controls"> \
                    <input type="text" id="switchWildcard_port_name3" popover="wildcard_eth_port_name_format"  /> \
                    <span class="help-inline">' + page.res[page.pagelan].type.display["wildcard_eth_port_name_format"] + '</span>\
                </div> \
            </div> \
            <div class="control-group hide" id="forth_port" >   \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="port.name_four"></OamStaticUI></label> \
                <div class="controls"> \
                    <input type="text" id="switchWildcard_port_name4" popover="wildcard_eth_port_name_format"  /> \
                    <span class="help-inline">' + page.res[page.pagelan].type.display["wildcard_eth_port_name_format"] + '</span>\
                </div> \
            </div> \
            <div class="control-group hide" id="fifth_port" >   \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="port.name_five"></OamStaticUI></label> \
                <div class="controls"> \
                    <input type="text" id="switchWildcard_port_name5" popover="wildcard_eth_port_name_format"  /> \
                    <span class="help-inline">' + page.res[page.pagelan].type.display["wildcard_eth_port_name_format"] + '</span>\
                </div> \
            </div> \
            <div class="control-group hide" id="fixth_port" >   \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="port.name_six"></OamStaticUI></label> \
                <div class="controls"> \
                    <input type="text" id="switchWildcard_port_name6" popover="wildcard_eth_port_name_format"  /> \
                    <span class="help-inline">' + page.res[page.pagelan].type.display["wildcard_eth_port_name_format"] + '</span>\
                </div> \
            </div> \
            <div class="control-group hide" id="serventh_port" >   \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="port.name_serven"></OamStaticUI></label> \
                <div class="controls"> \
                    <input type="text" id="switchWildcard_port_name7" popover="wildcard_eth_port_name_format"  /> \
                    <span class="help-inline">' + page.res[page.pagelan].type.display["wildcard_eth_port_name_format"] + '</span>\
                </div> \
            </div> \
            <div class="control-group hide" id="eigth_port" >   \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="port.name_eight"></OamStaticUI></label> \
                <div class="controls"> \
                    <input type="text" id="switchWildcard_port_name8" popover="wildcard_eth_port_name_format"  /> \
                    <span class="help-inline">' + page.res[page.pagelan].type.display["wildcard_eth_port_name_format"] + '</span>\
                </div> \
            </div> \
            <div class="control-group">   \
                <div class="controls"> \
                    <a id="anotherPortCreate">' + page.res[page.pagelan].type.display["switch_wildcard_add_port"] +'</a>\
                    <a id="anotherPortDelete">'  + page.res[page.pagelan].type.display["switch_wildcard_del_port"] +'</a>\
                </div> \
            </div> \
            <div class="control-group  port_bondmode hide">   \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="port.bondmode"></OamStaticUI></label> \
                <div class="controls" > \
                    <select id="slport_bondmode" class="OamUi" bindattr="val"  bind="port_bondmode"></select>\
                </div> \
            </div> \
            <div class="control-group  name_same_tips hide">   \
                <div class="controls" > \
                    <span class="badge badge-warning">'+page.res[page.pagelan].type.display["name_same_error"] +'</span>\
                </div> \
            </div>  \
        </fieldset></form>'; 
  

    ShowDiaglog("add_port_dlg", add_port_dlg_html, {
    	show:function(){
            var h5 = $("#add_port_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-plus"></i>' + h5.text());       

            $("#switchWildcard_port_name1").val("");
            $("#switchWildcard_port_name2").val("");
            $("#switchWildcard_port_name3").val("");
            $("#switchWildcard_port_name4").val("");
            $("#switchWildcard_port_name5").val("");
            $("#switchWildcard_port_name6").val("");
            $("#switchWildcard_port_name7").val("");
            $("#switchWildcard_port_name8").val("");
            $("#slport_bondmode").empty();
            $("#slport_bondmode").append('<option value="1">' + page.res[page.pagelan].type.BondMode["1"]);
            $("#slport_bondmode").append('<option value="4">' + page.res[page.pagelan].type.BondMode["4"]);
            $("#anotherPortCreate").show();
            $("#anotherPortDelete").hide();
            $("#second_port").hide();
            $("#third_port").hide();
            $("#forth_port").hide();
            $("#fifth_port").hide();
            $("#fixth_port").hide();
            $("#serventh_port").hide();
            $("#eigth_port").hide();
            $(".name_same_tips").hide();
            if(switch_type == 1){//软件虚拟交换
                $(".switchWildcardPortAdd_tip").html( page.res[page.pagelan].type.display["wildcard_port_name_format"]  );
                if(port_num >= 1){
                    $(".switchWildcardPortAdd_tip").html( page.res[page.pagelan].type.display["wildcard_eth_port_name_format"]  );
                }
            }
            else if(switch_type == 2){//硬件虚拟交换
                $(".switchWildcardPortAdd_tip").html( page.res[page.pagelan].type.display["wildcard_eth_port_name_format"]  );
            }
            num =1;
            /*
            if( (port_num+num)<8){
                 $("#anotherPortCreate").show();
            }
            else{
                $("#anotherPortCreate").hide();
            }*/
            if(port_num ==7){
                $("#anotherPortCreate").hide();
            }
            $("#anotherPortCreate").unbind("click").bind("click",function(event){
                $(".add_switchwildcard_port div:hidden").first().show();
                num +=1 ;
                if(( port_num ==0) && num>=2){
                    $(".port_bondmode").show();
                }
                if((port_num+num) == 8){
                    $("#anotherPortCreate").hide();
                }
                if(num>=2){
                    $("#anotherPortDelete").show();
                }
                if(event.keyCode == 13){
                    event.preventDefault();
                }
            });
            $("#anotherPortDelete").unbind("click").bind("click",function(event){
                switch(num){
                    case 2:
                        $("#second_port").hide();
                        break;
                    case 3:
                        $("#third_port").hide();
                        break;
                    case 4:
                        $("#forth_port").hide();
                        break;
                    case 5:
                        $("#fifth_port").hide();
                        break;
                    case 6:
                        $("#fixth_port").hide();
                        break;
                    case 7:
                        $("#serventh_port").hide();
                        break;
                    case 8:
                        $("#eigth_port").hide();
                        break;
                }
                num -= 1;
                if((port_num ==0) && num<2){
                    $(".port_bondmode").hide();
                }
                if(num == 1){
                    $("#anotherPortDelete").hide();
                }
                if( (port_num+num)<8){
                    $("#anotherPortCreate").show();
                }
                $(".control-group").removeClass("error");
            });
            $("#add_port_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });
            $(".port_bondmode").hide();
            if( port_num == 1){
                $(".port_bondmode").show();
                //$(".switchWildcardPortAdd_tip").empty();
            } 
            $("#switchWildcard_port_name1").unbind("keypress").bind("keypress",function(event){
                if(event.keyCode == 13){
                   // event.preventDefault();
                }
            });

             $("#switchWildcard_port_name1").unbind("keydown").bind("keydown",function(event){
                var name_one = $("#switchWildcard_port_name1").val();
                if( NameBeginWithLoop( name_one) ){
                    $("#anotherPortCreate").hide();num = 1;
                    $("#anotherPortDelete").hide();
                    $("#second_port").hide();
                    $("#third_port").hide();
                    $("#forth_port").hide();
                    $("#fifth_port").hide();
                    $("#fixth_port").hide();
                    $("#serventh_port").hide();
                    $("#eigth_port").hide();
                }
                else{
                    if( (port_num+num)<8){
                        $("#anotherPortCreate").show();
                    }
                }
            });
 
            $("#port_info_dlg").modal("hide");
            $(".control-group").removeClass("error");
            TogglePopover($("#add_loop_dlg"));
    	},
    	ok:function(){            
            var name1 = $("#switchWildcard_port_name1").val();
            var name2 = $("#switchWildcard_port_name2").val();
            var name3 = $("#switchWildcard_port_name3").val();
            var name4 = $("#switchWildcard_port_name4").val();
            var name5 = $("#switchWildcard_port_name5").val();
            var name6 = $("#switchWildcard_port_name6").val();
            var name7 = $("#switchWildcard_port_name7").val();
            var name8 = $("#switchWildcard_port_name8").val();
           
            var type ;//根据输入判断
            var bondmode = 0 ;
            bondmode = parseInt($("#slport_bondmode").val());


            $(".control-group").removeClass("error");
            if(NameBeginWithLoop(name1) ){
                if(port_num == 0&& switch_type == 1){
                    type = 4;
                }
                else{
                $("#switchWildcard_port_name1").parent().parent().addClass("error");
                return;
                }
            }
            else{
                type = 0;
            }
            var add_port_num = num;         
            var names;
            var  add_port_names =[];
            switch(num){
                case 1:                     
                    if( name1=="" ){
                        $("#switchWildcard_port_name1").parent().parent().addClass("error"); 
                        return;                       
                    }   
                    add_port_names.push(name1);
                    break;
                case 2:
                    if( name1=="" ){
                        $("#switchWildcard_port_name1").parent().parent().addClass("error"); 
                        return;                       
                    } 
                    if( NameBeginWithLoop(name2)||name2=="" ){
                        $("#switchWildcard_port_name2").parent().parent().addClass("error"); 
                        return;                       
                    }   
                   
                    add_port_names.push(name1);
                    add_port_names.push(name2);

                    break;
                case 3:
                    if( name1=="" ){
                        $("#switchWildcard_port_name1").parent().parent().addClass("error"); 
                        return;                       
                    }
                    if( NameBeginWithLoop(name2)||name2=="" ){
                        $("#switchWildcard_port_name2").parent().parent().addClass("error"); 
                        return;                       
                    }
                    if(  NameBeginWithLoop(name3)||name3=="" ){
                        $("#switchWildcard_port_name3").parent().parent().addClass("error");
                        return;                       
                    }  

                    add_port_names.push(name1);
                    add_port_names.push(name2);
                    add_port_names.push(name3);
                    
                    break;
                case 4:
                    if( name1=="" ){
                        $("#switchWildcard_port_name1").parent().parent().addClass("error"); 
                        return;                       
                    }
                    if( NameBeginWithLoop(name2)||name2 == "" ){
                        $("#switchWildcard_port_name2").parent().parent().addClass("error"); 
                        return;                       
                    }
                    if(  NameBeginWithLoop(name3)||name3 == "" ){
                        $("#switchWildcard_port_name3").parent().parent().addClass("error");
                        return;                       
                    }
                    if(  NameBeginWithLoop(name4)||name4 == "" ){
                        $("#switchWildcard_port_name4").parent().parent().addClass("error");
                        return;                       
                    } 

                    add_port_names.push(name1);
                    add_port_names.push(name2);
                    add_port_names.push(name3);
                    add_port_names.push(name4);
                    
                    break;
                case 5:
                    if( name1=="" ){
                        $("#switchWildcard_port_name1").parent().parent().addClass("error"); 
                        return;                       
                    }
                    if( NameBeginWithLoop(name2)||name2 == "" ){
                        $("#switchWildcard_port_name2").parent().parent().addClass("error"); 
                        return;                       
                    }
                    if(  NameBeginWithLoop(name3)||name3 == "" ){
                        $("#switchWildcard_port_name3").parent().parent().addClass("error");
                        return;                       
                    }
                    if(  NameBeginWithLoop(name4)||name4 == "" ){
                        $("#switchWildcard_port_name4").parent().parent().addClass("error");
                        return;                       
                    }
                    if(  NameBeginWithLoop(name5)||name5 == "" ){
                        $("#switchWildcard_port_name5").parent().parent().addClass("error");
                        return;                       
                    }  

                    add_port_names.push(name1);
                    add_port_names.push(name2);
                    add_port_names.push(name3);
                    add_port_names.push(name4);
                    add_port_names.push(name5);
                    
                    break;
                case 6:
                    if( name1=="" ){
                        $("#switchWildcard_port_name1").parent().parent().addClass("error"); 
                        return;                       
                    }
                    if( NameBeginWithLoop(name2)||name2 == "" ){
                        $("#switchWildcard_port_name2").parent().parent().addClass("error"); 
                        return;                       
                    }
                    if(  NameBeginWithLoop(name3)||name3 == "" ){
                        $("#switchWildcard_port_name3").parent().parent().addClass("error");
                        return;                       
                    }
                    if(  NameBeginWithLoop(name4)||name4 == "" ){
                        $("#switchWildcard_port_name4").parent().parent().addClass("error");
                        return;                       
                    }
                    if(  NameBeginWithLoop(name5)||name5 == "" ){
                        $("#switchWildcard_port_name5").parent().parent().addClass("error");
                        return;                       
                    }
                    if(  NameBeginWithLoop(name6)||name6 == "" ){
                        $("#switchWildcard_port_name6").parent().parent().addClass("error");
                        return;                       
                    } 

                    add_port_names.push(name1);
                    add_port_names.push(name2);
                    add_port_names.push(name3);
                    add_port_names.push(name4);
                    add_port_names.push(name5);
                    add_port_names.push(name6);
                   
                    break;
                case 7:
                    if( name1=="" ){
                        $("#switchWildcard_port_name1").parent().parent().addClass("error"); 
                        return;                       
                    }
                    if( NameBeginWithLoop(name2)||name2 == "" ){
                        $("#switchWildcard_port_name2").parent().parent().addClass("error"); 
                        return;                       
                    }
                    if(  NameBeginWithLoop(name3)||name3 == "" ){
                        $("#switchWildcard_port_name3").parent().parent().addClass("error");
                        return;                       
                    }
                    if(  NameBeginWithLoop(name4)||name4 == "" ){
                        $("#switchWildcard_port_name4").parent().parent().addClass("error");
                        return;                       
                    }
                    if(  NameBeginWithLoop(name5)||name5 == "" ){
                        $("#switchWildcard_port_name5").parent().parent().addClass("error");
                        return;                       
                    }
                    if(  NameBeginWithLoop(name6)||name6 == "" ){
                        $("#switchWildcard_port_name6").parent().parent().addClass("error");
                        return;                       
                    }
                    if(  NameBeginWithLoop(name7)||name7 == "" ){
                        $("#switchWildcard_port_name7").parent().parent().addClass("error");
                        return;                       
                    }

                    add_port_names.push(name1);
                    add_port_names.push(name2);
                    add_port_names.push(name3);
                    add_port_names.push(name4);
                    add_port_names.push(name5);
                    add_port_names.push(name6);
                    add_port_names.push(name7);
               
                    break;
                case 8:
                    if( name1=="" ){
                        $("#switchWildcard_port_name1").parent().parent().addClass("error"); 
                        return;                       
                    }
                    if( NameBeginWithLoop(name2)||name2 == "" ){
                        $("#switchWildcard_port_name2").parent().parent().addClass("error"); 
                        return;                       
                    }
                    if(  NameBeginWithLoop(name3)||name3 == "" ){
                        $("#switchWildcard_port_name3").parent().parent().addClass("error");
                        return;                       
                    }
                    if(  NameBeginWithLoop(name4)||name4 == "" ){
                        $("#switchWildcard_port_name4").parent().parent().addClass("error");
                        return;                       
                    }
                    if(  NameBeginWithLoop(name5)||name5 == "" ){
                        $("#switchWildcard_port_name5").parent().parent().addClass("error");
                        return;                       
                    }
                    if(  NameBeginWithLoop(name6)||name6 == "" ){
                        $("#switchWildcard_port_name6").parent().parent().addClass("error");
                        return;                       
                    }
                    if(  NameBeginWithLoop(name7)||name7 == "" ){
                        $("#switchWildcard_port_name7").parent().parent().addClass("error");
                        return;                       
                    }
                    if(  NameBeginWithLoop(name8)||name8 == "" ){
                        $("#switchWildcard_port_name8").parent().parent().addClass("error");
                        return;                       
                    }
                    add_port_names.push(name1);
                    add_port_names.push(name2);
                    add_port_names.push(name3);
                    add_port_names.push(name4);
                    add_port_names.push(name5);
                    add_port_names.push(name6);
                    add_port_names.push(name7);
                    add_port_names.push(name8);
                    break;
            }
            //*
            var port_names = new Array();
            for(var i=0; i<switchWildcardData.length; i++){
                 if(switchWildcardData[i][0] == switch_uuid){
                      port_type = switchWildcardData[i][9];
                      port_names = switchWildcardData[i][8].split(",");
                      break;
                 }
            }
            for(var i=0; i<add_port_names.length; i++){
                for(var j=i+1; j<add_port_names.length; j++){
                    if(add_port_names[i] == add_port_names[j]){
                        $(".name_same_tips").show();
                        return;
                    }
                }
            }
            for(var i=0;i<add_port_names.length; i++){
                for(var j=0; j<port_names.length; j++){
                    if(add_port_names[i] == port_names[j]){
                        $(".name_same_tips").show();
                        return;
                    }
                }
            }
            names = add_port_names.toString();
           // */
            var rs = xmlrpc_service.tecs.vnet["wcswitch"].port.cfg(xmlrpc_session, switch_name,switch_uuid,switch_type,type,bondmode,OP_ADD_SWITCH_PORT,add_port_num,names);
            if(rs[0] == 0){ 
                $("#add_port_dlg").modal("hide");
                $("#port_info_dlg").modal("show");
                port_num = port_num + add_port_num ;
                
                SwitchWildcardRefreshClick();
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

var switch_name;
var switch_uuid;
var switch_type;
var port_num;
var port_type;
var switch_bondMode;

function switch_port_click(a){
    switch_name = $(a).attr("switch_name");
    switch_uuid = $(a).attr("SwitchId");
    switch_type = parseInt( $(a).attr("switch_type") );
    port_num = parseInt( $(a).text() ); 
    switch_bondMode   = $(a).parent().next().text();
    
    var portTableHtml = 
            '<div align="right">  \
                <button class="btn" id="portCreate"> \
                    <i class="icon-plus"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="create"/></span> \
                </button> \
                <button class="btn" id="portRemove"> \
                    <i class="icon-trash"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="remove"/></span> \
                </button> \
                <button class="btn" id="portRefresh"> \
                    <i class="icon-refresh"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="refresh"/></span> \
                </button> \
            </div> \
            <table id="switchWildcardPortList" class="table table-striped table-bordered table-condensed"> \
                <thead> \
                    <tr> \
                        <th align="left" width="25px;"><INPUT id="switchWildcardPortSelecltAll" type="checkbox" /></th> \
                        <th align="left" width="250px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="port.name"/></th> \
                    </tr> \
                </thead> \
                <tbody style="table-layout:fixed;word-break:break-all"> </tbody> \
            </table> \
            <div></div><br/>';
            
     ShowDiaglog("port_info_dlg", portTableHtml, {
        init:function(){
            switchWildcardPortTable = $("#switchWildcardPortList").dataTable({
                "sPaginationType": "bootstrap",
                "oLanguage": tableLanguage[page.pagelan],
	            "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 1 ] }],
                "bFilter":false,
                "alternatively": "destory"
            });
            
            $("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            }); 
            $("#portCreate").click(CreatePortClick);
            $("#portRemove").click(removePort);
            $("#portRefresh").click(SwitchWildcardRefreshClick);
            $("#switchWildcardPortSelecltAll").click(switchWildcardPortSelecltAllClick);
        },
        show: function(){
           var h5 = $("#port_info_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-plus"></i>' + switch_name + " "+ h5.text() );
            SwitchPortWildcardRefresh();
        },
        close: function(){
            $("#port_info_dlg").modal("hide");
        }
    });   
}
function switchWildcardPortSelecltAllClick(){
    if($(this).attr("checked") == "checked"){
        $("#switchWildcardPortList tbody  input[type=checkbox]").attr("checked", true);
    }
    else {
        $("#switchWildcardPortList tbody input[type=checkbox]").attr("checked", false);
    }
}

function SwitchPortWildcardRefresh(){
            if(switch_uuid == null){
                return;
            }           

            var port_names = new Array();
            for(var i=0; i<switchWildcardData.length; i++){
                if(switchWildcardData[i][0] == switch_uuid){
                    port_type = switchWildcardData[i][9];
                    port_names = switchWildcardData[i][8].split(",");
                    break;
                }
            }
    
            $("#portCreate").show();
            if(port_type == 4||port_num==8){
                $("#portCreate").hide();
            }            

            var portData = new Array();
            for(var i =0 ; i<port_names.length; i++){
                if(port_names[i] == ""){
                    continue;
                }
                portData[i] = new Array();
                portData[i][0] = "<input type = 'checkbox' value = '"+ switch_uuid +"'/>";  
                portData[i][1] = port_names[i];
            }
            switchWildcardPortTable.fnClearTable();
            switchWildcardPortTable.fnAddData(portData);
            $("#switchWildcardPortList  input[type=checkbox]").attr("checked", false);
}

var currSwitchWildcardState = new Object();  
function SwitchWildcardRefreshClick(){
    ClearCurSelItem(currSwitchWildcardState);  
    RecordSelItem(switchWildcardTable, "switch_wildcard_list", currSwitchWildcardState);   
    pageno = currSwitchWildcardState.pageno;
    
    $.getJSON("../php/wildcard_switch.php", function (json){
        switchWildcardTable.fnClearTable();

        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["2", json[1]]);
            return;
        }

        switchWildcardData = json[1];

        var tableData = [];

        for (var i = 0; i < json[1].length; i++) {
            var oneRow = [];
            //oneRow[0] = "<input type = 'checkbox' value = '"+ json[1][i][0] +"'/>"; 
            oneRow[0] = GetItemCheckbox(json[1][i][0], currSwitchWildcardState);
            oneRow[1] = json[1][i][1];             
            oneRow[2] = "<span value='" + json[1][i][2] + "'  pg_uuid='" + json[1][i][11] + "'>" + page.res[page.pagelan].type.SwitchType[json[1][i][2]] + "</span>";
            oneRow[3] = '<a href="javascript:void(0);" onclick="portgroup_info_click(this)">'+ json[1][i][3] + '</a>';   
            oneRow[4] = "<span value='" + json[1][i][4] + "'>" + page.res[page.pagelan].type.EVB[json[1][i][4]] + "</span>";    
            oneRow[5] = json[1][i][5];                
            oneRow[6] = "<a href='javascript:void(0)' onclick='switch_port_click(this)' SwitchId = '"+ json[1][i][0] + "' switch_name ='"+ json[1][i][1] 
                              + "' switch_type ='"+ json[1][i][2] + "'>" + json[1][i][6] +"</a>";          
            if(json[1][i][10] == null){
               oneRow[7] = json[1][i][10];
            }
            else{
                oneRow[7] = page.res[page.pagelan].type.BondMode[json[1][i][10]];
            }          
            tableData.push(oneRow);                          
        };

        switchWildcardTable.fnAddData(tableData);
        
        var oSettings = switchWildcardTable.fnSettings();
        var wholeNO = json[1].length /oSettings._iDisplayLength;

        if(wholeNO <= 1){
        }
        else if(wholeNO < (pageno +1) ){
            switchWildcardTable.fnPageChange("last");
        }
        else{
            switchWildcardTable.fnPageChange(parseInt(pageno));
        }
        
        $("#switch_wildcard_list  input[type=checkbox]").attr("checked", false);     
        SwitchPortWildcardRefresh(); 
    });
    RefreshSwitchWildcardTask();
}

function switch_wildcard_task_delete_click(a){
        ShowDiaglog("del_switch_task_dlg", page.res[page.pagelan]["xid"]["val"]["del_switch_task_confirm"], {
        	show:function(){
                var h5 = $("#del_switch_task_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-trash"></i>' + h5.text());    
        	},
        	ok:function(){
                $("#del_switch_task_dlg").modal("hide");
               
                var uuid = $(a).attr("value");
                var rs = xmlrpc_service.tecs.vnet["wcdeltask"].cfg(xmlrpc_session, VNET_LIB_WC_TASK_SWITCH, uuid);                     

                ShowSynRpcInfo(rs);            
                RefreshSwitchWildcardTask();
        	}
        });
}

function RefreshSwitchWildcardTask(){
    $.getJSON("../php/wildcard_task_switch.php",function(json){
        switchWildcardTaskTable.fnClearTable();
        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["2",json[1]]);
            return;
        }
        var switchTaskData = json[1];
        for(var i=0; i<json[1].length; i++){
            if(json[1][i][5] == 1){
                switchTaskData[i][5] = "<a onclick='switch_wildcard_task_delete_click(this)' value='"+ json[1][i][6] +"'>delete</a>";
            }
            else if(json[1][i][5] == 0){
                 switchTaskData[i][5] = "";
            }
        }
        switchWildcardTaskTable.fnAddData(switchTaskData);
    });
}

function SwitchWildcardSelecltAllClick(){
    if($(this).attr("checked") == "checked") {
        $("#switch_wildcard_list tbody input[type=checkbox]").attr("checked", true);
    }
    else {
        $("#switch_wildcard_list tbody input[type=checkbox]").attr("checked", false);
    }       
}

var switch_timer_id;
$(function(){
    switchTable = $("#switch_list").dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] }, 
                         { "bSortable": false, "aTargets": [ 3 ] },
                         { "bSortable": false, "aTargets": [ 7 ] }],
        "alternatively" : "destory"
    });
    
    $.getJSON("../php/switch_records.php",function(json){
        if(json[0] > 0){          
            $('#switchTab a:eq(1)').tab('show');
            refresh_switch();
        }
        else{
            $('#switchTab a:first').tab('show');
        }
    });

    //$('#switchTab a:first').tab('show');
    $('#switchTab a').click(function (e){
        e.preventDefault();
        $(this).tab('show');

        if($(this).attr("id") == "switchListNav"){
            refresh_switch();
        }

        if($(this).attr("id") == "switchPortNav"){
            $("#selectSwitch").trigger("change");
        }
    }); 

    $("#create_switch_link").click(function(e){
        $('#switchTab a').eq(1).click();
        $("#switchAdd").click();
    });    

    $("#selectSwitch").unbind("change").bind("change", function(){
        modify_switch_name = $("#selectSwitch").find("option:selected").text();
        modify_switch_uuid = $("#selectSwitch").val();
        modify_switch_type = $("#selectSwitch").find("option:selected").attr("type");

        refresh_switch_port();
    }); 

 

    InitSelectPortgroup($("#selectPortgroup"), "", SelectPortgroupChange);

    switchPortTable = $("#switch_port").dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "alternatively" : "destory"
    });    

    $("#switchAdd").parent().click(add_switch);
    $("#switchDel").parent().click(del_switch);
    $("#switchRefresh").parent().click(refresh_switch);
    $("#switchSelecltAll").click(switch_select_all_click);

    $("#switchPortModify").parent().click(modify_switch_ports);
    $("#switchPortRefresh").parent().click(refresh_switch_port);

    switchWildcardTable = $("#switch_wildcard_list").dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] }, 
                         { "bSortable": false, "aTargets": [ 3 ] },
                         { "bSortable": false, "aTargets": [ 6 ] }],        
        "alternatively" : "destory"
    });    

    switchWildcardTaskTable = $("#switch_wildcard_task_list").dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs":[{"bSortable":false,"aTargets":[5]}],
        "alternatively" : "destory"
    });    

    $("#switchWildcardAdd").parent().click(SwitchWildcardAddClick);
    $("#switchWildcardDel").parent().click(SwitchWildcardDelClick);
    $("#switchWildcardRefresh").parent().click(SwitchWildcardRefreshClick);
    $("#switchWildcardSelecltAll").click(SwitchWildcardSelecltAllClick);

    refresh_switch();      

    SwitchWildcardRefreshClick();
    RefreshSwitchWildcardTask();
    
    switch_timer_id    = setInterval(refresh_switch, 10000);
});

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