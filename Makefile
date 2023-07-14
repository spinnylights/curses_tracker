CPPFLAGS := -std=c++17 $(shell pkg-config --cflags --libs ncurses sdl2)

curses_tracker: main.o sdl_util.o
	c++ -o $@ $(CPPFLAGS) $^

main.o: main.cpp
	c++ -c -o $@ $(CPPFLAGS) $^

sdl_util.o: sdl_util.cpp
	c++ -c -o $@ $(CPPFLAGS) $^

.PHONY: clean

clean:
	rm curses_tracker *.o
