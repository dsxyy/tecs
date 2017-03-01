var oPortableDiskDbt;

function RetrievePortableData( sSource, aoData, fnCallback ){
    var user_name = $("#selectUser").val() == null ? "" : $("#selectUser").val();
    user_name = GetFrmtStrOfMultiSelect(user_name);	
    aoData.push( { "name": "user_name", "value": user_name} );
	
    $.getJSON( sSource, aoData, function (json) {	   
        var prodata = json.data;
        if(json.errcode != ""){
            ShowNotyQueryDBErrorInfo(json);
        }	

        for(var i = 0;i < prodata.aaData.length;i++){
			prodata.aaData[i][1] = Capacity(prodata.aaData[i][1]);

            prodata.aaData[i][6] = prodata.aaData[i][5];

            prodata.aaData[i][5] = "";
            if(prodata.aaData[i][3] == "tohost"){
                prodata.aaData[i][3] = "";
                prodata.aaData[i][4] = "";

                var rs = xmlrpc_service.tecs.portabledisk.blockinfo.get(xmlrpc_session, prodata.aaData[i]["request_id"]); 
                if(rs[0] == 0){
                    prodata.aaData[i][5] = '<strong>Type : </strong> ' + rs[1] + '<br/><strong>Target : </strong> ' 
                                            + rs[2] + '<br/><strong>Lun ID:</strong> ' + rs[3] + '<br/><strong>IP : </strong> ' + rs[4];
                }   
                else {
                    ShowNotyRpcErrorInfo(rs);
                }                
            }            
        }
					
        fnCallback(prodata);
        ShowLastUpdateTime($("#tbPotableDisk").parent());
        $('[rel="tooltip"],[data-rel="tooltip"]').tooltip({"placement":"bottom",delay: { show: 100, hide: 200 }});
    });
}

/**********************************************************************
 * 函数名称: Refresh_PortableDisk_Data
 * 功能描述: 刷新表格
 * 输入参数:
 * 输出参数:
 * 返 回 值:
 * 其它说明:
 * 修改日期        版本号     修改人       修改内容
 * -----------------------------------------------
 * 
 ***********************************************************************/
function Refresh_PortableDisk_Data(){
    oPortableDiskDbt.fnDraw();
}


$(function(){

    //支持用户过滤
    InitSelectUser($("#selectUser"), $("#txUsername").val(), Refresh_PortableDisk_Data);

    //移动盘查询表格显示，需要直接从数据库查询
    oPortableDiskDbt = $('#tbPotableDisk').dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": true, "aTargets": [ 0 ] },
		               { "bSortable": true,   "aTargets": [ 1 ] },
		               { "bSortable": false,  "aTargets": [ 2 ] },
		               { "bSortable": false,  "aTargets": [ 3 ] },
                       { "bSortable": false,  "aTargets": [ 4 ] },
                       { "bSortable": false,  "aTargets": [ 5 ] },
		               { "bSortable": true,   "aTargets": [ 6 ] }					   
					   ],
        "alternatively" : "destory",
        "bProcessing": false,
        "bServerSide": true,
        "sAjaxSource": "../php/vm_pdisk.php",
        "fnServerData": RetrievePortableData
    } );
		
    //tabs标签
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
     
    //刷新 
    $("#pdisk_refresh").click(Refresh_PortableDisk_Data); 	
	
	//点击三级导航
    $('ul.breadcrumb li a').click(Switch_Vr_Page);
});