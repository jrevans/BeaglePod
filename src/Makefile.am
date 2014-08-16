INCLUDES = -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 \
	-I/usr/lib/glib-2.0/include/ -I/usr/include/libxml2
METASOURCES = AUTO
noinst_HEADERS = aap.h itunes.h ipod.h protocol.h
bin_PROGRAMS = aapd
aapd_SOURCES = aap.cpp ipod.cpp itunes.cpp main.cpp player.cpp plist.cpp
aapd_LDADD = -lgstreamer-0.10 -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt \
	-lxml2 -lglib-2.0
