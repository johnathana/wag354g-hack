
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

<HTML><HEAD><TITLE>ARP/RARP Table</TITLE>
<% no_cache(); %>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<link rel="stylesheet" type="text/css" href="style.css">
<SCRIPT src="common.js"></SCRIPT>
<SCRIPT language="Javascript" type="text/javascript" src="share.js"></SCRIPT>
<SCRIPT language="Javascript" type="text/javascript" src="capstatus.js"></SCRIPT>
<script language="JavaScript">
document.title=(stabutton.arptbl);
function refresh_win()
{
	window.location.href="ARPTable.asp";
}

</script>
</HEAD>
<BODY bgColor=white onload={window.focus();}>
<form method=<% get_http_method(); %> action=apply.cgi>
<input type=hidden name=submit_button>
<input type=hidden name=change_action>
<input type=hidden name=submit_type>
<CENTER>
<TABLE height=108 cellSpacing=1 width=673 border=0>
  <TBODY>
  <TR>
    <TD width=388 colSpan=2 height=27><font face="Arial" color="blue" size="4"><b><script>Capture(staarptbl.arptbl)</script></b></font></TD>
    <TD align=right width=80 height=27> 
   
      <script>document.write("<INPUT onclick=self.close() type=reset value=\"" + share.closes + "\">");</script></TD>
    <TD align=right width=104 height=27></TD></TR>
  <TR align=middle bgColor=#b3b3b3>
    <TH height=32 width="291"><FONT color=black><script>Capture(stalocalnet.ipaddr)</script></FONT></TH>
    <TH height=32 width="249"><FONT color=black><script>Capture(stalocalnet.macaddr)</script></FONT></TH>
    <TD height=32 width="104">&nbsp;
      <script>document.write("<INPUT onclick=refresh_win() type=button value=\"" + share.refreshs + "\">");</script>

      <script language=javascript>
var table = new Array(
<% dumparp(""); %>
);

var len = table.length;
var j = 0;




j = 0;
if ((len == 0)  ||  (len%2 != 0))
{
			document.write("<tr bgcolor=cccccc align=middle>");
			document.write("<td>None</td>");
			document.write("<td>None</td></tr>");
			
}
else
{
  for(j=0;j<len;j+=2)
  {
	
	
	document.write("<tr bgcolor=cccccc align=middle>");
	document.write("<td>"+table[j]+"</td>");
	document.write("<td>"+table[j+1]+"</td>");
	document.write("<td>"+"</td></tr>");
	
	
  }
}
</script>
  <TR align=middle bgColor=#cccccc>
    <TD bgcolor="#FFFFFF" height="35" width="291">&nbsp;</TD>
    <TD bgcolor="#FFFFFF" height="35" width="249">&nbsp;</TD>
    <TD bgcolor="#FFFFFF" height="35" width="104"> 
    &nbsp;</TD></TR>
</TBODY>
</TABLE>
</CENTER>
</FORM>
</BODY>
</HTML>
