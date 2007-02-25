
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

<% nvram_status_get("hidden1"); %>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD><FONT style="FONT-SIZE: 8pt"><script>Capture(stacontent.logsta)</script>:&nbsp;</FONT></TD>
          <TD><FONT style="FONT-SIZE: 8pt"><B>
<script language=javascript>
        var status1 = "<% nvram_status_get("status1"); %>";
        var status2 = "<% nvram_status_get("status2"); %>";
        if(status1 == "Status")         status1 = bmenu.statu;
        if(status2 == "Connecting")     status2 = hstatrouter2.connecting;
        else    if(status2 == "Disconnected")   status2 = hstatrouter2.disconnected;
        else    if(status2 == "Connected")      status2 = stacontent.conn;
	document.write(status2);
	document.write("&nbsp;&nbsp;");

	var but_arg = "<% nvram_status_get("button1"); %>";
        var wan_proto = "<% nvram_get("wan_proto"); %>";
        var but_type = "";
        if(but_arg == "Connect")        but_value = stacontent.connect;
        else if(but_arg == "Disconnect")        but_value = hstatrouter2.disconnect;
        but_type = but_arg +"_" + wan_proto;
	document.write("<INPUT type=button value='"+but_value+"' onClick=Connect(this.form,'"+but_type+"')>");
</script>
</B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
<% nvram_status_get("hidden2"); %>     

        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD><FONT style="FONT-SIZE: 8pt"><script>Capture(stainetconn.iface)</script>:&nbsp;</FONT></TD>
          <TD><FONT style="FONT-SIZE: 8pt"><B><% nvram_wanstatus_get("wan_interface", 10); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
<!--
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>MAC Address:</TD>
          <TD><b><% nvram_wanstatus_get("wan_hwaddr", 0); %></b></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
-->
	<TR>
	  <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD><FONT style="FONT-SIZE: 8pt"><script>Capture(stainetconn.ipaddr)</script>:&nbsp;</FONT></TD>
          <TD><FONT style="FONT-SIZE: 8pt"><B><% nvram_wanstatus_get("wan_ipaddr", 0); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=30 
            src="image/UI_04.gif" width=8 border=0></TD>
          <TD bgColor=#ffffff colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 bgColor=#ffffff height=25><FONT 
            style="FONT-SIZE: 8pt"><script>Capture(stainetconn.subnetmask)</script>:&nbsp;</FONT></TD>
          <TD width=296 bgColor=#ffffff height=25><FONT 
            style="FONT-SIZE: 8pt"><B><% nvram_wanstatus_get("wan_netmask", 0); %></B></FONT></TD>
          <TD width=13 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD width=15 bgColor=#ffffff height=25><IMG height=30 
            src="image/UI_05.gif" width=15 border=0></TD></TR>

        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=30 
            src="image/UI_04.gif" width=8 border=0></TD>
          <TD bgColor=#ffffff colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 bgColor=#ffffff height=25><FONT 
            style="FONT-SIZE: 8pt"><script>Capture(stainetconn.defaultgw)</script>:&nbsp;</FONT></TD>
          <TD width=296 bgColor=#ffffff height=25><FONT 
            style="FONT-SIZE: 8pt"><B><% nvram_wanstatus_get("wan_gateway", 0); %></B></FONT></TD>
          <TD width=13 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD width=15 bgColor=#ffffff height=25><IMG height=30 
            src="image/UI_05.gif" width=15 border=0></TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
          height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD><FONT style="FONT-SIZE: 8pt"><script>Capture(stainetconn.dns1)</script>:&nbsp;</FONT></TD>
          <TD><FONT style="FONT-SIZE: 8pt"><B><% nvram_wanstatus_get("wan_dns", 0); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
          height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(stainetconn.dns2)</script>:&nbsp;</FONT></TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><B><% nvram_wanstatus_get("wan_dns", 1); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
          height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(stainetconn.dns3)</script>:&nbsp;</FONT></TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><B><% nvram_wanstatus_get("wan_dns", 2); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
	
	<TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
          height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(stainetconn.wins)</script>:&nbsp;</FONT></TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><B><% nvram_get("wan_wins"); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>

<% pppoe_show_htm(0); %>	
	<TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
          height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(stainetconn.svcname)</script>:&nbsp;</FONT></TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><B><% nvram_wanstatus_get("srvname", 0); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
	<TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
          height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(stainetconn.acname)</script>:&nbsp;</FONT></TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><B><% nvram_wanstatus_get("acname", 0); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
<% pppoe_show_htm(1); %>	
