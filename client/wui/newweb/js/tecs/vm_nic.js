function ShowNicDiag(diag, droper, vm_data, action, drag_elem_obj){
    var info = '';
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["logic_network_name"] +'</label>'
    info += '           <div class="controls">';
    info += '               <select id="sllogic_network_id"></select>';  
    info += '            </div>';
    info += '       </div>'; 
    
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["loop"] + '</label>'
    info += '           <div class="controls">';
    info += '               <input data-no-uniform="true" type="checkbox" class="iphone-toggle rdloop">';
    info += '           </div>';
    info += '       </div>';
    
    info += '       <div class="control-group">';
    info += '           <label class="control-label">MAC</label>'
    info += '           <div class="controls">';
    info += '               <input id="nic_mac" type="text"></input>';
    info += '           </div>';
    info += '       </div>';    
    
    info += '       <div class="control-group model">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["nic.model"] + '</label>'
    info += '           <div class="controls">';
    info += '               <select id="slmodel" class="OamUi" bindattr="val" bind="model"></select>';
    info += '            </div>';
    info += '       </div>';
    
    info += '       <div class="control-group nic_ip hide">';
    info += '           <label class="control-label">' + 'IP' + '</label>'
    info += '           <div class="controls">';
    info += '               <input id="nic_ip" type="text"></input>';
    info += '            </div>';
    info += '       </div>';

    info += '       <div class="control-group nic_ip hide">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["netmask"] + '</label>'
    info += '           <div class="controls">';
    info += '               <input id="nic_netmask" type="text"></input>';
    info += '            </div>';
    info += '       </div>';

    info += '       <div class="control-group nic_ip hide">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["gateway"] + '</label>'
    info += '           <div class="controls">';
    info += '               <input id="nic_gateway" type="text"></input>';
    info += '            </div>';
    info += '       </div>';            
    
    info += '   </fieldset>';
    info += '</form>';
    drag_elem_obj.showdiagheader_callback(droper.find("div h5").text(), drag_elem_obj, diag);
    diag.find('div.modal-body').html(info);

    InitNicModel($("#slmodel"));

    $("#sllogic_network_id").unbind("change").bind("change", function(){
        var params = [];
        params.push({"name":"logicnet_uuid", "value":$("#sllogic_network_id").val()});

        $.getJSON("../php/logicnetwork_ipmode.php", params, function(json){
            if(json[0] == 0 && json[1] == 2){
                $(".nic_ip").show();
            } 
            else{
                $(".nic_ip").hide();
            }

            if(json[0] != 0){
                ShowNotyRpcErrorInfo(["11", json[1]]);   
            }
        });
    });

    //逻辑网络ID初始化
    InitLogicNetwork($("#sllogic_network_id"), function(){
        drag_elem_obj.initdata_callback(diag, vm_data, droper.find("div h5").text(),drag_elem_obj);

        //iOS / iPhone style toggle switch
        diag.find('.rdloop').iphoneStyle({
            checkedLabel: page.res[page.pagelan].type.YesOrNo["1"],
            uncheckedLabel: page.res[page.pagelan].type.YesOrNo["0"]
        });        

        $("#sllogic_network_id").trigger("change");        
    });

    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        diag.find(".control-group").removeClass('error');

        var mac = $("#nic_mac").val();
        if(mac != "" && !checkMAC(mac)){
            $("#nic_mac").parent().parent().addClass('error');
            return;            
        }

        if($(".nic_ip").is(":visible")){
            var ip      = $("#nic_ip").val();
            if(!checkIP(ip)){
                $("#nic_ip").parent().parent().addClass('error');
                return;
            }

            var mask    = $("#nic_netmask").val();
            if(!checkIP(mask)){
                $("#nic_netmask").parent().parent().addClass('error');
                return;
            }

            var gateway = $("#nic_gateway").val();
            if(gateway != "" && !checkIP(gateway)){
                $("#nic_gateway").parent().parent().addClass('error');
                return;
            }
        }        

        UpdateNicData(diag, droper, vm_data, drag_elem_obj);
    });

    diag.modal("show");
}


