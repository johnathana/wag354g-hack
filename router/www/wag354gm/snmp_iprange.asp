<tr>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=8>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=8>&nbsp;</TD>

          <TD colSpan=3 height=8 width="42"></TD>

          <TD width=127 height=8><FONT style="FONT-SIZE: 8pt" 
            face=Arial><script>Capture(share.iprange)</script>:</FONT></TD>

          <TD width=285 height=8 colspan="2">

            <FONT size=1><INPUT class=num maxLength=3 size=3 name=snmp_ipStartAddr0 onBlur=valid_range(this,1,223,"snmp_ipStartAddr") value='<%snmp_config("iprange", 0);%>'
>.<INPUT class=num maxLength=3 size=3 name=snmp_ipStartAddr1 onBlur=valid_range(this,0,255,"snmp_ipStartAddr") value='<%snmp_config("iprange", 1);%>'
>.<INPUT class=num maxLength=3 size=3 name=snmp_ipStartAddr2 onBlur=valid_range(this,0,255,"snmp_ipStartAddr") value='<%snmp_config("iprange", 2);%>'
>.<INPUT class=num maxLength=3 size=3 name=snmp_ipStartAddr3 onBlur=valid_range(this,1,254,"snmp_ipStartAddr") value='<%snmp_config("iprange", 3);%>'
>~<INPUT class=num maxLength=3 size=3 name=snmp_ipEndAddr0 onBlur=valid_range(this,1,223,"snmp_ipEndAddr") value='<%snmp_config("iprange", 4);%>'
>.<INPUT class=num maxLength=3 size=3 name=snmp_ipEndAddr1 onBlur=valid_range(this,0,255,"snmp_ipEndAddr") value='<%snmp_config("iprange", 5);%>'
>.<INPUT class=num maxLength=3 size=3 name=snmp_ipEndAddr2 onBlur=valid_range(this,0,255,"snmp_ipEndAddr") value='<%snmp_config("iprange", 6);%>'
>.<INPUT class=num maxLength=3 size=3 name=snmp_ipEndAddr3 onBlur=valid_range(this,1,254,"snmp_ipEndAddr") value='<%snmp_config("iprange", 7);%>'
></FONT></TD>

          <TD width=15 background=image/UI_05.gif height=8>&nbsp;</TD>

        </tr>

