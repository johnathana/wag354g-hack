
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

<HTML><HEAD><TITLE>DHCP Active IP Table</TITLE>
<% no_cache(); %>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<link rel="stylesheet" type="text/css" href="style.css">
<SCRIPT src="common.js"></SCRIPT>
<SCRIPT language="Javascript" type="text/javascript" src="share.js"></SCRIPT>
<SCRIPT language="Javascript" type="text/javascript" src="capaccess.js"></SCRIPT>
<SCRIPT language="Javascript" type="text/javascript" src="capstatus.js"></SCRIPT>
<script language="JavaScript">
document.title=(share.dhcpact);
function DHCPAct(F)
{
       F.submit_button.value="DHCPTable";
       F.submit_type.value="delete";
       F.change_action.value="gozila_cgi";
       F.submit();
}
function refresh_win()
{
	window.location.href="DHCPTable.asp";
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
    <TD width=473 colSpan=2 height=27><FONT face=Arial color=blue size="4"><B><script>Capture(stadhcptbl.dhcpactiveiptbl)</script></B></FONT></TD>
<!--    <TD align=right width=147 height=27>&nbsp;</TD>
    <TD align=right width=73 height=27>&nbsp;</TD>-->
    <TD align=right width=73 height=27>&nbsp;</TD></TR>
  <TR>
    <TD align=left colSpan=4 height=30><FONT face=Arial size=2><script>Capture(stadhcptbl.dhcpsrvipaddr)</script>:
	<B>&nbsp; <% nvram_get("lan_ipaddr"); %> </B></FONT></TD>
    <TD align=center height=30><!--<INPUT onclick=refresh_win() type=button value=" Refresh ">--><script>document.write("<INPUT onclick=refresh_win() type=button value=\"" + share.refreshs + "\">");</script></TD></TR>
  <TR align=middle bgColor=#b3b3b3>
    <TH height=32><FONT color=black><script>Capture(stadhcptbl.clienthostname)</script></FONT></TH>
    <TH height=32><FONT color=black><script>Capture(stadhcptbl.ipaddr)</script></FONT></TH>
    <TH height=32><FONT color=black><script>Capture(stadhcptbl.macaddr)</script></FONT></TH>
    <TH height=32><FONT color=black><script>Capture(stadhcptbl.expires)</script></FONT></TH>
    <TD height=32>
      <script>document.write("<input type=button name=action value=\"" + stadhcptbl.del + "\" onClick=DHCPAct(this.form)>");</script></TH>
<script language=javascript>
var table = new Array(
<% dumpleases(""); %>
);
var i = 0;
var count = 0;
var len = table.length;
var j = 0, k = 0, tablecount = 0, minvalue = 0, minindex = 0;
var indextable = new Array(len/5);
var tmptable = new Array(len/5);

for(;;)
{
	if(!table[j])
		break;
	tablecount++;
	tmptable[j+4] = table[j+4];
	j+=5;
}
j = 0;
minvalue = tmptable[j+4];
for(;;)
{
	if(!table[j])
	{
		if(j == 0)
		{
			indextable[k] = -1;
			break;
		}
		indextable[k++] = minindex;
		tmptable[minindex+4] = 257;
		tablecount--;
		minindex = 0;
		j = 0;
		minvalue = tmptable[j+4];
		continue;
	}
	else
	{
		if(tablecount == 0)
		{
			indextable[k] = -1;
			break;
		}
		if(tmptable[j+4])
		{
			if(parseInt(tmptable[j+4]) < minvalue)
			{
				minvalue = tmptable[j+4];
				minindex = j;
			}
		}
		j+=5;
	}
}

for(;;)
{
	if(indextable[i] == -1){
		if(i == 0){
			document.write("<tr bgcolor=cccccc align=middle>");
			document.write("<td>" + accinetacc.none +"</td>");
			document.write("<td>" + accinetacc.none +"</td>");
			document.write("<td>" + accinetacc.none +"</td>");
			document.write("<td>" + accinetacc.none +"</td>");
			document.write("<td>&nbsp;</td></tr>");
		}
		break;
	}
	k=indextable[i];
	document.write("<tr bgcolor=cccccc align=middle>");
	document.write("<td>"+table[k]+"</td>");
	document.write("<td>"+table[k+1]+"</td>");
	document.write("<td>"+table[k+2]+"</td>");
	document.write("<td>"+table[k+3]+"</td>");
	document.write("<td><input type=checkbox name=d_"+count+" value="+table[k+4]+"></td></tr>");
	count ++;
	i++;
}
</script>
  <TR align=middle bgColor=#cccccc>
    <TD bgcolor="#FFFFFF" height="35">&nbsp;</TD>
    <TD bgcolor="#FFFFFF" height="35">&nbsp;</TD>
    <TD bgcolor="#FFFFFF" height="35">&nbsp;</TD>
    <TD bgcolor="#FFFFFF" height="35">&nbsp;</TD>
    <TD bgcolor="#FFFFFF" height="35"> 
    <script>document.write("<INPUT onclick=self.close() type=reset value=\"" + stadhcptbl.closes + "\">");</script></TD></TR>
</TBODY>
</TABLE>
</CENTER>
</FORM>
</BODY>
</HTML>
