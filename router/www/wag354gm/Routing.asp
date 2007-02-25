
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

<HTML><HEAD><TITLE>Routing</TITLE>
<% no_cache(); %>
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
<script language="JavaScript" type="text/javascript" src="help.js"></script>
<SCRIPT language=javascript>
//document.title=(hrouting.phase1);
document.title=(settopmenu.advrouting);
<% wireless_support(5); %>share.productname = share.productname1;<% wireless_support(6); %>
var route_win = null;
function ViewRoute()
{
	route_win = self.open('RouteTable.asp', 'Route', 'alwaysRaised,resizable,scrollbars,width=720,height=600');
	route_win.focus();
}
<% support_elsematch("MULTIPVC_SUPPORT", "1", "", "/*"); %>     	
function ViewPvcRoute()
{
	var win = self.open('PVC_Selection.asp','PVCRouting','alwaysRaised,resizable,scrollbars,width=screen.availWidth,height=screen.availHeight');
	win.focus();
}
<% support_elsematch("MULTIPVC_SUPPORT", "1", "", "*/"); %>     	
function DeleteEntry(F)
{
//	if(confirm("Delete the Entry?")){
	if(confirm(errmsg.err122)){
		F.submit_button.value = "Routing";
		F.change_action.value = "gozila_cgi";
		F.submit_type.value = 'del';
		F.submit();
	}
}
function SelPvcConnection(num,F)
{
        F.submit_button.value = "Routing";
        F.change_action.value = "gozila_cgi";
        F.rip_interface.value=F.rip_interface.options[num].value;
        F.submit();
}

function to_submit(F)
{
	if(valid_value(F)){
		F.submit_button.value = "Routing";
		F.action.value = "Apply";
		F.submit();
	}
}

function allzeroip(F, N)
{
	var m = new Array(4);
	for(i=0;i<4;i++)
		m[i] = eval(N+"_"+i).value
	if(m[0] == "0" && m[1] == "0" && m[2] == "0" && m[3] == "0")
	{
		return true;	
	}else return false;
}

function valid_value(F)
{
//	if(!valid_ip(F,"F.route_ipaddr","Route IP",MASK_NO))
//	{
//	    F.route_ipaddr_0.focus();
//        return false;
//	}
	if (allzeroip(F,"F.route_ipaddr") && allzeroip(F,"F.route_netmask") && allzeroip(F,"F.route_gateway"))
		return true;
		
	if(!valid_range(F.route_ipaddr_0,1,223,"Route IP"))
	{
		F.route_ipaddr_0.focus();
		return false;
	}
	if(!valid_range(F.route_ipaddr_3,0,254,"Route IP"))
	{
		F.route_ipaddr_3.focus();
		return false;
	}
	if(!valid_mask(F,"F.route_netmask",BCST_NO))
	{
	    F.route_netmask_0.focus();
        return false;
	}

	if(!valid_ip(F,"F.route_gateway","Gateway",MASK_NO))
	{
		F.route_gateway_0.focus();
        return false;
	}
	if(!valid_range(F.route_gateway_0,0,223,"GateWay IP"))
	{
		F.route_gateway_0.focus();
		return false;
	}
	if(!valid_range(F.route_gateway_3,0,254,"GateWay IP"))
	{
		F.route_gateway_3.focus();
		return false;
	}
	return true;
}

function SelRoute(num,F)
{
	F.submit_button.value = "Routing";
	F.change_action.value = "gozila_cgi";
	F.route_page.value=F.route_page.options[num].value;
	F.submit();
}

function SelRIP(num, F)
{
   <% support_invmatch("RIP_EXTEND_SUPPORT", "1", "/*"); %>
   F.elements['rip_interface'].disabled = num;
   if (num){    
        choose_disable(F.SDroute_enable[0]);
        choose_disable(F.SDroute_enable[1]);
   }else{
        choose_enable(F.SDroute_enable[0]);
        choose_enable(F.SDroute_enable[1]);
    }
   F.elements['sel_MulticastBroadcast'].disabled = num;
   <% support_invmatch("RIP_EXTEND_SUPPORT", "1", "*/"); %>
   F.elements['sel_ripTX'].disabled = F.elements['sel_ripRX'].disabled = num;
}

function init()
{
	a = '<% nvram_get("rip_enable"); %>';
	if (a == '0')
		SelRIP(true, document.f1);
	else SelRIP(false, document.f1);
}

