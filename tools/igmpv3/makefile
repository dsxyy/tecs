########################################################################
###
###     This file is ushell module make entry. 
###     Author:  liuxuefeng, CSP Soft1
###     History:
###     1. liuxuefeng,  2012/03/28,   创建
###
########################################################################

#CFLAGS=-O -Wall
#CFLAGS=-O -DMAX_WBITS=14 -DMAX_MEM_LEVEL=7
#CFLAGS=-g -DDEBUG
#CFLAGS=-O3 -Wall -Wwrite-strings -Wpointer-arith -Wconversion \
#           -Wstrict-prototypes -Wmissing-prototypes

include $(_TECS_MAK_PATH)/compileoption.mak


_IGMPV3_PATH = $(_TECS_TOOLS_PATH)/igmpv3
_IGMPV3_INC_PATH = $(_IGMPV3_PATH)/include
_IGMPV3_SRC_PATH = $(_IGMPV3_PATH)/src

_IGMPV3_TMP_OBJ_PATH = $(_TEMP_OBJ_PATH)/igmpv3
_IGMPV3_TMP_DEP_PATH = $(_TEMP_DEP_PATH)/igmpv3

#_IGMPV3_OUTBIN_PATH是在TECS目录下生产一个bin目录，将生成的deflate 再拷贝到这个目录
_IGMPV3_TGT_PATH = $(_TARGET_BIN_BASE_PATH)/tools

_IGMPV3_ALL_C = $(wildcard $(_IGMPV3_SRC_PATH)/*.c)
_IGMPV3_ALL_O = $(addprefix $(_IGMPV3_TMP_OBJ_PATH)/,$(subst .c,.o,$(notdir $(_IGMPV3_ALL_C))))
_IGMPV3_ALL_D = $(addprefix $(_IGMPV3_TMP_DEP_PATH)/,$(subst .c,.d,$(notdir $(_IGMPV3_ALL_C))))

_IGMPV3_EXE_NAME = $(_IGMPV3_TGT_PATH)/igmpv3

IGMPV3CFLAGS = $(addprefix -I,$(_IGMPV3_INC_PATH))

export _IGMPV3_EXE_NAME = $(_IGMPV3_TGT_PATH)/IGMPV3

CPPFLAGS += $(IGMPV3CFLAGS) 
DEPCPPFLAGS += $(IGMPV3CFLAGS)
#依赖的GLIBC库
GLIBC_FOR_USHELL = -lrt -lpthread -D_GNU_SOURCE

$(_IGMPV3_TMP_DEP_PATH)/%.d:$(_IGMPV3_SRC_PATH)/%.c
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_IGMPV3_TMP_DEP_PATH) ]; then $(MKDIR) $(_IGMPV3_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_IGMPV3_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_IGMPV3_TMP_OBJ_PATH)/%.o:$(_IGMPV3_SRC_PATH)/%.c
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_IGMPV3_TMP_OBJ_PATH) ]; then $(MKDIR) $(_IGMPV3_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@


default:createdir $(_IGMPV3_ALL_O)
	$(CC) -o $(_IGMPV3_EXE_NAME) $(_IGMPV3_ALL_O)  $(GLIBC_FOR_USHELL)
	$(ECHO) Successfully make IGMPV3: $(_IGMPV3_TGT_PATH)/IGMPV3

createdir:
	@if [ ! -d $(_IGMPV3_TGT_PATH) ]; then $(MKDIR) $(_IGMPV3_TGT_PATH); fi
	@if [ ! -d $(_IGMPV3_TMP_DEP_PATH) ]; then $(MKDIR) $(_IGMPV3_TMP_DEP_PATH); fi
	@if [ ! -d $(_IGMPV3_TMP_OBJ_PATH) ]; then $(MKDIR) $(_IGMPV3_TMP_OBJ_PATH); fi

clean:
	$(RM) $(_IGMPV3_TMP_DEP_PATH)
	$(RM) $(_IGMPV3_TMP_OBJ_PATH)
	$(RM) $(_IGMPV3_TGT_PATH)/IGMPV3
	$(ECHO) Successfully cleaned!
	
ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(_IGMPV3_ALL_D)
endif
