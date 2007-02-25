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

<HTML><HEAD><TITLE>Firmware Upgrade</TITLE>
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
<SCRIPT language=javascript>
document.title=(admtopmenu.upgrade);
<% wireless_support(5); %>share.productname = share.productname1;<% wireless_support(6); %>
var showchar = '|';

var maxchars = 60;

var delay_time = 1000;

var counter=0;

var num=0;

function progress(){

	var F = document.forms[0];

	if(num == 10){

		clearTimeout(timerID);

		//alert("Upgrade is failed!");
		alert(errmsg.err110);
		return false;

	}

	if (counter < maxchars)	{

		counter++;

		var tmp = '';

		for (var i=0; i < counter; i++)

			tmp = tmp + showchar;

		F.process.value = tmp;

		timerID = setTimeout('progress()',delay_time);

	}

	else{

		counter = 0;

		num ++;

		progress();	

        }

}


function stop(){

	if(ie4)

  		document.all['style0'].style.visibility = 'hidden';

}


function upgrade_wan_1(F){

	if(ns4)

            delay_time = 1500;



        choose_disable(F.Upgrade_lan);


        F.submit_button.value = "upgrade_wan_test";

        F.submit();
}

function progress_wan(){

        var F = document.forms[0];

        if(num == 2){

                clearTimeout(timerID);

                //alert("Upgrade is failed!");
		alert(errmsg.err110);
                return false;

        }

        if (counter < maxchars) {

                counter++;

                var tmp = '';

                for (var i=0; i < counter; i++)

                        tmp = tmp + showchar;

                F.process_wan.value = tmp;

                timerID = setTimeout('progress_wan()',delay_time);

        }

        else{

                counter = 0;

                num ++;

                progress_wan();
	}
}

function cancel(F)
{
	window.location.href="Upgrade.asp";
}

function upgrade_lan(F){

	var len = F.file.value.length;

	var ext = new Array('.','b','i','n');

	if (F.file.value == '')	{

		//alert('Please select a file to upgrade !');
		alert(errmsg.err111);
		return false;

	}

	var IMAGE = F.file.value.toLowerCase();

	for (i=0; i < 4; i++)	{

		if (ext[i] != IMAGE.charAt(len-4+i)){

			//alert('Incorrect image file!');
			alert(errmsg.err112);
			return false;

		}

	}



	if(ns4)

		delay_time = 1500;

   		

	choose_disable(F.Upgrade_wan);



	F.submit_button.value = "Upgrade";
   	F.submit();

   	//document.onstop = stop;

   	progress();

}

<% nvram_match("no_wan_upgrade_version", "1", "var no_wan_upgrade_version=1;"); %>
<% nvram_match("no_wan_upgrade_version", "0", "var no_wan_upgrade_version=0;"); %>
<% nvram_invmatch("upgrade_wan_test", "1", "/*"); %>
function version()
{
	//alert("There is no latest version to upgrade");
	alert(errmsg.err113);
}
function init()
{
	if (no_wan_upgrade_version == 0)
	{
		if(confirm('<% show_upgrade_info(); %>'))
		{
        		document.firmware.submit_button.value = "upgrade_wan_download";
        		choose_disable(document.firmware.Upgrade_lan);
			document.firmware.submit();
			progress_wan();
		}
		else
		{
			
        		document.firmware.submit_button.value = "upgrade_wan_cancel";
			document.firmware.submit();
		}
	}
	if (no_wan_upgrade_version == 1)
		version();
}
<% nvram_invmatch("upgrade_wan_test", "1", "*/"); %>
<% nvram_match("upgrade_wan_test", "1", "/*"); %>
function init()
{
}
<% nvram_match("upgrade_wan_test", "1", "*/"); %> 
</script>

</HEAD>

<BODY onload=init()>

<DIV align=center>

<FORM name="firmware" method=post action=upgrade.cgi encType=multipart/form-data>

<input type=hidden name=submit_button>