function exit()
{
	closeWin(route_win);
}
</SCRIPT>

</HEAD>
<BODY onload=init() onunload=exit()>
<DIV align=center>
<FORM name=f1 action=apply.cgi method=<% get_http_method(); %>>
<input type=hidden name=submit_button>
<input type=hidden name=submit_type>
<input type=hidden name=change_action>
<input type=hidden name=action>
<input type=hidden name=static_route>
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
      style="FONT-SIZE: 15pt" face=Arial color=#ffffff><script>Capture(bmenu.setup)</script></FONT></H3></TD>
    <TD 
    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
    vAlign=center borderColor=#000000 width=646 bgColor=#000000 height=49>
      <TABLE 
      style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; BORDER-COLLAPSE: collapse; FONT-VARIANT: normal" 
      height=33 cellSpacing=0 cellPadding=0 bgColor=#6666cc border=0>
        <TBODY>
        <TR>
          <TD style="FONT-WEIGHT: bolder; FONT-SIZE: 10pt" align=right 
          bgColor=#6666cc height=33><FONT color=#ffffff><% wireless_support(1); %><script>Capture(share.wireless_g)</script><% wireless_support(2); %><script>Capture(share.productname)</script>&nbsp;&nbsp;</FONT></TD>
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
                <TD width=83  height=1><IMG height=10 src="image/UI_07.gif" width=83  border=0></TD>
                <TD width=83  height=1><IMG height=10 src="image/UI_06.gif" width=83  border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=68  height=1><IMG height=10 src="image/UI_06.gif" width=68  border=0></TD></TR>
              <TR>
                <TD align=middle bgColor=#6666cc height=20><FONT 
                  style="FONT-WEIGHT: 700" color=#ffffff><A 
                  style="TEXT-DECORATION: none" 
                  href="index.asp"><script>Capture(bmenu.setup)</script></A></FONT></TD>
                <% wireless_support(1); %><TD align=middle bgColor=#000000 height=20><FONT 
                  style="FONT-WEIGHT: 700" color=#ffffff>
                <a style="TEXT-DECORATION: none" href="Wireless_Basic.asp"><script>Capture(bmenu.wireless)</script></a></FONT></TD><% wireless_support(2); %>
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
                      <TD class=small width=140><A href="index.asp"><script>Capture(settopmenu.basicsetup)</script></A></TD>
                      <TD><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=100><A href="DDNS.asp"><script>Capture(settopmenu.ddns)</script></A></TD>
                      <TD><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <!--<TD class=small width=153><A href="WanMAC.asp">MAC Address Clone</A></TD>
                      <TD><P class=bar><font color='white'><b>|</b></font></P></TD>
-->
                      <TD class=small width=140><FONT style="COLOR: white"><script>Capture(settopmenu.advrouting)</script></FONT></TD>
<% support_invmatch("DHCP_DOMAIN_IP_MAPPING_SUPPORT", "1", "<!--"); %>
                      <TD><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=100><A href="VOIP.asp"><script>Capture(settopmenu.voip)</script></A></TD>
<% support_invmatch("DHCP_DOMAIN_IP_MAPPING_SUPPORT", "1", "-->"); %>

<% support_invmatch("HSIAB_SUPPORT", "1", "<!--"); %>
                      <TD><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=120><A href="HotSpot_Admin.asp"><script>Capture(settopmenu.hotspot)</script></TD>
<% support_invmatch("HSIAB_SUPPORT", "1", "-->"); %>
</TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>
<TABLE height=5 cellSpacing=0 cellPadding=0 width=806 bgColor=black border=0>
  <TBODY>
  <TR bgColor=black>
    <TD 
    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
    borderColor=#e7e7e7 width=163 bgColor=#e7e7e7 height=1><IMG height=15 
      src="image/UI_03.gif" width=164 border=0></TD>
    <TD 
    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
    width=646 height=1><IMG height=15 src="image/UI_02.gif" width=645 
      border=0></TD></TR></TBODY></TABLE>
