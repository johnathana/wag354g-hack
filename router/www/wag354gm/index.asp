
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

<HTML><HEAD><TITLE>Setup</TITLE>
<% no_cache(); %>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<link rel="stylesheet" type="text/css" href="style.css">
<style fprolloverstyle>
A:hover {color: #00FFFF}
.small A:hover {color: #00FFFF}
</style>
<script src="common.js"></script>
<script language="JavaScript" type="text/javascript" src="share.js"></script>
<script language="JavaScript" type="text/javascript" src="timezone.js"></script>
<script language="JavaScript" type="text/javascript" src="capsetup.js"></script>
<SCRIPT language=JavaScript>
document.title=(bmenu.setup);
<% wireless_support(5); %>share.productname = share.productname1;<% wireless_support(6); %>
var EN_DIS2 = '<% nvram_get("mtu_enable"); %>';	
var dhcp_win = null;
function SelDSLMode(num, F)
{
	F.submit_button.value = "index";
	F.change_action.value = "gozila_cgi";
	F.dsl_modulation.value = F.dsl_modulation.options[num].value;
	F.submit();
}
function valid_mtu(I)
{
	var start = null;
	var end = null;
<% support_invmatch("UNNUMBERED_SUPPORT", "1", ""); %>
	if(document.setup.now_encapmode.value == "pppoe" || document.setup.now_encapmode.value == "pppoa")
<% support_invmatch("UNNUMBERED_SUPPORT", "1", ""); %>
<% support_invmatch("UNNUMBERED_SUPPORT", "1", "/*"); %>
	if(document.setup.now_encapmode.value == "pppoe" || document.setup.now_encapmode.value == "pppoa" || document.setup.now_encapmode.value == "unpppoe"||document.setup.now_encapmode.value == "unpppoa") // kirby
<% support_invmatch("UNNUMBERED_SUPPORT", "1", "*/"); %>
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
		if(F._daylight_time.checked == false)
			F.daylight_time.value = 0;
		else
			F.daylight_time.value = 1;
/*		
		//leijun 2004-0614
		if(F._share_wanip.checked) F.share_wanip.value = 1;
		else F.share_wanip.value = 0;

		if(F._single_pc_mode.checked) F.single_pc_mode.value = 1;
		else F.single_pc_mode.value = 0;
*/
		F.submit_button.value = "index";
		F.action.value = "Apply";
        	F.submit();
	}
}

function check_vpivci(F)
{
	var vpivci_pairs = new Array(8);
	var i;
	var nowstring = F.wan_vpi.value+"."+F.wan_vci.value;

<% vpivci_getvalue("list"); %>

	for(i=0; i<8; i++)
	{
		if(nowstring == vpivci_pairs[i])
		{
			var j= i + 1;
			//alert("vpi and vci values have conflicts with connection " + j);
			alert(errmsg.err1 + j);
			return false;
		}
	}
	return true;
}

function valid_value(F)
{
	/*if(F.wan_applyonboot.checked==false)
	{
		if(!valid_dhcp_server(F))
			return false;
		if(F.router_name.value == ""){
			//alert("You must input a Router Name!");
			alert(errmsg.err2);
               	 	F.router_name.focus();
               		return false;
		}
		if(valid_mtu_config(F) == false)
		{
			//F.mtu_value.focus();
			return false;
		}
		if(F.now_encapmode.value == "1483bridged")
			F.bridged_config.disabled=true;
		else if(F.now_encapmode.value == "routed")
			F.routed_config.disabled=true;
		else if(F.now_encapmode.value == "pppoe")
			F.pppoe_config.disabled=true;
		else if(F.now_encapmode.value == "pppoa")
			F.pppoa_config.disabled=true;

		return true;
	}*/
		
<% support_invmatch("DSL_MODULA_SEL_SUPPORT", "1", "/*"); %>
      if(F.dsl_modulation.value != "MMODE" && F.dsl_modulation.value != "T1413" && F.dsl_modulation.value != "GDMT" && F.dsl_modulation.value != "GLITE" && F.dsl_modulation.value != "AD2MOD" && F.dsl_modulation.value != "A2PMOD")
		return false;
<% support_invmatch("DSL_MODULA_SEL_SUPPORT", "1", "*/"); %>

	if(F.wan_autodetect[1].checked==true)
	{
		if(!valid_vpi(F.wan_vpi))
		{
			F.wan_vpi.focus();
			return false;
		}
		if(!valid_vci(F.wan_vci))
		{
			F.wan_vci.focus();
			return false;
		}
		if(!check_vpivci(F))
		{
			F.wan_vpi.focus();
			return false;
		}
	}

	if(F.wan_qostype.selectedIndex != 0)
	{
		if(!valid_range(F.wan_pcr,1,65534,"PCR Value"))
		{
			F.wan_pcr.focus();
			return false;
		}
	}
	if(F.wan_qostype.selectedIndex == 2)
	{
		if(!valid_range(F.wan_scr,1,65534,"SCR Value"))
		{
			F.wan_scr.focus();
			return false;
		}
	}
	
	if(F.now_encapmode.value == "1483bridged")
	{
		if(F.bridged_dhcpenable[1].checked == true)
		{
			if(!valid_ip(F,"F.bridged_ipaddr","Bridged IP",ZERO_NO|MASK_NO))
                        {
                                F.bridged_ipaddr_0.focus();
                                return false;
                        }

			if(!valid_range(F.bridged_ipaddr_0,1,223,"IP"))
			{
				F.bridged_ipaddr_0.focus();
				return false;
			}
			if(!valid_range(F.bridged_ipaddr_3,1,254,"IP"))
			{
				F.bridged_ipaddr_3.focus();
				return false;
			}

<% support_match("GATEWAY_BEYOND_SUBNET_SUPPORT", "1", "/*"); %>
			if(!valid_mask(F,"F.bridged_netmask",ZERO_NO|BCST_NO))
<% support_match("GATEWAY_BEYOND_SUBNET_SUPPORT", "1", "*/"); %>
<% support_invmatch("GATEWAY_BEYOND_SUBNET_SUPPORT", "1", "/*"); %>
			if(!valid_mask(F,"F.bridged_netmask",ZERO_NO|BCST_OK))
<% support_invmatch("GATEWAY_BEYOND_SUBNET_SUPPORT", "1", "*/"); %>
			{
                                F.bridged_netmask_0.focus();
                                return false;
                        }

			if(!valid_ip(F,"F.bridged_gateway","Bridged Gateway",ZERO_NO|MASK_NO))
                        {
			        F.bridged_gateway_0.focus();
                                return false;
                        }
			if(!valid_range(F.bridged_gateway_0,1,223,"IP"))
			{
				F.bridged_gateway_0.focus();
				return false;
			}
			if(!valid_range(F.bridged_gateway_3,1,254,"IP"))
			{
				F.bridged_gateway_3.focus();
				return false;
			}

			if(!valid_ip_gw(F,"F.bridged_ipaddr","F.bridged_netmask", "F.bridged_gateway"))
                        {
			        F.bridged_gateway_0.focus();
                                return false;
                        }
			
			if(!valid_ip(F,"F.bridged_pdns","Bridged Primary DNS",MASK_NO))
                        {
			        F.bridged_pdns_0.focus();
                                return false;
                        }
			/*
			if(!valid_range(F.bridged_pdns_0,1,223,"IP"))
				return false;
			if(!valid_range(F.bridged_pdns_3,1,254,"IP"))
				return false;
			*/	
			if(!valid_ip(F,"F.bridged_sdns","Bridged Secondary DNS",MASK_NO))
                        {
			        F.bridged_sdns_0.focus();
                                return false;
                        }
			/*
			if(!valid_range(F.bridged_sdns_0,1,223,"IP"))
				return false;
			if(!valid_range(F.bridged_sdns_3,1,254,"IP"))
				return false;   
			*/   
			
		}
		<% ipppoe_support(3); %>
		if(F.ipppoe_enable.selectedIndex == 1)
		{
		if(F.ipppoe_username.value == "")
		{
			//alert("You must input a username!");
			alert(errmsg.err3);
			F.ipppoe_username.focus();
			return false;
		}
		if(F.ipppoe_password.value == "")
		{
			//alert("You must input a password!");
			alert(errmsg.err4);
			F.ipppoe_password.focus();
			return false;
		}
		if(F.ipppoe_demand[0].checked == true)
		{
			if(valid_range(F.ipppoe_idletime,1,9999,"Idle%20time") == false)
			{
				F.ipppoe_idletime.focus();
				return false;
			}
		}
		else if(F.ipppoe_demand[1].checked == true)
		{
			if(valid_range(F.ipppoe_redialperiod,20,180,"Redial%20period")  == false)
			{
				F.ipppoe_redialperiod.focus();
				return false;
			}
		}
		}
		<% ipppoe_support(4); %>
	}
<% support_elsematch("CLIP_SUPPORT", "1", "", "/*"); %>     	
	else if(F.now_encapmode.value == "clip")
	{
		if(!valid_ip(F,"F.clip_ipaddr","IPoA IP",ZERO_NO|MASK_NO))
		{
                        F.clip_ipaddr_0.focus();
                        return false;
		}
		if(!valid_range(F.clip_ipaddr_0,1,223,"IP"))
		{
			F.clip_ipaddr_0.focus();
			return false;
		}
		if(!valid_range(F.clip_ipaddr_3,1,254,"IP"))
		{
			F.clip_ipaddr_3.focus();
			return false;
		}

		if(!valid_mask(F,"F.clip_netmask",ZERO_NO|BCST_NO))
		{
                        F.clip_netmask_0.focus();
                        return false;
		}

		if(!valid_ip(F,"F.clip_gateway","IPoA Gateway",ZERO_NO|MASK_NO))
		{
                        F.clip_gateway_0.focus();
                        return false;
		}
		if(!valid_range(F.clip_gateway_0,1,223,"IP"))
		{
			F.clip_gateway_0.focus();
			return false;
		}
		if(!valid_range(F.clip_gateway_3,1,254,"IP"))
		{
			F.clip_gateway_3.focus();
			return false;
		}
		
		if(!valid_ip_gw(F,"F.clip_ipaddr","F.clip_netmask", "F.clip_gateway"))
                {
			F.clip_gateway_0.focus();
                        return false;
                }

		if(!valid_ip(F,"F.clip_pdns","Clip Primary DNS",MASK_NO))
		{
                        F.clip_pdns_0.focus();
                        return false;
		}
		if(!valid_ip(F,"F.clip_sdns","Clip Secondary DNS",MASK_NO))
		{
                        F.clip_sdns_0.focus();
                        return false;
		}
	}
<% support_elsematch("CLIP_SUPPORT", "1", "", "*/"); %>     	

	else if(F.now_encapmode.value == "routed")
	{
		if(!valid_ip(F,"F.routed_ipaddr","Routed IP",ZERO_NO|MASK_NO))
		{
                        F.routed_ipaddr_0.focus();
                        return false;
		}
		if(!valid_range(F.routed_ipaddr_0,1,223,"IP"))
		{
			F.routed_ipaddr_0.focus();
			return false;
		}
		if(!valid_range(F.routed_ipaddr_3,1,254,"IP"))
		{
			F.routed_ipaddr_3.focus();
			return false;
		}

		if(!valid_mask(F,"F.routed_netmask",ZERO_NO|BCST_NO))
		{
                        F.routed_netmask_0.focus();
                        return false;
		}

		if(!valid_ip(F,"F.routed_gateway","Routed Gateway",ZERO_NO|MASK_NO))
		{
                        F.routed_gateway_0.focus();
                        return false;
		}
		if(!valid_range(F.routed_gateway_0,1,223,"IP"))
		{
			F.routed_gateway_0.focus();
			return false;
		}
		if(!valid_range(F.routed_gateway_3,1,254,"IP"))
		{
			F.routed_gateway_3.focus();
			return false;
		}
		
		if(!valid_ip_gw(F,"F.routed_ipaddr","F.routed_netmask", "F.routed_gateway"))
                {
			F.routed_gateway_0.focus();
                        return false;
                }

		if(!valid_ip(F,"F.routed_pdns","Routed Primary DNS",MASK_NO))
		{
                        F.routed_pdns_0.focus();
                        return false;
		}
		/*
		if(!valid_range(F.routed_pdns_0,1,223,"IP"))
			return false;
		if(!valid_range(F.routed_pdns_3,1,254,"IP"))
			return false;
		*/
		if(!valid_ip(F,"F.routed_sdns","Routed Secondary DNS",MASK_NO))
		{
                        F.routed_sdns_0.focus();
                        return false;
		}
		/*
		if(!valid_range(F.routed_sdns_0,1,223,"IP"))
			return false;
		if(!valid_range(F.routed_sdns_3,1,254,"IP"))
			return false;
		*/
	}
	else if(F.now_encapmode.value == "pppoe")
	{
		if(F.pppoe_username.value == "")
		{
			//alert("You must input a username!");
			alert(errmsg.err3);
			F.pppoe_username.focus();
			return false;
		}
		if(F.pppoe_password.value == "")
		{
			//alert("You must input a password!");
			alert(errmsg.err4);
			F.pppoe_password.focus();
			return false;
		}
		if(F.pppoe_demand[0].checked == true)
		{
			if(valid_range(F.pppoe_idletime,1,9999,"Idle%20time") == false)
			{
				F.pppoe_idletime.focus();
				return false;
			}
		}
		else if(F.pppoe_demand[1].checked == true)
		{
			if(valid_range(F.pppoe_redialperiod,20,180,"Redial%20period")  == false)
			{
				F.pppoe_redialperiod.focus();
				return false;
			}
		}
		
		<% mpppoe_support(3); %>
		if(F.mpppoe_enable.selectedIndex == 1)
			//return true;
		{	
		if(F.mpppoe_username.value == "")
		{
			//alert("You must input a username!");
			alert(errmsg.err3);
			F.mpppoe_username.focus();
			return false;
		}
		if(F.mpppoe_password.value == "")
		{
			//alert("You must input a password!");
			alert(errmsg.err4);
			F.mpppoe_password.focus();
			return false;
		}
		if(F.mpppoe_demand[0].checked == true)
		{
			if(valid_range(F.mpppoe_idletime,1,9999,"Idle%20time") == false)
			{
				F.mpppoe_idletime.focus();
				return false;
			}
		}
		else if(F.mpppoe_demand[1].checked == true)
		{
			if(valid_range(F.mpppoe_redialperiod,20,180,"Redial%20period")  == false)
			{
				F.mpppoe_redialperiod.focus();
				return false;
			}
		}
		}
		<% mpppoe_support(4); %>
	}
	else if(F.now_encapmode.value == "pppoa")
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
			//alert("You must input a password!");
			alert(errmsg.err4);
			F.pppoa_password.focus();
			return false;
		}
		if(F.pppoa_demand[0].checked == true)
		{
			if(valid_range(F.pppoa_idletime,1,9999,"Idle%20time") == false)
			{
				F.pppoa_idletime.focus();
				return false;
			}
		}
		else if(F.pppoa_demand[1].checked == true)
		{
			if(valid_range(F.pppoa_redialperiod,20,180,"Redial%20period")  == false)
			{
				F.pppoa_redialperiod.focus();
				return false;
			}
		}
	}