<input type=hidden name=change_action>

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

      style="FONT-SIZE: 15pt" face=Arial 

    color=#ffffff><script>Capture(bmenu.admin)</script></FONT></H3></TD>

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

          <% wireless_support(1); %><script>Capture(share.wireless_g)</script><% wireless_support(2); %><script>Capture(share.productname)</script></span>&nbsp;&nbsp;</FONT></TD>

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
                <TD width=103 height=1><IMG height=10 src="<% wireless_support(3); %>" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="<% wireless_support(4); %>" width=103 border=0></TD>
                <TD width=68  height=1><IMG height=10 src="image/UI_06.gif" width=68  border=0></TD></TR>
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

                <TD align=middle bgColor=#6666cc height=20>

                  <P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" 

                  color=#ffffff>&nbsp;&nbsp;<script>Capture(bmenu.admin)</script>&nbsp;&nbsp;</FONT></P></TD>

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
                      <TD class=small width=80><A href="Management.asp"><script>Capture(admtopmenu.manage)</script></A></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=80><a href="Log.asp"><script>Capture(admtopmenu.report)</script></a></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=80><A href="Diagnostics.asp"><script>Capture(admtopmenu.diag)</script></A></TD>
<% support_elsematch("BACKUP_RESTORE_SUPPORT","1","","<!--"); %>
		      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD  class=small width=110><A href="Backup_Restore.asp"><script>Capture(admtopmenu.bakrest)</script></A></TD>
<% support_elsematch("BACKUP_RESTORE_SUPPORT","1","","-->"); %>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=110><A href="Factory_Defaults.asp"><script>Capture(admtopmenu.facdef)</script></A></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD width=120><FONT style="COLOR: white"><script>Capture(admtopmenu.upgrade)</script></FONT></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=63><A href='Reboot.asp'><script>Capture(admtopmenu.reboot)</script></A></TD>
                    </TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>

<TABLE height=5 cellSpacing=0 cellPadding=0 width=809 bgColor=black border=0>

  <TBODY>

  <TR bgColor=black>

    <TD 

    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 

    borderColor=#e7e7e7 width=163 bgColor=#e7e7e7 height=1><IMG height=15 

      src="image/UI_03.gif" width=163 border=0></TD>

    <TD 

    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 

    width=646 height=1><IMG height=15 src="image/UI_02.gif" width=646 

      border=0></TD></TR></TBODY></TABLE>

<TABLE height=23 cellSpacing=0 cellPadding=0 width=809 border=0>

  <TBODY>

  <TR>

    <TD width=634>

      <TABLE cellSpacing=0 cellPadding=0 border=0 width="634">

        <TBODY>

	<% wan_upgrade_support(1); %>
	<TR>
          <TD align=right width=154 bgColor=#000000 colSpan=3

            height=25><B><FONT style="FONT-SIZE: 9pt" face=Arial

            color=#ffffff><script>Capture(admleftmenu.upgradewan)</script></FONT></B></TD>

          <TD width=8 bgColor=#000000 height=25>&nbsp;</TD>

          <TD width=8 colSpan=3 height=25>&nbsp;</TD>

          <TD width=428 colSpan=2 height=25>&nbsp;</TD>

          <TD width=16 height=25>&nbsp;</TD>

          <TD width=16 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

	<TR>

          <TD align=right width=154 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD width=8 colSpan=3 height=25>&nbsp;</TD>

          <TD width=428 colSpan=2 height=25>

          <p align="center"><FONT face=Arial

                   size=4 ><B><script>Capture(admupgradewan.upgradeweb)</script>:</B></FONT></p></TD>

          <TD width=16 height=25>&nbsp;</TD>

          <TD width=16 background=image/UI_05.gif

        height=25>&nbsp;</TD></TR>
	
	 <TR>

          <TD align=right width=154 bgColor=#e7e7e7 colSpan=3

          height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD width=8 colSpan=3 height=25>&nbsp;</TD>
	  <TD width=71 height=25 align=center><FONT style="FONT-SIZE: 8pt"><script>Capture(admupgradewan.url)</script>1:</FONT></TD>
	  <TD width=357 height=25 align=left><a href=<% nvram_get("wan_upgrade_url1"); %> target=_blank class="aa">
<b><font face=Arial size=2 color=#0000ff><% nvram_get("wan_upgrade_url1"); %></font></b></a></TD>

          <TD height=25>&nbsp;</TD>

          <TD background=image/UI_05.gif height=25>&nbsp;</TD></TR>

	 <TR>

          <TD align=right width=154 bgColor=#e7e7e7 colSpan=3

          height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25>&nbsp;</TD>

	  <TD width=71 height=25 align=center><FONT style="FONT-SIZE: 8pt"><script>Capture(admupgradewan.url)</script>2:</FONT></TD>
	  <TD width=357 height=25 align=left><a href=<% nvram_get("wan_upgrade_url2"); %> target=_blank class="aa">
