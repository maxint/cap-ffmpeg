### Compiler

CC=gcc
CXX=g++

CFLAGS= 	-O3 -Wall
CPPFLAGS=	
LDFLAGS=	-shared -Wl,--no-undefined \
			-static-libgcc -static-libstdc++ -static -lpthread

CP=cp -p

### Project

MNAME= cv_ffmpeg
SRCS= ffmpeg.cpp ffmpeg_cap.cpp
INCS= -I../mingw32-build/install/include
LIBS= -L../mingw32-build/install/lib \
	  -lavformat -lavcodec -lavutil -lswscale
CPPFLAGS += -D__STDC_CONSTANT_MACROS -DCV_FFMPEG_EXPORTS
LDFLAGS += -Wl,--output-def=$(DEFOUT)

#### Build

DLLOUT:= $(MNAME).dll
LIBOUT:= $(MNAME).lib
DEFOUT:= $(MNAME).def
OBJS:= $(SRCS:%.cpp=%.o)

all: clean $(DLLOUT) $(LIBOUT)

clean:
	@-rm $(DLLOUT) $(OBJS) -f

%.o: %.cpp
	@echo CXX	$@
	@$(CXX) $(CFLAGS) $(CPPFLAGS) $(INCS) -c -o $@ $<

$(DLLOUT): $(OBJS) 
	@echo CXX 	$@
	$(CXX) $(LDFLAGS) $^ $(LIBS) -o $@

$(DEFOUT): $(DLLOUT)
$(LIBOUT): $(DEFOUT)
	@/d/TEMP/cmd_tools/vc6/lib /machine:x86 /def:$^ /out:$@
