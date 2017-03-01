var oDeployVmDbt;
var oFictionVmDbt;
var rootParent = null;
var activeNode = null;
var bkProgress_updatepg_id  = null;
var timer_vmop_updatepg_id = null;
var progress_updatepg_id   = null;
var vmdeploy_updatepg_id   = null;
var waiting_state   = 255;
var running_state   = 2;
var paused_state    = 3;
var shutoff_state   = 4;
var crashed_state   = 5;
var unknown_state   = 6;
var fiction_state   = 7;
var freeze_state    = 8;
var depolying_state = 9;
var starting_state  = 10;
var stoping_state   = 11;
var rebooting_state = 12;
var pausing_state   = 13;
var resuming_state  = 14;
var migrating_state = 16;
var reseting_state  = 17;
var destroy_state   = 18;
var saveasimage_state     = 22;
var cancelsaveimage_state = 23;
var pluginusb_state       = 24;
var plugoutusb_state      = 25;
var replugusb_state       = 26;
var pluginpdisk_state     = 27;
var plugoutpdisk_state    = 28;
var recoverimage_state    = 29;
var last_op_result_succ   = 0;
var last_op_result_processing = 33; 

var optableobj = new Object(); 

function CreateOpPermissionsTable(){
    var permission = new Array();
    permission[0] = "deployedvm_poweron";
    permission[1] = "deployedvm_poweroff";
    permission[2] = "deployedvm_reboot"; 
    permission[3] = "deployedvm_pause";
    permission[4] = "deployedvm_resume"; 
    permission[5] = "deployedvm_forcereboot";
    permission[6] = "deployedvm_canceldeploy";
    permission[7] = "deployedvm_forcepoweroff"; 
    permission[8] = "deployedvm_saveasimage";
    permission[9] = "deployedvm_recoverimage";
    permission[10] = "deployedvm_freeze";
    permission[11] = "deployedvm_unfreeze";    
        
    optableobj.permission = permission;
    
    var optable = new Array();
    var opobj = null;
    
    opobj = new Object();
    opobj.state = waiting_state;//waiting
    opobj.oppermission = new Object();
    opobj.oppermission[permission[0]] = false;
    opobj.oppermission[permission[1]] = false; 
    opobj.oppermission[permission[2]] = false; 
    opobj.oppermission[permission[3]] = false;
    opobj.oppermission[permission[4]] = false;
    opobj.oppermission[permission[5]] = false;
    opobj.oppermission[permission[6]] = true;
    opobj.oppermission[permission[7]] = false;
    opobj.oppermission[permission[8]] = false;
    opobj.oppermission[permission[9]] = false;    
    opobj.oppermission[permission[10]] = false;
    opobj.oppermission[permission[11]] = false;       
    optable.push(opobj); 
    
    opobj = new Object();
    opobj.state = running_state;//running
    opobj.oppermission = new Object();
    opobj.oppermission[permission[0]] = false;
    opobj.oppermission[permission[1]] = true; 
    opobj.oppermission[permission[2]] = true; 
    opobj.oppermission[permission[3]] = true;
    opobj.oppermission[permission[4]] = false;
    opobj.oppermission[permission[5]] = true;
    opobj.oppermission[permission[6]] = true;
    opobj.oppermission[permission[7]] = true; 
    opobj.oppermission[permission[8]] = false; 
    opobj.oppermission[permission[9]] = false;        
    opobj.oppermission[permission[10]] = false;
    opobj.oppermission[permission[11]] = false;             
    optable.push(opobj);
     
    opobj = new Object();
    opobj.state = paused_state;//paused
    opobj.oppermission = new Object();
    opobj.oppermission[permission[0]] = false;
    opobj.oppermission[permission[1]] = false; 
    opobj.oppermission[permission[2]] = false; 
    opobj.oppermission[permission[3]] = false;
    opobj.oppermission[permission[4]] = true;
    opobj.oppermission[permission[5]] = true;
    opobj.oppermission[permission[6]] = true;
    opobj.oppermission[permission[7]] = true;
    opobj.oppermission[permission[8]] = false;
    opobj.oppermission[permission[9]] = false;        
    opobj.oppermission[permission[10]] = false;
    opobj.oppermission[permission[11]] = false;              
    optable.push(opobj);
     
    opobj = new Object();
    opobj.state = shutoff_state;//shutoff
    opobj.oppermission = new Object();
    opobj.oppermission[permission[0]] = true;
    opobj.oppermission[permission[1]] = false; 
    opobj.oppermission[permission[2]] = false; 
    opobj.oppermission[permission[3]] = false;
    opobj.oppermission[permission[4]] = false;
    opobj.oppermission[permission[5]] = false;
    opobj.oppermission[permission[6]] = true;
    opobj.oppermission[permission[7]] = false;
    opobj.oppermission[permission[8]] = true;
    opobj.oppermission[permission[9]] = true;        
    opobj.oppermission[permission[10]] = true;
    opobj.oppermission[permission[11]] = false;             
    optable.push(opobj);
     
    opobj = new Object();
    opobj.state = crashed_state;//crashed
    opobj.oppermission = new Object();
    opobj.oppermission[permission[0]] = false;
    opobj.oppermission[permission[1]] = false; 
    opobj.oppermission[permission[2]] = false; 
    opobj.oppermission[permission[3]] = false;
    opobj.oppermission[permission[4]] = false;
    opobj.oppermission[permission[5]] = true;
    opobj.oppermission[permission[6]] = true;
    opobj.oppermission[permission[7]] = true; 
    opobj.oppermission[permission[8]] = false;
    opobj.oppermission[permission[9]] = false;        
    opobj.oppermission[permission[10]] = false;
    opobj.oppermission[permission[11]] = false;            
    optable.push(opobj);
     
    opobj = new Object();
    opobj.state = unknown_state;//unknown
    opobj.oppermission = new Object();
    opobj.oppermission[permission[0]] = false;
    opobj.oppermission[permission[1]] = false; 
    opobj.oppermission[permission[2]] = false; 
    opobj.oppermission[permission[3]] = false;
    opobj.oppermission[permission[4]] = false;
    opobj.oppermission[permission[5]] = false;
    opobj.oppermission[permission[6]] = true;
    opobj.oppermission[permission[7]] = false;
    opobj.oppermission[permission[8]] = false;
    opobj.oppermission[permission[9]] = false;        
    opobj.oppermission[permission[10]] = false;
    opobj.oppermission[permission[11]] = false;               
    optable.push(opobj);
     
    opobj = new Object();
    opobj.state = depolying_state;//depolying
    opobj.oppermission = new Object();
    opobj.oppermission[permission[0]] = false;
    opobj.oppermission[permission[1]] = false; 
    opobj.oppermission[permission[2]] = false; 
    opobj.oppermission[permission[3]] = false;
    opobj.oppermission[permission[4]] = false;
    opobj.oppermission[permission[5]] = false;
    opobj.oppermission[permission[6]] = true;
    opobj.oppermission[permission[7]] = false;
    opobj.oppermission[permission[8]] = false;
    opobj.oppermission[permission[9]] = false;        
    opobj.oppermission[permission[10]] = false;
    opobj.oppermission[permission[11]] = false;         
    optable.push(opobj);
     
    opobj = new Object();
    opobj.state = starting_state;//starting
    opobj.oppermission = new Object();
    opobj.oppermission[permission[0]] = false;
    opobj.oppermission[permission[1]] = false; 
    opobj.oppermission[permission[2]] = false; 
    opobj.oppermission[permission[3]] = false;
    opobj.oppermission[permission[4]] = false;
    opobj.oppermission[permission[5]] = false;
    opobj.oppermission[permission[6]] = true;
    opobj.oppermission[permission[7]] = false;
    opobj.oppermission[permission[8]] = false;
    opobj.oppermission[permission[9]] = false;        
    opobj.oppermission[permission[10]] = false;
    opobj.oppermission[permission[11]] = false;              
    optable.push(opobj);
     
    opobj = new Object();
    opobj.state = stoping_state;//stoping
    opobj.oppermission = new Object();
    opobj.oppermission[permission[0]] = false;
    opobj.oppermission[permission[1]] = false; 
    opobj.oppermission[permission[2]] = false; 
    opobj.oppermission[permission[3]] = false;
    opobj.oppermission[permission[4]] = false;
    opobj.oppermission[permission[5]] = false;
    opobj.oppermission[permission[6]] = true;
    opobj.oppermission[permission[7]] = true; 
    opobj.oppermission[permission[8]] = false;
    opobj.oppermission[permission[9]] = false;        
    opobj.oppermission[permission[10]] = false;
    opobj.oppermission[permission[11]] = false;             
    optable.push(opobj);
     
    opobj = new Object();
    opobj.state = rebooting_state;//rebooting
    opobj.oppermission = new Object();
    opobj.oppermission[permission[0]] = false;
    opobj.oppermission[permission[1]] = false; 
    opobj.oppermission[permission[2]] = false; 
    opobj.oppermission[permission[3]] = false;
    opobj.oppermission[permission[4]] = false;
    opobj.oppermission[permission[5]] = true;
    opobj.oppermission[permission[6]] = true;
    opobj.oppermission[permission[7]] = false;
    opobj.oppermission[permission[8]] = false;
    opobj.oppermission[permission[9]] = false;        
    opobj.oppermission[permission[10]] = false;
    opobj.oppermission[permission[11]] = false;             
    optable.push(opobj);
     
    opobj = new Object();
    opobj.state = pausing_state;//pausing
    opobj.oppermission = new Object();
    opobj.oppermission[permission[0]] = false;
    opobj.oppermission[permission[1]] = false; 
    opobj.oppermission[permission[2]] = false; 
    opobj.oppermission[permission[3]] = false;
    opobj.oppermission[permission[4]] = false;
    opobj.oppermission[permission[5]] = false;
    opobj.oppermission[permission[6]] = true;
    opobj.oppermission[permission[7]] = false;
    opobj.oppermission[permission[8]] = false;
    opobj.oppermission[permission[9]] = false;        
    opobj.oppermission[permission[10]] = false;
    opobj.oppermission[permission[11]] = false;              
    optable.push(opobj);
     
    opobj = new Object();
    opobj.state = resuming_state;//resuming
    opobj.oppermission = new Object();
    opobj.oppermission[permission[0]] = false;
    opobj.oppermission[permission[1]] = false; 
    opobj.oppermission[permission[2]] = false; 
    opobj.oppermission[permission[3]] = false;
    opobj.oppermission[permission[4]] = false;
    opobj.oppermission[permission[5]] = false;
    opobj.oppermission[permission[6]] = true;
    opobj.oppermission[permission[7]] = false;
    opobj.oppermission[permission[8]] = false;
    opobj.oppermission[permission[9]] = false;        
    opobj.oppermission[permission[10]] = false;
    opobj.oppermission[permission[11]] = false;               
    optable.push(opobj);
     
    opobj = new Object();
    opobj.state = migrating_state;//migrating
    opobj.oppermission = new Object();
    opobj.oppermission[permission[0]] = false;
    opobj.oppermission[permission[1]] = false; 
    opobj.oppermission[permission[2]] = false; 
    opobj.oppermission[permission[3]] = false;
    opobj.oppermission[permission[4]] = false;
    opobj.oppermission[permission[5]] = false;
    opobj.oppermission[permission[6]] = true;
    opobj.oppermission[permission[7]] = false;
    opobj.oppermission[permission[8]] = false;
    opobj.oppermission[permission[9]] = false;        
    opobj.oppermission[permission[10]] = false;
    opobj.oppermission[permission[11]] = false;            
    optable.push(opobj);
     
    opobj = new Object();
    opobj.state = reseting_state;//reseting
    opobj.oppermission = new Object();
    opobj.oppermission[permission[0]] = false;
    opobj.oppermission[permission[1]] = false; 
    opobj.oppermission[permission[2]] = false; 
    opobj.oppermission[permission[3]] = false;
    opobj.oppermission[permission[4]] = false;
    opobj.oppermission[permission[5]] = false;
    opobj.oppermission[permission[6]] = true;
    opobj.oppermission[permission[7]] = false;
    opobj.oppermission[permission[8]] = false;
    opobj.oppermission[permission[9]] = false;        
    opobj.oppermission[permission[10]] = false;
    opobj.oppermission[permission[11]] = false;              
    optable.push(opobj);

    opobj = new Object();
    opobj.state = destroy_state;//destroying
    opobj.oppermission = new Object();
    opobj.oppermission[permission[0]] = false;
    opobj.oppermission[permission[1]] = false; 
    opobj.oppermission[permission[2]] = false; 
    opobj.oppermission[permission[3]] = false;
    opobj.oppermission[permission[4]] = false;
    opobj.oppermission[permission[5]] = false;
    opobj.oppermission[permission[6]] = true;
    opobj.oppermission[permission[7]] = false;
    opobj.oppermission[permission[8]] = false;
    opobj.oppermission[permission[9]] = false;        
    opobj.oppermission[permission[10]] = false;
    opobj.oppermission[permission[11]] = false;             
    optable.push(opobj);
     
    opobj = new Object();
    opobj.state = saveasimage_state;//saveasimage
    opobj.oppermission = new Object();
    opobj.oppermission[permission[0]] = false;
    opobj.oppermission[permission[1]] = false; 
    opobj.oppermission[permission[2]] = false; 
    opobj.oppermission[permission[3]] = false;
    opobj.oppermission[permission[4]] = false;
    opobj.oppermission[permission[5]] = false;
    opobj.oppermission[permission[6]] = true;
    opobj.oppermission[permission[7]] = false;
    opobj.oppermission[permission[8]] = false;
    opobj.oppermission[permission[9]] = false;        
    opobj.oppermission[permission[10]] = false;
    opobj.oppermission[permission[11]] = false;             
    optable.push(opobj);

    opobj = new Object();
    opobj.state = pluginpdisk_state;//pluginpdisk
    opobj.oppermission = new Object();
    opobj.oppermission[permission[0]] = true;
    opobj.oppermission[permission[1]] = true; 
    opobj.oppermission[permission[2]] = true; 
    opobj.oppermission[permission[3]] = false;
    opobj.oppermission[permission[4]] = false;
    opobj.oppermission[permission[5]] = true;
    opobj.oppermission[permission[6]] = true;
    opobj.oppermission[permission[7]] = true;
    opobj.oppermission[permission[8]] = true;
    opobj.oppermission[permission[9]] = true;         
    opobj.oppermission[permission[10]] = false;
    opobj.oppermission[permission[11]] = false;               
    optable.push(opobj);	
	
    opobj = new Object();
    opobj.state = plugoutpdisk_state;//plugoutpdisk
    opobj.oppermission = new Object();
    opobj.oppermission[permission[0]] = true;
    opobj.oppermission[permission[1]] = true; 
    opobj.oppermission[permission[2]] = true; 
    opobj.oppermission[permission[3]] = false;
    opobj.oppermission[permission[4]] = false;
    opobj.oppermission[permission[5]] = true;
    opobj.oppermission[permission[6]] = true;
    opobj.oppermission[permission[7]] = true;
    opobj.oppermission[permission[8]] = true;
    opobj.oppermission[permission[9]] = true;      
    opobj.oppermission[permission[10]] = false;
    opobj.oppermission[permission[11]] = false;           
    optable.push(opobj);	

    opobj = new Object();
    opobj.state = freeze_state;//freeze
    opobj.oppermission = new Object();
    opobj.oppermission[permission[0]] = false;
    opobj.oppermission[permission[1]] = false; 
    opobj.oppermission[permission[2]] = false; 
    opobj.oppermission[permission[3]] = false;
    opobj.oppermission[permission[4]] = false;
    opobj.oppermission[permission[5]] = false;
    opobj.oppermission[permission[6]] = true;
    opobj.oppermission[permission[7]] = false; 
    opobj.oppermission[permission[8]] = false; 
    opobj.oppermission[permission[9]] = false;  
    opobj.oppermission[permission[10]] = false;
    opobj.oppermission[permission[11]] = true;             
    optable.push(opobj);	
    
    optableobj.optable    = optable;
} 

