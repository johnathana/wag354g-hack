
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

<HTML><HEAD><TITLE>VPN Log</TITLE>
<% no_cache(); %>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<script src="common.js"></script>
<script language="JavaScript" type="text/javascript" src="share.js"></script>
<script language="JavaScript" type="text/javascript" src="capsec.js"></script>
<SCRIPT language=JavaScript>
document.title=(secother.vpnlog);
</SCRIPT>
</HEAD>
<BODY bgColor=white onload={window.focus();}>
<FORM>
<CENTER>
<TABLE cellSpacing=1 width=537 border=0>
  <TBODY>
  <TR>
    <TH width=280 height=30 bgColor="#FFFFFF" align=left><FONT face=Arial color=blue size=4><B><script>Capture(secother.vpnlog)</script></B></FONT></TH>
    <TH width=280 height=30 bgColor="#FFFFFF" align=right><INPUT onclick=window.location.reload() type=button value=" Refresh "></TH></TR>
  <TR align=middle bgColor=#b3b3b3>
<TD colspan=2 bgColor="#FFFFFF" align=left><FONT face=Arial color=black size=2>
<% dumpvpnlog(""); %></FONT>
</TD>
</TR>
  <TR align=middle bgColor=#b3b3b3>
    <TH width=280 height="35" bgcolor="#FFFFFF">&nbsp;</TH>
    <TH width=280 height="35" bgcolor="#FFFFFF" align="right"> 
    <INPUT onclick=self.close() type=reset value=Close></TH></TR>
    </TBODY></TABLE></CENTER>&nbsp; </FORM></BODY></HTML>
