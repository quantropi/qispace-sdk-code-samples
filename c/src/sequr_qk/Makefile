# To build demo_sequr_key_gen and demo_sequr_key_query on Linux successful, please follow below steps
# 1. install libraries cjson and curl: sudo apt-get install libcjson-dev libcurl4-openssl-dev
# 2. copy & paste the `lib` folder from QEEP SDK into the current directory
# 3. run: make demo_sequr_key_gen & make demo_sequr_key_query

QEEPLIB_DIR ?=./lib
QEEPLIB ?= $(QEEPLIB_DIR)/libqeep-gcc-x64.a

CC = gcc
CFLAGS = -Wall -I/usr/include/cjson -I/usr/include/curl -I$(QEEPLIB_DIR)

# Libraries
LDFLAGS = -lcjson -lcurl

# Sources and object files
SEQUR_OBJ = sequr_util.o 
SEQUR_LIB = libsequr_util.a
HELP_OBJ = help_util.o
JSON_OBJ = json_util.o

SRCS = help_util.c  json_util.c  demo_sequr_key_gen.c  demo_sequr_key_query.c  sequr_util.c
OBJS:= ${SRCS:.c=.o}

all: $(OBJS) demo_sequr_key_gen demo_sequr_key_query

# Create static library for sequr_util
$(SEQUR_LIB): $(SEQUR_OBJ)
	@ar rcs $@ $^

# Compile c code into object file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

demo_sequr_key_gen: demo_sequr_key_gen.o $(SEQUR_LIB) $(HELP_OBJ) $(JSON_OBJ)
	$(CC) demo_sequr_key_gen.o $(SEQUR_OBJ) $(HELP_OBJ) $(JSON_OBJ) -o $@ -L$(QEEPLIB_DIR) -L. -lsequr_util $(LDFLAGS) $(QEEPLIB)

demo_sequr_key_query: demo_sequr_key_query.o $(SEQUR_LIB) $(HELP_OBJ) $(JSON_OBJ)
	$(CC) demo_sequr_key_query.o $(SEQUR_OBJ) $(HELP_OBJ) $(JSON_OBJ) -o $@ -L$(QEEPLIB_DIR) -L. -lsequr_util $(LDFLAGS) $(QEEPLIB)
	
clean:
	rm -f *.a *.o demo_sequr_key_gen demo_sequr_key_query 