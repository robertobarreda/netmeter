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
	<xsl:value-of select="@dest"/>
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
	<xsl:value-of select="@dest"/>
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
	<xsl:text>  RSVP [ t </xsl:text>
	<xsl:value-of select="@rate"/>
	<xsl:text> </xsl:text>
	<xsl:value-of select="@bucket_size"/>
	<xsl:text> </xsl:text>
	<xsl:value-of select="@peak_rate"/>
	<xsl:text> </xsl:text>
	<xsl:value-of select="@min"/>
	<xsl:text> </xsl:text>
	<xsl:value-of select="@max"/>
	<xsl:text> ]</xsl:text>
</xsl:template>

<xsl:template match="tos">
	<xsl:text>  TOS  </xsl:text>
	<xsl:value-of select="."/>
</xsl:template>

</xsl:stylesheet>
