
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

<HTML><HEAD><TITLE>Forward</TITLE>
<% no_cache(); %>

<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<link rel="stylesheet" type="text/css" href="style.css">
<style fprolloverstyle>
A:hover {color: #00FFFF}
.small A:hover {color: #00FFFF}
TD {FONT-WEIGHT: normal; FONT-SIZE: 8pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal}.headrow {	BACKGROUND: #cccccc; FONT: bold 8pt Arial,Helvetica,sans-serif; COLOR: black}</style>
<SCRIPT src="common.js"></SCRIPT>
<SCRIPT language="javascript" type="text/javascript" src="share.js"></SCRIPT>
<SCRIPT language="javascript" type="text/javascript" src="capapp.js"></SCRIPT>
<SCRIPT language=JavaScript>
//document.title=(apptopmenu.forward);
document.title=(apptopmenu.portrange);
<% wireless_support(5); %>share.productname = share.productname1;<% wireless_support(6); %>
function to_submit(F)

{

        F.submit_button.value = "Forward";

        F.action.value = "Apply";

        F.submit();

}



</SCRIPT></HEAD>

<BODY>

<DIV align=center>

<FORM name=portRange method=<% get_http_method(); %> action=apply.cgi>

<input type=hidden name=submit_button>

<input type=hidden name=action>

<input type=hidden name="forward_port" value="10">

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

      style="FONT-SIZE: 15pt" face=Arial color=#ffffff><script>Capture(bmenu.application)</script><BR>&amp; <script>Capture(bmenu.gaming)</script></FONT></H3></TD>

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
                <TD width=83  height=1><IMG height=10 src="image/UI_06.gif" width=83  border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="<% wireless_support(3); %>" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="<% wireless_support(4); %>" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=68  height=1><IMG height=10 src="image/UI_06.gif" width=68  border=0></TD></TR>
              <TR>

                <TD align=middle bgColor=#000000 height=20><FONT 

                  style="FONT-WEIGHT: 700" color=#ffffff><a 

                  style="TEXT-DECORATION: none" 

                  href="index.asp"><script>Capture(bmenu.setup)</script></a></FONT></TD>

                <% wireless_support(1); %><TD align=middle bgColor=#000000 height=20><FONT 

                  style="FONT-WEIGHT: 700" color=#ffffff>

                <a style="TEXT-DECORATION: none" href="Wireless_Basic.asp"><script>Capture(bmenu.wireless)</script></a></FONT></TD>
<% wireless_support(2); %>

                <TD align=middle bgColor=#000000 height=20><FONT 

                  style="FONT-WEIGHT: 700" color=#ffffff>

                <a style="TEXT-DECORATION: none" href="Firewall.asp"><script>Capture(bmenu.security)</script></a></FONT></TD>

                <TD align=middle bgColor=#000000 height=20><FONT 

                  style="FONT-WEIGHT: 700" color=#ffffff>

                <a style="TEXT-DECORATION: none" href="<% support_elsematch("PARENTAL_CONTROL_SUPPORT", "1", "Parental_Control.asp", "Filters.asp"); %>"><script>Capture(bmenu.accrestriction)</script></a></FONT></TD>

                <TD align=middle bgColor=#6666cc height=20>

                  <P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" 

                  color=#ffffff><script>Capture(bmenu.application)</script><BR>&amp; <script>Capture(bmenu.gaming)</script></a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>

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
			<TD width=230 class=small><A href="Forward_UPnP.asp"><script>Capture(apptopmenu.singleport)</script></A></TD>
			<TD><P class=bar><font color='white'><b>|</b></font></P></TD>
			<TD width=250 class=small><FONT style="COLOR: white"><script>Capture(apptopmenu.portrange)</script></A></TD>
			<TD><P class=bar><font color='white'><b>|</b></font></P></TD>
			<TD width=200 class=small><A href="Triggering.asp"><script>Capture(apptopmenu.porttrigger)</script></A></TD>
			<TD><P class=bar><font color='white'><b>|</b></font></P></TD>
			<TD width=120 class=small><A href="DMZ.asp"><script>Capture(apptopmenu.dmz)</script></a></TD>
			<TD><P class=bar><font color='white'><b>|</b></font></P></TD>
			<TD width=120 class=small><A href="qos.asp"><script>Capture(apptopmenu.qos)</script></a></TD>
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

      <TABLE cellSpacing=0 cellPadding=0 border=0>

        <TBODY>

        <TR>

          <TD align=right width=156 bgColor=#000000 colSpan=3 

            height=25><B><FONT face=Arial color=#ffffff><script>Capture(apptopmenu.portrange)</script></FONT></B></TD>

          <TD width=8 bgColor=#000000 height=25>&nbsp;</TD>

          <TD width=14 height=25>&nbsp;</TD>

          <TD width=17 height=25>&nbsp;</TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=101 height=25>&nbsp;</TD>

          <TD width=296 height=25>&nbsp;</TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif>&nbsp;</TD>

          <TD width=14 height=25>&nbsp;</TD>

          <TD colSpan=4>

            <CENTER>

            <TABLE style="BORDER-COLLAPSE: collapse" borderColor=#e7e7e7 height=24 cellSpacing=0 cellPadding=0 width=421 border=0>

              <TBODY>

              <tr>
				<TD width=66 class=headrow height=30>&nbsp; </TD><TD class=headrow colSpan=2 height=30 align=center>
				<B style="font-weight: bold"><script>Capture(appportrange.portrange)</script></B></TD>
				<TD width=217 colspan=3 class=headrow height=30>&nbsp; </TD>
				</tr>

              <TR>

                <TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle width=66 bgColor=#CCCCCC height=30><B><script>Capture(appportrange.app)</script></B></TD>

                <TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle width=82 bgColor=#CCCCCC height=30><B><script>Capture(appportrange.start)</script></B></TD>

                <TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle width=56 bgColor=#CCCCCC height=30><B><script>Capture(appportrange.end)</script></B></TD>

                <TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle width=69 bgColor=#CCCCCC height=30><B><script>Capture(appportrange.proto)</script></B></TD>

                <TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle width=85 bgColor=#CCCCCC height=30><B><script>Capture(appportrange.ipaddr)</script></B></TD>

                <TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle width=47 bgColor=#CCCCCC height=30><B><script>Capture(appportrange.enabled)</script></B></TD></TR>

              <TR align=middle>

                <TD width=66 height=30><FONT size=2><INPUT  maxLength=12 size=7 name=name0 onBlur=valid_name(this,"Name") value='<% port_forward_table("name","0"); %>' class=num></FONT></TD>

                <TD width=82 height=30><FONT face="Arial, Helvetica, sans-serif"><INPUT  maxLength=5 size=5 value='<% port_forward_table("from","0"); %>' name=from0 onBlur=valid_range(this,0,65535,"Port") class=num><span >&nbsp; <script>Capture(appportrange.to)</script></span></FONT></TD>

                <TD width=57 height=30><INPUT  maxLength=5 size=5 value='<% port_forward_table("to","0"); %>' name=to0 onBlur=valid_range(this,0,65535,"Port") class=num></TD>

                <TD align=middle width=70 height=30><FONT face=Arial color=blue>

			<SELECT size=1 name=pro0>

				<OPTION value=tcp <% port_forward_table("sel_tcp","0"); %>><script>Capture(appportrange.tcp)</script></OPTION>

				<OPTION value=udp <% port_forward_table("sel_udp","0"); %>><script>Capture(appportrange.udp)</script></OPTION>

				<OPTION value=both <% port_forward_table("sel_both","0"); %>><script>Capture(appportrange.both)</script></OPTION>

			</SELECT></FONT></TD>

                <TD width=105 height=30><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><INPUT  maxLength=3 size=3 value='<% port_forward_table("ip","0"); %>' name=ip0 onBlur=valid_range(this,0,254,"IP") class=num></FONT></TD>

                <TD width=47 height=30><INPUT type=checkbox value=on name=enable0 <% port_forward_table("enable","0"); %>></TD></TR>



              <TR align=middle>

                <TD width=66 height=30><FONT size=2><INPUT  maxLength=12 size=7 name=name1 onBlur=valid_name(this,"Name") value='<% port_forward_table("name","1"); %>' class=num></FONT></TD>

                <TD width=82 height=30><FONT face="Arial, Helvetica, sans-serif"><INPUT  maxLength=5 size=5 value='<% port_forward_table("from","1"); %>' name=from1 onBlur=valid_range(this,0,65535,"Port") class=num><span >&nbsp; <script>Capture(appportrange.to)</script></span></FONT></TD>

                <TD width=57 height=30><INPUT  maxLength=5 size=5 value='<% port_forward_table("to","1"); %>' name=to1 onBlur=valid_range(this,0,65535,"Port") class=num></TD>

                <TD align=middle width=70 height=30><FONT face=Arial color=blue>

			<SELECT size=1 name=pro1>

				<OPTION value=tcp <% port_forward_table("sel_tcp","1"); %>><script>Capture(appportrange.tcp)</script></OPTION>

				<OPTION value=udp <% port_forward_table("sel_udp","1"); %>><script>Capture(appportrange.udp)</script></OPTION>

				<OPTION value=both <% port_forward_table("sel_both","1"); %>><script>Capture(appportrange.both)</script></OPTION>

			</SELECT></FONT></TD>

                <TD width=105 height=30><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><INPUT  maxLength=3 size=3 value='<% port_forward_table("ip","1"); %>' name=ip1 onBlur=valid_range(this,0,254,"IP") class=num></FONT></TD>

                <TD width=47 height=30><INPUT type=checkbox value=on name=enable1 <% port_forward_table("enable","1"); %>></TD></TR>

              <TR align=middle>

                <TD width=66 height=30><FONT size=2><INPUT  maxLength=12 size=7 name=name2 onBlur=valid_name(this,"Name") value='<% port_forward_table("name","2"); %>' class=num></FONT></TD>

                <TD width=82 height=30><FONT face="Arial, Helvetica, sans-serif"><INPUT  maxLength=5 size=5 value='<% port_forward_table("from","2"); %>' name=from2 onBlur=valid_range(this,0,65535,"Port") class=num><span >&nbsp; <script>Capture(appportrange.to)</script></span></FONT></TD>

                <TD width=57 height=30><INPUT  maxLength=5 size=5 value='<% port_forward_table("to","2"); %>' name=to2 onBlur=valid_range(this,0,65535,"Port") class=num></TD>

                <TD align=middle width=70 height=30><FONT face=Arial color=blue>

			<SELECT size=1 name=pro2>

				<OPTION value=tcp <% port_forward_table("sel_tcp","2"); %>><script>Capture(appportrange.tcp)</script></OPTION>

				<OPTION value=udp <% port_forward_table("sel_udp","2"); %>><script>Capture(appportrange.udp)</script></OPTION>

				<OPTION value=both <% port_forward_table("sel_both","2"); %>><script>Capture(appportrange.both)</script></OPTION>

			</SELECT></FONT></TD>

                <TD width=105 height=30><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><INPUT  maxLength=3 size=3 value='<% port_forward_table("ip","2"); %>' name=ip2 onBlur=valid_range(this,0,254,"IP") class=num></FONT></TD>

                <TD width=47 height=30><INPUT type=checkbox value=on name=enable2 <% port_forward_table("enable","2"); %>></TD></TR>

              <TR align=middle>

                <TD width=66 height=30><FONT size=2><INPUT  maxLength=12 size=7 name=name3 onBlur=valid_name(this,"Name") value='<% port_forward_table("name","3"); %>' class=num></FONT></TD>

                <TD width=82 height=30><FONT face="Arial, Helvetica, sans-serif"><INPUT  maxLength=5 size=5 value='<% port_forward_table("from","3"); %>' name=from3 onBlur=valid_range(this,0,65535,"Port") class=num><span >&nbsp; <script>Capture(appportrange.to)</script></span></FONT></TD>

                <TD width=57 height=30><INPUT  maxLength=5 size=5 value='<% port_forward_table("to","3"); %>' name=to3 onBlur=valid_range(this,0,65535,"Port") class=num></TD>

                <TD align=middle width=70 height=30><FONT face=Arial color=blue>

			<SELECT size=1 name=pro3>

				<OPTION value=tcp <% port_forward_table("sel_tcp","3"); %>><script>Capture(appportrange.tcp)</script></OPTION>

				<OPTION value=udp <% port_forward_table("sel_udp","3"); %>><script>Capture(appportrange.udp)</script></OPTION>

				<OPTION value=both <% port_forward_table("sel_both","3"); %>><script>Capture(appportrange.both)</script></OPTION>

			</SELECT></FONT></TD>

                <TD width=105 height=30><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><INPUT  maxLength=3 size=3 value='<% port_forward_table("ip","3"); %>' name=ip3 onBlur=valid_range(this,0,254,"IP") class=num></FONT></TD>

                <TD width=47 height=30><INPUT type=checkbox value=on name=enable3 <% port_forward_table("enable","3"); %>></TD></TR>

              <TR align=middle>

                <TD width=66 height=30><FONT size=2><INPUT  maxLength=12 size=7 name=name4 onBlur=valid_name(this,"Name") value='<% port_forward_table("name","4"); %>' class=num></FONT></TD>

                <TD width=82 height=30><FONT face="Arial, Helvetica, sans-serif"><INPUT  maxLength=5 size=5 value='<% port_forward_table("from","4"); %>' name=from4 onBlur=valid_range(this,0,65535,"Port") class=num><span >&nbsp; <script>Capture(appportrange.to)</script></span></FONT></TD>

                <TD width=57 height=30><INPUT  maxLength=5 size=5 value='<% port_forward_table("to","4"); %>' name=to4 onBlur=valid_range(this,0,65535,"Port") class=num></TD>

                <TD align=middle width=70 height=30><FONT face=Arial color=blue>

			<SELECT size=1 name=pro4>

				<OPTION value=tcp <% port_forward_table("sel_tcp","4"); %>><script>Capture(appportrange.tcp)</script></OPTION>

				<OPTION value=udp <% port_forward_table("sel_udp","4"); %>><script>Capture(appportrange.udp)</script></OPTION>

				<OPTION value=both <% port_forward_table("sel_both","4"); %>><script>Capture(appportrange.both)</script></OPTION>

			</SELECT></FONT></TD>

                <TD width=105 height=30><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><INPUT  maxLength=3 size=3 value='<% port_forward_table("ip","4"); %>' name=ip4 onBlur=valid_range(this,0,254,"IP") class=num></FONT></TD>

                <TD width=47 height=30><INPUT type=checkbox value=on name=enable4 <% port_forward_table("enable","4"); %>></TD></TR>

              <TR align=middle>

                <TD width=66 height=30><FONT size=2><INPUT  maxLength=12 size=7 name=name5 onBlur=valid_name(this,"Name") value='<% port_forward_table("name","5"); %>' class=num></FONT></TD>

                <TD width=82 height=30><FONT face="Arial, Helvetica, sans-serif"><INPUT  maxLength=5 size=5 value='<% port_forward_table("from","5"); %>' name=from5 onBlur=valid_range(this,0,65535,"Port") class=num><span >&nbsp; <script>Capture(appportrange.to)</script></span></FONT></TD>

                <TD width=57 height=30><INPUT  maxLength=5 size=5 value='<% port_forward_table("to","5"); %>' name=to5 onBlur=valid_range(this,0,65535,"Port") class=num></TD>

                <TD align=middle width=70 height=30><FONT face=Arial color=blue>

			<SELECT size=1 name=pro5>

				<OPTION value=tcp <% port_forward_table("sel_tcp","5"); %>><script>Capture(appportrange.tcp)</script></OPTION>

				<OPTION value=udp <% port_forward_table("sel_udp","5"); %>><script>Capture(appportrange.udp)</script></OPTION>

				<OPTION value=both <% port_forward_table("sel_both","5"); %>><script>Capture(appportrange.both)</script></OPTION>

			</SELECT></FONT></TD>

                <TD width=105 height=30><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><INPUT  maxLength=3 size=3 value='<% port_forward_table("ip","5"); %>' name=ip5 onBlur=valid_range(this,0,254,"IP") class=num></FONT></TD>

                <TD width=47 height=30><INPUT type=checkbox value=on name=enable5 <% port_forward_table("enable","5"); %>></TD></TR>

              <TR align=middle>

                <TD width=66 height=30><FONT size=2><INPUT  maxLength=12 size=7 name=name6 onBlur=valid_name(this,"Name") value='<% port_forward_table("name","6"); %>' class=num></FONT></TD>

                <TD width=82 height=30><FONT face="Arial, Helvetica, sans-serif"><INPUT  maxLength=5 size=5 value='<% port_forward_table("from","6"); %>' name=from6 onBlur=valid_range(this,0,65535,"Port") class=num><span >&nbsp; <script>Capture(appportrange.to)</script></span></FONT></TD>

                <TD width=57 height=30><INPUT  maxLength=5 size=5 value='<% port_forward_table("to","6"); %>' name=to6 onBlur=valid_range(this,0,65535,"Port") class=num></TD>

                <TD align=middle width=70 height=30><FONT face=Arial color=blue>

			<SELECT size=1 name=pro6>

				<OPTION value=tcp <% port_forward_table("sel_tcp","6"); %>><script>Capture(appportrange.tcp)</script></OPTION>

				<OPTION value=udp <% port_forward_table("sel_udp","6"); %>><script>Capture(appportrange.udp)</script></OPTION>

				<OPTION value=both <% port_forward_table("sel_both","6"); %>><script>Capture(appportrange.both)</script></OPTION>

			</SELECT></FONT></TD>

                <TD width=105 height=30><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><INPUT  maxLength=3 size=3 value='<% port_forward_table("ip","6"); %>' name=ip6 onBlur=valid_range(this,0,254,"IP") class=num></FONT></TD>

                <TD width=47 height=30><INPUT type=checkbox value=on name=enable6 <% port_forward_table("enable","6"); %>></TD></TR>

              <TR align=middle>

                <TD width=66 height=30><FONT size=2><INPUT  maxLength=12 size=7 name=name7 onBlur=valid_name(this,"Name") value='<% port_forward_table("name","7"); %>' class=num></FONT></TD>

                <TD width=82 height=30><FONT face="Arial, Helvetica, sans-serif"><INPUT  maxLength=5 size=5 value='<% port_forward_table("from","7"); %>' name=from7 onBlur=valid_range(this,0,65535,"Port") class=num><span >&nbsp; <script>Capture(appportrange.to)</script></span></FONT></TD>

                <TD width=57 height=30><INPUT  maxLength=5 size=5 value='<% port_forward_table("to","7"); %>' name=to7 onBlur=valid_range(this,0,65535,"Port") class=num></TD>

                <TD align=middle width=70 height=30><FONT face=Arial color=blue>

			<SELECT size=1 name=pro7>

				<OPTION value=tcp <% port_forward_table("sel_tcp","7"); %>><script>Capture(appportrange.tcp)</script></OPTION>

				<OPTION value=udp <% port_forward_table("sel_udp","7"); %>><script>Capture(appportrange.udp)</script></OPTION>

				<OPTION value=both <% port_forward_table("sel_both","7"); %>><script>Capture(appportrange.both)</script></OPTION>

			</SELECT></FONT></TD>

                <TD width=105 height=30><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><INPUT  maxLength=3 size=3 value='<% port_forward_table("ip","7"); %>' name=ip7 onBlur=valid_range(this,0,254,"IP") class=num></FONT></TD>

                <TD width=47 height=30><INPUT type=checkbox value=on name=enable7 <% port_forward_table("enable","7"); %>></TD></TR>

              <TR align=middle>

                <TD width=66 height=30><FONT size=2><INPUT  maxLength=12 size=7 name=name8 onBlur=valid_name(this,"Name") value='<% port_forward_table("name","8"); %>' class=num></FONT></TD>

                <TD width=82 height=30><FONT face="Arial, Helvetica, sans-serif"><INPUT  maxLength=5 size=5 value='<% port_forward_table("from","8"); %>' name=from8 onBlur=valid_range(this,0,65535,"Port") class=num><span >&nbsp; <script>Capture(appportrange.to)</script></span></FONT></TD>

                <TD width=57 height=30><INPUT  maxLength=5 size=5 value='<% port_forward_table("to","8"); %>' name=to8 onBlur=valid_range(this,0,65535,"Port") class=num></TD>

                <TD align=middle width=70 height=30><FONT face=Arial color=blue>

			<SELECT size=1 name=pro8>

				<OPTION value=tcp <% port_forward_table("sel_tcp","8"); %>><script>Capture(appportrange.tcp)</script></OPTION>

				<OPTION value=udp <% port_forward_table("sel_udp","8"); %>><script>Capture(appportrange.udp)</script></OPTION>

				<OPTION value=both <% port_forward_table("sel_both","8"); %>><script>Capture(appportrange.both)</script></OPTION>

			</SELECT></FONT></TD>

                <TD width=105 height=30><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><INPUT  maxLength=3 size=3 value='<% port_forward_table("ip","8"); %>' name=ip8 onBlur=valid_range(this,0,254,"IP") class=num></FONT></TD>

                <TD width=47 height=30><INPUT type=checkbox value=on name=enable8 <% port_forward_table("enable","8"); %>></TD></TR>

              <TR align=middle>

                <TD width=66 height=30><FONT size=2><INPUT  maxLength=12 size=7 name=name9 onBlur=valid_name(this,"Name") value='<% port_forward_table("name","9"); %>' class=num></FONT></TD>

                <TD width=82 height=30><FONT face="Arial, Helvetica, sans-serif"><INPUT  maxLength=5 size=5 value='<% port_forward_table("from","9"); %>' name=from9 onBlur=valid_range(this,0,65535,"Port") class=num><span >&nbsp; <script>Capture(appportrange.to)</script></span></FONT></TD>

                <TD width=57 height=30><INPUT  maxLength=5 size=5 value='<% port_forward_table("to","9"); %>' name=to9 onBlur=valid_range(this,0,65535,"Port") class=num></TD>

                <TD align=middle width=70 height=30><FONT face=Arial color=blue>

			<SELECT size=1 name=pro9>

				<OPTION value=tcp <% port_forward_table("sel_tcp","9"); %>><script>Capture(appportrange.tcp)</script></OPTION>

				<OPTION value=udp <% port_forward_table("sel_udp","9"); %>><script>Capture(appportrange.udp)</script></OPTION>

				<OPTION value=both <% port_forward_table("sel_both","9"); %>><script>Capture(appportrange.both)</script></OPTION>

			</SELECT></FONT></TD>

                <TD width=105 height=30><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><INPUT  maxLength=3 size=3 value='<% port_forward_table("ip","9"); %>' name=ip9 onBlur=valid_range(this,0,254,"IP") class=num></FONT></TD>

                <TD width=47 height=30><INPUT type=checkbox value=on name=enable9 <% port_forward_table("enable","9"); %>></TD></TR>



                </TBODY></TABLE></CENTER></TD>

          <TD width=13 height=25>

          <TD width=15 

        background=image/UI_05.gif>&nbsp;</TD></TR></TBODY></TABLE></TD>

    <TD width=176 bgColor=#6666cc valign="top">

    <p style="line-height: 200%"><font color="#FFFFFF"><span >&nbsp;&nbsp; <a target="_blank" href="help/HPortRange.asp"><script>Capture(share.more)</script>...</a></span></font></TD></TR>
  
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
                <TD align=middle bgColor=#42498c width="101"><FONT color=#ffffff><B><A href='javascript:to_submit(document.forms[0])'><script>Capture(share.saves)</script></A></B></FONT>
                </TD>
                <TD align=middle width=8 bgColor=#6666cc>&nbsp;</TD>
                <TD align=middle bgColor=#434a8f width="111"><FONT color=#ffffff><B><A href='Forward.asp'><script>Capture(share.cancels)</script></A></B></FONT></TD>
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
