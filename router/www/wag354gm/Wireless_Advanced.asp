
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

<HTML><HEAD><TITLE>Wireless</TITLE>
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
<script language="JavaScript" type="text/javascript" src="capsetup.js"></script>
<script language="JavaScript" type="text/javascript" src="capwireless.js"></script>
<SCRIPT language=JavaScript>
document.title=(bmenu.wireless);
<% wireless_support(5); %>share.productname = share.productname1;<% wireless_support(6); %>
var win_options = 'alwaysRaised,resizable,scrollbars,width=660,height=460' ;

var wl_filter_win = null;
var EN_DIS =  '<% nvram_get("wl_macmode"); %>'

function to_submit(F)
{
        F.submit_button.value = "Wireless_Advanced";
        F.action.value = "Apply";
        F.submit();
}
</SCRIPT>

</HEAD>
<BODY>
<DIV align=center>
<FORM method=<% get_http_method(); %> name=wireless action=apply.cgi>
<input type=hidden name=submit_button>
<input type=hidden name=change_action>
<input type=hidden name=action>
<TABLE cellSpacing=0 cellPadding=0 width=809 border=0>
  <TBODY>
  <TR>
    <TD width=95><IMG src="image/UI_Linksys.gif" border=0 width="165" height="57"></TD>
    <TD vAlign=bottom align=right width=714 bgColor=#6666cc><FONT style="FONT-SIZE: 7pt" color=#ffffff><FONT face=Arial><script>Capture(share.firmwarever)</script>:&nbsp;<% get_firmware_version(); %>&nbsp;&nbsp;&nbsp;</FONT></FONT></TD></TR>
  <TR>
    <TD width=808 bgColor=#6666cc colSpan=2><IMG height=11 src="image/UI_10.gif" width=809 border=0></TD></TR></TBODY></TABLE>
<TABLE height=77 cellSpacing=0 cellPadding=0 width=809 bgColor=black border=0>
  <TBODY>
  <TR>
    <TD style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
    borderColor=#000000 align=middle width=163 height=49>
      <H3 style="MARGIN-TOP: 1px; MARGIN-BOTTOM: 1px">
      <font color="#FFFFFF" face="Arial" style="font-size: 15pt"><script>Capture(bmenu.wireless)</script></font></H3></TD>
    <TD style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
    vAlign=center borderColor=#000000 width=646 bgColor=#000000 height=49>
      <TABLE style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; BORDER-COLLAPSE: collapse; FONT-VARIANT: normal" 
      height=33 cellSpacing=0 cellPadding=0 bgColor=#6666cc border=0>
        <TBODY>
        <TR>
          <TD style="FONT-WEIGHT: bolder; FONT-SIZE: 10pt" align=right bgColor=#6666cc height=33><FONT color=#ffffff><% wireless_support(1); %><script>Capture(share.wireless_g)</script><% wireless_support(2); %><script>Capture(share.productname)</script>&nbsp;&nbsp;</FONT></TD>
          <TD borderColor=#000000 borderColorLight=#000000 align=middle width=109 bgColor=#000000 borderColorDark=#000000 height=12 rowSpan=2><FONT color=#ffffff><SPAN style="FONT-SIZE: 8pt"><B><% nvram_get("router_name"); %></B></SPAN></FONT></TD></TR>
        <TR>
          <TD style="FONT-WEIGHT: normal; FONT-SIZE: 1pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" width=537 bgColor=#000000 height=1>&nbsp;</TD></TR>
        <TR>
          <TD width=646 bgColor=#000000 colSpan=2 height=32>
            <TABLE id=AutoNumber1 style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; BORDER-COLLAPSE: collapse; FONT-VARIANT: normal" height=6 cellSpacing=0 cellPadding=0 width=637 border=0><TBODY>
              <TR style="BORDER-RIGHT: medium none; BORDER-TOP: medium none; FONT-WEIGHT: normal; FONT-SIZE: 1pt; BORDER-LEFT: medium none; COLOR: black; BORDER-BOTTOM: medium none; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" align=middle bgColor=#6666cc>
                <TD width=83  height=1><IMG height=10 src="image/UI_06.gif" width=83  border=0></TD>
                <TD width=83  height=1><IMG height=10 src="image/UI_07.gif" width=83  border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=68  height=1><IMG height=10 src="image/UI_06.gif" width=68  border=0></TD></TR>
              <TR>
                <TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><A style="TEXT-DECORATION: none" href="index.asp"><script>Capture(bmenu.setup)</script></A></FONT></TD>
                <TD align=middle bgColor=#6666cc height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><script>Capture(bmenu.wireless)</script></FONT></TD>
                <TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Firewall.asp"><script>Capture(bmenu.security)</script></a></FONT></TD>
                <TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="<% support_elsematch("PARENTAL_CONTROL_SUPPORT", "1", "Parental_Control.asp", "Filters.asp"); %>"><script>Capture(bmenu.accrestriction)</script></a></FONT></TD>
                <TD align=middle bgColor=#000000 height=20><P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Forward_UPnP.asp"><script>Capture(bmenu.application)</script> <BR>&amp; <script>Capture(bmenu.gaming)</script></a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>
                <TD align=middle bgColor=#000000 height=20><P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Management.asp">&nbsp;&nbsp;<script>Capture(bmenu.admin)</script></a>&nbsp;&nbsp;</FONT></P></TD>
                <TD align=middle bgColor=#000000 height=20><P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Status_Router.asp"><script>Capture(bmenu.statu)</script></a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>
              </TR>              
              <TR>
                <TD width=643 bgColor=#6666cc colSpan=7 height=21>
                  <TABLE borderColor=black height=21 cellSpacing=0 cellPadding=0>
                    <TBODY>
                    <TR align=center>
                      <TD class=small width=165><a href="Wireless_Basic.asp"><script>Capture(wltopmenu.basicsetup)</script></a></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=165><a href="WL_WPATable.asp"><script>Capture(wltopmenu.wlsecurity)</script></a></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=165><a href="Wireless_MAC.asp"><script>Capture(wltopmenu.wlaccess)</script></a></TD>
                      <TD width=1> <P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=210><FONT style="COLOR: white"><script>Capture(wltopmenu.advwlsetting)</script></FONT></TD>
                    </TR>
                    </TBODY>
                  </TABLE>
                </TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>
