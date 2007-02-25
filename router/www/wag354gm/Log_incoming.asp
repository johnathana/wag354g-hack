

<html>
<!--head><meta http-equiv=refresh content=3600;url=SysLog.htm></head-->
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<title>System Log</title>
<script src="common.js"></script>
<script language="JavaScript" type="text/javascript" src="share.js"></script>
<script language="JavaScript" type="text/javascript" src="capsec.js"></script>
<script language=JavaScript>
document.title=(secother.syslog);
function pre_button(F)
{
    	F.submit_type.value = "pagepre";
	    F.submit_button.value = "Log_incoming";
        F.change_action.value = "gozila_cgi";
        F.submit();

}
function next_button(F)
{
        F.submit_type.value = "pagenext";
	    F.submit_button.value = "Log_incoming";
        F.change_action.value = "gozila_cgi";
        F.submit();

}
function ClearSubmit(F)
{
        F.submit_type.value = "pageclear";
	    F.submit_button.value = "Log_incoming";
        F.change_action.value = "gozila_cgi";
        F.submit();

}
function pagerefresh(F)
{
        F.submit_type.value = "pagerefresh";
	    F.submit_button.value = "Log_incoming";
        F.change_action.value = "gozila_cgi";
        F.submit();

}

function LogSel(num,F)
{
         F.submit_button.value="Log_incoming";
         F.change_action.value="gozila_cgi";
         F.sel_sysLogType.value=F.sel_sysLogType.options[num].value;
         F.submit();
  	
}

</script>
<body bgcolor=white>
<form name=F1 action=apply.cgi method=<% get_http_method(); %>>
<input type=hidden name=submit_button>
<input type=hidden name=change_action>
<input type=hidden name=submit_type>
<input type=hidden name=action>
<center>
<table width=100% border=0 cellspacing=1>

<tr>
<td colspan=2><font face=Arial color=blue><b><script>Capture(secother.syslog)</script></td>
</tr>
<tr>
<td align=right>
<select name=sel_sysLogType onChange=LogSel(this.form.sel_sysLogType.selectedIndex,this.form)>
<option value="0" <%entry_logmsg(0);%>><script>Capture(share.alls)</script></option>
<option value="1" <%entry_logmsg(1);%>><script>Capture(secother.syslog)</script></option>
<option value="3" <%entry_logmsg(3);%>><script>Capture(secother.acclog)</script></option>
<option value="4" <%entry_logmsg(4);%>><script>Capture(secother.fwlog)</script></option>
<option value="2" <%entry_logmsg(2);%>><script>Capture(secother.vpnlog)</script></option>
<option value="5" <%entry_logmsg(5);%>><script>Capture(secother.upnplog)</script></option>
</select>&nbsp
<!-- <input type=button value=' Clear ' onClick=ClearSubmit(this.form)> --><script>document.write("<input type=button value=\"" + share.clears + "\"" + " onClick=ClearSubmit(this.form)>");</script>
&nbsp<!--<input type=button value=' pageRefresh ' onClick=pagerefresh(this.form)>--><script>document.write("<input type=button value=\"" + secother.pagefresh + "\"" + " onClick=pagerefresh(this.form)>");</script></td>
</tr>
</table>

<table width=100% border=1 cellspacing=1>
<tr>
<td>
<% dump_syslog(""); %></font></td></tr>
</table>

<table width=100% border=0 cellspacing=1>


<tr>
<td align=right>
<td align=right>
<!-- <input type=button value=' Clear ' onClick=ClearSubmit(this.form)>--><script>document.write("<input type=button value=\"" + share.clears + "\"" + " onClick=ClearSubmit(this.form)>");</script>
&nbsp<!--<input type=button value=' pageRefresh ' onClick=pagerefresh(this.form)>--><script>document.write("<input type=button value=\"" + secother.pagefresh + "\"" + " onClick=pagerefresh(this.form)>");</script>&nbsp; 
&nbsp<!--<input type=button value='Previous Page' onClick=pre_button(this.form)>--><script>document.write("<input type=button value=\"" + secother.prepage + "\"" + " onClick=pre_button(this.form)>");</script>&nbsp; 
&nbsp<!--<input type=button value=' Next Page ' onClick=next_button(this.form)>--><script>document.write("<input type=button value=\"" + secother.nextpage + "\"" + " onClick=next_button(this.form)>");</script>
</form></td>
</tr>
</table>

</center>
</body></html>
