var label_interfaces = "interfaces";
var label_item       = "item";
var label_index      = "index";
var label_function   = "function";
var label_mac        = "mac";
var label_network    = "network";
var label_netid      = "V4PF_NETID";
var label_rack       = "V4PF_RACK";
var label_shelf      = "V4PF_SHELF";
var label_slot       = "V4PF_SLOT";
var label_cpuid      = "V4PF_CPUID";
var label_board_attr = "V4PF_BOARD_ATTR";
var label_omm_type   = "omm_type";
var label_is_use_pdh       = "is_use_pdh";
var label_omm_ftp_serverip  = "server_ip";
var label_omm_ftp_clientip  = "client_ip";
var label_omm_ftp_netmask   = "netmask";
var label_omm_ftp_user      = "username";
var label_omm_ftp_pass      = "password";
var label_omm_ftp_relative_path = "relative_path";
var label_omm_ftp_filename      = "filename";
var label_vnfm_nodeip           = "node_ip";
var label_vnfm_netmask          = "netmask";
var label_vnfm_gateway          = "gateway";
var omm_over_fabric  = "1";
var omm_over_base    = "2";
var v4_context_filename = "config.xml";
var context_obj_list = new Array();
function ShowContextsDiag(diag, droper, vm_data, action, drag_elem_obj){
    var info = '';
    var context = new Object();
    for(var p in vm_data["cfg_info"]["base_info"]["context"]){
        context[p] = vm_data["cfg_info"]["base_info"]["context"][p];
    } 
    
    info += '<div class="container-fluid">';
    info +=     '<div class="row-fluid">';
    info +=         '<div class="span4 context-menu-span">';
    info +=             '<div class="well nav-collapse sidebar-nav">';
    info +=                 '<ul class="nav nav-tabs nav-stacked context-menu">';
    for(var p in context){
        info +=                 GetContextItemStr(p);
    }
    info +=                 '<li class="add_normal_context">';
    info +=                     '<a href="#">';
    info +=                         '<i class="icon-plus"></i> ';
    info +=                         '<span class="hidden-tablet">' + page.res[page.pagelan].type.opr["add_normal_context"] + '</span>';
    info +=                     '</a>';
    info +=                 '</li>';
    info +=                 '</ul>';
    info +=             '</div>';
    info +=         '</div>';
    info +=         '<div class="span8">';
    info +=             '<div class="row-fluid">'; 
    info +=                 '<div class="span12" id="context_right_content">';
    info +=                 '</div>';
    info +=             '</div>';
    info +=         '</div>';
    info +=     '</div>';
    info += '</div>';
    SetVmParaHeader(drag_elem_obj.diag_img, page.res[page.pagelan].type.display["context"], diag);
    diag.find('div.modal-body').html(info);
    
    //animating menus on hover
    $('ul.context-menu li:not(.nav-header)').hover(function(){
        $(this).animate({'margin-left':'+=5'},300);
    },
    function(){
        $(this).animate({'margin-left':'-=5'},300);
    });
    $('ul.context-menu li a').click(function( event ) {
        var $item = $( this ),
            $target = $( event.target );
        if($item.parent().hasClass("add_normal_context")){
            ShowNewContextItem(diag, vm_data, context, $item);
        }
        else{
            if ( $target.is( "span.icon-trash" ) ) {
                DeleteContextData($item, context);
            }
            else{
                InitContextData(diag, $item, context, vm_data["cfg_info"]["base_info"]["nics"]);
            }
        }
    });

    $('ul.context-menu li:eq(0)').addClass("active");
    $('ul.context-menu li a:eq(0)').click();
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        if(!SaveCurrentPage(diag, context)){
            return;
        }
        vm_data["cfg_info"]["base_info"]["context"] = context;
        if(GetContextCount(context) == 0){
            DeleteContextsDataCallBack(diag, droper, vm_data, drag_elem_obj);
        }
        diag.modal("hide");
    });
    diag.modal("show");
}

function ShowNewContextItem(diag, vm_data, context, add_item){
    var new_key      = GetDefaultNormalContextFileName(context);
    context[new_key] = '';
    add_item.parent().before(GetContextItemStr(new_key));
    $('ul.context-menu li a[data-value="' + new_key + '"]').parent().hover(function(){
        $(this).animate({'margin-left':'+=5'},300);
    },
    function(){
        $(this).animate({'margin-left':'-=5'},300);
    });
    $('ul.context-menu li a[data-value="' + new_key + '"]').click(function( event ) {
        var $item = $( this ),
            $target = $( event.target );
        if ( $target.is( "span.icon-trash" ) ) {
            DeleteContextData($item, context);
        }
        else{
            InitContextData(diag, $item, context, vm_data["cfg_info"]["base_info"]["nics"]);
        }
    });
}

function GetContextItemStr(key){
    var new_info = '';
    var new_title = key.length > 10 ? key.substr(0,10) + "..." : key;
    new_info += '<li>';
    new_info +=     '<a href="#" data-value="' + key + '">';
    new_info +=         '<span><img src="img/param_context_16.png"></img></span> ';
    new_info +=         '<span class="hidden-tablet">' + new_title + '</span>';
    new_info +=         '<span class="pull-right icon-trash" style="cursor:pointer;"></span>';
    new_info +=     '</a>';
    new_info += '</li>';
    return new_info;
}

function InitContextData(diag, alink, context, nics){ 
    //保存当前页
    if(!SaveCurrentPage(diag, context)){
        return;
    }
    var key = alink.attr("data-value");
    $('ul.context-menu li.active').removeClass('active');
    alink.parent('li').addClass('active');
    Get_Context_Obj(key).init_context_data_callback(diag, alink.attr("data-value"), context, nics);
}

function GetContextCount(context){
    var count = 0;
    for(var p in context){
        count++;
    } 
    return count;
}

function ShowContextsDetail(idx_pos,vm_data){ 
    var info = '';
    info += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
    info += '   <thead>';
    info += '       <tr>';
    info += '           <th width="30%">' + page.res[page.pagelan].type.field_name["filename"] + '</th>';
    info += '           <th width="70%">' + page.res[page.pagelan].type.field_name["filecontent"] + '</th>';
    info += '       </tr>';
    info += '   </thead>';
    info += '   <tbody>';
    for(var p in vm_data["cfg_info"]["base_info"]["context"]){
        info += '       <tr>';
        info += '           <td width="30%">' + p + '</td>';
        
        if(p == v4_context_filename){
            info += '           <td width="70%">' + ParseConfigXml2Web(vm_data["cfg_info"]["base_info"]["context"][p], vm_data["cfg_info"]["base_info"]["nics"]) + '</td>';
        }
        else{
            info += '           <td width="70%">' + vm_data["cfg_info"]["base_info"]["context"][p] + '</td>';
        }
        info += '       </tr>';
    } 
    info += '   </tbody>';
    info += '</table>';
    idx_pos.html(info);
}

