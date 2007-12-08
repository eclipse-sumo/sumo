<?xml version='1.0'?>
<xsl:stylesheet  
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:import href="http://docbook.sourceforge.net/release/xsl/current/html/chunk.xsl"/>

<xsl:template name="user.header.navigation">
<!-- -->
        <table border="0">
          <tr>
              <td><img src="../../images/64x64_web.gif" width="64" height="64"/></td>
              <td><img src="../../images/sumo_logo.gif" width="72" height="17"/><br/>
              <img src="../../images/sumo_full.gif" width="211" height="16"/></td>
          </tr>
          </table>
        <hr/>


<!-- Navigation bar begin -->
    <xsl:text disable-output-escaping="yes">
        &lt;table border="0" ID="Table1">
        &lt;tr>
    </xsl:text>
	<td valign="top" width="120">
        <div class="navbar">
            <a href="../../index.shtml" class="SUMOMainLink">Home</a><br/>
            <a href="../../overview.shtml" class="SUMOMainLink">Overview</a><br/>
            <a href="../documentation.shtml" class="SUMOMainLink">Documentation</a><br/>

            <a href="../userdocs.shtml" class="SUMOSubLink">User Docs</a><br/>
            <a href="user_index.shtml" class="SUMOSubSubLink">Index</a><br/>
            <a href="user_chp01.shtml" class="SUMOSubSubLink">1. Introduction</a><br/>
            <a href="user_chp02.shtml" class="SUMOSubSubLink">2. First Steps</a><br/>
            <a href="user_chp03.shtml" class="SUMOSubSubLink">3. Traffic Sims</a><br/>
            <a href="user_chp04.shtml" class="SUMOSubSubLink">4. Networks</a><br/>
            <a href="user_chp05.shtml" class="SUMOSubSubLink">5. Routes</a><br/>
            <a href="user_chp06.shtml" class="SUMOSubSubLink">6. Simulation</a><br/>
            <a href="user_chp07.shtml" class="SUMOSubSubLink">7. GUI</a><br/>
            <a href="user_chp08.shtml" class="SUMOSubSubLink">8. Tips+Tricks</a><br/>
            <a href="user_apa.shtml" class="SUMOSubSubLink">A. Conventions</a><br/>
            <a href="user_apb.shtml" class="SUMOSubSubLink">B. Included</a><br/>

            <a href="../moredocs.shtml" class="SUMOSubLink">More On...</a><br/>

			<a href="http://sumo.sourceforge.net/wiki/index.php/Publications" class="SUMOSubLink">Bibliography</a><br/>
			<a href="http://sumo.sourceforge.net/wiki/index.php/FAQ" class="SUMOSubLink">FAQ</a><br/>
			<!--
			<a href="manpages.shtml" class="SUMOSubLink">ManPages</a><br/>
			<a href="docs/examples.html" class="SUMOSubLink">Examples</a><br/>
			<a href="classdocs/index.html" class="SUMOSubLink">Class Docs</a><br/>
			-->

            <a href="../../screens/screenshots.shtml" class="SUMOMainLink">Screenshots</a><br/>
            <a href="../../downloads/downloads.shtml" class="SUMOMainLink">Downloads</a><br/>
            <a href="../../participants.shtml" class="SUMOMainLink">Participants</a><br/>
            <a href="../../projects.shtml" class="SUMOMainLink">Projects</a><br/>
            <a href="../../links/links.shtml" class="SUMOMainLink">Links</a><br/>
            <a href="../../disclaimer.shtml" class="SUMOMainLink">Disclaimer</a><br/>
            <a href="../../contact.shtml" class="SUMOMainLink">Contact</a><br/>

			<a href="http://sumo.sourceforge.net/wiki/" class="SUMOMainLink">wiki</a><br/>

            <center><br/>
            <a href="http://ivf.dlr.de" class="SUMOMainLink">
            <img src="../../images/logo_dlr.gif" width="40" height="42"/></a>
            <a href="http://www.zaik.uni-koeln.de/" class="SUMOMainLink">
            <img src="../../images/logo_zaik.gif" width="40" height="42"/></a><br/>

            <a href="http://sourceforge.net/projects/sumo/"><img src="http://sourceforge.net/sflogo.php?group_id=45607$%AMP%$type=1" width="88" height="31" border="0" alt="SourceForge.net Logo"/></a>
            </center>
        </div>
        </td><td width="20"></td>
<!-- Navigation bar end -->


    <xsl:text disable-output-escaping="yes">
        &lt;td valign="top">
        &lt;div class="SUMOMainText">
    </xsl:text>
<!-- Text area here -->

<!-- -->
</xsl:template>

<xsl:template name="user.footer.navigation">
<!-- -->

    <xsl:text disable-output-escaping="yes">
        &lt;/div>
        &lt;/td>
        &lt;/tr>&lt;/table>
    </xsl:text>

<!-- -->
</xsl:template>

</xsl:stylesheet>
