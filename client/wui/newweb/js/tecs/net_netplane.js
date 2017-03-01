var EN_ADD_NETPLANE = 1;
var EN_DEL_NETPLANE = 2;
var EN_MOD_NETPLANE = 3;
var EN_ADD_IPV4_RANGE = 1;
var EN_DEL_IPV4_RANGE = 2;
var EN_ADD_MAC_RANGE = 1; 
var EN_DEL_MAC_RANGE = 2;
var EN_ADD_VLAN_RANGE = 1;
var EN_DEL_VLAN_RANGE = 2;
var EN_ADD_SEGMENT_RANGE = 1;
var EN_DEL_SEGMENT_RANGE = 2;
var oNetplaneDbt;
function ShowBaseInfo(div, uuid){
    var sSource = "../php/netplane_base.php"; 
    var aoData  = { uuid: uuid};
    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        if(json.errcode != ""){ 
            ShowNotyQueryDBErrorInfo(json);
            return;
        }
        for(var i = 0;i < ackdata.aaData.length;i++){ 
            var info = '';
            info += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
            info += '   <tbody>';
            info += '       <tr>';
            info += '           <td width="30%">' + page.res[page.pagelan].xid.html["net_t1"] + '</td>';
            info += '           <td>' + ackdata.aaData[i][0] + '</td>';
            info += '       </tr>';
            
            info += '       <tr>';
            info += '           <td width="30%">MTU</td>';
            info += '           <td>' + ackdata.aaData[i][1] + '</td>';
            info += '       </tr>';
            
            info += '       <tr>';
            info += '           <td width="30%">' + page.res[page.pagelan].xid.html["net_t2"] + '</td>';
            info += '           <td>' + ackdata.aaData[i][2] + '</td>';
            info += '       </tr>';
            info += '   </tbody>';
            info += '</table>';
            div.html(info);  
        }
    });
}

function ShowIPrangeInfo(div, uuid){
    var sSource = "../php/netplane_iprange.php"; 
    var aoData  = { uuid: uuid};
    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        if(json.errcode != ""){ 
            ShowNotyQueryDBErrorInfo(json);
            return;
        }
        var info = '';
        info += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
        info += '   <tbody>';
        info += '       <tr>';
        info += '           <td width="40%">' + page.res[page.pagelan].type.display["ip_begin"] + '</td>';
        info += '           <td>' + page.res[page.pagelan].type.display["ip_end"] + '</td>';
        info += '       </tr>';
        for(var i = 0;i < ackdata.aaData.length;i++){ 
            info += '       <tr>';
            info += '           <td>' + ackdata.aaData[i][0] + '</td>';
            info += '           <td>' + ackdata.aaData[i][1] + '</td>';
            info += '       </tr>';
        }
        info += '   </tbody>';
        info += '</table>';
        div.html(info);
    });
}

function ShowMACrangeInfo(div, uuid){
    var sSource = "../php/netplane_macrange.php"; 
    var aoData  = { uuid: uuid};
    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        if(json.errcode != ""){ 
            ShowNotyQueryDBErrorInfo(json);
            return;
        }
        var info = '';
        info += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
        info += '   <tbody>';
        info += '       <tr>';
        info += '           <td width="40%">'+page.res[page.pagelan].type.display["startpos"] +'</td>';
        info += '           <td>' + page.res[page.pagelan].type.display["endpos"] + '</td>';
        info += '       </tr>';
        for(var i = 0;i < ackdata.aaData.length;i++){ 
            info += '       <tr>';
            info += '           <td>' + ackdata.aaData[i][0] + '</td>';
            info += '           <td>' + ackdata.aaData[i][1] + '</td>';
            info += '       </tr>';
        }
        info += '   </tbody>';
        info += '</table>';
        div.html(info);
    });
}

function ShowVLANrangeInfo(div, uuid){
    var sSource = "../php/netplane_vlanrange.php"; 
    var aoData  = { uuid: uuid};
    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        if(json.errcode != ""){ 
            ShowNotyQueryDBErrorInfo(json);
            return;
        }
        var info = '';
        info += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
        info += '   <tbody>';
        info += '       <tr>';
        info += '           <td width="40%">' + page.res[page.pagelan].type.display["vlan_begin"] + '</td>';
        info += '           <td>' + page.res[page.pagelan].type.display["vlan_end"] + '</td>';
        info += '       </tr>';
        info += '   </thead>';
        for(var i = 0;i < ackdata.aaData.length;i++){ 
            info += '       <tr>';
            info += '           <td>' + ackdata.aaData[i][0] + '</td>';
            info += '           <td>' + ackdata.aaData[i][1] + '</td>';
            info += '       </tr>';
        }
        info += '   </tbody>';
        info += '</table>';
        div.html(info);
    });
}

function ShowSegmentIDRangeInfo(div,uuid){
    var sSource = "../php/netplane_segmentIDrange.php"; 
    var aoData  = { uuid: uuid};
    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        if(json.errcode != ""){ 
            ShowNotyQueryDBErrorInfo(json);
            return;
        }
        var info = '';
        info += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
        info += '   <tbody>';
        info += '       <tr>';
        info += '           <td width="40%">' + page.res[page.pagelan].type.display["SegmentID_begin"] + '</td>';
        info += '           <td>' + page.res[page.pagelan].type.display["SegmentID_end"] + '</td>';
        info += '       </tr>';
        info += '   </thead>';
        for(var i = 0;i < ackdata.aaData.length;i++){ 
            info += '       <tr>';
            info += '           <td>' + ackdata.aaData[i][0] + '</td>';
            info += '           <td>' + ackdata.aaData[i][1] + '</td>';
            info += '       </tr>';
        }
        info += '   </tbody>';
        info += '</table>';
        div.html(info);
    });
}

function ShowNetPlaneDetail(uuid){
    var diag = $("#idx_detailDlg");
    var info = '';
    var netplane_obj_list = new Array();
    var netplane_obj = new Object();
    netplane_obj.title = page.res[page.pagelan].type.display["baseinfo"];
    netplane_obj.key = "tabBase";
    netplane_obj.show_callback = ShowBaseInfo;
    netplane_obj_list.push(netplane_obj);
    
    netplane_obj = new Object();
    netplane_obj.title = page.res[page.pagelan].type.display["IPrange"];
    netplane_obj.key = "tabIPrange";
    netplane_obj.show_callback = ShowIPrangeInfo;
    netplane_obj_list.push(netplane_obj);
    
    netplane_obj = new Object();
    netplane_obj.title = page.res[page.pagelan].type.display["MACrange"];
    netplane_obj.key = "tabMACrange";
    netplane_obj.show_callback = ShowMACrangeInfo;
    netplane_obj_list.push(netplane_obj); 
     
    netplane_obj = new Object();
    netplane_obj.title = page.res[page.pagelan].type.display["VLANrange"];
    netplane_obj.key = "tabVLANrange";
    netplane_obj.show_callback = ShowVLANrangeInfo;
    netplane_obj_list.push(netplane_obj); 
    
    netplane_obj = new Object();
    netplane_obj.title = page.res[page.pagelan].type.display["SegmentIDRange"];
    netplane_obj.key = "tabSegmentIDRangeRange";
    netplane_obj.show_callback = ShowSegmentIDRangeInfo;
    netplane_obj_list.push(netplane_obj);
    
    info += '<div class="container-fluid">';
    info +=     '<div class="row-fluid">';
    info +=         '<div class="span4 netplane-menu-span">';
    info +=             '<div class="well nav-collapse sidebar-nav">';
    info +=                 '<ul class="nav nav-tabs nav-stacked netplane-menu">';
    for(var i = 0;i < netplane_obj_list.length;i++){
        var netplane_obj = netplane_obj_list[i];
        info +=                 '<li>';
        info +=                     '<a href="#" data-value="' + netplane_obj.key + '">';
        info +=                         '<span class="hidden-tablet">' + netplane_obj.title + '</span>';
        info +=                     '</a>';
        info +=                 '</li>';
    }
    info +=                 '</ul>';
    info +=             '</div>';
    info +=         '</div>';
    info +=         '<div class="span8">';
    info +=             '<div class="row-fluid">'; 
    info +=                 '<div class="span12" id="netplane_right_content">';
    info +=                 '</div>';
    info +=             '</div>';
    info +=         '</div>';
    info +=     '</div>';
    info += '</div>';
    
    var title = '<i class="icon-zoom-in"></i> <span>' + page.res[page.pagelan].type.opr["detail"] + '</span>'; 
    diag.find('div.modal-header h5').html(title); 
    
    diag.find('div.modal-body').html(info);
    //animating menus on hover
    $('ul.netplane-menu li:not(.nav-header)').hover(function(){
        $(this).animate({'margin-left':'+=5'},300);
    },
    function(){
        $(this).animate({'margin-left':'-=5'},300);
    });
    $('ul.netplane-menu li a').click(function( event ) {
        var $item = $( this );
        var data_value = $item.attr("data-value");
        $('ul.netplane-menu li.active').removeClass('active');
        $item.parent('li').addClass('active');
        diag.find('#netplane_right_content').html("");
        for(var i = 0;i < netplane_obj_list.length;i++){
            var netplane_obj = netplane_obj_list[i];
            if(data_value == netplane_obj.key){ 
                var div = diag.find('#netplane_right_content');
                div.html("");
                div.append('<div id="loading" class="center">Loading...<div class="center"></div></div>');
                netplane_obj.show_callback(div, uuid);
                break;
            }
        }
    });
    $('ul.netplane-menu li:eq(0)').addClass("active");
    $('ul.netplane-menu li a:eq(0)').click();
    diag.modal("show");
    return;
} 

function Get_IPRange_List(ackdata){ 
    var iprange_list = new Array();
    for(var i = 0;i < ackdata.aaData.length;i++){
        var iprange_obj = new Object();
        iprange_obj.IPv4Start = ackdata.aaData[i][0];
        iprange_obj.IPv4End   = ackdata.aaData[i][1];
        iprange_obj.IPv4Mask   = ackdata.aaData[i][2];
        iprange_list.push(iprange_obj);
    } 
    return iprange_list;
}

function Get_MACRange_List(ackdata){
    var macrange_list = new Array();
    for(var i = 0;i < ackdata.aaData.length;i++){
        var macrange_obj = new Object();
        macrange_obj.MACStart = ackdata.aaData[i][0];
        macrange_obj.MACEnd   = ackdata.aaData[i][1];
        macrange_list.push(macrange_obj);
    } 
    return macrange_list;
} 

