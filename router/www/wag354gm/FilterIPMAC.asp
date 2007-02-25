
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

<HTML><HEAD><TITLE>List of PCs</TITLE>
<% no_cache(); %>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<link rel="stylesheet" type="text/css" href="style.css">
<SCRIPT src="common.js"></SCRIPT>
<SCRIPT language="Javascript" type="text/javascript" src="share.js"></SCRIPT>
<SCRIPT language="Javascript" type="text/javascript" src="capaccess.js"></SCRIPT>
<SCRIPT language=javascript>
document.title=(acclistpc.listpc);
function to_submit(F)
{
        F.submit_button.value = "FilterIPMAC";
        F.action.value = "Apply";
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
</SCRIPT>
</HEAD>

<BODY bgColor=white>
<FORM name=ipfilter action=apply.cgi method=<% get_http_method(); %>>
<input type=hidden name=submit_button>
<input type=hidden name=change_action>
<input type=hidden name=small_screen>
<input type=hidden name=action>
<input type=hidden name=filter_ip_value>
<input type=hidden name=filter_mac_value>
<CENTER>
<TABLE cellSpacing=0 cellPadding=10 width=533 border=1 height="397">
  <TBODY>
  <TR>
    <TD width=516>
      <TABLE cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD height=35>
            <P align=center><b><FONT face=Arial color=#0000ff size="4"><script>Capture(acclistpc.listpc)</script></FONT></b></P></TD></TR>
        <TR>
          <TD align=right height=27>
            <p align="center">
            <FONT face=Arial 
            size=2><script>Capture(acclistpc.entermac)</script></FONT></TD>
          </TR>
        <TR>
          <TD align=center height=27>
            <b><font face="Arial" size="-1"><script>Capture(acclistpc.mac01)</script>:</B></FONT><font face="Arial">&nbsp; 
            <INPUT maxLength=17 onBlur=valid_macs_all(this) size=17 name=mac0 value='<% filter_mac_get(0); %>' class=num></font>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
            <b><font face="Arial" size="-1"><script>Capture(acclistpc.mac05)</script>:</B></FONT><font face="Arial">&nbsp; 
            <INPUT maxLength=17 onBlur=valid_macs_all(this) size=17 name=mac4 value='<% filter_mac_get(4); %>' class=num></font></TD>
          </TR>
        <TR>
          <TD align=center height=27>
            <b><font face="Arial" size="-1"><script>Capture(acclistpc.mac02)</script>:</B></FONT><font face="Arial">&nbsp; 
            <INPUT maxLength=17 onBlur=valid_macs_all(this) size=17 name=mac1 value='<% filter_mac_get(1); %>' class=num></font>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
            <b><font face="Arial" size="-1"><script>Capture(acclistpc.mac06)</script>:</B></FONT><font face="Arial">&nbsp; 
            <INPUT maxLength=17 onBlur=valid_macs_all(this) size=17 name=mac5 value='<% filter_mac_get(5); %>' class=num></font></TD>
          </TR>
        <TR>
          <TD align=center height=27>
            <b><font face="Arial" size="-1"><script>Capture(acclistpc.mac03)</script>:</B></FONT><font face="Arial">&nbsp; 
            <INPUT maxLength=17 onBlur=valid_macs_all(this) size=17 name=mac2 value='<% filter_mac_get(2); %>' class=num></font>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
            <b><font face="Arial" size="-1"><script>Capture(acclistpc.mac07)</script>:</B></FONT><font face="Arial">&nbsp; 
            <INPUT maxLength=17 onBlur=valid_macs_all(this) size=17 name=mac6 value='<% filter_mac_get(6); %>' class=num></font></TD>
          </TR>
        <TR>
          <TD align=center height=27>
            <b><font face="Arial" size="-1"><script>Capture(acclistpc.mac04)</script>:</B></FONT><font face="Arial">&nbsp; 
            <INPUT maxLength=17 onBlur=valid_macs_all(this) size=17 name=mac3 value='<% filter_mac_get(3); %>' class=num></font>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
            <b><font face="Arial" size="-1"><script>Capture(acclistpc.mac08)</script>:</B></FONT><font face="Arial">&nbsp; 
            <INPUT maxLength=17 onBlur=valid_macs_all(this) size=17 name=mac7 value='<% filter_mac_get(7); %>' class=num></font></TD>
          </TR>
        <TR>
          <TD align=right> <HR color=#c0c0c0 align="right"> </TD>
          </TR>
        <TR>
          <TD align=right height=27>
            <p align="center"><font size="2" face="Arial"><script>Capture(acclistpc.enteripaddr)</script></font></TD>
          </TR>
        <TR>
          <TD align=center height=27>
            <FONT face=Arial size=-1><B><script>Capture(acclistpc.ip01)</script>:</B></FONT><font face="Arial"><font size="2">&nbsp; 
            <% prefix_ip_get("lan_ipaddr",1); %> </font> 
            <INPUT maxLength=3  onBlur=valid_range(this,0,254,"IP") size=3 name=ip0 value='<% filter_ip_get("ip",0); %>' class=num></font>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
            <FONT face=Arial size=-1><B><script>Capture(acclistpc.ip04)</script>:</B></FONT><font face="Arial"><font size="2">&nbsp; 
            <% prefix_ip_get("lan_ipaddr",1); %> </font> 
            <INPUT maxLength=3  onBlur=valid_range(this,0,254,"IP") size=3 name=ip3 value='<% filter_ip_get("ip",3); %>' class=num></font></TD>
          </TR>
        <TR>
          <TD align=center height=27>
            <FONT face=Arial size=-1><B><script>Capture(acclistpc.ip02)</script>:</B></FONT><font face="Arial"><font size="2">&nbsp; 
            <% prefix_ip_get("lan_ipaddr",1); %> </font> 
            <INPUT maxLength=3  onBlur=valid_range(this,0,254,"IP") size=3 name=ip1 value='<% filter_ip_get("ip",1); %>' class=num></font>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
            <FONT face=Arial size=-1><B><script>Capture(acclistpc.ip05)</script>:</B></FONT><font face="Arial"><font size="2">&nbsp; 
            <% prefix_ip_get("lan_ipaddr",1); %> </font> 
            <INPUT maxLength=3  onBlur=valid_range(this,0,254,"IP") size=3 name=ip4 value='<% filter_ip_get("ip",4); %>' class=num></font></TD>
          </TR>
        <TR>
          <TD align=center height=27>
            <FONT face=Arial size=-1><B><script>Capture(acclistpc.ip03)</script>:</B></FONT><font face="Arial"><font size="2">&nbsp; 
            <% prefix_ip_get("lan_ipaddr",1); %> </font> 
            <INPUT maxLength=3  onBlur=valid_range(this,0,254,"IP") size=3 name=ip2 value='<% filter_ip_get("ip",2); %>' class=num></font>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
            <FONT face=Arial size=-1><B><script>Capture(acclistpc.ip06)</script>:</B></FONT><font face="Arial"><font size="2">&nbsp; 
            <% prefix_ip_get("lan_ipaddr",1); %> </font> 
            <INPUT maxLength=3  onBlur=valid_range(this,0,254,"IP") size=3 name=ip5 value='<% filter_ip_get("ip",5); %>' class=num></font></TD>
          </TR>
        <TR>
          <TD align=right>
      <HR color=#c0c0c0 align="right">
          </TD>
          </TR>
        <TR>
          <TD align=right height=27>
            <p align="center"><font size="2" face="Arial"><script>Capture(acclistpc.enteriprange)</script></font></TD>
          </TR>
        <TR>
          <TD align=right height=27>
            <p align="left">
            <FONT face=Arial size=-1><B><script>Capture(acclistpc.iprange01)</script>:&nbsp; </B></FONT>
            <font face="Arial"><font size="2"><% prefix_ip_get("lan_ipaddr",1); %> </font> 
            <INPUT maxLength=3 onBlur=valid_range(this,0,254,"IP") size=3 name=ip_range0_0 value='<% filter_ip_get("ip_range0_0",6); %>' class=num><b><font size="2"> ~ </font> </b> 
            <INPUT maxLength=3 onBlur=valid_range(this,0,254,"IP") size=3 name=ip_range0_1 value='<% filter_ip_get("ip_range0_1",6); %>' class=num><font size="2">&nbsp; </font> </font>

            <FONT face=Arial size=-1><B><script>Capture(acclistpc.iprange02)</script>:&nbsp; </B></FONT>
            <font face="Arial"><font size="2"><% prefix_ip_get("lan_ipaddr",1); %> </font> 
            <INPUT maxLength=3 onBlur=valid_range(this,0,254,"IP") size=3 name=ip_range1_0 value='<% filter_ip_get("ip_range1_0",7); %>' class=num><b><font size="2"> ~ </font> </b> 
            <INPUT maxLength=3 onBlur=valid_range(this,0,254,"IP") size=3 name=ip_range1_1 value='<% filter_ip_get("ip_range1_1",7); %>' class=num><font size="2">&nbsp; </font> </font>
          </TD>
          </TR>
        </TBODY></TABLE></TD></TR></TBODY></TABLE>
<P><!--<INPUT type=button value="Save Settings" onClick=to_submit(document.ipfilter)>-->
<script>document.write("<INPUT type=button value=\"" + share.saves + "\" onClick=to_submit(document.ipfilter)>");</script>&nbsp;
<!-- <INPUT type=button value="Cancel Changes" onClick="javascript:window.location.href='FilterIPMAC.asp'">-->
<script>document.write("<INPUT type=button value=\"" + share.cancels + "\" onClick=\"javascript:window.location.href='FilterIPMAC.asp'\">");</script>&nbsp;
<script>document.write("<INPUT type=button value=\"" + share.closes + "\" onClick=self.close()>");</script>
</CENTER></P></FORM></BODY></HTML>