function ShowNicsDetail(idx_pos,vm_data){ 

	var sSource = "../php/logicnetwork.php"; 
    var nic_ids =Array();
    var nicName = Array();

    for(var i = 0;i < vm_data["cfg_info"]["base_info"]["nics"].length;i++){
        nic_ids[i] = vm_data["cfg_info"]["base_info"]["nics"][i]["logic_network_id"];           
    }
    
    $.getJSON( sSource,function (json) {
        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["11", json[1]]);
            return;
        }

        for(var i=0; i<nic_ids.length; i++){
            for(var j=0;j<json[1].length;j++){
                if(nic_ids[i] == json[1][j]["uuid"]){
                    nicName[i] = json[1][j]["name"];
                    break;
                }
            }

            if(nicName[i] == null){
                nicName[i] = page.res[page.pagelan].type.err_info["vm_niclogicnet_null"];
            }
        } 
              
        var info = '';
        info += '<table class="table table-condensed">';
        info += '   <tbody>';
        for(var i = 0;i < vm_data["cfg_info"]["base_info"]["nics"].length;i++){
            var nic = vm_data["cfg_info"]["base_info"]["nics"][i];
            if( vm_data["netinfo"].hasOwnProperty("nic_info") ){
                nic = vm_data["netinfo"]["nic_info"][i];
            }

            info += '       <tr>';
            info += '           <td width="30%">' + GetNicName(nic) + '</td>';
            info += '           <td width="70%">';
            info += '               <table class="table table-condensed">';
            info += '                   <tbody>';
            info += '                       <tr>';
            info += '                           <td width="50%">' + page.res[page.pagelan].type.field_name["logic_network_name"] + '</td>';
            info += '                           <td width="50%">' +  nicName[i]+ '</td>';
            info += '                       </tr>'; 
            info += '                       <tr>';
            info += '                           <td width="50%">' + page.res[page.pagelan].type.field_name["loop"] + '</td>';
            info += '                           <td width="50%">' +  GetBool2Res(nic["loop"])+ '</td>';
            info += '                       </tr>'; 
            info += '                       <tr>';
            info += '                           <td width="50%">' + page.res[page.pagelan].type.field_name["nic.model"] + '</td>';
            info += '                           <td width="50%">' + nic["model"] + '</td>';
            info += '                       </tr>'; 
            info += '                       <tr>';
            info += '                           <td width="50%">IP</td>';
            info += '                           <td width="50%">' + nic["ip"] + '</td>';
            info += '                       </tr>';
            info += '                       <tr>';
            info += '                           <td width="50%">' + page.res[page.pagelan].type.field_name["netmask"] +'</td>';
            info += '                           <td width="50%">' + nic["netmask"] + '</td>';
            info += '                       </tr>';
            info += '                       <tr>';
            info += '                           <td width="50%">' + page.res[page.pagelan].type.field_name["gateway"] +'</td>';
            info += '                           <td width="50%">' + nic["gateway"] + '</td>';
            info += '                       </tr>';
            info += '                       <tr>';
            info += '                           <td width="50%">MAC</td>';
            info += '                           <td width="50%">' + GetMACInfo(nic["mac"]) + '</td>';
            info += '                       </tr>';
            info += '                   </tbody>';
            info += '               </table>';
            info += '           </td>';
            info += '       </tr>'; 
        }

        info += '   </tbody>';
        info += '</table>';
        idx_pos.html(info);
    }); 
}


//网卡
function query_nicnetwork(cluster_name,vm_id,nic_id){
    var rs = null;
    try{
        rs = xmlrpc_service.tecs.vnic.map.query(xmlrpc_session,cluster_name,vm_id,nic_id);
        if(rs[0] != 0){
            ShowNotyRpcErrorInfo(rs);
            rs = null;
        }
    }
    catch(e){
        var em;
        if (e.toTraceString){
            em = e.toTraceString;
        }
        else{
            em = e.message;
        }
        rs = "Error Trace:\n\n" + em;
        alert(rs);
        rs = null;
    }
    return rs;
}