function Get_VLANRange_List(ackdata){
    var vlanrange_list = new Array();
    for(var i = 0;i < ackdata.aaData.length;i++){
        var vlanrange_obj = new Object();
        vlanrange_obj.vlanStart = ackdata.aaData[i][0];
        vlanrange_obj.vlanEnd   = ackdata.aaData[i][1];
        vlanrange_list.push(vlanrange_obj);
    } 
    return vlanrange_list;
}

function Get_IPRange_Title(iprange_obj){
    var title = "";
    title = iprange_obj.IPv4Start + " ~ " + iprange_obj.IPv4End;
    return title;
} 

function Get_MACRange_Title(macrange_obj){
    var title = "";
    title = macrange_obj.MACStart + " ~ " + macrange_obj.MACEnd;
    return title;
}

function Get_VLANRange_Title(vlanrange_obj){
    var title = "";
    title = vlanrange_obj.vlanStart + " ~ " + vlanrange_obj.vlanEnd;
    return title;
}

function Get_IPRange_UpdateFrm_Html(){
    var info = '';
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    info += '       <div class="control-group startpos">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.display["startpos"] +'</label>'
    info += '           <div class="controls">';
    info += '               <input type="text" popover="ip_format" class="OamUi span12" bindattr="val" bind="IPv4Start">'; 
    info += '               <span class="help-inline"></span>';
    info += '            </div>';
    info += '       </div>';
    
    info += '       <div class="control-group endpos">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.display["endpos"] +'</label>'
    info += '           <div class="controls">';
    info += '               <input type="text" popover="ip_format" class="OamUi span12" bindattr="val" bind="IPv4End">';
    info += '               <span class="help-inline"></span>';
    info += '            </div>';
    info += '       </div>';

    info += '       <div class="control-group mask">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.display["mask"] +'</label>'
    info += '           <div class="controls">';
    info += '               <input type="text" popover="mask_format" class="OamUi span12" bindattr="val" bind="IPv4Mask">';
    info += '               <span class="help-inline"></span>';
    info += '            </div>';
    info += '       </div>';
    
    info += '   </fieldset>';
    info += '</form>';
    
    info += '<table style="width: 100%; word-break: break-all;" class="table">';
    info += '<tbody>';       
    info += '    <tr style="display: table-row;" >';          
    info += '        <td width="40%">' + page.res[page.pagelan].type.display["startpos"] +'</td>';           
    info += '        <td><span class="span12" bind="IPv4Start"></span></td>';      
    info += '    </tr>';       
    info += '    <tr style="display: table-row;" >';         
    info += '        <td width="40%">' + page.res[page.pagelan].type.display["endpos"] +'</td>';           
    info += '        <td><span class="span12" bind="IPv4End"></span></td>';       
    info += '    </tr>';       
    info += '    <tr style="display: table-row;">';
    info += '        <td width="40%">' + page.res[page.pagelan].type.display["mask"] +'</td>';
    info += '        <td><span class="span12" bind="IPv4Mask"></span></td>';
    info += '    </tr>';
    info += '</tbody></table>'; 

    return info;
}

function Get_MACRange_UpdateFrm_Html(){
    var info = '';
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    info += '       <div class="control-group startpos">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.display["startpos"] +'</label>'
    info += '           <div class="controls">';
    info += '               <input type="text" popover="mac_format" class="OamUi span12" bindattr="val" bind="MACStart">';
    info += '               <span class="help-inline"></span>';
    info += '            </div>';
    info += '       </div>';
    
    info += '       <div class="control-group endpos">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.display["endpos"] +'</label>'
    info += '           <div class="controls">';
    info += '               <input type="text" popover="mac_format" class="OamUi span12" bindattr="val" bind="MACEnd">';
    info += '               <span class="help-inline"></span>';
    info += '            </div>';
    info += '       </div>';
    
    info += '   </fieldset>';
    info += '</form>';
    
    info += '<table style="width: 100%; word-break: break-all;" class="table">';
    info += '<tbody>';       
    info += '    <tr style="display: table-row;" >';          
    info += '        <td width="40%">' + page.res[page.pagelan].type.display["startpos"] +'</td>';           
    info += '        <td><span class="span12" bind="MACStart"></span></td>';      
    info += '    </tr>';       
    info += '    <tr style="display: table-row;" >';         
    info += '        <td width="40%">' + page.res[page.pagelan].type.display["endpos"] +'</td>';           
    info += '        <td><span class="span12" bind="MACEnd"></span></td>';       
    info += '    </tr>';       
    info += '</tbody></table>'; 

    return info;
} 

function Get_VLANRange_UpdateFrm_Html(){
    var info = '';
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    info += '       <div class="control-group startpos">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.display["startpos"] +'</label>'
    info += '           <div class="controls">';
    info += '               <input id="txVlanStart" popover="2~4094" type="text" class="OamUi span12" bindattr="val" bind="vlanStart">';
    info += '               <span class="help-inline"></span>';
    info += '            </div>';
    info += '       </div>';
    
    info += '       <div class="control-group endpos">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.display["endpos"] +'</label>'
    info += '           <div class="controls">';
    info += '               <input id="txVlanEnd" popover="2~4094" type="text" class="OamUi span12" bindattr="val" bind="vlanEnd">';
    info += '               <span class="help-inline"></span>';
    info += '            </div>';
    info += '       </div>';
    
    info += '   </fieldset>';
    info += '</form>';
    
    info += '<table style="width: 100%; word-break: break-all;" class="table">';
    info += '<tbody>';       
    info += '    <tr style="display: table-row;" >';          
    info += '        <td width="40%">' + page.res[page.pagelan].type.display["startpos"] +'</td>';           
    info += '        <td><span class="span12" bind="vlanStart"></span></td>';      
    info += '    </tr>';       
    info += '    <tr style="display: table-row;" >';         
    info += '        <td width="40%">' + page.res[page.pagelan].type.display["endpos"] +'</td>';           
    info += '        <td><span class="span12" bind="vlanEnd"></span></td>';       
    info += '    </tr>';       
    info += '</tbody></table>'; 
    
    return info;
}  

function Check_IPRange_UpdateFrm_Data_Valid(frm, range_list){
    var form_data = GetModifyForm(frm);
    frm.find(".startpos").removeClass("error");
    frm.find(".endpos").removeClass("error"); 
    frm.find(".help-inline").html("");
    if(form_data["IPv4Start"] == ""){
        frm.find(".startpos").addClass("error");
        frm.find("div.startpos .help-inline").html(page.res[page.pagelan].type.err_info["ip_noempty_input_error"]);
        return false;
    }
    if(!checkIP(form_data["IPv4Start"])){
        frm.find(".startpos").addClass("error");
        frm.find("div.startpos .help-inline").html(page.res[page.pagelan].type.err_info["IP_novalid"]);
        return false;
    }
    if(form_data["IPv4End"] == ""){
        frm.find(".endpos").addClass("error");
        frm.find("div.endpos .help-inline").html(page.res[page.pagelan].type.err_info["ip_noempty_input_error"]);
        return false;
    }
    if(!checkIP(form_data["IPv4End"])){
        frm.find(".endpos").addClass("error");
        frm.find("div.endpos .help-inline").html(page.res[page.pagelan].type.err_info["IP_novalid"]);
        return false;
    }
    if(IP2Decimal(form_data["IPv4Start"]) > IP2Decimal(form_data["IPv4End"])){
        frm.find(".endpos").addClass("error");
        frm.find("div.endpos .help-inline").html(page.res[page.pagelan].type.err_info["must_endpos_gt_startpos"]);
        return false;
    }
    return true;
}

function Check_MACRange_UpdateFrm_Data_Valid(frm, range_list){
    var form_data = GetModifyForm(frm);
    frm.find(".startpos").removeClass("error");
    frm.find(".endpos").removeClass("error"); 
    frm.find(".help-inline").html("");
    if(form_data["MACStart"] == ""){
        frm.find(".startpos").addClass("error");
        frm.find("div.startpos .help-inline").html(page.res[page.pagelan].type.err_info["mac_noempty_input_error"]);
        return false;
    }
    if(!checkMAC(form_data["MACStart"])){
        frm.find(".startpos").addClass("error");
        frm.find("div.startpos .help-inline").html(page.res[page.pagelan].type.err_info["MAC_novalid"]);
        return false;
    }
    if(form_data["MACEnd"] == ""){
        frm.find(".endpos").addClass("error");
        frm.find("div.endpos .help-inline").html(page.res[page.pagelan].type.err_info["mac_noempty_input_error"]);
        return false;
    }
    if(!checkMAC(form_data["MACEnd"])){
        frm.find(".endpos").addClass("error");
        frm.find("div.endpos .help-inline").html(page.res[page.pagelan].type.err_info["MAC_novalid"]);
        return false;
    }
    if(MAC2Decimal(form_data["MACStart"]) > MAC2Decimal(form_data["MACEnd"])){
        frm.find(".endpos").addClass("error");
        frm.find("div.endpos .help-inline").html(page.res[page.pagelan].type.err_info["must_endpos_gt_startpos"]);
        return false;
    }
    return true;
} 

function Check_VLANRange_UpdateFrm_Data_Valid(frm, range_list){
    var form_data = GetModifyForm(frm);
    frm.find(".startpos").removeClass("error");
    frm.find(".endpos").removeClass("error"); 
    frm.find(".help-inline").html("");
    if(form_data["vlanStart"] == ""){
        frm.find(".startpos").addClass("error");
        frm.find("div.startpos .help-inline").html(page.res[page.pagelan].type.err_info["vlan_noempty_input_error"]);
        return false;
    }
    if(form_data["vlanEnd"] == ""){
        frm.find(".endpos").addClass("error");
        frm.find("div.endpos .help-inline").html(page.res[page.pagelan].type.err_info["vlan_noempty_input_error"]);
        return false;
    }
    if(parseInt(form_data["vlanStart"]) > parseInt(form_data["vlanEnd"])){
        frm.find(".endpos").addClass("error");
        frm.find("div.endpos .help-inline").html(page.res[page.pagelan].type.err_info["must_endpos_gt_startpos"]);
        return false;
    }
    return true;
}

function Save_IPRange_UpdateFrm_Data(diag, range_list){
    if($('ul.range-menu li.active').length == 0){
        return true;
    } 
    var key = $('ul.range-menu li.active a').attr("data-value");
    return Update_IPRange_UpdateFrm_Data(diag, $('ul.range-menu li.active a'), range_list);
}

function Save_MACRange_UpdateFrm_Data(diag, range_list){
    if($('ul.range-menu li.active').length == 0){
        return true;
    } 
    var key = $('ul.range-menu li.active a').attr("data-value");
    return Update_MACRange_UpdateFrm_Data(diag, $('ul.range-menu li.active a'), range_list);
} 

