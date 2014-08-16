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

#include "itunes.h"

int main ( int argc, char* argv[] )
{
    int count=0,i=0;
	itunes_handle* itunes;
	
	itunes = itunes_open("/home/jpetrocik/Source/ipod/iTunes_Music_Library.xml");
    if (itunes == NULL){
        cout << "iTunes::open failed" << endl;
        return -1;
    }
    
    if (itunes_switch_to_main(itunes)){
        cout << "iTunes::switchToMain" << endl;
        return -1;
    }
    
	vector<isong> library = itunes_select(itunes, "Playlists", "Library");
	if (library.size() != 111){
		cout << "iTunes:select playlist expected: 111 actual: " << library.size() << endl;
		return -1;
	} 
    
	count = itunes_playlist_count(itunes);
	if (count != library.size()){
		cout << "iTunes:count expected: " << library.size() << " actual: " << count << endl;
		return -1;
    }   
    
    vector<string> artist = itunes_list(itunes, "Artist");
    if (artist.size() != 46){
        cout << "iTunes:list expected: 46 actual: " << artist.size() << endl;
        return -1;
    } 
	
	vector< isong > playlist = itunes_select(itunes, "Artist","T.A.T.U");
    if (playlist.size() != 2){
        cout << "iTunes:select expected: 2 actual: " << artist.size() << endl;
        return -1;
    }   
    
	isong song = itunes_position(itunes, 1);
	const string* name = song->find("Name")->second;
    if (name->compare("Not Gonna Get Us")){
		cout << "iTunes:position expected: Not Gonna Get Us actual: " << *name << endl;
        return -1;
    }   
    
	if (itunes_playlist_count(itunes) != 2){
		cout << "iTune:playlistCount expected: 2 actual " << itunes_playlist_count(itunes) << endl;
        return -1;
    }
    
	vector<isong> movies = itunes_select(itunes, "Playlists", "Movies");
	if (movies.size() != 1){
		cout << "iTunes:select playlist expected: 1 actual: " << movies.size() << endl;
		return -1;
	} 
    
	count = itunes_playlist_count(itunes);
	if (count != movies.size()){
		cout << "iTunes:count expected: " << movies.size() << " actual: " << count << endl;
		return -1;
	}   
    

    itunes_close(itunes);
    
    cout << "Successful" << endl;
    
}