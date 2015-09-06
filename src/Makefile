### Compiler

CC		=gcc
CXX		=g++

CFLAGS  =		-O3 -Wall
CPPFLAGS=
LDFLAGS	=	-shared -Wl,--no-undefined -static-libgcc -static-libstdc++ -static -lpthread

CP		=cp -p
RM		=rm -rf
MV		=mv
MKDIR	=mkdir -p

### Project

MNAME	= ffmpeg_cap
HEADERS	= ffmpeg_cap.h
SRCS	= ffmpeg.cpp ffmpeg_cap.cpp
INCS	= -I../build-mingw32/install/include
LIBS	= -L../build-mingw32/install/lib -lavformat -lavcodec -lavutil -lswresample -lswscale -liconv
CPPFLAGS+= -D__STDC_CONSTANT_MACROS -DFFMPEG_EXPORTS
LDFLAGS += -Wl,--output-def=$(DEFOUT)
ifndef PREFIX
	PREFIX= ../install
endif

#### Build

DLLOUT:= $(MNAME).dll
DEFOUT:= $(MNAME).def
OBJS:= $(SRCS:%.cpp=%.o)

all: $(DLLOUT)

clean:
	@$(RM) *.o *.dll *.lib *.def *.exp

install: $(DLLOUT)
	$(MKDIR) $(PREFIX)/{,inc,bin}
	$(CP) $(HEADERS) $(PREFIX)/inc
	$(MV) $(DLLOUT) $(PREFIX)/bin

#### Detail Build

%.o: %.cpp
	@echo CXX	$@
	@$(CXX) $(CFLAGS) $(CPPFLAGS) $(INCS) -c -o $@ $<

$(DLLOUT): $(OBJS)
	@echo CXX	$@
	$(CXX) $(LDFLAGS) $^ $(LIBS) -o $@

$(DEFOUT): $(DLLOUT)
