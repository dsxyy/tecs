
function OutFileFtpForm(hostip,user,pwd,port,lang){
    var outstr = '';
    outstr += '<APPLET id="myApplet"';
    outstr += 'CODEBASE="." ';
    outstr += 'code="com.test.ftp.FileFtpForm.class" ';
    outstr += 'ARCHIVE="/jar/FileFtpForm.jar" ';
    outstr += 'width=100% ';
    outstr += 'height=100% >';
    outstr += '<PARAM NAME="Host" VALUE="'+hostip+'"/>';
    outstr += '<PARAM NAME="User" VALUE="'+user+'"/>';
    outstr += '<PARAM NAME="Password" VALUE="'+pwd+'"/>'; 
    outstr += '<PARAM NAME="Port" VALUE="'+port+'"/>'; 
    outstr += '<PARAM NAME="Lang" VALUE="'+lang+'"/>'; 
    outstr += '</APPLET> ';
    $("#dvUpload").html(outstr);     
}

/*--main--*/
$(function(){
    var errData = LoadErrorFile();
    if(CheckJdkIsInstall(getArgs().lang,errData,"dvUpload")){
        OutFileFtpForm(getArgs().host,"#$%Tecs_Web_Ftp_Unique_User%$#",getArgs().password,getArgs().port,getArgs().lang);
    }
    /*-main end-*/
});/*ready*/
