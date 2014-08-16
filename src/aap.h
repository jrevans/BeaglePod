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
#ifndef AAP_H
#define AAP_H

#include <termios.h>

#include <cstring>
#include <vector>
#include <string>

#include <glib.h>

#include "protocol.h"


struct aap_callbacks {
	void (*airMode)();
	char* (*id)(char *, size_t);
	int (*trackStatus)(aap_track_status *);	
	void (*switchToMain)();
	int (*execute)(int);
    AAP_ACTION (*control)(AAP_ACTION);
	int (*count)(AAP_TYPE);
	AAP_MODE (*shuffleMode)();
	AAP_MODE (*repeatMode)();
	AAP_MODE (*setRepeatMode)(AAP_MODE);
	int (*playlistCount)();
	int (*currentPosition)();
	const char* (*trackTitle)(int, char *, size_t);
    const char*  (*trackAlbum)(int, char *, size_t);
    const char*  (*trackArtist)(int, char *, size_t);
	int (*jump)(int);
    std::vector<std::string> (*titles)(AAP_TYPE, unsigned int, unsigned int);
	int (*select)(AAP_TYPE, int);
};

/**
 * Maintains reference to serial ports used for connection
 */
struct aap_handle
{
	termios oldtio;
	termios newtio;
	int fd;
	aap_callbacks* _callbacks;
    int position;
	int polling;
};


/**
 * AAP message, see http://www.ipodlinux.org/wiki/Apple_Accessory_Protocol
 */
struct aap_msg
{
	int mode;
	int cmd;
	unsigned char* parameters;
	size_t plen;
	unsigned char checksum;
	unsigned char* raw;
};


aap_handle* aap_open_connection(aap_callbacks* );
		
void aap_close_connection ( aap_handle* ipod);

void aap_transmit_msg ( aap_handle* ipod, aap_msg* msg );

int aap_build_msg ( aap_msg *msg, unsigned char* source, int length );

aap_msg* aap_recieve_msg ( aap_handle* ipod );

void aap_create_ack_msg ( aap_msg* out, aap_msg* in);

static gboolean aap_callback ( GIOChannel *channel, GIOCondition condition, gpointer data);

static gboolean aap_push_status(gpointer data);
/*

int aap_initialize(aap_callbacks* method_callbacks);


void sentStatus(aap_handle* hd);
*/
#endif
