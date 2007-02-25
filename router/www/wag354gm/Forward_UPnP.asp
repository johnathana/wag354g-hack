
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

<HTML><HEAD><TITLE>UPnP Forward</TITLE>
<% no_cache(); %>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<link rel="stylesheet" type="text/css" href="style.css">
<style fprolloverstyle>
A:hover {color: #00FFFF}
.small A:hover {color: #00FFFF}
</style>
<script Src="common.js"></SCRIPT>
<SCRIPT language="Javascript" type="text/javascript" src="share.js"></SCRIPT>
<SCRIPT language="Javascript" type="text/javascript" src="capapp.js"></SCRIPT>
<script language=JavaScript>
//document.title=(share.upnp + " " + apptopmenu.forward);
document.title=(apptopmenu.singleport);
<% wireless_support(5); %>share.productname = share.productname1;<% wireless_support(6); %>
//add by lzh;
function SelPvcConnection(num,F)
{
	F.submit_button.value = "Forward_UPnP";
	F.change_action.value = "gozila_cgi";
	F.pvc_connection.value=F.pvc_connection.options[num].value;
	F.submit();
}

function to_submit(F)
{
        F.submit_button.value = "Forward_UPnP";
        F.action.value = "Apply";
        F.submit();
}

</script>

</head>

<BODY>
<DIV align=center>
<form name=forwardUpnp method=<% get_http_method(); %> action=apply.cgi>
<input type=hidden name=submit_button>
<input type=hidden name=action>
<input type=hidden name=change_action>
<input type=hidden name="forward_upnp" value="15">

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

      style="FONT-SIZE: 15pt" face=Arial color=#ffffff><script>Capture(bmenu.application)</script> <BR>&amp; <script>Capture(bmenu.gaming)</script></FONT></H3></TD>

    <TD 

    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 

    vAlign=center borderColor=#000000 width=646 bgColor=#000000 height=49>

      <TABLE 

      style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; BORDER-COLLAPSE: collapse; FONT-VARIANT: normal" 

      height=33 cellSpacing=0 cellPadding=0 bgColor=#6666cc border=0>

        <TBODY>

        <TR>

          <TD style="FONT-WEIGHT: bolder; FONT-SIZE: 10pt" align=right 

          bgColor=#6666cc height=33><FONT color=#ffffff><span><% wireless_support(1); %><script>Capture(share.wireless_g)</script><% wireless_support(2); %><script>Capture(share.productname)</script>&nbsp;&nbsp;</span></FONT></TD>

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

                <a style="TEXT-DECORATION: none" href="Wireless_Basic.asp"><script>Capture(bmenu.wireless)</script></a></FONT></TD><% wireless_support(2); %>

                <TD align=middle bgColor=#000000 height=20><FONT 

                  style="FONT-WEIGHT: 700" color=#ffffff>

                <a style="TEXT-DECORATION: none" href="Firewall.asp"><script>Capture(bmenu.security)</script></a></FONT></TD>

                <TD align=middle bgColor=#000000 height=20><FONT 

                  style="FONT-WEIGHT: 700" color=#ffffff>

                <a style="TEXT-DECORATION: none" href="<% support_elsematch("PARENTAL_CONTROL_SUPPORT", "1", "Parental_Control.asp", "Filters.asp"); %>"><script>Capture(bmenu.accrestriction)</script></a></FONT></TD>

                <TD align=middle bgColor=#6666cc height=20>

                  <P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" 

                  color=#ffffff><script>Capture(bmenu.application)</script> <BR>&amp; <script>Capture(bmenu.gaming)</script></a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>

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

                	<TD width=230 class=small><FONT style="COLOR: white"><script>Capture(apptopmenu.singleport)</script></A></TD>
			<TD><P class=bar><font color='white'><b>|</b></font></P></TD>
			<TD width=250 class=small><A href="Forward.asp"><script>Capture(apptopmenu.portrange)</script></A></TD>
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

          <TD align=right width=156 bgColor=#000000 

            height=25><B><FONT face=Arial color=#ffffff><script>Capture(appleftmenu.singleport)</script></FONT></B></TD>

          <TD width=8 bgColor=#000000 height=25>&nbsp;</TD>

          <TD width=14 height=25></TD>

          <TD width=17 height=25></TD>

          <TD width=64 height=25></TD>

          <TD width=50 height=25></TD>

          <TD width=296 height=25></TD>

          <TD width=13 height=25></TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

<% support_invmatch("MULTIPVC_SUPPORT", "1", "<!--"); %>
        <TR>

          <TD align=right width=156 bgColor=#e7e7e7 height=30><script>Capture(appleftmenu.pvcselect)</script></TD>
          <TD width=8 background=image/UI_04.gif>&nbsp;</TD>

          <TD width=14 height=25></TD>

          <TD width=17 height=25></TD>

          <TD width=114 height=25 colspan="2"><script>Capture(appsingleport.pvcselect)</script>:&nbsp;</TD>

          <TD width=296 height=25><SELECT name="pvc_connection" onChange=SelPvcConnection(this.form.pvc_connection.selectedIndex,this.form) size="1">
<% pvc_connection_table("select"); %></SELECT></TD>

          <TD width=13 height=25></TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
<% support_invmatch("MULTIPVC_SUPPORT", "1", "-->"); %>

<% support_match("MULTIPVC_SUPPORT", "1", "<!--"); %>
<input type=hidden name="pvc_connection" value=0>
<% support_match("MULTIPVC_SUPPORT", "1", "-->"); %>

        <TR>

          <TD align=right width=156 bgColor=#e7e7e7 height=30>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif>&nbsp;</TD>

          <TD width=14 height=25></TD>

          <TD width=17 height=25></TD>

          <TD width=114 height=25 colspan="2"></TD>

          <TD width=296 height=25></TD>

          <TD width=13 height=25></TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>


          <TD align=right width=156 bgColor=#e7e7e7 height=25><!--script>Capture(appleftmenu.portmaplist)</script>
            <p>&nbsp;</p>
            <p>&nbsp;</p>
            <p>&nbsp;</p>
            <p>&nbsp;</p>
            <p>&nbsp;</p>
            <p>&nbsp;</p>
            <p>&nbsp;</p>
            <p>&nbsp;</p>
            <p>&nbsp;</p>
            <p>&nbsp;</p>
            <p>&nbsp;</p>
            <p>&nbsp;</p>
            <p>&nbsp;</p>
            <p>&nbsp;</p-->
          </TD>

          <TD width=8 background=image/UI_04.gif>&nbsp;</TD>

          <TD width=14 height=25>&nbsp;</TD>

          <TD colSpan=4>

            <CENTER>

											<TABLE style='BORDER-COLLAPSE: collapse' borderColor=silver height=24 cellSpacing=0 cellPadding=0 width=421 border=1>
												<TR>
													<TD class=headrow align=middle height=30 width=78><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 8pt'><script>Capture(appsingleport.app)</script></font></B></TD>
													<TD class=headrow align=middle height=30 width=47><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 8pt'><script>Capture(appsingleport.extport)</script></font></B></TD>
													<TD class=headrow align=middle height=30 width=63><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 8pt'><script>Capture(appsingleport.intport)</script></font></B></TD>
													<TD class=headrow align=middle height=30 width=79><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 8pt'><script>Capture(appsingleport.proto)</script></font></B></TD>
													<TD class=headrow align=middle height=30 width=103><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 8pt'><script>Capture(appsingleport.ipaddr)</script></font></B></TD>
													<TD class=headrow align=middle height=30 width=13><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 8pt'><script>Capture(appsingleport.enabled)</script></font></B></TD>
												</TR>
												
												<TR align=center  height=30>
													<TD><input class=num name=name0 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","0"); %>'></TD>
													<TD><input class=num name=ext_port0 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","0"); %>'></TD>

													<TD><input class=num name=int_port0 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","0"); %>'></TD>
													<TD><FONT face=Arial color=blue>
														<SELECT size=1 name=protocol0 >
															<OPTION value=tcp <% forward_upnp("sel_tcp","0"); %>><script>Capture(appsingleport.tcp)</script></OPTION>
															<OPTION value=udp <% forward_upnp("sel_udp","0"); %>><script>Capture(appsingleport.udp)</script></OPTION>
														</SELECT>
													</FONT></TD>
													<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip0 size=3 maxlength=3 value='<% forward_upnp("ip","0"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
													<TD><input type=checkbox name=enabled0 value=1 <% forward_upnp("enabled","0"); %>></TD>
												</TR>
												
												<TR align=center  height=30>
													<TD><input class=num name=name1 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","1"); %>'></TD>
													<TD><input class=num name=ext_port1 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","1"); %>'></TD>
													<TD><input class=num name=int_port1 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","1"); %>'></TD>
													<TD><FONT face=Arial color=blue>
														<SELECT size=1 name=protocol1 >
															<OPTION value=tcp <% forward_upnp("sel_tcp","1"); %>><script>Capture(appsingleport.tcp)</script></OPTION>
															<OPTION value=udp <% forward_upnp("sel_udp","1"); %>><script>Capture(appsingleport.udp)</script></OPTION>
														</SELECT>
													</FONT></TD>
													<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip1 size=3 maxlength=3 value='<% forward_upnp("ip","1"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
													<TD><input type=checkbox name=enabled1 value=1 <% forward_upnp("enabled","1"); %>></TD>
												</TR>
												
												<TR align=center  height=30>
													<TD><input class=num name=name2 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","2"); %>'></TD>
													<TD><input class=num name=ext_port2 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","2"); %>'></TD>
													<TD><input class=num name=int_port2 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","2"); %>'></TD>
													<TD><FONT face=Arial color=blue>
														<SELECT size=1 name=protocol2 >
															<OPTION value=tcp <% forward_upnp("sel_tcp","2"); %>><script>Capture(appsingleport.tcp)</script></OPTION>
															<OPTION value=udp <% forward_upnp("sel_udp","2"); %>><script>Capture(appsingleport.udp)</script></OPTION>
														</SELECT>
													</FONT></TD>
													<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip2 size=3 maxlength=3 value='<% forward_upnp("ip","2"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
													<TD><input type=checkbox name=enabled2 value=1 <% forward_upnp("enabled","2"); %>></TD>
												</TR>
												
												<TR align=center  height=30>
													<TD><input class=num name=name3 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","3"); %>'></TD>
													<TD><input class=num name=ext_port3 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","3"); %>'></TD>
													<TD><input class=num name=int_port3 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","3"); %>'></TD>
													<TD><FONT face=Arial color=blue>
														<SELECT size=1 name=protocol3 >
															<OPTION value=tcp <% forward_upnp("sel_tcp","3"); %>><script>Capture(appsingleport.tcp)</script></OPTION>
															<OPTION value=udp <% forward_upnp("sel_udp","3"); %>><script>Capture(appsingleport.udp)</script></OPTION>
														</SELECT>
													</FONT></TD>
													<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip3 size=3 maxlength=3 value='<% forward_upnp("ip","3"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
													<TD><input type=checkbox name=enabled3 value=1 <% forward_upnp("enabled","3"); %>></TD>
												</TR>

												<TR align=center  height=30>
													<TD><input class=num name=name4 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","4"); %>'></TD>
													<TD><input class=num name=ext_port4 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","4"); %>'></TD>
													<TD><input class=num name=int_port4 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","4"); %>'></TD>
													<TD><FONT face=Arial color=blue>
														<SELECT size=1 name=protocol4 >
															<OPTION value=tcp <% forward_upnp("sel_tcp","4"); %>><script>Capture(appsingleport.tcp)</script></OPTION>
															<OPTION value=udp <% forward_upnp("sel_udp","4"); %>><script>Capture(appsingleport.udp)</script></OPTION>
														</SELECT>
													</FONT></TD>
													<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip4 size=3 maxlength=3 value='<% forward_upnp("ip","4"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
													<TD><input type=checkbox name=enabled4 value=1 <% forward_upnp("enabled","4"); %>></TD>
												</TR>

												<TR align=center  height=30>
													<TD><input class=num name=name5 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","5"); %>'></TD>
													<TD><input class=num name=ext_port5 valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","5"); %>'></TD>
													<TD><input class=num name=int_port5 valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","5"); %>'></TD>
													<TD><FONT face=Arial color=blue>
														<SELECT size=1 name=protocol5 >
															<OPTION value=tcp <% forward_upnp("sel_tcp","5"); %>><script>Capture(appsingleport.tcp)</script></OPTION>
															<OPTION value=udp <% forward_upnp("sel_udp","5"); %>><script>Capture(appsingleport.udp)</script></OPTION>
														</SELECT>
													</FONT></TD>
													<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip5 size=3 maxlength=3 value='<% forward_upnp("ip","5"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
													<TD><input type=checkbox name=enabled5 value=1 <% forward_upnp("enabled","5"); %>></TD>
												</TR>

												<TR align=center  height=30>
													<TD><input class=num name=name6 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","6"); %>'></TD>
													<TD><input class=num name=ext_port6 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","6"); %>'></TD>
													<TD><input class=num name=int_port6 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","6"); %>'></TD>
													<TD><FONT face=Arial color=blue>
														<SELECT size=1 name=protocol6 >
															<OPTION value=tcp <% forward_upnp("sel_tcp","6"); %>><script>Capture(appsingleport.tcp)</script></OPTION>
															<OPTION value=udp <% forward_upnp("sel_udp","6"); %>><script>Capture(appsingleport.udp)</script></OPTION>
														</SELECT>
													</FONT></TD>
													<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip6 size=3 maxlength=3 value='<% forward_upnp("ip","6"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
													<TD><input type=checkbox name=enabled6 value=1 <% forward_upnp("enabled","6"); %>></TD>
												</TR>

												<TR align=center  height=30>
													<TD><input class=num name=name7 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","7"); %>'></TD>
													<TD><input class=num name=ext_port7 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","7"); %>'></TD>
													<TD><input class=num name=int_port7 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","7"); %>'></TD>
													<TD><FONT face=Arial color=blue>
														<SELECT size=1 name=protocol7 >
															<OPTION value=tcp <% forward_upnp("sel_tcp","7"); %>><script>Capture(appsingleport.tcp)</script></OPTION>
															<OPTION value=udp <% forward_upnp("sel_udp","7"); %>><script>Capture(appsingleport.udp)</script></OPTION>
														</SELECT>
													</FONT></TD>
													<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip7 size=3 maxlength=3 value='<% forward_upnp("ip","7"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
													<TD><input type=checkbox name=enabled7 value=1 <% forward_upnp("enabled","7"); %>></TD>
												</TR>

												<TR align=center  height=30>
													<TD><input class=num name=name8 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","8"); %>'></TD>
													<TD><input class=num name=ext_port8 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","8"); %>'></TD>
													<TD><input class=num name=int_port8 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","8"); %>'></TD>
													<TD><FONT face=Arial color=blue>
														<SELECT size=1 name=protocol8 >
															<OPTION value=tcp <% forward_upnp("sel_tcp","8"); %>><script>Capture(appsingleport.tcp)</script></OPTION>
															<OPTION value=udp <% forward_upnp("sel_udp","8"); %>><script>Capture(appsingleport.udp)</script></OPTION>
														</SELECT>
													</FONT></TD>
													<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip8 size=3 maxlength=3 value='<% forward_upnp("ip","8"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
													<TD><input type=checkbox name=enabled8 value=1 <% forward_upnp("enabled","8"); %>></TD>
												</TR>

												<TR align=center  height=30>
													<TD><input class=num name=name9 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","9"); %>'></TD>
													<TD><input class=num name=ext_port9 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","9"); %>'></TD>
													<TD><input class=num name=int_port9 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","9"); %>'></TD>
													<TD><FONT face=Arial color=blue>
														<SELECT size=1 name=protocol9 >
															<OPTION value=tcp <% forward_upnp("sel_tcp","9"); %>><script>Capture(appsingleport.tcp)</script></OPTION>
															<OPTION value=udp <% forward_upnp("sel_udp","9"); %>><script>Capture(appsingleport.udp)</script></OPTION>
														</SELECT>
													</FONT></TD>
													<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip9 size=3 maxlength=3 value='<% forward_upnp("ip","9"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
													<TD><input type=checkbox name=enabled9 value=1 <% forward_upnp("enabled","9"); %>></TD>
												</TR>

												<TR align=center  height=30>
													<TD><input class=num name=name10 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","10"); %>'></TD>
													<TD><input class=num name=ext_port10 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","10"); %>'></TD>
													<TD><input class=num name=int_port10 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","10"); %>'></TD>
													<TD><FONT face=Arial color=blue>
														<SELECT size=1 name=protocol10 >
															<OPTION value=tcp <% forward_upnp("sel_tcp","10"); %>><script>Capture(appsingleport.tcp)</script></OPTION>
															<OPTION value=udp <% forward_upnp("sel_udp","10"); %>><script>Capture(appsingleport.udp)</script></OPTION>
														</SELECT>
													</FONT></TD>
													<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip10 size=3 maxlength=3 value='<% forward_upnp("ip","10"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
													<TD><input type=checkbox name=enabled10 value=1 <% forward_upnp("enabled","10"); %>></TD>
												</TR>

												<TR align=center  height=30>
													<TD><input class=num name=name11 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","11"); %>'></TD>
													<TD><input class=num name=ext_port11 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","11"); %>'></TD>
													<TD><input class=num name=int_port11 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","11"); %>'></TD>
													<TD><FONT face=Arial color=blue>
														<SELECT size=1 name=protocol11 >
															<OPTION value=tcp <% forward_upnp("sel_tcp","11"); %>><script>Capture(appsingleport.tcp)</script></OPTION>
															<OPTION value=udp <% forward_upnp("sel_udp","11"); %>><script>Capture(appsingleport.udp)</script></OPTION>
														</SELECT>
													</FONT></TD>
													<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip11 size=3 maxlength=3 value='<% forward_upnp("ip","11"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
													<TD><input type=checkbox name=enabled11 value=1 <% forward_upnp("enabled","11"); %>></TD>
												</TR>

												<TR align=center  height=30>
													<TD><input class=num name=name12 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","12"); %>'></TD>
													<TD><input class=num name=ext_port12 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","12"); %>'></TD>
													<TD><input class=num name=int_port12 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","12"); %>'></TD>
													<TD><FONT face=Arial color=blue>
														<SELECT size=1 name=protocol12>
															<OPTION value=tcp <% forward_upnp("sel_tcp","12"); %>><script>Capture(appsingleport.tcp)</script></OPTION>
															<OPTION value=udp <% forward_upnp("sel_udp","12"); %>><script>Capture(appsingleport.udp)</script></OPTION>
														</SELECT>
													</FONT></TD>
													<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip12 size=3 maxlength=3 value='<% forward_upnp("ip","12"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
													<TD><input type=checkbox name=enabled12 value=1 <% forward_upnp("enabled","12"); %>></TD>
												</TR>

												<TR align=center  height=30>
													<TD><input class=num name=name13 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","13"); %>'></TD>
													<TD><input class=num name=ext_port13 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","13"); %>'></TD>
													<TD><input class=num name=int_port13 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","13"); %>'></TD>
													<TD><FONT face=Arial color=blue>
														<SELECT size=1 name=protocol13>
															<OPTION value=tcp <% forward_upnp("sel_tcp","13"); %>><script>Capture(appsingleport.tcp)</script></OPTION>
															<OPTION value=udp <% forward_upnp("sel_udp","13"); %>><script>Capture(appsingleport.udp)</script></OPTION>
														</SELECT>
													</FONT></TD>
													<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip13 size=3 maxlength=3 value='<% forward_upnp("ip","13"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
													<TD><input type=checkbox name=enabled13 value=1 <% forward_upnp("enabled","13"); %>></TD>
												</TR>

												<TR align=center  height=30>
													<TD><input class=num name=name14 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","14"); %>'></TD>
													<TD><input class=num name=ext_port14 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","14"); %>'></TD>
													<TD><input class=num name=int_port14 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","14"); %>'></TD>
													<TD><FONT face=Arial color=blue>
														<SELECT size=1 name=protocol14>
															<OPTION value=tcp <% forward_upnp("sel_tcp","14"); %>><script>Capture(appsingleport.tcp)</script></OPTION>
															<OPTION value=udp <% forward_upnp("sel_udp","14"); %>><script>Capture(appsingleport.udp)</script></OPTION>
														</SELECT>
													</FONT></TD>
													<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip14 size=3 maxlength=3 value='<% forward_upnp("ip","14"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
													<TD><input type=checkbox name=enabled14 value=1 <% forward_upnp("enabled","14"); %>></TD>
												</TR>
												
											</TABLE>
                
                </CENTER></TD>

          <TD width=13 height=25>

          <TD width=15 

        background=image/UI_05.gif>&nbsp;</TD></TBODY></TABLE>
        
        </TD>

    <TD width=176 bgColor=#6666cc valign="top">

    <p style="line-height: 200%"><font color="#FFFFFF"><span >&nbsp;&nbsp; <a target="_blank" href="help/HPortForwarding.asp"><script>Capture(share.more)</script>...</a></span></font></TD></TR>
  
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

