<TABLE id=AutoNumber9 style="BORDER-COLLAPSE: collapse" borderColor=#111111 
height=23 cellSpacing=0 cellPadding=0 width=809 border=0>
  <TBODY>
  <TR>
    <TD width=633>
      <TABLE cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD align=right width=156 bgColor=#000000 colSpan=3 
            height=25><B><FONT style="FONT-SIZE: 9pt" face=Arial 
            color=#ffffff><script>Capture(settopmenu.advrouting)</script></FONT></B></TD>
          <TD width=8 bgColor=#000000 height=25>&nbsp;</TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=15 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;</TD>
          <TD width=296 height=25>&nbsp;</TD>
          <TD width=13 height=25></TD>
          <TD width=15 background=image/UI_05.gif 
        height=25></TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>
            <P align=right><FONT style="FONT-WEIGHT: 700"><script>Capture(setleftmenu.operamode)</script></FONT></P></TD>
          <TD width=9 background=image/UI_04.gif height=33>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><script>Capture(setadvrouting.nat)</script>:&nbsp;<font size="1"></font></TD>
          <TD width=296 height=25><font face="Arial" size="1">
			<span style="font-size: 8pt">
<input type=radio name=nat_enable value=1 <% nvram_match("nat_enable","1","checked"); %>></span></font><span style="font-weight: 400">
            <font face="Arial"><b> </b></font><script>Capture(share.enabled)</script><font face="Arial"><b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
            </b></font></span><font face="Arial" size="1">
                        <span style="font-size: 8pt">
<input type=radio name=nat_enable value=0 <% nvram_match("nat_enable","0","checked"); %>></span></font><span style="font-weight: 400"><script>Capture(share.disabled)</script>
                        </span></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
        height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=1>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=1></TD>
          <TD colSpan=6>
            <TABLE>
              <TBODY>
              <TR>
                <TD width=16 height=1>&nbsp;</TD>
                <TD width=13 height=1>&nbsp;</TD>
                <TD width=410 colSpan=3 height=1>
                  <HR color=#b5b5e6 SIZE=1>
                </TD>
                <TD width=13>&nbsp;</TD></TR></TBODY></TABLE></TD>
          <TD width=15 background=image/UI_05.gif height=1></TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>
            <P align=right><FONT style="FONT-WEIGHT: 700"><script>Capture(setleftmenu.dynrouting)</script></FONT></P></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><span ><script>Capture(setadvrouting.rip)</script>:</span></TD>
          <TD width=296 height=25>
                        <span style="font-size: 8pt">
<input type=radio name=rip_enable value=1 <% nvram_match("rip_enable","1","checked"); %> onClick="SelRIP(false, this.form)"></span><span style="font-weight: 400">
        <font face="Arial"><b> </b></font><script>Capture(share.enabled)</script><b><font face="Arial">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
        </font></b></span><b><span style="font-size: 8pt"><font face="Arial" size="1">
<font face="Arial" size="1">
                        <span style="font-size: 8pt">
<input type=radio name=rip_enable value=0 <% nvram_match("rip_enable","0","checked"); %> onClick="SelRIP(true, this.form)"></span></font></font></span></b><span style="font-weight: 400"><script>Capture(share.disabled)</script><font face="Arial"><b>
        </b></font></span>
        <font face="Arial" size="1"><span style="font-size: 8pt">&nbsp;&nbsp;</span></font></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 >
          <% support_invmatch("RIP_EXTEND_SUPPORT", "1", "<!--"); %>
 	  <script>Capture(setadvrouting.defaultroute)</script>
          <% support_invmatch("RIP_EXTEND_SUPPORT", "1", "-->"); %></TD>
          <TD width=296>
          <% support_invmatch("RIP_EXTEND_SUPPORT", "1", "<!--"); %>
          <input type="radio" name="SDroute_enable" value="1" <% nvram_match("send_droute","1","checked"); %>><script>Capture(share.enabled)</script>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
          <input type="radio" name="SDroute_enable" value="0" <% nvram_match("send_droute","0","checked"); %>><script>Capture(share.disabled)</script>
          <% support_invmatch("RIP_EXTEND_SUPPORT", "1", "-->"); %></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
        </TR>

        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 >
          <% support_invmatch("RIP_EXTEND_SUPPORT", "1", "<!--"); %>
	  <script>Capture(share.iface)</script>
          <% support_invmatch("RIP_EXTEND_SUPPORT", "1", "-->"); %></TD>
          <TD width=296>
          <% support_invmatch("RIP_EXTEND_SUPPORT", "1", "<!--"); %>
          <SELECT name="rip_interface" onChange=SelPvcConnection(this.form.rip_interface.selectedIndex,this.form) size="1">
          <% rip_interface_table("select"); %></SELECT>
          <% support_invmatch("RIP_EXTEND_SUPPORT", "1", "-->"); %></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
        </TR>

        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><script>Capture(setadvrouting.transver)</script>:</TD>
          <TD width=296 height=25>
        <font face="Arial" size="1"><span style="font-size: 8pt"><select name=sel_ripTX size=1>
