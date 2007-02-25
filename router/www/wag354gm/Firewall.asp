
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

<HTML><HEAD><TITLE>Firewall</TITLE>
<% no_cache(); %>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<link rel="stylesheet" type="text/css" href="style.css">
<style fprolloverstyle>
A:hover {color: #00FFFF}
.small A:hover {color: #00FFFF}
</style>
<SCRIPT src="common.js"></SCRIPT>
<script language="JavaScript" type="text/javascript" src="share.js"></script>
<script language="JavaScript" type="text/javascript" src="capsec.js"></script>
<SCRIPT language=JavaScript>
document.title=(sectopmenu.firewall);
<% wireless_support(5); %>share.productname = share.productname1;<% wireless_support(6); %>
function ViewLogIn()
{
//	self.open('firewall_log.asp','firewall_log','alwaysRaised,resizable,scrollbars,width=580,height=600').focus();
	self.open('firewall_log.asp','firewall_log','alwaysRaised,resizable,scrollbars,width=780,height=600').focus();

}



function to_submit(F)

{
	F.submit_button.value = "Firewall";
	if(F._block_wan.checked == true) F.block_wan.value = 1;
	else  F.block_wan.value = 0;

	if(F._block_java.checked == true) F.block_java.value = 1;
	else  F.block_java.value = 0;

	if(F._block_activex.checked == true) F.block_activex.value = 1;
	else  F.block_activex.value = 0;

	if(F._block_cookie.checked == true) F.block_cookie.value = 1;
	else  F.block_cookie.value = 0;

	if(F._block_proxy.checked == true) F.block_proxy.value = 1;
	else  F.block_proxy.value = 0;

	F.action.value = "Apply";
        F.submit();
}

function fw_enable_disable(F)
{
	if(F.filter[1].checked == true)
	{
		F.filter_level[0].disabled = true;
		F.filter_level[1].disabled = true;
		F.filter_level[2].disabled = true;
		F.filter_level[3].disabled = true;
	}
	else {
		F.filter_level[0].disabled = false;
		F.filter_level[1].disabled = false;
		F.filter_level[2].disabled = false;
		F.filter_level[3].disabled = false;
	}
}
function init()
{
	fw_enable_disable(document.firewall);
}

</SCRIPT>
</HEAD>
<BODY <% support_match("FIREWALL_LEVEL_SUPPORT", "1", "onload=init()"); %>>
<DIV align=center>
<FORM name=firewall method=<% get_http_method(); %> action=apply.cgi>
<input type=hidden name=submit_button>
<input type=hidden name=change_action>
<input type=hidden name=action>
<input type=hidden name=block_wan>
<input type=hidden name=block_java>
<input type=hidden name=block_proxy>
<input type=hidden name=block_activex>
<input type=hidden name=block_cookie>

<TABLE cellSpacing=0 cellPadding=0 width=809 border=0>
  <TBODY>
  <TR>
    <TD width=95><IMG src="image/UI_Linksys.gif" border=0 width="165" height="57"></TD>
    <TD vAlign=bottom align=right width=714 bgColor=#6666cc><FONT style="FONT-SIZE: 7pt" face=Arial color=#ffffff><script>Capture(share.firmwarever)</script>:&nbsp;<% get_firmware_version(); %>&nbsp;&nbsp;&nbsp;</FONT></FONT></TD></TR>
  <TR>
    <TD width=808 bgColor=#6666cc colSpan=2><IMG height=11 src="image/UI_10.gif" width=809 border=0></TD></TR></TBODY></TABLE>
<TABLE height=77 cellSpacing=0 cellPadding=0 width=809 bgColor=black border=0>
  <TBODY>
  <TR>
    <TD style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" borderColor=#000000 align=middle width=163 height=49>
      <H3 style="MARGIN-TOP: 1px; MARGIN-BOTTOM: 1px"><FONT style="FONT-SIZE: 15pt" face=Arial color=#ffffff><script>Capture(bmenu.security)</script></FONT></H3></TD>
    <TD style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" vAlign=center borderColor=#000000 width=646 bgColor=#000000 height=49>
      <TABLE style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; BORDER-COLLAPSE: collapse; FONT-VARIANT: normal" height=33 cellSpacing=0 cellPadding=0 bgColor=#6666cc border=0>
        <TBODY>
        <TR>
          <TD style="FONT-WEIGHT: bolder; FONT-SIZE: 10pt" align=right bgColor=#6666cc height=33><FONT color=#ffffff><% wireless_support(1); %><script>Capture(share.wireless_g)</script><% wireless_support(2); %><script>Capture(share.productname)</script>&nbsp;&nbsp;</FONT>
          <TD borderColor=#000000 borderColorLight=#000000 align=middle width=109 bgColor=#000000 borderColorDark=#000000 height=12 rowSpan=2><FONT color=#ffffff><SPAN style="FONT-SIZE: 8pt"><B><% nvram_get("router_name"); %></B></SPAN></FONT></TD></TR>
        <TR>
          <TD style="FONT-WEIGHT: normal; FONT-SIZE: 1pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" width=537 bgColor=#000000 height=1>&nbsp;</TD></TR>
        <TR>
          <TD width=646 bgColor=#000000 colSpan=2 height=32>
            <TABLE id=AutoNumber1 style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; BORDER-COLLAPSE: collapse; FONT-VARIANT: normal" height=6 cellSpacing=0 cellPadding=0 width=637 border=0>
              <TBODY>
              <TR style="BORDER-RIGHT: medium none; BORDER-TOP: medium none; FONT-WEIGHT: normal; FONT-SIZE: 1pt; BORDER-LEFT: medium none; COLOR: black; BORDER-BOTTOM: medium none; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" align=middle bgColor=#6666cc>
                <TD width=83  height=1><IMG height=10 src="image/UI_06.gif" width=83  border=0></TD>
                <TD width=83  height=1><IMG height=10 src="<% wireless_support(3); %>" width=83  border=0></TD>
                <TD width=103 height=1><IMG height=10 src="<% wireless_support(4); %>" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=68  height=1><IMG height=10 src="image/UI_06.gif" width=68  border=0></TD></TR>
              <TR>
                <TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><A style="TEXT-DECORATION: none" href="index.asp"><script>Capture(bmenu.setup)</script></A></FONT></TD><% wireless_support(1); %>
		<TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><A style="TEXT-DECORATION: none" href="Wireless_Basic.asp"><script>Capture(bmenu.wireless)</script></A></FONT></TD><% wireless_support(2); %>
                <TD align=middle bgColor=#6666cc height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><A style="TEXT-DECORATION: none" href="Firewall.asp"><script>Capture(bmenu.security)</script></A></FONT></TD>
                <TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><A style="TEXT-DECORATION: none" href="<% support_elsematch("PARENTAL_CONTROL_SUPPORT", "1", "Parental_Control.asp", "Filters.asp"); %>"><script>Capture(bmenu.accrestriction)</script></A></FONT></TD>
                <TD align=middle bgColor=#000000 height=20><P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" color=#ffffff><A style="TEXT-DECORATION: none" href="Forward_UPnP.asp"><script>Capture(bmenu.application)</script><BR>&amp; <script>Capture(bmenu.gaming)</script></A>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>
                <TD align=middle bgColor=#000000 height=20><P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" color=#ffffff><A style="TEXT-DECORATION: none" href="Management.asp">&nbsp;&nbsp;<script>Capture(bmenu.admin)</script></A>&nbsp;&nbsp;</FONT></P></TD>
                <TD align=middle bgColor=#000000 height=20><P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" color=#ffffff><A style="TEXT-DECORATION: none" href="Status_Router.asp"><script>Capture(bmenu.statu)</script></A>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD></TR>
              <TR>
                <TD width=643 bgColor=#6666cc colSpan=7 height=21>
                  <TABLE borderColor=black height=21 cellSpacing=0 cellPadding=0>
                    <TBODY>
                    <TR align=center>
                      <TD width=20></TD>
                      <% support_invmatch("VPN_SUPPORT", "1", "<!--"); %><TD width=61 class=small><FONT style="COLOR: white"><script>Capture(sectopmenu.firewall)</script></FONT></TD>
                      <TD width=4><P class=bar><font color='white'><b>|</b></font></P></TD><% support_invmatch("VPN_SUPPORT", "1", "<!--"); %>
                      <TD width=33></TD>
                      <% support_invmatch("VPN_SUPPORT", "1", "<!--"); %><TD width=49 class=small><a href="ipsec.asp"><script>Capture(sectopmenu.vpn)</script></a></TD><% support_invmatch("VPN_SUPPORT", "1", "-->"); %>
                      <TD width=1><!--P class=bar><font color='white'><b>|</b></font></P--></TD>
                      <TD class=small width=60></TD>
<% support_invmatch("GRE_SUPPORT", "1", "<!--"); %>
                      <TD width=4><font color='white'><b>|</b></font></TD>
                      <TD class=small width=26></TD>
                      <TD class=small width=72><% gre_config("ipsec_gre_link", ""); %></TD>
<% support_invmatch("GRE_SUPPORT", "1", "-->"); %>
</TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>
<TABLE height=5 cellSpacing=0 cellPadding=0 width=806 bgColor=black border=0>
  <TBODY>
  <TR bgColor=black>
    <TD 
    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
    borderColor=#e7e7e7 width=163 bgColor=#e7e7e7 height=1><IMG height=15 
      src="image/UI_03.gif" width=164 border=0></TD>
    <TD 
    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
    width=646 height=1><IMG height=15 src="image/UI_02.gif" width=645 
      border=0></TD></TR></TBODY></TABLE>
<TABLE id=AutoNumber9 style="BORDER-COLLAPSE: collapse" borderColor=#111111 
height=23 cellSpacing=0 cellPadding=0 width=809 border=0>
  <TBODY>
  <TR>
    <TD width=633>
      <TABLE cellSpacing=0 cellPadding=0 border=0 width="633">
        <TBODY>        
        <% support_match("VPN_SUPPORT", "1","<!--"); %><TR>
          <TD width=156 bgColor=#000000 height=35>
            <P align=right><B><FONT style="FONT-SIZE: 9pt" face=Arial 
            color=#ffffff><script>Capture(secleftmenu.vpnpass)</script></FONT></B></P></TD>
          <TD width=8 bgColor=#000000 height=25>&nbsp;</TD>
          <TD width=11 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;</TD>
          <TD width=296 height=25>&nbsp;</TD>
          <TD width=15 height=25>&nbsp;</TD>
          <TD width=15 height=25><IMG height=35 src="image/UI_05.gif" width=15 border=0></TD></TR>

        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=35 src="image/UI_04.gif" width=8 border=0></TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><script>Capture(secvpnpass.ipsecpass)</script>:</TD>
          <TD height=25>
            <TABLE id=AutoNumber12 cellSpacing=0 cellPadding=0 width=242 
            border=0>
              <TBODY>
              <TR>
                <TD width=27 height=25><INPUT type=radio value=1 
                  name=ipsec_pass <% nvram_match("ipsec_pass","1","checked"); %>></TD>
                <TD width=54 height=25><B><FONT style="FONT-SIZE: 8pt" 
                  face=Arial><script>Capture(share.enabled)</script></FONT></B></TD>
                <TD width=24 height=25><INPUT type=radio value=0 
                  name=ipsec_pass <% nvram_match("ipsec_pass","0","checked"); %>></TD>
                <TD width=137 height=25><B><FONT style="FONT-SIZE: 8pt" 
                  face=Arial size=2><script>Capture(share.disabled)</script></FONT></B></TD></TR></TBODY></TABLE></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 height=25><IMG height=35 src="image/UI_05.gif" width=15 border=0></TD></TR>

        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=35 src="image/UI_04.gif" width=8 border=0></TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><script>Capture(secvpnpass.pppoepass)</script>:</TD>
          <TD height=25>
            <TABLE id=AutoNumber12 cellSpacing=0 cellPadding=0 width=242 
            border=0>
              <TBODY>
              <TR>
                <TD width=27 height=25><INPUT type=radio value=1 
                  name=pppoe_pass <% nvram_match("pppoe_pass","1","checked"); %>></TD>
                <TD width=54 height=25><B><FONT style="FONT-SIZE: 8pt" 
                  face=Arial><script>Capture(share.enabled)</script></FONT></B></TD>
                <TD width=24 height=25><INPUT type=radio value=0 
                  name=pppoe_pass <% nvram_match("pppoe_pass","0","checked"); %>></TD>
                <TD width=137 height=25><B><FONT style="FONT-SIZE: 8pt" 
                  face=Arial size=2><script>Capture(share.disabled)</script></FONT></B></TD></TR></TBODY></TABLE></TD>        
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 height=25><IMG height=35 src="image/UI_05.gif" width=15 border=0></TD></TR>

        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=35 src="image/UI_04.gif" width=8 border=0></TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><script>Capture(secvpnpass.pptppass)</script>:</TD>
          <TD height=25>
            <TABLE id=AutoNumber12 cellSpacing=0 cellPadding=0 width=242 
            border=0>
              <TBODY>
              <TR>
                <TD width=27 height=25><INPUT type=radio value=1 
                  name=pptp_pass <% nvram_match("pptp_pass","1","checked"); %>></TD>
                <TD width=54 height=25><B><FONT style="FONT-SIZE: 8pt" 
                  face=Arial><script>Capture(share.enabled)</script></FONT></B></TD>
                <TD width=24 height=25><INPUT type=radio value=0 
                  name=pptp_pass <% nvram_match("pptp_pass","0","checked"); %>></TD>
                <TD width=137 height=25><B><FONT style="FONT-SIZE: 8pt" 
                  face=Arial size=2><script>Capture(share.disabled)</script></FONT></B></TD></TR></TBODY></TABLE></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 height=25><IMG height=35 src="image/UI_05.gif" width=15 border=0></TD></TR>

        <tr>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=35 src="image/UI_04.gif" width=8 border=0></TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><script>Capture(secvpnpass.l2tppass)</script>:</TD>
          <TD height=25>
            <TABLE id=AutoNumber12 cellSpacing=0 cellPadding=0 width=242 
            border=0>
              <TR>
                <TD width=27 height=25><INPUT type=radio value=1 
                  name=l2tp_pass <% nvram_match("l2tp_pass","1","checked"); %>></TD>
                <TD width=54 height=25><B><FONT style="FONT-SIZE: 8pt" 
                  face=Arial><script>Capture(share.enabled)</script></FONT></B></TD>
                <TD width=24 height=25><INPUT type=radio value=0 
                  name=l2tp_pass <% nvram_match("l2tp_pass","0","checked"); %>></TD>
                <TD width=137 height=25><B><FONT style="FONT-SIZE: 8pt" 
                  face=Arial size=2><script>Capture(share.disabled)</script></FONT></B></TD></TR></TABLE></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 height=25><IMG height=35 src="image/UI_05.gif" width=15 border=0></TD>
        </tr>

        <TR>
          <TD width=156 bgColor=#e7e7e7 height=1>&nbsp;</TD>
          <TD width=8 height=1><IMG height=35 src="image/UI_04.gif" width=8 border=0></TD>
          <TD height=25>&nbsp;</TD>
          <TD height=25>&nbsp;</TD>
          <TD colSpan=3>
            <HR color=#b5b5e6 SIZE=1>
          </TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 height=1><IMG height=35 src="image/UI_05.gif" width=15 border=0></TD></TR>
<% support_match("VPN_SUPPORT", "1","-->"); %>
        <TR>
          <TD width=156 bgColor=#000000 height=25>
            <p align="right"><b>
                <font face="Arial" color="#FFFFFF" style="font-size: 9pt">
                <script>Capture(sectopmenu.firewall)</script></font></b></TD>
          <TD width=8 bgColor=#000000 height=25>&nbsp;</TD>
          <TD width=11 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;</TD>
          <TD width=296 height=25>&nbsp;</TD>
          <TD width=15 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>
          <TD width=156 bgColor=#e7e7e7>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif>&nbsp;</TD>
          <TD colSpan=2 height=1 width="28">&nbsp;</TD>
          <TD width=411 colSpan=3 height=1>&nbsp;&nbsp;&nbsp;&nbsp;<script>Capture(secfirewall.fwprotection)</script>:&nbsp;&nbsp;
          <INPUT type=radio value=on name=filter <% support_match("FIREWALL_LEVEL_SUPPORT", "1", "onclick=fw_enable_disable(this.form)"); %> <% nvram_match("filter","on","checked"); %>><B><script>Capture(share.enabled)</script></B>&nbsp;
          <INPUT type=radio value=off name=filter <% support_match("FIREWALL_LEVEL_SUPPORT", "1", "onclick=fw_enable_disable(this.form)"); %> <% nvram_match("filter","off","checked"); %>><B><script>Capture(share.disabled)</script></B></TD>
          <TD width=15>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif>&nbsp;</TD></TR>

         <TR>
          <TD width=156 bgColor=#e7e7e7>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif>&nbsp;</TD>
          <TD colSpan=2 height=1 width="28">&nbsp;</TD>
          <TD width=411 colSpan=3 height=1>
            <HR color=#b5b5e6 SIZE=1>
          </TD>
          <TD width=15>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif>&nbsp;</TD></TR>
<% support_invmatch("FIREWALL_LEVEL_SUPPORT", "1", "<!--"); %>

        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>
          <p align="right"><font face="Arial" style="font-size: 8pt; font-weight: 700"> <script>Capture(secleftmenu.seclevel)</script></font></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="42">&nbsp;</TD>
          <TD width=397 height=25 colspan="2">
          <table border="0" cellpadding="0" cellspacing="0" id="AutoNumber12" width="300" style="border-collapse: collapse" bordercolor="#111111">

        <tr>
          <td height="25" width="75"><INPUT type=radio value=3 name=filter_level <% nvram_match("filter_level","3","checked"); %>><script>Capture(secfirewall.high)</script></td>
          <td height="25" width="75"><INPUT type=radio value=2 name=filter_level <% nvram_match("filter_level","2","checked"); %>> <script>Capture(secfirewall.medium)</script></td>
          <td height="25" width="75"><INPUT type=radio value=1 name=filter_level <% nvram_match("filter_level","1","checked"); %>> <script>Capture(secfirewall.low)</script></td>
          <td height="25" width="75"><INPUT type=radio value=0 name=filter_level <% nvram_match("filter_level","0","checked"); %>> <script>Capture(secfirewall.custom)</script></td></tr>

      </table></TD></TD>
          <TD width=15 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>
          <TD width=156 bgColor=#e7e7e7>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif>&nbsp;</TD>
          <TD colSpan=2 height=1 width="28">&nbsp;</TD>
          <TD width=411 colSpan=3 height=1>
            <HR color=#b5b5e6 SIZE=1>
          </TD>
          <TD width=15>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif>&nbsp;</TD></TR>
<% support_invmatch("FIREWALL_LEVEL_SUPPORT", "1", "-->"); %>

        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>
          <p align="right">
                <font face="Arial" style="font-size: 8pt; font-weight: 700"> <script>Capture(secleftmenu.adfilters)</script></font></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="42">&nbsp;</TD>
          <TD width=397 height=25 colspan="2"><table border="0" cellpadding="0" cellspacing="0" id="AutoNumber12" width="300">
                  <tr>
                    <td height="25" width="27"><INPUT type=checkbox value=1 name="_block_proxy" <% nvram_match("block_proxy","1","checked"); %>><b>
		    </td>
                    <td height="25" width="110"> <script>Capture(secfirewall.filterproxy)</script>&nbsp;
		    </td>
                    <td height="25" width="27">
    <INPUT type=checkbox value=1 name="_block_cookie" <% nvram_match("block_cookie","1","checked"); %>><b>
		    </td>
                    <td height="25" width="90"> <script>Capture(secfirewall.filtercookies)</script>&nbsp;
		    </td>
                  </tr>
                </table>
</TD>
          <TD width=15 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="42">&nbsp;</TD>
          <TD width=397 height=25 colspan="2"><table border="0" cellpadding="0" cellspacing="0" id="AutoNumber12" width="300" >
                  <tr>
                    <td height="25" width="27">
    <INPUT type=checkbox value=1 name="_block_java" <% nvram_match("block_java","1","checked"); %>><b>
		    </td>
                    <td height="25" width="110"> <script>Capture(secfirewall.filterapplets)</script>&nbsp;
		    </td>
                    <td height="25" width="27">
    <INPUT type=checkbox value=1 name="_block_activex" <% nvram_match("block_activex","1","checked"); %>><b>
		    </td>
                    <td height="25" width="90"> <script>Capture(secfirewall.filteractivex)</script>&nbsp;
		    </td>
                  </tr>
                </table>
</TD>
          <TD width=15 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif>&nbsp;</TD>
          <TD colSpan=2 height=1 width="28">&nbsp;</TD>
          <TD width=411 colSpan=3 height=1>
            <HR color=#b5b5e6 SIZE=1>
          </TD>
          <TD width=15>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>
          <p align="right">
                <font face="Arial" style="font-size: 8pt; font-weight: 700">
                <script>Capture(secleftmenu.blowanreq)</script></font></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="42">&nbsp;</TD>
          <TD width=397 height=25 colspan="2"><table border="0" cellpadding="0" cellspacing="0" id="AutoNumber12" width="300">
                  <tr>
                    <td height="25" width="27"><input type=checkbox value=1 name=_block_wan <% nvram_match("block_wan","1","checked"); %>></td>
                    <td height="25" width="240"> <script>Capture(secfirewall.blockinfo)</script>&nbsp; </td>
                  </tr>
                </table>
</TD>
          <TD width=15 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>               
<TR>
          <TD width=156 bgColor=#e7e7e7>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif>&nbsp;</TD>
          <TD colSpan=2 height=1 width="28">&nbsp;</TD>
          <TD width=411 colSpan=3 height=1>
            <HR color=#b5b5e6 SIZE=1>
          </TD>
          <TD width=15>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif>&nbsp;</TD></TR>
<TR>
          <TD width=156 bgColor=#e7e7e7>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif>&nbsp;</TD>
          <TD colSpan=2 height=1 width="28">&nbsp;</TD>
          <TD width=411 colSpan=3 height=1 align=center>
<!--            <input type=button value='View Logs' onClick='ViewLogIn()'>--> 
<script>document.write("<input type=button " + "value=\"" + share.viewlog + "\" onClick=ViewLogIn()>");</script>
          </TD>
          <TD width=15>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif>&nbsp;</TD></TR>
<TR>
          <TD width=156 bgColor=#e7e7e7>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif>&nbsp;</TD>
          <TD colSpan=2 height=1 width="28">&nbsp;</TD>
          <TD width=411 colSpan=3 height=1 align=center>
          </TD>
          <TD width=15>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif>&nbsp;</TD></TR>
        </TBODY></TABLE></TD>
    <TD vAlign=top width=176 bgColor=#6666cc>
      <TABLE cellSpacing=0 cellPadding=0 width=176 border=0>
        <TBODY>
        <TR>
          <TD width=11 bgColor=#6666cc height=25>&nbsp;</TD>
          <TD width=156 bgColor=#6666cc height=25><font color="#FFFFFF">
          <span ><a target="_blank" href="help/HSecurity.asp"><script>Capture(share.more)</script>...</a></span></font>&nbsp;</TD>
          <TD width=9 bgColor=#6666cc 
  height=25>&nbsp;</TD></TR></TBODY></TABLE></TD></TR>
  <!--<TR>
    <TD width=809 colSpan=2>
      <TABLE cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=30>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif>&nbsp;</TD>
          <TD width=16>&nbsp;</TD>
          <TD width=12>&nbsp;</TD>
          <TD width=411>&nbsp;</TD>
          <TD width=15>
          <TD width=15 background=image/UI_05.gif>&nbsp;</TD>
          <TD width=176 bgColor=#6666cc rowSpan=2>
          <IMG 
            src="image/UI_Cisco.gif" border=0 width="176" height="64"></TD></TR>
        <TR>
          <TD width=156 bgColor=#000000>&nbsp;</TD>
          <TD width=8 bgColor=#000000>&nbsp;</TD>
          <TD width=16 bgColor=#6666cc>&nbsp;</TD>
          <TD width=12 bgColor=#6666cc>&nbsp;</TD>
          <TD width=411 bgColor=#6666cc>
            <TABLE height=19 cellSpacing=0 cellPadding=0 align=right 
              border=0 width="76"><TBODY>
              <TR>
                <TD 
                  align=middle width="101">
                <INPUT type=button value="Save Settings" onClick=to_submit(this.form)></FONT></TD>
                <TD align=middle width=8 bgColor=#6666cc>&nbsp;</TD>
                <TD align=middle width="130"><INPUT type=reset value="Cancel Changes"></TD>
                <TD align=middle width=4 bgColor=#6666cc>&nbsp;</TD>
                </TR></TBODY></TABLE></TD>
          <TD width=15 bgColor=#6666cc height=33>&nbsp;</TD>
          <TD width=15 bgColor=#000000 
  height=33>&nbsp;</TD></TR></TBODY></TABLE></TD></TR>-->
  
   <TR>
         <TD width=809 colSpan=2>
         <TABLE cellSpacing=0 cellPadding=0 border=0>
         <TBODY>
         <TR>
          <TD width=156 bgColor=#e7e7e7 height=30>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif>&nbsp;</TD>
          <TD width=16>&nbsp;</TD>
          <TD width=12>&nbsp;</TD>
          <TD width=411>&nbsp;</TD>
          <TD width=15 height=1>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif>&nbsp;</TD>
          <TD width=176 bgColor=#6666cc rowSpan=2><IMG src="image/UI_Cisco.gif" border=0 width="176" height="64"></TD></TR>
        <TR>
          <TD width=156 bgColor=#000000>&nbsp;</TD>
          <TD width=8 bgColor=#000000>&nbsp;</TD>
          <TD width=16 bgColor=#6666cc>&nbsp;</TD>
          <TD width=12 bgColor=#6666cc>&nbsp;</TD>
          <TD width=411 bgColor=#6666cc>
            <TABLE height=19 cellSpacing=0 cellPadding=0 align=right border=0 width="220">
            <TBODY>
              <TR>
                <TD align=middle bgColor=#42498c width="101"><FONT color=#ffffff><B><A href='javascript:to_submit(document.forms[0]);'><script>Capture(share.saves)</script></A></B></FONT>
                </TD>
                <TD align=middle width=8 bgColor=#6666cc>&nbsp;</TD>
                <TD align=middle bgColor=#434a8f width="111"><FONT color=#ffffff><B><A href='Firewall.asp'><script>Capture(share.cancels)</script></A></B></FONT></TD>
                <TD align=middle width=4 bgColor=#6666cc>&nbsp;</TD>
                </TR></TBODY></TABLE></TD>
                <TD width=15 bgColor=#6666cc height=33>&nbsp;</TD>
                <TD width=15 bgColor=#000000 height=33>&nbsp;</TD>
              </TR>
            </TBODY>
            </TABLE>
          </TD>
        </TR>
  </TBODY></TABLE></FORM></DIV></BODY></HTML>

