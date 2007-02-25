<TR>
<input type=hidden name=keytype_config>
<TD bgColor=#e7e7e7 height=25 >&nbsp;</TD>
<TD width=8 height=25 ><IMG src='image/UI_04.gif' border=0 width=8 height=25></TD><TD width=14 height=25>&nbsp;</TD><TD width=17 height=25>&nbsp;</TD><TD width=13 height=25>&nbsp;</TD>
<TD><script>Capture(secother.encrypkey)</script>:</TD>
<TD>&nbsp;<input name=manual_enckey size=20 maxlength=50 onChange=checkenc(this.form,this,0) value='<% keytype_config("manual_enckey",0); %>' ></TD>
<TD width=13>&nbsp;</TD><TD width=15 ><IMG src='image/UI_05.gif' border=0 width=15 height=25></TD></TR>
<TR><TD bgColor=#e7e7e7 height=25 >&nbsp;</TD><TD width=8 height=25 ><IMG src='image/UI_04.gif' border=0 width=8 height=25></TD><TD width=14 height=25>&nbsp;</TD><TD width=17 height=25>&nbsp;</TD><TD width=13 height=25>&nbsp;</TD>
<TD><script>Capture(secother.authkey)</script>:</TD>
<TD>&nbsp;<input name="manual_authkey" size=20 maxlength=34 onChange=checkauth(this.form,this,0) value='<% keytype_config("manual_authkey",0); %>' ></TD>
<TD width=13>&nbsp;</TD>
<TD width=15 ><IMG src='image/UI_05.gif' border=0 width=15 height=25></TD></TR><TR><TD bgColor=#e7e7e7 height=25 >&nbsp;</TD><TD width=8 height=25 ><IMG src='image/UI_04.gif' border=0 width=8 height=25></TD><TD width=14 height=25>&nbsp;</TD><TD width=17 height=25>&nbsp;</TD>
<TD width=13 height=25>&nbsp;</TD><TD><script>Capture(secother.inboundspi)</script>:</TD><TD>&nbsp;0x<input name="manual_ispi" size=18 maxlength=3 onChange=checkspi(this.form,this) value='<% keytype_config("manual_ispi",0);  %>' >(0x100--0xFFF)</TD><TD width=13>&nbsp;</TD><TD width=15 ><IMG src='image/UI_05.gif' border=0 width=15 height=25></TD></TR><TR><TD bgColor=#e7e7e7 height=25 >&nbsp;</TD>
<TD width=8 height=25 ><IMG src='image/UI_04.gif' border=0 width=8 height=25></TD><TD width=14 height=25>&nbsp;</TD><TD width=17 height=25>&nbsp;</TD><TD width=13 height=25>&nbsp;</TD><TD><script>Capture(secother.outboundspi)</script>:</TD><TD>&nbsp;0x<input name=manual_ospi size=18 maxlength=3 onChange=checkspi(this.form,this) value='<% keytype_config("manual_ospi",0);  %>' >(0x100--0xFFF)</TD>
<TD width=13>&nbsp;</TD><TD width=15 ><IMG src='image/UI_05.gif' border=0 width=15 height=25></TD></TR>
