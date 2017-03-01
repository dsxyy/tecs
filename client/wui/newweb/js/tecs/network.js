/**
* page begin
*/
var page = {
    pageid: "network",
    pagelan: "ch",
    init: function(){
        page.pagelan = $.cookie('current_language') == null ? 'ch' :$.cookie('current_language');
        c_changeLangue(page);
        if(indexpage.pagelan != page.pagelan){         
            $('#languages a[data-value="'+page.pagelan+'"]').find('i').addClass('icon-ok');
            $('#languages a[data-value="'+indexpage.pagelan+'"]').find('i').removeClass('icon-ok');
            indexpage.pagelan = language;    
            indexpage.init();
        }
    }
}; 

var net_menu_html = 
   '<li> \
        <a href="net_overview.html"><span><img src="img/overview16.png"></img></span><span id="net_basicinfo"></span></a> \
    </li> \
    <li> \
        <a href="net_netplane.html"><span><img src="img/netplane16.png"></img></span><span id="net_netplane"></span></a> \
    </li> \
    <li> \
        <a href="net_portgroup.html"><span class="icon icon-color icon-gear"></span><span id="net_portgroup"></span></a> \
    </li> \
    <li> \
        <a href="net_switch.html"><span><img src="img/switch16.png"></img></span><span id="net_switch"></span></a> \
    </li> \
    <li> \
        <a href="net_logicnet.html"><span><img src="img/net16.png"></img></span><span id="net_logicnet"></span></a> \
    </li> \
    <li> \
        <a href="system_configuration.html"><span><img src="img/net16.png"></img></span><span id="system_configuration"></span></a> \
    </li> \
    <li> \
        <a href="net_port.html"><span><img src="img/port16.png"></img></span><span id="net_port"></span></a> \
    </li> \
    <li> \
        <a href="management_plane.html"><span><img src="img/kernel16.png"></img></span><span id="management_plane"></span></a> \
    </li>';

$(function(){
    page.init();  

    $("#net_list").append(net_menu_html);

    $('ul.main-menu li a').click(function(e){
        mainmenu_item_click(e,$(this));
    });

    $('ul.main-menu li a').eq(0).click();
    
    //animating menus on hover
    $('ul.main-menu li:not(.nav-header)').hover(function(){
        $(this).animate({'margin-left':'+=5'},300);
    },
    function(){
        $(this).animate({'margin-left':'-=5'},300);
    });    
});

function mainmenu_item_click(event,clink){
    event.preventDefault();
    $('ul.main-menu li').removeClass("active");
    $('ul.main-menu li a').each(function(){
        if($(this).attr('href') == clink.attr('href')){
           $(this).parent().addClass("active"); 
        }
    });

    Load_Page($("#right_content"), clink.attr('href'));

    c_changeLangue(page);
}

