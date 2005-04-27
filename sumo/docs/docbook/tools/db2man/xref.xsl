<?xml version='1.0'?>
<!-- vim:set sts=2 shiftwidth=2 syntax=sgml: -->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'>

<xsl:template match="xref">
  <xsl:variable name="targets" select="id(@linkend)"/>
  <xsl:variable name="target" select="$targets[1]"/>
  <xsl:variable name="type" select="local-name($target)"/>

  <xsl:choose>
    <xsl:when test="$type=''">
      <xsl:message>
        <xsl:text>xref to nonexistent id </xsl:text>
        <xsl:value-of select="@linkend"/>
      </xsl:message>
    </xsl:when>

    <xsl:when test="$type='refentry'">
      <xsl:call-template name="do-citerefentry">
        <xsl:with-param name="refentrytitle"
                        select="$target/refmeta/refentrytitle[1]"/>
        <xsl:with-param name="manvolnum"
			select="$target/refmeta/manvolnum"/>
      </xsl:call-template>
    </xsl:when>

    <xsl:when test="$type='refname'">
      <xsl:call-template name="do-citerefentry">
        <xsl:with-param name="refentrytitle" select="$target"/>
        <xsl:with-param name="manvolnum"
          select="$target/../../refmeta/manvolnum"/>
      </xsl:call-template>
    </xsl:when>

    <xsl:otherwise>
      <xsl:text>[xref to </xsl:text>
      <xsl:value-of select="$type"/>
      <xsl:text>]</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
