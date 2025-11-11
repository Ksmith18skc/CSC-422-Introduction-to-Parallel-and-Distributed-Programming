# Driver target
driver: driver.o myMalloc.o myMalloc-helper.o
	gcc -o driver driver.o myMalloc.o myMalloc-helper.o -lpthread

driver.o: driver.c myMalloc.h
	gcc -g -c driver.c

# Multithreaded Driver target
driver_multithreaded: driver_multithreaded.o myMalloc.o myMalloc-helper.o
	gcc -o driver_multithreaded driver_multithreaded.o myMalloc.o myMalloc-helper.o -lpthread

driver_multithreaded.o: driver_multithreaded.c myMalloc.h
	gcc -g -c driver_multithreaded.c

# TestSeq target
testSeq: testSeq.o myMalloc.o myMalloc-helper.o
	gcc -o testSeq testSeq.o myMalloc.o myMalloc-helper.o -lpthread

testSeq.o: testSeq.c myMalloc.h
	gcc -g -c testSeq.c

# TestCoarse target
testCoarse: testCoarse.o myMalloc.o myMalloc-helper.o
	gcc -o testCoarse testCoarse.o myMalloc.o myMalloc-helper.o -lpthread

testCoarse.o: testCoarse.c myMalloc.h
	gcc -g -c testCoarse.c

# TestFine target
testFine: testFine.o myMalloc.o myMalloc-helper.o
	gcc -o testFine testFine.o myMalloc.o myMalloc-helper.o -lpthread

testFine.o: testFine.c myMalloc.h
	gcc -g -c testFine.c

# myMalloc and helper targets
myMalloc.o: myMalloc.c myMalloc.h myMalloc-helper.h
	gcc -g -c myMalloc.c

myMalloc-helper.o: myMalloc-helper.c myMalloc-helper.h
	gcc -g -c myMalloc-helper.c

# Build all targets
all: driver driver_multithreaded testSeq testCoarse testFine

# Clean target
clean:
	rm -f *.o driver driver_multithreaded testSeq testCoarse testFine