function ParseConfigXml(xml){
    var v4config  = new Object();
    var boardinfo = new Object();
    var special_device = new Object();
    var omm_ftpserver  = new Object();
    var vnfm           = new Object();
    var nic_functype_list = new Array();
    
    var xmldoc = LoadXMLStr(xml);
        
    boardinfo.netid      = GetXmlNodeStr(xmldoc.getElementsByTagName(label_netid).item(0));
    boardinfo.rack       = GetXmlNodeStr(xmldoc.getElementsByTagName(label_rack).item(0));
    boardinfo.shelf      = GetXmlNodeStr(xmldoc.getElementsByTagName(label_shelf).item(0));
    boardinfo.slot       = GetXmlNodeStr(xmldoc.getElementsByTagName(label_slot).item(0));
    boardinfo.cpu        = GetXmlNodeStr(xmldoc.getElementsByTagName(label_cpuid).item(0));
    boardinfo.board_attr = GetXmlNodeStr(xmldoc.getElementsByTagName(label_board_attr).item(0));
    //网卡功能类型 
    var nicfunctionlist = xmldoc.getElementsByTagName(label_interfaces).item(0).childNodes;
    for(var i = 0;i < nicfunctionlist.length;i++){
        var nicfunction = nicfunctionlist[i];
        if(nicfunction.nodeType == "1"){ 
            var nic_func      = new Object();
            nic_func.index    = GetXmlNodeStr(nicfunction.getElementsByTagName("index").item(0));
            nic_func.functype = GetXmlNodeStr(nicfunction.getElementsByTagName("function").item(0));
            nic_func.mac      = GetXmlNodeStr(nicfunction.getElementsByTagName("mac").item(0));
            nic_functype_list.push(nic_func);
        }
    }
    //OMM Type
    var version                = xmldoc.getElementsByTagName("config")[0].getAttribute("version");
    var omm_type               = xmldoc.getElementsByTagName(label_omm_type).length == 0 ? "" : GetXmlNodeStr(xmldoc.getElementsByTagName(label_omm_type)[0]);
    
    special_device.is_use_pdh  = GetXmlNodeStr(xmldoc.getElementsByTagName(label_is_use_pdh).item(0));

    omm_ftpserver.server_ip    = GetXmlNodeStr(xmldoc.getElementsByTagName(label_omm_ftp_serverip).item(0));
    omm_ftpserver.client_ip    = GetXmlNodeStr(xmldoc.getElementsByTagName(label_omm_ftp_clientip).item(0));
    omm_ftpserver.netmask      = GetXmlNodeStr(xmldoc.getElementsByTagName(label_omm_ftp_netmask).item(0));
    omm_ftpserver.username     = GetXmlNodeStr(xmldoc.getElementsByTagName(label_omm_ftp_user).item(0));    
    omm_ftpserver.password     = GetXmlNodeStr(xmldoc.getElementsByTagName(label_omm_ftp_pass).item(0));
    omm_ftpserver.relative_path  = GetXmlNodeStr(xmldoc.getElementsByTagName(label_omm_ftp_relative_path).item(0));
    omm_ftpserver.filename     = GetXmlNodeStr(xmldoc.getElementsByTagName(label_omm_ftp_filename).item(0));

    vnfm.node_ip            =  GetXmlNodeStr(xmldoc.getElementsByTagName(label_vnfm_nodeip).item(0));
    vnfm.netmask            =  GetXmlNodeStr(xmldoc.getElementsByTagName(label_vnfm_netmask).item(1));
    vnfm.gateway            =  GetXmlNodeStr(xmldoc.getElementsByTagName(label_vnfm_gateway).item(0));

    v4config.boardinfo         = boardinfo;
    v4config.nic_functype_list = nic_functype_list;
    v4config.omm_type          = omm_type;
    v4config.special_device    = special_device;
    v4config.omm_ftpserver     = omm_ftpserver;
    v4config.vnfm              = vnfm;
    return v4config;
}

function GetConfigXml(v4_config_obj){
    var content;
    content = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>";
    content += "<config version=\"1.1.0\">";
    //content +=   "<date>" + RFC3339DateString(new Date()) + "</date>";
    //content +=   "<uuid>" + GenUUIDFast() + "</uuid>";
    content +=   "<cloud_plat><name>zxtecs</name></cloud_plat>";
    content +=   "<board>";
    content +=     "<" + label_netid + ">" + v4_config_obj.boardinfo.netid + "</" + label_netid + ">";
    content +=     "<" + label_rack  + ">" + v4_config_obj.boardinfo.rack + "</" + label_rack  + ">";
    content +=     "<" + label_shelf + ">" + v4_config_obj.boardinfo.shelf + "</" + label_shelf + ">";
    content +=     "<" + label_slot  + ">" + v4_config_obj.boardinfo.slot + "</" + label_slot  + ">"; 
    content +=     "<" + label_cpuid + ">" + v4_config_obj.boardinfo.cpu + "</" + label_cpuid + ">"; 
    content +=     "<" + label_board_attr + ">" + v4_config_obj.boardinfo.board_attr + "</" + label_board_attr + ">"; 
    content +=   "</board>";
    content +=   "<network>";
    content +=     "<" + label_interfaces + ">";
    for(var i = 0;i < v4_config_obj.nic_functype_list.length;i++){
        var nic_functype = v4_config_obj.nic_functype_list[i];
        content +=   "<item>";
        content +=     "<index>" + nic_functype.index + "</index>";
        content +=     "<function>" + nic_functype.functype + "</function>";
        content +=     "<mac>" + nic_functype.mac + "</mac>";
        content +=   "</item>";
    }
    content +=     "</" + label_interfaces + ">";
    content +=     "<" + label_omm_type +">" + v4_config_obj.omm_type + "</" + label_omm_type +">";
    content +=   "</network>"; 
    content +=   "<special_device>";
    content +=       "<" + label_is_use_pdh +">" + v4_config_obj.special_device.is_use_pdh + "</" + label_is_use_pdh +">";
    content +=   "</special_device>";
    content +=   "<omm_ftpserver>";
    content +=       "<" + label_omm_ftp_serverip +">" + v4_config_obj.omm_ftpserver.server_ip + "</" + label_omm_ftp_serverip +">";
    content +=       "<" + label_omm_ftp_clientip +">" + v4_config_obj.omm_ftpserver.client_ip + "</" + label_omm_ftp_clientip +">";
    content +=       "<" + label_omm_ftp_netmask +">" + v4_config_obj.omm_ftpserver.netmask + "</" + label_omm_ftp_netmask +">";
    content +=       "<" + label_omm_ftp_user +">" + v4_config_obj.omm_ftpserver.username + "</" + label_omm_ftp_user +">";
    content +=       "<" + label_omm_ftp_pass +">" + v4_config_obj.omm_ftpserver.password + "</" + label_omm_ftp_pass +">";
    content +=       "<" + label_omm_ftp_relative_path +">" + v4_config_obj.omm_ftpserver.relative_path + "</" + label_omm_ftp_relative_path +">";                    
    content +=       "<" + label_omm_ftp_filename +">" + v4_config_obj.omm_ftpserver.filename + "</" + label_omm_ftp_filename +">";                    
    content +=   "</omm_ftpserver>";
    content +=   "<vnfm>";
    content +=       "<" + label_vnfm_nodeip +">" + v4_config_obj.vnfm.node_ip + "</" + label_vnfm_nodeip +">";
    content +=       "<" + label_vnfm_netmask +">" + v4_config_obj.vnfm.netmask + "</" + label_vnfm_netmask +">";
    content +=       "<" + label_vnfm_gateway +">" + v4_config_obj.vnfm.gateway + "</" + label_vnfm_gateway +">";
    content +=   "</vnfm>";

    content += "</config>";
    return content;
}

