<HTML><HEAD><TITLE>Security</TITLE>
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
<script language="JavaScript" type="text/javascript" src="capsec.js"></script>
<script language="JavaScript" type="text/javascript" src="capadmin.js"></script>
<SCRIPT language=JavaScript>
document.title=(bmenu.security);
<% wireless_support(5); %>share.productname = share.productname1;<% wireless_support(6); %>
var ZERO_NO = 1;	/* 0x0000 0001*/
var ZERO_OK = 2;	/* 0x0000 0010*/
/* x.x.x.0*/
var MASK_NO = 4;	/* 0x0000 0100*/
var MASK_OK = 8;	/* 0x0000 1000*/
/* 255.255.255.255*/
var BCST_NO = 16;	/* 0x0001 0000*/
var BCST_OK = 32;	
var t_enable;

function tunnel_enable()
{
	t_enable = 1;
}

function tunnel_disable()
{
	t_enable = 0;
}

function verify_name(F)
{
	var name = new Array(5);
	var i;
	var name_str = F.tunnel_name.value;

	for(i = 0;i < 5;i++)
	{
		name[i] = "";
	}

<% name_array(); %>

	for(i = 0;i < 5;i++)
	{
		if(name_str == name[i])
		{
			var j = i + 1;
			//alert('Tunnel name conflicts with tunnel '+j);
			alert(errmsg.err34 + j);
			return false;
		}
	}
	return true;
}

function verify_spi(F)
{
	var in_spi = new Array(5);
	var out_spi = new Array(5);
	var i;
	var ispi = F.manual_ispi.value;
	var ospi = F.manual_ospi.value;

	for(i = 0;i < 5;i++)
	{
		in_spi[i] = "";
		out_spi[i] = "";
	}

<% spi_array(); %>
	for(i = 0;i < 5;i++)
	{
		if(ispi == in_spi[i] || ispi == out_spi[i] || ospi == in_spi[i] || ospi == out_spi[i])
		{
			//alert('SPI has been used!\n');
			alert(errmsg.err35);
			return false;
		}
	}
	return true;
}

function SelTunnel(num,F)
{
	F.submit_button.value = "ipsec";
    F.change_action.value = "gozila_cgi";
    F.tunnel_entry.value=F.tunnel_entry.options[num].value;
    F.submit();
}
	
function SelLTYPE(num,F)
{
        F.submit_button.value = "ipsec";
        F.change_action.value = "gozila_cgi";
        F.local_type.value=F.local_type.options[num].value;
        F.submit();
}

function SelLgw(num,F)
{
        F.submit_button.value = "ipsec";
        F.change_action.value = "gozila_cgi";
        F.localgw_conn.value = F.localgw_conn.options[num].value;
        F.submit();
}

function SelRTYPE(num,F)
{
        F.submit_button.value = "ipsec";
        F.change_action.value = "gozila_cgi";
        F.remote_type.value=F.remote_type.options[num].value;
        F.submit();
}

function SelSGTYPE(num,F)
{
	if((F.sg_type.value == 1) && (F.key_type.value == 1))
	{
		//alert('Remote security gateway value can Not be \'any\' when Key value is \'manual\'');
		alert(errmsg.err36);
		F.sg_type.value = 0;
		return false;
	}
        F.submit_button.value = "ipsec";
        F.change_action.value = "gozila_cgi";
        F.sg_type.value=F.sg_type.options[num].value;
        F.submit();
}

function SelKEYTYPE(num,F)
{
	if((F.sg_type.value == 1) && (F.key_type.value == 1))
	{
		//alert('Key value can Not be \'manual\' when remote security gateway value is \'any\'');
		alert(errmsg.err37);
		F.key_type.value = 0;
		return false;
	}
        F.submit_button.value = "ipsec";
        F.change_action.value = "gozila_cgi";
        F.key_type.value=F.key_type.options[num].value;
        F.submit();
}

function LogButton_check(F,V) 
{
	self.open('SysLog.asp?LogType=4','SysLog','alwaysRaised,resizable,scrollbars,width=600,height=440');
}

function to_delete(F)
{
	//if(confirm("Delete This Tunnel?")){
	if(confirm(errmsg.err122)){
	   	F.submit_button.value = "ipsec";
     		F.change_action.value = "gozila_cgi";
		F.submit_type.value = "delete";
	       	//F.action.value = "Apply";
       		F.submit();
	}
}

function checkenc(F,I,num)
{
	var len = I.value.length;
	var ch = new Array(16);
	var result=0;

	if ( (I.value.indexOf('0', 0) == 0) && (I.value.indexOf('x', 0) == 1) )
	{
		for (i=0; i < 10; i++)
			ch[i] = i;

		ch[10] = 'a'; ch[11] = 'b'; ch[12] = 'c';
		ch[13] = 'd'; ch[14] = 'e'; ch[15] = 'f';

		for (k=len-1; k >= 2; k--)
		{
			for (i=0; i < 16; i++) 
			{
				if (I.value.indexOf(ch[i], k) == -1)
					result=0;
				else if (I.value.indexOf(ch[i], k) != k)
					result=0;
				else 
				{
					result=1;
					break;
				}
			}
			if (result == 0) 
			{
				//alert('Character is out of range (0-9 or a-f) !');
				alert(errmsg.err38);
				I.value = I.defaultValue;
				return false;
			}
		}

		if(F.enc_type.value == 1 && len != 16)
		{
				//alert('The length for DES encryption Key should be 16!');
				alert(errmsg.err39);
				return false;
		}
		else if(F.enc_type.value == 2 && len != 48)
		{
				//alert('The length for 3DES encryption Key should be 48!');
				alert(errmsg.err40);
				return false;
		}
	}
	else 
	{
		if(F.enc_type.value == 1 && len != 8)
		{
				//alert('The length for DES encryption Key should be 8!');
				alert(errmsg.err41);
				return false;
		}
		else if(F.enc_type.value == 2 && len != 24)
		{
				//alert('The length for 3DES encryption Key should be 24!');
				alert(errmsg.err42);
				return false;
		}
	}
	return true;
}

