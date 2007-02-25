 <TR>
          <!--input type=hidden name=tstatus_config -->
          <TD width=156 bgColor=#e7e7e7 height=1>&nbsp;</TD>
          <TD width=8 height=1><IMG height=25 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD height=25>&nbsp;</TD>
          <TD height=25>&nbsp;</TD>
          <TD colSpan=3>
            <HR color=#b5b5e6 SIZE=1>
          </TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 height=1><IMG height=25 src="image/UI_05.gif" 
            width=15 border=0></TD></TR>
        <TR>
          <TD align=right bgColor=#e7e7e7 height=25><B>&nbsp;</B></TD>
          <TD width=8 height=25><IMG height=30 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD align=middle colSpan=2>&nbsp;
	
<!--	<INPUT type=button onclick='Connect(this.form,<% entry_config("click",0); %>)' value=' <% entry_config("connect",0); %>'>-->
<!--<INPUT type=button name=connect onclick='Connect(this.form,<% entry_config("click",0); %>)' value="">-->

<script>
	var configclick = "<% entry_config("click",0); %>";
	var connect_button = "<% entry_config("connect",0); %>";

	if(connect_button == "Connect")
		document.write("<INPUT type=button onclick=Connect(this.form," + configclick + ")" + " value=\"" + share.connect + "\">");
	else
		document.write("<INPUT type=button onclick=Connect(this.form," + configclick + ")" + " value=\"" + share.disconn + "\">");
			
</script>

	<!--INPUT onclick=LogButton_check(this.form,0) type=button value="View Logs">
	<INPUT onclick="{self.open('vpnlog.asp','VPNLog','alwaysRaised,resizable,scrollbars,width=800,height=600');}" type=button value="View Logs"-->
<!--	<INPUT onclick="{self.open('apply.cgi?submit_button=Log_incoming&change_action=gozila_cgi&submit_type=&action=&sel_sysLogType=2','VPNLog','alwaysRaised,resizable,scrollbars,width=800,height=600');}" type=button value="View Logs">--><script>document.write("<INPUT onclick=\"{self.open('apply.cgi?submit_button=Log_incoming&change_action=gozila_cgi&submit_type=&action=&sel_sysLogType=2','VPNLog','alwaysRaised,resizable,scrollbars,width=800,height=600');}\" type=button value=\"" + share.viewlog +"\">");</script>
<!--	<INPUT onclick="{self.open('ipsecadv.asp','IPSecAdvance','alwaysRaised,resizable,scrollbars,width=830,height=500');}" type=button value="Advanced Settings">--></TD>
          <TD width=13>&nbsp;</TD>
          <TD width=15><IMG height=30 src="image/UI_05.gif" width=15 
            border=0></TD></TR>


        <TR>
          <TD align=right bgColor=#e7e7e7 height=25><B>&nbsp;</B></TD>
          <TD width=8 height=25><IMG height=25 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD align=middle colSpan=2>&nbsp;
<!--	<INPUT onclick="{self.open('ipsecadv.asp','IPSecAdvance','alwaysRaised,resizable,scrollbars,width=830,height=500');}" type=button value="Advanced Settings">--><script>document.write("<INPUT onclick=\"{self.open('ipsecadv.asp','IPSecAdvance','alwaysRaised,resizable,scrollbars,width=830,height=500');}\" type=button value=\"" + share.advsetting + "\">");</script></TD>
          <TD width=13>&nbsp;</TD>
          <TD width=15><IMG height=25 src="image/UI_05.gif" width=15 
            border=0></TD></TR>
