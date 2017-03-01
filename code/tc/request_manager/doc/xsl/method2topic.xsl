<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" encoding="utf-8" doctype-public="-//OASIS//DTD DITA Topic//EN" doctype-system="dtd/topic.dtd"/>
	<!-- start dita topic templates -->
	<xsl:param name="lang">cn</xsl:param>
	<xsl:template match="/">
		<xsl:apply-templates/>
	</xsl:template>
	<!-- method object to dita topic -->
	<xsl:template match="method">
		<topic xmlns:ditaarch="http://dita.oasis-open.org/architecture/2005/" id="{@id}">
			<xsl:if test="$lang='cn'">
				<xsl:attribute name="xml:lang">zh_cn</xsl:attribute>
			</xsl:if>
			<xsl:if test="$lang='en'">
				<xsl:attribute name="xml:lang">en_us</xsl:attribute>
			</xsl:if>
			<xsl:apply-templates select="interface"/>
			<xsl:apply-templates select="shortdesc"/>
			<body>
				<xsl:apply-templates select="description"/>
				<xsl:apply-templates select="parameters"/>
				<xsl:apply-templates select="return"/>
				<xsl:apply-templates select="privilege"/>
				<xsl:apply-templates select="reference"/>
			</body>
		</topic>
	</xsl:template>
	<!-- interface element of topic object -->
	<xsl:template match="interface">
		<title>
			<xsl:value-of select="."/>
		</title>
	</xsl:template>
	<!-- shortdesc element of topic object -->
	<xsl:template match="shortdesc">
		<shortdesc>
			<xsl:if test="$lang='cn'">
				<xsl:value-of select="@lang_cn"/>
			</xsl:if>
			<xsl:if test="$lang='en'">
				<xsl:value-of select="@lang_en"/>
			</xsl:if>
		</shortdesc>
	</xsl:template>
	<!-- description element of topic object -->
	<xsl:template match="description">
		<section id="description">
			<title>
				<xsl:if test="$lang='cn'">功能说明</xsl:if>
				<xsl:if test="$lang='en'">Feature</xsl:if>
			</title>
			<p>
				<xsl:if test="$lang='cn'">
					<xsl:value-of select="@lang_cn"/>
				</xsl:if>
				<xsl:if test="$lang='en'">
					<xsl:value-of select="@lang_en"/>
				</xsl:if>
			</p>
		</section>
	</xsl:template>
	<!-- parameters element of topic object -->
	<xsl:template match="parameters">
		<section id="parameters">
			<title>
				<xsl:if test="$lang='cn'">参数说明</xsl:if>
				<xsl:if test="$lang='en'">Parameters</xsl:if>
			</title>
			<ol>
				<xsl:for-each select="child::*">
					<xsl:if test="name()='parameter'">
						<xsl:call-template name="parameter"/>
					</xsl:if>
					<xsl:if test="name()='struct'">
						<xsl:call-template name="struct"/>
					</xsl:if>
					<xsl:if test="name()='array'">
						<xsl:call-template name="array"/>
					</xsl:if>
				</xsl:for-each>
			</ol>
		</section>
	</xsl:template>
	<!-- privilege element of topic object -->
	<xsl:template match="privilege">
		<section id="Privilege">
			<title>
				<xsl:if test="$lang='cn'">操作权限</xsl:if>
				<xsl:if test="$lang='en'">Privilege level</xsl:if>
			</title>
			<ul>
				<xsl:for-each select="child::user">
					<xsl:call-template name="user"/>
				</xsl:for-each>
			</ul>
		</section>
	</xsl:template>
	<!-- reference element of topic object -->
	<xsl:template match="reference">
		<section id="See-Also">
			<title>
				<xsl:if test="$lang='cn'">参考接口</xsl:if>
				<xsl:if test="$lang='en'">See also</xsl:if>
			</title>
			<xsl:if test="child::*">
				<ul>
					<xsl:for-each select="child::xref">
						<xsl:call-template name="xref"/>
					</xsl:for-each>
				</ul>
			</xsl:if>
		</section>
	</xsl:template>
	<!-- return element of topic object -->
	<xsl:template match="return">
		<section id="Return">
			<title>
				<xsl:if test="$lang='cn'">返回值</xsl:if>
				<xsl:if test="$lang='en'">Returns</xsl:if>
			</title>
			<xsl:apply-templates select="success"/>
			<xsl:apply-templates select="fail"/>
		</section>
	</xsl:template>
	<!-- success element of topic object -->
	<xsl:template match="success">
		<b>
			<xsl:if test="$lang='cn'">成功</xsl:if>
			<xsl:if test="$lang='en'">Success</xsl:if>
		</b>
		<ol>
			<xsl:for-each select="child::*">
				<xsl:if test="name()='parameter'">
					<xsl:call-template name="parameter"/>
				</xsl:if>
				<xsl:if test="name()='struct'">
					<xsl:call-template name="struct"/>
				</xsl:if>
				<xsl:if test="name()='array'">
					<xsl:call-template name="array"/>
				</xsl:if>
			</xsl:for-each>
		</ol>
	</xsl:template>
	<!-- fail element of topic object -->
	<xsl:template match="fail">
		<b>
			<xsl:if test="$lang='cn'">失败</xsl:if>
			<xsl:if test="$lang='en'">Fail</xsl:if>
		</b>
		<ol>
			<xsl:for-each select="child::*">
				<xsl:if test="name()='parameter'">
					<xsl:call-template name="parameter"/>
				</xsl:if>
				<xsl:if test="name()='struct'">
					<xsl:call-template name="struct"/>
				</xsl:if>
				<xsl:if test="name()='array'">
					<xsl:call-template name="array"/>
				</xsl:if>
			</xsl:for-each>
		</ol>
	</xsl:template>
	<!-- struct element of paremters object -->
	<xsl:template name="struct">
		<li>
			<b>struct</b>
			<xsl:if test="@name!=''">
				<xsl:text> </xsl:text>
				<i>
					<xsl:value-of select="@name"/>
				</i>
			</xsl:if>
			<ol>
				<xsl:for-each select="child::*">
					<xsl:if test="name()='parameter'">
						<xsl:call-template name="parameter"/>
					</xsl:if>
					<xsl:if test="name()='struct'">
						<xsl:call-template name="struct"/>
					</xsl:if>
					<xsl:if test="name()='array'">
						<xsl:call-template name="array"/>
					</xsl:if>
				</xsl:for-each>
			</ol>
		</li>
	</xsl:template>
	<!-- array element of paremters object -->
	<xsl:template name="array">
		<li>
			<b>array</b>
			<xsl:if test="@name!=''">
				<xsl:text> </xsl:text>
				<i>
					<xsl:value-of select="@name"/>
				</i>
			</xsl:if>
			<xsl:if test="$lang='cn'">
				<xsl:if test="@lang_cn!=''">
					<xsl:text>: </xsl:text>
					<xsl:value-of select="@lang_cn"/>
				</xsl:if>
			</xsl:if>
			<xsl:if test="$lang='en'">
				<xsl:if test="@lang_cn!=''">
					<xsl:text>: </xsl:text>
					<xsl:value-of select="@lang_en"/>
				</xsl:if>
			</xsl:if>
			<xsl:if test="child::*">
				<ol>
					<xsl:for-each select="child::*">
						<xsl:if test="name()='parameter'">
							<xsl:call-template name="parameter"/>
						</xsl:if>
						<xsl:if test="name()='struct'">
							<xsl:call-template name="struct"/>
						</xsl:if>
						<xsl:if test="name()='array'">
							<xsl:call-template name="array"/>
						</xsl:if>
					</xsl:for-each>
					<xsl:if test="@repeat='yes'">
						<li>...</li>
					</xsl:if>
				</ol>
			</xsl:if>
		</li>
	</xsl:template>
	<!-- parameter element of paremters object -->
	<xsl:template name="parameter">
		<li>
			<b>
				<xsl:value-of select="@type"/>
			</b>
			<xsl:text> </xsl:text>
			<xsl:value-of select="@name"/>
			<xsl:text>: </xsl:text>
			<xsl:if test="$lang='cn'">
				<xsl:value-of select="@lang_cn"/>
			</xsl:if>
			<xsl:if test="$lang='en'">
				<xsl:value-of select="@lang_en"/>
			</xsl:if>
			<xsl:if test="enum">
				<ul>
					<xsl:for-each select="child::enum">
						<xsl:call-template name="enum"/>
					</xsl:for-each>
				</ul>
			</xsl:if>
			<xsl:if test="@name='error_code'">
				<xsl:if test="parent::fail">
					<xsl:if test="$lang='cn'">
						<note>更多错误信息请参考<xref href="tecs_error_code.xml">TECS错误码定义</xref>列表。</note>
					</xsl:if>
					<xsl:if test="$lang='en'">
						<note>Please visit <xref href="tecs_error_code.xml">tecs error code definition</xref> for detailed information.</note>
					</xsl:if>
				</xsl:if>
			</xsl:if>
		</li>
	</xsl:template>
	<!-- enum element of paremters object -->
	<xsl:template name="enum">
		<li>
			<xsl:value-of select="@name"/>
			<xsl:text>: </xsl:text>
			<xsl:if test="$lang='cn'">
				<xsl:value-of select="@lang_cn"/>
			</xsl:if>
			<xsl:if test="$lang='en'">
				<xsl:value-of select="@lang_en"/>
			</xsl:if>
		</li>
	</xsl:template>
	<!-- user element of privilege object -->
	<xsl:template name="user">
		<li>
			<xsl:value-of select="@name"/>
			<xsl:text>: </xsl:text>
			<xsl:if test="$lang='cn'">
				<xsl:value-of select="@lang_cn"/>
			</xsl:if>
			<xsl:if test="$lang='en'">
				<xsl:value-of select="@lang_en"/>
			</xsl:if>
		</li>
	</xsl:template>
	<!-- xref element of reference object -->
	<xsl:template name="xref">
		<li>
			<p>
				<xref href="{@href}">
					<xsl:value-of select="@name"/>
				</xref>
			</p>
		</li>
	</xsl:template>
</xsl:stylesheet>
