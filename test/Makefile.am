METASOURCES = AUTO
INCLUDES = -I$(top_srcdir)/src -I/usr/include/gstreamer-0.10 \
	-I/usr/lib/glib-2.0/include -I/usr/include/glib-2.0 -I/usr/include/libxml2
bin_PROGRAMS = testaap testitunes testplayer testplist
testplayer_SOURCES = player.cpp testplayer.cpp
testitunes_SOURCES = itunes.cpp plist.cpp testitunes.cpp
testitunes_LDADD = -lxml2
testaap_SOURCES = aap.cpp testaap.cpp
testaap_LDADD = -lglib-2.0
testplist_SOURCES = plist.cpp testplist.cpp
testplist_LDADD = -lxml2
testplayer_LDADD = -lgstreamer-0.10