<b><font face=Arial size=2 color=#0000ff><% nvram_get("wan_upgrade_url2"); %></font></b></a></TD>
          <TD height=25>&nbsp;</TD>

          <TD background=image/UI_05.gif height=25>&nbsp;</TD></TR>

	<TR>

          <TD align=right width=154 bgColor=#e7e7e7 colSpan=3

          height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25>&nbsp;</TD>

          <TD align=right colSpan=2 height=25>

          <p align="center"><FONT face=Arial color=#ff0000 size=2><script>Capture(admupgradewan.warning)</script></FONT></TD>

          <TD height=25>&nbsp;</TD>

          <TD background=image/UI_05.gif height=25>&nbsp;</TD></TR>
	<TR>
 <TD align=right width=154 bgColor=#e7e7e7 colSpan=3  height=25>&nbsp;</TD>
 <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD> 
 <TD colSpan=3 height=25>&nbsp;</TD>
 <TD colSpan=2>&nbsp;</TD>   
 <TD height=25>&nbsp;</TD>    
 <TD width=16 background=image/UI_05.gif                                                                                                                                                                                                                      height=25>&nbsp;</TD></TR>
	<TR>

          <TD align=right width=154 bgColor=#e7e7e7 colSpan=3

          height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <CENTER>

          <TD width=8 colSpan=3 height=25>&nbsp;</TD>

          <TD align=middle colSpan=2 height=25 width=428>&nbsp;<INPUT type=text name=process_wan size=40 maxlength=120 value='' onFocus=blur()></TD>

          <TD height=25 width=16>&nbsp;</TD></CENTER>

          <TD width=16 background=image/UI_05.gif

        height=25>&nbsp;</TD></TR>
	
	 <TR>

          <TD align=right width=154 bgColor=#e7e7e7 colSpan=3

          height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25>&nbsp;</TD>

          <TD align=middle colSpan=2 height=25><B><FONT color=red><script>Capture(admupgradewan.notinterrupted)</script></FONT></B></TD>

          <TD height=25>&nbsp;</TD>

          <TD width=16 background=image/UI_05.gif

        height=25>&nbsp;</TD></TR>
	
	<TR>

          <TD align=right width=154 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD width=8 colSpan=3 height=25>&nbsp;</TD>
          <TD align=center width=428 colSpan=2 height=25>
	    <script>document.write("<input onclick=upgrade_wan_1(this.form) type=button name=Upgrade_wan value=\"" + admbutton.upgrade + "\">");</script></TD>
          <TD width=16 height=25>&nbsp;</TD>
          <TD width=16 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
	<TR>

          <TD align=right width=151 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
	  <TD width=8 colSpan=3 height=25>&nbsp;</TD>
	  <TD width=428 colSpan=2 height=25>
                  <HR color=#b5b5e6 SIZE=1> </TD>
	  <TD width=16 height=25>&nbsp;</TD>
          <TD width=16 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

	<% wan_upgrade_support(2); %>

        <TR>
          <TD align=right width=154 bgColor=#000000 colSpan=3 

            height=25><B><FONT style="FONT-SIZE: 9pt" face=Arial 

            color=#ffffff><script>Capture(admleftmenu.upgradelan)</script></FONT></B></TD>

          <TD width=8 bgColor=#000000 height=25>&nbsp;</TD>

          <TD width=8 colSpan=3 height=25>&nbsp;</TD>

          <TD width=428 colSpan=2 height=25>&nbsp;</TD>

          <TD width=16 height=25>&nbsp;</TD>

          <TD width=16 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>

          <TD align=right width=154 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD width=8 colSpan=3 height=25>&nbsp;</TD>

          <TD width=428 colSpan=2 height=25>

          <p align="center"><FONT face=Arial 

                  color=#0000ff size=4><B><script>Capture(admupgradelan.upgradefw)</script></B></FONT></TD>

          <TD width=16 height=25>&nbsp;</TD>

          <TD width=16 background=image/UI_05.gif 

        height=25>&nbsp;</TD></TR>

        <TR>

          <TD align=right width=154 bgColor=#e7e7e7 colSpan=3 

          height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25>&nbsp;</TD>

          <TD align=center colSpan=2 height=25>&nbsp;<b><font size="2"><script>Capture(admupgradelan.filepath)</script></font></b><font size="2">:</font>&nbsp;&nbsp;<INPUT type=file name=file size="26"></TD>

          <TD height=25>&nbsp;</TD>

          <TD background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>

          <TD align=right width=154 bgColor=#e7e7e7 colSpan=3 

          height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25>&nbsp;</TD>

          <TD align=right colSpan=2 height=25>

          <p align="center"><FONT face=Arial color=#ff0000><script>Capture(admupgradelan.warning)</script></FONT></TD>

          <TD height=25>&nbsp;</TD>

          <TD background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>

          <TD align=right width=154 bgColor=#e7e7e7 colSpan=3 

          height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25>&nbsp;</TD>

          <TD colSpan=2>&nbsp;</TD>

          <TD height=25>&nbsp;</TD>

          <TD width=16 background=image/UI_05.gif 

        height=25>&nbsp;</TD></TR>

        <TR>

          <TD align=right width=154 bgColor=#e7e7e7 colSpan=3 

          height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <CENTER>

          <TD width=8 colSpan=3 height=25>&nbsp;</TD>

          <TD align=middle colSpan=2 height=25 width=428>&nbsp;<INPUT type=text name=process size=40 maxlength=120 value='' onFocus=blur()></TD>

          <TD height=25 width=16>&nbsp;</TD></CENTER>

          <TD width=16 background=image/UI_05.gif 

        height=25>&nbsp;</TD></TR>

        <TR>

          <TD align=right width=154 bgColor=#e7e7e7 colSpan=3 

          height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25>&nbsp;</TD>

          <TD align=middle colSpan=2 height=25><B><FONT color=red><script>Capture(admupgradelan.notinterrupted)</script></FONT></B></TD>

          <TD height=25>&nbsp;</TD>

          <TD width=16 background=image/UI_05.gif 

        height=25>&nbsp;</TD></TR>
