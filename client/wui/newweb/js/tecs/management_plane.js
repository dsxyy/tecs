var EN_ADD_KERNALPORT = 1;
var EN_DEL_KERNALPORT = 2;
var EN_MOD_KERNALPORT = 3;
var EN_ADD_WC_PORT = 1;
var EN_DEL_WC_PORT = 2;
var PORT_TYPE_KERNEL = 3;

var VNET_LIB_WC_TASK_KERNEL = 1;

function removeKernel(){
    var vCheckedbox = $(kernelTable.fnGetNodes()).find(":checkbox:checked");

    if(vCheckedbox.size() > 0) {
        ShowDiaglog("delete_kernel_dlg", page.res[page.pagelan]["xid"]["val"]["delete_kernel_confirm"], {
            show:function(){
                var h5 = $("#delete_kernel_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-trash"></i>' + page.res[page.pagelan].type.opr["remove"]);
            },
            ok:function(){
                vCheckedbox.each(function(){                   
                    var uuid = $(this).attr("value");
                    var type = parseInt( $(this).attr("kerneltype") );
                    var switchname = $(this).attr("switchname"); 
                    var vnauuid = $(this).attr("vna");
                    xmlrpc_service.tecs.vnet["kernalport"].cfg(xmlrpc_session, "", uuid, vnauuid,  "", "", "", type, EN_DEL_KERNALPORT, switchname,function(rs){  
                        if(rs[0] == 0){ 
                            $("#delete_kernel_dlg").modal("hide");
                           // kernelTable.fnDraw();
                            RefreshKernel();
                        }
                        else {
                            ShowNotyRpcErrorInfo(rs);
                        }                       
                    });                   
                });
            }
        });
    }
    else {
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]); 
    }
}

function checkKernelName(str){
     if (str === "")
        return false;
    var reg = new RegExp("^kernel[A-Za-z0-9_]{1,9}$");
    if (!reg.test(str))
        return false;
    
    return true;
}

