include $(TOOL_ROOT)/$(TARGET).mk
OBJ_DIR=$(OUTPUT)/$(TARGET)
ifeq ($(OUTPUT),)
OBJ_DIR=$(ROOT)/work/$(TARGET)
endif

AT=@

STATIC_OBJ_DIR=$(OBJ_DIR)/static
SHARE_OBJ_DIR=$(OBJ_DIR)/share
PROG_OBJ_DIR=$(OBJ_DIR)/prog
FEATURE3_OBJ_DIR=$(OBJ_DIR)/prog/feature3


STATIC_LIB = $(addprefix $(STATIC_OBJ_DIR)/, $(TARGET_STATIC_LIB))
SHARE_LIB = $(addprefix $(SHARE_OBJ_DIR)/, $(TARGET_SHARE_LIB))
PROG = $(addprefix $(OBJ_DIR)/, $(TARGET_PROG))

STATIC_LIB_OBJS = $(addprefix $(STATIC_OBJ_DIR)/, $(TARGET_STATIC_SRCS:.c=.o))
STATIC_LIB_DEPS = $(addprefix $(STATIC_OBJ_DIR)/, $(TARGET_STATIC_SRCS:.c=.d))

SHARE_LIB_OBJS = $(addprefix $(SHARE_OBJ_DIR)/, $(TARGET_SHARE_SRCS:.c=.o))
SHARE_LIB_DEPS = $(addprefix $(SHARE_OBJ_DIR)/, $(TARGET_SHARE_SRCS:.c=.d))


TEST_OBJ_DIR=$(OBJ_DIR)/cxxtest
TEST_CPPS = $(TEST_H:.h=.cpp)
TEST_OBJS = $(addprefix $(TEST_OBJ_DIR)/, $(TEST_CPPS:.cpp=.o))
TEST_PROG = $(addprefix $(TEST_OBJ_DIR)/, unit_test)
TEST_DEPS = $(TEST_OBJS:.o=.d)
TEST_RELATION_OBJ = $(addprefix $(PROG_OBJ_DIR)/, $(TARGET_PROG_CPP_SRCS:.cpp=.o))
TEST_ALL = $(TEST_PROG) $(TEST_OBJS) $(TEST_CPPS) $(TEST_DEPS)


CUNIT_TEST_OBJ_DIR=$(OBJ_DIR)/cunitTest
CUNIT_TEST_OBJS = $(addprefix $(CUNIT_TEST_OBJ_DIR)/, $(CUNIT_TESTS:.c=.o))
CUNIT_TEST_PROG = $(addprefix $(CUNIT_TEST_OBJ_DIR)/, cunit_test)
CUNIT_TEST_DEPS = $(CUNIT_TEST_OBJS:.o=.d)
CUNIT_TEST_NO_RELATION_SRC =app/sys/main.c
CUNIT_TEST_RELATION_SRC =  $(filter-out $(CUNIT_TEST_NO_RELATION_SRC),$(TARGET_PROG_SRCS))
CUNIT_TEST_RELATION_OBJ = $(addprefix $(PROG_OBJ_DIR)/, $(CUNIT_TEST_RELATION_SRC:.c=.o))
CUNIT_TEST_ALL = CUNIT_TEST_BUILD $(CUNIT_TEST_PROG) $(CUNIT_TEST_OBJS) $(CUNIT_TEST_DEPS)


PROG_OBJS = $(addprefix $(FEATURE3_OBJ_DIR)/, $(FEATURE3_SRCS:.c=.o)) $(addprefix $(PROG_OBJ_DIR)/, $(TARGET_PROG_SRCS:.c=.o))  $(addprefix $(PROG_OBJ_DIR)/, $(TARGET_PROG_CPP_SRCS:.cpp=.o))
PROG_DEPS = $(addprefix $(FEATURE3_OBJ_DIR)/, $(FEATURE3_SRCS:.c=.d)) $(addprefix $(PROG_OBJ_DIR)/, $(TARGET_PROG_SRCS:.c=.d))  $(addprefix $(PROG_OBJ_DIR)/, $(TARGET_PROG_CPP_SRCS:.cpp=.d)) 

TARGET_STATIC_CFLAGS += $(CFLAGS)

TARGET_SHARE_CFLAGS += $(CFLAGS)
TARGET_SHARE_LD_FLAGS += $(LD_FLAGS)