function FilterOp(){
    var permission = new Array();
    var vCheckedbox = $("#tbDeployVm>tbody").find(":checkbox:checked");
    for(var i = 0;i < optableobj.permission.length;i++){
        permission.push(optableobj.permission[i]); 
    }
    for(var i = 0;i < permission.length;i++){
        if(vCheckedbox.size() == 0){
            $("#"+permission[i]).removeAttr("disabled");
        }
        else{
            $("#"+permission[i]).attr("disabled","disabled");
        }
    }
    vCheckedbox.each(function(){
        var item             = $(this).data("item");
        var state            = item["state"];
        var last_op          = item["last_op"];
        var last_op_result   = item["last_op_result"];
        var opstate;
        if(last_op_result == last_op_result_succ){
            opstate = state;
        }
        else if(last_op_result == last_op_result_processing){
            opstate = last_op;
        } 
        else{
            opstate = state;
        }
        for(var i = 0;i < optableobj.optable.length;i++){
            var opobj = optableobj.optable[i];
            if(opobj.state == opstate){
                for(var p in opobj.oppermission){
                    if(!opobj.oppermission[p]){
                        for(var j = 0;j < permission.length;j++){
                            if(p == permission[j]){
                                permission.splice(j,1);
                                if(permission.length == 0){
                                     return; 
                                }
                            } 
                        } 
                    }  
                }
            } 
        }
    });
    for(var i = 0;i < permission.length;i++){
        $("#"+permission[i]).removeAttr("disabled");
    }
} 

function BindVmData(vmdata){ 
    for(var i = 0;i < vmdata.length && i < $("#tbDeployVm>tbody").find(":checkbox").length;i++){
        var cb = $("#tbDeployVm>tbody").find(":checkbox").eq(i);
        cb.data("item", vmdata[i]);
    }
} 

function BindFictonVmdata(vmdata) {
	for(var i= 0; i<vmdata.length && i< $("#tbFictonVm>tbody").find(":checkbox").length; i++){
        var cb = $("#tbFictonVm>tbody>tr").find(":checkbox").eq(i);
        cb.data("item", vmdata[i]);
    }
}

function GetVmStateImage(state){
    var imglist = new Array();
    var img = new Object();
    img.index = waiting_state; 
    img.name = "waiting.gif";
    imglist.push(img);
    
    img = new Object();
    img.index = running_state;
    img.name = "running.png"; 
    imglist.push(img);
    
    img = new Object();
    img.index = paused_state;
    img.name = "paused.png"; 
    imglist.push(img);
    
    img = new Object();
    img.index = shutoff_state;
    img.name = "shutoff.png";
    imglist.push(img);
     
    img = new Object();
    img.index = crashed_state;
    img.name = "crashed.png"; 
    imglist.push(img); 
    
    img = new Object();
    img.index = freeze_state;
    img.name = "freeze.png"; 
    imglist.push(img); 
    
    img = new Object();
    img.index = unknown_state;
    img.name = "unknown.png";
    imglist.push(img); 


    for(var i = 0;i < imglist.length;i++){
        if(state == imglist[i].index){
            return imglist[i].name;
        }
    }
    return "";
}

function GetVmStateImageURL(state,clname_vmstate,tc_config_version,run_version,clname_verdifferent){
    var stateinfo = page.res[page.pagelan].type.VmState[state];
    var verdeferent = page.res[page.pagelan].type.display["ver_different"];
    var imgurl = '<img data-rel="tooltip" class="' + clname_vmstate +'" src="./img/vm_status/' + GetVmStateImage(state) + '" title="' + stateinfo +'"/>';
    if(tc_config_version != run_version){
        imgurl += '<img data-rel="tooltip" class="' + clname_verdifferent + '" src="./img/vm_status/different.png" title="' + verdeferent +'"/>';
    }
    return imgurl;
}

function ShowVmState(rs){
    var vm = null;
    var vmdata = new Array();
    var vmobj = new Object();
    if(rs[0] != 0){
        ShowNotyRpcErrorInfo(rs);
        return;
    } 
    vm = rs[3][0]; 
    if(vm["state"] == fiction_state){
        RetrieveDeployVmData(); 
        return;
    }
    vmobj.state            = vm["state"];
    vmobj.id               = vm["base_info"]["id"].toString();
    vmobj.last_op          = 0;
    vmobj.last_op_result   = 0;
    vmobj.last_op_progress = 0;
    vmdata.push(vmobj);
    SetVmState(vmdata); 
    for(var i = 0;i < i < $("#tbDeployVm>tbody>tr").length;i++){
        var tr = $("#tbDeployVm>tbody>tr").eq(i);
        var item = tr.find("td:first").find("Input").data("item");
        if(item.id == vmobj.id){
            item.state = vmobj.state;
        }
    }
} 

function ShowOpProcess(nTr,tDynamicInfo){
    var progress_id = "pb_" + tDynamicInfo.id;
    if (!(oDeployVmDbt.fnIsOpen(nTr))){//新增一行显示进度
        var progressbar = GetOpProcessURL(progress_id,tDynamicInfo.last_op_progress);
        if(tDynamicInfo.last_op == saveasimage_state){
            progressbar += '<button class="btn" onclick="Cancelsaveasimage(' + "'" + tDynamicInfo.id + "'" + ')">';
            progressbar += '<i class="icon-remove-sign"></i>';
            progressbar += '<span>' + page.res[page.pagelan].type.opr["cancel"];
            progressbar += '</span></button>';
        } 
        oDeployVmDbt.fnOpen( nTr, progressbar, 'details' );
    }
    else{//更新进度
        var old_progress_flag = $("#"+progress_id)[0].className.split(" ")[1];
        var new_progress_flag = GetOpProgressFlag(tDynamicInfo.last_op_progress); 
        if(old_progress_flag != new_progress_flag){
            $("#"+progress_id).removeClass();
            $("#"+progress_id).addClass("progress").addClass(new_progress_flag).addClass("span6").addClass("active");
        }
        $("#"+progress_id+" .bar").attr("style","width: " + tDynamicInfo.last_op_progress + "%");
        $("#"+progress_id+" .bar").html(tDynamicInfo.last_op_progress + "%");
    }
} 

function Cancelsaveasimage(vid){
    var id = new I8(vid);
    xmlrpc_service.tecs.vmcfg["action"](xmlrpc_session, id,"cancel_upload_img",ShowInstanceVmCallBack);
}  

function UpDateVmDynamicInfo(dynamicinfo,vm_tr){
    var vm_td               = null;
    var clname_vmstate      = "vmstate";
    var clname_verdifferent = "verdifferent";
    if(vm_tr == null){
        return;
    }
    vm_td = vm_tr.find("td").eq(1);
    vm_ip = vm_tr.find("td").eq(8); 
    vm_op = vm_tr.find("td").eq(10);
    
    if(dynamicinfo.state == 4 || dynamicinfo.state == 8){
        vm_tr.find(".spModifyDeployVm").show();
        vm_tr.find(".spBackupDeployVm").show();				
    } 
    else{
        vm_tr.find(".spModifyDeployVm").hide();
        vm_tr.find(".spBackupDeployVm").hide();			
    }

    if(dynamicinfo.state == waiting_state){
        vm_td.html(GetVmStateImageURL(dynamicinfo.state,clname_vmstate,dynamicinfo.tc_config_version,dynamicinfo.run_version,clname_verdifferent));
        QueryVmCallBack(dynamicinfo.id,ShowVmState);
        /*var nTr = vm_tr[0];
        if ( oDeployVmDbt.fnIsOpen(nTr) ){
            oDeployVmDbt.fnClose( nTr );
        }*/
    }
    else if(dynamicinfo.last_op_result == last_op_result_succ){
        vm_td.html(GetVmStateImageURL(dynamicinfo.state,clname_vmstate,dynamicinfo.tc_config_version,dynamicinfo.run_version,clname_verdifferent));
        vm_op.show();
        /*var nTr = vm_tr[0];
        if ( oDeployVmDbt.fnIsOpen(nTr) ){
            oDeployVmDbt.fnClose( nTr );
        }*/
    }
    else if(dynamicinfo.last_op_result == last_op_result_processing){
        var nTr = vm_tr[0]; 
        var last_opinfo = page.res[page.pagelan].type.VmOpState[dynamicinfo.last_op];
        vm_td.html(last_opinfo);
        //vm_op.html('');
        vm_op.attr("disable","none");
    }
    else{
        vm_td.html(GetVmStateImageURL(dynamicinfo.state,clname_vmstate,dynamicinfo.tc_config_version,dynamicinfo.run_version,clname_verdifferent));
        /*var nTr = vm_tr[0];
        if ( oDeployVmDbt.fnIsOpen(nTr) ){
            oDeployVmDbt.fnClose( nTr );
        }*/
    }
    vm_ip.html(dynamicinfo["ip"]);
    FilterOp();

    $(".tooltip").hide();
    $('[rel="tooltip"],[data-rel="tooltip"]').tooltip({"placement":"bottom"});
}

