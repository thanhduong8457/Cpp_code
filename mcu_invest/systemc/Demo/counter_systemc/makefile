# SystemC install path: Put the systemc BUILD dir here.
# Here it assumes the director is in /home/<username>/systemc-2.3.3/BUILD 
TARGET     = counter_systemc
#TARGETPATH = $(shell pwd)
TARGETPATH = /mnt/c/Users/DELL/Documents/"Visual Studio 2019"/__My_Code__/systemc/C++/counter_systemc

USERNAME = $(shell whoami)

SYSTEMC_DIR   = /mnt/e/s956a/lib/systemc-2.3.1a

# Add source directoy for make to look in and add the project includes
INCS       += -I$(TARGETPATH)/inc
VPATH      = $(TARGETPATH)/src

# Defaults sources, headers and objects (in the VPATH)
SOURCES   = $(wildcard src/*.cpp)
HEADERS   = $(wildcard src/*.h)
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
	