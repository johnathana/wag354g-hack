
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

<HTML><HEAD><TITLE>Setup</TITLE>

<% no_cache(); %>

<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<link rel="stylesheet" type="text/css" href="style.css">

<style fprolloverstyle>

A:hover {color: #00FFFF}

.small A:hover {color: #00FFFF}

</style>

<script src="common.js"></script>
<script language="JavaScript" type="text/javascript" src="share.js"></script>
<script language="JavaScript" type="text/javascript" src="capsetup.js"></script>

<SCRIPT language=JavaScript>
document.title=(bmenu.setup);
<% wireless_support(5); %>share.productname = share.productname1;<% wireless_support(6); %>
function ddns_check(F,T)

{

	if(F.ddns_enable.value == 0)

		return true;

	else if(F.ddns_enable.value == 1){

		username = eval("F.ddns_username");

		passwd = eval("F.ddns_username");

		hostname = eval("F.ddns_hostname");

	}

	else if(F.ddns_enable.value == 2){

		username = eval("F.ddns_username_"+F.ddns_enable.value);

		passwd = eval("F.ddns_username_"+F.ddns_enable.value);

		hostname = eval("F.ddns_hostname_"+F.ddns_enable.value);

	}



	if(username.value == ""){

	//	alert("You must input a username!");
		alert(errmsg.err3);
		username.focus();

		return false;

	}

	if(passwd.value == ""){

		//alert("You must input a password!");
		alert(errmsg.err4);
		passwd.focus();

		return false;

	}

	if(hostname.value == ""){

		//alert("You must input a hostname!");
		alert(errmsg.err12);

		hostname.focus();

		return false;

	}

	return true;

}

function to_save(F)

{

	if(ddns_check(F,"update") == true){

		F.change_action.value = "gozila_cgi";

		F.submit_button.value = "DDNS";

		F.submit_type.value = "save";

       		F.action.value = "Apply";

       		F.submit();

	}

}

function to_submit(F)

{

	if(ddns_check(F,"save") == true){

		F.submit_button.value = "DDNS";

      		F.action.value = "Apply";

		F.submit();

	}

}

function SelDDNS(num,F)

{

        F.submit_button.value = "DDNS";

        F.change_action.value = "gozila_cgi";

        F.ddns_enable.value=F.ddns_enable.options[num].value;

        F.submit();

}

</SCRIPT>

</HEAD>

<BODY>

<DIV align=center>

<FORM name=ddns method=<% get_http_method(); %> action=apply.cgi>

<input type=hidden name=submit_button>

<input type=hidden name=action>

<input type=hidden name=change_action>

<input type=hidden name=submit_type>

<TABLE cellSpacing=0 cellPadding=0 width=809 border=0>

  <TBODY>

  <TR>

    <TD width=95>

    <IMG src="image/UI_Linksys.gif" 

      border=0 width="165" height="57"></TD>

    <TD vAlign=bottom align=right width=714 bgColor=#6666cc><FONT 

      style="FONT-SIZE: 7pt" color=#ffffff><FONT face=Arial><script>Capture(share.firmwarever)</script>:&nbsp;<% get_firmware_version(); %>&nbsp;&nbsp;&nbsp;</FONT></FONT></TD></TR>

  <TR>

    <TD width=808 bgColor=#6666cc colSpan=2>

    <IMG height=11 

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
                <TD width=83  height=1><IMG height=10 src="image/UI_07.gif" width=83  border=0></TD>
                <TD width=83  height=1><IMG height=10 src="image/UI_06.gif" width=83  border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=68  height=1><IMG height=10 src="image/UI_06.gif" width=68  border=0></TD></TR>
              <TR>

                <TD align=middle bgColor=#6666cc height=20><FONT 

                  style="FONT-WEIGHT: 700" color=#ffffff>

                <A 

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

                <TD align=middle bgColor=#000000 height=20>

                  <P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" 

                  color=#ffffff>

                  <a style="TEXT-DECORATION: none" href="Status_Router.asp"><script>Capture(bmenu.statu)</script></a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>

              </TR>

                <TD width=643 bgColor=#6666cc colSpan=7 height=21>

                  <TABLE borderColor=black height=21 cellSpacing=0 cellPadding=0>

                    <TBODY>

                    <TR align=center>

                      <TD class=small width=140> <A href="index.asp"><script>Capture(settopmenu.basicsetup)</script></A></TD>

                      <TD> <P class=bar><font color='white'><b>|</b></font></P></TD>

                      <TD class=small width=100><FONT style="COLOR: white"><script>Capture(settopmenu.ddns)</script></FONT></TD>

                      <TD> <P class=bar><font color='white'><b>|</b></font></P></TD>

                      <!--<TD class=small width=153> <A href="WanMAC.asp">MAC Address Clone</A></TD>

                      <TD> <P class=bar><font color='white'><b>|</b></font></P></TD>
-->
                      <TD class=small width=140> <A href="Routing.asp"><script>Capture(settopmenu.advrouting)</script></A></TD>

<% support_invmatch("DHCP_DOMAIN_IP_MAPPING_SUPPORT", "1", "<!--"); %>
                      <TD><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=100><A href="VOIP.asp"><script>Capture(settopmenu.voip)</script></A></TD>
<% support_invmatch("DHCP_DOMAIN_IP_MAPPING_SUPPORT", "1", "-->"); %>

<% support_invmatch("HSIAB_SUPPORT", "1", "<!--"); %>
                      <TD><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=120><A href="HotSpot_Admin.asp"><script>Capture(settopmenu.hotspot)</script></TD>
<% support_invmatch("HSIAB_SUPPORT", "1", "-->"); %>

		</TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>

<TABLE height=5 cellSpacing=0 cellPadding=0 width=806 bgColor=black border=0>

  <TBODY>

  <TR bgColor=black>

    <TD 

    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 

    borderColor=#e7e7e7 width=163 bgColor=#e7e7e7 height=1>

    <IMG height=15 

      src="image/UI_03.gif" width=164 border=0></TD>

    <TD 

    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 

    width=646 height=1>

    <IMG height=15 src="image/UI_02.gif" width=645 

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

            color=#ffffff><script>Capture(setleftmenu.ddns)</script></FONT></B></P></TD>

          <TD width=8 bgColor=#000000 height=25>&nbsp;</TD>

          <TD width=14 height=25>&nbsp;</TD>

          <TD width=17 height=25>&nbsp;</TD>

          <TD width=100 height=25>&nbsp;</TD>

          <TD width=310 height=25>&nbsp;</TD>


          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=2 width=31 height=25>&nbsp;</TD>

          <TD width=100 height=25><script>Capture(setddns.service)</script>:</TD>

          <TD width=310 height=25><SELECT onChange=SelDDNS(this.form.ddns_enable.selectedIndex,this.form) name="ddns_enable"> 

		<OPTION value=0 <% nvram_selmatch("ddns_enable", "0", "selected"); %>><b><script>Capture(share.disabled)</script></b></OPTION> 

      		<OPTION value=1 <% nvram_selmatch("ddns_enable", "1", "selected"); %>><b>DynDNS.org</b></OPTION>

      		<OPTION value=2 <% nvram_selmatch("ddns_enable", "2", "selected"); %>><b>TZO.com</b></OPTION>

	</SELECT>&nbsp;</TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=1>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>

          <TD colSpan=5 width=454>

            <TABLE>

              <TBODY>

              <TR>

                <TD width=16 height=1>&nbsp;</TD>

                <TD width=13 height=1>&nbsp;</TD>

                <TD width=410 colSpan=3 height=1>

                  <HR color=#b5b5e6 SIZE=1>

                </TD>

                <TD width=15 height=1>&nbsp;</TD></TR></TBODY></TABLE></TD>

          <TD width=15 background=image/UI_05.gif height=1>&nbsp;</TD></TR>

<% nvram_selmatch("ddns_enable","1","<!--"); %>

<% nvram_selmatch("ddns_enable","2","<!--"); %>



        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=2 width=31 height=25>&nbsp;</TD>

          <TD width=100 height=25>&nbsp;</FONT></TD>

          <TD width=310 height=25>&nbsp;</TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=2 width=31 height=25>&nbsp;</TD>

          <TD width=100 height=25>&nbsp;</FONT></TD>

          <TD width=310 height=25>&nbsp;</TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

		<TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=2 width=31 height=25>&nbsp;</TD>

          <TD width=100 height=25>&nbsp;</FONT></TD>

          <TD width=310 height=25>&nbsp;</TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

    	<TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=2 width=31 height=25>&nbsp;</TD>

          <TD width=100 height=25>&nbsp;</FONT></TD>

          <TD width=310 height=25>&nbsp;</TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

  

<% nvram_selmatch("ddns_enable","1","-->"); %>

<% nvram_selmatch("ddns_enable","2","-->"); %>

          

          

  

    <% nvram_selmatch("ddns_enable","0","<!--"); %>

	<% nvram_selmatch("ddns_enable","2","<!--"); %>

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=2 width=31 height=25>&nbsp;</TD>

          <TD width=100 height=25><FONT style="FONT-SIZE: 8pt" face=Arial><script>Capture(setddns.username)</script>:&nbsp;&nbsp;</FONT></TD>

          <TD width=310 height=25><FONT style="FONT-SIZE: 8pt" face=Arial>

			<input name="ddns_username" style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" size=20 maxlength=32 value='<% nvram_get("ddns_username"); %>' onFocus="check_action(this,0)" onBlur=valid_name(this,"User%20Name")><TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=2 width=31 height=25>&nbsp;</TD>

          <TD width=100 height=25><FONT style="FONT-SIZE: 8pt" face=Arial><script>Capture(setddns.password)</script>:&nbsp;&nbsp;</FONT></TD>

          <TD width=310 height=25><FONT style="FONT-SIZE: 8pt" face=Arial>

			<input type=password name="ddns_passwd" style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" size=20 maxlength=32 value='<% nvram_invmatch("ddns_passwd","","d6nw5v1x2pc7st9m"); %>' onFocus="check_action(this,0)" onBlur=valid_name(this,"Password")></FONT></TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=2 width=31 height=25>&nbsp;</TD>

          <TD width=100 height=25><FONT style="FONT-SIZE: 8pt" face=Arial><script>Capture(setddns.hostname)</script>:&nbsp;&nbsp;</FONT></TD>

          <TD width=310 height=25><FONT style="FONT-SIZE: 8pt" face=Arial>

			<input name="ddns_hostname" style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" size=20 maxlength=48 value='<% nvram_get("ddns_hostname"); %>' onFocus="check_action(this,0)" onBlur=valid_name(this,"Host%20Name")></b></FONT></TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <% nvram_selmatch("ddns_enable","2","-->"); %>

	<% nvram_selmatch("ddns_enable","1","<!--"); %>

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=2 width=31 height=25>&nbsp;</TD>

          <TD width=100 height=25><FONT style="FONT-SIZE: 8pt" face=Arial><script>Capture(setddns.email)</script>:&nbsp;&nbsp;</FONT></TD>

          <TD width=310 height=25><FONT style="FONT-SIZE: 8pt" face=Arial>

			<input name="ddns_username_2" style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" size=20 maxlength=32 value='<% nvram_get("ddns_username_2"); %>' onFocus="check_action(this,0)" onBlur=valid_name(this,"E-mail%20Address")></FONT></TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>    

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=2 width=31 height=25>&nbsp;</TD>

          <TD width=100 height=25><FONT style="FONT-SIZE: 8pt" face=Arial><script>Capture(setddns.password)</script>:&nbsp;&nbsp;</FONT></TD>

          <TD width=310 height=25><FONT style="FONT-SIZE: 8pt" face=Arial>

			<input type=password name="ddns_passwd_2" style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" size=20 maxlength=32 value='<% nvram_invmatch("ddns_passwd_2","","d6nw5v1x2pc7st9m"); %>' onFocus="check_action(this,0)" onBlur=valid_name(this,"Password")></FONT></TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=2 width=31 height=25>&nbsp;</TD>

          <TD width=100 height=25><FONT style="FONT-SIZE: 8pt" face=Arial><script>Capture(setddns.doname)</script>:&nbsp;&nbsp;</FONT></TD>

          <TD width=310 height=25><FONT style="FONT-SIZE: 8pt" face=Arial>

			<input name="ddns_hostname_2" style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" size=20 maxlength=48 value='<% nvram_get("ddns_hostname_2"); %>' onFocus="check_action(this,0)" onBlur=valid_name(this,"Domain%20Name")></b></FONT></TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <% nvram_selmatch("ddns_enable","1","-->"); %>

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=2 width=31 height=25>&nbsp;</TD>

          <TD width=100 height=25><FONT style="FONT-SIZE: 8pt" face=Arial><script>Capture(setddns.ipaddr)</script>:&nbsp;&nbsp;</FONT></TD>

          <TD width=310 height=25><FONT style="FONT-SIZE: 8pt" face=Arial><% show_ddns_ip(); %></FONT></TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=2 width=31 height=25>&nbsp;</TD>

          <TD width=100 height=25><FONT style="FONT-SIZE: 8pt" face=Arial><script>Capture(bmenu.statu)</script>:&nbsp;&nbsp;</FONT></TD>

          <TD width=310 height=25><FONT color=red><% show_ddns_status(); %></FONT></TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <% nvram_selmatch("ddns_enable","0","-->"); %>

        <TR>

          <TD width=156 bgColor=#e7e7e7 colspan=3 height=5>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=5>&nbsp;</TD>

          <TD width=454 colSpan=5 height=5>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif 

        height=5>&nbsp;</TD></TR></TBODY></TABLE></TD>

    <TD vAlign=top width=176 bgColor=#6666cc>

      <TABLE cellSpacing=0 cellPadding=0 width=176 border=0>

        <TBODY>

        <TR>

          <TD width=11 bgColor=#6666cc height=25>&nbsp;</TD>

          <TD width=156 bgColor=#6666cc height=25><font color="#FFFFFF">

          <span ><a target="_blank" href="help/HDDNS.asp"><script>Capture(share.more)</script>...</a></span></font></TD>

          <TD width=9 bgColor=#6666cc 

  height=25>&nbsp;</TD></TR></TBODY></TABLE></TD></TR>
<!--
  <TR>

    <TD width=809 colSpan=2>

      <TABLE cellSpacing=0 cellPadding=0 border=0>

        <TBODY>

        <TR>

          <TD width=156 bgColor=#e7e7e height=30>&nbsp;</TD>

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

              border=0 width="144"><TBODY>

              <TR>

                <TD align=middle width=109></FONT>

                <INPUT type=button value="  Save Settings" onClick=to_submit(this.form)></TD>

                <TD align=middle width=8 bgColor=#6666cc>&nbsp;</TD>

                <TD align=middle bgColor=#6666cc width="66">

                <INPUT type=button value=" Cancel Changes" onclick=window.location.reload()></TD>

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
                <TD align=middle bgColor=#434a8f width="111"><FONT color=#ffffff><B><A href=DDNS.asp><script>Capture(share.cancels)</script></A></B></FONT></TD>
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



