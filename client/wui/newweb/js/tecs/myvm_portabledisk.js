var oMyPDisktDbt;
var currPdiskState = new Object();

/**********************************************************************
 * 函数名称: RetrieveMyPortableData
 * 功能描述: 查询移动盘
 * 输入参数:
 * 输出参数:
 * 返 回 值:
 * 其它说明:
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 
 ***********************************************************************/
function RetrieveMyPortableData( sSource, aoData, fnCallback ){
    $("tbMyPDisk input[type=checkbox]").attr("checked", false);
    aoData.push( { "name": "user_name", "value": userdata.name} );

    $.getJSON( sSource, aoData, function (json) {	   
        var prodata = json.data;
        var requests   = []; 
        if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
        }
        for(var i = 0;i < prodata.aaData.length;i++){
            requests[i]   = prodata.aaData[i][0];		
			prodata.aaData[i][2] = Capacity(prodata.aaData[i][2]);
        }
				
        //绑定checkbox,添加卸载和挂载
        for(var i = 0;i < prodata.aaData.length;i++){
            prodata.aaData[i][0] = GetItemCheckbox(requests[i], currPdiskState);//'<input type="checkbox" value=' + '"' + requests[i] + '" >';
            prodata.aaData[i][6] = '';
			var oplink ='';
			if("" == prodata.aaData[i][4]){
                oplink = '<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["attach"] +'" onclick="OnAttachClick('+ "'" + requests[i]  + "'" +')"><i class="icon-resize-small"></i><span></span></a>';			
                oplink += ' | <a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["attach_hc"] +'" onclick="OnAttachHcClick('+ "'" + requests[i]  + "'" +')"><i class="icon icon-black icon-link"></i><span></span></a>';                
			}
            else {
                if(prodata.aaData[i][4] == "tohost"){
                    prodata.aaData[i][4] = "";
                    prodata.aaData[i][5] = "";

                    oplink = '<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["deattach_hc"] +'" onclick="OnDeattachHcClick('+ "'" + requests[i]  + "'" +')"><i class="icon icon-black icon-unlink"></i><span></span></a>';                        

                    //查询是否挂载到物理机
                    var rs = xmlrpc_service.tecs.portabledisk.blockinfo.get(xmlrpc_session, requests[i]); 
                    if(rs[0] == 0){
                        prodata.aaData[i][6] = '<strong>Type : </strong>' + rs[1] + '<br/><strong>Target : </strong>' 
                                                + rs[2] + '<br/><strong>Lun ID:</strong> ' + rs[3] + '<br/><strong>IP : </strong> ' + rs[4];
                    }   
                    else {
                        ShowNotyRpcErrorInfo(rs);
                    }                
                }
                else {
                    oplink = '<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["deattach"] +'" onclick="OnDeattachClick('+ "'" + requests[i]  + "'"+ "," +"'" + prodata.aaData[i][4]  + "'" +')"><i class="icon-resize-full"></i><span></span></a>';                                     
                }                
			}
            prodata.aaData[i][7] = oplink;		
        }
		
        fnCallback(prodata);
				
        ClearCurSelItem(currPdiskState);

		//显示更新时间
        ShowLastUpdateTime($("#tbMyPDisk").parent());

        $(".tooltip").hide();
        $('[rel="tooltip"],[data-rel="tooltip"]').tooltip({"placement":"bottom"});
    });
}


/**********************************************************************
 * 函数名称: OnDeattachClick
 * 功能描述: 卸载移动盘xmlrpc方法
 * 输入参数:
 * 输出参数:
 * 返 回 值:
 * 其它说明:
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 
 ***********************************************************************/
function OnDeattachClick(requestid,vid){ 
    var vidGet = parseInt(vid);
	var vmState= GetVmRunningInfo(vidGet);	
    var shutoff_state   = 4;		
    if(shutoff_state != vmState['computationinfo']['state']){	
        ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["vm_pdisk_need"]);
        return false;
    }	
	
    var rs = null;
    try{
        rs = xmlrpc_service.tecs.vmcfg.portable_disk.detach(xmlrpc_session, new I8(vidGet), requestid);
        if(rs[0] != 0 ){
            ShowNotyRpcErrorInfo(rs);
            return false;
        }		
    }catch(e){
      var em;
      if (e.toTraceString){
            em = e.toTraceString;
      }else{
            em = e.message;
      }
      rs = "Error Trace:\n\n" + em;
      alert(rs);
      return false;
    } 
		
    Refresh_MyPDisk_Data();
    return true;
}

