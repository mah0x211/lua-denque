TARGET=$(PACKAGE).$(LIB_EXTENSION)
SRC=$(wildcard src/*.c)
OBJ=$(SRC:.c=.o)
GCDAS=$(OBJ:.o=.gcda)
INSTALL?=install

ifdef DEQUE_COVERAGE
COVFLAGS=--coverage
endif


.PHONY: all install

all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) $(WARNINGS) $(COVFLAGS) $(CPPFLAGS) -o $@ -c $<

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS) $(PLATFORM_LDFLAGS) $(COVFLAGS)

install:
	$(INSTALL) $(TARGET) $(LIBDIR)
	rm -f $(OBJ) $(TARGET) $(GCDAS)
