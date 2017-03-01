#include "ini_file.h"
#include "ini_parser.h"
class IniFileEvent : public IniParser::Event
{
public:
      IniFileEvent(IniFile& ini_file): _ini_file(ini_file) {}    
      bool OnSection(const string &section)
      {
          _section = section;
          return false;
      }
      bool OnKey(const string &key)
      {
          _key = key;
          return false;
      }
      bool OnValue(const string &value)
      {
          _ini_file.SetValue(_section, _key, value);
          return false;
      }

      bool OnError(const string &error_data="")
      {
          _ini_file.SetError(error_data);
          return false;
      }
private:
    IniFile &_ini_file;
    string  _section;
    string  _key;
};

IniFile::IniFile(istream& in)
{
    _is_right = true;
    IniFileEvent ev(*this);
    IniParser(ev).Parse(in);
}

ostream& operator << (ostream &out, const IniFile &ini)
{
    for (IniFile::MapType::const_iterator si = ini._data.begin();
         si != ini._data.end(); ++si)
    {
        out << '[' << si->first << "]" << endl;
        for (IniFile::MapType::mapped_type::const_iterator it = 
             si->second.begin(); it != si->second.end(); ++it)
        {
            out << it->first << '=' << it->second << endl;
        }
    }
    return out;
}


