<HTML><HEAD><TITLE>EMI Test</TITLE>
<% no_cache(); %>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<SCRIPT language=JavaScript>
function to_submit(F)
{
	if(F.command.value == ""){
		alert("Please input  command!");
		return false;
	}

	F.submit_button.value = "emi_test";
	F.submit_type.value = "exec";
        F.change_action.value = "gozila_cgi";
	F.action.value = "Apply";
        F.submit();
}
function to_del(F)
{
	F.submit_button.value = "emi_test";
	F.submit_type.value = "del";
        F.change_action.value = "gozila_cgi";
	F.action.value = "Apply";
        F.submit();
}
</SCRIPT>
</HEAD>
<BODY bgColor=white>
<FORM method=get action=apply.cgi>
<input type=hidden name=submit_button value="emi_test">
<input type=hidden name=submit_type value="exec">
<input type=hidden name=change_action value="gozila_cgi">
<input type=hidden name=action value="Apply">

<CENTER>
<TABLE cellSpacing=1 width=100% border=0>
<TR>
	<TD width=70%><INPUT name=command maxlength=100 size=65></TD>
	<TD width=30%><INPUT type=button value="  Apply  " onClick=to_submit(this.form)>&nbsp;<INPUT onclick=window.location.replace('emi_test.asp') type=button value=" Refresh ">&nbsp;<INPUT type=button value="  Stop  " onClick=to_del(this.form)></TD>
</TR>
<TR>
	<TD width=70%>&nbsp;</TD>
	<TD width=30%><INPUT type=checkbox value="1" name="background"> run in background</TD>
</TR>
</TABLE>
</CENTER>
<br><br>
<hr>
<% dump_emi_test_log(); %>
</FORM>
</BODY>
</HTML>