function GetNicName(nic){
    var ret = '';
    var nic_index;
    if(nic.sriov == "0"){
        ret = page.res[page.pagelan].type.display["noraml_nic"];
        nic_index = nic.nic_index;
    }
    else{
        ret = page.res[page.pagelan].type.display["sriov_nic"];
        nic_index = nic.nic_index - 8;
    }
    ret += nic_index;
    return ret;
}

function GetMACInfo(mac){
    return mac == null ? "" : mac;
}

function ShowNormalNicHeader(nic_index, drag_elem_obj, diag){
    var diag_text = page.res[page.pagelan].type.display["noraml_nic"] + " " + nic_index;
    SetVmParaHeader(drag_elem_obj.diag_img, diag_text, diag);
}

function ShowSRIOVNicHeader(nic_index, drag_elem_obj, diag){
    var diag_text = page.res[page.pagelan].type.display["sriov_nic"] + " " + nic_index;
    SetVmParaHeader(drag_elem_obj.diag_img, diag_text, diag);
}

function InitLogicNetwork(sl, func){
    sl.empty();
    
	var sSource = "../php/logicnetwork.php"; 
    
	$.getJSON( sSource,function (json) {
        var logic_network_id =new Array();
		var logic_network_name =new Array();
        if(json[0] != 0){
            alert(json[1]);
            return;
        }
       
        for(var i = 0;i < json[1].length;i++){
            logic_network_id[i] = json[1][i]["uuid"];
			logic_network_name[i] = json[1][i]["name"];
        }  

        for(var i = 0;i < logic_network_id.length;i++){
            sl.append("<option value=" + logic_network_id[i] + ">" + logic_network_name[i] +"</option>");
        }

        func();
    });
}

function InitNicModel(sl){
   var nic_mode = [
        ["",page.res[page.pagelan].type.display["noassign"]],
        ["e1000","e1000"],
        ["rtl8139","rtl8139"], 
        ["ne2k_pci","ne2k_pci"],
        ["pcnet","pcnet"],
        ["netfront","netfront"]
    ];
    
    sl.empty();
    for(var i = 0;i < nic_mode.length;i++){
        var item = nic_mode[i];
        sl.append("<option value=" + item[0] + ">" + item[1] +"</option>");
    }
}
var logicnetwork_id ;
function GetFirstLogicNetwork(){
    var sSource = "../php/logicnetwork.php"; 
    
	 $.getJSON( sSource, function (json) {
        if(json[0] != 0){
            alert(json[1]);
			return null;
        }
	if(json[1][0] != null){
            logicnetwork_id = json[1][0]["uuid"];
        }
    });  
    return logicnetwork_id;
}

function Getloop(diag){
    if(diag.find(".rdloop").attr("checked") == "checked"){
        return true;
    }
    else{
        return false;
    }
}

function Setloop(isloop, diag){
    if(isloop){       
        diag.find(".rdloop").attr("checked", true);        
    }
    else{
        diag.find(".rdloop").attr("checked",false);
    }
} 

function InitNicData(diag, vm_data, nic_index, drag_elem_obj){
    var index = drag_elem_obj.getindex_callback(vm_data, nic_index);
    if(index != null){
        SetModifyForm(diag.find(".form-horizontal"), vm_data["cfg_info"]["base_info"]["nics"][index]);
        $("#sllogic_network_id").val(vm_data["cfg_info"]["base_info"]["nics"][index]["logic_network_id"]);
        Setloop(vm_data["cfg_info"]["base_info"]["nics"][index]["loop"], diag);
        $("#nic_mac").val(vm_data["cfg_info"]["base_info"]["nics"][index]["mac"]);
        $("#nic_ip").val(vm_data["cfg_info"]["base_info"]["nics"][index]["ip"]);
        $("#nic_netmask").val(vm_data["cfg_info"]["base_info"]["nics"][index]["netmask"]);
        $("#nic_gateway").val(vm_data["cfg_info"]["base_info"]["nics"][index]["gateway"]);
    }
}

