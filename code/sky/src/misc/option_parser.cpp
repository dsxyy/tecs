#include "common.h"
#include "option_parser.h"
#include "ini_file.h"
class Option
{
public:
    Option(const string& name, const string& description);
    virtual ~Option() {};
    virtual void SetValue(const string&) = 0;
    //是否需要输入参数值? 如果是布尔开关，一般是不需要的
    virtual bool IsValueExpected() = 0;
    bool Match(const string&);
    ostream& Print(ostream& out);
    ostream& AllStrPrint(const string & print,ostream& out);
    ostream& DescriptionStrPrint(const string & print,ostream& out);		
    virtual ostream& PrintValue(ostream& out) = 0;
protected:
    string longName;
    string shortName;
    string description;
    ostream& PrintNames(ostream& out);
    friend class OptionParser;
};

class StringOption : public Option
{
public:
    StringOption(const string& name, const string& description, string& v) : 
        Option(name, description), value(v)
    {}
    void SetValue(const string& v)
    {
        value = v;
    }
    bool IsValueExpected()
    {
        return true;
    }

    ostream& PrintValue(ostream& out)
    {
        if (longName.size())
        {
            out << longName << " = " << value << endl;
        }
        else
        {
            out << "-" << shortName << " = " << value << endl;
        }
        return out;
    }
private:
    string& value;
};

class IntegerOption : public Option
{
public:
    IntegerOption(const string& name, const string& description, int& v) : 
        Option(name, description), value(v)
    {}
    void SetValue(const string& v)
    {
        value = atoi(v.c_str());
    }
    bool IsValueExpected()
    {
        return true;
    }
    
    ostream& PrintValue(ostream& out)
    {
        if (longName.size())
        {
            out << longName << " = " << value << endl;
        }
        else
        {
            out << "-" << shortName << " = " << value << endl;
        }
        return out;
    }
private:
    int& value;
};

class BooleanOption : public Option
{
public:
    BooleanOption(const string& name, const string& description, bool& v) : 
        Option(name, description), value(v)
    {}
    void SetValue(const string&)
    {
        value = true;
    }
    bool IsValueExpected()
    {
        return false;
    }
    
    ostream& PrintValue(ostream& out)
    {
        if (longName.size())
        {
            out << longName << " = " << value << endl;
        }
        else
        {
            out << "-" << shortName << " = " << value << endl;
        }
        return out;
    }
private:
    bool& value;
};

class MultiStringOption : public Option
{
public:
    MultiStringOption(const string& name, const string& description, vector<string>& v) : 
        Option(name, description), value(v)
    {}
    void SetValue(const string& v)
    {
        value.push_back(v);
    }
    bool IsValueExpected()
    {
        return true;
    }
    ostream& PrintValue(ostream& out)
    {
        if (longName.size())
        {
            out << longName << " = ";
        }
        else
        {
            out << "-" << shortName << " = ";
        }

        vector<string>::iterator it;
        for(it = value.begin();it != value.end(); it++)
        {
            out << " " << *it; 
        }
        out << endl;
        return out;
    }
private:
    vector<string>& value;
};

class OptionMatch
{
public:
    OptionMatch(const string& argument);
    bool operator()(Option* option);
    bool isOption();
private:
    string name;
};

class OptionsError : public exception
{
public:
    OptionsError(const string& message) {};
};

Option::Option(const string& name, const string& desc) : description(desc)
{
    string::size_type i = name.find(",");
    if (i != string::npos)
    {
        //如果是一个中间有逗号的字符串，逗号之前是长名，后面是短名
        longName = name.substr(0, i);
        if (i + 1 < name.size())
            shortName = name.substr(i+1);
    }
    else
    {
        //如果只有一个单词，则认为是长名称
        longName = name;
    }
}

bool Option::Match(const string& name)
{
    return name == longName || name == shortName;
}

