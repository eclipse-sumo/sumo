<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:doc="http://nwalsh.com/xsl/documentation/1.0"
                exclude-result-prefixes="doc"
                version='1.0'>

<xsl:output method="html"
            encoding="ISO-8859-1"
            indent="no"/>

<!-- ********************************************************************
     $Id$
     ********************************************************************

     This file is part of the XSL DocBook Stylesheet distribution.
     See ../README or http://nwalsh.com/docbook/xsl/ for copyright
     and other information.

     ******************************************************************** -->

<!-- ==================================================================== -->

<xsl:include href="../html/docbook.xsl"/>

<!-- ==================================================================== -->

<xsl:template match="*" mode="process.root">
  <xsl:variable name="doc" select="self::*"/>

  <xsl:call-template name="root.messages"/>

  <html>
    <head>
      <xsl:call-template name="system.head.content">
        <xsl:with-param name="node" select="$doc"/>
      </xsl:call-template>
      <xsl:call-template name="head.content">
        <xsl:with-param name="node" select="$doc"/>
      </xsl:call-template>
      <xsl:call-template name="user.head.content">
        <xsl:with-param name="node" select="$doc"/>
      </xsl:call-template>
    </head>
    <body>
      <xsl:call-template name="body.attributes"/>
      <xsl:call-template name="user.header.content">
        <xsl:with-param name="node" select="$doc"/>
      </xsl:call-template>

        <table border="0">
          <tr>
              <td><img src="../../images/64x64_web.gif" width="64" height="64"/></td>
              <td><img src="../../images/sumo_logo.gif" width="72" height="17"/><br/>
              <img src="../../images/sumo_full.gif" width="211" height="16"/></td>
          </tr>
          </table>
        <hr/>


<!-- Navigation bar begin -->
        <table border="0">
        <tr><td valign="top" width="120">
        <div class="navbar">
            <a href="../../index.html" class="SUMOMainLink">Home</a><br/>
            <a href="../../overview.html" class="SUMOMainLink">Overview</a><br/>
            <a href="../../documentation.html" class="SUMOMainLink">Documentation</a><br/>
            <a href="../userdocs.html" class="SUMOSubLink">User Docs</a><br/>

            <a href="../devdocs.html" class="SUMOSubLink">Developer Docs</a><br/>

            <a href="../bibliography.html" class="SUMOSubLink">Bibliography</a><br/>

            <a href="faq.html" class="SUMOSubLink">FAQ</a><br/>
            <a href="../manpages.html" class="SUMOSubLink">ManPages</a><br/>

            <a href="../../screenshots.html" class="SUMOMainLink">Screenshots</a><br/>
            <a href="../../downloads.html" class="SUMOMainLink">Downloads</a><br/>
            <a href="../../participants.html" class="SUMOMainLink">Participants</a><br/>
            <a href="../../projects.html" class="SUMOMainLink">Projects</a><br/>
            <a href="../../links.html" class="SUMOMainLink">Links</a><br/>
            <a href="../../disclaimer.html" class="SUMOMainLink">Disclaimer</a><br/>
            <a href="../../contact.html" class="SUMOMainLink">Contact</a><br/>

            <center><br/>
            <a href="http://ivf.dlr.de" class="SUMOMainLink">
            <img src="../../images/logo_dlr.gif" width="40" height="42"/></a>
            <a href="http://www.zaik.uni-koeln.de/" class="SUMOMainLink">
            <img src="../../images/logo_zaik.gif" width="40" height="42"/></a><br/>

            <a href="http://sourceforge.net/projects/sumo/"><img src="../../images/sourceforge.gif" width="88" height="31" border="0" alt="SourceForge.net Logo"/></a>
            </center>
        </div>
        </td><td width="20"></td>
<!-- Navigation bar end -->


        <td valign="top">
        <div class="SUMOMainText">
<!-- Text area here -->

      <xsl:apply-templates select="."/>
      <xsl:call-template name="user.footer.content">
        <xsl:with-param name="node" select="$doc"/>
      </xsl:call-template>

