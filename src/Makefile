### Compiler

CC		= gcc
CXX		= g++

CFLAGS  = -O3 -Wall -I.
CPPFLAGS=
LDFLAGS	= -shared -Wl,--no-undefined

CP		= cp -p
RM		= rm -rf
MV		= mv
MKDIR	= mkdir -p

# hidden
CFLAGS  += -fvisibility=hidden -fdata-sections -ffunction-sections
CPPFLAGS+= -fvisibility-inlines-hidden
LDFLAGS += -Wl,--exclude-libs,ALL -Wl,--gc-sections -Wl,--as-needed -Wl,--strip-all

### Project

MNAME	= ffmpeg_cap
HEADERS	= ffmpeg_cap.h
SRCS	= ffmpeg_cap_impl.cpp ffmpeg_cap.cpp
INCS	= -I../build-ffmpeg-mingw32/install/include
LIBS	= -L../build-ffmpeg-mingw32/install/lib -lavformat -lavcodec -lavutil -lswresample -lswscale -static -liconv -static-libstdc++ -static-libgcc
# LIBS	= -L../build-ffmpeg-mingw32/install/lib -L../build-x264-mingw32/install/lib -lavformat -lavcodec -lavutil -lswresample -lswscale -lx264 -static -liconv -static-libstdc++ -static-libgcc
CFLAGS 	+= -Dffmpeg_cap_EXPORTS
CPPFLAGS+= -D__STDC_CONSTANT_MACROS
LDFLAGS += -Wl,--output-def=$(DEFOUT)
ifndef PREFIX
	PREFIX= ../install
endif

#### Build

DLLOUT	:= $(MNAME).dll
DEFOUT	:= $(MNAME).def
OBJS	:= $(SRCS:%.cpp=%.o)

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
	$(CXX) ${CFLAGS} ${CPPFLAGS} $(LDFLAGS) $^ $(LIBS) -o $@

$(DEFOUT): $(DLLOUT)
