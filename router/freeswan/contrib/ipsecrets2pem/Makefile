# $Id: Makefile,v 1.1.1.2 2005/03/28 06:57:36 sparq Exp $
CFLAGS=-g -Wall
LDLIBS=-lssl
TARGET=ipsecrets2pem
all : $(TARGET)

test: $(TARGET)
	./$(TARGET) < /etc/ipsec.secrets

check: $(TARGET)
	./$(TARGET) < /etc/ipsec.secrets | openssl rsa -check -noout

clean:
	rm -f $(TARGET) core *.o