<TABLE height=5 cellSpacing=0 cellPadding=0 width=806 bgColor=black border=0>
  <TBODY>
  <TR bgColor=black>
    <TD style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" borderColor=#e7e7e7 width=163 bgColor=#e7e7e7 height=1><IMG height=15 src="image/UI_03.gif" width=164 border=0></TD>
    <TD style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" width=646 height=1><IMG height=15 src="image/UI_02.gif" width=645 border=0></TD></TR></TBODY></TABLE>
<TABLE id=AutoNumber9 style="BORDER-COLLAPSE: collapse" borderColor=#111111 height=23 cellSpacing=0 cellPadding=0 width=809 border=0><TBODY>
  <TR>
    <TD width=633>
      <TABLE cellSpacing=0 cellPadding=0 border=0 width="633">
        <TBODY>
        <TR>
          <TD width=156 bgColor=#000000 height=25>
            <P align=right><B><font face="Arial" color="#FFFFFF" style="font-size: 9pt"><script>Capture(wlleftmenu.advwireless)</script></font></B></P></TD>
          <TD width=8 bgColor=black height=25>&nbsp;</TD>
          <TD width=6>&nbsp;</TD>
          <TD width=7>&nbsp;</TD>
          <TD width=8>&nbsp;</TD>
          <TD width=156>&nbsp;</TD>
          <TD width=262 colSpan="2">&nbsp;</TD>
          <TD width=15>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif>&nbsp;</TD></TR>
        <TR>
          <TD align=right width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="21">&nbsp;</TD>
          <TD width=156 height=25><font face="Arial" style="font-size: 8pt"><script>Capture(wlnetwork.authtype)</script>:</font></TD>
          <TD width=262 height=25 colSpan="2"><SELECT name="wl_auth">
    <option value="0" <% nvram_selmatch("wl_auth", "0", "selected"); %>>Auto</option>
    <option value="1" <% nvram_selmatch("wl_auth", "1", "selected"); %>><script>Capture(wlother.sharedkey)</script></option>
    </SELECT>&nbsp;&nbsp;(<script>Capture(share.defaults)</script>: Auto)</TD></TD>
          <TD width=15 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
