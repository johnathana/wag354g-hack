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
<HTML><HEAD><TITLE>Management</TITLE>
<% no_cache(); %>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<link rel="stylesheet" type="text/css" href="style.css">
<style fprolloverstyle>
A:hover {color: #00FFFF}
.small A:hover {color: #00FFFF}
</style>
<script src="common.js"></script>
<SCRIPT language="Javascript" type="text/javascript" src="share.js"></SCRIPT>
<SCRIPT language="Javascript" type="text/javascript" src="capadmin.js"></SCRIPT>
<SCRIPT language=JavaScript>
document.title=(admtopmenu.manage);
<% wireless_support(5); %>share.productname = share.productname1;<% wireless_support(6); %>
var EN_DIS1 = '<% nvram_get("remote_management"); %>'	

var wan_proto = '<% nvram_get("wan_proto"); %>'

var blank_passwd_flag = 1

function SelPort(num,F)
{	
	if(num == 1){
		<% support_invmatch("HTTPS_SUPPORT", "1", "/*"); %>
		if(ChangeRemotePasswd(F) == true)
<% support_invmatch("HTTPS_SUPPORT", "1", "*/"); %>

<% support_elsematch("HTTPS_SUPPORT", "1", "/*", ""); %>
		if(ChangePasswd(F) == true)
<% support_elsematch("HTTPS_SUPPORT", "1", "*/", ""); %>

			port_enable_disable(F,num);	
	}
	else
		port_enable_disable(F,num);

//	F.submit_button.value = "Management";
//        F.submit_type.value = "savebuf";
//        F.change_action.value = "gozila_cgi";
//	F.submit();

}

function port_enable_disable(F,I)

{

	EN_DIS2 = I;

	if ( I == "0" ){
		choose_disable(F.http_wanport);
<% support_invmatch("HTTPS_SUPPORT", "1", "/*"); %>
		choose_disable(F._remote_mgt_https);
		choose_disable(F.http_remote_username);
		choose_disable(F.http_remote_passwd);
		choose_disable(F.http_remote_passwdConfirm);
<% support_invmatch("HTTPS_SUPPORT", "1", "*/"); %>
		
		//For Restricted IP Range 2004-11-17
		if(F.ip_restrict.value == '0')
		{
			choose_disable(F.ip_restrict);
		}
		else if(F.ip_restrict.value == '1')
		{
			choose_disable(F.ip_restrict);
			choose_disable(F.range_ip_01);
			choose_disable(F.range_ip_02);
			choose_disable(F.range_ip_03);
			choose_disable(F.range_ip_04);
		}
		else if(F.ip_restrict.value == '2')
		{
			choose_disable(F.ip_restrict);
			choose_disable(F.range_ip_01);
			choose_disable(F.range_ip_02);
			choose_disable(F.range_ip_03);
			choose_disable(F.range_ip_04);
			choose_disable(F.range_ip_11);
			choose_disable(F.range_ip_12);
			choose_disable(F.range_ip_13);
			choose_disable(F.range_ip_14);

		}
	}
	else{
		choose_enable(F.http_wanport);
<% support_invmatch("HTTPS_SUPPORT", "1", "/*"); %>
		choose_enable(F._remote_mgt_https);
		choose_enable(F.http_remote_username);
		choose_enable(F.http_remote_passwd);
		choose_enable(F.http_remote_passwdConfirm);
<% support_invmatch("HTTPS_SUPPORT", "1", "*/"); %>

		//For Restricted IP Range 2004-11-17
		if(F.ip_restrict.value == '0')
		{
			choose_enable(F.ip_restrict);
		}
		else if(F.ip_restrict.value == '1')
		{
			choose_enable(F.ip_restrict);
			choose_enable(F.range_ip_01);
			choose_enable(F.range_ip_02);
			choose_enable(F.range_ip_03);
			choose_enable(F.range_ip_04);
		}
		else if(F.ip_restrict.value == '2')
		{
			choose_enable(F.ip_restrict);
			choose_enable(F.range_ip_01);
			choose_enable(F.range_ip_02);
			choose_enable(F.range_ip_03);
			choose_enable(F.range_ip_04);
			choose_enable(F.range_ip_11);
			choose_enable(F.range_ip_12);
			choose_enable(F.range_ip_13);
			choose_enable(F.range_ip_14);

		}
	}

}
function ChangePasswd(F)
{
	if(F.L_PasswdModify.value== 1 &&  (F.http_passwd.value == "d6nw5v1x2pc7st9m" || F.http_passwd.value == "<% nvram_get("http_def_passwd"); %>"))
	{
		//if(confirm('The Gateway is currently set to its default password. As a security measure, you must change the password before the Remote Management feature can be enabled. Click the OK button to change your password.  Click the Cancel button to leave the Remote Management feature disabled.'))
		if(confirm(errmsg.err104))
		{
			F.http_passwd.focus();
			F.http_passwd.select();
			F.remote_management[0].checked = true;
			port_enable_disable(F,1);

			return false;
		}
		else
		{		
			F.remote_management[1].checked = true;
			port_enable_disable(F,0);

			return false;
		}
	}
	else
		return true;
}

function ValidPasswd(F)
{
	if(F.http_passwd.value != F.http_passwd.defaultValue)
	{
		F.L_PasswdModify.value = 0;
	}	

	if(F.L_PasswdModify.value == 1 && F.remote_management[0].checked == true)
	{
		//alert("Please change gateway password first!");
		alert(errmsg.err148);
		port_enable_disable(F,1);
		F.http_passwd.focus();
		F.http_passwd.select();
		return false;
	}
		return true;
}

<% support_invmatch("HTTPS_SUPPORT", "1", "/*"); %>

function ChangeRemotePasswd(F)
{
	//if(F.PasswdModify.value==1 && (F.http_remote_passwd.value == "d6nw5v1x2pc7st9m" || F.http_remote_passwd.value == "<% nvram_get("http_def_passwd"); %>") && F._remote_mgt_https.checked == true)
	if(F.PasswdModify.value==1 && (F.http_remote_passwd.value == F.http_remote_passwd.defaultValue))
	{
		//if(confirm('The Remote Management is currently set to its default password. As a security measure, you must change the remote password before the Remote Management feature can be enabled. Click the OK button to change it.  Click the Cancel button to leave the Remote Management feature disabled.'))
		if(confirm(errmsg.err147))
		{
			//window.location.replace('Management.asp');
			port_enable_disable(F,1);
			F.http_remote_passwd.focus();
			F.http_remote_passwd.select();
			F.remote_management[0].checked = true;

			return false;
		}
		else
		{
			F.remote_management[1].checked = true;
			port_enable_disable(F,0);

			return false;
		}
	}
	else 
		return true;
}

function ValidRemotePasswd(F)
{
	//if(F.PasswdModify.value==1 && F.http_remote_passwd.value == "d6nw5v1x2pc7st9m" && F._remote_mgt_https.checked == true)
	if(F.http_remote_passwd.value != F.http_remote_passwd.defaultValue)
	{
		F.PasswdModify.value = 0;
	}	

	if(F.PasswdModify.value == 1 && F.remote_management[0].checked == true)
	{
		//alert("Please change remote password first!");
		alert(errmsg.err149);
		port_enable_disable(F,1);
		F.http_remote_passwd.focus();
		F.http_remote_passwd.select();

		return false;
	}
		return true;
}

<% support_invmatch("HTTPS_SUPPORT", "1", "*/"); %>

function valid_password(F)
{
	if (F.http_passwd.value != F.http_passwdConfirm.value)
	{	
		//alert("Confirmed password did not match Entered Password.  Please re-enter password");
		alert(errmsg.err114);

		F.http_passwdConfirm.focus();

		F.http_passwdConfirm.select();

		return false;
	}

	return true;
}

function validsnmp(I,start,end,M)
{
        valid_comma(I,M);
        valid_length(I,start,end,M);
}
function validpassword(I,start,end,M)

{

        valid_comma(I,M);

        valid_length(I,start,end,M);

}
function valid_length(I,start,end,M)
{
	M1 = unescape(M);


	d = I.value.length;
	if ( !(d<=end && d>=start) )
	{
		//alert(M1 +' length is out of range ['+ start + ' - ' + end +']');
		alert(M1 + errmsg.err24 + start + ' - ' + end + ']');
		I.value = I.defaultValue;
	}
}

function valid_comma(I,M)
{
        var i, len;

        len = I.value.length;
        M1 = unescape(M);
        if((I.value.indexOf('\''))>=0)
        {
                alert(M1 +' can\'t include \' ');
                I.value = I.defaultValue;
                return false;
        }

}

// For IP Range Restricted 2004-11-17
function SelIPRange(num, F)
{

	F.ip_restrict.value = F.ip_restrict.options[num].value;
	F.submit_button.value = "Management";
        F.submit_type.value = "savebuf";
        F.change_action.value = "gozila_cgi";
	F.submit();
}

// for Restricted IP Range 2004-10-28
function valid_range_ip_start(F)
{	
	var lan_ip = new Array(4);
	var lan_mask = new Array(4);
	var wan_ip = new Array(4);
	var tmp;

	var flag = true;
	var lan_ipaddr = '<% nvram_get("lan_ipaddr");%>';
	var lan_netmask ='<% nvram_get("lan_netmask"); %>';

	for(i=0; i<4; i++) {
		lan_ip[i] = lan_ipaddr.split('.')[i]
		lan_mask[i] =  lan_netmask.split('.')[i];
	}
	
	{
		wan_ip[0] = F.range_ip_01.value;
		wan_ip[1] = F.range_ip_02.value;
		wan_ip[2] = F.range_ip_03.value;
		wan_ip[3] = F.range_ip_04.value;
	}

	for(i=0; i<4; i++) {
		if((lan_ip[i] & lan_mask[i]) != (wan_ip[i] & lan_mask[i])) {
			flag = false;
			break;
		}else
			continue;
	}

	return flag;
}
// for Restricted IP Range 2004-11-17
function valid_range_ip_end(F)
{	
	var lan_ip = new Array(4);
	var lan_mask = new Array(4);
	var wan_ip = new Array(4);
	var tmp;

	var flag = true;
	var lan_ipaddr = '<% nvram_get("lan_ipaddr");%>';
	var lan_netmask ='<% nvram_get("lan_netmask"); %>';

	for(i=0; i<4; i++) {
		lan_ip[i] = lan_ipaddr.split('.')[i]
		lan_mask[i] =  lan_netmask.split('.')[i];
	}
	
	{
		wan_ip[0] = F.range_ip_11.value;
		wan_ip[1] = F.range_ip_12.value;
		wan_ip[2] = F.range_ip_13.value;
		wan_ip[3] = F.range_ip_14.value;
	}

	for(i=0; i<4; i++) {
		if((lan_ip[i] & lan_mask[i]) != (wan_ip[i] & lan_mask[i])) {
			flag = false;
			break;
		}else
			continue;
	}

	return flag;
}

//added by zls 2004-1221
function Sel_http_username(num,F)
{
	F.submit_button.value = "Management";
	F.change_action.value = "gozila_cgi";
	F.http_username_page.value=F.http_username_page.options[num].value;
	F.submit();
}


function to_submit(F)

{
	var flag;
	flag = 0;
	
	if( F.http_passwd.value != F.http_passwdConfirm.value )

		{

		//alert('Password confirmation is not matched.');
		alert(errmsg.err115);
		flag = 1;
		F.http_passwdConfirm.focus();

		F.http_passwdConfirm.select();

	//	return false;

		}

	else if((blank_passwd_flag == 1) && (F.http_passwd.value == ""))
	{
		//alert('The Gateway Password is blank, Please make sure you do want to do like this!');
		alert(errmsg.err116);
		flag  = 1;
		blank_passwd_flag = 2;
	}
	else
		F.action.value='Apply';

	//valid_password(F);

<% support_invmatch("HTTPS_SUPPORT", "1", "/*"); %>
	if(F._remote_mgt_https){
		if(F._remote_mgt_https.checked == true) F.remote_mgt_https.value = 1;
		else 	 F.remote_mgt_https.value = 0;
	}
<% support_invmatch("HTTPS_SUPPORT", "1", "*/"); %>

	if(F.remote_management[0].checked == true){
	
<% support_invmatch("HTTPS_SUPPORT", "1", "/*"); %>
		if(!ValidRemotePasswd(F)) flag = 1;
<% support_invmatch("HTTPS_SUPPORT", "1", "*/"); %>

<% support_elsematch("HTTPS_SUPPORT", "1", "/*", ""); %>
		if(!ValidPasswd(F)) flag = 1;
<% support_elsematch("HTTPS_SUPPORT", "1", "*/", ""); %>

	}

	if(F.remote_management[0].checked == true){

<% support_invmatch("HTTPS_SUPPORT", "1", "/*"); %>

                	if(F.http_remote_passwdConfirm.value != F.http_remote_passwd.value)
			{
				//alert('Remote Access Password confirmation is not matched');
				alert(errmsg.err117);
				flag = 1;
				F.http_remote_passwdConfirm.focus();
				F.http_remote_passwdConfirm.select();
			}else if(F.http_remote_passwd.value == "")
			{
				//alert('Remote Access Password must be non-blank!');
				alert(errmsg.err118);
				flag = 1;
				F.http_remote_passwd.focus();
				F.http_remote_passwd.select();
			} 		
<% support_invmatch("HTTPS_SUPPORT", "1", "*/"); %>

		// For Restricted IP Range 2004-11-17
		if(F.ip_restrict.value == '1')
		{
			if(!(F.range_ip_01.value <= 223 && F.range_ip_01.value >= 1)){
				//alert('IP value is out of range [1-233]');
				alert(errmsg.err120 + '[1-233]');
				flag = 1;
				F.range_ip_01.focus();
				F.range_ip_01.select();
			}else if(!(F.range_ip_02.value <= 255 && F.range_ip_02.value >= 0)){
				//alert('IP value is out of range [0-255]');
				alert(errmsg.err120 + '[0-255]');
				flag = 1;
				F.range_ip_02.focus();
				F.range_ip_02.select();
			}else if(!(F.range_ip_03.value <= 255 && F.range_ip_03.value >= 0)){
				//alert('IP value is out of range [0-255]');
				alert(errmsg.err120 + '[0-255]');
				flag = 1;
				F.range_ip_03.focus();
				F.range_ip_03.select();

			}else if(!(F.range_ip_04.value <= 254 && F.range_ip_04.value >= 1)){
				//alert('IP value is out of range [1-254]');
				alert(errmsg.err120 + '[1-254]');
				flag = 1;
				F.range_ip_04.focus();
				F.range_ip_04.select();
			}

			if(valid_range_ip_start(F)) {
				//alert('WAN IP and LAN IP is at the same subnet mask.');
				alert(errmsg.err125);
				flag = 1;
				F.range_ip_01.focus();
				F.range_ip_01.select();
				//return false;
			}
		}
		else if(F.ip_restrict.value == '2')
		{
			if(!(F.range_ip_01.value <= 223 && F.range_ip_01.value >= 1)){
				//alert('IP value is out of range [1-233]');
				alert(errmsg.err120 + '[1-233]');
				flag = 1;
				F.range_ip_01.focus();
				F.range_ip_01.select();
			}else if(!(F.range_ip_02.value <= 255 && F.range_ip_02.value >= 0)){
				//alert('IP value is out of range [0-255]');
				alert(errmsg.err120 + '[0-255]');
				flag = 1;
				F.range_ip_02.focus();
				F.range_ip_02.select();
			}else if(!(F.range_ip_03.value <= 255 && F.range_ip_03.value >= 0)){
				//alert('IP value is out of range [0-255]');
				alert(errmsg.err120 + '[0-255]');
				flag = 1;
				F.range_ip_03.focus();
				F.range_ip_03.select();

			}else if(!(F.range_ip_04.value <= 254 && F.range_ip_04.value >= 1)){
				//alert('IP value is out of range [1-254]');
				alert(errmsg.err120 + '[1-254]');
				flag = 1;
				F.range_ip_04.focus();
				F.range_ip_04.select();
			}else if(!(F.range_ip_11.value <= 223 && F.range_ip_11.value >= 1)){
				//alert('IP value is out of range [1-233]');
				alert(errmsg.err120 + '[1-233]');

				flag = 1;
				F.range_ip_11.focus();
				F.range_ip_11.select();
			}else if(!(F.range_ip_12.value <= 255 && F.range_ip_12.value >= 0)){
				//alert('IP value is out of range [0-255]');
				alert(errmsg.err120 + '[0-255]');
				flag = 1;
				F.range_ip_12.focus();
				F.range_ip_12.select();
			}else if(!(F.range_ip_13.value <= 255 && F.range_ip_13.value >= 0)){
				//alert('IP value is out of range [0-255]');
				alert(errmsg.err120 + '[0-255]');
				flag = 1;
				F.range_ip_13.focus();
				F.range_ip_13.select();

			}else if(!(F.range_ip_14.value <= 254 && F.range_ip_14.value >= 1)){
				//alert('IP value is out of range [1-254]');
				alert(errmsg.err120 + '[1-254]');
				flag = 1;
				F.range_ip_14.focus();
				F.range_ip_14.select();
			}

			if(valid_range_ip_start(F)) {
				//alert('WAN IP and LAN IP is at the same subnet mask.');
				alert(errmsg.err125);
				flag = 1;
				F.range_ip_01.focus();
				F.range_ip_01.select();
				//return false;
			}
			if(valid_range_ip_end(F)) {
				//alert('WAN IP and LAN IP is at the same subnet mask.');
				alert(errmsg.err125);
				flag = 1;
				F.range_ip_11.focus();
				F.range_ip_11.select();
				//return false;
			}

		}
	}

/////////////////////////////////////////////////////////
// below is snmp
	if(F.snmp_enable[0].checked == true){
   
		if(F.snmp_ip_addr.value == '1')
		{
			if(!(F.snmp_ipaddr0.value <= 223 && F.snmp_ipaddr0.value >= 1)){
				//alert('IP value is out of range [1-233]');
				alert(errmsg.err120 + '[1-233]');
				flag = 1;
				F.snmp_ipaddr0.focus();
				F.snmp_ipaddr0.select();
			}else if(!(F.snmp_ipaddr1.value <= 255 && F.snmp_ipaddr1.value >= 0)){
				//alert('IP value is out of range [0-255]');
				alert(errmsg.err120 + '[0-255]');
				flag = 1;
				F.snmp_ipaddr1.focus();
				F.snmp_ipaddr1.select();
			}else if(!(F.snmp_ipaddr2.value <= 255 && F.snmp_ipaddr2.value >= 0)){
				//alert('IP value is out of range [0-255]');
				alert(errmsg.err120 + '[0-255]');
				flag = 1;
				F.snmp_ipaddr2.focus();
				F.snmp_ipaddr2.select();

			}else if(!(F.snmp_ipaddr3.value <= 254 && F.snmp_ipaddr3.value >= 1)){
				//alert('IP value is out of range [1-254]');
				alert(errmsg.err120 + '[1-254]');
				flag = 1;
				F.snmp_ipaddr3.focus();
				F.snmp_ipaddr3.select();
			}

		}else if(F.snmp_ip_addr.value == '2')
		{
			if(!(F.snmp_ipStartAddr0.value <= 223 && F.snmp_ipStartAddr0.value >= 1)){
				//alert('IP value is out of range [1-233]');
				alert(errmsg.err120 + '[1-233]');
				flag = 1;
				F.snmp_ipStartAddr0.focus();
				F.snmp_ipStartAddr0.select();
			}else if(!(F.snmp_ipStartAddr1.value <= 255 && F.snmp_ipStartAddr1.value >= 0)){
				//alert('IP value is out of range [0-255]');
				alert(errmsg.err120 + '[0-255]');
				flag = 1;
				F.snmp_ipStartAddr1.focus();
				F.snmp_ipStartAddr1.select();
			}else if(!(F.snmp_ipStartAddr2.value <= 255 && F.snmp_ipStartAddr2.value >= 0)){
				//alert('IP value is out of range [0-255]');
				alert(errmsg.err120 + '[0-255]');
				flag = 1;
				F.snmp_ipStartAddr2.focus();
				F.snmp_ipStartAddr2.select();

			}else if(!(F.snmp_ipStartAddr3.value <= 254 && F.snmp_ipStartAddr3.value >= 1)){
				//alert('IP value is out of range [1-254]');
				alert(errmsg.err120 + '[1-254]');
				flag = 1;
				F.snmp_ipStartAddr3.focus();
				F.snmp_ipStartAddr3.select();
			}else if(!(F.snmp_ipEndAddr0.value <= 223 && F.snmp_ipEndAddr0.value >= 1)){
				//alert('IP value is out of range [1-233]');
				alert(errmsg.err120 + '[1-233]');
				flag = 1;
				F.snmp_ipEndAddr0.focus();
				F.snmp_ipEndAddr0.select();
			}else if(!(F.snmp_ipEndAddr1.value <= 255 && F.snmp_ipEndAddr1.value >= 0)){
				//alert('IP value is out of range [0-255]');
				alert(errmsg.err120 + '[0-255]');
				flag = 1;
				F.snmp_ipEndAddr1.focus();
				F.snmp_ipEndAddr1.select();
			}else if(!(F.snmp_ipEndAddr2.value <= 255 && F.snmp_ipEndAddr2.value >= 0)){
				//alert('IP value is out of range [0-255]');
				alert(errmsg.err120 + '[0-255]');
				flag = 1;
				F.snmp_ipEndAddr2.focus();
				F.snmp_ipEndAddr2.select();

			}else if(!(F.snmp_ipEndAddr3.value <= 254 && F.snmp_ipEndAddr3.value >= 1)){
				//alert('IP value is out of range [1-254]');
				alert(errmsg.err120 + '[1-254]');
				flag = 1;
				F.snmp_ipEndAddr3.focus();
				F.snmp_ipEndAddr3.select();
			}
		}
	}

// above is snmp

////////////////////////////////////////////////////////////
if(flag == 0)
{
	F.submit_button.value = "Management";
	F.submit();
}
//	return true;

}

function init() 

{    
	port_enable_disable(document.password, '<% nvram_gozila_get("remote_management"); %>');
	//add by CSH
	//alert("begin");
	SNMP_grey(document.forms[0]);
	//alert("end");

}        

function warn_user()
{
//	alert('This function may allow applications to configure the box and allow unexpected traffic to accesskey local devices.')	
	alert(errmsg.err119);
}
	
//add by lzh;
function SelPvcConnection(num,F)
{
	F.submit_type.value = "upnp_select_connect";
	F.submit_button.value = "Management";
	F.change_action.value = "gozila_cgi";
	F.upnp_pvc_connection.value=F.upnp_pvc_connection.options[num].value;
	F.submit();
}

//<!--add by CSH-->
function SelRTYPE(num,F)
{
	F.submit_button.value = "Management";
	F.change_action.value = "gozila_cgi";
	F.snmp_ip_addr.value=F.snmp_ip_addr.options[num].value;
	F.submit();
}

//add by peny 2005.1.6
function SelV3TYPE(num,F)

{
	F.submit_button.value = "Management";
	F.change_action.value = "gozila_cgi";
	F.authprotocol.value=F.authprotocol.options[num].value;
        F.submit();
}

//<!--add by CSH-->
function SNMP_grey(F)
{
		if(F.snmp_enable[1].checked)
		{
				if(F.snmp_ip_addr.value==0)
				{
						F.snmp_ip_addr.disabled=true;
				}
				else if(F.snmp_ip_addr.value==1)
				{
						F.snmp_ip_addr.disabled=true;
						F.snmp_ipaddr0.disabled=true;
						F.snmp_ipaddr1.disabled=true;
						F.snmp_ipaddr2.disabled=true;
						F.snmp_ipaddr3.disabled=true;
				}
				else if(F.snmp_ip_addr.value==2)
				{
						F.snmp_ip_addr.disabled=true;
						F.snmp_ipStartAddr0.disabled=true;
						F.snmp_ipStartAddr1.disabled=true;
						F.snmp_ipStartAddr2.disabled=true;
						F.snmp_ipStartAddr3.disabled=true;
						F.snmp_ipEndAddr0.disabled=true;
						F.snmp_ipEndAddr1.disabled=true;
						F.snmp_ipEndAddr2.disabled=true;
						F.snmp_ipEndAddr3.disabled=true;
				}
		}
		else
		{
				if(F.snmp_ip_addr.value==0)
				{
						F.snmp_ip_addr.disabled=false;
				}
				else if(F.snmp_ip_addr.value==1)
				{
						F.snmp_ip_addr.disabled=false;
						F.snmp_ipaddr0.disabled=false;
						F.snmp_ipaddr1.disabled=false;
						F.snmp_ipaddr2.disabled=false;
						F.snmp_ipaddr3.disabled=false;
				}
				else if(F.snmp_ip_addr.value==2)
				{
						F.snmp_ip_addr.disabled=false;
						F.snmp_ipStartAddr0.disabled=false;
						F.snmp_ipStartAddr1.disabled=false;
						F.snmp_ipStartAddr2.disabled=false;
						F.snmp_ipStartAddr3.disabled=false;
						F.snmp_ipEndAddr0.disabled=false;
						F.snmp_ipEndAddr1.disabled=false;
						F.snmp_ipEndAddr2.disabled=false;
						F.snmp_ipEndAddr3.disabled=false;
				}
		}
}


//add for IGMP Proxy 2004-10-10
<% support_elsematch("IGMP_PROXY_SUPPORT", "1", "", "<!--"); %>
function SelIgmpProxyChannel(num,F)
{
	F.submit_button.value = "Management";
	F.change_action.value = "gozila_cgi";
	F.igmp_proxy_channel.value=F.igmp_proxy_channel.options[num].value;
	F.submit();
}
<% support_elsematch("IGMP_PROXY_SUPPORT", "1", "", "-->"); %>
</SCRIPT>



</HEAD>

<BODY vLink=#b5b5e6 aLink=#ffffff link=#b5b5e6 onload=init()>

<DIV align=center>

<FORM name=password method=<% get_http_method(); %> action=apply.cgi>

<input type=hidden name=submit_button>

<input type=hidden name=change_action>

<input type=hidden name=submit_type>

<input type=hidden name=action>

<INPUT type=hidden name=PasswdModify value='<% passwd_match("http_remote_passwd"); %>'> 

<INPUT type=hidden name=L_PasswdModify value='<% l_passwd_match("http_passwd"); %>'> 
<!--INPUT type=hidden name=PasswdModify value='<% nvram_else_match("http_passwd", "admin", "1", "0"); %>'--> 

<!-- add by peny 2005.1.6-->
<input type=hidden name=trap_ipaddr value=4>

<input type=hidden name=snmpv3_auth>
<!--add by CSH-->
<input type=hidden name=snmp_ip_limit>

<input type=hidden name=remote_mgt_https>
<% support_elsematch("IGMP_PROXY_SUPPORT", "1", "", "<!--"); %>
<input type=hidden name=igmp_proxy_config>
<% support_elsematch("IGMP_PROXY_SUPPORT", "1", "", "-->"); %>
<TABLE cellSpacing=0 cellPadding=0 width=809 border=0>

  <TBODY>

  <TR>

    <TD width=95><IMG src="image/UI_Linksys.gif" border=0 width="165" height="57"></TD>

    <TD vAlign=bottom align=right width=714 bgColor=#6666cc><FONT style="FONT-SIZE: 7pt" color=#ffffff><FONT face=Arial><script>Capture(share.firmwarever)</script>:&nbsp;<% get_firmware_version(); %>&nbsp;&nbsp;&nbsp;</FONT></FONT></TD></TR>

  <TR>

    <TD width=808 bgColor=#6666cc colSpan=2><IMG height=11 src="image/UI_10.gif" width=809 border=0></TD></TR></TBODY></TABLE>

<TABLE height=77 cellSpacing=0 cellPadding=0 width=809 bgColor=black border=0>

  <TBODY>

  <TR>

    <TD style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" borderColor=#000000 align=middle width=163 height=49><H3 style="MARGIN-TOP: 1px; MARGIN-BOTTOM: 1px"><FONT style="FONT-SIZE: 15pt" face=Arial color=#ffffff><script>Capture(bmenu.admin)</script></FONT></H3></TD>

    <TD style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" vAlign=center borderColor=#000000 width=646 bgColor=#000000 height=49><TABLE style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; BORDER-COLLAPSE: collapse; FONT-VARIANT: normal" height=33 cellSpacing=0 cellPadding=0 bgColor=#6666cc border=0>

    <TBODY>

        <TR>

          <TD style="FONT-WEIGHT: bolder; FONT-SIZE: 10pt" align=right bgColor=#6666cc height=33><FONT color=#ffffff><% wireless_support(1); %><script>Capture(share.wireless_g)</script><% wireless_support(2); %><script>Capture(share.productname)</script>&nbsp;&nbsp;</FONT></TD>

          <TD borderColor=#000000 borderColorLight=#000000 align=middle width=109 bgColor=#000000 borderColorDark=#000000 height=12 rowSpan=2><FONT color=#ffffff><SPAN style="FONT-SIZE: 8pt"><B><% nvram_get("router_name"); %></B></SPAN></FONT></TD></TR>

        <TR>

          <TD style="FONT-WEIGHT: normal; FONT-SIZE: 1pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" width=537 bgColor=#000000 height=1>&nbsp;</TD></TR>

        <TR>

          <TD width=646 bgColor=#000000 colSpan=2 height=32>

            <TABLE id=AutoNumber1 style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; BORDER-COLLAPSE: collapse; FONT-VARIANT: normal" height=6 cellSpacing=0 cellPadding=0 width=637 border=0>

            <TBODY>

              <TR style="BORDER-RIGHT: medium none; BORDER-TOP: medium none; FONT-WEIGHT: normal; FONT-SIZE: 1pt; BORDER-LEFT: medium none; COLOR: black; BORDER-BOTTOM: medium none; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" align=middle bgColor=#6666cc>
                <TD width=83  height=1><IMG height=10 src="image/UI_06.gif" width=83  border=0></TD>
                <TD width=83  height=1><IMG height=10 src="image/UI_06.gif" width=83  border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="<% wireless_support(3); %>" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="<% wireless_support(4); %>" width=103 border=0></TD>
                <TD width=68  height=1><IMG height=10 src="image/UI_06.gif" width=68  border=0></TD></TR>

              <TR>

                <TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><A style="TEXT-DECORATION: none" href="index.asp"><script>Capture(bmenu.setup)</script></A></FONT></TD>

                <% wireless_support(1); %><TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Wireless_Basic.asp"><script>Capture(bmenu.wireless)</script></a></FONT></TD><% wireless_support(2); %>

                <TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Firewall.asp"><script>Capture(bmenu.security)</script></a></FONT></TD>

                <TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><A style="TEXT-DECORATION: none" href="<% support_elsematch("PARENTAL_CONTROL_SUPPORT", "1", "Parental_Control.asp", "Filters.asp"); %>"><script>Capture(bmenu.accrestriction)</script></A></FONT></TD>

                <TD align=middle bgColor=#000000 height=20><P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Forward_UPnP.asp"><script>Capture(bmenu.application)</script> <BR>&amp; <script>Capture(bmenu.gaming)</script></a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>

                <TD align=middle bgColor=#6666cc height=20><P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" color=#ffffff>&nbsp;&nbsp;<script>Capture(bmenu.admin)</script>&nbsp;&nbsp;</FONT></P></TD>

                <TD align=middle bgColor=#000000 height=20><P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Status_Router.asp"><script>Capture(bmenu.statu)</script></a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD></TR>             

              <TR>

                <TD width=643 bgColor=#6666cc colSpan=7 height=21>

                  <TABLE borderColor=black height=21 cellSpacing=0 cellPadding=0>

                    <TBODY>

                    <TR align=center>
                      <TD width=80><FONT style="COLOR: white"><script>Capture(admtopmenu.manage)</script></FONT></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=80><a href="Log.asp"><script>Capture(admtopmenu.report)</script></a></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=80><A href="Diagnostics.asp"><script>Capture(admtopmenu.diag)</script></A></TD>
<% support_elsematch("BACKUP_RESTORE_SUPPORT","1","","<!--"); %>
		      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD  class=small width=110><A href="Backup_Restore.asp"><script>Capture(admtopmenu.bakrest)</script></A></TD>
<% support_elsematch("BACKUP_RESTORE_SUPPORT","1","","-->"); %>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=110><A href="Factory_Defaults.asp"><script>Capture(admtopmenu.facdef)</script></A></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=120><A href="Upgrade.asp"><script>Capture(admtopmenu.upgrade)</script></A></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD class=small width=63><A href="Reboot.asp"><script>Capture(admtopmenu.reboot)</script></A></TD>
</TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>

<TABLE height=5 cellSpacing=0 cellPadding=0 width=806 bgColor=black border=0>

  <TBODY>

  <TR bgColor=black>

    <TD style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" borderColor=#e7e7e7 width=163 bgColor=#e7e7e7 height=1><IMG height=15 src="image/UI_03.gif" width=164 border=0></TD>

    <TD style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" width=646 height=1><IMG height=15 src="image/UI_02.gif" width=645 border=0></TD></TR></TBODY></TABLE>

<TABLE id=AutoNumber9 style="BORDER-COLLAPSE: collapse" borderColor=#111111 height=23 cellSpacing=0 cellPadding=0 width=809 border=0>

  <TBODY>

  <TR>

    <TD width=633>

      <TABLE cellSpacing=0 cellPadding=0 border=0 width="633">

        <TBODY>

        <TR>

          <TD width=156 bgColor=#000000 colSpan=3 height=25><P align=right><B><FONT style="FONT-SIZE: 9pt" color=#ffffff>
			<script>Capture(admleftmenu.gwaccess)</script></FONT></B></P></TD>

          <TD width=5 bgColor=#000000 height=25>&nbsp;</TD>

          <TD width=454 colSpan=6 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

	  <% support_invmatch("MULTIUSER_SUPPORT", "1", "<!--"); %>
        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25><p align="right"><FONT style="FONT-WEIGHT: 700"><span ><script>Capture(admleftmenu.localgwaccess)</script></span></FONT></TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42"></TD>
	  <TD width=127 height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(admgwaccess.gwuserlist)</script>:&nbsp;&nbsp; </FONT></TD>
           
          <TD width=272 height=25>&nbsp;<select size="1" name="http_username_page" onchange=Sel_http_username(this.form.http_username_page.selectedIndex,this.form)><%show_http_username_table("select");%> </select></TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
	  <% support_invmatch("MULTIUSER_SUPPORT", "1", "-->"); %>

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25><P align=right>&nbsp;</P></TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42">&nbsp;</TD>

          <TD width=127 height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(admgwaccess.gwusername)</script>:&nbsp;&nbsp; </FONT></TD>

         <% support_invmatch("MULTIUSER_SUPPORT", "1", "<!--"); %>
          <TD width=272 height=25><INPUT name="http_username" style="FONT-SIZE: 10pt; FONT-FAMILY: Arial"  maxLength=63 size=20 value="<% get_user("http_username"); %>"  onBlur=valid_name(this,"User%20Name")></TD>
         <% support_invmatch("MULTIUSER_SUPPORT", "1", "-->"); %>

         <% support_invmatch("SINGLEUSER_SUPPORT", "1", "<!--"); %>
          <TD width=272 height=25><INPUT name="http_username" style="FONT-SIZE: 10pt; FONT-FAMILY: Arial"  maxLength=63 size=20 value="<% nvram_get("http_username"); %>"  onBlur=valid_name(this,"User%20Name")></TD>
         <% support_invmatch("SINGLEUSER_SUPPORT", "1", "-->"); %>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25 rowspan="2">&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25 rowspan="2">&nbsp;</TD>

          <TD colSpan=3 height=13 width="42">&nbsp;</TD>

          <TD width=127 height=13><FONT style="FONT-SIZE: 8pt"><script>Capture(admgwaccess.gwpassword)</script>:&nbsp;&nbsp;</FONT></TD>

         <% support_invmatch("MULTIUSER_SUPPORT", "1", "<!--"); %>
          <TD width=272 height=13><INPUT  style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" type=password maxLength=63 size=20 value="d6nw5v1x2pc7st9m" name="http_passwd" onBlur=valid_name(this,"Password",SPACE_NO)></TD>
         <% support_invmatch("MULTIUSER_SUPPORT", "1", "-->"); %>

         <% support_invmatch("SINGLEUSER_SUPPORT", "1", "<!--"); %>
          <TD width=272 height=13><INPUT  style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" type=password maxLength=63 size=20 value="d6nw5v1x2pc7st9m" name="http_passwd" onBlur=valid_name(this,"Password",SPACE_NO)></TD>
         <% support_invmatch("SINGLEUSER_SUPPORT", "1", "-->"); %>


         <TD width=13 height=25 rowspan="2">&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25 rowspan="2">&nbsp;</TD></TR>

        <TR>

          <TD colSpan=3 height=12 width="42"></TD>

          <TD width=127 height=12><FONT style="FONT-SIZE: 8pt"><script>Capture(admgwaccess.reconfirm)</script>:&nbsp;&nbsp; </FONT></TD>

         <% support_invmatch("MULTIUSER_SUPPORT", "1", "<!--"); %>
          <TD width=272 height=12><INPUT  style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" type=password maxLength=63 size=20 value="d6nw5v1x2pc7st9m" name=http_passwdConfirm onBlur=valid_name(this,"Password",SPACE_NO)></TD>
         <% support_invmatch("MULTIUSER_SUPPORT", "1", "-->"); %>

         <% support_invmatch("SINGLEUSER_SUPPORT", "1", "<!--"); %>
          <TD width=272 height=12><INPUT  style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" type=password maxLength=63 size=20 value="d6nw5v1x2pc7st9m" name=http_passwdConfirm onBlur=valid_name(this,"Password",SPACE_NO)></TD>
         <% support_invmatch("SINGLEUSER_SUPPORT", "1", "-->"); %>

        </TR>

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=32>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=32>&nbsp;</TD>

          <TD colSpan=6 width="454" height="32">

            <TABLE>

              <TBODY>

              <TR>

                <TD width=16 height=30>&nbsp;</TD>

                <TD width=13 height=30>&nbsp;</TD>

                <TD vAlign=top width=410 colSpan=3 height=30><HR color=#b5b5e6 SIZE=1></TD>

                <TD width=15 height=30>&nbsp;</TD></TR></TBODY></TABLE></TD>

          <TD width=15 background=image/UI_05.gif height=32>&nbsp;</TD></TR>        

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=38><p align="right"><FONT style="FONT-WEIGHT: 700"><span ><script>Capture(admleftmenu.remotegwaccess)</script></span></FONT></TD>

          <TD width=8 background=image/UI_04.gif height=38>&nbsp;</TD>

          <TD colSpan=3 height=38 width="42">&nbsp;</TD>

          <TD width=127 height=38><SPAN  style="FONT-SIZE: 8pt"><script>Capture(admgwaccess.remotemgt)</script>:</SPAN></TD>

	   <TD width=272 height=38><INPUT type=radio value=1 name=remote_management <% remote_management_config("remote_management", 1);%> OnClick=SelPort(1,this.form)><b><script>Capture(share.enabled)</script></b>&nbsp;&nbsp;&nbsp;<INPUT type=radio value=0 name=remote_management <% remote_management_config("remote_management", 0); %> OnClick=SelPort(0,this.form)><b><script>Capture(share.disabled)</script></b></TD>

          <TD width=13 height=38>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=38>&nbsp;</TD></TR>

<% support_invmatch("HTTPS_SUPPORT", "1", "<!--"); %>
        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42"></TD>

          <TD width=127 height=25><SPAN  style="FONT-SIZE: 8pt"><script>Capture(admgwaccess.remoteusername)</script>:</SPAN></TD>

          <TD width=272 height=25><INPUT name="http_remote_username" style="FONT-SIZE: 10pt; FONT-FAMILY: Arial"  maxLength=63 size=20 value="<% nvram_get("http_remote_username", 0); %>"  onBlur=valid_name(this,"User%20Name")></TD>


          <TD width=13 height=25></TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42"></TD>

          <TD width=127 height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(admgwaccess.remotepassword)</script>:&nbsp; </FONT></TD>

          <TD width=272 height=25><INPUT  style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" type=password maxLength=63 size=20 value="d6nw5v1x2pc7st9m" name="http_remote_passwd" onBlur=valid_name(this,"Password",SPACE_NO)></TD>


          <TD width=13 height=25></TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42"></TD>

          <TD width=127 height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(admgwaccess.reconfirm)</script>: </FONT></TD>

          <TD width=272 height=25><INPUT  style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" type=password maxLength=63 size=20 value="d6nw5v1x2pc7st9m" name="http_remote_passwdConfirm" onBlur=valid_name(this,"Password",SPACE_NO)></TD>


          <TD width=13 height=25></TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
<% support_invmatch("HTTPS_SUPPORT", "1", "-->"); %>

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42">&nbsp;</TD>

          <TD width=127 height=25><SPAN  style="FONT-SIZE: 8pt"><script>Capture(admgwaccess.mgtport)</script>:</SPAN></TD>

          <TD width=272 height=25><INPUT class=num maxLength=5 size=7 value='<% remote_management_config("http_wanport", 0); %>' onBlur='valid_range(this,1,65535,"Port%20number")' name="http_wanport">&nbsp;&nbsp;</TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
<!-- Below for IP Range Resticted -->

<% show_ip_range_setting(); %>

<!-- Above for IP Range Restricted -->


<% support_invmatch("HTTPS_SUPPORT", "1", "<!--"); %>

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42">&nbsp;</TD>

          <TD width=127 height=25><script>Capture(admgwaccess.usehttps)</script>:</TD>

          <TD width=272 height=25><INPUT type=checkbox value=1 name=_remote_mgt_https <% nvram_match("remote_mgt_https","1","checked"); %>>&nbsp;</TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>
<% support_invmatch("HTTPS_SUPPORT", "1", "-->"); %>
        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>
            <p align="right"><b><script>Capture(admleftmenu.remoteupgrade)</script></b></p>
          </TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42">&nbsp;</TD>

          <TD width=127 height=25><script>Capture(admgwaccess.rmtupgrade)</script>:</TD>

          <TD width=272 height=25>

				
	<!--Changed by zhangbin-->
	<INPUT  type=radio value=1 name=remote_upgrade <% remote_management_config("remote_upgrade",1); %> ><b><script>Capture(share.enabled)</script></b>&nbsp;&nbsp;&nbsp;<INPUT  type=radio value=0 name=remote_upgrade <% remote_management_config("remote_upgrade",0); %> ><b><script>Capture(share.disabled)</script></b></TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <tr>

          <TD width=156 bgColor=#000000 colSpan=3 height=25><P align=right><B><FONT style="FONT-SIZE: 9pt" color=#ffffff><script>Capture(admleftmenu.snmp)</script></FONT></B></P></TD>

          <TD width=5 bgColor=#000000 height=25>&nbsp;</TD>

          <TD colSpan=6 width="454" height="27">

            <TABLE id="table1">

              <TBODY>

              <TR>

                <TD width=16 height=25>&nbsp;</TD>

                <TD width=12 height=25>&nbsp;</TD>

                <TD style="BORDER-TOP: 1px solid; BORDER-LEFT-WIDTH: 1px; BORDER-BOTTOM-WIDTH: 1px; BORDER-RIGHT-WIDTH: 1px" borderColor=#b5b5e6 width=411 height=25>&nbsp;</TD>

                <TD width=15 height=25>&nbsp;</TD></TR></TBODY></TABLE></TD>

          <TD width=15 background=image/UI_05.gif height=27>&nbsp;</TD>

	</tr>

	<tr>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42">&nbsp;</TD>

          <TD width=127 height=25>&nbsp;</TD>

          <TD width=272 height=25>

            &nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>

	</tr>

	<tr>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42">&nbsp;</TD>

          <TD width=127 height=25><SPAN  style="FONT-SIZE: 8pt"><script>Capture(admsnmp.snmp)</script>:</SPAN></TD>

          <TD width=272 height=25>

            <TABLE id=table2 cellSpacing=0 cellPadding=0 width=242 border=0>

              <TBODY>

              <TR>

                <TD width=242 height=25>

				<!--add by CSH  add onclick=SNMP_grey(this.form)-->
				<!--INPUT onclick=SNMP_grey(this.form) type=radio value=1 name=snmp_enable <% nvram_match("snmp_enable","1","checked"); %> ><b>Enable</b>&nbsp;&nbsp;&nbsp;<INPUT onclick=SNMP_grey(this.form) type=radio value=0 name=snmp_enable <% nvram_match("snmp_enable","0","checked"); %>><b>Disable</b></TD-->
	<!--Changed by CSH-->			
				<INPUT onclick=SNMP_grey(this.form) type=radio value=1 name=snmp_enable <% snmp_config("snmp_enable",1); %> ><b><script>Capture(share.enabled)</script></b>&nbsp;&nbsp;&nbsp;<INPUT onclick=SNMP_grey(this.form) type=radio value=0 name=snmp_enable <% snmp_config("snmp_enable",0); %>><b><script>Capture(share.disabled)</script></b></TD>

                </TR></TBODY></TABLE></TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>

		</tr>
<!--add by CSH-->
        <tr>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=21>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=21>&nbsp;</TD>

          <TD colSpan=3 height=21 width="42"></TD>

          <TD width=127 height=21></TD>

          <TD width=272 height=21>

            <SELECT name="snmp_ip_addr" onChange=SelRTYPE(this.form.snmp_ip_addr.selectedIndex,this.form)>
<option selected value="0" <% snmp_config("snmp_ip_addr", 0); %>><script>Capture(share.alls)</script></option>
<option value="1" <% snmp_config("snmp_ip_addr", 1); %>><script>Capture(share.ipaddrr)</script></option>
<option value="2" <% snmp_config("snmp_ip_addr", 2); %>><script>Capture(share.iprange)</script></option>





&nbsp;
</SELECT></TD>

          <TD width=13 height=21></TD>

          <TD width=15 background=image/UI_05.gif height=21>&nbsp;</TD>

        </tr>
<% show_snmp_setting(); %>
        <tr>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=32>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=32>&nbsp;</TD>

          <TD colSpan=6 width="454" height="32">

            <TABLE>

              <TBODY>

              <TR>

                <TD width=16 height=30>&nbsp;</TD>

                <TD width=13 height=30>&nbsp;</TD>

                <TD vAlign=top width=410 height=30><HR color=#b5b5e6 SIZE=1></TD>

                <TD width=15 height=30>&nbsp;</TD></TR></TBODY></TABLE></TD>

          <TD width=15 background=image/UI_05.gif height=32>&nbsp;</TD>
		</tr>

		<tr>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42">&nbsp;</TD>

          <TD width=127 height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(admsnmp.devname)</script>:&nbsp;</FONT></TD>

          <TD width=272 height=25>

            <FONT style="FONT-SIZE: 8pt" 
            face=Arial>
			<!--INPUT maxLength=39 name="devname" size="20" value='<% nvram_get("devname"); %>' )></FONT></TD-->
	<!--Changed by CSH-->
			<INPUT maxLength=39 name="devname" size="20" value='<% snmp_config("devname", 1); %>' )></FONT></TD>
          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>

		</tr>

        <tr>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42">&nbsp;</TD>

          <TD width=127 height=25><span style="font-size: 8pt"><script>Capture(admsnmp.snmp_v1v2)</script></span><FONT style="FONT-SIZE: 8pt">:</FONT></TD>

          <TD width=272 height=25>&nbsp;</TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
        </tr>

        <tr>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42">&nbsp;</TD>

          <TD width=127 height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(admsnmp.getcomm)</script>:&nbsp;</FONT></TD>

          <TD width=272 height=25>

            <FONT style="FONT-SIZE: 8pt" 
            face=Arial><INPUT maxLength=39 name="get_auth" size="20" value='<% nvram_get("get_auth"); %>' onBlur=validsnmp(this,2,39,'snmp')></FONT></TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
	</tr>

        <tr>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42">&nbsp;</TD>

          <TD width=127 height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(admsnmp.setcomm)</script>:&nbsp;</FONT></TD>

          <TD width=272 height=25>

            <FONT style="FONT-SIZE: 8pt" 
            face=Arial>
			<INPUT maxLength=39 name="set_auth" size="20" value='<% nvram_get("set_auth"); %>' onBlur=validsnmp(this,2,39,'snmp')></FONT></TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>

	</tr>
	<% support_invmatch("SNMPV3_SUPPORT", "1", "<!--"); %>
        <tr>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=19>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=19>&nbsp;</TD>

          <TD colSpan=3 height=19 width="42">&nbsp;</TD>

          <TD width=127 height=19><span style="font-size: 8pt"><script>Capture(admsnmp.snmp_v3)</script></span><FONT style="FONT-SIZE: 8pt">:</FONT></TD>

          <TD width=272 height=19>&nbsp;</TD>

          <TD width=13 height=19>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=19>&nbsp;</TD>
        </tr>
        <tr>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=12>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=12>&nbsp;</TD>

          <TD colSpan=3 height=12 width="42"></TD>

          <TD width=127 height=12><FONT style="FONT-SIZE: 8pt"><script>Capture(admsnmp.rwuser)</script>:</FONT></TD>

          <TD width=272 height=12>

          <FONT style="FONT-SIZE: 8pt" face=Arial><INPUT maxLength=39 name="v3rwuser" size="20" value='<% nvram_get("v3rwuser"); %>' onBlur=validsnmp(this,2,39,'snmp')></FONT></TD>

          <TD width=13 height=12></TD>

          <TD width=15 background=image/UI_05.gif height=12>&nbsp;</TD>

        </tr>
        <tr>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=21>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=21>&nbsp;</TD>

          <TD colSpan=3 height=21 width="42"></TD>

	  <TD width=127 height=12><FONT style="FONT-SIZE: 8pt"><script>Capture(admsnmp.authprotocol)</script>:</FONT></TD>

          <TD width=272 height=21><SELECT name="authprotocol" onChange=SelV3TYPE(this.form.authprotocol.selectedIndex,this.form)><option selected value="0" <% set_authproto("authprotocol", 0); %>><script>Capture(admsnmp.md5)</script></option><option value="1" <% set_authproto("authprotocol", 1); %>><script>Capture(admsnmp.sha)</script></option>&nbsp;</SELECT></TD>

          <TD width=13 height=21></TD>

          <TD width=15 background=image/UI_05.gif height=21>&nbsp;</TD>

        </tr>
        <tr>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42"></TD>

	  <TD width=127 height=12><FONT style="FONT-SIZE: 8pt"><script>Capture(admsnmp.authpassword)</script>:</FONT></TD>

          <TD width=272 height=25><INPUT  style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" type=password maxLength=63 size=20 value='<% nvram_gozila_get("v3authpasw"); %>' name="v3authpasw" onBlur=validpassword(this,8,39,'password')></TD>


          <TD width=13 height=25></TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>

        </tr>
        <tr>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42"></TD>

	  <TD width=127 height=12><FONT style="FONT-SIZE: 8pt"><script>Capture(admsnmp.privpassword)</script>:</FONT></TD>

          <TD width=272 height=25><INPUT  style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" type=password maxLength=63 size=20 value='<% nvram_gozila_get("v3privpasw"); %>' name="v3privpasw" onBlur=validpassword(this,8,39,'password')></TD>


          <TD width=13 height=25></TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>

        </tr>
        <tr>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42"></TD>

          <TD width=127 height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(admsnmp.md5passwd)</script>:&nbsp; </FONT></TD>

          <TD width=272 height=25><INPUT  style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" type=password maxLength=63 size=20 value='<% nvram_gozila_get("v3authpasw"); %>' name="v3authpasw" onBlur=validpassword(this,8,39,'password')></TD>


          <TD width=13 height=25></TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>

        </tr>
	<% support_invmatch("SNMPV3_SUPPORT", "1", "-->"); %>
        <tr>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42">&nbsp;</TD>

          <TD width=127 height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(admsnmp.trapmgt)</script>:</FONT></TD>

          <TD width=272 height=25>&nbsp;</TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>

        </tr>
        <tr>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=22>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=22>&nbsp;</TD>

          <TD colSpan=3 height=22 width="42"></TD>

          <TD width=127 height=22><FONT style="FONT-SIZE: 8pt"><script>Capture(admsnmp.trapto)</script>:</FONT></TD>

          <TD width=272 height=22>

            <INPUT class=num maxLength=3 onBlur=valid_range(this,1,223,"IP") size=3 value='<% get_single_ip("trap_ipaddr","0"); %>' name="trap_ipaddr_0"> . 
            <INPUT class=num maxLength=3 onBlur=valid_range(this,0,255,"IP") size=3 value='<% get_single_ip("trap_ipaddr","1"); %>' name="trap_ipaddr_1"> . 
            <INPUT class=num maxLength=3 onBlur=valid_range(this,0,255,"IP") size=3 value='<% get_single_ip("trap_ipaddr","2"); %>' name="trap_ipaddr_2"> . 
            <INPUT class=num maxLength=3 onBlur=valid_range(this,1,254,"IP") size=3 value='<% get_single_ip("trap_ipaddr","3"); %>' name="trap_ipaddr_3"></TD>

          <TD width=13 height=22></TD>

          <TD width=15 background=image/UI_05.gif height=22>&nbsp;</TD>

        </tr>

        <TR>

          <TD width=156 bgColor=#000000 colSpan=3 height=25><P align=right><B><FONT style="FONT-SIZE: 9pt" color=#ffffff><script>Capture(admleftmenu.upnp)</script></FONT></B></P></TD>

          <TD width=5 bgColor=#000000 height=25>&nbsp;</TD>

          <TD colSpan=6 width="454" height="27">

            <TABLE>

              <TBODY>

              <TR>

                <TD width=16 height=25>&nbsp;</TD>

                <TD width=12 height=25>&nbsp;</TD>

                <TD style="BORDER-TOP: 1px solid; BORDER-LEFT-WIDTH: 1px; BORDER-BOTTOM-WIDTH: 1px; BORDER-RIGHT-WIDTH: 1px" borderColor=#b5b5e6 width=411 height=25>&nbsp;</TD>

                <TD width=15 height=25>&nbsp;</TD></TR></TBODY></TABLE></TD>

          <TD width=15 background=image/UI_05.gif height=27>&nbsp;</TD></TR>

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42"></TD>

          <TD width=127 height=25><SPAN  style="FONT-SIZE: 8pt"><script>Capture(admupnp.upnp)</script>:</SPAN></TD>

          <TD width=272 height=25>

            <INPUT type=radio value=1 onclick=warn_user() name=upnp_enable <% nvram_match("upnp_enable","1","checked"); %> checked><b><script>Capture(share.enabled)</script></b>&nbsp;&nbsp;&nbsp;<INPUT type=radio value=0 name=upnp_enable <% nvram_match("upnp_enable","0","checked"); %>><b><script>Capture(share.disabled)</script></b></TD>

          <TD width=13 height=25></TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>               

	<% support_invmatch("MULTIPVC_SUPPORT", "1", "<!--"); %>
        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=38>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=38>&nbsp;</TD>

          <TD colSpan=3 height=38 width="42"></TD>

          <TD width=127 height=38><script>Capture(admupnp.selectpvc)</script>:</TD>

          <TD width=272 height=38>

            &nbsp;<SELECT name="upnp_pvc_connection" onChange=SelPvcConnection(this.form.upnp_pvc_connection.selectedIndex,this.form) size="1">
