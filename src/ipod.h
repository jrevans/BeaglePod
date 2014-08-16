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
#ifndef IPOD_H
#define IPOD_H

#include <vector>
#include <map>
#include <string>

#include "protocol.h"
#include "itunes.h"
#include "player.h"
#include "aap.h"

using namespace std;

	
/**
	* 0x0104
	*
	* Called to switch IPod to AIRMode, used to initialize
	*/
int ipod_create();

void ipod_destory();

void ipod_air_mode();

/**
	* 0x0012
	*
	* Returns Ipod identification
	*
	* Gen3 20GB: 0x01 0x02 (0x01 = Gen3 iPod, 0x02 = 20GB ??)
	* Gen4 30GB: 0x01 0x09
	* Gen5 30GB: 0x01 0x09
	*/
char* ipod_id ( char* id, size_t size );

/**
	* 0x0014
	*
	* Returns name of Ipod
	*
	*/
char* ipod_name ( char* name, size_t size );

/**
	* 0x0016
	*
	* Switch to the main playlist
	*/
void ipod_switch_to_main();

/**
	* 0x0017
	*
	* Switch to the given type and position
	*/
void ipod_switch_to ( AAP_TYPE type, int position );

/**
	* 0x0018
	*
	* Return the count of given type.  The count depends on previous call to  0x0038
	*/
int ipod_count ( AAP_TYPE type );


/**
	* 0x001A
	*
	* Return track title for given range
	*/
vector<string> ipod_titles ( AAP_TYPE type, unsigned int offset, unsigned int length );

/**
	* 0x001C
	*
	* Returns current track status
	*/
int ipod_track_status ( aap_track_status* status );

/**
	* 0x001E
	*
	* Current position in playlist
	*/
int ipod_current_position();

/**
	* 0x0020
	*
	* Returns the track title in given position in playlist
	*/
const char* ipod_track_title ( int position, char* name, size_t size );

/**
	* 0x0022
	*
	* Returns the track artisti in given position ion playlist
	*/
const char* ipod_track_artist ( int position, char* name, size_t size );

/**
	* 0x0024
	*
	* Returns the track album in given position ion playlist
	*/
const char* ipod_track_album ( int position, char* name, size_t size );

/**
	* 0x0028
	*
	* Switch to track.  Track depends on previous call to switch() 0x0017
	*/
int ipod_execute ( int position );

/**
	* 0x00029
	*
	* Toogle play status
	*
	* Play/Pause=0x01, Stop=0x02, Skip++=0x03, Skip--=0x04, FFwd=0x05, FRwd=0x06, StopFF/RW=0x07
	*/
AAP_MODE ipod_control ( AAP_MODE mode );

/**
	* 0x002C
	*
	* Return the current shuffleMode.
	*
	* off=0x00
	* songs=0x01
	* albums=0x02
	*/
AAP_MODE ipod_shuffle_mode();


/**
	* 0x002E
	*
	* Set the current shuffleMode.
	*
	* off=0x00
	* songs=0x01
	* albums=0x02
	*/
AAP_MODE ipod_set_shuffle_mode ( AAP_MODE mode );

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
AAP_MODE ipod_repeat_mode();

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
AAP_MODE ipod_set_repeat_mode ( AAP_MODE mode );

/**
	* 0x0035
	*
	* Returns the number of songs in the current playlist
	*/
int ipod_playlist_count();

/**
	* 0x0037
	*
	* Jump to given track in current playlist
	*/
int ipod_jump ( int position );

/**
	* 0x0038
	*
	* Selects the type from the current playlist to filter by
	*/
int ipod_select ( AAP_TYPE type, int position );

#endif