<% support_invmatch("UNNUMBERED_SUPPORT", "1", "/*"); %>
    else if(F.now_encapmode.value == "unpppoe") // kirby
	{
		if(F.unip_pppoe_username.value == "")
		{
			//alert("You must input a username!");
			alert(errmsg.err3);
			F.unip_pppoe_username.focus();
			return false;
		}
		if(F.unip_pppoe_password.value == "")
		{
			//alert("You must input a password!");
			alert(errmsg.err4);
			F.unip_pppoe_password.focus();
			return false;
		}
		if(F.unip_pppoe_demand[0].checked == true)
		{
			if(valid_range(F.unip_pppoe_idletime,1,9999,"Idle%20time") == false)
			{
				F.unip_pppoe_idletime.focus();
				return false;
			}
		}
		else if(F.unip_pppoe_demand[1].checked == true)
		{
			if(valid_range(F.unip_pppoe_redialperiod,20,180,"Redial%20period")  == false)
			{
				F.unip_pppoe_redialperiod.focus();
				return false;
			}
		}
	}

	else if(F.now_encapmode.value == "unpppoa")
	{
		if(F.unip_pppoa_username.value == "")
		{
			//alert("You must input a username!");
			alert(errmsg.err3);
			F.unip_pppoa_username.focus();
			return false;
		}
		if(F.unip_pppoa_password.value == "")
		{
			//alert("You must input a password!");
			alert(errmsg.err4);
			F.unip_pppoa_password.focus();
			return false;
		}
		if(F.unip_pppoa_demand[0].checked == true)
		{
			if(valid_range(F.unip_pppoa_idletime,1,9999,"Idle%20time") == false)
			{
				F.unip_pppoa_idletime.focus();
				return false;
			}
		}
		else if(F.unip_pppoa_demand[1].checked == true)
		{
			if(valid_range(F.unip_pppoa_redialperiod,20,180,"Redial%20period")  == false)
			{
				F.unip_pppoa_redialperiod.focus();
				return false;
			}
		}
	}

<% support_invmatch("UNNUMBERED_SUPPORT", "1", "*/"); %>

	if(F.now_encapmode.value == "bridgedonly")
		F.lan_proto.disabled = false;
		
	if(!valid_dhcp_server(F))
		return false;
	
	/*
	if(F.router_name.value == ""){
		//alert("You must input a Router Name!");
		alert(errmsg.err2);
                F.router_name.focus();
                return false;
	}
	*/
	if(valid_mtu_config(F) == false)
	{
		//F.mtu_value.focus();
		return false;
	}

	if(!valid_ip(F,"F.lan_ipaddr","Lan IP",ZERO_NO|MASK_NO))
	{
                F.lan_ipaddr_0.focus();
                return false;
	}
	if(!valid_range(F.lan_ipaddr_0,1,223,"IP"))
	{
		F.lan_ipaddr_0.focus();
		return false;
	}
	if(!valid_range(F.lan_ipaddr_3,1,254,"IP"))
	{
		F.lan_ipaddr_3.focus();
		return false;
	}

        if(!valid_range(F.timer_interval,60,9999,"SNTP"))
	{
		F.timer_interval.focus();
		return false;
	} 
	return true;
}