function GetV4ContextHtml(context,nics){
    var info = '';
    info += '<form class="form-horizontal">';
    info += '   <fieldset id="control_form">';
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["filename"] +'</label>'
    info += '           <div class="controls">';
    info += '               <label class="OamUi span12" bindattr="text" bind="key"></label>';
    info += '            </div>';
    info += '       </div>';
    
    info += '       <div class="control-group netid">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.display["netid"] +'<font color="red">*</font></label>'
    info += '           <div class="controls">';
    info += '               <input id="txNetId" type="text" class="OamUi span12" bindattr="val" bind="value.boardinfo.netid">';
    info += '               <span class="help-inline">' + page.res[page.pagelan].type.err_info["vm_v4contexts_netid_error"] +'</span>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '       <div class="control-group rack">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.display["rack"] +'<font color="red">*</font></label>'
    info += '           <div class="controls">';
    info += '               <input id="txRack" type="text" class="OamUi span12" bindattr="val" bind="value.boardinfo.rack">';
    info += '               <span class="help-inline">' + page.res[page.pagelan].type.err_info["vm_v4contexts_rack_error"] +'</span>';
    info += '            </div>';
    info += '       </div>';
    
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.display["shelf"] +'</label>'
    info += '           <div class="controls">';
    info += '               <select class="OamUi span12" id="slshelf" bindattr="val" bind="value.boardinfo.shelf"></select>';
    info += '               <span class="help-inline">1~3</span>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '       <div class="control-group slot">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.display["slot"] +'<font color="red">*</font></label>'
    info += '           <div class="controls">';
    info += '               <input id="txSlot" type="text" class="OamUi span12" bindattr="val" bind="value.boardinfo.slot">';
    info += '               <span class="help-inline">' + page.res[page.pagelan].type.err_info["vm_v4contexts_slot_error"] +'</span>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '       <div class="control-group">';
    info += '           <label class="control-label">CPU</label>'
    info += '           <div class="controls">';
    info += '               <select class="OamUi span12" id="slcpu" bindattr="val" bind="value.boardinfo.cpu"></select>';
    info += '               <span class="help-inline">1~8</span>';
    info += '            </div>';
    info += '       </div>';  
    
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.display["board_attr"] +'</label>'
    info += '           <div class="controls">';
    info += '               <select class="OamUi span12" id="slboard_attr" bindattr="val" bind="value.boardinfo.board_attr"></select>';
    info += '            </div>';
    info += '       </div>';
    

    for(var i = 0;i < context.value.nic_functype_list.length;i++){
        for(var j = i+1;j < context.value.nic_functype_list.length;j++){
            if(parseInt(context.value.nic_functype_list[i].index) > parseInt(context.value.nic_functype_list[j].index)){
                var temp = context.value.nic_functype_list[i];
                context.value.nic_functype_list[i] = context.value.nic_functype_list[j];
                context.value.nic_functype_list[j] = temp;
            }
        }
    }

    var nic_functype_list = context.value.nic_functype_list;
    if(nic_functype_list.length > 0){
        info += '       <hr>';
    }
    for(var i = 0;i < nic_functype_list.length;i++){
        var nic_functype = nic_functype_list[i];
        var index = GetIndexOfNic(nic_functype.index, nics);
        if(index == null){
            return "";
        }
        info += '       <div class="control-group">';
        info += '           <label class="control-label">' + GetNicName(nics[index]) +'</label>'
        info += '           <div class="controls">';
        info += '               <select class="span12 clfunc_type" data-value="' + nic_functype.index +'"></select>';
        info += '            </div>';
        info += '       </div>'; 
    }
    
    info += '       <hr>'; 
    
    info += '       <div class="control-group omm_type">';
    info += '           <label class="control-label"></label>'
    info += '           <div class="controls">';
    info += '               <label class="checkbox inline">';
    info += '                   <input type="checkbox" id="cbxfabric" value="option1" disabled > omm over fabric';
    info += '               </label>';
    info += '               <div style="clear:both"></div>';
    info += '               <label class="checkbox inline">';
    info += '                   <input type="checkbox" id="cbxbase" value="option2" disabled > omm over base';
    info += '               </label>';
    info += '               <span class="help-inline"></span>';
    info += '            </div>';
    info += '       </div>';

    info += '       <hr>'; 

    info += '       <div class="control-group">';
    info += '           <label class="control-label">PDH</label>'
    info += '           <div class="controls">';
    info += '                <input type="radio" name="optionsRadios" id="optionYes"> Yes';
    info += '                <input type="radio" name="optionsRadios" id="optionNo" checked> No';
    info += '           </div>';
    info += '       </div>';  
    
    info += '       <hr>';     

    info += '       <div class="control-group omm_ftp_server_ip">';
    info += '           <label class="control-label">OMM FtpServer IP</label>'
    info += '           <div class="controls">';
    info += '               <input id="txOmmFtpServerIp" type="text" class="OamUi span12" bindattr="val" bind="value.omm_ftpserver.server_ip">';    
    info += '           </div>';
    info += '       </div>'; 

    info += '       <div class="control-group omm_ftp_client_ip">';
    info += '           <label class="control-label">Client IP</label>'
    info += '           <div class="controls">';
    info += '               <input id="txOmmFtpClientIp" type="text" class="OamUi span12" bindattr="val" bind="value.omm_ftpserver.client_ip">';        
    info += '       </div>';
    info += '       </div>'; 

    info += '       <div class="control-group omm_ftp_netmask">';
    info += '           <label class="control-label">Netmask</label>'
    info += '           <div class="controls">';
    info += '               <input id="txOmmFtpNetmask" type="text" class="OamUi span12" bindattr="val" bind="value.omm_ftpserver.netmask">';            
    info += '           </div>';
    info += '       </div>';              

    info += '       <div class="control-group omm_ftp_user">';
    info += '           <label class="control-label">User</label>'
    info += '           <div class="controls">';
    info += '               <input id="txOmmFtpUser" type="text" maxlength=20 class="OamUi span12" bindattr="val" bind="value.omm_ftpserver.username">';            
    info += '           </div>';
    info += '       </div>';     

    info += '       <div class="control-group">';
    info += '           <label class="control-label">Password</label>'
    info += '           <div class="controls">';
    info += '               <input id="txOmmFtpPass" type="password" maxlength=20 class="OamUi span12" bindattr="val" bind="value.omm_ftpserver.password">';                
    info += '           </div>';
    info += '       </div>';         

    info += '       <div class="control-group">';
    info += '           <label class="control-label">Relative Path</label>'
    info += '           <div class="controls">';
    info += '               <input id="txOmmFtpRelativePath" type="text" class="OamUi span12" bindattr="val" bind="value.omm_ftpserver.relative_path">';                    
    info += '           </div>';
    info += '       </div>';         

    info += '       <div class="control-group omm_ftp_filename">';
    info += '           <label class="control-label">File Name</label>'
    info += '           <div class="controls">';
    info += '               <input id="txOmmFtpFile" type="text" maxlength=79 class="OamUi span12" bindattr="val" bind="value.omm_ftpserver.filename">';                    
    info += '           </div>';
    info += '       </div>';         

    info += '       <hr>'; 

    info += '       <div class="control-group vnfm_node_ip">';
    info += '           <label class="control-label">VNFM Node IP</label>'
    info += '           <div class="controls">';
    info += '               <input id="txVnfmNodeIP" type="text" class="OamUi span12" bindattr="val" bind="value.vnfm.node_ip">';                    
    info += '           </div>';
    info += '       </div>';  

    info += '       <div class="control-group vnfm_netmask">';
    info += '           <label class="control-label">Netmask</label>'
    info += '           <div class="controls">';
    info += '               <input id="txVnfmNetmask" type="text" class="OamUi span12" bindattr="val" bind="value.vnfm.netmask">';                        
    info += '           </div>';
    info += '       </div>';  

    info += '       <div class="control-group vnfm_gateway">';
    info += '           <label class="control-label">Gateway</label>'
    info += '           <div class="controls">';
    info += '               <input id="txVnfmGateway" type="text" class="OamUi span12" bindattr="val" bind="value.vnfm.gateway">';                        
    info += '           </div>';
    info += '       </div>';      

    info += '       <hr>';         

    info += '       <a></a>';   
    info += '   </fieldset>';

    info += '   <fieldset id="edit_form" class="hide">';
    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["filename"] +'</label>'
    info += '           <div class="controls">';
    info += '               <label class="OamUi span12" bindattr="text" bind="key"></label>';
    info += '            </div>';
    info += '       </div>';

    info += '       <div class="control-group value">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["filecontent"] +'</label>'
    info += '           <div class="controls">';
    info += '               <textarea rows=20 class="autogrow span12" id="v4config_xml"></textarea>';
    info += '            </div>';
    info += '       </div>'; 
    info += '       <a></a>';       
    info += '   </fieldset>';

    info += '</form>';
    return info;
}