function Save_VLANRange_UpdateFrm_Data(diag, range_list){
    if($('ul.range-menu li.active').length == 0){
        return true;
    } 
    var key = $('ul.range-menu li.active a').attr("data-value");
    return Update_VLANRange_UpdateFrm_Data(diag, $('ul.range-menu li.active a'), range_list);
} 

function Update_IPRange_UpdateFrm_Data(diag, alink, range_list){
    var old_key   = alink.attr("data-value");
    if(diag.find("#range_right_content").html() == ""){//首次加载,没有数据
        return true;
    }
    if(!Check_IPRange_UpdateFrm_Data_Valid(diag, range_list)){
        return false;
    }
    var form_data = GetModifyForm(diag);
    range_list[old_key] = form_data;
    var title = Get_IPRange_Title(form_data);
    alink.find("span").eq(0).html(title);
    return true;
}

function Update_MACRange_UpdateFrm_Data(diag, alink, range_list){
    var old_key   = alink.attr("data-value");
    if(diag.find("#range_right_content").html() == ""){//首次加载,没有数据
        return true;
    }
    if(!Check_MACRange_UpdateFrm_Data_Valid(diag, range_list)){
        return false;
    }
    var form_data = GetModifyForm(diag);
    range_list[old_key] = form_data;
    var title = Get_MACRange_Title(form_data);
    alink.find("span").eq(0).html(title);
    return true;
}

function Update_VLANRange_UpdateFrm_Data(diag, alink, range_list){
    var old_key   = alink.attr("data-value");
    if(diag.find("#range_right_content").html() == ""){//首次加载,没有数据
        return true;
    }
    if(!Check_VLANRange_UpdateFrm_Data_Valid(diag, range_list)){
        return false;
    }
    var form_data = GetModifyForm(diag);
    range_list[old_key] = form_data;
    var title = Get_VLANRange_Title(form_data);
    alink.find("span").eq(0).html(title);
    return true;
}

function Show_IPRange_UpdateFrm_Data(diag, alink, range_list){
    var key = alink.attr("data-value");
    $('ul.range-menu li.active').removeClass('active');
    alink.parent('li').addClass('active');
    diag.find('#range_right_content').html(Get_IPRange_UpdateFrm_Html());
    
    //数据绑定
    $("input[bind=IPv4Start]").val(range_list[key].IPv4Start);
    $("input[bind=IPv4End]").val(range_list[key].IPv4End);
    $("input[bind=IPv4Mask]").val(range_list[key].IPv4Mask);

    TogglePopover(diag);

    if(range_list[key].IPv4Start != null){
        $('#range_right_content').find("form").hide();
        $('#range_right_content').find("table").show();

        $("span[bind=IPv4Start]").text(range_list[key].IPv4Start);
        $("span[bind=IPv4End]").text(range_list[key].IPv4End);
        $("span[bind=IPv4Mask]").text(range_list[key].IPv4Mask);        
    }
}

function Show_MACRange_UpdateFrm_Data(diag, alink, range_list){
    var key = alink.attr("data-value");
    $('ul.range-menu li.active').removeClass('active');
    alink.parent('li').addClass('active');
    diag.find('#range_right_content').html(Get_MACRange_UpdateFrm_Html());
    
    //数据绑定
    $("input[bind=MACStart]").val(range_list[key].MACStart);
    $("input[bind=MACEnd]").val(range_list[key].MACEnd);

    TogglePopover(diag);

    if(range_list[key].MACStart != null){
        $('#range_right_content').find("form").hide();
        $('#range_right_content').find("table").show();

        $("span[bind=MACStart]").text(range_list[key].MACStart);
        $("span[bind=MACEnd]").text(range_list[key].MACEnd);       
    }    
}

function Show_VLANRange_UpdateFrm_Data(diag, alink, range_list){
    var key = alink.attr("data-value");
    $('ul.range-menu li.active').removeClass('active');
    alink.parent('li').addClass('active');
    diag.find('#range_right_content').html(Get_VLANRange_UpdateFrm_Html());
    
    //数据绑定
    $("input[bind=vlanStart]").val(range_list[key].vlanStart);
    $("input[bind=vlanEnd]").val(range_list[key].vlanEnd);

    TogglePopover(diag);
     
    z_numInput("txVlanStart",2,4094);
    z_numInput("txVlanEnd",2,4094);

    if(range_list[key].vlanStart != null){
        $('#range_right_content').find("form").hide();
        $('#range_right_content').find("table").show();

        $("span[bind=vlanStart]").text(range_list[key].vlanStart);
        $("span[bind=vlanEnd]").text(range_list[key].vlanEnd);
    }    
}

function Delete_Range_UpdateFrm_Data(alink, range_list){
    var key   = alink.attr("data-value");
    range_list.splice(key, 1); 
    alink.parent().remove();
    //data-value重新编号
    for(var i = 0;i < $("ul.range-menu li").length;i++){
        var item = $("ul.range-menu li").eq(i);
        if(item.hasClass("add_range")){
            continue;
        }
        item.find("a").eq(0).attr("data-value",i.toString());
    }
    if($('ul.range-menu li.active').length == 0){
        $('#range_right_content').html("");
        if($('ul.range-menu li:eq(0)').hasClass("add_range")){
            return;
        }
        $('ul.range-menu li:eq(0)').addClass("active");
        $('ul.range-menu li a:eq(0)').click();
    }
}

function Add_Range_Item(diag, add_item, range_list, title,del_range_updatefrm_data_callback, show_range_updatefrm_data_callback, save_range_updatefrm_data_callback, init_range_updatefrm_data_callback){
    var new_key  = range_list.length - 1;
    var new_info = '';
    new_info += '<li>';
    new_info +=     '<a href="#" data-value="' + new_key + '">';
    new_info +=         '<span class="hidden-tablet">' + title + '</span>';
    new_info +=         '<span class="pull-right icon-trash" style="cursor:pointer;"></span>';
    new_info +=     '</a>';
    new_info += '</li>';
    add_item.parent().before(new_info); 
    
     $('ul.range-menu li a[data-value="' + new_key + '"]').parent().hover(function(){
        $(this).animate({'margin-left':'+=5'},300);
    },
    function(){
        $(this).animate({'margin-left':'-=5'},300);
    });
    $('ul.range-menu li a[data-value="' + new_key + '"]').click(function( event ) {
        var $item = $( this ),
            $target = $( event.target );
        if ( $target.is( "span.icon-trash" ) ) {
            del_range_updatefrm_data_callback($item, range_list);
        }
        else{
            //保存当前页
            if(!save_range_updatefrm_data_callback(diag, range_list)){
                return;
            }
            show_range_updatefrm_data_callback(diag, $item, range_list);
        }
    }); 
    init_range_updatefrm_data_callback();
    $('ul.range-menu li.active').removeClass('active');
    $('ul.range-menu li a[data-value="' + new_key + '"]').parent().addClass('active');
    $('ul.range-menu li a[data-value="' + new_key + '"]').click();

    $('#range_right_content').find("form").show();
    $('#range_right_content').find("table").hide();

    return;
}

function Init_IPRange_UpdateFrm_Data(){
    //数据清空
    $("input[bind=IPv4Start]").val("");
    $("input[bind=IPv4End]").val("");
    $("input[bind=IPv4Mask]").val("");
}

function Init_MACRange_UpdateFrm_Data(){
    //数据清空
    $("input[bind=MACStart]").val("00:d0:d0");
    $("input[bind=MACEnd]").val("00:d0:d0");
}

function Init_VLANRange_UpdateFrm_Data(){
    //数据清空
    $("input[bind=vlanStart]").val("");
    $("input[bind=vlanEnd]").val("");
}

function Add_IPRange_UpdateFrm_Data(diag, add_item, range_list){
    //增加数据
    var iprange_obj = new Object();
    iprange_obj.IPv4Start = "";
    iprange_obj.IPv4End   = "";
    iprange_obj.IPv4Mask = "";
  
    range_list.push(iprange_obj);
    Add_Range_Item(diag, add_item, range_list, "New IP Range", Delete_Range_UpdateFrm_Data, Show_IPRange_UpdateFrm_Data, Save_IPRange_UpdateFrm_Data, Init_IPRange_UpdateFrm_Data);
}

function Add_MACRange_UpdateFrm_Data(diag, add_item, range_list){
    //增加数据
    var macrange_obj = new Object();
    macrange_obj.MACStart = "00:d0:d0";
    macrange_obj.MACEnd   = "00:d0:d0";
    range_list.push(macrange_obj);
    Add_Range_Item(diag, add_item, range_list, "New MAC Range", Delete_Range_UpdateFrm_Data, Show_MACRange_UpdateFrm_Data, Save_MACRange_UpdateFrm_Data, Init_MACRange_UpdateFrm_Data);
}

function Add_VLANRange_UpdateFrm_Data(diag, add_item, range_list){
    //增加数据
    var vlanrange_obj = new Object();
    vlanrange_obj.MACStart = "";
    vlanrange_obj.MACEnd   = "";
    range_list.push(vlanrange_obj);
    Add_Range_Item(diag, add_item, range_list, "New VLAN Range", Delete_Range_UpdateFrm_Data, Show_VLANRange_UpdateFrm_Data, Save_VLANRange_UpdateFrm_Data, Init_VLANRange_UpdateFrm_Data);
} 

function Submit_IPRange_Data(uuid, ackdata, range_list){
    var flag = false;
    var add_iprange_list = [];
    var del_iprange_list = [];
    for(var i = 0;i < range_list.length;i++){
        flag = false; 
        for(var j = 0;j < ackdata.aaData.length;j++){
            var iprange_obj = new Object();
            iprange_obj.IPv4Start = ackdata.aaData[j][0];
            iprange_obj.IPv4End   = ackdata.aaData[j][1];
            iprange_obj.IPv4Mask  = ackdata.aaData[j][2]; 
            if((iprange_obj.IPv4Start == range_list[i].IPv4Start) &&
               (iprange_obj.IPv4End == range_list[i].IPv4End)){
                flag = true;
            }
        }
        //增加 
        if(!flag){
            add_iprange_list.push(range_list[i]);
        }
    }
    for(var i = 0;i < ackdata.aaData.length;i++){
        var iprange_obj = new Object();
        iprange_obj.IPv4Start = ackdata.aaData[i][0];
        iprange_obj.IPv4End   = ackdata.aaData[i][1];
        iprange_obj.IPv4Mask  = ackdata.aaData[i][2];
        flag = false;
        for(var j = 0;j < range_list.length;j++){
            if((iprange_obj.IPv4Start == range_list[j].IPv4Start) &&
               (iprange_obj.IPv4End == range_list[j].IPv4End)){
                flag = true;
            }
        }
        //删除
        if(!flag){
            del_iprange_list.push(iprange_obj);
        }
    }
    for(var i = 0;i < add_iprange_list.length;i++){
        var add_iprange_obj = add_iprange_list[i];

        rs = xmlrpc_service.tecs.vnet.netplaneip.cfg(xmlrpc_session, uuid, add_iprange_obj.IPv4Start, add_iprange_obj.IPv4End, 
                add_iprange_obj.IPv4Mask, add_iprange_obj.IPv4Mask, "", "", 0, 0, EN_ADD_IPV4_RANGE);
        if(rs[0] != 0){
            ShowNotyRpcErrorInfo(rs);
            break;
        }
    }
     
    for(var j = 0;j < del_iprange_list.length;j++){
        var del_iprange_obj = del_iprange_list[j]; 

        rs = xmlrpc_service.tecs.vnet.netplaneip.cfg(xmlrpc_session, uuid, del_iprange_obj.IPv4Start, del_iprange_obj.IPv4End, 
                del_iprange_obj.IPv4Mask, del_iprange_obj.IPv4Mask, "", "", 0, 0, EN_DEL_IPV4_RANGE);
        if(rs[0] != 0){
            ShowNotyRpcErrorInfo(rs);
            break;
        }
    }
}