TARGET_PROG_CFLAGS += $(CFLAGS)
TARGET_PROG_CPPFLAGS += $(CPPFLAGS)

FEATURE3_PROG_CFLAGS += $(TARGET_PROG_CFLAGS) -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -Wno-unused-but-set-variable

TARGET_PROG_LD_FLAGS += $(LD_FLAGS)

STATIC_ALL=$(STATIC_LIB) $(STATIC_LIB_OBJS) $(STATIC_LIB_DEPS)
SHARE_ALL=$(SHARE_LIB) $(SHARE_LIB_OBJS) $(SHARE_LIB_DEPS)
PROG_ALL=$(PROG) $(PROG_OBJS) $(PROG_DEPS)
BUILD_MESSAGE_TIMESTAMP ?= __TIME__", "__DATE__

all:$(STATIC_ALL) $(SHARE_ALL) $(PROG_ALL) $(TEST_ALL) $(CUNIT_TEST_ALL)

$(STATIC_OBJ_DIR)/%.o:%.c
	$(AT) echo [Compile $(notdir $(CC)) $<]
	$(AT) mkdir -p $(dir $@)
	$(AT) $(CC) $(TARGET_STATIC_CFLAGS) -c -o $@ $<

$(STATIC_OBJ_DIR)/%.d:%.c
	@ mkdir -p $(dir $@)
	@ $(CC)  $(TARGET_STATIC_CFLAGS) -MM  -MT '$(basename $@).o'  $^ 1> $@

$(STATIC_LIB):$(STATIC_LIB_OBJS)
	$(AT) echo [Generate  $@]
	$(AT) $(AR)  rcs  $@ $(STATIC_LIB_OBJS)
	$(AT) echo 
	$(AT) echo 

$(SHARE_OBJ_DIR)/%.o:%.c
	$(AT) echo [Compile $(notdir $(CC)) $<]
	$(AT) mkdir -p $(dir $@)
	$(AT) $(CC) $(TARGET_SHARE_CFLAGS) -c -o $@ $<

$(SHARE_OBJ_DIR)/%.d:%.c
	@ mkdir -p $(dir $@)
	@ $(CC)  $(TARGET_SHARE_CFLAGS) -MM  -MT '$(basename $@).o'  $^ 1> $@

$(SHARE_LIB):$(SHARE_LIB_OBJS)
	$(AT) echo "Generate $@"
	$(AT) $(CC) --whole-archive -shared -Wl,-soname,$@ -o $@  -o $@ $(SHARE_LIB_OBJS) $(TARGET_SHARE_LD_FLAGS)
	$(AT) echo 
	$(AT) echo 

$(PROG_OBJ_DIR)/%.o:%.cpp
	$(AT) echo [Compile $(notdir $(CXX)) $<]
	$(AT) mkdir -p $(dir $@)
	$(AT) $(CXX) $(TARGET_PROG_CPPFLAGS) -c -o $@ $<

$(PROG_OBJ_DIR)/%.d:%.cpp
	@ mkdir -p $(dir $@)
	@ $(CXX)  $(TARGET_PROG_CPPFLAGS) -MM  -MT '$(basename $@).o'  $^ 1> $@
	
$(PROG_OBJ_DIR)/%.o:%.c
	$(AT) echo [Compile $(notdir $(CC)) $<]
	$(AT) mkdir -p $(dir $@)
	$(AT) $(CC) $(TARGET_PROG_CFLAGS) -c -o $@ $<

$(PROG_OBJ_DIR)/%.d:%.c
	@ mkdir -p $(dir $@)
	@ $(CC)  $(TARGET_PROG_CFLAGS) -MM  -MT '$(basename $@).o'  $^ 1> $@

$(FEATURE3_OBJ_DIR)/%.o:%.c
	$(AT) echo [Compile $(notdir $(CC)) $<]
	$(AT) mkdir -p $(dir $@)
	$(AT) $(CC) $(FEATURE3_PROG_CFLAGS) -c -o $@ $<

$(FEATURE3_OBJ_DIR)/%.d:%.c
	@ mkdir -p $(dir $@)
	@ $(CC)  $(FEATURE3_PROG_CFLAGS) -MM  -MT '$(basename $@).o'  $^ 1> $@