function ShowVmDynamicInfo(dynamicinfolist){ 
    for(var i = 0;i < dynamicinfolist.length;i++){
        var dynamicinfo = dynamicinfolist[i];
        var vCheckedbox = $("#tbDeployVm>tbody").find(":checkbox");
        vCheckedbox.each(function(){ 
            var vm_tr = null;
            var item = $(this).data("item");
            if(item["id"] == dynamicinfo.id){
                vm_tr = $(this).parent().parent();
                UpDateVmDynamicInfo(dynamicinfo,vm_tr);
            }
        });
    }
}

function TimerUpDateVmDynamicInfo(){
    var sSource = "../php/vm_dynamicinfo.php"; 
    var vmsid   = "";
    var vCheckedbox = $("#tbDeployVm>tbody").find(":checkbox");
    if($('#tbDeployVm').length == 0){
        clearInterval(timer_vmop_updatepg_id);
        return;
    }
    if(vCheckedbox.size() == 0){
        return;
    }
    vCheckedbox.each(function(){
        var item = $(this).data("item");
        vmsid += item["id"] + ",";
    });
    vmsid = vmsid.substring(0,vmsid.length - 1);
    var aoData  = { vmsid: vmsid};
    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
        }
        for(var i = 0;i < ackdata.aaData.length;i++){ 
            var vmobj = new Object();
            vmobj.id                = ackdata.aaData[i][0];
            vmobj.state             = parseInt(ackdata.aaData[i][1]);
            vmobj.last_op           = parseInt(ackdata.aaData[i][2]);
            vmobj.last_op_result    = parseInt(ackdata.aaData[i][3]);
            vmobj.last_op_progress  = parseInt(ackdata.aaData[i][4]);
            vmobj.ip                = ackdata.aaData[i][5];
            vmobj.tc_config_version = parseInt(ackdata.aaData[i][6]);
            vmobj.cc_config_version = parseInt(ackdata.aaData[i][7]);
            vmobj.run_version       = parseInt(ackdata.aaData[i][8]);
            vCheckedbox.each(function(){
                var vm_tr = null;
                var item = $(this).data("item");
                if(item == null || item == ""){
                    return;
                }
                if(item["id"] == vmobj["id"]){ 
                    vm_tr = $(this).parent().parent();
                    if(item["state"]             != vmobj["state"] || 
                       item["last_op_result"]    != vmobj["last_op_result"] || 
                       item["last_op_progress"]  != vmobj["last_op_progress"] || 
                       item["ip"]                != vmobj["ip"] ||
                       item["tc_config_version"] != vmobj["tc_config_version"] ||
                       item["cc_config_version"] != vmobj["cc_config_version"] ||
                       item["run_version"]       != vmobj["run_version"]){

                       vmobj.workflow          = item.workflow;
                       vmobj.cluster           = item.cluster;                          
                       $(this).data("item",vmobj);
                       UpDateVmDynamicInfo(vmobj,vm_tr);
                       //RefreshDeployVmData();
                    }
                }
            });
        }
        
    });
} 

function showVMActionProgressInfo(){
    //var vCheckedbox = $("#tbDeployVm>tbody").find(":checkbox");
   
    if($('#tbDeployVm').length == 0){
        clearInterval(progress_updatepg_id);
        return;
    }

    ShowWorkflowProgress(oDeployVmDbt, RefreshDeployVmData);

    /*vCheckedbox.each(function(){
        var item = $(this).data("item");        

        if(item["workflow"] != null){ 
            var tr = $(this).parent().parent();
            var nTr = tr[0];
            var progress_id = "pb_" + item["workflow"];

            if (!oDeployVmDbt.fnIsOpen(nTr)){
                var progressbar = GetOpProcessURL(progress_id, 0);
                oDeployVmDbt.fnOpen( nTr, progressbar, 'details');
            } 

            GetWorkflowProgress(item["workflow"], function(pi){
                var progress_id = "pb_" + pi[3];

                if(pi[1] == -1){
                    updateProcessByID(progress_id, pi[2]);
                } 
                else {
                    var rs = [];
                    if(pi[1] == 0){
                       rs[0] = 0;  rs[1] = "";
                    }
                    else {
                       rs[0] = 2; rs[1] = "workflow " + pi[0] + " failed, reason " + pi[2];
                    } 
                    ShowSynRpcInfo(rs);

                    RefreshDeployVmData();
                }                
            }, item["cluster"]);
        }
    });*/    
}

function RetrieveDeployVmData( sSource, aoData, fnCallback ){
    var project_name = $("#selectDeployProject").val() == null ? "" : $("#selectDeployProject").val();
    project_name = GetFrmtStrOfMultiSelect(project_name);
    aoData.push( { "name": "project_name", "value": project_name} );
    aoData.push( { "name": "user_name", "value": userdata.name} );

    $("#tbDeployVm input[type=checkbox]").attr("checked", false); 

    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        var vmdata = [];

        if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
        } 

        for(var i = 0;i < ackdata.aaData.length;i++){ 
            var vmobj = new Object();
            vmobj.state             = parseInt(ackdata.aaData[i][1]);
            vmobj.id                = ackdata.aaData[i][2];
            vmobj.ip                = ackdata.aaData[i][8];
            vmobj.last_op           = parseInt(ackdata.aaData[i][11]);
            vmobj.last_op_result    = parseInt(ackdata.aaData[i][12]);
            vmobj.last_op_progress  = parseInt(ackdata.aaData[i][13]);
            vmobj.tc_config_version = parseInt(ackdata.aaData[i][14]);
            vmobj.cc_config_version = parseInt(ackdata.aaData[i][15]);
            vmobj.run_version       = parseInt(ackdata.aaData[i][16]);
            vmobj.workflow          = ackdata.aaData[i].workflow;
            vmobj.cluster           = ackdata.aaData[i].cluster;
        
            vmdata.push(vmobj);
        } 

        for(var i = 0;i < vmdata.length;i++){
            //var chklink = '<input type="checkbox" value=' + vmdata[i].id +' onclick="FilterOp()">';
            //ackdata.aaData[i][0] = chklink;  
            ackdata.aaData[i][0] = GetItemCheckboxWithFunc(vmdata[i].id,currMyvmDeployVmState,"onclick","FilterOp()");            
            var morelink = '<a href="javascript:void(0);"  onclick="onMoreVmClick(';
            morelink     += "'" + "idx_detailDlg" + "',";
            morelink     += "'" + ackdata.aaData[i][2] + "'";
            morelink     += ')">' + ackdata.aaData[i][2] + '</a>';
            ackdata.aaData[i][2] = morelink;
            var oplink = '';
            oplink = '<span class="spModifyDeployVm"><a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["modify"] +'" onclick="Modidy_Vm('+ "'" + vmdata[i].id + "'" + "," + "'" + global_modifydeplofyvm_acton + "',0" +')"><i class="icon-edit"></i><span></span></a>&nbsp;|&nbsp;</span>';
            oplink += '<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["vnc"] +'" onclick="RemoteControlVm('+"'"+vmdata[i].id+"'"+')"><img src="img/remote_desktop_16.png"></img><span></span></a>'; 
            oplink += '&nbsp;|&nbsp;' + '<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["clone"] +'" onclick="CloneVm('+"'"+vmdata[i].id+"'"+')"><i class="icon icon-blue icon-copy"></i><span></span></a>';
            oplink += '&nbsp;|&nbsp;' + '<a data-rel="tooltip" href="javascript:void(0);" title="USB" onclick="VmUsb('+"'"+vmdata[i].id+"'"+')"><img src="img/usb.png"></img><span></span></a>';	
            oplink += '<span class="spBackupDeployVm">&nbsp;|&nbsp;' + '<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["image_backup"] +'" onclick="VmImageBackupWindow('+"'"+vmdata[i].id+"'" + "," + "'" + vmdata[i].cluster+ "'"  +')"><img src="img/backuprestore16.png"></img><span></span></a></span>';	
            oplink += "&nbsp;|&nbsp;" + '<a href="javascript:void(0);" onclick="vm_statistics($(this))" data-rel="tooltip" data-original-title="' + page.res[page.pagelan].type.opr["statistics"] + '"><i class="icon icon-image"></i></a>';             					
            if(vmdata[i].last_op_result == last_op_result_processing){			
		        ackdata.aaData[i][10] = '';
	        }
	        else{			
                ackdata.aaData[i][10] = oplink;
	        }           
        } 
        fnCallback(ackdata);
        AvoidGotoBlankPage(oDeployVmDbt);
        BindVmData(vmdata);

        ShowVmDynamicInfo(vmdata);

        FilterOp();

        //setTimeout(showVMActionProgressInfo, 50);

        ShowLastUpdateTime($("#tbDeployVm").parent());

        $(".tooltip").hide();
        $('[rel="tooltip"],[data-rel="tooltip"]').tooltip({"placement":"bottom"});
    });
}

function showDeployingProgressInfo() {
    //var $vCheckedbox = $("#tbFictonVm > tbody").find(":checkbox");    

    if($('#tbFictonVm').length == 0){
        clearInterval(vmdeploy_updatepg_id);
        return;
    }

    ShowWorkflowProgress(oFictionVmDbt, RefreshFictionVmData);

    var vCheckedbox = $(oFictionVmDbt.fnGetNodes()).find(":checkbox");

    vCheckedbox.each(function(){
        var item = $(this).data("item");        
        var tr = $(this).parent().parent(); 
        if (oFictionVmDbt.fnIsOpen(tr[0])){
            var btn = tr.next().find(".btn").eq(0);
            btn.unbind("click").bind("click", function(){
                xmlrpc_service.tecs.vmcfg["action"](xmlrpc_session, new I8(item.id), "cancel", function(rs, ex){
                    if(rs[0] != 0){
                        ShowNotyRpcErrorInfo(rs);
                    }
                });
            });
            btn.parent().show();
        }
    });       
}

function RetrieveFictionVmData( sSource, aoData, fnCallback ){
    var project_name = $("#selectFictionProject").val() == null ? "" : $("#selectFictionProject").val();
    project_name = GetFrmtStrOfMultiSelect(project_name);

    aoData.push( { "name": "project_name", "value": project_name} );
    aoData.push( { "name": "user_name", "value": userdata.name} );

    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;

		var vmdata = [];
        if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
        }
        for(var i = 0;i < ackdata.aaData.length;i++){
		    var vmobj = new Object();
            vmobj.id       = ackdata.aaData[i][1];
            vmobj.workflow = ackdata.aaData[i]["workflow"];
            vmdata.push(vmobj);

            //var chklink  = '<input type="checkbox" value=' + '"' + ackdata.aaData[i][1] + '" >';
            //ackdata.aaData[i][0] = chklink;
            ackdata.aaData[i][0] =  GetItemCheckbox(ackdata.aaData[i][1] , currMyvmFictionVmState);
            var morelink = '<a href="javascript:void(0);"  onclick="onMoreVmClick(';
            morelink     += "'" + "idx_detailDlg" + "',";
            morelink     += "'" + ackdata.aaData[i][1] + "'";
            morelink     += ')">' + ackdata.aaData[i][1] + '</a>';
            ackdata.aaData[i][1] = morelink;
            ackdata.aaData[i][3] = getLocalTime(json.data.aaData[i][3]);
            var oplink = ''; 
            oplink = '<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["modify"] +'" onclick="Modidy_Vm('+ "'" + vmobj.id + "'" + "," + "'" + global_modifyvm_acton + "',1" +')"><i class="icon-edit"></i><span></span></a>'; 
            oplink += '&nbsp;|&nbsp;' + '<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["clone"] +'" onclick="CloneVm('+"'"+vmobj.id+"'"+')"><i class="icon icon-blue icon-copy"></i><span></span></a>';
            oplink += "&nbsp;|&nbsp;" + '<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["lock"] +'" onclick="MutexVm('+"'"+vmobj.id+"'"+')"><i class="icon icon-locked"></i><span></span></a>'; 
            oplink += "&nbsp;|&nbsp;" + '<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["affinity"] +'" onclick="AffinityVm('+"'"+vmobj.id+"'"+')"><i class="icon icon-refresh"></i><span></span></a>';            
            ackdata.aaData[i][9] = oplink;
        }
        
        fnCallback(ackdata);
        AvoidGotoBlankPage(oFictionVmDbt);
        BindFictonVmdata(vmdata);
        ClearCurSelItem(currMyvmFictionVmState);
        //setTimeout(showDeployingProgressInfo, 50);

        ShowLastUpdateTime($("#tbFictonVm").parent());

        $(".tooltip").hide();
        $('[rel="tooltip"],[data-rel="tooltip"]').tooltip({"placement":"bottom"});
    });
}

function Modidy_Vm(vid, vm_action,tabindex){
    $('[rel="tooltip"],[data-rel="tooltip"]').tooltip("hide");
    Load_Page($('#content'),"vm_config.php?action=" + vm_action + "&vid=" + vid + "&tabindex=" + tabindex + "&level=1");
}


