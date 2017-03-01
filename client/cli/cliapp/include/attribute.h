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

#ifndef ATTRIBUTE_H_
#define ATTRIBUTE_H_

#include <string>
#include <map>
#include <sstream>
#include <algorithm>
#include <string.h>
using namespace std;


namespace zte_tecs
{
/**
 *  Attribute base class for name-value pairs. This class provides a generic
 *  interface to implement
 */
class Attribute
{
public:

    Attribute(const string& aname):_attribute_name(aname)
    {
        transform (
            _attribute_name.begin(),
            _attribute_name.end(),
            _attribute_name.begin(),
            (int(*)(int))toupper);
    };

    virtual ~Attribute(){};

    enum AttributeType
    {
        SIMPLE = 0,
        VECTOR = 1
    };

    /**
     *  Gets the name of the attribute.
     *    @return the attribute name
     */
    const string& get_attribute_name() const
    {
        return _attribute_name;
    };

    /**
     *  Marshall the attribute in a single string. The string MUST be freed
     *  by the calling function.
     *    @return a string (allocated in the heap) holding the attribute value.
     */
    virtual string * Marshall(const char * _sep = 0) const = 0;

    /**
     *  Write the attribute using a simple XML format. The string MUST be freed
     *  by the calling function.
     *    @return a string (allocated in the heap) holding the attribute value.
     */
	virtual string Serialize() const = 0;

    /**
     *  Builds a new attribute from a string.
     */
    virtual void UnMarshall(const string& sattr, const char * _sep = 0) = 0;

    /**
     *  Returns the attribute type
     */
    virtual AttributeType get_type() = 0;

private:

    /**
     *  The attribute name.
     */
    string _attribute_name;
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

/**
 *  The SingleAttribute class represents a simple attribute in the form
 *  NAME = VALUE.
 */

class SingleAttribute : public Attribute
{
public:

    SingleAttribute(const string& name):Attribute(name){};

    SingleAttribute(const string& name, const string& value):
        Attribute(name),_attribute_value(value){};

    ~SingleAttribute(){};

    /**
     *  Returns the attribute value, a string.
     */
    const string& get_attribute_value() const
    {
        return _attribute_value;
    };

    /**
     *  Marshall the attribute in a single string. The string MUST be freed
     *  by the calling function.
     *    @return a string (allocated in the heap) holding the attribute value.
     */
    string * Marshall(const char * _sep = 0) const
    {
        string * rs = new string;

        *rs = _attribute_value;

        return rs;
    };

    /**
     *  Write the attribute using a simple XML format:
     *
     *  <attribute_name>attribute_value</attribute_name>
     *
     *  The string MUST be freed by the calling function.
     *    @return a string (allocated in the heap) holding the attribute value.
     */
    string Serialize() const
    {
    	ostringstream       oss;

    	oss<<"<" << get_attribute_name()<< "><![CDATA["<<_attribute_value
    	   << "]]></" <<get_attribute_name() << ">";

    	return oss.str();
    }

    /**
     *  Builds a new attribute from a string.
     */
    void UnMarshall(const string& sattr, const char * _sep = 0)
    {
        _attribute_value = sattr;
    };

    /**
     *  Replaces the attribute value from a string.
     */
    void Replace(const string& sattr)
    {
        _attribute_value = sattr;
    };

    /**
     *  Returns the attribute type
     */
    AttributeType get_type()
    {
        return SIMPLE;
    };

private:

    string _attribute_value;
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

/**
 *  The VectorAttribute class represents an array attribute in the form
 *  NAME = [ VAL_NAME_1=VAL_VALUE_1,...,VAL_NAME_N=VAL_VALUE_N].
 */

class VectorAttribute : public Attribute
{
public:

    VectorAttribute(const string& name):Attribute(name){};

    VectorAttribute(const string& name,const  map<string,string>& value):
            Attribute(name),_attribute_value(value){};

    ~VectorAttribute(){};

    /**
     *  Returns the attribute value, a string.
     */
    const map<string,string>& get_attribute_value() const
    {
        return _attribute_value;
    };

    /**
     *
     */
    string VectorValue(const char *name) const;

    /**
     *  Marshall the attribute in a single string. The string MUST be freed
     *  by the calling function. The string is in the form:
     *  "VAL_NAME_1=VAL_VALUE_1,...,VAL_NAME_N=VAL_VALUE_N".
     *    @return a string (allocated in the heap) holding the attribute value.
     */
    string * Marshall(const char * _sep = 0) const;

    /**
     *  Write the attribute using a simple XML format:
     *
     *  <attribute_name>
     *    <val_name_1>val_value_1</val_name_1>
     *    ...
     *    <val_name_n>val_value_n</val_name_n>
     *  </attribute_name>
     *
     *  The string MUST be freed by the calling function.
     *    @return a string (allocated in the heap) holding the attribute value.
     */
    string Serialize() const;

    /**
     *  Builds a new attribute from a string of the form:
     *  "VAL_NAME_1=VAL_VALUE_1,...,VAL_NAME_N=VAL_VALUE_N".
     */
    void UnMarshall(const string& sattr, const char * _sep = 0);

    /**
     *  Replace the value of the given attribute with the provided map
     */
    void Replace(const map<string,string>& attr);

    /**
     *  Replace the value of the given vector attribute
     */
    void Replace(const string& name, const string& value);
    
    /**
     *  Returns the attribute type
     */
    AttributeType get_type()
    {
        return VECTOR;
    };

private:

	static const char * _magic_sep;

	static const int	_magic_sep_size;

    map<string,string> _attribute_value;
};

}

#endif /*ATTRIBUTE_H_*/