<!-- End of Text area -->
        </div>
        </td></tr></table>
        <p></p><hr/>

    </body>
  </html>
</xsl:template>

<xsl:template name="root.messages">
  <!-- redefine this any way you'd like to output messages -->
  <!-- DO NOT OUTPUT ANYTHING FROM THIS TEMPLATE -->
</xsl:template>


<xsl:template match="itemizedlist/listitem/para">
  <xsl:call-template name="myparagraph">
    <xsl:with-param name="class">
      <xsl:if test="@role and $para.propagates.style != 0">
        <xsl:value-of select="@role"/>
      </xsl:if>
    </xsl:with-param>
    <xsl:with-param name="content">
      <xsl:if test="position() = 1 and parent::listitem">
        <xsl:call-template name="anchor">
          <xsl:with-param name="node" select="parent::listitem"/>
        </xsl:call-template>
      </xsl:if>

      <xsl:call-template name="anchor"/>
      <xsl:apply-templates/>
    </xsl:with-param>
  </xsl:call-template>
</xsl:template>

<xsl:template match="caution/para">
  <xsl:call-template name="myparagraph">
    <xsl:with-param name="class">
      <xsl:if test="@role and $para.propagates.style != 0">
        <xsl:value-of select="@role"/>
      </xsl:if>
    </xsl:with-param>
    <xsl:with-param name="content">
      <xsl:if test="position() = 1 and parent::listitem">
        <xsl:call-template name="anchor">
          <xsl:with-param name="node" select="parent::listitem"/>
        </xsl:call-template>
      </xsl:if>

      <xsl:call-template name="anchor"/>
      <xsl:apply-templates/>
    </xsl:with-param>
  </xsl:call-template>
</xsl:template>


<xsl:template name="myparagraph">
  <xsl:param name="class" select="''"/>
  <xsl:param name="content"/>

  <xsl:variable name="p">
      <xsl:if test="$class != ''">
        <xsl:attribute name="class">
          <xsl:value-of select="$class"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:copy-of select="$content"/>
    <br/>
  </xsl:variable>

  <xsl:choose>
    <xsl:when test="$html.cleanup != 0">
      <xsl:call-template name="unwrap.p">
        <xsl:with-param name="p" select="$p"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:copy-of select="$p"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>



<xsl:template name="nongraphical.admonition">
  <div class="{name(.)}">
    <xsl:if test="$admon.style">
      <xsl:attribute name="style">
        <xsl:value-of select="$admon.style"/>
      </xsl:attribute>
    </xsl:if>

    <b>
      <xsl:call-template name="anchor"/>
      <xsl:if test="$admon.textlabel != 0 or title">
        <xsl:apply-templates select="." mode="object.title.markup"/>
      </xsl:if>
    </b>

    <xsl:apply-templates/>
  </div>
</xsl:template>


<xsl:template name="graphical.admonition">
  <xsl:variable name="admon.type">
    <xsl:choose>
      <xsl:when test="local-name(.)='note'">Note</xsl:when>
      <xsl:when test="local-name(.)='warning'">Warning</xsl:when>
      <xsl:when test="local-name(.)='caution'">Caution</xsl:when>
      <xsl:when test="local-name(.)='tip'">Tip</xsl:when>
      <xsl:when test="local-name(.)='important'">Important</xsl:when>
      <xsl:otherwise>Note</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <div class="{name(.)}">
    <xsl:if test="$admon.style != ''">
      <xsl:attribute name="style">
        <xsl:value-of select="$admon.style"/>
      </xsl:attribute>
    </xsl:if>
          <img alt="[{$admon.type}]">
            <xsl:attribute name="src">
              <xsl:call-template name="admon.graphic"/>
            </xsl:attribute>
          </img>
    <xsl:apply-templates/>
    </div>
</xsl:template>


<!-- ==================================================================== -->

<xsl:template name="chunk">
  <xsl:param name="node" select="."/>

  <!-- The default is that we are not chunking... -->
  <xsl:text>0</xsl:text>
</xsl:template>

<!-- ==================================================================== -->

</xsl:stylesheet>
