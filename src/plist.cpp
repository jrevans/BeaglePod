/***************************************************************************
 *   Copyright (C) 2009 by John Petrocik   *
 *   jpetrocik@supplyframe.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/*
Foundation class 	CoreFoundation type 	XML Tag 	Storage format
NSString 	CFString 	<string> 	UTF-8 encoded string
NSNumber 	CFNumber 	<real>, <integer> 	Decimal string
NSNumber 	CFBoolean 	<true />, or <false /> 	No data (tag only)
NSDate 	CFDate 	<date> 	ISO 8601 formatted string
NSData 	CFData 	<data> 	Base64 encoded data
NSArray 	CFArray 	<array> 	Can contain any number of child elements
NSDictionary 	CFDictionary 	<dict> 	Alternating <key> tags and plist element tags
*/

#include <cstring>
#include <iostream>

#include <libxml/parser.h>

#include "plist.h"

using namespace std;

#define PLIST_PLIST "plist"
#define PLIST_DICT "dict"
#define PLIST_KEY "key"
#define PLIST_ARRAY "array"
#define PLIST_STRING "string"
#define PLIST_INTEGER "integer"
#define PLIST_DATE "date"
#define PLIST_TRUE "true"
#define PLIST_FALSE "false"


/**
 * Function found in later version of xml2 library
 */
xmlNodePtr	xmlNextElementSibling (xmlNodePtr node)
{
	xmlNode *cur_node = NULL;
	
	for ( cur_node = node->next; cur_node; cur_node = cur_node->next )
	{
		if ( cur_node->type == XML_ELEMENT_NODE )
		{
			return cur_node;
		}	
	}
	
	return NULL;

}

xmlNodePtr	xmlNextElementChild (xmlNodePtr node)
{
    xmlNode *cur_node = NULL;
	
    for ( cur_node = node->children; cur_node; cur_node = cur_node->next )
    {
        if ( cur_node->type == XML_ELEMENT_NODE )
        {
            return cur_node;
        }	
    }
	
    return NULL;
}

const void* plist_read(const char* filename)
{
	const void* plist;
	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;
    xmlNode* firstElement = NULL;
    	
	/*parse the file and get the DOM */
	doc = xmlReadFile ( filename, NULL, XML_PARSE_NOBLANKS );

	if ( doc == NULL )
	{
		cout << "error: could not parse file " << filename << endl;
	}

	/*Get the root element node */
	root_element = xmlDocGetRootElement ( doc );

    if (strcmp((char *)root_element->name,PLIST_PLIST))
		return NULL;
	
    firstElement = xmlNextElementChild(root_element);
     
    plist = plist_parse(firstElement);
    
    /*free the document */
    xmlFreeDoc ( doc );

	/*
    *Free the global variables that may
    *have been allocated by the parser.
    */
    xmlCleanupParser();

    return plist;
}

const void* plist_parse(xmlNode* cur_node){
    
    if (strcmp((char *)cur_node->name,PLIST_DICT) == 0)
    {
        return plist_dictionary_to_map(cur_node);
    }
    else if (strcmp((char *)cur_node->name,PLIST_ARRAY) == 0)
    {
        return plist_array_to_vector(cur_node);
    }
    else if (strcmp((char *)cur_node->name,PLIST_STRING) == 0)
    {
        return plist_string_to_string(cur_node);
    }
    else if (strcmp((char *)cur_node->name,PLIST_DATE) == 0)
    {
        return plist_string_to_string(cur_node);
    }
    else if (strcmp((char *)cur_node->name,PLIST_INTEGER) == 0)
    {
        return plist_string_to_string(cur_node);
    }
    else if ( (strcmp((char *)cur_node->name,PLIST_TRUE) == 0) || (strcmp((char *)cur_node->name,PLIST_FALSE) == 0))
    {
        return plist_boolean_to_string(cur_node);
    }
    
    return NULL;
}

const string* plist_string_to_string(xmlNode* a_node){
    xmlChar* value;
    
    if (a_node == NULL)
        return NULL;
    
    if (a_node->children == NULL)
        return NULL;
    
    value = a_node->children->content;
    
    return new string((const char*)value);

}

const string* plist_boolean_to_string(xmlNode* a_node){
    
    if (strcmp((char *)a_node->name,PLIST_TRUE) == 0)
    {
        return new string("true");
    }
    else
        return new string("false");
}

pdict(pvoid) plist_dictionary_to_map(xmlNode *a_node)
{
	xmlNode *cur_node;
	map<string,const void*>* dictMap = new map<string, const void*>();
	
    if (strcmp((char*)a_node->name, PLIST_DICT)){
		cout << "Not a dictionary" << endl;
		return dictMap;
	}
	
	cur_node = xmlNextElementChild(a_node);

	while (cur_node != NULL){
		
        if (strcmp((char *)cur_node->name,PLIST_KEY) == 0){
            xmlNode *valueNode = NULL;
			xmlChar* key = NULL;
			const void* value = NULL;
					
			valueNode = xmlNextElementSibling(cur_node);
            
            key = cur_node->children->content;

            value = plist_parse(valueNode);
            
            if (value != NULL)
				dictMap->insert( pair<string,const void*>(string((const char*)key),value) );
		}
		
		cur_node = xmlNextElementSibling(cur_node);
	}
	
	return dictMap;
}

/**
 * Finds the next sibling that is an array
 */
parray(pvoid) plist_array_to_vector(xmlNode *a_node)
{
    xmlNode *cur_node = NULL;
	vector<const void*>* arrayVector = new vector<const void*>();

    if (strcmp((char*)a_node->name, PLIST_ARRAY)){
        cout << "Not an array" << endl;
        return arrayVector;
    }
	

    cur_node = xmlNextElementChild(a_node);

    while ( cur_node != NULL)
    {
		const void* value;
        
        value = plist_parse(cur_node);
        
        if (value != NULL)
            arrayVector->push_back( value);
        
        cur_node = xmlNextElementSibling(cur_node);
    }
	
    return arrayVector;

}