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

<HTML><HEAD><TITLE>QoS</TITLE>
<% no_cache(); %>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<link rel="stylesheet" type="text/css" href="style.css">
<style fprolloverstyle>
A:hover {color: #00FFFF}
.small A:hover {color: #00FFFF}
</style>
<script src="common.js"></script>
<SCRIPT language="javascript" type="text/javascript" src="share.js"></SCRIPT>
<SCRIPT language="javascript" type="text/javascript" src="capapp.js"></SCRIPT>
<SCRIPT language=JavaScript>
document.title=(apptopmenu.qos);
<% wireless_support(5); %>share.productname = share.productname1;<% wireless_support(6); %>
function QoS_grey(F)
{	
	if(F.qosSW[1].checked)
	{
			F.qos_ftp[0].disabled = true;
			F.qos_ftp[1].disabled = true;		
			F.qos_ftp[2].disabled = true;		
			F.qos_http[0].disabled = true;		
			F.qos_http[1].disabled = true;		
			F.qos_http[2].disabled = true;		
			F.qos_telnet[0].disabled = true;		
			F.qos_telnet[1].disabled = true;		
			F.qos_telnet[2].disabled = true;		
			F.qos_smtp[0].disabled = true;		
			F.qos_smtp[1].disabled = true;		
			F.qos_smtp[2].disabled = true;		
			F.qos_pop3[0].disabled = true;		
			F.qos_pop3[1].disabled = true;		
			F.qos_pop3[2].disabled = true;		
			F.qos_spec0[0].disabled = true;		
			F.qos_spec0[1].disabled = true;		
			F.qos_spec0[2].disabled = true;		
			F.qos_spec1[0].disabled = true;		
			F.qos_spec1[1].disabled = true;		
			F.qos_spec1[2].disabled = true;		
			F.qos_spec2[0].disabled = true;		
			F.qos_spec2[1].disabled = true;		
			F.qos_spec2[2].disabled = true;		

			choose_disable(F.qos_spec_port0);
			choose_disable(F.qos_spec_port1);
			choose_disable(F.qos_spec_port2);
<% support_invmatch("QOS_ADVANCE_SUPPORT", "1", "/*"); %>
			choose_disable(F.qos_size);
			choose_disable(F.qos_adv);
			choose_disable(F.qos_vlan);
			choose_disable(F.qos_vid);
<% support_invmatch("QOS_ADVANCE_SUPPORT", "1", "*/"); %>
                        /*choose_disable(F.vid_ip0);
			choose_disable(F.vid_ip1);
			choose_disable(F.vid_ip2);
			choose_disable(F.vid_ip3);*/
	}	
	else
	{		
			F.qos_ftp[0].disabled = false;
			F.qos_ftp[1].disabled = false;		
			F.qos_ftp[2].disabled = false;		
			F.qos_http[0].disabled = false;		
			F.qos_http[1].disabled = false;		
			F.qos_http[2].disabled = false;		
			F.qos_telnet[0].disabled = false;		
			F.qos_telnet[1].disabled = false;		
			F.qos_telnet[2].disabled = false;		
			F.qos_smtp[0].disabled = false;		
			F.qos_smtp[1].disabled = false;		
			F.qos_smtp[2].disabled = false;		
			F.qos_pop3[0].disabled = false;		
			F.qos_pop3[1].disabled = false;		
			F.qos_pop3[2].disabled = false;		
			F.qos_spec0[0].disabled = false;		
			F.qos_spec0[1].disabled = false;		
			F.qos_spec0[2].disabled = false;		
			F.qos_spec1[0].disabled = false;		
			F.qos_spec1[1].disabled = false;		
			F.qos_spec1[2].disabled = false;		
			F.qos_spec2[0].disabled = false;		
			F.qos_spec2[1].disabled = false;		
			F.qos_spec2[2].disabled = false;	
			choose_enable(F.qos_spec_port0);
			choose_enable(F.qos_spec_port1);
			choose_enable(F.qos_spec_port2);
<% support_invmatch("QOS_ADVANCE_SUPPORT", "1", "/*"); %>
			choose_enable(F.qos_size);
			choose_enable(F.qos_adv);
			choose_enable(F.qos_vlan);
			choose_enable(F.qos_vid);
<% support_invmatch("QOS_ADVANCE_SUPPORT", "1", "*/"); %>
			/*choose_enable(F.vid_ip0);
			choose_enable(F.vid_ip1);
			choose_enable(F.vid_ip2);
			choose_enable(F.vid_ip3);*/
	}
}

function valid_ip(F,N,M1,flag){
	var m = new Array(4);
	M = unescape(M1);

	for(i=0;i<4;i++)
	{
		m[i] = eval(N+i).value;
		if(m[i] == "")
		{
			//alert(M+' value is illegal!');
			alert(M + errmsg.err50);
			return false;
		}
	}

	if(m[0] == 127 || m[0] == 224){
		//alert(M+' value is illegal!');
		alert(M + errmsg.err50);
		return false;
	}
	if(m[0] == '0' && m[1] == '0' && m[2] == '0' && m[3] == '0'){
		if(flag & ZERO_NO){
			//alert(M+' value is illegal!');
			alert(M + errmsg.err50);
			return false;
		}
	}
	if((m[0] != '0' || m[1] != '0' || m[2] != '0') && m[3] == '0'){
		if(flag & MASK_NO){
			//alert(M+' value is illegal!');
			alert(M + errmsg.err50);
			return false;
		}
	}
	return true;
}

function portcheck(I)
{
	d = parseInt(I.value,10);
	if(!(d <= 65535 && d >= 0))
	{
		//alert('Port value is out of range[0 -65535]!');
		alert(errmsg.err94);
                I.value = I.defaultValue;
		return false;
	}
	return true;
}
function CheckSamePort(F,N)
{
	var p = new Array(3);
	for(i = 0;i < 3;i++)
	{
		p[i] = eval(N + i).value;
		if(p[i] == '20' || p[i] == '80' || p[i] == '21' || p[i] =='25' || p[i] == '110')
		{
			return false;
		}
	}

	if(p[0] != '0')
	{
		if(p[0] == p[1] || p[0] == p[2])
			return false;
	}
	else if(p[1] != '0')
	{
		if(p[1] == p[2])
			return false;
	}
	return true;
}
function Check_value(F)
{
	if(F.qosSW[0].checked)
	{
		if(!portcheck(F.qos_spec_port0))
		{
			F.qos_spec_port0.focus();
			return false;	
		}
		if(!portcheck(F.qos_spec_port1))
		{
			F.qos_spec_port1.focus();
			return false;	
		}
		if(!portcheck(F.qos_spec_port2))
		{
			F.qos_spec_port2.focus();
			return false;	
		}
		if(!CheckSamePort(F,"F.qos_spec_port"))
		{
			//alert('Specific Port illegal!');
			alert(errmsg.err95);
			return false;	
		}
<% support_invmatch("QOS_ADVANCE_SUPPORT", "1", "/*"); %>
		if(F.qos_adv.checked)
		{
			if(F.qos_size.value == "" || F.qos_size.value < 68 || F.qos_size.value > 1492)
			{
				//alert('Packet size value is out of range [68 - 1492]!');
				alert(errmsg.err96);
				F.qos_size.focus();
				return false;
			}
			F.qos_adv.value = F.hid_qos_adv.value = 1;
		}
		else
			F.qos_adv.value = F.hid_qos_adv.value = 0;
	
		if (F.qos_vlan.checked)
		{
			if(F.qos_vid.value == "" || F.qos_vid.value < 1 || F.qos_vid.value > 4094)
			{
				//alert('VID value is out of range [1 - 4094]!');
				alert(errmsg.err97);
				F.qos_vid.focus();
				return false;
			}
			F.qos_vlan.value = F.hid_qos_vlan.value = 1;
		}
		else 
			F.qos_vlan.value = F.hid_qos_vlan.value  = 0;
<% support_invmatch("QOS_ADVANCE_SUPPORT", "1", "*/"); %>
	}
	return true;
}

function to_submit(F)
{
		if(Check_value(F))
		{
			F.submit_button.value = "qos";
			F.action.value = "Apply";
			F.submit();
		}
}

</SCRIPT>

<BODY vLink=#b5b5e6 aLink=#ffffff link=#b5b5e6 onload=QoS_grey(document.forms[0])>
<DIV align=center>
<FORM name=F1 action=apply.cgi method=<% get_http_method(); %> >
<input type=hidden name=submit_button>
<input type=hidden name=change_action>
<input type=hidden name=submit_type>
<input type=hidden name=action>
<input type=hidden name="qos_config">
<TABLE cellSpacing=0 cellPadding=0 width=809 border=0>

  <TBODY>

  <TR>

    <TD width=95><IMG src="image/UI_Linksys.gif" border=0 width="165" height="57"></TD>

    <TD vAlign=bottom align=right width=714 bgColor=#6666cc><FONT 

      style="FONT-SIZE: 7pt" color=#ffffff><script>Capture(share.firmwarever)</script>:&nbsp;<% get_firmware_version(); %>&nbsp;&nbsp;&nbsp;</FONT></TD>

  </TR>

  <TR>

    <TD width=808 bgColor=#6666cc colSpan=2><IMG height=11 src="image/UI_10.gif" width=809 border=0></TD>

  </TR>

  </TBODY>

</TABLE>

<TABLE height=77 cellSpacing=0 cellPadding=0 width=809 bgColor=black border=0>

  <TBODY>

  <TR>

    <TD 

    style="FONT-WEIGHT: normal;COLOR: black; FONT-STYLE: normal;" align=middle width=163 height=49>

      <H3><FONT color=#ffffff><script>Capture(bmenu.application)</script><BR>&amp;&nbsp;<script>Capture(bmenu.gaming)</script></FONT></H3></TD>

    <TD width=646 bgColor=#000000 height=49>

      <TABLE 

      style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black;" 

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

          width=537 bgColor=#000000 height=1>&#12288;</TD></TR>

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
                <TD width=103 height=1><IMG height=10 src="<% wireless_support(3); %>" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="<% wireless_support(4); %>" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=68  height=1><IMG height=10 src="image/UI_06.gif" width=68  border=0></TD></TR>
              <TR>

                <TD align=middle bgColor=#000000 height=20><FONT 

                  style="FONT-WEIGHT: 700" color=#ffffff>

                <a style="TEXT-DECORATION: none" href="index.asp"><script>Capture(bmenu.setup)</script></a></FONT></TD>

                <% wireless_support(1); %><TD align=middle bgColor=#000000 height=20><FONT 

                  style="FONT-WEIGHT: 700" color=#ffffff>

                <a style="TEXT-DECORATION: none" href="Wireless_Basic.asp"><script>Capture(bmenu.wireless)</script></a></FONT></TD><% wireless_support(2); %>

                <TD align=middle bgColor=#000000 height=20><FONT 

                  style="FONT-WEIGHT: 700" color=#ffffff>

                <a style="TEXT-DECORATION: none" href="Firewall.asp"><script>Capture(bmenu.security)</script></a></FONT></TD>

                <TD align=middle bgColor=#000000 height=20><FONT 

                  style="FONT-WEIGHT: 700" color=#ffffff>

                <A style="TEXT-DECORATION: none" href="<% support_elsematch("PARENTAL_CONTROL_SUPPORT", "1", "Parental_Control.asp", "Filters.asp"); %>"><script>Capture(bmenu.accrestriction)</script></A></FONT></TD>

                <TD align=middle bgColor=#6666cc height=20>

                  <P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" 

                  color=#ffffff><script>Capture(bmenu.application)</script><BR>&amp; <script>Capture(bmenu.gaming)</script>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>

                <TD align=middle bgColor=#000000 height=20>

                  <P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" 

                  color=#ffffff>

                  <a style="TEXT-DECORATION: none" href="Management.asp">&nbsp;&nbsp;<script>Capture(bmenu.admin)</script></a>&nbsp;&nbsp;</FONT></P></TD>

                <TD align=middle bgColor=#000000 height=20>

                  <P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" 

                  color=#ffffff>

                  <a style="TEXT-DECORATION: none" href="Status_Router.asp"><script>Capture(bmenu.statu)</script></a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>

              </TR>              <TR>

                <TD width=643 bgColor=#6666cc colSpan=7 height=21>

                  <TABLE borderColor=black height=21 cellSpacing=0 cellPadding=0>

                    <TBODY>

                    <TR align=center>
			<TD width=230 class=small><A href="Forward_UPnP.asp"><script>Capture(apptopmenu.singleport)</script></A></TD>
			<TD><P class=bar><font color='white'><b>|</b></font></P></TD>
			<TD width=250 class=small><A href="Forward.asp"><script>Capture(apptopmenu.portrange)</script></A></TD>
			<TD><P class=bar><font color='white'><b>|</b></font></P></TD>
			<TD width=200 class=small><A href="Triggering.asp"><script>Capture(apptopmenu.porttrigger)</script></A></TD>
			<TD><P class=bar><font color='white'><b>|</b></font></P></TD>
			<TD width=120 class=small><A href="DMZ.asp"><script>Capture(apptopmenu.dmz)</script></a></TD>
			<TD><P class=bar><font color='white'><b>|</b></font></P></TD>
			<TD width=120 class=small><FONT style='COLOR:white'><script>Capture(apptopmenu.qos)</script></FONT></TD>
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
          <TD width=156 bgColor=#000000 colSpan=3 height=25>
            <P align=right><B><FONT style="FONT-SIZE: 9pt" face=Arial 
            color=#ffffff><script>Capture(appleftmenu.qos)</script></FONT></B></P></TD>
          <TD width=8 bgColor=#000000 height=25>&nbsp;</TD>
          <TD colSpan=6 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>
            <P align=right><FONT style="FONT-WEIGHT: 700"></FONT>&nbsp;</P></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><SPAN lang=zh-tw 
            style="FONT-SIZE: 8pt">&nbsp;</SPAN></TD>
          <TD width=296 height=25>
            <TABLE id=AutoNumber12 cellSpacing=0 cellPadding=0 width=242 
            border=0>
              <TBODY>
              <TR>
                <TD width=27 height=25><INPUT onclick=QoS_grey(this.form) 
                  type=radio value=1 name=qosSW <% qos_config("qosSW",1); %> ></TD>
                <TD width=54 height=25><B><FONT style="FONT-SIZE: 8pt" 
                  face=Arial><script>Capture(share.enabled)</script><SPAN lang=zh-tw>&nbsp;</SPAN></FONT></B></TD>
                <TD width=24 height=25><INPUT onclick=QoS_grey(this.form) 
                  type=radio <% qos_config("qosSW",0); %> value=0 name=qosSW></TD>
                <TD width=137 height=25><B><FONT style="FONT-SIZE: 8pt" 
                  face=Arial size=2><script>Capture(share.disabled)</script></FONT></B></TD></TR></TBODY></TABLE></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#000000 colSpan=3 height=25>
            <P align=right><B><FONT style="FONT-SIZE: 9pt" 
            color=#ffffff><script>Capture(appleftmenu.appbaseqos)</script></FONT></B></P></TD>
          <TD width=8 bgColor=#000000 height=25>&nbsp;</TD>
          <TD colSpan=6>
            <TABLE>
              <TBODY>
              <TR>
                <TD width=40 height=25>&nbsp;</TD>
                <TD style="BORDER-TOP: 1px solid; BORDER-LEFT-WIDTH: 1px; BORDER-BOTTOM-WIDTH: 1px; BORDER-RIGHT-WIDTH: 1px" borderColor=#b5b5e6 width=411 height=25>&nbsp;</TD>
                <TD width=15 height=25>&nbsp;</TD></TR></TBODY></TABLE></TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>
            <P align=right><FONT style="FONT-WEIGHT: 700"></FONT>&nbsp;</P></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt">&nbsp;&nbsp; 
            </FONT></TD>
          <TD width=296 height=25><SPAN lang=zh-tw 
            style="FONT-SIZE: 8pt"></SPAN><FONT style="FONT-SIZE: 8pt" 
            face=Arial></FONT>
            <TABLE>
              <CENTER>
              <TBODY>
              <TR>
                <TD align=middle width=80><B><script>Capture(appbaseqos.hpriority)</script></B></TD>
                <TD align=middle width=90><B><script>Capture(appbaseqos.mpriority)</script></B></TD>
                <TD align=middle width=80><B><script>Capture(appbaseqos.lpriority)</script></B></TD></CENTER></TR></TBODY></TABLE></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>
            <P align=right><FONT style="FONT-WEIGHT: 700"></FONT></P></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT 
            style="FONT-SIZE: 8pt"><B><script>Capture(appbaseqos.ftp)</script></B>&nbsp;&nbsp; </FONT></TD>
          <TD width=296 height=25><SPAN lang=zh-tw 
            style="FONT-SIZE: 8pt"></SPAN><FONT style="FONT-SIZE: 8pt" 
            face=Arial>
            <TABLE valign="top">
              <TBODY>
              <TR>
                <TD align=middle width=80><INPUT type=radio value=2 
                  name=qos_ftp <% qos_config("qos_ftp",2); %> >&nbsp;&nbsp;</TD>
                <TD align=middle width=80><INPUT type=radio value=1 
                  name=qos_ftp <% qos_config("qos_ftp",1); %> >&nbsp;&nbsp;</TD>
                <TD align=middle width=80><INPUT type=radio  value=0 
                  name=qos_ftp <% qos_config("qos_ftp",0); %> >&nbsp;&nbsp;</TD></TR></TBODY></TABLE></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>
            <P align=right><FONT style="FONT-WEIGHT: 700"></FONT></P></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT 
            style="FONT-SIZE: 8pt"><B><script>Capture(appbaseqos.http)</script></B>&nbsp;&nbsp; </FONT></TD>
          <TD width=296 height=25><SPAN lang=zh-tw 
            style="FONT-SIZE: 8pt"></SPAN><FONT style="FONT-SIZE: 8pt" 
            face=Arial>
            <TABLE valign="top">
              <TBODY>
              <TR>
                <TD align=middle width=80><INPUT type=radio value=2 
                  name=qos_http <% qos_config("qos_http",2); %> >&nbsp;&nbsp;</TD>
                <TD align=middle width=80><INPUT type=radio value=1 
                  name=qos_http <% qos_config("qos_http",1); %> >&nbsp;&nbsp;</TD>
                <TD align=middle width=80><INPUT type=radio value=0 
                  name=qos_http <% qos_config("qos_http",0); %> >&nbsp;&nbsp;</TD></TR></TBODY></TABLE></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>
            <P align=right><FONT style="FONT-WEIGHT: 700"></FONT></P></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT 
            style="FONT-SIZE: 8pt"><B><script>Capture(appbaseqos.telnet)</script></B>&nbsp;&nbsp; </FONT></TD>
          <TD width=296 height=25><SPAN lang=zh-tw 
            style="FONT-SIZE: 8pt"></SPAN><FONT style="FONT-SIZE: 8pt" 
            face=Arial>
            <TABLE valign="top">
              <TBODY>
              <TR>
                <TD align=middle width=80><INPUT type=radio value=2 
                  name=qos_telnet <% qos_config("qos_telnet",2); %> >&nbsp;&nbsp;</TD>
                <TD align=middle width=80><INPUT type=radio value=1 
                  name=qos_telnet <% qos_config("qos_telnet",1); %> >&nbsp;&nbsp;</TD>
                <TD align=middle width=80><INPUT type=radio  value=0 
                  name=qos_telnet <% qos_config("qos_telnet",0); %> >&nbsp;&nbsp;</TD></TR></TBODY></TABLE></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>
            <P align=right><FONT style="FONT-WEIGHT: 700"></FONT></P></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT 
            style="FONT-SIZE: 8pt"><B><script>Capture(appbaseqos.smtp)</script></B>&nbsp;&nbsp; </FONT></TD>
          <TD width=296 height=25><SPAN lang=zh-tw 
            style="FONT-SIZE: 8pt"></SPAN><FONT style="FONT-SIZE: 8pt" 
            face=Arial>
            <TABLE valign="top">
              <TBODY>
              <TR>
                <TD align=middle width=80><INPUT type=radio value=2 
                  name=qos_smtp <% qos_config("qos_smtp",2); %> >&nbsp;&nbsp;</TD>
                <TD align=middle width=80><INPUT type=radio value=1 
                  name=qos_smtp <% qos_config("qos_smtp",1); %> >&nbsp;&nbsp;</TD>
                <TD align=middle width=80><INPUT type=radio  value=0 
                  name=qos_smtp <% qos_config("qos_smtp",0); %> >&nbsp;&nbsp;</TD></TR></TBODY></TABLE></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>
            <P align=right><FONT style="FONT-WEIGHT: 700"></FONT></P></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT 
            style="FONT-SIZE: 8pt"><B><script>Capture(appbaseqos.pop3)</script></B>&nbsp;&nbsp; </FONT></TD>
          <TD width=296 height=25><SPAN lang=zh-tw 
            style="FONT-SIZE: 8pt"></SPAN><FONT style="FONT-SIZE: 8pt" 
            face=Arial>
            <TABLE valign="top">
              <TBODY>
              <TR>
                <TD align=middle width=80><INPUT type=radio value=2 
                  name=qos_pop3 <% qos_config("qos_pop3",2); %> >&nbsp;&nbsp;</TD>
                <TD align=middle width=80><INPUT type=radio value=1 
                  name=qos_pop3 <% qos_config("qos_pop3",1); %> >&nbsp;&nbsp;</TD>
                <TD align=middle width=80><INPUT type=radio  value=0 
                  name=qos_pop3 <% qos_config("qos_pop3",0); %> >&nbsp;&nbsp;</TD></TR></TBODY></TABLE></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>
            <P align=right><FONT style="FONT-WEIGHT: 700"></FONT></P></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt"><B><script>Capture(appbaseqos.specport)</script></B><INPUT class=num onBlur=valid_range(this,0,65535,"Port") 
            style="FONT-SIZE: 10pt; FONT-FAMILY: Courier" maxLength=5 size=6 
            value=<% qos_config("qos_spec_port0",0); %>  name=qos_spec_port0>&nbsp;&nbsp; </FONT></TD>
          <TD width=296 height=25><SPAN lang=zh-tw 
            style="FONT-SIZE: 8pt"></SPAN><FONT style="FONT-SIZE: 8pt" 
            face=Arial>
            <TABLE valign="top">
              <TBODY>
              <TR>
                <TD align=middle width=80><INPUT type=radio value=2 
                  name=qos_spec0 <% qos_config("qos_spec0",2); %> >&nbsp;&nbsp;</TD>
                <TD align=middle width=80><INPUT type=radio value=1 
                  name=qos_spec0 <% qos_config("qos_spec0",1); %> >&nbsp;&nbsp;</TD>
                <TD align=middle width=80><INPUT type=radio  value=0 
                  name=qos_spec0 <% qos_config("qos_spec0",0); %> >&nbsp;&nbsp;</TD></TR></TBODY></TABLE></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>
            <P align=right><FONT style="FONT-WEIGHT: 700"></FONT></P></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt"><B><script>Capture(appbaseqos.specport)</script></B><INPUT class=num onBlur=valid_range(this,0,65535,"Port")
            style="FONT-SIZE: 10pt; FONT-FAMILY: Courier" maxLength=5 size=6 
            value=<% qos_config("qos_spec_port1",0); %> name=qos_spec_port1>&nbsp;&nbsp; </FONT></TD>
          <TD width=296 height=25><SPAN lang=zh-tw 
            style="FONT-SIZE: 8pt"></SPAN><FONT style="FONT-SIZE: 8pt" 
            face=Arial>
            <TABLE valign="top">
              <TBODY>
              <TR>
                <TD align=middle width=80><INPUT type=radio value=2 
                  name=qos_spec1 <% qos_config("qos_spec1",2); %> >&nbsp;&nbsp;</TD>
                <TD align=middle width=80><INPUT type=radio value=1 
                  name=qos_spec1 <% qos_config("qos_spec1",1); %> >&nbsp;&nbsp;</TD>
                <TD align=middle width=80><INPUT type=radio  value=0 
                  name=qos_spec1 <% qos_config("qos_spec1",0); %> >&nbsp;&nbsp;</TD></TR></TBODY></TABLE></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>
            <P align=right><FONT style="FONT-WEIGHT: 700"></FONT></P></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt"><B><script>Capture(appbaseqos.specport)</script></B><INPUT class=num onBlur=valid_range(this,0,65535,"Port")
            style="FONT-SIZE: 10pt; FONT-FAMILY: Courier" maxLength=5 size=6 
            value=<% qos_config("qos_spec_port2",0); %>  name=qos_spec_port2>&nbsp;&nbsp; </FONT></TD>
          <TD width=296 height=25><SPAN lang=zh-tw 
            style="FONT-SIZE: 8pt"></SPAN><FONT style="FONT-SIZE: 8pt" 
            face=Arial>
            <TABLE valign="top">
              <TBODY>
              <TR>
                <TD align=middle width=80><INPUT type=radio value=2 
                  name=qos_spec2 <% qos_config("qos_spec2",2); %> >&nbsp;&nbsp;</TD>
                <TD align=middle width=80><INPUT type=radio value=1 
                  name=qos_spec2 <% qos_config("qos_spec2",1); %> >&nbsp;&nbsp;</TD>
                <TD align=middle width=80><INPUT type=radio  value=0 
                  name=qos_spec2 <% qos_config("qos_spec2",0); %> >&nbsp;&nbsp;</TD></TR></TBODY></TABLE></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
<% support_invmatch("QOS_ADVANCE_SUPPORT", "1", "<!--"); %>
         <tr>
          <TD width=156  bgColor=#e7e7e7 colSpan=3 height=25>
            <P align=right></P></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=6>
            <TABLE>
              <TBODY>
              <TR>
                <TD width=16 height=25>&nbsp;</TD>
                <TD width=12 height=25>&nbsp;</TD>
                <TD width=411 height=25>&nbsp;</TD>
                <TD width=15 height=25>&nbsp;</TD></TR></TBODY></TABLE></TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
        </tr>
         <TR>
          <TD width=156 bgColor=#000000 colSpan=3 height=25>
            <P align=right><B><FONT style="FONT-SIZE: 9pt" 
            color=#ffffff><script>Capture(appleftmenu.advqos)</script></FONT></B></P></TD>
          <TD width=8 bgColor=#000000 height=25>&nbsp;</TD>
          <TD colSpan=6>
            <TABLE>
              <TBODY>
              <TR>
                <TD width=16 height=25>&nbsp;</TD>
                <TD width=12 height=25>&nbsp;</TD>
                <TD 
                style="BORDER-TOP: 1px solid; BORDER-LEFT-WIDTH: 1px; BORDER-BOTTOM-WIDTH: 1px; BORDER-RIGHT-WIDTH: 1px" 
                borderColor=#b5b5e6 width=411 height=25>&nbsp;</TD>
                <TD width=15 height=25>&nbsp;</TD></TR></TBODY></TABLE></TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
         <tr>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>
            <P align=right>&nbsp;</P></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=397 height=25 colspan="2"> <input type=hidden name=hid_qos_adv value=0>
          <input type=checkbox value=1 name=qos_adv <%qos_config("qos_adv",0);%> ><font face=Arial size=2>&nbsp;<script>Capture(appadvqos.fragsize)</script>:&nbsp;<INPUT class=num  
            style="FONT-SIZE: 10pt; FONT-FAMILY: Courier" maxLength=5 size=6 
         name=qos_size value=<% qos_config("qos_size",0); %>  >&nbsp;<script>Capture(appadvqos.bytes)</script>&nbsp; </FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
        </tr>
         <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>
            <P align=right><FONT style="FONT-WEIGHT: 700"></FONT></P></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=397 height=25 colspan="2"> <input type=hidden name=hid_qos_vlan value=0>
          <input type=checkbox value=1 name=qos_vlan <%qos_config("qos_vlan",0);%> ><font face=Arial size=2>&nbsp;<script>Capture(appadvqos.enable8021p)</script><script>Capture(appadvqos.vlanvid)</script>: 
          <INPUT class=num  
            style="FONT-SIZE: 10pt; FONT-FAMILY: Courier" maxLength=5 size=6 
          name=qos_vid value=<% qos_config("qos_vid",0); %>  >.<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<script>Capture(share.ip)</script>:<INPUT class=num  
            maxLength=3 size=3 value='<% qos_config("ipaddr",0); %>' name="vid_ip0"> . <INPUT 
            class=num maxLength=3 size=3 
            value='<% qos_config("ipaddr",1); %>' name="vid_ip1"> . <INPUT class=num 
            maxLength=3 size=3 value='<% qos_config("ipaddr",2); %>' 
            name="vid_ip2"> . <INPUT class=num 
            maxLength=3 size=3 value='<% qos_config("ipaddr",3); %>' name="vid_ip3"></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
<% support_invmatch("QOS_ADVANCE_SUPPORT", "1", "-->"); %>
        <TR>
          <TD width=44 bgColor=#e7e7e7 height=5>&nbsp;</TD>
          <TD width=65 bgColor=#e7e7e7 height=5>&nbsp;</TD>
          <TD width=47 bgColor=#e7e7e7 height=5>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=5>&nbsp;</TD>
          <TD width=14 height=5></TD>
          <TD width=17 height=5></TD>
          <TD width=13 height=5></TD>
          <TD width=101 height=5></TD>
          <TD width=296 height=5></TD>
          <TD width=13 height=5></TD>
          <TD width=15 background=image/UI_05.gif 
        height=5>&nbsp;</TD></TR></TBODY></TABLE></TD>
    <TD vAlign=top width=176 bgColor=#6666cc>
      <TABLE cellSpacing=0 cellPadding=0 width=176 border=0>
        <TBODY>
        <TR>
          <TD width=11 bgColor=#6666cc height=25>&nbsp;</TD>
          <TD width=156 bgColor=#6666cc height=25><font color="#FFFFFF">
	  <a target="_blank" href="help/HQoS.asp"><script>Capture(share.more)</script>...</a></font></TD>
          <TD width=9 bgColor=#6666cc height=25>&nbsp;</TD></TR></TBODY></TABLE></TD></TR>
<TR>
    <TD width=809 colSpan=2>
      <TABLE cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=30>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif>&nbsp;</TD>
          <TD width=16>&nbsp;</TD>
          <TD width=12>&nbsp;</TD>
          <TD width=411>&nbsp;</TD>
          <TD width=15>
          <TD width=15 background=image/UI_05.gif>&nbsp;</TD>
          <TD width=176 bgColor=#6666cc rowSpan=2><IMG height=64 
            src="image/UI_Cisco.gif" width=176 border=0></TD></TR>
        <TR>
          <TD width=156 bgColor=#000000>&nbsp;</TD>
          <TD width=8 bgColor=#000000>&nbsp;</TD>
          <TD width=16 bgColor=#6666cc>&nbsp;</TD>
          <TD width=12 bgColor=#6666cc>&nbsp;</TD>
          <TD width=411 bgColor=#6666cc>
            <TABLE height=19 cellSpacing=0 cellPadding=0 align=right border=0>
              <TBODY>
              <TR>
                <TD align=middle width=101 bgColor=#434a8f><B><A 
                  href="javascript:to_submit(document.F1);"><script>Capture(share.saves)</script></A></B></FONT></TD>
                <TD align=middle width=8 bgColor=#6666cc>&nbsp;</TD>
                <TD align=middle width=103 bgColor=#434a8f><B><A 
                  href="javascript:window.location='qos.asp';"><script>Capture(share.cancels)</script></A></B></TD></TR></TBODY></TABLE></TD>
          <TD width=15 bgColor=#6666cc height=33>&nbsp;</TD>
          <TD width=15 bgColor=#000000 
  height=33>&nbsp;</TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></FORM></DIV></BODY></HTML>
