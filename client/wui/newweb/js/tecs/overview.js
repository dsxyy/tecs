/*--page begin--*/
var page = {
    pageid: "overview",
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

//找对应的导航项
function get_nav_item(alink){
    var clink = null;
    var i = 0;
    for(i = 0;i < $('ul.mainnav li a').length;i++){
        var mainnavlink = $('ul.mainnav li a').eq(i);
        if(mainnavlink.attr('href') == $(alink).attr('href')){
            break;
        }
    }
    if(i >= $('ul.mainnav li a').length){
        return clink;
    }
    return $('ul.mainnav li a').eq(i);
}
$(function(){
    page.init();
    // OamUi静态显示部分初始化
    $("OamStaticUI").each(function(){
        SetOamUiData($(this).parent(), $(this), $(this).attr("bindvalue"));
    });
    $(".enterflag").click(function(e){
        var clink = get_nav_item($(this));
        if(clink != null){
            nav_item_click(e,clink);
        }
    });    
});
