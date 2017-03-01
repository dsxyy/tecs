function Get_MyVm_OverViewCount(){
    var sSource = "../php/myvm_overview_count.php"; 
    var aoData  = {user_name: userdata.name};
    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
        }
        for(var i = 0;i < ackdata.aaData.length;i++){
            var myvm_overview_count = ackdata.aaData[i];
            for(var j = 0;j < myvm_overview_count.length;j++){                
                $('div.myvm_overview_count a:eq(' + j + ')' + ' div:eq(1) ').html(myvm_overview_count[j]);
            }
        }
    });
}

function Get_MyVm_Status_Count(){
    var sSource = "../php/get_vm_status.php"; 
    var aoData  = {user_name: userdata.name};
    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
        }
        for(var i = 0;i < ackdata.aaData.length;i++){
            var myvm_status_count = ackdata.aaData[i];
            var data = [
                { label: page.res[page.pagelan].type.VmState[3],  data: parseInt(myvm_status_count[1])},
                { label: page.res[page.pagelan].type.VmState[4],  data: parseInt(myvm_status_count[2])},
                { label: page.res[page.pagelan].type.VmState[5],  data: parseInt(myvm_status_count[3])}, 
                { label: page.res[page.pagelan].type.VmState[2],  data: parseInt(myvm_status_count[0])},
                { label: page.res[page.pagelan].type.VmState[6],  data: parseInt(myvm_status_count[4])},
            ];
            if($("#piechart").length){
                $.plot($("#piechart"), data,
                {
                    series: {
                        pie: {
                        show: true
                        }
                    },
                    grid: {
                        hoverable: true,
                        clickable: true
                    },
                    legend: {
                        container: $("#myvm_health_info"),
                        labelFormatter: function(label, series){
                            var formatLabel;
                            var percent = 0;
                            if(!isNaN(series.percent)){
                                percent = Math.floor(series.percent);
                            }
                            formatLabel = label + ' - ' + series.data[0][1] + 
                                ' (' + percent + '%)';
                            return formatLabel;
                        }
                    }
                });
            }
            break;
        }
    });
}

function Create_Vm(){
    if(!CheckIsCreateVm()){
        return;
    }
    Load_Page($('#content'),"vm_config.php?action=0" + "&level=0");
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
        $("#aUseVt").show();
    }
    return; 
}

$(window).unbind("resize").resize(function(){
    if($("#myvmrunning_piechart").is(":visible")){     
        Get_Vm_Running_Status_Count(userdata.name, $("#myvmrunning_piechart"), $("#myvm_running_info"));
        Get_Vm_Depoly_Status_Count(userdata.name, $("#myvmdeploy_piechart"), $("#myvm_deploy_info"));
    }
});

$(function(){
    //获取工程、虚拟机、镜像、移动硬盘总数
    Get_MyVm_OverViewCount();
    Get_Vm_Running_Status_Count(userdata.name, $("#myvmrunning_piechart"), $("#myvm_running_info"));
    Get_Vm_Depoly_Status_Count(userdata.name, $("#myvmdeploy_piechart"), $("#myvm_deploy_info"));
    //tabs
    $('#tabsVrOverview a:first').tab('show');
    $('#tabsVrOverview a').click(function (e) {
      e.preventDefault();
      $(this).tab('show');
    });
    // OamUi静态显示部分初始化
    $("OamStaticUI").each(function(){
        SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
    });

    if(userdata.role != 1){
        $(".breadcrumb").hide();
    }

    // 查询是否有可用模板
    $("#aUseVt").hide();
    xmlrpc_service.tecs.vmtemplate.query(xmlrpc_session, new I8(0), new I8(50), 2, "", ProcVtCallBack);
    $('div.myvm_overview_count a').click(Switch_Vr_Page); 
    $('ul.breadcrumb li a').click(Switch_Vr_Page);
    $("#aUseVt").click(Switch_Vr_Page);
    $("#aCreateVm").click(Create_Vm);
});