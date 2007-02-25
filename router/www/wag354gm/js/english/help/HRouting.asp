<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<!-- saved from url=(0032)http://192.168.1.1/HelpRoute.htm -->
<!-- saved from url=(0032)http://192.168.1.1/HelpRoute.htm --><HTML><HEAD><TITLE>Routing</TITLE>
<META http-equiv=Content-Type content="text/html; charset=windows-1252">
<META content="Microsoft FrontPage 4.0" name=GENERATOR>
<style>
<!--
LI.MsoNormal {
	FONT-SIZE: 12pt; FONT-FAMILY: "Times New Roman"; mso-style-parent: ""; margin-left:0cm; margin-right:0cm; margin-top:0cm; margin-bottom:0pt
}
-->
</style>
</HEAD>
<BODY onload=window.focus()>
<table border="0" width="100%">
  <tr>
    <td width="75%"><FONT face=Arial style="font-size: 16pt" color="#3333FF"><b>Advanced Routing </b></FONT>
    </td>
    <td width="25%">
      <p align="right"><font color="#3333ff"><b><span style="FONT-SIZE: 16pt; FONT-FAMILY: Arial"><a href="javascript:print();">
<img src="../image/Printer.gif" align="center" border="0" width="82" height="44"></a></span></b></font></td>
  </tr>
</table>
<FONT face=Arial size=2>
<P><STRONG>NAT</STRONG></P>
<P>NAT is Network Address Translation,&nbsp;which allows&nbsp;multiple computers 
to share one Internet connection.&nbsp; You can&nbsp;turn off&nbsp;NAT by 
selecting the&nbsp;<STRONG>Disable </STRONG>option.&nbsp; By default, NAT is set 
to <STRONG>Enable</STRONG>.</P>
<P><STRONG>Dynamic Routing</STRONG></P>
<P>The Dynamic Routing feature can be used to automatically adjust to physical 
changes in the network's layout. The Gateway uses the dynamic RIP protocol. It 
determines the route that the network packets take based on the fewest number of 
hops between the source and the destination. The RIP protocol regularly 
broadcasts routing information to other routers on the network. </P><B>
<P>To set up Dynamic Routing:</P>
<OL></B>
  <LI>Select <b>Enabled </b>for RIP mode.<LI>For the Transmit RIP Version dropdown menu,&nbsp;select the protocol you 
  want to use for transmitting data on the network&nbsp;protocol you want for 
  transmitting data on the network. 
  <LI>For the Receive RIP Version dropdown menu, select the&nbsp;protocol you 
  want to use for receiving data from the network. 
  <LI>Click the <B>Save Settings</B>&nbsp;button to save your changes. 
</LI></OL>
<P><STRONG>Static Routing</STRONG></P>
<P>When multiple routers are installed on your network, you will need to 
configure Static Routing. The static routing function determines the path that 
data follows over your network before and after it passes through the Gateway. 
You can use static routing to allow different IP domain users to access the 
Internet through&nbsp;the Gateway.<B> This is an advanced feature. Please 
proceed with caution</B>. </P>
<P>To set up static routing, you should add routing entries in the Gateway's 
table that tell the device where to send all incoming packets. All of your 
network routers should direct the default route entry to&nbsp;this 
Gateway.</P></FONT><FONT face=Arial size=2><B>
<P>To create a static route entry:</P>
<OL></B>
  <LI>Select&nbsp;an&nbsp;entry from the drop down list. The&nbsp;Gateway 
  supports up to 20 static&nbsp;route entries. 
  <LI>Enter the following data for the static route.
  </FONT>
    <table class=MsoNormalTable style="border-collapse: collapse; border-style: none; border-width: medium" cellSpacing=0 cellPadding=0 border="1" width="95%">
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top width="21%"><b><span style="font-family: Arial"><font face="Arial" size="3">Destination IP 
      Address</font></span></b></td>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top width="70%"><span style="font-family: Arial"><font face="Arial" size="3">Enter the network 
      address of the remote local LAN segment.&nbsp; For a standard Class C IP 
      domain, the network address is the first three fields of the Destination 
      LAN IP, while the last field should be zero.</font></span></td>
  </tr>
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><b><span style="font-family: Arial"><font face="Arial" size="3">Subnet Mask</font></span></b></td>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><span style="font-family: Arial"><font face="Arial" size="3">Enter the Subnet Mask 
      used on the destination IP domain.&nbsp; For a standard class C IP domain, 
      the Subnet Mask is 255.255.255.0.</font></span></td>
  </tr>
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><b><span style="font-family: Arial"><font face="Arial" size="3">Gateway</font></span></b></td>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><span style="font-family: Arial"><font face="Arial" size="3">If this Gateway is 
      used to connect your network to the Internet, then your Gateway IP is the 
      Gateway's IP Address.&nbsp; If you have another router handling your 
      network's Internet connection, enter the <b>IP Address</b> of that router 
      instead.</font></span></td>
  </tr>
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><b><span style="font-family: Arial"><font face="Arial" size="3">Hop Count</font></span></b></td>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><span style="font-family: Arial"><font face="Arial" size="3">Enter the Hop Count.&nbsp; 
      This is the number of hops to each node until the destination has been 
      reached.</font></span></td>
  </tr>
</table>
  <LI>Click the <STRONG>Save Settings</STRONG> button to save your changes. 
  Click <STRONG>Cancel Changes</STRONG> to cancel&nbsp; your changes.<FONT 
  face=Arial size=2></LI></OL>
<P>To view the current routing table, click the <STRONG>Show Routing 
Table</STRONG> button.<FONT face=Arial size=2> </FONT></P>
<P><STRONG>To delete&nbsp; a static route entry:</STRONG></P>
<OL>
  <LI>Select an entry from the drop-down list. 
  <LI>Click the <B>Delete Entry</B> button. 
  <LI>Click the <B>Save Settings</B> button to save your 
changes.</LI></OL></FONT><FONT size=2>
<P>
<HR>
<CENTER><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" onclick=self.close() type=button value=Close name=B3></CENTER>
</FONT></BODY></HTML>
