phone:
	mkdir -p bin && gcc -o bin/phone phone.c -ldl -lm -lpthread
loop:
	mkdir -p bin && gcc -o bin/loop loop.c -ldl -lm -lpthread