<% upnp_connection_table("select"); %></SELECT></TD>

          <TD width=13 height=38></TD>

          <TD width=15 background=image/UI_05.gif height=38>&nbsp;</TD></TR>

	<% support_invmatch("MULTIPVC_SUPPORT", "1", "-->"); %>

	<% support_match("MULTIPVC_SUPPORT", "1", "<!--"); %>
	<input type=hidden name="upnp_pvc_connection" value=0>
	<% support_match("MULTIPVC_SUPPORT", "1", "-->"); %>

 <!-- Below is for IGMP Proxy -->

	<% support_elsematch("IGMP_PROXY_SUPPORT", "1", "", "<!--"); %>
        <tr>
          <TD width=156 bgColor=#000000 colSpan=3 height=27><P align=right>
          <font color="#FFFFFF"><span style="font-size: 9pt"><b><script>Capture(admleftmenu.igmpproxy)</script></b></span></font></P></TD>
          <TD width=5 bgColor=#000000 height=25>&nbsp;</TD>
          <TD colSpan=6 width="454" height="27">
            <TABLE>
              <TBODY>
              <TR>
                <TD width=16 height=25>&nbsp;</TD>
                <TD width=12 height=25>&nbsp;</TD>
                <TD style="BORDER-TOP: 1px solid; BORDER-LEFT-WIDTH: 1px; BORDER-BOTTOM-WIDTH: 1px; BORDER-RIGHT-WIDTH: 1px" borderColor=#b5b5e6 width=411 height=25>&nbsp;</TD>
                <TD width=15 height=25>&nbsp;</TD></TR></TBODY></TABLE></TD>
          <!--TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
        </tr>
        <tr>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="42">&nbsp;</TD>
          <TD width=127 height=25><span style="font-size: 8pt"><script>Capture(admigmpproxy.pvcavail)</script></span></TD>
          <TD width=242 height=25>&nbsp; 
                <select size="1" name="igmp_proxy_channel" onchange=SelIgmpProxyChannel(this.form.igmp_proxy_channel.selectedIndex,this.form)>
                	<% igmp_proxy_channel(); %>
                </select>
           </TD>
          <TD width=13 height=25>&nbsp;</TD-->
          <TD width=15 background=image/UI_05.gif height=27>&nbsp;</TD>
        </tr>
         <tr>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>
          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>
          <TD colSpan=3 height=25 width="42">&nbsp;</TD>
          <TD width=127 height=25><span style="font-size: 8pt"><script>Capture(admigmpproxy.igmpproxy)</script></span></TD>
         <TD width=242 height=25>
                <INPUT type=radio value=1 name=igmp_proxy_enable <% igmp_proxy_config_setting("igmp_proxy_enable", 1); %>><b><script>Capture(share.enabled)</script></b>&nbsp;&nbsp;&nbsp;
                <INPUT type=radio value=0 name=igmp_proxy_enable <% igmp_proxy_config_setting("igmp_proxy_enable", 0); %>><b><script>Capture(share.disabled)</script></b>
         </TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
        </tr>

	<input type=hidden name="igmp_proxy_channel" value=0>
	<% support_elsematch("IGMP_PROXY_SUPPORT", "1", "", "-->"); %>