function Submit_MACRange_Data(uuid, ackdata, range_list){
    var flag = false;
    var add_macrange_list = [];
    var del_macrange_list = [];
    for(var i = 0;i < range_list.length;i++){
        flag = false; 
        for(var j = 0;j < ackdata.aaData.length;j++){
            var macrange_obj = new Object();
            macrange_obj.MACStart = ackdata.aaData[j][0];
            macrange_obj.MACEnd   = ackdata.aaData[j][1]; 
            if((macrange_obj.MACStart == range_list[i].MACStart) &&
               (macrange_obj.MACEnd == range_list[i].MACEnd)){
                flag = true;
            }
        }
        //增加 
        if(!flag){
            add_macrange_list.push(range_list[i]);
        }
    }

    for(var i = 0;i < ackdata.aaData.length;i++){
        var macrange_obj = new Object();
        macrange_obj.MACStart = ackdata.aaData[i][0];
        macrange_obj.MACEnd   = ackdata.aaData[i][1];
        flag = false;
        for(var j = 0;j < range_list.length;j++){
            if((macrange_obj.MACStart == range_list[j].MACStart) &&
               (macrange_obj.MACEnd == range_list[j].MACEnd)){
                flag = true;
            }
        }
        //删除
        if(!flag){
            del_macrange_list.push(macrange_obj);
        }
    }

    for(var i = 0;i < add_macrange_list.length;i++){
        var add_macrange_obj = add_macrange_list[i];

        rs = xmlrpc_service.tecs.vnet.netplanemac.cfg(xmlrpc_session, uuid, "", "", "", "",  
                add_macrange_obj.MACStart, add_macrange_obj.MACEnd, 0, 0, EN_ADD_MAC_RANGE);
        if(rs[0] != 0){
            ShowNotyRpcErrorInfo(rs);
            break;
        }
    }
     
    for(var j = 0;j < del_macrange_list.length;j++){
        var del_macrange_obj = del_macrange_list[j]; 

        rs = xmlrpc_service.tecs.vnet.netplanemac.cfg(xmlrpc_session, uuid, "", "", "", "", 
                del_macrange_obj.MACStart, del_macrange_obj.MACEnd, 0, 0, EN_DEL_MAC_RANGE);

        if(rs[0] != 0){
            ShowNotyRpcErrorInfo(rs);
            break;
        }
    }
}

function Submit_VLANRange_Data(uuid, ackdata, range_list){
    var flag = false;
    var add_vlanrange_list = [];
    var del_vlanrange_list = [];
    for(var i = 0;i < range_list.length;i++){
        flag = false; 
        for(var j = 0;j < ackdata.aaData.length;j++){
            var vlanrange_obj = new Object();
            vlanrange_obj.vlanStart = ackdata.aaData[j][0];
            vlanrange_obj.vlanEnd   = ackdata.aaData[j][1]; 
            if((vlanrange_obj.vlanStart == range_list[i].vlanStart) &&
               (vlanrange_obj.vlanEnd == range_list[i].vlanEnd)){
                flag = true;
            }
        }
        //增加 
        if(!flag){
            add_vlanrange_list.push(range_list[i]);
        }
    }
    for(var i = 0;i < ackdata.aaData.length;i++){
        var vlanrange_obj = new Object();
        vlanrange_obj.vlanStart = ackdata.aaData[i][0];
        vlanrange_obj.vlanEnd   = ackdata.aaData[i][1];
        flag = false;
        for(var j = 0;j < range_list.length;j++){
            if((vlanrange_obj.vlanStart == range_list[j].vlanStart) &&
               (vlanrange_obj.vlanEnd == range_list[j].vlanEnd)){
                flag = true;
            }
        }
        //删除
        if(!flag){
            del_vlanrange_list.push(vlanrange_obj);
        }
    }
    for(var i = 0;i < add_vlanrange_list.length;i++){
        var add_vlanrange_obj = add_vlanrange_list[i];

        rs = xmlrpc_service.tecs.vnet.netplanevlan.cfg(xmlrpc_session, uuid, "", "", "", "", "", "", 
                parseInt(add_vlanrange_obj.vlanStart), parseInt(add_vlanrange_obj.vlanEnd), EN_ADD_VLAN_RANGE);
        
        if(rs[0] != 0){
            ShowNotyRpcErrorInfo(rs);
            break;
        }
    }
     
    for(var j = 0;j < del_vlanrange_list.length;j++){
        var del_vlanrange_obj = del_vlanrange_list[j]; 

        rs = xmlrpc_service.tecs.vnet.netplanevlan.cfg(xmlrpc_session, uuid, "", "", "", "", "", "",  
                parseInt(del_vlanrange_obj.vlanStart), parseInt(del_vlanrange_obj.vlanEnd), EN_DEL_VLAN_RANGE);
        if(rs[0] != 0){
            ShowNotyRpcErrorInfo(rs);
            break;
        }
    }
}

function Get_NetplaneRange_Para_List(aoData){
    var range_para_list = new Array();
    
    var range_para_obj = new Object();
    range_para_obj.index = "iprange";
    range_para_obj.header = page.res[page.pagelan].type.display["IPrange"];
    range_para_obj.sSource = "../php/netplane_iprange.php";
    range_para_obj.aoData = aoData;
    range_para_obj.get_range_list_callback            = Get_IPRange_List;
    range_para_obj.get_range_title_callback           = Get_IPRange_Title;
    range_para_obj.show_range_updatefrm_data_callback = Show_IPRange_UpdateFrm_Data; 
    range_para_obj.add_range_updatefrm_data_callback  = Add_IPRange_UpdateFrm_Data;
    range_para_obj.del_range_updatefrm_data_callback  = Delete_Range_UpdateFrm_Data;
    range_para_obj.save_range_updatefrm_data_callback = Save_IPRange_UpdateFrm_Data;
    range_para_obj.submit_data_callback               = Submit_IPRange_Data;
    range_para_list.push(range_para_obj);
    
    range_para_obj = new Object();
    range_para_obj.index = "macrange";
    range_para_obj.header = page.res[page.pagelan].type.display["MACrange"];
    range_para_obj.sSource = "../php/netplane_macrange.php";
    range_para_obj.aoData = aoData;
    range_para_obj.get_range_list_callback            = Get_MACRange_List;
    range_para_obj.get_range_title_callback           = Get_MACRange_Title;
    range_para_obj.show_range_updatefrm_data_callback = Show_MACRange_UpdateFrm_Data; 
    range_para_obj.add_range_updatefrm_data_callback  = Add_MACRange_UpdateFrm_Data;
    range_para_obj.del_range_updatefrm_data_callback  = Delete_Range_UpdateFrm_Data;
    range_para_obj.save_range_updatefrm_data_callback = Save_MACRange_UpdateFrm_Data;
    range_para_obj.submit_data_callback               = Submit_MACRange_Data;
    range_para_list.push(range_para_obj); 
     
    range_para_obj = new Object();
    range_para_obj.index = "vlanrange";
    range_para_obj.header = page.res[page.pagelan].type.display["VLANrange"];
    range_para_obj.sSource = "../php/netplane_vlanrange.php";
    range_para_obj.aoData = aoData;
    range_para_obj.get_range_list_callback            = Get_VLANRange_List;
    range_para_obj.get_range_title_callback           = Get_VLANRange_Title;
    range_para_obj.show_range_updatefrm_data_callback = Show_VLANRange_UpdateFrm_Data; 
    range_para_obj.add_range_updatefrm_data_callback  = Add_VLANRange_UpdateFrm_Data;
    range_para_obj.del_range_updatefrm_data_callback  = Delete_Range_UpdateFrm_Data;
    range_para_obj.save_range_updatefrm_data_callback = Save_VLANRange_UpdateFrm_Data;
    range_para_obj.submit_data_callback               = Submit_VLANRange_Data;
    range_para_list.push(range_para_obj); 
    
    return range_para_list;
}

