/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：nfs_impl.cpp
* 文件标识：
* 内容摘要：基于nfs的映像代理模块实现文件
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2012年3月12日
*
* 修改记录1：
*     修改日期：2012/3/12
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
******************************************************************************/
#include "tecs_config.h"
#include "nfs.h"
#include "image_agent.h"
#include "log_agent.h"
#include <boost/algorithm/string.hpp>
#include "image_cache.h"
using namespace boost;

namespace zte_tecs
{
NfsIAImpl::NfsIAImpl(const NfsIAConfig& config):_config(config)
{
}

STATUS NfsIAImpl::Init()
{
    STATUS ret;
    ostringstream error;
    if (_config.root_dir.empty())
    {
        OutPut(SYS_ERROR,"empty image_tunnel_dir!\n");
        return ERROR_INVALID_ARGUMENT;
    }

    //cc向hc提供nfs方式的透传通道，需要将通道所在的dir加入nfs服务
    //并且提前准备好将来呈现给hc的nfs服务ip地址
    if (_config.server_ip.empty())
    {
        //如果用户没有指定另外的nfs服务地址，则使用管理平面ip
        TecsConfig *config = TecsConfig::Get();
        ret = GetIpAddress(config->get_management_interface(),_config.server_ip);
        if (SUCCESS != ret)
        {
            OutPut(SYS_ALERT, "Get ip address of management interface(%s) failed! ret = %d\n",
                  config->get_management_interface().c_str(),ret);
            SkyAssert(false);
            return ERROR;
        }
        OutPut(SYS_NOTICE, "nfs image agent uses management interface, ip = %s.\n",
              _config.server_ip.c_str());
    }
    else if (_config.server_ip == "localhost"  ||
             _config.server_ip == "*" ||
             0 == _config.server_ip.compare(0,3,"127"))
    {
        //禁止在loopback接口上启动nfs服务， 将CC本地ip带给hc 没有意义 !
        OutPut(SYS_ERROR,"illegal nfs server address on loopback interface: %s!\n",
              _config.server_ip.c_str());
        return ERROR_INVALID_ARGUMENT;
    }

    //创建nfs服务目录
    trim(_config.root_dir);
    regular_path(_config.root_dir);
    mkdir(_config.root_dir.c_str(),0600);
    _config.root_dir.append("/");
    _config.root_dir.append(ApplicationName());
    if (0 != access(_config.root_dir.c_str(),W_OK) && 0 != mkdir(_config.root_dir.c_str(),0600))
    {
        error << "_config.root_dir " << _config.root_dir
        << " not available! errno = " << errno;
        OutPut(SYS_DEBUG,"%s\n",error.str().c_str());
        SkyExit(-1,error.str());
    }

    //在本cc的nfs服务目录下创建cache目录，用于缓存image
    _cache_dir = _config.root_dir + "/cache";
    if ((0 != access(_cache_dir.c_str(),W_OK)) && (0 != mkdir(_cache_dir.c_str(),0600)))
    {
        error << "_cache_dir " << _cache_dir
        << " not available! errno = " << errno;
        OutPut(SYS_EMERGENCY,"%s\n",error.str().c_str());
        SkyExit(-1,error.str());
        return ERROR;
    }

    //首先保证nfs服务正在运行中
    EnsureNfsRunning();

    //将路径export为nfs服务
    OutPut(SYS_DEBUG,"exporting nfs directory %s\n",_config.root_dir.c_str());
    ret = ExportNfs(_config.root_dir,"*","");
    if (SUCCESS != ret)
    {
        error << "ExportNfs " << _config.root_dir
        << " failed! ret = " << ret;
        OutPut(SYS_EMERGENCY,"%s\n",error.str().c_str());
        SkyExit(-1,error.str());
    }

    return SUCCESS;
}

STATUS NfsIAImpl::GetLocalPath(ImageCache& cache,string& localdir,string& localfile)
{
    STATUS ret;

    if (cache._url.access_type != IMAGE_ACCESS_NFS)
    {
        //nfs image agent透传机制只能支持上游也是nfs的情况
        //todo: 如果不能支持，是不是原样回传?
        SkyAssert(false);
        return ERROR_NOT_SUPPORT;
    }
   
    //先将tc的nfs服务目录mount到cc本地
    NfsAccessOption access_option;
    if (false == access_option.deserialize(cache._url.access_option))
    {
        return ERROR_INVALID_ARGUMENT;
    }

    if (true == IsLocalUrl(access_option.server_ip))
    {
        /* nfs server就是自己，不需要mount*/
        localdir = access_option.shared_dir+"/" + to_string<int64>(cache._cache_id,dec);
        localfile = cache._url.path.substr(cache._url.path.find_first_of(':')+1);
        return SUCCESS;
    }
    
    //如果nfs服务的地址是10.0.0.1:/var/lib/tecs/image，则mount到本地的位置为:
    //_config.root_dir/10.0.0.1_var_lib_tecs_image
    string tmp(access_option.server_ip + access_option.shared_dir);
    replace(tmp.begin(),tmp.end(), '/', '_');
    localdir = _config.root_dir + "/" + tmp + "/" + to_string<int64>(cache._cache_id,dec);

    OutPut(SYS_DEBUG,"mounting nfs share %s:%s to %s ...\n",
          access_option.server_ip.c_str(),
          access_option.shared_dir.c_str(),
          localdir.c_str());

    ret = MountNfs(access_option.server_ip,access_option.shared_dir,localdir,TO_FORCE_DIR);
    if (SUCCESS != ret)
    {
        return ret;
    }

    //如果这个nfs服务已经mount过，MountNfs函数会返回实际mount的路径
    //这里需要确认一下是不是mount在我们希望的_config.root_dir目录下
    if (0 != localdir.compare(0,_config.root_dir.size(),_config.root_dir))
    {
        OutPut(SYS_ERROR,"nfs url %s:%s is already mounted on dir %s!\n",
              access_option.server_ip.c_str(),
              access_option.shared_dir.c_str(),
              localdir.c_str());
        return ERROR;
    }

    regular_path(access_option.shared_dir);
    regular_path(localdir);

    //假设tc(192.168.0.1)提供的服务根目录是/var/lib/tecs/image_repo/
    //实际的映像文件位于/var/lib/tecs/image_repo/admin/ttylinux.img
    //则来自tc的url.path为"192.168.0.1:/var/lib/tecs/image_repo/admin/ttylinux.img"  
    string nfs_server_uri = access_option.server_ip + ":" + access_option.shared_dir;
    localfile = cache._url.path;
    //这里是要将path中的"192.168.0.1:/var/lib/tecs/image_repo"去掉，只留下后半截: "/admin/ttylinux.img"
    localfile.erase(0,nfs_server_uri.size());
    //然后将其与new_root_dir,即"/_config.root_dir/10.0.0.1_var_lib_tecs_image"进行连接
    //生成本地路径"/_config.root_dir/10.0.0.1_var_lib_tecs_image/admin/ttylinux.img"
    localfile = localdir + localfile;
    return SUCCESS;
}

STATUS NfsIAImpl::GetMappedUrl(ImageCache& cache,string& error)
{
    STATUS ret;
    NfsAccessOption option;

    if (false == option.deserialize(cache._url.access_option))
    {
        return ERROR_INVALID_ARGUMENT;
    }

    if (true == IsLocalUrl(option.server_ip))
    {
        /*nfs server 和cc在同一台机器上 不需要修改url*/
        return SUCCESS;
    }

    //先检查一下是不是已经缓存过了
    ImageCachePool* cache_pool = ImageCachePool::GetInstance();
    SkyAssert(NULL != cache_pool);
    ImageCache tmp(cache._cache_id);
    ret = cache_pool->GetCacheById(tmp);
    if (ret == SQLDB_OK && tmp._state == ICS_READY)
    {
        //映像已经缓存好了
        OutPut(SYS_NOTICE,"cache %lld(%lld) is already cached as file %s.\n",cache._cache_id,cache._image_id,tmp._target_file.c_str());
        cache._url.path = _config.server_ip + ":" + tmp._target_file;
        cache._url.access_type = IMAGE_ACCESS_NFS;
        NfsAccessOption access_option(_config.server_ip,_config.root_dir);
        cache._url.access_option = access_option.serialize();
    }
    else
    {
        //如果cc上没有缓存该映像，则尝试透传
        //需要先mount到本地
        string localdir;
        string localfile;
        //GetLocalPath负责将来自tc的url首先mount到本地，得到一个本地的目录和全路径
        ret = GetLocalPath(cache,localdir,localfile);
        if (SUCCESS != ret)
        {
            return ret;
        }

        //这个新的mount路径虽然是位于_config.root_dir下面
        //但是仍然需要直接导出，不能依赖于_config.root_dir
        ret = ExportNfs(localdir,"*","");
        if(SUCCESS != ret)
        {
            error = "failed to exportfs!";
            OutPut(SYS_ERROR,"failed to exportfs %s! ret = %d\n",localdir.c_str(),ret);
            return ret;
        }

        //最后将入参中的path改成"_config.server_ip:/_config.root_dir/10.0.0.1_var_lib_tecs_image/admin/ttylinux.img"
        cache._url.path = _config.server_ip + ":" + localfile;
        cache._url.access_type = IMAGE_ACCESS_NFS;
        //new_root_dir现在是"/_config.root_dir/10.0.0.1_var_lib_tecs_image"
        //最后一步:更新入参中url的访问选项
        NfsAccessOption access_option_updated(_config.server_ip,localdir);
        cache._url.access_option = access_option_updated.serialize();
    #if 0
        //将映像缓存起来
        if(true == _config.auto_cache)
        {
            CacheImage(cache,error);
        }
    #endif
    }
    return SUCCESS;
}

STATUS NfsIAImpl::CachePrepare(ImageCache & cache)
{
    STATUS ret;
    string localdir;
    string localfile;

    //首先要把映像映射为本地可访问的路径
    OutPut(SYS_NOTICE,"NfsIAImpl will cache image %lld ...\n",cache._cache_id);
    ret = GetLocalPath(cache,localdir,localfile);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"failed to get image(%s) local path!\n",cache._url.path.c_str());
        return ret;
    }

    SkyAssert(false == localfile.empty());
    //检查需要拷贝的映像文件本地映射路径是否存在
    if (0 != access(localfile.c_str(),R_OK))
    {
        OutPut(SYS_ERROR,"access file %s failed! errno = %d\n",localfile.c_str(),errno);
        return ERROR_FILE_NOT_EXIST;
    }

    cache._state = ICS_NOSTATE;
    cache._source_file = "file://" + localfile;
    //_target_file在调用之前生成 需要入库
    //cache._target_file = _cache_dir + "/" + to_string<int64>(cache._id,dec) + ".img";
    if (cache._size == 0)
    {
        //如果缓存请求消息中没有明确映像大小，趁这里已经可以访问，将size获取到并记录到缓存信息表中
        struct stat64 s;
        if (0 == stat64(localfile.c_str(),&s))
        {
            cache._size = s.st_size;
        }
    }
    return SUCCESS;
}
STATUS NfsIAImpl::CacheRelease(ImageCache & cache)
{
    if (cache._url.access_type != IMAGE_ACCESS_NFS)
    {
        //nfs image agent透传机制只能支持上游也是nfs的情况
        //todo: 如果不能支持，是不是原样回传?
        SkyAssert(false);
        return ERROR_NOT_SUPPORT;
    }
   
    //先将tc的nfs服务目录mount到cc本地
    NfsAccessOption access_option;
    if (false == access_option.deserialize(cache._url.access_option))
    {
        return ERROR_INVALID_ARGUMENT;
    }

    if (true == IsLocalUrl(access_option.server_ip))
    {
        return SUCCESS;
    }
    
    //如果nfs服务的地址是10.0.0.1:/var/lib/tecs/image，则mount到本地的位置为:
    //_config.root_dir/10.0.0.1_var_lib_tecs_image
    string tmp(access_option.server_ip + access_option.shared_dir);
    replace(tmp.begin(),tmp.end(), '/', '_');
    string mountdir = _config.root_dir + "/" + tmp + "/" + to_string<int64>(cache._cache_id,dec);

    return UnMountNfs(mountdir);
}

}


