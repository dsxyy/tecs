var isIE = /*@cc_on!@*/ false;
var __de = document.documentElement;

/* jquery ui 表格 */
(function ($) {
    $.fn.styleTable = function (options) {
        var defaults = {
            css: 'styleTable'
        };
        options = $.extend(defaults, options);

        return this.each(function () {

            input = $(this);
            input.addClass(options.css);

            input.find("tr").live('mouseover mouseout', function (event) {
                if (event.type == 'mouseover') {
                    $(this).children("td").addClass("ui-state-hover");
                } else {
                    $(this).children("td").removeClass("ui-state-hover");
                }
            });

            input.find("th").addClass("ui-state-default");
            input.find("td").addClass("ui-widget-content");

            input.find("tr").each(function () {
                $(this).children("td:not(:first)").addClass("first");
                $(this).children("th:not(:first)").addClass("first");
            });
        });
    };
})(jQuery);


//获取所有grpname,填充到select中
function c_GetGrpNameList(sid)
{
	var o = $("#" + sid);
  if (o.size() != 1) {
     return;
  }
	var xmldata = amsAjax({"method": "GetGrpNamesList"});
 
	var ret = $("result",xmldata).text();
	if (ret < "0"){
  	showErrMsg(ret);
 
 	}
 	else{
 		var groupnum = $("sdwMapGrpNum",xmldata).text(); 
 		if(groupnum == 0)
 
 		{
 			o.append("<option>--------</option>");
 			return;	
 		} 
		$("cMapGrpNames",xmldata).each(function(i){
			if(i<groupnum)
			{ 
				o.append("<option>"+$(this).text()+"</option>");
      }
    });     
 	}
}  

function GetVdSelect(VdSelectID) 
{
    var vSelectID  = $("#" + VdSelectID);
    if (vSelectID.size() != 1) 
    {
        return;
    }
    var xmldata = amsAjax({"method": "GetAllVdNames"});
	var ret = $("result", xmldata).text();
	if (ret < "0")
	{
	    vSelectID.append("<option>--------</option>");
  	    showErrMsg(ret);
 	}
 	else 
 	{
 	    var vdnum = $(xmldata).find("sdwVdNum").text();
 	    if(vdnum === "0")
 	    {
 			vSelectID.append("<option>--------</option>");
 			return;	
 		} 
        for (i = 0; i < vdnum; i++) 
        {
		    vSelectID.append("<option>"+$(xmldata).find("cVdNames").eq(i).text()+"</option>");
        }
    }
}

function GetVolSelect(VolSelectID, VdName) 
{
    if (typeof VdName == 'undefined')
    {
        return;
    }

    var vVolSelectID  = $("#" + VolSelectID);
    if (vVolSelectID.size() != 1) 
    {
        return;
    }
    
    if(VdName == "")
    {
        vVolSelectID.append("<option>--------</option>"); 
        return;
    }
    
    var vGetVolNamesOnVdXML = amsAjax({"method": "GetVolNamesOnVd","ptAllVolNamesOnVd.cVdName": VdName});
    var ret = $(vGetVolNamesOnVdXML).find("result").text();
    if (ret < "0"){
        vVolSelectID.append("<option>--------</option>");
        showErrMsg(ret);
        return;
    }
    
    var volnum = $(vGetVolNamesOnVdXML).find("sdwVolNum").text();
    if (volnum === "0") {
        vVolSelectID.append("<option>--------</option>");
        return;
    }
    for (i = 0; i < volnum; i++) 
    {
        vVolSelectID.append("<option>"+$(vGetVolNamesOnVdXML).find("cVolNames").eq(i).text()+"</option>");
    }        
}

function InitSelectAllCheckBox()
{
    $("input[type=checkbox]").attr("checked", false);
}

function CheckBoxChecked(TableID, vThis)
{
    if (typeof TableID == 'undefined')
    {
        return;
    }
    var vTableID = TableID;
    if($(vThis).attr("checked")==true) 
    {
        $("#"+vTableID+" tbody input[type=checkbox]").attr("checked", true); 
    }
    else
    {
        $("#"+vTableID+" tbody input[type=checkbox]").attr("checked", false);
    }    
}

//判断字符串由字母数字下划线组成
function isStrValid(str)
{
	var usern = /^[a-zA-Z0-9_]{1,}$/;  
  if((!str)||(!str.match(usern))) {  
    return false; 
  }
	return true; 
} 

//判断邮箱格式
function checkEmail(str){
    if (str === "")
        return false;
    var reg = new RegExp("^[0-9a-zA-Z_]+[\.]?[0-9a-zA-Z_]*@[0-9a-zA-Z]+[\.]{1}[0-9a-zA-Z]+[\.]?[0-9a-zA-Z]+$");
    if (!reg.test(str))
        return false;
    
    return true;
}

//判断ip地址格式
function checkIP(str){
    if (str === "")
        return false;
    var reg = new RegExp("^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$");
    if (!reg.test(str))
        return false;

    return true;
}

//判断MAC地址格式
function checkMAC(str){
    if (str === "")
        return false;
    var temp = new RegExp("^[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}$");
    if (!temp.test(str))
    {
        return false;
    }
    return true;
} 

//判断域名格式
function checkDomain(str){
    if (str === "")
        return false;
    var reg = new RegExp("^[a-zA-Z0-9][-a-zA-Z0-9]{0,62}([.][a-zA-Z0-9][-a-zA-Z0-9]{0,62})+.?$");
    if (!reg.test(str))
        return false;

    return true;
}

function checkSessTimeout(errcode)
{
	if((errcode== (-16918048))||(errcode== (-16918024)))
  {
		 setTimeout(function(){
		   delCookie("sid"); 
    	 parent.location.href = "./index.html";
     }, 3000);	
  }			
}

function showErrMsg(errcode)
{
	for(var i=0;i<top.errData.length;i++)
	{
		if(errcode == top.errData[i].id)
		{ 
			if(top.userData.language == "ch")
				ShowMessage("","",top.errData[i].des_ch);	
			else
				ShowMessage("","",top.errData[i].des_en);	
			checkSessTimeout(errcode);
			return;				
		} 
	}
	if(top.userData.language == "ch")
		ShowMessage("","","未知错误:"+errcode);	 
	else
		ShowMessage("","","Unknown error:"+errcode);	
} 
function amsAjax(reqdata)
{
	var xmldata;
  var senddata = eval(reqdata);
	senddata.sid = top.userData.logID;
	senddata     = decodeURIComponent($.param(senddata));
	$.ajax({
  	url: "./cgi-bin/cgi_agt.cgi",
  	cache: false,
  	global: false,
  	data: senddata,
  	async:false,
  	success: function(s){
  	  xmldata = s;
  	},
  	error: function(r, s, t){
  	  ShowMessage("sys_error");
  	}
  });	
  return xmldata;
} 

