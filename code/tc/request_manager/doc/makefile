# DITAMAP Makefile
# Ma Zhaomian 2011.11

ifeq (,$(_VERSION))
export _VERSION     = V01.01.10
endif

export DOC_TARGET_PATH = $(_TECS_TARGET_PATH)/doc/xmlrpc

METHOD_XSL_FILE=xsl/method2topic.xsl

ERROR_XSL_FILE=xsl/maps2topic.xsl
GUI_PATH=../../../../client/cli/scripts
GUI_XML_FILE=$(GUI_PATH)/gui.xml

DITAMAP_XSL_FILE=xsl/docs2ditamap.xsl
DITAMAP_XML_FILE=$(DOC_TARGET_PATH)/tecs.files
DOCLIST=./docs2file.sh

REVISION_HISTORY=revision_history.xml
REVISION_XSL_FILE=xsl/revision2topic.xsl

ECHO=echo
XSLTPROC=xsltproc
BUILD=dita-ant.sh
LANG_CN=--param lang '"cn"'
LANG_EN=--param lang '"en"'
VERSION=--param version '"$(_VERSION)"'

.PHONY:all method errorcode revision ditamap filelist build clean

all:  method errorcode revision ditamap build

prepare:
	@if [ ! -d $(DOC_TARGET_PATH) ]; then $(MKDIR) $(DOC_TARGET_PATH); fi

method: prepare
	@for i in `dir -1 tecs_*.xml`; do $(ECHO) generate $$i ... ; $(XSLTPROC) $(LANG_CN) -o $(DOC_TARGET_PATH)/cn/$$i $(METHOD_XSL_FILE) $$i; $(XSLTPROC) $(LANG_EN) -o $(DOC_TARGET_PATH)/en/$$i $(METHOD_XSL_FILE) $$i; done

errorcode: prepare
	@$(ECHO) generate tecs_error_code.xml ...
	@$(XSLTPROC) $(LANG_CN) -o $(DOC_TARGET_PATH)/cn/tecs_error_code.xml $(ERROR_XSL_FILE) $(GUI_XML_FILE)
	@$(XSLTPROC) $(LANG_EN) -o $(DOC_TARGET_PATH)/en/tecs_error_code.xml $(ERROR_XSL_FILE) $(GUI_XML_FILE)

revision: prepare
	@$(ECHO) generate $(REVISION_HISTORY) ...
	@$(XSLTPROC) $(LANG_CN) -o $(DOC_TARGET_PATH)/cn/$(REVISION_HISTORY) $(REVISION_XSL_FILE) $(REVISION_HISTORY)
	@$(XSLTPROC) $(LANG_EN) -o $(DOC_TARGET_PATH)/en/$(REVISION_HISTORY) $(REVISION_XSL_FILE) $(REVISION_HISTORY)
	
filelist:
	@echo '<?xml version="'1.0'" encoding="'utf-8'"?>' >$(DITAMAP_XML_FILE)
	@echo '<xmlfiles>' >>$(DITAMAP_XML_FILE)
	@for i in `dir -1 tecs_*.xml`; do echo '  <xmlfile name="'$$i'"/>' >>$(DITAMAP_XML_FILE); done
	@echo '</xmlfiles>' >>$(DITAMAP_XML_FILE)

ditamap: filelist
	@$(ECHO) generate tecs ditamaps ...
	@$(XSLTPROC) $(LANG_CN) $(VERSION) -o $(DOC_TARGET_PATH)/tecs_cn.ditamap $(DITAMAP_XSL_FILE) $(DITAMAP_XML_FILE)
	@$(XSLTPROC) $(LANG_EN) $(VERSION) -o $(DOC_TARGET_PATH)/tecs_en.ditamap $(DITAMAP_XSL_FILE) $(DITAMAP_XML_FILE)
	@rm -f $(DITAMAP_XML_FILE)
	
build:
	@$(ECHO) build document of zh_cn ...
	@export DOC_TARGET_PATH=$(DOC_TARGET_PATH); $(BUILD) -f build/build_cn.xml >$(DOC_TARGET_PATH)/build_cn.log
	@$(ECHO) build document of en_us ...
	@$(BUILD) -f build/build_en.xml >$(DOC_TARGET_PATH)/build_en.log

clean:
	@$(ECHO) clean all intermediate files...
	@rm -rf $(DOC_TARGET_PATH)
	@$(ECHO) done!