function OnSelectDeployedVmAll(){
    if($(this).attr("checked") == "checked"){
        $("#tbDeployVm tbody input[type=checkbox]").attr("checked", true);
    }
    else{
        $("#tbDeployVm tbody input[type=checkbox]").attr("checked", false);
    }
    FilterOp();
}

function OnSelectFictionVmAll(){
    if($(this).attr("checked") == "checked"){
        $("#tbFictonVm tbody input[type=checkbox]").attr("checked", true);
    }
    else{
        $("#tbFictonVm tbody input[type=checkbox]").attr("checked", false);
    }
}

var currMyvmFictionVmState = new Object();
function RefreshFictionVmData(){
    if($('#tbFictonVm').length == 0) {
        return;
    }
    ClearCurSelItem(currMyvmFictionVmState);
    RecordSelItem(oFictionVmDbt, "tbFictonVm", currMyvmFictionVmState);  
    GotoCurrentPageOfDataTable(oFictionVmDbt);
} 

var currMyvmDeployVmState = new Object();
function RefreshDeployVmData(){
    ClearCurSelItem(currMyvmDeployVmState);
    RecordSelItem(oDeployVmDbt,"tbDeployVm",currMyvmDeployVmState);
    GotoCurrentPageOfDataTable(oDeployVmDbt);
}

function ShowFictionVmCallBack(rs,ex){
    if(rs == null || rs.length == 0){
        return;
    } 
    ShowSynRpcInfo(rs);
    RefreshFictionVmData();
}

function VmSaveAsImageCallBack(rs, ex){
    if(rs == null || rs.length == 0){
        return;
    } 
    ShowSynRpcInfo(rs);    
}

function ShowInstanceVmCallBack(rs,ex){
    if(rs == null || rs.length == 0){
        return;
    }

    if(rs[0] != 0) {
        ShowSynRpcInfo(rs);
    }

    //RefreshDeployVmData();
}

function ShowDeployVmCallBack(rs,ex){
    if(rs == null || rs.length == 0){
        return;
    } 
    if(rs[0] != 0){
        ShowSynRpcInfo(rs);
    }
    
    //RefreshDeployVmData();
    //RefreshFictionVmData();
} 

function ShowCancelDeployVmCallBack(rs,ex){
    if(rs == null || rs.length == 0){
        return;
    } 
    if(rs[0] != 0){
        ShowSynRpcInfo(rs);
    }
    
    //RefreshDeployVmData();
    //RefreshFictionVmData();
}

function action_vm(vCheckedbox,action_name){
    vCheckedbox.each(function(){
        var id = new I8($(this).attr("value"));
        if(action_name == "delete"){
            xmlrpc_service.tecs.vmcfg["action"](xmlrpc_session, id,action_name, ShowFictionVmCallBack);
        }
        else if(action_name == "deploy"){
            var rs = xmlrpc_service.tecs.vmcfg["action"](xmlrpc_session, id, action_name);
            if(rs[0] == 0){
                if(rs[1] != ""){
                    var obj = $(this).data("item");
                    obj.workflow = rs[1];
                    $(this).data("item", obj);    
                }
            }
            else {
                ShowSynRpcInfo(rs);
            }
        }
        else if(action_name == "cancel"){
            var rs = xmlrpc_service.tecs.vmcfg["action"](xmlrpc_session, id,action_name);
            if(rs[0] == 0){
                if(rs[1] != ""){
                    var obj = $(this).data("item");
                    obj.workflow = rs[1];
                    obj.cluster  = "";
                    $(this).data("item", obj);    
                }
            }
            else {
                ShowSynRpcInfo(rs);
            }            
        }
        else{
            var rs = xmlrpc_service.tecs.vmcfg["action"](xmlrpc_session, id,action_name);
            if(rs[0] == 0){
                if(rs[1] != ""){
                    var obj = $(this).data("item");
                    obj.workflow = rs[1];
                    $(this).data("item", obj);    
                }
            }
            else {
                ShowSynRpcInfo(rs);
            }            
        }
    });

    if(action_name == "delete"){
    }
    else if(action_name == "deploy"){
        showDeployingProgressInfo();
    }
    else if(action_name == "cancel"){
        showVMActionProgressInfo();
    }
    else{
        showVMActionProgressInfo();
    }
}

function RemoveVmCallback(){
    var vCheckedbox = $("#tbFictonVm>tbody").find(":checkbox:checked");
    action_vm(vCheckedbox,"delete");
}

function remove(){
    ShowBatchOpConfirmDiag($("#idx_confirmDlg"), "tbFictonVm", page.res[page.pagelan].type.err_info["delete_confirm_info"], RemoveVmCallback);
}

function deploy(){
    ClearAlertInfo();
    var vCheckedbox = $("#tbFictonVm>tbody").find(":checkbox:checked"); 
    if(vCheckedbox.size() == 0){
        ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["must_select_one_error"]);
        return;
    }
    action_vm(vCheckedbox,"deploy");
}

function cancelcallback (){
    var vCheckedbox = $("#tbDeployVm>tbody").find(":checkbox:checked");
    action_vm(vCheckedbox,"cancel");
}

function cancel(){
    ClearAlertInfo();
    var vCheckedbox = $("#tbDeployVm>tbody").find(":checkbox:checked"); 
    if(vCheckedbox.size() == 0){
        ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["must_select_one_error"]);
        return;
    }
    idx_showDlg({type:"confirm",msg:page.res[page.pagelan].type.err_info["cancel_deploy_confirm_info"],confirm:cancelcallback});
}

function poweron(){
    ClearAlertInfo();
    var syncfailed_vids = [];
    var vCheckedbox = $("#tbDeployVm>tbody").find(":checkbox:checked"); 
    if(vCheckedbox.size() == 0){
        ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["must_select_one_error"]);
        return;
    }
    //先同步-版本不一致
    vCheckedbox.each(function(){
        var item = $(this).data("item");
        var id   = new I8(item["id"]);
        var action_name = "sync";
        if(item["tc_config_version"] != item["cc_config_version"]){
            var rs = xmlrpc_service.tecs.vmcfg["action"](xmlrpc_session, id,action_name);
            if(rs[0]!=0){
                ShowSynRpcInfo(rs);
                //syncfailed_vids.push(item["id"]);
                $(this).attr("checked", false);
            }
        }
    });
    /*for(var i = 0;i < syncfailed_vids.length;i++){
        for(var j = 0;j < $("#tbDeployVm>tbody>tr").length;j++){
            var item = $("#tbDeployVm>tbody>tr").eq(j).find("td:first").find("Input").data("item");
            if(item != null && syncfailed_vids[i] == item["id"]){
                $("#tbDeployVm>tbody>tr").eq(j).find("td:first").find(":checkbox").attr("checked",false);
            }
        }
    }*/

    vCheckedbox = $("#tbDeployVm>tbody").find(":checkbox:checked");
    if(vCheckedbox.size() == 0){
        return;
    }    
    action_vm(vCheckedbox,"start");
}

function poweroff(){
    ClearAlertInfo();
    var vCheckedbox = $("#tbDeployVm>tbody").find(":checkbox:checked"); 
    if(vCheckedbox.size() == 0){
        ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["must_select_one_error"]);
        return;
    }
    action_vm(vCheckedbox,"stop");
}

function pause(){
    ClearAlertInfo();
    var vCheckedbox = $("#tbDeployVm>tbody").find(":checkbox:checked"); 
    if(vCheckedbox.size() == 0){
        ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["must_select_one_error"]);
        return;
    }
    action_vm(vCheckedbox,"pause");
}

function resume(){
    ClearAlertInfo();
    var vCheckedbox = $("#tbDeployVm>tbody").find(":checkbox:checked"); 
    if(vCheckedbox.size() == 0){
        ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["must_select_one_error"]);
        return;
    }
    action_vm(vCheckedbox,"resume");
}

function reboot(){
    ClearAlertInfo();
    var vCheckedbox = $("#tbDeployVm>tbody").find(":checkbox:checked"); 
    if(vCheckedbox.size() == 0){
        ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["must_select_one_error"]);
        return;
    }
    action_vm(vCheckedbox,"reboot");
}

function freeze(){
    ClearAlertInfo();
    var vCheckedbox = $("#tbDeployVm>tbody").find(":checkbox:checked"); 
    if(vCheckedbox.size() == 0){
        ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["must_select_one_error"]);
        return;
    }
    action_vm(vCheckedbox,"freeze");
}

function unfreeze(){
    ClearAlertInfo();
    var vCheckedbox = $("#tbDeployVm>tbody").find(":checkbox:checked"); 
    if(vCheckedbox.size() == 0){
        ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["must_select_one_error"]);
        return;
    }
    action_vm(vCheckedbox,"unfreeze");    
}

function force_reboot_callback(){
    var vCheckedbox = $("#tbDeployVm>tbody").find(":checkbox:checked"); 
    action_vm(vCheckedbox,"reset");
}

function force_reboot(){
    ClearAlertInfo();
    var vCheckedbox = $("#tbDeployVm>tbody").find(":checkbox:checked"); 
    if(vCheckedbox.size() == 0){
        ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["must_select_one_error"]);
        return;
    }
    idx_showDlg({type:"confirm",msg:page.res[page.pagelan].type.err_info["force_reboot_confirm_info"],confirm:force_reboot_callback});
}

function force_poweroff_callback(){
    var vCheckedbox = $("#tbDeployVm>tbody").find(":checkbox:checked"); 
    action_vm(vCheckedbox,"destroy");
}

function force_poweroff(){ 
    ClearAlertInfo();
    var vCheckedbox = $("#tbDeployVm>tbody").find(":checkbox:checked"); 
    if(vCheckedbox.size() == 0){
        ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["must_select_one_error"]);
        return;
    }
    idx_showDlg({type:"confirm",msg:page.res[page.pagelan].type.err_info["force_poweroff_confirm_info"],confirm:force_poweroff_callback});
}

function saveasimage(){
    ClearAlertInfo();
    var vCheckedbox = $("#tbDeployVm>tbody").find(":checkbox:checked"); 
    if(vCheckedbox.size() == 0){
        ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["must_select_one_error"]);
        return;
    }
    action_vm(vCheckedbox,"upload_img");
}

function recover_image(){
    ClearAlertInfo();
    var vCheckedbox = $("#tbDeployVm>tbody").find(":checkbox:checked"); 
    if(vCheckedbox.size() == 0){
        ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["must_select_one_error"]);
        return;
    }
    action_vm(vCheckedbox,"recover_image");    
}

function OnCreateVmClick(){
    var project_name = $("#txProject_name").val(); 
    if(!CheckIsCreateVm()){
        return;
    }
    Load_Page($('#content'),"vm_config.php?action=0&project_name=" + project_name  + "&level=1");
}

function MutexVm(vmid){
    var diag = $("#idx_confirmDlg"); 
    ShowVmMutexDiag(diag, vmid);
}

function CloneVm(vmid){
    var info = '';
    var diag = $("#idx_confirmDlg");
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    
    info += '       <div class="control-group vm_name">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["name"] + '<font color="red">*</font></label>';
    info += '           <div class="controls">';
    info += '               <input id="txVmName" type="text" maxlength="32" class="OamUi" bindattr="val" bind="vm_name">';
    info += '               <span class="help-inline">' + page.res[page.pagelan].type.err_info["text_input_error"] + '</span>';
    info += '            </div>';
    info += '       </div>';
     
    info += '       <div class="control-group vm_project">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["project"] + '</label>';
    info += '           <div class="controls">';
    info += '               <select id="slproject" class="OamUi" bindattr="val" bind="project_name"></select>';
    info += '            </div>';
    info += '       </div>'; 
     
    info += '       <div class="control-group vm_num">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["num"] + '<font color="red">*</font></label>';
    info += '           <div class="controls">';
    info += '               <input type="text" id="txVmNum" class="OamUi" bindattr="val" bind="vm_num" value="1">';
    info += '               <span class="help-inline">' + page.res[page.pagelan].type.err_info["vm_num_desc"] + '</span>';
    info += '            </div>';
    info += '       </div>';
    
    info += '   </fieldset>';
    info += '</form>';
    var header = '<i class="icon icon-blue icon-copy"></i>&nbsp;' + page.res[page.pagelan].type.opr["clone"] + "-" + vmid;
    diag.find('div.modal-header h5').html(header);
    diag.find('div.modal-body').html(info);
    z_strInput("txVmName",255); 
    z_numInput("txVmNum",1,10000);
    InitSlPorject($("#slproject"));
    var vm_data = GetCurrentVmInfo(vmid);
    if(vm_data == null){
        return;
    }    
    var vm_project_name = vm_data.project_name ;
    $("#slproject").val(vm_project_name);
    diag.modal("show"); 
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        var form_data = GetModifyForm(diag.find(".form-horizontal"));
        if(form_data["vm_name"] == ""){
            diag.find(".form-horizontal").find(".vm_name").addClass("error");
            return;
        }
        if(form_data["vm_num"] == ""){
            diag.find(".form-horizontal").find(".vm_num").addClass("error");
            return;
        }

        var ConfigInfo      = new Object();
        ConfigInfo.base_info    = GetVmBaseInfo(vm_data);
        ConfigInfo.vm_name      = form_data["vm_name"]; 
        ConfigInfo.project_name = form_data["project_name"];
        ConfigInfo.base_info.machine.reserved_backup = ConfigInfo.base_info.machine.reserved_backup;

        var AddVmData           = new Object();    
        AddVmData.cfg_info      = ConfigInfo;
        AddVmData.vm_num        = parseInt(form_data["vm_num"]);

        var rs = xmlrpc_service.tecs.vmcfg.allocate(xmlrpc_session, AddVmData);
        if(rs[0] != 0){
            ShowNotyRpcErrorInfo(rs);
            return;
        }
        RefreshFictionVmData();
        $('#vmTab a:eq(1)').tab('show');
        diag.modal("hide"); 
    });
}