function getArgs( ) {
  var args = new Object( );
  var query = location.search.substring(1);      // Get query string
  var pairs = query.split("&");                  // Break at ampersand
  for(var i = 0; i < pairs.length; i++) {
      var pos = pairs[i].indexOf('=');           // Look for "name=value"
      if (pos == -1) continue;                   // If not found, skip
      var argname = pairs[i].substring(0,pos);   // Extract the name
      var value = pairs[i].substring(pos+1);     // Extract the value
      value = decodeURIComponent(value);         // Decode it, if needed
      args[argname] = value;                     // Store as a property
  }
  return args;                                   // Return the object
}

 function c_changeLangue(page){
    if (top.linkData) {
        top.linkData.trace();
    }
    if (!page.res) {
        page.res = {};
    }
    if (page.res[page.pagelan]) {
        z_setLan(page.res[page.pagelan], page.pagelan);
        return;
    }
    var spath = "./res/" + page.pageid + "." + page.pagelan;
    $.ajax({
        cache: true,
        url: spath,
        global: false,
        async: false,
        /*dataType: "json",*/
        success: function(s){
        	//alert(s);
            var o = eval(s);
            z_setLan(o, page.pagelan);
            page.res[page.pagelan] = o;
        },
        error: function(r, s, t){
            alert("res error:" + spath);
        }
    });
};

function c_ajaxerror(r, s, t){
	  /*
    var aDlg = new z_dlg("_ajaxerr_dlg");
    aDlg.init(102);
    var cfg = {};
    cfg.title = {
        ch: "信息",
        en: "Message"
    };
    cfg.mess = {
        ch: "<br />连接服务器失败或服务器忙,请稍后重试!",
        en: "<br />Connect server fail,please try again later!"
    };
    cfg.height = 80;
    cfg.width = 300;
    aDlg.set(cfg);
    */
}

 function z_getDateStr(lan){
     lan = lan || "ch";
     var day = {
         en: ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'],
         ch: ['星期天', '星期一', '星期二', '星期三', '星期四', '星期五', '星期六']
     };
     var s = "";
     var d = new Date();
     s = day[lan][d.getDay()] + " ";
     s += d.getFullYear() + "/";
     s += (d.getMonth() + 1) + "/";
     s += d.getDate();
     return s;
 }

 function z_numInput(sid, min, max){
     var o = $("#" + sid);
     if (o.size() != 1) {
         return;
     }
     var tp = o.attr("type").toLowerCase();
     if ("text" != tp) {
         return;
     }
     o.data("min", min);
     o.data("max", max);
     function kufn(e){
         if (e.which == 37 || e.which == 39) {
             return true;
         }
         var t = $(this);
         var _min = t.data("min");
         var _max = t.data("max");
         var v = t.val();
         if(v.match(/[^\d]|(^0\d)/g) != null){
             v = v.replace(/[^\d]|(^0\d)/g, "");
             t.val(v);
         }
         var _v = "" + _min;
         if (v.length >= _v.length) {
             var i = parseInt(v, 10);
             
             if(_max < _min){
                 t.val(_min);                
                 return true;
             }

             if (i < _min) {
                 t.val(_min);
             }

             if (i > _max) {
                 t.val(_max);
             }
         }
         return true;
     }
    o.unbind("mousemove", kufn);
     o.bind("mousemove", kufn);
     o.unbind("blur", kufn);
     o.bind("blur", kufn);
     o.unbind("mouseout", kufn);
     o.bind("mouseout", kufn);

     function brfn(e){
         var t = $(this);
         var _min = t.data("min");
         var v = t.val();
         v = v.replace(/[^\d]|(^0\d)/g, "");
         if (v == "") {
             //t.val(_min);
         }
         else {
             var i = parseInt(v, 10);
             if (i < _min) {
                 t.val(_min);
             }
         }
     }
     o.unbind("blur", brfn);
     o.bind("blur", brfn);

     o.unbind("mouseout", brfn);
     o.bind("mouseout", brfn);
 }

 function z_strInput(sid, len){
     var o = $("#" + sid);
     if (o.size() != 1) {
         return;
     }
     var tp = o.attr("type").toLowerCase();
     if ("text" != tp && "password" != tp) {
         return;
     }
     function kpfn(e){
         if (e.which == 37 || e.which == 39) {
             return true;
         }
         var t = $(this);
         var v = t.val();
         if(v.match(/[^\w]/g) != null){
            v = v.replace(/[^\w]/g, "");
            if (v.length > len) {
                v = v.substr(0, len);
            }
            t.val(v);
         }
     }
     o.unbind("keyup", kpfn);
     o.bind("keyup", kpfn);
     o.unbind("blur", kpfn);
     o.bind("blur", kpfn);
 }

 function z_byteInput(sid, len){
     var o = $("#" + sid);
     if (o.size() != 1) {
         return;
     }
     var tp = o.attr("type").toLowerCase();
     if ("text" != tp && "password" != tp) {
         return;
     }
     function kbfn(e){
         if (e.which == 37 || e.which == 39) {
             return true;
         }
         var t = $(this);
         var v = t.val();
         var count = 0, idx = 0;
         while (idx < v.length) {
             count += v.charCodeAt(idx++) > 0xFF ? 2 : 1;
             if (count > len) {
                 v = v.substr(0, idx - 1);
                 break;
             }
         }
         t.val(v);
     }
     o.unbind("keyup", kbfn);
     o.bind("keyup", kbfn);
     o.unbind("blur", kbfn);
     o.bind("blur", kbfn);
 }

 function z_colorTable(id){
     var tb = $("#" + id);
     if (tb.size() == 0) {
         return;
     }
     var dby = tb.find("tbody");
     if (dby.size() == 0) {
         dby = tb;
     }
     dby.find("tr:odd").removeClass("odd even").addClass("odd");
     dby.find("tr:even").removeClass("odd even").addClass("even");
     if (tb.data("s") != 1) {
         tb.data("s", 1);
         dby.find("tr").live("click", function(){
             if ($(this).hasClass("on")) {
                 $(this).removeClass("on");
             }
             else {
                 dby.find("tr").removeClass("on");
                 $(this).addClass("on");
             }
         });
         dby.find("tr").live("mouseover", function(){
             $(this).addClass("over");
         });
         dby.find("tr").live("mouseout", function(){
             $(this).removeClass("over");
         });
     }
 };

 /*---xml fun--*/
 function z_getTXML(str, ch){
     if (!ch) {
         str = str.replace(/\r/g, "");
         str = str.replace(/\n/g, "");
         str = str.replace(/\t/g, "");
         str = str.replace(/[^\x00-\xff]/g, "");
     }
     str = "<cgi>" + str + "</cgi>";
     var xmlDoc = null;
     if (window.ActiveXObject) {
         xmlDoc = new ActiveXObject("Microsoft.XMLDOM");
         xmlDoc.async = false;
         xmlDoc.onreadystatechange = function(){
             if (xmlDoc.readyState == 4) {
                 if (xmlDoc.parseError.errorCode != 0) {
                     alert("An Error Occurred: " + xmlDoc.parseError.reason);
                 }
             }
         };
         xmlDoc.loadXML(str);
     }
     else {
         var parser = new DOMParser();
         xmlDoc = parser.parseFromString(str, "text/xml");
         if (!xmlDoc.firstChild.tagName) {
             var serr = xmlDoc.getElementsByTagName("parsererror")[0].childNodes[0].nodeValue;
             alert(serr);
         }
     }
     return xmlDoc;
 }

 function z_loadFXML(file){
     var xmlDoc = null;
     if (window.ActiveXObject) {
         xmlDoc = new ActiveXObject("Microsoft.XMLDOM");
     }
     else
         if (document.implementation && document.implementation.createDocument) {
             xmlDoc = document.implementation.createDocument("", "", null);
         }
         else {
             alert('Your browser cannot handle this script.');
         }
     xmlDoc.async = false;
     xmlDoc.load(file);
     return xmlDoc;
 }