/**********************************************************************
 * 函数名称: attachcallback
 * 功能描述: 挂载移动盘xmlrpc方法
 * 输入参数:
 * 输出参数:
 * 返 回 值:
 * 其它说明:
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 
 ***********************************************************************/
function attachcallback(frm ,requestid){

    var rs = null;
    var item = GetModifyForm(frm); 
    frm.find(".control-group").removeClass("error");	

	var vidGet     = item.vmtype;
    var diskTarget = item.pdisk_target;
    //vidGet = String(vidGet);
	
	if(null == vidGet || "" == vidGet){
	    frm.find(".curvm").addClass("error");	
	    return false;
	}
	
    if(null == diskTarget || "" == diskTarget){
        frm.find(".target").addClass("error");   
        return false;
    }    

	//提示用户只有已部署并关机的虚拟机才可以执行操作
    vidGet = parseInt(vidGet);	
	var vmState= GetVmRunningInfo(vidGet);	
    var shutoff_state   = 4;	
    if(shutoff_state != vmState['computationinfo']['state']){	
        frm.find(".curvm").addClass("error");		
		frm.find(".curvm .help-inline").addClass("error").show();
        return false;
    }	
	
    try{
        rs = xmlrpc_service.tecs.vmcfg.portable_disk.attach(xmlrpc_session, requestid, new I8(vidGet), item.pdisk_target, item.pdisk_bus);
        if(rs[0] != 0){
            ShowNotyRpcErrorInfo(rs);
            return false;
        } 
    }catch(e){
      alert(e.message);
      return false;
    } 
    Refresh_MyPDisk_Data();
    return true;
}

/**********************************************************************
 * 函数名称: OnAttachClick
 * 功能描述: 挂载移动盘
 * 输入参数:
 * 输出参数:
 * 返 回 值:
 * 其它说明:
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 
 ***********************************************************************/
function OnAttachClick(requestid){ 
    var params = [];
    params.push( { "name": "user_name", "value": userdata.name} );

    $.getJSON("../php/myvm_pdisk_vm.php", params, function(json){	
        var showInfo = '';			
	    var prodata = json.data;
		
        for(var i = 0;i < prodata.aaData.length;i++){
		    if(prodata.aaData[i]["disks"].length!=10){
             	showInfo +=  '<option disks=' + prodata.aaData[i]["disks"].toString() + ' value=' + prodata.aaData[i]["vid"] + '>' + prodata.aaData[i]["vid"] + '</option>';						
			}
        }	
		
        var info = '';
        var diag = $('#idx_confirmDlg');
        info += '<form class="form-horizontal">';
        info += '   <fieldset>';
    
        info += '       <div class="control-group curvm">';
        info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["vid"] + '<font color="red">*</font></label>';
        info += '           <div class="controls">';
        info += '              <select id="vm_id" class="OamUi" bindattr="val" bind="vmtype">';
        info +=                  showInfo;	
        info += '              </select>';	
        info += '              <br/><span class="help-inline hide">'  + page.res[page.pagelan].type.err_info["vm_pdisk_need"] + '</span>';			
        info += '            </div>';
        info += '       </div>';
		
        info += '       <div class="control-group bus">';
        info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["pbus"] + '<font color="red">*</font></label>';
        info += '           <div class="controls">';		
        info += '              <select id="txPdiskbus" class="OamUi" bindattr="val" bind="pdisk_bus">';	
        info += '              </select>';			
        info += '            </div>';
        info += '       </div>';	
		
        info += '       <div class="control-group target">';
        info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["ptarget"] + '<font color="red">*</font></label>';		
        info += '           <div class="controls">';		
        info += '              <select id="txPdisktarget" class="OamUi" bindattr="val" bind="pdisk_target">';			
        info += '              </select>';				
        info += '            </div>';
        info += '       </div>';		
	
        info += '   </fieldset>';
        info += '</form>';
     
        var header = '<i class="icon-resize-small"></i>&nbsp;' + page.res[page.pagelan].type.opr["attach"] + '-' + requestid;
        diag.find('div.modal-header h5').html(header);
        diag.find('div.modal-body').html(info);	
		
        for(var k in page.res[page.pagelan].type.BusType){
           $("#txPdiskbus").append("<option value=" + k + ">" + page.res[page.pagelan].type.BusType[k] + "</option>");		
        }
		
        $("#vm_id").unbind("change").bind("change", function(){
		    //选中切换时把错误提示隐藏
			$(".curvm").removeClass("error");
            $(".curvm .help-inline").removeClass("error").hide();	
            PortableDiskName($("#txPdiskbus").val(), $("#vm_id option:selected").attr("disks"));
        });
		
		$("#txPdiskbus").unbind("change").bind("change", function(){
            PortableDiskName($("#txPdiskbus").val(), $("#vm_id option:selected").attr("disks"));
        });
							
        $("#vm_id").trigger("change"); 

        diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
            if(!attachcallback(diag.find(".form-horizontal"), requestid)){
                return;
            }
            diag.modal("hide");
        });		

        diag.modal("show");        	
    });	

}