function portgroup_info_common(pg_uuid,pg_name){
    var portgroup_info_dlg_html = 
    '<div class="container-fluid">  \
        <div class="row-fluid">   \
            <div class="span4"> \
                <div class="well nav-collapse sidebar-nav"> \
                    <ul class="nav nav-tabs nav-stacked netplane-menu"> \
                        <li><a href="#" id="pgbase"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt1_baseinfo"/></a></li> \
                        <li><a href="#" id="pgIsolateInfo"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt2_isolateType"/></a></li> \
                        <li><a href="#" id="pgadv"><OamStaticUI bindattr="text" maptype="display" bindvalue="dt3_advance"/></a></li> \
                    </ul> \
                </div> \
            </div> \
            <div class="span8"> \
                <div class="row-fluid"> \
                    <div class="span12" id="pg_right_content"> </div>\
                </div> \
            </div> \
        </div> \
    </div>';
	
    ShowDiaglog("portgroup_info_dlg", portgroup_info_dlg_html, { 
        show:function(){
            $("OamStaticUI").each(function(){
                SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
            });  

            var h5 = $("#portgroup_info_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-zoom-in"></i>' + h5.text() + ' - ' + pg_name);

            var param = [];
            param.push({"name":"pg_uuid", "value":pg_uuid});            

            $.getJSON("../php/portgroup_info.php", param, function (json) {
                if(json[0] != 0){ 
                    ShowNotyRpcErrorInfo(["11", json[1]]);
                    return;
                }

                var baseinfo = '';
                baseinfo += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
                baseinfo += '   <tbody>';
                baseinfo += '       <tr>';
                baseinfo += '           <td width="30%">' + page.res[page.pagelan].type.display.dt1_name + '</td>';
                baseinfo += '           <td>' + pg_name + '</td>';
                baseinfo += '       </tr>';

                baseinfo += '       <tr>';
                baseinfo += '           <td width="30%">' + page.res[page.pagelan].type.display.dt1_netplan + '</td>';
                baseinfo += '           <td>' + json[1]["netplane_name"]  + '</td>';
                baseinfo += '       </tr>';
                
                baseinfo += '       <tr>';
                baseinfo += '           <td width="30%">' + page.res[page.pagelan].type.display.dt1_type + '</td>';
                baseinfo += '           <td>' + page.res[page.pagelan].type.PortgroupType[json[1]["pg_type"]] + '</td>';
                baseinfo += '       </tr>';
				
                baseinfo += '       <tr>';
                baseinfo += '           <td width="30%">' + page.res[page.pagelan].type.display.dt3_mtu + '</td>';
                baseinfo += '           <td>' + json[1]["mtu"] + '</td>';
                baseinfo += '       </tr>';
                baseinfo += '   </tbody>';
                baseinfo += '</table>'; 
                               
                $("#pgbase").unbind("click").bind("click", function(){
                    $("#portgroup_info_dlg").find("ul").find("li").removeClass("active");
                    $("#pgbase").parent().addClass("active");

                    $("#pg_right_content").html(baseinfo);
                });        

                var isolateType = json[1]["isolate_type"];                
                var isolateInfo = '';
                if(isolateType ==1){
            		var vxlaninfo = 'VXLAN:';
                    vxlaninfo += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
                    vxlaninfo +=' <tbody>';
                    vxlaninfo +=' <tr>';
                    if(json[1]["pg_type"] == 20){
                        vxlaninfo +=' <td>' + "segmentID" + '</td>';
                        vxlaninfo +=' <td>' + json[1]["segment"] +'</td>';
                    }
                    else if(json[1]["pg_type"] == 21){
                        vxlaninfo +=' <td>' + page.res[page.pagelan].type.display.dt2_isolate_vlan + '</td>';
                        vxlaninfo +=' <td>' + json[1]["isolate_no"] +'</td>';
                    }
                    vxlaninfo +=' </tr>';
                    vxlaninfo +=' </tbody>';
                    vxlaninfo +=' </table>';
                    isolateInfo = vxlaninfo;
                }
                else{

                    var vlanifno = 'VLAN:';
                    vlanifno += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
                    vlanifno += '   <tbody>';

                    if(json[1]["vlan_range"].length > 0){
                        vlanifno += '       <tr>';
                        vlanifno += '           <td width="30%">' + page.res[page.pagelan].type.display["VLANrange"] + '</td>';
                        vlanifno += '           <td>'; 
                        for(var i = 0;i < json[1]["vlan_range"].length;i++){ 
                            vlanifno += json[1]["vlan_range"][i][0] + ' ~ ' + json[1]["vlan_range"][i][1] + '<br />';
                        }
                        vlanifno += '           </td>';
                        vlanifno += '       </tr>';
                    }
                    
                    vlanifno += '       <tr>';
                    vlanifno += '           <td width="30%">' + page.res[page.pagelan].type.display.dt2_switch_port_mode + '</td>';
                    vlanifno += '           <td>' + page.res[page.pagelan].type.SwitchPortMode[json[1]["switchport_mode"]] + '</td>';
                    vlanifno += '       </tr>';
                    
                    if(json[1]["switchport_mode"] == 0){
                        vlanifno += '       <tr>';
                        vlanifno += '           <td width="30%">' + page.res[page.pagelan].type.display.dt2_vlan_id + '</td>';
                        vlanifno += '           <td>' + json[1]["native_vlan_num"] + '</td>';
                        vlanifno += '       </tr>';
                    }  
                    else {
                        if(json[1]["pg_type"] == 21){
                            vlanifno += '       <tr>';
                            vlanifno += '           <td width="30%">' + page.res[page.pagelan].type.display.dt2_isolate_vlan + '</td>';
                            vlanifno += '           <td>' + json[1]["isolate_no"] + '</td>';
                            vlanifno += '       </tr>';
                        }
                        else {
                            vlanifno += '       <tr>';
                            vlanifno += '           <td width="30%">' + page.res[page.pagelan].type.display.dt2_access_vlan + '</td>';
                            vlanifno += '           <td>' + json[1]["access_vlan_num"] + '</td>';
                            vlanifno += '       </tr>';
                        }
                    } 
                    if(isolateType ==2){
                            
                            vlanifno += '       <tr>';
                            vlanifno += '           <td width="30%">VXLAN:</td>';
                            vlanifno += '           <td></td>';
                            vlanifno += '       </tr>';
                            
                            vlanifno += '       <tr>';
                            vlanifno += '           <td width="30%"> All SegmentId</td>';
                            vlanifno += '           <td></td>';
                            vlanifno += '       </tr>';
                    
                    } 
                    vlanifno += '   </tbody>';
                    vlanifno += '</table>';  
                   
                    isolateInfo += vlanifno;            
                }

                $("#pgIsolateInfo").unbind("click").bind("click", function(){
                    $("#portgroup_info_dlg").find("ul").find("li").removeClass("active");
                    $("#pgIsolateInfo").parent().addClass("active");
                    $("#pg_right_content").html(isolateInfo);
                });  
                
                var advinfo = '';
                advinfo += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
                advinfo += '   <tbody>';
                advinfo += '       <tr>';
                advinfo += '           <td width="30%">' + page.res[page.pagelan].type.display.dt1_description + '</td>';
                advinfo += '           <td>' + json[1]["desc"]  + '</td>';
                advinfo += '       </tr>';                
                advinfo += '   </tbody>';
                advinfo += '</table>';                 
                $("#pgadv").unbind("click").bind("click", function(){
                    $("#portgroup_info_dlg").find("ul").find("li").removeClass("active");
                    $("#pgadv").parent().addClass("active");

                    $("#pg_right_content").html(advinfo);
                });

                $("#pgbase").trigger("click");
            });
        },
        close:function(){
            $("#portgroup_info_dlg").modal("hide");
        }   
    });
}	

