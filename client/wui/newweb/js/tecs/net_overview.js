$(function(){
    $('#create_netplane_link').click(function(e){
        mainmenu_item_click(e,$(this));
    });    

    $("#ipUseDetail").click(show_ip_detail);

    $("#macUseDetail").click(show_mac_detail);

    $("#vlanUseDetail").click(show_vlan_detail);
    
    $("#segmentUseDetail").click(show_segment_detail);
    
    $.getJSON("../php/net_overview.php", function(json){
        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["11", json[1]]);
            return;
        } 

        $("#netplane_count").text(json[1][0]);  
        $("#portgroup_count").text(json[1][1]);
        $("#logicnet_count").text(json[1][2]);
        $("#switch_count").text(json[1][3]);
    });

    $('#netBasicTab a:first').tab('show');
    $('#netBasicTab a').click(function (e){
        e.preventDefault();
        $(this).tab('show');

        if($(this).attr("id") == "netResNav"){
            show_res_pie();
        }      
    });    
    timer_Update_NetOverview = setInterval(show_res_pie,5000)
});

$(window).unbind("resize").resize(function(){
    if($("#ipPie").is(":visible")){
       show_res_pie();
    }
});

var timer_Update_NetOverview;
function show_res_pie(){

    if($("#netRes").is(":visible")){
        
    }
    else{
        clearInterval(timer_Update_NetOverview);
        return;
    }
    $.getJSON("../php/net_res_summary.php", function(json){
        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["11", json[1]]);
            return;                
        }            

        var mapinfo = page.res[page.pagelan]["type"]["usage"]; 
        var res = json[1];                        

        var ip_alloc = parseInt(res["ip_alloc"]);
        var ip_free  = parseInt(res["ip_total"]) - ip_alloc;
        var ipData = [ {label:mapinfo["used"], data:ip_alloc},
                       {label:mapinfo["free"], data:ip_free}];
        init_piechart("ipPie", ipData);                    

        var mac_alloc = parseInt(res["mac_alloc"]);
        var mac_free  = parseInt(res["mac_total"]) - mac_alloc;
        var macData = [ {label:mapinfo["used"], data:mac_alloc},
                        {label:mapinfo["free"], data:mac_free}];
        init_piechart("macPie", macData);                    

        var vlan_alloc = parseInt(res["vlan_alloc"]);
        var vlan_free  = parseInt(res["vlan_total"]) - vlan_alloc;
        var vlanData = [ {label:mapinfo["used"], data:vlan_alloc},
                        {label:mapinfo["free"], data:vlan_free}];
        init_piechart("vlanPie", vlanData);  
        
        var segment_alloc = parseInt(res["segment_alloc"]);
        var segment_free  = parseInt(res["segment_total"]) - segment_alloc;
        var segmentData = [ {label:mapinfo["used"], data:segment_alloc},
                        {label:mapinfo["free"], data:segment_free}];
        init_piechart("segmentPie", segmentData);        
    });    
} 

function show_segment_detail(){
    $.getJSON("../php/net_res_segment.php", function(json){
        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["11", json[1]]);
            return;
        } 

        var diag = $("#idx_detailDlg");

        var info = '';
        info += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
        info += '   <tbody>';
        info += '       <tr>';
        info += '           <td>' + 'Segment' + '</td>';
        info += '           <td>' + page.res[page.pagelan].xid.html["vid"] + '</td>';
        info += '       </tr>';

        for (var i = 0; i < json[1].length; i++) {
            info += '       <tr>';
            info += '           <td>' + json[1][i].segment_num + '</td>';
            info += '           <td>' + json[1][i].vm_id + '</td>';
            info += '       </tr>';
        };

        info += '   </tbody>';
        info += '</table>';

        var title = '<i class="icon-zoom-in"></i> <span>' + page.res[page.pagelan].xid.html["segmentUseDetail"] + '</span>'; 
        diag.find('div.modal-header h5').html(title); 
        diag.find('div.modal-body').html(info); 

        diag.modal('show');       
    });
}

