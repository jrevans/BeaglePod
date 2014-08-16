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

#ifndef PLIST_H
#define PLIST_H

#include <map>
#include <vector>
#include <string>

#include <libxml/tree.h>


using namespace std;

#define pvoid const void*
#define pdict(T) const map<string, T>*
#define parray(T) const vector<T>*
#define pstring const string*

/**
 * Function found in later version of xml2 library
 */
xmlNodePtr	xmlNextElementSibling ( xmlNodePtr node );

xmlNodePtr	xmlNextElementChild ( xmlNodePtr node );

/**
 * Takes a filename of a plist and parses it
 */
const void* plist_read (const char* filename );

const void* plist_parse(xmlNode* cur_node);

pstring plist_string_to_string(xmlNode* a_node);

pstring plist_boolean_to_string(xmlNode* a_node);
    
pdict(pvoid) plist_dictionary_to_map(xmlNode *a_node);

parray(pvoid) plist_array_to_vector(xmlNode *a_node);

#endif