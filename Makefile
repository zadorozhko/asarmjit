ASDIR = ${HOME}/Work/SimulIDE-dev/src/angel
CXXFLAGS = -arch arm64 -mcpu=apple-m1 -g -Wall -I${ASDIR}/include -Iarmjit
LD = ld
LDFLAGS = -arch arm64 -lstdc++ -L${ASDIR}/lib -langelscript
DELETER = rm -f

OBJDIR = objs
SRCNAMES = \
	armjit/block.cpp \
	armjit/registermanager.cpp \
	armjit/asregister.cpp \
	armjit/armregistermanager.cpp \
	armjit/vfpregistermanager.cpp \
	armjit/as_jit_arm.cpp \
	armjit/as_jit_arm_op.cpp \
	asjit.cpp utils.cpp
SRCDIR = ./

OBJ = $(SRCNAMES:.cpp=.o)


BIN = asjit

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	$(DELETER) $(OBJ) $(BIN) *.bak


.PHONY: all clean install uninstall
