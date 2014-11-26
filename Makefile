ALL=sensible-cvsb983

sensible-cvsb983:
	gcc -o sensible-cvsb983 src/sensible.c -ludev 

install:
	install sensible-cvsb983 /usr/local/bin