<!-- above is for IGMP Proxy --> 

        <!--TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=1>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>

          <TD colSpan=6 width="454">

            <TABLE>

              <TBODY>

              <TR>

                <TD width=16 height=1>&nbsp;</TD>

                <TD width=13 height=1>&nbsp;</TD>

                <TD width=410 height=1><HR color=#b5b5e6 SIZE=1></TD>

                <TD width=15>&nbsp;</TD></TR></TBODY></TABLE></TD>

          <TD width=15 background=image/UI_05.gif height=1>&nbsp;</TD></TR-->

	<% support_elsematch("CPED_TR064_SUPPORT","1","","<!--"); %>
         <TR>

          <TD width=156 bgColor=#000000 colSpan=3 height=27>
          <P align=right><B><FONT style="FONT-SIZE: 9pt" color=#ffffff><script>Capture(admleftmenu.tr064)</script></FONT></B></P>
          </TD>

          <TD width=5 bgColor=#000000 height=25>&nbsp;</TD>

          <TD colSpan=6 width="454" height="27">

            <TABLE>

              <TBODY>

              <TR>

                <TD width=16 height=25>&nbsp;</TD>

                <TD width=12 height=25>&nbsp;</TD>

                <TD style="BORDER-TOP: 1px solid; BORDER-LEFT-WIDTH: 1px; BORDER-BOTTOM-WIDTH: 1px; BORDER-RIGHT-WIDTH: 1px" borderColor=#b5b5e6 width=411 height=25>&nbsp;</TD>

		<TD width=15 height=25>&nbsp;</TD></TR></TBODY></TABLE></TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42">&nbsp;</TD>

          <TD width=127 height=25><span style="font-size: 8pt"><script>Capture(admtr064.tr064)</script></span><SPAN  style="FONT-SIZE: 8pt">:</SPAN></TD>

          <TD width=272 height=25>

            <TABLE id=AutoNumber12 cellSpacing=0 cellPadding=0 width=242 border=0>

              <TBODY>

              <TR>

                <TD width=242 height=25><INPUT type=radio value=1 name=tr064_enable <% nvram_match("tr064_enable","1","checked"); %>><b><script>Capture(share.enabled)</script></b>&nbsp;&nbsp;&nbsp;<INPUT type=radio value=0 name=tr064_enable <% nvram_match("tr064_enable","0","checked"); %>><b><script>Capture(share.disabled)</script></b></TD>

                </TR></TBODY></TABLE></TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>               
