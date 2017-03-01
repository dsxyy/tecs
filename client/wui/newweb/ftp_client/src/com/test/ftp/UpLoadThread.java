/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.test.ftp;

import com.test.ftp.ContinueFTP.UploadStatus;
import com.test.ftp.ResInfo.ResKey;
import java.io.File;
import java.io.IOException;

/**
 *
 * @author qilzh
 */
public class UpLoadThread extends Thread{
    private File fileIn;
    private FileFtpForm form;
    private String strHost;
    private String strPort;
    private String strUser;
    private String strPwd;
    private String strLang;
    public UpLoadThread(String strHost,String strPort,String strUser,String strPwd,String strLang,File fileIn,FileFtpForm form) {
        this.fileIn = fileIn;
        this.form = form;
        this.strHost = strHost;
        this.strPort = strPort;
        this.strUser = strUser;
        this.strPwd = strPwd;
        this.strLang = strLang;
    }

    private  ResKey ConvertUploadToResKey(UploadStatus status){
        if(status == UploadStatus.Create_Directory_Fail){
            return ResKey.Create_Directory_Fail;
        }
        else if(status == UploadStatus.Create_Directory_Success){
            return ResKey.Create_Directory_Success;
        }
        else if(status == UploadStatus.Upload_New_File_Success){
            return ResKey.Upload_New_File_Success;
        }
        else if(status == UploadStatus.Upload_New_File_Failed){
            return ResKey.Upload_New_File_Failed;
        }
        else if(status == UploadStatus.File_Exits){
            return ResKey.File_Exits;
        }
        else if(status == UploadStatus.Remote_Bigger_Local){
            return ResKey.Remote_Bigger_Local;
        }
        else if(status == UploadStatus.Upload_From_Break_Success){
            return ResKey.Upload_From_Break_Success;
        }
        else if(status == UploadStatus.Upload_From_Break_Failed){
            return ResKey.Upload_From_Break_Failed;
        }
        else if(status == UploadStatus.Delete_Remote_Faild){
            return ResKey.Delete_Remote_Faild;
        }
        return ResKey.Upload_New_File_Failed;
    }
    
    public void run() {
        ContinueFTP myftp = new ContinueFTP(form);
        ResInfo resinfo = new ResInfo(strLang);
        UploadStatus ret = UploadStatus.Upload_New_File_Failed;
        boolean bConnRet = false;
        try{
            ImageInfo image = new ImageInfo(fileIn,strHost,strPwd);
            if(!image.CheckImageIsRunning()){
                form.AppendResult(resinfo.GetResInfo(ResKey.Upload_New_File_Failed) + resinfo.GetResInfo(ResKey.Image_BeingUsed),true);
                return;
            }
            long l1 = System.currentTimeMillis();
            bConnRet = myftp.connect(strHost, Integer.parseInt(strPort), strUser, strPwd);
            if (bConnRet) {
                String remoteFile = myftp.getRemoteFileName(fileIn.getName());
                ret = myftp.upload(fileIn, remoteFile);
                if(ret == UploadStatus.Upload_New_File_Success ||
                   ret == UploadStatus.Upload_From_Break_Success){
                    long l2 = System.currentTimeMillis();
                    form.AppendResult(resinfo.GetResInfo(ConvertUploadToResKey(ret))+GetUpLoadTime(l1,l2),true);
                    form.SetImageInfoTimer(fileIn);
                }
                else{
                    ret = UploadStatus.Upload_New_File_Failed;
                    form.AppendResult(resinfo.GetResInfo(ConvertUploadToResKey(ret))+ ":" + myftp.ftpClient.getReplyString(),false);
                }
            }
        }
        catch(IOException e1){
            e1.printStackTrace();
            System.out.println("IO：" + e1.getMessage());
            form.AppendResult("文件传输出错:" + resinfo.GetResInfo(ConvertUploadToResKey(ret)) + ":" + myftp.ftpClient.getReplyString(),false);
        }
        catch (Exception e1) {
            e1.printStackTrace();
            System.out.println("连接FTP出错：" + e1.getMessage());
            form.AppendResult(resinfo.GetResInfo(ConvertUploadToResKey(ret))+ ":" + myftp.ftpClient.getReplyString(),false);
        }
        finally{
            form.SetUploadIsEnabled(true);
            if(bConnRet){
                try {
                    myftp.disconnect();
                } catch (IOException ex) {
                    System.out.println("关闭FTP连接出错：" + ex.getMessage());
                }
            }
        }
        return;
    }

    private String GetUpLoadTime(long l1,long l2){
        ResInfo resinfo = new ResInfo(strLang);
        String strTimeDesc = resinfo.GetResInfo(ResKey.Consuming) + ":";
        long time = (l2 - l1)/1000;                
        long hour = time/3600;
        time = time % 3600;
        long minute = time/60;
        time = time % 60;
        strTimeDesc += String.valueOf(hour) + resinfo.GetResInfo(ResKey.hour);
        strTimeDesc += String.valueOf(minute) + resinfo.GetResInfo(ResKey.minute);
        strTimeDesc += String.valueOf(time) + resinfo.GetResInfo(ResKey.second);
        return strTimeDesc;
    }
}
