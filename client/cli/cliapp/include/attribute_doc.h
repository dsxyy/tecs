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

#ifndef ATTRIBUTE_DOC_H_
#define ATTRIBUTE_DOC_H_

#include <string>
#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>

using namespace std;

namespace zte_tecs
{

class Attribute;

/**
 *  Base class for file AttributeDocs. A AttributeDoc is a file (or a string for the
 *  matter of fact) containing a set of attribute definitions of the form:
 *  NAME = VALUE
 *  where NAME is a string representing the name of the attribute, and VALUE can
 *  be a single string or a vector value (array of string pairs). The file can
 *  contain several attributes with the same name.
 */
class AttributeDoc //: public Serializable
{

    friend ostream &operator<<(ostream &os, const AttributeDoc &ad);

public:

    AttributeDoc(bool       replace_mode = false,
                    const char *xml_root     = "AttributeDoc"):
                         _replace_mode(replace_mode),
                         _xml_root(xml_root){};

    /**
     *  The class destructor frees all the attributes conforming the AttributeDoc
     */
    virtual ~AttributeDoc();


    /**
     *  Writes the AttributeDoc in a simple xml string:
     *  <AttributeDoc>
     *    <single>value</single>
     *    <vector>
     *      <attr>value</attr>
     *      ...
     *    </vector>
     *    ...
     *  </AttributeDoc>
     *  The name of the root element is set when the Template object is created
     *    @param xml string that hold the xml AttributeDoc representation
     *    @return a reference to the generated string
	 */
	 virtual string serialize() const ;


    /**
     *  Rebuilds the AttributeDoc from a xml formatted string
     *    @param xml_str The xml-formatted string
     *
     *    @return 0 on success, -1 otherwise
     */     
    virtual bool deserialize(const string &xml_str) ;


    /**
     *  Sets a new attribute, the attribute MUST BE ALLOCATED IN THE HEAP, and
     *  will be freed when the AttributeDoc destructor is called.
     *    @param attr pointer to the attribute
     */
    virtual void Set(Attribute * attr);

    /**
     *  Removes an attribute from the AttributeDoc. The attributes are returned. The
     *  attributes MUST be freed by the calling funtion
     *    @param name of the attribute
     *    @param values a vector containing a pointer to the attributes
     *    @returns the number of attributes removed
     */
    virtual int Remove(
        const string&        name,
        vector<Attribute *> &values);


    /**
     *  Removes an attribute from the AttributeDoc, and frees the attributes.
     *    @param name of the attribute
     *    @returns the number of attributes removed
     */
    virtual int Erase(const string &name);

    /**
     *  Copy all attributes from the AttributeDoc. The attributes  MUST be 
     *  freed by the calling funtion
     *    @param values a multimap containing a name and pointer to the attribute
     *    @returns the number of attributes of the AttributeDoc.
     */
     virtual int get_attributes(multimap<string, Attribute *> &values) const;

    /**
     *  Gets all the attributes with the given name.
     *    @param name the attribute name.
     *    @returns the number of elements in the vector
     */
    virtual int Get(const string& name, vector<const Attribute*> &values) const;

    /**
     *  Gets all the attributes with the given name,  non-const version
     *    @param name the attribute name.
     *    @returns the number of elements in the vector
     */
    virtual int Get(const string& name, vector<Attribute*> &values);

    /**
     *  Gets the value of a Single attributes (string) with the given name.
     *    @param name the attribute name.
     *    @param value the attribute value, a string, "" if the attribute is not
     *    defined or not Single
     */
    virtual void Get(string &name, string &value) const;

    virtual void Get(const char *name, string &value) const;

    /**
     *  Gets the value of a Single attributes (int) with the given name.
     *    @param name the attribute name.
     *    @param value the attribute value, an int, 0 if the attribute is not
     *    defined or not Single
     */
    virtual void Get(string &name, int  &value) const;

    virtual void Get(const char *name, int  &value) const;
	
    virtual void  Get(string &name, long long   &value) const;

    virtual void  Get(const char *name, long long   &value) const;

    /**
     *  Writes the AttributeDoc in a plain text string
     *    @param str string that hold the AttributeDoc representation
     *    @return a reference to the generated string
     */
    string &to_str(string &str) const;

    void Marshall(string &str, const char delim='\n');
	

private:

    /**
     *  The AttributeDoc attributes
     */
    multimap<string, Attribute *>   _attributes;

    bool                            _replace_mode;

    /**
     * Character to separate key from value when dump onto a string
     **/
    static const char				_separator;

    /**
     *  Name of the Root element for the XML document
     */
    string							_xml_root;
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

}

#endif /*ATTRIBUTE_DOC_H_*/