<% support_elsematch("CPED_TR064_SUPPORT","1","","-->"); %>

<% support_invmatch("CWMP_SUPPORT", "1", "<!--"); %>
        <TR>

          <TD width=156 bgColor=#000000 colSpan=3 height=27><P align=right><B><FONT style="FONT-SIZE: 9pt" color=#ffffff><script>Capture(admleftmenu.tr069)</script></FONT></B></P></TD>

          <TD width=5 bgColor=#000000 height=25>&nbsp;</TD>

          <TD colSpan=6 width="454" height="27">

            <TABLE>

              <TBODY>

              <TR>

                <TD width=16 height=25>&nbsp;</TD>

                <TD width=12 height=25>&nbsp;</TD>

                <TD style="BORDER-TOP: 1px solid; BORDER-LEFT-WIDTH: 1px; BORDER-BOTTOM-WIDTH: 1px; BORDER-RIGHT-WIDTH: 1px" borderColor=#b5b5e6 width=411 height=25>&nbsp;</TD>

                <TD width=15 height=25>&nbsp;</TD></TR></TBODY></TABLE></TD>

          <TD width=15 background=image/UI_05.gif height=27>&nbsp;</TD></TR>

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42">&nbsp;</TD>

          <TD width=127 height=25><SPAN  style="FONT-SIZE: 8pt"><script>Capture(admtr069.tr069)</script>:</SPAN></TD>

          <TD width=272 height=25>

            <TABLE cellSpacing=0 cellPadding=0 width=242 border=0>

              <TBODY>

              <TR>

                <TD width=242 height=25><INPUT type=radio value=1 name="cwmp_enable" <% nvram_match("cwmp_enable","1","checked"); %>><b><script>Capture(share.enabled)</script></b>&nbsp;&nbsp;&nbsp;<INPUT type=radio value=0 name="cwmp_enable" <% nvram_match("cwmp_enable","0","checked"); %>><b><script>Capture(share.disabled)</script></b></TD>

                </TR></TBODY></TABLE></TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>               
	
	<tr>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42">&nbsp;</TD>

          <TD width=127 height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(admtr069.acsurl)</script>:</FONT></TD>

          <TD width=272 height=25>

          <FONT style="FONT-SIZE: 8pt"><INPUT maxLength=256 name="acs_url" size="20" value='<% nvram_get("acs_url"); %>' )></FONT></TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>

	</tr>
	<tr>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42"></TD>

          <TD width=127 height=25><FONT style="FONT-SIZE: 8pt" 
            face=Arial><script>Capture(admtr069.acsusername)</script>:</FONT></TD>

          <TD width=272 height=25>

	  <FONT style="FONT-SIZE: 8pt" face=Arial><INPUT maxLength=64 name="acs_username" size="20" value='<% nvram_get("acs_username"); %>' )></FONT></TD>

          <TD width=13 height=25></TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>

	</tr>
	<tr>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42">&nbsp;</TD>

          <TD width=127 height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(admtr069.acspassword)</script>:&nbsp;</TD>

          <TD width=272 height=25>

          <INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" type=password maxLength=64  name="acs_password" size="20" value="d6nw5v1x2pc7st9m" onBlur=valid_name(this,"Password",SPACE_NO)></TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>

	</tr>
	
	<tr>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42">&nbsp;</TD>

          <TD width=127 height=25><FONT style="FONT-SIZE: 8pt"><script>Capture(admtr069.acspassword)</script>:&nbsp;</FONT></TD>

          <TD width=272 height=25>

            <INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" type=password maxLength=64  name="acs_password" size="20" value="d6nw5v1x2pc7st9m" onBlur=valid_name(this,"Password",SPACE_NO)></TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>

	</tr>
