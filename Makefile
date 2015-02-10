ALL=sensible-cvsb983

sensible-cvsb983:
	gcc -o src/sensible-cvsb983 src/sensible-cvsb983.c -Wall -ludev 

clean:
	rm src/sensible-cvsb983

install:
	install src/sensible-cvsb983 /usr/local/bin
