OBJ=OffMon.o
all	:	${OBJ}
	gcc -mwindows -mno-cygwin -o OffMon.exe ${OBJ} -lstdc++

clean:
	rm -f *.o *.exe
