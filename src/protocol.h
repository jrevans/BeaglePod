#ifndef PROTOCOL_H
#define PROTOCOL_H


/**
 * Messages sent to the IPOD
 */
#define SWITCH_MODE_AIR 0x0104

#define AAP_TRACK_INFO 0x000c

#define AAP_SIZE 0x0012

#define AAP_DEVICE_NAME 0x0014

#define AAP_SWITCH_MAIN_PLAYLIST 0x0016

#define AAP_SWITCH 0x0017 

#define AAP_COUNT 0x0018 

#define AAP_NAMES 0x001A

#define AAP_STATUS 0x001C 

#define AAP_CURRENT_POSITION 0x001E 

#define AAP_TRACK_TITLE 0x0020 

#define AAP_TRACK_ARTIST 0x0022 

#define AAP_TRACK_ALBUM 0x0024 

#define AAP_POLLING_MODE 0x0026

#define AAP_EXECUTE 0x0028

#define AAP_CONTROL 0X0029

#define AAP_SHUFFLE_MODE 0x002C

#define AAP_REPEAT_MODE 0X002F

#define AAP_SET_REPEAT_MODE 0x0031

#define AAP_PLAYLIST_COUNT 0x0035

#define AAP_JUMP 0x0037

#define AAP_38 0x0038

/**
 * Messages sent from the IPod
 */
#define IPOD_RESULT 0x0001

#define IPOD_TRACK_INFO 0x000d

#define IPOD_SIZE 0x0013;

#define IPOD_DEVICE_NAME 0x0015

#define IPOD_COUNT 0x0019

#define IPOD_NAME 0x001B

#define IPOD_STATUS 0x001D

#define IPOD_CURRENT_POSITION 0x001F

#define IPOD_TRACK_TITLE 0x0021

#define IPOD_TRACK_ARTIST 0x0023

#define IPOD_TRACK_ALBUM 0x025

#define IPOD_ELAPSED_TIME 0x0027

#define IPOD_SHUFFLE_MODE 0x002D

#define IPOD_SET_SHUFFLE_MODE 0x002E

#define IPOD_REPEAT_MODE 0x0030

#define IPOD_PLAYLIST_COUNT 0x0036


/**
 * Controls
 */
#define AAP_MODE unsigned char

#define MODE_STOPPED 0x00

#define MODE_PLAYING 0x01

#define MODE_PAUSED 0x02

/**
 * Actions Modes
 */
#define AAP_ACTION unsigned char

#define ACTION_PLAY 0x01 

#define ACTION_STOP 0x02 

#define ACTION_NEXT 0x03 

#define ACTION_PREVIOUS 0x04 

#define ACTION_FF 0x05 

#define ACTION_RR 0x06

#define ACTION_RESUME 0x07


#define APP_FIRST_TRACK 0xFFFFFF

/**
 * Types
 */
#define AAP_TYPE unsigned char

#define TYPE_PLAYLIST 0x01

#define TYPE_ARTIST 0x02

#define TYPE_ALBUM 0x03

#define TYPE_GENRE 0x04

#define TYPE_SONG 0x05

#define TYPE_COMPOSER 0x06

#define TYPE_PODCAST 0x08


struct aap_track_status
{
	int time;
	int elapse;
	AAP_MODE mode;
    int position;
};

#endif