<TR>
	<input type=hidden name=rtype_config>
          <TD bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=25 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD><script>Capture(share.ipaddr)</script>:&nbsp;</TD>
          <TD>&nbsp;<INPUT class=num onblur="valid_range(this,0,255,'IP')" 
            maxLength=3 size=3 value='<% rtype_config("ipaddr",0); %>' name="r_ipaddr0"> . <INPUT 
            class=num onblur="valid_range(this,0,255,'IP')" maxLength=3 size=3 
            value='<% rtype_config("ipaddr",1); %>' name="r_ipaddr1"> . <INPUT class=num 
            onblur="valid_range(this,0,255,'IP')" maxLength=3 size=3 value='<% rtype_config("ipaddr",2); %>' 
            name="r_ipaddr2"> . <INPUT class=num 
            onblur="valid_range(this,0,254,'IP')" 
            maxLength=3 size=3 value='<% rtype_config("ipaddr",3); %>' name="r_ipaddr3"></TD>
          <TD width=13>&nbsp;</TD>
          <TD width=15><IMG height=25 src="image/UI_05.gif" width=15 
            border=0></TD></TR>
        <TR>
          <TD width=160 bgColor=#e7e7e7 height=1>&nbsp;</TD>
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
