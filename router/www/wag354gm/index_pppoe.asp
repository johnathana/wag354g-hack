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
<input type=hidden name=pppoe_config>
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
	  <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=1><B><script>Capture(setother.pppoesetting)</script></B></TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;<script>Capture(setother.servicename)</script>:</TD>
          <TD width=296 height=25><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" maxLength=63 size=20 name="pppoe_servicename" value='<% pppoe_config("servicename", 0); %>' onBlur=valid_name(this,"Service%20Name",SPACE_NO)></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

	<TR>
	  <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=1></TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;<script>Capture(setddns.username)</script>:</TD>
          <TD width=296 height=25><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" maxLength=63 size=20 name="pppoe_username" value='<% pppoe_config("username", 0); %>' onBlur=valid_name(this,"User%20Name",SPACE_NO)></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;<script>Capture(setddns.password)</script>:&nbsp;&nbsp;</TD>
	  <TD width=296 height=25><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" maxLength=63 size=20 name="pppoe_password" type=password value='<% pppoe_config("password", 0); %>' onBlur=valid_name(this,"Password",SPACE_NO)></TD>
	  <TD width=13 height=25>&nbsp;</TD>
	  <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
	<TR>
	  <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
	  <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
	  <TD colSpan=3 height=25>&nbsp;</TD>
	  <TD width=397 height=25 colspan="2"><INPUT type=radio value=1 name="pppoe_demand" <% pppoe_config("demand", 1); %> onclick=pppoe_enable_disable(this.form,1) ><font color=<% nvram_else_match("aol_block_traffic1","1","gray","black"); %>><script>Capture(setother.condemand)</script>: <script>Capture(setother.maxidle)</script>&nbsp;</font>
	<INPUT class=num maxLength=4 size=4 value='<% pppoe_config("idletime", 0); %>' name="pppoe_idletime" onBlur=valid_range(this,1,9999,"Idle%20time") <% pppoe_config("idletime", 1); %>>&nbsp;<font color=<% nvram_else_match("aol_block_traffic1","1","gray","black"); %>><script>Capture(share.mins)</script></font></TD>
          </TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
       <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=397 height=25 colspan="2"><INPUT type=radio value=0 name="pppoe_demand" <% pppoe_config("demand", 0); %> onclick=pppoe_enable_disable(this.form,0)><font color=<% nvram_else_match("aol_block_traffic1","1","gray","black"); %>><script>Capture(setother.keepalive)</script>: <script>Capture(setother.radial)</script>&nbsp;</font>
	<INPUT class=num maxLength=4 size=4 value='<% pppoe_config("redialperiod", 0); %>' name="pppoe_redialperiod" onBlur=valid_range(this,20,180,"Redial%20period") <% pppoe_config("redialperiod", 1); %>>&nbsp;<font color=<% nvram_else_match("aol_block_traffic1","1","gray","black"); %>><script>Capture(share.sec)</script>.</font></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

<% support_elsematch("MPPPOE_SUPPORT", "1", "", "<!--"); %>     	
	<input type=hidden name=mpppoe_config>
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
          <TD width=101 height=25>&nbsp;<script>Capture(setother.secondpppoe)</script>:</TD>
          <TD width=296 height=25>
		<select name="mpppoe_enable" onChange=SelPPP(this.form.mpppoe_enable.selectedIndex,this.form)>
                        <option value="0" <% mpppoe_enable_setting("disable", 0); %>><b><script>Capture(share.disabled)</script></b></option>
                        <option value="1" <% mpppoe_enable_setting("enable", 1); %>><b><script>Capture(share.enabled)</script>&nbsp;</b></option>
                </select></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

<% mpppoe_enable_setting("disable", 2); %>
	<TR>
	  <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;<script>Capture(setother.servicename)</script>:</TD>
          <TD width=296 height=25><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" maxLength=63 size=20 name="mpppoe_servicename" value='<% mpppoe_config("servicename", 0); %>' onBlur=valid_name(this,"MPPPoE%20Service%20Name",SPACE_NO)></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

	<TR>
	  <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;<script>Capture(setddns.username)</script>:</TD>
          <TD width=296 height=25><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" maxLength=63 size=20 name="mpppoe_username" value='<% mpppoe_config("username", 0); %>' onBlur=valid_name(this,"MPPPoE%20User%20Name",SPACE_NO)></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;<script>Capture(setddns.password)</script>:&nbsp;&nbsp;</TD>
	  <TD width=296 height=25><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" maxLength=63 size=20 name="mpppoe_password" type=password value='<% mpppoe_config("password", 0); %>' onBlur=valid_name(this,"MPPPoE%20Password",SPACE_NO)></TD>
	  <TD width=13 height=25>&nbsp;</TD>
	  <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;<script>Capture(setother.matchs)</script>:&nbsp;&nbsp;</TD>
	  <TD width=296 height=25><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" maxLength=63 size=20 name="mpppoe_domainname" value='<% mpppoe_config("domainname", 0); %>' onBlur=valid_name(this,"MPPPoE%20Domain%20Name",SPACE_NO)></TD>
	  <TD width=13 height=25>&nbsp;</TD>
	  <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

	<TR>
	  <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
	  <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
	  <TD colSpan=3 height=25>&nbsp;</TD>
	  <TD width=397 height=25 colspan="2"><INPUT type=radio value=1 name="mpppoe_demand" <% mpppoe_config("demand", 1); %> onclick=mpppoe_enable_disable(this.form,1) ><font color=<% nvram_else_match("aol_block_traffic1","1","gray","black"); %>><script>Capture(setother.condemand)</script>: <script>Capture(setother.maxidle)</script>&nbsp;</font>
	<INPUT class=num maxLength=4 size=4 value='<% mpppoe_config("idletime", 0); %>' name="mpppoe_idletime" onBlur=valid_range(this,1,9999,"MPPPoE%20Idle%20time") <% mpppoe_config("idletime", 1); %>>&nbsp;<font color=<% nvram_else_match("aol_block_traffic1","1","gray","black"); %>><script>Capture(share.mins)</script></font></TD>
          </TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
       <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=397 height=25 colspan="2"><INPUT type=radio value=0 name="mpppoe_demand" <% mpppoe_config("demand", 0); %> onclick=mpppoe_enable_disable(this.form,0)><font color=<% nvram_else_match("aol_block_traffic1","1","gray","black"); %>><script>Capture(setother.keepalive)</script>: <script>Capture(setother.radial)</script>&nbsp;</font>
	<INPUT class=num maxLength=4 size=4 value='<% mpppoe_config("redialperiod", 0); %>' name="mpppoe_redialperiod" onBlur=valid_range(this,20,180,"MPPPoE%20Redial%20period") <% mpppoe_config("redialperiod", 1); %>>&nbsp;<font color=<% nvram_else_match("aol_block_traffic1","1","gray","black"); %>>Sec.</font></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
<% mpppoe_enable_setting("disable", 3); %>
<% support_elsematch("MPPPOE_SUPPORT", "1", "", "-->"); %>     	

