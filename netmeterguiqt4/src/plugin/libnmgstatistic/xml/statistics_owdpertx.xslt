<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" />
<xsl:strip-space elements="*" />
<xsl:template match="metadata" />
<xsl:template match="flow">Flow ID: <xsl:value-of select="@id"/>
TxDly(seg)	OWD(seg)
------------------------
<xsl:variable name="txt" select="seq/tx_time"/>
<xsl:variable name="hour-tx" select="substring-before($txt,':')"/>
<xsl:variable name="min-tx" select="substring-before(substring-after($txt,':'),':')"/>
<xsl:variable name="seg-tx" select="substring-before(substring-after(substring-after($txt,':'),':'),'.')"/>
<xsl:variable name="ms-tx" select="substring-after($txt,'.')"/>
<xsl:variable name="txres" select="($hour-tx * 3600) + ($min-tx * 60) + $seg-tx"/>
	
<xsl:for-each select="seq">
<xsl:variable name="rxt" select="rx_time"/>
<xsl:variable name="hour-rx" select="substring-before($rxt,':')"/>
<xsl:variable name="min-rx" select="substring-before(substring-after($rxt,':'),':')"/>
<xsl:variable name="seg-rx" select="substring-before(substring-after(substring-after($rxt,':'),':'),'.')"/>
<xsl:variable name="ms-rx" select="substring-after($rxt,'.')"/>
<xsl:variable name="rxres" select="($hour-rx * 3600) + ($min-rx * 60) + $seg-rx"/>
	
<xsl:variable name="txt-new" select="tx_time"/>
<xsl:variable name="hour-tx-new" select="substring-before($txt-new,':')"/>
<xsl:variable name="min-tx-new" select="substring-before(substring-after($txt-new,':'),':')"/>
<xsl:variable name="seg-tx-new" select="substring-before(substring-after(substring-after($txt-new,':'),':'),'.')"/>
<xsl:variable name="ms-tx-new" select="substring-after($txt-new,'.')"/>
<xsl:variable name="txres-new" select="($hour-tx-new * 3600) + ($min-tx-new * 60) + $seg-tx-new"/>
				
<xsl:variable name="res" select="(($txres-new - $txres)*1000000 + ($ms-tx-new - $ms-tx)) div 1000"/>
<xsl:variable name="res-new" select="(($rxres - $txres-new)*1000000 + ($ms-rx - $ms-tx-new)) div 1000"/>
<xsl:value-of select="format-number($res, '0.000')"/><xsl:text>		</xsl:text><xsl:value-of select="format-number($res-new, '0.000')"/>
<xsl:text>
</xsl:text>
</xsl:for-each>
<xsl:text>
</xsl:text>
</xsl:template>
</xsl:stylesheet>
