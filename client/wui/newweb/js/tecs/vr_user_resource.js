var oVrUserResourceDbt;

function GetLink(vrcnt,urldata){
    var link;
    if(vrcnt > 0){
        link = '<a href="javascript:void(0);" onclick="Switch_Vr_Page(' + "'" + urldata + "'" +')">'+ vrcnt + '</a>';
    }
    else{
        link = vrcnt;
    }
    return link;
}

function RetrieveVrOverviewData( sSource, aoData, fnCallback ){
    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        var items = [];
        if(json.errcode != ""){ 
            ShowNotyQueryDBErrorInfo(json);
        }
        for(var i = 0;i < ackdata.aaData.length;i++){
            var item           = new Object();
            item.user_name     = ackdata.aaData[i][0];
            item.project_count = ackdata.aaData[i][1];
            item.vmcfg_count   = ackdata.aaData[i][2];
            item.vmtpl_count   = ackdata.aaData[i][3];
            item.image_count   = ackdata.aaData[i][4];
            item.pdisk_count   = ackdata.aaData[i][5];			
            items[i] = item;
        }

        for(var i = 0;i < ackdata.aaData.length;i++){
            ackdata.aaData[i][1] = GetLink(items[i]["project_count"],"vr_project.php?user_name="+items[i]["user_name"]);
            ackdata.aaData[i][2] = GetLink(items[i]["vmcfg_count"],"vr_vm.php?user_name=" + items[i]["user_name"]);
            ackdata.aaData[i][3] = GetLink(items[i]["pdisk_count"],"vr_portabledisk.php?user_name=" + items[i]["user_name"]);               
            ackdata.aaData[i][4] = GetLink(items[i]["vmtpl_count"],"vr_vt.php?user_name=" +items[i]["user_name"]);
            ackdata.aaData[i][5] = GetLink(items[i]["image_count"],"vr_image.php?user_name=" + items[i]["user_name"]);
        }
        fnCallback(ackdata);
        ShowLastUpdateTime($("#tbUserResource").parent());
    });
}

function RefreshData(){
    oVrUserResourceDbt.fnDraw();
} 

$(function(){
    //datatable
    oVrUserResourceDbt = $('.datatable').dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "alternatively" : "destory",
        "bProcessing": false,
        "bServerSide": true,
        "sAjaxSource": "../php/vr_overview.php",
        "fnServerData": RetrieveVrOverviewData
    } );
    //tabs
    $('#tabsUserresource a:first').tab('show');
    $('#tabsUserresource a').click(function (e) {
      e.preventDefault();
      $(this).tab('show');
    });
    // OamUi静态显示部分初始化
    $("OamStaticUI").each(function(){
        SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
    });
    $("#refresh").click(RefreshData);
    $('ul.breadcrumb li a').click(Switch_Vr_Page);
});