<% support_invmatch("CWMP_SUPPORT", "1", "-->"); %>

        <% wlan_management_support(1); %>
        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=6 width="454" height="25">

            <TABLE>

              <TBODY>

              <TR>

                <TD width=16 height=1>&nbsp;</TD>

                <TD width=13 height=1>&nbsp;</TD>

                <TD width=410 height=1><HR color=#b5b5e6 SIZE=1></TD>

                <TD width=15>&nbsp;</TD></TR></TBODY></TABLE></TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>

         <TR>

          <TD width=156 bgColor=#000000 colSpan=3 height=27>
          <P align=right><B><FONT style="FONT-SIZE: 9pt" color=#ffffff>WLAN</script></FONT></B></P>
          </TD>

          <TD width=5 bgColor=#000000 height=25>&nbsp;</TD>

          <TD colSpan=6 width="454" height="27">

            <TABLE>

              <TBODY>

              <TR>

                <TD width=16 height=25>&nbsp;</TD>

                <TD width=12 height=25>&nbsp;</TD>


                <TD width=15 height=25>&nbsp;</TD></TR></TBODY></TABLE></TD>

          <TD width=15 background=image/UI_05.gif height=27>&nbsp;</TD></TR>

        <TR>
          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42">&nbsp;</TD>

          <TD width=127 height=25><span style="font-size: 8pt"><script>Capture(admwlan.mgtwlan)</script></span><SPAN  style="FONT-SIZE: 8pt">:</SPAN></TD>

          <TD width=272 height=25>

            <TABLE id=AutoNumber12 cellSpacing=0 cellPadding=0 width=242 border=0>

              <TBODY>

              <TR>

                <TD width=242 height=25><INPUT type=radio value=1 name=wl_management_enable <% nvram_match("wl_management_enable","1","checked"); %>><b><script>Capture(share.enabled)</script></b>&nbsp;&nbsp;&nbsp;<INPUT type=radio value=0 name=wl_management_enable <% nvram_match("wl_management_enable","0","checked"); %>><b><script>Capture(share.disabled)</script></b></TD>

                </TR></TBODY></TABLE></TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD></TR>


        <% wlan_management_support(2); %>

