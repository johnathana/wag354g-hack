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
var EN_DIS = '<% nvram_else_match("wl_gmode","-1","0","1"); %>';
function SelWL(num,F)
{
	if (num == 0)
		I = '0';
	else
		I = '1';
	wl_enable_disable(F,I);
}
function wl_enable_disable(F,I)
{
	EN_DIS = I;
	if( I == "0"){
		choose_disable(F.wl_ssid);
		choose_disable(F.wl_channel);
		choose_disable(F.wl_closed[0]);
		choose_disable(F.wl_closed[1]);
	}
	else{
		choose_enable(F.wl_ssid);
		choose_enable(F.wl_channel);
		choose_enable(F.wl_closed[0]);
		choose_enable(F.wl_closed[1]);
	}
}

function to_submit(F)
{
	if(F.wl_ssid.value == ""){
                //alert("You must input a SSID!");
		alert(errmsg.err23);

                F.wl_ssid.focus();
                return false;
        }

	F.submit_button.value = "Wireless_Basic";
	F.action.value = "Apply";
	F.submit();
}

function validSSID(I,start,end,M)
{
	valid_comma(I,M);
	valid_length(I,start,end,M);
}
function valid_comma(I,M)
{
	var i, len;
	
	len = I.value.length;
        M1 = unescape(M);
        if((I.value.indexOf('\''))>=0)
        {
                alert(M1 +' can\'t include \' ');	
		I.value = I.defaultValue;
		return false;
	}

}
function valid_length(I,start,end,M)
{
	M1 = unescape(M);


	d = I.value.length;
	if ( !(d<=end && d>=start) )
	{
		//alert(M1 +' length is out of range ['+ start + ' - ' + end +']');
		alert(M1 + errmsg.err23 + start + ' - ' + end +']');

		I.value = I.defaultValue;
	}
}

function init()
{
	wl_enable_disable(document.wireless,'<% nvram_else_match("wl_gmode","-1","0","1"); %>');
}
</SCRIPT>

</HEAD>
<BODY onload=init()>
<DIV align=center>
<FORM name=wireless method=<% get_http_method(); %> action=apply.cgi>
<input type=hidden name=submit_button value="Wireless_Basic">
<input type=hidden name=action value="Apply">

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
      style="FONT-SIZE: 15pt" face=Arial color=#ffffff><script>Capture(bmenu.wireless)</script></FONT></H3></TD>
    <TD 
    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
    vAlign=center borderColor=#000000 width=646 bgColor=#000000 height=49>
      <TABLE 
      style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; BORDER-COLLAPSE: collapse; FONT-VARIANT: normal" 
      height=33 cellSpacing=0 cellPadding=0 bgColor=#6666cc border=0>
        <TBODY>
        <TR>
          <TD style="FONT-WEIGHT: bolder; FONT-SIZE: 10pt" align=right 
          bgColor=#6666cc height=33><FONT color=#ffffff><% wireless_support(1); %><script>Capture(share.wireless_g)</script><% wireless_support(2); %><script>Capture(share.productname)</script>&nbsp;&nbsp;</FONT></TD>
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
                <TD width=83  height=1><IMG height=10 src="image/UI_07.gif" width=83  border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=68  height=1><IMG height=10 src="image/UI_06.gif" width=68  border=0></TD></TR>
              <TR>
                <TD align=middle bgColor=#000000 height=20><FONT 
                  style="FONT-WEIGHT: 700" color=#ffffff><A 
                  style="TEXT-DECORATION: none" 
                  href="index.asp"><script>Capture(bmenu.setup)</script></A></FONT></TD>
                <TD align=middle bgColor=#6666cc height=20><FONT 
                  style="FONT-WEIGHT: 700" color=#ffffff><script>Capture(bmenu.wireless)</script></FONT></TD>
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
                      <TD class=small width=165><FONT style="COLOR: white"><script>Capture(wltopmenu.basicsetup)</script></FONT></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=165><a href="WL_WPATable.asp"><script>Capture(wltopmenu.wlsecurity)</script></a></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=165><a href="Wireless_MAC.asp"><script>Capture(wltopmenu.wlaccess)</script></a></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=210><a href="Wireless_Advanced.asp"><script>Capture(wltopmenu.advwlsetting)</script></a></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>
