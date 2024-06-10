record:
	mkdir -p bin && gcc -o bin/record record.c -ldl -lm -lpthread
loop:
	mkdir -p bin && gcc -o bin/loop loop.c -ldl -lm -lpthread
playback:
	mkdir -p bin && gcc -o bin/playback playback.c -ldl -lm -lpthread
