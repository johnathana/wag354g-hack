
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

<HTML><HEAD><TITLE>Filter Summary Table</TITLE>
<% no_cache(); %>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<script src="common.js"></script>
<SCRIPT language="Javascript" type="text/javascript" src="share.js"></SCRIPT>
<SCRIPT language="Javascript" type="text/javascript" src="capaccess.js"></SCRIPT>
<SCRIPT language=JavaScript>
//document.title=(accinbsum.filtersumtable);
document.title=(accinetplcsum.inetplcsum);
function filter_del(F)
{
       F.submit_button.value="FilterSummary";
       F.change_action.value="gozila_cgi";
       F.submit_type.value="delete";
       F.submit();
}
function init()
{
	window.focus();
}
</SCRIPT>
</HEAD>
<BODY bgColor=white onload=init() onUnload="top.opener.window.location.href='Filters.asp'">
<form method=<% get_http_method(); %> action=apply.cgi>
<input type=hidden name=submit_button>
<input type=hidden name=submit_type>
<input type=hidden name=change_action>
<center><table border=0 cellspacing=1>
  <tr align=middle bgcolor=b3b3b3>
    <th width="550" colspan="4" bgcolor="#FFFFFF" height="30">
    <p align="left"><font face="Arial" size="4" color="#0000FF"><script>Capture(accinetplcsum.inetplcsum)</script></font></th>
    <th width="70" bgcolor="#FFFFFF" height="30">&nbsp;</th></tr><tr align=middle bgcolor=b3b3b3>
    <th width="50"><font face=Arial size=2><script>Capture(accinetplcsum.num)</script></th>
    <th width="200"><font face=Arial size=2><script>Capture(accinetplcsum.plcname)</script></th>
    <th width="150"><font face="Arial" size="2"><script>Capture(accinetplcsum.days)</script> </font>  </th>
    <th width="150"><font face="Arial" size="2"><script>Capture(accinetplcsum.timeofday)</script></font></th>
    <th width="70"><font face=Arial size=2><!--<input type=button value='Delete'  onClick=filter_del(this.form)>--><script>document.write("<input type=button value='" + share.deletes + "'  onClick=filter_del(this.form)>");</script></th></tr>
<% filter_summary_show(); %>
      <tr bgcolor=cccccc align=middle>
        <td width="50" bgcolor="#FFFFFF">&nbsp;</td>
        <td bgcolor="#FFFFFF" width="200">&nbsp;</td>
	<td bgcolor="#FFFFFF" width="150">&nbsp;</td>
        <td bgcolor="#FFFFFF" width="150">&nbsp;</td>
	<td bgcolor="#FFFFFF" width="70"><!-- <INPUT onclick=self.close() type=reset value=Close>--><script>document.write("<INPUT onclick=self.close() type=reset value='" + share.closes + "'>");</script></td>
</table></center></form></BODY></HTML>