function PortableDiskName(bustype, inused){
    $("#txPdisktarget").html('');    
    if(bustype == 'ide'){
        for(var i = 98; i <= 99;i++){
            var hddname = "hd" +  String.fromCharCode(i);
            if(inused == null || inused.indexOf(hddname) == -1) {
                $("#txPdisktarget").append("<option value=" + hddname + ">" + hddname +  "</option>");
            }              
        }                           
    }
    else {
        for(var i = 97; i <= 104;i++){
            var sddname = "sd" +  String.fromCharCode(i);
            if(inused == null || inused.indexOf(sddname) == -1){
                $("#txPdisktarget").append("<option value=" + sddname + ">" + sddname +  "</option>");              
            }
        }       
                    
    }
}

/**********************************************************************
 * 函数名称: Add_PortableDisk_Callback
 * 功能描述: 申请移动盘
 * 输入参数:
 * 输出参数:
 * 返 回 值:
 * 其它说明:
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 
 ***********************************************************************/
function Add_PortableDisk_Callback(frm,func_callback){
    var rs = null;    
    var item = GetModifyForm(frm);

    frm.find(".control-group").removeClass("error");
    if(item.clustetype == "" ||item.clustetype == null){
        frm.find(".getcluste").addClass("error");
        return false;
    }	
    if(item.size == "" || isNaN(item.size)){
        frm.find(".psize").addClass("error");
        return false;
    }	

	//页面中填入的是GB需要转成字节
	var sizeGet = item.size*1024*1024*1024;
    sizeGet = String(sizeGet);
    sizeGet = parseInt(sizeGet);
	
    try{
        rs = xmlrpc_service.tecs.portabledisk.create(xmlrpc_session, new I8(sizeGet), item.clustetype, item.user_volname);
		
		//暂时加上，操作进行中的错误码，后面添加工作流控制再修改
        if(rs[0] != 0 && rs[0] != 25){
            ShowNotyRpcErrorInfo(rs);
            return false;
        }
    }catch(e){
        alert(e.message);
        return false;
    }
		
	//申请成功后更新表格
    if(func_callback != null){
        func_callback();
    }
    return true;
}


/**********************************************************************
 * 函数名称: Create_Portabledisk
 * 功能描述: 添加移动盘配置窗口
 * 输入参数:
 * 输出参数:
 * 返 回 值:
 * 其它说明:
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 
 ***********************************************************************/
