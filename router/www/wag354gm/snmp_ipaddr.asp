
        <tr>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=13>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=13>&nbsp;</TD>

          <TD colSpan=3 height=13 width="42"></TD>

          <TD width=127 height=13><font style="font-size: 8pt" face="Arial"><script>Capture(share.ipaddrr)</script></font><FONT style="FONT-SIZE: 8pt" 
            face=Arial>:</FONT></TD>

          <TD width=272 height=13>

            <FONT size=2>
				<INPUT class=num maxLength=3 size=3 value='<%snmp_config("ipaddr", 0);%>' name=snmp_ipaddr0 onBlur=valid_range(this,1,223,"snmp_ipaddr") 
>.<INPUT class=num maxLength=3 size=3 name=snmp_ipaddr1 onBlur=valid_range(this,0,255,"snmp_ipaddr") value='<%snmp_config("ipaddr", 1);%>'
>.<INPUT class=num maxLength=3 size=3 name=snmp_ipaddr2 onBlur=valid_range(this,0,255,"snmp_ipaddr") value='<%snmp_config("ipaddr", 2);%>'
>.<INPUT class=num maxLength=3 size=3 name=snmp_ipaddr3 onBlur=valid_range(this,1,254,"snmp_ipaddr") value='<%snmp_config("ipaddr", 3);%>'></FONT></TD>

          <TD width=13 height=13></TD>

          <TD width=15 background=image/UI_05.gif height=13>&nbsp;</TD>

        </tr>
