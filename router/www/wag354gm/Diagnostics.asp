
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

<HTML><HEAD><TITLE>Diagnostics</TITLE>
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
<SCRIPT language="Javascript" type="text/javascript" src="capadmin.js"></SCRIPT>
<SCRIPT language=JavaScript>
document.title=(admtopmenu.diag);
<% wireless_support(5); %>share.productname = share.productname1;<% wireless_support(6); %>
var value=0;
function Refresh()
{
        var refresh_time = "<% show_ping_status("refresh_time"); %>";
        if(refresh_time == "")  refresh_time = 8000;
        if(value >= 1)
	{
                window.location.replace("Diagnostics.asp");
        }
	value ++;
}


function to_submit(F)
{
	F.action.value = "Apply";
	F.submit();
	return true;
}

function PingStart(F)
{
	F.change_action.value = "gozila_cgi";
	F.submit_type.value = "startping";
	F.submit();
	Refresh();
	return true;
}
</SCRIPT>
</HEAD>
<BODY>
<DIV align=center>
<FORM name=F1 method=<% get_http_method(); %> action=apply.cgi>
<input type=hidden name=ping_config>
<input type=hidden name=submit_button value="Diagnostics">
<input type=hidden name=change_action>
<input type=hidden name=action>
<input type=hidden name=submit_type>
<TABLE cellSpacing=0 cellPadding=0 width=809 border=0>
  <TBODY>
  <TR>
    <TD width=95><IMG src="image/UI_Linksys.gif" border=0 width="165" height="57"></TD>
    <TD vAlign=bottom align=right width=714 bgColor=#6666cc><FONT style="FONT-SIZE: 7pt" color=#ffffff><FONT face=Arial><script>Capture(share.firmwarever)</script>:&nbsp;<% get_firmware_version(); %>&nbsp;&nbsp;&nbsp;</FONT></FONT></TD></TR>
  <TR>
    <TD width=808 bgColor=#6666cc colSpan=2><IMG height=11 src="image/UI_10.gif" width=809 border=0></TD></TR>
  </TBODY>