function Create_Portabledisk(func_callback){
    $.getJSON("../php/myvm_pdisk_cluster.php", function(json){	
        var showInfo = '';			
		var prodata = json.data;
        for(var i = 0;i < prodata.aaData.length;i++){
		    //获取所在集群名
          	showInfo +=  "<option value="+prodata.aaData[i][1]+">"+ prodata.aaData[i][1] + "</option>";					
        }	
		
        var diag = $('#idx_confirmDlg');
        var info = '';
        info += '<form class="form-horizontal">';
        info += '   <fieldset>';
		
        info += '       <div class="control-group getcluste">';
        info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["clustername"] + '<font color="red">*</font></label>';
        info += '           <div class="controls">';
        info += '              <select id="cluster_type_input" class="OamUi" bindattr="val" bind="clustetype">';
        info +=                  showInfo;	
        info += '              </select>';	
        info += '            </div>';
        info += '       </div>';
		
        info += '       <div class="control-group psize">';
        info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["disksize"] + '<font color="red">*</font></label>';
        info += '           <div class="controls">';
        info += '               <input id="txPdisksize" type="text" maxlength="7" class="OamUi" bindattr="val" bind="size">';
        info += '               <span class="help-inline">' + page.res[page.pagelan].type.err_info["vm_disksize_desc"] + '</span>';		
        info += '            </div>';
        info += '       </div>';		

        info += '       <div class="control-group pname">';
        info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["desc"] + '</label>';
        info += '           <div class="controls">';
        info += '               <textarea id="txPdickName" class="autogrow OamUi" id="taDesc" maxlength="128" rows="6" bindattr="val" bind="user_volname"></textarea><p id="pSetDescInfo"></p>';
        info += '            </div>';		
	
        info += '       </div>';	
		
        info += '   </fieldset>';
        info += '</form>';
  
        var header = '<i class="icon-plus"></i>&nbsp;' + page.res[page.pagelan].type.opr["adddisk"];
        diag.find('div.modal-header h5').html(header);
        diag.find('div.modal-body').html(info); 
		
	    //移动盘校验
        LimitCharLenInput($("#txPdickName"),$("#pSetDescInfo"));
		
        diag.modal("show"); 
        diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
   
	        //如果创建成功，则重新绘制表格:这里调用申请移动盘的接口
            if(!Add_PortableDisk_Callback(diag.find(".form-horizontal"), func_callback)){
                return;
            }
            diag.modal("hide");
        });	
    });		
		
}

/**********************************************************************
 * 函数名称: create
 * 功能描述: 添加移动盘
 * 输入参数:
 * 输出参数:
 * 返 回 值:
 * 其它说明:
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 
 ***********************************************************************/
function create(){ 
    Create_Portabledisk(Refresh_MyPDisk_Data);
}

/**********************************************************************
 * 函数名称: RemovePortableCallback
 * 功能描述: 释放移动盘
 * 输入参数:
 * 输出参数:
 * 返 回 值:
 * 其它说明:
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 
 ***********************************************************************/
function RemovePortableCallback(vCheckedbox){
    ClearAlertInfo();
    vCheckedbox.each(function(){
        var itemindex = $(this).attr("value"); 
        var result = xmlrpc_service.tecs.portabledisk['delete'](xmlrpc_session, itemindex);
        if(result[0] == 0){

        }
        else {
            ShowSynRpcInfo(result);
        }
    }); 

    oMyPDisktDbt.fnDraw();
}
/**********************************************************************
 * 函数名称: remove
 * 功能描述: 删除选中项
 * 输入参数:
 * 输出参数:
 * 返 回 值:
 * 其它说明:
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 
 ***********************************************************************/
function remove(){
    ShowBatchOpConfirmDiag($("#idx_confirmDlg"), "tbMyPDisk", page.res[page.pagelan].type.err_info["delete_confirm_info"], RemovePortableCallback);
}

/**********************************************************************
 * 函数名称: Refresh_MyPDisk_Data
 * 功能描述: 刷新表格
 * 输入参数:
 * 输出参数:
 * 返 回 值:
 * 其它说明:
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 
 ***********************************************************************/
function Refresh_MyPDisk_Data(){
    if($("#tbMyPDisk").length == 0){
        clearInterval(pdisk_refresh_timer);
        return;
    }

    RecordSelItem(oMyPDisktDbt, "tbMyPDisk", currPdiskState);  
    GotoCurrentPageOfDataTable(oMyPDisktDbt);
}

/**********************************************************************
 * 函数名称: OnAttachHcClick
 * 功能描述: 挂载到物理机
 * 输入参数:
 * 输出参数:
 * 返 回 值:
 * 其它说明:
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 
 ***********************************************************************/
