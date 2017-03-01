var OP_ADD = 1;
var OP_DEL = 2;

function addSDN(){

	var add_sdn_dlg_html = 
        '<div class="span8" style="solid #dddddd;padding-right:10px;"> \
            <div class="row-fluid"> \
            <form class="form-horizontal"> <fieldset>  \
            <div class="control-group">   \
                <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="sdn.protocolType"></OamStaticUI></label> \
                <div class="controls" style="margin-left:110px;"> \
                    <select id="sdnProtocolType"></select> \
                </div> \
            </div>    \
            <div class="control-group">   \
                <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="sdn.controllerIP"></OamStaticUI></label> \
                <div class="controls" style="margin-left:110px;"><input type="text" popover="ip_format" id="SDNcontrollerIP" maxlength="15" /></div> \
            </div>    \
            <div class="control-group">   \
                <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="sdn.portNumber"></OamStaticUI></label> \
                <div class="controls" style="margin-left:110px;"><input type="text" popover="sdn_port_format" id="SDNportNumber" maxlength="15" /></div> \
            </div>    \
            <div class="control-group">   \
                <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="sdn.adaptorIP"></OamStaticUI></label> \
                <div class="controls" style="margin-left:110px;"><input type="text" popover="ip_format" id="SDNadaptorIP" maxlength="15" /></div> \
            </div>    \
            </fieldset></form> \
            </div>\
        </div>';    

    ShowDiaglog("add_sdn_dlg", add_sdn_dlg_html, {
    	show:function(){
            var h5 = $("#add_sdn_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-plus"></i>' + h5.text()); 

            $("#add_sdn_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });  

            $("#sdnProtocolType").empty();  
            $("#sdnProtocolType").append('<option value="tcp">' + page.res[page.pagelan].type.SDNprotocolType["tcp"]);
            $("#sdnProtocolType").append('<option value="ssl">' + page.res[page.pagelan].type.SDNprotocolType["ssl"]); 

            TogglePopover($("#add_sdn_dlg"));
    	},
    	ok:function(){
            var sdnProtocol = $("#sdnProtocolType").val();
            var SDNcontrollerIP = $("#SDNcontrollerIP").val();
            var SDNportNumber   = parseInt($("#SDNportNumber").val());
            var SDNadaptorIP  = $("#SDNadaptorIP").val();


            $(".control-group").removeClass("error");         
            if(!checkIP(SDNcontrollerIP) ){
                $("#SDNcontrollerIP").parent().parent().addClass("error");
                return;
            }
            if(!checkIP(SDNadaptorIP) ){
                $("#SDNadaptorIP").parent().parent().addClass("error");
                return;
            }
            if(SDNportNumber <= 0){
                $("#SDNportNumber").parent().parent().addClass("error");
                return;
            }
            var rs = xmlrpc_service.tecs.vnet["sdn"].cfg(xmlrpc_session, "", sdnProtocol, SDNportNumber,SDNcontrollerIP, "", SDNadaptorIP, OP_ADD);//tecs.vnet.sdn.cfg

            if(rs[0] == 0){ 
                $("#add_sdn_dlg").modal("hide");
                refreshSDN();
            }
            else {
                ShowNotyRpcErrorInfo(rs);
            }
    	}
    });

}

function delSDN(){
    var vCheckedbox = $("#sdnList>tbody").find(":checkbox:checked");
    if(vCheckedbox.size() == 0){
        ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["must_select_one_error"]);
        return;
    }
    else{
        ShowDiaglog("del_sdn_dlg", page.res[page.pagelan]["xid"]["val"]["del_sdn_confirm"], {
        	show:function(){
                var h5 = $("#del_sdn_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-trash"></i>' + h5.text());    
        	},
        	ok:function(){
                $("#del_sdn_dlg").modal("hide");                  

                var rs = xmlrpc_service.tecs.vnet["sdn"].cfg(xmlrpc_session, sdn_uuid, "", 0, "", quantum_uuid, "",OP_DEL);    

                ShowSynRpcInfo(rs);
                refreshSDN();
            }  
        });
    }
}

var sdn_uuid;
var quantum_uuid;
function refreshSDN(){
    $.getJSON("../php/system_configuration.php",function(json){
        if(json[0] != 0){ 
            ShowNotyRpcErrorInfo(["11", json[1]]);
            return;
        }  
        var ackdata = json[1];
        if(json[1].length == 1){
            sdn_uuid = json[1][0][5];
            quantum_uuid =  json[1][0][6] ;
            $("#sdnCreate").parent().hide();
        }
        else{
            $("#sdnCreate").parent().show();
        }
        sdnTable.fnClearTable();
        sdnTable.fnAddData(ackdata); 
        ShowLastUpdateTime($("#sdnList").parent());
    });
}

function SDN_select_all_click(){
    if($(this).attr("checked") == "checked") {
        $("#sdnList tbody input[type=checkbox]").attr("checked", true);
    }
    else {
        $("#sdnList tbody input[type=checkbox]").attr("checked", false);
    }  
}



$(function(){
    sdnTable = $("#sdnList").dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] }],
        "alternatively" : "destory"
    }); 
    
    $("#sdnCreate").parent().click(addSDN);
    $("#sdnDelete").parent().click(delSDN);
    $("#sdnRefresh").parent().click(refreshSDN);
    $("#sdn_SelecltAll").click(SDN_select_all_click);
    
    refreshSDN();
});