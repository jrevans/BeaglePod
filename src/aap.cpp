#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>
#include <string>

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <netinet/in.h>

#include <glib.h>

#include "aap.h"
#include "protocol.h"

#define BAUDRATE B19200
#define MODEMDEVICE "/dev/ttyUSB0"
//#define  MODEMDEVICE "/dev/ttyS0"

#define _POSIX_SOURCE 1 /* POSIX compliant source */

#define IO_SIZE 1000

using namespace std;

/**
 * pointer to msg recieved on serial port.  Its global
 * because complete msg might only arrive after mulitple
 * interupts
 */
unsigned char io[IO_SIZE];
unsigned char* ioptr;

/**
 * Opens serial connect
 */
aap_handle* aap_open_connection(aap_callbacks* method_callbacks)
{

    GIOChannel* ttyChannel;
    GSource* ttySource;
    
	printf ( "Connecting to %s...\n",MODEMDEVICE );
	aap_handle *aap = ( aap_handle* ) malloc ( sizeof ( aap_handle ) );

    aap->fd = open ( MODEMDEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK );
    if ( aap->fd <0 ) 
	{
		perror ( MODEMDEVICE );
		return NULL;
	}


	/* allow the process to receive SIGIO */
//	fcntl ( aap->fd, F_SETOWN, getpid() );

	/* Make the file descriptor asynchronous (the manual page says only
	O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
//	fcntl ( aap->fd, F_SETFL, FASYNC );

    tcgetattr ( aap->fd,&aap->oldtio ); /* save current port settings */

	//replace with memXXXX
    memset ( &aap->newtio, 0, sizeof ( struct termios ) );
    aap->newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    aap->newtio.c_iflag = 0; //IGNPAR;
    aap->newtio.c_oflag = 0;

	/* set input mode (non-canonical, no echo,...) */
    aap->newtio.c_lflag = 0;

    aap->newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
    aap->newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */

    tcflush ( aap->fd, TCIFLUSH );
    tcsetattr ( aap->fd,TCSANOW,&aap->newtio );

	printf ( "Connected\n" );

    aap->_callbacks = method_callbacks;

	//assign and fill
	ioptr=io;
	bzero ( io,IO_SIZE );

    //create channel
    ttyChannel = g_io_channel_unix_new(aap->fd);
	
	//setup callback
    g_io_add_watch( ttyChannel, (GIOCondition)(G_IO_IN|G_IO_ERR|G_IO_HUP|G_IO_NVAL), aap_callback, aap);
		
    return aap;

}

/**
 * Closes connection to ipod and cleans up
 */
void aap_close_connection ( aap_handle* aap)
{
	
	printf("Resetting %s\n",MODEMDEVICE);
	tcsetattr ( aap->fd,TCSANOW,&aap->oldtio );
	
	free(aap);
}

/**
 Send message to ipod
 */
void aap_transmit_msg ( aap_handle* ipod, aap_msg* msg )
{
	int wr,i;
	int size = msg->plen + 7;
	unsigned char data[size];
	int cmd = msg->cmd;
	int len;
	unsigned char *buffer = msg->parameters;

	memset ( data,0, size );

	//header
	data[0]=0XFF;
	data[1]=0x55;

	//length
	len = msg->plen+3;
	len = htonl ( len<<24 );
	memcpy ( data+2, &len,1 );

	//mode
	data[3]=0x04;

	//command
	cmd = htonl ( cmd<<16 );
	memcpy ( data+4, &cmd,2 );

	//parameter
	memcpy ( data+6,msg->parameters,msg->plen );

	//check sum
	unsigned char chks=0;
	for ( i=2;i<size-1;i++ )
	{
		chks+=data[i];
	}

	chks=0x100-chks;
	data[size-1]=chks;

	printf ( "Sending...\n" );
	for ( i=0;i<size;i++ )
	{
		printf ( "%#X:", data[i] );
	}
	printf ( "\n" );

	//send command to ipod
	wr=write ( ipod->fd,data,size );
}


/**
 * Builds a message for the given parameter string
 */
