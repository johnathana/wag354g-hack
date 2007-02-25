
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

<html><title>Log</title>
<meta http-equiv=refresh content="300; url=Log_all.asp">
<script src="common.js"></script>
<SCRIPT language=JavaScript>
function SelType(num,F)
{
        F.submit_button.value = "Log_all";
        F.change_action.value = "gozila_cgi";
        F.log_show_type.value=F.log_show_type.options[num].value;
        F.submit();
}
</SCRIPT>
<body bgcolor=white onload={window.location.href="#"}>
<form name=log method=<% get_http_method(); %> action=apply.cgi>
<input type=hidden name=submit_button>
<input type=hidden name=change_action>
<input type=hidden name=submit_type>
<input type=hidden name=action>
<center>
<table width=100% border=0 cellspacing=1>
<tr><td><font face=Arial color=blue><b>Log</td>
    <td align=right><select name="log_show_type" onChange=SelType(this.form.log_show_type.selectedIndex,this.form)>
<script language=JavaScript>
var table = new Array(
<% dump_log_settings("log_show_type"); %>
,'ALL','ALL','0'
);
var i=0;
for(;;){
  if(table[i]){
	var log_show_type = '<% nvram_get("log_show_type"); %>';
	var select = "";

	if(log_show_type == table[i])
		select = " selected";

	document.write("<option value="+table[i]+select+">"+table[i+1]+"</option>");
  	i = i + 3;
  }
  else{
	break;
  }
}
</script>
	</select>
</td>
<tr><td>&nbsp;</td>
    <td align=right><input type=button value=' Refresh ' onClick=window.location.replace("Log_all.asp")></td></tr>
</table>
<table width=100% border=1 cellspacing=1>
<tr>
<td>
<% dump_syslog("all"); %>
<br></td></tr></table>
<a name="#"></a>
<table width=100% border=0 cellspacing=1>
<tr><td align=center>The page will refresh automatically after 60 seconds.<td align=right><input type=button value=' Refresh ' onClick=window.location.replace("Log_all.asp")>
</td></tr></table></center></form></body></html>
