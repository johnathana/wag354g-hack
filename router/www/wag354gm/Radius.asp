
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
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;<script>Capture(wlother.rsaddr)</script>:</TD>
          <TD width=296 height=25><INPUT type=hidden name=wl_radius_ipaddr value=4>&nbsp;<INPUT size=3 maxlength=3 name=wl_radius_ipaddr_0 value='<% get_single_ip("wl_radius_ipaddr","0"); %>' onBlur=valid_range(this,0,255,"IP") class=num> .
                  <INPUT size=3 maxlength=3 name=wl_radius_ipaddr_1 value='<% get_single_ip("wl_radius_ipaddr","1"); %>' onBlur=valid_range(this,0,255,"IP") class=num> .
                  <INPUT size=3 maxlength=3 name=wl_radius_ipaddr_2 value='<% get_single_ip("wl_radius_ipaddr","2"); %>' onBlur=valid_range(this,0,255,"IP") class=num> .
                  <INPUT size=3 maxlength=3 name=wl_radius_ipaddr_3 value='<% get_single_ip("wl_radius_ipaddr","3"); %>' onBlur=valid_range(this,1,254,"IP") class=num></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;<script>Capture(wlother.rport)</script>:</TD>
          <TD width=296 height=25>&nbsp;<INPUT size=3 name=wl_radius_port value='<% nvram_get("wl_radius_port"); %>' maxlength=5 onBlur=valid_range(this,1,65535,"Port")></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;<script>Capture(wlother.sharedkey)</script>:</TD>
          <TD width=296 height=25>&nbsp;<INPUT size=20 name=wl_radius_key value='<% nvram_get("wl_radius_key"); %>' maxlength=79>
</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