function InitNoarmNicData(diag, vm_data, nic_index, drag_elem_obj){
    if(vm_data["cfg_info"]["base_info"]["virt_type"] == "hvm"){
        diag.find('div.modal-body').find(".model").show();
    }
    InitNicData(diag, vm_data, nic_index, drag_elem_obj);
}

function InitSRIOVNicData(diag, vm_data, nic_index, drag_elem_obj){ 
    diag.find('div.modal-body').find(".model").hide();
    InitNicData(diag, vm_data, nic_index, drag_elem_obj);
}


function UpdateNicData(diag, droper, vm_data, drag_elem_obj){
    var ret = drag_elem_obj.updatedatabyformdata_callback(diag.find(".form-horizontal"), vm_data, droper.find("div h5").text(), drag_elem_obj);
    if(ret){
        diag.modal("hide");
    }
}

function UpdateNormalNicDataByForm(frm, vm_data, nic_index, drag_elem_obj){

    var index = drag_elem_obj.getindex_callback(vm_data, nic_index); 
    if(index != null){
        var form_data                                               = GetModifyForm(frm);
        vm_data["cfg_info"]["base_info"]["nics"][index]["logic_network_id"]     = $("#sllogic_network_id").val();
        vm_data["cfg_info"]["base_info"]["nics"][index]["loop"]                 = Getloop(frm);
        vm_data["cfg_info"]["base_info"]["nics"][index]["mac"]                  = $("#nic_mac").val();
        
        if($(".nic_ip").is(":visible")){
            vm_data["cfg_info"]["base_info"]["nics"][index]["ip"]                      = $("#nic_ip").val();
            vm_data["cfg_info"]["base_info"]["nics"][index]["netmask"]                 = $("#nic_netmask").val();
            vm_data["cfg_info"]["base_info"]["nics"][index]["gateway"]                 = $("#nic_gateway").val();
        }
        else {
            vm_data["cfg_info"]["base_info"]["nics"][index]["ip"]                      = "";
            vm_data["cfg_info"]["base_info"]["nics"][index]["netmask"]                 = "";
            vm_data["cfg_info"]["base_info"]["nics"][index]["gateway"]                 = "";
        }
        
        if(vm_data["cfg_info"]["base_info"]["virt_type"] == "hvm"){
            vm_data["cfg_info"]["base_info"]["nics"][index]["model"] = form_data["model"];
        }
        else{
            vm_data["cfg_info"]["base_info"]["nics"][index]["model"] = "";
        }

        UpdateNicMac(vm_data["cfg_info"]["base_info"]["nics"][index]["nic_index"],  
                     vm_data["cfg_info"]["base_info"]["context"], 
                     vm_data["cfg_info"]["base_info"]["nics"][index]["mac"]);

        return true;
    }
    return false;
}

