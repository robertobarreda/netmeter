<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" encoding="UTF-8"/>

<xsl:template match="start">
	<xsl:text>START  </xsl:text>
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

<xsl:template match="port">
</xsl:template>

<xsl:template match="on">
	<xsl:value-of select="format-number((@time div 1000),'0.0##')"/>
	<xsl:text>  ON </xsl:text>
	<xsl:value-of select="@flow_id"/>
	<xsl:text> UDP  DST  </xsl:text>
	<xsl:value-of select="@dest"/>
	<xsl:text>  SRC  </xsl:text>
	<xsl:value-of select="//*/port/."/>
	<xsl:text>  </xsl:text>
	<xsl:choose>
		<xsl:when test="burst">
			<xsl:text>BURST  [</xsl:text>
			<xsl:value-of select="burst/@interval_txt" />
			<xsl:text>  </xsl:text>
			<xsl:value-of select="burst/@interval" />
			<xsl:text>  </xsl:text>
			<xsl:value-of select="@pattern"/>
			<xsl:text>  [</xsl:text>
			<xsl:value-of select="@rate"/>
			<xsl:text>  </xsl:text>
			<xsl:value-of select="@size"/>
			<xsl:text>]  </xsl:text>
			<xsl:value-of select="burst/@duration_txt" />
			<xsl:text>  </xsl:text>
			<xsl:value-of select="burst/@duration" />
			<xsl:text>]  </xsl:text>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="@pattern"/>
			<xsl:text>  [</xsl:text>
			<xsl:value-of select="@rate"/>
			<xsl:text>  </xsl:text>
			<xsl:value-of select="@size"/>
			<xsl:text>]  </xsl:text>
		</xsl:otherwise>
	</xsl:choose>
	<xsl:apply-templates select="tos"/>
</xsl:template>

<xsl:template match="mod">
	<xsl:value-of select="format-number((@time div 1000),'0.0##')"/>
	<xsl:text>  MOD  </xsl:text>
	<xsl:value-of select="@flow_id"/>
	<xsl:text>  DST  </xsl:text>
	<xsl:value-of select="@dest"/>
	<xsl:text>   </xsl:text>
	<xsl:choose>
		<xsl:when test="burst">
			<xsl:text>BURST  [</xsl:text>
			<xsl:value-of select="burst/@interval_txt" />
			<xsl:text>  </xsl:text>
			<xsl:value-of select="burst/@interval" />
			<xsl:text>  </xsl:text>
			<xsl:value-of select="@pattern"/>
			<xsl:text>  [</xsl:text>
			<xsl:value-of select="@rate"/>
			<xsl:text>  </xsl:text>
			<xsl:value-of select="@size"/>
			<xsl:text>]  </xsl:text>
			<xsl:value-of select="burst/@duration_txt" />
			<xsl:text>  </xsl:text>
			<xsl:value-of select="burst/@duration" />
			<xsl:text>]  </xsl:text>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="@pattern"/>
			<xsl:text>  [</xsl:text>
			<xsl:value-of select="@rate"/>
			<xsl:text>  </xsl:text>
			<xsl:value-of select="@size"/>
			<xsl:text>]  </xsl:text>
		</xsl:otherwise>
	</xsl:choose>
	<xsl:apply-templates select="tos"/>
</xsl:template>

<xsl:template match="off">
	<xsl:value-of select="format-number((@time div 1000),'0.0##')"/>
	<xsl:text>  OFF  </xsl:text>
	<xsl:value-of select="@flow_id"/>
</xsl:template>

<xsl:template match="tos">
	<xsl:text>TOS  </xsl:text>
	<xsl:value-of select="."/>
</xsl:template>

</xsl:stylesheet>
