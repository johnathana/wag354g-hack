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
<input type=hidden name=bridged_config>
	<TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=1>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=6>
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
	<tr>
          <td align=right bgColor=#e7e7e7 colSpan=3><b><script>Capture(setleftmenu.ipsetting)</script></b></td>

          <td width=8 height=25><img height=25 src="image/UI_04.gif" 
            width=8 border=0></td>
          <td colSpan=3>&nbsp;</td>
          <td colSpan=2><input type=radio value=1 name=bridged_dhcpenable onclick=dhcpc_enable_disable(this.form,1) <% bridged_config("dhcpenable", 1); %>><b> <script>Capture(setinetsetup.obtain)</script></b></td>
          <td>&nbsp;</td>
          <td><img height=25 src="image/UI_05.gif" width=15 
        border=0></td></tr>
	<tr>
          <td align=right bgColor=#e7e7e7 colSpan=3>&nbsp;</td>
          <td width=8 height=25><img height=25 src="image/UI_04.gif" 
            width=8 border=0></td>

          <td colSpan=3>&nbsp;</td>
          <td colSpan=2><input type=radio value=0 name=bridged_dhcpenable onclick=dhcpc_enable_disable(this.form,0) <% bridged_config("dhcpenable", 0); %>><b> <script>Capture(setinetsetup.usefollow)</script>:</b></td>
          <td>&nbsp;</td>
          <td><img height=25 src="image/UI_05.gif" width=15 
        border=0></td></tr>
	
	<TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial>&nbsp;<script>Capture(setinetsetup.inetipaddr)</script>:&nbsp;</FONT></TD>
          <TD width=296 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial><input type=hidden name="bridged_ipaddr" value=4>
		<INPUT class=num maxLength=3 size=3 value='<% bridged_config("ipaddr", 0); %>' name="bridged_ipaddr_0" onBlur=valid_range(this,1,223,"IP") <% bridged_config("ipaddr", 4); %>> .
		<INPUT class=num maxLength=3 size=3 value='<% bridged_config("ipaddr", 1); %>' name="bridged_ipaddr_1" onBlur=valid_range(this,0,255,"IP") <% bridged_config("ipaddr", 4); %>> .
		<INPUT class=num maxLength=3 size=3 value='<% bridged_config("ipaddr", 2); %>' name="bridged_ipaddr_2" onBlur=valid_range(this,0,255,"IP") <% bridged_config("ipaddr", 4); %>> .
		<INPUT class=num maxLength=3 size=3 value='<% bridged_config("ipaddr", 3); %>' name="bridged_ipaddr_3" onBlur=valid_range(this,1,254,"IP") <% bridged_config("ipaddr", 4); %>>
	  </FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
		<TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial>&nbsp;<script>Capture(setinetsetup.subnetmask)</script>:&nbsp;</FONT></TD>
          <TD width=296 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial><input type=hidden name="bridged_netmask" value=4>
		<INPUT class=num maxLength=3 size=3 value='<% bridged_config("netmask", 0); %>' name="bridged_netmask_0" onBlur=valid_range(this,0,255,"netmask") <% bridged_config("netmask", 4); %>> .
		<INPUT class=num maxLength=3 size=3 value='<% bridged_config("netmask", 1); %>' name="bridged_netmask_1" onBlur=valid_range(this,0,255,"netmask") <% bridged_config("netmask", 4); %>> .
		<INPUT class=num maxLength=3 size=3 value='<% bridged_config("netmask", 2); %>' name="bridged_netmask_2" onBlur=valid_range(this,0,255,"netmask") <% bridged_config("netmask", 4); %>> .
		<INPUT class=num maxLength=3 size=3 value='<% bridged_config("netmask", 3); %>' name="bridged_netmask_3" onBlur=valid_range(this,0,255,"netmask") <% bridged_config("netmask", 4); %>>
	  </FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
		<TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial>&nbsp;<script>Capture(share.gateway)</script>:&nbsp;</FONT></TD>
          <TD width=296 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial><input type=hidden name="bridged_gateway" value=4>
		<INPUT class=num maxLength=3 size=3 value='<% bridged_config("gateway", 0); %>' name="bridged_gateway_0" onBlur=valid_range(this,1,223,"IP") <% bridged_config("gateway", 4); %>> .
		<INPUT class=num maxLength=3 size=3 value='<% bridged_config("gateway", 1); %>' name="bridged_gateway_1" onBlur=valid_range(this,0,255,"IP") <%
bridged_config("gateway", 4); %>> .
		<INPUT class=num maxLength=3 size=3 value='<% bridged_config("gateway", 2); %>' name="bridged_gateway_2" onBlur=valid_range(this,0,255,"IP") <%
bridged_config("gateway", 4); %>> .
		<INPUT class=num maxLength=3 size=3 value='<% bridged_config("gateway", 3); %>' name="bridged_gateway_3" onBlur=valid_range(this,1,254,"IP") <%
bridged_config("gateway", 4); %>>
	  </FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
		<TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;<script>Capture(setinetsetup.pridns)</script>:&nbsp;</FONT></TD>
          <TD width=296 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial><input type=hidden name="bridged_pdns" value="3">
		<INPUT class=num maxLength=3 size=3 value='<% bridged_config("pdns", 0); %>' name="bridged_pdns_0" onBlur=valid_range(this,1,223,"IP") <%
bridged_config("pdns", 4); %>> .
		<INPUT class=num maxLength=3 size=3 value='<% bridged_config("pdns", 1); %>' name="bridged_pdns_1" onBlur=valid_range(this,0,255,"IP") <%
bridged_config("pdns", 4); %>> .
		<INPUT class=num maxLength=3 size=3 value='<% bridged_config("pdns", 2); %>' name="bridged_pdns_2" onBlur=valid_range(this,0,255,"IP") <%
