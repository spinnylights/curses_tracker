CPPFLAGS := -std=c++17 $(shell pkg-config --cflags --libs ncurses sdl2)

curses_tracker: main.o sdl_util.o ui_curses.o audio_sdl.o
	c++ -o $@ $(CPPFLAGS) $^

main.o: main.cpp
	c++ -c -o $@ $(CPPFLAGS) $^

sdl_util.o: sdl_util.cpp
	c++ -c -o $@ $(CPPFLAGS) $^

ui_curses.o: ui_curses.cpp
	c++ -c -o $@ $(CPPFLAGS) $^

audio_sdl.o: audio_sdl.cpp
	c++ -c -o $@ $(CPPFLAGS) $^

.PHONY: clean

clean:
	rm curses_tracker *.o
