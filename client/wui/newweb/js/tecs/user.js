var page = {
    pageid: "user",
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

function f_changeLan(){
    page.pagelan = $.cookie('current_language') == null ? 'ch' :$.cookie('current_language');
    c_changeLangue(page);
}

/**
*  main
**/
$(function(){
    page.init(); 
    userTable = $("#user_list").dataTable({
        "sPaginationType": "bootstrap",
        "oLanguage": tableLanguage[page.pagelan],
        "aoColumnDefs": [{ "bSortable": false, "aTargets": [ 0 ] } ],
        "alternatively" : "destory"
    });

    $("#userSelecltAll").click(UserSelectAll);
    $("#userRefresh").parent().click(ShowUser);
    $("#userCreate").parent().click(CreateUserDlg);
    $("#userRemove").parent().click(RemoveUser);
    $("#confirm_create").click(CreateUser);

    $('#create_user_dlg').modal({
        backdrop:"static",
        keyboard:false,
        "show":false
    });

    //f_changeLan();     

    clear_all_timer();

    $('ul.main-menu li a').click(function(e){
        mainmenu_item_click(e,$(this));
    });
    $('ul.main-menu li a').eq(0).trigger("click");    

    //animating menus on hover
    $('ul.main-menu li:not(.nav-header)').hover(function(){
        $(this).animate({'margin-left':'+=5'},300);
    },
    function(){
        $(this).animate({'margin-left':'-=5'},300);
    });     
});

function mainmenu_item_click(event,clink){
    ShowUser();
}


function UserSelectAll() {
    if($(this).attr("checked") == "checked") {
        $("#user_list tbody input[type=checkbox]").attr("checked", true);
    }
    else {
        $("#user_list tbody input[type=checkbox]").attr("checked", false);
    }
}

function CreateUserDlg() {
    $("#userName").val("");
    $("#password").val("");
    $("#confirm_pass").val("");  
    ClearAlertInfo();
    $('#create_user_dlg').modal('show');

    $("#create_user_dlg tbody input").unbind('keypress').bind('keypress',function(event){
        if(event.keyCode == 13){
            //window.event.returnValue = false;
            event.preventDefault();
            
        }
    })

}

function CreateUser() {
    if(!isStrValid($("#userName").val()) || $("#password").val() == "") {
        ShowNotyRpcErrorInfo(["invalid_param", ""]);
        return;
    }      

    var role;
    if($("#role_c_ca").attr("checked")){
        role = 1;
    }        
    else if($("#role_c_aa").attr("checked")){
        role = 2;
    }
    else if($("#role_c_a").attr("checked")){
        role = 3;      
    }

    var rs = xmlrpc_service.tecs.user.allocate(xmlrpc_session,
            $("#userName").val(), $("#password").val(), $("#confirm_pass").val(),
            role, "Default"/*$("#group").val()*/);      

    if(rs[0] == 0) {
        ClearAlertInfo();
        $('#create_user_dlg').modal('hide');
        ShowUser();
    }
    else {
        ShowNotyRpcErrorInfo(rs);
    }    
}

function RemoveUser() {
    var pageVal = page.res[page.pagelan]["xid"]["val"];    
    var vCheckedbox = $(userTable.fnGetNodes()).find(":checkbox:checked");

    var vresultSucceeded = [];
    var vresultFailed    = [];

    if(vCheckedbox.size() > 0) {
        ShowDiaglog("delete_user_dlg", pageVal["delete_user_confirm"], {
            ok: function(){
                $("#delete_user_dlg").modal("hide");
                vCheckedbox.each(function(){
                    var username = $(this).parent().parent().find("td").eq(1).text();
                    var rs = xmlrpc_service.tecs.user["delete"](xmlrpc_session, username);      

                    ShowSynRpcInfo(rs);
                });

                ShowUser();                
            }
        });
    }
    else {
        ShowNotyRpcErrorInfo(["no_operate_obj", ""]);
    }
}

function ShowUser() {
    try {
        //ClearAlertInfo();
        userTable.fnClearTable();

        var users = TecsObjList("user", 1, "" ,"");

        var userDatas = [];
        for(var i=0; i<users.length; i++)
        {
            var oneuser = users[i];
            //var namelink = '<a href="user_info.html?user='+ oneuser["username"] + '&prev=user">'
            var namelink = '<a href="#" onclick="user_info_click($(this))">' 
                            + oneuser["username"] + '</a>';

            var userData = [];
            userData[0] = '<input type="checkbox"/>';
            userData[1] = namelink;
            userData[2] = usertype[oneuser["role"]][page.pagelan];
            userData[3] = oneuser["phone"];
            userData[4] = oneuser["email"];
            userData[5] = oneuser["address"];
            userData[6] = userstatus[oneuser["enabled"]][page.pagelan];
            userData[7] = "<a href='javascript:void(0);' onclick='modify_userinfo(this)' data-rel='tooltip' data-original-title='"
                          + page.res[page.pagelan]["type"]["opr"]["modify_info"] + "'><i class='icon-info-sign'></i></a>";
            userData[7] += " | <a href='javascript:void(0);' onclick='modify_userpass(this)' data-rel='tooltip' data-original-title='"
                          + page.res[page.pagelan]["type"]["opr"]["modify_pass"] + "'><i class='icon-lock'></i></a>"; 

            userDatas[i] = userData;
        }
        userTable.fnAddData(userDatas);

        $('[data-rel="tooltip"]').tooltip({"placement":"bottom", delay:{show:100, hide:200}});

        $("#user_last_refresh_time").html(GetLastUpdateTime());
    }
    catch(e) {
        alert(e.msg);
    }
}

function user_info_click(a){
    var user_info_dlg_html = 
       '<div style="text-align:center;margin-left:-50px;"><IMG id="user_avatar" alt="" src="./img/user_green.png"></div> \
        <table> <tbody> \
            <tr> \
                <td class="td_l"><strong><span id="userInfoName"></span>:</strong></td> \
                <td class="td_r"><span id="luserInfoName"></span></td> \
            </tr> <tr> \
                <td class="td_l"><strong><span id="userInfoRole"></span>:</strong></td> \
                <td class="td_r"><span id="luserInfoRole"></span></td> \
            </tr> <tr> \
                <td class="td_l"><strong><span id="userInfoPhone"></span>:</strong></td> \
                <td class="td_r"><span id="luserInfoPhone"></span></td> \
            </tr> <tr> \
                <td class="td_l"><strong><span id="userInfoEmail"></span>:</strong></td> \
                <td class="td_r"><span id="luserInfoEmail"></span></td> \
            </tr> <tr> \
                <td class="td_l"><strong><span id="userInfoAddress"></span>:</strong></td> \
                <td class="td_r"><span id="luserInfoAddress"></span></td> \
            </tr> <tr> \
                <td class="td_l"><strong><span id="userInfoEnabled"></span>:</strong></td> \
                <td class="td_r"><span id="luserInfoEnabled"></span></td> \
            </tr> \
            </tr> <tr> \
                <td class="td_l"><strong><span id="userInfoMaxSession"></span>:</strong></td> \
                <td class="td_r"><span id="luserInfoMaxSession"></span></td> \
            </tr> \
        </tbody> </table> \
        <!--div style="text-align:center;margin-left:-10px;" id="user_modify_pane"> \
            <button class="btn"><i class="icon-info-sign"></i><span id="modify_user"></span></button> \
            <button class="btn"><i class="icon-lock"></i><span id="change_password"></span></button> \
        </div-->';

    curr_user = a.text();

    ShowDiaglog("user_info_dlg", user_info_dlg_html, {
        init:function(){
            $("#modify_user").parent().click(modify_user_click);
            $("#change_password").parent().click(change_password_click);
        },
        show:function(){
            var h5 = $("#user_info_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-zoom-in"></i>' + h5.text() + " - " + curr_user);
            refresh_user_info();
        },
        close: function(){
            $("#user_info_dlg").modal("hide");
        }
    });
}

function refresh_user_info() {
    var usersInfo = TecsObjList("user", 1, curr_user);
    for(var i=0; i<usersInfo.length; i++) {
        var oneuser = usersInfo[i];
        var role = oneuser["role"];

        $('#luserInfoName').text(oneuser["username"]);
        $('#luserInfoRole').text(usertype[role][page.pagelan]);
        $('#luserInfoPhone').text(oneuser["phone"]);
        $('#luserInfoEmail').text(oneuser["email"]);
        $('#luserInfoAddress').text(oneuser["address"]);
        $('#luserInfoEnabled').text(userstatus[oneuser["enabled"]][page.pagelan]);
        $('#luserInfoMaxSession').text(oneuser["max_session"]);

        if(role == 1){
            $("#user_avatar").attr("src", "./img/user_red.png");
        }
        else {
            $("#user_avatar").attr("src", "./img/user_green.png");
        }
    }
}

function modify_userinfo(user){
    curr_user = $(user).parent().parent().find("td").eq(1).text();
    modify_user_click();
}

function modify_user_click(){
    var modify_user_dlg_html =
       '<form class="form-horizontal"> <fieldset> \
            <div class="control-group">  \
                <label class="control-label"><span id="uu1_7"></span></label> \
                <div class="controls"><input id="userPhone" type="text"></input><label id="userPhone_readonly"></label></div> \
            </div>  \
            <div class="control-group">   \
                <label class="control-label"><span id="uu1_8"></span></label> \
                <div class="controls"><input id="userEmail" type="text"></input><label id="userEmail_readonly"></label></div> \
            </div> \
            <div class="control-group">   \
                <label class="control-label"><span id="uu1_9"></span></label> \
                <div class="controls"><input id="userAddr" type="text"></input><label id="userAddr_readonly"></label></div> \
            </div> \
            <div class="control-group">    \
                <label class="control-label"><span id="uu1_6"></span><span><font color="red">*</font></span></label> \
                <div class="controls"> \
                    <input data-no-uniform="true" type="checkbox" class="iphone-toggle" id="user_enable_toggle"> \
                </div> \
            </div> \
            <div class="control-group">   \
                <label class="control-label"><span id="uu1_10"></span></label> \
                <div class="controls"><input id="userMaxSession" type="text"></input></div> \
            </div> \
        </fieldset> </form>';

    ShowDiaglog("modify_user_dlg", modify_user_dlg_html, {
        show: function(){
            var h5 = $("#modify_user_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-info-sign"></i>' + h5.text() + " - " + curr_user);

            z_numInput("userMaxSession", 1, 999);

            var usersInfo = TecsObjList("user", 1, curr_user);
            if(usersInfo.length > 0) {
                var oneuser = usersInfo[0];

                $("#userPhone").val(oneuser["phone"]);
                $("#userEmail").val(oneuser["email"]);
                $("#userAddr").val(oneuser["address"]);
                $("#userMaxSession").val(oneuser["max_session"]);
                $("#userPhone_readonly").text(oneuser["phone"]);
                $("#userEmail_readonly").text(oneuser["email"]);
                $("#userAddr_readonly").text(oneuser["address"]);     

                if(curr_user != userdata.name){
                    $('#userPhone').hide();
                    $('#userEmail').hide();
                    $('#userAddr').hide();
                    $("#userPhone_readonly").show();
                    $("#userEmail_readonly").show();
                    $("#userAddr_readonly").show();
                }
                else {
                    $('#userPhone').show();
                    $('#userEmail').show();
                    $('#userAddr').show();
                    $("#userPhone_readonly").hide();
                    $("#userEmail_readonly").hide();
                    $("#userAddr_readonly").hide();
                }

                if(oneuser["enabled"] == 1){
                    $("#user_enable_toggle").attr("checked", true);
                }
                else {
                    $("#user_enable_toggle").attr("checked", false);
                }

                $("#user_enable_toggle").iphoneStyle({
                    duration:100,
                    checkedLabel: userstatus[1][page.pagelan],
                    uncheckedLabel: userstatus[2][page.pagelan]
                }).data("iphoneStyle").refresh();
            }
        },
        ok:function(){
            var enabled;
            if($("#user_enable_toggle").attr("checked") == "checked"){
               enabled = 1;
            }
            else {
               enabled = 2;
            }       

            var max_session = $("#userMaxSession").val();

            var rs = xmlrpc_service.tecs.user.set(xmlrpc_session, curr_user, "", "", 0, "", enabled, parseInt(max_session),
                               $("#userPhone").val(), $("#userEmail").val(), $("#userAddr").val(), "");  

            if(rs[0] == 0) {
                refresh_user_info();
                ShowUser();

                $("#modify_user_dlg").modal("hide");             
            }
            else {
                ShowNotyRpcErrorInfo(rs);
            }            
        }
    })
}

function modify_userpass(user){
    curr_user = $(user).parent().parent().find("td").eq(1).text();
    change_password_click();
}

function change_password_click(){
    var change_password_dlg_html = 
       '<table id="change_password_table"> <tbody> \
            <tr> \
                <td class="td_l"><span id="uu1_2"></span><span><font color="red">*</font></span></td> \
                <td class="td_r"><input id="userPass" maxlength="32" type="password"></td> \
            </tr> <tr>  \
                <td class="td_l"><span id="uu1_3"></span><span><font color="red">*</font></span></td> \
                <td class="td_r"><input id="confirmPass" maxlength="32" type="password"></td> \
            </tr>  \
        </tbody> </table>';

    ShowDiaglog("change_password_dlg", change_password_dlg_html, {
        show: function(){
            var h5 = $("#change_password_dlg").find('div.modal-header h5');
            h5.html('<i class="icon-lock"></i>' + h5.text() + " - " + curr_user);

            $("#userPass").val("");
            $("#confirmPass").val("");  
        },
        ok: function(){
            if($("#userPass").val() == "") {
                ShowNotyRpcErrorInfo(["invalid_param", ""]);
                return;
            }      

            var usersInfo = TecsObjList("user", 1, curr_user);
            if(usersInfo.length > 0) {
                var oneuser = usersInfo[0];

                var rs = xmlrpc_service.tecs.user.set(xmlrpc_session, curr_user, $("#userPass").val(), 
                                        $("#confirmPass").val(), 0, "", 0, oneuser["max_session"], "", "", "", "");          

                if(rs[0] == 0) {
                    $("#change_password_dlg").modal("hide");    

                    if(userdata.name == curr_user){
                        alert(relogin_notice[0][page.pagelan]);
                    }
                }
                else {
                    ShowNotyRpcErrorInfo(rs);
                }
            }
        }
    });
}