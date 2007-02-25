<HTML><HEAD><TITLE> Advanced IPSec VPN Tunnel Setup</TITLE>
<% no_cache(); %>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<link rel="stylesheet" type="text/css" href="style.css">
<style fprolloverstyle>
A:hover {color: #00FFFF}
.small A:hover {color: #00FFFF}
</style>
<script src="image/common.js"></script>
<script src="common.js"></script>
<script language="JavaScript" type="text/javascript" src="share.js"></script>
<script language="JavaScript" type="text/javascript" src="capsec.js"></script>
<script language=JavaScript>
document.title=(ipsecadv.advsetup);
function checklife(F)
{
	if(F.ipsecp1_klife.value < 300 || F.ipsecp1_klife.value > 604800)
	{
		//alert('Phase 1 Key Lifetime out of range [300~604800]!'); 
		alert(errmsg.err60);
		F.ipsecp1_klife.focus();
		return false;
	}
	return true;
}
	
function checkblock(F)
{	
	if(F.ipsec_blockip.checked)
	{
		if (F.ipsec_retrytimes.value > 256 || F.ipsec_retrytimes.value <= 0)
		{
			//alert('This value is out of range [1 - 256]');
			alert(errmsg.err61);

			F.ipsec_retrytimes.value = 5;
			F.ipsec_retrytimes.focus();
		}
		if (F.ipsec_blocksecs.value > 65535 || F.ipsec_blocksecs.value <= 0)
		{
			//alert('This value is out of range [1 - 65535]');	
			alert(errmsg.err62);

			F.ipsec_blocksecs.value = 60;
			F.ipsec_blocksecs.focus();

		}	
	}
}

function to_submit(F)
{
	if (F.ipsec_natt.checked)
	{
		F.ipsec_natt.value = 1;
		F.hid_ipsec_natt.value = 1;
	}
	else 
	{
		F.ipsec_natt.value = 0;
		F.hid_ipsec_natt.value = 0;
	}
	
	if (F.ipsec_netbios.checked)
	{
		F.ipsec_netbios.value = 1;
		F.hid_ipsec_netbios.value = 1;
	}
	else 
	{
		F.ipsec_netbios.value = 0;
		F.hid_ipsec_netbios.value = 0;
	}

	if (F.ipsec_antireplay.checked)
	{
		F.ipsec_antireplay.value = 1;
		F.hid_ipsec_antireplay.value = 1;
	}
	else 
	{
		F.ipsec_antireplay.value = 0;
		F.hid_ipsec_antireplay.value = 0;
	}	

	if (F.ipsec_keepalive.checked)
	{
		F.ipsec_keepalive.value = 1;
		F.hid_ipsec_keepalive.value = 1;
	}
	else 
	{
		F.ipsec_keepalive.value = 0;
		F.hid_ipsec_keepalive.value =0;
	}

	
	if (F.ipsec_blockip.checked)
	{
		F.ipsec_blockip.value = 1;
		F.hid_ipsec_blockip.value = 1;
	}
	else 
	{
		F.ipsec_blockip.value = 0;
		F.hid_ipsec_blockip.value = 0;
	}


		F.submit_button.value = "ipsecadv";
		F.action.value = "Apply";
		F.submit();
}

</script>

</head>

<BODY bgColor=#808080><CENTER>
<form name=F1 action=apply.cgi method=<% get_http_method(); %> onreset='return false'>
<input type=hidden name=submit_button>
<input type=hidden name=change_action>
<input type=hidden name=submit_type>
<input type=hidden name=action>
<input type=hidden name="lan_ipaddr" value="4">
<input type=hidden name=ipsecadv_config>
<input type=hidden name=hid_returnPoint value=''><TABLE borderColor=#111111 height=282 cellSpacing=0 cellPadding=0 width=633 bgColor=#ffffff border=0>
<TBODY><TR><TH width=1046 colSpan=2 height=282 bordercolor="#FFFFFF">
<TABLE height=296 cellSpacing=3 width=706 bgColor=#ffffff border=0>
<TBODY><TR><TH width=872 height=232>
<TABLE height=327 cellSpacing=0 width=355 bgColor=#ffffff border=0 style="border-collapse: collapse" bordercolor="#111111" cellpadding="0">
<TBODY>
<TR><TH vAlign=bottom align=right width=759 bgColor=#FFFFFF height=16>
<div align="center">
<table border="0" cellpadding="0" cellspacing="0" style="border-collapse: collapse" bordercolor="#111111" id="AutoNumber1" width="704" height="21">
<tr>
<td height="16" colspan="2" bgcolor="#6666CC" width="153"> &nbsp;</td>
<td height="16" colspan="5" bgcolor="#6666CC" width="551"> &nbsp;</td>
</tr>
<tr>
<td height="91" colspan="2" bgcolor="#000000" width="153"><p align="center"><font color="#FFFFFF" style="font-size: 15pt"><b><script>Capture(ipsecadv.advsetup)</script></b></font></td>

<td height="91" colspan="5" bgcolor="#6666CC" width="551">&nbsp;</td>
</tr>

<tr>
<td height="16" width="153" colspan="2" bgcolor="#E7E7E7"><b><img border="0" src="image/UI_03.gif" width="155" height="34"></b></td>
<td height="16" width="551" colspan="5"><b><img border="0" src="image/UI_02.gif" width="417%" height="34"></b></td>
</tr>
<tr>
<td height="297" bgcolor="#E7E7E7" width="147" rowspan="26">
</td><td height="7" width="8">
  <p style="word-spacing: 0; margin: 0"><b><img border="0" src="image/UI_04.gif" width="8" height="30"></b></p>
</td>
<td height="297" bgcolor="#FFFFFF" width="9" rowspan="26">
</td>
<td height="7" bgcolor="#FFFFFF" width="542" colspan=4 valign="middle" align="left">
  <p style="word-spacing: 0; margin: 0"><b><font size="2"><input type=hidden name=hid_vpnTunnelNo value=0></font><font face=Arial color=blue size=2><% ipsecadv_config("tunnel_entry",0); %></font></b></p>
</td>
</tr>
<tr><td height="10" width="8">
    <p style="word-spacing: 0; margin: 0"><b><img border="0" src="image/UI_04.gif" width="8" height="30"></b></p>
  </td>
<td height="10" bgcolor="#FFFFFF" width="542" colspan=4 valign="middle" align="left">
  <p style="word-spacing: 0; margin: 0"><font face=Arial size=2><b><script>Capture(ipsecadv.phase)</script> 1:</b></font></p>
  </td>
</tr>

<tr>
<td height="10" width="8">
  <p style="word-spacing: 0; margin: 0"><b><img border="0" src="image/UI_04.gif" width="8" height="30"></b></p>
</td><td height="90" bgcolor="#FFFFFF" width="20" rowspan="9">
  <p style="word-spacing: 0; margin: 0">&nbsp;</p>
</td>
<td height="20" bgcolor="#FFFFFF" width="110" rowspan="2">
  <p style="word-spacing: 0; margin: 0"><font face="Arial"><b> <script>Capture(ipsecadv.opmode)</script> 
  :</b></font></p>
</td>
<td height="10" bgcolor="#FFFFFF" width="412" colspan="2">
<p style="word-spacing: 0; margin: 0">
<font face=Arial>
<b>
<input type=radio name=ipsec_opmode value=0 <% ipsecadv_config("ipsec_opmode", 0); %>><script>Capture(ipsecadv.mainmode)</script> </b> </font></p>
 </td>
<!-- input type=hidden name=hid_vpnUNameSwitch value=0 --></tr><tr><td height="10" width="8">
    <p style="word-spacing: 0; margin: 0"><b><img border="0" src="image/UI_04.gif" width="8" height="30"></b></p>
  </td>
<td height="10" bgcolor="#FFFFFF" width="412" colspan="2">
  <p style="word-spacing: 0; margin: 0"><font face=Arial>
<b>
<input type=radio name=ipsec_opmode value=1 <% ipsecadv_config("ipsec_opmode", 1); %>><script>Capture(ipsecadv.aggmode)</script> 
 &nbsp;&nbsp;&nbsp;&nbsp;</b></font></p>
</td>
</tr>
<!--tr>
<td height="16" bgcolor="#E7E7E7" width="147">&nbsp;</td><td height="16" width="8"><img border="0" src="image/UI_04.gif" width="8" height="30"></td><td height="16" bgcolor="#FFFFFF" width="9">&nbsp;</td><td height="16" bgcolor="#FFFFFF" width="20">&nbsp;</td><td height="16" bgcolor="#FFFFFF" width="106">&nbsp;</td>
<td height="16" bgcolor="#FFFFFF" colspan="2" width="416">
<font face=Arial size=2>
<input type=checkbox  value=1 name=sel_enableUName <% nvram_match("ipsec_uname","1","checked"); %>> Username: &nbsp;&nbsp;
<input name=txt_vpnAggUName size=20 maxlength=24 value='<% show_ipsec_uname; %>'>
</td>
</tr-->
<tr>
<td height="10" width="8">
  <p style="word-spacing: 0; margin: 0"><b><img border="0" src="image/UI_04.gif" width="8" height="30"></b></p>
</td>
<td height="70" bgcolor="#FFFFFF" width="110" rowspan="7" valign="top">
  <p style="word-spacing: 0; margin: 0"><font face="Arial"><b> 
<script>Capture(ipsecadv.proposal)</script>  1:</b></font></p>
</td>
<td height="10" bgcolor="#FFFFFF" width="403" colspan="2">
  <p style="word-spacing: 0; margin: 0">&nbsp;</p>
</td>
</tr>
<tr>
<td height="10" width="8">
  <p style="word-spacing: 0; margin: 0"><b><img border="0" src="image/UI_04.gif" width="8" height="30"></b></p>
</td>
<td height="10" bgcolor="#FFFFFF" width="171">
<p style="word-spacing: 0; margin: 0">
<font face=Arial><b><script>Capture(ipsecadv.encryption)</script>  :</b></font></p>
</td>
<td height="10" bgcolor="#FFFFFF" width="232">
<font face=Arial><b><select name=ipsecp1_enc><option value=1  <% ipsecadv_config("ipsecp1_enc",1); %>> DES &nbsp; </option><option value=2 <% ipsecadv_config("ipsecp1_enc",2); %>> 3DES &nbsp; </option></select></b></font>
</td></tr><tr><td height="10" width="8">
    <p style="word-spacing: 0; margin: 0"><b><img border="0" src="image/UI_04.gif" width="8" height="30"></b></p>
  </td><td height="10" bgcolor="#FFFFFF" width="171">
    <p style="word-spacing: 0; margin: 0"><font face=Arial><b><script>Capture(ipsecadv.auth)</script> 
    :</b></font></p>
  </td><td height="10" bgcolor="#FFFFFF" width="232">
    <font face=Arial><b><select name=ipsecp1_auth><option value=1 <% ipsecadv_config("ipsecp1_auth",1); %>> MD5 &nbsp; </option><option value=2 <% ipsecadv_config("ipsecp1_auth",2); %>> SHA &nbsp; </option></select></b></font>
  </td></tr><tr><td height="10" width="8">
    <p style="word-spacing: 0; margin: 0"><b><img border="0" src="image/UI_04.gif" width="8" height="30"></b></p>
  </td><td height="10" bgcolor="#FFFFFF" width="171">
    <p style="word-spacing: 0; margin: 0"><font face=Arial><b><script>Capture(ipsecadv.group)</script> 
    :</b></font></p>
  </td><td height="10" bgcolor="#FFFFFF" width="232">
    <font face=Arial><b><select name=ipsecp1_group>
<option value=1 <% ipsecadv_config("ipsecp1_group",1); %> > 768-bit </option>
<option value=2 <% ipsecadv_config("ipsecp1_group",2); %> > 1024-bit </option>
</select></b></font>
  </td></tr>
<tr><td height="10" width="8">
    <p style="word-spacing: 0; margin: 0"><b><img border="0" src="image/UI_04.gif" width="8" height="30"></b></p>
  </td><td height="10" bgcolor="#FFFFFF" width="171">
    <p style="word-spacing: 0; margin: 0"><font face=Arial><b><script>Capture(ipsecadv.keytime)</script> 
    :</b></font></p>
  </td><td height="10" bgcolor="#FFFFFF" width="232">
    <font face=Arial><b><input name=ipsecp1_klife size=10 maxlength=10 onblur=checklife(this.form) value='<% ipsecadv_config("ipsecp1_klife",0); %>'> <script>Capture(share.seconds)</script></b></font>
  </td></tr>
<tr>
<td height="10" width="8">
  <p style="word-spacing: 0; margin: 0"><b><img border="0" src="image/UI_04.gif" width="8" height="30"></b></p>
  </td>
<td height="20" bgcolor="#FFFFFF" width="412" rowspan="2" colspan="2">
  <p style="word-spacing: 0; margin: 0"><font face=Arial>(<script>Capture(ipsecadv.note)</script>:DES/MD5/768, 3DES/SHA/1024 and 3DES/MD5/1024.)</font></p>
 </td></tr>
<tr>
<td height="10" width="8">
  <p style="word-spacing: 0; margin: 0"><b><img border="0" src="image/UI_04.gif" width="8" height="30"></b></p>
</td>
</tr>

<tr>
<td height="10" width="8">
  <p style="word-spacing: 0; margin: 0"><b><img border="0" src="image/UI_04.gif" width="8" height="30"></b></p>
</td>
<td height="10" bgcolor="#FFFFFF" width="542" colspan=4>
  <p style="word-spacing: 0; margin: 0"><font face=Arial size=2><b><script>Capture(ipsecadv.phase)</script> 
  2:</b></font></p>
</td>
</tr>

<tr>
<td height="10" width="8">
  <p style="word-spacing: 0; margin: 0"><b><img border="0" src="image/UI_04.gif" width="8" height="30"></b></p>
</td>
<td height="162" bgcolor="#FFFFFF" width="20" rowspan="6">
  <p style="word-spacing: 0; margin: 0">&nbsp;</p>
</td>
<td height="162" bgcolor="#FFFFFF" width="110" rowspan="6" valign="top">
  <p style="word-spacing: 0; margin: 0"><font face=Arial> <b> &nbsp;<script>Capture(ipsecadv.proposal)</script> :</b></font></p>
</td>
<td height="10" bgcolor="#FFFFFF" width="402" colspan="2">
  <p style="word-spacing: 0; margin: 0">&nbsp;</p>
</td>
</tr>
<tr>
<td height="10" width="8">
  <p style="word-spacing: 0; margin: 0"><b><img border="0" src="image/UI_04.gif" width="8" height="30"></b></p>
</td>
<td height="10" bgcolor="#FFFFFF" width="172">
  <p style="word-spacing: 0; margin: 0"><font face=Arial><b><script>Capture(ipsecadv.encryption)</script> 
  :</b></font></p>
</td>
<td height="10" bgcolor="#FFFFFF" width="230">
  <font face=Arial><b> <% ipsecadv_config("ipsecp2_enc",0);%></b></font>
</td>
</tr>
<tr>
<td height="10" width="8">
  <p style="word-spacing: 0; margin: 0"><b><img border="0" src="image/UI_04.gif" width="8" height="30"></b></p>
</td>
<td height="10" bgcolor="#FFFFFF" width="172">
  <p style="word-spacing: 0; margin: 0"><font face=Arial><b><script>Capture(ipsecadv.auth)</script> 
  :</b></font></p>
</td>
<td height="10" bgcolor="#FFFFFF" width="230">
  <font face=Arial><b> <% ipsecadv_config("ipsecp2_auth",0);%></b></font>
</td>
</tr>
<tr>
<td height="10" width="8">
  <p style="word-spacing: 0; margin: 0"><b><img border="0" src="image/UI_04.gif" width="8" height="30"></b></p>
</td>
<td height="10" bgcolor="#FFFFFF" width="172">
  <p style="word-spacing: 0; margin: 0"><font face=Arial><b>PFS :</b></font></p>
</td>
<td height="10" bgcolor="#FFFFFF" width="230">
  <font face=Arial><b><% ipsecadv_config("ipsecp2_pfs",0); %></b></font>
</td>
</tr>

<tr>
<td height="10" width="8">
  <p style="word-spacing: 0; margin: 0"><b><img border="0" src="image/UI_04.gif" width="8" height="30"></b></p>
</td>
<td height="10" bgcolor="#FFFFFF" width="172">
  <p style="word-spacing: 0; margin: 0"><font face=Arial><b><script>Capture(ipsecadv.group)</script> 
  :</b></font></p>
</td>
<td height="10" bgcolor="#FFFFFF" width="230">
  <font face=Arial><b><select name=ipsecp2_group>
<option value=1 <%ipsecadv_config("ipsecp2_group",1); %> > 768-bit </option>
<option value=2 <%ipsecadv_config("ipsecp2_group",2); %> > 1024-bit </option></select></b></font>
</td>
</tr>

<tr>
<td height="10" width="8">
  <p style="word-spacing: 0; margin: 0"><b><img border="0" src="image/UI_04.gif" width="8" height="30"></b></p>
</td>
<td height="10" bgcolor="#FFFFFF" width="172">
  <p style="word-spacing: 0; margin: 0"><font face=Arial><b><script>Capture(ipsecadv.keytime)</script> 
  :</b></font></p>
</td>
<td height="10" bgcolor="#FFFFFF" width="230">
  <font face=Arial><b><% ipsecadv_config("ipsecp2_klife",0); %> <script>Capture(share.seconds)</script></b></font>
</td>
</tr>
<tr>
<td height="10" width="8">
  <p style="word-spacing: 0; margin: 0"><b><img border="0" src="image/UI_04.gif" width="8" height="30"></b></p>
</td>
<td height="27" bgcolor="#FFFFFF" width="532" colspan="4">
  <p style="word-spacing: 0; margin: 0"><font face=Arial size=2><script>Capture(ipsecadv.othersetting)</script>:</font></p>
</td>
</tr>
<tr>
<td height="10" width="8">
  <p style="word-spacing: 0; margin: 0"><b><img border="0" src="image/UI_04.gif" width="8" height="30"></b></p>
  </td>
<td height="60" bgcolor="#FFFFFF" width="130" colspan=2 rowspan="6">
  <p style="word-spacing: 0; margin: 0">&nbsp;</p>
  </td>
<td height="10" bgcolor="#FFFFFF" width="412" colspan="2">
  <p style="word-spacing: 0; margin: 0"><font face=Arial>
 <b>
 <input type=hidden name=hid_ipsec_natt value=0 ><input type=checkbox value=1 name=ipsec_natt <%ipsecadv_config("ipsec_natt",0);%>><script>Capture(ipsecadv.nat)</script></b></font></p>
  </td>
</tr>
<tr>
<td height="10" width="8">
  <p style="word-spacing: 0; margin: 0"><b><img border="0" src="image/UI_04.gif" width="8" height="30"></b></p>
</td>
<td height="10" bgcolor="#FFFFFF" width="412" colspan="2">
  <p style="word-spacing: 0; margin: 0"><font face=Arial> <b> <input type=hidden name=hid_ipsec_netbios value=0 ><input type=checkbox value=1 name=ipsec_netbios <%ipsecadv_config("ipsec_netbios",0);%>><script>Capture(ipsecadv.bios)</script></b></font></p>
</td>
</tr>
<tr>
<td height="10" width="8">
  <p style="word-spacing: 0; margin: 0"><b><img border="0" src="image/UI_04.gif" width="8" height="30"></b></p>
</td>
<td height="10" bgcolor="#FFFFFF" width="412" colspan="2">
  <p style="word-spacing: 0; margin: 0"><font face=Arial>
<b>
<input type=hidden name=hid_ipsec_antireplay value=0><input type=checkbox value=1 name=ipsec_antireplay <%ipsecadv_config("ipsec_antireplay",0);%>><script>Capture(ipsecadv.antireplay)</script></b></font></p>
</td>
</tr>
<tr>
<td height="10" width="8">
  <p style="word-spacing: 0; margin: 0"><b><img border="0" src="image/UI_04.gif" width="8" height="30"></b></p>
</td>
<td height="10" bgcolor="#FFFFFF" width="412" colspan="2">
  <p style="word-spacing: 0; margin: 0"><font face=Arial>
 <b>
 <input type=hidden name=hid_ipsec_keepalive value=0><input type=checkbox value=1 name=ipsec_keepalive <%ipsecadv_config("ipsec_keepalive",0);%>><script>Capture(ipsecadv.keep)</script></b></font></p>
</td>
</tr>
<tr>
<td height="10" width="8">
  <p style="word-spacing: 0; margin: 0"><b><img border="0" src="image/UI_04.gif" width="8" height="30"></b></p>
</td>
<td height="20" bgcolor="#FFFFFF" width="412" colspan="2" rowspan="2">
  <p style="word-spacing: 0; margin: 0">
 <b>
 <font face=Arial>
 <input type=hidden name=hid_ipsec_blockip value=0><input type=checkbox value=1 name=ipsec_blockip <%ipsecadv_config("ipsec_blockip",0);%>><script>Capture(ipsecadv.ifthan)</script>
 <input name=ipsec_retrytimes size=3 maxlength=3 onblur=checkblock(this.form) value='<%ipsecadv_config("ipsec_retrytimes",0);%>'> <script>Capture(ipsecadv.mid)</script><script>Capture(ipsecadv.ipfor)</script></font></b> <b><font face=Arial>
 <input name=ipsec_blocksecs size=5 maxlength=5 onblur=checkblock(this.form) value='<%ipsecadv_config("ipsec_blocksecs",0);%>'> <script>Capture(ipsecadv.seconds)</script>
  </font></b></p>
 </td>
</tr>
<tr>
<td height="10" width="8">
  <p style="word-spacing: 0; margin: 0"><b><img border="0" src="image/UI_04.gif" width="8" height="30"></b></p>
</td>
</tr>
<tr>
<td height="50" width="8"><b><img border="0" src="image/UI_04.gif" width="8" height="50"></b></td>
<td height="50" width="542" bgcolor="#FFFFFF" colspan="4"><p align="right">
<!--input type=submit value='Save Settings' onClick=ReturnValue(document.F1)-->
<!--<input type=submit value='Save Settings' onClick=to_submit(this.form)>--><script>document.write("<input type=submit value=\"" + share.saves + "\"" + " onClick=to_submit(this.form)>");</script>
<!-- <input type=reset value='Cancel Changes' onClick="javascript:window.location.href='ipsecadv.asp'">--><script>document.write("<input type=reset value=\"" + share.cancels + "\"" + " onClick=\"javascript:window.location.href='ipsecadv.asp'\">");</script> </td></tr></table></div></TH></TR></TBODY></TABLE></TH></TR></TBODY></TABLE></TR></TBODY><!--script language=javascript>init(document.F1)</script--></TABLE></FORM></CENTER></BODY></HTML>
