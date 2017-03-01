var userdata = {
    name:"",
    pwd:"",
    language:"",
    session:"",
    role: ""
};

/*--page begin--*/
var indexpage = {
    pageid: "index",
    pagelan: "ch",
    init: function(){
        indexpage.pagelan = $.cookie('current_language') == null ? 'ch' :$.cookie('current_language');
        c_changeLangue(indexpage);
    },
    eixt: function(){
    }
}; 
var page = indexpage;

var errData;
var alarmData;

function nav_item_click(event,clink){
    event.preventDefault();
    Switch_Page("nav",clink,$('#content'),clink.attr('href'));
}

function Get_UserData(){
    if(window.name == null){
        window.location.href = "./login.html";        
        return;        
    }

    var cookieStr = $.cookie(window.name + '_session');
    
    if(cookieStr == null){
        window.location.href = "./login.html";        
        return;
    }

    var cookieInfo = cookieStr.split("_");

    var session  = cookieInfo[0];
    var username = cookieInfo[1];

    var tmp = new I8(0);
    try{
        var rs = xmlrpc_service.tecs.user.query(session, tmp, tmp, username);
        if(rs[0] == 0){
            var user = rs[3][0];
            userdata.name = username;
            userdata.pwd  = "";
            userdata.session = session;
            userdata.role = parseInt(user["role"]);
        }
        else{
            window.location.href = "./login.html";
        }
    }
    catch(e){
        window.location.href = "./login.html";
    }
}

function Check_IsLogin(){
    if($.cookie(window.name + '_session') == null){
        return false;
    }
    return true;
}

function check_session()
{
    var cookieStr = $.cookie(window.name + '_session');
        
    if(cookieStr == null){
        Logout();
    }
    else {
        xmlrpc_service.tecs.user.query(userdata.session, new I8(0), new I8(1), userdata.name, check_session_callback);
    }
}

function check_session_callback(rs, ex)
{
    if(rs[0] != 0){
        Logout();
    }
}

var curr_timer_id;

function clear_all_timer(){
    clearInterval(curr_timer_id);
}

function show_license_alert () {
    var license = xmlrpc_service.tecs.system.license.query(xmlrpc_session);
    if(license[1].effective_days > 0 && license[1].remaining_days < 5){
        var license_alert = "";
        if(indexpage.pagelan == "ch"){
            license_alert = "您的TECS系统授权将于" + license[1].remaining_days + "天后到期.";
        }
        else {
            license_alert = "Your TECS license will expire after " + license[1].remaining_days + " days.";
        }
         
        license_alert = "<div class='alert alert-error license-alert'>" + license_alert + "</div>";

        $(".license-alert").remove();
        $("#content").before(license_alert);
    }    
}

function UnlockSession(){
    var diag = $("#idx_sessionLock");
    if(diag.find("#tx_userPassword").val() == ""){
        diag.find(".user_pwd .help-inline").html(GetErrStr("session_pwd_error"));
        diag.find(".user_pwd").addClass("error");
        return false;
    }
    if(diag.find("#tx_userPassword").val() == userdata.pwd){ 
        diag.modal("hide");
    }
}

var UserInfoTitle = {
    Name:{ch:"用户名", en:"Name"},
    Role:{ch:"角色", en:"Role"},
    Phone:{ch:"电话", en:"Phone"},
    Email:{ch:"邮箱", en:"Email"},
    Address:{ch:"地址", en:"Address"},
    Enabled:{ch:"状态", en:"Status"},
    MaxSession:{ch:"最大连接数", en:"Max Session"},
    ChangeInfo:{ch:"修改信息", en:"Change Information"},
    ChangePass:{ch:"修改密码", en:"Change Password"},
    Pass:{ch:"新密码", en:"New Password"},
    ConfirmPass:{ch:"确认新密码", en:"Confirm New Password"}
}

