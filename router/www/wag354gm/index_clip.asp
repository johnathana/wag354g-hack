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
	<input type=hidden name=clip_config>
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
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=25><b><script>Capture(setleftmenu.ipsetting)</script></b></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial>&nbsp;<script>Capture(setddns.ipaddr)</script>:&nbsp;</FONT></TD>
          <TD width=296 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial><input type=hidden name="clip_ipaddr" value=4>
		<INPUT class=num maxLength=3 size=3 value='<% clip_config("ipaddr", 0); %>' name="clip_ipaddr_0" onBlur=valid_range(this,1,223,"IP")> .
		<INPUT class=num maxLength=3 size=3 value='<% clip_config("ipaddr", 1); %>' name="clip_ipaddr_1" onBlur=valid_range(this,0,255,"IP")> .
		<INPUT class=num maxLength=3 size=3 value='<% clip_config("ipaddr", 2); %>' name="clip_ipaddr_2" onBlur=valid_range(this,0,255,"IP")> .
		<INPUT class=num maxLength=3 size=3 value='<% clip_config("ipaddr", 3); %>' name="clip_ipaddr_3" onBlur=valid_range(this,1,254,"IP")>
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
            face=Arial><input type=hidden name="clip_netmask" value=4>
		<INPUT class=num maxLength=3 size=3 value='<% clip_config("netmask", 0); %>' name="clip_netmask_0" onBlur=valid_range(this,0,255,"netmask")> .
		<INPUT class=num maxLength=3 size=3 value='<% clip_config("netmask", 1); %>' name="clip_netmask_1" onBlur=valid_range(this,0,255,"netmask")> .
		<INPUT class=num maxLength=3 size=3 value='<% clip_config("netmask", 2); %>' name="clip_netmask_2" onBlur=valid_range(this,0,255,"netmask")> .
		<INPUT class=num maxLength=3 size=3 value='<% clip_config("netmask", 3); %>' name="clip_netmask_3" onBlur=valid_range(this,0,255,"netmask")>
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
            face=Arial><input type=hidden name="clip_gateway" value=4>
		<INPUT class=num maxLength=3 size=3 value='<% clip_config("gateway", 0); %>' name="clip_gateway_0" onBlur=valid_range(this,1,223,"IP")> .
		<INPUT class=num maxLength=3 size=3 value='<% clip_config("gateway", 1); %>' name="clip_gateway_1" onBlur=valid_range(this,0,255,"IP")> .
		<INPUT class=num maxLength=3 size=3 value='<% clip_config("gateway", 2); %>' name="clip_gateway_2" onBlur=valid_range(this,0,255,"IP")> .
		<INPUT class=num maxLength=3 size=3 value='<% clip_config("gateway", 3); %>' name="clip_gateway_3" onBlur=valid_range(this,1,254,"IP")>
	  </FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
		<TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;<script>Capture(setinetsetup.pridns)</script>:&nbsp;</FONT></TD>
          <TD width=296 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial><input type=hidden name="clip_dns" value="3">
		<INPUT class=num maxLength=3 size=3 value='<% clip_config("pdns", 0); %>' name="clip_pdns_0" onBlur=valid_range(this,1,223,"IP")> .
		<INPUT class=num maxLength=3 size=3 value='<% clip_config("pdns", 1); %>' name="clip_pdns_1" onBlur=valid_range(this,0,255,"IP")> .
		<INPUT class=num maxLength=3 size=3 value='<% clip_config("pdns", 2); %>' name="clip_pdns_2" onBlur=valid_range(this,0,255,"IP")> .
		<INPUT class=num maxLength=3 size=3 value='<% clip_config("pdns", 3); %>' name="clip_pdns_3" onBlur=valid_range(this,1,254,"IP")>
	  </FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
		<TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;<script>Capture(setinetsetup.secdns)</script>:&nbsp;</FONT></TD>
          <TD width=296 height=25><FONT style="FONT-SIZE: 8pt" face=Arial>
		<INPUT class=num maxLength=3 size=3 value='<% clip_config("sdns", 0); %>' name="clip_sdns_0" onBlur=valid_range(this,1,223,"IP")> .
		<INPUT class=num maxLength=3 size=3 value='<% clip_config("sdns", 1); %>' name="clip_sdns_1" onBlur=valid_range(this,0,255,"IP")> .
		<INPUT class=num maxLength=3 size=3 value='<% clip_config("sdns", 2); %>' name="clip_sdns_2" onBlur=valid_range(this,0,255,"IP")> .
		<INPUT class=num maxLength=3 size=3 value='<% clip_config("sdns", 3); %>' name="clip_sdns_3" onBlur=valid_range(this,1,254,"IP")>
	  </FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
