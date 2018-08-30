test:
	gcc test.c leptjson.h leptjson.c -o test
	./test
	rm test