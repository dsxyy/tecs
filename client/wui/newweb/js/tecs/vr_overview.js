var timer_Update_VmOverview;

function GetVrOverviewCount(){
    var sSource = "../php/vr_overview_count.php"; 
    var aoData  = {};
    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        if(json.errcode != ""){ 
            ShowNotyQueryDBErrorInfo(json);
        }
        for(var i = 0;i < ackdata.aaData.length;i++){
            var vr_overview_count = ackdata.aaData[i];
            for(var j = 0;j < vr_overview_count.length;j++){                
                $('div.vr_overview_count a:eq(' + j + ')' + ' div:eq(1) ').html(vr_overview_count[j]);
                $('div.vr_overview_count a:eq(' + j + ')').attr('data-original-title',"");
                $('div.vr_overview_count a:eq(' + j + ')' + ' span:eq(1) ').html("");
                $('div.vr_overview_count a:eq(' + j + ')' + ' span:eq(1) ').removeClass("notification");
            }
        }
    });
}

function TimerUpDateVrOverviewCount(){
    if($('div.vr_overview_count').length == 0){
        clearInterval(timer_Update_VmOverview);
        return;
    }
    var sSource = "../php/vr_overview_count.php"; 
    var aoData  = {};
    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        if(json.errcode != ""){ 
            ShowNotyQueryDBErrorInfo(json);
        }
        for(var i = 0;i < ackdata.aaData.length;i++){
            var vr_overview_count = ackdata.aaData[i];
            for(var j = 0;j < vr_overview_count.length;j++){
                var old_vr_overview_count = $('div.vr_overview_count a:eq(' + j + ')' + ' div:eq(1)').text();
                var new_count = parseInt(vr_overview_count[j]) - parseInt(old_vr_overview_count);
                if(new_count > 0){
                    var new_title = new_count + " new records";
                    //$('div.vr_overview_count a:eq(' + j + ')').attr('data-original-title',new_title);
                    $('div.vr_overview_count a:eq(' + j + ')' + ' span:eq(1) ').html(new_count);
                    $('div.vr_overview_count a:eq(' + j + ')' + ' span:eq(1) ').removeClass("notification").addClass("notification");
                }
                else{
                    //$('div.vr_overview_count a:eq(' + j + ')').attr('data-original-title',"");
                    $('div.vr_overview_count a:eq(' + j + ')' + ' span:eq(1) ').html("");
                    $('div.vr_overview_count a:eq(' + j + ')' + ' span:eq(1) ').removeClass("notification");
                }
            }
        }
    });
}

function RefreshVrOverviewData(){
    oOverviewData.fnDraw();
}

$(window).unbind("resize").resize(function(){
    if($("#vmrunning_piechart").is(":visible")){        
        Get_Vm_Running_Status_Count("", $("#vmrunning_piechart"), $("#vrvm_running_info"));
        Get_Vm_Depoly_Status_Count("", $("#vmdeploy_piechart"), $("#vrvm_deploy_info"));
    }
});

$(function(){
    //获取工程、虚拟机、模板、镜像总数
    GetVrOverviewCount(); 
    Get_Vm_Running_Status_Count("", $("#vmrunning_piechart"), $("#vrvm_running_info"));
    Get_Vm_Depoly_Status_Count("", $("#vmdeploy_piechart"), $("#vrvm_deploy_info"));
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
    $('div.vr_overview_count a').click(Switch_Vr_Page);
    timer_Update_VmOverview = setInterval(TimerUpDateVrOverviewCount,5000);//5s更新一次状态 
    $('ul.breadcrumb li a').click(Switch_Vr_Page);
});