bridged_config("pdns", 4); %>> .
		<INPUT class=num maxLength=3 size=3 value='<% bridged_config("pdns", 3); %>' name="bridged_pdns_3" onBlur=valid_range(this,1,254,"IP") <%
bridged_config("pdns", 4); %>>
	  </FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
		<TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;<script>Capture(setinetsetup.secdns)</script>:&nbsp;</FONT></TD>
          <TD width=296 height=25><FONT style="FONT-SIZE: 8pt" face=Arial>
		<INPUT class=num maxLength=3 size=3 value='<% bridged_config("sdns", 0); %>' name="bridged_sdns_0" onBlur=valid_range(this,1,223,"IP") <%
bridged_config("sdns", 4); %>> .
		<INPUT class=num maxLength=3 size=3 value='<% bridged_config("sdns", 1); %>' name="bridged_sdns_1" onBlur=valid_range(this,0,255,"IP") <%
bridged_config("sdns", 4); %>> .
		<INPUT class=num maxLength=3 size=3 value='<% bridged_config("sdns", 2); %>' name="bridged_sdns_2" onBlur=valid_range(this,0,255,"IP") <%
bridged_config("sdns", 4); %>> .
		<INPUT class=num maxLength=3 size=3 value='<% bridged_config("sdns", 3); %>' name="bridged_sdns_3" onBlur=valid_range(this,1,254,"IP") <%
bridged_config("sdns", 4); %>>
	  </FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

<% support_elsematch("IPPPOE_SUPPORT", "1", "", "<!--"); %>     	
	<input type=hidden name=ipppoe_config>
	<TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=1>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=6>
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
	<TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;<script>Capture(setinetsetup.pppoesession)</script>:</TD>
          <TD width=296 height=25>
		<select name="ipppoe_enable" onChange=SelIPPP(this.form.ipppoe_enable.selectedIndex,this.form)>
                        <option value="0" <% ipppoe_enable_setting("disable", 0); %>><b><script>Capture(share.disabled)</script></b></option>
                        <option value="1" <% ipppoe_enable_setting("enable", 1); %>><b><script>Capture(share.enabled)</script>&nbsp;</b></option>
                </select></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

<% ipppoe_enable_setting("disable", 2); %>
	<TR>
	  <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;<script>Capture(setother.servicename)</script>:</TD>
          <TD width=296 height=25><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" maxLength=63 size=20 name="ipppoe_servicename" value='<% ipppoe_config("servicename", 0); %>' onBlur=valid_name(this,"IPPPPoE%20Service%20Name",SPACE_NO)></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

	<TR>
	  <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;<script>Capture(setddns.username)</script>:</TD>
          <TD width=296 height=25><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" maxLength=63 size=20 name="ipppoe_username" value='<% ipppoe_config("username", 0); %>' onBlur=valid_name(this,"IPPPPoE%20User%20Name",SPACE_NO)></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
 	  <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;<script>Capture(setddns.password)</script>:&nbsp;&nbsp;</TD>
	  <TD width=296 height=25><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" maxLength=63 size=20 name="ipppoe_password" type=password value='<% ipppoe_config("password", 0); %>' onBlur=valid_name(this,"IPPPPoE%20Password",SPACE_NO)></TD>
	  <TD width=13 height=25>&nbsp;</TD>
	  <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;<script>Capture(setother.matchs)</script>:&nbsp;&nbsp;</TD>
	  <TD width=296 height=25><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" maxLength=63 size=20 name="ipppoe_domainname" value='<% ipppoe_config("domainname", 0); %>' onBlur=valid_name(this,"IPPPPoE%20Domain%20Name",SPACE_NO)></TD>
	  <TD width=13 height=25>&nbsp;</TD>
	  <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

	<TR>
	  <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
	  <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
	  <TD colSpan=3 height=25>&nbsp;</TD>
	  <TD width=397 height=25 colspan="2"><INPUT type=radio value=1 name="ipppoe_demand" <% ipppoe_config("demand", 1); %> onclick=ipppoe_enable_disable(this.form,1) ><font color=<% nvram_else_match("aol_block_traffic1","1","gray","black"); %>><script>Capture(setother.condemand)</script>: <script>Capture(setother.maxidle)</script>&nbsp;</font>
	<INPUT class=num maxLength=4 size=4 value='<% ipppoe_config("idletime", 0); %>' name="ipppoe_idletime" onBlur=valid_range(this,1,9999,"IPPPPoE%20Idle%20time") <% ipppoe_config("idletime", 1); %>>&nbsp;<font color=<% nvram_else_match("aol_block_traffic1","1","gray","black"); %>><script>Capture(share.mins)</script></font></TD>
          </TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
       <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=397 height=25 colspan="2"><INPUT type=radio value=0 name="ipppoe_demand" <% ipppoe_config("demand", 0); %> onclick=ipppoe_enable_disable(this.form,0)><font color=<% nvram_else_match("aol_block_traffic1","1","gray","black"); %>><script>Capture(setother.keepalive)</script>: <script>Capture(setother.radial)</script>&nbsp;</font>
	<INPUT class=num maxLength=4 size=4 value='<% ipppoe_config("redialperiod", 0); %>' name="ipppoe_redialperiod" onBlur=valid_range(this,20,180,"IPPPPoE%20Redial%20period") <% ipppoe_config("redialperiod", 1); %>>&nbsp;<font color=<% nvram_else_match("aol_block_traffic1","1","gray","black"); %>><script>Capture(share.sec)</script>.</font></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
<% ipppoe_enable_setting("disable", 3); %>
<% support_elsematch("IPPPOE_SUPPORT", "1", "", "-->"); %>     	

