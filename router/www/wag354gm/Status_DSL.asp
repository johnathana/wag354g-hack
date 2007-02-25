
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

<HTML><HEAD><TITLE>Status</TITLE>
<% no_cache(); %>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<link rel="stylesheet" type="text/css" href="style.css">
<style fprolloverstyle>
A:hover {color: #00FFFF}
.small A:hover {color: #00FFFF}
</style>

<script src="common.js"></script>
<SCRIPT language="Javascript" type="text/javascript" src="share.js"></SCRIPT>
<SCRIPT language="Javascript" type="text/javascript" src="capsetup.js"></SCRIPT>
<SCRIPT language="Javascript" type="text/javascript" src="capstatus.js"></SCRIPT>
<script language=Javascript>
document.title=(setddns.statu);
<% wireless_support(5); %>share.productname = share.productname1;<% wireless_support(6); %>
function SelCONNECTION(num,F)
{
 	F.submit_button.value = "Status_DSL";
        F.change_action.value = "gozila_cgi";
        F.status_connection.value=F.status_connection.options[num].value;
        F.submit();
}
var value=0;
function Refresh()
{
	var refresh_time = "<% show_status("refresh_time"); %>";
	if(refresh_time == "")	refresh_time = 15000;
	if (value>=1)
	{
		window.location.replace("Status_DSL.asp");
	}
	value++;
	timerID=setTimeout("Refresh()",refresh_time);
}

function init()
{
	Refresh();
}
</script>
</HEAD>
<BODY onload=init()>
<DIV align=center>
<FORM name=setup method=<% get_http_method(); %> action=apply.cgi>
<input type=hidden name=submit_button>
<input type=hidden name=change_action>
<TABLE cellSpacing=0 cellPadding=0 width=809 border=0>
  <TBODY>
  <TR>
    <TD width=95><IMG src="image/UI_Linksys.gif" border=0 width="165" height="57"></TD>
    <TD vAlign=bottom align=right width=714 bgColor=#6666cc><FONT 
      style="FONT-SIZE: 7pt" color=#ffffff><FONT face=Arial><script>Capture(share.firmwarever)</script>:&nbsp;<% get_firmware_version(); %>&nbsp;&nbsp;&nbsp;</FONT></FONT></TD></TR>
  <TR>
    <TD width=808 bgColor=#6666cc colSpan=2><IMG height=11 
      src="image/UI_10.gif" width=809 border=0></TD></TR></TBODY></TABLE>
<TABLE height=77 cellSpacing=0 cellPadding=0 width=809 bgColor=black border=0>
  <TBODY>
  <TR>
    <TD 
    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
    borderColor=#000000 align=middle width=163 height=49>
      <H3 style="MARGIN-TOP: 1px; MARGIN-BOTTOM: 1px"><FONT 
      style="FONT-SIZE: 15pt" face=Arial color=#ffffff><script>Capture(bmenu.statu)</script></FONT></H3></TD>
    <TD 
    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
    vAlign=center borderColor=#000000 width=646 bgColor=#000000 height=49>
      <TABLE 
      style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; BORDER-COLLAPSE: collapse; FONT-VARIANT: normal" 
      height=33 cellSpacing=0 cellPadding=0 bgColor=#6666cc border=0>
        <TBODY>
        <TR>
          <TD style="FONT-WEIGHT: bolder; FONT-SIZE: 10pt" align=right 
          bgColor=#6666cc height=33><FONT color=#ffffff><span><% wireless_support(1); %><script>Capture(share.wireless_g)</script><% wireless_support(2); %><script>Capture(share.productname)</script>&nbsp;&nbsp;</span></FONT></TD>
          <TD borderColor=#000000 borderColorLight=#000000 align=middle 
          width=109 bgColor=#000000 borderColorDark=#000000 height=12 
            rowSpan=2><FONT color=#ffffff><SPAN 
            style="FONT-SIZE: 8pt"><B><% nvram_get("router_name"); %></B></SPAN></FONT></TD></TR>
        <TR>
          <TD 
          style="FONT-WEIGHT: normal; FONT-SIZE: 1pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
          width=537 bgColor=#000000 height=1>&nbsp;</TD></TR>
        <TR>
          <TD width=646 bgColor=#000000 colSpan=2 height=32>
            <TABLE id=AutoNumber1 
            style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; BORDER-COLLAPSE: collapse; FONT-VARIANT: normal" 
            height=6 cellSpacing=0 cellPadding=0 width=637 border=0>
              <TBODY>
              <TR 
              style="BORDER-RIGHT: medium none; BORDER-TOP: medium none; FONT-WEIGHT: normal; FONT-SIZE: 1pt; BORDER-LEFT: medium none; COLOR: black; BORDER-BOTTOM: medium none; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
              align=middle bgColor=#6666cc>
                <TD width=83  height=1><IMG height=10 src="image/UI_06.gif" width=83  border=0></TD>
                <TD width=83  height=1><IMG height=10 src="image/UI_06.gif" width=83  border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="<% wireless_support(3); %>" width=103 border=0></TD>
                <TD width=68  height=1><IMG height=10 src="<% wireless_support(4); %>" width=68  border=0></TD></TR>
              <TR>
                <TD align=middle bgColor=#000000 height=20><FONT 
                  style="FONT-WEIGHT: 700" color=#ffffff><A 
                  style="TEXT-DECORATION: none" 
                  href="index.asp"><script>Capture(bmenu.setup)</script></A></FONT></TD>
                <% wireless_support(1); %><TD align=middle bgColor=#000000 height=20><FONT 
                  style="FONT-WEIGHT: 700" color=#ffffff>
                <a style="TEXT-DECORATION: none" href="Wireless_Basic.asp"><script>Capture(bmenu.wireless)</script></a></FONT></TD><% wireless_support(2); %>
                <TD align=middle bgColor=#000000 height=20><FONT 
                  style="FONT-WEIGHT: 700" color=#ffffff>
                <a style="TEXT-DECORATION: none" href="Firewall.asp"><script>Capture(bmenu.security)</script></a></FONT></TD>
                <TD align=middle bgColor=#000000 height=20><FONT 
                  style="FONT-WEIGHT: 700" color=#ffffff>
                <a style="TEXT-DECORATION: none" href="<% support_elsematch("PARENTAL_CONTROL_SUPPORT", "1", "Parental_Control.asp", "Filters.asp"); %>"><script>Capture(bmenu.accrestriction)</script></a></FONT></TD>
                <TD align=middle bgColor=#000000 height=20>
                  <P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" 
                  color=#ffffff>
                  <a style="TEXT-DECORATION: none" href="Forward_UPnP.asp"><script>Capture(bmenu.application)</script>
                  <BR>&amp; <script>Capture(bmenu.gaming)</script></a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>
                <TD align=middle bgColor=#000000 height=20>
                  <P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" 
                  color=#ffffff>
                  <a style="TEXT-DECORATION: none" href="Management.asp">&nbsp;&nbsp;<script>Capture(bmenu.admin)</script></a>&nbsp;&nbsp;</FONT></P></TD>
                <TD align=middle bgColor=#6666cc height=20>
                  <P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" 
                  color=#ffffff><script>Capture(bmenu.statu)</script>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>
              </TR>
              <TR>
                <TD width=643 bgColor=#6666cc colSpan=7 height=21>
                  <TABLE borderColor=black height=21 cellSpacing=0 cellPadding=0>
                    <TBODY>
                    <TR align=center>
                      <TD class=small width=90><A href="Status_Router.asp"><script>Capture(statopmenu.gateway)</script></A></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=125><A href="Status_Lan.asp"><script>Capture(statopmenu.localnet)</script></FONT></TD>
                      <% wireless_support(1); %><TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=95><A href="Status_Wireless.asp"><script>Capture(statopmenu.wireless)</script></A></TD><% wireless_support(2); %>
		      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD width=125><FONT style="COLOR: white"><script>Capture(statopmenu.dslconn)</script></FONT></TD>
                    </TR>
                    </TBODY>
                  </TABLE>
                </TD>
              </TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>
<TABLE height=5 cellSpacing=0 cellPadding=0 width=806 bgColor=black border=0>
  <TBODY>
  <TR bgColor=black>
    <TD 
    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
    borderColor=#e7e7e7 width=163 bgColor=#e7e7e7 height=1><IMG height=15 
      src="image/UI_03.gif" width=164 border=0></TD>
    <TD 
    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
    width=646 height=1><IMG height=15 src="image/UI_02.gif" 
      width=645 border=0></TD></TR></TBODY></TABLE>
<TABLE id=AutoNumber9 style="BORDER-COLLAPSE: collapse" borderColor=#111111 
height=23 cellSpacing=0 cellPadding=0 width=809 border=0>
  <TBODY>
  <TR>
    <TD width=633>
      <TABLE cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=156 bgColor=#000000 height=25>
            <P align=right><B><FONT style="FONT-SIZE: 9pt" face=Arial 
            color=#ffffff><script>Capture(staleftmenu.dslstatus)</script></FONT></B></P></TD>
          <TD width=8 bgColor=#000000 height=25>&nbsp;</TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=171 height=25>&nbsp;</TD>
          <TD width=226 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
 	
	<TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(stadslstatus.dslstatus)</script>:&nbsp;</FONT></TD>
          <TD width=226 height=25>&nbsp;<FONT 
            style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("dslstatus", 10); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
        
	<TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(stadslstatus.dslmodulmode)</script>:&nbsp;</FONT></TD>
          <TD width=226 height=25>&nbsp;<FONT 
            style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("trainedmode", 10); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>

	<TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(stadslstatus.dslpathmode)</script>:&nbsp;</FONT></TD>
          <TD width=226 height=25>&nbsp;<FONT 
            style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("trainedpath", 10); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>

        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD><FONT style="FONT-SIZE: 8pt"><script>Capture(stadslstatus.downrate)</script>:&nbsp;</FONT></TD>
          <TD>&nbsp;<FONT style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("dsrate", 0); %>bps</B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(stadslstatus.uprate)</script>:&nbsp;</FONT></TD>
          <TD width=226 height=25>&nbsp;<FONT 
            style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("usrate", 0); %>bps</B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
        
	<TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(stadslstatus.downmargin)</script>:&nbsp;</FONT></TD>
          <TD width=226 height=25>&nbsp;<FONT 
            style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("dsmargin", 0); %> db</B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(stadslstatus.upmargin)</script>:&nbsp;</FONT></TD>
          <TD width=226 height=25>&nbsp;<FONT 
            style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("usmargin", 0); %> db</B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
	<TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(stadslstatus.downlineatn)</script>:&nbsp;</FONT></TD>
          <TD width=226 height=25>&nbsp;<FONT 
            style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("dsatten", 0); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
	<TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(stadslstatus.uplineatn)</script>:&nbsp;</FONT></TD>
          <TD width=226 height=25>&nbsp;<FONT 
            style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("usatten", 0); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>

	<TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(stadslstatus.downtranspower)</script>:&nbsp;</FONT></TD>
          <TD width=226 height=25>&nbsp;<FONT 
            style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("dspower", 0); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
	<TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(stadslstatus.uptranspower)</script>:&nbsp;</FONT></TD>
          <TD width=226 height=25>&nbsp;<FONT 
            style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("uspower", 0); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>


	<TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"></FONT></TD>
          <TD width=226 height=25>&nbsp;<FONT 
            style="FONT-SIZE: 8pt"></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>

        <TR>
          <TD width=156 bgColor=#000000 height=25>
            <P align=right><B><FONT style="FONT-SIZE: 9pt" face=Arial 
            color=#ffffff><script>Capture(staleftmenu.pvcconn)</script></FONT></B></P></TD>
          <TD width=8 bgColor=#000000 height=25>&nbsp;</TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=171 height=25>&nbsp;</TD>
          <TD width=226 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>

<% support_invmatch("MULTIPVC_SUPPORT", "1", "<!--"); %>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(stapvcconn.connect)</script>:&nbsp;</FONT></TD>
	  <TD height=25 width=226>&nbsp;<SELECT name="status_connection" onChange=SelCONNECTION(this.form.status_connection.selectedIndex,this.form)><% status_connection_table("select"); %></SELECT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
<% support_invmatch("MULTIPVC_SUPPORT", "1", "-->"); %>

        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(stapvcconn.encaps)</script>:&nbsp;</FONT></TD>
          <TD height=25>&nbsp;<FONT 
            style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("encapmode", 10); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
	  
	   <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(stapvcconn.multiplex)</script>:&nbsp;</FONT></TD>
          <TD height=25>&nbsp;<FONT 
            style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("multiplex"); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>

	   <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(stapvcconn.qos)</script>:&nbsp;</FONT></TD>
          <TD height=25>&nbsp;<FONT 
            style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("qos"); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>

	   <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(stapvcconn.pcrrate)</script>:&nbsp;</FONT></TD>
          <TD height=25>&nbsp;<FONT 
            style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("pcr"); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>

	   <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(stapvcconn.scrrate)</script>:&nbsp;</FONT></TD>
          <TD height=25>&nbsp;<FONT 
            style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("scr"); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>

	   <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(stapvcconn.autodetect)</script>:&nbsp;</FONT></TD>
          <TD height=25>&nbsp;<FONT 
            style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("autodetect", 10); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>

        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><span ><script>Capture(stapvcconn.vpi)</script>:&nbsp;</span></FONT></TD>
          <TD height=25><b>&nbsp;<% nvram_dslstatus_get("vpi"); %>&nbsp;</b></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>

        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><span >VCI:</span><FONT style="FONT-SIZE: 8pt"></FONT></TD>
          <TD height=25><b>&nbsp;<% nvram_dslstatus_get("vci"); %>&nbsp;</b></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
	
	<TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(stapvcconn.enable)</script>:&nbsp;</FONT></TD>
          <TD height=25>&nbsp;<FONT 
            style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("applyonboot", 10); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
  
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(stapvcconn.pvcstatus)</script>:&nbsp;</FONT></TD>
          <TD height=25>&nbsp;<FONT 
            style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("pvcstatus", 10); %><%nvram_dslstatus_get("pvcerrstr"); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
  
        </TBODY></TABLE></TD>
    <TD vAlign=top width=176 bgColor=#6666cc>
    <p style="line-height: 200%"><span >&nbsp;&nbsp;&nbsp; </span><font color="#FFFFFF">
          <span ><a target="_blank" href="help/HRouterStatus.asp"><script>Capture(share.more)</script>...</a></span></font></TD></TR>
  
   <TR>
<TD width=809 colSpan=2>
<TABLE cellSpacing=0 cellPadding=0 border=0>

<TR>
<TD width=156 bgColor=#e7e7e height=30>&nbsp;</TD>
<TD width=8 ><IMG src='image/UI_04.gif' border=0 width=8 height=30></TD>
<TD width=16>&nbsp;</TD>
<TD width=12>&nbsp;</TD>
<TD width=411>&nbsp;</TD>
<TD width=15>&nbsp;</TD>
<TD width=15 ><IMG src='image/UI_05.gif' border=0 width=15 height=30></TD>
<TD width=176 bgColor=#6666cc rowSpan=2><IMG height=64 
src='image/UI_Cisco.gif' width=176 border=0></TD></TR>
<TR>
<TD width=156 bgColor=#000000>&nbsp;</TD>
<TD width=8 bgColor=#000000>&nbsp;</TD>
<TD width=16 bgColor=#6666cc>&nbsp;</TD>
<TD width=12 bgColor=#6666cc>&nbsp;</TD>
<TD width=411 bgColor=#6666cc>
<TABLE height=19 cellSpacing=0 cellPadding=0 width=220 align=right border=0>


<TR>
<TD align=middle width=171>&nbsp;</TD>
<TD align=middle width=8 bgColor=#6666cc>&nbsp;</TD>
<TD align=middle width=103 bgColor=#434a8f><FONT color=#ffffff><B><A href='Status_DSL.asp'><script>Capture(share.refreshs)</script></A></B></FONT></TD>
</TR>
</TABLE></TD>
<TD width=15 bgColor=#6666cc height=33>&nbsp;</TD>
<TD width=15 bgColor=#000000 height=33>&nbsp;</TD>
</TR></TABLE></TD></TR>


  </TBODY></TABLE></FORM></DIV></BODY></HTML>
