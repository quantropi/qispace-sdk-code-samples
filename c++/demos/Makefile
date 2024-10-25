ROOT_DIR?= ../..

include make.auto_platform

ifeq ("$(wildcard ../../libmasq)","")
    $(error MASQ SDK not found, please contact Quantropi to obtain!!!)
endif

all: masq_kem masq_ds

masq_kem:
	make -f make.kem 
	make -f make.kem clean 

masq_ds: 
	make -f make.ds 
	make -f make.ds clean
	 
clean:
	make -f make.kem clean 
	make -f make.ds clean 