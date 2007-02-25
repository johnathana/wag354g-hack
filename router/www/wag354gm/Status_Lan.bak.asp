
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
<SCRIPT language=JavaScript>
document.title=(setddns.statu);
var dhcp_win = null;
function ViewDHCP()
{
	dhcp_win = self.open('DHCPTable.asp','inLogTable','alwaysRaised,resizable,scrollbars,width=720,height=600');
	dhcp_win.focus();
}
var arp_win = null;
function ViewARP()
{
	arp_win = self.open('ARPTable.asp','inLogTable','alwaysRaised,resizable,scrollbars,width=720,height=600');
	arp_win.focus();
}

function exit()
{
        closeWin(dhcp_win);
        closeWin(arp_win);
}

var value=0;
function Refresh()
{
	var refresh_time = "<% show_status("refresh_time"); %>";
	if(refresh_time == "")	refresh_time = 30000;
	if (value>=1)
	{
		window.location.replace("Status_Lan.asp");
	}
	value++;
	timerID=setTimeout("Refresh()",refresh_time);
}

function init()
{
	Refresh();
}


</SCRIPT>

</HEAD>
<BODY onunload=exit()>
<DIV align=center>
<TABLE cellSpacing=0 cellPadding=0 width=809 border=0>
  <TBODY>
  <TR>
    <TD width=95><IMG src="image/UI_Linksys.gif" border=0 width="165" height="57"></TD>
    <TD vAlign=bottom align=right width=714 bgColor=#6666cc><FONT 
      style="FONT-SIZE: 7pt" color=#ffffff><FONT face=Arial>Firmware Version:&nbsp;<% get_firmware_version(); %>&nbsp;&nbsp;&nbsp;</FONT></FONT></TD></TR>
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
      style="FONT-SIZE: 15pt" face=Arial color=#ffffff>Status</FONT></H3></TD>
    <TD 
    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
    vAlign=center borderColor=#000000 width=646 bgColor=#000000 height=49>
      <TABLE 
      style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; BORDER-COLLAPSE: collapse; FONT-VARIANT: normal" 
      height=33 cellSpacing=0 cellPadding=0 bgColor=#6666cc border=0>
        <TBODY>
        <TR>
          <TD style="FONT-WEIGHT: bolder; FONT-SIZE: 10pt" align=right 
          bgColor=#6666cc height=33><FONT color=#ffffff><span >
          <% wireless_support(1); %>Wireless-G<% wireless_support(2); %> ADSL</span> Gateway&nbsp;&nbsp;</FONT></TD>
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
                <TD width=83 height=1><IMG height=10 
                  src="image/UI_06.gif" width=83 border=0></TD>
                <TD width=73 height=1><IMG height=10 
                  src="image/UI_06.gif" width=83 border=0></TD>
                <TD width=113 height=1><IMG height=10 
                  src="image/UI_06.gif" width=83 border=0></TD>
                <TD width=103 height=1><IMG height=10 
                  src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=85 height=1><IMG height=10 
                  src="image/UI_06.gif" width=100 border=0></TD>
                <TD width=115 height=1><IMG height=10 
                  src="<% wireless_support(3); %>" width=115 border=0></TD>
                <TD width=74 height=1><IMG height=10 
                  src="<% wireless_support(4); %>" width=79 border=0></TD></TR>
              <TR>
                <TD align=middle bgColor=#000000 height=20><FONT 
                  style="FONT-WEIGHT: 700" color=#ffffff><A 
                  style="TEXT-DECORATION: none" 
                  href="index.asp">Setup</A></FONT></TD>
                <% wireless_support(1); %><TD align=middle bgColor=#000000 height=20><FONT 
                  style="FONT-WEIGHT: 700" color=#ffffff>
                <a style="TEXT-DECORATION: none" href="Wireless_Basic.asp">Wireless</a></FONT></TD><% wireless_support(2); %>
                <TD align=middle bgColor=#000000 height=20><FONT 
                  style="FONT-WEIGHT: 700" color=#ffffff>
                <a style="TEXT-DECORATION: none" href="Firewall.asp">Security</a></FONT></TD>
                <TD align=middle bgColor=#000000 height=20><FONT 
                  style="FONT-WEIGHT: 700" color=#ffffff>
                <a style="TEXT-DECORATION: none" href="<% support_elsematch("PARENTAL_CONTROL_SUPPORT", "1", "Parental_Control.asp", "Filters.asp"); %>">Access Restrictions</a></FONT></TD>
                <TD align=middle bgColor=#000000 height=20>
                  <P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" 
                  color=#ffffff>
                  <a style="TEXT-DECORATION: none" href="Forward_UPnP.asp">Applications 
                  <BR>&amp; Gaming</a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>
                <TD align=middle bgColor=#000000 height=20>
                  <P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" 
                  color=#ffffff>
                  <a style="TEXT-DECORATION: none" href="Management.asp">Administration</a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>
                <TD align=middle bgColor=#6666cc height=20>
                  <P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" 
                  color=#ffffff>Status&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>
              </TR>
              <TR>
                <TD width=643 bgColor=#6666cc colSpan=7 height=21>
                  <TABLE borderColor=black height=21 cellSpacing=0 cellPadding=0>
                    <TBODY>
                    <TR align=left>
                      <TD width=25></TD>
                      <TD class=small width=65><A href="Status_Router.asp">Gateway</A></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD width=25></TD>
                      <TD width=100><FONT style="COLOR: white">Local Network</FONT></TD>
                      <% wireless_support(1); %><TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD width=25></TD>
                      <TD class=small width=70><span >&nbsp;</span><A href="Status_Wireless.asp">Wireless</A></TD><% wireless_support(2); %>
		      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
		      <TD width=25></TD>
                      <TD class=small width=100><span >&nbsp;</span><A href="Status_DSL.asp">DSL Connection</A></TD>
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
            color=#ffffff>Local Network</FONT></B></P></TD>
          <TD width=8 bgColor=#000000 height=25>&nbsp;</TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;</TD>
          <TD width=296 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>
          <p align="right">&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD><FONT style="FONT-SIZE: 8pt"><span >MAC</span> 
          Address:</FONT></TD>
          <!--<TD>&nbsp;<FONT 
            style="FONT-SIZE: 8pt"><B><% nvram_get("lan_hwaddr"); %></B></FONT></TD>-->
         <TD>&nbsp;<FONT 
            style="FONT-SIZE: 8pt"><B><% hwaddr_get("lan"); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD><FONT style="FONT-SIZE: 8pt">IP Address:</FONT></TD>
          <TD>&nbsp;<FONT style="FONT-SIZE: 8pt"><B><% nvram_get("lan_ipaddr"); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt">Subnet Mask:</FONT></TD>
          <TD width=296 height=25>&nbsp;<FONT 
            style="FONT-SIZE: 8pt"><B><% nvram_get("lan_netmask"); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt">DHCP Server:</FONT></TD>
          <TD height=25>&nbsp;<FONT 
            style="FONT-SIZE: 8pt"><B><% lan_dhcp_setting(""); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>

