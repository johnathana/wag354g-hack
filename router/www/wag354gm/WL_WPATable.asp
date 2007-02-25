
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

<HTML><HEAD><TITLE>WPA</TITLE>

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
<SCRIPT language=javascript>
<% wireless_support(5); %>share.productname = share.productname1;<% wireless_support(6); %>
function SelMode(num,F)
{
	F.submit_button.value = "WL_WPATable";
        F.change_action.value = "gozila_cgi";
        F.security_mode.value=F.security_mode.options[num].value;
        F.submit();
}

function to_submit(F)
{
	if(valid_value(F)){
		F.submit_button.value = "WL_WPATable";
		F.action.value = "Apply";
		//F.security_mode_last.value = F.security_mode.value;
		//F.wl_wep_last.value = F.wl_wep.value;
        	F.submit();
	}
}

function valid_value(F)
{
	if(F.security_mode.value == "disabled")	return true;
	if(!valid_wpa_psk(F) || !valid_wep(F) || !valid_radius(F))	return false;
	else	return true;
}

function valid_radius(F)

{
	if(F.security_mode.value == "radius" || F.security_mode.value == "wpa"){
		if(F.wl_radius_key.value == ""){
			//alert("Please enter a Shared Key!");
			alert(errmsg.err25);
			F.wl_radius_key.focus();
			return false;
		}
		if(!valid_ip(F,"F.wl_radius_ipaddr","Radius%20Server%20Address",ZERO_NO|MASK_NO))
			return false;
	}
	return true;
}

function isxdigit1(I,M)
{
	for(i=0 ; i<I.value.length; i++){
		ch = I.value.charAt(i).toLowerCase();
		if(ch >= '0' && ch <= '9' || ch >= 'a' && ch <= 'f'){}
		else{
			//alert(M +' have illegal hexadecimal digits or over 63 characters!');
			alert(M + errmsg.err26);
			I.value = I.defaultValue;	
			return false;
		}
	}
	return true;
}


function valid_wpa_psk(F)
{
	if(F.security_mode.value == "psk"){
		if(F.wl_wpa_psk.value.length >=8 && F.wl_wpa_psk.value.length <= 63 ){
			if(!isascii(F.wl_wpa_psk,F.wl_wpa_psk.value)) return false;
		}
		else{
			//alert("Invalid Key, must be between 8 and 63 ASCII characters");
			alert(errmsg.err27);
			return false;
		}
	}
	return true;	
}

function valid_wep(F)
{
	if(document.forms[0].security_mode.value == "psk" || document.forms[0].security_mode.value == "wpa")
		return true;
	if (ValidateKey(F.wl_key1, F.wl_wep_bit.options[F.wl_wep_bit.selectedIndex].value,1) == false)
		return false;  
  	if (ValidateKey(F.wl_key2, F.wl_wep_bit.options[F.wl_wep_bit.selectedIndex].value,2) == false)
       		return false;
	if (ValidateKey(F.wl_key3, F.wl_wep_bit.options[F.wl_wep_bit.selectedIndex].value,3) == false)
		return false;
	if (ValidateKey(F.wl_key4, F.wl_wep_bit.options[F.wl_wep_bit.selectedIndex].value,4) == false)
		return false;
	for (var i=1; i <= 4; i++) {
		if(F.wl_key[i-1].checked){
			aaa = eval("F.wl_key"+i).value;
			//if(aaa == "" && F.security_mode.value == "wep"){
			if(aaa == ""){
                       		//alert('You have to enter a key for Key '+i);
				alert(errmsg.err28 + i);
                		return false;
			}
			break;
		} 
	}
    return true;
}

function ValidateKey(key, bit, index)
{
	if(bit == 64){
		switch(key.value.length){
			case 0: break;
			case 10: if(!isxdigit(key,key.value)) return false; break;
//			default: alert("Invalid Length in key " + key.value); return false;
			default: alert(errmsg.err29 + key.value); return false;
		}
	}
	else{
		switch(key.value.length){
			case 0: break;
			case 26: if(!isxdigit(key,key.value)) return false; break;
//			default: alert("Invalid Length in key " + key.value); return false;
			default: alert(errmsg.err29 + key.value); return false;
		}
	}
	return true;
}