function setCookie(name,value)//两个参数，一个是cookie的名子，一个是值
{
    var Days = 1; //此 cookie 将被保存 0 天
    var exp  = new Date();    //new Date("December 31, 9998");
    exp.setTime(exp.getTime() + Days*24*60*60*1000);
    document.cookie = name + "="+ escape (value) ;
    //+ ";expires=" + exp.toGMTString();
}
function getCookie(name)//取cookies函数        
{
    var arr = document.cookie.match(new RegExp("(^| )"+name+"=([^;]*)(;|$)"));
    if(arr != null) return unescape(arr[2]); return null;
}

function delCookie(name)//删除cookie
{
    var exp = new Date();
    exp.setTime(exp.getTime() - 1);
    var cval=getCookie(name);
    if(cval!=null) document.cookie= name + "="+cval+";expires="+exp.toGMTString();
}

function z_cookie(name, value, options){
    if (typeof value != 'undefined') {
        options = options ||
        {};
        if (value === null) {
            value = '';
            options.expires = -1;
        }
        var expires = '';
        var date;
        date = new Date();
        if (options.expires && (typeof options.expires == 'number' || options.expires.toUTCString)) {
            if (typeof options.expires == 'number') {
                date.setTime(date.getTime() + (options.expires * 24 * 60 * 60 * 1000));
            }
            else {
                date = options.expires;
            }
        }
        else {
            date.setTime(date.getTime() + (10 * 24 * 60 * 60 * 1000));
        }
        expires = '; expires=' + date.toUTCString();
        var path = options.path ? '; path=' + options.path : '';
        var domain = options.domain ? '; domain=' + options.domain : '';
        var secure = options.secure ? '; secure' : '';
        document.cookie =[name,'=',encodeURIComponent(value),expires,path,domain,secure] .join('');
    }
    else {
        var cookieValue = null;
        if (document.cookie && document.cookie != '') {
            var cookies = document.cookie.split(';');
            for (var i = 0; i < cookies.length; i++) {
                var cookie = jQuery.trim(cookies[i]);
                if (cookie.substring(0, name.length + 1) == (name + '=')) {
                    cookieValue = decodeURIComponent(cookie.substring(name.length + 1));
                    break;
                }
            }
        }
        return cookieValue;
    }
}

/* ******** */
 /*
 json resource file:
 ({
 xid:{
 html:{id:html_string},
 title:{id:title_string},
 val:{id:value_string},
 stitle:{id:title_string},
 title:{id:stip_string},
 txt:{id:text_string}
 },
 xclass:{
 title:{class:title_string},
 html:{class:html_string},
 val:{class:value_string},
 stitle:{class:title_string},
 title:{class:stip_string},
 txt:{class:text_string}
 }
 })
 */
