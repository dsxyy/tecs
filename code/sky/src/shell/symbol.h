#ifndef SKY_SYMBOL_H
#define SKY_SYMBOL_H

#define MAX_SYM_LEN 128

class Symbol
{
public:
    Symbol();
    Symbol(const char* name,SymbolType type,uintptr addr,uint32 len);
    void Assign(uintptr value) const;
    void Display() const;
    
    const char *_name;
    SymbolType _type;
    uintptr    _addr;
    uint32     _len;
};

const Symbol *GetSymbol(char *name);
#endif