</TABLE>
<TABLE height=77 cellSpacing=0 cellPadding=0 width=809 bgColor=black border=0>
  <TBODY>
  <TR>
    <TD style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" borderColor=#000000 align=middle width=163 height=49>
      <H3 style="MARGIN-TOP: 1px; MARGIN-BOTTOM: 1px"><FONT style="FONT-SIZE: 15pt" face=Arial color=#ffffff><script>Capture(bmenu.admin)</script></FONT></H3></TD>
    <TD style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" vAlign=center borderColor=#000000 width=646 bgColor=#000000 height=49>
      <TABLE style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; BORDER-COLLAPSE: collapse; FONT-VARIANT: normal" height=33 cellSpacing=0 cellPadding=0 bgColor=#6666cc border=0>
        <TBODY>
        <TR>
          <TD style="FONT-WEIGHT: bolder; FONT-SIZE: 10pt" align=right bgColor=#6666cc height=33><FONT color=#ffffff><% wireless_support(1); %><script>Capture(share.wireless_g)</script><% wireless_support(2); %><script>Capture(share.productname)</script>&nbsp;&nbsp;</FONT></TD>
          <TD borderColor=#000000 borderColorLight=#000000 align=middle width=109 bgColor=#000000 borderColorDark=#000000 height=12 rowSpan=2><FONT color=#ffffff><SPAN style="FONT-SIZE: 8pt"><B><% nvram_get("router_name"); %></B></SPAN></FONT></TD></TR>
        <TR>
          <TD style="FONT-WEIGHT: normal; FONT-SIZE: 1pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" width=537 bgColor=#000000 height=1>&nbsp;</TD></TR>
        <TR>
          <TD width=646 bgColor=#000000 colSpan=2 height=32>
            <TABLE id=AutoNumber1 style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; BORDER-COLLAPSE: collapse; FONT-VARIANT: normal" height=6 cellSpacing=0 cellPadding=0 width=637 border=0>
              <TBODY>
              <TR style="BORDER-RIGHT: medium none; BORDER-TOP: medium none; FONT-WEIGHT: normal; FONT-SIZE: 1pt; BORDER-LEFT: medium none; COLOR: black; BORDER-BOTTOM: medium none; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" align=middle bgColor=#6666cc>
                <TD width=83  height=1><IMG height=10 src="image/UI_06.gif" width=83  border=0></TD>
                <TD width=83  height=1><IMG height=10 src="image/UI_06.gif" width=83  border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="<% wireless_support(3); %>" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="<% wireless_support(4); %>" width=103 border=0></TD>
                <TD width=68  height=1><IMG height=10 src="image/UI_06.gif" width=68  border=0></TD></TR>
              <TR>
                <TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><A style="TEXT-DECORATION: none" href="index.asp"><script>Capture(bmenu.setup)</script></A></FONT></TD>
                <% wireless_support(1); %><TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Wireless_Basic.asp"><script>Capture(bmenu.wireless)</script></a></FONT></TD><% wireless_support(2); %>
                <TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Firewall.asp"><script>Capture(bmenu.security)</script></a></FONT></TD>
                <TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="<% support_elsematch("PARENTAL_CONTROL_SUPPORT", "1", "Parental_Control.asp", "Filters.asp"); %>"><script>Capture(bmenu.accrestriction)</script></a></FONT></TD>
                <TD align=middle bgColor=#000000 height=20><P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Forward_UPnP.asp"><script>Capture(bmenu.application)</script> <BR>&amp; <script>Capture(bmenu.gaming)</script></a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>
                <TD align=middle bgColor=#6666cc height=20><P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" color=#ffffff>&nbsp;&nbsp;<script>Capture(bmenu.admin)</script>&nbsp;&nbsp;</FONT></P></TD>
                <TD align=middle bgColor=#000000 height=20><P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Status_Router.asp"><script>Capture(bmenu.statu)</script></a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD></TR>
   		<TR>
                <TD width=643 bgColor=#6666cc colSpan=7 height=21>
                  <TABLE borderColor=black height=21 cellSpacing=0 cellPadding=0>
                    <TBODY>
                     <TR align=center>
                      <TD class=small width=80><A href="Management.asp"><script>Capture(admtopmenu.manage)</script></A></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=80><A href="Log.asp"><script>Capture(admtopmenu.report)</script></A></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD width=80><FONT style="COLOR: white"><script>Capture(admtopmenu.diag)</script></FONT></TD>
<% support_elsematch("BACKUP_RESTORE_SUPPORT","1","","<!--"); %>
		      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD  class=small width=110><A href="Backup_Restore.asp"><script>Capture(admtopmenu.bakrest)</script></A></TD>
<% support_elsematch("BACKUP_RESTORE_SUPPORT","1","","-->"); %>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small  width=110><A href="Factory_Defaults.asp"><script>Capture(admtopmenu.facdef)</script></A></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=120><A href="Upgrade.asp"><script>Capture(admtopmenu.upgrade)</script></A></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=63><A href='Reboot.asp'><script>Capture(admtopmenu.reboot)</script></A></TD>
</TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>
<TABLE height=5 cellSpacing=0 cellPadding=0 width=806 bgColor=black border=0>
  <TBODY>
  <TR bgColor=black>
    <TD borderColor=#e7e7e7 width=163 bgColor=#e7e7e7 height=1><IMG height=15 src="image/UI_03.gif" width=164 border=0></TD>
    <TD width=646 height=1><IMG height=15 src="image/UI_02.gif" width=645 border=0></TD></TR></TBODY></TABLE>
