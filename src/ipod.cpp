/***************************************************************************
 *   Copyright (C) 2008 by John Petrocik   *
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
#include "ipod.h"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <map>
#include <string>

#include <glib.h>

struct ipod_handle {
	itunes_handle* itunes;
	player_handle* player;
    aap_handle* aap;
} ipod;	

/**
 * 0x0104
 *
 * Called to switch IPod to AIRMode, used to initialize
 */
int ipod_create()
{
    aap_callbacks* callbacks;
    
    ipod.itunes = itunes_open("/home/john/Source/ipod/iTunes_Music_Library.xml");
    if (ipod.itunes == NULL)
    {
        return 1;
    }
	
    ipod.player = player_create();
    
    //setup callbacks
	callbacks = new aap_callbacks();
	callbacks->airMode = &ipod_air_mode;
	callbacks->id = &ipod_id;
	callbacks->trackStatus = &ipod_track_status;
	callbacks->switchToMain = &ipod_switch_to_main;
	callbacks->execute= &ipod_execute;
	callbacks->control= &ipod_control;
	callbacks->count= &ipod_count;
	callbacks->shuffleMode= &ipod_shuffle_mode;
	callbacks->repeatMode= &ipod_repeat_mode;
	callbacks->setRepeatMode= &ipod_set_repeat_mode;
	callbacks->playlistCount= &ipod_playlist_count;
	callbacks->currentPosition= &ipod_current_position;
	callbacks->trackTitle= &ipod_track_title;
	callbacks->trackAlbum= &ipod_track_album;
	callbacks->trackArtist = &ipod_track_artist;
	callbacks->jump= &ipod_jump;
	callbacks->titles= &ipod_titles;
	callbacks->select= &ipod_select;

    //open connection
    ipod.aap = aap_open_connection(callbacks);
    if (ipod.aap == NULL){
        printf("Unable to open connection");
        return 1;
    }


}

void ipod_destory()
{
    aap_close_connection (ipod.aap);

}

void ipod_air_mode(){
    printf("switching to AIR Modde");
}
/**
 * 0x0012
 *
 * Returns Ipod identification
 *
 * Gen3 20GB: 0x01 0x02 (0x01 = Gen3 iPod, 0x02 = 20GB ??)
 * Gen4 30GB: 0x01 0x09
 * Gen5 30GB: 0x01 0x09
 */
char* ipod_id ( char* id, size_t size )
{
	char _id[] = { 0x01,0x02 };

	return strncpy ( id, _id, size );


}

/**
 * 0x0014
 *
 * Returns name of Ipod
 *
 */
char* ipod_name ( char* name, size_t size )
{
	char _name[] = "Aap Deamon v1.0";

	return strncpy ( name, _name, size );
}

/**
 * 0x0016
 *
 * Switch to the main playlist
 */
void ipod_switch_to_main()
{
	itunes_switch_to_main(ipod.itunes);
    
}

/**
 * 0x0017
 *
 * Switch to the given type and position.  It appears this can
 * be call at anytime and is bases on the main playlist/root
 */
void ipod_switch_to ( AAP_TYPE type, int position )
{
	
	/*
	 * I think a call to switch differ from a call to
	 * select by reseting itself to main, but then again
	 * maybe not :)
	 */
	itunes_switch_to_main(ipod.itunes);
	
	ipod_select(type, position);


	
}

/**
 * 0x0018
 *
 * Return the count of given type.  The count depends on previous call to  0x0038
 */
int ipod_count ( AAP_TYPE type )
{

	switch (type){
		case TYPE_PLAYLIST:
			return itunes_list (ipod.itunes, "Playlists").size();
		case TYPE_ARTIST:
			return itunes_list (ipod.itunes, "Artist").size();
		case TYPE_ALBUM:
			return itunes_list (ipod.itunes, "Album").size();
		case TYPE_GENRE:
			return itunes_list (ipod.itunes, "Genre").size();
		case TYPE_SONG:
			return itunes_list (ipod.itunes, "Name").size();
		case TYPE_COMPOSER:
			return itunes_list (ipod.itunes, "Composer").size();
		default:
			return 0;
	}
	
}


/**
 * 0x001A
 *
 * Return track title for given range
 */
vector<string> ipod_titles ( AAP_TYPE type, unsigned int offset, unsigned int length)
{
	vector<string> names;
	int i=0;
	int end = offset+length;
	
	switch (type){
		case TYPE_PLAYLIST:
			names = itunes_list (ipod.itunes, "Playlists");
			break;
		case TYPE_ARTIST:
			names = itunes_list (ipod.itunes, "Artist");
			break;
		case TYPE_ALBUM:
			names = itunes_list (ipod.itunes, "Album");
			break;
		case TYPE_GENRE:
			names = itunes_list (ipod.itunes, "Genre");
			break;
		case TYPE_SONG:
			names = itunes_list (ipod.itunes, "Name");
			break;
		case TYPE_COMPOSER:
			names = itunes_list (ipod.itunes, "Composer");
			break;
		default:
			cout << "Unsupported type" << endl;
	}
		
    names.erase(names.begin(),names.begin()+offset);
    names.erase(names.begin()+length,names.end());
	
	return names;
}