<option value="1" <% nvram_match_routing("send", "1", "selected"); %>><script>Capture(setadvrouting.rip)</script>1</option>
<option value="2" <% nvram_match_routing("send", "2", "selected"); %>><script>Capture(setadvrouting.rip)</script>1-<script>Capture(setadvrouting.compatible)</script></option>
<option value="3" <% nvram_match_routing("send", "3", "selected"); %>><script>Capture(setadvrouting.rip)</script>2</option>
<option value="0" <% nvram_match_routing("send", "0", "selected"); %>><script>Capture(share.disabled)</script></option>
</select></span></font></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
        </TR>

        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><script>Capture(setadvrouting.receiver)</script>:</TD>
          <TD width=296 height=25>
          <font face="Arial" size="1"><span style="font-size: 8pt"><select name=sel_ripRX size=1>
<option value="1" <% nvram_match_routing("receive", "1", "selected"); %>> RIP1</option>
<option value="2" <% nvram_match_routing("receive", "2", "selected"); %>> RIP2</option>
<option value="0" <% nvram_match_routing("receive", "0", "selected"); %>><script>Capture(share.disabled)</script></option>
</select></span></font></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
        </TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101>
          <% support_invmatch("RIP_EXTEND_SUPPORT", "1", "<!--"); %>
          <script>Capture(setadvrouting.multibroad)</script>:
          <% support_invmatch("RIP_EXTEND_SUPPORT", "1", "-->"); %></TD>
          <TD width=296>
          <% support_invmatch("RIP_EXTEND_SUPPORT", "1", "<!--"); %>
         <select size="1" name="sel_MulticastBroadcast">
         <option value="0" <% nvram_match_routing("MulticastBroadcast","0","selected"); %>><script>Capture(setadvrouting.multicast)</script></option>
         <option value="1" <% nvram_match_routing("MulticastBroadcast","1","selected"); %>><script>Capture(setadvrouting.broadcast)</script></option>
         </select> <% support_invmatch("RIP_EXTEND_SUPPORT", "1", "-->"); %></TD>
         <TD width=13 height=25>&nbsp;</TD>
         <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
       </TR>

       <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=1>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=6>
            <TABLE>
              <TBODY>
              <TR>
                <TD width=16 height=1></TD>
                <TD width=13 height=1>&nbsp;</TD>
                <TD width=410 colSpan=3 height=1>
                  <HR color=#b5b5e6 SIZE=1>
                </TD>
                <TD width=13>&nbsp;</TD></TR></TBODY></TABLE></TD>
          <TD width=15 background=image/UI_05.gif 
height=50>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>
            <P align=right><FONT style="FONT-WEIGHT: 700"><script>Capture(setleftmenu.staticrouting)</script></FONT></P></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><script>Capture(setadvrouting.selnum)</script>:</TD>
          <TD width=296 height=25>
	    <TABLE id=AutoNumber10 style="BORDER-COLLAPSE: collapse" 
            borderColor=#111111 cellSpacing=0 cellPadding=0 width=265 align=left 
            border=0>
              <TBODY>
              <TR>
                <TD width=80 colSpan=2>&nbsp;<SELECT size=1 name="route_page" onChange=SelRoute(this.form.route_page.selectedIndex,this.form)>
	<% static_route_table("select"); %></SELECT>
                <TD>
                <TD>&nbsp;&nbsp;<!--<INPUT onclick="DeleteEntry(this.form)" type=button value="Delete This Entry">--><script>document.write("<INPUT onclick=DeleteEntry(this.form) type=button value=\"" + setadvrouting.deletes +"\">");</script></TD></TR></TBODY></TABLE></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
        height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(setadvrouting.dstipaddr)</script>:</FONT></TD>
          <TD width=296 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial>&nbsp;<input type="hidden" name="route_ipaddr" value="4"><input name="route_ipaddr_0" value="<% static_route_setting("ipaddr","0"); %>" size="3" maxlength="3" onBlur=valid_range(this,1,223,"IP") class=num>.
	<input name="route_ipaddr_1" value="<% static_route_setting("ipaddr","1"); %>" size="3" maxlength="3" onBlur=valid_range(this,0,255,"IP") class=num>.
	<input name="route_ipaddr_2" value="<% static_route_setting("ipaddr","2"); %>" size="3" maxlength="3" onBlur=valid_range(this,0,255,"IP") class=num>.
	<input name="route_ipaddr_3" value="<% static_route_setting("ipaddr","3"); %>" size="3" maxlength="3" onBlur=valid_range(this,0,254,"IP") class=num>
