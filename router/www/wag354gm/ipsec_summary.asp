<html>

<title>Summary</title>
<script src="common.js"></script>
<script language="JavaScript" type="text/javascript" src="share.js"></script>
<script language="JavaScript" type="text/javascript" src="capsec.js"></script>
<SCRIPT language=JavaScript>
document.title=(share.summary);
</SCRIPT>
<body bgcolor=white>
<center>
<table width=100% border=0 cellspacing=1>
<tr>
<td colspan=2><font face=Arial color=blue><b><script>Capture(secother.vpnsum)</script></td>
<td align=right><form><!-- <input type=button value=' Refresh ' onClick={window.location='ipsec_summary.asp'}>--><script>document.write("<input type=button value='" + share.refreshs + "' onClick={window.location='ipsec_summary.asp'}>");</script></form></td>
</tr>
</table>
<tr><td colspan=2><font face=verdana size=1.5>WAN IP: <% entry_config("wanip",0);%> </td></tr>
<table width=100% border=0 cellspacing=1>
<tr align=middle bgcolor=b3b3b3>
<th><font face=verdana size=1.5>No.</th>
<th><font face=verdana size=1.5><script>Capture(secother.tuname)</script></th>
<th><font face=verdana size=1.5><script>Capture(bmenu.statu)</script></th>
<th><font face=verdana size=1.5><script>Capture(secother.localgrp)</script></th>
<th><font face=verdana size=1.5><script>Capture(secother.remotegrp)</script></th>
<th><font face=verdana size=1.5><script>Capture(secother.remotegw)</script></th>
<th><font face=verdana size=1.5><script>Capture(secother.secmethod)</script></th>
</tr>
<% ipsec_show_summary(); %>
</table>
</center>
</body>
</html>
