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
 
#include <iostream>

#include <unistd.h>
#include <gst/gst.h>

#include "player.h"
#include "protocol.h"

using namespace std;

int testStatus(){
    player_handle* player;
    aap_track_status status;
    
	player = player_create();
	if (player == NULL)
		cout << "Player::create failed" << endl;
	
	player_set_control_mode(player, MODE_PLAYING);
    
    sleep(10);

	if (player_status(player, &status)){
        cout << "Player::status failed" << endl;
        return -1;
    }
    
    if (status.mode!=MODE_PLAYING){
        cout << "Player::status wrong mode returned" << endl;
        return -1;
    }
    
    if (status.time!=3600000){
        cout << "Player::status wrong time" << endl;
        return -1;
    }
    
        
    if (status.elapse==0){
        cout << "Player::status no elapse time" << endl;
        return -1;
    }
    
    return 0;
}

int main ( int argc, char* argv[] )
{
	
	gst_init (&argc, &argv);
	

    if (testStatus())
        return 1;    
}