</SPAN></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
        height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=9 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt"><SPAN 
            ><script>Capture(setadvrouting.subnet)</script>:</SPAN></FONT></TD>
          <TD width=296 height=25>&nbsp;<input type="hidden" name="route_netmask" value="4"><input name="route_netmask_0" value="<% static_route_setting("netmask","0"); %>" size="3" maxlength="3" onBlur=valid_range(this,0,255,"IP") class=num>.
	<input name="route_netmask_1" value="<% static_route_setting("netmask","1"); %>" size="3" maxlength="3" onBlur=valid_range(this,0,255,"IP") class=num>.
	<input name="route_netmask_2" value="<% static_route_setting("netmask","2"); %>" size="3" maxlength="3" onBlur=valid_range(this,0,255,"IP") class=num>.
	<input name="route_netmask_3" value="<% static_route_setting("netmask","3"); %>" size="3" maxlength="3" onBlur=valid_range(this,0,255,"IP") class=num>
</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
        height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt"><SPAN 
            ><script>Capture(share.gateway)</script>:</SPAN></FONT></TD>
          <TD width=296 height=25>&nbsp;<input type="hidden" name="route_gateway" value="4"><input name="route_gateway_0" value="<% static_route_setting("gateway","0"); %>" size="3" maxlength="3" onBlur=valid_range(this,0,223,"IP") class=num>.
	<input name="route_gateway_1" value="<% static_route_setting("gateway","1"); %>" size="3" maxlength="3" onBlur=valid_range(this,0,255,"IP") class=num>.
	<input name="route_gateway_2" value="<% static_route_setting("gateway","2"); %>" size="3" maxlength="3" onBlur=valid_range(this,0,255,"IP") class=num>.
	<input name="route_gateway_3" value="<% static_route_setting("gateway","3"); %>" size="3" maxlength="3" onBlur=valid_range(this,0,254,"IP") class=num>
</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
        height=25>&nbsp;</TD></TR>
        
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(setadvrouting.hop)</script>: 
            </FONT></TD>
             <TD align=left width=296 height=25>&nbsp;<input name="route_metric" value= "<% static_route_setting("metric","0"); %>" size="3" maxlength="2" onBlur=valid_range(this,1,15,setadvrouting.hop) class=num></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
        height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=6>
            <TABLE>
              <TBODY>
              <TR>
                <TD width=16 height=1>&nbsp;</TD>
                <TD width=13 height=1>&nbsp;</TD>
                <TD width=410 colSpan=3 height=1>
                  <HR color=#b5b5e6 SIZE=1>
                </TD>
                <TD width=13>&nbsp;</TD></TR></TBODY></TABLE></TD>
          <TD width=15 background=image/UI_05.gif 
height=1>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>
            <P align=right>&nbsp;</P></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=6 height=25>
            <TABLE>
              <TBODY>
              <TR>
                <TD width=16 height=25>&nbsp;</TD>
                <TD width=13 height=25>&nbsp;</TD>
                <TD width=410 colSpan=3 height=1><!-- <INPUT onclick="ViewRoute()" type=button value="Show Routing Table" name=button2></TD> --><script>document.write("<INPUT onclick=\"ViewRoute()\" type=button value=\"" + setadvrouting.showtable + "\" name=button2></TD>");</script>
                <TD width=13>&nbsp;</TD></TR></TBODY></TABLE></TD>
          <TD width=15 background=image/UI_05.gif 