function z_jsonres(r,l,o)
{
    var v;
    if(!l){ l='ch';}
    if(!o){ o=$(document);}
    if (r.xid)
    {
        var t=r.xid;
        if (t.html) { for (v in t.html) { $("#" + v).html(t.html[v][l]); } }
        if (t.val) { for (v in t.val) { $("#" + v).val(t.val[v][l]); } }
        if (t.title) { for (v in t.title) {$("#" + v).attr("stip", t.title[v][l]); } }
        if (t.stitle) { for (v in t.stitle) { $("#" + v).attr("title", t.stitle[v][l]); } }
        if (t.txt) { for (v in t.txt) { $("#" + v).text(t.txt[v][l]); } }
        if (t.dialog)
        {
            for (v in t.dialog)
            {
                $("#" + v).dialog("option","title",t.dialog[v]);
                if (r.common)
                {
                    var buttons = $("#"+v).dialog("option", "buttons");
                    for (button in buttons)
                    {
                        buttons[button].text = r.common[buttons[button].id][l] || buttons[button].id;
                    }
                    $("#" + v).dialog("option","buttons",buttons);
             }
            }
        }
        if (t.xValue)
        {
            for ( VData in t.xValue)
            {
                var dataValue = $("#" + VData).data("value");
                var dataView  = "";
                if(dataValue)
                {
                    dataView = t.xValue[VData][dataValue][l];
                    if(dataView)
                    {
                        $("#" + VData).html(dataView);
                    }
                    else
                    {
                        $("#" + VData).html(dataValue);
                    }
                }
            }
        }
        if (t.xDialogTitle)
        {
            for (vDialogID in t.xDialogTitle)
            {
                $("#" + vDialogID).dialog("option",  "title", t.xDialogTitle[vDialogID][l]);
            }
        }
        if (t.xDialogButtonText)
        {
            for (vDialogID2 in t.xDialogButtonText)
            {
                var buttons2 = $("#"+vDialogID2).dialog("option", "buttons");
                for (vbutton2 in buttons2)
                {
                   if(buttons2[vbutton2].id)
                   {
                      buttons2[vbutton2].text = t.xDialogButtonText[vDialogID2][buttons2[vbutton2].id][l] || buttons2[vbutton2].id;
                   }
                }
                $("#" + vDialogID2).dialog("option","buttons",buttons2);
            }
        }
    }
    if (r.xclass)
    {
        var c=r.xclass;
        if (c.title) { for (v in c.title) { o.find("." + v).attr("stip", c.title[v][l]); } }
        if (c.stitle) { for (v in c.stitle) { o.find("." + v).attr("title", c.stitle[v][l]); } }
        if (c.html) { for (v in c.html) { o.find("." + v).html(c.html[v][l]); } }
        if (c.val) { for (v in c.val) { o.find("." + v).val(c.val[v][l]); } }
        if (c.txt) { for (v in c.txt) { o.find("." + v).text(c.txt[v][l]); } }
        if (c.xValue)
        {
            for ( VClass in c.xValue)
            {
                $("." + VClass).each(function()
                {
                    var dataValue2 = $(this).data("value");
                    var dataView2  = "";
                    if(dataValue2)
                    {
                        dataView2 = c.xValue[VClass][dataValue2];
                        if(dataView2)
                        {
                            $(this).html(dataView2);
                        }
                        else
                        {
                            $(this).html(dataValue2);
                        }
                    }
                })
            }
        }
    }
}
var keyvaluefile = {};
function z_setLan(r, lan)
{
    //if (!z_is.Object(r)) { return; }
    //自动组件设置
    if($("#Lib_MessageTitle").length > 0)
    {
        AutoMessageSetLan(lan);
    }
    //设置批量执行结果对话框的语言
    if($("#LIB_BatchResultInfo").length > 0)
    {
        showBatchResultSetLan(lan);
    } 
    
    //设置批量选择对话框的语言
    if($("#LIB_List").length > 0)
    {
        showListSetLan(lan);
    }
    if (r.xid)
    {
        var t=r.xid;
        if (t.html) { for (v in t.html) {$("#" + v).html(t.html[v]); } }
        if (t.val) { for (v in t.val) { $("#" + v).val(t.val[v]); } }
        if (t.title) { for (v in t.title) {$("#" + v).attr("stip", t.title[v]); } }
        if (t.stitle) { for (v in t.stitle) { $("#" + v).attr("title", t.stitle[v]); } }
        if (t.txt) { for (v in t.txt) { $("#" + v).text(t.txt[v]); } }
        if (t.dialog)
        {
            for (v in t.dialog)
            {
                $("#" + v).dialog("option","title",t.dialog[v]);
                var buttons = $("#"+v).dialog("option", "buttons");
                for (button in buttons)
                {
                    var temp  =  buttons[button].id;
                    var temp1 = CommonResource[buttons[button].id];
                    if(   typeof(buttons[button].id) != 'undefined'
                       && typeof(CommonResource["common"][buttons[button].id]) != 'undefined')
                    {
                        buttons[button].text = CommonResource["common"][buttons[button].id][lan] || buttons[button].id;
                    }
                }
                $("#" + v).dialog("option","buttons",buttons);

            }
        }
        if (t.xValue)
        {
            for ( VData in t.xValue)
            {
                if(typeof $("#" + VData).data("value") != "undefined")
                {
                    var dataValue = $("#" + VData).data("value");
                    var dataView  = "";
                    if(   typeof t.xValue[VData] != "undefined"            
                         && typeof (t.xValue[VData][dataValue])!= "undefined") 

                    {
                        $("#" + VData).html(t.xValue[VData][dataValue]);
                    }
                    else
                    {
                        $("#" + VData).html(dataValue);
                    }
                }
            }
        }
        if (t.xDialogTitle)
        {
            for (vDialogID in t.xDialogTitle)
            {
                $("#" + vDialogID).dialog("option",  "title", t.xDialogTitle[vDialogID]);
            }
        }
        if (t.xDialogButtonText)
        {
            for ( vDialogID3 in t.xDialogButtonText)
            {
                var buttons3 = $("#"+vDialogID3).dialog("option", "buttons");
                for (vbutton3 in buttons3)
                {
                    if(buttons3[vbutton3].id)
                    {
                        buttons3[vbutton3].text = t.xDialogButtonText[vDialogID3][buttons3[vbutton3].id] || buttons3[vbutton3].id;
                    }
                }
                $("#" + vDialogID3).dialog("option","buttons",buttons3);
            }
        }
        if (t.xKeyValue)
        {
            for (vKeyValue in t.xKeyValue)
            {
                 if(typeof $("#" + vKeyValue).data("value") != "undefined"  )
                 {
                      var vdataKey   = $("#" + vKeyValue).data("key");
                      var vdataValue = $("#" + vKeyValue).data("value") ;

                      if(   typeof $("#" + vKeyValue).data("key") != "undefined"
                         && typeof (keyvaluefile[lan])!= "undefined"
                         && typeof (keyvaluefile[lan][vdataKey])!= "undefined"
                         && typeof (keyvaluefile[lan][vdataKey][vdataValue])!= "undefined")
                      {

                          $("#" + vKeyValue).html(keyvaluefile[lan][vdataKey][vdataValue]);
                      }
                      else
                      {
                           $("#" + vKeyValue).html(vdataValue);
                      }
                 }
            }
        }
    }

    $(".xKeyValue" ).each(function()
    {
       if(typeof $(this).data("value") != "undefined" )
       {
            var vdataKey1   = $(this).data("key");
            var vdataValue1 = $(this).data("value") ;

            if(    typeof $(this).data("key")!= "undefined"
                && typeof (keyvaluefile[lan])!= "undefined"
                && typeof (keyvaluefile[lan][vdataKey1])!= "undefined"
                && typeof (keyvaluefile[lan][vdataKey1][vdataValue1])!= "undefined")
            {

               $(this).html(keyvaluefile[lan][vdataKey1][vdataValue1]);
            }
            else
            {
                $(this).html(vdataValue1);
            }
       }
    });
    if (r.xclass)
    {
        var c=r.xclass;
        if (c.title) { for (v in c.title) { $("." + v).attr("stip", c.title[v]); } }
        if (c.stitle) { for (v in c.stitle) {$("." + v).attr("title", c.stitle[v]); } }
        if (c.html) { for (v in c.html) { $("." + v).html(c.html[v]); } }
        if (c.val) { for (v in c.val) { $("." + v).val(c.val[v]); } }
        if (c.txt) { for (v in c.txt) { $("." + v).text(c.txt[v]); } }
        if (c.xValue)
        {
            for ( vClass in c.xValue)
            {
                $("." + vClass).each(function()
                {
                    var dataValue2 = $(this).data("value");
                    var dataView2  = "";
                    if(dataValue2)
                    {
                        dataView2 = c.xValue[vClass][dataValue2];
                        if(dataView2)
                        {
                            $(this).html(dataView2);
                        }
                        else
                        {
                            $(this).html(dataValue2);
                        }
                    }
                })
            }
        }
        if (c.xKeyValue)
        {
            for (vClass2 in c.xKeyValue)
            {
                 $("." + vClass2).each(function()
                 {
                    if(typeof $(this).data("value") != "undefined" )
                    {
                         var vdataKey   = $(this).data("key");
                         var vdataValue = $(this).data("value") ;

                         if(    typeof $(this).data("key")!= "undefined"
                             && typeof (keyvaluefile[lan])!= "undefined"
                             && typeof (keyvaluefile[lan][vdataKey])!= "undefined"
                             && typeof (keyvaluefile[lan][vdataKey][vdataValue])!= "undefined")
                         {

                            $(this).html(keyvaluefile[lan][vdataKey][vdataValue]);
                         }
                         else
                         {
                             $(this).html(vdataValue);
                         }
                    }
                 });
            }
        }
    }
}

 function zst(a, b){
     function z(x){
         var i = parseInt(x.slice(1), 10);
         i == NaN ? 0 : i;
         return i;
     }
     return z(b) - z(a);
 }
/*
 $(document).keydown(function(e){
     if (e.which == 13) {
         var a = [];
         for (var x in __dlg.ocz) {
             a.push(x);
         }
         a.sort(zst);
         for (var i = 0, j = a.length; i < j; i++) {
             var o = __dlg.ocz[a[i]];
             if (o.bshow) {
                 o.okclk();
                 break;
             }
         }
     }
 });
*/


