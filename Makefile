ROOT=$(shell pwd)
TOOL_ROOT=$(ROOT)/build

ifndef VERSION_MAJOR
VERSION_MAJOR			:= 1
endif
ifndef VERSION_MINOR
VERSION_MINOR			:= 0
endif
ifndef VERSION_REV
VERSION_REV             := 1
endif
ifndef VERSION_BUILD
VERSION_BUILD           := 0
endif
BUILD_STRING    :=      $(shell git log -n 1 --pretty=format:"%h")

VERSION_STRING			:= v${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_REV}.${VERSION_BUILD}:"(${BUILD_STRING})"

TARGET_FILES=$(filter-out $(TOOL_ROOT)/cmn.mk, $(shell ls $(TOOL_ROOT)/*.mk))

TARGETS=$(notdir $(basename $(TARGET_FILES)))

CLEAN_TARGETS=$(addsuffix .clean, $(TARGETS))
RELEASE_TARGETS=$(addsuffix .release, $(TARGETS))
AT=@

OPTS=ROOT=$(ROOT) TOOL_ROOT=$(TOOL_ROOT) OUTPUT=$(OUTPUT) VERSION_STRING=$(VERSION_STRING)
ifneq ($(CROSS_COMPILE),)
OPTS+=CROSS_COMPILE=$(CROSS_COMPILE)
endif 
all:$(TARGETS)

$(TARGETS): 
	$(AT)printf  "\t\t   +---------------------------+\n"
	$(AT)printf  "\t\t  /                            /|\n"
	$(AT)printf  "\t\t /    Application Build       / |\n"
	$(AT)printf  "\t\t/----------------------------/  |\n"
	$(AT)printf  "\t\t|                           |   |\n"
	$(AT)printf  "\t\t|      Building...          |  /\n"
	$(AT)printf  "\t\t|     %-22s| /\n" $@
	$(AT)printf  "\t\t|                           |/\n"
	$(AT)printf  "\t\t+---------------------------+\n"
	$(AT) make -s -f $(TOOL_ROOT)/subsys.makefile $(OPTS) TARGET=$@
	$(AT) make -s -f tools/Makefile $(OPTS) TARGET=$@
    
$(CLEAN_TARGETS):
	$(AT) make -s -f $(TOOL_ROOT)/subsys.makefile $(OPTS) TARGET=$(basename $@) clean

clean:$(CLEAN_TARGETS)
    
