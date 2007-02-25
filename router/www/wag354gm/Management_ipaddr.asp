

	<input type=hidden name=ip_range_config>
	<TR>
	<TD width=156 bgcolor=#e7e7e7 colspan=3 height=25>&nbsp;</TD>
	<TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
	<TD colspan=3 height=25 width="42">&nbsp;</TD>
	<TD width=127 height=25> <script>Capture(admgwaccess.allowip)</script>:</TD>
	<TD width=272 height=25>
	<select name=ip_restrict onchange='SelIPRange(this.form.ip_restrict.selectedIndex, this.form)' >
	<option value=0 <% ip_restrict_config("ip_restrict", 0);%> ><script>Capture(share.alls)</script></option>
	<option value=1 <% ip_restrict_config("ip_restrict", 1);%> ><script>Capture(share.ipaddrr)</script></option>
	<option value=2 <% ip_restrict_config("ip_restrict", 2);%> ><script>Capture(share.iprange)</script></option>
	</select>
	</TD>
	<TD width=13 height=25>&nbsp;</TD>
	<TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
	</TR>

	<TR>
	<TD width=156 bgcolor=#e7e7e7 colspan=3 height=25>&nbsp;</TD>
	<TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
	<TD colspan=3 height=25 width="42">&nbsp;</TD>
	<TD width=127 height=25> </TD>
	<TD width=272 height=25>
	  <input type="text" name="range_ip_01" size="3" maxlength="3" onBlur=valid_range(this,1,223,"IP") class="num" value=<% ip_restrict_config("range_start_ip","0"); %> >.
          <input type="text" name="range_ip_02" size="3" maxlength="3" onBlur=valid_range(this,0,255,"IP") class="num" value=<% ip_restrict_config("range_start_ip","1"); %> >.
          <input type="text" name="range_ip_03" size="3" maxlength="3" onBlur=valid_range(this,0,255,"IP") class="num" value=<% ip_restrict_config("range_start_ip","2"); %> >.
          <input type="text" name="range_ip_04" size="3" maxlength="3" onBlur=valid_range(this,1,254,"IP") class="num" value=<% ip_restrict_config("range_start_ip","3"); %> > 
	</TD>

	<TD width=13 height=25>&nbsp;</TD>
	<TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
	</TR>