function RemoteControlVm(vmid){ 
    var info = '';
    var diag = $("#idx_confirmDlg");
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';
    
    info += '       <div class="control-group remote_pwd">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["remotepwd"] + '<font color="red">*</font></label>';
    info += '           <div class="controls">';
    info += '               <input id="txinputremotepwd" type="password" maxlength="20">';
    info += '               <span class="help-inline">' + page.res[page.pagelan].type.err_info["vm_remotepwd_desc"] + '</span>';
    info += '            </div>';
    info += '       </div>';
    
    info += '   </fieldset>';
    info += '</form>';
    var header = '<img src="img/remote_desktop_16.png"></img>&nbsp;' + page.res[page.pagelan].type.opr["vnc"] + "-" + vmid;
    diag.find('div.modal-header h5').html(header);
    diag.find('div.modal-body').html(info);
    diag.modal("show"); 
    function open_remote_vm(){
        var rs = QueryVms(vmid); 
        if(rs == null || rs.length==0){
            return;
        }
        var item = rs;
        var lang = $.cookie('current_language') == null ? 'ch' :$.cookie('current_language');
        var strTargetUrl = "vm_vnc.htm";
        strTargetUrl    += "?host="+item["computationinfo"]["vnc_ip"]; 
        strTargetUrl    += "&port="+item["computationinfo"]["vnc_port"];
        strTargetUrl    += "&pwd="+$("#txinputremotepwd").val();
        strTargetUrl    += "&lang="+lang;
        strTargetUrl    += "&vid="+vmid;
        OpenWinToCenter("800","600",strTargetUrl);
        diag.modal("hide"); 
    }
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',open_remote_vm);
	$("#txinputremotepwd").unbind('keypress').bind('keypress',function(event){
	if(event.keyCode == 13){
	    open_remote_vm();
	} 
    });
}

function VmUsb(vmid){
    var vm_usb_dlg_html = 
        '<div align="right"> \
            <button class="btn"><i class="icon-refresh"></i><span id="usbRefresh"></span></button> \
        </div> \
        <table id="usb_list" class="table table-striped table-bordered table-condensed"> \
            <thead><tr> \
                  <th><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Usb.vendor_id"></OamStaticUI></th> \
                  <th><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Usb.manufacturer"></OamStaticUI></th> \
                  <th><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Usb.product_id"></OamStaticUI></th> \
                  <th><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Usb.vendor"></OamStaticUI></th> \
                  <th><OamStaticUI bindattr="text" maptype="field_name" bindvalue="Usb.state"></OamStaticUI></th> \
                  <th></th> \
            </tr></thead> <tbody></tbody> \
        </table>';

    ShowDiaglog("vm_usb_dlg", vm_usb_dlg_html, {
        init: function() {
            usbTable = $("#usb_list").dataTable({
                "oLanguage": tableLanguage[page.pagelan],
                "sDom":"t", 
                "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 4 ] }, 
                                 { "bSortable": false, "aTargets": [ 5 ] } ],                               
                "alternatively" : "destory"
            }); 
            $("#usbRefresh").text(page.res[page.pagelan].type.opr["refresh"]);
            $("#usbRefresh").parent().click(refresh_usb);            
        },
        show: function() {
            $("#usb_list").find("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });

            var h5 = $("#vm_usb_dlg").find('div.modal-header h5');
            h5.html('<img src="img/usb.png"></img>&nbsp;USB - ' + vmid); 
            query_usb_vm_id = vmid;
            refresh_usb();
        },
        close: function() {
            $("#vm_usb_dlg").modal("hide");
        }
    });        
}

function refresh_usb(){
    usbTable.fnClearTable();

    var rs = xmlrpc_service.tecs.vmcfg.query_usb_device(xmlrpc_session, new I8(query_usb_vm_id));

    if(rs[0] == 0){
        var usbDevs = rs[1];
        var usbData = [];
        for (var i = 0; i < usbDevs.length; i++) {
            var oneUsb = [];
            oneUsb[0] = usbDevs[i].vendor_id.toString(16);
            oneUsb[1] = usbDevs[i].manufacturer;
            oneUsb[2] = usbDevs[i].product_id.toString(16);
            oneUsb[3] = usbDevs[i].name;
            oneUsb[4] = get_usb_state(usbDevs[i]);
            oneUsb[5] = get_usb_op(usbDevs[i]);
            usbData.push(oneUsb);
        };
        usbTable.fnAddData(usbData);

        $(".tooltip").hide();
        $('[data-rel="tooltip"]').tooltip({"placement":"bottom"});        
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }
}

function get_usb_state(usb){
    if(usb.is_conflict == 1){
        return "<span data-rel='tooltip' data-original-title='"
               + page.res[page.pagelan].type.USB["conflict"] + "'><i class='icon icon-color icon-alert'></i></span>";
    }

    if(usb.use_vid != -1 && usb.use_vid != query_usb_vm_id){
        return "<span data-rel='tooltip' data-original-title='" + page.res[page.pagelan].type.USB["usedby_pre"] + usb.use_vid 
               + page.res[page.pagelan].type.USB["usedby_post"] + "'><i class='icon icon-color icon-locked'></i></span>";
    }

    var state_html = "";

    if(usb.use_vid == -1){
        state_html =  "<span data-rel='tooltip' data-original-title='"
               + page.res[page.pagelan].type.USB["free"] + "'><i class='icon icon-color icon-unlink'></i></span>";        
    }
    else {
        state_html =  "<span data-rel='tooltip' data-original-title='"
               + page.res[page.pagelan].type.USB["inuse"] + "'><i class='icon icon-color icon-link'></i></span>";                  
    }

    if(usb.online_states == 0){
        state_html += " | <span data-rel='tooltip' data-original-title='"
               + page.res[page.pagelan].type.USB["online"] + "'><i class='icon icon-color icon-check'></i></span>";
    }
    else if(usb.online_states == 1) {
        state_html += " | <span data-rel='tooltip' data-original-title='"
               + page.res[page.pagelan].type.USB["offline"] + "'><i class='icon icon-color icon-close'></i></span>";
    }
    else {
        state_html = "<span data-rel='tooltip' data-original-title='"
               + page.res[page.pagelan].type.USB["unknown"] + "'><i class='icon icon-color icon-help'></i></span>";
    }     

    return state_html;   
}

function get_usb_op(usb){
    if(usb.is_conflict == 1){
        return "";
    }

    if(usb.use_vid != -1 && usb.use_vid != query_usb_vm_id){
        return "";
    }

    var opHtml = "";

    if(usb.use_vid == -1){
        opHtml =  "<a href='javascript:void(0);' onclick='plug_in_usb(this)' data-rel='tooltip' data-original-title='"
               + page.res[page.pagelan].type.USB["plugin"] + "'><i class='icon icon-color icon-link'></i></a>";        
    }
    else {
        opHtml =  "<a href='javascript:void(0);' onclick='plug_out_usb(this)' data-rel='tooltip' data-original-title='"
               + page.res[page.pagelan].type.USB["plugout"] + "'><i class='icon icon-color icon-unlink'></i></a>";

        if(usb.online_states == 0) {
            opHtml +=  " | <a href='javascript:void(0);' onclick='replug_usb(this)' data-rel='tooltip' data-original-title='"
                   + page.res[page.pagelan].type.USB["re-plug"] + "'><i class='icon icon-color icon-refresh'></i></a>"; 
        }
        else if(usb.online_states == 1) {
            opHtml += ""; 
        }
        else {
            opHtml = "";
        }                        
    } 

    return opHtml;
}

function plug_in_usb(usb){
    var td = $(usb).parent();
    var tr = td.parent();
    var usb_name = tr.find("td").eq(3).text();
    var vendor_id = tr.find("td").eq(0).text();
    var product_id = tr.find("td").eq(2).text();        

    var usbparam = {cmd:"plug_in_usb", op_vid:new I8(query_usb_vm_id), 
                    vendor_id:parseInt("0x" + vendor_id), product_id:parseInt("0x" + product_id),
                    name:usb_name.toString()};
    var rs = xmlrpc_service.tecs.vmcfg.operate_usb_device(xmlrpc_session, usbparam);

    if(rs[0] != 0){
        ShowNotyRpcErrorInfo(rs);
    }
    else {
        $('[rel="tooltip"],[data-rel="tooltip"]').tooltip("hide");
        td.html('<img src="img/spinner-mini.gif"></img>');
        usb_state_timer = setInterval(refresh_usb_state, 5000);
    }
}

function plug_out_usb(usb){
    var td = $(usb).parent();
    var tr = td.parent();
    var usb_name = tr.find("td").eq(3).text();
    var vendor_id = tr.find("td").eq(0).text();
    var product_id = tr.find("td").eq(2).text();        

    var usbparam = {cmd:"plug_out_usb", op_vid:new I8(query_usb_vm_id), 
                    vendor_id:parseInt("0x" + vendor_id), product_id:parseInt("0x" + product_id), 
                    name:usb_name.toString()};
    var rs = xmlrpc_service.tecs.vmcfg.operate_usb_device(xmlrpc_session, usbparam);

    if(rs[0] != 0){
        ShowNotyRpcErrorInfo(rs);
    }
    else {
        $('[rel="tooltip"],[data-rel="tooltip"]').tooltip("hide");  
        td.html('<img src="img/spinner-mini.gif"></img>');
        usb_state_timer = setInterval(refresh_usb_state, 5000);        
    } 
}

function replug_usb(usb){
    var td = $(usb).parent();
    var tr = td.parent();
    var usb_name = tr.find("td").eq(3).text();
    var vendor_id = tr.find("td").eq(0).text();
    var product_id = tr.find("td").eq(2).text();        

    var usbparam = {cmd:"out_in_usb", op_vid:new I8(query_usb_vm_id), 
                    vendor_id:parseInt("0x" + vendor_id), product_id:parseInt("0x" + product_id), 
                    name:usb_name.toString()};    
    var rs =  xmlrpc_service.tecs.vmcfg.operate_usb_device(xmlrpc_session, usbparam);

    if(rs[0] != 0){
        ShowNotyRpcErrorInfo(rs);
    }
    else {
        $('[rel="tooltip"],[data-rel="tooltip"]').tooltip("hide");
        td.html('<img src="img/spinner-mini.gif"></img>');
        usb_state_timer = setInterval(refresh_usb_state, 5000);        
    } 
}

function refresh_usb_state(){
    refresh_usb();
    clearInterval(usb_state_timer);
}

