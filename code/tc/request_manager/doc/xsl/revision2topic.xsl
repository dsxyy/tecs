<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" encoding="utf-8" doctype-public="-//OASIS//DTD DITA Topic//EN" doctype-system="dtd/topic.dtd"/>
	<!-- start dita topic templates -->
	<xsl:param name="lang">cn</xsl:param>
	<xsl:template match="/">
		<xsl:apply-templates/>
	</xsl:template>
	<!-- error code object to dita topic -->
	<xsl:template match="revisions">
		<topic xmlns:ditaarch="http://dita.oasis-open.org/architecture/2005/" id="revision_history.xml">
			<xsl:if test="$lang='cn'">
				<xsl:attribute name="xml:lang">zh_cn</xsl:attribute>
			</xsl:if>
			<xsl:if test="$lang='en'">
				<xsl:attribute name="xml:lang">en_us</xsl:attribute>
			</xsl:if>
			<title>
				<xsl:if test="$lang='cn'">修订记录</xsl:if>
				<xsl:if test="$lang='en'">Revision History</xsl:if>
			</title>
			<shortdesc>
				<xsl:if test="$lang='cn'">文档修订历史记录。</xsl:if>
				<xsl:if test="$lang='en'">Revision history of this document .</xsl:if>
			</shortdesc>
			<body>
				<xsl:apply-templates select="revision"/>
			</body>
		</topic>
	</xsl:template>
	<xsl:template match="revision">
		<section>
			<title>
				<xsl:value-of select="@version"/>
			</title>
			<simpletable relcolwidth="18* 32* 50*">
				<sthead>
					<xsl:if test="$lang='cn'">
						<stentry>修改日期</stentry>
						<stentry>修订位置</stentry>
						<stentry>修订说明</stentry>
					</xsl:if>
					<xsl:if test="$lang='en'">
						<stentry>Date</stentry>
						<stentry>Location</stentry>
						<stentry>Comment</stentry>
					</xsl:if>
				</sthead>
				<xsl:apply-templates select="entry"/>
			</simpletable>
		</section>
	</xsl:template>
	<xsl:template match="entry">
		<strow>
			<stentry>
				<xsl:value-of select="date"/>
			</stentry>
			<stentry>
				<xsl:if test="location/@link=''">
					<xsl:value-of select="location"/>
				</xsl:if>
				<xsl:if test="location/@link!=''">
					<xref href="{location/@link}">
						<xsl:value-of select="location"/>
					</xref>
				</xsl:if>
			</stentry>
			<stentry>
				<xsl:if test="$lang='cn'">
					<xsl:value-of select="desc_cn"/>
				</xsl:if>
				<xsl:if test="$lang='en'">
					<xsl:value-of select="desc_en"/>
				</xsl:if>
			</stentry>
		</strow>
	</xsl:template>
	<xsl:template name="replaceFunc">
		<xsl:param name="text"/>
		<xsl:param name="replace"/>
		<xsl:param name="by"/>
		<xsl:choose>
			<xsl:when test="contains($text,$replace)">
				<xsl:value-of select="substring-before($text,$replace)"/>
				<xsl:value-of select="$by"/>
				<xsl:call-template name="replaceFunc">
					<xsl:with-param name="text" select="substring-after($text,$replace)"/>
					<xsl:with-param name="replace" select="$replace"/>
					<xsl:with-param name="by" select="$by"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$text"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
</xsl:stylesheet>