function Show_Range_Update_Diag(index, name, aoData, uuid){
    var range_para_list = Get_NetplaneRange_Para_List(aoData);
    var range_para_obj = null;
    var diag = $('#idx_confirmDlg');
    for(var i = 0; i < range_para_list.length;i++){
        var range_para_obj_tmp = range_para_list[i];
        if(range_para_obj_tmp.index == index){
            range_para_obj = range_para_list[i];
            break;
        }
    }
    if(range_para_obj == null){
        return;
    }
    var header = '<i class="icon-edit"></i> ' + range_para_obj.header + '-' + name;
    diag.find('div.modal-header h5').html(header);
    diag.find('div.modal-body').html('<div id="loading" class="center">Loading...<div class="center"></div></div>');
    $.getJSON( range_para_obj.sSource, range_para_obj.aoData, function (json) {
        var ackdata = json.data;
        if(json.errcode != ""){ 
            ShowNotyQueryDBErrorInfo(json);
            return;
        } 
        var range_list = range_para_obj.get_range_list_callback(ackdata);
        var info = ''; 
        info += '<div class="container-fluid">';
        info +=     '<div class="row-fluid">';
        info +=         '<div class="span4 range-menu-span">';
        info +=             '<div class="well nav-collapse sidebar-nav">';
        info +=                 '<ul class="nav nav-tabs nav-stacked range-menu">';
        for(var i = 0;i < range_list.length;i++){
            var title = range_para_obj.get_range_title_callback(range_list[i]);
            info +=                 '<li>';
            info +=                     '<a href="#" data-value="' + i + '">';
            info +=                         '<span class="hidden-tablet">' + title + '</span>';
            info +=                         '<span class="pull-right icon-trash" style="cursor:pointer;"></span>';
            info +=                     '</a>';
            info +=                 '</li>';
        }
        info +=                 '<li class="add_range">';
        info +=                     '<a href="#">';
        info +=                         '<i class="icon-plus"></i> ';
        info +=                         '<span class="hidden-tablet">' + page.res[page.pagelan].type.opr["add"] +'</span>';
        info +=                     '</a>';
        info +=                 '</li>';
        info +=                 '</ul>';
        info +=             '</div>';
        info +=         '</div>';
        info +=         '<div class="span8">';
        info +=             '<div class="row-fluid">'; 
        info +=                 '<div class="span12" id="range_right_content">';
        info +=                 '</div>';
        info +=             '</div>';
        info +=         '</div>';
        info +=     '</div>';
        info += '</div>';
        diag.find('div.modal-body').html(info); 
        //animating menus on hover
        $('ul.range-menu li:not(.nav-header)').hover(function(){
            $(this).animate({'margin-left':'+=5'},300);
        },
        function(){
            $(this).animate({'margin-left':'-=5'},300);
        });
        $('ul.range-menu li a').click(function( event ) {
            var $item = $( this ),
                $target = $( event.target );
            if($item.parent().hasClass("add_range")){
                //保存当前页
                if(!range_para_obj.save_range_updatefrm_data_callback(diag, range_list)){
                    return;
                }
                range_para_obj.add_range_updatefrm_data_callback(diag, $item, range_list);
            }
            else{
                if ( $target.is( "span.icon-trash" ) ) {
                    range_para_obj.del_range_updatefrm_data_callback($item, range_list);
                }
                else{
                    //保存当前页
                    if(!range_para_obj.save_range_updatefrm_data_callback(diag, range_list)){
                        return;
                    }
                    range_para_obj.show_range_updatefrm_data_callback(diag, $item, range_list);
                }
            }
        });
        $('ul.range-menu li:eq(0)').addClass("active");
        $('ul.range-menu li a:eq(0)').click();
        diag.modal("show");
        diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
            //保存当前页
            if(!range_para_obj.save_range_updatefrm_data_callback(diag, range_list)){
                return;
            }
            range_para_obj.submit_data_callback(uuid, ackdata, range_list);
            OnRefresh();
            diag.modal("hide");
        });
    });
} 

function ShowIPrangeDetail(uuid, name){
    var aoData  = { uuid: uuid};
    Show_Range_Update_Diag("iprange", name, aoData, uuid);
    return;
}

function ShowMACrangeDetail(uuid, name){
    var aoData  = { uuid: uuid};
    Show_Range_Update_Diag("macrange", name, aoData, uuid);
    return;
}

function ShowVLANrangeDetail(uuid, name){
    var aoData  = { uuid: uuid};
    Show_Range_Update_Diag("vlanrange", name, aoData, uuid);
    return;
} 

function ShowMACUsageDetail(uuid, name){
    $.getJSON("../php/net_res_mac.php", function(json){
        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["11", json[1]]);
            return;
        } 

        var diag = $("#idx_detailDlg");

        var info = '';
        info += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
        info += '   <tbody>';
        info += '       <tr>';
        info += '           <td>' + 'MAC' + '</td>';
        info += '           <td>' + page.res[page.pagelan].xid.html["vid"] + '</td>';
        info += '       </tr>';

        for (var i = 0; i < json[1].length; i++) {
            if(json[1][i].netplane_uuid == uuid){            
                info += '       <tr>';
                info += '           <td>' + json[1][i].mac + '</td>';
                info += '           <td>' + json[1][i].vid + '</td>';
                info += '       </tr>';
            }
        };

        info += '   </tbody>';
        info += '</table>';

        var title = '<i class="icon-zoom-in"></i> <span>' + page.res[page.pagelan].xid.html["macUseDetail"] + '</span>' + ' - ' + name; 
        diag.find('div.modal-header h5').html(title); 
        diag.find('div.modal-body').html(info); 

        diag.modal('show');       
    });
}

function ShowVLANUsageDetail(uuid, name){
    $.getJSON("../php/net_res_vlan.php", function(json){
        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["11", json[1]]);
            return;
        } 

        var diag = $("#idx_detailDlg");

        var info = '';
        info += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
        info += '   <tbody>';
        info += '       <tr>';
        info += '           <td>' + 'Vlan' + '</td>';
        info += '           <td>' + page.res[page.pagelan].xid.html["vid"] + '</td>';
        info += '       </tr>';

        for (var i = 0; i < json[1].length; i++) {
            if(json[1][i].netplane_uuid == uuid){
                info += '       <tr>';
                info += '           <td>' + json[1][i].vlan_num + '</td>';
                info += '           <td>' + json[1][i].vm_id + '</td>';
                info += '       </tr>';
            }
        };

        info += '   </tbody>';
        info += '</table>';

        var title = '<i class="icon-zoom-in"></i> <span>' + page.res[page.pagelan].xid.html["vlanUseDetail"] + '</span>' + ' - ' + name; 
        diag.find('div.modal-header h5').html(title); 
        diag.find('div.modal-body').html(info); 

        diag.modal('show');       
    });
}

function ShowSegmentUsageDetail(uuid,name){
    $.getJSON("../php/net_res_segment.php", function(json){
        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["11", json[1]]);
            return;
        } 

        var diag = $("#idx_detailDlg");

        var info = '';
        info += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
        info += '   <tbody>';
        info += '       <tr>';
        info += '           <td>' + 'Segment' + '</td>';
        info += '           <td>' + page.res[page.pagelan].xid.html["vid"] + '</td>';
        info += '       </tr>';

        for (var i = 0; i < json[1].length; i++) {
            if(json[1][i].netplane_uuid == uuid){
                info += '       <tr>';
                info += '           <td>' + json[1][i].segment_num + '</td>';
                info += '           <td>' + json[1][i].vm_id + '</td>';
                info += '       </tr>';
            }
        };

        info += '   </tbody>';
        info += '</table>';

        var title = '<i class="icon-zoom-in"></i> <span>' + page.res[page.pagelan].xid.html["segmentUseDetail"] + '</span>' + ' - ' + name; 
        diag.find('div.modal-header h5').html(title); 
        diag.find('div.modal-body').html(info); 

        diag.modal('show');       
    });
}

function RetrieveNetPlaneData( sSource, aoData, fnCallback ){
   // $("#tbNetplane input[type=checkbox]").attr("checked", false);

    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        var uuids = [];
        var names = [];
        if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
        } 
        for(var i = 0;i < ackdata.aaData.length;i++){
            uuids[i] = ackdata.aaData[i][0];
            names[i] = ackdata.aaData[i][1];
        }
        //绑定checkbox/IP/MAC/VLAN Range链接
        for(var i = 0;i < ackdata.aaData.length;i++){
            //ackdata.aaData[i][0] = '<input type="checkbox" value=' + '"' + uuids[i] + '" >';
            ackdata.aaData[i][0] = GetItemCheckbox(uuids[i], currNetplaneState);
            var detaillink = '<a href="javascript:void(0);" onclick="ShowNetPlaneDetail(' + "'" + uuids[i] + "'" + ')">' + names[i] + '</a>';
            ackdata.aaData[i][1] = detaillink;
            
            //var _link = '<a href="javascript:void(0);" onclick="ShowIPrangeDetail(' + "'" + uuids[i] + "'," + "'" + names[i] + "'" + ')">' + ackdata.aaData[i][4] + '</a>';
            //ackdata.aaData[i][4] = _link;
            
            var _link = '<a href="javascript:void(0);" onclick="ShowMACUsageDetail(' + "'" + uuids[i] + "', '" + names[i] + "'" + ')">' + ackdata.aaData[i][4] + '</a>';
            ackdata.aaData[i][4] = _link;
            
            var _link = '<a href="javascript:void(0);" onclick="ShowVLANUsageDetail(' + "'" + uuids[i] + "', '" + names[i] + "'" + ')">' + ackdata.aaData[i][5] + '</a>';
            ackdata.aaData[i][5] = _link;
            
            var _link = '<a href="javascript:void(0);" onclick="ShowSegmentUsageDetail(' + "'" + uuids[i] + "', '" + names[i] + "'" + ')">' + ackdata.aaData[i][6] + '</a>';
            ackdata.aaData[i][6] = _link;
            
            var oplink = '<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["modify"] +'" onclick="OnModify('+ "'" + uuids[i] + "'," + "'" + names[i] + "'" + ')"><i class="icon-edit"></i><span></span></a>';
            oplink += ' | <a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.display["netplaneres"] +'" onclick="OnModifyRes('+ "'" + uuids[i] + "'," + "'" + names[i] + "'" + ')"><i class="icon-cog"></i><span></span></a>'; 
            ackdata.aaData[i][7] = oplink;
        }
        fnCallback(ackdata);
        AvoidGotoBlankPage(oNetplaneDbt);
        ShowLastUpdateTime($("#tbNetplane").parent());

        $(".tooltip").hide();
        $('[rel="tooltip"],[data-rel="tooltip"]').tooltip({"placement":"bottom"});
    });
}

function Create_Netplane(frm){
    var rs = null;
    var item = GetModifyForm(frm);
    frm.find(".name").removeClass("error");
    if(item.name == ""){
        frm.find(".name").addClass("error");
        return false;
    }
    try{
        rs = xmlrpc_service.tecs.vnet.netplane.cfg(xmlrpc_session, item.name, "", item.description, parseInt(item.mtu), EN_ADD_NETPLANE);
        if(rs[0] != 0){
            ShowNotyRpcErrorInfo(rs);
            return false;
        }
    }catch(e){
        alert(e);
        return false;
    } 
    return true;
}

function GetNetplaneBaseHtml(){
    var info = '';
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    
    info += '       <div class="control-group name">';
    info += '           <label class="control-label">' + page.res[page.pagelan].xid.html["net_t1"] + '</label>';
    info += '           <div class="controls">';
    info += '               <input id="txName" popover="name_format" type="text" maxlength="32" class="OamUi" bindattr="val" bind="name">';
    info += '            </div>';
    info += '       </div>';
    
    info += '       <div class="control-group">';
    info += '           <label class="control-label">MTU</label>';
    info += '           <div class="controls">';
    info += '               <input id="txMTU" popover="mtu_format" type="text" class="OamUi" bindattr="val" bind="mtu" value="1500">';
    info += '               <span class="help-inline"></span>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].xid.html["net_t2"] + '</label>';
    info += '           <div class="controls">';
    info += '               <textarea id="taDesc" class="autogrow OamUi" id="taDesc" maxlength="128" rows="6" bindattr="val" bind="description"></textarea><p id="pSetDescInfo"></p>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '   </fieldset>';
    info += '</form>'; 
    return info;
}

