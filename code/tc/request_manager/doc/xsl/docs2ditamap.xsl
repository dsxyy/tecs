<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" encoding="utf-8" doctype-public="-//OASIS//DTD DITA Map//EN" doctype-system="dtd/map.dtd"/>
	<!-- start dita topic templates -->
	<xsl:param name="lang">cn</xsl:param>
	<xsl:param name="version">V01.01.10</xsl:param>
	<xsl:template match="/">
  	<xsl:apply-templates/>
	</xsl:template>
	<!-- error code object to dita topic -->
	<xsl:template match="xmlfiles">
		<map xmlns:ditaarch="http://dita.oasis-open.org/architecture/2005/">
			<xsl:if test="$lang='cn'">
				<xsl:attribute name="title">ZXTECS XML-RPC API参考手册 <xsl:value-of select="$version"/></xsl:attribute>
				<xsl:attribute name="xml:lang">zh_cn</xsl:attribute>
			</xsl:if>
			<xsl:if test="$lang='en'">
				<xsl:attribute name="title">ZXTECS XML-RPC API REFERENCE <xsl:value-of select="$version"/></xsl:attribute>
				<xsl:attribute name="xml:lang">en_us</xsl:attribute>
			</xsl:if>
			<topicref>
				<xsl:if test="$lang='cn'">
					<xsl:attribute name="href">cn/revision_history.xml</xsl:attribute>
				</xsl:if>
				<xsl:if test="$lang='en'">
					<xsl:attribute name="href">en/revision_history.xml</xsl:attribute>
				</xsl:if>
			</topicref>
			<topicref>
				<xsl:if test="$lang='cn'">
					<xsl:attribute name="navtitle">API接口说明</xsl:attribute>
				</xsl:if>
				<xsl:if test="$lang='en'">
					<xsl:attribute name="navtitle">API Reference</xsl:attribute>
				</xsl:if>
                                <topicref>
					<xsl:if test="$lang='cn'">
						<xsl:attribute name="navtitle">系统监控</xsl:attribute>
					</xsl:if>
					<xsl:if test="$lang='en'">
						<xsl:attribute name="navtitle">System Monitor</xsl:attribute>
					</xsl:if>
					<xsl:apply-templates select="xmlfile">
						<xsl:with-param name="catalog">_system_</xsl:with-param>
					</xsl:apply-templates>
				</topicref>
				<topicref>
					<xsl:if test="$lang='cn'">
						<xsl:attribute name="navtitle">集群管理</xsl:attribute>
					</xsl:if>
					<xsl:if test="$lang='en'">
						<xsl:attribute name="navtitle">Cluster Management</xsl:attribute>
					</xsl:if>
					<xsl:apply-templates select="xmlfile">
						<xsl:with-param name="catalog">_clustermanager_</xsl:with-param>
					</xsl:apply-templates>
				</topicref>
				<topicref>
					<xsl:if test="$lang='cn'">
						<xsl:attribute name="navtitle">告警管理</xsl:attribute>
					</xsl:if>
					<xsl:if test="$lang='en'">
						<xsl:attribute name="navtitle">Alarm Management</xsl:attribute>
					</xsl:if>
					<xsl:apply-templates select="xmlfile">
						<xsl:with-param name="catalog">alarm_</xsl:with-param>
					</xsl:apply-templates>
				</topicref>
				<topicref>
					<xsl:if test="$lang='cn'">
						<xsl:attribute name="navtitle">主机管理</xsl:attribute>
					</xsl:if>
					<xsl:if test="$lang='en'">
						<xsl:attribute name="navtitle">Host Management</xsl:attribute>
					</xsl:if>
					<xsl:apply-templates select="xmlfile">
						<xsl:with-param name="catalog">_host_</xsl:with-param>
					</xsl:apply-templates>
				</topicref>
				<topicref>
					<xsl:if test="$lang='cn'">
						<xsl:attribute name="navtitle">文件管理</xsl:attribute>
					</xsl:if>
					<xsl:if test="$lang='en'">
						<xsl:attribute name="navtitle">file Management</xsl:attribute>
					</xsl:if>
					<xsl:apply-templates select="xmlfile">
						<xsl:with-param name="catalog">_file_</xsl:with-param>
					</xsl:apply-templates>
				</topicref>
				<topicref>
					<xsl:if test="$lang='cn'">
						<xsl:attribute name="navtitle">映像管理</xsl:attribute>
					</xsl:if>
					<xsl:if test="$lang='en'">
						<xsl:attribute name="navtitle">Image Management</xsl:attribute>
					</xsl:if>
					<xsl:apply-templates select="xmlfile">
						<xsl:with-param name="catalog">_image_</xsl:with-param>
					</xsl:apply-templates>
				</topicref>
				<topicref>
					<xsl:if test="$lang='cn'">
						<xsl:attribute name="navtitle">端口管理</xsl:attribute>
					</xsl:if>
					<xsl:if test="$lang='en'">
						<xsl:attribute name="navtitle">Port Management</xsl:attribute>
					</xsl:if>
					<xsl:apply-templates select="xmlfile">
						<xsl:with-param name="catalog">_port_</xsl:with-param>
					</xsl:apply-templates>
				</topicref>
				<topicref>
					<xsl:if test="$lang='cn'">
						<xsl:attribute name="navtitle">网络平面管理</xsl:attribute>
					</xsl:if>
					<xsl:if test="$lang='en'">
						<xsl:attribute name="navtitle">Netplane Management</xsl:attribute>
					</xsl:if>
					<xsl:apply-templates select="xmlfile">
						<xsl:with-param name="catalog">_netplane_</xsl:with-param>
					</xsl:apply-templates>
				</topicref>				
				<topicref>
					<xsl:if test="$lang='cn'">
						<xsl:attribute name="navtitle">工程管理</xsl:attribute>
					</xsl:if>
					<xsl:if test="$lang='en'">
						<xsl:attribute name="navtitle">Project Management</xsl:attribute>
					</xsl:if>
					<xsl:apply-templates select="xmlfile">
						<xsl:with-param name="catalog">_project_</xsl:with-param>
					</xsl:apply-templates>
				</topicref>
				<topicref>
					<xsl:if test="$lang='cn'">
						<xsl:attribute name="navtitle">TRUNK管理</xsl:attribute>
					</xsl:if>
					<xsl:if test="$lang='en'">
						<xsl:attribute name="navtitle">Trunk Management</xsl:attribute>
					</xsl:if>
					<xsl:apply-templates select="xmlfile">
						<xsl:with-param name="catalog">_trunk_</xsl:with-param>
					</xsl:apply-templates>
				</topicref>
				<topicref>
					<xsl:if test="$lang='cn'">
						<xsl:attribute name="navtitle">用户管理</xsl:attribute>
					</xsl:if>
					<xsl:if test="$lang='en'">
						<xsl:attribute name="navtitle">User Management</xsl:attribute>
					</xsl:if>
					<xsl:apply-templates select="xmlfile">
						<xsl:with-param name="catalog">_user_</xsl:with-param>
					</xsl:apply-templates>
				</topicref>
				<topicref>
					<xsl:if test="$lang='cn'">
						<xsl:attribute name="navtitle">用户组管理</xsl:attribute>
					</xsl:if>
					<xsl:if test="$lang='en'">
						<xsl:attribute name="navtitle">User Group Management</xsl:attribute>
					</xsl:if>
					<xsl:apply-templates select="xmlfile">
						<xsl:with-param name="catalog">_usergroup_</xsl:with-param>
					</xsl:apply-templates>
				</topicref>
				<topicref>
					<xsl:if test="$lang='cn'">
						<xsl:attribute name="navtitle">会话管理</xsl:attribute>
					</xsl:if>
					<xsl:if test="$lang='en'">
						<xsl:attribute name="navtitle">Session Management</xsl:attribute>
					</xsl:if>
					<xsl:apply-templates select="xmlfile">
						<xsl:with-param name="catalog">_session_</xsl:with-param>
					</xsl:apply-templates>
				</topicref>
				<topicref>
					<xsl:if test="$lang='cn'">
						<xsl:attribute name="navtitle">版本管理</xsl:attribute>
					</xsl:if>
					<xsl:if test="$lang='en'">
						<xsl:attribute name="navtitle">Version Management</xsl:attribute>
					</xsl:if>
					<xsl:apply-templates select="xmlfile">
						<xsl:with-param name="catalog">_version_</xsl:with-param>
					</xsl:apply-templates>
				</topicref>
				<topicref>
					<xsl:if test="$lang='cn'">
						<xsl:attribute name="navtitle">虚拟机配置</xsl:attribute>
					</xsl:if>
					<xsl:if test="$lang='en'">
						<xsl:attribute name="navtitle">Virtual Machine Configuration</xsl:attribute>
					</xsl:if>
					<xsl:apply-templates select="xmlfile">
						<xsl:with-param name="catalog">_vmcfg_</xsl:with-param>
					</xsl:apply-templates>
				</topicref>
				<topicref>
					<xsl:if test="$lang='cn'">
						<xsl:attribute name="navtitle">虚拟机模板管理</xsl:attribute>
					</xsl:if>
					<xsl:if test="$lang='en'">
						<xsl:attribute name="navtitle">Virtual Machine Templates Management</xsl:attribute>
					</xsl:if>
					<xsl:apply-templates select="xmlfile">
						<xsl:with-param name="catalog">_vmtemplate_</xsl:with-param>
					</xsl:apply-templates>
				</topicref>
				<topicref>
					<xsl:if test="$lang='cn'">
						<xsl:attribute name="navtitle">虚拟网络管理</xsl:attribute>
					</xsl:if>
					<xsl:if test="$lang='en'">
						<xsl:attribute name="navtitle">Virtual Network Management</xsl:attribute>
					</xsl:if>
					<xsl:apply-templates select="xmlfile">
						<xsl:with-param name="catalog">_vnet_</xsl:with-param>
					</xsl:apply-templates>
				</topicref>
				<topicref>
					<xsl:if test="$lang='cn'">
						<xsl:attribute name="navtitle">TCU管理</xsl:attribute>
					</xsl:if>
					<xsl:if test="$lang='en'">
						<xsl:attribute name="navtitle">TCU Management</xsl:attribute>
					</xsl:if>
					<xsl:apply-templates select="xmlfile">
						<xsl:with-param name="catalog">_tcu_</xsl:with-param>
					</xsl:apply-templates>
				</topicref>
				<topicref>
					<xsl:if test="$lang='cn'">
						<xsl:attribute name="navtitle">工作流管理</xsl:attribute>
					</xsl:if>
					<xsl:if test="$lang='en'">
						<xsl:attribute name="navtitle">Workflow Management</xsl:attribute>
					</xsl:if>
					<xsl:apply-templates select="xmlfile">
						<xsl:with-param name="catalog">_workflow_</xsl:with-param>
					</xsl:apply-templates>
				</topicref>
			</topicref>
			<topicref>
				<xsl:if test="$lang='cn'">
					<xsl:attribute name="navtitle">附录</xsl:attribute>
					<topicref href="cn/tecs_error_code.xml"/>
				</xsl:if>
				<xsl:if test="$lang='en'">
					<xsl:attribute name="navtitle">Appendices</xsl:attribute>
					<topicref href="en/tecs_error_code.xml"/>
				</xsl:if>
			</topicref>
		</map>
	</xsl:template>
	<xsl:template match="xmlfile">
		<xsl:param name="catalog"/>
		<xsl:if test="contains(@name,$catalog)">
			<topicref>
        <xsl:if test="$lang='cn'">
          <xsl:attribute name="href">cn/<xsl:value-of select="@name"/></xsl:attribute>
        </xsl:if>
				<xsl:if test="$lang='en'">
          <xsl:attribute name="href">en/<xsl:value-of select="@name"/></xsl:attribute>
        </xsl:if>
			</topicref>
		</xsl:if>
	</xsl:template>
</xsl:stylesheet>
