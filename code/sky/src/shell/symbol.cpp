/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：symbol.cpp
* 文件标识：见配置管理计划书
* 内容摘要：符号表管理模块实现文件
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2011年8月16日
*
* 修改记录1：
*     修改日期：2011/07/01
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
*******************************************************************************/
#include "common.h"
#include "symbol.h"

static int sym_print_on = 0;  
DEFINE_DEBUG_VAR(sym_print_on);
#define Debug(fmt,arg...) \
        do \
        { \
            if(sym_print_on) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)
static bool WildMatch(const char *wild, const char *string);

class MemoryPool
{
    #define ALIGNMENT sizeof(void *)
    #define STATIC_POOL_SIZE (64 * 1024)
    #define DYNAMIC_POOL_SIZE (64 * 1024)
public:
    MemoryPool()
    {
        memset(m_static_memory,0,sizeof(m_static_memory));
        Init();
    }

    ~MemoryPool()
    {
        Clear();
    }

    char *AllocateString(const char *source = 0, std::size_t size = 0)
    {
        //开始地址和长度必须至少指定一个
        SkyAssert(source || size);     
        if (size == 0)
            size = Measure(source) + 1;

        //分配一段内存，将字符串拷贝过来
        char *result = static_cast<char *>(AllocateAligned(size * sizeof(char)));
        if(!result)
            return NULL;
        
        if (source)
        {
            for (std::size_t i = 0; i < size; ++i)
            {
                result[i] = source[i];
            }
        }
        return result;
    }

    void Clear()
    {
        while (m_begin != m_static_memory)
        {
            //逐段释放内存
            char *previous_begin = reinterpret_cast<header *>(Align(m_begin))->previous_begin;
            delete[] m_begin;
            m_begin = previous_begin;
        }
        Init();
    }

    uint32 PoolSize() const
    {
        return m_dynamic_size + STATIC_POOL_SIZE;
    }

private:
    struct header
    {
        char *previous_begin;
    };

    void Init()
    {
        m_dynamic_size = 0;
        m_begin = m_static_memory;
        m_ptr = Align(m_begin);
        m_end = m_static_memory + sizeof(m_static_memory);
    }

    size_t Measure(const char *p)
    {
        const char *tmp = p;
        while (*tmp) 
            ++tmp;
        return tmp - p;
    }
        
    char *Align(char *ptr)
    {
        std::size_t alignment = ((ALIGNMENT - (std::size_t(ptr) & (ALIGNMENT - 1))) & (ALIGNMENT - 1));
        return ptr + alignment;
    }

    char *AllocateRaw(std::size_t size)
    {
        //分配新的原始内存
        void *memory = new char[size];
        if (!memory)     
            return NULL;
        Debug("Allocate raw memory!\n");
        m_dynamic_size += size;
        return static_cast<char *>(memory);
    }

    void *AllocateAligned(std::size_t size)
    {
        // Calculate aligned pointer
        char *result = Align(m_ptr);

        // If not enough memory left in current pool, allocate a new pool
        if (result + size > m_end)
        {
            // Calculate required pool size (may be bigger than RAPIDXML_DYNAMIC_POOL_SIZE)
            std::size_t pool_size = DYNAMIC_POOL_SIZE;
            if (pool_size < size)
                pool_size = size;

            // Allocate
            std::size_t alloc_size = sizeof(header) + (2 * ALIGNMENT - 2) + pool_size;     
            // 2 alignments required in worst case: one for header, one for actual allocation
            char *raw_memory = AllocateRaw(alloc_size);
            if(!raw_memory)
                return NULL;
            
            // Setup new pool in allocated memory
            char *pool = Align(raw_memory);
            header *new_header = reinterpret_cast<header *>(pool);
            new_header->previous_begin = m_begin;
            m_begin = raw_memory;
            m_ptr = pool + sizeof(header);
            m_end = raw_memory + alloc_size;

            // Calculate aligned pointer again using new pool
            result = Align(m_ptr);
        }

        // Update pool and return aligned pointer
        m_ptr = result + size;
        return result;
    }

    uint32 m_dynamic_size;
    char *m_begin;    // Start of raw memory making up current pool
    char *m_ptr;      // First free byte in current pool
    char *m_end;      // One past last available byte in current pool
    char m_static_memory[STATIC_POOL_SIZE];    // Static raw memory
};

Symbol::Symbol()
{
    _name = NULL;
    _type = SYMBOL_UNKNOWN;
    _addr = 0;
    _len = 0;
}

Symbol::Symbol(const char* name,SymbolType type,uintptr addr,uint32 len)
{
    _name = name;
    _type = type;
    _addr = addr;
    _len = len;
}

void Symbol::Assign(uintptr value) const
{
    if (!_addr)
    {
        return;
    }

    switch (_len)
    {
    case 1:
        *((int8 *)_addr) = (int8)value;
        break;

    case 2:
        *((int16 *)_addr) = (int16)value;
        break;

    case 4:
        *((int32 *)_addr) = (int32)value;
        break;

    case 8:
        *((int64 *)_addr) = (int64)value;
        break;

    default:
        break;
    }
}

