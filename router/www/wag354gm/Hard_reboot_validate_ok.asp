
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

<html>
<head>
<% no_cache(); %>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->


<script src="common.js"></script>
<script language="JavaScript" type="text/javascript" src="share.js"></script>

<SCRIPT language=JavaScript>
var submit_button = '<% get_web_page_name(); %>';
function to_submit()
{
	if(submit_button == "")
		history.go(-1);
	else if(submit_button == "WL_WEPTable.asp")
		self.close();
	else
		//document.location.href =  submit_button;
		self.close();
	
}
</SCRIPT>
</head>
<body bgcolor="black">
<form>
<center><table BORDER=0 CELLSPACING=0 CELLPADDING=0 WIDTH=340 height=230 >
<tr BGCOLOR="white">
<th HEIGHT=200><font face="Verdana" size=4 color="black">Hard Rebooting Now.<!--script>Capture(share.hreboot)</script--></font>
<p><p>
<input type="button" name="action" value="Close" OnClick=to_submit()><!--script>document.write("<input type=button name=action value=" + share.closes + " onClick=to_submit()>");</script-->
</th>
</tr>
</table></center>
</form>
</body>

