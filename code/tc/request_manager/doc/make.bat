@echo off

@set XSLTPROC=xsltproc
@set LANG_CN=--param lang 'cn'
@set LANG_EN=--param lang 'en'

REM DOC_DIR is used by make.bat run in request_manager\doc\
REM DOC_TARGET_PATH is used by build_xx.xml run in request_manager\doc\build\
REM modified by zhang wenjian
@set DOC_DIR=..\..\..\..\target\doc\xmlrpc
@set DOC_TARGET_PATH=..\..\..\..\..\target\doc\xmlrpc
@set DITAMAP_XML_FILE=%DOC_DIR%\tecs.files

@if "%1" =="clean" 			@goto :clean
REM @if "%1" =="docs2file" 		@goto :docs2file
REM @if "%1" =="method2topic" 	@goto :method2topic
REM @if "%1" =="ditamap" 		@goto :ditamap
REM @if "%1" =="build" 			@goto :build

@if not exist %DOC_DIR% @mkdir %DOC_DIR%

@call :docs2file > %DITAMAP_XML_FILE%
@call :method2topic
@call :maps2topic
@call :revision
@call :ditamap
@call :build

@set XSLTPROC=
@set LANG_CN=
@set LANG_EN=
@set DOC_DIR=
@set DOC_TARGET_PATH=
@set DITAMAP_XML_FILE=

@goto :EOF

:docs2file
@echo.^<?xml version="1.0" encoding="UTF-8"?^>
@echo.^<xmlfiles^>
@for  %%i in (tecs_*.xml) do (
    @echo.  ^<xmlfile name="%%i"/^> 
)
@echo.^</xmlfiles^>
@goto :EOF

:method2topic
@set XSL_FILE=xsl/method2topic.xsl
@for %%i in (tecs_*.xml) do (
    @echo generate %%i ...
    @%XSLTPROC% %LANG_CN% -o %DOC_DIR%\cn\%%i %XSL_FILE% %%i
    @%XSLTPROC% %LANG_EN% -o %DOC_DIR%\en\%%i %XSL_FILE% %%i
)
@set XSL_FILE=
@goto :EOF

:maps2topic
@set MAPS_FILE=common_error_code.xml
@set XSL_FILE=xsl/maps2topic.xsl
@set TARGET_FILE=tecs_error_code.xml
@echo generate %TARGET_FILE% ...
@%XSLTPROC% %LANG_CN% -o %DOC_DIR%\cn\%TARGET_FILE% %XSL_FILE% %MAPS_FILE%
@%XSLTPROC% %LANG_EN% -o %DOC_DIR%\en\%TARGET_FILE% %XSL_FILE% %MAPS_FILE%
@set TARGET_FILE=
@set MAPS_FILE=
@set XSL_FILE=
@goto :EOF

:revision
@set XSL_FILE=xsl/revision2topic.xsl
@set REVISION_FILE=revision_history.xml
@echo generate %REVISION_FILE% ...
@%XSLTPROC% %LANG_CN% -o %DOC_DIR%\cn\%REVISION_FILE% %XSL_FILE% %REVISION_FILE%
@%XSLTPROC% %LANG_EN% -o %DOC_DIR%\en\%REVISION_FILE% %XSL_FILE% %REVISION_FILE%
@set REVISION_FILE=
@set XSL_FILE=
@goto :EOF

:ditamap

@echo generate tecs ditamap ...
@set XSL_FILE=xsl/docs2ditamap.xsl
@%XSLTPROC% %LANG_CN% -o %DOC_DIR%\tecs_cn.ditamap %XSL_FILE% %DITAMAP_XML_FILE%
@%XSLTPROC% %LANG_EN% -o %DOC_DIR%\tecs_en.ditamap %XSL_FILE% %DITAMAP_XML_FILE%
@set XSL_FILE=
@goto :EOF

:EOF

:build

@echo build document of zh_cn ...
@call dita-ant -f build\build_cn.xml >%DOC_DIR%\build_cn.log
@echo build document of en_us ...
@call dita-ant -f build\build_en.xml >%DOC_DIR%\build_en.log

@goto :EOF

:clean
@rmdir /q /s %DOC_DIR% 
@goto :EOF

:EOF