function keyMode(num)
{
	var keylength;
	var key1='';
	var key2='';
	var key3='';
	var key4='';
	if(num == 0 || num == 64)
		keylength = 40 /4;
	else
		keylength = 104 /4;
	document.forms[0].wl_key1.maxLength = keylength;
	document.forms[0].wl_key2.maxLength = keylength;
	document.forms[0].wl_key3.maxLength = keylength;
	document.forms[0].wl_key4.maxLength = keylength;
	for (var i=0; i<keylength; i++)
	{
    		key1 +=  document.forms[0].wl_key1.value.charAt(i);
	    	key2 +=  document.forms[0].wl_key2.value.charAt(i);
    		key3 +=  document.forms[0].wl_key3.value.charAt(i);
    		key4 +=  document.forms[0].wl_key4.value.charAt(i);
	}
	document.forms[0].wl_key1.value = key1;
	document.forms[0].wl_key2.value = key2;
	document.forms[0].wl_key3.value = key3;
	document.forms[0].wl_key4.value = key4;
}

function generateKey(F)

{		
	if (F.wl_passphrase.value == "")
	{
		//alert("Please enter a Passphase!");
		alert(errmsg.err30);
		F.wl_passphrase.focus();
		return false;
	}
	F.submit_button.value = "WL_WPATable";
        F.change_action.value = "gozila_cgi";
	if(F.wl_wep_bit.value == 64)
		F.submit_type.value = "key_64";
	else
		F.submit_type.value = "key_128";
        F.submit();
}

function init()
{
	if(document.forms[0].security_mode.value == "wep" || document.forms[0].security_mode.value == "radius")
        	//keyMode(<% nvram_gozila_get("wl_wep_bit"); %>);
        	keyMode(document.wpa.wl_wep_bit.value);
}

</SCRIPT>
</HEAD>
<BODY onload=init()>
<DIV align=center>
<FORM name=wpa method=<% get_http_method(); %> action=apply.cgi>
<input type=hidden name=submit_button>
<input type=hidden name=change_action>
<input type=hidden name=submit_type>
<input type=hidden name=action>
<input type=hidden name=security_mode_last>
<input type=hidden name=wl_wep_last>
<TABLE cellSpacing=0 cellPadding=0 width=809 border=0>
  <TBODY>
  <TR>
    <TD width=95><IMG src="image/UI_Linksys.gif" border=0 width="165" height="57"></TD>
    <TD vAlign=bottom align=right width=714 bgColor=#6666cc><FONT style="FONT-SIZE: 7pt" color=#ffffff><FONT face=Arial>
	<script>Capture(share.firmwarever)</script>:&nbsp;<% get_firmware_version(); %>&nbsp;&nbsp;&nbsp;</FONT></FONT></TD></TR>
  <TR>
    <TD width=808 bgColor=#6666cc colSpan=2><IMG height=11 src="image/UI_10.gif" width=809 border=0></TD></TR></TBODY></TABLE>
