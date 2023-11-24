CC = g++
CXX = g++
LD = g++

INCLUDE_DIRS := src/client src/server src/
INCLUDES = $(addprefix -I, $(INCLUDE_DIRS))

TARGETS = src/client/user src/server/server
TARGET_EXECS = user AS

CLIENT_SOURCES := $(wildcard src/client/*.cpp)
UTILS_SOURCES := $(wildcard src/utils/*.cpp)
SERVER_SOURCES := $(wildcard src/server/*.cpp)
SOURCES := $(CLIENT_SOURCES) $(UTILS_SOURCES) $(SERVER_SOURCES)

CLIENT_HEADERS := $(wildcard src/client/*.hpp)
UTILS_HEADERS := $(wildcard src/utils/*.hpp)
SERVER_HEADERS := $(wildcard src/server/*.hpp)
HEADERS := $(CLIENT_HEADERS) $(UTILS_HEADERS) $(SERVER_HEADERS)

CLIENT_OBJECTS := $(CLIENT_SOURCES:.cpp=.o)
UTILS_OBJECTS := $(UTILS_SOURCES:.cpp=.o)
SERVER_OBJECTS := $(SERVER_SOURCES:.cpp=.o)
OBJECTS := $(CLIENT_OBJECTS) $(UTILS_OBJECTS) $(SERVER_OBJECTS)

CXXFLAGS = -std=c++17
LDFLAGS = -std=c++17

CXXFLAGS += $(INCLUDES)
LDFLAGS += $(INCLUDES)


#LDFLAGS = -fsanitize=address -lasan

CXXFLAGS += -fdiagnostics-color=always
CXXFLAGS += -Wall
#CXXFLAGS += -Werror
CXXFLAGS += -Wextra
CXXFLAGS += -Wcast-align
CXXFLAGS += -Wconversion
CXXFLAGS += -Wfloat-equal
CXXFLAGS += -Wformat=2
CXXFLAGS += -Wnull-dereference
CXXFLAGS += -Wshadow
CXXFLAGS += -Wsign-conversion
CXXFLAGS += -Wswitch-default
CXXFLAGS += -Wswitch-enum
CXXFLAGS += -Wundef
CXXFLAGS += -Wunreachable-code
CXXFLAGS += -Wunused
LDFLAGS += -pthread


.PHONY: all clean fmt fmt-check package

all: $(TARGET_EXECS)

fmt: $(SOURCES) $(HEADERS)
	clang-format -i $^

fmt-check: $(SOURCES) $(HEADERS)
	clang-format -n --Werror $^

src/server/server: $(SERVER_OBJECTS) $(SERVER_HEADERS) $(UTILS_OBJECTS) $(UTILS_HEADERS)
src/client/user: $(CLIENT_OBJECTS) $(CLIENT_HEADERS) $(UTILS_OBJECTS) $(UTILS_HEADERS)

AS: src/server/server
	cp src/server/server AS
user: src/client/user
	cp src/client/user user

clean:
	rm -f $(OBJECTS) $(TARGETS) $(TARGET_EXECS) project.zip

clean-data:
	rm -rf .data

package:
	cp README.md readme.txt
	zip project.zip $(SOURCES) $(HEADERS) Makefile .clang-format readme.txt *.xlsx