$(PROG):$(PROG_OBJS)
	$(AT) echo "[Generate build infomation]"
	$(AT) echo 'const char build_message[]  = $(BUILD_MESSAGE_TIMESTAMP);  \
		const char version_string[] = "${VERSION_STRING}";' > $(OBJ_DIR)/build_message.c
	$(AT) $(CC) $(TARGET_PROG_CFLAGS) -c -o $(OBJ_DIR)/build_message.o $(OBJ_DIR)/build_message.c
	$(AT) echo [Linking  $@]
	$(AT) $(CXX) -o $@ $(OBJ_DIR)/build_message.o $(PROG_OBJS) $(TARGET_PROG_LD_FLAGS)
	$(AT) echo 
	$(AT) echo 

$(TEST_CPPS):%.cpp:%.h
	$(AT) tools/cxxtest-4.3/bin/cxxtestgen --part --error-printer -o $@ $<

$(TEST_OBJ_DIR)/%.d:%.cpp
	$(AT) mkdir -p $(dir $@)
	$(AT) $(CXX)  $(TARGET_PROG_CPPFLAGS) -MM  -MT '$(basename $@).o'  $^ 1> $@

$(TEST_OBJ_DIR)/%.o:%.cpp
	$(AT) echo [Compile $(notdir $(CXX)) $<]
	$(AT) mkdir -p $(dir $@)
	$(AT) $(CXX) $(TARGET_PROG_CPPFLAGS) -I./tools/cxxtest-4.3 -c -o $@ $<

$(TEST_PROG):$(TEST_OBJS)
	$(AT) echo "[unit test build infomation]"
	$(AT) tools/cxxtest-4.3/bin/cxxtestgen --root --error-printer -o $(TEST_OBJ_DIR)/unit_test.cpp
	$(AT) $(CXX) -I./tools/cxxtest-4.3 $(TARGET_PROG_CPPFLAGS) -c -o $(TEST_OBJ_DIR)/unit_test.o $(TEST_OBJ_DIR)/unit_test.cpp
	$(AT) echo [Linking  $@]
	$(AT) $(CXX) -o $@ $(TEST_OBJ_DIR)/unit_test.o $(TEST_OBJS) $(TEST_RELATION_OBJ) $(TARGET_PROG_LD_FLAGS)
	$(AT) echo 
	$(AT) echo 

$(CUNIT_TEST_OBJ_DIR)/%.d:%.c
	$(AT) mkdir -p $(dir $@)
	$(AT) $(CC)  $(TARGET_PROG_CFLAGS) -MM  -MT '$(basename $@).o'  $^ 1> $@

$(CUNIT_TEST_OBJ_DIR)/%.o:%.c
	$(AT) echo [Compile $(notdir $(CXX)) $<]
	$(AT) mkdir -p $(dir $@)
	$(AT) $(CC) $(TARGET_PROG_CFLAGS) -I./tools/cxxtest-4.3 -c -o $@ $<

$(CUNIT_TEST_PROG):$(CUNIT_TEST_OBJS)
	$(AT) echo "[Cunit test build infomation]"
	$(AT) $(CC)  $(TARGET_PROG_CFLAGS) -c -o $(CUNIT_TEST_OBJ_DIR)/cunitTest.o tools/cunit/cunitTest.c
	$(AT) $(CC)  $(TARGET_PROG_CFLAGS) -c -o $(CUNIT_TEST_OBJ_DIR)/cunit.o tools/cunit/cunit.c
	$(AT) echo [Linking  $@]
	$(AT) $(CC) -o $@ $(CUNIT_TEST_OBJ_DIR)/cunitTest.o $(CUNIT_TEST_OBJ_DIR)/cunit.o $^ $(CUNIT_TEST_RELATION_OBJ) $(TARGET_PROG_LD_FLAGS)
	$(AT) echo 
	$(AT) echo 

CUNIT_TEST_BUILD:
	python tools/cunit/cunit.py $(CUNIT_TESTS)

-include $(STATIC_LIB_DEPS) $(SHARE_LIB_DEPS) $(PROG_DEPS) $(TEST_DEPS) $(CUNIT_TEST_DEPS)


clean:
	@ rm -rf $(STATIC_OBJ_DIR)  $(SHARE_OBJ_DIR) $(PROG_OBJ_DIR) $(PROG) $(TEST_OBJ_DIR) $(TEST_CPPS) $(CUNIT_TEST_OBJ_DIR) tools/cunit/cunit.c tools/cunit/*.h
	@ rm -rf $(OBJ_DIR)/build_message.o $(OBJ_DIR)/build_message.c

