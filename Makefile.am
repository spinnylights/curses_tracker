bin_PROGRAMS = curses_tracker
if TESTS
bin_PROGRAMS += tests
endif

curses_tracker_CXXFLAGS = -I$(top_srcdir)/include/$(PACKAGE_NAME) $(SDL_CFLAGS) $(CURSES_CFLAGS) $(SQLITE3_CFLAGS)
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
	src/pulse_delay.cpp \
	src/toggle.cpp \
	src/stopwatch.cpp \
	src/wavetable.cpp \
	src/sequencer.cpp \
	src/audio_sdl.cpp

tests_CXXFLAGS = $(curses_tracker_CXXFLAGS) -I/usr/include/doctest/ -I/usr/local/include/doctest
tests_LDADD = $(curses_tracker_LDADD)
tests_SOURCES = \
	src/pulse_delay.cpp \
	test/pulse_delay.cpp \
	src/pulse_train.cpp \
	test/pulse_train.cpp \
	test/main.cpp
