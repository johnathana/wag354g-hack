
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

<HTML><HEAD><TITLE>Backup & Restore</TITLE>

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
document.title=(admtopmenu.bakrest);
<% wireless_support(5); %>share.productname = share.productname1;<% wireless_support(6); %>
function to_restore(F)

{

	var len = F.restore.value.length;

	var ext = new Array('.','c','f','g');

	if (F.restore.value == '')	{

		//alert('Please select a file to upgrade !');
		alert(errmsg.err98);
		return false;

	}

	var IMAGE = F.restore.value.toLowerCase();

	for (i=0; i < 4; i++)	{

		if (ext[i] != IMAGE.charAt(len-4+i)){

			//alert('Incorrect image file!');
			alert(errmsg.err99);
			return false;

		}

	}



	F.submit_button.value = "Backup_Restore";

        F.submit();

	

}

</SCRIPT>

</HEAD>

<BODY vLink=#b5b5e6 aLink=#ffffff link=#b5b5e6>

<DIV align=center>

<FORM name=restore method=post action=restore.cgi encType=multipart/form-data>

<input type=hidden name=submit_button>



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


	<TD style="FONT-WEIGHT: bolder; FONT-SIZE: 10pt" align=right bgColor=#6666cc height=33><FONT color=#ffffff><% wireless_support(1); %><script>Capture(share.wireless_g)</script><% wireless_support(2); %><script>Capture(share.productname)</script>&nbsp;&nbsp;</FONT></TD>

          <TD borderColor=#000000 borderColorLight=#000000 align=middle width=109 bgColor=#000000 borderColorDark=#000000 height=12 rowSpan=2><FONT color=#ffffff><SPAN style="FONT-SIZE: 8pt"><B><% nvram_get("router_name"); %></B></SPAN></FONT></TD></TR>

          <!--TD style="FONT-WEIGHT: bolder; FONT-SIZE: 10pt" align=right 

          bgColor=#6666cc height=33><FONT color=#ffffff>Wireless-G Broadband Router&nbsp;&nbsp;</FONT></TD>

          <TD borderColor=#000000 borderColorLight=#000000 align=middle 

          width=109 bgColor=#000000 borderColorDark=#000000 height=12 

            rowSpan=2><FONT color=#ffffff><SPAN 

            style="FONT-SIZE: 8pt"><B><% nvram_get("router_name"); %></B></SPAN></FONT></TD></TR-->

        <TR>

          <TD 

          style="FONT-WEIGHT: normal; FONT-SIZE: 1pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 

          width=537 bgColor=#000000 height=1>¡@</TD></TR>

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

                <% wireless_support(1); %><TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Wireless_Basic.asp"><script>Capture(bmenu.wireless)</script></a></FONT></TD><% wireless_support(2); %>
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
                      <TD class=small width=80><a href="Management.asp"><script>Capture(admtopmenu.manage)</script></A></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=80><a href="Log.asp"><script>Capture(admtopmenu.report)</script></a></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=80><A href="Diagnostics.asp"><script>Capture(admtopmenu.diag)</script></A></TD>
		      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD  class=small width=110><FONT style="COLOR: white"><script>Capture(admtopmenu.bakrest)</script></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=110><A href="Factory_Defaults.asp"><script>Capture(admtopmenu.facdef)</script></A></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=120><A href="Upgrade.asp"><script>Capture(admtopmenu.upgrade)</script></A></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=63><A href="Reboot.asp"><script>Capture(admtopmenu.reboot)</script></A></TD>
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

    width=646 height=1><IMG height=15 src="image/UI_02.gif" 

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

            <P align=right><b>

                <font face="Arial" color="#FFFFFF" style="font-size: 8pt"><script>Capture(admleftmenu.bakconfig)</script></font></b></P></TD>

          <TD width=8 bgColor=#000000 height=25>&nbsp;</TD>

          <TD colSpan=6 height=25 width="454">&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif 

          height=25>&nbsp;</TD></TR>

	<TR>

	  <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>

	  <p align="right">

		<font style="font-weight: 700"><span >&nbsp;</span></font></TD>

	  <TD width=8 background=image/UI_04.gif  height=25>&nbsp;</TD>

	  <TD colSpan=3 height=25 width="42">&nbsp;</TD>

	  <TD width=399 height=25 colSpan=2>
	    <script>document.write("<INPUT onclick=window.location.href=\"/<% get_backup_name(); %>\" type=button value=\"" + admbutton.backup + "\">");</script></TD>

	  <TD width=13 height=25>&nbsp;</TD>

	  <TD width=15 background=image/UI_05.gif 

	  height=25>&nbsp;</TD></TR>

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>

            <P align=right>&nbsp;</P></TD>

          <TD width=8 background=image/UI_04.gif 

