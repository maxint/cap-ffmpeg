### Compiler

CC=gcc
CXX=g++

CFLAGS= 	-O3 -Wall
CPPFLAGS=	
LDFLAGS=	-shared -Wl,--no-undefined

CP=cp -p

### Project

MNAME= ffmpeg
SRCS= ffmpeg.cpp ffmpeg_cap.cpp
INCS= -I../mingw-build/install/include
LIBS= -L../mingw-build/install/lib -lavcodec -lavformat -lavutil -lswscale -lpthread
CPPFLAGS += -D__STDC_CONSTANT_MACROS

#### Build

OUT:= $(MNAME).dll
OBJS:= $(SRCS:%.cpp=%.o)

all: $(OUT)

clean:
	@-rm $(OUT) $(OBJS) -f

%.o: %.cpp
	@echo CXX	$@
	@$(CXX) $(CFLAGS) $(CPPFLAGS) $(INCS) -c -o $@ $<

$(OUT): $(OBJS) 
	@echo CXX 	$@
	$(CXX) $(LDFLAGS) $^ $(LIBS) -o $@

