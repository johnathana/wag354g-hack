
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

<HTML><HEAD><TITLE>Ping Test</TITLE>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<script src="common.js"></script>
<SCRIPT language="Javascript" type="text/javascript" src="share.js"></SCRIPT>
<SCRIPT language="Javascript" type="text/javascript" src="capadmin.js"></SCRIPT>
<SCRIPT language=javascript>
document.title=(admleftmenu.ping);
function to_submit(F,I)
{
	if(valid(F,I)){
		F.submit_type.value = I;
		F.submit_button.value = "Ping";
		F.change_action.value = "gozila_cgi";
		F.submit();
	}
}
function valid(F,I)
{
	if(I == "start" && F.ping_ip.value == ""){
		//alert("You must input an IP Address or Domain Name!");
		alert(errmsg.err86);
		F.ping_ip.focus();
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
		window.location.replace("Ping.asp");
	}
	value++;
	timerID=setTimeout("Refresh()",refresh_time);
}
function init()
{
	window.location.href = "#";
	<% onload("Ping", "Refresh();"); %>
}
function exit()
{
	//if(!confirm("Do you want to stop ping?"))
	//	self.close();
	//else{
		to_submit(document.ping,"stop");
		self.close();
	//}
}
</SCRIPT>
</HEAD>
<BODY text=#000000 bgColor=#ffffff onload="init()">
<FORM name=ping method=<% get_http_method(); %> action=apply.cgi>
<input type=hidden name=submit_button value="Ping">
<input type=hidden name=submit_type value="start">
<input type=hidden name=action value="Apply">
<input type=hidden name=change_action value="gozila_cgi">
<div align="center">
  <center>
  <table border="0" cellpadding="0" cellspacing="0" style="border-collapse: collapse; border-width: 0" bordercolor="#111111" id="AutoNumber1" width="458" height="322">
    <tr>
      <td style="border-style: none; border-width: medium" width="194" height="19">
      <b><font face="Arial" size="4" color="#0000FF"><script>Capture(admping.pingtest)</script></font></b></td>
      <td style="border-style: none; border-width: medium" width="224" height="19">&nbsp;</td>
      <td style="border-style: none; border-width: medium" height="19" align="right" width="41">&nbsp;</td>
    </tr>
    <tr>
      <td style="border-style: none; border-width: medium" width="194" height="39">
      <b><FONT face=Arial size=2><script>Capture(admping.ipaddr_domainname)</script>:</FONT></b></td>
      <td style="border-style: none; border-width: medium" width="224" height="39" align="left"> 
    <INPUT maxLength=31 name=ping_ip size="31" value="<% nvram_selget("ping_ip"); %>" onBlur=valid_name(this,"IP")></td>
      <td style="border-style: none; border-width: medium" height="39" align="right" width="41">
      <INPUT onclick="to_submit(this.form,'start');" type=button value=Ping name=ping></td>
    </tr>
    <tr>
      <td style="border-style: none; border-width: medium" width="194" height="22">
      <b>
    <font face="Arial" size="2"><script>Capture(admping.numbers)</script>: </font> <FONT face=Arial size=1>
    &nbsp;&nbsp; </FONT> </b></td>
      <td style="border-style: none; border-width: medium" width="224" height="22"> 
    <B> <FONT face=Arial size=1>
    <SELECT name=ping_times> 
        <OPTION value=5 <% nvram_selmatch("ping_times", "5", "selected"); %>>5</OPTION> 
        <OPTION value=10 <% nvram_selmatch("ping_times", "10", "selected"); %>>10</OPTION>
        <option value=0 <% nvram_selmatch("ping_times", "0", "selected"); %>><script>Capture(admping.unlimited)</script></option>
    </SELECT></FONT></B></td>
      <td style="border-style: none; border-width: medium" height="22" align="right" width="41"> &nbsp;</td>
    </tr>
    <tr>
      <td style="border-style: none; border-width: medium" width="194" height="18">
      </td>
      <td style="border-style: none; border-width: medium" width="224" height="18"> 
      </td>
      <td style="border-style: none; border-width: medium" height="18" align="right" width="41">
      </td>
    </tr>
    <tr>
      <td style="border-style: double; border-width: 3" width="453" colspan="3" height="293" bordercolor="#808080" valign=top>
<script language=javascript>
var table = new Array(
<% dump_ping_log(""); %>
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
      <td style="border-style: none; border-width: medium" height="45" colspan="3" align="right" width="459">
      <p align="right">&nbsp; 
    <script>document.write("<INPUT onclick=\"to_submit(this.form,'stop');\" type=button value=\"" + admbutton.stop + "\">");</script>&nbsp;
      <script>document.write("<INPUT  onclick=\"to_submit(this.form,'clear');\" type=button value=\"" + admbutton.clearlog +"\">");</script>&nbsp; 
    <script>document.write("<INPUT onclick=\"exit()\" type=button value=\" + admbutton.close +"\">");</script></td>
<a name="#"></a>
    </tr>
  </table>
  </center>
</div>
<CENTER>
<p><WINDWEB_URL>
</p>
</CENTER></BODY></HTML>