/* ******** */
var z_showdom = {};
z_showdom.on = false;
z_showdom.timeid = null;
z_showdom.init = function(){
    if (z_showdom.on) {
        if ($("#_debug").size() == 0) {
            $("<div id=_debug></div>").css({
                display: "none",
                position: "absolute",
                border: "1px solid #3399ff",
                zIndex: "100000",
                padding: "5px",
                backgroundColor: "#eaf5ff",
                opacity: "0.9"
            }).appendTo($('body'));
        }
        $(document).mouseover(function(e){
            var o = $(e.target);
            if (o.size() > 0 && o.attr("id") != "_debug") {
                if (o.data("mk") != 0) {
                    o.data("mk", 0);
                    var shtml = "";
                    var x = e.pageX, y = e.pageY;
                    var offset = o.offset();
                    shtml += "x: " + e.pageX + " y: " + e.pageY;
                    shtml += "<br/>" + o.attr("tagName") + " id: " + o.attr("id");
                    shtml += "<br/>class: " + o.attr("class").replace(/debug/g, "");
                    shtml += "<br/>style: " + o.attr("style");
                    shtml += "<br/>left:" + offset.left + " top:" + offset.top;
                    shtml += "<br/>width/height:" + o.outerWidth() + "/" + o.outerHeight();
                    /*shtml+="<br/>html:"+o.html().replace(/>/g,"]").replace(/</g,"[");*/
                    var y = y > 450 ? y - 150 : y + 10;
                    o.addClass("debug");
                    $("#_debug").html(shtml).css({
                        "display": "",
                        "top": y + "px",
                        left: "0px"
                    });
                    clearTimeout(z_showdom.timeid);
                    z_showdom.timeid = setTimeout(function(){
                        $("#_debug").hide().html("");
                    }, 6000);
                    o.mouseout(function(e){
                        o.data("mk", 1);
                        o.removeClass("debug");
                    });
                }
                return false;
            }
        });
    }
}
/* ****** */

var CommonResource =
{
    common:
    {
        OK          : {ch:"确认", en:"OK"},
        Cancel      : {ch:"取消", en:"Cancel"},
        Reset       : {ch:"重置", en:"Reset"},
        "Lib_MessageTitle": {ch:"信息", en:"Information"}
    },
    message :
    {
        OK     :{ch:"确认", en:"OK"},
        Cancel :{ch:"取消", en:"Cancel"},
        Reset  :{ch:"重置", en:"Reset"}
    }
}

function ShowMessage(dialog_option, static_create, text_string)
{
    var title_id    = dialog_option.title_id || "";
    var text_id     = dialog_option.text_id  || "";
    var text_str    = text_string || "";

    var OnlyOneID   = 0;
    //只有一个参数的时候
    if(title_id == "" && text_id == "")
    {
        title_id        = "Lib_MessageTitle";
        text_id         = dialog_option || "";

        OnlyOneID = 1;
    }

    //设置对话框选项
    var option      = {autoOpen:false,modal:true};
    $.extend(option, dialog_option.option);
    option.buttons  = [];
    //设置按钮
    //默认只有 OK 选项
    if (   typeof(dialog_option.OK) == "undefined"
        && typeof(dialog_option.Cancel) == "undefined"
        && typeof(dialog_option.Reset) == "undefined")
    {
        var btn = {id:"OK"};
        btn.click = function(){$(this).dialog( "close" );};
        option.buttons.push(btn);
    }
    //要什么按钮, 添加什么按钮
    else
    {
        if (dialog_option.OK)
        {
            var btn = {id:"OK"};
            btn.click = dialog_option.OK.click || function(){$(this).dialog( "close" );};
            option.buttons.push(btn);
        }
        if (dialog_option.Cancel)
        {
            var btn = {id:"Cancel"};
            btn.click = dialog_option.Cancel.click || function(){$(this).dialog( "close" );};
            option.buttons.push(btn);
        }
        if (dialog_option.Reset)
        {
            var btn = {id:"Reset"};
            btn.click = dialog_option.Reset.click || function(){$(this).dialog( "close" );};
            option.buttons.push(btn);
        }
    }

    //使用已有的HTML中的ID创建
    if (static_create == true && OnlyOneID == 0)
    {
        $("#"+title_id).dialog(option);
    }
    //默认添加新对象
    else
    {
        //动态生成简单消息框
        var AutoID = title_id;
        if($("#"+AutoID).length > 0)
        {
            $("#"+AutoID).remove();
        }
        $("<div/>").attr("id", AutoID).html(text_str+"<span id="+text_id+"></span>").dialog(option);
    }
    $("#"+title_id).dialog(option);
    c_changeLangue(page);
    $("#"+title_id).dialog("open");
}

function AutoMessageSetLan(lan)
{
    $("#Lib_MessageTitle").dialog("option","title",CommonResource["common"]["Lib_MessageTitle"][lan]);

    var buttons3 = $("#Lib_MessageTitle").dialog("option", "buttons");
    for (vbutton3 in buttons3)
    {
       if(typeof buttons3[vbutton3].id != "undefined")
       {
          if(typeof CommonResource["common"][buttons3[vbutton3].id][lan] != "undefined")
          {
            buttons3[vbutton3].text = CommonResource["common"][buttons3[vbutton3].id][lan]
          }
          else
          {
            buttons3[vbutton3].text = buttons3[vbutton3].id;
          }
       }
    }
    $("#Lib_MessageTitle").dialog("option","buttons",buttons3);
}

function Tip(MessageID, LocationID, Errcodein)
{
    var MID = MessageID||"";
    var LID = LocationID||"";
    var Ecode = Errcodein||"";
    var Einfo = "";
    
    if(Ecode != "" && typeof(top.errData) != "undefined")
    {
        for(var i=0;i<top.errData.length;i++)
	    {
	    	if((Ecode) == top.errData[i].id)
	    	{ 
	    		if(top.userData.language == "ch")
	    			Einfo = top.errData[i].des_ch;	
	    		else
	    			Einfo = top.errData[i].des_en;	
	    		break;				
	    	} 
	    }
	    if(Einfo == "")
	    {
	        if(top.userData.language == "ch")
	    	    Einfo = "未知错误:"+Ecode;	 
	        else
	    	    Einfo = "Unknown error:"+Ecode;
	    }
    }

    var LIDtag =false;

    if(LID == "")
    {
        LID = "#checkinfotip";
    }
    else
    {
        LID = "#"+ LocationID;
        LIDtag = true;
    }

    if($(LID).length > 0 || $(".TipClass").length > 0)
    {
        if(!$(LID).hasClass("TipClass"))
        {
            $(LID).addClass("TipClass");
        }

        if(MID != "" || Ecode != "")
        {
            $(LID).html("");
            var vDiv = "<span id=" + MID + "></span>";
            var vEinf = "<span >"+Einfo+"</span>";
            if(Ecode != "" && typeof(top.errData) != "undefined")
            {
               $(vEinf).appendTo(LID);
            }
            $(vDiv).appendTo(LID);
            c_changeLangue(page);
            $(LID).css({'text-align': 'center','color' : 'red'});
            $(LID).show();
            checkSessTimeout(Errcodein);
        }
        else if(LIDtag)
        {
            $(LID).html("");
            $(LID).hide();
        }
        else
        {
            $(".TipClass").each(function()
            {
                $(this).html("");
                $(this).hide();
            });
        }
    }
}