int aap_build_msg ( aap_msg *msg, unsigned char* buffer, int length )
{
	int read = 0;

	for (int i=0;i<length;i++ )
	{
		printf ( "%#X:", io[i] );
	}
	printf ( "\n" );
	
	/*
	 * Check to see if we have the minium length
	 */
	if ( length<7 ) 
		return 0;

	/*
	 * See if we have a header
	 */
	if ( buffer[0]==0xFF && buffer[1]==0X55 )
	{
		
		/*
		 * Make sure the complete message is here
		 */
		unsigned char msgLen = buffer[2] + 4;
		if ( length >= msgLen )
		{
			unsigned int *value = ( unsigned int * ) malloc ( sizeof ( int ) );;

			//cmd
			memcpy ( value, ( buffer+4 ),sizeof ( int ) );
			*value = htonl ( *value<<16 );
			msg->cmd=*value;

			//parameter length
			msg->plen = buffer[2]-3;

			//msg
			if ( msg->plen>0 )
			{
				unsigned char * data = ( unsigned char* ) malloc ( sizeof ( unsigned char ) *msg->plen );
				//construct message
				memcpy ( data,buffer+6,msg->plen );
				msg->parameters=data;
			}

			//raw data
			msg->raw=buffer;


			//shift unread
			length-=msgLen;
			memmove ( buffer,buffer+msgLen,length );

			return msgLen;

		}


	}
	else
	{
		printf ( "Bogus first byte\n" );
		length--;
		memmove ( buffer,buffer+1,length );
		read = aap_build_msg ( msg,buffer,length );
		read++;
	}

	return read;
}

/**
 * Reads message from the serial port and tries to build a message
 */
aap_msg* aap_recieve_msg ( aap_handle* ipod )
{
    aap_msg *msg = new aap_msg();;
	memset ( msg,'0',sizeof ( msg ) );

	printf ( "Reading\n" );
	
	//read and move the pointer
	ioptr += read( ipod->fd,ioptr,IO_SIZE );

	//build message and move pointer
	ioptr -= aap_build_msg (msg, io, ioptr-io );
    
//    if (msg->cmd=0){
//        delete(msg);
//        return NULL;
//    }
    
	return msg;
}

/**
 * Composes an ack msg
 */
void aap_create_ack_msg ( aap_msg* out, aap_msg* in )
{
	unsigned char *parameters = ( unsigned char * ) malloc ( sizeof ( unsigned char ) *3 );

	//results
	parameters[0]=0;

	//cmd echo
	int cmd = in->cmd;
	cmd = htonl ( cmd<<16 );
	memcpy ( parameters+1, &cmd,2 );

	out->cmd=IPOD_RESULT;
	out->parameters=parameters;
	out->plen=3;

}

/**
 * Reads the message off the serial line and callsback
 */
