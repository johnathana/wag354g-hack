
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

<HTML><HEAD><TITLE>Wireless Active Client MAC List</TITLE>
<% no_cache(); %>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<SCRIPT src="common.js"></SCRIPT>
<script language="JavaScript" type="text/javascript" src="share.js"></script>
<script language="JavaScript" type="text/javascript" src="capsetup.js"></script>
<script language="JavaScript" type="text/javascript" src="capwireless.js"></script>
<SCRIPT language=javascript>
document.title=(wlother.wlcmlist);
function MACAct(F)
{
	if(valid_value(F)){
		F.submit_button.value="Wireless_Connected";
		F.change_action.value="gozila_cgi";
		F.submit_type.value="add_mac";
		F.submit();
	}
}
function valid_value(F)
{	
	var num = F.elements.length;
	var count = 0;

	for(i=0;i<num;i++){
		if(F.elements[i].type == "checkbox"){
			if(F.elements[i].checked == true)
				count = count + 1;
		}
	}
	if(count > 40){
		//alert("The total checks exceed 40 counts!");
		alert(errmsg.err32);
		return false;
	}
	return true;
}
function init()
{
	<% onload("Wireless_Connected", "setTimeout('opener.window.location.reload()',500)"); %>
	window.focus();
}
</SCRIPT>
</HEAD>
<BODY bgColor=white onload=init()>
<form method=<% get_http_method(); %> action=apply.cgi>
<input type=hidden name=submit_button>
<input type=hidden name=submit_type>
<input type=hidden name=change_action>
<CENTER>
<TABLE height=9 cellSpacing=1 width=606 border=0>
  <TBODY>
  <TR>
    <TD width=310 colSpan=2 height=30><b>
    <font face="Arial" color="#0000FF" size="4">
    <script>Capture(wlother.wlcmlist)</script></font></b></TD>
    <TD align=right width=156 height=30>&nbsp;</TD>
    <TD align=center width=141 height=30><!--<INPUT onclick=window.location.reload() type=button value=" Refresh ">--><script>document.write("<INPUT onclick=window.location.reload() type=button value=\"" + share.refreshs + "\">");</script></TD></TR>
 <TR align=middle bgColor=#b3b3b3>
    <TH height=30 width="167"><FONT face=Arial size=2><script>Capture(wlother.computername)</script></FONT></TH>
    <TH height=30 width="140"><FONT face=Arial size=2><script>Capture(share.ipaddr)</script></FONT></TH>
    <TH height=30 width="156"><FONT face=Arial size=2><script>Capture(share.macaddr)</script></FONT></TH>
    <TH height=30 width="141"><FONT face="Arial" size="2"><script>Capture(wlother.macfilter)</script></FONT></TH></TR>
<% wireless_connected_table("online"); %>
  <TR align=middle bgColor=#cccccc>
    <TD height="24" width="167" bgcolor="#FFFFFF">&nbsp;</TD>
    <TD height="24" width="140" bgcolor="#FFFFFF">&nbsp;</TD>
    <TD height="33" width="297" bgcolor="#FFFFFF" colspan="2" align="right" valign="bottom"><font face=verdana size=2>
    <!-- <input type=button name=action value='Update Filter List' onClick=MACAct(this.form)>--><script>document.write("<input type=button name=action value='" + wlother.updatelist + "' onClick=MACAct(this.form)>");</script>&nbsp;
    </font> 
   <!-- <INPUT onclick=self.close() type=reset value=Close>--><script>document.write("<INPUT onclick=self.close() type=reset value='" + share.closes + "'>");</script></TD>
    </TR></TBODY></TABLE></CENTER>
</BODY></HTML>