function show_ip_detail(){
    $.getJSON("../php/net_res_ip.php", function(json){
        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["11", json[1]]);
            return;
        } 

        var diag = $("#idx_detailDlg");

        var info = '';
        info += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
        info += '   <tbody>';
        info += '       <tr>';
        info += '           <td>' + 'IP' + '</td>';
        info += '           <td>' + page.res[page.pagelan].xid.html["mask"] + '</td>';
        info += '           <td>' + page.res[page.pagelan].xid.html["vid"] + '</td>';
        info += '       </tr>';

        for (var i = 0; i < json[1].length; i++) {
            info += '       <tr>';
            info += '           <td>' + json[1][i].ip + '</td>';
            info += '           <td>' + json[1][i].mask + '</td>';
            info += '           <td>' + json[1][i].vid + '</td>';
            info += '       </tr>';
        };

        info += '   </tbody>';
        info += '</table>';

        var title = '<i class="icon-zoom-in"></i> <span>' + page.res[page.pagelan].xid.html["ipUseDetail"] + '</span>'; 
        diag.find('div.modal-header h5').html(title); 
        diag.find('div.modal-body').html(info); 

        diag.modal('show');       
    });
}

function show_mac_detail(){
    $.getJSON("../php/net_res_mac.php", function(json){
        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["11", json[1]]);
            return;
        } 

        var diag = $("#idx_detailDlg");

        var info = '';
        info += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
        info += '   <tbody>';
        info += '       <tr>';
        info += '           <td>' + 'MAC' + '</td>';
        info += '           <td>' + page.res[page.pagelan].xid.html["vid"] + '</td>';
        info += '       </tr>';

        for (var i = 0; i < json[1].length; i++) {
            info += '       <tr>';
            info += '           <td>' + json[1][i].mac + '</td>';
            info += '           <td>' + json[1][i].vid + '</td>';
            info += '       </tr>';
        };

        info += '   </tbody>';
        info += '</table>';

        var title = '<i class="icon-zoom-in"></i> <span>' + page.res[page.pagelan].xid.html["macUseDetail"] + '</span>'; 
        diag.find('div.modal-header h5').html(title); 
        diag.find('div.modal-body').html(info); 

        diag.modal('show');       
    });
}

function show_vlan_detail(){
    $.getJSON("../php/net_res_vlan.php", function(json){
        if(json[0] != 0){
            ShowNotyRpcErrorInfo(["11", json[1]]);
            return;
        } 

        var diag = $("#idx_detailDlg");

        var info = '';
        info += '<table class="table table-condensed" style="width:100%;word-break:break-all">';
        info += '   <tbody>';
        info += '       <tr>';
        info += '           <td>' + 'VLAN' + '</td>';
        info += '           <td>' + page.res[page.pagelan].xid.html["vid"] + '</td>';
        info += '       </tr>';

        for (var i = 0; i < json[1].length; i++) {
            info += '       <tr>';
            info += '           <td>' + json[1][i].vlan_num + '</td>';
            info += '           <td>' + json[1][i].vm_id + '</td>';
            info += '       </tr>';
        };

        info += '   </tbody>';
        info += '</table>';

        var title = '<i class="icon-zoom-in"></i> <span>' + page.res[page.pagelan].xid.html["vlanUseDetail"] + '</span>'; 
        diag.find('div.modal-header h5').html(title); 
        diag.find('div.modal-body').html(info); 

        diag.modal('show');       
    });
}

function init_piechart(id, data, extra) {
    $.plot($("#"+id), data, {
        series: { pie: { show: true } },    
        legend: { 
            container: $("#"+id).next(),
            labelFormatter: function(label, series){
                var formatLabel;

                if(isNaN(series.percent)){
                    series.percent = 0;
                }

                formatLabel = label + ' : ' + series.data[0][1] + 
                                  ' (' + Math.floor(series.percent) + '%)';                

                return formatLabel;
            }
        }
    });    
}