<TR>

          <TD align=right width=154 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD width=8 colSpan=3 height=25>&nbsp;</TD>
          <TD align=center width=428 colSpan=2 height=25>
	    <script>document.write("<input onclick=upgrade_lan(this.form) type=button name=Upgrade_lan value=\"" + admbutton.upgrade + "\">");</script></TD>
          <TD width=16 height=25>&nbsp;</TD>
          <TD width=16 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

</TBODY></TABLE></TD>

    <TD vAlign=top width=176 bgColor=#6666cc>

      <TABLE cellSpacing=0 cellPadding=0 width=176 border=0>

        <TBODY>

        <TR>

          <TD width=11 bgColor=#6666cc height=25>&nbsp;</TD>

          <TD width=154 bgColor=#6666cc height=25><a target="_blank" href="help/HFirmwareUpgrade.asp"><script>Capture(share.more)</script>...</a></TD>

          <TD width=9 bgColor=#6666cc 

  height=25>&nbsp;</TD></TR></TBODY></TABLE></TD></TR>
  
  <TR>
<TD width=809 colSpan=2>
<TABLE cellSpacing=0 cellPadding=0 border=0>

<TR>
<TD width=154 bgColor=#e7e7e height=30>&nbsp;</TD>
<TD width=8 ><IMG src='image/UI_04.gif' border=0 width=8 height=30></TD>
<TD width=16>&nbsp;</TD>
<TD width=12>&nbsp;</TD>
<TD width=411>&nbsp;</TD>
<TD width=15>
<TD width=15 ><IMG src='image/UI_05.gif' border=0 width=15 height=30></TD>
<TD width=176 bgColor=#6666cc rowSpan=2><IMG height=64 
src='image/UI_Cisco.gif' width=176 border=0></TD></TR>
<TR>
<TD width=155 bgColor=#000000>&nbsp;</TD>
<TD width=8 bgColor=#000000>&nbsp;</TD>
<TD width=16 bgColor=#6666cc>&nbsp;</TD>
<TD width=12 bgColor=#6666cc>&nbsp;</TD>
<TD width=412 bgColor=#6666cc>
<TABLE height=19 cellSpacing=0 cellPadding=0 width=220 align=right border=0>


<TR>
<TD align=middle width=101>&nbsp;</TD>
<TD align=middle width=8 bgColor=#6666cc>&nbsp;</TD>
<TD align=middle width=111 bgColor=#434a8f><FONT color=#ffffff><B><A href='javascript:cancel(document.forms[0]);'><script>Capture(admbutton.cancelupgrade)</script></A></B></FONT></TD>
</TR>
</TABLE></TD>
<TD width=15 bgColor=#6666cc height=33>&nbsp;</TD>
<TD width=15 bgColor=#000000 height=33>&nbsp;</TD>
</TR></TABLE></TD></TR>
  </TBODY></TABLE></FORM></DIV></BODY></HTML>
