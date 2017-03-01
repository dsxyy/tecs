/* -------------------------------------------------------------------------- */
/* Copyright 2002-2011, OpenNebula Project Leads (OpenNebula.org)             */
/*                                                                            */
/* Licensed under the Apache License, Version 2.0 (the "License"); you may    */
/* not use this file except in compliance with the License. You may obtain    */
/* a copy of the License at                                                   */
/*                                                                            */
/* http://www.apache.org/licenses/LICENSE-2.0                                 */
/*                                                                            */
/* Unless required by applicable law or agreed to in writing, software        */
/* distributed under the License is distributed on an "AS IS" BASIS,          */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   */
/* See the License for the specific language governing permissions and        */
/* limitations under the License.                                             */
/* -------------------------------------------------------------------------- */

#include "attribute.h"

namespace zte_tecs
{
const char * VectorAttribute::_magic_sep      = "@^_^@";
const int    VectorAttribute::_magic_sep_size = 5;

string * VectorAttribute::Marshall(const char * _sep) const
{
    ostringstream os;
    string *      rs;
    const char *  my_sep;

    map<string,string>::const_iterator it;

    if ( _sep == 0 )
    {
    	my_sep = _magic_sep;
    }
    else
    {
    	my_sep = _sep;
    }

    if ( _attribute_value.size() == 0 )
    {
        return 0;
    }

    it = _attribute_value.begin();

    os << it->first << "=" << it->second;

    for (++it; it != _attribute_value.end(); it++)
    {
        os << my_sep << it->first << "=" << it->second;
    }

    rs = new string;

    *rs = os.str();

    return rs;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

string  VectorAttribute::Serialize() const
{
	map<string,string>::const_iterator    it;
	ostringstream                   oss;

	oss << "<" << get_attribute_name()<< ">";

	for (it=_attribute_value.begin();it!=_attribute_value.end();it++)
	{
		oss << "<" << it->first << "><![CDATA[" << it->second
			<< "]]></"<< it->first << ">";
	}

	oss << "</"<< get_attribute_name() << ">";

	return oss.str();
}


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void VectorAttribute::UnMarshall(const string& sattr, const char * _sep)
{
    size_t  bpos=0,epos,mpos;
    string  tmp;
    bool    cont = true;

    const char *  my_sep;
    int           my_sep_size;

    if ( _sep == 0 )
    {
        my_sep      = _magic_sep;
        my_sep_size = _magic_sep_size;
    }
    else
    {
        my_sep      = _sep;
        my_sep_size = strlen(_sep);
    }

    while(cont)
    {
    	epos=sattr.find(my_sep,bpos);

    	if (epos == string::npos)
    	{
    		tmp  = sattr.substr(bpos);
    		cont = false;
    	}
    	else
    	{
    		tmp  = sattr.substr(bpos,epos-bpos);
    		bpos = epos + my_sep_size;
    	}

    	mpos = tmp.find('=');

    	if (mpos == string::npos)
    	{
    		continue;
    	}

        _attribute_value.insert(make_pair(tmp.substr(0,mpos),
                                         tmp.substr(mpos+1)));
    }
}
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void VectorAttribute::Replace(const map<string,string>& attr)
{
	_attribute_value = attr;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void VectorAttribute::Replace(const string& name, const string& value)
{
    map<string,string>::iterator it;

    it = _attribute_value.find(name);
    
    if ( it != _attribute_value.end() )
    {
        _attribute_value.erase(it);
    }
    
    _attribute_value.insert(make_pair(name,value));
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

string VectorAttribute::VectorValue(const char *name) const
{
    map<string,string>::const_iterator it;

    it = _attribute_value.find(name);

    if ( it == _attribute_value.end() )
    {
        return "";
    }
    else
    {
        return it->second;
    }
}

}