function OnCreate(){
    var diag = $('#idx_confirmDlg');
    var header = '<i class="icon-plus"></i>&nbsp;' + page.res[page.pagelan].xid.dialog["create_netplane_dlg"];
    diag.find('div.modal-header h5').html(header);
    diag.find('div.modal-body').html(GetNetplaneBaseHtml()); 
    z_strInput("txName",255);
    z_numInput("txMTU",1280,9000);
    LimitCharLenInput($("#taDesc"),$("#pSetDescInfo"));
    TogglePopover(diag);
    diag.modal("show"); 
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        if(!Create_Netplane(diag.find('div.modal-body form'))){
            return;
        }
        OnRefresh();
        diag.modal("hide");
    });
}

function Modify_Netplane(uuid, frm){
    var rs = null;
    var item = GetModifyForm(frm);
    frm.find(".name").removeClass("error");
    if(item.name == ""){
        frm.find(".name").addClass("error");
        return false;
    }
    try{
        rs = xmlrpc_service.tecs.vnet.netplane.cfg(xmlrpc_session, item.name, uuid, item.description, 
                                                   parseInt(item.mtu), EN_MOD_NETPLANE);
        if(rs[0] != 0){
            ShowNotyRpcErrorInfo(rs);
            return false;
        }
    }catch(e){
        alert(e);
        return false;
    } 
    return true;
}

function OnModify(uuid, name){
    var diag = $('#idx_confirmDlg');
    var header = '<i class="icon-edit"></i>&nbsp;' + page.res[page.pagelan].xid.dialog["netplane_desc_dlg"] + '-' + name;
    diag.find('div.modal-header h5').html(header); 
    diag.find('div.modal-body').html('<div id="loading" class="center">Loading...<div class="center"></div></div>');
    var sSource = "../php/netplane_base.php"; 
    var aoData  = { uuid: uuid};
    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        if(json.errcode != ""){ 
            ShowNotyQueryDBErrorInfo(json);
            return;
        }
        if(ackdata.aaData.length != 1){
            retrun;
        }
        diag.find('div.modal-body').html(GetNetplaneBaseHtml());
        $("#txName").parent().parent().hide();
        z_strInput("txName",255);
        z_numInput("txMTU", 1280, 9000);
        LimitCharLenInput($("#taDesc"),$("#pSetDescInfo"));
        TogglePopover(diag);
        $("#txName").val(ackdata.aaData[0][0]);
        $("#txMTU").val(ackdata.aaData[0][1]);
        $("#taDesc").val(ackdata.aaData[0][2]);
        diag.modal("show");
        diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
            if(!Modify_Netplane(uuid, diag.find('div.modal-body form'))){
                return;
            }
            OnRefresh();
            diag.modal("hide");
        });
    });
}

function OnModifyRes(uuid, name){
    clicked_netplane_uuid = uuid;

    var modify_netplaneres_dlg_html = 
    '<ul class="nav nav-tabs" id="tabsNetplaneRes"> \
        <li class="active"><a href="#tabNetplaneMac"><OamStaticUI bindattr="text" maptype="display" bindvalue="MACrange"/></a></li>     \
        <li><a href="#tabNetplaneVlan"><OamStaticUI bindattr="text" maptype="display" bindvalue="VLANrange"/></a></li>   \
        <li><a href="#tabSegmentIDRange"><OamStaticUI bindattr="text" maptype="display" bindvalue="SegmentIDRange"/></a></li>\
        <li><a href="#tabNetplaneIP"><OamStaticUI bindattr="text" maptype="display" bindvalue="IPrange"/></a></li>\
    </ul>  \
    <div class="tab-content"> \
        <div class="tab-pane active" id="tabNetplaneMac"> \
            <div class="span8" style="border-right:1px solid #dddddd;padding-right:10px;"> \
                <div class="row-fluid"> \
                    <div class="form-horizontal span8"> \
                        <div class="control-group"> \
                            <label class="control-label" style="width:100px;"><span id="netplane_mac_begin"></span></label> \
                            <div class="controls" style="margin-left:110px;"> \
                                <input type="text" popover="mac_format" id="netplaneMacBegin" /> \
                            </div> \
                        </div> \
                        <div class="control-group"> \
                            <label class="control-label" style="width:100px;"><span id="netplane_mac_end"></span></label> \
                            <div class="controls" style="margin-left:110px;"> \
                                <input type="text" popover="mac_format" id="netplaneMacEnd" /> \
                            </div> \
                        </div> \
                    </div> \
                </div> \
                <div class="row-fluid"> \
                    <div align="right"> \
                        <button class="btn"><i class="icon-plus"></i><span id="netplaneMacAdd"></span></button>  \
                        <button class="btn"><i class="icon-trash"></i><span id="netplaneMacDel"></span></button>  \
                        <button class="btn"><i class="icon-refresh"></i><span id="netplaneMacRefresh"></span></button> \
                    </div> \
                    <table id="netplane_mac_list" class="table table-striped table-bordered table-condensed">  \
                        <thead>  \
                            <tr>  \
                                <th style="width:45%;" align="left"><span id="netplane_mac_t1"></span></th>  \
                                <th style="width:45%;" align="left"><span id="netplane_mac_t2"></span></th>  \
                                <th style="width:10%;"></th>  \
                            </tr>  \
                        </thead>  \
                        <tbody style="word-wrap:break-word;word-break:break-all;">  \
                        </tbody>  \
                    </table> \
                </div> \
            </div> \
            <div class="span4" id="netplanemac_tips"> \
            </div> \
        </div> \
        <div class="tab-pane" id="tabNetplaneVlan"> \
            <div class="span8" style="border-right:1px solid #dddddd;padding-right:10px;"> \
                <h4 style="color:#555555;">Private Vlan:</h4> <br /> \
                <div class="row-fluid"> \
                    <div class="form-horizontal span8"> \
                        <div class="control-group"> \
                            <label class="control-label" style="width:100px;"><span id="netplane_vlan_begin"></span></label> \
                            <div class="controls" style="margin-left:110px;"> \
                                <input type="text" popover="2~4094" maxlength="4" id="netplaneVlanBegin" /> \
                            </div> \
                        </div> \
                        <div class="control-group"> \
                            <label class="control-label" style="width:100px;"><span id="netplane_vlan_end"></span></label> \
                            <div class="controls" style="margin-left:110px;"> \
                                <input type="text" popover="2~4094" maxlength="4" id="netplaneVlanEnd" /> \
                            </div> \
                        </div> \
                    </div> \
                </div> \
                <div class="row-fluid"> \
                    <div align="right"> \
                        <button class="btn"><i class="icon-plus"></i><span id="netplaneVlanAdd"></span></button>  \
                        <button class="btn"><i class="icon-trash"></i><span id="netplaneVlanDel"></span></button>  \
                        <button class="btn"><i class="icon-refresh"></i><span id="netplaneVlanRefresh"></span></button> \
                    </div> \
                    <table id="netplane_vlan_list" class="table table-striped table-bordered table-condensed">  \
                        <thead>  \
                            <tr>  \
                                <th style="width:45%;" align="left"><span id="netplane_vlan_t1"></span></th>  \
                                <th style="width:45%;" align="left"><span id="netplane_vlan_t2"></span></th>  \
                                <th style="width:10%;"></th>  \
                            </tr>  \
                        </thead>  \
                        <tbody style="word-wrap:break-word;word-break:break-all;">  \
                        </tbody>  \
                    </table> \
                </div> \
                <h4 style="color:#555555;">Share Vlan:</h4> <br /> \
                <div id="share_vlan_info"></div> \
            </div> \
            <div class="span4" id="netplanevlan_tips"> \
            </div> \
        </div> \
        <div class="tab-pane" id="tabSegmentIDRange"> \
            <div class="span8" style="border-right:1px solid #dddddd;padding-right:10px;"> \
                <h4 style="color:#555555;">Private SegmentID:</h4> <br /> \
                <div class="row-fluid"> \
                    <div class="form-horizontal span8"> \
                        <div class="control-group"> \
                            <label class="control-label" style="width:100px;"><span id="netplane_SegmentID_begin"></span></label> \
                            <div class="controls" style="margin-left:110px;"> \
                                <input type="text" popover="1~16000000" maxlength="8" id="netplaneSegmentIDbegin" /> \
                            </div> \
                        </div> \
                        <div class="control-group"> \
                            <label class="control-label" style="width:100px;"><span id="netplane_SegmentID_end"></span></label> \
                            <div class="controls" style="margin-left:110px;"> \
                                <input type="text" popover="1~16000000" maxlength="8" id="netplaneSegmentIDend" /> \
                            </div> \
                        </div> \
                    </div> \
                </div> \
                <div class="row-fluid"> \
                    <div align="right"> \
                        <button class="btn"><i class="icon-plus"></i><span id="netplaneSegmentIDAdd"></span></button>  \
                        <button class="btn"><i class="icon-trash"></i><span id="netplaneSegmentIDDel"></span></button>  \
                        <button class="btn"><i class="icon-refresh"></i><span id="netplaneSegmentIDRefresh"></span></button> \
                    </div> \
                    <table id="netplane_SegmentID_list" class="table table-striped table-bordered table-condensed">  \
                        <thead>  \
                            <tr>  \
                                <th style="width:45%;" align="left"><span id="netplane_SegmentID_t1"></span></th>  \
                                <th style="width:45%;" align="left"><span id="netplane_SegmentID_t2"></span></th>  \
                                <th style="width:10%;"></th>  \
                            </tr>  \
                        </thead>  \
                        <tbody style="word-wrap:break-word;word-break:break-all;">  \
                        </tbody>  \
                    </table> \
                </div> \
                <h4 style="color:#555555;">Share SegmentID:</h4> <br /> \
                <div id="share_SegmentID_info"></div> \
            </div> \
            <div class="span4" id="netplaneSegmentID_tips"> \
            </div> \
        </div> \
        <div class="tab-pane" id="tabNetplaneIP"> \
            <div class="span8" style="border-right:1px solid #dddddd;padding-right:10px;"> \
                <div class="row-fluid">\
                <form class="form-horizontal span8"> \
                    <div class="control-group">   \
                        <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="netplane.ip_begin"></OamStaticUI></label> \
                        <div class="controls" style="margin-left:110px;"><input type="text" popover="ip_format" id="netplane_ip_begin" maxlength="15" /></div> \
                    </div>    \
                    <div class="control-group">   \
                        <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="netplane.ip_end"></OamStaticUI></label> \
                        <div class="controls" style="margin-left:110px;"><input type="text" popover="ip_format" id="netplane_ip_end" maxlength="15" /></div> \
                    </div>    \
                    <div class="control-group">   \
                        <label class="control-label" style="width:100px;"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="netplane.ip_mask"></OamStaticUI></label> \
                        <div class="controls" style="margin-left:110px;"><input type="text" popover="mask_format" id="netplane_ip_mask" maxlength="15" /></div> \
                    </div>    \
                </form> \
                </div> \
                <div class="row-fluid"> \
                    <div align="right"> \
                        <button class="btn"><i class="icon-plus"></i><span id="netplaneIpAdd"></span></button>  \
                        <button class="btn"><i class="icon-trash"></i><span id="netplaneIpDel"></span></button>  \
                        <button class="btn"><i class="icon-refresh"></i><span id="netplaneIpRefresh"></span></button> \
                    </div> \
                    <table id="netplane_IP_list" class="table table-striped table-bordered table-condensed">  \
                        <thead>  \
                            <tr>  \
                                <th style="width:30%;" align="left"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="netplane.ip_begin"></OamStaticUI></th>  \
                                <th style="width:30%;" align="left"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="netplane.ip_end"></OamStaticUI></th>  \
                                <th style="width:30%;" align="left"><OamStaticUI bindattr="text" maptype="field_name" bindvalue="netplane.ip_mask"></OamStaticUI></th>  \
                                <th style="width:10%;"></th> \
                            </tr>  \
                        </thead>  \
                        <tbody style="word-wrap:break-word;word-break:break-all;">  \
                        </tbody>  \
                    </table> \
                </div> \
            </div> \
            <div class="span4" id="netplaneIP_tips"> \
            </div> \
        </div> \
    </div>';

    ShowDiaglog("modify_netplaneres_dlg", modify_netplaneres_dlg_html, {
        init: function(){
            netplaneMacTable = $("#netplane_mac_list").dataTable({
                "sPaginationType": "bootstrap",
                "bFilter":false,
                "bInfo": false,
                "bPaginate": false,
                "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 2 ] }],
                "oLanguage": tableLanguage[page.pagelan]
            }); 

            netplaneVlanTable = $("#netplane_vlan_list").dataTable({
                "sPaginationType": "bootstrap",
                "bFilter":false,
                "bInfo": false,
                "bPaginate": false,
                "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 2 ] }],
                "oLanguage": tableLanguage[page.pagelan]
            });

            netplaneSegmentIDTable = $("#netplane_SegmentID_list").dataTable({
                "sPaginationType":"bootstrap",
                "bFilter":false,
                "bInfo":false,
                "bPaginate":false,
                "aoColumnDefs":[{"bSortable":false,"aTargets":[2]}],
                "oLanguage":tableLanguage[page.pagelan]
            });
            
            netplaneIpTable = $("#netplane_IP_list").dataTable({
                "sPaginationType": "bootstrap",
                "bFilter":false,
                "bInfo": false,
                "bPaginate": false,
                "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 3] }],
                "oLanguage": tableLanguage[page.pagelan]
            }); 
            //tabs
            $('#tabsNetplaneRes a:first').tab('show');
            $('#tabsNetplaneRes a').click(function (e) {
                e.preventDefault();
                $(this).tab('show');
            });            

            TogglePopover($("#modify_netplaneres_dlg"));  

            $("#netplaneMacAdd").parent().click(AddNetplaneMacClick);
            $("#netplaneMacDel").parent().click(DelNetplaneMacClick);
            $("#netplaneMacRefresh").parent().click(RefreshNetplaneMacClick);
            $("#netplaneVlanAdd").parent().click(AddNetplaneVlanClick);
            $("#netplaneVlanDel").parent().click(DelNetplaneVlanClick);
            $("#netplaneVlanRefresh").parent().click(RefreshNetplaneVlanClick);
            $("#netplaneSegmentIDAdd").parent().click(AddNetplaneSegmentIDClick);
            $("#netplaneSegmentIDDel").parent().click(DelNetplaneSegmentIDClick);
            $("#netplaneSegmentIDRefresh").parent().click(RefreshNetplaneSegmentIDClick);
            $("#netplaneIpAdd").parent().bind("click", AddNetplaneIp);
            $("#netplaneIpDel").parent().bind("click", DelNetplaneIp);
            $("#netplaneIpRefresh").parent().click(RefreshNetplaneIp);

            $("#netplanemac_tips").html(page.res[page.pagelan].type.display["netplane_mac_tips"]);
            $("#netplanevlan_tips").html(page.res[page.pagelan].type.display["netplane_vlan_tips"]);
            $("#netplaneSegmentID_tips").html(page.res[page.pagelan].type.display["netplane_segmentID_tips"]);
        },
        show: function(){
            z_numInput("netplaneVlanBegin", 2, 4094);
            z_numInput("netplaneVlanEnd", 2, 4094);
            z_numInput("netplaneSegmentIDbegin", 1, 16000000);
            z_numInput("netplaneSegmentIDend", 1, 16000000);

            var h5 = $("#modify_netplaneres_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-cog"></i>' + h5.text() + ' - ' + name); 

            $("#modify_netplaneres_dlg").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });   

            $("#netplaneMacBegin").val("00:d0:d0:");
            $("#netplaneMacEnd").val("00:d0:d0:");

            RefreshNetplaneMacClick();
            RefreshNetplaneVlanClick();
            RefreshNetplaneSegmentIDClick();
            RefreshNetplaneIp();
        },
        close: function(){

        }
    })
}

