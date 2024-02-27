GCC = g++
CFLAGS = -std=c++17 -O2 -g 

CFILES = $(wildcard *.cpp)
HFILES = $(wildcard *.h)

all: main

main: $(HFILES) $(CFILES) 
	$(GCC) $(CFILES) -o $@ $(CFLAGS) 

.PHONY: clean
clean:
	rm main
