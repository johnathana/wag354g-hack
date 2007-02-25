
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

<HTML><HEAD><TITLE>Routing Table</TITLE>
<% no_cache(); %>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<SCRIPT src="common.js"></SCRIPT>
<script language="JavaScript" type="text/javascript" src="share.js"></script>
<script language="JavaScript" type="text/javascript" src="capsetup.js"></script>
<SCRIPT language=JavaScript>
document.title=(setadvrouting.routingtable);
</SCRIPT>
</HEAD>
<BODY bgColor=white>
<FORM>
<CENTER>
<TABLE height=100 cellSpacing=1 width=673 border=0>
  <TBODY>
  <TR>
    <TD width=343 colSpan=3 height=30><B><FONT face=Arial color=#0000ff size="4"><script>Capture(share.routable)</script></FONT></B></TD>
    <TD align=right width=140 height=30>&nbsp;</TD>
    <TD align=center><!--<INPUT onclick=window.location.reload() type=button value=" Refresh ">--><script>document.write("<INPUT onclick=window.location.reload() type=button value=\"" +  share.refreshs + "\">");</script></TD></TR>
  <TR align=middle bgColor=#b3b3b3>
    <TH height=30><FONT face=Arial size=2><script>Capture(share.destlanip)</script></FONT></TH>
    <TH height=30><FONT face=Arial size=2><script>Capture(setnetsetup.subnet)</script></FONT></TH>
    <TH height=30><FONT face=Arial size=2><script>Capture(share.gateway)</script></FONT></TH>
    <TH height=30><FONT face=Arial size=2><script>Capture(share.hopcount)</script></FONT></TH>
    <TH height=30><FONT face=Arial size=2><script>Capture(share.iface)</script></FONT></TH></TR>
<script language=javascript>
var table = new Array(
<% dump_route_table(""); %>
);

function IsExistcontent(num)
{
	var t = 0;
	
	while(t < num)
	{
		if ((table[num] == table[t]) && (table[num + 1] == table[t + 1]) && (table[num + 2] == table[t + 2])) return true;
		//t += 4;
		t += 5;
	}
	return false;
}

var i = 0;
for(;;){
	if(!table[i]){
		if(i == 0){
			document.write("<tr bgcolor=cccccc align=middle>");
			document.write("<td>None</td>");
			document.write("<td>None</td>");
			document.write("<td>None</td>");
			document.write("<td>None</td>");
			document.write("<td>None</td></tr>");
		}
		break;
	}
	if (IsExistcontent(i))
	{
		//i = i + 4;
		i = i + 5;
		continue;
	}

	<% wireless_support(5); %> if(table[i+4] == "LAN")	table[i+4] = "LAN &amp; Wireless";
	else <% wireless_support(6); %> 
		if(table[i+4] == "WAN") table[i+4] = "WAN (Internet)";
	document.write("<tr bgcolor=#cccccc align=middle>");
	document.write("<td>"+table[i]+"</td>");
	document.write("<td>"+table[i+1]+"</td>");
	document.write("<td>"+table[i+2]+"</td>");
	document.write("<td>"+table[i+3]+"</td>");
	document.write("<td>"+table[i+4]+"</tr>");
	//i = i + 4;
	i = i + 5;
}
</script>
  <TR align=middle bgColor=#cccccc>
    <TD bgcolor="#FFFFFF" height="35">&nbsp;</TD>
    <TD bgcolor="#FFFFFF" height="35">&nbsp;</TD>
    <TD bgcolor="#FFFFFF" height="35">&nbsp;</TD>
    <TD bgcolor="#FFFFFF" height="35">&nbsp;</TD>
    <TD bgcolor="#FFFFFF" height="35"> 
   <!-- <INPUT onclick=self.close() type=reset value=Close>--><script>document.write("<INPUT onclick=self.close() type=reset value=\"" + share.closes + "\">");</script></TD></TR></TBODY></TABLE></CENTER></FORM></BODY></HTML>