height=25>&nbsp;</TD></TR>
<!--<INPUT onclick="ViewPvcRoute()" type=button value="PVC Routing Setting" name=button3></TD>-->
<% support_elsematch("MULTIPVC_SUPPORT", "1", "", "<!--"); %>     	
        <tr>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=6 height=25>
            <TABLE>
              <TBODY>
              <TR>
                <TD width=16 height=25>&nbsp;</TD>
                <TD width=13 height=25>&nbsp;</TD>
                <TD width=410 colSpan=3 height=25>
                  <HR color=#b5b5e6 SIZE=1>
                </TD>
                <TD width=13>&nbsp;</TD></TR></TBODY></TABLE></TD>
          <TD width=15 background=image/UI_05.gif 
height=25>&nbsp;</TD>
		</tr>
		<tr>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>
            <P align=right><span style="font-weight: 700"><script>Capture(setadvrouting.pvcpolicy)</script></span></P></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=6 height=25>
            <TABLE>
              <TBODY>
              <TR>
                <TD width=16 height=25>&nbsp;</TD>
                <TD width=13 height=25>&nbsp;</TD>
                <TD width=410 colSpan=3 height=25><script>document.write("<INPUT onclick=\"ViewPvcRoute()\" type=button value=\"" + setadvrouting.pvcsetting + "\" name=button3></TD>");</script>
                <TD width=13>&nbsp;</TD></TR></TBODY></TABLE></TD>
          <TD width=15 background=image/UI_05.gif 
height=25>&nbsp;</TD>
		</tr>
<% support_elsematch("MULTIPVC_SUPPORT", "1", "", "-->"); %>     	
 
    <TR>
          <TD width=8 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=47 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=100 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=http://192.168.1.1/image/UI_04.gif height=25></TD>
          <TD width=454 colSpan=6 height=25></TD>
          <TD width=15 background=http://192.168.1.1/image/UI_05.gif 
        height=25></TD></TR></TBODY></TABLE></TD>
    <TD vAlign=top width=176 bgColor=#6666cc>
      <TABLE cellSpacing=0 cellPadding=0 width=176 border=0>
        <TBODY>
        <TR>
          <TD width=11 bgColor=#6666cc height=25>&nbsp;</TD>
          <TD width=156 bgColor=#6666cc height=25><font color="#FFFFFF">
          <span ><a target="_blank" href="help/HRouting.asp"><script>Capture(share.more)</script>...</a></span></font></TD>
          <TD width=8 bgColor=#6666cc 
  height=25>&nbsp;</TD></TR></TBODY></TABLE></TD></TR>
  <TR>
    <TD width=809 colSpan=2>
      <TABLE cellSpacing=0 cellPadding=0 border=0 height=25>
        <TBODY>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 background=http://192.168.1.1/image/UI_04.gif>&nbsp;</TD>
          <TD width=16>&nbsp;</TD>
          <TD width=12>&nbsp;</TD>
          <TD width=411>&nbsp;</TD>
          <TD width=15>
          <TD width=15 background=http://192.168.1.1/image/UI_05.gif>&nbsp;</TD>
          <TD width=176 bgColor=#6666cc rowSpan=2>
          <IMG 
            src="http://192.168.1.1/image/UI_Cisco.gif" border=0 width="176" height="64"></TD></TR>
        <TR>
          <TD width=156 bgColor=#000000>&nbsp;</TD>
          <TD width=8 bgColor=#000000>&nbsp;</TD>
          <TD width=15 bgColor=#6666cc>&nbsp;</TD>
          <TD width=12 bgColor=#6666cc>&nbsp;</TD>
          <TD width=411 bgColor=#6666cc>
            <TABLE height=19 cellSpacing=0 cellPadding=0 align=right 
              border=0 width=220><TBODY>
              <TR>
                <TD align=middle bgColor=#42498c width="101"><FONT color=#ffffff><B><A href='javascript:to_submit(document.forms[0]);'><script>Capture(share.saves)</script></A></B></FONT>
                </TD>
                <TD align=middle width=8 bgColor=#6666cc>&nbsp;</TD>
                <TD align=middle bgColor=#434a8f width="111"><FONT color=#ffffff><B><A href='Routing.asp'><script>Capture(share.cancels)</script></A></B></FONT></TD>
                <TD align=middle width=4 bgColor=#6666cc>&nbsp;</TD>
                </TR></TBODY></TABLE></TD>
                <TD width=15 bgColor=#6666cc height=28>&nbsp;</TD>
                <TD width=15 bgColor=#000000 height=28>&nbsp;</TD>
              </TR>
  </TBODY></TABLE></TD></TR></TBODY></TABLE></FORM></DIV></BODY></HTML>