function checkauth(F,I,num)
{
	var len = I.value.length;
	var ch = new Array(16);
	var result=0;

	if ( (I.value.indexOf('0', 0) == 0) && (I.value.indexOf('x', 0) == 1) )
	{
		for (i=0; i < 10; i++)
			ch[i] = i;

		ch[10] = 'a'; ch[11] = 'b'; ch[12] = 'c';
		ch[13] = 'd'; ch[14] = 'e'; ch[15] = 'f';

		for (k=len-1; k >= 2; k--)
		{
			for (i=0; i < 16; i++) 
			{
				if (I.value.indexOf(ch[i], k) == -1)
					result=0;
				else if (I.value.indexOf(ch[i], k) != k)
					result=0;
				else 
				{
					result=1;
					break;
				}
			}
			if (result == 0) 
			{
				//alert('Character is out of range (0-9 or a-f) !');
				alert(errmsg.err38);
				I.value = I.defaultValue;
				return false;
			}
		}

		if(F.auth_type.value == 1 && len != 32)
		{
				//alert('The length for MD5 authentication Key should be 32!');
				alert(errmsg.err43);
				return false;
		}
		else if(F.auth_type.value == 2 && len != 40)
		{
				//alert('The length for SHA1 authentication Key should be 40!');
				alert(errmsg.err44);
				return false;
		}
	}
	else 
	{
		if(F.auth_type.value == 1 && len != 16)
		{
				//alert('The length for MD5 authentication Key should be 16!');
				alert(errmsg.err45);
				return false;
		}
		else if(F.auth_type.value == 2 && len != 20)
		{
				//alert('The length for SHA1 authentication Key should be 20!');
				alert(errmsg.err46);
				return false;
		}
	}
	return true;
}

function checkspi(F,I)
{
	var value = I.value;
	var len = I.value.length;
	var ch = new Array(16);
	var result=0;

	if( value < 100 || len != 3 )
	{
		//alert('The SPI value should be between 0x100 and 0xFFF!');
		alert(errmsg.err47);
		return false;
	}
	else
	{
		for (i=0; i < 10; i++)
			ch[i] = i;

		ch[10] = 'a'; ch[11] = 'b'; ch[12] = 'c';
		ch[13] = 'd'; ch[14] = 'e'; ch[15] = 'f';

		for (k=len-1; k >= 0; k--)
		{
			for (i=0; i < 16; i++) 
			{
				if (I.value.indexOf(ch[i], k) == -1)
					result=0;
				else if (I.value.indexOf(ch[i], k) != k)
					result=0;
				else 
				{
					result=1;
					break;
				}
			}
			if (result == 0) 
			{
				//alert('The SPI value should be between 0x100 and 0xFFF!');
				alert(errmsg.err47);
				I.value = I.defaultValue;
				return false;
			}
		}
	}
	return true;
}

function checkKEY(I,F,maxlen,num)
{
	var len=I.value.length;
	var ch = new Array(16);
	var result=0;


	if ( (I.value.indexOf('0', 0) == 0) && (I.value.indexOf('x', 0) == 1) )
	{

		for (i=0; i < 10; i++)
			ch[i] = i;

		ch[10] = 'a'; ch[11] = 'b'; ch[12] = 'c';
		ch[13] = 'd'; ch[14] = 'e'; ch[15] = 'f';

		for (k=len-1; k >= 2; k--) {
			for (i=0; i < 16; i++) {
				if (I.value.indexOf(ch[i], k) == -1)
					result=0;
				else if (I.value.indexOf(ch[i], k) != k)
					result=0;
				else {
					result=1;
					break;
				}
			}
			if (result == 0) {
//				alert('Character is out of range (0-9 or a-f) !');
				alert(errmsg.err38);
				I.value = I.defaultValue;
				return;
			}
		}
	}
	 else 
	{
			if (len > maxlen)
			{
				//alert('The length of the Key must be less '+maxlen+' characters !');
				alert(errmsg.err48 + maxlen + errmsg.err49);
				I.value = I.defaultValue;
				return;
			}
	}
}

function Connect(F,I)
{
	if(I == 1)
        	F.submit_type.value = "connect";
	else 
        	F.submit_type.value = "disconnect";
	F.submit_button.value = "ipsec";
        F.change_action.value = "gozila_cgi";
        F.submit();

}