<TABLE height=77 cellSpacing=0 cellPadding=0 width=809 bgColor=black border=0>
  <TBODY>
  <TR>
    <TD style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
	borderColor=#000000 align=middle width=163 height=49><H3 style="MARGIN-TOP: 1px; MARGIN-BOTTOM: 1px">
	<FONT style="FONT-SIZE: 15pt" face=Arial color=#ffffff><script>Capture(bmenu.wireless)</script></FONT></H3></TD>
    <TD style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
	vAlign=center borderColor=#000000 width=646 bgColor=#000000 height=49>
	<TABLE style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; BORDER-COLLAPSE: collapse; FONT-VARIANT: normal" height=33 cellSpacing=0 cellPadding=0 bgColor=#6666cc border=0>
        <TBODY>
        <TR>
          <TD style="FONT-WEIGHT: bolder; FONT-SIZE: 10pt" align=right bgColor=#6666cc height=33><FONT color=#ffffff>
		<% wireless_support(1); %><script>Capture(share.wireless_g)</script><% wireless_support(2); %><script>Capture(share.productname)</script>&nbsp;&nbsp;</FONT></TD>
          <TD borderColor=#000000 borderColorLight=#000000 align=middle width=109 bgColor=#000000 borderColorDark=#000000 height=12 rowSpan=2>
		<FONT color=#ffffff><SPAN style="FONT-SIZE: 8pt"><B><% nvram_get("router_name"); %></B></SPAN></FONT></TD></TR>
        <TR>
          <TD style="FONT-WEIGHT: normal; FONT-SIZE: 1pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
		width=537 bgColor=#000000 height=1>&nbsp;</TD></TR>
        <TR>
          <TD width=646 bgColor=#000000 colSpan=2 height=32>
            <TABLE id=AutoNumber1 style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; BORDER-COLLAPSE: collapse; FONT-VARIANT: normal" height=6 cellSpacing=0 cellPadding=0 width=637 border=0>
              <TBODY>
              <TR style="BORDER-RIGHT: medium none; BORDER-TOP: medium none; FONT-WEIGHT: normal; FONT-SIZE: 1pt; BORDER-LEFT: medium none; COLOR: black; BORDER-BOTTOM: medium none; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" align=middle bgColor=#6666cc>
                <TD width=83  height=1><IMG height=10 src="image/UI_06.gif" width=83  border=0></TD>
                <TD width=83  height=1><IMG height=10 src="image/UI_07.gif" width=83  border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=68  height=1><IMG height=10 src="image/UI_06.gif" width=68  border=0></TD></TR>
              <TR>
		<TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff>
			<A style="TEXT-DECORATION: none" href="index.asp"><script>Capture(bmenu.setup)</script></A></FONT></TD>
                <TD align=middle bgColor=#6666cc height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff>
			<script>Capture(bmenu.wireless)</script></FONT></TD>
                <TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff>
			<a style="TEXT-DECORATION: none" href="Firewall.asp"><script>Capture(bmenu.security)</script></a></FONT></TD>
                <TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff>
			<a style="TEXT-DECORATION: none" href="<% support_elsematch("PARENTAL_CONTROL_SUPPORT", "1", "Parental_Control.asp", "Filters.asp"); %>">
				<script>Capture(bmenu.accrestriction)</script></a></FONT></TD>
                <TD align=middle bgColor=#000000 height=20><P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" color=#ffffff>
			<a style="TEXT-DECORATION: none" href="Forward_UPnP.asp"><script>Capture(bmenu.application)</script> 
				<BR>&amp; <script>Capture(bmenu.gaming)</script></a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>
                <TD align=middle bgColor=#000000 height=20><P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" color=#ffffff>
			<a style="TEXT-DECORATION: none" href="Management.asp">&nbsp;&nbsp;<script>Capture(bmenu.admin)</script></a>&nbsp;&nbsp;</FONT></P></TD>
                <TD align=middle bgColor=#000000 height=20><P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" color=#ffffff>
			<a style="TEXT-DECORATION: none" href="Status_Router.asp"><script>Capture(bmenu.statu)</script></a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>
	      </TR>
	      </TR>
	      <TR>
                <TD width=643 bgColor=#6666cc colSpan=7 height=21>
                  <TABLE borderColor=black height=21 cellSpacing=0 cellPadding=0>
                    <TBODY>
                    <TR align=center>
                      <TD class=small width=165><a href="Wireless_Basic.asp"><script>Capture(wltopmenu.basicsetup)</script></a></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=165><FONT style="COLOR: white"><script>Capture(wltopmenu.wlsecurity)</script></FONT></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=165><a href="Wireless_MAC.asp"><script>Capture(wltopmenu.wlaccess)</script></a></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=210><a href="Wireless_Advanced.asp"><script>Capture(wltopmenu.advwlsetting)</script></a></FONT></TD>
                    </TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>
<TABLE height=5 cellSpacing=0 cellPadding=0 width=806 bgColor=black border=0>
  <TBODY>
  <TR bgColor=black>
    <TD style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
		borderColor=#e7e7e7 width=163 bgColor=#e7e7e7 height=1><IMG height=15 src="image/UI_03.gif" width=164 border=0></TD>
    <TD style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
		width=646 height=1><IMG height=15 src="image/UI_02.gif" width=645 border=0></TD></TR></TBODY></TABLE>
<TABLE id=AutoNumber9 style="BORDER-COLLAPSE: collapse" borderColor=#111111 height=23 cellSpacing=0 cellPadding=0 width=809 border=0>
  <TBODY>
  <TR>
    <TD width=633>
      <TABLE cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=156 bgColor=#000000 colSpan=3 height=25>
            <P align=right><B><FONT style="FONT-SIZE: 9pt" face=Arial color=#ffffff><script>Capture(wltopmenu.wlsecurity)</script></FONT></B></P></TD>
          <TD width=8 bgColor=#000000 height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</TD>
          <TD width=101 height=25>&nbsp;</TD>
          <TD width=296 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <!--TR>
          <TD width=156 bgColor=#000000 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 bgColor=#000000 height=25>&nbsp;</TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;</TD>
          <TD width=296 height=25><INPUT type=radio value="enabled" name="security_mode" <% nvram_invmatch("security_mode", "disabled", "checked"); %>><B>Enable</B>&nbsp;&nbsp;<INPUT type=radio value="disabled" name="security_mode" <% nvram_match("security_mode", "disabled", "checked"); %>><B>Disable</B></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR-->
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</TD>
          <TD width=101 height=25><script>Capture(wlnetwork.secmode)</script>:</TD>
          <TD width=296 height=25>&nbsp;<SELECT onChange=SelMode(this.form.security_mode.selectedIndex,this.form) name=security_mode> 
                  <OPTION value="disabled" <% nvram_selmatch("security_mode", "disabled", "selected"); %>><script>Capture(share.disabled)</script></OPTION>
                  <OPTION value="psk" <% nvram_selmatch("security_mode", "psk", "selected"); %>><script>Capture(wlnetwork.prekey)</script></OPTION>
