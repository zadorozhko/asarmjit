CXXFLAGS = -O2 -I../angelscript/sdk/angelscript/include -isysroot /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS2.2.sdk -Iarmjit
CC = arm-apple-darwin9-gcc-4.0.1
CXX = arm-apple-darwin9-g++-4.0.1
LD = ld
LDFLAGS = -isysroot /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS2.2.sdk -lstdc++ -langelscript -L../angelscript/sdk/angelscript/lib-ipod
DELETER = rm -f

OBJDIR = ipod
SRCNAMES = test.cpp utils.cpp \
	armjit/block.cpp \
	armjit/registermanager.cpp \
	armjit/armregistermanager.cpp \
	armjit/vfpregistermanager.cpp \
	armjit/as_jit_arm.cpp
SRCDIR = ./

OBJ = $(SRCNAMES:.cpp=.o)
platform=/Developer/Platforms/iPhoneOS.platform
allocate=${platform}/Developer/usr/bin/codesign_allocate 


BIN = asjittest
	
all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(LDFLAGS)
	export CODESIGN_ALLOCATE=${allocate} && codesign -fs "Fredrik Ehnbom" $(BIN) 


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

depend:
	makedepend $(SRCNAMES) -f Makefile.depend 2> /dev/null
	
clean:
	$(DELETER) $(OBJ) $(BIN) *.bak


.PHONY: all clean install uninstall
include Makefile.depend
