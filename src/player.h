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
#ifndef PLAYER_H
#define PLAYER_H

#include <cstring>
#include <vector>
#include <map>
#include <string>

#include "protocol.h"

using namespace std;

#define player_songlist vector< const map<string, const string*>* >

struct player_handle {
	AAP_MODE _controlMode; //stop play pause
	AAP_MODE _shuffleMode;
	AAP_MODE _repeatMode;
	int _currentPosition;
	player_songlist _playlist;
};

player_handle* player_create();

const player_songlist player_playlist(player_handle*);

int player_set_playlist(player_handle*, player_songlist, int);

int player_set_control_mode(player_handle*, AAP_MODE);

int player_contorl_mode(player_handle*);

int player_set_shuffle_mode(player_handle*, AAP_MODE);

int player_shuffle_mode(player_handle*);

int player_set_repeat_mode(player_handle*, AAP_MODE);

int player_repeat_mode(player_handle*);

int player_playlist_count(player_handle*);

int player_current_position(player_handle*); 

int player_status(player_handle*, aap_track_status*);

int player_jump(player_handle*, int position);
        
int player_set_current_position(player_handle* player, int position);

#endif
