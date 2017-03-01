#include "common.h"
#include "option_parser.h"
#include "ini_file.h"
class Option
{
public:
    Option(const string& name, const string& description);
    virtual ~Option() {};
    virtual void SetValue(const string&) = 0;
    //�Ƿ���Ҫ�������ֵ? ����ǲ������أ�һ���ǲ���Ҫ��
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
        //�����һ���м��ж��ŵ��ַ���������֮ǰ�ǳ����������Ƕ���
        longName = name.substr(0, i);
        if (i + 1 < name.size())
            shortName = name.substr(i+1);
    }
    else
    {
        //���ֻ��һ�����ʣ�����Ϊ�ǳ�����
        longName = name;
    }
}

bool Option::Match(const string& name)
{
    return name == longName || name == shortName;
}

/*���������Ű溯��*/
ostream& Option::DescriptionStrPrint(const string & print,ostream& out)
{
    string           strSub;
    char             buf[LENGTH_OF_PRINTLINE + 1];
    int              index;
    string           tmpString = print;
	
    while(tmpString.length() > LENGTH_WITH_DESCRIPTION)
    {
         /* ��֤���ʲ��ܱ���*/
         if(' ' != tmpString.at(LENGTH_WITH_DESCRIPTION - 1))
         {
             /*�Ӻ���ǰ�ң��ҵ���һ���ո�λ��*/
             index = tmpString.find_last_of(' ',LENGTH_WITH_DESCRIPTION - 1);
             /*��ȡindex ������ַ�*/
             strSub = tmpString.substr(index + 1);
         		 
             /*���������*/
             tmpString.copy(buf,index);
             buf[index] = '\0';
             /*�������֮����Ҫ����*/
             out.width(index+ LENGTH_WITHOUT_DESCRIPTION );	
         }
         else
         {
             /*��ȡLENGTH_WITH_DESCRIPTION ������ַ�*/
             strSub = tmpString.substr(LENGTH_WITH_DESCRIPTION);
             /*���������*/	
             tmpString.copy(buf,LENGTH_WITH_DESCRIPTION - 1);       
             buf[LENGTH_WITH_DESCRIPTION] = '\0';
     				
              /*�������֮����Ҫ����*/
              out.width(LENGTH_WITH_DESCRIPTION + LENGTH_WITHOUT_DESCRIPTION );
          }	    		
         
           /*���뻻�з�*/        	  
           out<<buf<<'\n';
         	 
           tmpString = strSub;
       }  

	/*�������֮����Ҫ����*/
       out.width(tmpString.length() + LENGTH_WITHOUT_DESCRIPTION );	

       out<< tmpString<<'\n';
	
       return out;	
		
}

/*�����Ű�*/
ostream& Option::AllStrPrint(const string & print,ostream& out)
{
    string           strSub;
    char             buf[LENGTH_OF_PRINTLINE + 1];
    int              index;
    string           tmpString = print;
	
    if(tmpString.length() > LENGTH_OF_PRINTLINE)
    {
        /* ��֤���ʲ��ܱ���*/
        if(' ' != tmpString.at(LENGTH_OF_PRINTLINE - 1))
        {
            /*�Ӻ���ǰ�ң��ҵ���һ���ո�λ��*/
            index = tmpString.find_last_of(' ',LENGTH_OF_PRINTLINE - 1);
     	     /*��ȡindex ������ַ�*/
            strSub = tmpString.substr(index + 1);
     		 
     	     /*���������*/
     	     tmpString.copy(buf,index);
            buf[index] = '\0';
        }
        else
        {
            /*��ȡLENGTH_OF_PRINTLINE ������ַ�*/
            strSub = tmpString.substr(LENGTH_OF_PRINTLINE);
     	      /*���������*/	
     	     tmpString.copy(buf,LENGTH_OF_PRINTLINE - 1);       
     	     buf[LENGTH_OF_PRINTLINE] = '\0';
        }	 
     		
     	  /*���뻻�з�*/        	  
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

    /*�����е��ַ�����������*/	
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

    /*���������ַ��������������Ϊ30, ���ಿ���ÿո���*/
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

    /*������ȳ�����LENGTH_WITHOUT_DESCRIPTION*/
    if(PrintAll.length() > LENGTH_WITHOUT_DESCRIPTION)
    {
        /*����*/
        out<<names.str()<<'\n';
	PrintAll = description;
	 /*����֮�����������Ű�*/
        DescriptionStrPrint(PrintAll,out);
    }
    else
    {
         /*��Ҫ������������ݣ��м��ÿո���������*/
         PrintAll = names.str() + description;     
	  /*�����Ű�*/
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
    //���help���ش򿪣�������ӡ������Ϣ֮�󷵻ش���
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


/*��������в�����֪ѡ���У��򷵻�ERROR_OPTION_NOT_EXIST*/
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
                //�������κ�ѡ��������в���
                arguments.push_back(argument);
	            return ERROR_OPTION_NOT_EXIST;
            }
        }
        
        if (help)
        {
            //���help���ش򿪣�������ӡ������Ϣ֮�󷵻ش���
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

//��ini�ļ�����ѡ��
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



