<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<!-- saved from url=(0037)http://10.128.32.102/WLActiveConn.htm -->
<HTML><HEAD><TITLE>Wireless Clients Connected</TITLE>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<SCRIPT src="common.js"></SCRIPT>
<SCRIPT language="Javascript" type="text/javascript" src="share.js"></SCRIPT>
<SCRIPT language="Javascript" type="text/javascript" src="capstatus.js"></SCRIPT>
<SCRIPT language="Javascript" type="text/javascript" src="capwireless.js"></SCRIPT>
<SCRIPT language=javascript>
document.title=(stabutton.wlclntconn);
</SCRIPT>
<META content="MSHTML 6.00.2800.1106" name=GENERATOR></HEAD>
<BODY bgColor=white onload=window.focus()>
<FORM name=F1 action=Gozila.cgi method=get>
<CENTER>
<TABLE height=9 cellSpacing=1 width=430 border=0>
  <TBODY>
  <TR>
    <TD colSpan=2 height=30><FONT face=Arial color=#0000ff size=4><script>Capture(wlother.netcomputer)</script></FONT></TD>
    <TD align=right height=30>&nbsp;
      <script>document.write("<INPUT onclick=window.location.reload() type=button value=\"" + share.refreshs + "\">");</script></TD></TR>
  <TR align=middle bgColor=#b3b3b3>
    <TD vAlign=bottom bgColor=#ffffff height=19 colapsn="2">&nbsp;</TD></TR>
  <TR align=middle bgColor=#b3b3b3>
    <TH><FONT face=verdana size=2><script>Capture(wlother.computername)</script></FONT></TH>
    <TH><FONT face=verdana size=2><script>Capture(share.ipaddr)</script></FONT></TH>
    <TH><FONT face=verdana size=2><script>Capture(share.macaddr)</script></FONT></TH></TR>

 <% wireless_active_table("online"); %>

  <TR align=middle>
    <TD bgColor=#ffffff height=24>&nbsp;</TD>
    <TD bgColor=#ffffff height=24>&nbsp;</TD>
    <TD bgColor=#ffffff height=24>&nbsp;</TD></TR>
  <TR align=middle>
    <TD bgColor=#ffffff colSpan=3 height=24>
      <script>document.write("<INPUT onclick=window.close() type=button value=\"" + share.closes + "\">");</script></TD></TR></TBODY></TABLE></CENTER></FORM></BODY></HTML>
