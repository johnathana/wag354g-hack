
<!--
*********************************************************
*   Copyright 2003, CyberTAN  Inc.  All Rights Reserved *
*********************************************************

This is UNPUBLISHED PROPRIETARY SOURCE CODE of CyberTAN Inc.
the contents of this file may not be disclosed to third parties,
copied or duplicated in any form without the prior written
permission of CyberTAN Inc.

This software should be used as a reference only, and it not
intended for production use!


THIS SOFTWARE IS OFFERED "AS IS", AND CYBERTAN GRANTS NO WARRANTIES OF ANY
KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.  CYBERTAN
SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE
-->

<HTML><HEAD><TITLE>Outgoing Log Table</TITLE>
<% no_cache(); %>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<script src="common.js">
<script language="JavaScript" type="text/javascript" src="share.js"></script>
<script language="JavaScript" type="text/javascript" src="capsec.js"></script>
<SCRIPT language=JavaScript>
document.title=(secother.outgoing);
</SCRIPT>
</HEAD>
<BODY bgColor=white onload={window.focus();}>
<FORM>
<CENTER>
<TABLE cellSpacing=1 width=721 border=0>
  <TBODY>
  <TR>
    <TH width=443 height=30 bgColor="#FFFFFF" align=left colspan=2><FONT face=Arial color=blue size=4><B><script>Capture(secother.outgoing)</script>*Outgoing Log Table</B></FONT></TH>
    <TH width=307 height=30 bgColor="#FFFFFF" align=right><INPUT onclick=window.location.reload() type=button value=" Refresh "></TH></TR>
  <TR align=middle bgColor=#b3b3b3>
    <TH width=163 bgcolor="#C0C0C0"><FONT face=Arial size=2><script>Capture(secother.lanip)</script>*LAN IP</FONT></TH>
    <TH width=280 bgcolor="#C0C0C0"><FONT face=Arial size=2><script>Capture(secother.desturlip)</script>*Destination URL/IP</FONT></TH>
    <TH width=307 bgcolor="#C0C0C0"><FONT face=Arial size=2><script>Capture(secother.srvportnum)</script>*Service/Port Number</FONT></TH></TR>
<% dumplog("outgoing"); %>
  <TR>
    <TD align=middle></TD>
    <TD align=middle></TD>
    <TD align=right height="35"> 
    <INPUT onclick=self.close() type=reset value=Close></TD></TR>
    </TBODY></TABLE></CENTER>
<P> </P></FORM></BODY></HTML>
