
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

<HTML><HEAD><TITLE>MAC Address Access List</TITLE>
<% no_cache(); %>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<SCRIPT src="common.js"></SCRIPT>
<script language="JavaScript" type="text/javascript" src="share.js"></script>
<script language="JavaScript" type="text/javascript" src="capsetup.js"></script>
<script language="JavaScript" type="text/javascript" src="capwireless.js"></script>
<SCRIPT language=javascript>
document.title=(wlnetwork.maclist);
var active_win = null;
function ViewActive()
{
	active_win = self.open('Wireless_Connected.asp','ActiveTable','alwaysRaised,resizable,scrollbars,width=650,height=450');
	active_win.focus();
}
function to_submit(F)
{
        F.submit_button.value = "WL_FilterTable";
        F.action.value = "Apply";
        F.submit();
}
function SelPage(num,F)
{
	F.submit_button.value = "WL_FilterTable";
	F.change_action.value = "gozila_cgi";
	F.wl_filter_page.value=F.wl_filter_page.options[num].value;
	F.submit();
}
function valid_macs_all(I)
{
	if(I.value == "")
		return true;
	else if(I.value.length == 12)
		valid_macs_12(I);
	else if(I.value.length == 17)
		valid_macs_17(I);
	else{
		//alert('The MAC Address length is not correct!!');
		alert(errmsg.err31);

		I.value = I.defaultValue;
        }
	
}
function exit()
{
	closeWin(active_win);
}
</SCRIPT>

</HEAD>
<BODY bgColor=white onload={window.focus();} onunload=exit()>
<FORM name=macfilter method=<% get_http_method(); %> action=apply.cgi>
<input type=hidden name=submit_button>
<input type=hidden name=submit_type>
<input type=hidden name=change_action>
<input type=hidden name=action>
<input type=hidden name=wl_mac_list>

<CENTER>
<TABLE cellSpacing=0 cellPadding=10 width=436 border=1>
  <TBODY>
  <TR>
    <TD width=412>
      <TABLE height=320 cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD colSpan=5 height=17 align="center">
            <P align=center><B><FONT face=Arial color=#0000ff size="4"><script>Capture(wlother.maclist)</script></FONT></B></P></TD></TR>
        <TR>
          <TD colSpan=5 height=40 align="center">
            <FONT face=Arial size=2><script>Capture(wlother.enter)</script>: xxxxxxxxxxxx/xx:xx:xx:xx:xx:xx</FONT></TD></TR>
	<% wireless_filter_table("input"); %>
        <TR>
          <TD colSpan=5 height=40 align="center" valign="top">
            <p style="margin-top: 5; margin-bottom: 5"><font face="Arial"> 
<!--<INPUT id=button4 onclick="ViewActive()" type=button value="Wireless Client MAC List" name=button5>--><script>document.write("<INPUT id=button4 onclick=ViewActive() type=button value=\"" + wlother.wlcmlist + "\"  name=button5>");</script></font></p>
            </TD></TR>
          </TBODY></TABLE></TD></TR></TBODY></TABLE>
<P><!--<INPUT type=button value="Save Settings" onClick=to_submit(this.form)>-->
<script>document.write("<INPUT type=button value=\"" + share.saves + "\" onClick=to_submit(this.form)>");</script>&nbsp;
<!-- <INPUT type=reset value="Cancel Changes" name=cancel onClick=window.location.reload()>-->
<script>document.write("<INPUT type=reset value=\"" + share.cancels + "\" name=cancel onClick=window.location.reload()>");</script>&nbsp;
<script>document.write("<INPUT type=button value=\"" + share.closes + "\" onClick=self.close()>");</script>

</CENTER></P></FORM></BODY></HTML>