<% nvram_invmatch("lan_proto", "dhcp", "<!--"); %>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><span >Start IP Address</span>:</FONT></TD>
          <TD height=25><b>&nbsp;<% prefix_ip_get("lan_ipaddr",1); %><% nvram_get("dhcp_start"); %>&nbsp;</b></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
        <!--   guohong  2004.07.13  -->
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25 colspan="2"><INPUT id=button2 onclick="ViewARP()" type=button  value="  ARP/RARP   Table" name=arp_table </TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>  
          
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><span >End IP Address</span><FONT style="FONT-SIZE: 8pt">:</FONT></TD>
          <TD height=25>&nbsp;<b><script language=javascript>
	var prefix = "<% prefix_ip_get("lan_ipaddr",1); %>";
	var start = <% nvram_get("dhcp_start"); %>;
	var num = <% nvram_get("dhcp_num"); %>;
	document.write(prefix);
	document.write(start+num-1);
          </script>
&nbsp;</b></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR> 
<% nvram_invmatch("lan_proto", "dhcp", "-->"); %>
		<TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=5 height=25><HR color=#b5b5e6 SIZE=1></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25 colspan="2"><INPUT id=button1 onclick="ViewDHCP()" type=button <% nvram_invmatch("lan_proto", "dhcp", "disabled"); %> value="DHCP Clients Table" name=dhcp_table </TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
		<TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25 colspan="2">&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>


        
        </TBODY></TABLE></FORM></TD>
    <TD vAlign=top width=176 bgColor=#6666cc>
    <p style="line-height: 200%"><span >&nbsp;&nbsp;&nbsp; </span><font color="#FFFFFF">
          <span ><a target="_blank" href="help/HRouterStatus.asp">More...</a></span></font></TD></TR>
  <!--
  <TR>
    <TD width=809 colSpan=2>
      <TABLE cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=30>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif>&nbsp;</TD>
          <TD width=131>&nbsp;</TD>
          <TD width=323>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif>&nbsp;</TD>
          <TD width=176 bgColor=#6666cc rowSpan=2>
          <IMG 
            src="image/UI_Cisco.gif" border=0 width="176" height="64"></TD></TR>
        <TR>
          <TD width=156 bgColor=#000000>&nbsp;</TD>
          <TD width=8 bgColor=#000000>&nbsp;</TD>
          <TD width=131 bgColor=#6666cc>&nbsp;</TD>
          <TD width=323 bgColor=#6666cc>
            <div align="right">
            <TABLE height=19 cellSpacing=0 cellPadding=0 
              border=0 width="41" style="border-collapse: collapse" bordercolor="#111111"><TBODY>
              <TR>
                <TD align=middle width="61"><INPUT onclick="window.location.replace('Status_Lan.asp')" type=button value=Refresh></TD>
                <TD align=middle width=4 bgColor=#6666cc>&nbsp;</TD>
                </TR></TBODY></TABLE>
            </div>
          </TD>
          <TD width=15 bgColor=#000000 
  height=33>&nbsp;</TD></TR></TBODY></TABLE></TD></TR>
  -->
   <TR>
<TD width=809 colSpan=2>
<TABLE cellSpacing=0 cellPadding=0 border=0>

<TR>
<TD width=156 bgColor=#e7e7e height=30>&nbsp;</TD>
<TD width=8 ><IMG src='image/UI_04.gif' border=0 width=8 height=30></TD>
<TD width=16>&nbsp;</TD>
<TD width=12>&nbsp;</TD>
<TD width=411>&nbsp;</TD>
<TD width=15>
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
<TD align=middle width=101>&nbsp;</TD>
<TD align=middle width=8 bgColor=#6666cc>&nbsp;</TD>
<TD align=middle width=103 bgColor=#434a8f><FONT color=#ffffff><B><A href='Status_Lan.asp'>Refresh</A></B></FONT></TD>
</TR>
</TABLE></TD>
<TD width=15 bgColor=#6666cc height=33>&nbsp;</TD>
<TD width=15 bgColor=#000000 height=33>&nbsp;</TD>
</TR></TABLE></TD></TR>


  </TBODY></TABLE></FORM></DIV></BODY></HTML>


