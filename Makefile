plz : plz.o linkedlist.o
	gcc plz.o linkedlist.o -o plz -lpthread
plz.o : plz.c plz.h
	gcc plz.c -c -o plz.o -lpthread
linkedlist.o : linkedlist.c plz.h
	gcc linkedlist.c -c -o linkedlist.o
clean :
	rm *.o