function ParseConfigXml2Web(xml, nics){ 
    var content = "";  
    var v4config = ParseConfigXml(xml);
    
    content += page.res[page.pagelan].type.display["netid"];
    content += ":" + v4config.boardinfo.netid + "<br>";
    
    content += page.res[page.pagelan].type.display["rack"];
    content += ":" + v4config.boardinfo.rack + "<br>"; 
    
    content += page.res[page.pagelan].type.display["shelf"];
    content += ":" + v4config.boardinfo.shelf + "<br>";  
    
    content += page.res[page.pagelan].type.display["slot"];
    content += ":" + v4config.boardinfo.slot + "<br>";
     
    content += "CPU";
    content += ":" + v4config.boardinfo.cpu + "<br>";
   
    content += page.res[page.pagelan].type.display["board_attr"];
    content += ":" + page.res[page.pagelan].type.BoardAttr[v4config.boardinfo.board_attr] + "<br>"; 

    for(var i = 0;i < v4config.nic_functype_list.length;i++){
        var nic_functype = v4config.nic_functype_list[i];
        var index = GetIndexOfNic(nic_functype.index, nics);
        if(index == null){
            return;
        }
        content += GetNicName(nics[index]) + ":";
        content += page.res[page.pagelan].type.FunctionType[nic_functype.functype] +"<br>";
    }
    
    if(v4config.omm_type == omm_over_fabric){
        content += "omm over fabric<br>";
    }
    else if(v4config.omm_type == omm_over_base){
        content += "omm over base<br>";
    }

    content += "PDH";
    content += ":" + (v4config.special_device.is_use_pdh=="1" ? "Yes" : "No") + "<br>";

    content += "OMM FTPServer IP";
    content += ":" + v4config.omm_ftpserver.server_ip + "<br>";

    content += "Client IP";
    content += ":" + v4config.omm_ftpserver.client_ip + "<br>";

    content += "Netmask";
    content += ":" + v4config.omm_ftpserver.netmask + "<br>";

    content += "User";
    content += ":" + v4config.omm_ftpserver.username + "<br>";

    content += "Password";
    content += ":" + v4config.omm_ftpserver.password + "<br>";

    content += "Relative Path";
    content += ":" + v4config.omm_ftpserver.relative_path + "<br>";

    content += "File Name";
    content += ":" + v4config.omm_ftpserver.filename + "<br>";

    content += "VNFM Node IP";
    content += ":" + v4config.vnfm.node_ip + "<br>";

    content += "Netmask";
    content += ":" + v4config.vnfm.netmask + "<br>";

    content += "Gateway";
    content += ":" + v4config.vnfm.gateway + "<br>";        

    return content;
}

function Init_Context_Obj_List(){
    var context_obj                          = new Object();
    context_obj.key                          = "normal";
    context_obj.get_context_html_callback    = GetNormalContextHtml;
    context_obj.init_context_data_callback   = InitNormalContext;
    context_obj.update_context_data_callback = UpdateNormalContextData;
    context_obj_list.push(context_obj);
     
    context_obj                              = new Object();
    context_obj.key                          = v4_context_filename;
    context_obj.get_context_html_callback    = GetV4ContextHtml;
    context_obj.init_context_data_callback   = InitV4Context;
    context_obj.update_context_data_callback = UpdateV4ContextData;
    context_obj_list.push(context_obj);
}