function AddNetplaneMacClick(){
    var macbegin = $("#netplaneMacBegin").val();
    var macend= $("#netplaneMacEnd").val();

    $(".control-group").removeClass("error");
    if(macbegin == "" || !checkMAC(macbegin)){
        $("#netplaneMacBegin").parent().parent().addClass("error");
        return;
    }
    if(macend == "" || !checkMAC(macend) || MAC2Decimal(macbegin) > MAC2Decimal(macend)){
        $("#netplaneMacEnd").parent().parent().addClass("error");
        return;
    }

    var rs = xmlrpc_service.tecs.vnet.netplanemac.cfg(xmlrpc_session, clicked_netplane_uuid, 
                 "", "", "", "", macbegin, macend, 0, 0, 0, 0, EN_ADD_MAC_RANGE);    

    if(rs[0] == 0){
        RefreshNetplaneMacClick();
    }
    else{
        ShowNotyRpcErrorInfo(rs);
    }
}

function DelNetplaneMacClick(){
    var macbegin = $("#netplaneMacBegin").val();
    var macend= $("#netplaneMacEnd").val();

    $(".control-group").removeClass("error");
    if(macbegin == "" || !checkMAC(macbegin)){
        $("#netplaneMacBegin").parent().parent().addClass("error");
        return;
    }
    if(macend == "" || !checkMAC(macend) || MAC2Decimal(macbegin) > MAC2Decimal(macend)){
        $("#netplaneMacEnd").parent().parent().addClass("error");
        return;
    }

    var rs = xmlrpc_service.tecs.vnet.netplanemac.cfg(xmlrpc_session, clicked_netplane_uuid, 
                 "", "", "", "", macbegin, macend, 0, 0,0, 0, EN_DEL_MAC_RANGE);    

    if(rs[0] == 0){
        RefreshNetplaneMacClick();
    }
    else{
        ShowNotyRpcErrorInfo(rs);
    }
}

function delete_one_macrange(macrange){
    var macbegin = $(macrange).parent().parent().find("td").eq(0).text();
    var macend = $(macrange).parent().parent().find("td").eq(1).text();

    var rs = xmlrpc_service.tecs.vnet.netplanemac.cfg(xmlrpc_session, clicked_netplane_uuid, 
                 "", "", "", "", macbegin, macend, 0, 0, 0, 0, EN_DEL_MAC_RANGE);    

    if(rs[0] == 0){
        RefreshNetplaneMacClick();
    }
    else{
        ShowNotyRpcErrorInfo(rs);
    }    
}

function RefreshNetplaneMacClick(){
    var sSource = "../php/netplane_macrange.php"; 
    var aoData  = { uuid: clicked_netplane_uuid};
    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        if(json.errcode != ""){ 
            ShowNotyQueryDBErrorInfo(json);
            return;
        }

        netplaneMacTable.fnClearTable();
        netplaneMacTable.fnAddData(ackdata.aaData);
    });
}

function AddNetplaneVlanClick(){
    var vlanbegin = parseInt($("#netplaneVlanBegin").val());
    var vlanend   = parseInt($("#netplaneVlanEnd").val());

    $(".control-group").removeClass("error");
    if(isNaN(vlanbegin) || vlanbegin < 2 || vlanbegin > 4094){
        $("#netplaneVlanBegin").parent().parent().addClass("error");
        return;
    }
    if(isNaN(vlanend) || vlanend < 2 || vlanend > 4094){
        $("#netplaneVlanEnd").parent().parent().addClass("error");
        return;
    }

    var rs = xmlrpc_service.tecs.vnet.netplanevlan.cfg(xmlrpc_session, clicked_netplane_uuid, 
                 "", "", "", "", "", "", vlanbegin, vlanend,0,0, EN_ADD_MAC_RANGE);    

    if(rs[0] == 0){
        RefreshNetplaneVlanClick();
    }
    else{
        ShowNotyRpcErrorInfo(rs);
    }
}

function DelNetplaneVlanClick(){
    var vlanbegin = parseInt($("#netplaneVlanBegin").val());
    var vlanend   = parseInt($("#netplaneVlanEnd").val());

    $(".control-group").removeClass("error");
    if(isNaN(vlanbegin) || vlanbegin < 2 || vlanbegin > 4094){
        $("#netplaneVlanBegin").parent().parent().addClass("error");
        return;
    }
    if(isNaN(vlanend) || vlanend < 2 || vlanend > 4094){
        $("#netplaneVlanEnd").parent().parent().addClass("error");
        return;
    }

    var rs = xmlrpc_service.tecs.vnet.netplanevlan.cfg(xmlrpc_session, clicked_netplane_uuid, 
                "", "", "", "", "", "", vlanbegin, vlanend,0,0, EN_DEL_VLAN_RANGE);    

    if(rs[0] == 0){
        RefreshNetplaneVlanClick();
    }
    else{
        ShowNotyRpcErrorInfo(rs);
    }
}

function delete_one_vlanrange(vlanrange){
    var vlanbegin = $(vlanrange).parent().parent().find("td").eq(0).text();
    var vlanend = $(vlanrange).parent().parent().find("td").eq(1).text();

    var rs = xmlrpc_service.tecs.vnet.netplanevlan.cfg(xmlrpc_session, clicked_netplane_uuid, 
                "", "", "", "", "", "", parseInt(vlanbegin), parseInt(vlanend),0,0, EN_DEL_VLAN_RANGE);    

    if(rs[0] == 0){
        RefreshNetplaneVlanClick();
    }
    else{
        ShowNotyRpcErrorInfo(rs);
    }
}