/**********************************************************************
* 函数名称: ErrorCodeToInfo
* 功能描述: 错误码转换成错误信息
* 输入参数: 错误信息
* 输出参数:
* 返 回 值:

* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011-11-24       V1.0       yuanhan      创建
***********************************************************************/
function ErrorCodeToInfo(Errcodein)
{
    var Ecode = Errcodein;
    var Einfo = "";

    for(var i=0;i<top.errData.length;i++)
    {
    	if((Ecode) == top.errData[i].id)
    	{
    		if(top.userData.language == "ch")
    			Einfo = top.errData[i].des_ch;
    		else
    			Einfo = top.errData[i].des_en;
    		break;
    	}
    }
    if(Einfo == "")
    {
        if(top.userData.language == "ch")
    	    Einfo = "未知错误:"+Ecode;
        else
    	    Einfo = "Unknown error:"+Ecode;
    }

    return Einfo;
}


var MBYTE_FOR_KILO   = 1024;
var GBYTE_FOR_KILO   = 1048576;
var TBYTE_FOR_KILO   = 1073741824;
function FormatSizeFromKilo(sizeKiloByte)
{
    if (sizeKiloByte >= TBYTE_FOR_KILO)
    {
         return((sizeKiloByte/TBYTE_FOR_KILO).toFixed(0) + " TB");
    }
    else if (sizeKiloByte >= GBYTE_FOR_KILO)
    {
        return((sizeKiloByte/GBYTE_FOR_KILO).toFixed(0) + " GB");
    }
    else if (sizeKiloByte >= MBYTE_FOR_KILO)
    {
        return((sizeKiloByte/MBYTE_FOR_KILO).toFixed(0) + " MB");
    }
    else
    {
        return(sizeKiloByte + " KB");
    }
}

/**********************************************************************
* 函数名称: KiloBYTEToMillionBYTE
* 功能描述: 把千字节的数据, 格式化成兆字节, 每三位加一个逗号
* 输入参数: iKiloByte 以 KB 为单位的数量
* 输出参数:

* 返 回 值:
* 其它说明: 替换 原来的 FormatSizeFromKilo 显示方式
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011-04-11      V1.0       yuanhan      创建
***********************************************************************/
function KiloBYTEToMillionBYTE(sizeKiloByte)
{
    var iVarNumber  = Math.floor(sizeKiloByte/1024);
    var iVar        = iVarNumber.toString();

    var len         = Math.floor(iVar.length / 3);  /* 取数据长度 */
    var remainder   = iVar.length % 3;              /* 取余数 */
    var i           = 0;
    var m           = 0;
    if (len < 1)                                      /* 小于3位直接返回 */
    {
        return (iVar+ " MB");
    }

    var array       = iVar.split("");
    var resultArray = [];                             /* 每三数占一位 */

    var j = 0;
    if (remainder !== 0)                              /* 前几位处理 */
    {
        var temp = "";
        for ( i = 0; i < remainder; i++)
        {
            temp = temp + array[i];
        }
        resultArray[j] = temp;
        j = 1;
    }

    for ( m = 0; m < len; m++)
    {
        resultArray[j] = ""+ array[m * 3 + remainder]+
                         array[m * 3 + remainder + 1]+
                         array[m * 3 + remainder + 2];
        j++;
    }
    return (resultArray.join(",")+ " MB");
}