<!-- wwzh
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>Basic Rate:</TD>
          <TD width=296 height=25 colSpan="2"><SELECT  name="wl_rateset"> 
     		 <OPTION value="12" <% nvram_selmatch("wl_rateset", "12", "selected"); %>>1-2 Mbps&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</OPTION> 
      		 <OPTION value="default" <% nvram_selmatch("wl_rateset", "default", "selected"); %>>Default</OPTION>  
        	 <OPTION value="all" <% nvram_selmatch("wl_rateset", "all", "selected"); %>>All</OPTION>
    		</SELECT>&nbsp;&nbsp;(Default: Default)</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
 wwzh-->
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="21">&nbsp;</TD>
          <TD width=156 height=25><font face="Arial" style="font-size: 8pt"><script>Capture(wlnetwork.control)</script>:</font></TD>
          <TD width=262 height=25 colSpan="2"><SELECT name="wl_rate"> 
<!-- wwzh
      	<OPTION value="1000000" <% nvram_selmatch("wl_rate", "1000000", "selected"); %>>1 Mbps</OPTION> 
	<OPTION value="2000000" <% nvram_selmatch("wl_rate", "2000000", "selected"); %>>2 Mbps</OPTION> 
	<OPTION value="5500000" <% nvram_selmatch("wl_rate", "5500000", "selected"); %>>5.5 Mbps</OPTION> 
        <OPTION value="6000000" <% nvram_selmatch("wl_rate", "6000000", "selected"); %>>6 Mbps</OPTION>
        <OPTION value="9000000" <% nvram_selmatch("wl_rate", "9000000", "selected"); %>>9 Mbps</OPTION>
        <OPTION value="11000000" <% nvram_selmatch("wl_rate", "11000000", "selected"); %>>11 Mbps</OPTION>

-->
	 <OPTION value="12000000" <% nvram_selmatch("wl_rate", "12000000", "selected"); %>>12 Mbps</OPTION>
        <OPTION value="18000000" <% nvram_selmatch("wl_rate", "18000000", "selected"); %>>18 Mbps</OPTION>
        <OPTION value="24000000" <% nvram_selmatch("wl_rate", "24000000", "selected"); %>>24 Mbps</OPTION>
        <OPTION value="36000000" <% nvram_selmatch("wl_rate", "36000000", "selected"); %>>36 Mbps</OPTION>
        <OPTION value="48000000" <% nvram_selmatch("wl_rate", "48000000", "selected"); %>>48 Mbps</OPTION>
        <OPTION value="54000000" <% nvram_selmatch("wl_rate", "54000000", "selected"); %>>54 Mbps</OPTION>
        <OPTION value="0" <% nvram_selmatch("wl_rate", "0", "selected"); %>><script>Capture(share.auto)</script>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</OPTION>
    </SELECT>&nbsp;&nbsp;(<script>Capture(share.defaults)</script>: <script>Capture(share.auto)</script>)</TD>
          <TD width=15 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
<!-- wwzh
         <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>CTS Protection Mode:</TD>
          <TD width=296 height=25>
          <SELECT name="wl_gmode_protection"> 
    <option value="off" <% nvram_selmatch("wl_gmode_protection", "off", "selected"); %>>Disable&nbsp;&nbsp;&nbsp;</option>
    <option value="auto" <% nvram_selmatch("wl_gmode_protection", "auto", "selected"); %>>Auto</option>
    </SELECT>&nbsp;&nbsp;(Default: Disable)</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>   
         <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><span >Frame Burst</span>:</TD>
          <TD width=296 height=25>
          <SELECT name="wl_frameburst"> 
    <option value="off" <% nvram_selmatch("wl_frameburst", "off", "selected"); %>>Disable&nbsp;&nbsp;&nbsp;</option>
    <option value="on" <% nvram_selmatch("wl_frameburst", "on", "selected"); %>>Enable</option>
    </SELECT>&nbsp;&nbsp;(Default: Disable)</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