<!--
                  <OPTION value="wpa" <% nvram_selmatch("security_mode", "wpa", "selected"); %>><script>Capture(wlnetwork.radius)</script></OPTION> 
-->
<!-- wwzh
                  <option value="radius" <% nvram_selmatch("security_mode", "radius", "selected"); %>>RADIUS</option>
wwzh -->
                  <option value="wep" <% nvram_selmatch("security_mode", "wep", "selected"); %>><script>Capture(wlnetwork.wep)</script></option>
                </SELECT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <% show_wpa_setting(); %>               
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</TD>
          <TD width=101 height=25>&nbsp;</TD>
          <TD width=296 height=25><B>&nbsp;</B></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=1>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=6>
            <TABLE>
              <TBODY>
              <TR>
                <TD width=20 height=1>&nbsp;</TD>
                <TD width=410 colSpan=3 height=1><HR color=#b5b5e6 SIZE=1></TD>
		<TD width=20 height=1>&nbsp;</TD>
		</TR></TBODY></TABLE></TD>
          <TD width=15 background=image/UI_05.gif height=1>&nbsp;</TD></TR>
        <TR>
          <TD width=44 bgColor=#e7e7e7 height=5>&nbsp;</TD>
          <TD width=65 bgColor=#e7e7e7 height=5>&nbsp;</TD>
          <TD width=47 bgColor=#e7e7e7 height=5>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=5>&nbsp;</TD>
	  <TD colSpan=3 height=25>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</TD>
          <TD width=101 height=5>&nbsp;</TD>
          <TD width=296 height=5>&nbsp;</TD>
          <TD width=13 height=5>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=5>&nbsp;</TD></TR></TBODY></TABLE></TD>
    <TD vAlign=top width=176 bgColor=#6666cc>
      <TABLE cellSpacing=0 cellPadding=0 width=176 border=0>
        <TBODY>
        <TR>
          <TD width=11 bgColor=#6666cc height=25>&nbsp;</TD>
          <TD width=156 bgColor=#6666cc height=25><font color="#FFFFFF">
		<span ><a target="_blank" href="help/HWPA.asp"><script>Capture(share.more)</script>...</a></span></font></TD>
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
          <TD width=15>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif>&nbsp;</TD>
          <TD width=176 bgColor=#6666cc rowSpan=2><IMG src="image/UI_Cisco.gif" border=0 width="176" height="64"></TD></TR>
        <TR>
          <TD width=156 bgColor=#000000>&nbsp;</TD>
          <TD width=8 bgColor=#000000>&nbsp;</TD>
          <TD width=16 bgColor=#6666cc>&nbsp;</TD>
          <TD width=12 bgColor=#6666cc>&nbsp;</TD>
          <TD width=411 bgColor=#6666cc>
            <TABLE height=19 cellSpacing=0 cellPadding=0 align=right border=0 width="216"><TBODY>
              <TR>
                <TD align=middle bgColor=#42498c width="101"><FONT color=#ffffff>
			<B><A href='javascript:to_submit(document.forms[0])'><script>Capture(share.saves)</script></A></B></FONT></TD>
                <TD align=middle width=8 bgColor=#6666cc>&nbsp;</TD>
                <TD align=middle width="103" bgColor=#434a8f><FONT color=#ffffff>
			<B><A href=WL_WPATable.asp><script>Capture(share.cancels)</script></A></B></FONT></TD>
                <TD align=middle width=4 bgColor=#6666cc>&nbsp;</TD>
                </TR></TBODY></TABLE></TD>
          <TD width=15 bgColor=#6666cc height=33>&nbsp;</TD>
          <TD width=15 bgColor=#000000 height=33>&nbsp;</TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></FORM></DIV></BODY></HTML>