function valid_ip(F,N,M1,flag){
	var m = new Array(4);
	M = unescape(M1);

	for(i=0;i<4;i++)
		m[i] = eval(N+i).value;
	if(m[0] == 127 || m[0] == 224){
		//alert(M+' value is illegal!');
		alert(M + errmsg.err50);
		return false;
	}
	if(m[0] == '0' && m[1] == '0' && m[2] == '0' && m[3] == '0'){
		if(flag & ZERO_NO){
//			alert(M+' value is illegal!');
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

function valid_sg_ip(F,N,M1,flag){
	var m = new Array(4);
	M = unescape(M1);

	for(i=0;i<4;i++)
		m[i] = eval(N+i).value;

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

function valid_mask(F,N,flag){
	var match0 = -1;
	var match1 = -1;
	var m = new Array(4);
	for(i=0;i<4;i++)
		m[i] = eval(N+i).value;

	if(m[0] == '0' && m[1] == '0' && m[2] == '0' && m[3] == '0'){
		if(flag & ZERO_NO){
			//alert('Illegal subnet mask!');
			alert(errmsg.err51);
			return false;
		}
		else if(flag & ZERO_OK){
			return true;
		}
	}

	if(m[0] == '255' && m[1] == '255' && m[2] == '255' && m[3] == '255'){
		if(flag & BCST_NO){
			//alert('Illegal subnet mask!');
			alert(errmsg.err51);
			return false;
		}
		else if(flag & BCST_OK){
			return true;
		}
	}

	for(i=3;i>=0;i--){
		for(j=1;j<=8;j++){
			if((m[i] % 2) == 0)   match0 = (3-i)*8 + j;
			else if(((m[i] % 2) == 1) && match1 == -1)   match1 = (3-i)*8 + j;
			m[i] = Math.floor(m[i] / 2);
		}
	}
	if(match0 > match1){
		//alert('Illegal subnet mask!');
		alert(errmsg.err51);
		return false;
	}
	return true;
}

function valid_ip_gw(F,I,N,G)
{
	var IP = new Array(4);
	var NM = new Array(4);
	var GW = new Array(4);

	for(i=0;i<4;i++)
		IP[i] = eval(I+i).value;
	for(i=0;i<4;i++)
		NM[i] = eval(N+i).value;
	for(i=0;i<4;i++)
		GW[i] = eval(G+i).value;

	for(i=0;i<4;i++){
		if((IP[i] & NM[i]) != (GW[i] & NM[i])){
			//alert('IP address and gateway is not at same subnet mask!');
			alert(errmsg.err52);
			return false;
		}
	}
	if((IP[0] == GW[0]) && (IP[1] == GW[1]) && (IP[2] == GW[2]) && (IP[3] == GW[3])){
		//alert('IP address and gateway can\'t be same!');
		alert(errmsg.err53);
		return false;
	}
	
	return true;
}

function valid_ip_deviceip(F,I,N,G,K)
{	
	var IP = new Array(4);	
	var NM = new Array(4);	
	var GW = new Array(4);	
	var KM = new Array(4);	
	for(i=0;i<4;i++)		
		IP[i] = eval(I+i).value;	
	for(i=0;i<4;i++)		
		NM[i] = eval(N+i).value;	
	for(i=0;i<4;i++)		
		GW[i] = eval(G+i).value;
	for(i=0;i<4;i++)		
		KM[i] = eval(K+i).value;
	for(i=0;i<4;i++)
	{
		if((IP[i] & NM[i]) != (GW[i] & KM[i]))
		{			
			//alert('Device IP address and Local Secure Group ip  is not at same subnet mask!');
			alert(errmsg.err54);
			return false;		
		}	
	}		
	return true;
}

function check_colon(I,M1)
{
	M = unescape(M1);
	for(i = 0;i < I.value.length; i++)
	{
		ch = I.value.charAt(i);
		if(ch == ':')
		{
			//alert(M + 'is not allow \':\'');
			alert(M + errmsg.err55);
			return false;
		}
	}
	return true;
}


function valid_value(F)
{
	if(F.tunnel_name.value == "")
	{
		//alert("Please enter this tunnel name!");
		alert(errmsg.err56);

		F.tunnel_name.focus();
		return false;
	}
	if(!check_colon(F.tunnel_name,"Tunnel Name") || !check_space(F.tunnel_name,"Tunnel Name"))
	{
		F.tunnel_name.focus();
		return false;
	}
	if(!verify_name(F))
	{
		F.tunnel_name.focus();
		return false;
	}


	if(F.local_type.value == 0)
	{
		if(!valid_ip(F,'F.l_ipaddr','IP',ZERO_NO|MASK_NO))
			return false;
		if(!valid_ip_deviceip(F,'F.l_ipaddr','F.dev_netmask','F.dev_ip','F.dev_netmask'))	
			return false;	
	}
	if(F.local_type.value == 1)
	{
		if(!valid_ip(F,'F.l_ipaddr','IP',ZERO_NO))
			return false;
		if(!valid_mask(F,'F.l_netmask',ZERO_OK|BCST_NO))	
			return false;	
		if(!valid_ip_deviceip(F,'F.l_ipaddr','F.l_netmask','F.dev_ip','F.dev_netmask'))	
			return false;	
	}

	if(F.localgw_conn.value == 8)
	{
		//alert('No WAN Connection!');
		alert(errmsg.err57);
		return false;
	}

	if(F.remote_type.value == 0)
	{
		if(!valid_ip(F,'F.r_ipaddr','IP',ZERO_NO|MASK_NO))
			return false;
		if(!valid_range(F.r_ipaddr0,1,223,"IP"))
		{
			F.r_ipaddr0.focus();
			return false;
		}

	}
	if(F.remote_type.value == 1)
	{
		if(!valid_ip(F,'F.r_ipaddr','IP',ZERO_NO))
			return false;
		if(!valid_range(F.r_ipaddr0,1,223,"IP"))
		{
			F.r_ipaddr0.focus();
			return false;
		}

		if(!valid_mask(F,'F.r_netmask',ZERO_NO|BCST_NO))	
		{
			return false;	
		}
	}

	if(F.sg_type.value == 0)
	{
		if(!valid_ip(F,'F.sg_ipaddr','Gateway',ZERO_NO|MASK_NO))
			return false;
		if(!valid_range(F.sg_ipaddr0,1,223,"IP"))
		{
			F.sg_ipaddr0.focus();
			return false;
		}

	}
	
	if(F.key_type.value == 0)
	{
		if(F.auto_presh.value == "")
		{
			//alert('Please enter Pre-shared Key!');
			alert(errmsg.err58);
			return false;
		}
/*		if(!check_colon(F.auto_presh,"Pre-shared Key") || !check_space(F.auto_presh,"Pre-shared Key"))
		{
			F.auto_presh.focus();
			return false;
		}*/

		if(F.auto_klife.value < 120 || F.auto_klife.value > 604800)
		{
			//alert('Key Lifetime out of range [120~604800]!');
			alert(errmsg.err59);
			return false;
		}
		
	}
	else if(F.key_type.value == 1)
	{
		if(!checkenc(F,F.manual_enckey,0))
			return false;
		else if(!checkauth(F,F.manual_authkey,0))
			return false;
		if(!checkspi(F,F.manual_ispi))
			return false;
		if(!checkspi(F,F.manual_ospi))
			return false;
		if(!verify_spi(F))
			return false;
		
	}
	return true;
}

function valid_tunnelname(F)
{
	if(!check_colon(F.tunnel_name,"Tunnel Name") || !check_space(F.tunnel_name,"Tunnel Name"))
	{
		F.tunnel_name.focus();
		return false;
	}
	return true;
}

function to_submit(F)
{
	if((t_enable == 1) || (F.tunnel_status[0].checked))
	{
		if(valid_value(F))
		{
			F.submit_button.value = "ipsec";
			F.action.value = "Apply";
			F.submit();
		}
	}
	else
	{
		if(F.tunnel_name != "")
		{
			if(valid_tunnelname(F))
			{
				F.submit_button.value = "ipsec";
				F.action.value = "Apply";
				F.submit();
			}
		}
	}
}
</SCRIPT>
</HEAD>
<!--<BODY onload=init()>-->
<BODY>
<DIV align=center>
<FORM name=F1 action=apply.cgi method=<% get_http_method(); %>>
<input type=hidden name=submit_button>
<input type=hidden name=change_action>
<input type=hidden name=submit_type>
<input type=hidden name=action>
<!--input type=hidden name="lan_ipaddr" value="4"-->
<input type=hidden name="entry_config">
<input type=hidden name="gre_config">
<input type=hidden name=dev_ip0 value=<% entry_config("dev_ip",0); %> >
<input type=hidden name=dev_ip1 value=<% entry_config("dev_ip",1); %> >
<input type=hidden name=dev_ip2 value=<% entry_config("dev_ip",2); %> >
<input type=hidden name=dev_ip3 value=<% entry_config("dev_ip",3); %> >
<input type=hidden name=dev_netmask0 value=<% entry_config("dev_netmask",0); %> >
<input type=hidden name=dev_netmask1 value=<% entry_config("dev_netmask",1); %> >
<input type=hidden name=dev_netmask2 value=<% entry_config("dev_netmask",2); %> >
<input type=hidden name=dev_netmask3 value=<% entry_config("dev_netmask",3); %> >

<!--IMG 
src="image/UI_11.gif" width="1" height="1"><IMG src=""-->
<TABLE cellSpacing=0 cellPadding=0 width=809 border=0>
  <TBODY>
  <TR>
    <TD width=95><IMG src="image/UI_Linksys.gif" border=0 width="165" height="57"></TD>
    <TD vAlign=bottom align=right width=714 bgColor=#6666cc><FONT 
      style="FONT-SIZE: 7pt" face=Arial color=#ffffff><script>Capture(share.firmwarever)</script>: 
      <% get_firmware_version(); %>&nbsp;&nbsp;</FONT></TD></TR>
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
      style="FONT-SIZE: 15pt" face=Arial color=#ffffff><script>Capture(bmenu.security)</script></FONT></H3></TD>
    <TD 
    style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" 
    vAlign=center borderColor=#000000 width=646 bgColor=#000000 height=49>
      <TABLE 
      style="FONT-WEIGHT: normal; FONT-SIZE: 10pt; COLOR: black; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; BORDER-COLLAPSE: collapse; FONT-VARIANT: normal" 
      height=33 cellSpacing=0 cellPadding=0 bgColor=#6666cc border=0>
        <TBODY>
        <TR>
          <TD style="FONT-WEIGHT: bolder; FONT-SIZE: 10pt" align=right 
          bgColor=#6666cc height=33><FONT color=#ffffff><% wireless_support(1); %><script>Capture(share.wireless_g)</script><% wireless_support(2); %><script>Capture(share.productname)</script>&nbsp;&nbsp;</FONT>
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
              <TR style="BORDER-RIGHT: medium none; BORDER-TOP: medium none; FONT-WEIGHT: normal; FONT-SIZE: 1pt; BORDER-LEFT: medium none; COLOR: black; BORDER-BOTTOM: medium none; FONT-STYLE: normal; FONT-FAMILY: Arial, Helvetica, sans-serif; FONT-VARIANT: normal" align=middle bgColor=#6666cc>
                <TD width=83  height=1><IMG height=10 src="image/UI_06.gif" width=83  border=0></TD>
                <TD width=83  height=1><IMG height=10 src="<% wireless_support(3); %>" width=83  border=0></TD>
                <TD width=103 height=1><IMG height=10 src="<% wireless_support(4); %>" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_06.gif" width=103 border=0></TD>
                <TD width=68  height=1><IMG height=10 src="image/UI_06.gif" width=68  border=0></TD></TR>

              <TR>
                <TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><A style="TEXT-DECORATION: none" href="index.asp"><script>Capture(bmenu.setup)</script></A></FONT></TD><% wireless_support(1); %>
                <TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><A style="TEXT-DECORATION: none" href="Wireless_Basic.asp"><script>Capture(bmenu.wireless)</script></A></FONT></TD><% wireless_support(2); %>
                <TD align=middle bgColor=#6666cc height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><A style="TEXT-DECORATION: none" href="Firewall.asp"><script>Capture(bmenu.security)</script></A></FONT></TD>
                <TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><A style="TEXT-DECORATION: none" href="<% support_elsematch("PARENTAL_CONTROL_SUPPORT", "1", "Parental_Control.asp", "Filters.asp"); %>"><script>Capture(bmenu.accrestriction)</script></A></FONT></TD>
                <TD align=middle bgColor=#000000 height=20><P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" color=#ffffff><A style="TEXT-DECORATION: none" href="Forward_UPnP.asp"><script>Capture(bmenu.application)</script><BR>&amp; <script>Capture(bmenu.gaming)</script></A>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>
                <TD align=middle bgColor=#000000 height=20><P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" color=#ffffff><A style="TEXT-DECORATION: none" href="Management.asp">&nbsp;&nbsp;<script>Capture(bmenu.admin)</script></A>&nbsp;&nbsp;</FONT></P></TD>
                <TD align=middle bgColor=#000000 height=20><P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" color=#ffffff><A style="TEXT-DECORATION: none" href="Status_Router.asp"><script>Capture(bmenu.statu)</script></A>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD></TR>
              <TR>
                <TD width=643 bgColor=#6666cc colSpan=7 height=21>
                  <!--TABLE height=21 cellSpacing=0 cellPadding=0 border=0 width="281"-->
		  <TABLE borderColor=black height=21 cellSpacing=0 cellPadding=0>
                    <TBODY>

                    <TR align=center>
                      <TD width=20></TD>
                      <TD width=61 class=small><A href="Firewall.asp"><script>Capture(sectopmenu.firewall)</script></A></TD>
                        <TD width=4><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD width=33></TD><!--<TD width=100><FONT style='COLOR: white'>VPN</FONT></TD>-->
                      <TD width=49 class=small><FONT style='COLOR: white'><script>Capture(sectopmenu.vpn)</script></FONT></TD>
		      <TD width=1></TD>
		      <TD class=small width=60></TD>
<% support_invmatch("GRE_SUPPORT", "1", "<!--"); %>
                      <TD width=4><font color='white'><b>|</b></font></TD>
                      <TD class=small width=26></TD>
                      <TD class=small width=72><% gre_config("ipsec_gre_link", ""); %></TD>
<% support_invmatch("GRE_SUPPORT", "1", "-->"); %>
</TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>
<!--TABLE height=5 cellSpacing=0 cellPadding=0 width=806 bgColor=black border=0>
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
      -->
<!--TABLE height=5 cellSpacing=0 cellPadding=0 width=806 bgColor=black border=0>
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
  -->    
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
          <TD width=156 bgColor=#000000 height=25>
            <P align=right><B><FONT style="FONT-SIZE: 9pt" face=Arial 
            color=#ffffff><script>Capture(secleftmenu.vpnpass)</script></FONT></B></P></TD>
          <TD width=8 bgColor=#000000 height=25>&nbsp;</TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=110 height=25>&nbsp;</TD>
          <TD width=287 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 height=25><IMG height=30 src="image/UI_05.gif" 
            width=15 border=0></TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=30 src="image/UI_04.gif" width=8 border=0></TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><script>Capture(secvpnpass.ipsecpass)</script>:</TD>
          <TD height=25>
            <TABLE id=AutoNumber12 cellSpacing=0 cellPadding=0 width=242 
            border=0>
              <TBODY>
              <TR>
                <TD width=27 height=25><INPUT type=radio value=1 
                  name=ipsec_pass <% nvram_match("ipsec_pass","1","checked"); %>></TD>
                <TD width=54 height=25><B><FONT style="FONT-SIZE: 8pt" 
                  face=Arial><script>Capture(share.enabled)</script></FONT></B></TD>
                <TD width=24 height=25><INPUT type=radio value=0 
                  name=ipsec_pass <% nvram_match("ipsec_pass","0","checked"); %>></TD>
                <TD width=137 height=25><B><FONT style="FONT-SIZE: 8pt" 
                  face=Arial size=2><script>Capture(share.disabled)</script></FONT></B></TD></TR></TBODY></TABLE></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 height=25><IMG height=30 src="image/UI_05.gif" 
            width=15 border=0></TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=30 src="image/UI_04.gif" width=8 border=0></TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><script>Capture(secvpnpass.pppoepass)</script>:</TD>
          <TD height=25>
            <TABLE id=AutoNumber12 cellSpacing=0 cellPadding=0 width=242 
            border=0>
              <TBODY>
              <TR>
                <TD width=27 height=25><INPUT type=radio value=1 
                  name=pppoe_pass <% nvram_match("pppoe_pass","1","checked"); %>></TD>
                <TD width=54 height=25><B><FONT style="FONT-SIZE: 8pt" 
                  face=Arial><script>Capture(share.enabled)</script></FONT></B></TD>
                <TD width=24 height=25><INPUT type=radio value=0 
                  name=pppoe_pass <% nvram_match("pppoe_pass","0","checked"); %>></TD>
                <TD width=137 height=25><B><FONT style="FONT-SIZE: 8pt" 
                  face=Arial size=2><script>Capture(share.disabled)</script></FONT></B></TD></TR></TBODY></TABLE></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 height=25><IMG height=30 src="image/UI_05.gif" 
            width=15 border=0></TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=30 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><script>Capture(secvpnpass.pptppass)</script>:</TD>
          <TD height=25>
            <TABLE id=AutoNumber12 cellSpacing=0 cellPadding=0 width=242 
            border=0>
              <TBODY>
              <TR>
                <TD width=27 height=25><INPUT type=radio value=1 
                  name=pptp_pass <% nvram_match("pptp_pass","1","checked"); %>></TD>
                <TD width=54 height=25><B><FONT style="FONT-SIZE: 8pt" 
                  face=Arial><script>Capture(share.enabled)</script></FONT></B></TD>
                <TD width=24 height=25><INPUT type=radio value=0 
                  name=pptp_pass <% nvram_match("pptp_pass","0","checked"); %>></TD>
                <TD width=137 height=25><B><FONT style="FONT-SIZE: 8pt" 
                  face=Arial size=2><script>Capture(share.disabled)</script></FONT></B></TD></TR></TBODY></TABLE></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 height=25><IMG height=30 src="image/UI_05.gif" 
            width=15 border=0></TD></TR>
        <tr>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=30 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD colSpan=3 height=25>&nbsp;</TD>
          <TD height=25><script>Capture(secvpnpass.l2tppass)</script>:</TD>
          <TD height=25>
            <TABLE id=AutoNumber12 cellSpacing=0 cellPadding=0 width=242 
            border=0>
              <TR>
                <TD width=27 height=25><INPUT type=radio value=1 
                  name=l2tp_pass <% nvram_match("l2tp_pass","1","checked"); %>></TD>
                <TD width=54 height=25><B><FONT style="FONT-SIZE: 8pt" 
                  face=Arial><script>Capture(share.enabled)</script></FONT></B></TD>
                <TD width=24 height=25><INPUT type=radio value=0 
                  name=l2tp_pass <% nvram_match("l2tp_pass","0","checked"); %>></TD>
                <TD width=137 height=25><B><FONT style="FONT-SIZE: 8pt" 
                  face=Arial size=2><script>Capture(share.disabled)</script></FONT></B></TD></TR></TABLE></TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 height=25><IMG height=30 
            src="image/UI_05.gif" width=15 border=0></TD>
        </tr>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=1>&nbsp;</TD>
          <TD width=8 height=1><IMG height=30 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD height=25>&nbsp;</TD>
          <TD height=25>&nbsp;</TD>
          <TD colSpan=3>
            <HR color=#b5b5e6 SIZE=1>
          </TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 height=1><IMG height=30 src="image/UI_05.gif" 
            width=15 border=0></TD></TR>
        <TR>
          <TD width=156 bgColor=#000000 height=25>
            <P align=right><B><FONT style="FONT-SIZE: 9pt" face=Arial 
            color=#ffffff><script>Capture(secleftmenu.ipsecvpntunnel)</script></FONT></B></P></TD>
          <TD width=8 bgColor=#000000 height=25>&nbsp;</TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=110 height=25>&nbsp;</TD>
          <TD width=287 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 height=25><IMG height=25 src="image/UI_05.gif" 
            width=15 border=0></TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=30 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD><script>Capture(secipsectunnel.selentry)</script>:</TD>
          <TD>&nbsp;<SELECT onChange=SelTunnel(this.form.tunnel_entry.selectedIndex,this.form) name=tunnel_entry>
          <% tunnel_select("select"); %>
          </SELECT>
          </TD>
          <TD width=13>&nbsp;</TD>
          <TD width=15><IMG height=30 src="image/UI_05.gif" width=15 
            border=0></TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=25 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD>&nbsp;</TD>
          <TD>&nbsp;<!-- <INPUT onclick=to_delete(this.form) type=button value=Delete> --><script>document.write("<INPUT onclick=to_delete(this.form) type=button value='" + share.deletes + "'>");</script>&nbsp;&nbsp;&nbsp;&nbsp;<!-- <INPUT onclick="{self.open('ipsec_summary.asp','ipsecSummary','alwaysRaised,resizable,scrollbars,width=650,height=400');}" type=button value=Summary> --><script>document.write("<INPUT onclick=\"{self.open('ipsec_summary.asp','ipsecSummary','alwaysRaised,resizable,scrollbars,width=650,height=400');}\" type=button value='" + share.summary + "'>");</script></TD>
          <TD width=13>&nbsp;</TD>
          <TD width=15><IMG height=25 src="image/UI_05.gif" width=15 
            border=0></TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=25 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD><script>Capture(secipsectunnel.ipsectunnel)</script>:</TD>
          <TD><INPUT type=radio onclick=tunnel_enable() value=1 name=tunnel_status <% entry_config("tunnel_status",1); %>>      
            <B><script>Capture(share.enabled)</script></B>&nbsp;&nbsp;&nbsp;&nbsp;            
			<INPUT type=radio onclick=tunnel_disable() value=0 name=tunnel_status <% entry_config("tunnel_status",0);%>><B><script>Capture(share.disabled)</script></B></TD>
          <TD width=13>&nbsp;</TD>
          <TD width=15><IMG height=25 src="image/UI_05.gif" width=15 
            border=0></TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=25>&nbsp;</TD>
          <TD width=8 height=25><IMG height=25 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD><script>Capture(secipsectunnel.tunnelname)</script>:</TD>
          <TD>&nbsp;<INPUT maxLength=15 size=20 name=tunnel_name value='<% entry_config("tunnel_name",""); %>'></TD>
          <TD width=13>&nbsp;</TD>
          <TD width=15><IMG height=25 src="image/UI_05.gif" width=15 
            border=0></TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=1>&nbsp;</TD>
          <TD width=8 height=1><IMG height=25 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD height=25>&nbsp;</TD>
          <TD height=25>&nbsp;</TD>
          <TD colSpan=3>
            <HR color=#b5b5e6 SIZE=1>
          </TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 height=1><IMG height=25 src="image/UI_05.gif" 
            width=15 border=0></TD></TR>
        <tr>
          <TD width=156 align=right bgColor=#e7e7e7 height=25><B><script>Capture(secleftmenu.localsecgrp)</script></B></TD>
          <TD width=8 height=25><IMG height=30 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD>&nbsp;</TD>
          <TD>&nbsp;
          <SELECT onchange=SelLTYPE(this.form.local_type.selectedIndex,this.form) name=local_type >
            <OPTION value=0  <% entry_config("local_type", 0); %>><script>Capture(share.ipaddr)</script></OPTION>
            <OPTION value=1  <% entry_config("local_type", 1); %>><script>Capture(share.subnet)</script></OPTION>
          </SELECT>
          </TD>
          <TD width=13>&nbsp;</TD>
          <TD width=15><IMG height=30 src="image/UI_05.gif" width=15 
            border=0></TD>
        </tr>
<% show_ltype_setting(); %>
        <!--tr>
          <TD width=156 bgColor=#e7e7e7 height=1>&nbsp;</TD>
          <TD width=8 height=1><IMG height=25 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD height=25>&nbsp;</TD>
          <TD height=25>&nbsp;</TD>
          <TD colSpan=3>
            <HR color=#b5b5e6 SIZE=1>
          </TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 height=1><IMG height=25 src="image/UI_05.gif" 
            width=15 border=0></TD>
        </tr-->
                <tr>
          <TD width=156 align=right bgColor=#e7e7e7 height=25><B><script>Capture(secleftmenu.localsecgw)</script></B></TD>
          <TD width=8 height=25><IMG height=30 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD>&nbsp;</TD>
          <TD>&nbsp;
	<SELECT onChange=SelLgw(this.form.localgw_conn.selectedIndex,this.form) name=localgw_conn>
<% localgw_select("select"); %>
          </SELECT>
          </TD>
          <TD width=13>&nbsp;</TD>
          <TD width=15><IMG height=30 src="image/UI_05.gif" width=15 
            border=0></TD>
        </tr>
<TR>
          <TD width=156 bgColor=#e7e7e7 height=1>&nbsp;</TD>
          <TD width=8 height=1><IMG height=25 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD height=25>&nbsp;</TD>
          <TD height=25>&nbsp;</TD>
          <TD colSpan=3>
            <HR color=#b5b5e6 SIZE=1>
          </TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 height=1><IMG height=25 src="image/UI_05.gif" 
            width=15 border=0></TD></TR>

        <TR>
          <TD width=156 align=right bgColor=#e7e7e7 height=25><B><script>Capture(secleftmenu.remotesecgrp)</script></B></TD>
          <TD width=8 height=25><IMG height=30 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD>&nbsp;</TD>
          <TD>&nbsp;<SELECT onchange=SelRTYPE(this.form.remote_type.selectedIndex,this.form) 
            name=remote_type>
            <OPTION value=0 <% entry_config("remote_type", 0); %>><script>Capture(share.ipaddr)</script></OPTION>
            <OPTION value=1 <% entry_config("remote_type", 1); %>><script>Capture(share.subnet)</script></OPTION>
            <!--<OPTION value=2 <% entry_config("remote_type", 2); %>>IP Range</OPTION>-->
            <OPTION value=4 <% entry_config("remote_type", 4); %>><script>Capture(share.hosts)</script></OPTION>
			<OPTION value=3 <% entry_config("remote_type", 3); %>><script>Capture(share.any)</script></OPTION>
			
			</SELECT></TD>
          <TD width=13>&nbsp;</TD>
          <TD width=15><IMG height=30 src="image/UI_05.gif" width=15 
            border=0></TD></TR>
           
<% show_rtype_setting(); %>
<!--
        <TR>
          <TD align=right bgColor=#e7e7e7 height=25><B><script>Capture(secleftmenu.remote)</script></B></TD>
          <TD width=8 height=25><IMG height=25 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD></TD>
          <TD>&nbsp;</TD>
          <TD width=13>&nbsp;</TD>
          <TD width=15><IMG height=25 src="image/UI_05.gif" width=15 border=0></TD></TR>
    -->        
        <TR>
          <TD width=156 align=right bgColor=#e7e7e7 height=25><B><script>Capture(secleftmenu.remotesecgw)</script></B></TD>
          <TD width=8 height=25><IMG height=30 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD></TD>
          <TD>&nbsp;
          <SELECT onchange=SelSGTYPE(this.form.sg_type.selectedIndex,this.form)  name=sg_type>
            <OPTION value=0 <% entry_config("sg_type", 0); %>><script>Capture(share.ipaddr)</script></OPTION>
            <OPTION value=2 <% entry_config("sg_type", 2); %>><script>Capture(share.fqdn)</script></OPTION>
            <OPTION value=1 <% entry_config("sg_type", 1); %>><script>Capture(share.any)</script></OPTION>
          </SELECT>
          </TD>
          <TD width=13>&nbsp;</TD>
          <TD width=15><IMG height=30 src="image/UI_05.gif" width=15 
            border=0></TD></TR>

 <% show_sg_setting(); %>
      <TR>
          <TD width=156 bgColor=#e7e7e7 height=1>&nbsp;</TD>
          <TD width=8 height=1><IMG height=25 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD height=25>&nbsp;</TD>
          <TD height=25>&nbsp;</TD>
          <TD colSpan=3>
            <HR color=#b5b5e6 SIZE=1>
          </TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 height=1><IMG height=25 src="image/UI_05.gif" 
            width=15 border=0></TD></TR>

      
        <TR>
          <TD width=156 align=right bgColor=#e7e7e7 height=25><B></B></TD>
          <TD width=8 height=25><IMG height=25 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD><script>Capture(secipsectunnel.encryption)</script>:&nbsp;</TD>
          <TD>&nbsp;
          <SELECT name=enc_type>
          <OPTION value=1 <% entry_config("enc_type", 1); %>><script>Capture(share.des)</script></OPTION>
          <OPTION value=2 <% entry_config("enc_type", 2); %>>3<script>Capture(share.des)</script></OPTION>
          <!--OPTION value=3 <% entry_config("enc_type", 3); %>><script>Capture(share.aes)</script></OPTION-->
          <OPTION value=0 <% entry_config("enc_type", 0); %>><script>Capture(share.disabled)</script></OPTION>
          </SELECT>
          </TD>
          <TD width=13>&nbsp;</TD>
          <TD width=15><IMG height=25 src="image/UI_05.gif" width=15 
            border=0></TD></TR>
        <TR>
          <TD width=156 align=right bgColor=#e7e7e7 height=25><B></B></TD>
          <TD width=8 height=25><IMG height=25 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD><script>Capture(secipsectunnel.auth)</script>:&nbsp;</TD>
          <TD>&nbsp;
          <SELECT name=auth_type>
          <OPTION value=1 <% entry_config("auth_type", 1); %>><script>Capture(admsnmp.md5)</script></OPTION>
          <OPTION value=2 <% entry_config("auth_type", 2); %>><script>Capture(admsnmp.sha)</script></OPTION>
          <!-- OPTION value=0 <% entry_config("auth_type", 0); %>><script>Capture(share.disabled)</script></OPTION-->
          </SELECT>
          </TD>
          <TD width=13>&nbsp;</TD>
          <TD width=15><IMG height=25 src="image/UI_05.gif" width=15 
            border=0></TD></TR>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=1>&nbsp;</TD>
          <TD width=8 height=1><IMG height=25 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD height=25>&nbsp;</TD>
          <TD height=25>&nbsp;</TD>
          <TD colSpan=3>
            <HR color=#b5b5e6 SIZE=1>
          </TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 height=1><IMG height=25 src="image/UI_05.gif" 
            width=15 border=0></TD></TR>
        <TR>
          <TD align=right bgColor=#e7e7e7 height=25><B><script>Capture(secleftmenu.keymgnt)</script></B></TD>
          <TD width=8 height=25><IMG height=25 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD>&nbsp;</TD>
          <TD>&nbsp;
          <SELECT onchange=SelKEYTYPE(this.form.key_type.selectedIndex,this.form) name=key_type>
          <OPTION value=0 <% entry_config("key_type", 0); %>><script>Capture(share.auto)</script>.(IKE)</OPTION>
          <OPTION value=1 <% entry_config("key_type", 1); %>><script>Capture(share.manual)</script></OPTION>
          </SELECT>
          </TD>
          <TD width=13>&nbsp;</TD>
          <TD width=15><IMG height=25 src="image/UI_05.gif" width=15 
            border=0></TD></TR>
<% show_keytype_setting(); %>
            
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=1>&nbsp;</TD>
          <TD width=8 height=1><IMG height=25 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD height=25>&nbsp;</TD>
          <TD height=25>&nbsp;</TD>
          <TD colSpan=3>
            <HR color=#b5b5e6 SIZE=1>
          </TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 height=1><IMG height=25 src="image/UI_05.gif" 
            width=15 border=0></TD></TR>
        <TR>
          <TD width=156 align=right bgColor=#e7e7e7 height=25><B><script>Capture(bmenu.statu)</script></B></TD>
          <TD width=8 height=25><IMG height=25 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD colspan = "2"><B><% entry_config("tstatus",10); %></B></TD>
          <TD width=13>&nbsp;</TD>
          <TD width=15><IMG height=25 src="image/UI_05.gif" width=15 
            border=0></TD></TR>
        <!--TR>
          <TD width=156 bgColor=#e7e7e7 height=1>&nbsp;</TD>
          <TD width=8 height=1><IMG height=25 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD height=25>&nbsp;</TD>
          <TD height=25>&nbsp;</TD>
          <TD colSpan=3>
            <HR color=#b5b5e6 SIZE=1>
          </TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD width=15 height=1><IMG height=25 src="image/UI_05.gif" 
            width=15 border=0></TD></TR>
        <TR>
          <TD width=156 align=right bgColor=#e7e7e7 height=25><B>&nbsp;</B></TD>
          <TD width=8 height=25><IMG height=25 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD width=14 height=25>&nbsp;</TD>
          <TD width=17 height=25>&nbsp;</TD>
          <TD width=13 height=25>&nbsp;</TD>
          <TD align=middle colSpan=2>&nbsp;<INPUT type=hidden value=1 
	    name=hid_ipSecConnect><INPUT onclick=Connect(this.form,1) type=button value=" Connect "><INPUT
            type=hidden value=0 name=hid_logviewButtonCheck><INPUT type=hidden
            value=65520 name=hid_logviewTypeButtonCheck><INPUT onclick=LogButton_check(this.form,0) type=button value="View Logs"><INPUT onclick="{self.open('IPSecAdvance.asp','IPSecAdvance','alwaysRaised,resizable,scrollbars,width=830,height=500',tunnel_value);}" type=button value="Advanced Settings"></TD>
          <TD width=13>&nbsp;</TD>
          <TD width=15><IMG height=25 src="image/UI_05.gif" width=15 
            border=0></TD></TR>
        -->
        <% show_connect_setting(); %>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=5>&nbsp;</TD>
          <TD width=8 height=5><IMG height=25 src="image/UI_04.gif" 
            width=8 border=0></TD>
          <TD width=454 colSpan=6 height=5></TD>
          <TD width=15 height=5><IMG height=25 src="image/UI_05.gif" 
            width=15 border=0></TD></TR></TBODY></TABLE></TD>
    <TD vAlign=top width=176 bgColor=#6666cc>
      <TABLE cellSpacing=0 cellPadding=0 width=176 border=0>
        <TBODY>
        <TR>
          <TD width=11 bgColor=#6666cc height=25>&nbsp;</TD>
          <TD width=156 bgColor=#6666cc height=25><FONT color=#ffffff><a target="_blank" href="help/HVPNSecurity.asp"><script>Capture(share.more)</script>...</a></FONT></TD>
          <TD width=9 bgColor=#6666cc 
  height=25>&nbsp;</TD></TR></TBODY></TABLE></TD></TR>
  <TR>
    <TD width=809 colSpan=2>
      <TABLE cellSpacing=0 cellPadding=0 border=0>
        <TBODY>
        <TR>
          <TD width=156 bgColor=#e7e7e7 height=30>&nbsp;</TD>
          <TD width=8><IMG height=30 src="image/UI_04.gif" width=8 
            border=0></TD>
          <TD width=16>&nbsp;</TD>
          <TD width=12>&nbsp;</TD>
          <TD width=411>&nbsp;</TD>
           <TD width=15>&nbsp;</TD>
          <!--TD width=15-->
          <TD width=15><IMG height=30 src="image/UI_05.gif" width=15 
            border=0></TD>
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
                <TD align=middle width=101 bgColor=#42498c><FONT color=#ffffff><B><A href='javascript:to_submit(document.F1);'><script>Capture(share.saves)</script></A></B></FONT>
<!--INPUT type=button onclick=to_submit(this.form) value="Save Settings"--></TD>
                <TD align=middle width=8 bgColor=#6666cc>&nbsp;</TD>
                <TD align=middle width=103 bgColor=#434a8f><FONT color=#ffffff><B><A href="ipsec.asp"><script>Capture(share.cancels)</script></A></B></FONT>
<!--INPUT type=button value="Cancel Changes" onclick=window.location.reload()--></TD></TR></TBODY></TABLE></TD>
          <TD width=15 bgColor=#6666cc height=33>&nbsp;</TD>
          <TD width=15 bgColor=#000000 
  height=33>&nbsp;</TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></FORM></DIV></BODY></HTML>