function Get_Context_Obj(key){
    if(key == v4_context_filename){
        return context_obj_list[1];
    }
    else{
        return context_obj_list[0];
    }
}

function GetNormalContextHtml(){
    var info = '';
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    info += '       <div class="control-group key">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["filename"] +'<font color="red">*</font></label>'
    info += '           <div class="controls">';
    info += '               <input type="text" class="OamUi span12" bindattr="val" bind="key">';
    info += '               <span class="help-inline">' + page.res[page.pagelan].type.err_info["vm_contextkey_desc"] + '</span>';
    info += '            </div>';
    info += '       </div>';
    
    info += '       <div class="control-group value">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["filecontent"] +'</label>'
    info += '           <div class="controls">';
    info += '               <textarea rows=15 class="OamUi autogrow span12" bindattr="val" bind="value"></textarea>';
    info += '               <span class="help-inline"></span>';
    info += '            </div>';
    info += '       </div>'; 
    
    info += '   </fieldset>';
    info += '</form>';
    
    return info;
}


function GetContextObj(key, context){
    if(context[key] != null){
        var new_context = new Object();
        new_context.key = key;
        if(key == v4_context_filename){
            var v4config = ParseConfigXml(context[key]);
            new_context.value = v4config;
        } 
        else{
            new_context.value = context[key];
        }
        return new_context;
    };
    return null;
}

function InitNormalContext(diag, key, context){
    var context_obj = GetContextObj(key, context); 
    if(context_obj != null){
        diag.find('#context_right_content').html(GetNormalContextHtml());
        SetModifyForm($("#context_right_content"), context_obj);
    }
}

function InitSlShelf(sl){ 
    sl.empty();
    var shelf = new Array();
    for(var i = 1;i <= 3;i++){
        sl.append('<option value="' + i.toString() + '">' + i.toString() + '</option>');
    }
}

function InitSlCPU(sl){
    sl.empty();
    for(var i = 1;i <= 8;i++){
        sl.append('<option value="' + i.toString() + '">' + i.toString() + '</option>');
    }
}

function InitBoardAttr(sl){
    sl.empty();
    for(var p in page.res[page.pagelan].type.BoardAttr){
        sl.append("<option value="+p+">"+ page.res[page.pagelan].type.BoardAttr[p] +"</option>");
    }
}

function InitFuncType(sl){
    sl.empty();
    for(var p in page.res[page.pagelan].type.FunctionType){
        sl.append("<option value="+p+">"+ page.res[page.pagelan].type.FunctionType[p] +"</option>");
    }
}  

function InitV4Context(diag, key, context, nics){
    var context_obj = GetContextObj(key, context); 
    if(context_obj != null){
        diag.find('#context_right_content').html(GetV4ContextHtml(context_obj,nics));
        InitSlShelf(diag.find("#slshelf"));
        InitSlCPU(diag.find("#slcpu"));
        InitBoardAttr(diag.find("#slboard_attr"));
        InitFuncType(diag.find(".clfunc_type"));
        z_numInput("txNetId",1,253); 
        z_numInput("txRack",1,32); 
        z_numInput("txSlot",1,28);
        SetModifyForm($("#context_right_content"), context_obj);

        for(var i = 0;i < context_obj["value"]["nic_functype_list"].length;i++){
            var nic_functype = context_obj["value"]["nic_functype_list"][i];
            $('.clfunc_type[data-value="' + nic_functype.index + '"]').val(nic_functype.functype);
        }
        FuncTypeChange();
        
        if(context_obj["value"]["omm_type"] == omm_over_fabric){
            $("#cbxfabric").attr("checked",true);
        }
        else if(context_obj["value"]["omm_type"] == omm_over_base){
            $("#cbxbase").attr("checked",true);
        }
        else{
        }
        diag.find(".clfunc_type").change(FuncTypeChange);
    }
}

function DeleteContextData(alink, context){
    var key   = alink.attr("data-value");
    var context_obj = GetContextObj(key, context); 
    if(context_obj != null){
        delete context[key];
        alink.parent().remove();
    }
    if($('ul.context-menu li.active').length == 0){
        $('#context_right_content').html("");
        if($('ul.context-menu li:eq(0)').hasClass("add_normal_context")){
            return;
        }
        $('ul.context-menu li:eq(0)').addClass("active");
        $('ul.context-menu li a:eq(0)').click();
    }
}

function SaveCurrentPage(diag, context){
    if($('ul.context-menu li.active').length == 0){
        return true;
    } 
    var key = $('ul.context-menu li.active a').attr("data-value");
    return Get_Context_Obj(key).update_context_data_callback(diag, $('ul.context-menu li.active a'), context);
}

function CheckNormalContextValid(frm, old_key, context){
    var cnt = 0;
    var form_data = GetModifyForm(frm); 
    var contentcnt = 0;
    frm.find(".key").removeClass("error");
    frm.find(".value").removeClass("error"); 
    frm.find(".key .help-inline").html("");
    frm.find(".value .help-inline").html("");
    if(form_data["key"] == ""){
        frm.find(".key").addClass("error");
        frm.find(".key .help-inline").html(page.res[page.pagelan].type.err_info["vm_contextkey_desc"]);
        return false;
    }
    if(old_key != form_data["key"]){
        for(var p in context){
            if(form_data["key"] == p){
                cnt++;
            }
        } 
        if(cnt > 0){
            frm.find(".key .help-inline").html(page.res[page.pagelan].type.err_info["vm_contextkey_same_error"]);
            frm.find(".key").addClass("error");
            return false;
        }
    } 
    if( form_data["key"].charAt(0) == '.' ||
        form_data["key"].charAt(0) == '..'){
        frm.find(".key .help-inline").html(page.res[page.pagelan].type.err_info["vm_contextkey_specification_error"]);
        frm.find(".key").addClass("error");
        return false;
    }
    if( form_data["key"].indexOf("/") > -1 ||
        form_data["key"].indexOf("\\") > -1 ||
        form_data["key"].indexOf(":") > -1 ||
        form_data["key"].indexOf("*") > -1 ||
        form_data["key"].indexOf("?") > -1 ||
        form_data["key"].indexOf('"') > -1 ||
        form_data["key"].indexOf('<') > -1 ||
        form_data["key"].indexOf('>') > -1 ||
        form_data["key"].indexOf('|') > -1 ||
        form_data["key"].indexOf('&') > -1 ||
        form_data["key"].indexOf('$') > -1 ||
        form_data["key"].indexOf(' ') > -1 ||
        form_data["key"].indexOf('+') > -1 ||
        form_data["key"].indexOf('=') > -1 ||
        form_data["key"].indexOf('%') > -1){
        frm.find(".key .help-inline").html(page.res[page.pagelan].type.err_info["vm_contextkey_specification_error"]);
        frm.find(".key").addClass("error");
        return false;
    }
    if( form_data["value"].length >0 && $.trim(form_data["value"]).length == 0){ 
        frm.find(".value .help-inline").html(page.res[page.pagelan].type.err_info["vm_contextvalue_input_error"]);
        frm.find(".value").addClass("error");
        return false;
    }
    for(var p in context){
        if(p != form_data["key"]){
            contentcnt += p.length;
            contentcnt += context[p].length;
        }
    }
    contentcnt +=  form_data["key"].length;
    contentcnt +=  form_data["value"].length;
    //文件名和文件内容不能超过100K
    if(contentcnt > 100 * 1024){
        frm.find(".value .help-inline").html(page.res[page.pagelan].type.err_info["vm_contexts_contentnum_error"]);
        frm.find(".value").addClass("error");
        return false;
    }
    return true;
}

