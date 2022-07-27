include make.auto_platform

QISPACE_API_ON?=0

ifeq ("$(wildcard ./lib)","")
    $(error QiSpace SDK not found, please contact Quantropi to obtain!!!)
endif

UTIL_OBJS= base64.$(OBJ_EXT)
ifeq ($(QISPACE_API_ON),1)
ifeq ("$(wildcard ./json/include)","")
    $(error REST API Util depends not found, please install nlohmann/json!!!)
endif
DEFINITIONS += $(DEF_FLAG)QISPACE_API_ON
CFLAGS += $(INCLUDE_FLAG)./json/include 
UTIL_OBJS += RestApiUtil.$(OBJ_EXT)
endif

all: sequr pqrnd qeep

install_depends:
# Install json
git clone https://github.com/nlohmann/json.git

# Install curl
ifeq ($(PLATFORM_AUTODETECT),linux)
	sudo apt-get install libcurl4-openssl-dev
# Verify install
	sudo find /usr -name 'curl.h'
endif
ifeq ($(PLATFORM_AUTODETECT),osx)
	brew install curl
endif

%.$(OBJ_EXT): ./src/%.cpp
	$(CPP) -std=c++11 $(CC_FLAG) $(CFLAGS) $(DEFINITIONS) $(OBJ_FLAG)$@ $<

depends: $(UTIL_OBJS)

sequr: depends
	make -f make.sequr QISPACE_API_ON=$(QISPACE_API_ON)

pqrnd: depends
	make -f make.pqrnd QISPACE_API_ON=$(QISPACE_API_ON)

qeep: depends
	make -f make.qeep QISPACE_API_ON=$(QISPACE_API_ON)


clean:
	make -f make.qeep clean
	make -f make.sequr clean
	make -f make.pqrnd clean
