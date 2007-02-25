
<html><head><title>Wireless</title>

<meta http-equiv="expires" content="0">
<meta http-equiv="cache-control" content="no-cache">
<meta http-equiv="pragma" content="no-cache">
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<link rel="stylesheet" type="text/css" href="style.css">
<style fprolloverstyle>
A:hover {color: #00FFFF}
.small A:hover {color: #00FFFF}
</style>
<SCRIPT src="common.js"></SCRIPT>
<script language="JavaScript" type="text/javascript" src="share.js"></script>
<script language="JavaScript" type="text/javascript" src="capsetup.js"></script>
<script language="JavaScript" type="text/javascript" src="capwireless.js"></script>
<SCRIPT language=javascript>
document.title=(bmenu.wireless);
<% wireless_support(5); %>share.productname = share.productname1;<% wireless_support(6); %>
var win_options = 'alwaysRaised, resizable, scrollbars,width=660,height=460';
var EN_DIS = '<% nvram_get("wl_macmode"); %>';
var active_win = null;

function closeWin(win_var)
{
	if ((win_var != null) && (win_var.close) || ((win_var != null) && (win_var.closed==false)))
		win_var.close();
}
function SelMac(num, F)
{
	F.submit_button.value = "Wireless_MAC";
	F.change_action.value = "gozila_cgi";
	F.wl_macmode1.value = F.wl_macmode1.value;
	F.submit();
}
function showClientMacList()
{
	active_win = self.open('WL_FilterTable.asp', 'ClientMacList', 'alwaysRaised,resizable, scrollbars, width=600, height=450');
	active_win.focus();
}
function to_submit(F)
{
	F.submit_button.value = "Wireless_MAC";
	F.action.value = "Apply";
	F.submit(); 
}
function valid_macs_all(I)
{
	if(I.value == "")
		return true;
	else if(I.value.length == 12)
		valid_macs_12(I);
	else if(I.value.length == 17)
		valid_macs_17(I);
	else
	{
		//alert('The MAC Address length is not correct!!');
		alert(errmsg.err31);

		I.value = I.defaultValue;
	}
}
function exit()
{
	closeWin(active_win);
}
</SCRIPT>

</HEAD>
<BODY onload={window.focus();} onunload=exit()>
<DIV align=center>
<form name=wireless method=<% get_http_method(); %> action=apply.cgi>
<input type=hidden name=submit_button>
<input type=hidden name=submit_type>
<input type=hidden name=change_action>
<input type=hidden name=action>
<!-- <input type=hidden name=small_screen> -->
<TABLE cellSpacing=0 cellPadding=0 width=809 border=0>
<TBODY><TR><TD width=95><IMG height=57 src='image/UI_Linksys.gif' width=165 border=0></TD>
<TD vAlign=bottom align=right width=714 bgColor=#6666cc>
<FONT style='FONT-SIZE: 7pt' color=white face=Arial><script>Capture(share.firmwarever)</script>: <% get_firmware_version(); %>&nbsp;&nbsp;&nbsp;</FONT>
</TD></TR>
<TR><TD width=808 colSpan=2 bgColor=#6666cc>
<IMG height=11 src='image/UI_10.gif' width=809 border=0></TD></TR>
</TBODY></TABLE>
<TABLE height=77 cellSpacing=0 cellPadding=0 width=809 bgColor=black border=0>
<TBODY>
<TR><TD borderColor=black height=49 width=163 align=middle>
<H3 style='MARGIN-TOP: 1px; MARGIN-BOTTOM: 1px'>
<FONT style='FONT-SIZE: 15pt' face=Arial color=white><script>Capture(bmenu.wireless)</script></FONT></H3></TD>
<TD vAlign=center borderColor=black width=646 bgColor=black height=49>
<TABLE style='FONT-SIZE: 10pt; COLOR: black; BORDER-COLLAPSE: collapse;' height=33 cellSpacing=0 cellPadding=0 bgColor=#6666cc border=0>
<TBODY><TR><TD style='font-size:10pt; font-weight:bolder' bgColor=#6666cc height=33 align=right><FONT color='#ffffff'><% wireless_support(1); %><script>Capture(share.wireless_g)</script><% wireless_support(2); %><script>Capture(share.productname)</script>&nbsp;&nbsp;
<TD align=middle borderColor=#000000 borderColorLight=#000000 width=109 bgColor=#000000 borderColorDark=#000000 height=12 rowSpan=2>
<FONT color=#ffffff><SPAN style='FONT-SIZE: 8pt'><B><% nvram_get("router_name"); %></B></SPAN></FONT></TD></FONT></TD></TR>
<TR><TD style='FONT-SIZE: 1pt; COLOR: black;' width=537 bgColor=black height=1>&nbsp;</TD></TR>
        <TR>
          <TD width=646 bgColor=#000000 colSpan=2 height=32>
            <TABLE id=AutoNumber1 
            style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; BORDER-COLLAPSE: collapse; FONT-VARIANT: normal" 
            height=6 cellSpacing=0 cellPadding=0 width=637 border=0>
              <TBODY>
              <TR 
              style="BORDER-RIGHT: medium none; BORDER-TOP: medium none; FONT-WEIGHT: normal; FONT-SIZE: 1pt; BORDER-LEFT: medium none; COLOR: black; BORDER-BOTTOM: medium none; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
              align=middle bgColor=#6666cc>
                <TD width=83  height=1><IMG height=10 src="image/UI_06.gif" width=83  border=0></TD>
                <TD width=83  height=1><IMG height=10 src="image/UI_07.gif" width=83  border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=68  height=1><IMG height=10 src="image/UI_06.gif" width=68  border=0></TD></TR>
              <TR>
                <TD align=middle bgColor=#000000 height=20><FONT 
                  style="FONT-WEIGHT: 700" color=#ffffff><A 
                  style="TEXT-DECORATION: none" 
                  href="index.asp"><script>Capture(bmenu.setup)</script></A></FONT></TD>
                <TD align=middle bgColor=#6666cc height=20><FONT 
                  style="FONT-WEIGHT: 700" color=#ffffff><script>Capture(bmenu.wireless)</script></FONT></TD>
                <TD align=middle bgColor=#000000 height=20><FONT 
                  style="FONT-WEIGHT: 700" color=#ffffff>
                <a style="TEXT-DECORATION: none" href="Firewall.asp"><script>Capture(bmenu.security)</script></a></FONT></TD>
                <TD align=middle bgColor=#000000 height=20><FONT 
                  style="FONT-WEIGHT: 700" color=#ffffff>
                <a style="TEXT-DECORATION: none" href="<% support_elsematch("PARENTAL_CONTROL_SUPPORT", "1", "Parental_Control.asp", "Filters.asp"); %>"><script>Capture(bmenu.accrestriction)</script></a></FONT></TD>
                <TD align=middle bgColor=#000000 height=20>
                  <P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" 
                  color=#ffffff>
                  <a style="TEXT-DECORATION: none" href="Forward_UPnP.asp"><script>Capture(bmenu.application)</script> 
                  <BR>&amp; <script>Capture(bmenu.gaming)</script></a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>
                <TD align=middle bgColor=#000000 height=20>
                  <P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" 
                  color=#ffffff>
                  <a style="TEXT-DECORATION: none" href="Management.asp">&nbsp;&nbsp;<script>Capture(bmenu.admin)</script></a>&nbsp;&nbsp;</FONT></P></TD>
                <TD align=middle bgColor=#000000 height=20>
                  <P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" 
                  color=#ffffff>
                  <a style="TEXT-DECORATION: none" href="Status_Router.asp"><script>Capture(bmenu.statu)</script></a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>
              </TR>
              <TR>
                <TD width=643 bgColor=#6666cc colSpan=7 height=21>
                  <TABLE borderColor=black height=21 cellSpacing=0 cellPadding=0>
                    <TBODY>
                    <TR align=center>
                      <TD class=small width=165><a href="Wireless_Basic.asp"><script>Capture(wltopmenu.basicsetup)</script></a></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=165><a href="WL_WPATable.asp"><script>Capture(wltopmenu.wlsecurity)</script></a></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=165><FONT style="COLOR: white"><script>Capture(wltopmenu.wlaccess)</script></FONT></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=210><a href="Wireless_Advanced.asp"><script>Capture(wltopmenu.advwlsetting)</script></a></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>
<TABLE height=5 cellSpacing=0 cellPadding=0 width=806 bgColor=black border=0>
<TR bgColor=black>
<TD width=163 height=1><img border=0 src=image/UI_03.gif width=164 height=15></TD>
<TD width=646 height=1><img border=0 src=image/UI_02.gif width=645 height=15></TD>
</TR>
</TABLE>


<table border=0 cellpadding=0 cellspacing=0 width=809 height=72>
<tr>
<td width=633>
<table border=0 cellpadding=0 cellspacing=0>
<tr>
<td width=156 height=25 bgcolor=#000000 colspan=3><p align=right><b><font face=Arial color=#FFFFFF style=font-size: 9pt><script>Capture(wlleftmenu.wlnetaccess)</script></font></b></td>
<td width=8 height=25 bgcolor=#000000>&nbsp;</td>
<td width=16 height=5></td>
<td width=12 height=5></td>
<td width=69 height=5></td>
<td width=124 height=5></td>
<td width=218 height=5></td>
<td width=15 height=5></td>
<TD width=15 ><IMG src='image/UI_05.gif' border=0 width=15 height=35></TD>
</tr>
<tr>
<TD width=156 colspan=3 height=25 bgcolor=#E7E7E7>&nbsp;</TD>
<TD width=8 ><IMG src='image/UI_04.gif' border=0 width=8 height=35></TD>
<TD width=28 colspan=2>&nbsp;</TD>

<td width=416 height=1 colspan=4><input type=radio name=wl_macmode1  value=disabled onClick=SelMac('disabled',document.forms[0]) <% nvram_selmatch("wl_macmode1", "disabled", "checked"); %>><b><script>Capture(wlnetwork.allowall)</script></b></font></td>
<TD width=15 ><IMG src='image/UI_05.gif' border=0 width=15 height=35></TD>
</tr>
<tr>
<TD width=156 colspan=3 height=25 bgcolor=#E7E7E7>&nbsp;</TD>
<TD width=8 ><IMG src='image/UI_04.gif' border=0 width=8 height=35></TD>
<TD width=28 colspan=2>&nbsp;</TD>
<td width=416 height=1 colspan=4><input onClick=SelMac('other',document.forms[0]) type=radio value=other name=wl_macmode1 <% nvram_selmatch("wl_macmode1", "other", "checked"); %>><b><script>Capture(wlnetwork.restrictacc)</script></b></font></td>
<TD width=15 ><IMG src='image/UI_05.gif' border=0 width=15 height=35></TD>
</tr>
</table>



<div ID='adv_list' >
<TABLE cellSpacing=0 cellPadding=0 border=0>
<tr>
<TD width=156 colspan=3 height=25 bgcolor=#E7E7E7>&nbsp;</TD>
<TD width=8 ><IMG src='image/UI_04.gif' border=0 width=8 height=35></TD>
<TD width=28 colspan=2>&nbsp;</TD>
<!--wwzh -->
<% nvram_selmatch("wl_macmode1", "disabled", "<TD width=416 colspan=4>&nbsp;</TD> <!--"); %>
<td width=416 height=1 colspan=4><input type=radio name="wl_macmode"  value="deny" <% nvram_invmatch("wl_macmode", "allow", "checked"); %>><b><script>Capture(wlnetwork.prevent)</script> </b><script>Capture(wlnetwork.preinfo)</script></font></td> 
<% nvram_selmatch("wl_macmode1", "disabled", "-->"); %>

<TD width=15 ><IMG src='image/UI_05.gif' border=0 width=15 height=35></TD>
</tr>
<tr>
<TD width=156 colspan=3 height=25 bgcolor=#E7E7E7>&nbsp;</TD>
<TD width=8 ><IMG src='image/UI_04.gif' border=0 width=8 height=35></TD>
<TD width=28 colspan=2>&nbsp;</TD>

<!-- wwzh -->
<% nvram_selmatch("wl_macmode1", "disabled", "<TD width=416 colspan=4>&nbsp;</TD> <!--"); %>

<td width=416 height=1 colspan=4><input type=radio name="wl_macmode" value="allow" <% nvram_match("wl_macmode", "allow", "checked"); %>><b><script>Capture(wlnetwork.permitonly)</script> </b><script>Capture(wlnetwork.perinfo)</script></font></td> 
<% nvram_selmatch("wl_macmode1", "disabled", "-->"); %>


<TD width=15 ><IMG src='image/UI_05.gif' border=0 width=15 height=35></TD>
</tr>
<tr>
<TD width=156 colspan=3 height=25 bgcolor=#E7E7E7>&nbsp;</TD>
<TD width=8 ><IMG src='image/UI_04.gif' border=0 width=8 height=35></TD>
<TD width=28 colspan=2>&nbsp;</TD>
<td width=416 height=1 colspan=4>&nbsp;</td>
<TD width=15 ><IMG src='image/UI_05.gif' border=0 width=15 height=35></TD>
</tr>

<!-- wwzh 0325
<tr>
<TD width=156 colspan=3 height=25 bgcolor=#E7E7E7>&nbsp;</TD>
<TD width=8 ><IMG src='image/UI_04.gif' border=0 width=8 height=300></TD>
<TD width=28 colspan=2>&nbsp;</TD>
<td colspan=4 align=center>
<table border=0 cellpadding=0 cellspacing=0>


</table>
</td>
<TD width=15 ><IMG src='image/UI_05.gif' border=0 width=15 height=300></TD>
</tr>

-->

<tr>
<TD width=156 colspan=3 height=25 bgcolor=#E7E7E7>&nbsp;</TD>
<TD width=8 ><IMG src='image/UI_04.gif' border=0 width=8 height=35></TD>
<td width=16 height=5></td>
<td width=12 height=5></td>
<td width=69 height=5></td>
<td width=124 height=5></td>
<td width=218 height=5></td>
<td width=15 height=5></td>
<TD width=15 ><IMG src='image/UI_05.gif' border=0 width=15 height=35></TD>
</tr>

<TR>
<TD width=156 colspan=3 height=25 bgcolor=#E7E7E7>&nbsp;</TD>
<TD width=8 ><IMG src='image/UI_04.gif' border=0 width=8 height=25></TD>

<!-- wwzh  -->

<% nvram_selmatch("wl_macmode1", "disabled", "<TD width=454 colspan=6 height=25>&nbsp;</TD> <!--"); %>

<TD colSpan=6 width=454 align=center>
<TABLE border=0 cellPadding=0 cellSpacing=0 height=19 width=337>
<TR><TD align=middle bgColor=#999999 width=337>
<B><A href='javascript:showClientMacList();'><script>Capture(wlnetwork.editmaclist)</script></A></B>
</TD></TR>
</TABLE></TD>
<% nvram_selmatch("wl_macmode1", "disabled", "-->"); %>

<TD width=15 ><IMG src='image/UI_05.gif' border=0 width=15 height=25></TD>
</TR>
</table>
</div>
<TABLE cellSpacing=0 cellPadding=0 border=0>

<tr>
<TD width=156 colspan=3 height=25 bgcolor=#E7E7E7>&nbsp;</TD>
<TD width=8 ><IMG src='image/UI_04.gif' border=0 width=8 height=25></TD>
<TD width=28 colspan=2>&nbsp;</TD>
<td width=411 height=1 colspan=3>&nbsp;</td>
<td width=15 height=25>&nbsp;</td>
<TD width=15 ><IMG src='image/UI_05.gif' border=0 width=15 height=25></TD>
</tr>
</table></td>


<TD vAlign=top width=176 bgColor=#6666cc>
<TABLE cellSpacing=0 cellPadding=0 width=176 border=0>
<TR>
<TD width=11 bgColor=#6666cc height=25>&nbsp;</TD>
<TD width=156 bgColor=#6666cc height=25><FONT color=#ffffff><a target="_blank" href="help/HWireless.asp"><script>Capture(share.more)</script>...</a></FONT></TD>
<TD width=9 bgColor=#6666cc height=25>&nbsp;</TD></TR></TABLE></TD></TR>

<tr>
<td width=809 colspan=2>
<table border=0 cellpadding=0 cellspacing=0>
<TR>
<TD width=156 height=30 bgcolor=#E7E7E7>&nbsp;</TD>
<TD width=8 ><IMG src='image/UI_04.gif' border=0 width=8 height=30></TD>
<TD width=28 colspan=2>&nbsp;</TD>
<TD width=411>&nbsp;</TD>
<TD width=15>&nbsp;</TD>
<TD width=15 ><IMG src='image/UI_05.gif' border=0 width=15 height=30></TD>
<TD width=176 height=58 bgcolor=#6666CC rowspan=2><img border=0 src=image/UI_Cisco.gif width=176 height=64></TD>
</TR>
<tr>
<td width=156 bgcolor=#000000>&nbsp;</td>
<td width=8 bgcolor=#000000>&nbsp;</td>
<td width=28 colspan=2 bgcolor=#6666CC>&nbsp;</td>
<td width=411 bgcolor=#6666CC>
<table border=0 cellpadding=0 cellspacing=0  width=220 align=right height=19>
<!-- wwzh
<script language=javascript>init(document.F1);</script>
-->

<TR>
<TD align=middle bgColor=#42498c width=101><FONT color=#ffffff>
<B><A href='javascript:to_submit(document.forms[0])'><script>Capture(share.saves)</script></A></B></FONT></TD>
<TD align=middle width=8 bgColor=#6666cc>&nbsp;</TD>
<TD align=middle width=103 bgColor=#434a8f><FONT color=#ffffff>
<B><A href=Wireless_MAC.asp><script>Capture(share.cancels)</script></A></B></FONT></TD>
</TR>
</table>
</td>
<td width=15 height=33 bgcolor=#6666CC>&nbsp;</td>
<td width=15 height=33 bgcolor=#000000>&nbsp;</td>
</tr>
</table></td>
</tr>
</table>
</form></DIV>    
</body>

</html>