/*描述部分排版函数*/
ostream& Option::DescriptionStrPrint(const string & print,ostream& out)
{
    string           strSub;
    char             buf[LENGTH_OF_PRINTLINE + 1];
    int              index;
    string           tmpString = print;
	
    while(tmpString.length() > LENGTH_WITH_DESCRIPTION)
    {
         /* 保证单词不能被拆开*/
         if(' ' != tmpString.at(LENGTH_WITH_DESCRIPTION - 1))
         {
             /*从后往前找，找到第一个空格位置*/
             index = tmpString.find_last_of(' ',LENGTH_WITH_DESCRIPTION - 1);
             /*截取index 开外的字符*/
             strSub = tmpString.substr(index + 1);
         		 
             /*加入结束符*/
             tmpString.copy(buf,index);
             buf[index] = '\0';
             /*描述项换行之后，需要对齐*/
             out.width(index+ LENGTH_WITHOUT_DESCRIPTION );	
         }
         else
         {
             /*截取LENGTH_WITH_DESCRIPTION 开外的字符*/
             strSub = tmpString.substr(LENGTH_WITH_DESCRIPTION);
             /*加入结束符*/	
             tmpString.copy(buf,LENGTH_WITH_DESCRIPTION - 1);       
             buf[LENGTH_WITH_DESCRIPTION] = '\0';
     				
              /*描述项换行之后，需要对齐*/
              out.width(LENGTH_WITH_DESCRIPTION + LENGTH_WITHOUT_DESCRIPTION );
          }	    		
         
           /*加入换行符*/        	  
           out<<buf<<'\n';
         	 
           tmpString = strSub;
       }  

	/*描述项换行之后，需要对齐*/
       out.width(tmpString.length() + LENGTH_WITHOUT_DESCRIPTION );	

       out<< tmpString<<'\n';
	
       return out;	
		
}

/*整体排版*/
ostream& Option::AllStrPrint(const string & print,ostream& out)
{
    string           strSub;
    char             buf[LENGTH_OF_PRINTLINE + 1];
    int              index;
    string           tmpString = print;
	
    if(tmpString.length() > LENGTH_OF_PRINTLINE)
    {
        /* 保证单词不能被拆开*/
        if(' ' != tmpString.at(LENGTH_OF_PRINTLINE - 1))
        {
            /*从后往前找，找到第一个空格位置*/
            index = tmpString.find_last_of(' ',LENGTH_OF_PRINTLINE - 1);
     	     /*截取index 开外的字符*/
            strSub = tmpString.substr(index + 1);
     		 
     	     /*加入结束符*/
     	     tmpString.copy(buf,index);
            buf[index] = '\0';
        }
        else
        {
            /*截取LENGTH_OF_PRINTLINE 开外的字符*/
            strSub = tmpString.substr(LENGTH_OF_PRINTLINE);
     	      /*加入结束符*/	
     	     tmpString.copy(buf,LENGTH_OF_PRINTLINE - 1);       
     	     buf[LENGTH_OF_PRINTLINE] = '\0';
        }	 
     		
     	  /*加入换行符*/        	  
     	 out<<buf<<'\n'; 	 
        tmpString = strSub;
        DescriptionStrPrint(tmpString,out);	 
   } 
   else
   {
       out<< tmpString<<'\n';
   }

   return out;		 
}

ostream& Option::PrintNames(ostream& out)
{
    string PrintString;

    /*把所有的字符串连接起来*/	
    if (shortName.size())
    {
        PrintString = "-" + shortName;

        if (IsValueExpected()) 
        {
            PrintString +=  " VALUE"; 
	    PrintString +=  ", --";
	    PrintString +=  longName;
	    PrintString +=  " VALUE " ;
        }
        else
        {  
	    PrintString +=  ", --";
	    PrintString +=  longName;
        }
	           	
    }
    else
    {		 
        if (IsValueExpected())
        {
	    PrintString += "--" ;
	    PrintString += longName;
	    PrintString += " VALUE ";     

        }
        else
        {  
	    PrintString += "--" ;
	    PrintString += longName; 
        }
		 
    }

    /*不含描述字符的配置项长度设置为30, 空余部分用空格补齐*/
    if(PrintString.length() < LENGTH_WITHOUT_DESCRIPTION)
    {
        PrintString.append((LENGTH_WITHOUT_DESCRIPTION - PrintString.length()),' ');
    }	

    out<<PrintString;   

    return out;
}

ostream& Option::Print(ostream& out)
{
    stringstream     names;
    string           PrintAll;
    string           strSub;
	
    PrintNames(names);

    PrintAll = names.str();

    /*配置项长度超过了LENGTH_WITHOUT_DESCRIPTION*/
    if(PrintAll.length() > LENGTH_WITHOUT_DESCRIPTION)
    {
        /*换行*/
        out<<names.str()<<'\n';
	PrintAll = description;
	 /*换行之后，描述部分排版*/
        DescriptionStrPrint(PrintAll,out);
    }
    else
    {
         /*需要输出的所有内容，中间用空格连接起来*/
         PrintAll = names.str() + description;     
	  /*整体排版*/
         AllStrPrint(PrintAll,out);
    }
	
     return out;
}

