
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

<HTML>
<HEAD>
<TITLE>Status</TITLE>
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
<SCRIPT language="Javascript" type="text/javascript" src="capstatus.js"></SCRIPT>
<script language=javascript>
document.title=(setddns.statu);
<% wireless_support(5); %>share.productname = share.productname1;<% wireless_support(6); %>
function SelCONNECTION(num,F)
{
 	F.submit_button.value = "Status_DSL";
    F.change_action.value = "gozila_cgi";
    F.status_connection.value=F.status_connection.options[num].value;
    F.submit();
}

var value=0;
function Refresh()
{
	var refresh_time = "<% show_status("refresh_time"); %>";
	if(refresh_time == "")	refresh_time = 1500000;
	if (value>=1)
	{
		window.location.replace("Summary.asp");
	}
	value++;
	timerID=setTimeout("Refresh()",refresh_time);
}

function init()
{
	Refresh();
}
</script>

</HEAD>

<BODY onload=init()>

<DIV align=center>

<FORM name=setup method=<% get_http_method(); %> action=apply.cgi>
	<input type=hidden name=submit_button>
	<input type=hidden name=change_action>
	
	<TABLE cellSpacing=0 cellPadding=0 width=809 border=0>
	<TBODY>
  	<TR>
	    <TD width=95><IMG src="image/UI_Linksys.gif" border=0 width="165" height="57"></TD>
    	<TD vAlign=bottom align=right width=714 bgColor=#6666cc><FONT style="FONT-SIZE: 7pt" color=#ffffff><FONT face=Arial><script>Capture(share.firmwarever)</script>:&nbsp;<% get_firmware_version(); %>&nbsp;&nbsp;&nbsp;</FONT></FONT></TD>
   	</TR>
    <TR>
    	<TD width=808 bgColor=#6666cc colSpan=2><IMG height=11 src="image/UI_10.gif" width=809 border=0></TD>
    </TR>
    </TBODY>
    </TABLE>
	      
	<TABLE height=77 cellSpacing=0 cellPadding=0 width=809 bgColor=black border=0>
  	<TBODY>
  	<TR>
    	<TD style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
    		borderColor=#000000 align=middle width=163 height=49>
      		<H3 style="MARGIN-TOP: 1px; MARGIN-BOTTOM: 1px"><font face="Arial" style="font-size: 15pt" color="#FFFFFF">Summary</font></H3>
      	</TD>
   		<TD style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
		    vAlign=center borderColor=#000000 width=646 bgColor=#000000 height=49>
      		<TABLE style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; BORDER-COLLAPSE: collapse; FONT-VARIANT: normal" 
			       height=33 cellSpacing=0 cellPadding=0 bgColor=#6666cc border=0>
        	<TBODY>
        	<TR>
          		<TD style="FONT-WEIGHT: bolder; FONT-SIZE: 10pt" align=right bgColor=#6666cc height=33>
          			<FONT color=#ffffff><script>Capture(share.productname)</script>&nbsp;&nbsp;</FONT></TD>
          		</TD>
          		<TD borderColor=#000000 borderColorLight=#000000 align=middle width=109 bgColor=#000000 borderColorDark=#000000 height=12 rowSpan=2>
          			<FONT color=#ffffff><SPAN style="FONT-SIZE: 8pt"><B><% nvram_get("router_name"); %></B></SPAN></FONT>
          		</TD>
          	</TR>
        <TR>
        	<TD style="FONT-WEIGHT: normal; FONT-SIZE: 1pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
		        width=537 bgColor=#000000 height=1>&nbsp;
		    </TD>
		</TR>
        <TR>
          <TD width=646 bgColor=#000000 colSpan=2 height=32>
            <TABLE id=AutoNumber1 style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; BORDER-COLLAPSE: collapse; FONT-VARIANT: normal" 
            	   height=6 cellSpacing=0 cellPadding=0 width=637 border=0>
            <TBODY>
            <TR style="BORDER-RIGHT: medium none; BORDER-TOP: medium none; FONT-WEIGHT: normal; FONT-SIZE: 1pt; BORDER-LEFT: medium none; COLOR: black; BORDER-BOTTOM: medium none; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
              	align=middle bgColor=#6666cc>
                <TD width=83 height=1><IMG height=10 src="image/UI_06.gif" width=83 border=0></TD>
                <TD width=73 height=1><IMG height=10 src="image/UI_06.gif" width=83 border=0></TD>
                <TD width=113 height=1><IMG height=10 src="image/UI_06.gif" width=83 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=85 height=1><IMG height=10 src="image/UI_06.gif" width=100 border=0></TD>
                <TD width=115 height=1><IMG height=10 src="<% wireless_support(3); %>" width=115 border=0></TD>
                <TD width=74 height=1><IMG height=10 src="<% wireless_support(4); %>" width=79 border=0></TD>
            </TR>
            <TR>
                <TD align=middle bgColor=#000000 height=20>
                	<FONT style="FONT-WEIGHT: 700" color=#ffffff><A style="TEXT-DECORATION: none" href="index.asp"><script>Capture(bmenu.setup)</script></A></FONT>
                </TD>
                <% wireless_support(1); %>
                <TD align=middle bgColor=#000000 height=20>
                	<FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Wireless_Basic.asp"><script>Capture(bmenu.wireless)</script></a></FONT>
               	</TD>
               	<% wireless_support(2); %>
                <TD align=middle bgColor=#000000 height=20>
                	<FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Firewall.asp"><script>Capture(bmenu.security)</script></a></FONT>
                </TD>
                <TD align=middle bgColor=#000000 height=20>
                	<FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="<% support_elsematch("PARENTAL_CONTROL_SUPPORT", "1", "Parental_Control.asp", "Filters.asp"); %>"><script>Capture(bmenu.accrestriction)</script></a></FONT>
               	</TD>
                <TD align=middle bgColor=#000000 height=20>
                  <P style="MARGIN-BOTTOM: 4px">
                  <FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Forward_UPnP.asp"><script>Capture(bmenu.application)</script> <BR>&amp; <script>Capture(bmenu.gaming)</script></a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT>
                  </P>
                </TD>
                <TD align=middle bgColor=#000000 height=20>
                  <P style="MARGIN-BOTTOM: 4px">
                  <FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Management.asp"><script>Capture(bmenu.admin)</script></a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT>
                  </P>
                </TD>
                <TD align=middle bgColor=#6666cc height=20>
                  <P style="MARGIN-BOTTOM: 4px">
                  <FONT style="FONT-WEIGHT: 700" color=#ffffff><script>Capture(bmenu.statu)</script>&nbsp;&nbsp;&nbsp;&nbsp;</FONT>
                  </P>
                </TD>
            </TR>
            <TR>
                <TD width=643 bgColor=#6666cc colSpan=7 height=21>
                  <TABLE borderColor=black height=21 cellSpacing=0 cellPadding=0>
                  <TBODY>
                  <TR align=left>
                  	<TD width=25></TD>
                    <TD class=small width=65><A href="Status_Router.asp"><script>Capture(share.gateway)</script></A></TD>
                    <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                    <TD width=25></TD>
                    <TD class=small width=100><A href="Status_Lan.asp"><script>Capture()</script>Local Network</FONT></TD>
                    <% wireless_support(1); %>
                    <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                    <TD width=25></TD>
                    <TD class=small width=70><span >&nbsp;</span><A href="Status_Wireless.asp"><script>Capture(bmenu.wireless)</script></A></TD>
                    <% wireless_support(2); %>
		      		<TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
		      		<TD width=25></TD>
                    <TD class=small width=100><span>&nbsp;</span><A href="Status_DSL.asp"><script>Capture()</script>DSL Connection</A></TD>
					<TD  width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
					<TD width=25></TD>
					<TD width=75><FONT style="COLOR: white"><span>&nbsp;</span><script>Capture(share.summary)</script></FONT></TD>
                  </TR>
                  </TBODY>
                  </TABLE>
                </TD>
            </TR>
            </TBODY>
            </TABLE>
          </TD>
        </TR>
        </TBODY>
        </TABLE>
        </TD>
	</TR>
	</TBODY>
	</TABLE>
 
 
<TABLE height=5 cellSpacing=0 cellPadding=0 width=806 bgColor=black border=0>
  <TBODY>
  <TR bgColor=black>
    <TD 
    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
    borderColor=#e7e7e7 width=163 bgColor=#e7e7e7 height=1><IMG height=15 
      src="image/UI_03.gif" width=119 border=0></TD>
    <TD 
    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
    width=646 height=1><IMG height=15 src="image/UI_02.gif" 
      width=690 border=0></TD></TR></TBODY></TABLE>

<!--             
	<TABLE height=5 cellSpacing=0 cellPadding=0 width=806 bgColor=black border=0>
  	<TBODY>
  	<TR bgColor=black>
    	<TD style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
    		borderColor=#e7e7e7 width=156 bgColor=#e7e7e7 height=1>
    		<IMG height=15 src="image/UI_03.gif" width=119 border=0>
    	</TD>
    	<TD style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
		    width=590 height=1>
		    <IMG height=19 src="image/UI_02.gif" width=690 border=0>
		</TD>
	</TR>
	</TBODY>
	</TABLE>