function UpdateSRIOVNicDataByForm(frm, vm_data, nic_index, drag_elem_obj){
    
    var index = drag_elem_obj.getindex_callback(vm_data, nic_index); 
    if(index != null){
        var form_data                                    = GetModifyForm(frm);
     	vm_data["cfg_info"]["base_info"]["nics"][index]["logic_network_id"]  = $("#sllogic_network_id").val();
        vm_data["cfg_info"]["base_info"]["nics"][index]["loop"]      = Getloop(frm);
        vm_data["cfg_info"]["base_info"]["nics"][index]["mac"]       = $("#nic_mac").val();        
        vm_data["cfg_info"]["base_info"]["nics"][index]["model"]     = "";

        if($(".nic_ip").is(":visible")){
            vm_data["cfg_info"]["base_info"]["nics"][index]["ip"]                      = $("#nic_ip").val();
            vm_data["cfg_info"]["base_info"]["nics"][index]["netmask"]                 = $("#nic_netmask").val();
            vm_data["cfg_info"]["base_info"]["nics"][index]["gateway"]                 = $("#nic_gateway").val();
        }
        else {
            vm_data["cfg_info"]["base_info"]["nics"][index]["ip"]                      = "";
            vm_data["cfg_info"]["base_info"]["nics"][index]["netmask"]                 = "";
            vm_data["cfg_info"]["base_info"]["nics"][index]["gateway"]                 = "";
        }

        UpdateNicMac(vm_data["cfg_info"]["base_info"]["nics"][index]["nic_index"], 
                     vm_data["cfg_info"]["base_info"]["context"], 
                     vm_data["cfg_info"]["base_info"]["nics"][index]["mac"]);

        return true;
    }
    return false;
}

function AddDefaultNormalNicData(nic_index,vm_data){
    var nic          = new Object();
    nic.nic_index    = parseInt(nic_index);
    nic.sriov        = false;
    nic.model        = "";
    nic.loop         = false;
    nic.logic_network_id = logicnetwork_id;//默认的ID
    nic.mac          = "";
    nic.ip           = "";
    nic.netmask      = "";
    nic.gateway      = "";
    vm_data["cfg_info"]["base_info"]["nics"].push(nic); 
    AddNicContext(nic.nic_index, vm_data["cfg_info"]["base_info"]["context"]);
    return true;
}

function AddDefaultSRIOVNicData(nic_index,vm_data){
    var nic          = new Object();
    //var logic_network_id = GetFirstLogicNetwork();
    nic.nic_index    = parseInt(nic_index) + 8;
    nic.sriov        = true;
    nic.model        = "";
    nic.loop         = false;
    nic.mac          = "";    
    nic.logic_network_id = logicnetwork_id;//默认的ID
    nic.ip           = "";
    nic.netmask      = "";
    nic.gateway      = "";    
    vm_data["cfg_info"]["base_info"]["nics"].push(nic);
    AddNicContext(nic.nic_index, vm_data["cfg_info"]["base_info"]["context"]);
    return true;
}

function GetIndexOfNormalNic(vm_data, nic_index){
    for(var i = 0;i < vm_data["cfg_info"]["base_info"]["nics"].length;i++){
        var nic = vm_data["cfg_info"]["base_info"]["nics"][i];
        if(nic.nic_index == nic_index){
            return i;
        }
    } 
    return null;
}

function GetIndexOfSRIOVNic(vm_data, nic_index){
    for(var i = 0;i < vm_data["cfg_info"]["base_info"]["nics"].length;i++){
        var nic = vm_data["cfg_info"]["base_info"]["nics"][i];
        if(nic.nic_index == (parseInt(nic_index) + 8)){
            return i;
        }
    } 
    return null;
}

function GetIndexOfNic(nic_index, nics){
    for(var i = 0;i < nics.length;i++){
        var nic = nics[i];
        if(parseInt(nic.nic_index) == nic_index){
            return i;
        }
    } 
    return null;
}

function AddNicData(droper, drag_elem_obj, vm_data, action, diag, drager){
    var ret = drag_elem_obj.adddefaultdata_callback(droper.find("div h5").text(), vm_data);
    if(!ret){
        return;
    }
    drag_elem_obj.showimg_callback(droper, drag_elem_obj, vm_data);
    return;
}

