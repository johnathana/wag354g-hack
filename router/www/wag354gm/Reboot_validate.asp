
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

<HTML><HEAD>
<meta http-equiv="Content-Language" content="zh-cn">
<TITLE>Credentials Validate</TITLE>
<% no_cache(); %>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<link rel="stylesheet" type="text/css" href="style.css">
<script src="common.js"></script>
<SCRIPT language="Javascript" type="text/javascript" src="share.js"></SCRIPT>
<SCRIPT language="Javascript" type="text/javascript" src="capadmin.js"></SCRIPT>
<script language="JavaScript">
document.title=(admtopmenu.credval);
function to_submit(F)
{

		if(F.reboot_username.value=="")
		{
			//alert("Please input username!");
			alert(errmsg.err108);
			F.reboot_username.focus();
			return false;
		}
		if(F.reboot_password.value=="")
		{
			//alert("Please input password!");
			alert(errmsg.err109);
			F.reboot_password.focus();
			return false;
		}
	
	   if(window.name=='Hardreboot')
       	    F.submit_type.value="HRebootvalidate";
       	else if(window.name=='Softreboot')
       	    F.submit_type.value="SRebootvalidate";
       	
	F.submit_button.value="Reboot_validate";

       F.action.value='Rebootvalidate';
       F.submit();
}
function init()
{
	var width=screen.availWidth/2-190;
	var height=screen.availHeight/2-130;
	window.moveTo(width,height);
	window.focus();
}
</script>
</HEAD>
<BODY bgColor=white onload=init()>
<form name=default method=<% get_http_method(); %> action=apply.cgi>
<CENTER>
<input type=hidden name=submit_button>
<input type=hidden name=action>
<input type=hidden name=submit_type>

&nbsp;<p><FONT style="FONT-SIZE: 8pt"><script>Capture(admreboot.gwusername)</script>:&nbsp;&nbsp; </FONT><INPUT name="reboot_username" style="FONT-SIZE: 10pt; FONT-FAMILY: Arial"  maxLength=63 size=20 value=""  onBlur=valid_name(this,"User%20Name")>
</p>
<p><FONT style="FONT-SIZE: 8pt"><script>Capture(admreboot.gwpassword)</script>:&nbsp;&nbsp; </FONT><INPUT name="reboot_password" style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" type=password maxLength=63 size=20 value=""  onBlur=valid_name(this,"Password",SPACE_NO)>
</p>
<p>&nbsp;</p>
<p><!--<input type="button" value="Submit" name="sub_button" onclick=to_submit(this.form)>--><script>document.write("<input type=button value=" + share.submit + " name=sub_button Onclick=to_submit(this.form)>");</script>
&nbsp;&nbsp;&nbsp;
<!--<input type="reset" value="Cancel" name="can_button">--><script>document.write("<input type=reset value=" + share.cance + " name=can_button>");</script>&nbsp;&nbsp;&nbsp;
<!--<input type="button" value="Close" name="close_buttong" onclick="self.close()">--><script>document.write("<input type=button value=" + share.closes + " name=close_buttong onclick=self.close()>");</script></p>

</CENTER>
</form>
</BODY>
</HTML>