function UpdateNormalContextData(diag, alink, context){
    var old_key   = alink.attr("data-value");
    if(diag.find("#context_right_content").html() == ""){//首次加载,没有数据
        return true;
    }
    if(!CheckNormalContextValid(diag, old_key, context)){
        return false;
    }
    var context_obj = GetContextObj(old_key, context);
    if(context_obj != null){
        var form_data = GetModifyForm(diag);
        delete context[old_key];
        context[form_data["key"]]   = form_data["value"];
        var title = form_data["key"].length > 10 ? form_data["key"].substr(0,10) + "..." : form_data["key"];
        alink.find("span").eq(1).html(title);
        alink.attr("data-value", form_data["key"]);
    }
    return true;
}

function CheckV4ContextValid(frm, context){
    var cnt = 0;
    var form_data = GetModifyForm(frm); 
    var contentcnt = 0;
    frm.find(".control-group").removeClass("error");
    frm.find(".omm_type .help-inline").html("");
    
    if(form_data["value.boardinfo.netid"] == ""){ 
        frm.find(".netid").addClass("error");
        return false;
    }
    if(form_data["value.boardinfo.rack"] == ""){ 
        frm.find(".rack").addClass("error");
        return false;
    } 
    if(form_data["value.boardinfo.slot"] == ""){ 
        frm.find(".slot").addClass("error");
        return false;
    }
    
    if(form_data["value.omm_ftpserver.server_ip"] != "" && !checkIP(form_data["value.omm_ftpserver.server_ip"])){
        frm.find(".omm_ftp_server_ip").addClass("error");
        return false;
    }

    if(form_data["value.omm_ftpserver.client_ip"] != "" && !checkIP(form_data["value.omm_ftpserver.client_ip"])){
        frm.find(".omm_ftp_client_ip").addClass("error");
        return false;
    }

    /*if(!checkIP(form_data["value.omm_ftpserver.netmask"])){
        frm.find(".omm_ftp_netmask").addClass("error");        
        return false;
    }

    if(form_data["value.omm_ftpserver.username"] == ""){
        frm.find(".omm_ftp_user").addClass("error");        
        return false;
    }

    if(form_data["value.omm_ftpserver.filename"] == ""){
        frm.find(".omm_ftp_filename").addClass("error");        
        return false;
    }*/        

    if(form_data["value.vnfm.node_ip"] != "" && !checkIP(form_data["value.vnfm.node_ip"])){
        frm.find(".vnfm_node_ip").addClass("error");        
        return false;
    }

    if(form_data["value.vnfm.netmask"] != "" && !checkIP(form_data["value.vnfm.netmask"])){
        frm.find(".vnfm_netmask").addClass("error");        
        return false;
    }

    /*if(!checkIP(form_data["value.vnfm.gateway"])){
        frm.find(".vnfm_gateway").addClass("error");        
        return false;
    }*/
    
    if($("#cbxfabric").attr("checked") && $("#cbxbase").attr("checked")){ 
        frm.find(".omm_type").addClass("error");
        frm.find(".omm_type .help-inline").html(page.res[page.pagelan].type.err_info["vm_select_omm_functype"]);
        return false;
    }
    return true;
}

function UpdateV4ContextData(diag, alink, context){
    var key   = alink.attr("data-value");
    if(diag.find("#context_right_content").html() == ""){//首次加载,没有数据
        return true;
    }
    if(!CheckV4ContextValid(diag, context)){
        return false;
    }
    var form_data                      = GetModifyForm(diag);
    var v4_config_obj                  = ParseConfigXml(context[key]);
    v4_config_obj.boardinfo.netid      = form_data["value.boardinfo.netid"];
    v4_config_obj.boardinfo.rack       = form_data["value.boardinfo.rack"];
    v4_config_obj.boardinfo.shelf      = form_data["value.boardinfo.shelf"];
    v4_config_obj.boardinfo.slot       = form_data["value.boardinfo.slot"];
    v4_config_obj.boardinfo.cpu        = form_data["value.boardinfo.cpu"];
    v4_config_obj.boardinfo.board_attr = form_data["value.boardinfo.board_attr"];
    
    $(".clfunc_type").each(function(){ 
        var nic_index = $(this).attr("data-value");
        for(var i = 0;i < v4_config_obj.nic_functype_list.length;i++){
            var nic_functype = v4_config_obj.nic_functype_list[i];
            if(nic_functype.index == nic_index){
                v4_config_obj.nic_functype_list[i].functype = $(this).attr("value");
            }
        }
    }); 
    
    if($("#cbxfabric").attr("checked")){
        v4_config_obj.omm_type = omm_over_fabric;
    } 
    else if($("#cbxbase").attr("checked")){
        v4_config_obj.omm_type = omm_over_base;
    }
    else{
        v4_config_obj.omm_type = "";
    }

    if($("#optionYes").attr("checked")){
        v4_config_obj.special_device.is_use_pdh  = 1;    
    }
    else {
        v4_config_obj.special_device.is_use_pdh  = 0;
    }
    
    v4_config_obj.omm_ftpserver.server_ip    = form_data["value.omm_ftpserver.server_ip"];
    v4_config_obj.omm_ftpserver.client_ip    = form_data["value.omm_ftpserver.client_ip"];
    v4_config_obj.omm_ftpserver.netmask      = form_data["value.omm_ftpserver.netmask"];
    v4_config_obj.omm_ftpserver.username     = form_data["value.omm_ftpserver.username"];
    v4_config_obj.omm_ftpserver.password     = form_data["value.omm_ftpserver.password"];
    v4_config_obj.omm_ftpserver.relative_path  = form_data["value.omm_ftpserver.relative_path"];
    v4_config_obj.omm_ftpserver.filename     = form_data["value.omm_ftpserver.filename"];

    v4_config_obj.vnfm.node_ip            =  form_data["value.vnfm.node_ip"];
    v4_config_obj.vnfm.netmask            =  form_data["value.vnfm.netmask"];
    v4_config_obj.vnfm.gateway            =  form_data["value.vnfm.gateway"];

    context[key] = GetConfigXml(v4_config_obj);
    return true;
}