function ShowNormalNicImage(droper, drag_elem_obj, vm_data){
    for(var i = 0;i < droper.length;i++){
        var droper_pos = droper.eq(i);
        var nic_index = droper_pos.find("div h5").text();
        for(var j = 0;j < vm_data["cfg_info"]["base_info"]["nics"].length;j++){
            var nic = vm_data["cfg_info"]["base_info"]["nics"][j];
            if(nic.nic_index == nic_index){
                var info = '<span class="span12"><img src="img/' + drag_elem_obj.img + '"/></span>';  
                var class_name = drag_elem_obj.clname; 
                info += '<span class="pull-right icon-trash ' + class_name + '" style="cursor:pointer;"></span>'; 
                droper_pos.find(".box-content").find(".span12").html(info); 
                droper_pos.addClass(class_name).addClass("configed");
                droper.data("data", drag_elem_obj);
            }
        }
    }
}

function ShowSRIOVNicImage(droper, drag_elem_obj, vm_data){
    for(var i = 0;i < droper.length;i++){
        var droper_pos = droper.eq(i);
        var nic_index = droper_pos.find("div h5").text();
        for(var j = 0;j < vm_data["cfg_info"]["base_info"]["nics"].length;j++){
            var nic = vm_data["cfg_info"]["base_info"]["nics"][j];
            if(nic.nic_index - 8 == nic_index){
                var info = '<span class="span12"><img src="img/' + drag_elem_obj.img + '"/></span>';  
                var class_name = drag_elem_obj.clname; 
                info += '<span class="pull-right icon-trash ' + class_name + '" style="cursor:pointer;"></span>'; 
                droper_pos.find(".box-content").find(".span12").html(info); 
                droper_pos.addClass(class_name).addClass("configed");
                droper.data("data", drag_elem_obj);
            }
        }
    }
}

function DeleteNicDataCallBack(diag, droper, vm_data, drag_elem_obj){ 
    var index = drag_elem_obj.getindex_callback(vm_data, droper.find("div h5").text());
    if(index != null){ 
        DeleteNicContext(vm_data["cfg_info"]["base_info"]["nics"][index]["nic_index"], vm_data["cfg_info"]["base_info"]["context"]);
        vm_data["cfg_info"]["base_info"]["nics"].splice(index,1);
    }
    droper.find(".box-content").find(".span12").html("");
    droper.removeClass("configed");
    diag.modal("hide");
}

function DeleteNicsData(diag, droper, vm_data, drag_elem_obj){
    diag.find('div.modal-header h5').html(page.res[page.pagelan].type.display["confirm_op"]);
    diag.find('div.modal-body').html(page.res[page.pagelan].type.err_info["delete_confirm_info"]);
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        DeleteNicDataCallBack(diag, droper, vm_data, drag_elem_obj);
    });
    diag.modal("show");
}

function CheckNicModelValid(virt_type,nics){
    /*var bIsNetFront = false;
    if(virt_type == "hvm"){ 
        for(var i = 0;i < nics.length;i++){
            var nic = nics[i];
            if(nic.model == "netfront"){
                bIsNetFront = true;
                break;
            }
        }
        if(bIsNetFront){
            for(var i = 0;i < nics.length;i++){
                var nic = nics[i];
                if(nic["sriov"] == 0 && nic.model != "netfront"){
                    ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["vm_nicmodel_desc"]);
                    return false;
                }
            }
        }
    }*/

    for(var i = 0;i < nics.length;i++){
        var nic = nics[i];
        if(nic["logic_network_id"] == null){
            ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["vm_niclogicnet_desc"]);
            return false;
        }
    }
    return true;
}

function ReorderNics(nics){
    var nicsNotSriov = new Array();
    var nicsSriov = new Array();
    var j =0,k=0;
    for(var i = 0;i < nics.length;i++){      
        if(nics[i].sriov == false){
            nicsNotSriov[j] = nics[i];
            j++;
        }
        else if(nics[i].sriov == true){
            nicsSriov[k] = nics[i];
            k++;
        }        
    }
    for(var i =0;i< nicsNotSriov.length;i++){
        nicsNotSriov[i].nic_index = i+1;
    }
    for(var i = 0;i< nicsSriov.length;i++){
        nicsSriov[i].nic_index = i+9;
    }
    nics = nicsNotSriov.concat(nicsSriov);
    return nics;
}