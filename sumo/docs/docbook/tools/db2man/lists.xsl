<?xml version='1.0'?>
<!-- vim:set sts=2 shiftwidth=2 syntax=sgml: -->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'>

<xsl:template match="para|simpara|remark" mode="list">
  <xsl:variable name="foo">
    <xsl:apply-templates/>
  </xsl:variable>
  <xsl:value-of select="normalize-space($foo)"/>
  <xsl:text>&#10;</xsl:text>
  <xsl:if test="following-sibling::para or following-sibling::simpara or
		following-sibling::remark">
    <!-- Make sure multiple paragraphs within a list item don't -->
    <!-- merge together.                                        -->
    <xsl:text>&#10;</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="varlistentry|glossentry">
  <xsl:text>&#10;.TP&#10;</xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="variablelist|glosslist" mode="list">
  <xsl:text>&#10;.RS&#10;</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>&#10;.RE&#10;</xsl:text>
</xsl:template>

<xsl:template match="varlistentry/term|glossterm">
  <xsl:variable name="content">
    <xsl:apply-templates/>
  </xsl:variable>
  <xsl:value-of select="normalize-space($content)"/>
  <xsl:text>, </xsl:text>
</xsl:template>

<xsl:template
     match="varlistentry/term[position()=last()]|glossterm[position()=last()]"
     priority="2">
  <xsl:variable name="content">
    <xsl:apply-templates/>
  </xsl:variable>
  <xsl:value-of select="normalize-space($content)"/>
</xsl:template>

<xsl:template match="varlistentry/listitem|glossdef">
  <xsl:text>&#10;</xsl:text>
  <xsl:apply-templates mode="list"/>
</xsl:template>

<xsl:template match="itemizedlist/listitem">
  <xsl:text>\(bu&#10;</xsl:text>
  <xsl:apply-templates mode="list"/>
  <xsl:if test="position()!=last()">
    <xsl:text>.TP&#10;</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="orderedlist/listitem|procedure/step">
  <xsl:number format="1."/>
  <xsl:text>&#10;</xsl:text>
  <xsl:apply-templates mode="list"/>
  <xsl:if test="position()!=last()">
    <xsl:text>.TP&#10;</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="itemizedlist|orderedlist|procedure">
  <xsl:text>&#10;.TP 3&#10;</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>.LP&#10;</xsl:text>
</xsl:template>

</xsl:stylesheet>
