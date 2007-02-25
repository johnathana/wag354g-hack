
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

<HTML><HEAD><TITLE>Traceroute Test</TITLE>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<script src="common.js"></script>
<script language="JavaScript" type="text/javascript" src="share.js"></script>
<SCRIPT language=javascript>
document.title=(share.tracetest);
function to_submit(F,I)
{
	if(valid(F,I)){
		F.submit_type.value = I;
		F.submit_button.value = "Traceroute";
		F.change_action.value = "gozila_cgi";
		F.submit();
	}
}
function valid(F,I)
{
	if(I == "start" && F.traceroute_ip.value == ""){
		//alert("You must input an IP Address or Domain Name!");
		alert(errmsg.err86);

		F.traceroute_ip.focus();
		return false;
	}
	return true;
}
var value=0;
function Refresh()
{
	refresh_time = 5000;
	if (value>=1)
	{
		window.location.replace("Traceroute.asp");
	}
	value++;
	timerID=setTimeout("Refresh()",refresh_time);
}
function init()
{
	window.location.href = "#";
	<% onload("Traceroute", "Refresh();"); %>
}
function exit()
{
	//if(!confirm("Do you want to stop traceroute?"))
	//	self.close();
	//else{
		to_submit(document.traceroute,"stop");
		self.close();
	//}
}
</SCRIPT>
</HEAD>
<BODY text=#000000 bgColor=#ffffff onload="init();">
<FORM name=traceroute method=<% get_http_method(); %> action=apply.cgi>
<input type=hidden name=submit_button value="Traceroute">
<input type=hidden name=submit_type value="start">
<input type=hidden name=action value="Apply">
<input type=hidden name=change_action value="gozila_cgi">
<div align="center">
  <center>
  <table border="0" cellpadding="0" cellspacing="0" style="border-collapse: collapse; border-width: 0" bordercolor="#111111" id="AutoNumber1" width="525" height="360">
    <tr>
      <td style="border-style: none; border-width: medium" width="188" height="19">
      <b><font face="Arial" size="4" color="#0000FF"><script>Capture(share.tracetest)</script></font></b></td>
      <td style="border-style: none; border-width: medium" width="231" height="19">&nbsp;</td>
      <td style="border-style: none; border-width: medium" height="19" align="right" width="106">&nbsp;</td>
    </tr>
    <tr>
      <td style="border-style: none; border-width: medium" width="188" height="39">
      <b><FONT face=Arial size=2><script>Capture(share.ipaddr)</script> or <script>Capture(share.doname)</script>:</FONT></b></td>
      <td style="border-style: none; border-width: medium" width="231" height="39"> 
    <INPUT maxLength=31 size=31 name=traceroute_ip value="<% nvram_selget("traceroute_ip"); %>" onBlur=valid_name(this,"IP")></td>
      <td style="border-style: none; border-width: medium" height="39" align="right" width="106">
      <INPUT onclick="to_submit(this.form,'start');" type=button value=Traceroute name=Traceroute></td>
    </tr>
    <tr>
      <td style="border-style: double; border-width: 3" width="519" colspan="3" height="293" bordercolor="#808080" valign=top>
<script language=javascript>
var table = new Array(
<% dump_traceroute_log(""); %>
);
   var i = 0;
   for(;;){
	if(!table[i])	break;
        document.write(table[i]+"<br>");
        i = i + 1;
   }

</script>

</td>
    </tr>
    <tr>
      <td style="border-style: none; border-width: medium" height="45" colspan="3" align="right" width="525">
      <p align="right">&nbsp; 
    <INPUT onclick="to_submit(this.form,'stop');" type=button value="Stop">&nbsp;
    <INPUT onclick="to_submit(this.form,'clear');" type=button value="Clear Log">&nbsp; 
    <INPUT onclick="exit()" type=button value="Close"></td>
    <a name="#"></a>
    </tr>
  </table>
  </center>
</div>
<CENTER>
<p><WINDWEB_URL>
</p>
</CENTER></BODY></HTML>
