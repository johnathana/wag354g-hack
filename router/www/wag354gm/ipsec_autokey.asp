<TR>
          <input type=hidden name=keytype_config>
          <TD bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=25 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD><script></script>PFS:</TD>
          <TD><INPUT type=radio value=1 
            name=auto_pfs <% keytype_config("auto_pfs",1); %>><B><script>Capture(share.enabled)</script></B>&nbsp;&nbsp;&nbsp;&nbsp;<INPUT 
            type=radio value=0 name=auto_pfs <% keytype_config("auto_pfs",0); %>><B><script>Capture(share.disabled)</script></B></TD>
          <TD width=13>&nbsp;</TD>
          <TD width=15><IMG height=25 src="image/UI_05.gif" width=15 
            border=0></TD></TR>
        <TR>
          <TD align=right bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=45 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD><script>Capture(secipsectunnel.presharedkey)</script>:</TD>
          <TD>&nbsp;<INPUT maxLength=24 onchange=checkKEY(this,this.form,24,1) 
            name=auto_presh size="20" value='<% keytype_config("auto_presh",""); %>'>&nbsp;</TD>
          <TD width=13>&nbsp;</TD>
          <TD width=15><IMG height=45 src="image/UI_05.gif" width=15 
            border=0></TD></TR>
        <TR>
          <TD width=156 align=right bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=45 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD><script>Capture(secipsectunnel.keytime)</script>:</TD>
          <TD>&nbsp;<INPUT maxLength=10 value='<% keytype_config("auto_klife",0); %>' name=auto_klife size="20">&nbsp; 
            Sec.</TD>
          <TD width=13>&nbsp;</TD>
          <TD width=15><IMG height=45 src="image/UI_05.gif" width=15 
            border=0></TD></TR>
