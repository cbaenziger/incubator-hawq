all: gppkg

BLD_TOP=../../../../../
GPMGMT=$(BLD_TOP)/tools
include $(BLD_TOP)/build-utils/pivotal/Makefile.global

# GPPC specific
include ../Makefile.version

pg_config_path := $(BLD_TOP)/src/include/pg_config.h
GP_VERSION_NUM := $(shell grep 'define  *GP_VERSION_NUM' $(pg_config_path) \
	| perl -ne '$$m1=int($$1/10000),$$m2=int(($$1-$$m1*10000)/100) if/^.*?([0-9]+)/;print "$$m1.$$m2$$/"' )

OS=$(word 1,$(subst _, ,$(BLD_ARCH)))
ARCH=$(shell uname -p)

LIBGPPC_DIR=`cd .. && pwd`
LIBGPPC_VER=$(SO_MAJOR_VERSION).$(SO_MINOR_VERSION)
LIBGPPC_REL=1
LIBGPPC_RPM_FLAGS="--define 'libgppc_dir $(LIBGPPC_DIR)' --define 'libgppc_ver $(LIBGPPC_VER)' --define 'libgppc_rel $(LIBGPPC_REL)'"
LIBGPPC_RPM=libgppc-$(LIBGPPC_VER)-$(LIBGPPC_REL).$(ARCH).rpm
LIBGPPC_GPPKG=libgppc-$(LIBGPPC_VER)-$(OS)-$(ARCH).gppkg

TARGET_GPPKG=$(LIBGPPC_GPPKG)
EXTRA_CLEAN+=$(LIBGPPC_RPM) $(LIBGPPC_GPPKG)# gppkg_spec.yml.in

#
# Generic rules to build gppkgs included here
#
include $(BLD_TOP)/build-utils/pivotal/gppkg.mk

gppkg_spec.yml.in: gppkg_spec.yml.source.in
	cat $< | sed 's/#gpver/$(GP_VERSION_NUM)/g' | \
		sed 's/#gppcver/$(SO_MAJOR_VERSION).$(SO_MINOR_VERSION)/g' > $@
	 

gppkg: 
	$(MAKE) $(LIBGPPC_RPM) RPM_FLAGS=$(LIBGPPC_RPM_FLAGS)
	$(MAKE) $(LIBGPPC_GPPKG) MAIN_RPM=$(LIBGPPC_RPM)

.PHONY: gppkg
