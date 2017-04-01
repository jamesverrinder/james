#!/bin/bash
COMPILE_ERROR="$(g++ -c src/*.cpp --std=c++11 2>&1)"

if [ -z "$COMPILE_ERROR" ]; then
	echo "saving output file..."
	LINK_ERROR=$(g++ -o StarshipFontana *.o -lSDL2 -lSDL2_image)
	echo "finished"
else
	echo "ERROR!"
	echo $COMPILE_ERROR
fi

