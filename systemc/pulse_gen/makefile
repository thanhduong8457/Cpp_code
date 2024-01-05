# SystemC install path: Put the systemc BUILD dir here.
# Here it assumes the director is in /home/<username>/systemc-2.3.3/BUILD 
TARGET     = pulse_gen_systemc
#TARGETPATH = $(shell pwd)
TARGETPATH = /mnt/c/Users/duong.le-thanh/Documents/'Visual Studio 2015'/__Code__/systemc/pulse_gen_makefile

USERNAME = $(shell whoami)

SYSTEMC_DIR   = /mnt/d/MCU_Modeling/s956a/lib/systemc-2.3.1a

# Add source directoy for make to look in and add the project includes
INCS       += -I$(TARGETPATH)
VPATH      = $(TARGETPATH)

# Defaults sources, headers and objects (in the VPATH)
SOURCES   = $(wildcard ./*.cpp)
HEADERS   = $(wildcard ./*.h)
DATAFILES = $(wildcard *.dat *.vcd)
OBJECTS   = $(SOURCES:.cpp=.o)

# Compiler/linker and flags
CXX        =  g++
LXX        =  g++
CFLAGS     =  -I$(SYSTEMC_DIR)/include -fpermissive -std=c++11
CFLAGS     += $(INCS)
LDFLAGS    = -L$(SYSTEMC_DIR)/lib-linux64
LIBS       = -Wl,-Bstatic -lsystemc -Wl,-Bdynamic -lpthread

# "Phonies"
.PHONY: all clean

# Rules
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LXX) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $(TARGET)

$(OBJECTS): %.o:%.cpp $(HEADERS)
	$(CXX) -c $(CFLAGS) $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	-rm -rf $(OBJECTS) $(TARGET) $(DATAFILES)
	