height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42">&nbsp;</TD>

          <TD width=149 height=25>

                &nbsp;</TD>

          <TD width=250 height=25><SPAN  

            style="FONT-SIZE: 8pt">&nbsp;</SPAN></TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif 

          height=25>&nbsp;</TD></TR>                

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=1>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>

          <TD colSpan=6 width="454">

            <TABLE>

              <TBODY>

              <TR>

                <TD width=16 height=1>&nbsp;</TD>

                <TD width=13 height=1>&nbsp;</TD>

                <TD width=410 colSpan=3 height=1>

                  <HR color=#b5b5e6 SIZE=1>

                </TD>

                <TD width=15>&nbsp;</TD></TR></TBODY></TABLE></TD>

          <TD width=15 background=image/UI_05.gif 

        height=1>&nbsp;</TD></TR>



        <TR>

          <TD width=156 bgColor=#000000 colSpan=3 height=25>

            <P align=right><B><FONT style="FONT-SIZE: 9pt" 

            color=#ffffff><span><script>Capture(admleftmenu.restconfig)</script></span></FONT></B></P></TD>

          <TD width=8 bgColor=#000000 height=25>&nbsp;</TD>

          <TD colSpan=6 width="454">

            <TABLE>

              <TBODY>

              <TR>

                <TD width=16 height=25>&nbsp;</TD>

                <TD width=12 height=25>&nbsp;</TD>

                <TD width=411 height=25>&nbsp;</TD>

                <TD width=15 height=25>&nbsp;</TD></TR></TBODY></TABLE></TD>

          <TD width=15 background=image/UI_05.gif 

          height=25>&nbsp;</TD></TR>

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>

          <p align="right">

                <font style="font-weight: 700"><span >&nbsp;</span></font></TD>

          <TD width=8 background=image/UI_04.gif 

height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42">&nbsp;</TD>

          <TD width=399 height=25 colSpan=2><script>Capture(admrestore.selectfile)</script>&nbsp;&nbsp;<INPUT type=file name=restore size="20"></TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif 

          height=25>&nbsp;</TD></TR>             

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>

          <p align="right">

                <font style="font-weight: 700"><span >&nbsp;</span></font></TD>

          <TD width=8 background=image/UI_04.gif  height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42">&nbsp;</TD>

          <TD width=399 height=25 colSpan=2>

		<script>document.write("<INPUT onclick=to_restore(this.form) type=button value=\"" + admbutton.restore + "\">");</script></TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif 

          height=25>&nbsp;</TD></TR>             

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=1>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>

          <TD colSpan=6 width="454">

            <TABLE>

              <TBODY>

              <TR>

                <TD width=16 height=1>&nbsp;</TD>

                <TD width=13 height=1>&nbsp;</TD>

                <TD width=410 colSpan=3 height=1>&nbsp;</TD>

                <TD width=15>&nbsp;</TD></TR></TBODY></TABLE></TD>

          <TD width=15 background=image/UI_05.gif height=1>&nbsp;</TD></TR>

        <TR>

          <TD width=43 bgColor=#e7e7e7 height=5>&nbsp;</TD>

          <TD width=64 bgColor=#e7e7e7 height=5>&nbsp;</TD>

          <TD width=49 bgColor=#e7e7e7 height=5>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=5>&nbsp;</TD>

          <TD width=11 height=5></TD>

          <TD width=18 height=5></TD>

          <TD width=13 height=5></TD>

          <TD width=149 height=5></TD>

          <TD width=250 height=5></TD>

          <TD width=13 height=5></TD>

          <TD width=15 background=image/UI_05.gif height=5>&nbsp;</TD></TR></TBODY></TABLE></TD>

    <TD vAlign=top width=176 bgColor=#6666cc>

      <TABLE cellSpacing=0 cellPadding=0 width=176 border=0>

        <TBODY>

        <TR>

          <TD width=11 bgColor=#6666cc height=25>&nbsp;</TD>

          <TD width=156 bgColor=#6666cc height=25><font color="#FFFFFF">

	  <span><a target="_blank" href="help/HBackup.asp"><script>Capture(share.more)</script>...</a></span></font></TD>

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

          <TD width=15>

          <TD width=15 background=image/UI_05.gif>&nbsp;</TD>

          <TD width=176 bgColor=#6666cc height=58 rowSpan=2>

          <IMG 

            src="image/UI_Cisco.gif" border=0 width="176" height="64"></TD></TR>

        <TR>

          <TD width=156 bgColor=#000000>&nbsp;</TD>

          <TD width=8 bgColor=#000000>&nbsp;</TD>

          <TD width=16 bgColor=#6666cc>&nbsp;</TD>

          <TD width=12 bgColor=#6666cc>&nbsp;</TD>

          <TD width=411 bgColor=#6666cc>

            &nbsp;</TD>

          <TD width=15 bgColor=#6666cc height=33>&nbsp;</TD>

          <TD width=15 bgColor=#000000 

  height=33>&nbsp;</TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></FORM></DIV></BODY></HTML>

