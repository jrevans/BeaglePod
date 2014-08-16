/***************************************************************************
 *   Copyright (C) 2009 by John Petrocik   *
 *   john@petrocik.net   *
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

#include "plist.h"

#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

void assertNotNull ( const void* ptr )
{
	if ( ptr == NULL )
	{
		cout << "Assert is null" << endl;
		exit ( -1 );
	}
}

void assertEquals ( const string expected, const string actual )
{
	if ( strcmp ( actual.c_str(),expected.c_str() ) !=0 )
	{
		cout << "Expected: " << expected << " Actual: " << actual << endl;
		exit ( -1 );
	}
}

void assertTrue ( int condition )
{
	if ( condition == 0 )
	{
		cout << "Expected: TRUE Actual: FALSE" << endl;
		exit ( -1 );

	}
}
int main ( int argc, char* argv[] )
{
	map<string, const void*> plist;

	pdict(pvoid) plist_ptr = ( pdict(pvoid) ) plist_read ( "/home/jpetrocik/Source/ipod/iTunes_Music_Library.xml" );
	assertNotNull ( plist_ptr );
	plist = *plist_ptr;

	pdict(pvoid) tracks = ( pdict(pvoid) ) plist["Tracks"];
	assertNotNull ( tracks );

	pdict(pstring) song = ( pdict(pstring) ) tracks->find ( "175" )->second;
	assertNotNull ( song );
	assertEquals ( "Freedom For My People", * ( song->find ( "Name" )->second ) );

	parray(pvoid) playlist = ( parray(pvoid) ) plist["Playlists"];
	assertTrue ( playlist->size() > 0 );

	pdict(pstring) list = ( pdict(pstring) ) playlist->at ( 0 );
	assertNotNull ( list );
	assertEquals("Library",* (list->find("Name")->second));
	
	parray(pvoid) songlist = ( parray(pvoid) ) list->find ( "Playlist Items" )->second;
	pdict(pstring) playlist_song = ( pdict(pstring) ) songlist->at ( 0 );
	assertNotNull ( playlist_song );
	assertEquals("235",* (playlist_song->find("Track ID")->second));
	
	cout << "Test Passed!" << endl;
}