void Symbol::Display() const
{
    if (!_addr)
    {
        return;
    }

    printf("%s[size=%u] value = %.*lu = 0x%.*lx\n",_name,_len,_len,*(uintptr *)_addr,_len,*(uintptr *)_addr);
}

struct strCmp
{
    bool operator()( const char* s1, const char* s2 ) const
    {
        return strcmp( s1, s2 ) < 0;
    }
};

class SymbolRegistry:public MemoryPool
{
public:
    SymbolRegistry() {};
    STATUS RegisterSymbol(const char* name, SymbolType type, uintptr addr, uint32 len);
    static SymbolRegistry* GlobalRegistry();
    const Symbol* FindSymbol(const char* name);
    void DisplaySymbols(const char* wild);
private:
    typedef map<const char*, Symbol,strCmp> SymbolMap;
    typedef SymbolMap::iterator SymbolIterator;
    SymbolMap _symbols;
    static SymbolRegistry* _global_registry;
};

SymbolRegistry* SymbolRegistry::_global_registry = NULL;

SymbolRegisterer::SymbolRegisterer(const char *name, SymbolType type, uintptr addr, uint32 len)
{
    if(!name)
        return;
    
    if (strlen(name) >= MAX_SYM_LEN)
    {
        printf("warning! symbol %s too long!\n",name);
        return;
    }

    //只接受基本数据类型
    if (type == SYMBOL_VAR &&
            len != 1 &&
            len != 2 &&
            len != 4 &&
            len != 8)
    {
        printf("warning! symbol %s has an invalid length(%d bytes)!\n",name,len);
        return;
    }

    STATUS ret = SymbolRegistry::GlobalRegistry()->RegisterSymbol(name,type,addr,len);
    if (SUCCESS != ret)
    {
        printf("warning! symbol %s add failed!\n",name);
    }
    Debug("symbol %s added.\n",name);
}

SymbolRegistry* SymbolRegistry::GlobalRegistry()
{
    if (!_global_registry)
    {
        _global_registry = new SymbolRegistry();
    }
    return _global_registry;
}

STATUS SymbolRegistry::RegisterSymbol(const char *name, SymbolType type, uintptr addr, uint32 len)
{
    if(!name)
        return ERROR_INVALID_ARGUMENT;
    
    //符号注册表中的元素不允许重复
    if (_symbols.find(name) != _symbols.end())
    {
        return ERROR_DUPLICATED_OP;
    }

    char *symbol_name = AllocateString(name);
    Symbol new_symbol(symbol_name,type,addr,len);
    _symbols[name] = new_symbol;
    return SUCCESS;
}

const Symbol* SymbolRegistry::FindSymbol(const char *name)
{
    if(!name)
        return NULL;
    
    SymbolIterator it = _symbols.find(name);
    if (it != _symbols.end())
    {
        return &(it->second);
    }
    else
    {
        return NULL;
    }
}

void SymbolRegistry::DisplaySymbols(const char* wild)
{
    int index = 1;
    SymbolIterator it;
    printf("---------------------symbols---------------------\n");
    printf("%-6s %-32s %-4s %-4s\n","index","name","type","size");
    for ( it=_symbols.begin() ; it != _symbols.end(); it++ )
    {
        if(wild && false == WildMatch(wild,it->first))
        {
            continue;
        }
   
        printf("%-6d %-32s %-4d %-4d\n",index++,it->first,it->second._type,it->second._len);
    }
    printf("symbols pool size: %d bytes\n",PoolSize());
}

static bool WildMatch(const char *wild, const char *str)
{
    const char *cp = NULL, *mp = NULL;
    SkyAssert(NULL != wild && NULL != str);
    
    /* 未出现星号时，逐个字符比较 */
    while ((*str) && (*wild != '*'))
    {
        if ((*wild != *str) && (*wild != '?'))
        {
            /* 字符不同，且wild又不是'?'，匹配失败 */
            return false;
        }
        wild++;
        str++;
    }

    /* wild中出现了星号 */
    while (*str)
    {
        if (*wild == '*')
        {
            /* 星号后面再没有其他字符时，直接匹配成功 */
            if (!*++wild)
            {
                return true;
            }
            mp = wild;
            cp = str+1;
        }
        else if ((*wild == *str) || (*wild == '?'))
        {
            wild++;
            str++;
        }
        else
        {
            wild = mp;
            str = cp++;
        }
    }

    while (*wild == '*')
    {
        wild++;
    }
    return !*wild;
}

const Symbol *GetSymbol(char *name)
{
    return SymbolRegistry::GlobalRegistry()->FindSymbol(name);
}

static void symfind(const char* wild)
{
    SymbolRegistry::GlobalRegistry()->DisplaySymbols(wild);
}

static void symfindall()
{
    symfind(NULL);
}

DEFINE_DEBUG_CMD(sym,symfindall);
DEFINE_DEBUG_CMD(help,symfindall);
DEFINE_DEBUG_FUNC(symfind);

