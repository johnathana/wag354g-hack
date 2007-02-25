
<% status_htm("ppp", "multipppoe_hidden", 0); %>
 	<TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD></TD>
          <TD></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD><FONT style="FONT-SIZE: 8pt"><script>Capture(share.iface)</script>:</FONT></TD>
          <TD><FONT style="FONT-SIZE: 8pt"><B><% nvram_wanstatus_get("wan_interface", 1); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
	<TR>
	  <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD><FONT style="FONT-SIZE: 8pt"><script>Capture(share.ipaddr)</script>:</FONT></TD>
          <TD><FONT style="FONT-SIZE: 8pt"><B><% nvram_wanstatus_get("wan_ipaddr", 1); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
 	
	<TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=30 
            src="image/UI_04.gif" width=8 border=0></TD>
          <TD bgColor=#ffffff colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 bgColor=#ffffff height=25><FONT 
            style="FONT-SIZE: 8pt"><script>Capture(share.subnet)</script>:</FONT></TD>
          <TD width=296 bgColor=#ffffff height=25><FONT 
            style="FONT-SIZE: 8pt"><B><% nvram_wanstatus_get("wan_netmask", 1); %></B></FONT></TD>
          <TD width=13 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD width=15 bgColor=#ffffff height=25><IMG height=30 
            src="image/UI_05.gif" width=15 border=0></TD></TR>

        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
          height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD><FONT style="FONT-SIZE: 8pt">DNS 1:</FONT></TD>
          <TD><FONT style="FONT-SIZE: 8pt"><B><% nvram_wanstatus_get("wan_dns", 3); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
          height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt">DNS 2:</FONT></TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><B><% nvram_wanstatus_get("wan_dns", 4); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
          height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt">DNS 3:</FONT></TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><B><% nvram_wanstatus_get("wan_dns", 5); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
	<TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
          height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(share.srvname)</script>:</FONT></TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><B><% nvram_wanstatus_get("srvname", 1); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>
	<TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif 
          height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(share.acname)</script>:</FONT></TD>
          <TD height=25><FONT style="FONT-SIZE: 8pt"><B><% nvram_wanstatus_get("acname", 1); %></B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
          height=25>&nbsp;</TD></TR>

<% status_htm("ppp", "multipppoe_hidden", 1); %>
