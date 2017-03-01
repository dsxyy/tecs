/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：api_pub.h
* 文件标识：
* 内容摘要：api和TECS内部均可见的公共文件
* 当前版本：1.0
* 作    者：Bob
* 完成日期：2011年10月19日
*
* 修改记录1：
*    修改日期：2011/7/26
*    版 本 号：V1.0
*    修 改 人：Bob
*    修改内容：创建
*******************************************************************************/
#ifndef TECS_API_PUB_H
#define TECS_API_PUB_H
#include <stdio.h>


#define STR_LEN_32    ((int)32)
#define STR_LEN_64    ((int)64)
#define STR_LEN_128   ((int)128)
#define STR_LEN_256   ((int)256)
#define STR_LEN_512   ((int)512)
    

#define VALID_NORMAL_NAME_CHAR(c)   \
    ((((c) >= '0') && ((c) <= '9')) || (((c) >= 'a') && ((c) <= 'z')) || \
     (((c) >= 'A') && ((c) <= 'Z')) || ((c) == '_') || ((c) == '-'))

#define VALID_EXTENDED_NAME_CHAR(c)   \
    ( (((c) >= '0') && ((c) <= '9')) || (((c) >= 'a') && ((c) <= 'z')) || \
      (((c) >= 'A') && ((c) <= 'Z')) || ((c) == '_') || ((c) == '-') || \
      ((c) == '@')||((c) == '.') )

#define VALID_COMPLEX_NAME_CHAR(c)   \
    (((c) != '/')&& ((c) != '\\') && ((c) != ':') &&\
     ((c) != '*')&& ((c) != '?')  && ((c) != '"') && \
     ((c) != '<') && ((c) != '>')  && ((c) != '|')&& \
     ((c) != '&') && ((c) != '$')  && ((c) != ' ')&& \
     ((c) != '+') && ((c) != '=')  && ((c) != '%'))

class StringCheck
{
public:
    enum NameType
    {
        NORMAL_NAME    = 0, // 数字、字母、中划线、下划线，必须以字母或下划线打头。
        EXTENDED_NAME  = 1, //数字、字母、中划线、下划线，随便打头，外加支持@和.
        COMPLEX_NAME   = 2  // 满足linux下文件名称的命名规则
    };

    static bool CheckNormalName(const string &name, 
                                         uint32 len_min,
                                         uint32 len_max)
    {
         return CheckString(name, len_min, len_max, StringCheck::NORMAL_NAME);
    }
    
    static bool CheckExtendedName(const string &name,  
                                         uint32 len_min,
                                         uint32 len_max)
    {
         return CheckString(name, len_min, len_max, StringCheck::EXTENDED_NAME);
    }

    static bool CheckComplexName(const string &name,  
                                         uint32 len_min,
                                         uint32 len_max)
    {
         return CheckString(name, len_min, len_max, StringCheck::COMPLEX_NAME);
    }

    static bool CheckIsAllSpace(const string &in_str)
    {
        if (in_str.empty())
        {
           return false;
        }

        const char *ch = in_str.c_str();        
        for(uint32 i = 0; i < in_str.size(); i++, ch++)
        {
            if(*ch != ' ')
            {
               return false;
            }
        }
        return true;
    };

    static bool CheckSize(const string &in_str, 
                                uint32 len_min,
                                uint32 len_max)
    {
        if ((in_str.size() < len_min) || (in_str.size() > len_max))
        {
            return false;
        }
        return true;
    }      
   
private:  

   static bool CheckString(const string &in_str,
                                uint32 len_min,
                                uint32 len_max,                                
                                NameType name_type)
    {
        if ( (in_str.size() < len_min)
              ||(in_str.size() > len_max))
        {
            return false;
        }

        if(NORMAL_NAME == name_type)
        {
           return NormalName(in_str);
        }
        else if(EXTENDED_NAME == name_type)
        {
           return ExtendName(in_str);
        }
        else if(COMPLEX_NAME == name_type)
        {
           return ComplexName(in_str);
        }

        return true;
    }

    static bool NormalName(const string &in_str)
    {
        bool  first_char = true;
        
        const char *c = in_str.c_str();
        
        for (uint32 i = 0; i < in_str.size(); i++, c++)
        {
            if (first_char)
            {   
                //首字母检查
                if (   (((*c) >= 'a') && ((*c) <= 'z'))  
                     ||(((*c) >= 'A') && ((*c) <= 'Z')) 
                     ||(((*c) >= '0') && ((*c) <= '9'))
                     ||((*c) == '_'))
                {
                    first_char = false;
                    continue;
                }
                return false; 
            }
            
            if (!VALID_NORMAL_NAME_CHAR(*c))
            {
               return false;
            }
        }
        return true;
    }

    static bool ExtendName(const string &in_str)
    {
        const char *c = in_str.c_str();
        for (uint32 i = 0; i < in_str.size(); i++, c++)
        {
            if (!VALID_EXTENDED_NAME_CHAR(*c))
            {
                return false;
            }
        }
        return true;
    }

    static bool ComplexName(const string &in_str)
    {
        const char *c = in_str.c_str();
        //
        if ((in_str == ".")||(in_str == "..") )
        {
            return false;
        }
         
        for (uint32 i = 0; i < in_str.size(); i++, c++)
        {
            if (!VALID_COMPLEX_NAME_CHAR(*c))
            {
                return false;
            }
        }
        return true;
    }
    
};


#endif