<TABLE id=AutoNumber9 style="BORDER-COLLAPSE: collapse" borderColor=#111111 height=23 cellSpacing=0 cellPadding=0 width=809 border=0>
  <TBODY>
  <TR>
    <TD width=633>
      <TABLE cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD align=right width=156 bgColor=#000000 colSpan=3 height=25><B><FONT style="FONT-SIZE: 9pt" face=Arial color=#ffffff><script>Capture(admleftmenu.ping)</script></FONT></B></TD>
          <TD width=8 bgColor=#000000 height=25>&nbsp;</TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;</TD>
          <TD width=296 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25><p align="right"><font style="font-weight: 700"><span >&nbsp;<script>Capture(admleftmenu.pingparam)</script></span></font></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD width=14 height=25></TD>
          <TD width=17 height=25></TD>
          <TD width=13 height=25></TD>
          <TD width=101 height=25><SPAN  style="FONT-SIZE: 8pt"><script>Capture(admping.targetip)</script>:</SPAN></TD>
          <TD width=296 height=25>
         <INPUT class=num maxLength=3  size=3 value='<% ping_config("ping_ipaddr", 0); %>' name="ping_ipaddr_0" onBlur=valid_range(this,0,223,"IP") <% ping_config("ping_ipaddr", 4); %>> . 
		<INPUT class=num maxLength=3  size=3 value='<% ping_config("ping_ipaddr", 1); %>' name="ping_ipaddr_1" onBlur=valid_range(this,0,255,"IP") <% ping_config("ping_ipaddr", 4); %> > . 
		<INPUT class=num maxLength=3  size=3 value='<% ping_config("ping_ipaddr", 2); %>' name="ping_ipaddr_2" onBlur=valid_range(this,0,255,"IP") <% ping_config("ping_ipaddr", 4); %> > . 
		<INPUT class=num maxLength=3  size=3  value='<% ping_config("ping_ipaddr", 3); %>' name=ping_ipaddr_3 onBlur=valid_range(this,1,254,"IP") <% ping_config("ping_ipaddr", 4); %> ></TD>
          <TD width=13 height=25></TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
    
         <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD width=14 height=25></TD>
          <TD width=17 height=25></TD>
          <TD width=13 height=25></TD>
          <TD width=101 height=25><SPAN  style="FONT-SIZE: 8pt"><script>Capture(admping.size)</script>:</SPAN></TD>
          <TD width=296 height=25>
         <INPUT class=num style='FONT-SIZE: 10pt; FONT-FAMILY: Courier' maxLength=4  size=4 name=ping_size onBlur=valid_range(this,60,1514,admping.size)  value='<% ping_config("ping_size", 0); %>'>&nbsp;<script>Capture(admping.bytes)</script></TD>
          <TD width=13 height=25></TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
         <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD width=14 height=25></TD>
          <TD width=17 height=25></TD>
          <TD width=13 height=25></TD>
          <TD width=101 height=25><SPAN  style="FONT-SIZE: 8pt"><script>Capture(admping.numbers)</script>:</SPAN></TD>
          <TD width=296 height=25>
         <INPUT class=num style='FONT-SIZE: 10pt; FONT-FAMILY: Courier' maxLength=4  size=4 name=ping_number value='<% ping_config("ping_number", 0); %>' onBlur=valid_range(this,1,100,admping.numbers)> <script>Capture(admping.range)</script></TD>
          <TD width=13 height=25></TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD width=14 height=25></TD>
          <TD width=17 height=25></TD>
          <TD width=13 height=25></TD>
          <TD width=101 height=25><SPAN  style="FONT-SIZE: 8pt"><script>Capture(admping.interval)</script>:</SPAN></TD>
          <TD width=296 height=25>
         <INPUT class=num style='FONT-SIZE: 10pt; FONT-FAMILY: Courier' onBlur=valid_range(this,0,9999,admping.interval) maxLength=4  size=5 name=ping_interval value='<% ping_config("ping_interval", 0); %>'>&nbsp;<script>Capture(admping.milliseconds)</script> </TD>
          <TD width=13 height=25></TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD width=14 height=25></TD>
          <TD width=17 height=25></TD>
          <TD width=13 height=25></TD>
          <TD width=101 height=25><SPAN  style="FONT-SIZE: 8pt"><script>Capture(admping.timeout)</script>:</SPAN></TD>
          <TD width=296 height=25>
         <INPUT class=num style='FONT-SIZE: 10pt; FONT-FAMILY: Courier' maxLength=4 onBlur=valid_range(this,0,9999,admping.timeout) size=5 name=ping_timeout value='<% ping_config("ping_timeout", 0); %>'>&nbsp;<script>Capture(admping.milliseconds)</script> </TD>
          <TD width=13 height=25></TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD width=14 height=25></TD>
          <TD width=17 height=25></TD>
          <TD width=13 height=25></TD>
          <TD width=101 height=25></TD>
          <TD width=296 height=25>
         <input type=hidden name=hid_pingStart value=$[hid_pingStart]>
	    <script>document.write("<INPUT type=button value=\"" + admbutton.starttest + "\" onClick=PingStart(F1)>");</script></TD>
          <TD width=13 height=25></TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
            
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD width=14 height=25></TD>
          <TD width=17 height=25></TD>
          <TD width=13 height=25></TD>
          <TD width=101 height=25><SPAN  style="FONT-SIZE: 8pt"><script>Capture(admping.result)</script>:</SPAN></TD>
          <TD width=296 height=25><b>&nbsp;<% dump_ping_log(); %></b></TD>
          <TD width=13 height=25></TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
            
          
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=1>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=6>
            <TABLE>
              <TBODY>
              <TR>
                <TD width=16 height=1>&nbsp;</TD>
                <TD width=13 height=1>&nbsp;</TD>
                <TD width=410 colSpan=3 height=1>&nbsp;</TD>
                <TD width=15 height=1>&nbsp;</TD></TR></TBODY></TABLE></TD>
          <TD width=15 background=image/UI_05.gif height=1>&nbsp;</TD></TR>
        <TR>
          <TD width=44 bgColor=#e7e7e7 height=5>&nbsp;</TD>
          <TD width=65 bgColor=#e7e7e7 height=5>&nbsp;</TD>
          <TD width=47 bgColor=#e7e7e7 height=5>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=5>&nbsp;</TD>
          <TD width=14 height=5></TD>
          <TD width=17 height=5></TD>
          <TD width=13 height=5></TD>
          <TD width=101 height=5></TD>
          <TD width=296 height=5></TD>
          <TD width=13 height=5></TD>
          <TD width=15 background=image/UI_05.gif height=5>&nbsp;</TD></TR></TBODY></TABLE></TD>
          <TD vAlign=top width=176 bgColor=#6666cc>
          <TABLE cellSpacing=0 cellPadding=0 width=176 border=0>
          <TBODY>
                <TR>
                <TD width=11 bgColor=#6666cc height=25>&nbsp;</TD>
                <TD width=156 bgColor=#6666cc height=25><a target="_blank" href="help/HPingTest.asp"><script>Capture(share.more)</script>...</a></TD>
                <TD width=9 bgColor=#6666cc height=25>&nbsp;</TD></TR></TBODY></TABLE></TD></TR>
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
                <TD align=middle bgColor=#42498c width="101"><FONT color=#ffffff><B><A href='javascript:to_submit(F1);'><script>Capture(share.saves)</script></A></B></FONT>
                </TD>
                <TD align=middle width=8 bgColor=#6666cc>&nbsp;</TD>
                <TD align=middle bgColor=#434a8f width="111"><FONT color=#ffffff><B><A href='javascript:window.location.reload();'><script>Capture(share.cancels)</script></A></B></FONT></TD>
                <TD align=middle width=4 bgColor=#6666cc>&nbsp;</TD>
                </TR></TBODY></TABLE></TD>
                <TD width=15 bgColor=#6666cc height=33>&nbsp;</TD>
                <TD width=15 bgColor=#000000 height=33>&nbsp;</TD>
              </TR>
            </TBODY>
            </TABLE>
          </TD>
        </TR>
      </FORM>
    </TBODY>
  </TABLE>
</DIV>
</BODY>
</HTML>
