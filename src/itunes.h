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
#ifndef ITUNES_H
#define ITUNES_H

#include <iostream>

#include <map>
#include <vector>
#include <string>


using namespace std;

#define isong const map<string, const string*>*

struct itunes_handle
{
	vector< isong > _songlist;
    map< string, void* >* _library;
};

itunes_handle* itunes_open ( const char* filename );

void itunes_close ( itunes_handle* );

/**
 * Return a list of string for the given key.  Like a list of artists
 */
vector<string> itunes_list ( itunes_handle*, string key );

/**
 * Filters the current song lost by given value, where value is of type key.  Filter by
 * artist named Akon
 */
const vector< isong > itunes_select ( itunes_handle*, string key, string value );

/**
 * Returns the song at the given index
 */
isong itunes_position ( itunes_handle*, int index );

/**
 * Resets the songlist to the main library
 */
int itunes_switch_to_main ( itunes_handle* );

/**
 * Return the count of song in current songlost
 */
int itunes_playlist_count ( itunes_handle* );

/**
 * Return the current songlist with filters applied.
 */
const vector< isong > itunes_playlist ( itunes_handle* );


#endif