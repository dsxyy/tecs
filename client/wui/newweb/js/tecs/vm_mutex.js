function ShowVmMutexDiag(diag, vid){
    var info = ''; 
    info += '<div class="container-fluid">';
    info +=     '<div class="row-fluid">';
    info +=         '<div class="box span5">';
    info +=             '<div class="box-header well">';
    info +=                 '<h3>' + page.res[page.pagelan].type.display["nomutexvmlist"] +'</h3>';
    info +=             '</div>';
    info +=             '<div class="box-content">';
    info +=                 '<select id="slnomutexvmlist" multiple class="span12" style="height:300px;"></select>';
    info +=             '</div>';
    info +=         '</div>';
                    
    info +=         '<div class="span2" style="height:300px;margin-top:160px;">';
    info +=             '<button class="btn span12" onclick="addmutex_vm();">  >  </button><br><br>';
    info +=             '<button class="btn span12" onclick="delmutex_vm();" style="margin-left:0px;">  <  </button>';
    info +=             '<input type="hidden" id="txMutex_Vid" value="' + vid + '">';
    info +=         '</div>'; 
                    
    info +=         '<div class="box span5">';
    info +=             '<div class="box-header well">';
    info +=                 '<h3>' + page.res[page.pagelan].type.display["mutexedvmlist"] +'</h3>';
    info +=             '</div>';
    info +=             '<div class="box-content">';
    info +=                 '<select id="slmutexedvmlist" multiple class="span12" style="height:300px;"></select>';
    info +=             '</div>';
    info +=         '</div>'; 
    info +=     '</div>';
    info += '</div>';
     
    var header = '<i class="icon icon-locked"></i>' + page.res[page.pagelan].type.display["mutex"] + '-' + vid;
    diag.find('div.modal-header h5').html(header);
    diag.find('div.modal-body').html(info);
    
    //获取互斥虚拟机列表
    var rs = ShowMutexRelation(vid);
    if(rs == null){
        return;
    }
    for(var i = 0;i < rs.length;i++){
        var mutexid;
        var vm = rs[i];
        if(vm["des"].val == vid){
            mutexid = vm["src"];
        }
        else{
            mutexid = vm["des"];
        }
        $("#slmutexedvmlist").append("<option value=" + mutexid + ">" + mutexid + "</option>");
    }
    //异步获取未互斥虚拟机
    xmlrpc_service.tecs.vmcfg.staticinfo.query(xmlrpc_session, new I8(0), new I8(50), "query_all", new I8(0), ShowNoMutexVmCallback);
    diag.find('#idx_confirmDlg_confirm').unbind('click').bind('click',function(){
        UpdateMutexRelation(diag, vid);
    });
    diag.modal("show");
}

function ShowNoMutexVmCallback(rs,ex){
    if(rs[0] != 0){
        ShowNotyRpcErrorInfo(rs);
        return;
    }
    var vmlist = rs[3]; 
    for(var i = 0;i < vmlist.length;i++){ 
        var vm = vmlist[i]; 
        if(vm["vid"] == $("#txMutex_Vid").val()){//不能等于自己
            continue;
        }
        var j; 
        var mutex_vmlist = $("#slmutexedvmlist").find('option');
        for(j = 0;j < mutex_vmlist.length;j++){
            var mutexvm = mutex_vmlist.eq(j);
            if(mutexvm.attr("value") == vm["vid"].val){
                break;
            }
        }
        if(j < mutex_vmlist.length){
            continue;
        }
        $("#slnomutexvmlist").append("<option value=" + vm["vid"].val + ">" + vm["vid"].val + "</option>");
    }
    if(rs[1] != -1){
        xmlrpc_service.tecs.vmcfg.staticinfo.query(xmlrpc_session, rs[1], new I8(50), "query_all", new I8(0), ShowNoMutexVmCallback);
    }
}

function ShowVmMutexDetail(idx_pos, vm_data, vm_action){
    var info = '';
    info += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
    info += '   <tbody>';
    info += '       <tr>';
    info += '           <td width="30%">' + page.res[page.pagelan].type.display["mutex"] + '</td>';
    info += '           <td>' + GetMutexVmId(vm_data["vid"]) + '</td>';
    info += '       </tr>';
    
    info += '   </tbody>';
    info += '</table>';
    
    idx_pos.html(info);
}

