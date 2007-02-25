
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

<TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</TD>
          <TD width=101 height=25><script>Capture(wlother.defaultkey)</script>:</TD>
          <TD width=296 height=25><FONT face=Arial size=2><B>&nbsp;<INPUT type=radio value=1 name=wl_key <% nvram_match("wl_key","1","checked"); %>> 1&nbsp;<input type=hidden name=wl_WEP_key><input type=hidden name=wl_wep value=restricted>
			<INPUT type=radio value=2 name=wl_key <% nvram_match("wl_key","2","checked"); %>> 2&nbsp; 
			<INPUT type=radio value=3 name=wl_key <% nvram_match("wl_key","3","checked"); %>> 3&nbsp; 
			<INPUT type=radio value=4 name=wl_key <% nvram_match("wl_key","4","checked"); %>> 4&nbsp; </B></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</TD>
          <TD width=101 height=25><script>Capture(wlother.wepencryp)</script>:</TD>
          <TD width=296 height=25>&nbsp;<SELECT name=wl_wep_bit size=1 onChange=keyMode(this.form.wl_wep_bit.selectedIndex)> 
			<!--<OPTION value=64 <% nvram_selmatch("wl_wep_bit","64","selected"); %>>64 bits 10 hex digits</OPTION>-->
			<OPTION value=64 <% nvram_selmatch("wl_wep_bit","64","selected"); %>>64 <script>Capture(wlnetwork.bit)</script> (10 <script>Capture(wlnetwork.hexdigits)</script>)</OPTION> 
			<!--<OPTION value=128 <% nvram_selmatch("wl_wep_bit","128","selected"); %>>128 bits 26 hex digits&nbsp;</OPTION>-->
			<OPTION value=128 <% nvram_selmatch("wl_wep_bit","128","selected"); %>>128 <script>Capture(wlnetwork.bit)</script> (26 <script>Capture(wlnetwork.hexdigits)</script>)</OPTION> 
		</SELECT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</TD>
          <TD width=101 height=25><script>Capture(wlother.passphrase)</script>:</TD>
          <TD width=296 height=25>&nbsp;<INPUT maxLength=16 name=wl_passphrase size=20 value='<% get_wep_value("passphrase"); %>'>&nbsp;
			<INPUT type=hidden value=Null name=generateButton>
		<!--	<INPUT type=button value='Generate' onclick=generateKey(this.form) name=wepGenerate>--><script>document.write("<INPUT type=button value='" + share.generate + "' onclick=generateKey(this.form) name=wepGenerate>");</script></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
<TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</TD>
          <TD width=101 height=25><script>Capture(share.key)</script> 1:</TD>
          <TD width=296 height=25>&nbsp;<INPUT size=36 name=wl_key1 value='<% get_wep_value("key1"); %>'></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>          
<TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</TD>
          <TD width=101 height=25><script>Capture(share.key)</script> 2:</TD>
          <TD width=296 height=25>&nbsp;<INPUT size=36 name=wl_key2 value='<% get_wep_value("key2"); %>'></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>          
<TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</TD>
          <TD width=101 height=25><script>Capture(share.key)</script> 3:</TD>
          <TD width=296 height=25>&nbsp;<INPUT size=36 name=wl_key3 value='<% get_wep_value("key3"); %>'></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>          
<TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</TD>
          <TD width=101 height=25><script>Capture(share.key)</script> 4:</TD>
          <TD width=296 height=25>&nbsp;<INPUT size=36 name=wl_key4 value='<% get_wep_value("key4"); %>'></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