function valid_dhcpusers(F)
{	
	var valuetmp;
	isdigit(F.dhcp_num,"DHCP users number");
	isdigit(F.dhcp_start,"DHCP users start");
        a0 = parseInt(F.lan_ipaddr_3.value,10);
        a1 = parseInt(F.dhcp_start.value,10);
        a3 = parseInt(F.dhcp_num.value,10);

	if(F.dhcp_start.value == F.lan_ipaddr_3.value)
	{
               	//alert("DHCP start ip is the same with lan ip address");
		alert(errmsg.err5);
                return false;
	}
	
	switch(F.lan_netmask.selectedIndex)
	{
		case 0:
			a2 = 256;
			break;
		case 1:
			a2 = 128;
			break;
		case 2:
			a2 = 64;
			break;
		case 3:
			a2 = 32;
			break;
		case 4:
			a2 = 16;
			break;
		case 5:
			a2 = 8;
			break;
		case 6:
			a2 = 4;
			break;
		default:
			a2 = 256;
			break;
	}
	
	a4 = a1 & (256 - a2);
	a5 = a0 & (256 - a2);
	
	if((a0 - a5) < (a1 - a4))
		valuetmp = a2 - 1 - (a1 - a4);
	else
		valuetmp = a2 - 2 - (a1 - a4);
	
	if(a3 < 1 || a3 > valuetmp)
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

        a0 = parseInt(F.lan_ipaddr_3.value,10);
        a1 = parseInt(F.dhcp_start.value,10);
	
	if(F.dhcp_start.value == F.lan_ipaddr_3.value)
	{
               	//alert("DHCP start ip address is the same with lan ip address");
               	alert(errmsg.err5);

                return false;
	}
	
	switch(F.lan_netmask.selectedIndex)
	{
		case 0:
			a2 = 256;
			break;
		case 1:
			a2 = 128;
			break;
		case 2:
			a2 = 64;
			break;
		case 3:
			a2 = 32;
			break;
		case 4:
			a2 = 16;
			break;
		case 5:
			a2 = 8;
			break;
		case 6:
			a2 = 4;
			break;
		default:
			a2 = 256;
			break;
	}
	
	if((a3 = (a1 & (256 - a2))) != (a0 & (256 - a2)))
	{
               	//alert("DHCP start ip address with lan ipaddress isn't in the same network");
		alert(errmsg.err9);

                return false;
	}
	
	a4 = a3 + 1;
	a5 = a3 + a2 - 2
	
	if(a1 < a4 || a1 > a5)
	{
               // alert("DHCP start ip is out of range. Must be [" + a4 + "-" + a5 + "]");
                alert(errmsg.err10 + "[" + a4 + "-" + a5 + "]");

                return false;
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
               // alert("Out of range, please adjust start IP address or user's numbers.");
		alert(errmsg.err11);

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
        <% support_invmatch("MULTIPVC_SUPPORT", "1", "/*"); %>
                F.dhcp_advance.disabled = true;
        <% support_invmatch("MULTIPVC_SUPPORT", "1", "*/"); %>
        <% support_invmatch("AUTODETECT_DHCP_SUPPORT", "1", "/*"); %>
                //add by lzh;
                choose_enable(F.lan_detect_dhcp[0]);
                choose_enable(F.lan_detect_dhcp[1]);
        <% support_invmatch("AUTODETECT_DHCP_SUPPORT", "1", "*/"); %>
	}
	else if( T == "dhcp") {
		EN_DIS = 1;
		for(i = start; i<=end ;i++)
			choose_enable(F.elements[i]);
		choose_disable(F.dhcpserver_ipaddr_0);
		choose_disable(F.dhcpserver_ipaddr_1);
		choose_disable(F.dhcpserver_ipaddr_2);
		choose_disable(F.dhcpserver_ipaddr_3);
        <% support_invmatch("MULTIPVC_SUPPORT", "1", "/*"); %>
                F.dhcp_advance.disabled = false;
        <% support_invmatch("MULTIPVC_SUPPORT", "1", "*/"); %>
        <% support_invmatch("AUTODETECT_DHCP_SUPPORT", "1", "/*"); %>
                //add by lzh;
                choose_enable(F.lan_detect_dhcp[0]);
                choose_enable(F.lan_detect_dhcp[1]);
        <% support_invmatch("AUTODETECT_DHCP_SUPPORT", "1", "*/"); %>
	}
	else if( T == "dhcprelay" ) {
		EN_DIS = 0;
		for(i = start; i<=end ;i++)
			choose_disable(F.elements[i]);
		choose_enable(F.dhcpserver_ipaddr_0);
		choose_enable(F.dhcpserver_ipaddr_1);
		choose_enable(F.dhcpserver_ipaddr_2);
		choose_enable(F.dhcpserver_ipaddr_3);
        <% support_invmatch("MULTIPVC_SUPPORT", "1", "/*"); %>
                F.dhcp_advance.disabled = true;
        <% support_invmatch("MULTIPVC_SUPPORT", "1", "*/"); %>
        <% support_invmatch("AUTODETECT_DHCP_SUPPORT", "1", "/*"); %>
                //add by lzh;
                choose_disable(F.lan_detect_dhcp[0]);
                choose_disable(F.lan_detect_dhcp[1]);
        <% support_invmatch("AUTODETECT_DHCP_SUPPORT", "1", "*/"); %>
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

<% support_invmatch("UNNUMBERED_SUPPORT", "1", "/*"); %>
//add by kirby;
function unip_pppoe_enable_disable(F,I)
{
	if( I == "0"){
		choose_disable(F.unip_pppoe_idletime);
		choose_enable(F.unip_pppoe_redialperiod);
	}
	else{
		choose_enable(F.unip_pppoe_idletime);
		choose_disable(F.unip_pppoe_redialperiod);
	}
}

//add by kirby;
function unip_pppoa_enable_disable(F,I)
{
	if( I == "0"){
		choose_disable(F.unip_pppoa_idletime);
		choose_enable(F.unip_pppoa_redialperiod);
	}
	else{
		choose_enable(F.unip_pppoa_idletime);
		choose_disable(F.unip_pppoa_redialperiod);
	}
}

<% support_invmatch("UNNUMBERED_SUPPORT", "1", "*/"); %>

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

//add by lzh;
function SelLanDetectDhcp(num,T,F)
{
	if (num == 1)
	{
		choose_disable(F.lan_proto[2]);	
		choose_disable(F.dhcpserver_ipaddr_0);
		choose_disable(F.dhcpserver_ipaddr_1);
		choose_disable(F.dhcpserver_ipaddr_2);
		choose_disable(F.dhcpserver_ipaddr_3);
        <% support_invmatch("MULTIPVC_SUPPORT", "1", "/*"); %>
                F.dhcp_advance.disabled = true;
        <% support_invmatch("MULTIPVC_SUPPORT", "1", "*/"); %>
	}else
	{
		choose_enable(F.lan_proto[2]);	
		if (T == "dhcprelay")
		{
			choose_enable(F.dhcpserver_ipaddr_0);
			choose_enable(F.dhcpserver_ipaddr_1);
			choose_enable(F.dhcpserver_ipaddr_2);
			choose_enable(F.dhcpserver_ipaddr_3);
		}else{
			choose_disable(F.dhcpserver_ipaddr_0);
			choose_disable(F.dhcpserver_ipaddr_1);
			choose_disable(F.dhcpserver_ipaddr_2);
			choose_disable(F.dhcpserver_ipaddr_3);
		}
        <% support_invmatch("MULTIPVC_SUPPORT", "1", "/*"); %>
                F.dhcp_advance.disabled = true;
        <% support_invmatch("MULTIPVC_SUPPORT", "1", "*/"); %>
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

	//junzhao 2004.10.22
	if(document.setup.now_encapmode.value == "clip")
	{
		document.setup.wan_multiplex[0].checked = true;
		document.setup.wan_multiplex[0].disabled = true;
		document.setup.wan_multiplex[1].disabled = true;
	}
	else	
	{
		document.setup.wan_multiplex[0].disabled = false;
		document.setup.wan_multiplex[1].disabled = false;
	}
	
	if(document.setup.now_encapmode.value == "bridgedonly")
	{
		document.setup.lan_proto[1].checked = true;
		dhcp_enable_disable(document.setup,'static');
		document.setup.lan_proto[0].disabled = true;
		document.setup.lan_proto[1].disabled = true;
		document.setup.lan_proto[2].disabled = true;
		//add by lzh;
		//document.setup.lan_detect_dhcp[0].disabled = true;
		//document.setup.lan_detect_dhcp[1].disabled = true;
	}
	else
	{
		document.setup.lan_proto[0].disabled = false;
		document.setup.lan_proto[1].disabled = false;
		document.setup.lan_proto[2].disabled = false;
		dhcp_enable_disable(document.setup,'<% nvram_get("lan_proto"); %>');
		//add by lzh;
		SelLanDetectDhcp('<% nvram_get("lan_detect_dhcp"); %>','<% nvram_get("lan_proto"); %>',document.setup);
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

<% support_invmatch("CLOUD_SUPPORT", "1", "/*"); %>
//junzhao add for cloud
function SelCloud(num, F)
{
        if(num == 1)
                choose_enable(F.cloud_config);
        else if(num == 0)
                choose_disable(F.cloud_config);
}

var cloud_win = null;
function ViewCloudwin(F)
{
        val = F.wan_connection.value;
        cloud_win = self.open('Cloud.asp',val,'alwaysRaised,resizable,scrollbars,width=650,height=550');
        cloud_win.focus();
}
<% support_invmatch("CLOUD_SUPPORT", "1", "*/"); %>

function exit()
{
        closeWin(dhcp_reservedip_win);
<% support_invmatch("CLOUD_SUPPORT", "1", "/*"); %>
        closeWin(cloud_win);
<% support_invmatch("CLOUD_SUPPORT", "1", "*/"); %>
}

</SCRIPT>
</HEAD>
<BODY onload="init()" onunload=exit()>
<DIV align=center>
<FORM name=setup method=<% get_http_method(); %> action=apply.cgi>
<input type=hidden name=submit_button>
<input type=hidden name=change_action>
<input type=hidden name=submit_type>
<input type=hidden name=action>
<input type=hidden name=daylight_time value=0>
<input type=hidden name=lan_ipaddr value=4>
<input type=hidden name=dhcpserver_ipaddr>
<input type=hidden name=vcc_config>
<input type=hidden name=mtu_config>
<TABLE cellSpacing=0 cellPadding=0 width=809 border=0>
  <TBODY>
  <TR>
    <TD width=95>
    <IMG src="image/UI_Linksys.gif" 
      border=0 width="165" height="57"></TD>
    <TD vAlign=bottom align=right width=714 bgColor=#6666cc><FONT 
      style="FONT-SIZE: 7pt" color=#ffffff><FONT face=Arial><script>Capture(share.firmwarever)</script>:&nbsp;<% get_firmware_version(); %>&nbsp;&nbsp;&nbsp;</FONT></FONT></TD></TR>
  <TR>
    <TD width=808 bgColor=#6666cc colSpan=2>
    <IMG height=11 
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
                  style="FONT-WEIGHT: 700" color=#ffffff>
                <A 
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
                <TD width=643 bgColor=#6666cc colSpan=7 height=21>
                  <TABLE borderColor=black height=21 cellSpacing=0 cellPadding=0>
                    <TBODY>
                    <TR align=center>
                      <TD class=small width=140><FONT style="COLOR: white"><script>Capture(settopmenu.basicsetup)</script></FONT></TD>
                      <TD><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=100><A href="DDNS.asp"><script>Capture(settopmenu.ddns)</script></A></TD>
                      <TD><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <!--<TD class=small width=153><A href="WanMAC.asp">MAC Address Clone</A></TD>
                      <TD><P class=bar><font color='white'><b>|</b></font></P></TD>-->
                      <TD class=small width=140><A href="Routing.asp"><script>Capture(settopmenu.advrouting)</script></TD>
<% support_invmatch("DHCP_DOMAIN_IP_MAPPING_SUPPORT", "1", "<!--"); %>
                      <TD><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=100><A href="VOIP.asp"><script>Capture(settopmenu.voip)</script></A></TD>
<% support_invmatch("DHCP_DOMAIN_IP_MAPPING_SUPPORT", "1", "-->"); %>
<% support_invmatch("HSIAB_SUPPORT", "1", "<!--"); %>
                      <TD><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=120><A href="HotSpot_Admin.asp"><script>Capture(settopmenu.hotspot)</script></TD>
<% support_invmatch("HSIAB_SUPPORT", "1", "-->"); %>
                    </TR>
                    </TBODY>
                  </TABLE>
                </TD>
              </TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>
<TABLE height=5 cellSpacing=0 cellPadding=0 width=806 bgColor=black border=0>
  <TBODY>
  <TR bgColor=black>
    <TD 
    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
    borderColor=#e7e7e7 width=163 bgColor=#e7e7e7 height=1>
    <IMG height=15 
      src="image/UI_03.gif" width=164 border=0></TD>
    <TD 
    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
    width=646 height=1>
    <IMG height=15 src="image/UI_02.gif" width=645 
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
<% support_invmatch("MULTIPVC_SUPPORT", "1", "<!--"); %>
        <TR>
	  <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=1><B><script>Capture(setleftmenu.pvcconn)</script></B></TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=6>
            <TABLE>
              <TBODY>
              <TR>
                <TD width=16 height=1>&nbsp;</TD>
                <TD width=28 height=1>&nbsp;</TD> 
		<TD width=150 height=25><script>Capture(setinetsetup.please)</script>:</TD>            		<TD width=230 height=25><SELECT name="wan_connection" onChange=SelCONNECTION(this.form.wan_connection.selectedIndex,this.form)>
<% connection_table("select"); %></SELECT></TD>			 
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
          	<TD width=296 height=25>&nbsp;<input type=checkbox value=1 name=wan_applyonboot <% vcc_config("applyonboot", 0); %>></TD>
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
<% support_invmatch("MULTIPVC_SUPPORT", "1", "-->"); %>

<% support_match("MULTIPVC_SUPPORT", "1", "<!--"); %>
<input type=hidden name="wan_connection" value=0>
<input type=hidden name="wan_applyonboot" value=1>
<% support_match("MULTIPVC_SUPPORT", "1", "-->"); %>

          <TR>
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=1><B><script>Capture(setleftmenu.inetconntype)</script></B></TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=6>
            <TABLE>
              <TBODY>
              <TR>
                <TD width=16 height=1>&nbsp;</TD>
                <TD width=28 height=1>&nbsp;</TD> 
          	<TD width=92 height=25><script>Capture(setinetsetup.encap)</script>:&nbsp;</TD>
          	<TD width=296 height=25>&nbsp;<SELECT name="wan_encapmode" onChange=SelTYPE(this.form.wan_encapmode.selectedIndex,this.form)>
	                <OPTION value="1483bridged" <% vcc_config("encapmode", 0); %>>RFC <script>Capture(share.bridged)</script></OPTION> 
                     	<OPTION value="routed" <% vcc_config("encapmode", 1); %>>RFC <script>Capture(share.routed)</script></OPTION>
                <% support_elsematch("CLIP_SUPPORT", "1", "", "<!--"); %>     	
			<OPTION value="clip" <% vcc_config("encapmode", 7); %>>IPoA</OPTION>
                <% support_elsematch("CLIP_SUPPORT", "1", "", "-->"); %>     	
	                <OPTION value="pppoe" <% vcc_config("encapmode", 2); %>>RFC 2516 PPPoE</OPTION> 
			<OPTION value="pppoa" <% vcc_config("encapmode", 3); %>>RFC 2364 PPPoA</OPTION>

<% support_invmatch("UNNUMBERED_SUPPORT", "1", "<!--"); %>
			<OPTION value="unpppoe" <% vcc_config("encapmode", 8); %>><script>Capture(share.unnumpppoe)</script></OPTION>
			<OPTION value="unpppoa" <% vcc_config("encapmode", 9); %>><script>Capture(share.unnumpppoa)</script></OPTION>
<% support_invmatch("UNNUMBERED_SUPPORT", "1", "-->"); %>

                        <!--<OPTION value="pptp" <% vcc_config("encapmode", 4); %>>PPTP</OPTION>-->
			<OPTION value="bridgedonly" <% vcc_config("encapmode", 5); %>><script>Capture(share.bridgeonly)</script></OPTION>
			</SELECT></TD>
                <TD width=15>&nbsp;</TD></TR></TBODY></TABLE></TD>
                <TD width=15 background=image/UI_05.gif height=1>&nbsp;</TD>	
		<input type=hidden name=now_encapmode value=<% vcc_config("encapmode", 6); %>>
<!--	<TR>	                        
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=1><B>VC Settings</B></TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=6>
	  <TABLE>
              <TBODY>
              <TR>
                <TD width=16 height=1>&nbsp;</TD>
                <TD width=28 height=1>&nbsp;</TD> 
          	<TD width=92 height=25>Virtual Circuit:&nbsp;</TD>
          	<TD width=296 height=25>&nbsp;<INPUT onblur=vpiCheck(this) maxLength=3 size=8 value='<% vcc_config("vpi", 0); %>' 
            name=wan_vpi>&nbsp;&nbsp;VPI (Range 0~255)</TD>
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
          	<TD width=92 height=25>&nbsp;&nbsp;</TD>
          	<TD width=296 height=25>&nbsp;<INPUT onblur=vciCheck(this) maxLength=5 size=8 value='<% vcc_config("vci", 0); %>' 
            name=wan_vci>&nbsp;&nbsp;VCI (Range 32~65535)</TD>
                <TD width=15>&nbsp;</TD></TR></TBODY></TABLE></TD>
                <TD width=15 background=image/UI_05.gif height=1>&nbsp;</TD></TR>           
-->	 
	<TR>	                        
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=1><B><script>Capture(setleftmenu.vcsetting)</script></B></TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=6>
	  <TABLE>
              <TBODY>
              <TR>
                <TD width=16 height=1>&nbsp;</TD>
                <TD width=28 height=1>&nbsp;</TD> 
          	<TD width=92 height=25><script>Capture(setinetsetup.multiplexing)</script>:&nbsp;</TD>
          	<TD width=296 height=25>&nbsp;<INPUT type=radio value=llc name=wan_multiplex <% vcc_config("multiplex", 0); %>><b> 
            LLC</b>&nbsp;<input type=radio value=vc name=wan_multiplex <% vcc_config("multiplex", 1); %>><b> 
          VC</b></TD>
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
          	<TD width=92 height=25><script>Capture(setinetsetup.qostype)</script>:&nbsp;</TD>
          	<TD width=296 height=25>&nbsp;<SELECT name="wan_qostype" onChange=SelQOS(this.form.wan_qostype.selectedIndex,this.form)>
	                <OPTION value="ubr" <% vcc_config("qos", 0); %>>UBR</OPTION> 
                     	<OPTION value="cbr" <% vcc_config("qos", 1); %>>CBR</OPTION>
	                <OPTION value="vbr" <% vcc_config("qos", 2); %>>VBR</OPTION> 
			</SELECT></TD>
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
          	<TD width=92 height=25><script>Capture(setinetsetup.pcrrate)</script>:&nbsp;</TD>
          	<TD width=296 height=25>&nbsp;<INPUT class=num maxLength=5 size=5 value='<% vcc_config("pcr", 0); %>' 
            name=wan_pcr <% vcc_config("pcr", 1); %> onBlur=valid_range(this,1,65534,"PCR")>&nbsp;cps&nbsp;</TD>
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
          	<TD width=92 height=25><script>Capture(setinetsetup.scrrate)</script>:&nbsp;</TD>
          	<TD width=296 height=25>&nbsp;<INPUT class=num maxLength=5 size=5 value='<% vcc_config("scr", 0); %>'
            name=wan_scr <% vcc_config("scr", 1); %> onBlur=valid_range(this,1,65534,"SCR")>&nbsp;cps&nbsp;</TD>
                <TD width=15>&nbsp;</TD></TR></TBODY></TABLE></TD>
                <TD width=15 background=image/UI_05.gif height=1>&nbsp;</TD></TR>              
<!--	 
	<TR>	                        
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=1></TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=6>
	  <TABLE>
              <TBODY>
              <TR>
                <TD width=16 height=1>&nbsp;</TD>
                <TD width=28 height=1>&nbsp;</TD> 
          	<TD width=92 height=25>&nbsp;</TD>
                <TD width=15>&nbsp;</TD></TR></TBODY></TABLE></TD>
                <TD width=15 background=image/UI_05.gif height=1>&nbsp;</TD></TR>               	
-->	
	<TR>	                        
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=1></TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=6>
	  <TABLE>
              <TBODY>
              <TR>
                <TD width=16 height=1>&nbsp;</TD>
                <TD width=28 height=1>&nbsp;</TD> 
          	<TD width=92 height=25><script>Capture(setinetsetup.autodetect)</script>:&nbsp;</TD>
          	<TD width=296 height=25>&nbsp;<INPUT type=radio value=1 name=wan_autodetect <% vcc_config("autodetect", 1); %> onClick="SelAutodetect(1, this.form)"><b> 
            <script>Capture(share.enabled)</script></b>&nbsp;<input type=radio value=0 name=wan_autodetect <% vcc_config("autodetect", 0); %>  onClick="SelAutodetect(0, this.form)"><b><script>Capture(share.disabled)</script></b></TD>
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
          	<TD width=92 height=25><script>Capture(setinetsetup.vircir)</script>:&nbsp;</TD>
          	<TD width=296 height=25>&nbsp;<INPUT class=num onblur=vpiCheck(this) maxLength=3 size=5 value=<% vcc_config("vpi", 0); %> 
            name=wan_vpi>&nbsp;&nbsp;VPI (<script>Capture(share.range)</script> 0~255)</TD>
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
          	<TD width=92 height=25>&nbsp;&nbsp;</TD>
          	<TD width=296 height=25>&nbsp;<INPUT class=num onblur=vciCheck(this) maxLength=5 size=5 value=<% vcc_config("vci", 0); %>
            name=wan_vci>&nbsp;&nbsp;VCI (<script>Capture(share.range)</script> 32~65535)</TD>
                <TD width=15>&nbsp;</TD></TR></TBODY></TABLE></TD>
                <TD width=15 background=image/UI_05.gif height=1>&nbsp;</TD></TR>           
<% support_invmatch("DSL_MODULA_SEL_SUPPORT", "1", "<!--"); %>
        <TR>
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=1></TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=6>
          <TABLE>
              <TBODY>
              <TR>
                <TD width=16 height=1>&nbsp;</TD>
                <TD width=28 height=1>&nbsp;</TD>
                <TD width=92 height=25>DSL Modulation:&nbsp;</TD>
                <TD width=296 height=25>&nbsp;<SELECT name="dsl_modulation" onChange=SelDSLMode(this.form.dsl_modulation.selectedIndex,this.form)>
                	<OPTION value="MMODE" <% nvram_selmatch("dsl_modulation", "MMODE", "selected"); %>>MultiMode</OPTION>
			<OPTION value="T1413" <% nvram_selmatch("dsl_modulation", "T1413", "selected"); %>>T1.413</OPTION>
                	<OPTION value="GDMT" <% nvram_selmatch("dsl_modulation", "GDMT", "selected"); %>>G.dmt</OPTION>
                	<OPTION value="GLITE" <% nvram_selmatch("dsl_modulation", "GLITE", "selected"); %>>G.lite</OPTION>
                	<OPTION value="AD2MOD" <% nvram_selmatch("dsl_modulation", "AD2MOD", "selected"); %>>ADSL2</OPTION>
                	<OPTION value="A2PMOD" <% nvram_selmatch("dsl_modulation", "A2PMOD", "selected"); %>>ADSL2+</OPTION></SELECT></TD>
                <TD width=15>&nbsp;</TD></TR></TBODY></TABLE></TD>
                <TD width=15 background=image/UI_05.gif height=1>&nbsp;</TD></TR>
<% support_invmatch("DSL_MODULA_SEL_SUPPORT", "1", "-->"); %>
<!--
	<TR>	                        
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=1></TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=6>
	  <TABLE>
              <TBODY>
              <TR>
                <TD width=16 height=1>&nbsp;</TD>
                <TD width=28 height=1>&nbsp;</TD> 
          	<TD width=92 height=25>&nbsp;</TD>
                <TD width=15>&nbsp;</TD></TR></TBODY></TABLE></TD>
                <TD width=15 background=image/UI_05.gif height=1>&nbsp;</TD></TR>               	
-->
<!--	<TR>	                        
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=1></TD>
          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>
          <TD colSpan=6>
	  <TABLE>
              <TBODY>
              <TR>
                <TD width=16 height=1>&nbsp;</TD>
                <TD width=28 height=1>&nbsp;</TD> 
          	<TD width=92 height=25>&nbsp;Enable:&nbsp;</TD>
          	<TD width=296 height=25>&nbsp;<input type=checkbox value=1 name=wan_applyonboot <% vcc_config("applyonboot", 0); %>></TD>
                <TD width=15>&nbsp;</TD></TR></TBODY></TABLE></TD>
                <TD width=15 background=image/UI_05.gif height=1>&nbsp;</TD></TR>-->           	 
        <% show_index_setting(); %>
        
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
        <!--
	<TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>
            <P align=right><FONT style="FONT-WEIGHT: 700"><B>Optional Settings</B><BR><B>(required by some ISPs)</B></FONT></P></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial>&nbsp;Router Name:&nbsp;</FONT></TD>
          <TD width=296 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial><INPUT maxLength=39 name="router_name" size="20" value='<% nvram_get("router_name"); %>' onBlur=valid_name(this,"Router%20Name")></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
        height=25>&nbsp;</TD></TR>
	-->
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25><P align=right><FONT style="FONT-WEIGHT: 700"><B><script>Capture(setleftmenu.optsetting)</script></B><BR><B>(<script>Capture(setleftmenu.info)</script>)</B></FONT></P></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial><script>Capture(setinetsetup.hostname)</script>:&nbsp;</FONT></TD>
          <TD width=296 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial><INPUT maxLength=30 name="wan_hostname" size="20" value='<% nvram_get("wan_hostname"); %>' onBlur=valid_name(this,"Host%20Name")></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif 
        height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial><script>Capture(setinetsetup.doname)</script>:&nbsp;</FONT></TD>
          <TD width=296 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial><INPUT maxLength=63 name="wan_domain" size="20" value='<% nvram_get("wan_domain"); %>' onBlur=valid_name(this,"Domain%20name",SPACE_NO)></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial><script>Capture(setinetsetup.mtu)</script>:</FONT></TD>
          <TD width=296 height=25><B><select name="mtu_auto" onChange=SelMTU(this.form.mtu_auto.selectedIndex,this.form)>
			<option value="1" <% mtu_config("auto", "1"); %>><b><script>Capture(share.auto)</script></b></option>
			<option value="0" <% mtu_config("auto", "0"); %>><b><script>Capture(share.manual)</script>&nbsp;</b></option>
		</select></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial></FONT><script>Capture(setinetsetup.size)</script>:</B></TD>
          <TD width=296 height=25><INPUT class=num name="mtu_value" maxLength=4 onBlur=valid_mtu(this) size=5 value=<% mtu_config("value", 0); %> ></TD>
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
          <TD align=right width=156 bgColor=#000000 colSpan=3 
            height=25><B><FONT style="FONT-SIZE: 9pt" face=Arial 
            color=#ffffff><script>Capture(setleftmenu.netsetup)</script></FONT></B></TD>
          <TD width=8 bgColor=#000000 height=25>&nbsp;</TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=101 height=25>&nbsp;</TD>
          <TD width=296 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=25><B><script>Capture(setleftmenu.routerip)</script></B></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><script>Capture(setnetsetup.localip)</script>:</TD>
          <TD width=296 height=25><INPUT class=num maxLength=3 onBlur=valid_range(this,1,223,"IP") size=3 value='<% get_single_ip("lan_ipaddr","0"); %>' name="lan_ipaddr_0"> . 
            <INPUT class=num maxLength=3 onBlur=valid_range(this,0,255,"IP") size=3 value='<% get_single_ip("lan_ipaddr","1"); %>' name="lan_ipaddr_1"> . 
            <INPUT class=num maxLength=3 onBlur=valid_range(this,0,255,"IP") size=3 value='<% get_single_ip("lan_ipaddr","2"); %>' name="lan_ipaddr_2"> . 
            <INPUT class=num maxLength=3 onBlur=valid_range(this,1,254,"IP") size=3 value='<% get_single_ip("lan_ipaddr","3"); %>' name="lan_ipaddr_3"></TD>  
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt" face=Arial><script>Capture(setnetsetup.subnet)</script>:</FONT></TD>
          <TD width=296 height=25><SELECT class=num size=1 name="lan_netmask"><OPTION 
              value=255.255.255.0 <% nvram_match("lan_netmask", "255.255.255.0", "selected"); %>>255.255.255.0</OPTION><OPTION 
              value=255.255.255.128 <% nvram_match("lan_netmask", "255.255.255.128", "selected"); %>>255.255.255.128</OPTION><OPTION 
              value=255.255.255.192 <% nvram_match("lan_netmask", "255.255.255.192", "selected"); %>>255.255.255.192</OPTION><OPTION 
              value=255.255.255.224 <% nvram_match("lan_netmask", "255.255.255.224", "selected"); %>>255.255.255.224</OPTION><OPTION 
              value=255.255.255.240 <% nvram_match("lan_netmask", "255.255.255.240", "selected"); %>>255.255.255.240</OPTION><OPTION 
              value=255.255.255.248 <% nvram_match("lan_netmask", "255.255.255.248", "selected"); %>>255.255.255.248</OPTION><OPTION 
              value=255.255.255.252 <% nvram_match("lan_netmask", "255.255.255.252", "selected"); %>>255.255.255.252</OPTION></SELECT></TD>
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
                <TD width=410 colSpan=3 height=1><HR color=#b5b5e6 SIZE=1></TD>
                <TD width=15 height=1>&nbsp;</TD></TR></TBODY></TABLE></TD>
          <TD width=15 background=image/UI_05.gif height=1>&nbsp;</TD></TR>

<!-- for cloud -->
<% support_invmatch("CLOUD_SUPPORT", "1", "<!--"); %>
        <TR>
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=25><B><script>Capture(setleftmenu.cloudsetting)</script></B></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><script>Capture(setleftmenu.cloud)</script><script>Capture(share.enabled)</script></TD>
          <TD width=296 height=25><input type="radio" name="cloud_enable" value="1" <% nvram_selmatch("cloud_enable", "1", "checked"); %> onClick="SelCloud(1,this.form)"><B><span ><script>Capture(share.enabled)</script></span></B>
          <input type="radio" name="cloud_enable" value="0" <% nvram_selmatch("cloud_enable", "0", "checked"); %> onClick="SelCloud(0,this.form)"><B><span ><script>Capture(share.disabled)</script></span></B>
          <INPUT type=button name=cloud_config <% nvram_selmatch("cloud_enable", "0", "disabled=true"); %> value="Configuration" onclick=ViewCloudwin(this.form)></TD>  
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
                <TD width=410 colSpan=3 height=1><HR color=#b5b5e6 SIZE=1></TD>
                <TD width=15 height=1>&nbsp;</TD></TR></TBODY></TABLE></TD>
          <TD width=15 background=image/UI_05.gif height=1>&nbsp;</TD></TR>
<% support_invmatch("CLOUD_SUPPORT", "1", "-->"); %>
<!-- end for cloud -->

        <TR>
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=25><B><script>Capture(setleftmenu.netaddress)</script></B><BR><B><script>Capture(setleftmenu.serversetting)</script> (DHCP)</B></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><script>Capture(setnetsetup.ldhcpserver)</script>:</TD>
          <TD width=296 height=25><input type="radio" name="lan_proto" value="dhcp" <% nvram_selmatch("lan_proto", "dhcp", "checked"); %> onClick="SelDHCP('dhcp',this.form)"><B><span ><script>Capture(share.enabled)</script></span></B>
          <input type="radio" name="lan_proto" value="static" <% nvram_selmatch("lan_proto", "static", "checked"); %> onClick="SelDHCP('static',this.form)"><B><span ><script>Capture(share.disabled)</script></span></B>
          <input type="radio" name="lan_proto" value="dhcprelay" <% nvram_selmatch("lan_proto", "dhcprelay", "checked"); %> onClick="SelDHCP('dhcprelay',this.form)"><B><span ><script>Capture(setnetsetup.dhcprelay)</script></span></B></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
<input type=hidden name=dhcp_check>
 	<TR>
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><script>Capture(setnetsetup.dhcprelayserver)</script>:</TD>
          <TD width=296 height=25><INPUT class=num maxLength=3 onBlur=valid_range(this,1,223,"IP") size=3 value='<% get_single_ip("dhcpserver_ipaddr","0"); %>' name="dhcpserver_ipaddr_0"> . 
            <INPUT class=num maxLength=3 onBlur=valid_range(this,0,255,"IP") size=3 value='<% get_single_ip("dhcpserver_ipaddr","1"); %>' name="dhcpserver_ipaddr_1"> . 
            <INPUT class=num maxLength=3 onBlur=valid_range(this,0,255,"IP") size=3 value='<% get_single_ip("dhcpserver_ipaddr","2"); %>' name="dhcpserver_ipaddr_2"> . 
            <INPUT class=num maxLength=3 onBlur=valid_range(this,1,254,"IP") size=3 value='<% get_single_ip("dhcpserver_ipaddr","3"); %>' name="dhcpserver_ipaddr_3">&nbsp;
<% support_invmatch("MULTIPVC_SUPPORT", "1", "<!--"); %>
            <INPUT type=button name=dhcp_advance value="Advanced" onclick=ViewDHCPAdvance(this.form)></TD> 
<% support_invmatch("MULTIPVC_SUPPORT", "1", "-->"); %>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

<% support_invmatch("AUTODETECT_DHCP_SUPPORT", "1", "<!--"); %>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><script>Capture(setnetsetup.autodhcp)</script>:</TD>
          <TD width=296 height=25>
          		<INPUT type=radio value=1 name=lan_detect_dhcp <% nvram_match("lan_detect_dhcp","1","checked"); %> onclick="SelLanDetectDhcp(1,'<% nvram_get("lan_proto"); %>',this.form)" checked><b><script>Capture(share.enabled)</script></b>&nbsp;&nbsp;&nbsp;
                <INPUT type=radio value=0 name=lan_detect_dhcp <% nvram_match("lan_detect_dhcp","0","checked"); %> onclick="SelLanDetectDhcp(0,'<% nvram_get("lan_proto"); %>',this.form)"><b><script>Capture(share.disabled)</script></b>
		</TD>          
          <TD width=13 height=25></TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
<% support_invmatch("AUTODETECT_DHCP_SUPPORT", "1", "-->"); %>

        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt" face=Arial><script>Capture(setnetsetup.startip)</script>:</FONT></TD>
          <TD width=296 height=25>&nbsp;<b><% prefix_ip_get("lan_ipaddr",1); %></b><B><INPUT maxLength=3 onBlur=valid_dhcpstart(this.form) size=3 value='<% nvram_get("dhcp_start"); %>' name="dhcp_start" class=num>&nbsp; </B>
		</TD>          
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt" face=Arial><script>Capture(setnetsetup.maxnum)</script>:</FONT></TD>
          <TD width=296 height=25>&nbsp;<INPUT maxLength=3 onBlur=valid_dhcpusers(this.form)  size=3 value='<% nvram_get("dhcp_num"); %>' name="dhcp_num" class=num></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial><script>Capture(setnetsetup.clientlease)</script>:</FONT></TD>
          <TD width=296 height=25><INPUT maxLength=4 onBlur=valid_range(this,0,9999,"DHCP%20Lease%20Time") size=4 value='<% nvram_get("dhcp_lease"); %>' name="dhcp_lease" class=num>&nbsp;<script>Capture(setnetsetup.minutes)</script> (0 <script>Capture(setnetsetup.moneday)</script>)</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial><script>Capture(setnetsetup.staticdns)</script> 1:&nbsp;</FONT></TD>
          <TD width=296 height=25>&nbsp;<input type=hidden name=wan_dns value=4><INPUT maxLength=3 onBlur=valid_range(this,0,223,"DNS") size=3 value='<% get_dns_ip("wan_dns","0","0"); %>' name="wan_dns0_0" <% check_proto_fordns(1); %> class=num > . 
            <INPUT maxLength=3 onBlur=valid_range(this,0,255,"DNS") size=3 value='<% get_dns_ip("wan_dns","0","1"); %>' name="wan_dns0_1" <% check_proto_fordns(1); %> class=num> . 
            <INPUT maxLength=3 onBlur=valid_range(this,0,255,"DNS") size=3 value='<% get_dns_ip("wan_dns","0","2"); %>' name="wan_dns0_2" <% check_proto_fordns(1); %> class=num> . 
            <INPUT maxLength=3 onBlur=valid_range(this,0,254,"DNS") size=3 value='<% get_dns_ip("wan_dns","0","3"); %>' name="wan_dns0_3" <% check_proto_fordns(1); %> class=num ></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial><script>Capture(setnetsetup.staticdns)</script> 2:&nbsp;</FONT></TD>
          <TD width=296 height=25>&nbsp;<INPUT maxLength=3 onBlur=valid_range(this,0,223,"DNS") size=3 value='<% get_dns_ip("wan_dns","1","0"); %>' name="wan_dns1_0" <% check_proto_fordns(1); %> class=num> . 
            <INPUT maxLength=3 onBlur=valid_range(this,0,255,"DNS") size=3 value='<% get_dns_ip("wan_dns","1","1"); %>' name="wan_dns1_1" <% check_proto_fordns(1); %> class=num > . 
            <INPUT maxLength=3 onBlur=valid_range(this,0,255,"DNS") size=3 value='<% get_dns_ip("wan_dns","1","2"); %>' name="wan_dns1_2" <% check_proto_fordns(1); %> class=num > . 
            <INPUT maxLength=3 onBlur=valid_range(this,0,254,"DNS") size=3 value='<% get_dns_ip("wan_dns","1","3"); %>' name="wan_dns1_3" <% check_proto_fordns(1); %> class=num ></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial><script>Capture(setnetsetup.staticdns)</script> 3:&nbsp;</FONT></TD>
          <TD width=296 height=25>&nbsp;<INPUT maxLength=3 onBlur=valid_range(this,0,223,"DNS") size=3 value='<% get_dns_ip("wan_dns","2","0"); %>' <% check_proto_fordns(1); %> name="wan_dns2_0" class=num > . 
            <INPUT maxLength=3 onBlur=valid_range(this,0,255,"DNS") size=3 value='<% get_dns_ip("wan_dns","2","1"); %>' name="wan_dns2_1" <% check_proto_fordns(1); %> class=num> . 
            <INPUT maxLength=3 onBlur=valid_range(this,0,255,"DNS") size=3 value='<% get_dns_ip("wan_dns","2","2"); %>' name="wan_dns2_2" <% check_proto_fordns(1); %> class=num> . 
            <INPUT maxLength=3 onBlur=valid_range(this,0,254,"DNS") size=3 value='<% get_dns_ip("wan_dns","2","3"); %>' name="wan_dns2_3" <% check_proto_fordns(1); %> class=num ></B></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 9pt" 
            face=Arial><script>Capture(setnetsetup.wins)</script>:&nbsp;</FONT></TD>
          <TD width=296 height=25>&nbsp;<input type=hidden name=wan_wins value=4><INPUT maxLength=3 onBlur=valid_range(this,0,223,"WINS") size=3 value='<% get_single_ip("wan_wins","0"); %>' name="wan_wins_0" class=num> . 
            <INPUT maxLength=3 onBlur=valid_range(this,0,255,"WINS") size=3 value='<% get_single_ip("wan_wins","1"); %>' name="wan_wins_1" class=num> . 
            <INPUT maxLength=3 onBlur=valid_range(this,0,255,"WINS") size=3 value='<% get_single_ip("wan_wins","2"); %>' name="wan_wins_2" class=num> . 
            <INPUT maxLength=3 onBlur=valid_range(this,0,254,"WINS") size=3 value='<% get_single_ip("wan_wins","3"); %>' name="wan_wins_3" class=num></TD>
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
        <!--TR>
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=25><B>Time Setting</B></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><input type=radio name=time_mode>Manually</TD>
          <TD width=296 height=25>&nbsp;</TD>  
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial>&nbsp;Date:</FONT></TD>
          <TD width=296 height=25>&nbsp;<font face="Arial" style="font-size: 8pt">
                	<INPUT class=num maxLength=4 size=4 name="year" value="<% show_time_setting("year","0"); %>" style="font-family:Courier; font-size:10pt">&nbsp;-&nbsp;
                	<INPUT class=num maxLength=3 size=3 name="mon" value="<% show_time_setting("mon","0"); %>" style="font-family:Courier; font-size:10pt">&nbsp;-&nbsp;
                	<INPUT class=num maxLength=3 size=3 name="mday" value="<% show_time_setting("mday","0"); %>" style="font-family:Courier; font-size:10pt">&nbsp;
                	(yyyy-mm-dd)</font></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial>&nbsp;Time:</FONT></TD>
          <TD width=296 height=25>&nbsp;<font face="Arial" style="font-size: 8pt">
                	<INPUT class=num maxLength=3 size=3 name="hour" value="<% show_time_setting("hour","0"); %>" style="font-family:Courier; font-size:10pt">:&nbsp;
                	<INPUT class=num maxLength=3 size=3 name="min" value="<% show_time_setting("min","0"); %>" style="font-family:Courier; font-size:10pt">:&nbsp;
                	<INPUT class=num maxLength=3 size=3 name="sec" value="<% show_time_setting("sec","0"); %>" style="font-family:Courier; font-size:10pt">&nbsp;(hh-mm-ss)	</font></TD>
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
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=25><B>Time Setting</B></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><input type=radio name=time_mode checked value="1">Automatically</TD>
          <TD width=296 height=25>&nbsp;</TD>  
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR-->
        <TR>
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=25><b><script>Capture(setleftmenu.timesetting)</script></b></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=101 height=25><script>Capture(setnetsetup.timezone)</script>:</TD>
          <TD width=296 height=25>&nbsp;</TD>  
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
        <TR>
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=397 height=25 colspan="2">&nbsp;<select name="time_zone" onChange=SelTime(this.form.time_zone.selectedIndex,this.form)>
<option value="-12 1 0"  <% nvram_match("time_zone", "-12 1 0",  "selected"); %>> <script>Capture(timezon.Kwajalein)</script></option>
<option value="-11 1 0"  <% nvram_match("time_zone", "-11 1 0",  "selected"); %>> <script>Capture(timezon.Midway)</script></option>
<option value="-10 1 0"  <% nvram_match("time_zone", "-10 1 0",  "selected"); %>> <script>Capture(timezon.Hawaii)</script></option>
<option value="-09 1 1"  <% nvram_match("time_zone", "-09 1 1",  "selected"); %>> <script>Capture(timezon.Alaska)</script></option>
<option value="-08 1 1"  <% nvram_match("time_zone", "-08 1 1",  "selected"); %>> <script>Capture(timezon.Pacific)</script></option>
<option value="-07 1 0"  <% nvram_match("time_zone", "-07 1 0",  "selected"); %>> <script>Capture(timezon.Arizona)</script></option>
<option value="-07 2 1"  <% nvram_match("time_zone", "-07 2 1",  "selected"); %>> <script>Capture(timezon.Mountain)</script></option>
<option value="-06 1 0"  <% nvram_match("time_zone", "-06 1 0",  "selected"); %>> <script>Capture(timezon.Mexico)</script></option>
<option value="-06 2 1"  <% nvram_match("time_zone", "-06 2 1",  "selected"); %>> <script>Capture(timezon.Central)</script></option>
<option value="-05 1 0"  <% nvram_match("time_zone", "-05 1 0",  "selected"); %>> <script>Capture(timezon.Indiana)</script></option>
<option value="-05 2 1"  <% nvram_match("time_zone", "-05 2 1",  "selected"); %>> <script>Capture(timezon.Eastern)</script></option>
<option value="-04 1 0"  <% nvram_match("time_zone", "-04 1 0",  "selected"); %>> <script>Capture(timezon.Bolivia)</script></option>
<option value="-04 2 1"  <% nvram_match("time_zone", "-04 2 1",  "selected"); %>> <script>Capture(timezon.Atlantic)</script></option>
<option value="-03.5 1 1"<% nvram_match("time_zone", "-03.5 1 1","selected"); %>> <script>Capture(timezon.Newfoundland)</script></option>
<option value="-03 1 0"  <% nvram_match("time_zone", "-03 1 0",  "selected"); %>> <script>Capture(timezon.Guyana)</script></option>
<option value="-03 2 1"  <% nvram_match("time_zone", "-03 2 1",  "selected"); %>> <script>Capture(timezon.Brazil)</script></option>
<option value="-02 1 0"  <% nvram_match("time_zone", "-02 1 0",  "selected"); %>> <script>Capture(timezon.Mid)</script></option>
<option value="-01 1 2"  <% nvram_match("time_zone", "-01 1 2",  "selected"); %>> <script>Capture(timezon.Azores)</script></option>
<option value="+00 1 0"  <% nvram_match("time_zone", "+00 1 0",  "selected"); %>> <script>Capture(timezon.Gambia)</script></option>
<option value="+00 2 2"  <% nvram_match("time_zone", "+00 2 2",  "selected"); %>> <script>Capture(timezon.England)</script></option>
<option value="+01 1 0"  <% nvram_match("time_zone", "+01 1 0",  "selected"); %>> <script>Capture(timezon.Tunisia)</script></option>
<option value="+01 2 2"  <% nvram_match("time_zone", "+01 2 2",  "selected"); %>> <script>Capture(timezon.France)</script></option>
<option value="+02 1 0"  <% nvram_match("time_zone", "+02 1 0",  "selected"); %>> <script>Capture(timezon.South)</script></option>
<option value="+02 2 2"  <% nvram_match("time_zone", "+02 2 2",  "selected"); %>> <script>Capture(timezon.Greece)</script></option>
<option value="+03 1 0"  <% nvram_match("time_zone", "+03 1 0",  "selected"); %>> <script>Capture(timezon.Iraq)</script></option>
<option value="+04 1 0"  <% nvram_match("time_zone", "+04 1 0",  "selected"); %>> <script>Capture(timezon.Armenia)</script></option>
<option value="+05 1 0"  <% nvram_match("time_zone", "+05 1 0",  "selected"); %>> <script>Capture(timezon.Pakistan)</script></option>
<option value="+05.5 1 0"<% nvram_match("time_zone", "+05.5 1 0","selected"); %>> <script>Capture(timezon.India)</script></option>
<option value="+06 1 0"  <% nvram_match("time_zone", "+06 1 0",  "selected"); %>> <script>Capture(timezon.Bangladesh)</script></option>
<option value="+07 1 0"  <% nvram_match("time_zone", "+07 1 0",  "selected"); %>> <script>Capture(timezon.Thailand)</script></option>
<option value="+08 1 0"  <% nvram_match("time_zone", "+08 1 0",  "selected"); %>> <script>Capture(timezon.China)</script></option>
<option value="+08 2 0"  <% nvram_match("time_zone", "+08 2 0",  "selected"); %>> <script>Capture(timezon.Singapore)</script></option>
<option value="+09 1 0"  <% nvram_match("time_zone", "+09 1 0",  "selected"); %>> <script>Capture(timezon.Japan)</script></option>
<option value="+10 1 0"  <% nvram_match("time_zone", "+10 1 0",  "selected"); %>> <script>Capture(timezon.Guam)</script></option>
<option value="+10 2 4"  <% nvram_match("time_zone", "+10 2 4",  "selected"); %>> <script>Capture(timezon.Australia)</script></option>
<option value="+11 1 0"  <% nvram_match("time_zone", "+11 1 0",  "selected"); %>> <script>Capture(timezon.Solomon)</script></option>
<option value="+12 1 0"  <% nvram_match("time_zone", "+12 1 0",  "selected"); %>> <script>Capture(timezon.Fiji)</script></option>
<option value="+12 2 4"  <% nvram_match("time_zone", "+12 2 4",  "selected"); %>> <script>Capture(timezon.New_Zealand)</script></option>

	</select></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
          
        <TR>  
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD> 
          <TD width=101 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial><script>Capture(setnetsetup.tinterval)</script>:</FONT></TD>
          <TD width=296 height=25>&nbsp;<INPUT maxLength=4 onBlur=valid_range(this,60,9999,"SNTP") size=8 value='<% nvram_get("timer_interval"); %>' name="timer_interval" class=num>&nbsp;<script>Capture(setnetsetup.seconds)</script></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
          
        <TR>
          <TD align=right width=156 bgColor=#e7e7e7 colSpan=3 height=25></TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD width=397 height=25 colspan="2"><INPUT type=checkbox value="1" name="_daylight_time" <% nvram_match("daylight_time","1","checked"); %>><font face="Arial" style="font-size: 8pt"><script>Capture(setnetsetup.lastinfo)</script></font></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
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
          <TD width=156 bgColor=#6666cc height=25><font color="#FFFFFF">
          <span ><a target="_blank" href="help/HSetup.asp"><script>Capture(share.more)</script>...</a></span></font></TD>
          <TD width=9 bgColor=#6666cc 
  height=25>&nbsp;</TD></TR></TBODY></TABLE></TD></TR>
  <!--<TR>
    <TD width=809 colSpan=2>
      <TABLE cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=156 bgColor=#e7e7e height=30>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif>&nbsp;</TD>
          <TD width=16>&nbsp;</TD>
          <TD width=12>&nbsp;</TD>
          <TD width=411>&nbsp;</TD>
          <TD width=15>
          <TD width=15 background=image/UI_05.gif>&nbsp;</TD>
          <TD width=176 bgColor=#6666cc rowSpan=2>
          <IMG 
            src="image/UI_Cisco.gif" border=0 width="176" height="64"></TD></TR>
        <TR>
          <TD width=156 bgColor=#000000>&nbsp;</TD>
          <TD width=8 bgColor=#000000>&nbsp;</TD>
          <TD width=16 bgColor=#6666cc>&nbsp;</TD>
          <TD width=12 bgColor=#6666cc>&nbsp;</TD>
          <TD width=411 bgColor=#6666cc>
            <TABLE height=19 cellSpacing=0 cellPadding=0 align=right 
              border=0><TBODY>

		<input type=hidden name=now_connection value=<% nvram_gozila_get("wan_connection"); %>>

              
                <TD align=middle width=101></FONT><INPUT type=button value="Save Settings" onClick=to_submit(this.form)></TD>
                <TD align=middle width=8 bgColor=#6666cc>&nbsp;</TD>
                <TD align=middle bgColor=#6666cc><INPUT type=button value="Cancel Changes" onclick=window.location.reload()></TD>
                </TR></TBODY></TABLE></TD>
          <TD width=15 bgColor=#6666cc height=33>&nbsp;</TD>
          <TD width=15 bgColor=#000000 
  height=33>&nbsp;</TD></TR></TBODY></TABLE></TD></TR>>-->
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
          <TD width=176 bgColor=#6666cc rowSpan=2><IMG src="image/UI_Cisco.gif" border=0 width="176" height="64"></TD></TR>
        <TR>
          <TD width=156 bgColor=#000000>&nbsp;</TD>
          <TD width=8 bgColor=#000000>&nbsp;</TD>
          <TD width=16 bgColor=#6666cc>&nbsp;</TD>
          <TD width=12 bgColor=#6666cc>&nbsp;</TD>
          <TD width=411 bgColor=#6666cc>
            <TABLE height=19 cellSpacing=0 cellPadding=0 align=right border=0 width="220">
            <TBODY>
              <TR>
                <TD align=middle bgColor=#42498c width="101"><FONT color=#ffffff><B><A href='javascript:to_submit(document.forms[0]);'><script>Capture(share.saves)</script></A></B></FONT>
                </TD>
                <TD align=middle width=8 bgColor=#6666cc>&nbsp;</TD>
               <!-- <TD align=middle bgColor=#434a8f width="103"><FONT color=#ffffff><B><A href='javascript:window.location.reload();'>Cancel Changes</A></B></FONT></TD> -->
               <TD align=middle bgColor=#434a8f width="111"><FONT color=#ffffff><B><A href=index.asp><script>Capture(share.cancels)</script></A></B></FONT></TD> 
                <TD align=middle width=4 bgColor=#6666cc>&nbsp;</TD>
                </TR></TBODY></TABLE></TD>
                <TD width=15 bgColor=#6666cc height=33>&nbsp;</TD>
                <TD width=15 bgColor=#000000 height=33>&nbsp;</TD>
              </TR>
            </TBODY>
            </TABLE>
          </TD>
        </TR>
  </TBODY></TABLE></FORM></DIV></BODY></HTML>