function addmutex_vm(){
    var sellist = $("#slnomutexvmlist").find('option:selected');
    for (var i = 0;i < sellist.length;i++){
        var selitem = sellist.eq(i);
        $("#slmutexedvmlist").append("<option value="+selitem.attr("value")+">"+selitem.text()+"</option>");
        selitem.remove();
    }
}

function delmutex_vm(){
    var sellist = $("#slmutexedvmlist").find('option:selected');
    for (var i = 0;i < sellist.length;i++){
        var selitem = sellist.eq(i);
        $("#slnomutexvmlist").append("<option value="+selitem.attr("value")+">"+selitem.text()+"</option>");
        selitem.remove();
    }
} 

function UpdateMutexRelation(diag, vid){
    var addvmlist = $("#slmutexedvmlist").find('option');
    var delvmlist = $("#slnomutexvmlist").find('option');
      
    var mutexvmlist = new Array();
    //获取互斥关系
    var rs = ShowMutexRelation(vid);
    if(rs == null){
        return;
    }
    for(var i = 0;i < rs.length;i++){
        var mutexid;
        var vm = rs[i];
        if(vm["des"].val == vid){
            mutexid = vm["src"];
        }
        else{
            mutexid = vm["des"];
        }
        mutexvmlist.push(mutexid);
    }
    
    if(addvmlist.length > 0){
        var vmlist = new Array();
        var i = 0;
        var j = 0;
        for(i = 0;i < addvmlist.length;i++){
            var selitem = addvmlist.eq(i); 
            for(j = 0;j < mutexvmlist.length;j++){
                if(mutexvmlist[j].val == selitem.attr("value")){
                    break;
                }
            }
            if(j >= mutexvmlist.length){//如果不在互斥列表中-增加 
                var mutexmap = new Object();
                mutexmap.src = new I8(vid);
                mutexmap.des = new I8(selitem.attr("value"));
                vmlist.push(mutexmap);
            }
        } 
        if(vmlist.length > 0){
            var mutexdata = new Object();
            mutexdata.type = 0;
            mutexdata.vec_vm = vmlist;
            var rs = SetMutexRelation(mutexdata);
            if(rs == null){
                return false;
            }
        }
    }
    if(delvmlist.length > 0){
        var vmlist = new Array();
        var i = 0;
        var j = 0;
        for(i = 0;i < delvmlist.length;i++){
            var selitem = delvmlist.eq(i); 
            for(j = 0;j < mutexvmlist.length;j++){
                if(mutexvmlist[j].val == selitem.attr("value")){
                    break;
                }
            } 
            if(j < mutexvmlist.length){//如果在互斥列表中-删除
                var mutexmap = new Object();
                mutexmap.src = new I8(vid);
                mutexmap.des = new I8(selitem.attr("value"));
                vmlist.push(mutexmap);
            }
        } 
        if(vmlist.length > 0){
            var mutexdata = new Object();
            mutexdata.type = 1;
            mutexdata.vec_vm = vmlist;
            var rs = SetMutexRelation(mutexdata);
            if(rs == null){
                return false;
            }
        }
    }
    diag.modal("hide");
    return true;
}

function SetMutexRelation(data){
    var rs;
    try{
        rs = xmlrpc_service.tecs.vmcfg.relation.set(xmlrpc_session,data);
        if(rs[0]!=0){
            ShowNotyRpcErrorInfo(rs);
            rs = null;
        }
    }
    catch(e)
    {
        alert(e);
        rs = null;
    }
    return rs;
}

function GetMutexVmId(vid){
    var ret = "";
    var rs = ShowMutexRelation(vid);
    if(rs == null){
        return;
    }
    
    for(var i = 0;i < rs.length;i++){
        var mutexid;
        var vm = rs[i];
        if(vm["des"].val == vid){
            mutexid = vm["src"];
        }
        else{
            mutexid = vm["des"];
        }
        ret += mutexid.toString() + ",";
    }
    if(ret.length > 0){
        ret = ret.substring(0,ret.length - 1);
    }
    return ret;
}

function ShowMutexRelation(vid){
    var rs;
    try{
        rs = xmlrpc_service.tecs.vmcfg.query_relation(xmlrpc_session,new I8(vid));
        if(rs[0]!=0){
            ShowNotyRpcErrorInfo(rs);
            rs = null;
        }
    }
    catch(e){
        alert(e);
        rs = null;
    }
    return rs[1];
}
