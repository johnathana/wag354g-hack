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
<input type=hidden name=unip_pppoe_config>
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
	  <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=1><B>IP Unnumbered over PPPoE Settings</B></TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;<script>Capture(setother.servicename)</script>:</TD>
          <TD width=296 height=25><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" maxLength=63 size=20 name="unip_pppoe_servicename" value='<% unip_pppoe_config("servicename", 0); %>' onBlur=valid_name(this,"Service%20Name",SPACE_NO)></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

	<TR>
	  <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=1></TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;<script>Capture(setddns.username)</script>:</TD>
          <TD width=296 height=25><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" maxLength=63 size=20 name="unip_pppoe_username" value='<% unip_pppoe_config("username", 0); %>' onBlur=valid_name(this,"User%20Name",SPACE_NO)></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;<script>Capture(setddns.password)</script>:&nbsp;&nbsp;</TD>
	  <TD width=296 height=25><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" maxLength=63 size=20 name="unip_pppoe_password" type=password value='<% unip_pppoe_config("password", 0); %>' onBlur=valid_name(this,"Password",SPACE_NO)></TD>
	  <TD width=13 height=25>&nbsp;</TD>
	  <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
	<TR>
	  <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
	  <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
	  <TD colSpan=3 height=25>&nbsp;</TD>
	  <TD width=397 height=25 colspan="2"><INPUT type=radio value=1 name="unip_pppoe_demand" <% unip_pppoe_config("demand", 1); %> onclick=unip_pppoe_enable_disable(this.form,1) ><font color=<% nvram_else_match("aol_block_traffic1","1","gray","black"); %>><script>Capture(setother.condemand)</script>: <script>Capture(setother.maxidle)</script>&nbsp;</font>
	<INPUT class=num maxLength=4 size=4 value='<% unip_pppoe_config("idletime", 0); %>' name="unip_pppoe_idletime" onBlur=valid_range(this,1,9999,"Idle%20time") <% unip_pppoe_config("idletime", 1); %>>&nbsp;<font color=<% nvram_else_match("aol_block_traffic1","1","gray","black"); %>>Min.</font></TD>
          </TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
       <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=397 height=25 colspan="2"><INPUT type=radio value=0 name="unip_pppoe_demand" <% unip_pppoe_config("demand", 0); %> onclick=unip_pppoe_enable_disable(this.form,0)><font color=<% nvram_else_match("aol_block_traffic1","1","gray","black"); %>><script>Capture(setother.keepalive)</script>: <script>Capture(setother.radial)</script>&nbsp;</font>
	<INPUT class=num maxLength=4 size=4 value='<% unip_pppoe_config("redialperiod", 0); %>' name="unip_pppoe_redialperiod" onBlur=valid_range(this,20,180,"Redial%20period") <% unip_pppoe_config("redialperiod", 1); %>>&nbsp;<font color=<% nvram_else_match("aol_block_traffic1","1","gray","black"); %>><script>Capture(share.sec)</script>.</font></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

