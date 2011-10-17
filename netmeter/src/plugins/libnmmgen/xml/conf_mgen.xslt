<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" encoding="UTF-8"/>

<xsl:template match="start">
	<xsl:text>START  </xsl:text>
	<xsl:value-of select="."/>
</xsl:template>

<xsl:template match="port">
	<xsl:text>PORT  </xsl:text>
	<xsl:value-of select="."/>
</xsl:template>

<xsl:template match="interface">
	<xsl:text>INTERFACE  </xsl:text>
	<xsl:value-of select="."/>
</xsl:template>

<xsl:template match="ttl">
	<xsl:text>TTL  </xsl:text>
	<xsl:value-of select="."/>
</xsl:template>

<xsl:template match="on">
	<xsl:value-of select="@time"/>
	<xsl:text>  </xsl:text>
	<xsl:value-of select="@flow_id"/>
	<xsl:text>  ON  </xsl:text>
	<xsl:value-of select="translate(@dest,'/',':')"/>
	<xsl:text>  </xsl:text>
	<xsl:value-of select="@pattern"/>
	<xsl:text>  </xsl:text>
	<xsl:value-of select="@rate"/>
	<xsl:text>  </xsl:text>
	<xsl:value-of select="@size"/>
	<xsl:apply-templates select="rsvp"/>
	<xsl:apply-templates select="tos"/>
</xsl:template>

<xsl:template match="mod">
	<xsl:value-of select="@time"/>
	<xsl:text>  </xsl:text>
	<xsl:value-of select="@flow_id"/>
	<xsl:text>  MOD </xsl:text>
	<xsl:value-of select="translate(@dest,'/',':')"/>
	<xsl:text>  </xsl:text>
	<xsl:value-of select="@pattern"/>
	<xsl:text>  </xsl:text>
	<xsl:value-of select="@rate"/>
	<xsl:text>  </xsl:text>
	<xsl:value-of select="@size"/>
	<xsl:apply-templates select="rsvp"/>
	<xsl:apply-templates select="tos"/>
</xsl:template>

<xsl:template match="off">
	<xsl:value-of select="@time"/>
	<xsl:text>  </xsl:text>
	<xsl:value-of select="@flow_id"/>
	<xsl:text>  OFF </xsl:text>
</xsl:template>

<xsl:template match="rsvp">
	<xsl:text>  RSVP  </xsl:text>
	<xsl:value-of select="@spec"/>
</xsl:template>

<xsl:template match="tos">
	<xsl:text>  TOS  </xsl:text>
	<xsl:value-of select="."/>
</xsl:template>

</xsl:stylesheet>