/********************************************************************** 
* 函数名称: ChangeView                                           
* 功能描述:                                       
* 输入参数:                                            
* 输出参数:                                                                 
                                                                        
* 返 回 值:                                                                
* 其它说明: 切换视图                                  
* 修改日期        版本号     修改人       修改内容                                    
* -----------------------------------------------                       
* 2011-07-20      V1.0       yuanhan      创建                            
***********************************************************************/ 
function ChangeView()
{
    if( typeof top.userData != 'undefined'
        && top.userData.loginLevel != 'undefined'
        && top.userData.loginLevel == 1)
    {        
        $(".guestview" ).each(function()
        {
            $(this).hide();
        });
    }
}
/**********************************************************************
* 函数名称: showBatchResultResource  
* 功能描述: 批量执行结果对话框的资源文件
* 输入参数: 
* 输出参数:

* 返 回 值:
* 其它说明: 
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011-07-11      V1.0       yuanhan      创建
***********************************************************************/
var showBatchResultResource =
{
    "LIB_BatchResultInfoTitle" :{ch:"执行结果", en:"Result Information"}, 
    "LIB_successID":{ch:"成功", en:"Succeeded"},
    "LIB_failID":{ch:"失败", en:"Failed"},
    "LIB_successNull":{ch:"无", en:"NULL"},
    "LIB_failNull":{ch:"无", en:"NULL"}
    
}
/**********************************************************************
* 函数名称: showBatchResult  
* 功能描述: 显示批量执行结果
* 输入参数: 
* 输出参数:

* 返 回 值:
* 其它说明: 
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011-07-11      V1.0       yuanhan      创建
***********************************************************************/
function showBatchResult(successResultPara,failResultPara,optionPara)
{
    if($("#LIB_BatchResultInfo").length > 0){
        $("#LIB_BatchResultInfo").remove();
    }    
    
    if(successResultPara.length == 0 && failResultPara.length == 0){
        return;
    }

    var successResultIn = successResultPara||"";
    var failResultIn    = failResultPara||"";
    var optionIn        = optionPara||"";
    
    var DivTemplate     = "<div  id=\"LIB_BatchResultInfo\"  class=\"d_win\" style=\"display:none\">  \
                                 <table class=tb_info>                                                \
                                     <tbody>                                                          \
                                         <tr class=\"success\">                                       \
                                             <td id=\"LIB_successID\" class=\"td_l\"></td>            \
                                             <td id=\"LIB_successResult\" class=\"td_r\" ></td>       \
                                         </tr>                                                        \
                                         <tr class=\"fail\">                                          \
                                             <td id=\"LIB_failID\" class=\"td_l\"></td>               \
                                             <td id=\"LIB_failResult\" class=\"td_r\"></td>           \
                                         </tr>                                                        \
                                     </tbody>                                                         \
                                 </table>                                                             \
                           </div>                                                                     ";
                         
    
    var option =  { modal: true, height: 200, width : 400, autoOpen: false};
    if (optionIn !== "")
    {
        $.extend(option, optionIn);
    } 
    
    //增加确定按钮
    option.buttons  = [];
    var btn = {id:"OK"};
    btn.click = function(){$(this).dialog( "close" );};
    option.buttons.push(btn);    
    
    var vdialog = $(DivTemplate).dialog(option);
    vdialog.dialog(option);//解决 toXmlRPC 按钮的问题
    if(successResultIn !== "")
    {
        $("#LIB_BatchResultInfo #LIB_successResult").html(successResultIn);
    }
    else
    {
        $("#LIB_BatchResultInfo tr.success").remove();
    }
    if(failResultIn !== "")
    {
        $("#LIB_BatchResultInfo #LIB_failResult").html(failResultIn);
    }
    else
    {
        $("#LIB_BatchResultInfo tr.fail").remove();
    }
    vdialog.dialog("open");
}
/**********************************************************************
* 函数名称: showBatchResultSetLan  
* 功能描述: 设置批量执行结果对话框的语言
* 输入参数: 
* 输出参数:

* 返 回 值:
* 其它说明: 
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011-07-11      V1.0       yuanhan      创建
***********************************************************************/
function showBatchResultSetLan(lan)
{
    $("#LIB_BatchResultInfo").dialog("option","title",showBatchResultResource["LIB_BatchResultInfoTitle"][lan]);

    
    var buttons4 = $("#LIB_BatchResultInfo").dialog("option", "buttons");
    for (vbutton4 in buttons4)
    {
       if(typeof buttons4[vbutton4].id != "undefined")
       {
          if(typeof CommonResource["common"][buttons4[vbutton4].id][lan] != "undefined")
          {
            buttons4[vbutton4].text = CommonResource["common"][buttons4[vbutton4].id][lan];
          }
          else
          {
            buttons4[vbutton4].text = buttons4[vbutton4].id;
          }
       }
    }
    $("#LIB_BatchResultInfo").dialog("option","buttons",buttons4);    
    

    $("#LIB_BatchResultInfo #LIB_successID").html(showBatchResultResource["LIB_successID"][lan]);
    $("#LIB_BatchResultInfo #LIB_successNull").html(showBatchResultResource["LIB_successNull"][lan]);
    $("#LIB_BatchResultInfo #LIB_failID").html(showBatchResultResource["LIB_failID"][lan]);
    $("#LIB_BatchResultInfo #LIB_failNull").html(showBatchResultResource["LIB_failNull"][lan]);        
}
/**********************************************************************
* 函数名称: showListResource  
* 功能描述: 批量选择对话框的资源文件
* 输入参数: 
* 输出参数:

* 返 回 值:
* 其它说明: 
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011-07-20      V1.0       yuanhan      创建
***********************************************************************/
var showListResource =
{
    LIB_List_Nodata : {ch:"列表为空", en:"List is empty."},
    OK          : {ch:"确认", en:"OK"},
    Cancel      : {ch:"取消", en:"Cancel"},
    Reset       : {ch:"重置", en:"Reset"},
    "LIB_ListTitle": {ch:"列表", en:"List"}    
};
/**********************************************************************
* 函数名称: showListSetLan  
* 功能描述: 设置批量选择对话框的语言
* 输入参数: 
* 输出参数:

* 返 回 值:
* 其它说明: 
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011-07-20      V1.0       yuanhan      创建
***********************************************************************/
function showListSetLan(lan)
{
    $("#LIB_List").dialog("option","title",showListResource["LIB_ListTitle"][lan]);

    $("#LIB_List_Nodata").html(showListResource["LIB_List_Nodata"][lan]) ;

    var buttons4 = $("#LIB_List").dialog("option", "buttons");
    for (vbutton4 in buttons4)
    {
       if(typeof buttons4[vbutton4].id != "undefined")
       {
          if(typeof showListResource[buttons4[vbutton4].id][lan] != "undefined")
          {
            buttons4[vbutton4].text = showListResource[buttons4[vbutton4].id][lan]
          }
          else
          {
            buttons4[vbutton4].text = buttons4[vbutton4].id;
          }
       }
    }
    $("#LIB_List").dialog("option","buttons",buttons4);     
};
/**********************************************************************
* 函数名称: showList  
* 功能描述: 显示选择对话框
* 输入参数: keyarrayIn     选择的值列表
            valuearrayIn   选择的显示列表
            funcIn         选择之后的回调函数
            optionIn       对话框参数
            oldkeyarrayIn  预先选中的值列表
* 输出参数:

* 返 回 值:
* 其它说明: 
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011-07-20      V1.0       yuanhan      创建
***********************************************************************/
function showList (keyarrayIn, valuearrayIn, funcIn, optionIn, oldkeyarrayIn)
{
    if(!$.isArray(keyarrayIn))
    {
        return;
    }
    
    if($("#LIB_List").length > 0)
    {
        $("#LIB_List").remove();
    }

    var obj = {};
    var j   = 0;
    var keyarray    = keyarrayIn;
    var valuearray  = [];
    var oldkeyarray = [];

    if($.isArray(valuearrayIn))
    {
        valuearray = valuearrayIn;
        for(var i = 0; i < keyarray.length && i< valuearray.length ; i++ )
        {
            obj[keyarray[i]]=valuearray[i];
            j++;
        }
    }
    
    if($.isArray(oldkeyarrayIn))
    {
        oldkeyarray = oldkeyarrayIn;
    }

    while(j < keyarray.length)
    {
        obj[keyarray[j]]=keyarray[j];
        j++;
    }

    keyarray.sort();

    var vdiv    = "<div id=\"LIB_List\" class=\"d_win\" style=\"display:none\"> \
                        <table id=\"LIB_ListTable\" class=\"tablesorter\">      \
                          <tfoot style=\"display:none\">                        \
                            <tr>                                                \
                              <td id=\"LIB_List_Nodata\" colspan=\"8\"></td>    \
                            </tr>                                               \
                          </tfoot>                                              \
                          <tbody>                                               \
                          </tbody>                                              \
                        </table>                                                \
                    </div>                                                      ";

    var vtr   = "<tr >                                                                                     \
                    <td style=\"width:10%;\"><input type=\"checkbox\" class=\"checkbox\" value=\"\"/></td> \
                    <td class=\"LIB_ListValue\" style=\"width:90%;\"></td>                                         \
                 </tr>                                                                                     ";
                 
   var jdiv = $(vdiv);
   var jtr  = $(vtr);
   var k = 0;
   var rowaddedNum = 0;
   for(k=0; k< keyarray.length; k++)
   {
        var jtrTemp = jtr.clone();        
        jtrTemp.data("LIB_ListKey", keyarray[k]).find("td.LIB_ListValue").html(obj[keyarray[k]]); 
        var itemp = $.inArray(keyarray[k],oldkeyarray);
        if(itemp > -1)
        {
            jtrTemp.find("td .checkbox").attr("checked", "checked");
        }
        jdiv.find("table").append(jtrTemp);
        rowaddedNum++;
   }
   
   var option      = {autoOpen:false,modal:true};
   $.extend(option, optionIn);
   option.buttons  = [];
   var btnOK = {id:"OK"};
   btnOK.click = function(){                
        var vCheckedbox = $("#LIB_ListTable").find(":checkbox:checked");
        var vresultarray    = [];
        var vresult         = "";
        if(vCheckedbox.size() > 0)
        {
            vCheckedbox.each(function()
            {
                var tableRow        = $(this).closest("tr");
                var vListKey        = tableRow.data("LIB_ListKey");
                vresultarray.push(vListKey);
            });            
        }
        vresult = vresultarray.join();
        $(this).dialog( "close" );
        if(isFunction(funcIn))
        {
            funcIn(vresult);
        }
    };
   option.buttons.push(btnOK);
   
   var btnCancel = {id:"Cancel"};
   btnCancel.click = function(){$(this).dialog( "close" );};
   option.buttons.push(btnCancel);
   
   
   jdiv.dialog(option);
   jdiv.dialog("open");

   if(rowaddedNum==0)
   {
      $("tfoot", jdiv).show();
   }
   else
   {
      $("tfoot", jdiv).hide();
   }


   jdiv.find("tr").each(function(){
        var vLIB_ListKey  = $(this).data("LIB_ListKey");
        var iCheckInArray = $.inArray(vLIB_ListKey,oldkeyarray);
        if(iCheckInArray > -1)
        {
            $(this).find("td .checkbox").attr("checked", "checked");
        }

   })

};
/**********************************************************************
* 函数名称: isFunction  
* 功能描述: 判断是不是一个函数
* 输入参数: 
* 输出参数:

* 返 回 值:
* 其它说明: 
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011-07-20      V1.0       yuanhan      创建
***********************************************************************/
function isFunction( fn ) { 
return (!!fn 
       && !fn.nodeName 
       && fn.constructor != String 
       && fn.constructor != RegExp 
       && fn.constructor != Array 
       && /function/i.test( fn + "" )); 
};

