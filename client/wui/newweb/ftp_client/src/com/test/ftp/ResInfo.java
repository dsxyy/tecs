/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.test.ftp;
/**
 *
 * @author qilzh
 */
public class ResInfo {
    private String strLang = "ch";

    public ResInfo(String strLang){
        this.strLang = strLang;
    }

    //枚举资源文件值
    public enum ResKey {
        Create_Directory_Fail,
        Create_Directory_Success,
        Upload_New_File_Success,
        Upload_New_File_Failed,
        File_Exits,
        Remote_Bigger_Local,
        Upload_From_Break_Success,
        Upload_From_Break_Failed,
        Delete_Remote_Faild,
        Begin_Query_image,
        Begin_Modify_image,
        Query_image_Failed,
        Succ_Modify_image,
        Failed_Modify_image,
        Image_BeingUsed,
        Consuming,
        hour,
        minute,
        second,
        name,
        type,
        containerformat,
        bus,
        arch,
        description,
        os_type,
        select_file,
        browse,
        upload,
        clear,
        name_null,
        name_input_error,
        name_length_error,
        desc_length_error,
        must_select_file,
        open_file;
    }
    
    private String[] arrChRes = {
                                "远程服务器相应目录创建失败!",
                                "远程服务器创建目录成功!",
                                "上传文件成功!",
                                "上传文件失败!",
                                "文件已经存在!",
                                "远程文件大于本地文件!",
                                "断点续传成功!",
                                "断点续传失败!",
                                "删除远程文件失败!",
                                "正在查询映像",
                                "已经找到镜像，正在修改.....",
                                "查找镜像失败",
                                "修改镜像成功!",
                                "修改镜像失败!",
                                "镜像正在被使用.",
                                "共耗时",
                                "时",
                                "分",
                                "秒",
                                "名称",
                                "类型",
                                "容器格式",
                                "总线",
                                "体系结构",
                                "描述",
                                "操作系统",
                                "文件选择",
                                "浏览......",
                                "上传",
                                "清除",
                                "请输入名称!",
                                "名称必须由数字、字母或下划线组成，且不能为空!",
                                "名称长度不能大于32!",
                                "描述信息长度不能大于128!",
                                "请选择文件!",
                                "打开文件"
                                };
    private String[] arrEnRes = {
                                "The corresponding directory of the remote server creation failed!",
                                "Remote server to create the directory!",
                                "Upload the file is successfully!",
                                "Upload file failed!",
                                "File already exists!",
                                "Remote file is larger than the local file!",
                                "resuming success!",
                                "resuming failed!",
                                "Failed to delete a remote file!",
                                "Query image",
                                "Have found the image, is being modified",
                                "Query image failed",
                                "To modify the image successfully!",
                                "Failed to modify the image",
                                "The image is being used.",
                                "Total time-consuming",
                                "hour",
                                "minute",
                                "second",
                                "name",
                                "type",
                                "container format",
                                "bus",
                                "arch",
                                "description",
                                "os_type",
                                "file select",
                                "browse......",
                                "upload",
                                "clear",
                                "Please enter a name!",
                                "The name must consist of numbers, letters or an underscore, and can not be empty!",
                                "Name length can not be greater than 32!",
                                "Description length is not greater than 128!",
                                "Please select the file!",
                                "Open File"
                                };
    

    public String GetResInfo(ResKey key){
        if(strLang.equals("ch")){
            return arrChRes[key.ordinal()];
        }
        else if(strLang.equals("en")){
            return arrEnRes[key.ordinal()];
        }
        return "";
    }
}
