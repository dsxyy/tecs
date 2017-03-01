/*--page begin--*/
var page = {
    pageid: "login",
    pagelan: "ch",
    init: function(){
        page.pagelan = $.cookie('current_language') == null ? 'ch' :$.cookie('current_language');
        c_changeLangue(page);
    },
    eixt: function(){
    }
};

var indexpage = page;

function form_validation(formdata){ 
    if(formdata.username.value == "" || formdata.password.value == ""){
        $(".alert").removeClass("alert-info").addClass("alert-error").html(page.res[page.pagelan].xid.html["input_login_error"]);
        return false;
    } 
    
    window.name = formdata.username.value;

    var cookieStr = $.cookie(window.name + '_session');

    if(cookieStr != null){
        var cookieInfo = cookieStr.split("_");

        var session  = cookieInfo[0];
        var username = cookieInfo[1];

        var tmp = new I8(0);
        try{
            var rs = xmlrpc_service.tecs.user.query(session, tmp, tmp, username);
            if(rs[0] == 0){
                return true;
            } 
        }
        catch(e){
            ShowErrorInfoNoClose(1, e, $(".alert"));
            return false;
        }        
    }

    //表单检查-省略
    try{
        var rs = xmlrpc_service.tecs.session.login(formdata.username.value + ":" + formdata.password.value);
        if(rs[0] == 0 || rs[0] == 23){
            //记录cookie
            var current_session = rs[1] + "_" + formdata.username.value;
            $.cookie(window.name + '_session', current_session);
            return true;
        }
        else{
            ShowErrorInfoNoClose(rs[0],rs[1],$(".alert"));
        }
    }
    catch(e){
        ShowErrorInfoNoClose(1, e, $(".alert"));
    }
    return false;
}

$(document).ready(function(){
    page.init();
    
    var current_theme = $.cookie('current_theme')==null ? 'cerulean' :$.cookie('current_theme');
    switch_theme(current_theme);
    
    //加载语言
    load_language("login");
	
    errData = LoadErrorFile();

    //disbaling some functions for Internet Explorer
    if($.browser.msie){
        $('#is-ajax').prop('checked',false);
        $('#for-is-ajax').hide();
        $('#toggle-fullscreen').hide();
        $('.login-box').find('.input-large').removeClass('span10');
    }
    $('[rel="tooltip"],[data-rel="tooltip"]').tooltip({"placement":"bottom"});
    xmlrpc_service = init_xmlrpc("");
    
    $("#username").focus();
});
		
		
