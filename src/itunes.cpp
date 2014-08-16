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


#include <cstdlib>
#include <iostream>

#include <map>
#include <vector>
#include <string>
#include <algorithm>

#include "plist.h"
#include "itunes.h"

using namespace std;


itunes_handle itunes; 

itunes_handle* itunes_open(const char *filename){
    map<string,void*>* plist;
	
    /*
	* this initialize the library and check potential ABI mismatches
	* between the version it was compiled for and the actual shared
	* library used.
	*/
	LIBXML_TEST_VERSION

	//open plist
	plist = (map<string,void*>*)plist_read (filename );
    if ( plist == NULL )
	{
		cout << "Error parsing plist" << endl;
		return NULL;
	}
        
    itunes._library = plist;
	
	return &itunes;
}

void itunes_close(itunes_handle* itunes)
{
	//free(itunes);
}

vector<string> itunes_list (itunes_handle* itunes, string key )
{
	vector<string> names;
	vector<string>::iterator it;

	if ( key.compare ( "Playlists" ) == 0 )
	{
		parray ( pdict ( pvoid ) ) _playlists = ( parray ( pdict ( pvoid ) ) ) itunes->_library->find ( "Playlists" )->second;
	
		for ( int i=0; i<_playlists->size() ; i++ )
		{
			pdict ( pvoid ) aList = ( pdict ( pvoid ) ) _playlists->at ( i );
			pstring name = (pstring)aList->find ( "Name" )->second;
			names.push_back ( *name );
		}
	}
	else
	{
		for ( int i=0; i<itunes->_songlist.size(); i++ )
		{
			isong song = itunes->_songlist[i];
			pstring name = song->find ( key )->second;
			if ( name != NULL && name->length() > 0 )
			{
				names.push_back ( *name );
			}
		}
	
      	// using default comparison (operator <):
        sort ( names.begin(), names.end() );

       	// using default comparison:
        it = unique ( names.begin(), names.end() );

        names.resize ( it - names.begin() );

    }
	

    return names;

}

const vector< isong > itunes_select (itunes_handle* itunes, string key, string value )
{

	if ( key.compare ( "Playlists" ) == 0 )
	{
		parray ( pdict ( pvoid ) ) _playlists = ( parray ( pdict ( pvoid ) ) ) itunes->_library->find ( "Playlists" )->second;
		pdict ( isong ) _tracks = (pdict ( isong )) itunes->_library->find ( "Tracks" )->second;
	
		itunes->_songlist.erase (itunes->_songlist.begin(),itunes->_songlist.end());


		//find requested playlist
		for ( int i=0; i<_playlists->size() ; i++ )
		{
			pdict ( pvoid ) aList = ( pdict ( pvoid ) ) _playlists->at ( i );
			pstring name = (pstring)aList->find ( "Name" )->second;
			if ( name != NULL && name->compare ( value ) == 0 )
			{
                
				parray(pdict(pvoid)) songs = (parray(pdict(pvoid))) aList->find( "Playlist Items" )->second;
                if (songs == NULL)
                    break;
                
				for ( int i=0; i<songs->size() ; i++ )
				{
					pdict(pstring) track = (pdict(pstring))songs->at(i);
					pstring id = track->find("Track ID")->second;
					
					isong song = _tracks->find(*id)->second;
					itunes->_songlist.push_back(song);
					
				}
				break;
			}
		}
	}
	else
	{
		vector< isong >::iterator it;
	
		it = itunes->_songlist.begin();
	
		while ( it != itunes->_songlist.end() )
		{
			isong song = *it;
			pstring name = song->find ( key )->second;
			if ( name == NULL || name->compare ( value ) )
			{
				it = itunes->_songlist.erase ( it );
			}
			else
			{
				++it;
			}
		}
	}
	return itunes->_songlist;

}

int itunes_switch_to_main(itunes_handle* itunes){
    map<string,void*>::iterator i;
    map<string, void*>* tracks = (map<string, void*>*)itunes->_library->find("Tracks")->second;
	isong song;
    
    itunes->_songlist.erase (itunes->_songlist.begin(),itunes->_songlist.end());
    
    for ( i=tracks->begin() ; i != tracks->end(); i++ )
    {
		song = (isong)(*i).second;
        itunes->_songlist.push_back(song);
    }

    return 0;
}

isong itunes_position(itunes_handle* itunes, int index){
	return itunes->_songlist[index];
}

int itunes_playlist_count(itunes_handle* itunes){
	return itunes->_songlist.size();
}

const vector< isong > itunes_playlist(itunes_handle* itunes){
	return itunes->_songlist;
}