function createKernel(){
    var add_kernel_dlg_html = 
        '<form class="form-horizontal"> <fieldset>  \
            <div class="control-group">   \
                <label class="control-label"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="kernel.name"></OamStaticUI></span></label> \
                <div class="controls">\
				    <input type="text" id="kernel_name" popover="kernel_name_format" value="kernel" maxlength="15" />\
				</div> \
            </div>    \
            <div class="control-group">\
                <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="kernel.isdhcp"></OamStaticUI></label> \
                <div class="controls"><select id="kernel_isdhcp"></select></div>\
            </div>\
            <div class="control-group kernelIP hide">   \
                <label class="control-label"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="kernel.ip"></OamStaticUI></span></label> \
                <div class="controls"><input type="text" popover="ip_format" id="kernel_ip" /></div> \
            </div>    \
            <div class="control-group kernelMask hide">   \
                <label class="control-label"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="kernel.mask"></OamStaticUI></span></label> \
                <div class="controls"><input type="text" popover="mask_format" id="kernel_mask" /></div> \
            </div>    \
            <div class="control-group">   \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="kernel.switchList"></OamStaticUI></label> \
                <div class="controls"><select id="kernel_switch"></select></div> \
            </div>    \
            <div class="control-group">   \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="kernel.type"></OamStaticUI></label> \
                <div class="controls"> <select id="kernel_type"></select></div> \
            </div>    \
            <div class="control-group">\
                <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="kernel.pgList"></OamStaticUI></label> \
                <div class="controls">\
                    <select id="kernel_pg"></select> \
                </div>\
            </div>\
            <div class="control-group">   \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="kernel.vnaList"></OamStaticUI></label> \
                <div class="controls"> \
				    <select id="kernel_vna"></select> \
				</div> \
            </div>    \
        </fieldset></form>';    

    ShowDiaglog("add_kernel_dlg", add_kernel_dlg_html, {
    	show:function(){
            $("#kernel_name").val("kernel");
            var h5 = $("#add_kernel_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-plus"></i>' + h5.text() ); 

            $("#add_kernel_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });  

            $("#kernel_switch").empty();
            $.getJSON("../php/kernel_switch.php", function(json){
                if(json[0] != 0){
                    alert(json[1]);
                    return;
                }

                for (var i = 0; i < json[1].length; i++) {
                    $("#kernel_switch").append("<option value='" + json[1][i]["name"] +"'>" + json[1][i]["name"]);
                } 
            });

            pg_mgr = Array();
            pg_storge = Array();
            
            $("#kernel_pg").empty();
            $.getJSON("../php/kernel_portgroup.php", function(json){
                if(json[0] != 0){
                    alert(json[1]);
                    return;
                }
                for (var i = 0,j=0,k=0; i < json[1].length; i++) {
                    
                    if(json[1][i]["pg_type"] == 10 ){
                        pg_mgr[j] = json[1][i];
                        
                        $("#kernel_pg").append("<option value='" + json[1][i]["uuid"] +"'>" + pg_mgr[j]["name"]);
                        j++;
                    }
                    if(json[1][i]["pg_type"] == 11){
                        pg_storge[k] = json[1][i];
                        k++;                   
                    }
                } 
            });
       
            $("#kernel_vna").empty();
            $.getJSON("../php/kernel_vna.php", function(json){
                if(json[0] != 0){
                    alert(json[1]);
                    return;
                }

                for (var i = 0; i < json[1].length; i++) {
                    $("#kernel_vna").append("<option value='" + json[1][i]["uuid"] +"'>" + json[1][i]["uuid"]);
                } 
            });
                                   
            $("#kernel_type").empty();  
            $("#kernel_type").append('<option value="10">' + page.res[page.pagelan].type.KernelType["0"]);
            $("#kernel_type").append('<option value="11">' + page.res[page.pagelan].type.KernelType["1"]); 
            
            $("#kernel_isdhcp").empty();
            $("#kernel_isdhcp").append('<option value="1">' + page.res[page.pagelan].type.KernelDhcpType["1"]);
            $("#kernel_isdhcp").append('<option value="0">' + page.res[page.pagelan].type.KernelDhcpType["0"]);
                       
            $("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });

            $("#kernel_isdhcp").unbind("change").bind("change", function (){
                var dhcp = parseInt($("#kernel_isdhcp").val());
                if(dhcp == 0){
                    $(".kernelIP").show();
                    $(".kernelMask").show();
                }
                else if(dhcp == 1){
                    $(".kernelIP").hide();
                    $(".kernelMask").hide();
                }
            });
            $("#kernel_isdhcp").trigger("change");
            
            
            $("#kernel_type").unbind("change").bind("change",function(){
                var type = parseInt($("#kernel_type").val());
                if(type == 10){//MGR
                    $("#kernel_pg").empty();
                    for (var i = 0; i < pg_mgr.length; i++){
                        $("#kernel_pg").append("<option value='" + pg_mgr[i]["uuid"] +"'>" + pg_mgr[i]["name"]);
                    }                                     
                }
                else if(type == 11){
                    $("#kernel_pg").empty();               
                    for (var i = 0; i < pg_storge.length; i++){
                        $("#kernel_pg").append("<option value='" + pg_storge[i]["uuid"] +"'>" + pg_storge[i]["name"]);
                    }   
                }
            });

            TogglePopover($("#add_kernel_dlg"));
    	},
    	ok:function(){
            var name = $("#kernel_name").val();
            var type = parseInt($("#kernel_type").val());
			var switch_name = $("#kernel_switch").val();
            var vna_uuid = $("#kernel_vna").val();
            var pg_uuid =$("#kernel_pg").val();
            var dhcp = parseInt($("#kernel_isdhcp").val());
            var ip = "";
            var mask = "";
            if(dhcp == 0){
                ip  = $("#kernel_ip").val();
                mask  = $("#kernel_mask").val();

                if(!checkIP(ip) ){
                    $("#kernel_ip").parent().parent().addClass("error");
                    return;
                }
                if(checkIP(mask) == false){
                    $("#kernel_mask").parent().parent().addClass("error");
                    return;
                }             
                var ip_array = ip.split(".");
                var mask_array = mask.split(".");
                for(var i=0; i<ip_array.length;i++){
                    ip_array[i] = parseInt( ip_array[i] );
                    mask_array[i] = parseInt( mask_array[i]);
                    var result = ip_array[i] & (~mask_array[i]);
                    if(result != 0){
                        break;
                    }
                    if(i == 3 && result == 0){
                        $("#kernel_ip").parent().parent().addClass("error");
                        $("#kernel_mask").parent().parent().addClass("error");
                        return; 
                    }
                }
            }
                       
            $(".control-group").removeClass("error");
            if(!checkKernelName(name)){
                $("#kernel_name").parent().parent().addClass("error");
                return;
            }
			if(pg_uuid == null){
                $("#kernel_pg").parent().parent().addClass("error");
                return;
            }
            xmlrpc_service.tecs.vnet["kernalport"].cfg(xmlrpc_session, name, "", vna_uuid, pg_uuid, ip, mask, type, EN_ADD_KERNALPORT, switch_name,function(rs){
                if(rs[0] == 0){ 
                    $("#add_kernel_dlg").modal("hide");
                    RefreshKernel();
                }
                else {
                    ShowNotyRpcErrorInfo(rs);
                }
            });
    	}
    });
	
}

function  KernelSelecltAll(){
       if($("#kernelSelecltAll").attr("checked") == "checked"){
	        $("#kernelList tbody input[type=checkbox]").attr("checked", true); 
	   }
	   else{
	        $("#kernelList tbody input[type=checkbox]").attr("checked", false); 
	   }
}

var currKernelState = new Object();  
function RefreshKernel(){
    if($("#kernelList").is(":visible")){  
    }
    else {
        clearInterval(kernel_timer_id);
        return;
    }  
   
    ClearCurSelItem(currKernelState);  
    RecordSelItem(kernelTable, "kernelList", currKernelState);   
    pageno = currKernelState.pageno;
    
    $.getJSON("../php/net_port_kernel.php",function(json){
        kernelTable.fnClearTable();
	    var kernelInfo = json.data;
		
		if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
            return;
        }
	for(var i=0;i<json.data.aaData.length;i++){
        if(json.data.aaData[i][4] == 0){
           /* kernelInfo.aaData[i][0] = "<input type='checkbox'  disabled='disabled' value="+"'"+json.data.aaData[i]["uuid"] +"'  "  +"vna='"+ json.data.aaData[i]["vna_uuid"]+"'  "   +"kerneltype='"+ json.data.aaData[i][8]+"'  "   +"switchname='"+ json.data.aaData[i][10]+ "'/>";*/
        var keys = new Array("vna","kerneltype","switchname");
        var values = new Array(json.data.aaData[i]["vna_uuid"], json.data.aaData[i][8], json.data.aaData[i][10]);
 
        kernelInfo.aaData[i][0] = GetItemCheckboxWithFunc(json.data.aaData[i]["uuid"], currKernelState,keys,values);
        
        }
        else{
           /* kernelInfo.aaData[i][0] = "<input type='checkbox' value="+"'"+json.data.aaData[i]["uuid"] +"'  "  +"vna='"+ json.data.aaData[i]["vna_uuid"]+"'  "   +"kerneltype='"+ json.data.aaData[i][8]+"'  "  +"pguuid='"+ json.data.aaData[i]['pg_uuid']+"'  "  +"switchname='"+ json.data.aaData[i][10]+ "'/>";*/
           var keys = new Array("vna", "kerneltype", "pguuid","switchname" );
           var values = new Array(json.data.aaData[i]["vna_uuid"],json.data.aaData[i][8],json.data.aaData[i]['pg_uuid'],json.data.aaData[i][10] );
           kernelInfo.aaData[i][0] = GetItemCheckboxWithFunc(json.data.aaData[i]["uuid"], currKernelState,keys,values);
           }
        kernelInfo.aaData[i][4] = page.res[page.pagelan].type.VNAIsOnline[json.data.aaData[i][4]];
        kernelInfo.aaData[i][8] = page.res[page.pagelan].type.kernel_type[json.data.aaData[i][8]];
        kernelInfo.aaData[i][9] = '<a href="javascript:void(0);" onclick="plane_portgroup_info_click(this)">'+ json.data.aaData[i][9] + '</a>'; 	
	}

        kernelTable.fnAddData(kernelInfo.aaData);
        
        var oSettings = kernelTable.fnSettings();
        var wholeNO = json.data.aaData.length /oSettings._iDisplayLength;

        if(wholeNO <= 1){
        }
        else if(wholeNO < (pageno +1) ){
            kernelTable.fnPageChange("last");
        }
        else{
            kernelTable.fnPageChange(parseInt(pageno));
        }
            
        $("#kernellist_last_refresh_time").html(GetLastUpdateTime());
	});
        $("#kernelList input[type=checkbox]").attr("checked", false); 
}

function plane_portgroup_info_click(pg){
    var pg_name = $(pg).text();
    var pg_uuid = $(pg).parent().parent().find("td input").eq(0).attr('pguuid');
    portgroup_info_common(pg_uuid,pg_name);
}

function checkKernelWildcardName(str){
     if (str === "")
        return false;
    var reg = new RegExp("^kernelw[A-Za-z0-9_]{1,8}$");
    if (!reg.test(str))
        return false;
 
    return true;
}
function KernelWildcardAddClick(){
    var add_kernelcfg_dlg_html = 
        '<form class="form-horizontal"> <fieldset>  \
            <div class="control-group">   \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="kernel.name"></OamStaticUI></label> \
                <div class="controls"> \
				    <input type="text" id="kernelWildCardname" popover="kernel_wildcard_name_format" value="kernelw" maxlength="15"/>\
				</div> \
            </div>    \
            <div class="control-group">   \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="kernel.type"></OamStaticUI></label> \
                <div class="controls"> \
                    <select id="wildcard_kernel_type"></select> \
                </div> \
            </div>    \
            <div class="control-group">\
                <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="kernel.pgList"></OamStaticUI></label> \
                <div class="controls">\
                    <select id="wildcard_kernel_pg"></select> \
                </div>\
            </div>\
            <div class="control-group">   \
                <label class="control-label"><OamStaticUI bindattr="text" maptype="display" bindvalue="kernel.switchList"></OamStaticUI></label> \
                <div class="controls"><select id="wildcard_kernel_switch"></select></div> \
            </div>    \
        </fieldset></form>';  
        
     ShowDiaglog("add_kernelcfg_dlg", add_kernelcfg_dlg_html, {
    	show:function(){
            $("#kernelWildCardname").val("kernelw");
            var h5 = $("#add_kernelcfg_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-plus"></i>' + h5.text()); 

            $("#add_kernelcfg_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });  
                                   
            $("#wildcard_kernel_type").empty();  
            $("#wildcard_kernel_type").append('<option value="10">' + page.res[page.pagelan].type.KernelType["0"]);
            $("#wildcard_kernel_type").append('<option value="11">' + page.res[page.pagelan].type.KernelType["1"]); 

            $("#wildcard_kernel_switch").empty();
            $.getJSON("../php/wildcard_kernel_switch.php", function(json){
                if(json[0] != 0){
                    alert(json[1]);
                    return;
                }

                for (var i = 0; i < json[1].length; i++) {
                    $("#wildcard_kernel_switch").append("<option value='" + json[1][i]["uuid"] +"'>" + json[1][i]["name"]);
                } 
            });
            wildcard_pg_mgr = Array();
            wildcard_pg_storge = Array();
            $("#wildcard_kernel_pg").empty();
            $.getJSON("../php/kernel_portgroup.php", function(json){
                if(json[0] != 0){
                    alert(json[1]);
                    return;
                }
                for (var i = 0,j=0,k=0; i < json[1].length; i++) {
                    
                    if(json[1][i]["pg_type"] == 10 ){
                        wildcard_pg_mgr[j] = json[1][i];
                        
                        $("#wildcard_kernel_pg").append("<option value='" + json[1][i]["uuid"] +"'>" + wildcard_pg_mgr[j]["name"]);
                        j++;
                    }
                    if(json[1][i]["pg_type"] == 11){
                        wildcard_pg_storge[k] = json[1][i];
                        k++;                   
                    }
                } 
            });
            $("#wildcard_kernel_type").unbind("change").bind("change",function(){
                var type = parseInt($("#wildcard_kernel_type").val());
                if(type == 10){//MGR
                    $("#wildcard_kernel_pg").empty();
                    for (var i = 0; i < wildcard_pg_mgr.length; i++){
                        $("#wildcard_kernel_pg").append("<option value='" + wildcard_pg_mgr[i]["uuid"] +"'>" + wildcard_pg_mgr[i]["name"]);
                    }                                     
                }
                else if(type == 11){
                    $("#wildcard_kernel_pg").empty();               
                    for (var i = 0; i < wildcard_pg_storge.length; i++){
                        $("#wildcard_kernel_pg").append("<option value='" + wildcard_pg_storge[i]["uuid"] +"'>" + wildcard_pg_storge[i]["name"]);
                    }   
                }
            });
            $("#kernelWildCardname").unbind("keypress").bind("keypress",function(event){
                if(event.keyCode == 13){
                    event.preventDefault();
                }
            });
            TogglePopover($("#add_kernelcfg_dlg"));
    	},
    	ok:function(){
            var name = $("#kernelWildCardname").val();
            var type = parseInt($("#wildcard_kernel_type").val());
            var pg = $("#wildcard_kernel_pg").val();
            var switchUuid = $("#wildcard_kernel_switch").val();
             
            $(".control-group").removeClass("error");
            if(!checkKernelWildcardName(name)){
                $("#kernelWildCardname").parent().parent().addClass("error");
                return;
            }
			
            var rs = xmlrpc_service.tecs.vnet["wcvport"].cfg(xmlrpc_session, name, "", PORT_TYPE_KERNEL, type, pg, switchUuid, EN_ADD_WC_PORT);

            if(rs[0] == 0){ 
                
                $("#add_kernelcfg_dlg").modal("hide");
                KernelWildcardRefreshClick();
            }
            else {
                ShowNotyRpcErrorInfo(rs);
            }
    	}
    });
	
}

function KernelWildcardDelClick(){
   
   var vCheckedbox = $(kernelWildcardTable.fnGetNodes()).find(":checkbox:checked");

    if(vCheckedbox.size() > 0) {
        ShowDiaglog("delete_kernel_dlg", page.res[page.pagelan]["xid"]["val"]["delete_kernel_confirm"], {
            show:function(){
                var h5 = $("#delete_kernel_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-trash"></i>' + h5.text());
            },
            ok:function(){
                vCheckedbox.each(function(){
                    var uuid = $(this).attr("value");                  
                    var rs = xmlrpc_service.tecs.vnet["wcvport"].cfg(xmlrpc_session, "", uuid, PORT_TYPE_KERNEL, 0, "",  "", EN_DEL_WC_PORT); 
                    $("#delete_kernel_dlg").modal("hide");
                    if(rs[0] == 0){  
                        ShowNetSynRpcInfo(rs);                    
                    }
                    else {
                        ShowNotyRpcErrorInfo(rs);
                    }
                });
                KernelWildcardRefreshClick();
            }
        });
    }
    else {
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]); 
    }    
}

function KernelWildcardRefreshClick(){
    $.getJSON("../php/wildcard_kernel.php", function (json){
        kernelWildcardTable.fnClearTable();

        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["2", json[1]]);
            return;
        }

        var taskData = json[1];

        for (var i = 0; i < json[1].length; i++) {
            //json[1][i][0] = "<input type='checkbox' value="+"'"+json[1][i][0] +"'/>";
            //taskData.push(json[1]);
            taskData[i][0] = "<input type='checkbox' value="+"'"+json[1][i][0] +"'  pguuid="+"'"+json[1][i][5] +"'/>";
			taskData[i][3] = '<a href="javascript:void(0);" onclick="plane_portgroup_info_click(this)">'+ json[1][i][3] + '</a>'; 	
        };

        kernelWildcardTable.fnAddData(taskData);
        $("#kernel_wildcard_list input[type=checkbox]").attr("checked",false);
    });
    RefreshKernelTask();
}

function KernelWildcardSelecltAllClick(){
    if($(this).attr("checked") == "checked") {
        $("#kernel_wildcard_list tbody input[type=checkbox]").attr("checked", true);
    }
    else {
        $("#kernel_wildcard_list tbody input[type=checkbox]").attr("checked", false);
    }    
}

function kernel_wildcard_task_delete_click(a){
        ShowDiaglog("del_kernel_task_dlg", page.res[page.pagelan]["xid"]["val"]["del_kernel_task_confirm"], {
        	show:function(){
                var h5 = $("#del_kernel_task_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-trash"></i>' + h5.text());    
        	},
        	ok:function(){
                $("#del_kernel_task_dlg").modal("hide");
               
                var uuid = $(a).attr("value");
                var rs = xmlrpc_service.tecs.vnet["wcdeltask"].cfg(xmlrpc_session, VNET_LIB_WC_TASK_KERNEL,  uuid);                     

                ShowSynRpcInfo(rs);            
                RefreshKernelTask();
        	}
        });
}
function RefreshKernelTask(){
    $.getJSON("../php/wildcard_kernel_task.php",function(json){
        kernelWildcardTaskTable.fnClearTable();

        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["2",json[1]]);
            return;
        }
        var kernelTaskData = json[1];
        for(var i=0; i<json[1].length; i++){
            kernelTaskData[i][3] = page.res[page.pagelan]["type"]["kernel_type"][json[1][i][3]];
            if(json[1][i][6] == 1){
                kernelTaskData[i][6] = "<a onclick='kernel_wildcard_task_delete_click(this)' value='"+ json[1][i][7] +"'>delete</a>";
            }
            else if(json[1][i][6] == 0){
                 kernelTaskData[i][6] = "";
            }
        }
        kernelWildcardTaskTable.fnAddData(kernelTaskData);
    });
}
 