vector<string>& OptionParser::getArguments()
{
    return arguments;
}

void OptionParser::Add(Option* option)
{
    options.push_back(option);
}

void OptionParser::Add(const string& name, string& value, const string& description)
{
    Add(new StringOption(name, description, value));
}

void OptionParser::Add(const string& name, int& value, const string& description)
{
    Add(new IntegerOption(name, description, value));
}

void OptionParser::Add(const string& name, bool& value, const string& description)
{
    Add(new BooleanOption(name, description, value));
}

void OptionParser::Add(const string& name, vector<string>& value, const string& description)
{
    Add(new MultiStringOption(name, description, value));
}

OptionMatch::OptionMatch(const string& argument)
{
    if (argument.find("--") == 0)
    {
        name = argument.substr(2);
    }
    else if (argument.find("-") == 0)
    {
        name = argument.substr(1);
    }
}

bool OptionMatch::operator()(Option* option)
{
    return option->Match(name);
}

bool OptionMatch::isOption()
{
    return name.size() > 0;
}

OptionParser::OptionParser(const string& s, const string& d) : summary(s), description(d), help(false)
{
    Add("help,h", help, "Show this message.");
}

Option* OptionParser::getOption(const string& argument)
{
    OptionMatch match(argument);
    if (match.isOption())
    {
        Options::iterator i = find_if(options.begin(), options.end(), match);
        if (i == options.end())
        {
            cout<<"the option: "<<argument<<" is not exist, please try -h or --help for more information"<<endl;
	    return 0;
        }
        else
        {
            return *i;
        }
    }
    else
    {
        return 0;
    }
}

void OptionParser::error(const string& message)
{
    cout << summary << endl << endl;
    cerr << "Error: " << message << "; try --help for more information" << endl;
}

void OptionParser::PrintHelp()
{
    //如果help开关打开，仅仅打印帮助信息之后返回错误
    cout << summary << endl << endl;
    cout << description << endl << endl;
    cout << "Options: " << endl;
    for (Options::iterator i = options.begin(); i != options.end(); ++i)
    {
        (*i)->Print(cout);
    }
}

void OptionParser::PrintAll()
{
    for (Options::iterator i = options.begin(); i != options.end(); ++i)
    {
        (*i)->PrintValue(cout);
    }
}


/*如果参数中不在已知选项中，则返回ERROR_OPTION_NOT_EXIST*/
STATUS OptionParser::ParseCmdArgs(int argc, char** argv)
{
    try
    {
        for (int i = 1; i < argc; ++i)
        {
            string argument = argv[i];
            Option* o = getOption(argument);
            if (o)
            {
                if (o->IsValueExpected())
                {
                    if (i + 1 < argc)
                    {
                        o->SetValue(argv[++i]);
                    }
                    else
                    {
                        stringstream error;
                        error << "Value expected for option " << o->longName;
                        throw OptionsError(error.str());
                    }
                }
                else
                {
                    o->SetValue("");
                }
            }
            else
            {
                //不属于任何选项的命令行参数
                arguments.push_back(argument);
	            return ERROR_OPTION_NOT_EXIST;
            }
        }
        
        if (help)
        {
            //如果help开关打开，仅仅打印帮助信息之后返回错误
            PrintHelp();
            return SUCCESS;
        }
    }
	
    catch (const exception& e)
    {
        error(e.what());
        return ERROR;
    }
	
	return SUCCESS;
}

//从ini文件解析选项
bool OptionParser::ParseIniFile(const char* filename,const char* section)
{   
    ifstream in(filename);
    if (!in)
    {
        cout << "could not open file \"" << filename << '"' << endl;
        return false;
    }

    IniFile ini(in);
    if (!ini.IsRight())
    {
        cout << "could not parse \"" << ini.GetErrorData()<< '"' << endl;
        return false;
    }
    vector<Option*>::iterator it;
    for(it=options.begin();it!=options.end();it++)
    {
        //cout << "finding value for option " << (*it)->longName << endl;
        string value = ini.GetValue(section, (*it)->longName);
        if(value.empty())
        {
            continue;
        }
        
        //cout << (*it)->longName << " value = " << value << endl;
        if ((*it)->IsValueExpected())
        {
            (*it)->SetValue(value);
        }
        else if(value == "yes")
        {
            (*it)->SetValue("");
        }
    }
  
    return true;
}
    
OptionParser::~OptionParser()
{
    for (Options::iterator i = options.begin(); i != options.end(); ++i)
    {
        delete *i;
    }
}