function AffinityVm(vid){
    affinity_dlg_vid = vid;
    var vm_affinity_region_html = 
        '<div align="right">  \
            <button class="btn" id="AffinityRegionAdd"> \
                <i class="icon-plus"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="create"/></span> \
            </button> \
            <button class="btn" id="AffinityRegionDel"> \
                <i class="icon-trash"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="remove"/></span> \
            </button> \
            <button class="btn" id="AffinityRegionRefresh"> \
                <i class="icon-refresh"></i><span><OamStaticUI bindattr="text" maptype="opr" bindvalue="refresh"/></span> \
            </button> \
        </div> \
        <table id="AffinityRegionList" class="table table-striped table-bordered table-condensed"> \
            <thead> \
                <tr> \
                    <th align="left" width="25px;">' + TableHeadCheckbox() + '</th> \
                    <th align="left" width="75px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="affinity_id"/></span></th> \
                    <th align="left" width="75px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="affinity_name"/></span></th> \
                    <th align="left" width="50px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="affinity_level"/></span></th> \
                    <th align="left" width="50px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="affinity_is_forced"/></span></th> \
                    <th align="left" width="100px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="description"/></span></th> \
                    <th align="left" width="50px;"><span><OamStaticUI bindattr="text" maptype="display" bindvalue="affinity_vms"/></span></th> \
                    <th align="left" width="50px;"></th> \
                </tr> \
            </thead> \
            <tbody style="table-layout:fixed;word-break:break-all"> </tbody> \
        </table> ';

    ShowDiaglog("vm_affinity_region_dlg", vm_affinity_region_html, {
        init:function(){
            $("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            }); 

            AffinityRegionTable = $("#AffinityRegionList").dataTable({
                "sPaginationType": "bootstrap",
                "oLanguage": tableLanguage[page.pagelan],
                "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] },
                                { "bSortable": false, "aTargets": [ 1 ] },
                                 { "bSortable": false, "aTargets": [ 7] },
                                 { "bSortable": false, "aTargets": [ 3 ] },
                                 { "bSortable": false, "aTargets": [ 4 ] },
                                 { "bSortable": false, "aTargets": [ 5 ] },                                 
                                 { "bSortable": false, "aTargets": [ 6 ] }],                
                //"bServerSide": true,
                "bFilter":true
            }); 

            $("#AffinityRegionAdd").unbind("click").bind("click", AddAffinityRegion);
            $("#AffinityRegionDel").unbind("click").bind("click", DelAffinityRegion);
            $("#AffinityRegionRefresh").unbind("click").bind("click", RefreshAffinityRegion);
        },
        show:function(){
            var h5 = $("#vm_affinity_region_dlg").find('div.modal-header h5');
            h5.html('<i class="icon icon-refresh"></i>' + h5.text() + " - " + vid);   

            RefreshAffinityRegion();
        },
        close:function(){
            $("#vm_affinity_region_dlg").modal("hide");
        }
    });
}

var currAffinityRegionState = new Object();

function RefreshAffinityRegion(){
    RecordSelItem(AffinityRegionTable, "AffinityRegionList", currAffinityRegionState); 

    var params = [];
    params.push({"name":"user", "value": userdata.name});
    params.push({"name":"vid",  "value": affinity_dlg_vid});

    $.getJSON("../php/myvm_vm_affinity.php", params, function(json){
        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["11", json[1]]);
            return;
        }

        var rawdata = json[1];

        var affinityRegionData = [];
        for (var i = 0; i < rawdata.length; i++) {
            var oneAffinityRegion = [];
            oneAffinityRegion[0] = TableBodyCheckbox("value=" + rawdata[i]["id"] + CheckboxIsChecked(rawdata[i]["id"], currAffinityRegionState));
            oneAffinityRegion[1] = rawdata[i]["id"];
            oneAffinityRegion[2] = rawdata[i]["name"];
            oneAffinityRegion[3] = rawdata[i]["level"];
            oneAffinityRegion[4] = "<span value=" + rawdata[i]["is_forced"] + ">" + yes_no[rawdata[i]["is_forced"]][page.pagelan] + "</span>"; 
            oneAffinityRegion[5] = rawdata[i]["description"];
            oneAffinityRegion[6] = rawdata[i]["vms"];
            oneAffinityRegion[7] = '<a href="javascript:void(0);" onclick="ModifyAffinityRegion(this)" data-rel="tooltip" data-original-title="' + page.res[page.pagelan].type.opr["modify"] + '"><i class="icon-edit"></i></a>';;
            if(json["added"] == null){
                oneAffinityRegion[7] += ' | <a href="javascript:void(0);" onclick="VmAddtoAffinityRegion('+ rawdata[i]["id"] +')" data-rel="tooltip" data-original-title="' + page.res[page.pagelan].type.opr["addtoaffinity"] + '"><i class="icon-plus"></i></a>';
            }
            else if(json["added"] == rawdata[i]["id"]){
                oneAffinityRegion[7] += ' | <a href="javascript:void(0);" onclick="VmDelfromAffinityRegion('+ rawdata[i]["id"] +')" data-rel="tooltip" data-original-title="' + page.res[page.pagelan].type.opr["delfromaffinity"] + '"><i class="icon-minus"></i></a>';
            }
            affinityRegionData.push(oneAffinityRegion);
        };

        AffinityRegionTable.fnClearTable();
        AffinityRegionTable.fnAddData(affinityRegionData);
        ClearCurSelItem(currAffinityRegionState);  
        $(".tooltip").hide();
        $('[rel="tooltip"],[data-rel="tooltip"]').tooltip({"placement":"bottom"});        
    });    
}

function VmAddtoAffinityRegion(region_id){
    var params = new Object();
    params["affinity_region_id"] = new I8(region_id);
    params["vids"] = new Array(); params["vids"].push(new I8(affinity_dlg_vid));

    var rs = xmlrpc_service.tecs.vmcfg.affinity_region.member["add"](xmlrpc_session, params);
    if(rs[0] == 0){
        RefreshAffinityRegion();                
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }
}

function VmDelfromAffinityRegion(region_id){
    var params = new Object();
    params["affinity_region_id"] = new I8(region_id);
    params["vids"] = new Array(); params["vids"].push(new I8(affinity_dlg_vid));

    var rs = xmlrpc_service.tecs.vmcfg.affinity_region.member["delete"](xmlrpc_session, params);
    if(rs[0] == 0){
        RefreshAffinityRegion();                
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }    
}

function AddAffinityRegion(){
    var add_affinityregion_dlg_html = 
    '<form class="form-horizontal"> \
        <fieldset> \
            <div class="control-group">  \
                <label class="control-label"><span>' + page.res[page.pagelan].type.display["affinity_name"] + '</span></label> \
                <div class="controls"><input id="inAffinityRegionName" popover="affinity_name_format" type="text" maxlength="32"></input></div> \
            </div> \
            <div class="control-group">  \
                <label class="control-label"><span>' + page.res[page.pagelan].type.display["affinity_level"] + '</span></label> \
                <div class="controls"><select id="slAffinityRegionLevel" type="text" maxlength="15"></select></input></div> \
            </div> \
            <div class="control-group">  \
                <label class="control-label"><span>' + page.res[page.pagelan].type.display["affinity_is_forced"] + '</span></label> \
                <div class="controls"><input data-no-uniform="true" type="checkbox" class="iphone-toggle" id="inAffinityRegionIsForced"></div> \
            </div> \
            <div class="control-group">  \
                <label class="control-label"><span>' + page.res[page.pagelan].type.display["description"] + '</span></label> \
                <div class="controls"><textarea type="text" maxlength="128" rows="6" id="inAffinityRegionDesc"></textarea><p id="inAffinityRegionDescTip"></p></div> \
            </div>  \
        </fieldset> \
    </form>';

    $("#vm_affinity_region_dlg").modal("hide");

    ShowDiaglog("add_affinityregion_dlg", add_affinityregion_dlg_html, {
        show:function(){
            var h5 = $("#add_affinityregion_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-plus"></i>' + h5.text());   

            $("#inAffinityRegionIsForced").iphoneStyle({
                checkedLabel: yes_no[1][page.pagelan],
                uncheckedLabel: yes_no[0][page.pagelan]
            });

            $("#slAffinityRegionLevel").empty();
            $("#slAffinityRegionLevel").append('<option value="rack">rack');
            $("#slAffinityRegionLevel").append('<option value="shelf">shelf');
            $("#slAffinityRegionLevel").append('<option value="board">board');

            LimitCharLenInput($("#inAffinityRegionDesc"),$("#inAffinityRegionDescTip"));
            
            TogglePopover($("#add_affinityregion_dlg"));
        },
        ok:function(){             
            var arname       = $("#inAffinityRegionName").val();
            var arlevel      = $("#slAffinityRegionLevel").val().toString();
            var is_forced    = $("#inAffinityRegionIsForced").attr("checked")? 1:0;
            var desc         = $("#inAffinityRegionDesc").val();
            
            $(".control-group").removeClass("error");
            if(arname == ""){
                $("#inAffinityRegionName").parent().parent().addClass("error");
                return;
            }

            var rs = xmlrpc_service.tecs.vmcfg.affinity_region.allocate(xmlrpc_session, arname, desc, arlevel, parseInt(is_forced));
            if(rs[0] == 0){
                $("#add_affinityregion_dlg").modal("hide"); 
                $("#vm_affinity_region_dlg").modal("show");
                RefreshAffinityRegion();                
            }
            else {
                ShowNotyRpcErrorInfo(rs);
            }
        },
        cancel:function(){
            $("#add_affinityregion_dlg").modal("hide");
            $("#vm_affinity_region_dlg").modal("show");
        }
    });
}

function ModifyAffinityRegion(ar){
    var modify_affinityregion_dlg_html = 
    '<form class="form-horizontal"> \
        <fieldset> \
            <div class="control-group">  \
                <label class="control-label"><span>' + page.res[page.pagelan].type.display["affinity_name"] + '</span></label> \
                <div class="controls"><input id="modiAffinityRegionName" type="text" maxlength="32"></input></div> \
            </div> \
            <div class="control-group">  \
                <label class="control-label"><span>' + page.res[page.pagelan].type.display["affinity_level"] + '</span></label> \
                <div class="controls"><select id="modiAffinityRegionLevel" type="text" maxlength="15"></select></input></div> \
            </div> \
            <div class="control-group">  \
                <label class="control-label"><span>' + page.res[page.pagelan].type.display["affinity_is_forced"] + '</span></label> \
                <div class="controls"><input data-no-uniform="true" type="checkbox" class="iphone-toggle" id="modiAffinityRegionIsForced"></div> \
            </div> \
            <div class="control-group">  \
                <label class="control-label"><span>' + page.res[page.pagelan].type.display["description"] + '</span></label> \
                <div class="controls"><textarea type="text" maxlength="128" rows="6" id="modiAffinityRegionDesc"></textarea><p id="modiAffinityRegionDescTip"></p></div> \
            </div>  \
        </fieldset> \
    </form>';

    var id    = $(ar).parent().parent().find(":checkbox").eq(0).attr("value");
    var name  = $(ar).parent().parent().find("td").eq(2).text();
    var level = $(ar).parent().parent().find("td").eq(3).text();
    var is_forced = $(ar).parent().parent().find("td").eq(4).find("span").eq(0).attr("value");
    var desc  = $(ar).parent().parent().find("td").eq(5).text();

    $("#vm_affinity_region_dlg").modal("hide");

    ShowDiaglog("modify_affinityregion_dlg", modify_affinityregion_dlg_html, {
        show:function(){
            var h5 = $("#modify_affinityregion_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-plus"></i>' + h5.text());   

            $("#modiAffinityRegionLevel").empty();
            $("#modiAffinityRegionLevel").append('<option value="rack">rack');
            $("#modiAffinityRegionLevel").append('<option value="shelf">shelf');
            $("#modiAffinityRegionLevel").append('<option value="board">board');

            if(is_forced == 1){
                $("#modiAffinityRegionIsForced").attr("checked", true);
            }
            else {
                $("#modiAffinityRegionIsForced").attr("checked", false);
            }

            $("#modiAffinityRegionIsForced").iphoneStyle({
                checkedLabel: yes_no[1][page.pagelan],
                uncheckedLabel: yes_no[0][page.pagelan],
            }).change();            

            LimitCharLenInput($("#modiAffinityRegionDesc"),$("#modiAffinityRegionDescTip"));

            $("#modiAffinityRegionName").val(name);
            $("#modiAffinityRegionLevel").val(level);
            $("#modiAffinityRegionDesc").val(desc);            
        },
        ok:function(){             
            var arname       = $("#modiAffinityRegionName").val();
            var arlevel      = $("#modiAffinityRegionLevel").val().toString();
            var is_forced    = $("#modiAffinityRegionIsForced").attr("checked")? 1:0;
            var desc         = $("#modiAffinityRegionDesc").val();
            
            $(".control-group").removeClass("error");
            if(arname == ""){
                $("#modiAffinityRegionName").parent().parent().addClass("error");
                return;
            }

            var rs = xmlrpc_service.tecs.vmcfg.affinity_region.set(xmlrpc_session, new I8(id), arname, desc, arlevel, parseInt(is_forced));
            if(rs[0] == 0){
                $("#modify_affinityregion_dlg").modal("hide"); 
                $("#vm_affinity_region_dlg").modal("show");
                RefreshAffinityRegion();                
            }
            else {
                ShowNotyRpcErrorInfo(rs);
            }
        },
        cancel:function(){
            $("#modify_affinityregion_dlg").modal("hide");
            $("#vm_affinity_region_dlg").modal("show");
        }
    });    
}

function DelAffinityRegion(){
    var vCheckedbox = $(AffinityRegionTable.fnGetNodes()).find(":checkbox:checked");

    if(vCheckedbox.length > 0){
        $("#vm_affinity_region_dlg").modal("hide");

        ShowDiaglog("del_affinityregion_dlg", page.res[page.pagelan].type.err_info["delete_confirm_info"], {
            show:function(){
                var h5 = $("#del_affinityregion_dlg").find('div.modal-header h5');
                h5.html('<i class="icon-trash"></i>' + h5.text()); 
            },
            ok:function(){ 
                $("#del_affinityregion_dlg").modal("hide");
                $("#vm_affinity_region_dlg").modal("show");

                vCheckedbox.each(function(){   
                    var affinity_region_id = $(this).attr("value");

                    var rs = xmlrpc_service.tecs.vmcfg.affinity_region["delete"](xmlrpc_session, new I8(affinity_region_id));
                    if(rs[0] == 0){
                    }
                    else {
                        ShowNotyRpcErrorInfo(rs);
                    }
                });

                RefreshAffinityRegion();
            },
            cancel:function(){
                $("#del_affinityregion_dlg").modal("hide");
                $("#vm_affinity_region_dlg").modal("show");
            }
        });
    }
    else {
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]);              
    }
}

