<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" encoding="utf-8" doctype-public="-//OASIS//DTD DITA Topic//EN" doctype-system="dtd/topic.dtd"/>
	<!-- start dita topic templates -->
	<xsl:param name="lang">cn</xsl:param>
	
	<xsl:template match="/">
		<xsl:apply-templates/>
	</xsl:template>
	<!-- error code object to dita topic -->
	<xsl:template match="nodes">
		<topic xmlns:ditaarch="http://dita.oasis-open.org/architecture/2005/" id="tecs_error_code.xml">
			<xsl:if test="$lang='cn'">
				<xsl:attribute name="xml:lang">zh_cn</xsl:attribute>
			</xsl:if>
			<xsl:if test="$lang='en'">
				<xsl:attribute name="xml:lang">en_us</xsl:attribute>
			</xsl:if>
			<title>
				<xsl:if test="$lang='cn'">tecs错误返回码列表</xsl:if>
				<xsl:if test="$lang='en'">tecs error code list</xsl:if>
			</title>
			<shortdesc>
				<xsl:if test="$lang='cn'">
					tecs系统所有错误返回码列表。
				</xsl:if>
				<xsl:if test="$lang='en'">
					list of tecs return error code.
				</xsl:if>
			</shortdesc>
			<body>
				<section id="description">
					<title>
						<xsl:if test="$lang='cn'">错误码说明</xsl:if>
						<xsl:if test="$lang='en'">Error code definition</xsl:if>
					</title>
					<xsl:if test="$lang='cn'">
						<p>tecs系统所有错误返回码及含义如下表所示, 当tecs xmlrpc接口返回失败（错误码不为0）时，除了错误码外，通常还会返回附加的错误信息，如错误发生位置等，结合错误码和错误附加信息，可以获得详细的错误描述。</p>
					</xsl:if>
					<xsl:if test="$lang='en'">
						<p>All error code of tecs system is shown in following table. When error occurs in a tecs rpc call, tecs return a none-zero error code with an additional error string to indicate error location in details, which can give an exactly error descreption of last rpc call.</p>
					</xsl:if>
				</section>
				<section>
					<simpletable id="error_code_define" relcolwidth="25* 75*">
						<sthead>
							<xsl:if test="$lang='cn'">
								<stentry>错误码取值</stentry>
								<stentry>错误说明</stentry>
							</xsl:if>
							<xsl:if test="$lang='en'">
								<stentry>error code value</stentry>
								<stentry>comment</stentry>
							</xsl:if>
						</sthead>
						<xsl:apply-templates select="node/Maps/Map/MapItem"/>
					</simpletable>
				</section>
			</body>
		</topic>
	</xsl:template>
	<xsl:template match="node/Maps/Map/MapItem">
		<strow>
			<stentry>
				<xsl:value-of select="@Value"/>
			</stentry>
			<stentry>
				<xsl:if test="$lang='cn'">
					<xsl:value-of select="@Name-chs"/>
				</xsl:if>
				<xsl:if test="$lang='en'">
					<xsl:value-of select="@Name-en"/>
				</xsl:if>
			</stentry>
		</strow>
	</xsl:template>
</xsl:stylesheet>
