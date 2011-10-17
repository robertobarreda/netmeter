<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" encoding="UTF-8"/>

<xsl:template match="ports">
	<xsl:text>PORT </xsl:text>
	<xsl:value-of select="."/>
</xsl:template>

<xsl:template match="resv">
	<xsl:value-of select="@time"/>
	<xsl:text>  RESV  </xsl:text>
	<xsl:value-of select="translate(@dest,'/',':')"/>
	<xsl:text>  </xsl:text>
	<xsl:value-of select="@style"/>
	<xsl:text>  </xsl:text>
	<xsl:value-of select="@spec"/>
	<xsl:text>  </xsl:text>
</xsl:template>

<xsl:template match="unresv">
	<xsl:value-of select="@time"/>
	<xsl:text>  UNRESV  </xsl:text>
	<xsl:value-of select="translate(@dest,'/',':')"/>
	<xsl:text>  </xsl:text>
	<xsl:value-of select="@time"/>
</xsl:template>

</xsl:stylesheet>