function ProcVtCallBack(rs){
    if(rs == null){
        return;
    }
    if(rs[0] != 0){
        ShowNotyRpcErrorInfo(rs);
        return;
    }
    if(rs[3].length > 0){
        $(".uservt").removeAttr("disabled");
    }
    return; 
}

//-------------  new backup--------------
/**********************************************************************
* 函数名称: VmImageBackupWindow
* 功能描述: 创建镜像备份查询对话框
* 输入参数: 
* 输出参数: 
* 返 回 值:
* 其它说明:
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013-08-26       V1.0       ts          创建
***********************************************************************/
function VmImageBackupWindow(vmid , vmcluster){
    var vm_image_backup_dlg_html = 
        '<div class= "row-fluid ">\
		    <div  class = "span12 box" >\
			    <div  class = "box-content" style = "height:180px;overflow:auto;margin-top:0px;"> \
		            <div id= "showbackup" class="" style ="position:relative"></div>\
				</div>\
			</div>\
		</div>\
		<div class = "row-fluid ">\
		    <div  class = "span12  ">\
			    <div class = "box-content well" style = "overflow:auto;margin-top:0px;">\
		            <div class = "form-horizontal span10">\
					    <fieldset>\
						    <div class="control-group" style = "margin-bottom:8px">\
		                        <label class="control-label"  style = "width :80px">'+ page.res[page.pagelan].type.field_name["name"] +' </label>\
		                        <div class="controls" style = "margin-left:100px">\
								    <p id="taName"  style ="padding-top:5px"></p>\
		                        </div>\
							</div>\
		                    <div class="control-group ">\
		                        <label class="control-label"  style = "width :80px">'+ page.res[page.pagelan].type.field_name["desc"] +'</label>\
		                        <div class="controls" style = "margin-left:100px">\
								    <textarea id="taDes" class="autogrow OamUi"  maxlength="128" rows="3"></textarea>\
									<p id="pSetDescInfo"></p>\
		                        </div>\
							</div>\
		                </fieldset>\
					</div>\
		        <div class="span2" >\
				    <div class="controls">\
					    <button id="creatBackup" class="btn">\
						    <i class="icon-plus"></i><span>'+ page.res[page.pagelan].type.opr["create"] +'</span>\
						</button>\
					</div>\
		            <div class="controls">\
					    <button id="restoreBackup" class="btn">\
						    <i class="icon-retweet"></i><span>'+ page.res[page.pagelan].type.opr["resume"] +'</span>\
						</button>\
					</div>\
		            <div class="controls">\
					    <button id="removeBackup" class="btn">\
						    <i class="icon-trash"></i><span>'+ page.res[page.pagelan].type.opr["remove"] +'</span>\
						</button>\
					</div>\
		            <div class="controls">\
					    <button id="editBackup" class="btn">\
						    <i class="icon-edit"></i><span>'+ page.res[page.pagelan].type.opr["modify"] +'</span>\
						</button>\
					</div>\
				</div>\
            </div>\
		</div>\
	</div>';

    ShowDiaglog("vm_backup_dlg", vm_image_backup_dlg_html, {
        init: function() {		
			$("#creatBackup").parent().click(VmImageBackupCreate); 	
			$("#restoreBackup").parent().click(VmImageBackupRestore); 	
			$("#removeBackup").parent().click(VmImageBackupRemove); 	
			$("#editBackup").parent().click(VmImageBackupModify); 	   		
        },
        show: function() {
            var h5 = $("#vm_backup_dlg").find('div.modal-header h5');
            h5.html('<img src="img/backuprestore16.png"/>&nbsp;' +  page.res[page.pagelan].type.opr["image_backup"]+"-" + vmid ); 			
            vm_id = vmid;
			vm_cluster = vmcluster;
            getBackupData();			
        },
        close: function() {
            $("#vm_backup_dlg").modal("hide");
			$("#showbackup").html("");
            cleanDescript();		
        }
    });        
}

/**********************************************************************
* 函数名称: getBackupData
* 功能描述: 刷新镜像备份显示
* 输入参数: 
* 输出参数: 
* 返 回 值:
* 其它说明:
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013-08-26       V1.0       ts          创建
***********************************************************************/
function getBackupData(){
    var params = [];
    params.push({"name":"vmid", "value":vm_id});
    params.push({"name":"cluster", "value": vm_cluster});
		
    $.getJSON("../php/myvm_imagebackup.php", params, function(json){
        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["11", json[1]]);
            return;
        }
        var bkData = [];
		var tmp = [];
		
        if(0 == json[1].length){
		    //清空当前显示
	        $("#showbackup").html("");
		    return;		
		}
		
        for (var i = 0; i < json[1].length; i++) {		
		    var oneRow = [];
			oneRow[0] = json[1][i][0];
			oneRow[1] = json[1][i][1];
			oneRow[2] = json[1][i][2];			
		    oneRow[3] = json[1][i][3];	
		    oneRow[4] = json[1][i][4];	
		    oneRow[5] = json[1][i][5];		
	        tmp.push(json[1][i][5]);
		    bkData[i] = oneRow;
        };
       
		var allParents = uniqueArray(tmp); 				
        var orderList = getParentOrder(bkData,allParents);	
        initNodeFunc(orderList);
		
		$("#showbackup div div").each(function(){
		    var that = $(this);
			var target = that.attr("tarvalue");				
			var time = that.find("label").attr("data");	
            var rid = that.attr("data");	
			var decs = that.attr("title");
			
			//设置备份时间显示
			that.find("label").html(time);	
			if("undefined" == time){
			    that.find("label").html(vm_id);
			}
			
			//更换在用及原镜像的图片
			if(rid == rootParent){				
			    that.find("img").attr("src","img/computer32.png");				
			}
			if(rid == activeNode){			
			    that.find("img").attr("src","img/Solaris.png");			
			}
            										
		    that.click(function(){
			/*   if(rid == rootParent){	
                    cleanDescript();					
                    $(" #showbackup div").find("div").removeClass("fc-state-highlight");				
				    return;
				}*/
                cleanDescript();							
			    $("#taDes").val(decs);					
                $("#taName").html(rid);	
                $("#taName").data("tar",target);	

                //添加选中高亮状态				
				$(" #showbackup div").find("div").removeClass("fc-state-highlight");
                that.addClass("fc-state-highlight");
				
				$("#removeBackup").removeAttr("disabled");
				$("#creatBackup").removeAttr("disabled");
				$("#editBackup").removeAttr("disabled");				
                if(rid == rootParent){
                    $("#creatBackup").attr("disabled","disabled");	
                    $("#removeBackup").attr("disabled","disabled");	
                    $("#editBackup").attr("disabled","disabled");
                    //为获取base恢复操作时的target					
                    for (var i = 0; i < orderList.length; i++) {		
	                    for(var j = 0; j<orderList[i].childrensArry.length;j++ ){		
                             $("#taName").data("tar",orderList[i].childrensArry[j].target);					
		                }
                    }						
				}				
											
			    //选中项的状态为在用，则不允许删除	
                if(rid == activeNode){				
				    $("#removeBackup").attr("disabled","disabled");				
				}
                				
			}); 
        });	  
		
    });	
}

/**********************************************************************
* 函数名称: VmImageBackupCreate
* 功能描述: 创建镜像备份对话框显示
* 输入参数: 
* 输出参数: 
* 返 回 值:
* 其它说明:
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013-08-26       V1.0       ts          创建
***********************************************************************/
function VmImageBackupCreate(){
    var info = '';
    var diag = $("#idx_confirmDlg");
    info += '<form class="form-horizontal">';
    info += '   <fieldset>';    
    info += '       <div class="control-group vm_name">';
    info += '           <label class="control-label">'+ page.res[page.pagelan].type.field_name["backup_target"] +'<font color="red">*</font></label>';
    info += '           <div class="controls">';
    info += '              <select id="txPdisktarget" class="OamUi" bindattr="val" bind="backup_target">';		
    info += '                 <option value="hda">hda</option>';	
    info += '                 <option value="hdb">hdb</option>';	
    info += '                 <option value="hdc">hdc</option>';	
    info += '                 <option value="hdd">hdd</option> </select>';		
    info += '           </div>';
    info += '       </div>';

    info += '       <div class="control-group">';
    info += '           <label class="control-label">' + page.res[page.pagelan].type.field_name["desc"] + '</label>';
    info += '           <div class="controls">';
    info += '               <textarea id="taDesc" class="autogrow OamUi" id="taDesc" maxlength="128" rows="6" bindattr="val" bind="description"></textarea><p id="pSetDescInfo"></p>';
    info += '            </div>';
    info += '       </div>'; 
	
    info += '   </fieldset>';
    info += '</form>';
	
    var header = '<i class="icon icon-blue icon-save"></i>&nbsp;' + page.res[page.pagelan].type.opr["create_backup"] + "-" + vm_id;
    diag.find('div.modal-header h5').html(header);
    diag.find('div.modal-body').html(info);	
	
    var vidGet = parseInt(vm_id);	
    diag.modal("show"); 
	
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        var frm = GetModifyForm(diag.find(".form-horizontal"));		
        var rs = xmlrpc_service.tecs.vmcfg.image_backup.create(xmlrpc_session, new I8(vidGet), frm["backup_target"], frm["description"])
        if(rs[0] != 0){
            ShowNotyRpcErrorInfo(rs);
            return;
        }
        diag.modal("hide"); 
		//显示进度
	    QueryCurProgress(page.res[page.pagelan].type.VmOpState[34],'image_backup_create');			
    });
}

/**********************************************************************
* 函数名称: VmImageBackupRemove
* 功能描述: 删除镜像备份
* 输入参数: 
* 输出参数: 
* 返 回 值:
* 其它说明:
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013-08-26       V1.0       ts          创建
***********************************************************************/
function VmImageBackupRemove(){
    var imgid = $("#taName").html();	
    if("" == imgid)	{
		ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["vm_backup_select_one"]);
		return;	
	}
	
	var vidGet = parseInt(vm_id);
    var rs = xmlrpc_service.tecs.vmcfg.image_backup['delete'](xmlrpc_session, new I8(vidGet), imgid);	
    if(rs[0] != 0){
        ShowNotyRpcErrorInfo(rs);
        return;
    }
	
    QueryCurProgress(page.res[page.pagelan].type.VmOpState[35],'image_backup_delete');	
    cleanDescript();
}

/**********************************************************************
* 函数名称: VmImageBackupRestore
* 功能描述: 镜像备份恢复
* 输入参数: 
* 输出参数: 
* 返 回 值:
* 其它说明:
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013-08-26       V1.0       ts          创建
***********************************************************************/
function VmImageBackupRestore(){
    var imgid = $("#taName").html();	
    if("" == imgid)	{
		ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["vm_backup_select_one"]);
		return;	
	}
	var target = $("#taName").data('tar'); 
	var vidGet = parseInt(vm_id);

    var rs = xmlrpc_service.tecs.vmcfg.image_backup.restore(xmlrpc_session, new I8(vidGet), imgid, target);
    if(rs[0] != 0){
        ShowNotyRpcErrorInfo(rs);
        return;
    }
    ShowNotyRpcSuccessInfo(rs);	
	getBackupData();		
}

