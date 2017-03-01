/*--page begin--*/
var page = {
    pageid: "vm",
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
    },
    eixt: function(){
    }
}; 

function mainmenu_item_click(event,clink){
    event.preventDefault();
    Switch_Vr_Page(clink.attr('href'));
}

$(function(){
    page.init();
    $('ul.main-menu li a').click(function(e){
        mainmenu_item_click(e,$(this));
    });
    if($("#txFlag").val() == ""){
        $('ul.main-menu li a').eq(0).click();
    }
    
    //animating menus on hover
    $('ul.main-menu li:not(.nav-header)').hover(function(){
        $(this).animate({'margin-left':'+=5'},300);
    },
    function(){
        $(this).animate({'margin-left':'-=5'},300);
    });
    // OamUi��̬��ʾ���ֳ�ʼ��
    $("OamStaticUI").each(function(){
        SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
    });
});