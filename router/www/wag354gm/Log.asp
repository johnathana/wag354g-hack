
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

<HTML><HEAD><TITLE>Reporting</TITLE>
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
document.title=(admtopmenu.report);
<% wireless_support(5); %>share.productname = share.productname1;<% wireless_support(6); %>
var EN_DIS = '<% nvram_get("log_enable"); %>'

function to_submit(F)

{
//	var resolv_smtp_server;
//	var resolv_success = "success";
	if(F.log_enable[0].checked)

		F.log_level.value = "2";

	else

		F.log_level.value = "0";
	
	F.submit_button.value = "Log";
	F.action.value='Apply';

	if(F.email_enable[0].checked==true)
	{
		if(F.smtp_server.value == "")
		{
			//alert("You must input a smtp server name!");
			alert(errmsg.err101);

			F.smtp_server.focus();
		}else if(F.email_for_log.value == "")
		{
			//alert("You must input a email address for log!");
			alert(errmsg.err102);

			F.email_for_log.focus();
		}else if(F.email_for_return.value == "")
		{
			//alert("You must input a email address for return!");
			alert(errmsg.err103);
			F.email_for_return.focus();
		}
		else
		{
			F.submit();
		}
	}
	else
	{
		F.submit();
	}
	
	/*	if(F.email_enable[0].checked)
	{
		<% smtp_server_resolv("smtp_server"); %>
		if(resolv_smtp_server != resolv_success)
		{
			//alert(F.smtp_server.value + " can not be resolved!");
			alert(F.smtp_server.value + errmsg.124);
			F.smtp_server.focus();
		}
		else
		{
			F.submit();
		}
	}
	else*/
		
}

function ViewLogIn()
{
        self.open('Log_incoming.asp','inLogTable','alwaysRaised,resizable,scrollbars,width=780,height=600').focus();
}


function SelLog(num,F)

{

	log_enable_disable(F,num);

}

function log_enable_disable(F,I)

{

        EN_DIS = I;

        if ( I == "0" ){

                choose_disable(F.log_incoming);

                choose_disable(F.log_outgoing);

                choose_disable(F.log_ipaddr);

        }

        else{

                choose_enable(F.log_incoming);

                choose_enable(F.log_outgoing);

                choose_enable(F.log_ipaddr);

        }

}

function ViewLogOut()

{

	self.open('Log_outgoing.asp','outLogTable','alwaysRaised,resizable,scrollbars,width=760,height=600').focus();

}

function ViewLog()

{

	self.open('Log_all.asp','inLogTable','alwaysRaised,resizable,scrollbars,width=580,height=600').focus();

}

function init() 

{               

        log_enable_disable(document.log,'<% nvram_get("log_enable"); %>');

}



</SCRIPT>

</HEAD>

<BODY onload=init()>

<DIV align=center>

<FORM name=log method=<% get_http_method(); %> action=apply.cgi>
<input type=hidden name=email_config>
<input type=hidden name=submit_button>

<input type=hidden name=change_action>

<input type=hidden name=action>

<INPUT type=hidden name=log_level> 



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

    <TD style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" borderColor=#000000 align=middle width=163 height=49><H3 style="MARGIN-TOP: 1px; MARGIN-BOTTOM: 1px"><FONT style="FONT-SIZE: 15pt" face=Arial color=#ffffff><script>Capture(bmenu.admin)</script></FONT></H3></TD>

    <TD style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" vAlign=center borderColor=#000000 width=646 bgColor=#000000 height=49><TABLE style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; BORDER-COLLAPSE: collapse; FONT-VARIANT: normal" height=33 cellSpacing=0 cellPadding=0 bgColor=#6666cc border=0>

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

                <TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><A style="TEXT-DECORATION: none" href="<% support_elsematch("PARENTAL_CONTROL_SUPPORT", "1", "Parental_Control.asp", "Filters.asp"); %>"><script>Capture(bmenu.accrestriction)</script></A></FONT></TD>

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
                      <TD  width=80><span style="color: #FFFFFF"><script>Capture(admtopmenu.report)</script></span></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=80><A href="Diagnostics.asp"><script>Capture(admtopmenu.diag)</script></A></TD>
<% support_elsematch("BACKUP_RESTORE_SUPPORT","1","","<!--"); %>
		      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD  class=small width=110><A href="Backup_Restore.asp"><script>Capture(admtopmenu.bakrest)</script></A></TD>