function GetDefaultNormalContextFileName(context){
    var fix = "Document_";
    var cnt = 1;
    var ret;
    while(1){
        ret = fix + cnt;
        var flag = false;
        for(var p in context){
            if(ret == p){
                flag = true;
                break;
            }
        }
        if(!flag){
            break;
        }
        cnt++;
    } 
    return ret;
}

function AddNormalContextData(droper, drag_elem_obj, vm_data, vm_action){
    var key   = GetDefaultNormalContextFileName(vm_data["cfg_info"]["base_info"]["context"]);
    vm_data["cfg_info"]["base_info"]["context"][key] = "";
    ShowContextImage(droper, drag_elem_obj, vm_data);
    return;
} 

function AddV4ContextData(droper, drag_elem_obj, vm_data, vm_action){
    var key = v4_context_filename; 
    var content;
    
    for(var p in vm_data["cfg_info"]["base_info"]["context"]){
        if(p == v4_context_filename){
            return;
        }
    }
    
    content = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>";
    content += "<config version=\"1.1.0\">";
    //content +=   "<date>" + RFC3339DateString(new Date()) + "</date>";
    //content +=   "<uuid>" + GenUUIDFast() + "</uuid>";
    content +=   "<cloud_plat><name>zxtecs</name></cloud_plat>";
    content += "  <board>";
    content += "    <" + label_netid + ">1</" + label_netid + ">";
    content += "    <" + label_rack  + ">1</" + label_rack  + ">";
    content += "    <" + label_shelf + ">1</" + label_shelf + ">";
    content += "    <" + label_slot  + ">1</" + label_slot  + ">"; 
    content += "    <" + label_cpuid + ">1</" + label_cpuid + ">"; 
    content += "    <" + label_board_attr + ">1</" + label_board_attr + ">"; 
    content += "  </board>";
    content += "  <network>";
    content += "    <" + label_interfaces + ">";
    for(var i = 0;i < vm_data["cfg_info"]["base_info"]["nics"].length;i++){
        var nic = vm_data["cfg_info"]["base_info"]["nics"][i];
        content += "      <item>";
        content += "        <index>" + nic["nic_index"] + "</index>";
        content += "        <function>UNKNOWN</function>";
        content += "        <mac>" + nic["mac"] + "</mac>";
        content += "      </item>";
    }
    content += "    </" + label_interfaces + ">";
    content += "    <" + label_omm_type +"></" + label_omm_type +">";
    content += "  </network>"; 
    content +=   "<special_device>";
    content +=       "<" + label_is_use_pdh +"></" + label_is_use_pdh +">";
    content +=   "</special_device>";
    content +=   "<omm_ftpserver>";
    content +=       "<" + label_omm_ftp_serverip +"></" + label_omm_ftp_serverip +">";
    content +=       "<" + label_omm_ftp_clientip +"></" + label_omm_ftp_clientip +">";
    content +=       "<" + label_omm_ftp_netmask +"></" + label_omm_ftp_netmask +">";
    content +=       "<" + label_omm_ftp_user +"></" + label_omm_ftp_user +">";
    content +=       "<" + label_omm_ftp_pass +"></" + label_omm_ftp_pass +">";
    content +=       "<" + label_omm_ftp_relative_path +"></" + label_omm_ftp_relative_path +">";                    
    content +=       "<" + label_omm_ftp_filename +"></" + label_omm_ftp_filename +">";                    
    content +=   "</omm_ftpserver>";
    content +=   "<vnfm>";
    content +=       "<" + label_vnfm_nodeip +"></" + label_vnfm_nodeip +">";
    content +=       "<" + label_vnfm_netmask +"></" + label_vnfm_netmask +">";
    content +=       "<" + label_vnfm_gateway +"></" + label_vnfm_gateway +">";
    content +=   "</vnfm>";    
    content += "</config>";
    vm_data["cfg_info"]["base_info"]["context"][key] = content;
    ShowContextImage(droper, drag_elem_obj, vm_data);
    return;
}

function ShowContextImage(droper, drag_elem_obj, vm_data){
    if(GetContextCount(vm_data["cfg_info"]["base_info"]["context"]) == 0){
        return;
    }
    if(!droper.hasClass("configed")){
        Init_Context_Obj_List();
        var info = '<span class="span12"><img src="img/' + drag_elem_obj.img + '"/></span>'; 
        var class_name = drag_elem_obj.clname;
        info += '<span class="pull-right icon-trash ' + class_name + '" style="cursor:pointer;"></span>';
        droper.find(".box-content").find(".span12").html(info); 
        droper.addClass(class_name).addClass("configed");
        droper.data("data", drag_elem_obj);
    }
}

function DeleteContextsDataCallBack(diag, droper, vm_data, drag_elem_obj){
    vm_data["cfg_info"]["base_info"]["context"] = new Object();
    droper.find(".box-content").find(".span12").html("");
    diag.modal("hide");
    droper.removeClass("configed");
}

function DeleteContextsData(diag, droper, vm_data, drag_elem_obj){
    diag.find('div.modal-header h5').html(page.res[page.pagelan].type.display["confirm_op"]);
    diag.find('div.modal-body').html(page.res[page.pagelan].type.err_info["delete_confirm_info"]);
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        DeleteContextsDataCallBack(diag, droper, vm_data, drag_elem_obj);
    });
    diag.modal("show");
}

function InsertNode(parentnode,name,value,xmldoc){
    var newel    = xmldoc.createElement(name);
    var newtext  = xmldoc.createTextNode(value);
    newel.appendChild(newtext);
    return parentnode.appendChild(newel);
}

function GetSubNodeText(node){
    var str = "";
    if(node.nodeType != "1"){
        return "";
    }
    str += "<" + node.tagName + ">";
    var subnodelist = node.childNodes;
    for(var i = 0;i < subnodelist.length;i++){
        if(subnodelist[i].nodeType == "1"){
            str += "<" + subnodelist[i].nodeName + ">" + GetXmlNodeStr(subnodelist[i]) + "</" + subnodelist[i].nodeName + ">";
        }
    }
    str += "</" + node.tagName + ">";
    return str;
}