<TABLE height=5 cellSpacing=0 cellPadding=0 width=806 bgColor=black border=0>
  <TBODY>
  <TR>
    <TD borderColor=#e7e7e7 width=163 bgColor=#e7e7e7 height=1><IMG height=15 
      src="image/UI_03.gif" width=164 border=0></TD>
    <TD width=646 height=1><IMG height=15 src="image/UI_02.gif" 
      width=645 border=0></TD></TR></TBODY></TABLE>
<TABLE id=AutoNumber9 style="BORDER-COLLAPSE: collapse" borderColor=#111111 
height=23 cellSpacing=0 cellPadding=0 width=809 border=0>
  <TBODY>
  <TR>
    <TD width=633>
      <TABLE cellSpacing=0 cellPadding=0 border=0 width="633">
        <TBODY>
        <TR>
          <TD width=156 bgColor=#000000 colSpan=3 height=25>
            <P align=right><B>
                <font face="Arial" color="#FFFFFF" style="font-size: 9pt">
                <script>Capture(wlleftmenu.wlnetwork)</script></font></B></P></TD>
          <TD width=8 bgColor=black height=25>&nbsp;</TD>
          <TD width=6>&nbsp;</TD>
          <TD width=7>&nbsp;</TD>
          <TD width=8>&nbsp;</TD>
          <TD width=156>&nbsp;</TD>
          <TD width=262>&nbsp;</TD>
          <TD width=15>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif>&nbsp;</TD></TR>
         
        <TR>
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 
            height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="21">&nbsp;</TD>
          <TD width=156 height=25><font face="Arial" style="font-size: 9pt"><script>Capture(wlnetwork.wlnetmode)</script>:</font></TD>
          <TD width=262 height=25>&nbsp;&nbsp;<font face=verdana size=2><b><select name="wl_gmode" onChange=SelWL(this.form.wl_gmode.selectedIndex,this.form)>
	  		<option value="-1" <% nvram_match("wl_gmode", "-1", "selected"); %>><script>Capture(share.disabled)</script></option>
	  		<option value="1" <% nvram_match("wl_gmode", "1", "selected"); %>><script>Capture(wlnetwork.mixed)</script></option>
	  		<option value="0" <% nvram_match("wl_gmode", "0", "selected"); %>><script>Capture(wlnetwork.bonly)</script></option>
			<option value="2" <% nvram_match("wl_gmode", "2", "selected"); %>><script>Capture(wlnetwork.gonly)</script></option>
		</select></TD>
          <TD width=15 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 
          height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="21">&nbsp;</TD>
          <TD width=156 height=25><font face="Arial" style="font-size: 9pt"><script>Capture(wlnetwork.wlnetname)</script>(SSID):</font></TD>
          <TD width=262 height=25>&nbsp;&nbsp;<INPUT maxLength=32 value='<% nvram_get("wl_ssid"); %>' name="wl_ssid" size="20"  onBlur=validSSID(this,2,32,'SSID')></font></TD>
          <TD width=15 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 
          height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="21">&nbsp;</TD>
          <TD width=156 height=25><font face="Arial" style="font-size: 9pt"><script>Capture(wlnetwork.wlchannel)</script>:</font></TD>
          <TD width=262 height=25>&nbsp;&nbsp;<select name="wl_channel" onFocus="check_action(this,0)">
<script language=javascript>
	var max_channel = '<% get_wl_max_channel(); %>';
	var wl_channel = '<% nvram_get("wl_channel"); %>';
	var buf = "";
	//var freq = new Array("","2.412","2.417","2.422","2.427","2.432","2.437","2.442","2.447","2.452","2.457","2.462","2.467","2.472");
	var freq = new Array("","412","417","422","427","432","437","442","447","452","457","462","467","472");
	for(i=1 ; i<=max_channel ; i++){
		if(i == wl_channel)	buf = "selected";
		else			buf = "";
		//document.write("<option value="+i+" "+buf+">"+i+" - "+freq[i]+"GHz</option>");
		document.write("<option value="+i+" "+buf+">"+i+" - "+"2"+setinetsetup.symbol+freq[i]+" GHz</option>");
	}
