function ShowVmInfoDiag(diag, vm_data, vm_action){
    var info = '';
    
    var vm_info_obj_list = new Array();
    var vm_info_obj = new Object();
    vm_info_obj.title = page.res[page.pagelan].type.display["overview"];
    vm_info_obj.img = "home16.png";
    vm_info_obj.key = "tabOverview";
    vm_info_obj.show_callback = ShowSummaryDetail;
    vm_info_obj_list.push(vm_info_obj); 
    
    vm_info_obj = new Object();
    vm_info_obj.title = "CPU";
    vm_info_obj.img = "cpu_16.png";
    vm_info_obj.key = "tabCPU";
    vm_info_obj.show_callback = ShowCPUDetail;
    vm_info_obj_list.push(vm_info_obj);  
    
    vm_info_obj = new Object();
    vm_info_obj.title = page.res[page.pagelan].type.display["memory"];
    vm_info_obj.img = "memory_16.png";
    vm_info_obj.key = "tabMemory";
    vm_info_obj.show_callback = ShowMemoryDetail;
    vm_info_obj_list.push(vm_info_obj);
    
    vm_info_obj = new Object();
    vm_info_obj.title = page.res[page.pagelan].type.field_name["image"];
    vm_info_obj.img = "image16.png";
    vm_info_obj.key = "tabImage";
    vm_info_obj.show_callback = ShowMachineImageDetail;
    vm_info_obj_list.push(vm_info_obj); 
    
    vm_info_obj = new Object(); 
    vm_info_obj.title = page.res[page.pagelan].type.display["disk"];
    vm_info_obj.img = "disk_16.png";
    vm_info_obj.key = "tabDisk";
    vm_info_obj.show_callback = ShowDisksDetail;
    vm_info_obj_list.push(vm_info_obj); 
    
    vm_info_obj = new Object(); 
    vm_info_obj.title = page.res[page.pagelan].type.display["nic"];
    vm_info_obj.img = "nic_16.png";
    vm_info_obj.key = "tabNic";
    vm_info_obj.show_callback = ShowNicsDetail;
    vm_info_obj_list.push(vm_info_obj);  
     
    vm_info_obj = new Object();
    vm_info_obj.title = page.res[page.pagelan].type.display["context"];
    vm_info_obj.img = "directory_16.png";
    vm_info_obj.key = "tabContext";
    vm_info_obj.show_callback = ShowContextsDetail;
    vm_info_obj_list.push(vm_info_obj);  
    
    vm_info_obj = new Object();
    vm_info_obj.title = page.res[page.pagelan].type.display["watch_dog"];
    vm_info_obj.img = "watch_dog_16.png";
    vm_info_obj.key = "tabWatchDog";
    vm_info_obj.show_callback = ShowWatchDogDetail;
    vm_info_obj_list.push(vm_info_obj);
    
    vm_info_obj = new Object();
    vm_info_obj.title = page.res[page.pagelan].type.display["pci_device"];
    vm_info_obj.img = "pdh_16.png";
    vm_info_obj.key = "tabPCIDevice";
    vm_info_obj.show_callback = ShowPCIDeviceDetail;
    vm_info_obj_list.push(vm_info_obj);

    if(vm_action == 2){
        vm_info_obj = new Object();
        vm_info_obj.title = page.res[page.pagelan].type.display["remote_control"];
        vm_info_obj.img = "remote_desktop_16.png";
        vm_info_obj.key = "tabRemote";
        vm_info_obj.show_callback = ShowRemoteDesktopDetail;
        vm_info_obj_list.push(vm_info_obj);
         
        vm_info_obj = new Object();
        vm_info_obj.title = page.res[page.pagelan].type.display["mutex"];
        vm_info_obj.img = "lock_16.png";
        vm_info_obj.key = "tabLock";
        vm_info_obj.show_callback = ShowVmMutexDetail;
        vm_info_obj_list.push(vm_info_obj);
    }
     
    info += '<div class="container-fluid">';
    info +=     '<div class="row-fluid">';
    info +=         '<div class="span4 vminfo-menu-span">';
    info +=             '<div class="well nav-collapse sidebar-nav">';
    info +=                 '<ul class="nav nav-tabs nav-stacked vminfo-menu">';
    for(var i = 0;i < vm_info_obj_list.length;i++){
        var vm_info_obj = vm_info_obj_list[i];
        info +=                 '<li>';
        info +=                     '<a href="#" data-value="' + vm_info_obj.key + '">';
        info +=                         '<span><img src="img/' + vm_info_obj.img + '"></img></span> ';
        info +=                         '<span class="hidden-tablet">' + vm_info_obj.title + '</span>';
        info +=                     '</a>';
        info +=                 '</li>';
    }
    info +=                 '</ul>';
    info +=             '</div>';
    info +=         '</div>';
    info +=         '<div class="span8">';
    info +=             '<div class="row-fluid">'; 
    info +=                 '<div class="span12" id="vminfo_right_content">';
    info +=                 '</div>';
    info +=             '</div>';
    info +=         '</div>';
    info +=     '</div>';
    info += '</div>';
    
    diag.find('div.modal-body').html(info);
    //animating menus on hover
    $('ul.vminfo-menu li:not(.nav-header)').hover(function(){
        $(this).animate({'margin-left':'+=5'},300);
    },
    function(){
        $(this).animate({'margin-left':'-=5'},300);
    });
    $('ul.vminfo-menu li a').click(function( event ) {
        var $item = $( this );
        var data_value = $item.attr("data-value");
        $('ul.vminfo-menu li.active').removeClass('active');
        $item.parent('li').addClass('active');
        diag.find('#vminfo_right_content').html("");
        for(var i = 0;i < vm_info_obj_list.length;i++){
            var vm_info_obj = vm_info_obj_list[i];
            if(data_value == vm_info_obj.key){  
                vm_info_obj.show_callback(diag.find('#vminfo_right_content'), vm_data, vm_action);
            }
        }
    });
    $('ul.vminfo-menu li:eq(0)').addClass("active");
    $('ul.vminfo-menu li a:eq(0)').click();
    diag.modal("show");
}
