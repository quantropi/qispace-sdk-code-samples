include make.auto_platform

QISPACE_API_ON?=0

ifeq ("$(wildcard ./libqeep)","")
    $(error QiSpace QEEP SDK not found, please contact Quantropi to obtain!!!)
endif

ifeq ("$(wildcard ./libmasq)","")
    $(error QiSpace MASQ SDK not found, please contact Quantropi to obtain!!!)
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

all: sequr pqrnd qeep masq_kem masq_ds

install_depends:
# Install curl
ifeq ($(PLATFORM_AUTODETECT),linux)
	sudo apt-get install libcurl4-openssl-dev
# Verify install
	sudo find /usr -name 'curl.h'
endif
ifeq ($(PLATFORM_AUTODETECT),osx)
	brew install curl
endif

# Install json
	git clone https://github.com/nlohmann/json.git

%.$(OBJ_EXT): ./src/%.cpp
	$(CPP) -std=c++11 $(CC_FLAG) $(CFLAGS) $(DEFINITIONS) $(OBJ_FLAG)$@ $<

depends: $(UTIL_OBJS)

sequr: depends
ifeq ($(QISPACE_API_ON),1)
	make -f make.sequr QISPACE_API_ON=$(QISPACE_API_ON)
else
	@echo "Contact Quantropi to obtain QiSpace SEQUR SDK."
endif

pqrnd: depends
	make -f make.pqrnd QISPACE_API_ON=$(QISPACE_API_ON)

qeep: depends
	make -f make.qeep QISPACE_API_ON=$(QISPACE_API_ON)

masq_kem:
	make -f make.kem

masq_ds: 
	make -f make.ds 

clean:
	make -f make.qeep clean
	make -f make.sequr clean
	make -f make.pqrnd clean
	make -f make.kem clean
	make -f make.ds clean

