<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<!-- saved from url=(0039)http://192.168.1.1/help/HManagement.asp -->
<HTML><HEAD><TITLE>Security Help</TITLE>
<META http-equiv=Content-Type content="text/html; charset=windows-1252">
<STYLE>P.MsoNormal {
	FONT-SIZE: 12pt; FONT-FAMILY: "Times New Roman"; mso-style-parent: ""; margin-left:0cm; margin-right:0cm; margin-top:0cm; margin-bottom:0pt
}
TABLE.MsoNormalTable {
	FONT-SIZE: 10pt; FONT-FAMILY: "Times New Roman"; mso-style-parent: ""
}
LI.MsoNormal {
	FONT-SIZE: 12pt; FONT-FAMILY: "Times New Roman"; mso-style-parent: ""; margin-left:0cm; margin-right:0cm; margin-top:0cm; margin-bottom:0pt
}
</STYLE>

<META content="Microsoft FrontPage 4.0" name=GENERATOR></HEAD>
<BODY bgColor=white>
<FORM>
<table border="0" width="100%">
  <tr>
    <td width="75%"><FONT color=#3333ff><B><SPAN lang=EN-US 
style="FONT-SIZE: 16pt; FONT-FAMILY: Arial">Management</SPAN><SPAN 
style="FONT-SIZE: 16pt; FONT-FAMILY: Arial">&nbsp;</SPAN></B></FONT></td>
    <td width="25%">
      <p align="right"><FONT color=#3333ff><B><SPAN 
style="FONT-SIZE: 16pt; FONT-FAMILY: Arial"> 
<A href="javascript:print();"><IMG height=44 
src="../image/Printer.gif" width=82 align=center 
border=0></A></SPAN></B></FONT></td>
  </tr>
</table>
<P class=MsoNormal>&#12288;</P>
<P class=MsoNormal><SPAN lang=EN-US style="FONT-FAMILY: Arial">The 
<I>Management</I> screen allows you to change the Gateway's security settings. It 
is strongly recommended that you change the factory default password of the 
Gateway, which is <B>admin</B>. All users who try to access the Gateway's 
web-based utility or Setup Wizard will be prompted for the Gateway's 
password.</SPAN></P>
<P class=MsoNormal>&#12288;</P>
<table class=MsoNormalTable style="border-collapse: collapse; border-style: none; border-width: medium" cellSpacing=0 cellPadding=0 border="1" width="100%">
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top width="20%"><b><span style="font-family: Arial"><font size="3">Gateway Username</font></span></b></td>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top width="70%"><SPAN lang=EN-US style="FONT-FAMILY: Arial"><font size="3">The new 
      Username must not exceed 63 characters in length.</font> </SPAN></td>
  </tr>
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><b><font size="3"><span style="font-family: Arial">Gateway</span><SPAN lang=EN-US style="FONT-FAMILY: Arial"> 
      Password</SPAN></font></b></td>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><SPAN lang=EN-US style="FONT-FAMILY: Arial"><font size="3">The new 
      Password must not exceed 30 characters in length and must not include any 
      spaces. Enter the new Password a second time to confirm 
  it.</font></SPAN></td>
  </tr>
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><B><SPAN lang=EN-US style="FONT-FAMILY: Arial"><font size="3">Remote 
      Management</font></SPAN></B></td>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top>
      <P class=MsoNormal><SPAN lang=EN-US style="FONT-FAMILY: Arial"><font size="3">This 
      feature allows you to manage your Gateway from a remote location, via the 
      Internet.&nbsp; To disable this feature, keep the default setting, 
      <B>Disable</B>. To enable this feature, select<B> Enable</B>, and use the 
      specified port (default is <B><% remote_management_config("http_wanport", 0); %></B>) on your PC to remotely manage the 
      Gateway. You must also change the Gateway's default password to one of your 
      own, if you haven't already. A unique password will increase 
      security.</font></SPAN></P>
      <P class=MsoNormal><font size="3">&nbsp;</font></P>
      <P class=MsoNormal><SPAN lang=EN-US style="FONT-FAMILY: Arial"><font size="3">To remotely 
      manage the Gateway, enter <B>http://xxx.xxx.xxx.xxx:<% remote_management_config("http_wanport", 0); %></B> (the x's 
      represent the Gateway's Internet IP address, and <% remote_management_config("http_wanport", 0); %> represents the 
      specified port) in your web browser's <I>Address</I> field. You will be 
      asked for the Gateway's password. After successfully entering the password, 
      you will be able to access the Gateway's web-based utility.</font></SPAN></P>
      <P class=MsoNormal><font size="3">&nbsp;</font></P>
      <P class=MsoNormal><SPAN lang=EN-US style="FONT-FAMILY: Arial"><font size="3">Note: If 
      the Remote Management feature is enabled, anyone who knows the Gateway's 
      Internet IP address and password will be able to alter the Gateway's 
      settings.</font></SPAN></P>&nbsp;</td>
  </tr>
</table>
<P class=MsoNormal><SPAN lang=EN-US style="FONT-FAMILY: Arial"></SPAN>&#12288;</P>
<P class=MsoNormal><b><font face="Arial">SNMP</font></b></P>
<P class=MsoNormal>&#12288;</P>
<P class=MsoNormal><font face="Arial">SNMP is a popular network monitoring and 
management protocol.&nbsp; To enable SNMP identification, click <b>Enabled.</b>&nbsp; 
To disable SNMP, click <b>Disabled.</b></font></P>
<P class=MsoNormal>&#12288;</P>
<table class=MsoNormalTable style="border-collapse: collapse; border-style: none; border-width: medium" cellSpacing=0 cellPadding=0 border="1" width="100%">
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top width="20%"><b><span style="font-family: Arial"><font size="3">Device Name</font></span></b></td>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top width="70%"><SPAN lang=EN-US style="FONT-FAMILY: Arial"><font size="3">You may 
      enter a new Device Name to simply your network management.&nbsp; The new 
      Device Name must not exceed 39 characters in length.</font> </SPAN></td>
  </tr>
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><b><span style="font-family: Arial"><font size="3">Get Community</font></span></b></td>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><span style="font-family: Arial"><font size="3">Enter the password 
      that allows read-only access to the Gateway's SNMP information.</font></span></td>
  </tr>
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><b><span style="font-family: Arial"><font size="3">Set Community</font></span></b></td>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><span style="font-family: Arial"><font size="3">Enter the password 
      that allows read/write access to the Gateway's SNMP information.</font></span></td>
  </tr>
</table>
<P class=MsoNormal>&#12288;</P>
<P class=MsoNormal><b><font face="Arial">UPnP</font></b></P>
<P class=MsoNormal>&#12288;</P>
<P class=MsoNormal><font face="Arial">UPnP allows Windows XP to automatically 
configure the Gateway for various Internet applications, such as gaming and 
videoconferencing.&nbsp; To enable UPnP, click <b>Enabled.</b>&nbsp; To disable 
UPnP, click <b>Disabled.</b></font></P>
<P class=MsoNormal>&#12288;</P>
<P class=MsoNormal><SPAN lang=EN-US style="FONT-FAMILY: Arial">Check all the 
values and click <B>Save Settings</B> to save your settings. Click <B>Cancel 
Changes</B> to cancel your unsaved changes.</SPAN></P>
<P class=MsoNormal>&#12288;</P>
<P 
class=MsoNormal> 

<CENTER><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" onclick=self.close() type=button value=Close name=B3></CENTER>
<P></P></FORM></BODY></HTML>
