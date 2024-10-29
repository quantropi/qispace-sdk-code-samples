# Required building Platform: 
#        - Ubuntu or RaspberryPI: gcc 
#        - MacOS:  clang
# To build sequr_util library please install libraries cjson and curl 
#     - Ubuntu 22.04 and RaspberryPI:  sudo apt-get install libcjson-dev libcurl4-openssl-dev
#	  - Ubuntu 24.04: sudo apt-get install libcjson-dev libcurl4-gnutls-dev
#     - MacOS: 	brew install curl cjson

ROOT_DIR?= ../..

include make.auto_platform

QEEPLIB_DIR ?=$(ROOT_DIR)/libqeep
QEEPLIB ?= $(QEEPLIB_DIR)/$(LIB_PREFIX)qeep-$(LIBNAME_SUFFIX).$(LIB_EXT)


ifeq ("$(wildcard ../../libqeep)","")
    $(error QiSpace QEEP SDK not found, please contact Quantropi to obtain!!!)
endif

CFLAGS +=  $(INCLUDE_FLAG)$(QEEPLIB_DIR)

# Libraries
LDFLAGS += -lcjson -lcurl

# Sources and object files
UTIL_OBJS = help_util.$(OBJ_EXT)
UTIL_OBJS += json_util.$(OBJ_EXT)
UTIL_OBJS += sequr_util.$(OBJ_EXT)

SEQUR_UTIL_LIB = $(LIB_PREFIX)sequr_util-$(LIBNAME_SUFFIX).$(LIB_EXT)

all: $(SEQUR_UTIL_LIB) demo_sequr_key_gen demo_sequr_key_query demo_sequr_qe_gen demo_qeep demo_pqrnd

ifeq ($(PLATFORM_AUTODETECT),osx)
LDFLAGS +="-L/usr/local/opt/curl/lib"
CFLAGS +="-I/usr/local/opt/curl/include"
endif

# Create static library for sequr_util
$(SEQUR_UTIL_LIB): $(UTIL_OBJS)
	@$(AR) $(ARFLAGS) $@ $^

# Compile c code into object file
%.$(OBJ_EXT): ./%.c 
	$(CC) $(CC_FLAG) $(CFLAGS) $(DEFINITIONS) $(OBJ_FLAG)$@ $<

demo_sequr_key_gen: $(UTIL_OBJS) demo_sequr_key_gen.$(OBJ_EXT) 
	$(LD) $(EXEOUT_FLAG)$@$(EXE_EXT) $(LINK_GROUP_BEGIN) $^ $(QEEPLIB) $(SEQUR_UTIL_LIB) $(LINK_GROUP_END) $(LDFLAGS)  

demo_sequr_key_query: $(UTIL_OBJS) demo_sequr_key_query.$(OBJ_EXT) 
	$(LD) $(EXEOUT_FLAG)$@$(EXE_EXT) $(LINK_GROUP_BEGIN) $^ $(QEEPLIB) $(SEQUR_UTIL_LIB) $(LINK_GROUP_END) $(LDFLAGS)  

demo_sequr_qe_gen: demo_sequr_qe_gen.$(OBJ_EXT) 
	$(LD) $(EXEOUT_FLAG)$@$(EXE_EXT) $(LINK_GROUP_BEGIN) $^ $(QEEPLIB) $(SEQUR_UTIL_LIB) $(LINK_GROUP_END) $(LDFLAGS)  

demo_qeep: demo_qeep.$(OBJ_EXT) 
	$(LD) $(EXEOUT_FLAG)$@$(EXE_EXT) $(LINK_GROUP_BEGIN) $^ $(QEEPLIB) $(SEQUR_UTIL_LIB) $(LINK_GROUP_END) $(LDFLAGS)  

demo_pqrnd: demo_pqrnd.$(OBJ_EXT) 
	$(LD) $(EXEOUT_FLAG)$@$(EXE_EXT) $(LINK_GROUP_BEGIN) $^ $(QEEPLIB) $(SEQUR_UTIL_LIB) $(LINK_GROUP_END) $(LDFLAGS)  

clean:
	rm -f $(SEQUR_UTIL_LIB) *.$(OBJ_EXT) demo_sequr_key_gen$(EXE_EXT) demo_sequr_key_query$(EXE_EXT) \
	demo_qeep$(EXE_EXT) demo_sequr_qe_gen$(EXE_EXT) demo_pqrnd$(EXE_EXT) 