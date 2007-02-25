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
    <td width="75%"><FONT color=#3333ff><B><SPAN 
style="FONT-SIZE: 16pt; FONT-FAMILY: Arial">Quality of Service (QoS)&nbsp;</SPAN></B></FONT></td>
    <td width="25%">
      <p align="right"><FONT color=#3333ff><B><SPAN 
style="FONT-SIZE: 16pt; FONT-FAMILY: Arial"> 
<A href="javascript:print();">
<IMG height=44 src="../image/Printer.gif" width=82 align=center 
border=0></A></SPAN></B></FONT></td>
  </tr>
</table>
<P class=MsoNormal>&#12288;</P>
<P class=MsoNormal><SPAN lang=EN-US style="FONT-FAMILY: Arial">The <i>QoS</i> screen allows you to 
enable and change the Gateway's QoS settings for upstream traffic.&nbsp; It 
is recommended that you change the QoS settings only if certain real-time 
applications need to have high-priority network traffic.&nbsp;&nbsp; </SPAN></P>
<P class=MsoNormal>&#12288;</P>
<table class=MsoNormalTable style="border-collapse: collapse; border-style: none; border-width: medium" cellSpacing=0 cellPadding=0 border="1" width="100%">
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top width="20%"><b><span style="font-family: Arial"><font size="3">Application</font></span></b></td>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top width="70%"><span style="font-family: Arial"><font size="3">Application-based QoS 
      manages information as it is transmitted and received.&nbsp; Depending on 
      the settings of the <i>QoS</i> screen, this feature will assign 
      information a high (traffic on this queue shares 60% of the total 
      bandwidth), medium (traffic on this queue shares 18% of the total 
      bandwidth), or low priority (traffic on this queue shares 1% of the total 
      bandwidth) for the five preset applications and three additional 
      applications that you specify.</font></span></td>
  </tr>
<% wireless_support(1); %>
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><b><span style="font-family: Arial"><font size="3">Fragment packet's 
      size of AF and BE traffic</font></span></b></td>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><span style="font-family: Arial"><font size="3">Select this option to 
      fragmentize the packet sizes for AF (Assured Forwarding) and BE (Best 
      Effort) queues so that it will increase the efficiency for transporting EF 
      (expedited forwarding) queues.&nbsp; Enter a range between 68~1492 bytes.</font></span></td>
  </tr>
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><b><span style="font-family: Arial"><font size="3">Enable 802.1p P 
      bits scheduling</font></span></b></td>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><span style="font-family: Arial"><font size="3">Select this option to 
      enable 802.1p P bits classification.&nbsp; Enter the VLAN VID (VLAN 
      Identifier) number in the field.</font></span></td>
  </tr>
<% wireless_support(2); %>
</table>
<P class=MsoNormal>&#12288;</P>
<P class=MsoNormal><SPAN lang=EN-US style="FONT-FAMILY: Arial">Check all the 
values and click <B>Save Settings</B> to save your settings. Click <B>Cancel 
Changes</B> to cancel your unsaved changes.</SPAN></P>
<P class=MsoNormal>&#12288;</P>
<P 
class=MsoNormal> 

<CENTER><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" onclick=self.close() type=button value=Close name=B3></CENTER>
<P></P></FORM></BODY></HTML>
