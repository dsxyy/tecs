/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.test.ftp;

import com.test.ftp.ResInfo.ResKey;
import java.io.File;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.HashMap;
import java.util.Timer;
import java.util.Vector;
import org.apache.xmlrpc.XmlRpcException;
import org.apache.xmlrpc.client.XmlRpcClient;
import org.apache.xmlrpc.client.XmlRpcClientConfigImpl;

/**
 *
 * @author qilzh
 */
public class ImageInfo extends java.util.TimerTask{
    public void run() {
        // TODO Auto-generated method stub
        int ret = 0;
        ResInfo resinfo = new ResInfo(strLang);
        this.formFtp.AppendResult(resinfo.GetResInfo(ResKey.Begin_Query_image) + "......",true);
        Object[] result = QueryImageInfo();
        if(result != null){
            int imagecount = ((Long)result[2]).intValue();
            Object[] obj = (Object[])(result[3]);
            if(imagecount > 0){
                HashMap h = (HashMap)obj[0];
                System.out.println(h.get("id").toString());
                this.formFtp.AppendResult(resinfo.GetResInfo(ResKey.Begin_Modify_image) + "......",true);
                ret = SetImageInfo(h.get("id").toString());
                if(ret == 0){
                    this.formFtp.AppendResult(resinfo.GetResInfo(ResKey.Succ_Modify_image),true);
                    this.formFtp.SetPbProcess(100);
                }
                else{
                    this.formFtp.AppendResult(resinfo.GetResInfo(ResKey.Failed_Modify_image),true);
                }
                System.out.print(ret);
                this.timer.cancel();
            }
        }
        else{
            System.out.println("再找");
        }
        count++;
        if(count > 3){
            this.timer.cancel();
            this.formFtp.AppendResult(resinfo.GetResInfo(ResKey.Query_image_Failed),false);
        }
        return;
    }
    
    public ImageInfo(Timer timer,File fileIn,String strHost,String strSession,String strLang,HashMap hsSetImageParam,FileFtpForm form){
        this.timer = timer;
        this.fileIn = fileIn;
        this.strHost = strHost;
        this.strSession = strSession;
        this.strLang = strLang;
        this.hsSetImageParam = hsSetImageParam;
        this.formFtp = form;
    }

    public ImageInfo(File fileIn,String strHost,String strSession){
        this.fileIn = fileIn;
        this.strHost = strHost;
        this.strSession = strSession;
    }

    public boolean CheckImageIsRunning(){
        Object[] result = QueryImageInfo();
        if(result != null){
            int imagecount = ((Long)result[2]).intValue();
            Object[] obj = (Object[])(result[3]);
            if(imagecount > 0){
                HashMap h = (HashMap)obj[0];
                if((Integer)(h.get("running_vms")) > 0){
                    return false;
                }
            }
        }
        return true;
    }

    public int SetImageInfo(String id){
        int ret = 0;
        try {
            // config client
            String strServerURL = "http://" + strHost + ":8080/RPC2";
            XmlRpcClientConfigImpl config = new XmlRpcClientConfigImpl();
            config.setServerURL(new URL(strServerURL));
            config.setEnabledForExtensions(true);
            config.setConnectionTimeout(60 * 1000);
            config.setReplyTimeout(60 * 1000);

            // create a new XmlRpcClient object and bind above config object with it
            XmlRpcClient client = new XmlRpcClient();
            client.setConfig(config);
            // create parameter list
            Vector params = new Vector();
            params.addElement(strSession);
            params.addElement(new Long(id));
            params.addElement(hsSetImageParam.get("name").toString());
            params.addElement(hsSetImageParam.get("type").toString());
            params.addElement(hsSetImageParam.get("arch").toString());
            params.addElement(hsSetImageParam.get("bus").toString());
            params.addElement(hsSetImageParam.get("description").toString());
            params.addElement(hsSetImageParam.get("os_type").toString());
            params.addElement(hsSetImageParam.get("container_format").toString());
            Object[] result = (Object []) client.execute("tecs.image.set", params);
            ret = ((Integer)result[0]).intValue();
        } catch (MalformedURLException e) {
            System.out.println(e.toString());
        } catch (XmlRpcException e) {
            System.out.println(e.toString());
        } catch (IOException e) {
            e.printStackTrace();
        }
        return ret;
    }

    public Object[] QueryImageInfo(){
        Object[] result = null;
        try {
            // config client
            String strServerURL = "http://" + strHost + ":8080/RPC2";
            int type = 3;
            XmlRpcClientConfigImpl config = new XmlRpcClientConfigImpl();
            config.setServerURL(new URL(strServerURL));
            config.setEnabledForExtensions(true);
            config.setConnectionTimeout(60 * 1000);
            config.setReplyTimeout(60 * 1000);

            // create a new XmlRpcClient object and bind above config object with it
            XmlRpcClient client = new XmlRpcClient();
            client.setConfig(config);
            // create parameter list
            Vector params = new Vector();
            params.clear();
            params.addElement(strSession);
            params.addElement(new Long(0));
            params.addElement(new Long(50));
            params.addElement(type);
            params.addElement("");
            params.addElement(new Long(-1));
            params.addElement(fileIn.getName());
            result = (Object []) client.execute("tecs.image.query", params);
            int ret = ((Integer)result[0]).intValue();
            if(ret == 0){
                return result;
            }
            else{
                return null;
            }
        } catch (MalformedURLException e) {
            System.out.println(e.toString());
        } catch (XmlRpcException e) {
            System.out.println(e.toString());
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }
    private Timer timer;
    private File fileIn;
    private String strHost;
    private String strSession;
    private String strLang;
    private HashMap hsSetImageParam;
    private FileFtpForm formFtp = null;
    private int count = 0;
}
