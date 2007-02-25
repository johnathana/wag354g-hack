
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

<HTML><HEAD><TITLE>Port Triggering</TITLE>
<% no_cache(); %>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<link rel="stylesheet" type="text/css" href="style.css">
<style fprolloverstyle>
A:hover {color: #00FFFF}
.small A:hover {color: #00FFFF}
</style>
<script src="common.js"></script>
<SCRIPT language="javascript" type="text/javascript" src="share.js"></SCRIPT>
<SCRIPT language="javascript" type="text/javascript" src="capapp.js"></SCRIPT>
<SCRIPT language=JavaScript>
document.title=(apptopmenu.porttrigger);
<% wireless_support(5); %>share.productname = share.productname1;<% wireless_support(6); %>
function to_submit(F)
{
	F.submit_button.value = "Triggering";
	F.action.value = "Apply";
       	F.submit();
}
</SCRIPT>
</HEAD>
<BODY>
<DIV align=center>
<FORM name=trigger action=apply.cgi method=<% get_http_method(); %>>
<input type=hidden name=submit_button>
<input type=hidden name=change_action>
<input type=hidden name=submit_type>
<input type=hidden name=action>
<input type=hidden name=port_trigger value="10">
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
      style="FONT-SIZE: 15pt" face=Arial color=#ffffff><script>Capture(bmenu.application)</script> <BR>&amp; 
      <script>Capture(bmenu.gaming)</script></FONT></H3></TD>
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
                <a style="TEXT-DECORATION: none" href="Wireless_Basic.asp"><script>Capture(bmenu.wireless)</script></a></FONT></TD><% wireless_support(2); %>
                <TD align=middle bgColor=#000000 height=20><FONT 
                  style="FONT-WEIGHT: 700" color=#ffffff>
                <a style="TEXT-DECORATION: none" href="Firewall.asp"><script>Capture(bmenu.security)</script></a></FONT></TD>
                <TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><A style="TEXT-DECORATION: none" href="<% support_elsematch("PARENTAL_CONTROL_SUPPORT", "1", "Parental_Control.asp", "Filters.asp"); %>"><script>Capture(bmenu.accrestriction)</script></A></FONT></TD>
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
              </TR>              <TR>
                <TD width=643 bgColor=#6666cc colSpan=7 height=21>
                  <TABLE borderColor=black height=21 cellSpacing=0 cellPadding=0>
                    <TBODY>
                    <TR align=center>
			<TD width=230 class=small><A href="Forward_UPnP.asp"><script>Capture(apptopmenu.singleport)</script></A></TD>
			<TD><P class=bar><font color='white'><b>|</b></font></P></TD>
			<TD width=250 class=small><A href="Forward.asp"><script>Capture(apptopmenu.portrange)</script></A></TD>
			<TD><P class=bar><font color='white'><b>|</b></font></P></TD>
			<TD width=200 class=small><FONT style="COLOR: white"><script>Capture(apptopmenu.porttrigger)</script></A></TD>
			<TD><P class=bar><font color='white'><b>|</b></font></P></TD>
			<TD width=120 class=small><A href="DMZ.asp"><script>Capture(apptopmenu.dmz)</script></a></TD>
			<TD><P class=bar><font color='white'><b>|</b></font></P></TD>
			<TD width=120 class=small><A href="qos.asp"><script>Capture(apptopmenu.qos)</script></a></TD>
                    </TR>
                    </TBODY>
                  </TABLE>
                </TD>
              </TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>
<TABLE height=5 cellSpacing=0 cellPadding=0 width=806 bgColor=black border=0>
  <TBODY>
  <TR bgColor=black>
    <TD style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" borderColor=#e7e7e7 width=163 bgColor=#e7e7e7 height=1><IMG height=15 src="image/UI_03.gif" width=164 border=0></TD>        
    <TD style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
    height=1><IMG height=15 src="image/UI_02.gif" width=645 border=0></TD>
  </TR>
</TBODY></TABLE>
<TABLE style="BORDER-COLLAPSE: collapse" borderColor=#111111 height=23 cellSpacing=0 cellPadding=0 width=809 border=0>
  <TBODY>
  <TR>
    <TD>
      <TABLE cellSpacing=0 cellPadding=0 width=633 border=0>
        <TBODY>
        <TR>
          <TD align=right width=156 bgColor=#000000 colSpan=3 height=25><B><FONT style="FONT-SIZE: 9pt" face=Arial color=#ffffff><script>Capture(appleftmenu.porttrigger)</script></FONT></B></TD>
          <TD width=8 bgColor=#000000 height=25></TD>
          <TD colSpan=6 height=25></TD>
          <TD width=15 background=image/UI_05.gif></TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif></TD>
          <TD width=13 height=25></TD>
          <TD align=middle width=408 colSpan=4 height=51>
            <TABLE id=AutoNumber16 style="BORDER-COLLAPSE: collapse" 
            borderColor=#e7e7e7 height=24 cellSpacing=0 cellPadding=0 width=428 border=0>
              <TBODY>
              <TR>
                <TD 
                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
                align=middle width=79 bgColor=#CCCCCC height=30></TD>
                <TD style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
                align=middle bgColor=#CCCCCC colSpan=2 height=30><B><script>Capture(appporttrigger.triggerrange)</script></B></TD>
                <TD style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
                align=middle bgColor=#CCCCCC colSpan=2 height=30><B><script>Capture(appporttrigger.forwardrange)</script></B></TD>
              	<TD 
                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
                align=middle width=33 bgColor=#CCCCCC height=30></TD></TR>
              <TR>
                <TD 
                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
                align=middle bgColor=#CCCCCC  height=30><B><script>Capture(appporttrigger.app)</script></B></TD>
                <TD 
                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
                align=middle width=65 bgColor=#CCCCCC height=30><B><script>Capture(appporttrigger.startport)</script></B></TD>
                <TD 
                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
                align=middle width=55 bgColor=#CCCCCC height=30><B><script>Capture(appporttrigger.endport)</script></B></TD>
                <TD 
                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
                align=middle width=65 bgColor=#CCCCCC height=30><B><script>Capture(appporttrigger.startport)</script></B></TD>
                <TD 
                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
                align=middle width=55 bgColor=#CCCCCC height=30><B><script>Capture(appporttrigger.endport)</script></B></TD>
                <TD 
                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 
                align=middle width=10 bgColor=#CCCCCC height=30><B><script>Capture(appporttrigger.enable)</script> 
                </B></TD>
              </TR>
              
              <TR>
                <TD align=middle height=30><FONT size=2>
				<INPUT class=num maxLength=12 size=16 name=name0 onBlur=valid_name(this,"Name") value='<% port_trigger_table("name","0"); %>'></FONT></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_from0 value='<% port_trigger_table("i_from","0"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_to0 value='<% port_trigger_table("i_to","0"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_from0 value='<% port_trigger_table("o_from","0"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_to0 value='<% port_trigger_table("o_to","0"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                <TD align=middle height=30>
                <INPUT type=checkbox name=enable0 value=on <% port_trigger_table("enable","0"); %>></TD>    
              </TR>
              <TR>
                <TD align=middle height=30><FONT size=2>
				<INPUT class=num maxLength=12 size=16 name=name1 onBlur=valid_name(this,"Name") value='<% port_trigger_table("name","1"); %>'></FONT></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_from1 value='<% port_trigger_table("i_from","1"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_to1 value='<% port_trigger_table("i_to","1"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_from1 value='<% port_trigger_table("o_from","1"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_to1 value='<% port_trigger_table("o_to","1"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                <TD align=middle height=30>
                <INPUT type=checkbox name=enable1 value=on <% port_trigger_table("enable","1"); %>></TD>    
              </TR>
              <TR>
                <TD align=middle height=30><FONT size=2>
				<INPUT class=num maxLength=12 size=16 name=name2 onBlur=valid_name(this,"Name") value='<% port_trigger_table("name","2"); %>'></FONT></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_from2 value='<% port_trigger_table("i_from","2"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_to2 value='<% port_trigger_table("i_to","2"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_from2 value='<% port_trigger_table("o_from","2"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_to2 value='<% port_trigger_table("o_to","2"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                <TD align=middle height=30>
                <INPUT type=checkbox name=enable2 value=on <% port_trigger_table("enable","2"); %>></TD>    
              </TR>
              <TR>
                <TD align=middle height=30><FONT size=2>
				<INPUT class=num maxLength=12 size=16 name=name3 onBlur=valid_name(this,"Name") value='<% port_trigger_table("name","3"); %>'></FONT></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_from3 value='<% port_trigger_table("i_from","3"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_to3 value='<% port_trigger_table("i_to","3"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_from3 value='<% port_trigger_table("o_from","3"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_to3 value='<% port_trigger_table("o_to","3"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                <TD align=middle height=30>
                <INPUT type=checkbox name=enable3 value=on <% port_trigger_table("enable","3"); %>></TD>    
              </TR>
              <TR>
                <TD align=middle height=30><FONT size=2>
				<INPUT class=num maxLength=12 size=16 name=name4 onBlur=valid_name(this,"Name") value='<% port_trigger_table("name","4"); %>'></FONT></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_from4 value='<% port_trigger_table("i_from","4"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_to4 value='<% port_trigger_table("i_to","4"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_from4 value='<% port_trigger_table("o_from","4"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_to4 value='<% port_trigger_table("o_to","4"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                <TD align=middle height=30>
                <INPUT type=checkbox name=enable4 value=on <% port_trigger_table("enable","4"); %>></TD>    
              </TR>
              <TR>
                <TD align=middle height=30><FONT size=2>
				<INPUT class=num maxLength=12 size=16 name=name5 onBlur=valid_name(this,"Name") value='<% port_trigger_table("name","5"); %>'></FONT></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_from5 value='<% port_trigger_table("i_from","5"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_to5 value='<% port_trigger_table("i_to","5"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_from5 value='<% port_trigger_table("o_from","5"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_to5 value='<% port_trigger_table("o_to","5"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                <TD align=middle height=30>
                <INPUT type=checkbox name=enable5 value=on <% port_trigger_table("enable","5"); %>></TD>    
              </TR>
              <TR>
                <TD align=middle height=30><FONT size=2>
				<INPUT class=num maxLength=12 size=16 name=name6 onBlur=valid_name(this,"Name") value='<% port_trigger_table("name","6"); %>'></FONT></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_from6 value='<% port_trigger_table("i_from","6"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_to6 value='<% port_trigger_table("i_to","6"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_from6 value='<% port_trigger_table("o_from","6"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_to6 value='<% port_trigger_table("o_to","6"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                <TD align=middle height=30>
                <INPUT type=checkbox name=enable6 value=on <% port_trigger_table("enable","6"); %>></TD>    
              </TR>
              <TR>
                <TD align=middle height=30><FONT size=2>
				<INPUT class=num maxLength=12 size=16 name=name7 onBlur=valid_name(this,"Name") value='<% port_trigger_table("name","7"); %>'></FONT></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_from7 value='<% port_trigger_table("i_from","7"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_to7 value='<% port_trigger_table("i_to","7"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_from7 value='<% port_trigger_table("o_from","7"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_to7 value='<% port_trigger_table("o_to","7"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                <TD align=middle height=30>
                <INPUT type=checkbox name=enable7 value=on <% port_trigger_table("enable","7"); %>></TD>    
              </TR>
              <TR>
                <TD align=middle height=30><FONT size=2>
				<INPUT class=num maxLength=12 size=16 name=name8 onBlur=valid_name(this,"Name") value='<% port_trigger_table("name","8"); %>'></FONT></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_from8 value='<% port_trigger_table("i_from","8"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_to8 value='<% port_trigger_table("i_to","8"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_from8 value='<% port_trigger_table("o_from","8"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_to8 value='<% port_trigger_table("o_to","8"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                <TD align=middle height=30>
                <INPUT type=checkbox name=enable8 value=on <% port_trigger_table("enable","8"); %>></TD>    
              </TR>
              <TR>
                <TD align=middle height=30><FONT size=2>
				<INPUT class=num maxLength=12 size=16 name=name9 onBlur=valid_name(this,"Name") value='<% port_trigger_table("name","9"); %>'></FONT></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_from9 value='<% port_trigger_table("i_from","9"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=i_to9 value='<% port_trigger_table("i_to","9"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_from9 value='<% port_trigger_table("o_from","9"); %>' onBlur=valid_range(this,0,65535,"Port")>&nbsp;<script>Capture(appporttrigger.to)</script></TD>
                <TD align=middle height=30><INPUT class=num maxLength=5 size=5 name=o_to9 value='<% port_trigger_table("o_to","9"); %>' onBlur=valid_range(this,0,65535,"Port")></TD>
                <TD align=middle height=30>
                <INPUT type=checkbox name=enable9 value=on <% port_trigger_table("enable","9"); %>></TD>    
              </TR>
              </TBODY></TABLE></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif>&nbsp;</TD></TR></TBODY></TABLE></TD>
    <TD vAlign=top width=176 bgColor=#6666cc>
    <p style="line-height: 200%"><font color="#FFFFFF"><span >&nbsp;&nbsp;<a target="_blank" href="help/HPortTriggering.asp">
    <script>Capture(share.more)</script>...</a></span></font></TD></TR>

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
                <TD align=middle bgColor=#434a8f width="111"><FONT color=#ffffff><B><A href='Triggering.asp'><script>Capture(share.cancels)</script></A></B></FONT></TD>
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
