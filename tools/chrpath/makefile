# Makefile for chrpath
# 版权所有 (C)2008, 深圳市中兴通讯股份有限公司
# For conditions of distribution and use.


#CFLAGS=-O -Wall
#CFLAGS=-O -DMAX_WBITS=14 -DMAX_MEM_LEVEL=7
#CFLAGS=-g -DDEBUG
#CFLAGS=-O3 -Wall -Wwrite-strings -Wpointer-arith -Wconversion \
#           -Wstrict-prototypes -Wmissing-prototypes

include $(_TECS_MAK_PATH)/compileoption.mak


_CHRPATH_PATH = $(_TECS_TOOLS_PATH)/chrpath
_CHRPATH_INC_PATH = $(_CHRPATH_PATH)/include
_CHRPATH_SRC_PATH = $(_CHRPATH_PATH)/src

_CHRPATH_TMP_OBJ_PATH = $(_TEMP_OBJ_PATH)/chrpath
_CHRPATH_TMP_DEP_PATH = $(_TEMP_DEP_PATH)/chrpath

#_CHRPATH_OUTBIN_PATH是在TULIP目录下生产一个bin目录，将生成的deflate 再拷贝到这个目录
_CHRPATH_TGT_PATH = $(_TARGET_BIN_BASE_PATH)/tools

_CHRPATH_ALL_C = $(wildcard $(_CHRPATH_SRC_PATH)/*.c)
_CHRPATH_ALL_O = $(addprefix $(_CHRPATH_TMP_OBJ_PATH)/,$(subst .c,.o,$(notdir $(_CHRPATH_ALL_C))))
_CHRPATH_ALL_D = $(addprefix $(_CHRPATH_TMP_DEP_PATH)/,$(subst .c,.d,$(notdir $(_CHRPATH_ALL_C))))

_CHRPATH_EXE_NAME = $(_CHRPATH_TGT_PATH)/chrpath

CHRPATHCFLAGS = $(addprefix -I,$(_CHRPATH_INC_PATH)) -DHAVE_CONFIG_H

export _CHRPATH_EXE_NAME = $(_CHRPATH_TGT_PATH)/chrpath

CPPFLAGS += $(CHRPATHCFLAGS)
DEPCPPFLAGS += $(CHRPATHCFLAGS)

$(_CHRPATH_TMP_DEP_PATH)/%.d:$(_CHRPATH_SRC_PATH)/%.c
	$(ECHO) generate $(notdir $@)
	@if [ ! -d $(_CHRPATH_TMP_DEP_PATH) ]; then $(MKDIR) $(_CHRPATH_TMP_DEP_PATH); fi
	$(CC) $(DEPCPPFLAGS) -c $< | \
             sed "s/$*\.o[ :]*/$(subst /,\/,$(_CHRPATH_TMP_OBJ_PATH))\/$*.o \: /g" > $@
    
$(_CHRPATH_TMP_OBJ_PATH)/%.o:$(_CHRPATH_SRC_PATH)/%.c
	$(ECHO) compile $(notdir $@)
	@if [ ! -d $(_CHRPATH_TMP_OBJ_PATH) ]; then $(MKDIR) $(_CHRPATH_TMP_OBJ_PATH); fi
	$(ECHO) Making $@ .....
	$(CC) $(CPPFLAGS) -c $< -o $@


default:createdir $(_CHRPATH_ALL_O)
	$(CC) -o $(_CHRPATH_EXE_NAME) $(_CHRPATH_ALL_O)
	$(ECHO) Successfully make chrpath: $(_CHRPATH_TGT_PATH)/chrpath

createdir:
	@if [ ! -d $(_CHRPATH_TGT_PATH) ]; then $(MKDIR) $(_CHRPATH_TGT_PATH); fi
	@if [ ! -d $(_CHRPATH_TMP_DEP_PATH) ]; then $(MKDIR) $(_CHRPATH_TMP_DEP_PATH); fi
	@if [ ! -d $(_CHRPATH_TMP_OBJ_PATH) ]; then $(MKDIR) $(_CHRPATH_TMP_OBJ_PATH); fi

clean:
	$(RM) $(_CHRPATH_TMP_DEP_PATH)
	$(RM) $(_CHRPATH_TMP_OBJ_PATH)
	$(RM) $(_CHRPATH_TGT_PATH)/chrpath
	$(ECHO) Successfully cleaned!
	
ifeq (,$(_TECS_DISABLE_DEPEND))
-include $(_CHRPATH_ALL_D)
endif
