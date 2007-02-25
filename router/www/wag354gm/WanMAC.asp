
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

<HTML><HEAD><TITLE>WanMAC</TITLE>
<% no_cache(); %>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<link rel="stylesheet" type="text/css" href="style.css">
<style fprolloverstyle>
A:hover {color: #00FFFF}
.small A:hover {color: #00FFFF}
</style>
<SCRIPT src="common.js"></SCRIPT>
<SCRIPT language=JavaScript>
<% wireless_support(5); %>share.productname = share.productname1;<% wireless_support(6); %>
function to_submit(F)
{	
	F.submit_button.value = "WanMAC";
	F.action.value = "Apply";
       	F.submit();
	
}

function SelCONNECTION(num,F)
{
        F.submit_button.value = "WanMAC";
        F.change_action.value = "gozila_cgi";
        F.status_connection.value=F.status_connection.options[num].value;
        F.submit();
}

function CloneMAC(F)
{
       	F.submit_button.value = "WanMAC";
      	F.change_action.value = "gozila_cgi";
	F.submit_type.value = "clone_mac";
       	F.action.value = "Apply";
       	F.submit();
}
function SelMac(num,F)
{
        mac_enable_disable(F,num);
}
function mac_enable_disable(F,I)
{
        EN_DIS3 = I;
        if ( I == 0 )
	{
                //choose_disable(F.macclone_config);
                choose_disable(F.macclone_addr_0);
                choose_disable(F.macclone_addr_1);
                choose_disable(F.macclone_addr_2);
                choose_disable(F.macclone_addr_3);
                choose_disable(F.macclone_addr_4);
                choose_disable(F.macclone_addr_5);
                choose_disable(F.clone_b);
        }
        else
	{
                //choose_enable(F.macclone_config);
                choose_enable(F.macclone_addr_0);
                choose_enable(F.macclone_addr_1);
                choose_enable(F.macclone_addr_2);
                choose_enable(F.macclone_addr_3);
                choose_enable(F.macclone_addr_4);
                choose_enable(F.macclone_addr_5);
                choose_enable(F.clone_b);
        }
}
function init()
{       
	if(document.mac.macclone_enable[0].checked == true)       
        	mac_enable_disable(document.mac,1);
	else if(document.mac.macclone_enable[1].checked == true)       
        	mac_enable_disable(document.mac,0);
}
</SCRIPT>
</HEAD>
<BODY onload="init()">
<DIV align=center>
<FORM name=mac method=<% get_http_method(); %> action=apply.cgi>
<input type=hidden name=submit_button>
<input type=hidden name=change_action>
<input type=hidden name=submit_type>
<input type=hidden name=action>
<TABLE cellSpacing=0 cellPadding=0 width=809 border=0>
  <TBODY>
  <TR>
    <TD width=95><IMG src="image/UI_Linksys.gif" 
      border=0 width="165" height="57"></TD>
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
      style="FONT-SIZE: 15pt" face=Arial color=#ffffff><script>Capture(bmenu.setup)</script></FONT></H3></TD>
    <TD 
    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
    vAlign=center borderColor=#000000 width=646 bgColor=#000000 height=49>
      <TABLE 
      style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; BORDER-COLLAPSE: collapse; FONT-VARIANT: normal" 
      height=33 cellSpacing=0 cellPadding=0 bgColor=#6666cc border=0>
        <TBODY>
        <TR>
          <TD style="FONT-WEIGHT: bolder; FONT-SIZE: 10pt" align=right 
          bgColor=#6666cc height=33><FONT color=#ffffff><script>Capture(share.productname)</script>&nbsp;&nbsp;</FONT></TD>
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
                  src="image/UI_07.gif" width=83 border=0></TD>
                <TD width=73 height=1><IMG height=10 
                  src="image/UI_06.gif" width=83 border=0></TD>
                <TD width=113 height=1><IMG height=10 
                  src="image/UI_06.gif" width=83 border=0></TD>
                <TD width=103 height=1><IMG height=10 
                  src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=85 height=1><IMG height=10 
                  src="image/UI_06.gif" width=100 border=0></TD>
                <TD width=115 height=1><IMG height=10 
                  src="image/UI_06.gif" width=115 border=0></TD>
                <TD width=74 height=1><IMG height=10 
                  src="image/UI_06.gif" width=79 border=0></TD></TR>
              <TR>
                <TD align=middle bgColor=#6666cc height=20><FONT 
                  style="FONT-WEIGHT: 700" color=#ffffff><A 
                  style="TEXT-DECORATION: none" 
                  href="index.asp"><script>Capture(bmenu.setup)</script></A></FONT></TD>
                <TD align=middle bgColor=#000000 height=20><FONT 
                  style="FONT-WEIGHT: 700" color=#ffffff>
                <a style="TEXT-DECORATION: none" href="Wireless_Basic.asp"><script>Capture(bmenu.wireless)</script></a></FONT></TD>
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
                  <a style="TEXT-DECORATION: none" href="Management.asp"><script>Capture(bmenu.admin)</script></a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>
                <TD align=middle bgColor=#000000 height=20>
                  <P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" 
                  color=#ffffff>
                  <a style="TEXT-DECORATION: none" href="Status_Router.asp"><script>Capture(bmenu.statu)</script></a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>
              </TR>
              <TR>
                <TD width=643 bgColor=#6666cc colSpan=7 height=21>
                  <TABLE borderColor=black height=21 cellSpacing=0 cellPadding=0>
                    <TBODY>
                    <TR align=center>
                      <TD width=40></TD>
                      <TD class=small width=135><A href="index.asp"><script>Capture(settopmenu.basicsetup)</script></A></TD>
                      <TD> <P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD width=75></TD>
                      <TD class=small width=100><A href="DDNS.asp"><script>Capture(settopmenu.ddns)</script></A></TD>
                      <TD> <P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD width=22></TD>
                      <TD width=153><FONT style="COLOR: white"><script>Capture(setother.macaddrclone)</script></FONT></TD>
                      <TD> <P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD width=30></TD>
                      <TD class=small width=140><A href="Routing.asp"><script>Capture(settopmenu.advrouting)</script></A></TD>
<% support_invmatch("HSIAB_SUPPORT", "1", "<!--"); %>
                      <TD><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD width=30></TD>
                      <TD class=small width=140><A href="HotSpot_Admin.asp"><script>Capture(settopmenu.hotspot)</script></TD>
<% support_invmatch("HSIAB_SUPPORT", "1", "-->"); %>
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
      <TABLE cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=156 bgColor=#000000 colSpan=3 height=25>
            <P align=right><B><FONT style="FONT-SIZE: 9pt" face=Arial 
            color=#ffffff><script>Capture(setother.macclone)</script></FONT></B></P></TD>
          <TD width=8 bgColor=#000000 height=25>&nbsp;</TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=397 height=25 colspan="2">&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>       
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=24>&nbsp;</TD>
          <TD width=14 height=24>&nbsp;</TD>
          <TD width=17 height=24>&nbsp;</TD>
          <TD width=13 height=24>&nbsp;</TD>
          <TD width=60 height=24>&nbsp;<script>Capture(share.connection)</script>:</TD>
          <TD height=24 width=296><SELECT name="status_connection" onChange=SelCONNECTION(this.form.status_connection.selectedIndex,this.form)><% status_connection_table("select"); %></SELECT></TD>  
          <TD width=13 height=24>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif
          height=24>&nbsp;</TD></TR>

        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=24>&nbsp;</TD>
          <TD width=14 height=24>&nbsp;</TD>
          <TD width=17 height=24>&nbsp;</TD>
          <TD width=13 height=24>&nbsp;</TD>
          <TD width=397 height=24 colspan="2"><INPUT type=radio value=1 name=macclone_enable <% macclone_config("macclone_enable", 1); %> OnClick=SelMac(1,this.form)><B><script>Capture(share.enabled)</script></B>&nbsp;&nbsp;
                <INPUT type=radio value=0 name=macclone_enable <% macclone_config("macclone_enable", 0); %> OnClick=SelMac(0,this.form)><B><script>Capture(share.disabled)</script></B></TD>
          <TD width=13 height=24>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=24>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=24>&nbsp;</TD>
          <TD width=14 height=24>&nbsp;</TD>
          <TD width=17 height=24>&nbsp;</TD>
          <TD width=13 height=24>&nbsp;</TD>
          <TD width=101 height=24>&nbsp;<script>Capture(setother.userdefine)</script>:</TD>
          <TD width=296 height=24><input type=hidden name="macclone_config">
                                  <input name="macclone_addr_0" value='<% macclone_config("macclone_addr", 0); %>' size=2 maxlength=2 onBlur=valid_mac(this,0) class=num> :
                                  <input name="macclone_addr_1" value='<% macclone_config("macclone_addr", 1); %>' size=2 maxlength=2 onBlur=valid_mac(this,1) class=num> :
                                  <input name="macclone_addr_2" value='<% macclone_config("macclone_addr", 2); %>' size=2 maxlength=2 onBlur=valid_mac(this,1) class=num> :
                                  <input name="macclone_addr_3" value='<% macclone_config("macclone_addr", 3); %>' size=2 maxlength=2 onBlur=valid_mac(this,1) class=num> :
                                  <input name="macclone_addr_4" value='<% macclone_config("macclone_addr", 4); %>' size=2 maxlength=2 onBlur=valid_mac(this,1) class=num> :
                                  <input name="macclone_addr_5" value='<% macclone_config("macclone_addr", 5); %>' size=2 maxlength=2 onBlur=valid_mac(this,1) class=num>
          <TD width=13 height=24>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=24>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=4></TD>
          <TD width=200 height=25><INPUT onclick="CloneMAC(this.form)" type=button value="Clone Your PC's MAC" name=clone_b></TD>
          <TD></TD>
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
                <TD width=410 colSpan=3 height=1>
                  <HR color=#b5b5e6 SIZE=1>
                </TD>
                <TD width=15>&nbsp;</TD></TR></TBODY></TABLE></TD>
          <TD width=15 background=image/UI_05.gif height=1>&nbsp;</TD></TR>
        <TR>
          <TD width=44 bgColor=#e7e7e7 height=5>&nbsp;</TD>
          <TD width=65 bgColor=#e7e7e7 height=5>&nbsp;</TD>
          <TD width=47 bgColor=#e7e7e7 height=5>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=5>&nbsp;</TD>
          <TD width=454 colSpan=6 height=5></TD>
          <TD width=15 background=image/UI_05.gif height=5>&nbsp;</TD></TR></TBODY></TABLE></TD>
    <TD vAlign=top width=176 bgColor=#6666cc>
      <TABLE cellSpacing=0 cellPadding=0 width=176 border=0>
        <TBODY>
        <TR>
          <TD width=11 bgColor=#6666cc height=25>&nbsp;</TD>
          <TD width=156 bgColor=#6666cc height=25><font color="#FFFFFF">
          <span ><a target="_blank" href="help/HMAC.asp"></a></span></font></TD>
          <TD width=9 bgColor=#6666cc height=25>&nbsp;</TD></TR></TBODY></TABLE></TD></TR>
  <TR>
    <TD width=809 colSpan=2>
      <TABLE cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=30>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif>&nbsp;</TD>
          <TD colSpan=3>&nbsp;</TD>
          <TD width=15>
          <TD width=15 background=image/UI_05.gif>&nbsp;</TD>
          <TD width=176 bgColor=#6666cc rowSpan=2>
          <IMG src="image/UI_Cisco.gif" border=0 width="176" height="64"></TD></TR>
        <TR>
          <TD width=156 bgColor=#000000>&nbsp;</TD>
          <TD width=8 bgColor=#000000>&nbsp;</TD>
          <TD width=16 bgColor=#6666cc>&nbsp;</TD>
          <TD width=12 bgColor=#6666cc>&nbsp;</TD>
          <TD width=411 bgColor=#6666cc>
            <TABLE height=19 cellSpacing=0 cellPadding=0 align=right border=0>
            <TBODY>
              <TR>
                <TD align=middle>
                <INPUT type=button value="Save Settings" onClick=to_submit(this.form)></FONT></TD>
                <TD align=middle width=8 bgColor=#6666cc>&nbsp;</TD>
                <TD align=middle><INPUT type=button value="Cancel Changes" name=cancel onclick=window.location.reload()></TD>
                </TR></TBODY></TABLE></TD>
          <TD width=15 bgColor=#6666cc height=33>&nbsp;</TD>
          <TD width=15 bgColor=#000000 height=33>&nbsp;</TD>
          </TR>
          </TBODY>
          </TABLE>
          </TD>
          </TR>
          </TBODY>
          </TABLE>
          </FORM>
          </DIV>
          </BODY>
          </HTML>
