CMN_CPP_SRCS = \
			$(wildcard app/feature2/*.cpp) \

CMN_SRCS=   \
			$(wildcard app/feature1/*.c)  \
            $(wildcard app/sys/*.c)  \


FEATURE3_SRCS = \
			$(wildcard app/feature3/*.c) \

TEST_H = \
       $(wildcard app/feature2/unitTest/*.h) \

CUNIT_TESTS = \
       $(wildcard app/feature1/unitTest/*.c) \
 

ifeq ($(PRODUCT),)
	PRODUCT=product
endif


CMN_CFLAGS+=  \
			 -Iapp/include \
			 -rdynamic -Wall 
			 #-Werror

CMN_CPPFLAGS+=  \
			 -Iapp/include \
			 -rdynamic -Wall 
			 #-Werror

#此处可加入版本控制宏定义
#ifeq ($(LOOTOM_DEV_VER),"GDOLT2000s-08")
#CMN_CFLAGS+=-DGDOLT2000_08
#endif

#ifeq ($(LOOTOM_DEV_VER),"GDOLT2000s-16")
#CMN_CFLAGS+=-DGDOLT2000_16
#endif

#ifeq ($(LOOTOM_DEV_VER),"GDOLT2000-500-10G")
#CMN_CFLAGS+=-DGDOLT2000_500_10G
#endif
			 
CMN_LDFLAGS=
