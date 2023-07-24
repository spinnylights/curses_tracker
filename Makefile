CPPFLAGS := -Og -ggdb -std=c++17 $(shell pkg-config --cflags --libs ncurses sdl2 sqlite3)

curses_tracker: main.o sdl_util.o ui_curses.o audio_sdl.o event.o chk_errno.o note.o db.o exceptions.o curve_db.o curve.o
	c++ -o $@ $(CPPFLAGS) $^

main.o: main.cpp
	c++ -c -o $@ $(CPPFLAGS) $^

sdl_util.o: sdl_util.cpp sdl_util.hpp
	c++ -c -o $@ $(CPPFLAGS) $<

ui_curses.o: ui_curses.cpp ui_curses.hpp note.o
	c++ -c -o $@ $(CPPFLAGS) $<

audio_sdl.o: audio_sdl.cpp audio_sdl.hpp
	c++ -c -o $@ $(CPPFLAGS) $<

event.o: event.cpp event.hpp
	c++ -c -o $@ $(CPPFLAGS) $<

chk_errno.o: chk_errno.cpp chk_errno.hpp
	c++ -c -o $@ $(CPPFLAGS) $<

note.o: note.cpp note.hpp
	c++ -c -o $@ $(CPPFLAGS) $<

db.o: db.cpp db.hpp exceptions.o
	c++ -c -o $@ $(CPPFLAGS) $<

exceptions.o: exceptions.cpp exceptions.hpp
	c++ -c -o $@ $(CPPFLAGS) $<

curve_db.o: curve_db.cpp curve_db.hpp
	c++ -c -o $@ $(CPPFLAGS) $<

curve.o: curve.cpp curve.hpp
	c++ -c -o $@ $(CPPFLAGS) $<

.PHONY: clean

clean:
	rm curses_tracker *.o