/**
 * 0x001C
 *
 * Returns current track status
 */
int ipod_track_status ( aap_track_status* currentStatus )
{
	return player_status(ipod.player, currentStatus);
}

/**
 * 0x001E
 *
 * Current position in playlist
 */
int ipod_current_position()
{
	return player_current_position(ipod.player);
}

/**
 * 0x0020
 *
 * Returns the track title in given position in playlist
 */
const char* ipod_track_title ( int position, char* name, size_t size )
{
    map<string, const string*>::const_iterator it;
    const player_songlist playlist = player_playlist(ipod.player);
    
//    if (position >= playlist.size())
//        position = 0;
    
    isong song = playlist[position];
    it = song->find("Name");
    
    if (it != song->end())
        strncpy(name, it->second->c_str(), size);
	
    return name;
}

/**
 * 0x0022
 *
 * Returns the track artisti in given position ion playlist
 */
const char* ipod_track_artist ( int position, char* name, size_t size )
{
    map<string, const string*>::const_iterator it;
    const player_songlist playlist = player_playlist(ipod.player);
	
    isong song = playlist[position];
    it = song->find("Artist");
    
    if (it != song->end())
        strncpy(name, it->second->c_str(), size);
	
    return name;
}

/**
 * 0x0024
 *
 * Returns the track album in given position ion playlist
 */
const char* ipod_track_album ( int position, char* name, size_t size )
{
    map<string, const string*>::const_iterator it;
    const player_songlist playlist = player_playlist(ipod.player);
	
    isong song = playlist[position];
    it = song->find("Album");
    
    if (it != song->end())
        strncpy(name, it->second->c_str(), size);
    
    return name;
}

/**
 * 0x0028
 *
 * Switch to track.  Track depends on previous call to switch() 0x0017
 */
int ipod_execute ( int position )
{
	const vector< isong > playlist = itunes_playlist(ipod.itunes);
	
	player_set_playlist(ipod.player, playlist, position);
    
    return 1;
}

/**
 * 0x00029
 *
 * Toogle play status
 *
 * Play/Pause=0x01, Stop=0x02, Skip++=0x03, Skip--=0x04, FFwd=0x05, FRwd=0x06, StopFF/RW=0x07
 */
AAP_MODE ipod_control ( AAP_MODE mode )
{
	return player_set_control_mode(ipod.player, mode);
}

/**
 * 0x002C
 *
 * Return the current shuffleMode.
 *
 * off=0x00
 * songs=0x01
 * albums=0x02
 */
AAP_MODE ipod_shuffle_mode()
{
	return player_shuffle_mode(ipod.player);
}


/**
 * 0x002E
 *
 * Set the current shuffleMode.
 *
 * off=0x00
 * songs=0x01
 * albums=0x02
 */
AAP_MODE ipod_set_shuffle_mode ( AAP_MODE mode )
{
	return player_set_shuffle_mode(ipod.player, mode);
}

/**
 * 0x002F
 *
 * Return the current repeat mode.
 *
 * off=0x00
 * one song=0x01
 * all songs=0x02
 *
 */
AAP_MODE ipod_repeat_mode()
{
	return player_repeat_mode(ipod.player);
}

/**
 * 0x0031
 *
 * Sets the repeat mode.
 *
 * off=0x00
 * one song=0x01
 * all songs=0x02
 *
 */
AAP_MODE ipod_set_repeat_mode ( AAP_MODE mode )
{
	return player_set_repeat_mode(ipod.player, mode);
	
}

/**
 * 0x0035
 *
 * Returns the number of songs in the current playlist
 */
int ipod_playlist_count()
{
	return player_playlist_count(ipod.player);
}

/**
 * 0x0037
 *
 * Jump to given track in current playlist
 */
int ipod_jump ( int position )
{
	player_jump(ipod.player, position);
}

/**
 * 0x0038
 *
 * Selects the type from the current playlist to filter by
 */
int ipod_select ( AAP_TYPE type, int position )
{
	vector<string> list;

	switch (type){
		case TYPE_PLAYLIST:
			list = itunes_list (ipod.itunes, "Playlists");
			itunes_select(ipod.itunes, "Playlists", list[position]);
			break;
		case TYPE_ARTIST:
			list = itunes_list (ipod.itunes, "Artist");
			itunes_select(ipod.itunes, "Artist", list[position]);
			break;
		case TYPE_ALBUM:
			list = itunes_list (ipod.itunes, "Album");
			itunes_select(ipod.itunes, "Album", list[position]);
			break;
		case TYPE_GENRE:
			list = itunes_list (ipod.itunes, "Genre");
			itunes_select(ipod.itunes, "Genre", list[position]);
			break;
		case TYPE_SONG:
			list = itunes_list (ipod.itunes, "Name");
			itunes_select(ipod.itunes, "Name", list[position]);
			break;
		case TYPE_COMPOSER:
			list = itunes_list (ipod.itunes, "Composer");
			itunes_select(ipod.itunes, "Composer", list[position]);
			break;
	}
	
	return 0;
}

