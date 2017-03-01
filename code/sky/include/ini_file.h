
#ifndef SKY_INI_INIFILE_H
#define SKY_INI_INIFILE_H

#include <sstream>
#include <map>
using namespace std;
class IniFile
{
public:
    IniFile() {_is_right = true;}
    explicit IniFile(istream &in);

    bool IsExist(const string &section) const
    {
        return _data.find(section) != _data.end();
    }

    bool IsExist(const string &section, const string &token) const
    {
        MapType::const_iterator si = _data.find(section);
        return (si != _data.end()) && \
               (si->second.find(token) != si->second.end());
    }

    string GetValue(const string &section, const string &token, 
                    const string &def = string()) const
    {
        MapType::const_iterator si = _data.find(section);
        if (si != _data.end())
        {
            MapType::mapped_type::const_iterator token_it;
            if (si->second.end() != (token_it = si->second.find(token)))
                return token_it->second;
        }
        return def;
    }

    template <typename T>
    T GetValueT(const string &section, const string &token, const T &def) const
    {
        MapType::const_iterator si = _data.find(section);
        if (si != _data.end())
        {
            MapType::mapped_type::const_iterator token_it = si->second.find(token);

            if (si->second.end() != (token_it = si->second.find(token)))
            {
                istringstream s(token_it->second);
                T value;
                s >> value;
                if (s)
                {
                    return value;
                }
            }
        }
        return def;
    }

    void SetValue(const string &section, const string &key, const string &value)
    {
        _data[section][key] = value;
    }

    template <typename T>
    void SetValueT(const string &section, const string &key, const T &value)
    {
        ostringstream v;
        v << value;
        _data[section][key] = v.str();
    }
    
    void SetError(const string &error_data)
    {
          _is_right = false;
          _error_data = error_data;
    }
    
    bool IsRight()
    {
          return _is_right;
    }
    
    const string &GetErrorData()
    {
         return _error_data;
    }

    /**
    * Get the names of sections.
    * example1:
    * <code>
    *  IniFile ini("my.ini");
    *
    *  // copy names of sections into a container
    *  set<string> keys;
    *  ini.GetSections(inserter(s, s.begin());
    *
    *  // or printing the names:
    *  ini.GetSections(ostream_iterator(cout, "\n"));
    * </code>
    */
    template <typename OutputIterator>
    void GetSections(OutputIterator oi)
    {
        for (MapType::const_iterator it = _data.begin(); it != _data.end(); 
             ++it, ++oi)
        {
            *oi = it->first;
        }
    }

    /**
    * Get the keys of a section.
    * example1:
    * <code>
    *  IniFile ini("my.ini");
    *
    *  // copy keys into a container
    *  set<string> keys;
    *  ini.GetKeys("section2", inserter(s, s.begin());
    *
    *  // or printing the keys:
    *  ini.GetKeys("section2", ostream_iterator(cout, "\n"));
    * </code>
    */
    template <typename OutputIterator>
    void GetKeys(const string &section, OutputIterator oi)
    {
        MapType::const_iterator si = _data.find(section);
        if (si != _data.end())
        {
            for (MapType::mapped_type::const_iterator it = si->second.begin();
                 it != si->second.end(); ++it, ++oi)
            {
                *oi = it->first;
            }
        }
    }

private:
    friend ostream& operator << (ostream &out, const IniFile &ini);
    typedef map<string, map<string, string> > MapType;
    MapType _data;
    bool    _is_right;
    string  _error_data;
};

/// Outputs ini-file to a output-stream
ostream& operator << (ostream& out, const IniFile& ini);
#endif // SKY_INI_INIFILE_H