static gboolean aap_callback ( GIOChannel *channel, GIOCondition condition, gpointer data )
{

	aap_handle* hd = ( aap_handle* ) data;

	switch ( condition )
	{
		case G_IO_IN:
		{

			aap_msg* msg_in = aap_recieve_msg ( hd );
//           if (msg_in == NULL)
//                break;
            
			switch ( msg_in->cmd )
			{
				case SWITCH_MODE_AIR:  //if (buf[4]==0x01 && buf[5]==0x04)
				{
					aap_msg msg;
					printf ( "AIR mode requested\n" );

					hd->_callbacks->airMode();

					break;
				}
				case AAP_SIZE:  //else if (buf[4]==0x00 && buf[5]==0x12)
				{
					aap_msg msg;
					char size[2];

					printf ( "Size request\n" );

					hd->_callbacks->id ( size, 2 );

					msg.cmd = IPOD_SIZE;
					msg.parameters= ( unsigned char* ) size;
					msg.plen=2;
					aap_transmit_msg ( hd, &msg );

					break;
				}
				case AAP_STATUS:  //else if (buf[4]==0x00 && buf[5]==0x1C)
				{
					aap_msg msg;
					unsigned char buffer[9];
					aap_track_status status;
					int total, elapse;

					printf ( "Status\n" );

					//track time
					hd->_callbacks->trackStatus ( &status );
					total = htonl ( status.time );
					memcpy ( buffer, &total,4 );

					//elaspe time
					elapse = htonl ( status.elapse );
					memcpy ( buffer+4, &elapse,4 );

					//status STOP|PLAYING|PAUSE
					buffer[8]=status.mode;

					msg.cmd = IPOD_STATUS;
					msg.parameters=buffer;
					msg.plen=9;

					aap_transmit_msg ( hd, &msg );

					break;
				}
				case AAP_SWITCH_MAIN_PLAYLIST:  //else if (buf[4]==0x00 && buf[5]==0x16)
				{
					aap_msg msg;
					printf ( "Switch Main Playlist\n" );

					hd->_callbacks->switchToMain();

					aap_create_ack_msg ( &msg,msg_in );

					aap_transmit_msg ( hd, &msg );

					break;
				}
				case AAP_EXECUTE: //else if (buf[4]==0x00 && buf[5]==0x28)
				{
					aap_msg msg;
					unsigned int *indexPtr;
					unsigned int index;

					printf ( "Execute\n" );

					indexPtr = ( unsigned int* ) msg_in->parameters;
					index = ntohl ( *indexPtr );

                    if (index == APP_FIRST_TRACK)
                        index=0;
                    
                    //update the track position
                    hd->position = index;

					hd->_callbacks->execute ( index );

					aap_create_ack_msg ( &msg, msg_in );
					aap_transmit_msg ( hd, &msg );

					break;
				}
				case AAP_CONTROL: //else if (buf[4]==0x00 && buf[5]==0x29)
				{
					AAP_MODE command;
					aap_msg msg;
					printf ( "Control\n" );

					command = msg_in->parameters[0];

					hd->_callbacks->control ( command );

					aap_create_ack_msg ( &msg, msg_in );
					aap_transmit_msg ( hd, &msg );

					break;
				}
				case AAP_COUNT: //else if (buf[4]==0x00 && buf[5]==0x18)
				{
					aap_msg msg;
					AAP_TYPE type;
					unsigned char buffer[4];
					int count;

					type = msg_in->parameters[0];

					printf ( "Count of %X\n", type );

					count = hd->_callbacks->count ( type );

					msg.cmd=IPOD_COUNT;

					count = htonl ( count );
					memcpy ( buffer, &count,4 );
					msg.parameters=buffer;
					msg.plen=4;
					aap_transmit_msg ( hd, &msg );

					break;
				}
				case AAP_SHUFFLE_MODE: //else if (buf[4]==0x00 && buf[5]==0x2C)
				{
					aap_msg msg;
					AAP_MODE mode;
					unsigned char buffer[1];

					printf ( "Shuffle Mode\n" );

					mode = hd->_callbacks->shuffleMode();

					msg.cmd=IPOD_SHUFFLE_MODE;

					buffer[0]=mode;
					msg.parameters = buffer;
					msg.plen=1;
					aap_transmit_msg ( hd, &msg );

					break;
				}
				case AAP_REPEAT_MODE: //else if (buf[4]==0x00 && buf[5]==0x2F)
				{
					aap_msg msg;
					AAP_MODE mode;
					unsigned char buffer[1];

					printf ( "Repeat Mode\n" );

					mode = hd->_callbacks->repeatMode();

					msg.cmd=IPOD_REPEAT_MODE;

					buffer[0]=mode;
					msg.parameters = buffer;
					msg.plen=1;
					aap_transmit_msg ( hd, &msg );

					break;
				}
				case AAP_SET_REPEAT_MODE: //else if (buf[4]==0x00 && buf[5]==0x31)
				{
					aap_msg msg;
					AAP_MODE mode;

					printf ( "Set Repeat Mode\n" );

					mode = msg_in->parameters[0];
					hd->_callbacks->setRepeatMode ( mode );

					aap_create_ack_msg ( &msg, msg_in );
					aap_transmit_msg ( hd, &msg );

					break;
				}
				case AAP_POLLING_MODE: //else if (buf[4]==0x00 && buf[5]==0x26 && buf[6]==0x01)
				{
					aap_msg msg;
					AAP_MODE mode;

					printf ( "Polling Mode\n" );

					mode = msg_in->parameters[0];
					if (mode)
                    {
                        hd->polling=TRUE;
                        g_timeout_add(500, aap_push_status , hd);
                    } else {
						hd->polling = FALSE;
                    }
                    
					aap_create_ack_msg ( &msg, msg_in );
					aap_transmit_msg ( hd, &msg );

					break;
				}
				case AAP_PLAYLIST_COUNT: //else if (buf[4]==0x00 && buf[5]==0x35)
				{
					aap_msg msg;
					unsigned char buffer[4];
					int count;

					printf ( "Playlist Count\n" );

					count = hd->_callbacks->playlistCount();
					count = htonl ( count );


					msg.cmd=IPOD_PLAYLIST_COUNT;

					memcpy ( buffer, &count,4 );
					msg.parameters = buffer;
					msg.plen=4;
					aap_transmit_msg ( hd, &msg );

					break;
				}
				case AAP_CURRENT_POSITION: //else if (buf[4]==0x00 && buf[5]==0x1E)
				{
					aap_msg msg;
					unsigned char buffer[4];
					int currentIndex;

					printf ( "Current Position\n" );

					currentIndex = hd->_callbacks->currentPosition();
					currentIndex = htonl ( currentIndex );

					msg.cmd=IPOD_CURRENT_POSITION;

					memcpy ( buffer, &currentIndex,4 );
					msg.parameters = buffer;
					msg.plen=4;
					aap_transmit_msg ( hd, &msg );

					break;
				}
				case AAP_TRACK_INFO: //else if (buf[4]==0x00 && buf[5]==0x0C)
				{
					aap_msg msg;
					int elaspe;
					unsigned char buffer[11];

					printf ( "Track Info\n" );
					printf ( "Unhandled request....." );
					//exit ( -1 );

					break;
				}
				case AAP_TRACK_TITLE: //else if (buf[4]==0x00 && buf[5]==0x20)
				{
					unsigned int *indexPtr;
					unsigned int index;
					char title[256];
					aap_msg msg;

					printf ( "Track Title\n" );

					indexPtr = ( unsigned int* ) msg_in->parameters;
					index = ntohl ( *indexPtr );

					hd->_callbacks->trackTitle ( index,title,256 );

					msg.cmd=IPOD_TRACK_TITLE;

					msg.parameters = ( unsigned char* ) title;
					msg.plen=strlen ( title ) +1;
					aap_transmit_msg ( hd, &msg );

					break;
				}
				case AAP_TRACK_ALBUM: //else if (buf[4]==0x00 && buf[5]==0x24)
				{
					unsigned int *indexPtr;
					unsigned int index;
					char album[256];
					aap_msg msg;

					printf ( "Track Album\n" );

					indexPtr = ( unsigned int* ) msg_in->parameters;
					index = ntohl ( *indexPtr );

					hd->_callbacks->trackAlbum ( index,album,256 );

					msg.cmd=IPOD_TRACK_ALBUM;
					msg.parameters = ( unsigned char* ) album;
					msg.plen=strlen ( album ) +1;
					aap_transmit_msg ( hd, &msg );

					break;
				}
				case AAP_TRACK_ARTIST: //else if (buf[4]==0x00 && buf[5]==0x22)
				{
					unsigned int *indexPtr;
					unsigned int index;
					char artist[256];
					aap_msg msg;

					printf ( "Tract Artist\n" );

					indexPtr = ( unsigned int* ) msg_in->parameters;
					index = ntohl ( *indexPtr );
					hd->_callbacks->trackArtist ( index,artist, 256 );

					msg.cmd=IPOD_TRACK_ARTIST;
					msg.parameters = ( unsigned char* ) artist;
					msg.plen=strlen ( artist ) +1;
					aap_transmit_msg ( hd, &msg );
					break;
				}
				case AAP_JUMP: //else if (buf[4]==0x00 && buf[5]==0x37)
				{
					unsigned int *indexPtr;
					unsigned int nextIndex;
					aap_msg msg;

					printf ( "Jump\n" );

					indexPtr = ( unsigned int* ) msg_in->parameters;
					nextIndex = ntohl ( *indexPtr );

					hd->_callbacks->jump ( nextIndex );

					//started = time ( NULL );

					aap_create_ack_msg ( &msg, msg_in );
					aap_transmit_msg ( hd, &msg );

					break;
				}
				case AAP_NAMES: //else if (buf[4]==0x00 && buf[5]==0x1A)
				{
					aap_msg msg;
					AAP_TYPE type;
					unsigned int *indexPtr;
					unsigned int offset=0, length=0, i=0;
					unsigned char* parameters;
					vector<string> names;

					printf ( "Names\n" );

					parameters = msg_in->parameters;
					type = parameters[0];

					indexPtr = ( unsigned int* ) ( parameters+1 );
					offset = ntohl ( *indexPtr );

					indexPtr = ( unsigned int* ) ( parameters+5 );
					length = ntohl ( *indexPtr );

					names = hd->_callbacks->titles ( type, offset, length );

					for ( i=0;i<length;i++ )
					{
						string name = names[i];
						size_t msg_length = name.length() +5; //offset(number(4)) + name + \0
						unsigned char buffer[msg_length];
						unsigned int position;

						position = htonl ( i+offset );
						memcpy ( buffer,&position,4 );

						printf ( "%s\n", name.c_str() );
						memcpy ( buffer+4,name.c_str(),name.length() +1 );

						msg.cmd=IPOD_NAME;
						msg.parameters = buffer;
						msg.plen=msg_length;
						aap_transmit_msg ( hd, &msg );
					}

					break;
				}
				case AAP_38: //else if (buf[4]==0x00 && buf[5]==0x38)
				{
					unsigned char* parameters;
					unsigned int *indexPtr;
					unsigned int position;
					AAP_TYPE type;
					aap_msg msg;

					printf ( "Select\n" );

					parameters = msg_in->parameters;
					type = parameters[0];

					indexPtr = ( unsigned int* ) ( parameters+1 );
					position = ntohl ( *indexPtr );

					hd->_callbacks->select ( type,position );

					aap_create_ack_msg ( &msg, msg_in );
					aap_transmit_msg ( hd, &msg );

					break;
				}
				default:
					printf ( "Unhandled AAP message\n" );
			}
            
            //housekeeping
            delete(msg_in);
			
            break;
		}
		default:
			printf ( "Unhandled I/O condition\n" );
	}

	return TRUE;
}


static gboolean aap_push_status(gpointer data)
{
    aap_handle* hd = (aap_handle*)data;
    
	aap_msg msg;
	unsigned char *buffer;
	aap_track_status status;

	buffer = ( unsigned char* ) malloc ( sizeof ( unsigned char ) *5 );
	if ( buffer == NULL )
	{
		printf ( "Opps" );
		exit ( 1 );
	}

	hd->_callbacks->trackStatus(&status);
    
    //The track changed since last status update
    if (status.position != hd->position)
    {
        int currentIndex;
        
        printf ( "Pushing track change\n" );
        
        buffer[0]=0x01;
        currentIndex = htonl ( (int) status.position );
        memcpy ( buffer+1, &currentIndex,4 );
        
        //update the track position
        hd->position = status.position;
    }
    else
    {
        time_t elapse;
        
        printf ( "Pushing elapsed time\n" );
        
        buffer[0]=0x04;
        elapse = htonl ( ( int ) status.elapse );
        memcpy ( buffer+1,&elapse,4 );
    }
    

	msg.cmd=IPOD_ELAPSED_TIME;
	msg.parameters = buffer;
	msg.plen=5;

	aap_transmit_msg ( hd, &msg );
    
	return hd->polling;
}