function GetV4ConfigContext(xmldoc){
    var frmxmlstr = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>";
    frmxmlstr += "<" + xmldoc.documentElement.tagName + " version=\"" + xmldoc.documentElement.getAttribute("version") + "\">";
    frmxmlstr += "<cloud_plat><name>zxtecs</name></cloud_plat>";    
    frmxmlstr += GetSubNodeText(xmldoc.documentElement.getElementsByTagName("board").item(0));
    frmxmlstr += "<" + label_network + ">";
    frmxmlstr += "<" + label_interfaces + ">\n";
    for(var i = 0;i < xmldoc.getElementsByTagName(label_interfaces)[0].childNodes.length;i++){
        frmxmlstr += GetSubNodeText(xmldoc.getElementsByTagName(label_interfaces)[0].childNodes[i]);
    }
    frmxmlstr += "</" + label_interfaces + ">";
    var ommfunctype = xmldoc.getElementsByTagName("omm_type").length == 0?"":GetXmlNodeStr(xmldoc.getElementsByTagName("omm_type")[0]);
    frmxmlstr += "<omm_type>" + ommfunctype + "</omm_type>";
    frmxmlstr += "</" + label_network + ">";
    frmxmlstr += GetSubNodeText(xmldoc.documentElement.getElementsByTagName("special_device").item(0));
    frmxmlstr += GetSubNodeText(xmldoc.documentElement.getElementsByTagName("omm_ftpserver").item(0));
    frmxmlstr += GetSubNodeText(xmldoc.documentElement.getElementsByTagName("vnfm").item(0));
    frmxmlstr += "</" + xmldoc.documentElement.tagName + ">";
    return frmxmlstr;
}

function AddNicContext(nic_index, context){
    var xmldoc   = null;
    var itemnode = null;
    var indexnode = null;
    var functionnode = null;
    if(context[v4_context_filename] == null){
        return;
    }

    xmldoc       = LoadXMLStr(context[v4_context_filename]); 
    
    itemnode = InsertNode(xmldoc.getElementsByTagName(label_interfaces)[0],label_item,"",xmldoc);//插入item节点
    if(itemnode == null){
        return;
    }
    
    indexnode = InsertNode(itemnode,label_index,nic_index.toString(),xmldoc);//插入index节点
    if(indexnode == null){
        return;
    }
    
    functionnode = InsertNode(itemnode,label_function,"UNKNOWN",xmldoc);//插入function节点
    if(functionnode == null){
        return;
    }

    macnode = InsertNode(itemnode, label_mac, "", xmldoc);//插入function节点
    if(macnode == null){
        return;
    }    

    context[v4_context_filename] = GetV4ConfigContext(xmldoc);
}

function UpdateNicMac(nic_index, context, mac){
    if(context[v4_context_filename] == null){
        return;
    }
    
    var xmldoc  = LoadXMLStr(context[v4_context_filename]); 
    
    var nicfunctionlist = xmldoc.getElementsByTagName(label_interfaces).item(0).childNodes;
    for(var i = 0;i < nicfunctionlist.length;i++){
        var nicfunction = nicfunctionlist[i];
        if(nicfunction.nodeType == "1"){ 
            var idx  = GetXmlNodeStr(nicfunction.getElementsByTagName("index").item(0));

            if(parseInt(idx) == parseInt(nic_index)){
                 SetXmlNodeStr(nicfunction.getElementsByTagName("mac").item(0), mac);
            }
        }
    }

    context[v4_context_filename] = GetV4ConfigContext(xmldoc);
}

function GetFuncTypeCount(xmldoc,functype){
    var count = 0;
    var niclist = xmldoc.getElementsByTagName(label_interfaces)[0].childNodes;
    for (var i = 0;i < niclist.length;i++){
        var node = niclist[i];
        if(node.nodeType != "1"){
            continue;
        }
        if(functype == GetXmlNodeStr(node.getElementsByTagName("function").item(0))){
            count++;
        }
    }
    return count;
}

function DeleteNicContext(nic_index, context){
    var xmldoc   = null;
    
    if(context[v4_context_filename] == null){
        return;
    }
    xmldoc       = LoadXMLStr(context[v4_context_filename]);
    var nodelist = xmldoc.getElementsByTagName(label_interfaces)[0].childNodes;
    for(var i = 0;i < nodelist.length;i++){
        var node = nodelist[i];
        if(node.nodeType == "1"){
            if(GetXmlNodeStr(node.getElementsByTagName("index").item(0)) == nic_index){
                xmldoc.getElementsByTagName(label_interfaces).item(0).removeChild(node);
                break;
            }
        }
    }
    
    var omm_type = xmldoc.getElementsByTagName(label_omm_type).length == 0 ? "" : GetXmlNodeStr(xmldoc.getElementsByTagName(label_omm_type)[0]);
    var functype = "";
    if(omm_type == omm_over_fabric){
        functype = "INTERMEDIA";
    }
    else if(omm_type == omm_over_base){
        functype = "CTRL";
    } 
    else{
        ;
    }
    
    if(functype != ""){
        var count = GetFuncTypeCount(xmldoc,functype); 
        if(count == 0){
            SetXmlNodeStr(xmldoc.getElementsByTagName("omm_type")[0], "");
        }
    }
    
    context[v4_context_filename] = GetV4ConfigContext(xmldoc);
    return;
}

function FuncTypeChange(){
    var ctrlcnt = 0;
    var intermediacnt = 0;
    var omccnt = 0; 

    $(".clfunc_type").each(function(){
        if($(this).attr("value") == "CTRL"){
           ctrlcnt++;
        }
        else if($(this).attr("value") == "INTERMEDIA"){
            intermediacnt++; 
        }
        else if($(this).attr("value") == "OMC"){
            omccnt++;
        }
        else{
            ;
        }
    });
    if(omccnt == 0){ 
        //可以选 只能二选一
        if(ctrlcnt > 0 && intermediacnt > 0){
            $("#cbxfabric").removeAttr("disabled");
            $("#cbxbase").removeAttr("disabled");
        }
        else if(ctrlcnt > 0){
            $("#cbxfabric").attr("disabled","disabled");
            $("#cbxbase").removeAttr("disabled"); 
            $("#cbxfabric").attr("checked",false);
        }
        else if(intermediacnt > 0){
            $("#cbxfabric").removeAttr("disabled");
            $("#cbxbase").attr("disabled","disabled");
            $("#cbxbase").attr("checked",false); 
        }
        else{
            $("#cbxfabric").attr("disabled","disabled");
            $("#cbxbase").attr("disabled","disabled");
            $("#cbxfabric").attr("checked",false);
            $("#cbxbase").attr("checked",false);
        }
    } 
    else{
        $("#cbxfabric").attr("disabled","disabled");
        $("#cbxbase").attr("disabled","disabled");
        $("#cbxfabric").attr("checked",false);
        $("#cbxbase").attr("checked",false);
    }
}
