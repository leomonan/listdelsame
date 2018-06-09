#####################################################################  
## file        : test makefile for build current dir .c            ##  
#####################################################################  
  
CC      = gcc  
CPP     = g++  
RM      = rm -rf  
  
## debug flag  
DBG_ENABLE   = 1  
  
## source file path  
SRC_PATH   := ./  
  
## target exec file name  
TARGET     := task  
  
## get all source files  
//SRCS         += $(wildcard $(SRC_PATH)*.c)  
SRCS_CPP         += $(wildcard *.cpp)
SRCS_C         += $(wildcard *.c)

$(warning SRCS_C = $(SRCS_C))  
$(warning SRCS_CPP = $(SRCS_CPP))

## all .o based on all .c  
OBJS_C        := $(SRCS_C:.c=.o)  
OBJS_CPP        := $(SRCS_CPP:.cpp=.o)

$(warning OBJS_C = $(OBJS_C))  
$(warning OBJS_CPP = $(OBJS_CPP))
## need libs, add at here  
LIBS :=  marking
  
## used headers  file path  
INCLUDE_PATH := .  
  
## used include librarys file path  
LIBRARY_PATH := ./lib  

#	CFLAGS := -fvisibility=hidden -O2 
 
## debug for debug info, when use gdb to debug  
#ifeq (1, ${DBG_ENABLE})   
    CFLAGS += -D_DEBUG -O0 -ggdb -DDEBUG_ON=1 -v
#endif  
  
## get all include path  
CFLAGS  += $(foreach dir, $(INCLUDE_PATH), -I$(dir))  
  
## get all library path  
LDFLAGS += $(foreach lib, $(LIBRARY_PATH), -L$(lib))  
  
## get all librarys  
LDFLAGS += $(foreach lib, $(LIBS), -l$(lib))  
LDFLAGS +=  -lstdc++
  
all: clean build  
  
build: 
ifneq (, $(SRCS_C))
	$(CC) -c $(CFLAGS) $(SRCS_C)  
endif
ifneq (, $(SRCS_CPP))
	$(CPP) -c $(CFLAGS) $(SRCS_CPP)  
endif
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS_C) $(OBJS_CPP) $(LDFLAGS)  
#	$(RM) $(OBJS_C) $(OBJS_CPP) -v 
  
clean:  
	$(RM) $(OBJS_C) $(OBJS_CPP) $(TARGET) -v
