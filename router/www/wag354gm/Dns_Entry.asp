
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

<HTML><HEAD><TITLE>DNS Entries Table</TITLE>
<% no_cache(); %>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<link rel="stylesheet" type="text/css" href="style.css">
<SCRIPT src="common.js"></SCRIPT>
<script language="JavaScript" type="text/javascript" src="share.js"></script>

<SCRIPT language=javascript>
document.title=(stadnsentrytbl.dnsentrytbl);
function checkIP2(sIPAddress)
{
    var exp=/^(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/;
    var reg = sIPAddress.match(exp);
     
    if(reg==null)
    {
        return false;
    }
    return true;
}


function to_submit(F)
{
	if (checkIP2(F.resolve_ip.value) == false)
	{
		//alert("Invalid IP address");
		alert(errmsg.err73);

		return false;
	}
	if (F.add_domainname.value == "")
	{
		//alert("Invalid DomainName");
		alert(errmsg.err74);

		return false;
	}
	F.submit_button.value="Dns_Entry";
        F.change_action.value="gozila_cgi";
        F.submit_type.value="add_dns_entry";
	choose_disable(F.delete_dns_entry);
	choose_disable(F.add_dns_entry);
        F.submit();

}
function delete_submit(F)
{
	if (checkIP2(F.resolve_ip.value) == false)
	{
		//alert("Invalid IP address");
		alert(errmsg.err73);

		return false;
	}
	if (F.add_domainname.value == "")
	{
		//alert("Invalid DomainName");
		alert(errmsg.err74);

	
		return false;
	}
	F.submit_button.value="Dns_Entry";
        F.change_action.value="gozila_cgi";
        F.submit_type.value="delete_dns_entry";
	choose_disable(F.add_dns_entry);
	choose_disable(F.delete_dns_entry);
        F.submit();
}
function closeWin(F)
{
	
}
function load_init()
{
}
function SelEntry(F, name, ip)
{
	F.add_domainname.value = name.value;
	
	F.resolve_ip.value = ip.value;
}
</SCRIPT>
</HEAD>

<BODY bgColor=white onload=load_init()>
<FORM name=dnsentry method=<% get_http_method(); %> action=apply.cgi>
<input type=hidden name=submit_button>
<input type=hidden name=submit_type>
<input type=hidden name=change_action>
<input type=hidden name=action>

<CENTER>
<TABLE cellSpacing=0 cellPadding=10 width=420 border=1 height="212">
  <TBODY>
  <TR>
    <TD width=396>
      <TABLE height=101 cellSpacing=0 cellPadding=0 border=0 width="398">
        <TBODY>
        <TR>
          <TD height="37">
            <P align=center><b><font face="Arial" size="4" color="#0000FF"><script>Capture(share.dnstable)</script></font></b></P>
		  </TD>
		</TR>
		<tr>
		  <td>
  			<P align=left><script>Capture(share.doname)</script>: <FONT style="FONT-SIZE: 8pt" face=Arial><!--webbot
            bot="Validation" B-Value-Required="TRUE" I-Maximum-Length="62" --><INPUT  maxLength=62 name="add_domainname" size="19"></FONT></P>
		  </td>
		</tr>
		<tr>
		<td>
            <P align=left>DNS <script>Capture(share.ipaddr)</script>:&nbsp;<FONT style="FONT-SIZE: 8pt" face=Arial>
            <!--webbot bot="Validation" B-Value-Required="TRUE"
            I-Maximum-Length="62" --><INPUT  maxLength=62 name="resolve_ip" size="19">
            </FONT></P>		
            <P align=left><FONT style="FONT-SIZE: 8pt" face=Arial>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
            </FONT><INPUT type=button value="Add/Modify" onClick=to_submit(this.form) name="add_dns_entry">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
            <INPUT type=button value="   Delete  " onClick=delete_submit(this.form) name="delete_dns_entry">&nbsp;&nbsp;&nbsp;</P>		
		</td>
		</tr>
		<TABLE style='BORDER-COLLAPSE: collapse' borderColor=silver height=25 cellSpacing=0 cellPadding=0 width=398 border=1>
			<TR>
				<TD class=headrow align=middle height=25 width=132><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 10pt'><script>Capture(share.doname)</script></font></B></TD>
				<TD class=headrow align=middle height=25 width=178><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 10pt'><script>Capture(share.ipaddr)</script></font></B></TD>
				<TD class=headrow align=middle height=25 width=60><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 10pt'><script>Capture(share.selects)</script></font></B></TD>
			</TR> 
			<!--wwzh 
			<TR>
				<TD class=headrow align=middle height=25 width=132><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 10pt'>wwzh.google.com</font></B></TD>
				<TD class=headrow align=middle height=25 width=178><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 10pt'>61.144.56.100</font></B></TD>
				<TD class=headrow align=middle height=25 width=60><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 10pt'><input type=radio value=0 onclick="SelEntry(this.form, "eeee", "ffff")"></font></B></TD>
			</TR> -->
			<% dns_entry_table(""); %> 
	    </TABLE>
    	<p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
        &nbsp;&nbsp; <INPUT type=button value="Close" onClick=self.close()></TBODY></TABLE>
<P>&nbsp; </CENTER></P>
</FORM>
</BODY>

