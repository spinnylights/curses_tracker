bin_PROGRAMS = curses_tracker

curses_tracker_CXXFLAGS = -I$(top_srcdir)/include $(SDL_CFLAGS) $(CURSES_CFLAGS) $(SQLITE3_CFLAGS)
curses_tracker_LDADD = $(SDL_LIBS) $(CURSES_LIBS) $(SQLITE3_LDFLAGS)
curses_tracker_SOURCES = \
	src/ui_curses.cpp \
	src/sdl_util.cpp \
	src/note.cpp \
	src/main.cpp \
	src/exceptions.cpp \
	src/db.cpp \
	src/curve_db.cpp \
	src/curve.cpp \
	src/curves.cpp \
	src/curses_curve_view.cpp \
	src/chk_errno.cpp \
	src/synth.cpp \
	src/delay.cpp \
	src/pulse_train.cpp \
	src/audio_sdl.cpp
