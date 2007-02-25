
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

<HTML><HEAD><TITLE>DHCP Reserved IP List</TITLE>
<% no_cache(); %>
<META http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<!--<META http-equiv=Content-Type content="text/html; charset=unicode">-->

<link rel="stylesheet" type="text/css" href="style.css">
<style fprolloverstyle>
A:hover {color: #00FFFF}
.small A:hover {color: #00FFFF}
</style>
<script src="common.js"></script>
<script src="common.js"></script>
<script language="JavaScript" type="text/javascript" src="share.js"></script>

<SCRIPT language=JavaScript>
document.title=(share.dhcpreslist);
function check_repeat_value(F,N,num,I)
{
	if ((I.value == "00:00:00:00:00:00") || (I.value == "000000000000"))
	{
		alert("The mac value can not be zero !");
		//alert(errmsg.err63);

		I.value = I.defaultValue;
		return false;
	}
	for (i = 0; i < num; i ++)
	{
		val = eval(N+i);
		if (I == val) continue;
		if (I.value == val.value) 
		{
			//alert("The value existed, please try another one !");
			alert(errmsg.err64);
			I.value = I.defaultValue;
			return false;
		}
	}
	return true;
}

function valid_host_name(F,I,check_box)
{
	if(I.value != "")
	{
		total_num = 0;
		len = I.value.length - 1;
		while(1)
		{
			ch = I.value.charAt(len);
			if (ch == ' ') len --;
			else break;
		}
		for(i = 0 ; i <= len; i ++)
		{
			ch = I.value.charAt(i);
			if ((ch == '_') || (ch == '-')) continue;
			if (((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (_ch <= 'Z'))) continue;
			
			if (!((ch >= '0') && (ch <= '9')))
			{
			//	alert('Standard name may include char(a-z, A-Z);digital(0-9)and symbol (-,_)');
				alert(errmsg.err65);

				I.value = I.defaultValue;
				check_box.checked = 0;
				return false;
			}else total_num ++;

		}
		if (total_num >= I.value.length)
		{
				//alert('Standard name must not be all digitals !');
				alert(errmsg.err66);

				I.value = I.defaultValue;
				check_box.checked = 0;
				return false;
		}
		if (!check_repeat_value(F,"F.dhcp_host",10,I))
    	{
    		check_box.checked = 0;
    		return false;
    	}
	}else {
		check_box.checked = 0;
		I.defaultValue = "";
	}
	return true;
}
function valid_check(mac_host, ip, I)
{
	if ((mac_host.value == "") || (ip.value == ""))
	{
		//alert("Can not be checked !");
		alert(errmsg.err67);

		I.checked = 0;
		return false;
	}
	return true;
}
function valid_my_ip(F,I,check_box,start,end)
{
	if(I.value == "")
	{
		if (check_box.checked) check_box.checked = 0;
		I.defaultValue = "";
		return true;
	}
	else{
		var start, end;
		var index = window.name.indexOf('_');
		if (index == -1)
		{
			start = 1;
			end = 253;
		}else {
			start = window.name.substring(0, index);
			var len = window.name.length;
			end = window.name.substring(index+1,len);
			if (start == '') start = 1;
			if (end == '') end = 253;
			else end = parseInt(end, 10) + parseInt(start, 10);
			if (parseInt(end, 10) > 253) end = 253;
		}
	
		if (valid_range(I,start,end,"IP") == false)
		{
			I.value = I.defaultValue; 
			check_box.checked = 0;
			return false;
		}
	    if (!check_repeat_value(F,"F.dhcp_mac_ip",10,I))
    	{
    		check_box.checked = 0;
    		return false;
    	}
	    if (!check_repeat_value(F,"F.dhcp_host_ip",10,I))
    	{
    		check_box.checked = 0;
    		
    		return false;
    	}
	}
	return true;
}

function valid_sharewanip_mac(F,I)
{
	if(I.value == "")
	{
		I.defaultValue = "";
		return true;
	}
	else if(I.value.length == 12)
		valid_macs_12(I);
	else if(I.value.length == 17)
		valid_macs_17(I);
	else{
		//alert('The MAC Address length is not correct!!');
		alert(errmsg.err68);

		I.value = I.defaultValue;
		return false;
        }
    if (!check_repeat_value(F,"F.dhcp_mac",11,I))
    {
    	return false;
    }
}

function valid_macs_all(F,I,check_box)
{
	if(I.value == "")
	{
		check_box.checked = 0;
		I.defaultValue = "";
		return true;
	}
	else if(I.value.length == 12)
		valid_macs_12(I);
	else if(I.value.length == 17)
		valid_macs_17(I);
	else{
		//alert('The MAC Address length is not correct!!');
	`	alert(errmsg.err68);
		I.value = I.defaultValue;
		check_box.checked = 0;
		return false;
        }
    if (!check_repeat_value(F,"F.dhcp_mac",11,I))
    {
    	check_box.checked = 0;
    	return false;
    }
}
<% support_elsematch("STB_SUPPORT", "1", "", "/*"); %>	
function stb_enable_switch(F) 
{
	if(!F.stb_enable.checked)
	{
		choose_disable(F.stbvalue_0);
		choose_disable(F.stbvalue_1);
		choose_disable(F.stbvalue_2);
		choose_disable(F.stbvalue_3);
		choose_disable(F.enable_0);
		choose_disable(F.enable_1);
		choose_disable(F.enable_2);
		choose_disable(F.enable_3);
		choose_disable(F.telus_ipaddr_0);
		choose_disable(F.telus_ipaddr_1);
		choose_disable(F.telus_ipaddr_2);
		choose_disable(F.telus_ipaddr_3);
	}
	else
	{
		choose_enable(F.stbvalue_0);
		choose_enable(F.stbvalue_1);
		choose_enable(F.stbvalue_2);
		choose_enable(F.stbvalue_3);
		choose_enable(F.enable_0);
		choose_enable(F.enable_1);
		choose_enable(F.enable_2);
		choose_enable(F.enable_3);
		choose_enable(F.telus_ipaddr_0);
		choose_enable(F.telus_ipaddr_1);
		choose_enable(F.telus_ipaddr_2);
		choose_enable(F.telus_ipaddr_3);
	}
}

function check_spec(I,M1)
{
	M = unescape(M1);
	for(i = 0;i < I.value.length; i++)
	{
		ch = I.value.charAt(i);
		if(ch == '?')
		{
			//alert(M + 'is not allow \'?\'');
			alert(M + errmsg.err55);

			I.focus();
			return false;
		}
	}
	return true;
}

function valid_stbvalue(F)
{
	
	if(!F.stb_enable.checked)
		return true;
	if((F.enable_0.checked) && (F.stbvalue_0.value == ""))
	{
		//alert("Please enter type value");
		alert(errmsg.err69);

		F.stbvalue_0.focus();
	   	return false;
	}
	else if((F.enable_1.checked) && (F.stbvalue_1.value==""))
	{
		//alert("Please enter type value");
		alert(errmsg.err69);

		F.stbvalue_1.focus();
	   	return false;
	}
	else if((F.enable_2.checked) && (F.stbvalue_2.value==""))
	{
		//alert("Please enter type value");
		alert(errmsg.err69);

		F.stbvalue_2.focus();
	   	return false;
	}	
	else if((F.enable_3.checked) && (F.stbvalue_3.value==""))
	{
		//alert("Please enter type value");
		alert(errmsg.err69);

		F.stbvalue_3.focus();
	   	return false;
	}	
	
   if(!valid_ip(F,"F.telus_ipaddr","Telus Server IP",ZERO_NO|MASK_NO))
                        {
                                F.telus_ipaddr_0.focus();
                                return false;
                        }

			if(!valid_range(F.telus_ipaddr_0,1,223,"IP"))
			{
				F.telus_ipaddr_0.focus();
				return false;
			}
			if(!valid_range(F.telus_ipaddr_3,1,254,"IP"))
			{
				F.telus_ipaddr_3.focus();
				return false;
			}
	
	if(check_spec(F.stbvalue_0,"STB%20Type") &&
	   check_spec(F.stbvalue_1,"STB%20Type") &&
	   check_spec(F.stbvalue_2,"STB%20Type") &&
	   check_spec(F.stbvalue_3,"STB%20Type"))
		return true;
	else 
		return false;
}
<% support_elsematch("STB_SUPPORT", "1", "", "*/"); %>	


var old_share_value=-1;
function share_wan_ip(F)
{
	old_share_value = F._share_wanip.checked;
	F._share_wanip_comm.disabled = !old_share_value;
	if (old_share_value == 1)
		//alert("If this function enable, it will disable DMZ function by force!");
		alert(errmsg.err70);

	if (!F._single_pc_mode.checked)
	{
		if (F.dhcp_mac10.value == "")
		{
			//alert('Please write a MAC address!');
			alert(errmsg.err71);

			F._share_wanip.checked = 0;
			return false;
		}else if (F.share_wanip_lease.value == "")
		{
			//alert('Please write a lease time!');
			alert(errmsg.err72);

			F._share_wanip.checked = 0;
			return false;
		}
	}
}

function single_pcmode(F)
{
	if (old_share_value == -1)
		old_share_value = F._share_wanip.checked;

	if (F._single_pc_mode.checked)
	{
		F._share_wanip.checked = 1;
		F.dhcp_mac10.disabled = true;
		F.share_wanip_if.disabled = true;
		F.share_wanip_lease.disabled = true;
		F._share_wanip_comm.disabled = false;
	}else 
	{
		F.dhcp_mac10.disabled = false;
		F.share_wanip_if.disabled = false;
		F.share_wanip_lease.disabled = false;
		F._share_wanip.checked = old_share_value;
		F._share_wanip_comm.disabled = !old_share_value;

		if ((F.share_wanip_lease.value == "") || (F.dhcp_mac10.value == ""))
		{
			F._share_wanip.checked = 0;;
			F._share_wanip_comm.disabled = true;
		}
	}
}



function to_submit(F)
{
    F.submit_button.value = "dhcp_reserved";
    F.action.value = "Apply";
	//leijun 2004-0623
	if(F._share_wanip.checked) F.share_wanip.value = 1;
	else F.share_wanip.value = 0;

	if(F._single_pc_mode.checked) F.single_pc_mode.value = 1;
	else F.single_pc_mode.value = 0;

	if(F._share_wanip_comm.checked) F.share_wanip_comm.value = 1;
	else F.share_wanip_comm.value = 0;
	
	F.dhcp_mac10.disabled = false;
	F.share_wanip_if.disabled = false;
	F.share_wanip_lease.disabled = false;
<% support_elsematch("STB_SUPPORT", "1", "", "/*"); %>	
        if(valid_stbvalue(F))
<% support_elsematch("STB_SUPPORT", "1", "", "*/"); %>	


    F.submit();
}

function load_init()
{
	window.focus();
	if (document.dhcpreserved._single_pc_mode.checked)
	{
		document.dhcpreserved.dhcp_mac10.disabled = true;
		document.dhcpreserved.share_wanip_if.disabled = true;
		document.dhcpreserved.share_wanip_lease.disabled = true;
	}
	document.dhcpreserved._share_wanip_comm.disabled = !document.dhcpreserved._share_wanip.checked;
<% support_elsematch("STB_SUPPORT", "1", "", "/*"); %>	
<% dump_stb_entry(); %>	
<% support_elsematch("STB_SUPPORT", "1", "", "*/"); %>

}
</SCRIPT>
</HEAD>

<BODY bgColor=white onload=load_init()>
<FORM name=dhcpreserved method=<% get_http_method(); %> action=apply.cgi>
<input type=hidden name=submit_button>
<input type=hidden name=submit_type>
<input type=hidden name=change_action>
<input type=hidden name=action>
<input type=hidden name=start_ip>
<input type=hidden name=dhcp_reserved_ip_list>
<input type=hidden name=share_wanip>
<input type=hidden name=single_pc_mode>
<input type=hidden name=share_wanip_comm>

<% support_elsematch("STB_SUPPORT", "1", "", "<!--"); %>
<input type=hidden name=stbentry_config>
<input type=hidden name=telus_config>
<% support_elsematch("STB_SUPPORT", "1", "", "-->"); %>


<CENTER>
<TABLE cellSpacing=0 cellPadding=10 width=420 border=1 height="212">
  <TBODY>
  <TR>
    <TD width=396>
      <TABLE height=30 cellSpacing=0 cellPadding=0 border=0 width="398">
        <TBODY>
        <TR>
          <TD colSpan=5 height=39 align="center">
            <P align=center><b><font face="Arial" size="4" color="#0000FF"><script>Capture(share.dhcp)</script>
			</font><FONT face=Arial color=#0000ff size="4"><script>Capture(share.dhcpreslist)</script></FONT></b></P>
		  </TD>
		</TR>
		<TR>
		
		<TABLE style='BORDER-COLLAPSE: collapse' borderColor=silver height=25 cellSpacing=0 cellPadding=0 width=398 border=1>

			<TR>
				<TD class=headrow align=middle height=25 width=24><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 10pt'></font></B></TD>
				<TD class=headrow align=middle height=25 width=132><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 10pt'><script>Capture(share.macaddr)</script></font></B></TD>
				<TD class=headrow align=middle height=25 width=178><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 10pt'><script>Capture(share.ipaddr)</script></font></B></TD>
				<TD class=headrow align=middle height=25 width=60><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 10pt'><script>Capture(share.enable)</script></font></B></TD>
			</TR> 
			<% dhcp_reserved_ip("mac"); %>
			<TR>
				<TD class=headrow align=middle height=25 width=24><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 10pt'></font></B></TD>
				<TD class=headrow align=middle height=25 width=132><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 10pt'><script>Capture(share.hostname)</script></font></B></TD>
				<TD class=headrow align=middle height=25 width=178><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 10pt'><script>Capture(share.ipaddr)</script></font></B></TD>
				<TD class=headrow align=middle height=25 width=60><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 10pt'><script>Capture(share.enable)</script></font></B></TD>
			</TR>
			<% dhcp_reserved_ip("host"); %>
	    </TABLE>
      <TABLE height=30 cellSpacing=0 cellPadding=0 border=0 width="397">
        <TBODY>
        <TR>
          <TD height=39 align="center">
            <P align=center><b><font face="Arial" size="4" color="#0000FF"><script>Capture(share.sharewan)</script></FONT></b></P>
		  </TD>
		</TR>
		<TABLE style='BORDER-COLLAPSE: collapse' borderColor=silver height=24 cellSpacing=0 cellPadding=0 width=398 border=1>
			<TR>
				<TD class=headrow align=middle height=25 width=126><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 10pt'><script>Capture(share.macaddr)</script></font></B></TD>
				<TD class=headrow align=middle height=25 width=172><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 10pt'><script>Capture(share.wanipface)</script></font></B></TD>
				<TD class=headrow align=middle height=25 width=100><B><font style='FONT-FAMILY: Arial; FONT-SIZE: 10pt'><script>Capture(share.lease)</script></font></B></TD>
			</TR>
			<% dhcp_reserved_ip("share"); %>
<!--			<TR align=center>
	<TD width=126 height=25><FONT style="FONT-SIZE: 10pt" face=Arial><INPUT maxLength=17 size=17 name=share_wanip_mac value="" onblur=valid_sharewanip_mac(this.form,this) class=num></TD>
	<TD width=172 height=25><FONT style="FONT-SIZE: 10pt" face=Arial><SELECT name=share_wanip_if onChange="">
		<option value="255" selected>Auto Search</option>
</SELECT></TD>
	<TD width=100 height=25><FONT style="FONT-SIZE: 10pt" face=Arial><INPUT maxLength=4 size=4 name=share_wanip_lease value=10 onBlur=valid_range(this,0,9999,"Share-WAN-IP-Lease-Time") class=num>&nbsp;minutes</TD></TR>
-->
		</TABLE>
		<tr><TD><font style='FONT-FAMILY: Arial; FONT-SIZE: 10pt'>
			&nbsp;<input type=checkbox value=1 onclick=share_wan_ip(this.form) name=_share_wanip <% nvram_match("share_wanip","1","checked"); %> > <script>Capture(share.sharewan)</script> &nbsp;&nbsp;&nbsp;<font style='FONT-FAMILY: Arial; FONT-SIZE: 10pt'><input type=checkbox value=1 name=_share_wanip_comm <% nvram_match("share_wanip_comm","1","checked"); %> ><script>Capture(share.communicate)</script></font>&nbsp;&nbsp;&nbsp;&nbsp;
 		<p>&nbsp;<input type=checkbox value=1 onclick=single_pcmode(this.form) name=_single_pc_mode <% nvram_match("single_pc_mode","1","checked"); %> > <script>Capture(share.singlemode)</script>
 		</TD></tr>
		</TR>
		</TBODY>
      </TABLE>
    </TD></TR></TBODY></TABLE>
 <% support_elsematch("STB_SUPPORT", "1", "", "<!--"); %>
      <TABLE height=30 cellSpacing=0 cellPadding=0 border=0 width="398">

        <TBODY>

        <TR>

          <TD height=39 align="center">

            <P align=center><b><font face="Arial" size="4" color="#0000FF"><script>Capture(share.stbtype)</script></font></b></P>

		  </TD>

		</TR>

				
		<TABLE style='BORDER-COLLAPSE: collapse' borderColor=silver height=25 cellSpacing=0 cellPadding=0 width=398 border=1>



			<tr>

				<TD class=headrow align=middle height=25 width=50>
				<font size="2"><b><span style="font-family: Arial"><script>Capture(share.indexs)</script></span></b></font></TD>

				<TD class=headrow align=middle height=25 width=277>
				<font size="2"><b><span style="font-family: Arial"><script>Capture(share.matchtype)</script></span></b></font></TD>

				<TD class=headrow align=middle height=25 width=66>
				<font size="2"><b><span style="font-family: Arial"><script>Capture(share.selects)</script></span></b></font></TD>

			</tr>



			<TR>

				<TD class=headrow align=middle height=25 width=50>1</TD>

				<TD class=headrow align=middle height=25 width=277><FONT size=2>

				<INPUT maxLength=50 size=50 name=stbvalue_0></FONT></TD>

				<TD class=headrow align=middle height=25 width=66>

                <INPUT type=checkbox name=enable_0 value=1></TD>

			</TR> 



			<TR>

				<TD class=headrow align=middle height=25 width=50>2</TD>

				<TD class=headrow align=middle height=25 width=277><FONT size=2>

				<INPUT maxLength=50 size=50 name=stbvalue_1></FONT></TD>

				<TD class=headrow align=middle height=25 width=66>

                <INPUT type=checkbox name=enable_1 value=1></TD>

			</TR> 



			<TR>

				<TD class=headrow align=middle height=25 width=50>3</TD>

				<TD class=headrow align=middle height=25 width=277><FONT size=2>

				<INPUT maxLength=50 size=50 name=stbvalue_2></FONT></TD>

				<TD class=headrow align=middle height=25 width=66>

                <INPUT type=checkbox name=enable_2 value=1></TD>

			</TR> 



			<TR>

				<TD class=headrow align=middle height=25 width=50>4</TD>

				<TD class=headrow align=middle height=25 width=277><FONT size=2>

				<INPUT maxLength=50 size=50 name=stbvalue_3></FONT></TD>

				<TD class=headrow align=middle height=25 width=66>

                <INPUT type=checkbox name=enable_3 value=1></TD>

			</TR> 

			</TABLE>
			<p align="center"><FONT style="FONT-SIZE: 8pt" 
            face=Arial><b><script>Capture(share.telus)</script></b>:<input type=hidden name="telus_ipaddr" value=4>
            <INPUT class=num maxLength=3 size=3 value='<% telus_config("ipaddr", 0); %>' name="telus_ipaddr_0" onBlur=valid_range(this,1,223,"IP") <% telus_config("ipaddr", 4); %>> .
		<INPUT class=num maxLength=3 size=3 value='<% telus_config("ipaddr", 1); %>' name="telus_ipaddr_1" onBlur=valid_range(this,0,255,"IP") <% telus_config("ipaddr", 4); %>> .
		<INPUT class=num maxLength=3 size=3 value='<% telus_config("ipaddr", 2); %>' name="telus_ipaddr_2" onBlur=valid_range(this,0,255,"IP") <% telus_config("ipaddr", 4); %>> .
		<INPUT class=num maxLength=3 size=3 value='<% telus_config("ipaddr", 3); %>' name="telus_ipaddr_3" onBlur=valid_range(this,1,254,"IP") <% telus_config("ipaddr", 4); %>></FONT>
			<p align="center"><font style='FONT-FAMILY: Arial; FONT-SIZE: 10pt'><input type=checkbox value=1 onclick=stb_enable_switch(this.form) name=stb_enable <% nvram_match("stb_enable","1","checked"); %> > 
			</font><font size="2"><span style="font-family: Arial"><script>Capture(share.stbenable)</script></span></font>
        <p align="center">          

&nbsp;
		</TBODY>

      </TABLE>

    </TD></TR></TBODY></TABLE>
<% support_elsematch("STB_SUPPORT", "1", "", "-->"); %>   
<P><INPUT type=button value="Save Settings" onClick=to_submit(this.form)>&nbsp; <INPUT type=button value="Cancel Changes" name=cancel onClick="javascript:window.location.href='dhcp_reserved.asp'"></CENTER></P>
</FORM>
</BODY>
</HTML>
