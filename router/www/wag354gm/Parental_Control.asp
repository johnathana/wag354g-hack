<!--
*********************************************************
*   Copyright 2003, CyberTAN  Inc.  All Rights Reserved *
*********************************************************

This is UNPUBLISHED PROPRIETARY SOURCE CODE of CyberTAN Inc.
the contents of this file may not be disclosed to third parties,
copied or duplicated in any form without the prior written
permission of CyberTAN Inc.

This software should be used as a reference only, and it not
intended for production use!


THIS SOFTWARE IS OFFERED "AS IS", AND CYBERTAN GRANTS NO WARRANTIES OF ANY
KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.  CYBERTAN
SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE
-->

<html><head><title>Parental Control</title>
<% no_cache(); %>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<link rel="stylesheet" type="text/css" href="style.css">
<style fprolloverstyle>
A:hover {color: #00FFFF}
.small A:hover {color: #00FFFF}
</style>
<SCRIPT src="common.js"></SCRIPT>
<SCRIPT language="javascript" type="text/javascript" src="share.js"></SCRIPT>
<SCRIPT language="Javascript" type="text/javascript" src="capaccess.js"></SCRIPT>
<Script Language="JavaScript">
document.title=(acctopmenu.parctrl);
<% wireless_support(5); %>share.productname = share.productname1;<% wireless_support(6); %>
function to_submit(F)
{       
        F.submit_button.value = "Parental_Control";
        F.action.value = "Apply";
        F.submit();
}
function SelMode(num,F)
{
        F.submit_button.value = "Parental_Control";
        F.change_action.value = "gozila_cgi";
	F.artemis_enable.value = F.artemis_enable.value;
        F.submit();
}

</script>

</head>

<BODY>
<DIV align=center>
<FORM name=filters method=<% get_http_method(); %> action=apply.cgi>
<input type=hidden name=submit_button>
<input type=hidden name=change_action>
<input type=hidden name=submit_type>
<input type=hidden name=action>

<TABLE cellSpacing=0 cellPadding=0 width=809 border=0>
  <TBODY>
  <TR>
    <TD width=95><IMG src="image/UI_Linksys.gif" 
      border=0 width="165" height="57"></TD>
    <TD vAlign=bottom align=right width=714 bgColor=#6666cc><FONT 
      style="FONT-SIZE: 7pt" color=#ffffff><FONT face=Arial><script>Capture(share.firmwarever)</script>:&nbsp;<% get_firmware_version(); %>&nbsp;&nbsp;&nbsp;</FONT></FONT></TD></TR>
  <TR>
    <TD width=808 bgColor=#6666cc colSpan=2><IMG height=11 
      src="image/UI_10.gif" width=809 border=0></TD></TR></TBODY></TABLE>
<TABLE height=77 cellSpacing=0 cellPadding=0 width=809 bgColor=black border=0>
  <TBODY>
  <TR>
    <TD 
    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
    borderColor=#000000 align=middle width=163 height=49>
      <H3 style="MARGIN-TOP: 1px; MARGIN-BOTTOM: 1px"><FONT 
      style="FONT-SIZE: 15pt" face=Arial color=#ffffff><script>Capture(bmenu.accrestriction)</script></FONT></H3></TD>
    <TD 
    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
    vAlign=center borderColor=#000000 width=646 bgColor=#000000 height=49>
      <TABLE 
      style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; BORDER-COLLAPSE: collapse; FONT-VARIANT: normal" 
      height=33 cellSpacing=0 cellPadding=0 bgColor=#6666cc border=0>
        <TBODY>
        <TR>
          <TD style="FONT-WEIGHT: bolder; FONT-SIZE: 10pt" align=right 
          bgColor=#6666cc height=33><FONT color=#ffffff><span><% wireless_support(1); %><script>Capture(share.wireless_g)</script><% wireless_support(2); %><script>Capture(share.productname)</script>&nbsp;&nbsp;</span></FONT></TD>
          <TD borderColor=#000000 borderColorLight=#000000 align=middle 
          width=109 bgColor=#000000 borderColorDark=#000000 height=12 
            rowSpan=2><FONT color=#ffffff><SPAN 
            style="FONT-SIZE: 8pt"><B><% nvram_get("router_name"); %></B></SPAN></FONT></TD></TR>
        <TR>
          <TD 
          style="FONT-WEIGHT: normal; FONT-SIZE: 1pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
          width=537 bgColor=#000000 height=1>&nbsp;</TD></TR>
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
                <TD width=83  height=1><IMG height=10 src="image/UI_06.gif" width=83  border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_07.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=68  height=1><IMG height=10 src="image/UI_06.gif" width=68  border=0></TD></TR>
              <TR>
                <TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><A style="TEXT-DECORATION: none" href="index.asp"><script>Capture(bmenu.setup)</script></A></FONT></TD>
 <% wireless_support(1); %><TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Wireless_Basic.asp"><script>Capture(bmenu.wireless)</script></a></FONT></TD>
 <% wireless_support(2); %>
               <TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Firewall.asp"><script>Capture(bmenu.security)</script></a></FONT></TD>
                <TD align=middle bgColor=#6666cc height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><script>Capture(bmenu.accrestriction)</script></a></FONT></TD>
                <TD align=middle bgColor=#000000 height=20><P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Forward_UPnP.asp"><script>Capture(bmenu.application)</script><BR>&amp;<script>Capture(bmenu.gaming)</script></a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>
                <TD align=middle bgColor=#000000 height=20><P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Management.asp">&nbsp;&nbsp;<script>Capture(bmenu.admin)</script></a>&nbsp;&nbsp;</FONT></P></TD>
                <TD align=middle bgColor=#000000 height=20><P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Status_Router.asp"><script>Capture(bmenu.statu)</script></a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>
              </TR>
            <tr>
              <td width="643" style="border-style: none; border-width: medium; font-style:normal; font-variant:normal; font-weight:normal; font-size:10pt; font-family:Arial, Helvetica, sans-serif; color:black" bgcolor="#6666CC" height="21" colspan="7">
              <table border="0" cellpadding="0" cellspacing="0" style="border-collapse: collapse" bordercolor="#111111" id="AutoNumber12" height="21">
                <TR align=center>
                      <TD width=20></TD>
                      <TD class=small width=100><FONT style="COLOR: white"><script>Capture(acctopmenu.parctrl)</script></FONT></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD width=25></TD>
                      <TD class=small width=90><% nvram_else_match("artemis_enable", "1","Internet Access","<A href=Filters.asp><script>Capture(acctopmenu.inetacc)</script></A>"); %></TD>
                      <TD><P class=bar>&nbsp;</P></TD>
                      <TD width=22></TD>
<% support_match("FIREWALL_LEVEL_SUPPORT", "1", "
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=90><A href='Filters_inb.asp'><script>Capture(acctopmenu.inbtraffic)</script></A></TD>
                      <TD><P class=bar>&nbsp;</P></TD>
                      <TD width=22></TD>"); %>
                      <TD class=small width=153>&nbsp;</TD>
                      <TD><P class=bar>&nbsp;</P></TD>
                      <TD width=30></TD>
                      <TD class=small width=140>&nbsp;</TD>
                    </TR>
              </table>
              </td>
            </tr>
          </table>
          </TD></TR></TBODY></TABLE></TD></TR>
  </TABLE>

   <TABLE height=5 cellSpacing=0 cellPadding=0 width=806 bgColor=black border=0>  
  <TR bgColor=black>
    <TD width=163 height=1 style="font-style: normal; font-variant: normal; font-weight: normal; font-size: 10pt; font-family: Arial, Helvetica, sans-serif; color: black" bgcolor="#E7E7E7" bordercolor="#E7E7E7">
			<img border="0" src="image/UI_03.gif" width="164" height="15"></TD>
    <TD width=646 height=1 style="font-style: normal; font-variant: normal; font-weight: normal; font-size: 10pt; font-family: Arial, Helvetica, sans-serif; color: black" bgcolor="#FFFFFF">
			<img border="0" src="image/UI_02.gif" width="645" height="15"></TD></TR>
  </TABLE>


  <table border="0" cellpadding="0" cellspacing="0" style="border-collapse: collapse" bordercolor="#111111" id="AutoNumber9" width="809" height="23">
    <tr><td width=633><table border="0" cellpadding="0" cellspacing="0">           
              <tr>
                <td width="156" height="25" bgcolor="#000000" colspan="3">
                <p align="right"><b>
                <font face="Arial" color="#FFFFFF" style="font-size: 9pt"><script>Capture(accleftmenu.signup)</script></font></b></td>
                <td width="8" height="25" bgcolor="#000000">
                &nbsp;</td>
                <td width="14" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="17" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="13" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="101" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="296" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="13" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="15" height="25" bgcolor="#FFFFFF">
                <img border="0" src="image/UI_05.gif" height="25" width="15"></td>
              </tr>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=50 src="image/UI_04.gif" width=8 border=0></TD>
          <TD width=14 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD width=17 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD width=410 bgColor=#ffffff colSpan=3 height=25><INPUT type=radio value=1 name=artemis_enable <% nvram_selmatch("artemis_enable", "1", "checked"); %>><B><script>Capture(share.enabled)</script></B>&nbsp;&nbsp;
                <INPUT type=radio value=0 name=artemis_enable <% nvram_selmatch("artemis_enable", "0", "checked"); %>><B><script>Capture(share.disabled)</script></B></TD>
          <TD width=13 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD width=15 bgColor=#ffffff height=25><IMG height=50 src="image/UI_05.gif" width=15 border=0></TD></TR>
<% nvram_selmatch("artemis_enable", "0", "<!--"); %>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=30 src="image/UI_04.gif" width=8 border=0></TD>
          <TD width=14 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD width=17 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD width=410 bgColor=#ffffff colSpan=3 height=25><b><script>Capture(accsignup.msg1)</script></b></TD>
          <TD width=13 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD width=15 bgColor=#ffffff height=25><IMG height=30 src="image/UI_05.gif" width=15 border=0></TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=80>&nbsp;</TD>
          <TD width=8 height=80><IMG height=80 src="image/UI_04.gif" width=8 border=0></TD>
          <TD width=14 bgColor=#ffffff height=80>&nbsp;</TD>
          <TD width=17 bgColor=#ffffff height=80>&nbsp;</TD>
          <TD width=13 bgColor=#ffffff height=80>&nbsp;</TD>
          <TD width=397 bgColor=#ffffff colSpan=2 height=80><li><script>Capture(accsignup.msg1)</script></li><br><li><script>Capture(accsignup.msg2)</script></li><br><li><script>Capture(accsignup.msg3)</script></li></TD>
          <TD width=13 bgColor=#ffffff height=80>&nbsp;</TD>
          <TD width=15 bgColor=#ffffff height=80><IMG height=80 src="image/UI_05.gif" width=15 border=0></TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=30 src="image/UI_04.gif" width=8 border=0></TD>
          <TD width=14 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD width=17 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD bgColor=#ffffff colSpan=3 height=25><input type=button value="Sign up for Parental Control service" onclick="self.open('http://pcsvc.ourlinksys.com/us/trial.asp','','alwaysRaised,resizable,scrollbars,titlebar,menubar,toolbar,status,width=640,height=480');">&nbsp;&nbsp;&nbsp;<input type=button value="More Info" onclick="self.open('http://www.linksys.com/pcsvc/info.asp','','alwaysRaised,resizable,scrollbars,titlebar,menubar,toolbar,status,width=800,height=600');"></TD>
          <TD width=13 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD width=15 bgColor=#ffffff height=25><IMG height=30 src="image/UI_05.gif" width=15 border=0></TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=30 src="image/UI_04.gif" width=8 border=0></TD>
          <TD width=14 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD width=17 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD width=410 bgColor=#ffffff colSpan=3 height=25><script>Capture(accsignup.msg5)</script></TD>
          <TD width=13 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD width=15 bgColor=#ffffff height=25><IMG height=30 src="image/UI_05.gif" width=15 border=0></TD></TR>
              
              <tr>
                <td width="156" height="1" bgcolor="#E7E7E7" colspan=3>&nbsp;</td>  
                <td width="8" height="1">
                <font face="Arial">
                <img border="0" src="image/UI_04.gif" width="8" height="30"></font></td>
                <td colspan=6><table><tr>
                <td width="16" height="1" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="13" height="1" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="410" height="1" bgcolor="#FFFFFF" colspan="3">
                <hr color="#B5B5E6" size="1"></td>
                <td width="15" height="1" bgcolor="#FFFFFF">&nbsp;</td>
                </tr></table></td>
                <td width="15" height="1" bgcolor="#FFFFFF">
                <img border="0" src="image/UI_05.gif" height="30" width="15"></font></td>
              </tr>
           <tr>
                <td width="156" height="25" bgcolor="#000000" colspan="3">
                <p align="right"><b>
                <font face="Arial" color="#FFFFFF" style="font-size: 9pt">
                Manage Account</font></b></td>
                <td width="8" height="25" bgcolor="#000000">
                &nbsp;</td>
                <td width="14" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="17" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="13" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="101" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="296" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="13" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="15" height="25" bgcolor="#FFFFFF">
                <img border="0" src="image/UI_05.gif" height="25" width="15"></td>
              </tr>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=30 src="image/UI_04.gif" width=8 border=0></TD>
          <TD width=14 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD width=17 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD width=410 bgColor=#ffffff colSpan=3 height=25><b><script>Capture(accsignup.msg6)</script></b></TD>
          <TD width=13 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD width=15 bgColor=#ffffff height=25><IMG height=30 src="image/UI_05.gif" width=15 border=0></TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=30 src="image/UI_04.gif" width=8 border=0></TD>
          <TD width=14 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD width=17 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD width=13 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD width=397 bgColor=#ffffff colSpan=2 height=25><script>Capture(accinetacc.status)</script>:&nbsp;&nbsp;&nbsp;<% nvram_else_match("artemis_provisioned", "1","device is provisioned","device not provisioned"); %></TD>
          <TD width=13 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD width=15 bgColor=#ffffff height=25><IMG height=30 src="image/UI_05.gif" width=15 border=0></TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=30 src="image/UI_04.gif" width=8 border=0></TD>
          <TD width=14 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD width=17 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD width=13 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD width=397 bgColor=#ffffff colSpan=2 height=25><input type=button value="Manage Account" onclick="self.open('http://pcsvc.ourlinksys.com/us/admin.asp','','alwaysRaised,resizable,scrollbars,titlebar,menubar,toolbar,status,width=800,height=600');"></TD>
          <TD width=13 bgColor=#ffffff height=25>&nbsp;</TD>
          <TD width=15 bgColor=#ffffff height=25><IMG height=30 src="image/UI_05.gif" width=15 border=0></TD></TR>
<% nvram_selmatch("artemis_enable", "0", "-->"); %>
              <tr>
                <td width="44" height="5" bgcolor="#E7E7E7"></td>
                <td width="65" height="5" bgcolor="#E7E7E7"></td>
                <td width="47" height="5" bgcolor="#E7E7E7"></td>
                <td width="8" height="5">
                <font style="font-size: 2pt" face="Arial">
                <img border="0" src="image/UI_04.gif" width="8" height="5"></font></td>
                <td width="14" height="5" bgcolor="#FFFFFF"></td>
                <td width="17" height="5" bgcolor="#FFFFFF"></td>
                <td width="13" height="5" bgcolor="#FFFFFF"></td>
                <td width="101" height="5" bgcolor="#FFFFFF"></td>
                <td width="296" height="5" bgcolor="#FFFFFF"></td>
                <td width="13" height="5" bgcolor="#FFFFFF"></td>
                <td width="15" height="5" bgcolor="#FFFFFF">
                <img border="0" src="image/UI_05.gif" height="5" width="15"></font></td>
              </tr>
              </table></td>
              
              <td width="176" valign=top bgcolor=#6666CC><table border="0" cellpadding="0" cellspacing="0" width="176"><tr>
                <td width="11" height="25" bgcolor="#6666CC">&nbsp;</td>
                <td width="156" height="25" bgcolor="#6666CC">
                <font color="#FFFFFF">
          <span ><!--a target="_blank" href="help/HFilters.asp">More...</a--></span></font></td>
                <td width="9" height="25" bgcolor="#6666CC">&nbsp;</td>
              </tr></table></td>
              
              </tr>

	      <tr>
                <td width="809" colspan=2><table border="0" cellpadding="0" cellspacing="0">
		<tr> 
		<td width="156" height="25" bgcolor="#E7E7E7">&nbsp;</td>
                <td width="8" height="25">
                <font style="font-size: 2pt" face="Arial">
                <img border="0" src="image/UI_04.gif" width="8" height="30"></font></td>
                <td width="16" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="12" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="411" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="15" height="25" bgcolor="#FFFFFF">&nbsp;</TD>
                <td width="15" height="25" bgcolor="#FFFFFF">
                <font face="Arial">
                <img border="0" src="image/UI_05.gif" height="30" width="15"></font></td>
                <td width="176" height="58" bgcolor="#6666CC" rowspan="2">
                <img border="0" src="image/UI_Cisco.gif" width="176" height="64"></td>
		</tr>
		<tr>
		<td width="156" height="33" bgcolor="#000000">&nbsp;</td>
                <td width="8" height="33" bgcolor="#000000">&nbsp;</td>
                <td width="16" height="33" bgcolor="#6666CC">&nbsp;</td>
                <td width="12" height="33" bgcolor="#6666CC">&nbsp;</td>
                <td width="411" height="33" bgcolor="#6666CC">
            <TABLE height=19 cellSpacing=0 cellPadding=0 align=right border=0 width="220">
              <TR>
                <TD align=middle bgColor=#42498c width="101"><FONT color=#ffffff><B><A href='javascript:to_submit(document.filters)'><script>Capture(share.saves)</script></A></B></FONT>
                </TD>
                <TD align=middle width=4 bgColor=#6666cc>&nbsp;</TD>
               <!-- <TD align=middle bgColor=#434a8f width="103"><FONT color=#ffffff><B><A href='javascript:window.location.reload()'>Cancel Changes</A></B></FONT></TD> -->
               <TD align=middle bgColor=#434a8f width="111"><FONT color=#ffffff><B><A href=Parental_Control.asp><script>Capture(share.cancels)</script></A></B></FONT></TD> 
                <TD align=middle width=4 bgColor=#6666cc>&nbsp;</TD>
                </TR></TABLE>
                </td>
                <td width="10" height="33" bgcolor="#6666CC">&nbsp;</td>
                <td width="15" height="33" bgcolor="#000000">&nbsp;</td>
		</tr>
		</table></td>
              </tr>
              </table>
    		</form></div>
            </body>

</html>
