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

#include "player.h"

#include <cstdlib>
#include <iostream>

#include <gst/gst.h>

using namespace std;

GstElement *play = NULL;
GstBus *bus;

string folder = "file:///home/john/Music/iTunes/iTunes Music/";

static gboolean my_bus_callback ( GstBus *bus, GstMessage *message, gpointer data )
{
	player_handle* player = ( player_handle* ) data;

	g_print ( "Got %s message\n", GST_MESSAGE_TYPE_NAME ( message ) );

	switch ( GST_MESSAGE_TYPE ( message ) )
	{
		case GST_MESSAGE_ERROR:
		{
			GError *err;
			gchar *debug;

			gst_message_parse_error ( message, &err, &debug );
			g_print ( "Error: %s\n", err->message );
			g_error_free ( err );
			g_free ( debug );

			break;
		}
		case GST_MESSAGE_EOS:
			player_jump ( player, player->_currentPosition+1 );
			break;
		default:
			/* unhandled message */
			break;
	}

	return TRUE;
}

int player_play_song ( player_handle* player )
{

	if ( play )
	{
		gst_element_set_state ( play, GST_STATE_NULL );
		gst_object_unref ( GST_OBJECT ( play ) );
	}

	const map<string, const string*>* song = player->_playlist[player->_currentPosition];
	const string* location = song->find ( "Location" )->second;
	string filename = folder + *location;

	gst_init ( 0, NULL );

	cout << "Reqeusted " << filename << endl;

	/* set up */
	play = gst_element_factory_make ( "playbin2", "play" );
	g_object_set ( G_OBJECT ( play ), "uri", filename.c_str() , NULL );

	bus = gst_pipeline_get_bus ( GST_PIPELINE ( play ) );
	gst_bus_add_watch ( bus, my_bus_callback, player );
	gst_object_unref ( bus );

	if ( player->_controlMode == MODE_PLAYING )
		gst_element_set_state ( play, GST_STATE_PLAYING );

//     _controlMode=MODE_PLAYING;

}

player_handle* player_create()
{
	player_handle* player;

	player = ( player_handle* ) new player_handle();
	if ( player == NULL )
		return NULL;

	player->_controlMode=MODE_STOPPED;
	player->_shuffleMode=0x00;
	player->_repeatMode=0x00;
	player->_currentPosition = 0;

	return player;
}

int player_set_control_mode ( player_handle* player, AAP_MODE action )
{


	switch ( action )
	{
		case ACTION_PLAY:
			player->_controlMode= ( player->_controlMode == MODE_PLAYING ) ?MODE_PAUSED:MODE_PLAYING;
			if ( player->_controlMode==MODE_PLAYING )
			{
				if ( gst_element_set_state ( GST_ELEMENT_CAST ( play ), GST_STATE_PLAYING ) == GST_STATE_CHANGE_FAILURE )
					cout << "State change failed, unable to pause" << endl;
				cout << "Playing...." << endl;
			}
			else
			{
				if ( gst_element_set_state ( GST_ELEMENT_CAST ( play ), GST_STATE_PAUSED ) == GST_STATE_CHANGE_FAILURE )
					cout << "State change failed, unable to play" << endl;
				cout << "Paused...." << endl;

			}
			break;

		case ACTION_STOP:
			player->_controlMode=MODE_STOPPED;
			if ( gst_element_set_state ( GST_ELEMENT_CAST ( play ), GST_STATE_PAUSED ) == GST_STATE_CHANGE_FAILURE )
				cout << "State change failed, unable to pause" << endl;
			break;

		case ACTION_NEXT:
			player_set_current_position ( player, player->_currentPosition+1 );
			break;
		case ACTION_PREVIOUS:
			player_set_current_position ( player, player->_currentPosition-1 );
			break;
		case ACTION_FF:
		case ACTION_RR:
		case ACTION_RESUME:
			break;
	}

	return player->_controlMode;

}

int player_contorl_mode ( player_handle* player )
{
	return player->_controlMode;
}

int player_set_shuffle_mode ( player_handle* player, AAP_MODE mode )
{
	player->_shuffleMode = mode;

	return player->_shuffleMode;
}

int player_shuffle_mode ( player_handle* player )
{
	return player->_shuffleMode;
}

int player_set_repeat_mode ( player_handle* player, AAP_MODE mode )
{
	player->_repeatMode = mode;
}

int player_repeat_mode ( player_handle* player )
{
	return player->_repeatMode;
}


int player_set_playlist ( player_handle* player, player_songlist playlist, int position=0 )
{

	player->_playlist=playlist;

	player_set_current_position ( player, position );

//     player_play_song(player);

}

const player_songlist player_playlist ( player_handle* player )
{
	return player->_playlist;
}

int player_playlist_count ( player_handle* player )
{
	return player->_playlist.size();
}

int player_current_position ( player_handle* player )
{
	return player->_currentPosition;
}

int player_set_current_position ( player_handle* player, int position )
{
	if ( position >= player->_playlist.size() )
		position=0;

	if ( position < 0 )
		position=0;

	player->_currentPosition=position;

	player_play_song ( player );

	return player->_currentPosition;
}

int player_status ( player_handle* player, aap_track_status* currentStatus )
{
	time_t elapse;

	GstFormat fmt = GST_FORMAT_TIME;
	gint64 pos=0, len=0;

	if ( play != NULL )
	{
		if ( gst_element_query_position ( GST_ELEMENT_CAST ( play ), &fmt, &pos ) )
		{
			cout << "Position: " << pos << endl;
		}

		if ( gst_element_query_duration ( GST_ELEMENT_CAST ( play ), &fmt, &len ) )
		{
			cout << "Duration: " << len << endl;
		}
	}
	currentStatus->time= ( int ) ( len/1000000 );
	currentStatus->elapse= ( int ) ( pos/1000000 );
	currentStatus->mode=player->_controlMode;
	currentStatus->position=player->_currentPosition;

	return 0;
}

int player_jump ( player_handle* player, int position )
{
	player_set_current_position ( player, position );

//    player_play_song(player);

}
