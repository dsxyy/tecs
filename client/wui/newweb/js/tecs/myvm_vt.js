var oMyVmVtDtb;

function RetrieveMyVmVtData( sSource, aoData, fnCallback ){
    var language   = page.pagelan;
    var role = userdata.role;
    $("#tbVt input[type=checkbox]").attr("checked", false);
    aoData.push( { "name": "role",  "value": role} );
    aoData.push( { "name": "user_name", "value": userdata.name} );
    aoData.push( { "name": "language",  "value": language} ); 
    $.getJSON( sSource, aoData, function (json) {
        var ackdata = json.data;
        var vts_name = [];
        if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
        }
        for(var i = 0;i < ackdata.aaData.length;i++){
            vts_name[i] = ackdata.aaData[i][0];
        }
        for(var i = 0;i < ackdata.aaData.length;i++){            
            var morelink = '<a href="javascript:void(0);"  onclick="onMoreVtClick(';
            morelink     += "'" + "idx_detailDlg" + "',";
            morelink     += "'" + vts_name[i] + "'";
            morelink     += ')">' + vts_name[i] + '</a>';
            ackdata.aaData[i][0] = morelink;
            var oplink = '<a data-rel="tooltip" href="javascript:void(0);" title="' + page.res[page.pagelan].type.opr["create_vm"] +'" onclick="OnCreateVmByVtClick('+ "'" + vts_name[i] + "'" +')"><img src="img/computer16.png"></img><span></span></a>';
            ackdata.aaData[i][5] = oplink;
        }
        fnCallback(ackdata);
        ShowLastUpdateTime($("#tbVt").parent());

        $(".tooltip").hide();
        $('[rel="tooltip"],[data-rel="tooltip"]').tooltip({"placement":"bottom"});
    });
}

function OnCreateVmByVtClick(vid){
    $('[rel="tooltip"],[data-rel="tooltip"]').tooltip("hide");
    if(!CheckIsCreateVm()){
        return;
    }
    Load_Page($('#content'),"vm_config.php?action=6&vid=" + vid + "&level=1");
}

function RefreshData(){
    oMyVmVtDtb.fnDraw();
}

$(function(){
    //datatable
    oMyVmVtDtb = $('.datatable').dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 5 ] }],
        "alternatively" : "destory",
        "bProcessing": false,
        "bServerSide": true,
        "sAjaxSource": "../php/myvm_vt.php",
        "fnServerData": RetrieveMyVmVtData
    } );
    //tabs
    $('#tabsVrvt a:first').tab('show');
    $('#tabsVrvt a').click(function (e) {
      e.preventDefault();
      $(this).tab('show');
    });
    $('.modal').modal({
        backdrop:"static",
        keyboard:false,
        "show":false
    });

    if(userdata.role != 1){
        $(".breadcrumb").hide();
    }    
    
    $("OamStaticUI").each(function(){
        SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
    });
    $("#refresh").click(RefreshData);
    $('ul.breadcrumb li a').click(Switch_Vr_Page);
});