function OnAttachHcClick(requestid){
    var info = '';
    var diag = $('#idx_confirmDlg');
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
 
    info += '       <div class="control-group ip">';
    info += '           <label class="control-label">' + 'IP'/*page.res[page.pagelan].type.field_name["ip"]*/ + '<font color="red">*</font></label>';
    info += '           <div class="controls">';        
    info += '                <input type="text" id="pdisk_ip" maxlength="15" />'     
    info += '           </div>';
    info += '       </div>';    
    
    info += '       <div class="control-group iSCSI">';
    info += '           <label class="control-label">' + 'iSCSI'/*page.res[page.pagelan].type.field_name["iscsi"]*/ + '<font color="red">*</font></label>';      
    info += '           <div class="controls">';        
    info += '                <input type="text" id="pdisk_iscsi" maxlength="64" />'              
    info += '            </div>';
    info += '       </div>';        

    info += '   </fieldset>';
    info += '</form>';
 
    var header = '<i class="icon icon-black icon-link"></i>&nbsp;' + page.res[page.pagelan].type.opr["attach"] + '-' + requestid;
    diag.find('div.modal-header h5').html(header);
    diag.find('div.modal-body').html(info); 

    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        var ip = $("#pdisk_ip").val();
        var iscsi = $("#pdisk_iscsi").val();

        diag.find(".control-group").removeClass("error");
        if(null == ip || "" == ip){
            diag.find(".ip").addClass("error");   
            return false;
        }
        
        if(null == iscsi || "" == iscsi){
            diag.find(".iSCSI").addClass("error");   
            return false;
        }            

        var rs = xmlrpc_service.tecs.portabledisk.attachhost(xmlrpc_session, requestid, ip, iscsi);

        if(rs[0] == 0){
            diag.modal("hide");    
        }
        else {
            ShowNotyRpcErrorInfo(rs);
        }        
    });     

    diag.modal("show");          
}

/**********************************************************************
 * 函数名称: OnDeattachHcClick
 * 功能描述: 从物理机卸载
 * 输入参数:
 * 输出参数:
 * 返 回 值:
 * 其它说明:
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 
 ***********************************************************************/
function OnDeattachHcClick(requestid){
    var rs = xmlrpc_service.tecs.portabledisk.detachhost(xmlrpc_session, requestid);

    if(rs[0] != 0){
        ShowNotyRpcErrorInfo(rs);
    }
}

/**********************************************************************
 * 函数名称: OnSelectAll
 * 功能描述: 全部选中
 * 输入参数:
 * 输出参数:
 * 返 回 值:
 * 其它说明:
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 
 ***********************************************************************/
function OnSelectAll(){
    if($(this).attr("checked") == "checked"){
        $("#tbMyPDisk tbody input[type=checkbox]").attr("checked", true);
    }
    else{
        $("#tbMyPDisk tbody input[type=checkbox]").attr("checked", false);
    }
}

$(function(){
    //移动盘查询表格显示，需要直接从数据库查询
    oMyPDisktDbt = $('#tbMyPDisk').dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] },
		               { "bSortable": true, "aTargets": [ 1 ] },
		               { "bSortable": true, "aTargets": [ 2 ] },
		               { "bSortable": false, "aTargets": [ 3 ] },
		               { "bSortable": false, "aTargets": [ 4 ] },					   
                       { "bSortable": false, "aTargets": [ 5 ] },					   
					   { "bSortable": false, "aTargets": [ 6] },
                       { "bSortable": false, "aTargets": [ 7] }],
        "alternatively" : "destory",
        "bProcessing": false,
        "bServerSide": true,
        "sAjaxSource": "../php/myvm_pdisk.php",
        "fnServerData": RetrieveMyPortableData
    } );
		
    //tabs
    $('#tabsDisk a:first').tab('show');
    $('#tabsDisk a').click(function (e) {
      e.preventDefault();
      $(this).tab('show');
    });
			
    // OamUi静态显示部分初始化,国际化
    $("OamStaticUI").each(function(){
        SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
    });
	
	//鉴权
    if(userdata.role != 1){
        $(".breadcrumb").hide();
    }
     
    //页面的按钮	 
    $("#SelecltAll").click(OnSelectAll);
    $("#myportable_refresh").click(Refresh_MyPDisk_Data); 	
    $("#remove").click(remove);
    $("#create").click(create);
	
    pdisk_refresh_timer = setInterval(Refresh_MyPDisk_Data, 3000);
	
	//点击三级导航
    $('ul.breadcrumb li a').click(Switch_Vr_Page);
});
