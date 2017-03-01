/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�nfs_impl.cpp
* �ļ���ʶ��
* ����ժҪ������nfs��ӳ�����ģ��ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2012��3��12��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/3/12
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
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

    //cc��hc�ṩnfs��ʽ��͸��ͨ������Ҫ��ͨ�����ڵ�dir����nfs����
    //������ǰ׼���ý������ָ�hc��nfs����ip��ַ
    if (_config.server_ip.empty())
    {
        //����û�û��ָ�������nfs�����ַ����ʹ�ù���ƽ��ip
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
        //��ֹ��loopback�ӿ�������nfs���� ��CC����ip����hc û������ !
        OutPut(SYS_ERROR,"illegal nfs server address on loopback interface: %s!\n",
              _config.server_ip.c_str());
        return ERROR_INVALID_ARGUMENT;
    }

    //����nfs����Ŀ¼
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

    //�ڱ�cc��nfs����Ŀ¼�´���cacheĿ¼�����ڻ���image
    _cache_dir = _config.root_dir + "/cache";
    if ((0 != access(_cache_dir.c_str(),W_OK)) && (0 != mkdir(_cache_dir.c_str(),0600)))
    {
        error << "_cache_dir " << _cache_dir
        << " not available! errno = " << errno;
        OutPut(SYS_EMERGENCY,"%s\n",error.str().c_str());
        SkyExit(-1,error.str());
        return ERROR;
    }

    //���ȱ�֤nfs��������������
    EnsureNfsRunning();

    //��·��exportΪnfs����
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
        //nfs image agent͸������ֻ��֧������Ҳ��nfs�����
        //todo: �������֧�֣��ǲ���ԭ���ش�?
        SkyAssert(false);
        return ERROR_NOT_SUPPORT;
    }
   
    //�Ƚ�tc��nfs����Ŀ¼mount��cc����
    NfsAccessOption access_option;
    if (false == access_option.deserialize(cache._url.access_option))
    {
        return ERROR_INVALID_ARGUMENT;
    }

    if (true == IsLocalUrl(access_option.server_ip))
    {
        /* nfs server�����Լ�������Ҫmount*/
        localdir = access_option.shared_dir+"/" + to_string<int64>(cache._cache_id,dec);
        localfile = cache._url.path.substr(cache._url.path.find_first_of(':')+1);
        return SUCCESS;
    }
    
    //���nfs����ĵ�ַ��10.0.0.1:/var/lib/tecs/image����mount�����ص�λ��Ϊ:
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

    //������nfs�����Ѿ�mount����MountNfs�����᷵��ʵ��mount��·��
    //������Ҫȷ��һ���ǲ���mount������ϣ����_config.root_dirĿ¼��
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

    //����tc(192.168.0.1)�ṩ�ķ����Ŀ¼��/var/lib/tecs/image_repo/
    //ʵ�ʵ�ӳ���ļ�λ��/var/lib/tecs/image_repo/admin/ttylinux.img
    //������tc��url.pathΪ"192.168.0.1:/var/lib/tecs/image_repo/admin/ttylinux.img"  
    string nfs_server_uri = access_option.server_ip + ":" + access_option.shared_dir;
    localfile = cache._url.path;
    //������Ҫ��path�е�"192.168.0.1:/var/lib/tecs/image_repo"ȥ����ֻ���º���: "/admin/ttylinux.img"
    localfile.erase(0,nfs_server_uri.size());
    //Ȼ������new_root_dir,��"/_config.root_dir/10.0.0.1_var_lib_tecs_image"��������
    //���ɱ���·��"/_config.root_dir/10.0.0.1_var_lib_tecs_image/admin/ttylinux.img"
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
        /*nfs server ��cc��ͬһ̨������ ����Ҫ�޸�url*/
        return SUCCESS;
    }

    //�ȼ��һ���ǲ����Ѿ��������
    ImageCachePool* cache_pool = ImageCachePool::GetInstance();
    SkyAssert(NULL != cache_pool);
    ImageCache tmp(cache._cache_id);
    ret = cache_pool->GetCacheById(tmp);
    if (ret == SQLDB_OK && tmp._state == ICS_READY)
    {
        //ӳ���Ѿ��������
        OutPut(SYS_NOTICE,"cache %lld(%lld) is already cached as file %s.\n",cache._cache_id,cache._image_id,tmp._target_file.c_str());
        cache._url.path = _config.server_ip + ":" + tmp._target_file;
        cache._url.access_type = IMAGE_ACCESS_NFS;
        NfsAccessOption access_option(_config.server_ip,_config.root_dir);
        cache._url.access_option = access_option.serialize();
    }
    else
    {
        //���cc��û�л����ӳ������͸��
        //��Ҫ��mount������
        string localdir;
        string localfile;
        //GetLocalPath��������tc��url����mount�����أ��õ�һ�����ص�Ŀ¼��ȫ·��
        ret = GetLocalPath(cache,localdir,localfile);
        if (SUCCESS != ret)
        {
            return ret;
        }

        //����µ�mount·����Ȼ��λ��_config.root_dir����
        //������Ȼ��Ҫֱ�ӵ���������������_config.root_dir
        ret = ExportNfs(localdir,"*","");
        if(SUCCESS != ret)
        {
            error = "failed to exportfs!";
            OutPut(SYS_ERROR,"failed to exportfs %s! ret = %d\n",localdir.c_str(),ret);
            return ret;
        }

        //�������е�path�ĳ�"_config.server_ip:/_config.root_dir/10.0.0.1_var_lib_tecs_image/admin/ttylinux.img"
        cache._url.path = _config.server_ip + ":" + localfile;
        cache._url.access_type = IMAGE_ACCESS_NFS;
        //new_root_dir������"/_config.root_dir/10.0.0.1_var_lib_tecs_image"
        //���һ��:���������url�ķ���ѡ��
        NfsAccessOption access_option_updated(_config.server_ip,localdir);
        cache._url.access_option = access_option_updated.serialize();
    #if 0
        //��ӳ�񻺴�����
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

    //����Ҫ��ӳ��ӳ��Ϊ���ؿɷ��ʵ�·��
    OutPut(SYS_NOTICE,"NfsIAImpl will cache image %lld ...\n",cache._cache_id);
    ret = GetLocalPath(cache,localdir,localfile);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"failed to get image(%s) local path!\n",cache._url.path.c_str());
        return ret;
    }

    SkyAssert(false == localfile.empty());
    //�����Ҫ������ӳ���ļ�����ӳ��·���Ƿ����
    if (0 != access(localfile.c_str(),R_OK))
    {
        OutPut(SYS_ERROR,"access file %s failed! errno = %d\n",localfile.c_str(),errno);
        return ERROR_FILE_NOT_EXIST;
    }

    cache._state = ICS_NOSTATE;
    cache._source_file = "file://" + localfile;
    //_target_file�ڵ���֮ǰ���� ��Ҫ���
    //cache._target_file = _cache_dir + "/" + to_string<int64>(cache._id,dec) + ".img";
    if (cache._size == 0)
    {
        //�������������Ϣ��û����ȷӳ���С���������Ѿ����Է��ʣ���size��ȡ������¼��������Ϣ����
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
        //nfs image agent͸������ֻ��֧������Ҳ��nfs�����
        //todo: �������֧�֣��ǲ���ԭ���ش�?
        SkyAssert(false);
        return ERROR_NOT_SUPPORT;
    }
   
    //�Ƚ�tc��nfs����Ŀ¼mount��cc����
    NfsAccessOption access_option;
    if (false == access_option.deserialize(cache._url.access_option))
    {
        return ERROR_INVALID_ARGUMENT;
    }

    if (true == IsLocalUrl(access_option.server_ip))
    {
        return SUCCESS;
    }
    
    //���nfs����ĵ�ַ��10.0.0.1:/var/lib/tecs/image����mount�����ص�λ��Ϊ:
    //_config.root_dir/10.0.0.1_var_lib_tecs_image
    string tmp(access_option.server_ip + access_option.shared_dir);
    replace(tmp.begin(),tmp.end(), '/', '_');
    string mountdir = _config.root_dir + "/" + tmp + "/" + to_string<int64>(cache._cache_id,dec);

    return UnMountNfs(mountdir);
}

}