function UserInfo(){
    var login_user_info_dlg_html = 
       '<div style="text-align:center;margin-left:-50px;"><IMG id="login_user_avatar" alt="" src="./img/user_green.png"></div> \
        <table> <tbody> \
            <tr> \
                <td class="td_l"><strong>' + UserInfoTitle["Name"][page.pagelan] + ':</strong></td> \
                <td class="td_r"><span id="loginUserName"></span></td> \
            </tr> <tr> \
                <td class="td_l"><strong>' + UserInfoTitle["Role"][page.pagelan] + ':</strong></td> \
                <td class="td_r"><span id="loginUserRole"></span></td> \
            </tr> <tr> \
                <td class="td_l"><strong>' + UserInfoTitle["Phone"][page.pagelan] + ':</strong></td> \
                <td class="td_r"><span id="loginUserPhone"></span></td> \
            </tr> <tr> \
                <td class="td_l"><strong>' + UserInfoTitle["Email"][page.pagelan] + ':</strong></td> \
                <td class="td_r"><span id="loginUserEmail"></span></td> \
            </tr> <tr> \
                <td class="td_l"><strong>' + UserInfoTitle["Address"][page.pagelan] + ':</strong></td> \
                <td class="td_r"><span id="loginUserAddress"></span></td> \
            </tr> <tr style="display:none;"> \
                <td class="td_l"><strong>' + UserInfoTitle["Enabled"][page.pagelan] + ':</strong></td> \
                <td class="td_r"><span id="loginUserEnabled"></span></td> \
            </tr> \
            </tr> <tr> \
                <td class="td_l"><strong>' + UserInfoTitle["MaxSession"][page.pagelan] + ':</strong></td> \
                <td class="td_r"><span id="loginUserMaxSession"></span></td> \
            </tr> \
        </tbody> </table> \
        <div style="text-align:center;margin-left:-10px;" id="user_modify_pane"> \
            <button class="btn"><i class="icon-info-sign"></i><span id="change_infomation">' + UserInfoTitle["ChangeInfo"][page.pagelan] + '</span></button> \
            <button class="btn"><i class="icon-lock"></i><span id="change_password">' + UserInfoTitle["ChangePass"][page.pagelan] + '</span></button> \
        </div>';

    var diag = $("#idx_detailDlg"); 

    diag.find('div.modal-header h5').html(userdata.name);
    diag.find('div.modal-body').html(login_user_info_dlg_html);
    diag.modal("show");

    $("#change_infomation").parent().click(change_login_user_click);
    $("#change_password").parent().click(change_login_password_click);    

    var usersInfo = TecsObjList("user", 1, userdata.name);
    if( usersInfo.length > 0 ) {
        var oneuser = usersInfo[0];
        var role = oneuser["role"];                

        $('#loginUserName').text(oneuser["username"]);
        $('#loginUserRole').text(usertype[role][page.pagelan]);
        $('#loginUserPhone').text(oneuser["phone"]);
        $('#loginUserEmail').text(oneuser["email"]);
        $('#loginUserAddress').text(oneuser["address"]);
        $('#loginUserEnabled').text(userstatus[oneuser["enabled"]][page.pagelan]);  
        $('#loginUserMaxSession').text(oneuser["max_session"]);              

        if(role == 1){
            $("#login_user_avatar").attr("src", "./img/user_red.png");
        }
        else {
            $("#login_user_avatar").attr("src", "./img/user_green.png");
        }
    }

    $("#idx_confirmDlg_close").unbind("click").bind("click", function(){
        diag.modal("hide");
    });
}

function change_login_user_click(){
    var change_login_user_dlg_html =
       '<form class="form-horizontal"> <fieldset> \
            <div class="control-group">  \
                <label class="control-label">' + UserInfoTitle["Phone"][page.pagelan] + '</label> \
                <div class="controls"><input id="loginUserPhoneSet" type="text"></input></div> \
            </div>  \
            <div class="control-group">   \
                <label class="control-label">' + UserInfoTitle["Email"][page.pagelan] + '</label> \
                <div class="controls"><input id="loginUserEmailSet" type="text"></input></div> \
            </div> \
            <div class="control-group">   \
                <label class="control-label">' + UserInfoTitle["Address"][page.pagelan] + '</label> \
                <div class="controls"><input id="loginUserAddrSet" type="text"></input></div> \
            </div> \
            <div class="control-group">   \
                <label class="control-label">' + UserInfoTitle["MaxSession"][page.pagelan] + '</label> \
                <div class="controls"><input id="loginUserMaxSessionSet" type="text"></input></div> \
            </div> \
        </fieldset> </form>';

    var diag = $("#idx_confirmDlg"); 

    diag.find('div.modal-header h5').html(userdata.name);
    diag.find('div.modal-body').html(change_login_user_dlg_html);
    diag.modal("show");

    $("#idx_detailDlg").modal("hide");

    z_numInput("loginUserMaxSessionSet", 1, 999);

    $("#loginUserPhoneSet").val($('#loginUserPhone').text());
    $("#loginUserEmailSet").val($('#loginUserEmail').text());
    $("#loginUserAddrSet").val($('#loginUserAddress').text());
    $("#loginUserMaxSessionSet").val($('#loginUserMaxSession').text());

    $("#idx_confirmDlg_confirm").unbind("click").bind("click", function(){
        var rs = xmlrpc_service.tecs.user.set(xmlrpc_session, userdata.name, "", "", 0, "", 0, parseInt($("#loginUserMaxSessionSet").val()),
                                   $("#loginUserPhoneSet").val(), $("#loginUserEmailSet").val(), $("#loginUserAddrSet").val(), "");          

        if(rs[0] == 0) { 
            diag.modal("hide");    
            UserInfo();            
        }
        else {
            ShowNotyRpcErrorInfo(rs);
        }   
    });

    $("#idx_confirmDlg_cancel").unbind("click").bind("click", function(){
        diag.modal("hide");
        $("#idx_confirmDlg_cancel").unbind("click");        
        UserInfo();
    });
}

