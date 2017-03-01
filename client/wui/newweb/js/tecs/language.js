function switch_language(language, page_name){
    /*--page begin--
    var switch_page = {
        pageid: page_name,
        pagelan: "ch",
        init: function(){
            switch_page.pagelan = $.cookie('current_language') == null ? 'ch' :$.cookie('current_language');
            c_changeLangue(switch_page);
        },
        eixt: function(){
        }
    };
    switch_page.init();*/
    
    indexpage.pagelan = language;    
    indexpage.init();

    page.pagelan = language;
    page.init();

    $('ul.mainnav li.active a').eq(0).click();
}

function load_language(page_name){
    //themes, change CSS with JS
    //default theme(CSS) is cerulean, change it if needed
    var current_language = $.cookie('current_language')==null ? 'ch' :$.cookie('current_language');
    switch_language(current_language, page_name);
	
    $('#languages a[data-value="'+current_language+'"]').find('i').addClass('icon-ok');
				 
    $('#languages a').click(function(e){
        e.preventDefault();
        current_language = $(this).attr('data-value');
        $.cookie('current_language',current_language,{expires:365});
        switch_language(current_language, page_name);
        $('#languages i').removeClass('icon-ok');
        $(this).find('i').addClass('icon-ok');
    });
    return current_language;
}

