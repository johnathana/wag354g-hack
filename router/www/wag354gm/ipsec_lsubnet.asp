<TR>
			<input type=hidden name=ltype_config>
          <TD bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=25 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD>IP:&nbsp;</TD>
          <TD>&nbsp;<INPUT class=num onblur="valid_range(this,0,255,'IP')" 
            maxLength=3 size=3 value='<% ltype_config("ipaddr",0); %>' name="l_ipaddr0"> . <INPUT 
            class=num onblur="valid_range(this,0,255,'IP')" maxLength=3 size=3 
            value='<% ltype_config("ipaddr",1); %>' name="l_ipaddr1"> . <INPUT class=num 
            onblur="valid_range(this,0,255,'IP')" maxLength=3 size=3 value='<% ltype_config("ipaddr",2); %>' name="l_ipaddr2"> . <INPUT class=num 
            onblur="valid_range(this,0,254,'IP')"  
            maxLength=3 size=3 value='<% ltype_config("ipaddr",3); %>' name="l_ipaddr3"></TD>
          <TD width=13>&nbsp;</TD>
          <TD width=15><IMG height=25 src="image/UI_05.gif" width=15 
            border=0></TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=25 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD><script>Capture(share.netmask)</script>:&nbsp;</TD>
          <TD>&nbsp;<INPUT class=num onblur="valid_range(this,0,255,'Mask')" 
            maxLength=3 size=3 value='<% ltype_config("netmask",0); %>' name="l_netmask0"> . <INPUT 
            class=num onblur="valid_range(this,0,255,'Mask')" maxLength=3 size=3 
            value='<% ltype_config("netmask",1); %>' name="l_netmask1"> . <INPUT class=num 
            onblur="valid_range(this,0,255,'Mask')" maxLength=3 size=3 value='<% ltype_config("netmask",2); %>' name="l_netmask2"> . <INPUT class=num 
            onblur="valid_range(this,0,255,'Mask')" maxLength=3 size=3 value='<% ltype_config("netmask",3); %>' name="l_netmask3"></TD>
          <TD width=13>&nbsp;</TD>
          <TD width=15><IMG height=25 src="image/UI_05.gif" width=15 
            border=0></TD></TR>
<TR>
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