function MapJson2Html(json, jqObj, array_offset)
{
    jqObj.children().each(function()
    {
        if ($(this).attr("class") && ($(this).attr("class")=="array"))
        {
            $(this).find("#context").children().remove();
            // 循环数组(注意，不支持二维数组)
            var count = $(this).attr("count");
            for (var i=0;i<json[count];i++)
            {
                var newdiv = $(this).find(".template").clone().removeClass("template").css("display", "");
                MapJson2Html(json, newdiv, i);
                var org_html = $(this).find("#context").html();
                $(this).find("#context").html(org_html + newdiv.unwrap().html());
            }
        }
        else if ($(this).attr("name"))
        {
            var node_name = $(this).attr("name");
            if (json[node_name])
            {
                var sub_json;
                if (array_offset != null)
                {
                    sub_json = json[node_name][array_offset];
                }
                else
                {
                    sub_json = json[node_name];
                }
                if (typeof(sub_json) == "object")
                {
                    MapJson2Html(sub_json, $(this));
                }
                else
                {
                    $(this).html(sub_json);
                }

            }
        }
        else
        {
            MapJson2Html(json, $(this), array_offset);
        }
    });
}

function getLocalTime(dateString){
    if(dateString == null){
        return "";
    }
    
    var s = dateString;
    var d = new Date(Date.parse(s.replace(/-/g,"/")));
    var minutes = d.getTimezoneOffset();	
	d.setHours(d.getHours() - minutes/60);
	return d;
}

function toggle_fn() {
    $(this).parent().parent().next().children().children().slideToggle();
    //$(this).parent().parent().next().children().toggle();
    $(this).toggleClass("ui-icon-minus");
    if ($(this).hasClass("ui-icon-minus"))
    {
        $(this).trigger("onexpand");
    }
}
$.fn.extend({
    jExpand: function (onexpand) {
        $(this).addClass("ui-icon ui-icon-plus");
        $(this).css("cursor", "pointer");
        $(this).live("click", toggle_fn);
        if (typeof onexpand != 'undefined')
        {
            $(this).live("onexpand", onexpand);
        }
    }
});

function OpenWinToCenter( winWidth,winHeight, url ){
	var win = window.open( url,"","toolbar=0,menubar=0,status=0,location=0,resizable=1,scrollbars=1,width="+winWidth+",height="+winHeight+",left="+(window.screen.width-winWidth)/2+",top="+(window.screen.height-winHeight)/2 );
	win.focus();
	return false;
} 

(function($){
$.fn.AddSubRow = function(options){
    var settings = {rowspan: false, onExpand:"", onClose:""};
    $.extend(settings, options);
    $(this).addClass("ui-icon ui-icon-triangle-1-e");
    $(this).live("click", function()
    {
        var thisRow = $(this).closest("tr");
        var nextRow = $(this).closest("tr").next();
        if(nextRow.hasClass('expand-child'))
        {
            $(this).toggleClass("ui-icon-triangle-1-e");
            $(this).toggleClass("ui-icon-triangle-1-s");
            if($(this).hasClass("ui-icon-triangle-1-s") && settings.onExpand !== "")
            {
                $(this).trigger("onExpand");
            }
            if($(this).hasClass("ui-icon-triangle-1-e") && settings.onClose !== "")
            {
                $(this).trigger("onClose");
            }
            $("> td > div", nextRow).slideToggle();
        }
    });
    if(settings.rowspan)
    {
       $(this).live("click", function()
       {
            var thisRow = $(this).closest("tr");
            var nextRow = $(this).closest("tr").next();
            if(nextRow.is(":visible"))
            {
                thisRow.find("td").eq(0).attr("rowspan","2");
            }
            else
            {
                thisRow.find("td").eq(0).attr("rowspan","1");
            }
       });
    }
    if(settings.onExpand !== "")
    {
        $(this).live("onExpand", settings.onExpand);
    }
    if(settings.onExpand !== "")
    {
        $(this).live("onClose", settings.onClose);
    }
};
})(jQuery);


// A more performant, but slightly bulkier, RFC4122v4 solution.  We boost performance
// by minimizing calls to random()
var UUID_CHARS = '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz'.split('');

function GenUUIDFast() {
  var chars = UUID_CHARS, uuid = new Array(36), rnd=0, r;  

  for (var i = 0; i < 36; i++) {
    if (i==8 || i==13 ||  i==18 || i==23) {
      uuid[i] = '-';
    } else if (i==14) {
      uuid[i] = '4';
    } else {
      if (rnd <= 0x02) rnd = 0x2000000 + (Math.random()*0x1000000)|0;
      r = rnd & 0xf;
      rnd = rnd >> 4;
      uuid[i] = chars[(i == 19) ? (r & 0x3) | 0x8 : r];
    }
  }  

  return uuid.join('');
};

//Date object to rfc3339 format string
function RFC3339DateString(d) {
    function pad(n){return n<10 ? '0'+n : n}
    return   d.getUTCFullYear()    +'-'
          + pad(d.getUTCMonth()+1) +'-'
          + pad(d.getUTCDate())    +' '
          + pad(d.getUTCHours())   +':'
          + pad(d.getUTCMinutes()) +':'
          + pad(d.getUTCSeconds()) +''
          + d.getTimezoneOffset()/60;
}