<% support_elsematch("BACKUP_RESTORE_SUPPORT","1","","-->"); %>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=110><A href="Factory_Defaults.asp"><script>Capture(admtopmenu.facdef)</script></A></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=120><A href="Upgrade.asp"><script>Capture(admtopmenu.upgrade)</script></A></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=63><A href='Reboot.asp'><script>Capture(admtopmenu.reboot)</script></A></TD>
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

          <TD width=156 bgColor=#000000 height=25>

            <P align=right><B><FONT style="FONT-SIZE: 9pt" face=Arial 

            color=#ffffff><script>Capture(admleftmenu.report)</script></FONT></B></P></TD>

          <TD width=8 bgColor=#000000 height=25>&nbsp;</TD>

          <TD width=454 colSpan=6 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>

          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD width=52 colSpan=3 height=25></TD>

          <TD width=147 height=25><SPAN  

            style="FONT-SIZE: 8pt"><script>Capture(admreport.logs)</script>:</SPAN></TD>

          <TD width=242 height=25>

            <TABLE id=AutoNumber12 cellSpacing=0 cellPadding=0 width=242 

            border=0>

              <TBODY>

              <TR>

                <TD width=242 height=25><B>

                <INPUT type=radio value=1 name=log_enable <% nvram_match("log_enable", "1", "checked"); %> OnClick=SelLog(1,this.form)><script>Capture(share.enabled)</script>

                <INPUT type=radio value=0 name=log_enable <% nvram_match("log_enable", "0", "checked"); %> OnClick=SelLog(0,this.form)><script>Capture(share.disabled)</script></B></TD>

                </TR></TBODY></TABLE></TD>

          <TD width=13 height=25></TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>


        <TR>

          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD width=52 height=25 colSpan=3></TD>

          <TD width=147 height=25><script>Capture(admreport.logviewerip)</script>:</TD>

          <TD width=242 height=25>

            <TABLE id=AutoNumber12 cellSpacing=0 cellPadding=0 width=242 border=0>

              <TBODY>

              <TR>

                <TD width=242 height=25>&nbsp;&nbsp;<B><% prefix_ip_get("lan_ipaddr",1); %><INPUT class=num maxLength=3 name=log_ipaddr onBlur=valid_range(this,1,254,"IP") size=3 value="<% nvram_get("log_ipaddr"); %>"></TD>

                </TR></TBODY></TABLE></TD>

          <TD width=13 height=25></TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>


        <TR>

          <TD width=156 bgColor=#e7e7e7 height=1>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>

          <TD colSpan=6>

            <TABLE>

              <TBODY>

              <TR>

                <TD width=24 height=1>&nbsp;</TD>

                <TD width=16 height=1>&nbsp;</TD>

                <TD width=410 colSpan=3 height=1>

                  <HR color=#b5b5e6 SIZE=1>

                </TD>

                <TD width=15 height=1>&nbsp;</TD></TR></TBODY></TABLE></TD>

          <TD width=15 background=image/UI_05.gif height=1>&nbsp;</TD></TR>

        <tr>
			<TD width=156 align=right bgColor=#e7e7e7 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">
			<b style="font-weight: bold"><script>Capture(admleftmenu.emailalt)</script></b></TD>
			<TD width=8 height=25 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal" >
			<IMG src="image/UI_04.gif"  border=0 width=8 height=30></TD>
			<TD colspan=3 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">&nbsp;</TD>
			<TD style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal"><script>Capture(admreport.emailalerts)</script>:</TD>
			<TD style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">
			<INPUT type=radio value=1 name="email_enable" <% email_enable_config("email_enable", 1); %> ><b style="font-weight: bold"><script>Capture(share.enabled)</script></b>&nbsp;<INPUT type=radio value=0 name="email_enable" <% email_enable_config("email_enable", 0); %> ><b style="font-weight: bold"><script>Capture(share.disabled)</script></b></TD>
			<TD style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">&nbsp;</TD>
			<TD style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal" >
			<IMG src="image/UI_05.gif"  border=0 width=15 height=30></TD>
		</tr>
		<tr>
			<TD align=right bgColor=#e7e7e7 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">&nbsp;</TD>
			<TD width=8 height=25 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal" >
			<IMG src="image/UI_04.gif"  border=0 width=8 height=30></TD>
			<TD colspan=3 width=52 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">&nbsp;</TD>
			<TD width=147 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal"><script>Capture(admreport.denservice)</script>:</TD>
			<TD width=242 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal"><INPUT name=email_thresholds size=3 maxLength=3 value='<% email_config("email_thresholds"); %>' onBlur=valid_range(this,20,100,'Thresholds%20of%20Dos') >&nbsp;<script>Capture(admreport.events)</script>&nbsp;<script>Capture(admreport.eventsrange)</script></TD>
			<TD width=13 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">&nbsp;</TD>
			<TD style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal" >
			<IMG src="image/UI_05.gif"  border=0 width=15 height=30></TD>
		</tr>
		<tr>
			<TD align=right bgColor=#e7e7e7 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">&nbsp;</TD>
			<TD width=8 height=25 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal" >
				<IMG src="image/UI_04.gif"  border=0 width=8 height=30></TD>
			<TD colspan=3 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">&nbsp;</TD>
			<TD style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal"><script>Capture(admreport.smtpserver)</script>:</TD>
			<TD style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal"><INPUT name=smtp_server size=30 maxLength=31 value='<% email_config("smtp_server"); %>' onBlur=valid_name(this,"Smtp%20Server%20Name",SPACE_NO)></TD>
			<TD style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">&nbsp;</TD>
			<TD style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal" >
			<IMG src="image/UI_05.gif"  border=0 width=15 height=30></TD>
		</tr>
		<tr>
			<TD align=right bgColor=#e7e7e7 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">&nbsp;</TD>
			<TD width=8 height=25 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal" >
			<IMG src="image/UI_04.gif"  border=0 width=8 height=50></TD>
			<TD colspan=3 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">&nbsp;</TD>
			<TD width=147 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal"><script>Capture(admreport.emailaddr)</script>:</TD>
			<TD style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal"><INPUT name=email_for_log size=30 maxLength=31 value='<% email_config("email_for_log"); %>' onBlur=valid_name(this,"Email%20For%20Log%20Name",SPACE_NO)></TD>
			<TD style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">&nbsp;</TD>
			<TD style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal" >
			<IMG src="image/UI_05.gif" border=0 width=15 height=50></TD>
		</tr>
		<tr>
			<TD align=right bgColor=#e7e7e7 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">&nbsp;</TD>
			<TD width=8 height=25 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal" >
			<IMG src="image/UI_04.gif"  border=0 width=8 height=30></TD>
			<TD colspan=3 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">&nbsp;</TD>
			<TD style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal"><script>Capture(admreport.returnaddr)</script>:</TD>
			<TD style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal"><INPUT name=email_for_return size=30 maxLength=31 value='<% email_config("email_for_return"); %>' onBlur=valid_name(this,"Email%20Return%20Name",SPACE_NO)></TD>
			<TD style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">&nbsp;</TD>
			<TD style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal" >
			<IMG src="image/UI_05.gif"  border=0 width=15 height=30></TD>
		</tr>
		<tr>
			<TD width=156 bgColor=#e7e7e7 height=1 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">&nbsp;</TD>
			<TD width=8 height=1 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal" >
			<IMG src="image/UI_04.gif"  border=0 width=8 height=25></TD>
			<TD height=25 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">&nbsp;</TD>
			<TD height=25 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">&nbsp;</TD>
			<TD colSpan=3 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal"><HR color=#b5b5e6 SIZE=1></TD>
			<TD width=13 height=25 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">&nbsp;</TD>
			<TD width=15 height=1 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal" >
			<IMG src="image/UI_05.gif"  border=0 width=15 height=25></TD>
		</tr>
		<tr>
			<TD bgColor=#e7e7e7 height=25 align=right style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">
			<B style="font-weight: bold">&nbsp;</B></TD>
			<TD width=8 height=25 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal" >
			<IMG src="image/UI_04.gif"  border=0 width=8 height=25></TD>
			<TD width=15 height=25 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">&nbsp;</TD>
			<TD width=27 height=25 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">&nbsp;</TD>
			<TD width=10 height=25 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">&nbsp;</TD>
			<TD colspan=2 align=center style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">&nbsp;
			  <script>document.write("<input type=button value=\"" + admbutton.viewlogs + "\" onClick=\"ViewLogIn()\">");</script></TD>
			<TD width=13 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal">&nbsp;</TD>
			<TD width=15 style="font-weight: normal; font-size: 8pt; color: black; font-style: normal; font-family: Arial, Helvetica, sans-serif; font-variant: normal" >
			<IMG src="image/UI_05.gif"  border=0 width=15 height=25></TD>
		</tr>
		</TBODY></TABLE></TD>

    <TD vAlign=top width=176 bgColor=#6666cc>

      <TABLE cellSpacing=0 cellPadding=0 width=176 border=0>

        <TBODY>

        <TR>

          <TD width=11 bgColor=#6666cc height=25>&nbsp;</TD>

          <TD width=156 bgColor=#6666cc height=25><font color="#FFFFFF">

          <span ><a target="_blank" href="help/HLog.asp"><script>Capture(share.more)</script>...</a></span></font>&nbsp;</TD>

          <TD width=9 bgColor=#6666cc 

  height=25>&nbsp;</TD></TR></TBODY></TABLE></TD></TR>
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
  
  </TBODY></TABLE></FORM></DIV></BODY></HTML>

