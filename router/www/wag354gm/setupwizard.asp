<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">

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

<HTML><HEAD><TITLE>Setup Wizard</TITLE>
<% no_cache(); %>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<link rel="stylesheet" type="text/css" href="style.css">
<style fprolloverstyle>
A:hover {color: #00FFFF}
.small A:hover {color: #00FFFF}
</style>
<script src="common.js"></script>

<SCRIPT language=JavaScript>
<% wireless_support(5); %>share.productname = share.productname1;<% wireless_support(6); %>
var EN_DIS2 = '0';	
var dhcp_win = null;

function valid_mtu(I)
{
	var start = null;
	var end = null;
	if(document.setup.now_encapmode.value == "pppoe" || document.setup.now_encapmode.value == "pppoa")
	{
		start = 576;
		end = 1492;
	}
	else
	{
		start = 576;
		end = 1500;
	}

	if(valid_range(I,start,end,"MTU") == false)
	{
		I.focus();
		return false;
	}
	else
		return true;

}

function valid_mtu_config(F)
{
	if(F.mtu_auto[F.mtu_auto.selectedIndex].value == "0")
	{
		if(valid_mtu(F.mtu_value) == false)
		{
			F.mtu_value.focus();
			return false;
		}
	}
	return true;
}

function SelMTU(num,F)
{
	mtu_enable_disable(F,num);
}

function mtu_enable_disable(F,I)
{
	EN_DIS1 = I;
	if ( I == 0 )
		choose_disable(F.mtu_value);
	else
		choose_enable(F.mtu_value);
}

function SelCONNECTION(num,F)
{
	F.submit_button.value = "index";
	F.change_action.value = "gozila_cgi";
	F.wan_connection.value=F.wan_connection.options[num].value;
	F.submit();
}

function SelTYPE(num,F)
{
	if(F.wan_encapmode.options[num].value == "bridgedonly")
	{
		F.wan_autodetect[0].disabled = true;
		F.wan_autodetect[1].disabled = true;
		F.wan_vpi.disabled = false;
		F.wan_vci.disabled = false;
	}
	else
	{
		F.wan_autodetect[0].disabled = false;
		F.wan_autodetect[1].disabled = false;
	}
		
        F.submit_button.value = "index";
        F.submit_type.value = "savebuf";
        F.change_action.value = "gozila_cgi";
        F.wan_encapmode.value=F.wan_encapmode.options[num].value;
        F.submit();
}

function SelAutodetect(num, F)
{
	if(num == 1)
	{
		choose_disable(F.wan_vpi);
		choose_disable(F.wan_vci);
	}
	else if(num == 0)
	{
		choose_enable(F.wan_vpi);
		choose_enable(F.wan_vci);
	}
}

function SelQOS(num,F)
{
	if(F.wan_qostype.options[num].value == "ubr")
		choose_disable(F.wan_pcr);
		//F.wan_pcr.disabled=true;
	else
		choose_enable(F.wan_pcr);
		//F.wan_pcr.disabled=false;
	if(F.wan_qostype.options[num].value != "vbr")
		choose_disable(F.wan_scr);
		//F.wan_scr.disabled=true;
	else
		choose_enable(F.wan_scr);
		//F.wan_scr.disabled=false;
}

function SelPPP(num,F)
{
        F.submit_button.value = "index";
        F.change_action.value = "gozila_cgi";
        F.submit_type.value = "savebuf";
        F.mpppoe_enable.value = F.mpppoe_enable.options[num].value;
        F.submit();
}

function SelIPPP(num,F)
{
        F.submit_button.value = "index";
        F.change_action.value = "gozila_cgi";
        F.submit_type.value = "savebuf";
        F.ipppoe_enable.value = F.ipppoe_enable.options[num].value;
        F.submit();
}

function to_apply(F)
{
	F.submit_button.value = "index";
        F.change_action.value = "gozila_cgi";
	F.submit_type.value = "apply";
        F.submit();
}

function to_submit(F)
{
	if(valid_value(F))
	{
		F.submit_button.value = "setupwizard";
		F.action.value = "ApplyWizard";
        	F.submit();
	}
}

function check_vpivci(F)
{
	var vpivci_pairs = new Array(8);
	var i;
	var nowstring = F.wan_vpi.value+"."+F.wan_vci.value;

	vpivci_pairs[0] = 0.0;
	vpivci_pairs[1] = 0.0;
	vpivci_pairs[2] = 0.0;
	vpivci_pairs[3] = 0.0;
	vpivci_pairs[4] = 0.0;
	vpivci_pairs[5] = 0.0;
	vpivci_pairs[6] = 0.0;
	vpivci_pairs[7] = 0.0;


	for(i=0; i<8; i++)
	{
		if(nowstring == vpivci_pairs[i])
		{
			var j= i + 1;
			//alert("vpi and vci values have conflicts with connection " + j);
			alert(errmsg.err1);

			return false;
		}
	}
	return true;
}

function valid_value(F)
{
		

       if(F.now_encapmode.value == "pppoa")
	{
		if(F.pppoa_username.value == "")
		{
			//alert("You must input a username!");
			alert(errmsg.err3);

			F.pppoa_username.focus();
			return false;
		}
		if(F.pppoa_password.value == "")
		{
			//alert("You must input a passwd!");
			alert(errmsg.err4);
			F.pppoa_password.focus();
			return false;
		}
	}


	return true;
}

function valid_dhcpusers(F)
{	
	var valuetmp;
	isdigit(F.dhcp_num,"DHCP users number");
	isdigit(F.dhcp_start,"DHCP users start");
        a1 = parseInt(F.dhcp_num.value,10);
        a2 = parseInt(F.dhcp_start.value,10);
	
	//if(F.dhcp_num.value == 1)
	{
		if(F.dhcp_start.value == F.lan_ipaddr_3.value)
		{
               		//alert("DHCP start ip is the same with lan ipaddr");
			alert(errmsg.err5);

                	return false;
		}
	}
	if(F.lan_netmask.selectedIndex == 0)
		valuetmp = 255 - a2;
	else if(F.lan_netmask.selectedIndex == 1)
		valuetmp = 127 - a2;
	else if(F.lan_netmask.selectedIndex == 2)
		valuetmp = 63 - a2;
	else if(F.lan_netmask.selectedIndex == 3)
		valuetmp = 31 - a2;
	else if(F.lan_netmask.selectedIndex == 4)
		valuetmp = 15 - a2;
	else if(F.lan_netmask.selectedIndex == 5)
		valuetmp = 7 - a2;
	else if(F.lan_netmask.selectedIndex == 6)
		valuetmp = 3 - a2;

	if(a1 < 1 || a1 > valuetmp)
	{
		if(valuetmp != 1)
                	//alert("DHCP users numbers is out of range. Must be [1-" + valuetmp + "]");
			alert(errmsg.err6 + valuetmp + "]");
		else
                	//alert("DHCP users numbers is out of range. Must be 1");
			alert(errmsg.err7);

                return false;
	}

	return true;
}

function valid_dhcpstart(F)
{	
	isdigit(F.dhcp_start,"DHCP start ip");
        a1 = parseInt(F.dhcp_start.value,10);
	
	if(F.dhcp_start.value == F.lan_ipaddr_3.value)
	{
		//if(F.dhcp_num.value == 1)
		{
               		//alert("DHCP start ip is the same with lan ipaddr");
			alert(errmsg.err5);

                	return false;
		}
	}
	
	if(F.lan_netmask.selectedIndex == 0)
	{
		if(a1 < 1 || a1 > 254)
		{
                	//alert("DHCP start ip is out of range. Must be [1-254]");
			alert(errmsg.err15);

                	return false;
		}
	}
	else if(F.lan_netmask.selectedIndex == 1)
	{
		if(a1 < 1 || a1 > 126)
		{
                	//alert("DHCP start ip is out of range. Must be [1-126]");
			alert(errmsg.err16);

                	return false;
		}
	}
	else if(F.lan_netmask.selectedIndex == 2)
	{
		if(a1 < 1 || a1 > 62)
		{
                	//alert("DHCP start ip is out of range. Must be [1-62]");
			alert(errmsg.err17);
                	return false;
		}
	}
	else if(F.lan_netmask.selectedIndex == 3)
	{
		if(a1 < 1 || a1 > 30)
		{
                	//alert("DHCP start ip is out of range. Must be [1-30]");
			alert(errmsg.err18);

                	return false;
		}
	}
	else if(F.lan_netmask.selectedIndex == 4)
	{
		if(a1 < 1 || a1 > 14)
		{
                	//alert("DHCP start ip is out of range. Must be [1-14]");
			alert(errmsg.err19);

                	return false;
		}
	}
	else if(F.lan_netmask.selectedIndex == 5)
	{
		if(a1 < 1 || a1 > 6)
		{
                	//alert("DHCP start ip is out of range. Must be [1-6]");
			alert(errmsg.err20);
                	return false;
		}
	}
	else if(F.lan_netmask.selectedIndex == 6)
	{
		if(a1 < 1 || a1 > 2)
		{
                	//alert("DHCP start ip is out of range. Must be [1-2]");
		`	alert(errmsg.err21);

                	return false;
		}
	}
	return true;
}

function valid_dhcp_server(F)
{
	if(F.lan_proto[1].checked == true)
	{
		F.dhcpserver_ipaddr.disabled = true;
                return true;
	}
	
	if(F.lan_proto[2].checked == true)
	{
		if(!valid_ip(F,"F.dhcpserver_ipaddr","Lan DHCPServer IP",ZERO_NO|MASK_NO))
		{
                	F.dhcpserver_ipaddr_0.focus();
       	         	return false;
		}
		if(!valid_range(F.dhcpserver_ipaddr_0,1,223,"IP"))
		{	
			F.dhcpserver_ipaddr_0.focus();
			return false;
		}
		if(!valid_range(F.dhcpserver_ipaddr_3,1,254,"IP"))
		{
			F.dhcpserver_ipaddr_3.focus();
			return false;
		}
		return true;
	}
		
        a1 = parseInt(F.dhcp_start.value,10);
        a2 = parseInt(F.dhcp_num.value,10);
        if(a1 + a2 > 255){
                //alert("Out of range, please adjust start IP address or user's numbers.");
		alert(errmsg.err22);

                return false;
        } 

	if(!valid_dhcpstart(F))
	{
		F.dhcp_start.focus();
		return false;
	}      
	if(!valid_dhcpusers(F))
	{
		F.dhcp_num.focus();
		return false;
	}      

	if(F.wan_dns0_0.disabled == false)
	{
       		if(!valid_ip(F,"F.wan_dns0","DNS",MASK_NO))
                	return false;
	}
	if(F.wan_dns1_0.disabled == false)
	{
        	if(!valid_ip(F,"F.wan_dns1","DNS",MASK_NO))
                	return false;
	}
	if(F.wan_dns2_0.disabled == false)
	{
        	if(!valid_ip(F,"F.wan_dns2","DNS",MASK_NO))
                	return false;
	}

        if(!valid_ip(F,"F.wan_wins","WINS",MASK_NO))
                return false;
	
	F.dhcpserver_ipaddr.disabled = true;

	return true;
}
function SelDHCP(T,F)
{
	dhcp_enable_disable(F,T);
}

function dhcp_enable_disable(F,T)
{
	var start = '';
	var end = '';
 	var total = F.elements.length;
	for(i=0 ; i < total ; i++){
		if(F.elements[i].name == "dhcp_start")	start = i;
		if(F.elements[i].name == "wan_wins_3")	end = i;
	}
	if(start == '' || end == '')	return true;

	if( T == "static" ) {
		EN_DIS = 0;
		for(i = start; i<=end ;i++)
			choose_disable(F.elements[i]);
		choose_disable(F.dhcpserver_ipaddr_0);
		choose_disable(F.dhcpserver_ipaddr_1);
		choose_disable(F.dhcpserver_ipaddr_2);
		choose_disable(F.dhcpserver_ipaddr_3);
		F.dhcp_advance.disabled = true;
	}
	else if( T == "dhcp") {
		EN_DIS = 1;
		for(i = start; i<=end ;i++)
			choose_enable(F.elements[i]);
		choose_disable(F.dhcpserver_ipaddr_0);
		choose_disable(F.dhcpserver_ipaddr_1);
		choose_disable(F.dhcpserver_ipaddr_2);
		choose_disable(F.dhcpserver_ipaddr_3);
		F.dhcp_advance.disabled = false;
	}
	else if( T == "dhcprelay" ) {
		EN_DIS = 0;
		for(i = start; i<=end ;i++)
			choose_disable(F.elements[i]);
		choose_enable(F.dhcpserver_ipaddr_0);
		choose_enable(F.dhcpserver_ipaddr_1);
		choose_enable(F.dhcpserver_ipaddr_2);
		choose_enable(F.dhcpserver_ipaddr_3);
		F.dhcp_advance.disabled = true;
	}

	
}
function SelTime(num,f)
{
	aaa = f.time_zone.options[num].value.charAt(6);
	daylight_enable_disable(f,aaa);
}

function pppoe_enable_disable(F,I)
{
	if( I == "0"){
		choose_disable(F.pppoe_idletime);
		choose_enable(F.pppoe_redialperiod);
	}
	else{
		choose_enable(F.pppoe_idletime);
		choose_disable(F.pppoe_redialperiod);
	}
}

function mpppoe_enable_disable(F,I)
{
	if( I == "0"){
		choose_disable(F.mpppoe_idletime);
		choose_enable(F.mpppoe_redialperiod);
	}
	else{
		choose_enable(F.mpppoe_idletime);
		choose_disable(F.mpppoe_redialperiod);
	}
}

function ipppoe_enable_disable(F,I)
{
	if( I == "0"){
		choose_disable(F.ipppoe_idletime);
		choose_enable(F.ipppoe_redialperiod);
	}
	else{
		choose_enable(F.ipppoe_idletime);
		choose_disable(F.ipppoe_redialperiod);
	}
}

function pppoa_enable_disable(F,I)
{
	if( I == "0"){
		choose_disable(F.pppoa_idletime);
		choose_enable(F.pppoa_redialperiod);
	}
	else{
		choose_enable(F.pppoa_idletime);
		choose_disable(F.pppoa_redialperiod);
	}
}

function dhcpc_enable_disable(F,I)
{
	if( I == "1")
	{
		choose_disable(F.bridged_ipaddr_0);
		choose_disable(F.bridged_ipaddr_1);
		choose_disable(F.bridged_ipaddr_2);
		choose_disable(F.bridged_ipaddr_3);
		
		choose_disable(F.bridged_netmask_0);
		choose_disable(F.bridged_netmask_1);
		choose_disable(F.bridged_netmask_2);
		choose_disable(F.bridged_netmask_3);
		
		choose_disable(F.bridged_gateway_0);
		choose_disable(F.bridged_gateway_1);
		choose_disable(F.bridged_gateway_2);
		choose_disable(F.bridged_gateway_3);
		
		choose_disable(F.bridged_pdns_0);
		choose_disable(F.bridged_pdns_1);
		choose_disable(F.bridged_pdns_2);
		choose_disable(F.bridged_pdns_3);
	
		choose_disable(F.bridged_sdns_0);
		choose_disable(F.bridged_sdns_1);
		choose_disable(F.bridged_sdns_2);
		choose_disable(F.bridged_sdns_3);

		choose_enable(F.wan_dns0_0);
		choose_enable(F.wan_dns0_1);
		choose_enable(F.wan_dns0_2);
		choose_enable(F.wan_dns0_3);
		choose_enable(F.wan_dns1_0);
		choose_enable(F.wan_dns1_1);
		choose_enable(F.wan_dns1_2);
		choose_enable(F.wan_dns1_3);
		choose_enable(F.wan_dns2_0);
		choose_enable(F.wan_dns2_1);
		choose_enable(F.wan_dns2_2);
		choose_enable(F.wan_dns2_3);
	}
	else	
	{
		choose_enable(F.bridged_ipaddr_0);
		choose_enable(F.bridged_ipaddr_1);
		choose_enable(F.bridged_ipaddr_2);
		choose_enable(F.bridged_ipaddr_3);
		
		choose_enable(F.bridged_netmask_0);
		choose_enable(F.bridged_netmask_1);
		choose_enable(F.bridged_netmask_2);
		choose_enable(F.bridged_netmask_3);
		
		choose_enable(F.bridged_gateway_0);
		choose_enable(F.bridged_gateway_1);
		choose_enable(F.bridged_gateway_2);
		choose_enable(F.bridged_gateway_3);
		
		choose_enable(F.bridged_pdns_0);
		choose_enable(F.bridged_pdns_1);
		choose_enable(F.bridged_pdns_2);
		choose_enable(F.bridged_pdns_3);
	
		choose_enable(F.bridged_sdns_0);
		choose_enable(F.bridged_sdns_1);
		choose_enable(F.bridged_sdns_2);
		choose_enable(F.bridged_sdns_3);

		choose_disable(F.wan_dns0_0);
		choose_disable(F.wan_dns0_1);
		choose_disable(F.wan_dns0_2);
		choose_disable(F.wan_dns0_3);
		choose_disable(F.wan_dns1_0);
		choose_disable(F.wan_dns1_1);
		choose_disable(F.wan_dns1_2);
		choose_disable(F.wan_dns1_3);
		choose_disable(F.wan_dns2_0);
		choose_disable(F.wan_dns2_1);
		choose_disable(F.wan_dns2_2);
		choose_disable(F.wan_dns2_3);
	}	
}

function daylight_enable_disable(F,aaa)
{
	if(aaa == 0){
                F._daylight_time.checked = false;
                choose_disable(F._daylight_time);
                F.daylight_time.value = 0;
        }
        else{
                choose_enable(F._daylight_time);                
                F._daylight_time.checked = true;
                F.daylight_time.value = 1;
        }

}
function init()
{
	var dnsdisable;
	aaa = document.setup.time_zone.options[document.setup.time_zone.selectedIndex].value.charAt(6);
	if(aaa == 0){
                document.setup._daylight_time.checked = false;
                choose_disable(document.setup._daylight_time);
                document.setup.daylight_time.value = 0;
        }
	if(document.setup.wan_dns0_0.disabled==true)
		dnsdisable = 1;
	else
		dnsdisable = 0;
	
	if(document.setup.now_encapmode.value == "bridgedonly")
	{
		document.setup.lan_proto[1].checked = true;
		dhcp_enable_disable(document.setup,'static');
		document.setup.lan_proto[0].disabled = true;
		document.setup.lan_proto[1].disabled = true;
		document.setup.lan_proto[2].disabled = true;
	}
	else
	{
		document.setup.lan_proto[0].disabled = false;
		document.setup.lan_proto[1].disabled = false;
		document.setup.lan_proto[2].disabled = false;
		dhcp_enable_disable(document.setup,'dhcp');
	}

	if(dnsdisable == 1 && document.setup.wan_dns0_0.disabled == false)
	{
		document.setup.wan_dns0_0.disabled = true;
		document.setup.wan_dns0_1.disabled = true;
		document.setup.wan_dns0_2.disabled = true;
		document.setup.wan_dns0_3.disabled = true;
		document.setup.wan_dns1_0.disabled = true;
		document.setup.wan_dns1_1.disabled = true;
		document.setup.wan_dns1_2.disabled = true;
		document.setup.wan_dns1_3.disabled = true;
		document.setup.wan_dns2_0.disabled = true;
		document.setup.wan_dns2_1.disabled = true;
		document.setup.wan_dns2_2.disabled = true;
		document.setup.wan_dns2_3.disabled = true;
	}
}
//added by leijun
var dhcp_reservedip_win = null;
function ViewDHCPAdvance(F)
{
	val = F.dhcp_start.value + '_' + F.dhcp_num.value;
	dhcp_reservedip_win = self.open('dhcp_reserved.asp',val,'alwaysRaised,resizable,scrollbars,width=650,height=550');
	dhcp_reservedip_win.focus();
}

function exit()
{
	closeWin(dhcp_reservedip_win);
}

</SCRIPT>

<META content="MSHTML 6.00.2800.1106" name=GENERATOR></HEAD>
<BODY>
<DIV align=center>
<FORM name=setup action=apply.cgi method=post><INPUT type=hidden 
name=submit_button> <INPUT type=hidden name=change_action> <INPUT type=hidden 
name=submit_type> <INPUT type=hidden name=action> <INPUT type=hidden value=0 
name=daylight_time> <INPUT type=hidden value=4 name=lan_ipaddr> <INPUT 
type=hidden name=dhcpserver_ipaddr> <INPUT type=hidden name=vcc_config> <INPUT 
type=hidden name=mtu_config> 
<TABLE cellSpacing=0 cellPadding=0 width=809 border=0>
  <TBODY>
  <TR>
    <TD width=95><IMG height=57 src="image/UI_Linksys.gif" width=165 
      border=0></TD>
    <TD vAlign=bottom align=right width=714 bgColor=#6666cc><FONT 
      style="FONT-SIZE: 7pt" color=#ffffff><FONT face=Arial><script>Capture(share.firmwarever)</script>:&nbsp;1.00.06s&nbsp;&nbsp;&nbsp;</FONT></FONT></TD></TR>
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
            rowSpan=2><FONT color=#ffffff><SPAN style="FONT-SIZE: 8pt"><B><% nvram_get("router_name"); %></B></SPAN></FONT></TD></TR>
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
                <TD align=middle bgColor=#000000 height=20><FONT 
                  style="FONT-WEIGHT: 700" color=#ffffff><A 
                  style="TEXT-DECORATION: none" 
                  href="Wireless_Basic.asp"><script>Capture(bmenu.wireless)</script></A></FONT></TD>
                <TD align=middle bgColor=#000000 height=20><FONT 
                  style="FONT-WEIGHT: 700" color=#ffffff><A 
                  style="TEXT-DECORATION: none" 
                  href="Firewall.asp"><script>Capture(bmenu.security)</script></A></FONT></TD>
                <TD align=middle bgColor=#000000 height=20><FONT 
                  style="FONT-WEIGHT: 700" color=#ffffff><A 
                  style="TEXT-DECORATION: none" 
                  href="Filters.asp"><script>Capture(bmenu.accrestriction)</script></A></FONT></TD>
                <TD align=middle bgColor=#000000 height=20>
                  <P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" 
                  color=#ffffff><A style="TEXT-DECORATION: none" 
                  href="Forward_UPnP.asp"><script>Capture(bmenu.application)</script> 
                  <BR>&amp; <script>Capture(bmenu.gaming)</script></A>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>
                <TD align=middle bgColor=#000000 height=20>
                  <P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" 
                  color=#ffffff><A style="TEXT-DECORATION: none" 
                  href="Management.asp">&nbsp;&nbsp;<script>Capture(bmenu.admin)</script></A>&nbsp;&nbsp;</FONT></P></TD>
                <TD align=middle bgColor=#000000 height=20>
                  <P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" 
                  color=#ffffff><A style="TEXT-DECORATION: none" 
                  href="Status_Router.asp"><script>Capture(bmenu.statu)</script></A>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD></TR>
              <TR>
                <TD width=643 bgColor=#6666cc colSpan=7 height=21>
                  <TABLE borderColor=black height=21 cellSpacing=0 cellPadding=0>
                    <TBODY>
                    <TR align=center>
                      <TD width=40></TD>
                      <TD class=small width=135><A
                        href="index.asp"><script>Capture(settopmenu.basicsetup)</script></A></TD>
                      <TD>
                        <P class=bar><FONT color=white><B>|</B></FONT></P></TD>
                      <TD width=75></TD>
                      <TD class=small width=100><A 
                        href="DDNS.asp"><script>Capture(settopmenu.ddns)</script></A></TD>
                      <TD>
                        <P class=bar><FONT color=white><B>|</B></FONT></P></TD><!--<TD width=22></TD>
                      <TD class=small width=153><A href="WanMAC.asp">MAC Address Clone</A></TD>
                      <TD>
                        <P class=bar><font color='white'><b>|</b></font></P></TD>-->
                      <TD width=30></TD>
                      <TD class=small width=140><A 
                        href="Routing.asp"><script>Capture(settopmenu.advrouting)</script></A></TD><!--
                      <TD><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD width=30></TD>
                      <TD class=small width=140><A href="HotSpot_Admin.asp">Hot Spot</TD>
--></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>
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
            color=#ffffff><script>Capture(setleftmenu.inetsetup)</script></FONT></B></TD>
          <TD width=8 bgColor=#000000 height=25>&nbsp;</TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;</TD>
          <TD width=296 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=1><B><script>Capture(setleftmenu.pvcconn)</script></B></TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=6>
            <TABLE>
              <TBODY>
              <TR>
                <TD width=16 height=1>&nbsp;</TD>
                <TD width=28 height=1>&nbsp;</TD>
                <TD width=150 height=25><script>Capture(setinetsetup.please)</script>:</TD>
                <TD width=230 height=25><SELECT 
                  onchange=SelCONNECTION(this.form.wan_connection.selectedIndex,this.form) 
                  name=wan_connection><option value=0 checked>1<% connection_table(""); %>



<!--
                    <OPTION value=1>2</OPTION> <OPTION value=2>3</OPTION> 
                    <OPTION value=3>4</OPTION> <OPTION value=4>5</OPTION> 
                    <OPTION value=5>6</OPTION> <OPTION value=6>7</OPTION> 
                    <OPTION value=7>8</OPTION>--></SELECT></TD>
                <TD width=15>&nbsp;</TD></TR></TBODY></TABLE></TD>
          <TD width=15 background=image/UI_05.gif height=1>&nbsp;</TD></TR>
        <TR>
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=1></TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=6>
            <TABLE>
              <TBODY>
              <TR>
                <TD width=16 height=1>&nbsp;</TD>
                <TD width=28 height=1>&nbsp;</TD>
                <TD width=90 height=25><script>Capture(setinetsetup.enablenow)</script>:</TD>
                <TD width=296 height=25>&nbsp;<INPUT type=checkbox value=1 name=wan_applyonboot <% vcc_config("applyonboot", 0); %>></TD>
                <TD width=15>&nbsp;</TD></TR></TBODY></TABLE></TD>
          <TD width=15 background=image/UI_05.gif height=1>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=1>&nbsp;</TD>
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
                <TD width=15 height=1>&nbsp;</TD></TR></TBODY></TABLE></TD>
          <TD width=15 background=image/UI_05.gif height=1>&nbsp;</TD></TR>
        <TR>
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 
            height=1><B><script>Capture(setleftmenu.inetconntype)</script></B></TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=6>
            <TABLE>
              <TBODY>
              <TR>
                <TD width=16 height=1>&nbsp;</TD>
                <TD width=28 height=1>&nbsp;</TD>
                <TD width=92 height=25><script>Capture(setinetsetup.encap)</script>:&nbsp;</TD>
                <TD width=296 height=25>&nbsp;RFC 2346 PPPoA<!--<SELECT 
                  onchange=SelTYPE(this.form.wan_encapmode.selectedIndex,this.form) 
                  name=wan_encapmode> <OPTION value=1483bridged>RFC 1483 
                    Bridged</OPTION> <OPTION value=routed>RFC 1483 
                    Routed</OPTION> <OPTION value=pppoe>RFC 2516 PPPoE</OPTION> 
                    <OPTION value=pppoa selected>RFC 2364 PPPoA</OPTION> <OPTION 
                    value=bridgedonly>Bridge Mode Only</OPTION></SELECT>-->
		</TD>
                <TD width=15>&nbsp;</TD></TR></TBODY></TABLE></TD>
          <TD width=15 background=image/UI_05.gif 
        height=1>&nbsp;</TD></TR><INPUT type=hidden value=pppoa 
        name=now_encapmode> 
        
       
       <INPUT type=hidden name=pppoa_config> 
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=1>&nbsp;</TD>
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
                <TD width=15 height=1>&nbsp;</TD></TR></TBODY></TABLE></TD>
          <TD width=15 background=image/UI_05.gif height=1>&nbsp;</TD></TR>
        <TR>
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 
            height=1><B><script>Capture(setother.pppoasetting)</script></B></TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD><!--<TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>-->
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;<script>Capture(setddns.username)</script>:</TD>
          <TD width=296 height=25><INPUT 
            onblur='valid_name(this,"User%20Name",SPACE_NO)' 
            style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" maxLength=63 
            value='<% pppoa_config("username", 0); %>' name=pppoa_username onBlur=valid_name(this,"User%20Name",SPACE_NO)></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;<script>Capture(setddns.password)</script>:&nbsp;&nbsp;</TD>
          <TD width=296 height=25><INPUT 
            onblur='valid_name(this,"Password",SPACE_NO)' 
            style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" type=password 
            maxLength=63 value=value='<% pppoa_config("password", 0); %>' onBlur=valid_name(this,"Password",SPACE_NO) name=pppoa_password></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        
        
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=1>&nbsp;</TD>
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
                <TD width=15 height=1>&nbsp;</TD></TR></TBODY></TABLE></TD>
          <TD width=15 background=image/UI_05.gif height=1>&nbsp;</TD></TR>
	  
	  
      
        <TR>
          <TD width=44 bgColor=#e7e7e7 height=5>&nbsp;</TD>
          <TD width=65 bgColor=#e7e7e7 height=5>&nbsp;</TD>
          <TD width=47 bgColor=#e7e7e7 height=5>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=5>&nbsp;</TD>
          <TD width=454 colSpan=6 height=5></TD>
          <TD width=15 background=image/UI_05.gif 
        height=5>&nbsp;</TD></TR></TBODY></TABLE></TD>
    <TD vAlign=top width=176 bgColor=#6666cc>
      <TABLE cellSpacing=0 cellPadding=0 width=176 border=0>
        <TBODY>
        <TR>
          <TD width=11 bgColor=#6666cc height=25>&nbsp;</TD>
          <TD width=156 bgColor=#6666cc height=25><FONT color=#ffffff><SPAN><A 
            href="help/HSetup.asp" 
            target=_blank>More...</A></SPAN></FONT></TD>
          <TD width=9 bgColor=#6666cc 
  height=25>&nbsp;</TD></TR></TBODY></TABLE></TD></TR>
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
          <TD width=15 height=1>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif>&nbsp;</TD>
          <TD width=176 bgColor=#6666cc rowSpan=2><IMG height=64 
            src="image/UI_Cisco.gif" width=176 border=0></TD></TR>
        <TR>
          <TD width=156 bgColor=#000000>&nbsp;</TD>
          <TD width=8 bgColor=#000000>&nbsp;</TD>
          <TD width=16 bgColor=#6666cc>&nbsp;</TD>
          <TD width=12 bgColor=#6666cc>&nbsp;</TD>
          <TD width=411 bgColor=#6666cc>
            <TABLE height=19 cellSpacing=0 cellPadding=0 width=220 align=right 
            border=0>
              <TBODY>
              <TR>
                <TD align=middle width=101 bgColor=#42498c><FONT 
                  color=#ffffff><B><A 
                  href="javascript:to_submit(document.forms[0]);">Save 
                  Settings</A></B></FONT> </TD>
                <TD align=middle width=8 bgColor=#6666cc>&nbsp;</TD><!-- <TD align=middle bgColor=#434a8f width="103"><FONT color=#ffffff><B><A href='javascript:window.location.reload();'>Cancel Changes</A></B></FONT></TD> -->
                <TD align=middle width=111 bgColor=#434a8f><FONT 
                  color=#ffffff><B><A 
                  href="index.asp">Cancel 
                  Changes</A></B></FONT></TD>
                <TD align=middle width=4 
            bgColor=#6666cc>&nbsp;</TD></TR></TBODY></TABLE></TD>
          <TD width=15 bgColor=#6666cc height=33>&nbsp;</TD>
          <TD width=15 bgColor=#000000 
  height=33>&nbsp;</TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></FORM></DIV></BODY></HTML>