$(function(){
    $('#kernelTab a:first').tab('show');
    $('#kernelTab a').click(function (e){
        e.preventDefault();
        $(this).tab('show');

        if($(this).attr("id") == "kernelInfoNav"){
            RefreshKernel();
        }
    }); 

    //datatable
    kernelTable = $("#kernelList").dataTable({
                "sPaginationType": "bootstrap",
                "oLanguage": tableLanguage[page.pagelan],
                "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] }],
                "alternatively": "destory"          
            });

    // OamUi静态显示部分初始化
    $("OamStaticUI").each(function(){
        SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
    });

    $("#kernelSelecltAll").click(KernelSelecltAll);
    $("#create").click(createKernel);
    $("#remove").click(removeKernel);
    $("#refresh").click(RefreshKernel);
    
    kernelWildcardTable = $("#kernel_wildcard_list").dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] }, 
                         { "bSortable": false, "aTargets": [ 3 ] }],        
        "alternatively" : "destory"
    });    

    kernelWildcardTaskTable = $("#kernel_wildcard_task_list").dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs":[{"bSortable":false, "aTargets":[6]}],
        "alternatively" : "destory"
    });    

    $("#kernelWildcardAdd").parent().click(KernelWildcardAddClick);
    $("#kernelWildcardDel").parent().click(KernelWildcardDelClick);
    $("#kernelWildcardRefresh").parent().click(KernelWildcardRefreshClick);
    $("#kernelWildcardSelecltAll").click(KernelWildcardSelecltAllClick);
    RefreshKernel();
    KernelWildcardRefreshClick();
    RefreshKernelTask();
    
    kernel_timer_id    = setInterval(RefreshKernel, 30000);
});
            