/**********************************************************************
* 函数名称: VmImageBackupModify
* 功能描述: 修改描述
* 输入参数: 
* 输出参数: 
* 返 回 值:
* 其它说明:
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013-08-26       V1.0       ts          创建
***********************************************************************/
function VmImageBackupModify(){
    var imgid = $("#taName").html();	
    if("" == imgid)	{
		ShowBottomNotyErrorInfo(page.res[page.pagelan].type.err_info["vm_backup_select_one"]);		
		return;	
	}		
    var decEdit = $("#taDes").val();
	if(decEdit == "undefined"){	
	    decEdit = "";
	}		
    var vidGet = parseInt(vm_id);
    var rs = xmlrpc_service.tecs.vmcfg.image_backup.modify(xmlrpc_session, new I8(vidGet), imgid, decEdit);
    if(rs[0] != 0){
        ShowNotyRpcErrorInfo(rs);
        return;
    }	
	getBackupData();
}

/**********************************************************************
* 函数名称: cleanDescript
* 功能描述: 清空描述
* 输入参数: 
* 输出参数: 
* 返 回 值:
* 其它说明:
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013-08-26       V1.0       ts          创建
***********************************************************************/
function cleanDescript(){
	$("#taName").html('');
	$("#taDes").val('');		
}

/**********************************************************************
* 函数名称: QueryCurProgress
* 功能描述: 进度显示对话框
* 输入参数: 
* 输出参数: 
* 返 回 值:
* 其它说明:
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013-08-26       V1.0       ts          创建
***********************************************************************/
function QueryCurProgress(dlgTitle,workflow){
    var vm_image_backup_dlg_html = '<div id="backprog" > </div>';
    ShowDiaglog("vm_image_backup_Pro", vm_image_backup_dlg_html, {
        init: function() {	
        },
        show: function() {
            var h5 = $("#vm_image_backup_Pro").find('div.modal-header h5');
            h5.html('<img src="img/wait.gif"/>&nbsp;' + dlgTitle +"..." ); 			
            //创建进度条
            var progressbar = GetOpProcessURL("pbc_" + workflow, 0);
            $("#backprog").html(progressbar);  						
			progressBarCtrol();	
            if(bkProgress_updatepg_id == null){
    	        bkProgress_updatepg_id = setInterval(progressBarCtrol,3000);
            }			 			
        },
        close: function() {
            $("#vm_image_backup_Pro").modal("hide");
        }
    });        

}

/**********************************************************************
* 函数名称: progressBarCtrol
* 功能描述: 更新当前进度
* 输入参数: 
* 输出参数: 
* 返 回 值:
* 其它说明:
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013-08-26       V1.0       ts          创建
***********************************************************************/
function progressBarCtrol(){
    //更新虚拟机的状态
	RefreshDeployVmData();
 //   GotoCurrentPageOfDataTable(oDeployVmDbt);		
	var vCheckedbox = $("#tbDeployVm>tbody").find(":checkbox");
    vCheckedbox.each(function(){
        var item  = $(this).data("item");
		if(item["id"] == vm_id){						
            if(($("#backprog .progress").attr("fetching") == "false")&&(item["workflow"] == null)){
			    $("#vm_image_backup_Pro").modal("hide");
				clearInterval(bkProgress_updatepg_id);
				bkProgress_updatepg_id = null;
				getBackupData();
				$("#backprog").html('');
                return;
            }			
	 
            $("#backprog .progress").attr("fetching", "true");				
            if(item["workflow"] != null){	
                GetWorkflowProgress(item["workflow"], function(pi){			
                    var progress_id = "pbc_" + pi[0];
                    $("#" + progress_id).attr("fetching", "false");
                    if(pi[1] == -1){
                        UpdateProgressByID(progress_id, pi[2], pi[0]);

                    } else {
                        var rs = [];
                        if(pi[1] == 0){
                           rs[0] = 0;  rs[1] = "";
                        } else {
                           rs[0] = 2; rs[1] = "workflow " + pi[0] + " failed, reason " + pi[2];
                        } 						

			            $("#vm_image_backup_Pro").modal("hide");
				        clearInterval(bkProgress_updatepg_id);
				        bkProgress_updatepg_id = null;
				        getBackupData();
				        $("#backprog").html('');	
                        ShowSynRpcInfo(rs);							
                    } 
						
                }, item["cluster"]);										
            } 							
	    } 			
    });		
}

/**********************************************************************
* 函数名称: uniqueArray
* 功能描述: 剔除重复的父镜像
* 输入参数: 
* 输出参数: 
* 返 回 值:
* 其它说明:
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013-08-26       V1.0       ts          创建
***********************************************************************/
function uniqueArray(data){ 
	data = data || []; 
	var a = {}; 
	len = data.length; 
	for (var i=0; i<len;i++){ 
		var v = data[i]; 
		if (typeof(a[v]) == 'undefined'){ 
			a[v] = 1; 
		} 
	}; 
	data.length=0; 
	for (var i in a){ 
		data[data.length] = i; 
	} 
	return data; 
} 

/**********************************************************************
* 函数名称: getParentOrder
* 功能描述: 获取父镜像的子镜像
* 输入参数: 
* 输出参数: 
* 返 回 值:
* 其它说明:
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013-08-26       V1.0       ts          创建
***********************************************************************/
function getParentOrder(bkdata, parentlist) {
    var pcOrder = [];
	activeNode = null;
 	for(var i=0; i< parentlist.length; i++) {	
        var parentInfo = new Object();	
		
        parentInfo.pID = parentlist[i];
		parentInfo.childrensArry = [];
		parentInfo.cNum = 0;
		parentInfo.rootFlag = true;
	    for(var j=0; j< bkdata.length; j++) {	
            var childrensInfo = new Object();		
		    if(bkdata[j][5] == parentInfo.pID ){
				childrensInfo.id = bkdata[j][0];
                childrensInfo.state = bkdata[j][1];  
                childrensInfo.time = bkdata[j][2]; 	
                childrensInfo.target = bkdata[j][3]; 					
                childrensInfo.dec = bkdata[j][4]; 				
				parentInfo.childrensArry.push(childrensInfo);				
				parentInfo.cNum = parentInfo.cNum + 1;
	            if(1 == bkdata[j][1]){				
		            activeNode = bkdata[j][0];							
		        }									
			}
            if(bkdata[j][0] == parentInfo.pID){
			    parentInfo.rootFlag  = false;
			}			
		}	
		pcOrder.push(parentInfo);
	}	
	return pcOrder;
}

/**********************************************************************
* 函数名称: initNodeFunc
* 功能描述: 生成树形图
* 输入参数: 
* 输出参数: 
* 返 回 值:
* 其它说明:
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013-08-26       V1.0       ts          创建
***********************************************************************/
function initNodeFunc(orderList){
    var NodeTree = [];    
    for (var i = 0; i < orderList.length; i++) {
	    if(typeof(NodeTree[orderList[i].pID])!="object"){	
		    NodeTree[orderList[i].pID] = new OrgNode(); 
        }
		NodeTree[orderList[i].pID].Text = orderList[i].pID;	
		
	    for(var j = 0; j<orderList[i].childrensArry.length;j++ ){		
			if(typeof(NodeTree[orderList[i].childrensArry[j].id])!="object"){				
			    NodeTree[orderList[i].childrensArry[j].id] = new OrgNode(); 			
			}
			
			NodeTree[orderList[i].childrensArry[j].id].Text = orderList[i].childrensArry[j].id;				
			NodeTree[orderList[i].childrensArry[j].id].Description = orderList[i].childrensArry[j].dec;
			NodeTree[orderList[i].childrensArry[j].id].Createtime = orderList[i].childrensArry[j].time;		
			NodeTree[orderList[i].childrensArry[j].id].Target = orderList[i].childrensArry[j].target;					
			NodeTree[orderList[i].childrensArry[j].id].Link = "#";
			NodeTree[orderList[i].pID].Nodes.Add(NodeTree[orderList[i].childrensArry[j].id]); 			
		}
    }
		
    for (var i = 0; i < orderList.length; i++) {	
	    if(true == orderList[i].rootFlag){
		    rootParent = orderList[i].pID;
            break;								
		}	
    }
	
	//清空当前显示
	$("#showbackup").html("");
	
    var OrgShows=new OrgShow(NodeTree[rootParent]); 
    OrgShows.Top=0; 
    OrgShows.Left=0; 
    OrgShows.IntervalWidth=20; 
    OrgShows.IntervalHeight=30; 
    OrgShows.ShowType=2; 
    OrgShows.BoxHeight=190; 
    OrgShows.LineSize=3;  
    OrgShows.LineColor="#cccccc"; 
    OrgShows.Run(); 	
}

$(function(){
    CreateOpPermissionsTable();
    InitSelectProject($("#selectDeployProject"), userdata.name, $("#txProject_name").val(), userdata.role, false, RefreshDeployVmData);
    InitSelectProject($("#selectFictionProject"), userdata.name, $("#txProject_name").val(), userdata.role, false, RefreshFictionVmData);
    //datatable
    oDeployVmDbt = $('#tbDeployVm').dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] },{ "bSortable": false, "aTargets": [ 1 ] },{ "bSortable": false, "aTargets": [ 8 ] },{ "bSortable": false, "aTargets": [ 10 ] }], 
        "alternatively" : "destory",
        "bProcessing": false,
        "bServerSide": true,
        "sAjaxSource": "../php/myvm_deployvm.php",
        "fnServerData": RetrieveDeployVmData
    } );
    
    oFictionVmDbt = $('#tbFictonVm').dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] },{ "bSortable": false, "aTargets": [ 9 ] }], 
        "alternatively" : "destory",
        "bProcessing": false,
        "bServerSide": true,
        "sAjaxSource": "../php/myvm_fictionvm.php",
        "fnServerData": RetrieveFictionVmData
    } );
    
    if(timer_vmop_updatepg_id == null){
        timer_vmop_updatepg_id = setInterval(TimerUpDateVmDynamicInfo, 3000);//每3S更新一次状态
    }

    if(progress_updatepg_id == null){
    	progress_updatepg_id = setInterval(showVMActionProgressInfo,1000);
    }
    
    if(vmdeploy_updatepg_id == null){
    	vmdeploy_updatepg_id = setInterval(showDeployingProgressInfo,1000);
    }

    if(userdata.role != 1){
        $(".breadcrumb").hide();
    }    
    
    $('#vmTab a').click(function (e) {
        e.preventDefault(); 
        $(this).tab('show');

        if($(this).attr("href") == "#deployed_vm"){
            RefreshDeployVmData();            
            //setTimeout(showVMActionProgressInfo,500);
        }

        if($(this).attr("href") == "#fiction_vm")
        {
            RefreshFictionVmData();
            //setTimeout(showDeployingProgressInfo,500);
        }        
    });
    
    var tab_index = $("#txTabIndex").val() == "" ? "0" : $("#txTabIndex").val();
    $('#vmTab a:eq('+ tab_index +')').tab('show');

    $('.modal').modal({
        backdrop:"static",
        keyboard:false,
        "show":false
    });

    $(".uservt").attr("disabled","disabled");
    xmlrpc_service.tecs.vmtemplate.query(xmlrpc_session, new I8(0), new I8(50), 2, "", ProcVtCallBack);
    // OamUi静态显示部分初始化
    $("OamStaticUI").each(function(){
        SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
    });
    $("#SelecltDeployedVmAll").click(OnSelectDeployedVmAll);
    $("#SelecltFictionVmAll").click(OnSelectFictionVmAll);
    $("#deployedvm_create,#fictionvm_create").click(OnCreateVmClick);
    $("#fictionvm_refresh").click(function(){RefreshFictionVmData(); /*setTimeout(showDeployingProgressInfo,500);*/});
    $("#deployedvm_refresh").click(function(){RefreshDeployVmData(); /*setTimeout(showVMActionProgressInfo,500);*/});
    $("#fictionvm_remove").click(remove);
    $("#fictionvm_deploy").click(deploy);
    $("#deployedvm_canceldeploy").click(cancel);
    $("#deployedvm_poweron").click(poweron);
    $("#deployedvm_poweroff").click(poweroff);
    $("#deployedvm_pause").click(pause);
    $("#deployedvm_resume").click(resume);
    $("#deployedvm_reboot").click(reboot);
    $("#deployedvm_forcereboot").click(force_reboot);
    $("#deployedvm_forcepoweroff").click(force_poweroff);
    $("#deployedvm_saveasimage").click(saveasimage);
    $("#deployedvm_recoverimage").click(recover_image);
    $("#deployedvm_freeze").click(freeze);
    $("#deployedvm_unfreeze").click(unfreeze);        
    $('ul.breadcrumb li a').click(Switch_Vr_Page);
    $('.uservt').click(Switch_Vr_Page);
});