function change_login_password_click(){
    var change_login_password_dlg_html = 
       '<table> <tbody> \
            <tr> \
                <td class="td_l">' + UserInfoTitle["Pass"][page.pagelan] + '<span><font color="red">*</font></span></td> \
                <td class="td_r"><input id="loginUserPass" maxlength="32" type="password"></td> \
            </tr> \
            <tr>  \
                <td class="td_l">' + UserInfoTitle["ConfirmPass"][page.pagelan] + '<span><font color="red">*</font></span></td> \
                <td class="td_r"><input id="loginUserConfirmPass" maxlength="32" type="password"></td> \
            </tr>  \
        </tbody> </table>';

    var diag = $("#idx_confirmDlg"); 

    diag.find('div.modal-header h5').html(userdata.name);
    diag.find('div.modal-body').html(change_login_password_dlg_html);
    diag.modal("show");

    $("#idx_detailDlg").modal("hide");

    $("#loginUserPass").val("");
    $("#loginUserConfirmPass").val("");  

    $("#idx_confirmDlg_confirm").unbind("click").bind("click", function(){
        if($("#loginUserPass").val() == "") {
            ShowNotyRpcErrorInfo(["invalid_param", ""]);
            return;
        }

        var usersInfo = TecsObjList("user", 1, userdata.name);
        if(usersInfo.length > 0) {
            var oneuser = usersInfo[0];

            var rs = xmlrpc_service.tecs.user.set(xmlrpc_session, userdata.name, $("#loginUserPass").val(), 
                                                  $("#loginUserConfirmPass").val(), 0, "", 0, oneuser["max_session"], "", "", "", "");                

            if(rs[0] == 0) { 
                diag.modal("hide");    
                Logout();            
            }
            else {
                ShowNotyRpcErrorInfo(rs);
            }   
        }
    });

    $("#idx_confirmDlg_cancel").unbind("click").bind("click", function(){
        diag.modal("hide");
        $("#idx_confirmDlg_cancel").unbind("click");
        UserInfo();
    });
}

function Logout(){
    xmlrpc_service.tecs.session.logout(xmlrpc_session);
    window.location.href = "./login.html";
}

function LoadAlarmFile(){
    var spath = "./res/alarm.res";
    return Load_ResFile(spath);
}

$(function(){
    //加载资源文件
    errData = LoadErrorFile();
    alarmData = LoadAlarmFile();

    indexpage.init();
    //加载主题
    load_theme();
    //加载语言
    userdata.language = load_language("index");
    $('.modal').modal({
        backdrop:"static",
        keyboard:false,
        "show":false
    });
    
    $('ul.nav li a').click(function(e){
        clear_all_timer();
        show_license_alert();
        nav_item_click(e,$(this));
    });
    
    xmlrpc_service = init_xmlrpc("");

    //获取用户信息
    Get_UserData();
    if(!Check_IsLogin()){
        window.location.href="./login.html";
        return;
    }
    $("#loginUsername").html(" " + userdata.name);
    $("#txLockUsername").val(userdata.name);
    //管理员
    if(userdata.role == 1){
        $('ul.nav li a').eq(0).trigger("click");
    }
    else{
        $('ul.nav li').hide();
        $('ul.nav li:last a').eq(0).trigger("click");
    }
    $("#tx_userPassword").bind('keydown', function (e) {
        var key = e.which;
        if (key == 13) {
            e.preventDefault();
            UnlockSession();
        }
    });

    setInterval(check_session, 30*1000);

    $("#aUserInfo").click(UserInfo);
    $("#aLogout").click(Logout); 
    $("#unlock").click(UnlockSession);
});