wwzh -->
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="21">&nbsp;</TD>
          <TD width=156 height=25><font face="Arial" style="font-size: 8pt"><script>Capture(wlnetwork.beacon)</script>:</font></TD>
          <TD width=40 height=25>
              <INPUT maxLength=5 onBlur=valid_range(this,1,65535,wlnetwork.beacon) size=4 value='<% nvram_selget("wl_bcn"); %>' name="wl_bcn">&nbsp;&nbsp;</TD>
          <TD width=190 height=25>
              (<script>Capture(share.defaults)</script>: 100, <script>Capture(wlnetwork.milli)</script>, <script>Capture(share.range)</script>: 1 - 65535)</TD>
          <TD width=15 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="21">&nbsp;</TD>
          <TD width=156 height=25><font face="Arial" style="font-size: 8pt"><script>Capture(wlnetwork.dtim)</script>:</font></TD>
          <TD width=262 height=25 colSpan="2">
              <INPUT maxLength=3 onBlur=valid_range(this,1,255,wlnetwork.dtim) size=4 value='<% nvram_selget("wl_dtim"); %>' name="wl_dtim">&nbsp;&nbsp;(<script>Capture(share.defaults)</script>: <% get_wl_value("default_dtim"); %>, <script>Capture(share.range)</script>: 1 - 255)</TD>
          <TD width=15 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="21">&nbsp;</TD>
          <TD width=156 height=25><font face="Arial" style="font-size: 8pt"><script>Capture(wlnetwork.frag)</script>:</font></TD>
          <TD width=262 height=25 colSpan="2">
              <INPUT maxLength=4 onBlur=valid_range(this,256,4096,wlnetwork.frag) size=4 value='<% nvram_selget("wl_frag"); %>' name="wl_frag">&nbsp;&nbsp;(<script>Capture(share.defaults)</script>: 2346, <script>Capture(share.range)</script>: 256 - 4096)</TD>
          <TD width=15 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>       
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="21">&nbsp;</TD>
          <TD width=156 height=25><font face="Arial" style="font-size: 8pt"><script>Capture(wlnetwork.rts)</script>:</font></TD>
          <TD width=262 height=25 colSpan="2">
              <INPUT maxLength=4 onBlur=valid_range(this,0,4096,wlnetwork.rts) size=4 value='<% nvram_selget("wl_rts"); %>' name="wl_rts">&nbsp;&nbsp;(<script>Capture(share.defaults)</script>: 2347, <script>Capture(share.range)</script>: 0 - 4096)</TD>
          <TD width=15 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>    
        </TBODY></TABLE></TD>
    <TD vAlign=top width=176 bgColor=#6666cc>
      <TABLE cellSpacing=0 cellPadding=0 width=176 border=0>
        <TBODY>
        <TR>
          <TD width=11 bgColor=#6666cc height=25>&nbsp;</TD>
          <TD width=156 bgColor=#6666cc height=25><font color="#FFFFFF"><span ><a target="_blank" href="help/HWireless.asp"><script>Capture(share.more)</script>...</a></span></font></TD>
          <TD width=9 bgColor=#6666cc height=25>&nbsp;</TD></TR></TBODY></TABLE></TD></TR>
  <TR>
    <TD width=809 colSpan=2>
      <TABLE cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=154 bgColor=#e7e7e7 height=30>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif>&nbsp;</TD>
          <TD width=16>&nbsp;</TD>
          <TD width=12>&nbsp;</TD>
          <TD width=411>&nbsp;</TD>
          <TD width=15>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif>&nbsp;</TD>
          <TD width=176 bgColor=#6666cc rowSpan=2><IMG src="image/UI_Cisco.gif" border=0 width="176" height="64"></TD></TR>
        <TR>
          <TD width=156 bgColor=#000000>&nbsp;</TD>
          <TD width=8 bgColor=#000000>&nbsp;</TD>
          <TD width=16 bgColor=#6666cc>&nbsp;</TD>
          <TD width=12 bgColor=#6666cc>&nbsp;</TD>
          <TD width=411 bgColor=#6666cc>
            <TABLE height=19 cellSpacing=0 cellPadding=0 width=220 align=right border=0>
            <TBODY>
              <TR>
                <TD align=middle bgColor=#42498c width=101><FONT color=#ffffff><B><A href='javascript:to_submit(document.forms[0])'><script>Capture(share.saves)</script></A></B></FONT></TD>
                <TD align=middle width=8 bgColor=#6666cc>&nbsp;</TD>
                <TD align=middle width=111 bgColor=#434a8f><FONT color=#ffffff><B><A href=Wireless_Advanced.asp><script>Capture(share.cancels)</script></A></B></FONT></TD>
                </TR></TBODY></TABLE></TD>
          <TD width=15 bgColor=#6666cc height=33>&nbsp;</TD>
          <TD width=15 bgColor=#000000 height=33>&nbsp;</TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>
</FORM></DIV></BODY></HTML>