function RefreshNetplaneVlanClick(){
    var sSource = "../php/netplane_vlanrange.php"; 
    var aoData  = { uuid: clicked_netplane_uuid};
    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        if(json.errcode != ""){ 
            ShowNotyQueryDBErrorInfo(json);
            return;
        }

        $("#share_vlan_info").text(ackdata.sharevlan);

        netplaneVlanTable.fnClearTable();
        netplaneVlanTable.fnAddData(ackdata.aaData);
    });
}

function RemoveNetplaneCallback(vCheckedbox){
    ClearAlertInfo();

    vCheckedbox.each(function(){
        var uuid = $(this).attr("value");
        var tr = $(this).parent().parent();                      
        var netplane = tr.find("td").eq(1).text();
        var desc = tr.find("td").eq(3).text();

        var result = xmlrpc_service.tecs.vnet.netplane.cfg(xmlrpc_session, netplane, uuid, desc, 0, EN_DEL_NETPLANE);
        //ShowSynRpcInfo(result);
        ShowNetSynRpcInfo(result);
    }); 
    
    OnRefresh();
}

function OnRemove(){
    ShowBatchOpConfirmDiag($("#idx_confirmDlg"), 
                           "tbNetplane", 
                           page.res[page.pagelan].type.err_info["delete_confirm_info"], 
                           RemoveNetplaneCallback);

    var h5 = $("#idx_confirmDlg").find('div.modal-header h5');
    h5.html('<i class="icon-trash"></i>' + page.res[page.pagelan].xid.dialog["remove_netplane_dlg"]);    
}

function OnSelectAll(){
    if($(this).attr("checked") == "checked"){
        $("#tbNetplane tbody input[type=checkbox]").attr("checked", true);
    }
    else{
        $("#tbNetplane tbody input[type=checkbox]").attr("checked", false);
    }
}

var currNetplaneState = new Object();
function OnRefresh(){
    if($('#tbNetplane').length == 0) {
        clearInterval(netplane_timer_id);
        return;
    }
   // oNetplaneDbt.fnDraw();
    ClearCurSelItem(currNetplaneState);  
    RecordSelItem(oNetplaneDbt, "tbNetplane", currNetplaneState);   
    GotoCurrentPageOfDataTable(oNetplaneDbt);
}

function AddNetplaneSegmentIDClick(){
    var SegmentIDbegin = parseInt($("#netplaneSegmentIDbegin").val());
    var SegmentIDend   = parseInt($("#netplaneSegmentIDend").val());

    $(".control-group").removeClass("error");
    if(isNaN(SegmentIDbegin) || SegmentIDbegin < 1 || SegmentIDbegin > 16000000){
        $("#netplaneSegmentIDbegin").parent().parent().addClass("error");
        return;
    }
    if(isNaN(SegmentIDend) || SegmentIDend < 1 || SegmentIDend > 16000000){
        $("#netplaneSegmentIDend").parent().parent().addClass("error");
        return;
    }

    var rs = xmlrpc_service.tecs.vnet.netplanesegment.cfg(xmlrpc_session, clicked_netplane_uuid, 
                 "", "", "", "", "", "", 0, 0,  SegmentIDbegin,SegmentIDend,EN_ADD_MAC_RANGE);    

    if(rs[0] == 0){
        RefreshNetplaneSegmentIDClick();
    }
    else{
        ShowNotyRpcErrorInfo(rs);
    }

}

function DelNetplaneSegmentIDClick(){
	var SegmentIDbegin = parseInt($("#netplaneSegmentIDbegin").val());
    var SegmentIDend   = parseInt($("#netplaneSegmentIDend").val());

    $(".control-group").removeClass("error");
    if(isNaN(SegmentIDbegin) || SegmentIDbegin < 1 || SegmentIDbegin > 16000000){
        $("#netplaneSegmentIDbegin").parent().parent().addClass("error");
        return;
    }
    if(isNaN(SegmentIDend) || SegmentIDend < 1 || SegmentIDend > 16000000){
        $("#netplaneSegmentIDend").parent().parent().addClass("error");
        return;
    }

    var rs = xmlrpc_service.tecs.vnet.netplanesegment.cfg(xmlrpc_session, clicked_netplane_uuid, 
                "", "", "", "", "", "", 0, 0,SegmentIDbegin,SegmentIDend, EN_DEL_VLAN_RANGE);    

    if(rs[0] == 0){
        RefreshNetplaneSegmentIDClick();
    }
    else{
        ShowNotyRpcErrorInfo(rs);
    }
}

function delete_one_segmentrange(segmentrange){
    var SegmentIDbegin = $(segmentrange).parent().parent().find("td").eq(0).text();
    var SegmentIDend = $(segmentrange).parent().parent().find("td").eq(1).text();

    var rs = xmlrpc_service.tecs.vnet.netplanesegment.cfg(xmlrpc_session, clicked_netplane_uuid, 
                "", "", "", "", "", "",0,0, parseInt(SegmentIDbegin), parseInt(SegmentIDend), EN_DEL_VLAN_RANGE);    

    if(rs[0] == 0){
       RefreshNetplaneSegmentIDClick();
    }
    else{
        ShowNotyRpcErrorInfo(rs);
    }
}

function RefreshNetplaneSegmentIDClick(){
    var sSource = "../php/netplane_segmentIDrange.php"; 
    var aoData  = { uuid: clicked_netplane_uuid};
    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        if(json.errcode != ""){ 
            ShowNotyQueryDBErrorInfo(json);
            return;
        }

        $("#share_SegmentID_info").text(ackdata.sharesegment);

        netplaneSegmentIDTable.fnClearTable();
        netplaneSegmentIDTable.fnAddData(ackdata.aaData);
    });
}

function AddNetplaneIp(){
    $(".control-group").removeClass("error");

    var ipbegin = $("#netplane_ip_begin").val();
    var ipend = $("#netplane_ip_end").val();
    var mask =  $("#netplane_ip_mask").val();    

    if(ipbegin == "" || !checkIP(ipbegin)){
        $("#netplane_ip_begin").parent().parent().addClass("error");
        return false;
    }    

    if(ipend == "" || !checkIP(ipend)){
        $("#netplane_ip_end").parent().parent().addClass("error");
        return false;
    }    

    if(mask == "" || !checkIP(mask)){
        $("#netplane_ip_mask").parent().parent().addClass("error");
        return false;
    } 
    
    if(IP2Decimal(ipbegin) > IP2Decimal(ipend)){
        $("#netplane_ip_end").parent().parent().addClass("error");
        return false;
    }    

   var rs = xmlrpc_service.tecs.vnet.netplaneip.cfg(xmlrpc_session, clicked_netplane_uuid, 
                 ipbegin, ipend, mask, mask, "", "", 0, 0,  0, 0,EN_ADD_MAC_RANGE);    

    if(rs[0] == 0){
        RefreshNetplaneIp();
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }
}

function DelNetplaneIp(){
    var ipbegin = $("#netplane_ip_begin").val();
    var ipend = $("#netplane_ip_end").val();
    var mask =  $("#netplane_ip_mask").val();       

    if(ipbegin == "" || !checkIP(ipbegin)){
        $("#netplane_ip_begin").parent().parent().addClass("error");
        return false;
    }    

    if(ipend == "" || !checkIP(ipend)){
        $("#netplane_ip_end").parent().parent().addClass("error");
        return false;
    }    

    if(mask == "" || !checkIP(mask)){
        $("#netplane_ip_mask").parent().parent().addClass("error");
        return false;
    } 
    
    if(IP2Decimal(ipbegin) > IP2Decimal(ipend)){
        $("#netplane_ip_end").parent().parent().addClass("error");
        return false;
    }    

   var rs = xmlrpc_service.tecs.vnet.netplaneip.cfg(xmlrpc_session, clicked_netplane_uuid, 
                 ipbegin, ipend, mask, mask, "", "", 0, 0,  0,0,EN_DEL_VLAN_RANGE);  

    if(rs[0] == 0){
        RefreshNetplaneIp();
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }
}

function delete_one_iprange(iprange){
    var ipbegin = $(iprange).parent().parent().find("td").eq(0).text();
    var ipend = $(iprange).parent().parent().find("td").eq(1).text();
    var mask = $(iprange).parent().parent().find("td").eq(2).text();  

    var rs = xmlrpc_service.tecs.vnet.netplaneip.cfg(xmlrpc_session, clicked_netplane_uuid, 
                 ipbegin, ipend, mask, mask, "", "", 0, 0,  0, 0,EN_DEL_VLAN_RANGE);  

    if(rs[0] == 0){
        RefreshNetplaneIp();
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }       
}

function RefreshNetplaneIp(){
    var sSource = "../php/netplane_iprange.php"; 
    var aoData  = { uuid: clicked_netplane_uuid};   

    $.getJSON(sSource, aoData, function(json){
        if(json.errcode != ""){
            ShowNotyRpcErrorInfo(json);
            return;
        }        
        var ackdata = json.data;       
        netplaneIpTable.fnClearTable();
        netplaneIpTable.fnAddData(ackdata.aaData);
    });  
}

$(function(){
    //datatable
    oNetplaneDbt = $('#tbNetplane').dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan], 
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] },
                         { "bSortable": false, "aTargets": [ 3 ] },
                         { "bSortable": false, "aTargets": [ 4 ] },
                         { "bSortable": false, "aTargets": [ 5 ] }, 
                         { "bSortable": false, "aTargets": [ 6 ] },
                         { "bSortable": false, "aTargets": [ 7 ] }],
        "alternatively" : "destory",
        "bProcessing": false,
        "bServerSide": true,
        "sAjaxSource": "../php/net_netplane.php",
        "fnServerData": RetrieveNetPlaneData
    } );

    $.getJSON("../php/netplane_records.php",function(json){
        if(json[0] > 0){
            $('#tabsNetplane a:eq(1)').tab('show');
        }
        else{
    $('#tabsNetplane a:first').tab('show');
        }
    });
    //tabs
    //$('#tabsNetplane a:first').tab('show');
    $('#tabsNetplane a').click(function (e) {
        e.preventDefault();
        $(this).tab('show');
    });

    $("#create_netplane_link").click(function(e){
        $('#tabsNetplane a').eq(1).click();
        $("#create").click();
    });

    $('#create_portgroup_link').click(function(e){
        mainmenu_item_click(e,$(this));
    });    

    // OamUi静态显示部分初始化
    $("OamStaticUI").each(function(){
        SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
    });
    netplane_timer_id    = setInterval(OnRefresh, 30000);
    
    $("#SelecltAll").click(OnSelectAll);
    $("#create").click(OnCreate);
    $("#remove").click(OnRemove);
    $("#refresh").click(OnRefresh);
});