-->
	<TABLE id=AutoNumber9 style="BORDER-COLLAPSE: collapse" borderColor=#111111 height=23 cellSpacing=0 cellPadding=0 width=809 border=0>
  	<TBODY>
  	<TR>
    	<TD width=633>
      		<TABLE cellSpacing=0 cellPadding=0 border=0 width="641">
        	<TBODY>
        	<TR>
          		<TD width=87 bgColor=#000000 height=25>
            		<P align=center><b>
                    <font face="Arial" style="font-size: 9pt" color="#FFFFFF">
                    <script>Capture(share.gateway)</script></font></b></P>
            	</TD>
          		<TD width=13 bgColor=#000000 height=25>&nbsp;</TD>
          		<TD width=21 height=25 colspan="3">&nbsp;</TD>
          		<TD width=195 height=25>&nbsp;</TD>
          		<TD width=271 height=25>&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
 	
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture(share.firmwarever</script>:</TD>
          		<TD width=271 height=25><B><% get_firmware_version(); %>, Aug 15 2004</B></TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
        
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture(share.macaddr)</script>:</TD>
          		<TD width=271 height=25><b><% hwaddr_get("wan"); %></TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>

			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture()</script>Current Time:</TD>
          		<TD width=271 height=25><B><% localtime( "<script>Capture(share.notava)</script>" ); %></B></TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>

        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD width="195">Connection:</TD>
          		<TD width="271">
          			<SELECT name="status_connection" onChange=SelCONNECTION(this.form.status_connection.selectedIndex,this.form)>
          				<% status_connection_table("select"); %>
          			</SELECT>
          		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture()</script>Login Type:</TD>
          		<TD width=271 height=25><b><% nvram_wanstatus_get("login_type", 10); %></b></TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
 		 	<% show_status_setting(); %>

			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture(setnetsetup.timezone)</script>:</TD>
          		<TD width=271 height=25>
          			<b>
          			<script language=javascript>
          				var time_zone = "<% nvram_get("time_zone");%>";
  	
          				if(time_zone == "-12 1 0")
          					document.write("GMT-12:00 Kwajalein");
          				else if(time_zone == "-11 1 0")
          					document.write("GMT-11:00 Midway Island, Samoa");
          				else if(time_zone == "-10 1 0")
          					document.write("GMT-10:00 Hawaii");
          				else if(time_zone == "-09 1 1")
          					document.write("GMT-09:00 Alaska");
          				else if(time_zone == "-08 1 1")
          					document.write("GMT-08:00 Pacific Time (USA &amp; Canada)");
          				else if(time_zone == "-07 1 0")
          					document.write("GMT-07:00 Arizona");
          				else if(time_zone == "-07 2 1")
          					document.write("GMT-07:00 Mountain Time (USA &amp; Canada)");
          				else if(time_zone == "-06 1 0")
          					document.write("GMT-06:00 Mexico");
          				else if(time_zone == "-06 2 1")
          					document.write("GMT-06:00 Central Time (USA &amp; Canada)");
          				else if(time_zone == "-05 1 0")
          					document.write("GMT-05:00 Indiana East, Colombia, Panama");
          				else if(time_zone == "-05 2 1")
          					document.write("GMT-05:00 Eastern Time (USA &amp; Canada)");
          				else if(time_zone == "-04 1 0")
          					document.write("GMT-04:00 Bolivia, Venezuela");
          				else if(time_zone == "-04 2 1")
          					document.write("GMT-04:00 Atlantic Time (Canada), Brazil West");
          				else if(time_zone == "-03.5 1 1")
          					document.write("GMT-03:30 Newfoundland");
          				else if(time_zone == "-03 1 0")
          					document.write("GMT-03:00 Guyana");
          				else if(time_zone == "-03 2 1")
          					document.write("GMT-03:00 Brazil East, Greenland");
          				else if(time_zone == "-02 1 0")
          					document.write("GMT-02:00 Mid-Atlantic");
          				else if(time_zone == "-01 1 2")
          					document.write("GMT-01:00 Azores");
          				else if(time_zone == "+00 1 0")
          					document.write("GMT Gambia, Liberia, Morocco");
          				else if(time_zone == "+00 2 2")
          					document.write("GMT England");
          				else if(time_zone == "+01 1 0")
          					document.write("GMT+01:00 Tunisia");
          				else if(time_zone == "+01 2 2")
          					document.write("GMT+01:00 France, Germany, Italy");
          				else if(time_zone == "+02 1 0")
          					document.write("GMT+02:00 South Africa");
          				else if(time_zone == "+02 2 2")
          					document.write("GMT+02:00 Greece, Ukraine, Romania, Turkey");
          				else if(time_zone == "+03 1 0")
          					document.write("GMT+03:00 Iraq, Jordan, Kuwait");
          				else if(time_zone == "+04 1 0")
          					document.write("GMT+04:00 Armenia")
          				else if(time_zone == "+05 1 0")
          					document.write("GMT+05:00 Pakistan, Russia");
          				else if(time_zone == "+06 1 0")
          					document.write("GMT+06:00 Bangladesh, Russia");
          				else if(time_zone == "+07 1 0")
          					document.write("GMT+07:00 Thailand, Russia");
          				else if(time_zone == "+08 1 0")
          					document.write("GMT+08:00 China, Hong Kong, Australia Western");
          				else if(time_zone == "+08 2 0")
          					document.write("GMT+08:00 Singapore, Taiwan, Russia");
          				else if(time_zone == "+09 1 0")
          					document.write("GMT+09:00 Japan, Korea");
          				else if(time_zone == "+10 1 0")
          					document.write("GMT+10:00 Guam, Russia");
          				else if(time_zone == "+10 2 4")
          					document.write("GMT+10:00 Australia");
          				else if(time_zone == "+11 1 0")
          					document.write("GMT+11:00 Solomon Islands");
          				else if(time_zone == "+12 1 0")
          					document.write("GMT+12:00 Fiji");
          				else if(time_zone == "+12 2 4")
          					document.write("GMT+12:00 New Zealand");  		
					</script>
         			</b>
         		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif heiht=25>&nbsp;</TD>
			</TR>

			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture(setnetsetup.tinterval)</script>:</TD>
          		<TD width=271 height=25><b><% nvram_get("timer_interval");%></b></TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>

        	<TR>
          		<TD width=87 bgColor=#000000 height=25>
          			<P align=center><b><font face="Arial" style="font-size: 9pt" color="#FFFFFF">LAN</font></b></P>
          		</TD>
          		<TD width=13 bgColor=#000000 height=25>&nbsp;</TD>
          		<TD width=4 height=25>&nbsp;</TD>
          		<TD width=5 height=25>&nbsp;</TD>
          		<TD width=12 height=25>&nbsp;</TD>
         		<TD width=195 height=25>&nbsp;</TD>
          		<TD width=271 height=25>&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>

        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture(share.macaddr)</script>:</TD>
          		<TD height=25 width="271"><B><% hwaddr_get("lan"); %></B></TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
	  
	   		<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture(share.ipaddr)</script>:</TD>
          		<TD height=25 width="271"><B><% nvram_get("lan_ipaddr"); %></B></TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>

	   		<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture(share.subnet)</script>:</TD>
         		<TD height=25 width="271"><B><% nvram_get("lan_netmask"); %></B></B></TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>

	   		<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture(share.dhcpserver)</script></TD>
          		<TD height=25 width="271"><B><% lan_dhcp_setting("dom"); %>d</B></TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>

	   		<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture()</script>Start IP Address:</TD>
          		<TD width=271 height=25><b><% prefix_ip_get("lan_ipaddr",1); %><% nvram_get("dhcp_start"); %>&nbsp;</b></TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>

	   		<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture()</script>End IP Address:</TD>
          		<TD height=25 width="271">
          			<b>
          			<script language=javascript>
						var prefix = "<% prefix_ip_get("lan_ipaddr",1); %>";
						var start = <% nvram_get("dhcp_start"); %>;
						var num = <% nvram_get("dhcp_num"); %>;
						document.write(prefix);
						document.write(start+num-1);
			        </script>
					&nbsp;
					</b>
				</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>

		    <TR>
	        	<TD width=87 bgColor=#000000 height=25>
            		<P align=center>
            		<b><font face="Arial" style="font-size: 9pt" color="#FFFFFF"><script>Capture()</script>Wireless</font></b>
            		</P>
            	</TD>
          		<TD width=13 bgColor=#000000 height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">&nbsp;</TD>
		        <TD height=25 width="271">&nbsp;<B></B></TD>
          		<TD width=47 height=25>&nbsp;</TD>
                <TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
            </TR>

	   		<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture()</script>Firmware Version:</TD>
          		<TD height=25 width="271"><B><% nvram_get("wl_firmware_version"); %></B></TD>
         	 	<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>

	   		<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture()</script>MAC Address:</TD>
          		<TD height=25 width="271"><B><% nvram_get("wl0_hwaddr"); %>&nbsp</B></TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>

        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture()</script>Mode:</TD>
          		<TD height=25 width="271"><b><% nvram_match("wl_gmode", "1", "Mixed"); %><% nvram_match("wl_gmode", "2", "G-Only"); %><% nvram_match("wl_gmode", "-1", "Disabled"); %><% nvram_match("wl_gmode", "0", "B-Only"); %>&nbsp;</b></TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">SSID:</TD>
          		<TD height=25 width="271"><b><% nvram_get("wl_ssid"); %>&nbsp;</b></TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
	
        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture()</script>Channel:</TD>
          		<TD height=25 width="271"><b><% nvram_get("wl_channel"); %>&nbsp;</b></TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
	
        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">SSID <script>Capture()</script>Broadcast:</TD>
          		<TD height=25 width="271">
          			<b>
          			<script language=javascript>
          				var state = "<% nvram_selget("wl_closed");%>";
          				if(state == 0)
          					document.write("Enabled");
          				else
          					document.write("Disabled");
          			</script>
          			</b>
          		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
	
        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture(share.dhcpserver)</script></TD>
          		<TD height=25 width="271"><B><% lan_dhcp_setting("dom"); %>d</B></TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
	
        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">Encryption Function</TD>
          		<TD height=25 width="271"><b><% nvram_match("security_mode", "disabled", "Disabled"); %><% nvram_invmatch("security_mode", "disabled", "Enabled"); %>&nbsp;</b></TD>
		  		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
	
        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture()</script>Security Mode:</TD>
          		<TD height=25 width="271"><b><% nvram_selget("security_mode");%></b></TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>     
	
	
        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture()</script>Access:</TD>
          		<TD height=25 width="271"> 
		  			<b>
		  			<script language=javascript>
		  				var state = "<% nvram_selget("wl_macmode");%>";
		  				if(state == "disabled")
		  					document.write("Allow All");
		  				else if(state == "deny")
		  					document.write("Restrict Access");
		  				else 
		  					document.write(state);
		  			</script>
		  			</b>
		  		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
	
       	 	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture()</script>Authentication Type:</TD>
          		<TD height=25 width="271">
          			<b>
         	 		<script language=javascript>
          				var state = "<% nvram_selget("wl_auth");%>";
          				if(state == 1)
          					document.write("Shared Key");
          				else
          					document.write("Auto");
          			</script>
          			</b>
          		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
	
        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture(wlnetwork.control)</script>:</TD>
		  		<TD width=271><b><% nvram_selget("wl_rate"); %></b></TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
	
        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture(wlnetwork.beacon)</script>:</TD>
          		<TD height=25 width="271"><b><% nvram_selget("wl_bcn"); %></b></TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
	
        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture(wlnetwork.dtim)</script>:</TD>
          		<TD height=25 width="271"><b><% nvram_selget("wl_dtim"); %></b></TD>
		        <TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
	
        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture(wlnetwork.frag)</script>:</TD>
          		<TD height=25 width="271"><b><% nvram_selget("wl_frag"); %></b></TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
	
        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture(wlnetwork.rts)</script>:</TD>
          		<TD height=25 width="271"><b><% nvram_selget("wl_rts"); %></b></TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
	
			<TR>
	        	<TD width=87 bgColor=#000000 height=25>
            		<p align="center"><b><font face="Arial" style="font-size: 9pt" color="#FFFFFF">DSL</font></b></p>
            	</TD>
          		<TD width=13 bgColor=#000000 height=25>&nbsp;</TD>
		        <TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">&nbsp;</TD>
          		<TD height=25 width="271">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
 
			<TR>
          		<TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25>&nbsp;</TD>
          		<TD height=25><script>Capture()</script>DSL Status:</TD>
          		<TD width=226 height=25>&nbsp;<FONT style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("dslstatus", 10); %></B></FONT></TD>
          		<TD width=13 height=25>&nbsp;</TD>
          		<TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
        
			<TR>
          		<TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25>&nbsp;</TD>
          		<TD height=25><script>Capture()</script>DSL Modulation Mode:</TD>
          		<TD width=226 height=25>&nbsp;<FONT style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("trainedmode", 10); %></B></FONT></TD>
          		<TD width=13 height=25>&nbsp;</TD>
          		<TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>

			<TR>
				<TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25>&nbsp;</TD>
          		<TD height=25><script>Capture()</script>DSL Path Mode:</TD>
          		<TD width=226 height=25>&nbsp;<FONT style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("trainedpath", 10); %></B></FONT></TD>
          		<TD width=13 height=25>&nbsp;</TD>
          		<TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>

        	<TR>
          		<TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25>&nbsp;</TD>
          		<TD><script>Capture()</script>Downstream Rate:</TD>
          		<TD><b>&nbsp;<% nvram_dslstatus_get("dsrate", 0); %>bps</b></TD>
          		<TD width=13 height=25>&nbsp;</TD>
          		<TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
          	
	        <TR>
		    	<TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25>&nbsp;</TD>
          		<TD height=25><script>Capture()</script>Upstream Rate:</TD>
          		<TD width=226 height=25><b>&nbsp;<% nvram_dslstatus_get("usrate", 0); %>bps</b></TD>
          		<TD width=13 height=25>&nbsp;</TD>
          		<TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
        
			<TR>
          		<TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25>&nbsp;</TD>
          		<TD height=25><script>Capture()</script>Downstream Margin:</TD>
          		<TD width=226 height=25><b>&nbsp;<% nvram_dslstatus_get("dsmargin", 0); %> db</b></TD>
          		<TD width=13 height=25>&nbsp;</TD>
          		<TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
          	
        	<TR>
          		<TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25>&nbsp;</TD>
          		<TD height=25><script>Capture()</script>Upstream Margin:</TD>
          		<TD width=226 height=25><b>&nbsp;<% nvram_dslstatus_get("usmargin", 0); %> db</b></TD>
          		<TD width=13 height=25>&nbsp;</TD>
          		<TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
          	
			<TR>
          		<TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25>&nbsp;</TD>
          		<TD height=25><script>Capture()</script>Downstream Line Attenuation:</TD>
          		<TD width=226 height=25>&nbsp;<FONT style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("dsatten", 0); %></B></FONT></TD>
          		<TD width=13 height=25>&nbsp;</TD>
          		<TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
          	
			<TR>
         		<TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25>&nbsp;</TD>
          		<TD height=25><script>Capture()</script>Upstream Line Attenuation:</TD>
          		<TD width=226 height=25>&nbsp;<FONT style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("usatten", 0); %></B></FONT></TD>
          		<TD width=13 height=25>&nbsp;</TD>
          		<TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
          	
			<TR>
          		<TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25>&nbsp;</TD>
          		<TD height=25><script>Capture()</script>Downstream Transmit Power:</TD>
          		<TD width=226 height=25>&nbsp;<FONT style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("dspower", 0); %></B></FONT></TD>
          		<TD width=13 height=25>&nbsp;</TD>
          		<TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
          	
			<TR>
	        	<TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
		        <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
		        <TD colSpan=3 height=25>&nbsp;</TD>
          		<TD height=25><script>Capture()</script>Upstream Transmit Power:</TD>
          		<TD width=226 height=25>&nbsp;<FONT style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("uspower", 0); %></B></FONT></TD>
		        <TD width=13 height=25>&nbsp;</TD>
        		<TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
        	</TR>

	        <TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">&nbsp;</TD>
          		<TD width=271 height=25>&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif heiht=25>&nbsp;</TD>
        	</TR>
        	
        	<TR>
	        	<TD width=87 bgColor=#000000 height=25>
              		<P align=center><b><font face="Arial" style="font-size: 9pt" color="#FFFFFF">PVC</font></b></P>
              	</TD>
          	  	<TD width=13 bgColor=#000000 height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">&nbsp;</TD>
          		<TD height=25 width="271">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
        	</TR>

        	<TR>
          		<TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25>&nbsp;</TD>
          		<TD height=25><script>Capture()</script>Connection:</TD>
	  			<TD height=25 width=226>&nbsp;<SELECT name="status_connection" onChange=SelCONNECTION(this.form.status_connection.selectedIndex,this.form)><% status_connection_table("select"); %></SELECT></TD>
          		<TD width=13 height=25>&nbsp;</TD>
          		<TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>

        	<TR>
          		<TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25>&nbsp;</TD>
          		<TD height=25><script>Capture(setinetsetup.encap)</script>:</TD>
          		<TD height=25>&nbsp;<FONT style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("encapmode", 10); %></B></FONT></TD>
          		<TD width=13 height=25>&nbsp;</TD>
          		<TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
          	
          	<TR>
          		<TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25>&nbsp;</TD>
          		<TD height=25><script>Capture(setinetsetup.multiplexing)</script>:</TD>
          		<TD height=25>&nbsp;<FONT style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("multiplex"); %></B></FONT></TD>
          		<TD width=13 height=25>&nbsp;</TD>
          		<TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>

	   		<TR>
          		<TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25>&nbsp;</TD>
          		<TD height=25><script>Capture(setinetsetup.qostype)</script>:</TD>
          		<TD height=25>&nbsp;<FONT style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("qos"); %></B></FONT></TD>
          		<TD width=13 height=25>&nbsp;</TD>
          		<TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>

	   		<TR>
          		<TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25>&nbsp;</TD>
          		<TD height=25><script>Capture(setinetsetup.pcrrate)</script>:</TD>
          		<TD height=25>&nbsp;<FONT style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("pcr"); %></B></FONT></TD>
          		<TD width=13 height=25>&nbsp;</TD>
          		<TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>

	   		<TR>
          		<TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25>&nbsp;</TD>
          		<TD height=25><script>Capture(setinetsetup.scrrate)</script>:</TD>
          		<TD height=25>&nbsp;<FONT style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("scr"); %></B></FONT></TD>
          		<TD width=13 height=25>&nbsp;</TD>
          		<TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>

	   		<TR>
          		<TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25>&nbsp;</TD>
          		<TD height=25><script>Capture(setinetsetup.autodetect)</script>:</TD>
          		<TD height=25>&nbsp;<FONT style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("autodetect", 10); %>d</B></FONT></TD>
          		<TD width=13 height=25>&nbsp;</TD>
          		<TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>

        	<TR>
          		<TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25>&nbsp;</TD>
          		<TD height=25><span >VPI:</span></TD>
          		<TD height=25><b>&nbsp;<% nvram_dslstatus_get("vpi"); %>&nbsp;</b></TD>
          		<TD width=13 height=25>&nbsp;</TD>
          		<TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>

        	<TR>
          		<TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25>&nbsp;</TD>
          		<TD height=25><span >VCI:</span></TD>
          		<TD height=25><b>&nbsp;<% nvram_dslstatus_get("vci"); %>&nbsp;</b></TD>
          		<TD width=13 height=25>&nbsp;</TD>
          		<TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
	
			<TR>
          		<TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25>&nbsp;</TD>
          		<TD height=25><script>Capture(share.enabled)</script>:</TD>
          		<TD height=25>&nbsp;<FONT style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("applyonboot", 10); %></B></FONT></TD>
          		<TD width=13 height=25>&nbsp;</TD>
          		<TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
        	<TR>
          		<TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25>&nbsp;</TD>
          		<TD height=25><script>Capture()</script>PVC Status:</TD>
          		<TD height=25>&nbsp;<FONT style="FONT-SIZE: 8pt"><B><% nvram_dslstatus_get("pvcstatus", 10); %><%nvram_dslstatus_get("pvcerrstr"); %></B></FONT></TD>
          		<TD width=13 height=25>&nbsp;</TD>
          		<TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
          
        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">&nbsp;</TD>
          		<TD width=271 height=25>&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif heiht=25>&nbsp;</TD>
        	</TR>
	
			<TR>
	        	<TD width=87 bgColor=#000000 height=25>
            		<P align=center><b><font face="Arial" style="font-size: 9pt" color="#FFFFFF"><script>Capture(bmenu.security)</script></font></b></P>
            	</TD>
          		<TD width=13 bgColor=#000000 height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">&nbsp;</TD>
          		<TD height=25 width="271">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25><p align="center"><script>Capture(sectopmenu.firewall)</script></p></TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 align="center" width="233">&nbsp;</TD>
          		<TD height=25 align="center" width="233">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture(secfirwall.fwprotection)</script>:</TD>
          		<TD height=25 width="271">
          			<b>
          			<script language=javascript>
          				var state="<% nvram_get("filter");%>"
          				if(state == "on")
          					document.write("Enabled");
          				else
          					document.write("Disabled");
          			</script>
          			</b>
          		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture()</script>Additional Filters:</TD>
          		<TD height=25 width="271">
          			<b>
          			<script language=javascript>
          				var proxy="<% nvram_get("block_proxy");%>";
          				var cookie="<% nvram_get("block_cookie");%>";
          				var java="<% nvram_get("block_java");%>";
          				var activex="<% nvram_get("block_activex");%>";
          				if(proxy == 1)
          					document.write("  Proxy");
          				if(cookie == 1 )
          					document.write("  Cookies");
          				if(java == 1)
          					document.write("  Applets");
          				if(activex == 1)
          					document.write("  ActiveX");     		
          			</script>
          			</b>
          		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
			
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture(secleftmenu.blowanreq)</script></TD>
          		<TD height=25 width="271">
          			<b>
	          		<script language=javascript>
			          	var state = "<% nvram_get("block_wan");%>"
          				if(state == 1)
			          		document.write("Enabled");
          				else
			          		document.write("Disabled");
          			</script>
          			</b>
          		</TD>
		        <TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
        		<TD width=87 bgColor=#e7e7e7 height=25><p align="center">VPN</p></TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="233">&nbsp;</TD>
          		<TD height=25 width="233">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><p align="left"><b><script>Capture(secleftmenu.vpnpass)</script></b></TD>
          		<TD height=25 width="271">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture(secvpnpass.ipsecpass)</script>:</TD>
          		<TD height=25 width="271">
          			<b>
          			<script language=javascript>
          				var state = "<% nvram_get("ipsec_pass"); %>"
          				if(state == 1)
          					document.write("Enabled");
          				else
          					document.write("Disabled");
          			</script>
          			</b>
          		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
         		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture(secvpnpass.pppoepass)</script>:</TD>
          		<TD height=25 width="271">
          			<b>
          			<script language=javascript>
          				var state = "<% nvram_get("pppoe_pass"); %>"
          				if(state == 1)
          					document.write("Enabled");
          				else
          					document.write("Disabled");
          			</script>
         	 		</b>          
          		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture(secvpnpass.pptppass)</script>:</TD>
          		<TD height=25 width="271">
          			<b>
          			<script language=javascript>
          				var state = "<% nvram_get("pptp_pass"); %>"
          				if(state == 1)
          					document.write("Enabled");
          				else
          					document.write("Disabled");
          			</script>
          			</b>
          		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture(secvpnpass.l2tppass)</script>:</TD>
          		<TD height=25 width="271">
          			<b>
          			<script language=javascript>
          				var state = "<% nvram_get("l2tp_pass"); %>"
          				if(state == 1)
          					document.write("Enabled");
          				else
          					document.write("Disabled");
          			</script>
          			</b>          
          		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">&nbsp;</TD>
          		<TD height=25 width="271">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 align="center" width="195"><p align="left"><b><script>Capture(secipsectunnel.ipsectunnel)</script></b></TD>
          		<TD height=25 align="center" width="271">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture()</script>Tunnel Entry:</TD>
          		<TD height=25 width="271">
          			<b>
          			<script language=javascript>
          				var entry = "<% nvram_get("tunnel_entry"); %>";
          				document.write((entry-0) + 1);
          			</script>
          			</b>         
          		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture(secipsectunnel.ipsectunnel)</script>:</TD>
          		<TD height=25 width="271">
          			<b>
          			<script language=javascript>
          				var entry = "<% nvram_get("tunnel_entry");%>";
          				var str = "<% nvram_get("entry_config");%>";
          				var name = str.split(' ');
          				var tunnel_enable = name[entry].split(':')[0];
          				if(tunnel_enable == 1)
          					document.write("Enabled");
          				else
          					document.write("Disabled");
          			</script>
          			</b>
          		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture(secipsectunnel.tunnelname)</script>:</TD>
          		<TD height=25 width="271">
          			<b>
          			<!-- below is great JavaScript code -->
          			<script language=javascript>
          				var entry = "<% nvram_get("tunnel_entry");%>";
          				var str = "<% nvram_get("entry_config"); %>";
          				var name = str.split(' ');
          				var display = name[entry].split(':')[1];
          				document.write(display);
          			</script>
          			</b>         
          		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">&nbsp;</TD>
          		<TD height=25 width="271">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><p align="left"><b><script>Capture(secleftmenu.localsecgrp)</script></b></p></TD>
          		<TD height=25 width="271">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
                    
		  	<TR>
  	      		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"></TD>
   		  		<TD height=25 width="271"> 
   		    		<SELECT onchange=SelLTYPE(this.form.local_type.selectedIndex,this.form) name=local_type >
            			<OPTION value=0  <% entry_config("local_type", 0); %>><script>Capture(share,ipaddr)</script></OPTION>
            			<OPTION value=1  <% entry_config("local_type", 1); %>><script>Capture(share.subnet)</script></OPTION>
          			</SELECT>
          		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>         
          
  			<TR>
  	      		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture(share.ipaddr)</script>:</TD>
   		  		<TD height=25 width="271">
   		  			<b>
          			<script language=javascript>
  						var str = "<% nvram_get("ltype_config");%>";
  						var entry = "<% nvram_get("tunnel_entry");%>";
  						var array = str.split(' ');
  						var option = array[entry].split(':');
  						if(option.length == 2)
	  						document.write(option[0]);
           			</script>
           			</b>    		    		    		  
   		  		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
          
  			<TR>
  	      		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">Subnet Address:</TD>
   		  		<TD height=25 width="271">
   		  			<b>
          			<script language=javascript>
  						var str = "<% nvram_get("ltype_config");%>";
  						var entry = "<% nvram_get("tunnel_entry");%>";
  						var array = str.split(' ');
  						var option = array[entry].split(':');
  						if(option.length == 2)
	  						document.write(option[1]);
	  					else
	  						document.write("None");
           			</script>
           			</b>		    		    		     		  
   		  		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
          	
  			<TR>
  	      		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">&nbsp;</TD>
   		  		<TD height=25 width="271">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
          
  			<TR>
  	      		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><p align="left"><b>Local Security Gateway:</b></TD>
   		  		<TD height=25 width="271">
		  			<SELECT onChange=SelLgw(this.form.localgw_conn.selectedIndex,this.form) name=localgw_conn>
		  				<% localgw_select("select"); %>
          			</SELECT>  		  
   		  		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
            
  			<TR>
  	      		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">&nbsp;</TD>
   		  		<TD height=25 width="271">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
          	  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><p align="left"><b>Remote Secure Group</b></TD>
          		<TD height=25 width="271">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">&nbsp;</TD>
          		<TD height=25 width="271">
          			<SELECT onchange=SelRTYPE(this.form.remote_type.selectedIndex,this.form) name=remote_type>
            			<OPTION value=0 <% entry_config("remote_type", 0); %>>IP Addr.</OPTION>
            			<OPTION value=1 <% entry_config("remote_type", 1); %>>Subnet</OPTION>
			            <OPTION value=4 <% entry_config("remote_type", 4); %>>Host</OPTION>
						<OPTION value=3 <% entry_config("remote_type", 3); %>>Any</OPTION>
					</SELECT>          
          		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR> 
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">IP Address:</TD>
          		<TD height=25 width="271">&nbsp;
          			<b>
          			<script language=javascript>
  						var str = "<% nvram_get("rtype_config");%>";
  						var entry = "<% nvram_get("tunnel_entry");%>";
  						var array = str.split(' ');
  						var option = array[entry].split(':');
  						if(option.length == 2)
	  						document.write(option[0]);
	  					else
	  						document.write("None");
           			</script>
           			</b>    		           
          		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><script>Capture(share.subnet)</script>:</TD>
          		<TD height=25 width="271">&nbsp;
          			<b>
          			<script language=javascript>
  						var str = "<% nvram_get("ltype_config");%>";
  						var entry = "<% nvram_get("tunnel_entry");%>";
  						var array = str.split(' ');
  						var option = array[entry].split(':');
  						if(option.length == 2)
	  						document.write(option[1]);
	  					else
	  						document.write("None");
           			</script>
           			</b> 		           
          		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">&nbsp;</TD>
          		<TD height=25 width="271">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><b><script>Capture(secleftmenu.remotesecgw)</script>:</b></TD>
          		<TD height=25 width="271">
          			<SELECT onchange=SelSGTYPE(this.form.sg_type.selectedIndex,this.form)  name=sg_type>
            			<OPTION value=0 <% entry_config("sg_type", 0); %>><script>Capture(share.ipaddr)</script></OPTION>
            			<OPTION value=2 <% entry_config("sg_type", 2); %>>FQDN</OPTION>
            			<OPTION value=1 <% entry_config("sg_type", 1); %>><script>Capture(share.any)</script></OPTION>
          			</SELECT>   
          		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">&nbsp;</TD>
          		<TD height=25 width="271">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">Encryption:</TD>
          		<TD height=25 width="271">
          			<b>
          			<script language=javascript>
          				var str = "<% nvram_get("entry_config");%>";
          				var entry = "<% nvram_get("tunnel_entry");%>";
          				var array = str.split(' ');
          				var temp = array[entry].split(':');
          				var valu = temp[6];
          				if(valu == 0)
          					document.write("Disabled");
          				else if(valu == 1)
          					document.write("DES");
          				else if(valu == 2)
          					document.write("3DES");
          				else if(valu == 3)
          					document.write("AES");
          			</script>        
          			<b>  
          		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
         		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
         	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">Authentication:</TD>
          		<TD height=25 width="271">
          			<b>
          			<script language=javascript>
          				var str = "<% nvram_get("entry_config");%>";
          				var entry = "<% nvram_get("tunnel_entry");%>";
          				var array = str.split(' ');
          				var temp = array[entry].split(':');
          				var valu = temp[7];
          				if(valu == 1)
          					document.write("MD5");
          				else if(valu == 2)
          					document.write("SHA");
          			</script>          
          			</b>
          		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">&nbsp;</TD>
          		<TD height=25 width="271">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 align="center" width="195"><p align="left"><b><script>Capture(secleftmenu.keymgnt)</script></b></TD>
          		<TD height=25 align="center" width="271">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"></TD>
          		<TD height=25 width="271"><p align="left">
          			<SELECT onchange=SelKEYTYPE(this.form.key_type.selectedIndex,this.form) name=key_type>
          				<OPTION value=0 <% entry_config("key_type", 0); %>>Auto.(IKE)</OPTION>
          				<OPTION value=1 <% entry_config("key_type", 1); %>>Manual</OPTION>
          			</SELECT>
          		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  			<% show_keytype_setting(); %>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 align="center" width="195">&nbsp;</TD>
          		<TD height=25 align="center" width="271">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><b><script>Capture(bmenu.statu)</script></b></TD>
          		<TD height=25 width="271"><B><% entry_config("tstatus",0); %></B></TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
	        	<TD width=87 bgColor=#000000 height=25>
	        		<P align=center>
	        		<b><font face="Arial" style="font-size: 9pt" color="#FFFFFF"><script>Capture(bmenu.accrestriction)</script></font></b>
	        		</P>
	        	</TD>
          		<TD width=13 bgColor=#000000 height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">&nbsp;</TD>
          		<TD height=25 width="271">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">Account Sign-Up:</TD>
          		<TD height=25 width="271">
          			<B>
          			<script language=javascript>
          				var state="<% nvram_get("artemis_enable"); %>"
          				if(state == 1)
          					document.write("Enabled");
          				else
          					document.write("Disabled");
          			</script>
          			<B>
          		</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">&nbsp;</TD>
          		<TD height=25 width="271">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
		        <TD width=87 bgColor=#000000 height=25>
            		<P align=center>
            			<b><font face="Arial" style="font-size: 9pt" color="#FFFFFF"><script>Capture(bmenu.application)</script> &amp; <script>Capture(bmenu.gaming)</script></font></b>
            		</P>
            	</TD>
          		<TD width=13 bgColor=#000000 height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">&nbsp;</TD>
          		<TD height=25 width="271">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 colspan="2" width="466"><p align="center"><b><script>Capture(apptopmenu.singleport)</script></b></TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 colspan="2" width="466">
            		<CENTER>
					<TABLE style='BORDER-COLLAPSE: collapse' borderColor=silver height=24 cellSpacing=0 cellPadding=0 width=421 border=1>
						<TR>
							<TD class=headrow align=middle height=30 width=78><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 8pt'><script>Capture(bmenu.application)</script></font></B></TD>
							<TD class=headrow align=middle height=30 width=47><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 8pt'><script>Capture(appsingleport.extport)</script></font></B></TD>
							<TD class=headrow align=middle height=30 width=63><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 8pt'><script>Capture(appsingleport.intport)</script></font></B></TD>
							<TD class=headrow align=middle height=30 width=79><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 8pt'><script>Capture(appsingleport.proto)</script></font></B></TD>
							<TD class=headrow align=middle height=30 width=103><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 8pt'><script>Capture(appsingleport.ipaddr)</script></font></B></TD>
							<TD class=headrow align=middle height=30 width=13><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 8pt'><script>Capture(share.enabled)</script></font></B></TD>
						</TR>
												
						<TR align=center  height=30>
							<TD><input class=num name=name0 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","0"); %>'></TD>
							<TD><input class=num name=ext_port0 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","0"); %>'></TD>
							<TD><input class=num name=int_port0 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","0"); %>'></TD>
							<TD><FONT face=Arial color=blue>
								<SELECT size=1 name=protocol0>
									<OPTION value=tcp <% forward_upnp("sel_tcp","0"); %>>TCP</OPTION>
									<OPTION value=udp <% forward_upnp("sel_udp","0"); %>>UDP</OPTION>
								</SELECT>
								</FONT>
							</TD>
							<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip0 size=3 maxlength=3 value='<% forward_upnp("ip","0"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
							<TD><input type=checkbox name=enabled0 value=1 <% forward_upnp("enabled","0"); %>></TD>
						</TR>
												
						<TR align=center  height=30>
							<TD><input class=num name=name1 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","1"); %>'></TD>
							<TD><input class=num name=ext_port1 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","1"); %>'></TD>
							<TD><input class=num name=int_port1 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","1"); %>'></TD>
							<TD><FONT face=Arial color=blue>
								<SELECT size=1 name=protocol1>
									<OPTION value=tcp <% forward_upnp("sel_tcp","1"); %>>TCP</OPTION>
									<OPTION value=udp <% forward_upnp("sel_udp","1"); %>>UDP</OPTION>
								</SELECT>
								</FONT>
							</TD>
							<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip1 size=3 maxlength=3 value='<% forward_upnp("ip","1"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
							<TD><input type=checkbox name=enabled1 value=1 <% forward_upnp("enabled","1"); %>></TD>
						</TR>
												
						<TR align=center  height=30>
							<TD><input class=num name=name2 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","2"); %>'></TD>
							<TD><input class=num name=ext_port2 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","2"); %>'></TD>
							<TD><input class=num name=int_port2 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","2"); %>'></TD>
							<TD><FONT face=Arial color=blue>
								<SELECT size=1 name=protocol2>
									<OPTION value=tcp <% forward_upnp("sel_tcp","2"); %>>TCP</OPTION>
									<OPTION value=udp <% forward_upnp("sel_udp","2"); %>>UDP</OPTION>
								</SELECT>
								</FONT>
							</TD>
							<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip2 size=3 maxlength=3 value='<% forward_upnp("ip","2"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
							<TD><input type=checkbox name=enabled2 value=1 <% forward_upnp("enabled","2"); %>></TD>
						</TR>
												
						<TR align=center  height=30>
							<TD><input class=num name=name3 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","3"); %>'></TD>
							<TD><input class=num name=ext_port3 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","3"); %>'></TD>
							<TD><input class=num name=int_port3 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","3"); %>'></TD>
							<TD><FONT face=Arial color=blue>
								<SELECT size=1 name=protocol3>
									<OPTION value=tcp <% forward_upnp("sel_tcp","3"); %>>TCP</OPTION>
									<OPTION value=udp <% forward_upnp("sel_udp","3"); %>>UDP</OPTION>
								</SELECT>
								</FONT>
							</TD>
							<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip3 size=3 maxlength=3 value='<% forward_upnp("ip","3"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
							<TD><input type=checkbox name=enabled3 value=1 <% forward_upnp("enabled","3"); %>></TD>
						</TR>

						<TR align=center  height=30>
							<TD><input class=num name=name4 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","4"); %>'></TD>
							<TD><input class=num name=ext_port4 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","4"); %>'></TD>
							<TD><input class=num name=int_port4 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","4"); %>'></TD>
							<TD><FONT face=Arial color=blue>
								<SELECT size=1 name=protocol4>
									<OPTION value=tcp <% forward_upnp("sel_tcp","4"); %>>TCP</OPTION>
									<OPTION value=udp <% forward_upnp("sel_udp","4"); %>>UDP</OPTION>
								</SELECT>
								</FONT>
							</TD>
							<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip4 size=3 maxlength=3 value='<% forward_upnp("ip","4"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
							<TD><input type=checkbox name=enabled4 value=1 <% forward_upnp("enabled","4"); %>></TD>
						</TR>

						<TR align=center  height=30>
							<TD><input class=num name=name5 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","5"); %>'></TD>
							<TD><input class=num name=ext_port5 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","5"); %>'></TD>
							<TD><input class=num name=int_port5 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","5"); %>'></TD>
							<TD><FONT face=Arial color=blue>
								<SELECT size=1 name=protocol5>
									<OPTION value=tcp <% forward_upnp("sel_tcp","5"); %>>TCP</OPTION>
									<OPTION value=udp <% forward_upnp("sel_udp","5"); %>>UDP</OPTION>
								</SELECT>
								</FONT>
							</TD>
							<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip5 size=3 maxlength=3 value='<% forward_upnp("ip","5"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
							<TD><input type=checkbox name=enabled5 value=1 <% forward_upnp("enabled","5"); %>></TD>
						</TR>

						<TR align=center  height=30>
							<TD><input class=num name=name6 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","6"); %>'></TD>
							<TD><input class=num name=ext_port6 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","6"); %>'></TD>
							<TD><input class=num name=int_port6 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","6"); %>'></TD>
							<TD><FONT face=Arial color=blue>
								<SELECT size=1 name=protocol6>
									<OPTION value=tcp <% forward_upnp("sel_tcp","6"); %>>TCP</OPTION>
									<OPTION value=udp <% forward_upnp("sel_udp","6"); %>>UDP</OPTION>
								</SELECT>
								</FONT>
							</TD>
							<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip6 size=3 maxlength=3 value='<% forward_upnp("ip","6"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
							<TD><input type=checkbox name=enabled6 value=1 <% forward_upnp("enabled","6"); %>></TD>
						</TR>

						<TR align=center  height=30>
							<TD><input class=num name=name7 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","7"); %>'></TD>
							<TD><input class=num name=ext_port7 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","7"); %>'></TD>
							<TD><input class=num name=int_port7 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","7"); %>'></TD>
							<TD><FONT face=Arial color=blue>
								<SELECT size=1 name=protocol7>
									<OPTION value=tcp <% forward_upnp("sel_tcp","7"); %>>TCP</OPTION>
									<OPTION value=udp <% forward_upnp("sel_udp","7"); %>>UDP</OPTION>
								</SELECT>
								</FONT>
							</TD>
							<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip7 size=3 maxlength=3 value='<% forward_upnp("ip","7"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
							<TD><input type=checkbox name=enabled7 value=1 <% forward_upnp("enabled","7"); %>></TD>
						</TR>

						<TR align=center  height=30>
							<TD><input class=num name=name8 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","8"); %>'></TD>
							<TD><input class=num name=ext_port8 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","8"); %>'></TD>
							<TD><input class=num name=int_port8 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","8"); %>'></TD>
							<TD><FONT face=Arial color=blue>
								<SELECT size=1 name=protocol8>
									<OPTION value=tcp <% forward_upnp("sel_tcp","8"); %>>TCP</OPTION>
									<OPTION value=udp <% forward_upnp("sel_udp","8"); %>>UDP</OPTION>
								</SELECT>
								</FONT>
							</TD>
							<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip8 size=3 maxlength=3 value='<% forward_upnp("ip","8"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
							<TD><input type=checkbox name=enabled8 value=1 <% forward_upnp("enabled","8"); %>></TD>
						</TR>

						<TR align=center  height=30>
							<TD><input class=num name=name9 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","9"); %>'></TD>
							<TD><input class=num name=ext_port9 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","9"); %>'></TD>
							<TD><input class=num name=int_port9 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","9"); %>'></TD>
							<TD><FONT face=Arial color=blue>
								<SELECT size=1 name=protocol9>
									<OPTION value=tcp <% forward_upnp("sel_tcp","9"); %>>TCP</OPTION>
									<OPTION value=udp <% forward_upnp("sel_udp","9"); %>>UDP</OPTION>
								</SELECT>
								</FONT>
							</TD>
							<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip9 size=3 maxlength=3 value='<% forward_upnp("ip","9"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
							<TD><input type=checkbox name=enabled9 value=1 <% forward_upnp("enabled","9"); %>></TD>
						</TR>

						<TR align=center  height=30>
							<TD><input class=num name=name10 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","10"); %>'></TD>
							<TD><input class=num name=ext_port10 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","10"); %>'></TD>
							<TD><input class=num name=int_port10 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","10"); %>'></TD>
							<TD><FONT face=Arial color=blue>
								<SELECT size=1 name=protocol10>
									<OPTION value=tcp <% forward_upnp("sel_tcp","10"); %>>TCP</OPTION>
									<OPTION value=udp <% forward_upnp("sel_udp","10"); %>>UDP</OPTION>
								</SELECT>
								</FONT>
							</TD>
							<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip10 size=3 maxlength=3 value='<% forward_upnp("ip","10"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
							<TD><input type=checkbox name=enabled10 value=1 <% forward_upnp("enabled","10"); %>></TD>
						</TR>

						<TR align=center  height=30>
							<TD><input class=num name=name11 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","11"); %>'></TD>
							<TD><input class=num name=ext_port11 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","11"); %>'></TD>
							<TD><input class=num name=int_port11 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","11"); %>'></TD>
							<TD><FONT face=Arial color=blue>
								<SELECT size=1 name=protocol11>
									<OPTION value=tcp <% forward_upnp("sel_tcp","11"); %>>TCP</OPTION>
									<OPTION value=udp <% forward_upnp("sel_udp","11"); %>>UDP</OPTION>
								</SELECT>
								</FONT>
							</TD>
							<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip11 size=3 maxlength=3 value='<% forward_upnp("ip","11"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
							<TD><input type=checkbox name=enabled11 value=1 <% forward_upnp("enabled","11"); %>></TD>
						</TR>

						<TR align=center  height=30>
							<TD><input class=num name=name12 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","12"); %>'></TD>
							<TD><input class=num name=ext_port12 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","12"); %>'></TD>
							<TD><input class=num name=int_port12 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","12"); %>'></TD>
							<TD><FONT face=Arial color=blue>
								<SELECT size=1 name=protocol12>
									<OPTION value=tcp <% forward_upnp("sel_tcp","12"); %>>TCP</OPTION>
									<OPTION value=udp <% forward_upnp("sel_udp","12"); %>>UDP</OPTION>
								</SELECT>
								</FONT>
							</TD>
							<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip12 size=3 maxlength=3 value='<% forward_upnp("ip","12"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
							<TD><input type=checkbox name=enabled12 value=1 <% forward_upnp("enabled","12"); %>></TD>
						</TR>

						<TR align=center  height=30>
							<TD><input class=num name=name13 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","13"); %>'></TD>
							<TD><input class=num name=ext_port13 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","13"); %>'></TD>
							<TD><input class=num name=int_port13 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","13"); %>'></TD>
							<TD><FONT face=Arial color=blue>
								<SELECT size=1 name=protocol13>
									<OPTION value=tcp <% forward_upnp("sel_tcp","13"); %>>TCP</OPTION>
									<OPTION value=udp <% forward_upnp("sel_udp","13"); %>>UDP</OPTION>
								</SELECT>
								</FONT>
							</TD>
							<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip13 size=3 maxlength=3 value='<% forward_upnp("ip","13"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
							<TD><input type=checkbox name=enabled13 value=1 <% forward_upnp("enabled","13"); %>></TD>
						</TR>

						<TR align=center  height=30>
							<TD><input class=num name=name14 onBlur=valid_name(this,"Name") size=7 maxlength=128 value='<% forward_upnp("name","14"); %>'></TD>
							<TD><input class=num name=ext_port14 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("ext_port","14"); %>'></TD>
							<TD><input class=num name=int_port14 onBlur=valid_range(this,0,65535,"Port") size=5 maxlength=5 value='<% forward_upnp("int_port","14"); %>'></TD>
							<TD><FONT face=Arial color=blue>
								<SELECT size=1 name=protocol14>
									<OPTION value=tcp <% forward_upnp("sel_tcp","14"); %>>TCP</OPTION>
									<OPTION value=udp <% forward_upnp("sel_udp","14"); %>>UDP</OPTION>
								</SELECT>
								</FONT>
							</TD>
							<TD><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><input class=num name=ip14 size=3 maxlength=3 value='<% forward_upnp("ip","14"); %>' onBlur=valid_range(this,0,254,'IP')></FONT></TD>
							<TD><input type=checkbox name=enabled14 value=1 <% forward_upnp("enabled","14"); %>></TD>
						</TR>
												
			  		</TABLE>
              		</CENTER>          
                </TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 colspan="2" width="466">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 colspan="2" width="466"><p align="center"><b><script>Capture(apptopmenu.portrange)</script></b></TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 colspan="2" width="466">
            		<CENTER>
		            <TABLE style="BORDER-COLLAPSE: collapse" borderColor=#e7e7e7 height=24 cellSpacing=0 cellPadding=0 width=421 border=0>
		            <TBODY>
              			<TR>
							<TD width=66 class=headrow height=30>&nbsp; </TD><TD class=headrow colSpan=2 height=30 align=center>
								<B style="font-weight: bold"><script>Capture(appportrange.portrange)</script> </B>
							</TD>
							<TD width=217 colspan=3 class=headrow height=30>&nbsp; </TD>
						</TR>

              			<TR>
			                <TD style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
				                align=middle width=66 bgColor=#CCCCCC height=30>
				                <B><script>Capture(appportrange.app)</script></B>
				            </TD>
                			<TD style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
                				align=middle width=82 bgColor=#CCCCCC height=30>
                				<B><script>Capture(appportrange.start)</script></B>
                			</TD>
                			<TD style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
                				align=middle width=56 bgColor=#CCCCCC height=30>
                				<B><script>Capture(appportrange.end)</script></B>
                			</TD>
                			<TD style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
                				align=middle width=69 bgColor=#CCCCCC height=30>
                				<B><script>Capture(appportrange.proto)</script></B>
                			</TD>
                			<TD style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
                				align=middle width=85 bgColor=#CCCCCC height=30>
                				<B><script>Capture(appportrange.ipaddr)</script></B>
                			</TD>
                			<TD style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
                				align=middle width=47 bgColor=#CCCCCC height=30>
                				<B><script>Capture(share.enabled)</script></B>
                			</TD>
                		</TR>

			            <TR align=middle>
                			<TD width=66 height=30><FONT size=2><INPUT  maxLength=12 size=7 name=name0 onBlur=valid_name(this,"Name") value='<% port_forward_table("name","0"); %>' class=num></FONT></TD>
			                <TD width=82 height=30><FONT face="Arial, Helvetica, sans-serif"><INPUT  maxLength=5 size=5 value='<% port_forward_table("from","0"); %>' name=from0 onBlur=valid_range(this,0,65535,"Port") class=num><span >&nbsp; to</span></FONT></TD>
			                <TD width=57 height=30><INPUT  maxLength=5 size=5 value='<% port_forward_table("to","0"); %>' name=to0 onBlur=valid_range(this,0,65535,"Port") class=num></TD>
			                <TD align=middle width=70 height=30>
			                	<FONT face=Arial color=blue>
								<SELECT size=1 name=pro0>
									<OPTION value=tcp <% port_forward_table("sel_tcp","0"); %>>TCP</OPTION>
									<OPTION value=udp <% port_forward_table("sel_udp","0"); %>>UDP</OPTION>
									<OPTION value=both <% port_forward_table("sel_both","0"); %>><script>Capture(share.both)</script></OPTION>
								</SELECT>
								</FONT>
							</TD>
			                <TD width=105 height=30><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><INPUT  maxLength=3 size=3 value='<% port_forward_table("ip","0"); %>' name=ip0 onBlur=valid_range(this,0,254,"IP") class=num></FONT></TD>
			                <TD width=47 height=30><INPUT type=checkbox value=on name=enable0 <% port_forward_table("enable","0"); %>></TD>
						</TR>

              			<TR align=middle>
                			<TD width=66 height=30><FONT size=2><INPUT  maxLength=12 size=7 name=name1 onBlur=valid_name(this,"Name") value='<% port_forward_table("name","1"); %>' class=num></FONT></TD>
			                <TD width=82 height=30><FONT face="Arial, Helvetica, sans-serif"><INPUT  maxLength=5 size=5 value='<% port_forward_table("from","1"); %>' name=from1 onBlur=valid_range(this,0,65535,"Port") class=num><span >&nbsp; to</span></FONT></TD>
			                <TD width=57 height=30><INPUT  maxLength=5 size=5 value='<% port_forward_table("to","1"); %>' name=to1 onBlur=valid_range(this,0,65535,"Port") class=num></TD>
			                <TD align=middle width=70 height=30><FONT face=Arial color=blue>
								<SELECT size=1 name=pro1>
									<OPTION value=tcp <% port_forward_table("sel_tcp","1"); %>>TCP</OPTION>
									<OPTION value=udp <% port_forward_table("sel_udp","1"); %>>UDP</OPTION>
									<OPTION value=both <% port_forward_table("sel_both","1"); %>><script>Capture(share.both)</script></OPTION>
								</SELECT></FONT>
							</TD>
                			<TD width=105 height=30><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><INPUT  maxLength=3 size=3 value='<% port_forward_table("ip","1"); %>' name=ip1 onBlur=valid_range(this,0,254,"IP") class=num></FONT></TD>
			                <TD width=47 height=30><INPUT type=checkbox value=on name=enable1 <% port_forward_table("enable","1"); %>></TD>
						</TR>
		
              			<TR align=middle>
			                <TD width=66 height=30><FONT size=2><INPUT  maxLength=12 size=7 name=name2 onBlur=valid_name(this,"Name") value='<% port_forward_table("name","2"); %>' class=num></FONT></TD>
			                <TD width=82 height=30><FONT face="Arial, Helvetica, sans-serif"><INPUT  maxLength=5 size=5 value='<% port_forward_table("from","2"); %>' name=from2 onBlur=valid_range(this,0,65535,"Port") class=num><span >&nbsp; to</span></FONT></TD>
			                <TD width=57 height=30><INPUT  maxLength=5 size=5 value='<% port_forward_table("to","2"); %>' name=to2 onBlur=valid_range(this,0,65535,"Port") class=num></TD>
			                <TD align=middle width=70 height=30><FONT face=Arial color=blue>
								<SELECT size=1 name=pro2>
									<OPTION value=tcp <% port_forward_table("sel_tcp","2"); %>>TCP</OPTION>
									<OPTION value=udp <% port_forward_table("sel_udp","2"); %>>UDP</OPTION>
									<OPTION value=both <% port_forward_table("sel_both","2"); %>><script>Capture(share.both)</script></OPTION>
								</SELECT></FONT></TD>
			                <TD width=105 height=30><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><INPUT  maxLength=3 size=3 value='<% port_forward_table("ip","2"); %>' name=ip2 onBlur=valid_range(this,0,254,"IP") class=num></FONT></TD>
			                <TD width=47 height=30><INPUT type=checkbox value=on name=enable2 <% port_forward_table("enable","2"); %>></TD>
			            </TR>
				        
						<TR align=middle>
			                <TD width=66 height=30><FONT size=2><INPUT  maxLength=12 size=7 name=name3 onBlur=valid_name(this,"Name") value='<% port_forward_table("name","3"); %>' class=num></FONT></TD>
			                <TD width=82 height=30><FONT face="Arial, Helvetica, sans-serif"><INPUT  maxLength=5 size=5 value='<% port_forward_table("from","3"); %>' name=from3 onBlur=valid_range(this,0,65535,"Port") class=num><span >&nbsp; to</span></FONT></TD>
			                <TD width=57 height=30><INPUT  maxLength=5 size=5 value='<% port_forward_table("to","3"); %>' name=to3 onBlur=valid_range(this,0,65535,"Port") class=num></TD>
			                <TD align=middle width=70 height=30>
			                	<FONT face=Arial color=blue>
								<SELECT size=1 name=pro3>
									<OPTION value=tcp <% port_forward_table("sel_tcp","3"); %>>TCP</OPTION>
									<OPTION value=udp <% port_forward_table("sel_udp","3"); %>>UDP</OPTION>
									<OPTION value=both <% port_forward_table("sel_both","3"); %>><script>Capture(share.both)</script></OPTION>
								</SELECT>
								</FONT>
							</TD>
			                <TD width=105 height=30><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><INPUT  maxLength=3 size=3 value='<% port_forward_table("ip","3"); %>' name=ip3 onBlur=valid_range(this,0,254,"IP") class=num></FONT></TD>
			                <TD width=47 height=30><INPUT type=checkbox value=on name=enable3 <% port_forward_table("enable","3"); %>></TD>
			            </TR>
			            
		              	<TR align=middle>
                			<TD width=66 height=30><FONT size=2><INPUT  maxLength=12 size=7 name=name4 onBlur=valid_name(this,"Name") value='<% port_forward_table("name","4"); %>' class=num></FONT></TD>
			                <TD width=82 height=30><FONT face="Arial, Helvetica, sans-serif"><INPUT  maxLength=5 size=5 value='<% port_forward_table("from","4"); %>' name=from4 onBlur=valid_range(this,0,65535,"Port") class=num><span >&nbsp; to</span></FONT></TD>
			                <TD width=57 height=30><INPUT  maxLength=5 size=5 value='<% port_forward_table("to","4"); %>' name=to4 onBlur=valid_range(this,0,65535,"Port") class=num></TD>
			                <TD align=middle width=70 height=30>
			                	<FONT face=Arial color=blue>
								<SELECT size=1 name=pro4>
									<OPTION value=tcp <% port_forward_table("sel_tcp","4"); %>>TCP</OPTION>
									<OPTION value=udp <% port_forward_table("sel_udp","4"); %>>UDP</OPTION>
									<OPTION value=both <% port_forward_table("sel_both","4"); %>><script>Capture(share.both)</script></OPTION>
								</SELECT>
								</FONT>
							</TD>
			                <TD width=105 height=30><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><INPUT  maxLength=3 size=3 value='<% port_forward_table("ip","4"); %>' name=ip4 onBlur=valid_range(this,0,254,"IP") class=num></FONT></TD>
			                <TD width=47 height=30><INPUT type=checkbox value=on name=enable4 <% port_forward_table("enable","4"); %>></TD>
			            </TR>
			            
			            <TR align=middle>
			                <TD width=66 height=30><FONT size=2><INPUT  maxLength=12 size=7 name=name5 onBlur=valid_name(this,"Name") value='<% port_forward_table("name","5"); %>' class=num></FONT></TD>
			                <TD width=82 height=30><FONT face="Arial, Helvetica, sans-serif"><INPUT  maxLength=5 size=5 value='<% port_forward_table("from","5"); %>' name=from5 onBlur=valid_range(this,0,65535,"Port") class=num><span >&nbsp; to</span></FONT></TD>
				            <TD width=57 height=30><INPUT  maxLength=5 size=5 value='<% port_forward_table("to","5"); %>' name=to5 onBlur=valid_range(this,0,65535,"Port") class=num></TD>
			                <TD align=middle width=70 height=30>
			                	<FONT face=Arial color=blue>
								<SELECT size=1 name=pro5>
									<OPTION value=tcp <% port_forward_table("sel_tcp","5"); %>>TCP</OPTION>
									<OPTION value=udp <% port_forward_table("sel_udp","5"); %>>UDP</OPTION>
									<OPTION value=both <% port_forward_table("sel_both","5"); %>><script>Capture(share.both)</script></OPTION>
								</SELECT>
								</FONT>
							</TD>
			                <TD width=105 height=30><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><INPUT  maxLength=3 size=3 value='<% port_forward_table("ip","5"); %>' name=ip5 onBlur=valid_range(this,0,254,"IP") class=num></FONT></TD>
			                <TD width=47 height=30><INPUT type=checkbox value=on name=enable5 <% port_forward_table("enable","5"); %>></TD>
			            </TR>
			            
						<TR align=middle>
			                <TD width=66 height=30><FONT size=2><INPUT  maxLength=12 size=7 name=name6 onBlur=valid_name(this,"Name") value='<% port_forward_table("name","6"); %>' class=num></FONT></TD>
			                <TD width=82 height=30><FONT face="Arial, Helvetica, sans-serif"><INPUT  maxLength=5 size=5 value='<% port_forward_table("from","6"); %>' name=from6 onBlur=valid_range(this,0,65535,"Port") class=num><span >&nbsp; to</span></FONT></TD>
			                <TD width=57 height=30><INPUT  maxLength=5 size=5 value='<% port_forward_table("to","6"); %>' name=to6 onBlur=valid_range(this,0,65535,"Port") class=num></TD>
			                <TD align=middle width=70 height=30>
			                	<FONT face=Arial color=blue>
								<SELECT size=1 name=pro6>
									<OPTION value=tcp <% port_forward_table("sel_tcp","6"); %>>TCP</OPTION>
									<OPTION value=udp <% port_forward_table("sel_udp","6"); %>>UDP</OPTION>
									<OPTION value=both <% port_forward_table("sel_both","6"); %>><script>Capture(share.both)</script></OPTION>
								</SELECT>
								</FONT>
							</TD>
			                <TD width=105 height=30><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><INPUT  maxLength=3 size=3 value='<% port_forward_table("ip","6"); %>' name=ip6 onBlur=valid_range(this,0,254,"IP") class=num></FONT></TD>
			                <TD width=47 height=30><INPUT type=checkbox value=on name=enable6 <% port_forward_table("enable","6"); %>></TD>
			            </TR>
						
						<TR align=middle>
			                <TD width=66 height=30><FONT size=2><INPUT  maxLength=12 size=7 name=name7 onBlur=valid_name(this,"Name") value='<% port_forward_table("name","7"); %>' class=num></FONT></TD>
			                <TD width=82 height=30><FONT face="Arial, Helvetica, sans-serif"><INPUT  maxLength=5 size=5 value='<% port_forward_table("from","7"); %>' name=from7 onBlur=valid_range(this,0,65535,"Port") class=num><span >&nbsp; to</span></FONT></TD>
			                <TD width=57 height=30><INPUT  maxLength=5 size=5 value='<% port_forward_table("to","7"); %>' name=to7 onBlur=valid_range(this,0,65535,"Port") class=num></TD>
			                <TD align=middle width=70 height=30>
			                	<FONT face=Arial color=blue>
								<SELECT size=1 name=pro7>
									<OPTION value=tcp <% port_forward_table("sel_tcp","7"); %>>TCP</OPTION>
									<OPTION value=udp <% port_forward_table("sel_udp","7"); %>>UDP</OPTION>
									<OPTION value=both <% port_forward_table("sel_both","7"); %>><script>Capture(share.both)</script></OPTION>
								</SELECT>
								</FONT>
							</TD>
			                <TD width=105 height=30><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><INPUT  maxLength=3 size=3 value='<% port_forward_table("ip","7"); %>' name=ip7 onBlur=valid_range(this,0,254,"IP") class=num></FONT></TD>
			                <TD width=47 height=30><INPUT type=checkbox value=on name=enable7 <% port_forward_table("enable","7"); %>></TD>
			            </TR>
			            
			            <TR align=middle>
			                <TD width=66 height=30><FONT size=2><INPUT  maxLength=12 size=7 name=name8 onBlur=valid_name(this,"Name") value='<% port_forward_table("name","8"); %>' class=num></FONT></TD>
			                <TD width=82 height=30><FONT face="Arial, Helvetica, sans-serif"><INPUT  maxLength=5 size=5 value='<% port_forward_table("from","8"); %>' name=from8 onBlur=valid_range(this,0,65535,"Port") class=num><span >&nbsp; to</span></FONT></TD>
			                <TD width=57 height=30><INPUT  maxLength=5 size=5 value='<% port_forward_table("to","8"); %>' name=to8 onBlur=valid_range(this,0,65535,"Port") class=num></TD>
			                <TD align=middle width=70 height=30><FONT face=Arial color=blue>
								<SELECT size=1 name=pro8>
									<OPTION value=tcp <% port_forward_table("sel_tcp","8"); %>>TCP</OPTION>
									<OPTION value=udp <% port_forward_table("sel_udp","8"); %>>UDP</OPTION>
									<OPTION value=both <% port_forward_table("sel_both","8"); %>><script>Capture(share.both)</script></OPTION>
								</SELECT>
								</FONT>
							</TD>
			                <TD width=105 height=30><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><INPUT  maxLength=3 size=3 value='<% port_forward_table("ip","8"); %>' name=ip8 onBlur=valid_range(this,0,254,"IP") class=num></FONT></TD>
			                <TD width=47 height=30><INPUT type=checkbox value=on name=enable8 <% port_forward_table("enable","8"); %>></TD>
			            </TR>
			            
		                <TR align=middle>
			                <TD width=66 height=30><FONT size=2><INPUT  maxLength=12 size=7 name=name9 onBlur=valid_name(this,"Name") value='<% port_forward_table("name","9"); %>' class=num></FONT></TD>
			                <TD width=82 height=30><FONT face="Arial, Helvetica, sans-serif"><INPUT  maxLength=5 size=5 value='<% port_forward_table("from","9"); %>' name=from9 onBlur=valid_range(this,0,65535,"Port") class=num><span >&nbsp; to</span></FONT></TD>
			                <TD width=57 height=30><INPUT  maxLength=5 size=5 value='<% port_forward_table("to","9"); %>' name=to9 onBlur=valid_range(this,0,65535,"Port") class=num></TD>
				            <TD align=middle width=70 height=30>
				            	<FONT face=Arial color=blue>
								<SELECT size=1 name=pro9>
									<OPTION value=tcp <% port_forward_table("sel_tcp","9"); %>>TCP</OPTION>
									<OPTION value=udp <% port_forward_table("sel_udp","9"); %>>UDP</OPTION>
									<OPTION value=both <% port_forward_table("sel_both","9"); %>><script>Capture(share.both)</script></OPTION>
								</SELECT>
								</FONT>
							</TD>
			                <TD width=105 height=30><FONT style="FONT-SIZE: 8pt" face=Arial><% prefix_ip_get("lan_ipaddr",1); %><INPUT  maxLength=3 size=3 value='<% port_forward_table("ip","9"); %>' name=ip9 onBlur=valid_range(this,0,254,"IP") class=num></FONT></TD>
			                <TD width=47 height=30><INPUT type=checkbox value=on name=enable9 <% port_forward_table("enable","9"); %>></TD>
			            </TR>
                	</TBODY>
	                </TABLE>
    	            </CENTER>          
          		</TD>
        	  	<TD width=47 height=25>&nbsp;</TD>
	          	<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
			</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">&nbsp;</TD>
          		<TD height=25 width="271">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 colspan="2" width="466"><p align="center"><b><script>Capture(apptopmenu.porttrigger)</script></b></TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 colspan="2" width="466">
            		<TABLE id=AutoNumber16 style="BORDER-COLLAPSE: collapse" borderColor=#e7e7e7 height=24 cellSpacing=0 cellPadding=0 width=428 border=0>
              		<TBODY>
              			<TR>
                <TD style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
	                align=middle width=79 bgColor=#CCCCCC height=30>
	            </TD>
                <TD style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
                	align=middle bgColor=#CCCCCC colSpan=2 height=30>
                	<B><script>Capture(appporttrigger.triggerrange)</script></B>
                </TD>
                <TD style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
                	align=middle bgColor=#CCCCCC colSpan=2 height=30><B><script>Capture(appporttrigger.forwardrange)</script></B>
                </TD>
              	<TD style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
                	align=middle width=33 bgColor=#CCCCCC height=30>
                </TD>
            </TR>
                
              	<TR>
                	<TD style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
                		align=middle bgColor=#CCCCCC  height=30>
                		<B><script>Capture(bmenu.application)</script></B>
                	</TD>
                	<TD style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
                		align=middle width=65 bgColor=#CCCCCC height=30>
                		<B><script>Capture(appporttrigger.startport)</script></B>
                	</TD>
                	<TD style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
                		align=middle width=55 bgColor=#CCCCCC height=30>
                		<B><script>Capture(appporttrigger.endport)</script></B>
               		</TD>
                	<TD style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
                		align=middle width=65 bgColor=#CCCCCC height=30>
                		<B><script>Capture(appporttrigger.startport)</script></B>
                	</TD>
                	<TD style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
                		align=middle width=55 bgColor=#CCCCCC height=30>
                		<B><script>Capture(appporttrigger.endport)</script></B>
                	</TD>
                	<TD style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
                		align=middle width=10 bgColor=#CCCCCC height=30>
                		<B><script>Capture(share.enabled)</script> </B>
                	</TD>
            	</TR>
              
              	<TR>
                	<TD align=middle height=30>
                		<FONT size=2>
						<INPUT class=num maxLength=12 size=16 name=name0 onBlur=valid_name(this,"Name") value='<% port_trigger_table("name","0"); %>'>
						</FONT>
					</TD>
                	<TD align=middle height=30>
                		<INPUT class=num maxLength=5 size=5 name=i_from0 value='<% port_trigger_table("i_from","0"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;to
                	</TD>
                	<TD align=middle height=30>
                		<INPUT class=num maxLength=5 size=5 name=i_to0 value='<% port_trigger_table("i_to","0"); %>' onBlur=valid_range(this,0,65535,"Port")>
               		</TD>
                	<TD align=middle height=30>
                		<INPUT class=num maxLength=5 size=5 name=o_from0 value='<% port_trigger_table("o_from","0"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;to
               		</TD>
                	<TD align=middle height=30>
                		<INPUT class=num maxLength=5 size=5 name=o_to0 value='<% port_trigger_table("o_to","0"); %>' onBlur=valid_range(this,0,65535,"Port")>
                	</TD>
                	<TD align=middle height=30>
                		<INPUT type=checkbox name=enable0 value=on <% port_trigger_table("enable","0"); %>>
                	</TD>    
              	</TR>
              	
              	<TR>
                	<TD align=middle height=30><FONT size=2>
					<INPUT class=num maxLength=12 size=16 name=name1 onBlur=valid_name(this,"Name") value='<% port_trigger_table("name","1"); %>'></FONT></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_from1 value='<% port_trigger_table("i_from","1"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;to</TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_to1 value='<% port_trigger_table("i_to","1"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_from1 value='<% port_trigger_table("o_from","1"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;to</TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_to1 value='<% port_trigger_table("o_to","1"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                	<TD align=middle height=30><INPUT type=checkbox name=enable1 value=on <% port_trigger_table("enable","1"); %>></TD>    
              	</TR>
              	
              	<TR>
                	<TD align=middle height=30><FONT size=2>
					<INPUT class=num maxLength=12 size=16 name=name2 onBlur=valid_name(this,"Name") value='<% port_trigger_table("name","2"); %>'></FONT></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_from2 value='<% port_trigger_table("i_from","2"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;to</TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_to2 value='<% port_trigger_table("i_to","2"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_from2 value='<% port_trigger_table("o_from","2"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;to</TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_to2 value='<% port_trigger_table("o_to","2"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                	<TD align=middle height=30><INPUT type=checkbox name=enable2 value=on <% port_trigger_table("enable","2"); %>></TD>    
              	</TR>
              	
              	<TR>
                	<TD align=middle height=30><FONT size=2>
					<INPUT class=num maxLength=12 size=16 name=name3 onBlur=valid_name(this,"Name") value='<% port_trigger_table("name","3"); %>'></FONT></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_from3 value='<% port_trigger_table("i_from","3"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_to3 value='<% port_trigger_table("i_to","3"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_from3 value='<% port_trigger_table("o_from","3"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_to3 value='<% port_trigger_table("o_to","3"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                	<TD align=middle height=30><INPUT type=checkbox name=enable3 value=on <% port_trigger_table("enable","3"); %>></TD>    
              	</TR>
              	
              	<TR>
                	<TD align=middle height=30><FONT size=2><INPUT class=num maxLength=12 size=16 name=name4 onBlur=valid_name(this,"Name") value='<% port_trigger_table("name","4"); %>'></FONT></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_from4 value='<% port_trigger_table("i_from","4"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_to4 value='<% port_trigger_table("i_to","4"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_from4 value='<% port_trigger_table("o_from","4"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_to4 value='<% port_trigger_table("o_to","4"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                	<TD align=middle height=30><INPUT type=checkbox name=enable4 value=on <% port_trigger_table("enable","4"); %>></TD>    
              	</TR>
              	
              	<TR>
                	<TD align=middle height=30><FONT size=2><INPUT class=num maxLength=12 size=16 name=name5 onBlur=valid_name(this,"Name") value='<% port_trigger_table("name","5"); %>'></FONT></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_from5 value='<% port_trigger_table("i_from","5"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_to5 value='<% port_trigger_table("i_to","5"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_from5 value='<% port_trigger_table("o_from","5"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_to5 value='<% port_trigger_table("o_to","5"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                	<TD align=middle height=30><INPUT type=checkbox name=enable5 value=on <% port_trigger_table("enable","5"); %>></TD>    
              	</TR>
              	
              	<TR>
                	<TD align=middle height=30><FONT size=2><INPUT class=num maxLength=12 size=16 name=name6 onBlur=valid_name(this,"Name") value='<% port_trigger_table("name","6"); %>'></FONT></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_from6 value='<% port_trigger_table("i_from","6"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_to6 value='<% port_trigger_table("i_to","6"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_from6 value='<% port_trigger_table("o_from","6"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_to6 value='<% port_trigger_table("o_to","6"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                	<TD align=middle height=30><INPUT type=checkbox name=enable6 value=on <% port_trigger_table("enable","6"); %>></TD>    
              	</TR>
              	
              	<TR>
                	<TD align=middle height=30><FONT size=2><INPUT class=num maxLength=12 size=16 name=name7 onBlur=valid_name(this,"Name") value='<% port_trigger_table("name","7"); %>'></FONT></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_from7 value='<% port_trigger_table("i_from","7"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_to7 value='<% port_trigger_table("i_to","7"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_from7 value='<% port_trigger_table("o_from","7"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_to7 value='<% port_trigger_table("o_to","7"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                	<TD align=middle height=30><INPUT type=checkbox name=enable7 value=on <% port_trigger_table("enable","7"); %>></TD>    
              	</TR>
              	
              	<TR>
                	<TD align=middle height=30><FONT size=2><INPUT class=num maxLength=12 size=16 name=name8 onBlur=valid_name(this,"Name") value='<% port_trigger_table("name","8"); %>'></FONT></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_from8 value='<% port_trigger_table("i_from","8"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_to8 value='<% port_trigger_table("i_to","8"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_from8 value='<% port_trigger_table("o_from","8"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_to8 value='<% port_trigger_table("o_to","8"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                	<TD align=middle height=30><INPUT type=checkbox name=enable8 value=on <% port_trigger_table("enable","8"); %>></TD>    
              	</TR>
              	
              	<TR>
                	<TD align=middle height=30><FONT size=2><INPUT class=num maxLength=12 size=16 name=name9 onBlur=valid_name(this,"Name") value='<% port_trigger_table("name","9"); %>'></FONT></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_from9 value='<% port_trigger_table("i_from","9"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_to9 value='<% port_trigger_table("i_to","9"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_from9 value='<% port_trigger_table("o_from","9"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                	<TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_to9 value='<% port_trigger_table("o_to","9"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                	<TD align=middle height=30><INPUT type=checkbox name=enable9 value=on <% port_trigger_table("enable","9"); %>></TD>    
              	</TR>
              </TBODY>
              </TABLE>         
          </TD>
          <TD width=47 height=25>&nbsp;</TD>
          <TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
        </TR>
  
		<TR>
          <TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="21">&nbsp;</TD>
          <TD height=25 colspan="2" width="466">&nbsp;</TD>
          <TD width=47 height=25>&nbsp;</TD>
          <TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
  
		<TR>
          <TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="21">&nbsp;</TD>
          <TD height=25 width="195">DMZ:</TD>
          <TD height=25 width="271">&nbsp;
          	<b>
	        <script language=javascript>
          		var state = "<% nvram_get("dmz_enable"); %>"
          		if(state == 1)
          			document.write("Enabled");
          		else
          			document.write("Disabled");
          	</script>
          	</b>
          </TD>
          <TD width=47 height=25>&nbsp;</TD>
          <TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
        </TR>
          
         <TR>
	      <TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="21">&nbsp;</TD>
          <TD width=195 height=25><script>Capture(appdmz.dmzhostip)</script>:</TD>
          <TD width=271 height=25>
          
          	<script language=javascript>
          		var state = "<% nvram_get("dmz_enable"); %>";
          		if(state == 1)		
		          	document.write("<b>&nbsp;" + "<% prefix_ip_get("lan_ipaddr",1); %>" + "<% nvram_get("dmz_ipaddr"); %>");
				else
					document.write("<b>&nbsp;Unavailable</b>");		          	
			</script>
			
		  </TD>
          <TD width=47 height=25>&#12288;</TD>
          <TD width=7 background=image/UI_05.gif height=25>&#12288;</TD>
         </TR>


	<TR>
          <TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="21">&nbsp;</TD>
          <TD height=25 width="195">&nbsp;</TD>
          <TD height=25 width="271">&nbsp;</TD>
          <TD width=47 height=25>&nbsp;</TD>
          <TD width=7 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
  
	<TR>
          <TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="21">&nbsp;</TD>
          <TD height=25 width="195">&nbsp;</TD>
          <TD height=25 width="271">&nbsp;</TD>
          <TD width=47 height=25>&nbsp;</TD>
          <TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
  
	<TR>
          <TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="21">&nbsp;</TD>
          <TD height=25 width="195"><b><script>Capture(setinetsetup.qostype)</script></b>:</TD>
          <TD height=25 width="271">&nbsp;<b>
          <script language=javascript>
          	var state = "<% nvram_get("qosSW"); %>"
          	if(state == 1)
          		document.write("Enabled");
          	else
          		document.write("Disabled");
          </script></b></TD>
          <TD width=47 height=25>&nbsp;</TD>
          <TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
  
	<TR>
          <TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="21">&nbsp;</TD>
          <TD height=25 width="195">&nbsp;</TD>
          <TD height=25 width="271">&nbsp;</TD>
          <TD width=47 height=25>&nbsp;</TD>
          <TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
  
	<TR>
          <TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="21">&nbsp;</TD>
          <TD height=25 width="195"><b><script>Capture()</script>Application-based QoS</b></TD>
          <TD height=25 width="271">&nbsp;</TD>
          <TD width=47 height=25>&nbsp;</TD>
          <TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
  
        <TR>
          <TD width=87 bgColor=#e7e7e7 height=25>
            <P align=right><FONT style="FONT-WEIGHT: 700"></FONT>&nbsp;</P></TD>
          <TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="21">&nbsp;</TD>
          <TD width=195 height=25><FONT style="FONT-SIZE: 8pt">&nbsp;&nbsp; </FONT></TD>
          <TD width=271 height=25><SPAN lang=zh-tw style="FONT-SIZE: 8pt"></SPAN><FONT style="FONT-SIZE: 8pt" face=Arial></FONT>
            <TABLE>
              <CENTER>
              <TBODY>
              <TR>
                <TD align=middle width=80><B><script>Capture(appbaseqos.hpriority)</script></B></TD>
                <TD align=middle width=90><B><script>Capture(appbaseqos.mpriority)</script></B></TD>
                <TD align=middle width=80><B><script>Capture(appbaseqos.lpriority)</script></B></TD></CENTER></TR></TBODY></TABLE></TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	 </TR>
	
        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25><P align=right><FONT style="FONT-WEIGHT: 700"></FONT></P></TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD width=195 height=25>FTP<B>&nbsp;&nbsp; </B> </TD>
          		<TD width=271 height=25>
          		<FONT style="FONT-SIZE: 8pt" face=Arial>
            	<TABLE valign="top">
              	<TBODY>
              	<TR>
                <TD align=middle width=80><INPUT type=radio value=2 name=qos_ftp <% qos_config("qos_ftp",2); %> >&nbsp;&nbsp;</TD>
                <TD align=middle width=80><INPUT type=radio value=1 name=qos_ftp <% qos_config("qos_ftp",1); %> >&nbsp;&nbsp;</TD>
                <TD align=middle width=80><INPUT type=radio  value=0 name=qos_ftp <% qos_config("qos_ftp",0); %> >&nbsp;&nbsp;</TD>
                </TR>
                </TBODY>
                </TABLE>
                </FONT>
                </TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>

        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25><P align=right>&nbsp;</P></TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD width=195 height=25>HTTP<B>&nbsp;&nbsp; </B> </TD>
          		<TD width=271 height=25>
          		<FONT style="FONT-SIZE: 8pt" face=Arial>
            	<TABLE valign="top">
              	<TBODY>
              	<TR>
                <TD align=middle width=80><INPUT type=radio value=2 name=qos_http <% qos_config("qos_http",2); %> >&nbsp;&nbsp;</TD>
                <TD align=middle width=80><INPUT type=radio value=1 name=qos_http <% qos_config("qos_http",1); %> >&nbsp;&nbsp;</TD>
                <TD align=middle width=80><INPUT type=radio value=0 name=qos_http <% qos_config("qos_http",0); %> >&nbsp;&nbsp;</TD>
                </TR>
                </TBODY>
                </TABLE>
                </FONT>
                </TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
          	
        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25><P align=right>&nbsp;</P></TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD width=195 height=25>Telnet&nbsp;<B>&nbsp; </B> </TD>
          		<TD width=271 height=25>
          		<FONT style="FONT-SIZE: 8pt" face=Arial>
            	<TABLE valign="top">
              	<TBODY>
              	<TR>
                	<TD align=middle width=80><INPUT type=radio value=2 name=qos_telnet <% qos_config("qos_telnet",2); %> >&nbsp;&nbsp;</TD>
                	<TD align=middle width=80><INPUT type=radio value=1 name=qos_telnet <% qos_config("qos_telnet",1); %> >&nbsp;&nbsp;</TD>
                	<TD align=middle width=80><INPUT type=radio  value=0 name=qos_telnet <% qos_config("qos_telnet",0); %> >&nbsp;&nbsp;</TD>
                </TR>
                </TBODY>
                </TABLE>
                </FONT>
                </TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
          	
        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25><P align=right>&nbsp;</P></TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD width=195 height=25>SMTP<B>&nbsp;&nbsp; </B> </TD>
          		<TD width=271 height=25><FONT style="FONT-SIZE: 8pt" face=Arial>
            	<TABLE valign="top">
              	<TBODY>
              	<TR>
                	<TD align=middle width=80><INPUT type=radio value=2 name=qos_smtp <% qos_config("qos_smtp",2); %> >&nbsp;&nbsp;</TD>
                	<TD align=middle width=80><INPUT type=radio value=1 name=qos_smtp <% qos_config("qos_smtp",1); %> >&nbsp;&nbsp;</TD>
                	<TD align=middle width=80><INPUT type=radio  value=0 name=qos_smtp <% qos_config("qos_smtp",0); %> >&nbsp;&nbsp;</TD>
                </TR>
                </TBODY>
                </TABLE>
                </FONT>
                </TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
          	
        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25><P align=right>&nbsp;</P></TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD width=195 height=25>POP3<B>&nbsp;&nbsp; </B> </TD>
          		<TD width=271 height=25>
          		<FONT style="FONT-SIZE: 8pt" face=Arial>
            	<TABLE valign="top">
              	<TBODY>
              	<TR>
                	<TD align=middle width=80><INPUT type=radio value=2 name=qos_pop3 <% qos_config("qos_pop3",2); %> >&nbsp;&nbsp;</TD>
                	<TD align=middle width=80><INPUT type=radio value=1 name=qos_pop3 <% qos_config("qos_pop3",1); %> >&nbsp;&nbsp;</TD>
                	<TD align=middle width=80><INPUT type=radio  value=0 name=qos_pop3 <% qos_config("qos_pop3",0); %> >&nbsp;&nbsp;</TD>
                </TR>
                </TBODY>
                </TABLE>
                </FONT>
                </TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
          	
        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25><P align=right>&nbsp;</P></TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD width=195 height=25><script>Capture(appbaseqos.specport)</script><B>
          			<INPUT class=num style="FONT-FAMILY: Courier" maxLength=5 size=6 value=<% qos_config("qos_spec_port0",0); %>  name=qos_spec_port0>&nbsp;&nbsp; </B>
          		</TD>
          		<TD width=271 height=25><FONT style="FONT-SIZE: 8pt" face=Arial>
            	<TABLE valign="top">
              	<TBODY>
              	<TR>
                	<TD align=middle width=80><INPUT type=radio value=2 name=qos_spec0 <% qos_config("qos_spec0",2); %> >&nbsp;&nbsp;</TD>
                	<TD align=middle width=80><INPUT type=radio value=1 name=qos_spec0 <% qos_config("qos_spec0",1); %> >&nbsp;&nbsp;</TD>
                	<TD align=middle width=80><INPUT type=radio  value=0 name=qos_spec0 <% qos_config("qos_spec0",0); %> >&nbsp;&nbsp;</TD>
                </TR>
                </TBODY>
                </TABLE>
                </FONT>
                </TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
          	
        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25><P align=right>&nbsp;</P></TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD width=195 height=25><script>Capture(appbaseqos.specport)</script><B>
          			<INPUT class=num onblur=PortCheck(this) style="FONT-FAMILY: Courier" maxLength=5 size=6 
            		value=<% qos_config("qos_spec_port1",0); %> name=qos_spec_port1>&nbsp;&nbsp; </B></TD>
          		<TD width=271 height=25><FONT style="FONT-SIZE: 8pt" face=Arial>
            	<TABLE valign="top">
              	<TBODY>
              		<TR>
               	 		<TD align=middle width=80><INPUT type=radio value=2 name=qos_spec1 <% qos_config("qos_spec1",2); %> >&nbsp;&nbsp;</TD>
                		<TD align=middle width=80><INPUT type=radio value=1 name=qos_spec1 <% qos_config("qos_spec1",1); %> >&nbsp;&nbsp;</TD>
                		<TD align=middle width=80><INPUT type=radio  value=0 name=qos_spec1 <% qos_config("qos_spec1",0); %> >&nbsp;&nbsp;</TD>
               		</TR>
               	</TBODY>
               	</TABLE></FONT>
               	</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
          	
        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25><P align=right>&nbsp;</P></TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD width=195 height=25><script>Capture(appbaseqos.specport)</script><B>
          			<INPUT class=num onblur=PortCheck(this) style="FONT-FAMILY: Courier" maxLength=5 size=6 
            				value=<% qos_config("qos_spec_port2",0); %>  name=qos_spec_port2>&nbsp;&nbsp; </B>
            	</TD>
          		<TD width=271 height=25><FONT style="FONT-SIZE: 8pt" face=Arial>
            	<TABLE valign="top">
              	<TBODY>
              	<TR>
                <TD align=middle width=80><INPUT type=radio value=2 name=qos_spec2 <% qos_config("qos_spec2",2); %> >&nbsp;&nbsp;</TD>
                <TD align=middle width=80><INPUT type=radio value=1 name=qos_spec2 <% qos_config("qos_spec2",1); %> >&nbsp;&nbsp;</TD>
                <TD align=middle width=80><INPUT type=radio  value=0 name=qos_spec2 <% qos_config("qos_spec2",0); %> >&nbsp;&nbsp;</TD>
             </TR>
             </TBODY>
             </TABLE></FONT>
             </TD>
          	<TD width=47 height=25>&nbsp;</TD>
          	<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
          	
         	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">&nbsp;</TD>
          		<TD height=25 width="271">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
        	</TR>
        
        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195"><b><script>Capture(appleftmenu.advqos)</script></b></TD>
          		<TD height=25 width="271">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
        	</TR>
        
          	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25><P align=right>&nbsp;</P></TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
         		<TD width=466 height=25 colspan="2"> <input type=hidden name=hid_qos_adv value=0>
          			<input type=checkbox value=1 name=qos_adv <%qos_config("qos_adv",0);%> ><font face=Arial size=2>&nbsp;<script>Capture(appadvqos.fragsize)</script> : <INPUT class=num  
            			style="FONT-SIZE: 10pt; FONT-FAMILY: Courier" maxLength=5 size=6 name=qos_size value=<% qos_config("qos_size",0); %>  >&nbsp;bytes.&nbsp; </FONT>
       			</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
        	</TR>
        
         	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25><P align=right><FONT style="FONT-WEIGHT: 700"></FONT></P></TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD width=466 height=25 colspan="2"> <input type=hidden name=hid_qos_vlan value=0>
          			<input type=checkbox value=1 name=qos_vlan <%qos_config("qos_vlan",0);%> ><font face=Arial size=2>&nbsp;Enable 802.1p P bits scheduling.VLAN'S VID: 
          			<INPUT class=num style="FONT-SIZE: 10pt; FONT-FAMILY: Courier" maxLength=5 size=6 name=qos_vid value=<% qos_config("qos_vid",0); %>  >
       			</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
 
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">&nbsp;</TD>
          		<TD height=25 width="271">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
  
			<TR>
		        <TD width=87 bgColor=#000000 height=25><P align=center><b><font face="Arial" style="font-size: 9pt" color="#FFFFFF"><script>Capture()</script>Miscellaneous</font></b></P></TD>
          		<TD width=13 bgColor=#000000 height=25>&nbsp;</TD>
		        <TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">&nbsp;</TD>
          		<TD height=25 width="271">&nbsp;</TD>
          		<TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">SNMP:</TD>
          		<TD height=25 width="271">&nbsp;<B>
          			<script language=javascript>
          			var state="<% nvram_get("snmp_enable"); %>"
          			if(state == 1)
          				document.write("Enabled");
          			else
          				document.write("Disabled");
          			</script></B>
          		</TD>
                <TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
			<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
          		<TD height=25 width="195">UPnP:</TD>
          		<TD height=25 width="271">&nbsp;<B>
          			<script language=javascript>
          				var state="<% nvram_get("upnp_enable"); %>"
          				if(state == 1)
          					document.write("Enabled");
          				else
          					document.write("Disabled");
          			</script></B>
          		</TD>
                <TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
  
        	<TR>
          		<TD width=87 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          		<TD width=13 background=image/UI_04.gif height=25>&nbsp;</TD>
          		<TD colSpan=3 height=25 width="21">&nbsp;</TD>
         		<TD height=25 width="195">WT-82</TD>
          		<TD height=25 width="271">&nbsp;<B>
          			<script language=javascript>
          			var state="<%nvram_get("wt82_enable");%>"
          			if(state == 1)
          				document.write("Enabled");
          			else
          				document.write("Disabled");         		
          			</script></B>
          		</TD>
                <TD width=47 height=25>&nbsp;</TD>
          		<TD width=7 background=image/UI_05.gif height=25>&nbsp;</TD>
          	</TR>
	        </TBODY>
	        </TABLE>
	    </TD>
    	<TD vAlign=top width=176 bgColor=#6666cc><p style="line-height: 200%"><span >&nbsp;&nbsp;&nbsp; </span><font color="#FFFFFF">
          <span ></span></font>
        </TD>
     </TR>


<TR>
<TD width=809 colSpan=2>

<TABLE cellSpacing=0 cellPadding=0 border=0>
	<TR>
	<TD width=110 bgColor=#e7e7e height=30>&nbsp;</TD>
	<TD width=8 ><IMG src='image/UI_04.gif' border=0 width=8 height=30></TD>
	<TD width=16>&nbsp;</TD>
	<TD width=12>&nbsp;</TD>
	<TD width=465>&nbsp;</TD>
	<TD width=15>&nbsp;</TD>
	<TD width=15 ><IMG src='image/UI_05.gif' border=0 width=15 height=30></TD>
	<TD width=168 bgColor=#6666cc rowSpan=2><IMG height=64 src='image/UI_Cisco.gif' width=168 border=0></TD>
	</TR>

	<TR>
	<TD width=110 bgColor=#000000>&nbsp;</TD>
	<TD width=8 bgColor=#000000>&nbsp;</TD>
	<TD width=16 bgColor=#6666cc>&nbsp;</TD>
	<TD width=12 bgColor=#6666cc>&nbsp;</TD>
	<TD width=465 bgColor=#6666cc>&nbsp;</TD>
	<TD width=15 bgColor=#6666cc height=33>&nbsp;</TD>
	<TD width=15 bgColor=#000000 height=33>&nbsp;</TD>
	</TR>
</TABLE>

</TD>
</TR>


</TBODY>
</TABLE>
</FORM>
</DIV>
</BODY>
</HTML>
