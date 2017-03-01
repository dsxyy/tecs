function OutVncForm(ip,port,pwd){
    var strapplet = '<APPLET ';
    strapplet    += 'CODEBASE="." ';
    strapplet    += 'CODE="com.tightvnc.vncviewer.VncViewer"'; 
    strapplet    += 'ARCHIVE="/jar/TightVncViewer.jar" ';
    strapplet    += 'WIDTH="1024" HEIGHT="768">';
    strapplet    +=  '<PARAM NAME="HOST" VALUE="'+ip+'">';
    strapplet    +=  '<PARAM name="Password" value="'+pwd+'">'; 
    strapplet    +=  '<PARAM NAME="PORT" VALUE="'+port+'">'; 
    strapplet    +=  '<PARAM NAME="Open New Window" VALUE="no">'; 
    strapplet    +=  '<param name="ShowControls" value="no" />';
    strapplet    +=  '<param name="ViewOnly" value="no" />';
    strapplet    +=  '<param name="ShareDesktop" value="yes" />'; 
    strapplet    +=  '<param name="AllowCopyRect" value="yes" />'; 
    strapplet    +=  '<param name="Encoding" value="Tight" />'; 
    strapplet    +=  '<param name="CompressionLevel" value="" />'; 
    strapplet    +=  '<param name="JpegImageQuality" value="" />';
    strapplet    +=  '<param name="LocalPointer" value="On" />'; 
    strapplet    +=  '<param name="colorDepth" value="" />';
    strapplet    +=  '<param name="ScalingFactor" value="100" />';
    strapplet    +=  '</APPLET>';
    $("#dvVnc").html(strapplet); 
} 

/*--main--*/
$(function(){
    var errData = LoadErrorFile();
    window.document.title= "instance " + getArgs().vid;
    if(CheckJdkIsInstall(getArgs().lang,errData,"dvVnc")){
        OutVncForm(getArgs().host,getArgs().port,getArgs().pwd); 
    }
    /*-main end-*/
});/*ready*/