</script>
		  </select></TD>
          <TD width=15 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 
            height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="21">&nbsp;</TD>
          <TD width=156 height=25><font face="Arial" style="font-size: 9pt"><script>Capture(wlnetwork.ssid)</script>:</font></TD>
          <TD width=262 height=25>
                <table border="1" cellpadding="0" cellspacing="0" style="border-collapse: collapse; border-width: 0" bordercolor="#111111" id="AutoNumber17" width="146">
                  <tr>
                    <td style="border-style: none; border-width: medium" height="25" width="146">
          <span ></span><INPUT type=radio value=0 name=wl_closed <% nvram_match("wl_closed","0","checked"); %>><b><script>Capture(share.enabled)</script></b>&nbsp;<INPUT type=radio value=1 name=wl_closed <% nvram_match("wl_closed","1","checked"); %>><b><script>Capture(share.disabled)</script></td>
                  </tr>
                </table>
                </TD>
          <TD width=15 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>                                              
<% support_invmatch("WL_STA_SUPPORT", "1", "<!--"); %>
        <TR>
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 
            height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="21">&nbsp;</TD>
          <TD width=156 height=25><font face="Arial" style="font-size: 9pt"><script>Capture(wlnetwork.ssidap)</script>:</font></TD>
          <TD width=262 height=25>&nbsp;&nbsp;<INPUT maxLength=32 value='<% nvram_get("wl_ap_ssid"); %>' name="wl_ap_ssid" size="20"  onBlur=validSSID(this,2,32,'SSID')></TD>
          <TD width=15 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>                                              
        <TR>
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 
            height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="21">&nbsp;</TD>
          <TD width=156 height=25><font face="Arial" style="font-size: 9pt"><script>Capture(wlnetwork.dfap)</script></font></TD>
          <TD width=262 height=25>&nbsp;<input type=hidden name="wl_ap_ip" value=4>
                <INPUT class=num maxLength=3 size=3 value='<% get_single_ip("wl_ap_ip","0"); %>' name="wl_ap_ip_0" onBlur=valid_range(this,0,255,"IP")> .
                <INPUT class=num maxLength=3 size=3 value='<% get_single_ip("wl_ap_ip","1"); %>' name="wl_ap_ip_1" onBlur=valid_range(this,0,255,"IP")> .
                <INPUT class=num maxLength=3 size=3 value='<% get_single_ip("wl_ap_ip","2"); %>' name="wl_ap_ip_2" onBlur=valid_range(this,0,255,"IP")> .
                <INPUT class=num maxLength=3 size=3 value='<% get_single_ip("wl_ap_ip","3"); %>' name="wl_ap_ip_3" onBlur=valid_range(this,0,254,"IP")></TD>
          <TD width=15 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>                                              
<% support_invmatch("WL_STA_SUPPORT", "1", "-->"); %>
        </TBODY></TABLE></TD>
    <TD vAlign=top width=176 bgColor=#6666cc>
      <TABLE cellSpacing=0 cellPadding=0 width=176 border=0>
        <TBODY>
        <TR>
          <TD width=11 bgColor=#6666cc height=25>&nbsp;</TD>
          <TD width=156 bgColor=#6666cc height=25><font color="#FFFFFF">
          <span ><a target="_blank" href="help/HWireless.asp"><script>Capture(share.more)</script>...</a></span></font></TD>
          <TD width=9 bgColor=#6666cc 
  height=25>&nbsp;</TD></TR></TBODY></TABLE></TD></TR>
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
          <TD width=176 bgColor=#6666cc rowSpan=2>
          <IMG 
            src="image/UI_Cisco.gif" border=0 width="176" height="64"></TD></TR>
        <TR>
          <TD width=156 bgColor=#000000>&nbsp;</TD>
          <TD width=8 bgColor=#000000>&nbsp;</TD>
          <TD width=16 bgColor=#6666cc>&nbsp;</TD>
          <TD width=12 bgColor=#6666cc>&nbsp;</TD>
          <TD width=411 bgColor=#6666cc>
            <TABLE height=19 cellSpacing=0 cellPadding=0 width=220 align=right 
            border=0>
              <TBODY>
              <TR>
                <TD align=middle bgColor=#42498c width=101><FONT color=#ffffff>
                 <B><A href='javascript:to_submit(document.forms[0])'><script>Capture(share.saves)</script></A></B></FONT></TD>
                <TD align=middle width=8 bgColor=#6666cc>&nbsp;</TD>
                <TD align=middle width=111 bgColor=#434a8f><FONT color=#ffffff>
                    <B><A href=Wireless_Basic.asp><script>Capture(share.cancels)</script></A></B></FONT></TD>
                </TR></TBODY></TABLE></TD>
          <TD width=15 bgColor=#6666cc height=33>&nbsp;</TD>
          <TD width=15 bgColor=#000000 
  height=33>&nbsp;</TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></FORM></DIV></BODY></HTML>
