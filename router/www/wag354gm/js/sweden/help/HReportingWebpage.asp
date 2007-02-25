<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<!-- saved from url=(0026)http://192.168.1.1/Log.asp -->
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
--><HTML><HEAD><TITLE>Rapportering</TITLE>
<META http-equiv=expires content=0>
<META http-equiv=cache-control content=no-cache>
<META http-equiv=pragma content=no-cache>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1"> <LINK href="Reporting webpage_files/style.css" type=text/css rel=stylesheet>
<STYLE fprolloverstyle>A:hover { COLOR: #00ffff } .small A:hover { COLOR: #00ffff }
</STYLE>

<SCRIPT src="Reporting webpage_files/common.js"></SCRIPT>

<SCRIPT language=JavaScript>

var EN_DIS = '0'

function to_submit(F)

{
//	var resolv_smtp_server;
//	var resolv_success = "success";
	if(F.log_enable[0].checked)

		F.log_level.value = "2";

	else

		F.log_level.value = "0";

	F.submit_button.value = "Log";
	F.action.value='Verkställ';

	if(F.email_enable[0].checked==true)
	{
		if(F.smtp_server.value == "")
		{
			alert("Du måste ange ett SMTP-servernamn.");
			F.smtp_server.focus();
		}else if(F.email_for_log.value == "")
		{
			alert("Du måste ange en e-postadress för logg.");
			F.email_for_log.focus();
		}else if(F.email_for_return.value == "")
		{
			alert("Du måste ange en e-postadress för svar.");
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

		if(resolv_smtp_server != resolv_success)
		{
			alert(F.smtp_server.value + " kan inte lösas.");
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
        self.open('Log_incoming.asp','inLogTable','alwaysRaised,resizable,scrollbars,width=580,height=600').focus();
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

        }

        else{

                choose_enable(F.log_incoming);

                choose_enable(F.log_outgoing);

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

        log_enable_disable(document.log,'0');

}



</SCRIPT>

<META content="MSHTML 6.00.2800.1400" name=GENERATOR></HEAD>
<BODY onload=init()> <DIV align=center>
<FORM name=log action=apply.cgi method=post>
<INPUT type=hidden
name=email_config> <INPUT type=hidden name=submit_button>
<INPUT type=hidden
name=change_action> <INPUT type=hidden name=action>
<INPUT type=hidden
name=log_level>
<TABLE cellSpacing=0 cellPadding=0 width=809 border=0>
  <TBODY>
  <TR>
    <TD width=95> <IMG height=57 src="../image/UI_Linksys.gif"       width=165 border=0></TD>
    <TD vAlign=bottom align=right width=714 bgColor=#6666cc> <FONT       style="FONT-SIZE: 7pt" color=#ffffff><FONT face=Arial>Version av fast programvara: 0.01.16v   </FONT></FONT></TD></TR>
  <TR>
    <TD width=808 bgColor=#6666cc colSpan=2> <IMG height=11       src="../image/UI_10.gif" width=809 border=0></TD></TR></TBODY></TABLE>
<TABLE height=77 cellSpacing=0 cellPadding=0 width=809 bgColor=black border=0>
  <TBODY>
  <TR>

<TD
    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
    borderColor=#000000 align=middle width=163 height=49>
      <H3 style="MARGIN-TOP: 1px; MARGIN-BOTTOM: 1px"> <FONT       style="FONT-SIZE: 15pt" face=Arial     color=#ffffff>Administration</FONT></H3></TD>

<TD
    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
    vAlign=center borderColor=#000000 width=646 bgColor=#000000 height=49>

<TABLE
      style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; BORDER-COLLAPSE: collapse; FONT-VARIANT: normal"
      height=33 cellSpacing=0 cellPadding=0 bgColor=#6666cc border=0>
        <TBODY>
        <TR>

<TD style="FONT-WEIGHT: bolder; FONT-SIZE: 10pt" align=right           bgColor=#6666cc height=33><FONT color=#ffffff>Wireless-G ADSL-gateway</FONT></TD>

<TD borderColor=#000000 borderColorLight=#000000 align=middle           width=109 bgColor=#000000 borderColorDark=#000000 height=12             rowSpan=2><FONT color=#ffffff><SPAN style="FONT-SIZE: 8pt"><B>WAG54G V.2</B></SPAN></FONT></TD></TR>
        <TR>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 1pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          width=537 bgColor=#000000 height=1> </TD></TR>
        <TR>
          <TD width=646 bgColor=#000000 colSpan=2 height=32>

<TABLE id=AutoNumber1
            style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; BORDER-COLLAPSE: collapse; FONT-VARIANT: normal"
            height=6 cellSpacing=0 cellPadding=0 width=637 border=0>
              <TBODY>

<TR
              style="BORDER-RIGHT: medium none; BORDER-TOP: medium none; FONT-WEIGHT: normal; FONT-SIZE: 1pt; BORDER-LEFT: medium none; COLOR: black; BORDER-BOTTOM: medium none; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
              align=middle bgColor=#6666cc>
                <TD width=83 height=1> <IMG height=10                   src="../image/UI_06.gif" width=83 border=0></TD>
                <TD width=73 height=1> <IMG height=10                   src="../image/UI_06.gif" width=83 border=0></TD>
                <TD width=113 height=1> <IMG height=10                   src="../image/UI_06.gif" width=83 border=0></TD>
                <TD width=103 height=1> <IMG height=10                   src="../image/UI_06.gif" width=103 border=0></TD>
                <TD width=85 height=1> <IMG height=10                   src="../image/UI_06.gif" width=100 border=0></TD>
                <TD width=115 height=1> <IMG height=10                   src="../image/UI_07.gif" width=115 border=0></TD>
                <TD width=74 height=1> <IMG height=10                   src="../image/UI_06.gif" width=79               border=0></TD></TR>
              <TR>
                <TD align=middle bgColor=#000000 height=20> <FONT                   style="FONT-WEIGHT: 700" color=#ffffff> <A                   style="TEXT-DECORATION: none"                   href="http://192.168.1.1/index.asp">Konfiguration</A></FONT></TD>
<TD align=middle bgColor=#000000 height=20>               <A                   style="TEXT-DECORATION: none"                   href="http://192.168.1.1/Wireless_Basic.asp"> <FONT                   style="FONT-WEIGHT: 700" color=#ffffff>Trådlöst</A></FONT></TD>
                <TD align=middle bgColor=#000000 height=20> <FONT                   style="FONT-WEIGHT: 700" color=#ffffff> <A                   style="TEXT-DECORATION: none"                   href="http://192.168.1.1/Firewall.asp">Säkerhet</A></FONT></TD>
                <TD align=middle bgColor=#000000 height=20> <FONT                   style="FONT-WEIGHT: 700" color=#ffffff> <A                   style="TEXT-DECORATION: none"                   href="http://192.168.1.1/Filters.asp">Åtkomstbegränsningar</A></FONT></TD>
                <TD align=middle bgColor=#000000 height=20>
                  <P style="MARGIN-BOTTOM: 4px"> <FONT style="FONT-WEIGHT: 700"                   color=#ffffff> <A style="TEXT-DECORATION: none"                   href="http://192.168.1.1/Forward_UPnP.asp">Tillämpningar <BR>&amp; spel</A>    </FONT></P></TD>
                <TD align=middle bgColor=#6666cc height=20>
                  <P style="MARGIN-BOTTOM: 4px"> <FONT style="FONT-WEIGHT: 700"                   color=#ffffff>Administration    </FONT></P></TD>
                <TD align=middle bgColor=#000000 height=20>
                                    <P style="MARGIN-BOTTOM: 4px"> <FONT style="FONT-WEIGHT: 700"                   color=#ffffff> <A style="TEXT-DECORATION: none"                   href="http://192.168.1.1/Status_Router.asp">Status</A>    </FONT></P></TD></TR>
               <TR>
                <TD width=643 bgColor=#6666cc colSpan=7 height=21>

<TABLE borderColor=black height=21 cellSpacing=0 cellPadding=0
                  width=643>
                    <TBODY>
                    <TR align=left>
                      <TD width=25></TD>
                      <TD class=small width=80> <A                         href="http://192.168.1.1/Management.asp">Hantering</A></TD>
                      <TD width=1>
                        <P class=bar><FONT color=white><B>|</B></FONT></P></TD>
                      <TD width=20></TD>
                      <TD width=40> <SPAN                         style="COLOR: #ffffff">Rapportering</SPAN></TD>
                      <TD width=1>
                                                <P class=bar><FONT color=white><B>|</B></FONT></P></TD>
                      <TD width=15></TD>
                      <TD class=small width=75> <A                         href="http://192.168.1.1/Diagnostics.asp">Diagnostik</A></TD>
                      <TD width=1>
                                                <P class=bar><FONT color=white><B>|</B></FONT></P></TD>
                      <TD width=15></TD>
                      <TD class=small width=95> <A                         href="http://192.168.1.1/Factory_Defaults.asp">Fabriksinställningar</A></TD>
                      <TD width=1>
                                                <P class=bar><FONT color=white><B>|</B></FONT></P></TD>
                      <TD width=15></TD>
                      <TD class=small width=105> <A                         href="http://192.168.1.1/Upgrade.asp">Uppgradera fast programvara</A></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>
<TABLE height=5 cellSpacing=0 cellPadding=0 width=806 bgColor=black border=0>
  <TBODY>
  <TR bgColor=black>

<TD
    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
    borderColor=#e7e7e7 width=163 bgColor=#e7e7e7 height=1> <IMG height=15       src="../image/UI_03.gif" width=164 border=0></TD>

<TD
    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
    width=646 height=1> <IMG height=15 src="../image/UI_02.gif"       width=645 border=0></TD></TR></TBODY></TABLE>
<TABLE id=AutoNumber9 style="BORDER-COLLAPSE: collapse" borderColor=#111111
height=23 cellSpacing=0 cellPadding=0 width=809 border=0>
  <TBODY>
  <TR>
    <TD width=633>
      <TABLE cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=156 bgColor=#000000 height=25>
<P align=right><B> <FONT style="FONT-SIZE: 9pt" face=Arial             color=#ffffff>Rapportering</FONT></B></P></TD>
          <TD width=8 bgColor=#000000 height=25> </TD>
          <TD width=454 colSpan=6 height=25> </TD>

<TD width=15 background="../image/UI_05.gif"
          height=25> </TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25> </TD>

<TD width=8 background="../image/UI_04.gif"
          height=25> </TD>
          <TD width=26 colSpan=3 height=25></TD>
<TD width=150 height=25><SPAN style="FONT-SIZE: 8pt">Logg:</SPAN></TD>
          <TD width=265 height=25>

<TABLE id=AutoNumber12 cellSpacing=0 cellPadding=0 width=242
            border=0>
              <TBODY>
              <TR>
                <TD width=242 height=25><B>
<INPUT onclick=SelLog(1,this.form)
                  type=radio value=1 name=log_enable>Aktivera
<INPUT                   onclick=SelLog(0,this.form) type=radio CHECKED value=0                   name=log_enable>Avaktivera</B></TD><g1024 /TR></TBODY></TABLE></TD>
          <TD width=13 height=25></TD>

<TD width=15 background="../image/UI_05.gif"
          height=25> </TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25> </TD>

<TD width=8 background="../image/UI_04.gif"
          height=25> </TD>
          <TD width=26 colSpan=3 height=25></TD>
          <TD width=150 height=25>IP-adress för Logviewer:</TD>
          <TD width=265 height=25>

<TABLE id=AutoNumber12 cellSpacing=0 cellPadding=0 width=242
            border=0>
              <TBODY>
              <TR>
                <TD width=242 height=25>  <B>192.168.1.
<INPUT                   class=num onblur='valid_range(this,1,255,"IP")' maxLength=3                   size=3 value=255 name=log_ipaddr></B></TD><g1024 /TR></TBODY></TABLE></TD>
          <TD width=13 height=25></TD>

<TD width=15 background="../image/UI_05.gif"
          height=25> </TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=1> </TD>

<TD width=8 background="../image/UI_04.gif"
          height=1> </TD>
          <TD colSpan=6>
            <TABLE>
              <TBODY>
              <TR>
                <TD width=16 height=1> </TD>
                <TD width=13 height=1> </TD>
                <TD width=410 colSpan=3 height=1>
                  <HR color=#b5b5e6 SIZE=1>
                </TD>
                <TD width=15 height=1> </TD></TR></TBODY></TABLE></TD>

<TD width=15 background="../image/UI_05.gif"
          height=1> </TD></TR>
        <TR>

<TD           style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"           align=right bgColor=#e7e7e7><B style="FONT-WEIGHT: bold">Varningsmeddelanden<br>via&nbsp;e-post</B></TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          width=8 height=25> <IMG height=25             src="../image/UI_04.gif" width=8 border=0></TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          colSpan=3> </TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal">Varningsmeddelanden<br>via&nbsp;e-post:</TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal">
<INPUT
            type=radio value=1 name=email_enable><B             style="FONT-WEIGHT: bold">Aktiverad</B>
<INPUT type=radio CHECKED
            value=0 name=email_enable><B           style="FONT-WEIGHT: bold">Avaktiverad</B></TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"> </TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"> <IMG             height=25 src="../image/UI_05.gif" width=15           border=0></TD></TR>
        <TR>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          align=right bgColor=#e7e7e7> </TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          width=8 height=25> <IMG height=28             src="../image/UI_04.gif" width=8 border=0></TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          width=26 colSpan=3> </TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          width=150>DoS-gränsvärden (Denial of Service Thresholds):</TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          width=265>
<INPUT
            onblur="valid_range(this,20,100,'Thresholds%20of%20Dos')"
            maxLength=3 size=3 value=20
            name=email_thresholds> händelser (20-100)</TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          width=13> </TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"> <IMG             height=28 src="../image/UI_05.gif" width=15           border=0></TD></TR>
        <TR>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          align=right bgColor=#e7e7e7> </TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          width=8 height=25> <IMG height=25             src="../image/UI_04.gif" width=8 border=0></TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          colSpan=3> </TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal">SMTP-postserver:</TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal">
<INPUT
            onblur='valid_name(this,"Smtp%20Server%20Name",SPACE_NO)'
            maxLength=31 size=30 name=smtp_server></TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"> </TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"> <IMG             height=25 src="../image/UI_05.gif" width=15           border=0></TD></TR>
        <TR>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          align=right bgColor=#e7e7e7> </TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          width=8 height=25> <IMG height=28             src="../image/UI_04.gif" width=8 border=0></TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          colSpan=3> </TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          width=150>E-postadress för varningsloggar:</TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal">
<INPUT
            onblur='valid_name(this,"Email%20For%20Log%20Name",SPACE_NO)'
            maxLength=31 size=30 name=email_for_log></TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"> </TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"> <IMG             height=28 src="../image/UI_05.gif" width=15           border=0></TD></TR>
        <TR>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          align=right bgColor=#e7e7e7> </TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          width=8 height=25> <IMG height=25             src="../image/UI_04.gif" width=8 border=0></TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          colSpan=3> </TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal">E-postadress för svar:</TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal">
<INPUT
            onblur='valid_name(this,"Email%20Return%20Name",SPACE_NO)'
            maxLength=31 size=30 name=email_for_return></TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"> </TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"> <IMG             height=25 src="../image/UI_05.gif" width=15           border=0></TD></TR>
        <TR>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          width=156 bgColor=#e7e7e7 height=1> </TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          width=8 height=1> <IMG height=25             src="../image/UI_04.gif" width=8 border=0></TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          height=25> </TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          height=25> </TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          colSpan=3>
            <HR color=#b5b5e6 SIZE=1>
          </TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          width=13 height=25> </TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          width=15 height=1> <IMG height=25             src="../image/UI_05.gif" width=15 border=0></TD></TR>
        <TR>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          align=right bgColor=#e7e7e7 height=25> <B             style="FONT-WEIGHT: bold"> </B></TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          width=8 height=25> <IMG height=25             src="../image/UI_04.gif" width=8 border=0></TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          width=5 height=25> </TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          width=17 height=25> </TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          width=4 height=25> </TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          align=middle colSpan=2> <INPUT onclick=ViewLogIn() type=button value="Visa loggar"></TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          width=13> </TD>

<TD
          style="FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal"
          width=15> <IMG height=25 src="../image/UI_05.gif"             width=15 border=0></TD></TR></TBODY></TABLE></TD>
    <TD vAlign=top width=176 bgColor=#6666cc>
      <TABLE cellSpacing=0 cellPadding=0 width=176 border=0>
        <TBODY>
        <TR>
          <TD width=11 bgColor=#6666cc height=25> </TD>
          <TD width=156 bgColor=#6666cc height=25><FONT color=#ffffff><SPAN> <A             href="http://192.168.1.1/help/HLog.asp"             target=_blank></A></SPAN></FONT></TD>

<TD width=9 bgColor=#6666cc
  height=25> </TD></TR></TBODY></TABLE></TD></TR>
<!--
  <TR>

    <TD width=809 colSpan=2>

      <TABLE cellSpacing=0 cellPadding=0 border=0>

        <TBODY>

        <TR>

          <TD width=156 bgColor=#e7e7e7 height=30> </TD>

          <TD width=8 background=../image/UI_04.gif> </TD>

          <TD width=16> </TD>

          <TD width=12> </TD>

          <TD width=411> </TD>

          <TD width=15>

          <TD width=15 background=../image/UI_05.gif> </TD>

          <TD width=176 bgColor=#6666cc height=58 rowSpan=2>

          <IMG

            src="../image/UI_Cisco.gif" border=0 width="176" height="64"></TD></TR>

        <TR>

          <TD width=156 bgColor=#000000> </TD>

          <TD width=8 bgColor=#000000> </TD>

          <TD width=16 bgColor=#6666cc> </TD>

          <TD width=12 bgColor=#6666cc> </TD>

          <TD width=411 bgColor=#6666cc>

            <TABLE height=19 cellSpacing=0 cellPadding=0 align=right

              border=0 width="156"><TBODY>

              <TR>

                <TD width="50">

                <INPUT onClick=to_submit(this.form) type=button value="Spara inställningar"></FONT></TD>

                <TD width=8 bgColor=#6666cc> </TD>

                <TD width="130"><INPUT type=button value="Avbryt ändringar" onclick=window.location.reload()></TD>

                <TD align=middle width=4 bgColor=#6666cc> </TD>

                </TR></TBODY></TABLE></TD>

          <TD width=15 bgColor=#6666cc height=33> </TD>

          <TD width=15 bgColor=#000000

  height=33> </TD></TR></TBODY></TABLE></TD></TR>-->
  <TR>
    <TD width=809 colSpan=2>
      <TABLE cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=30> </TD>
          <TD width=8 background="../image/UI_04.gif"> </TD>
          <TD width=16> </TD>
          <TD width=12> </TD>
          <TD width=411> </TD>
          <TD width=15 height=1> </TD>

<TD width=15
background="../image/UI_05.gif"> </TD>
          <TD width=176 bgColor=#6666cc rowSpan=2> <IMG height=64             src="../image/UI_Cisco.gif" width=176 border=0></TD></TR>
        <TR>
          <TD width=156 bgColor=#000000> </TD>
          <TD width=8 bgColor=#000000> </TD>
          <TD width=16 bgColor=#6666cc> </TD>
          <TD width=12 bgColor=#6666cc> </TD>
          <TD width=411 bgColor=#6666cc>

<TABLE height=19 cellSpacing=0 cellPadding=0 width=220 align=right
            border=0>
              <TBODY>
              <TR>
                <TD align=middle width=101 bgColor=#42498c> <FONT                   color=#ffffff><B> <A                   href="javascript:to_submit(document.forms[0]);">Spara inställningar</A></B></FONT> </TD>
                <TD align=middle width=8 bgColor=#6666cc> </TD>
                <TD align=middle width=111 bgColor=#434a8f> <FONT                   color=#ffffff><B> <A                   href="javascript:window.location.reload();">Avbryt ändringar</A></B></FONT></TD>

<TD align=middle width=4
            bgColor=#6666cc> </TD></TR></TBODY></TABLE></TD>
          <TD width=15 bgColor=#6666cc height=33> </TD>
          <TD width=15 bgColor=#000000
  height=33> </TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></FORM></DIV></BODY></HTML>