<% support_elsematch("CLI_SUPPORT", "1", "", "<!--"); %>
        <tr>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=1>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=1>&nbsp;</TD>

          <TD colSpan=6 width="454">

            <TABLE>

              <TBODY>

              <TR>

                <TD width=16 height=1>&nbsp;</TD>

                <TD width=13 height=1>&nbsp;</TD>

                <TD width=410 height=1><HR color=#b5b5e6 SIZE=1></TD>

                <TD width=15>&nbsp;</TD></TR></TBODY></TABLE></TD>

          <TD width=15 background=image/UI_05.gif height=1>&nbsp;</TD>
        </tr>
        <tr>

          <TD width=156 bgColor=#000000 colSpan=3 height=25>
          <P align=right><b><font style="font-size: 9pt" color="#ffffff">Telnet</font></b></P>
          </TD>

          <TD width=5 bgColor=#000000 height=25></TD>

          <TD colSpan=6 width="454">

            <TABLE>

              <TBODY>

              <TR>

                <TD width=16 height=25>&nbsp;</TD>

                <TD width=12 height=25>&nbsp;</TD>


                <TD width=15 height=25>&nbsp;</TD></TR></TBODY></TABLE></TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
        </tr>
        <tr>
        <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=25>&nbsp;</TD>

          <TD colSpan=3 height=25 width="42">&nbsp;</TD>

          <TD width=127 height=25>
            <p align="left"><span style="font-size: 8pt">Telnet:</span></p>
          </TD>

          <TD width=272 height=25>

            <TABLE id=AutoNumber12 cellSpacing=0 cellPadding=0 width=242 border=0>

              <TBODY>

              <TR>

                <TD width=242 height=25><INPUT type=radio value=1 name=telnet_enable <% nvram_match("telnet_enable","1","checked"); %>><b>Enable</b>&nbsp;&nbsp;&nbsp;<INPUT type=radio value=0 name=telnet_enable <% nvram_match("telnet_enable","0","checked"); %>><b>Disable</b></TD>

                </TR></TBODY></TABLE></TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&nbsp;</TD>
        </tr>
