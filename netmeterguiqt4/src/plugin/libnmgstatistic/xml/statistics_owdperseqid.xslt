<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" />
<xsl:strip-space elements="*" />
<xsl:template match="metadata" />
<xsl:template match="flow">Flow ID: <xsl:value-of select="@id"/>
SeqID	OWD (seg)
-----------------
<xsl:for-each select="seq">
<xsl:variable name="rxt" select="rx_time"/>
<xsl:variable name="txt" select="tx_time"/>

<xsl:variable name="hour-rx" select="substring-before($rxt,':')"/>
<xsl:variable name="min-rx" select="substring-before(substring-after($rxt,':'),':')"/>
<xsl:variable name="seg-rx" select="substring-before(substring-after(substring-after($rxt,':'),':'),'.')"/>
<xsl:variable name="ms-rx" select="substring-after($rxt,'.')"/>
		
<xsl:variable name="hour-tx" select="substring-before($txt,':')"/>
<xsl:variable name="min-tx" select="substring-before(substring-after($txt,':'),':')"/>
<xsl:variable name="seg-tx" select="substring-before(substring-after(substring-after($txt,':'),':'),'.')"/>
<xsl:variable name="ms-tx" select="substring-after($txt,'.')"/>
			
<xsl:variable name="rxres" select="($hour-rx * 3600) + ($min-rx * 60) + $seg-rx"/>
<xsl:variable name="txres" select="($hour-tx * 3600) + ($min-tx * 60) + $seg-tx"/>
<xsl:variable name="res" select="(($rxres - $txres)*1000000 + ($ms-rx - $ms-tx)) div 1000"/>
	
<xsl:value-of select="@id"/><xsl:text>	</xsl:text><xsl:value-of select="format-number($res, '0.000')"/>
<xsl:text>
</xsl:text>
</xsl:for-each>
<xsl:text>
</xsl:text>
</xsl:template>
</xsl:stylesheet>