<% support_elsematch("CLI_SUPPORT", "1", "", "-->"); %>

        <TR>

          <TD width=42 bgColor=#e7e7e7 height=19>&nbsp;</TD>

          <TD width=62 bgColor=#e7e7e7 height=19>&nbsp;</TD>

          <TD width=52 bgColor=#e7e7e7 height=19>&nbsp;</TD>

          <TD width=8 background=image/UI_04.gif height=19>&nbsp;</TD>

          <TD width=12 height=19></TD>

          <TD width=17 height=19></TD>

          <TD width=13 height=19></TD>

          <TD width=127 height=19></TD>

          <TD width=272 height=19></TD>

          <TD width=13 height=19></TD>

          <TD width=15 background=image/UI_05.gif height=19>&nbsp;</TD></TR></TBODY></TABLE></TD>

    <TD vAlign=top width=176 bgColor=#6666cc>

      <TABLE cellSpacing=0 cellPadding=0 width=176 border=0>

        <TBODY>

        <TR>

          <TD width=11 bgColor=#6666cc height=25></TD>

          <TD width=156 bgColor=#6666cc height=25><font color="#FFFFFF"><span><a target="_blank" href="help/HManagement.asp"><script>Capture(share.more)</script>...</a></span></font></TD>

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
                <TD align=middle bgColor=#434a8f width="111"><FONT color=#ffffff><B><A href='Management.asp'><script>Capture(share.cancels)</script></A></B></FONT></TD>
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
