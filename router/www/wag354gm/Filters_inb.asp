
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

<html><head><title>Access Restriction</title>
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
document.title=(bmenu.accrestriction1);
<% filter_inb_init(); %>
var summary_win_inb = null;
var ipmac_win = null;
function dayall(F){
	if(F.day_all_inb.checked == false)
		I = 1;
	else
		I = 0;
	day_enable_disable(F,I);
}
function day_enable_disable(F,I){
        if(I == 1){
		choose_enable(F.week0_inb);
		choose_enable(F.week1_inb);
		choose_enable(F.week2_inb);
		choose_enable(F.week3_inb);
		choose_enable(F.week4_inb);
		choose_enable(F.week5_inb);
		choose_enable(F.week6_inb);
	}
	else if(I == 0){
		choose_disable(F.week0_inb);
		choose_disable(F.week1_inb);
		choose_disable(F.week2_inb);
		choose_disable(F.week3_inb);
		choose_disable(F.week4_inb);
		choose_disable(F.week5_inb);
		choose_disable(F.week6_inb);
	}
}
function timeall(F,I){
	time_enable_disable(F,I);
}
function time_enable_disable(F,I){
	if(I == 1){
		choose_enable(F.start_hour_inb);
		choose_enable(F.start_min_inb);
		choose_enable(F.start_time_inb);
		choose_enable(F.end_hour_inb);
		choose_enable(F.end_min_inb);
		choose_enable(F.end_time_inb);
	}
	else if(I == 0){
		choose_disable(F.start_hour_inb);
		choose_disable(F.start_min_inb);
		choose_disable(F.start_time_inb);
		choose_disable(F.end_hour_inb);
		choose_disable(F.end_min_inb);
		choose_disable(F.end_time_inb);
	}
}
function ViewSummary(){
	summary_win_inb = self.open('Filter_Inb_Summary.asp','Filter_Inb_Summary','alwaysRaised,resizable,scrollbars,width=700,height=420');
	summary_win_inb.focus();
}
function to_submit(F)
{
	if(valid(F) == true){
		F.submit_type.value = "save";
  	     	F.submit_button.value = "Filters_inb";
       		F.action.value = "Apply";
       		F.submit();
	}
}
function to_save(F)
{
	if(valid(F) == true){
      	 	F.submit_button.value = "Filters_inb";
       		F.change_action.value = "gozila_cgi";
		F.submit_type.value = "save";
       		F.action.value = "Apply";
       		F.submit();
	}
}
function to_delete(F)
{
	if(confirm("Delete the Policy?")){
	   	F.submit_button.value = "Filters_inb";
     		F.change_action.value = "gozila_cgi";
		F.submit_type.value = "delete";
	       	F.action.value = "Apply";
       		F.submit();
	}
}
function valid(F)
{
	if(F.day_all_inb.checked == false && F.week0_inb.checked == false && F.week1_inb.checked == false && F.week2_inb.checked == false && F.week3_inb.checked == false && F.week4_inb.checked == false && F.week5_inb.checked == false && F.week6_inb.checked == false){
		//alert("You must at least select a day.");
		alert(errmsg.err88);
		return false;
	}
	if(F.time_all_inb[1].checked == true){
		start = (parseInt(F.start_time_inb.value, 10)*12 + parseInt(F.start_hour_inb.value, 10)) * 60 + parseInt(F.start_min_inb.value, 10);
		end = (parseInt(F.end_time_inb.value, 10)*12 + parseInt(F.end_hour_inb.value, 10)) * 60 + parseInt(F.end_min_inb.value, 10);
		if(end <= start){
			//alert("The end time must be bigger than start time!");
			alert(errmsg.err89);
			return false;
		}
	}
	if(F.f_status1_inb[1].checked == true)	// Disable
		F.f_status_inb.value = "0";
	else {					// Enable
		if(F.f_status2_inb[1].checked == true)	// Allow
			F.f_status_inb.value = "2";
		else					// deny
			F.f_status_inb.value = "1";
	}
	return true;
}
function SelFilter(num,F)
{
        F.submit_button.value = "Filters_inb";
        F.change_action.value = "gozila_cgi";
        F.f_id_inb.value=F.f_id_inb.options[num].value;
        F.submit();
}
function exit()
{
	closeWin(summary_win_inb);
	closeWin(ipmac_win);
}
function init()
{
	port_type_init();
	day_enable_disable(document.filters_inb,'<% filter_inb_tod_get("day_all_init"); %>');
	time_enable_disable(document.filters_inb,'<% filter_inb_tod_get("time_all_init"); %>');
//	setBlockedServicesValue();
	Status(document.filters_inb, '<% filter_inb_policy_get("f_inb_status","onload_status"); %>');
	choose_disable(document.filters_inb.port0_start);
	choose_disable(document.filters_inb.port0_end);
	choose_disable(document.filters_inb.port1_start);
	choose_disable(document.filters_inb.port1_end);
}

function service(id, name, port_start, port_end, protocol){
	this.id = id;
	this.name = name;
	this.start = port_start;
	this.end = port_end;
	this.protocol = protocol;
	this.deleted = false;
	this.modified = false;
}

services=new Array();
services_length=0;
/* Init. services data structure */
<% filter_port_services_get("all_list", "0"); %>

servport_name0 = "<% filter_port_services_get("service", "0"); %>";
servport_name1 = "<% filter_port_services_get("service", "1"); %>";
function search_service_index(name){
 var i;
        for(i=0 ; i<services_length ; i++){
                if(name == services[i].name){
                        return i;
                }
        }
        return -1;
}
function setBlockedServicesValue(){
var index;
	/* for service port 0 */
	index = search_service_index(servport_name0);
	if(index!=-1){
		document.filters_inb.port0_start.value = services[index].start;
		document.filters_inb.port0_end.value = services[index].end;
		document.filters_inb.blocked_service0.selectedIndex = index+1; /* first will be none */
	}

	/* for service port 1 */
	index = search_service_index(servport_name1);
	if(index!=-1){
		document.filters_inb.port1_start.value = services[index].start;
		document.filters_inb.port1_end.value = services[index].end;
		document.filters_inb.blocked_service1.selectedIndex = index+1; /* first will be none */
	}
}

function onChange_blockedServices(index, start, end){
	index--
	if(index == -1){
		start.value = '';
		end.value = '';
	}
	else{
		start.value = services[index].start;
		end.value = services[index].end;
	}
}
function Status(F,I){
	var start = '';
        var end = '';
        var total = F.elements.length;
        for(i=0 ; i < total ; i++){
                if(F.elements[i].name == "blocked_service0")  start = i;
                if(F.elements[i].name == "url5")  end = i;
        }
        if(start == '' || end == '')    return true;

        if(I == "deny" ) {
                for(i = start; i<=end ;i++)
                        choose_disable(F.elements[i]);
        }
        else {
                for(i = start; i<=end ;i++)
                        choose_enable(F.elements[i]);
		choose_disable(document.filters.port0_start);
		choose_disable(document.filters.port0_end);
		choose_disable(document.filters.port1_start);
		choose_disable(document.filters.port1_end);
	}
}

function port_enable_disable(F, I, type)
{
 var port_start = eval("F.inbstart" +I);
 var port_end = eval("F.inbend" + I);

	if (type == 0){
		choose_disable(eval(port_start));
		choose_disable(eval(port_end));
	}
	else {
		choose_enable(eval(port_start));
		choose_enable(eval(port_end));
	}
}
function lan_ip_enable_disable(F,I,type)
{
 var lan_ip = eval("F.inblan" +I);

	if (type == 1){
		choose_enable(eval(lan_ip));
	}
	else {
		choose_disable(eval(lan_ip));
	}
}
function port_type_init()
{
	var i;
	for (i=0; i<4 ;i++){
	var port_type = eval("document.filters_inb.inbport" + i + "[0].checked");
		if (eval(port_type))
			port_enable_disable(document.filters_inb, i, 0);
		else
			port_enable_disable(document.filters_inb, i, 1);
	var lan_type = eval("document.filters_inb.alllan" + i + "[0].checked");
		if (eval(lan_type))
			lan_ip_enable_disable(document.filters_inb, i, 0);
		else
			lan_ip_enable_disable(document.filters_inb, i, 1);
	}
}

</script>

</head>

<BODY onunload=exit() onload=init()>
<DIV align=center>
<FORM name=filters_inb method=<% get_http_method(); %> action=apply.cgi>
<input type=hidden name=submit_button>
<input type=hidden name=change_action>
<input type=hidden name=submit_type>
<input type=hidden name=action>
<input type=hidden name=filter_inb_mix>
<input type=hidden name=filter_inb_policy>
<input type=hidden name=f_status_inb>

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
          bgColor=#6666cc height=33><FONT color=#ffffff><script>Capture(share.production)</script>&nbsp;&nbsp;</FONT></TD>
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
                <TD width=83 height=1><IMG height=10 src="image/UI_06.gif" width=83 border=0></TD>
                <TD width=73 height=1><IMG height=10 src="image/UI_06.gif" width=83 border=0></TD>
                <TD width=113 height=1><IMG height=10 src="image/UI_06.gif" width=83 border=0></TD>
                <TD width=103 height=1><IMG height=10 src="image/UI_07.gif" width=103 border=0></TD>
                <TD width=85 height=1><IMG height=10 src="image/UI_06.gif" width=100 border=0></TD>
                <TD width=115 height=1><IMG height=10 src="image/UI_06.gif" width=115 border=0></TD>
                <TD width=74 height=1><IMG height=10 src="image/UI_06.gif" width=79 border=0></TD></TR>
              <TR>
                <TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><A style="TEXT-DECORATION: none" href="index.asp"><script>Capture(bmenu.setup)</script></A></FONT></TD>
                <TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Wireless_Basic.asp"><script>Capture(bmenu.wireless)</script></a></FONT></TD>
                <TD align=middle bgColor=#000000 height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Firewall.asp"><script>Capture(bmenu.security)</script></a></FONT></TD>
                <TD align=middle bgColor=#6666cc height=20><FONT style="FONT-WEIGHT: 700" color=#ffffff><script>Capture(bmenu.accrestriction)</script></a></FONT></TD>
                <TD align=middle bgColor=#000000 height=20><P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Forward_UPnP.asp"><script>Capture(bmenu.application)</script><BR>&amp; <script>Capture(bmenu.gaming)</script></a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>
                <TD align=middle bgColor=#000000 height=20><P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Management.asp"><script>Capture(bmenu.admin)</script></a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>
                <TD align=middle bgColor=#000000 height=20><P style="MARGIN-BOTTOM: 4px"><FONT style="FONT-WEIGHT: 700" color=#ffffff><a style="TEXT-DECORATION: none" href="Status_Router.asp"><script>Capture(bmenu.statu)Status</a>&nbsp;&nbsp;&nbsp;&nbsp;</FONT></P></TD>
              </TR>
            <tr>
              <td width="643" style="border-style: none; border-width: medium; font-style:normal; font-variant:normal; font-weight:normal; font-size:10pt; font-family:Arial, Helvetica, sans-serif; color:black" bgcolor="#6666CC" height="21" colspan="7">
              <table border="0" cellpadding="0" cellspacing="0" style="border-collapse: collapse" bordercolor="#111111" id="AutoNumber12" height="21">
                <TR align=center>
                      <TD width=20></TD>
<% support_match("PARENTAL_CONTROL_SUPPORT", "1", "
                      <TD class=small width=100><A href='Parental_Control.asp'><script>Capture(acctopmenu.parctrl)</script></A></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD width=25></TD>"); %>
                      <TD class=small width=90><A href='Filters.asp'><script>Capture(acctopmenu.inetacc)</script></A></TD>
                      <TD width=1><P class=bar><font color='white'><b>|</b></font></P></TD>
                      <TD width=22></TD>
                      <TD class=small width=90><FONT style="COLOR: white"><script>Capture(acctopmenu.inbtraffic)</script></FONT></TD>
                      <TD><P class=bar>&nbsp;</P></TD>
                      <TD width=22></TD>
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
				<font face="Arial" style="font-size: 9pt" color="#FFFFFF">
				<script>Capture(accleftmenu.inbtraffic)</script></font></b></td>
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
              
              <tr>
              	<td width="156" height="25" bgcolor="#E7E7E7" colspan=3>&nbsp;</td> 
              	<td width="8" height="25">
                <img border="0" src="image/UI_04.gif" width="8" height="28"></td>
                <td width="14" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="17" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td height="25" bgcolor="#FFFFFF" width="13">&nbsp;</td>
                <td height="25" bgcolor="#FFFFFF" width="101">
                <span style="font-size: 8pt"><script>Capture(accinbtraffic.inbtrafficplc)</script></span>: </font>
                </td>
                <td height="25" bgcolor="#FFFFFF" width="296">
      &nbsp;<SELECT name=f_id_inb onChange=SelFilter(this.form.f_id_inb.selectedIndex,this.form)>
<% filter_inb_policy_select(); %>
    </SELECT>&nbsp;
    		<script>document.write("<input type=button value=\"" + accbutton.del + "\" onClick=to_delete(this.form)>");</script>&nbsp;
    		<script>document.write("<input type=button value=\"" + accbutton.summary + "\" OnClick=\"ViewSummary()\">");</script></td>
                <td width="13" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="15" height="25" bgcolor="#FFFFFF">
                <img border="0" src="image/UI_05.gif" height="28" width="15"></td>
              </tr>
              
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
              	<td width="156" height="25" bgcolor="#E7E7E7" colspan=3>&nbsp;</td> 
              	<td width="8" height="25">
                <img border="0" src="image/UI_04.gif" width="8" height="25"></td>
                <td width="14" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="17" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td height="25" bgcolor="#FFFFFF" width="13">&nbsp;</td>
                <td height="25" bgcolor="#FFFFFF" width="101">
                <font style="font-size: 8pt"><script>Capture(accinbtraffic.statuStatus: </font>
                </td>
                <td height="25" bgcolor="#FFFFFF" width="296">
		<INPUT type=radio value=enable name=f_status1_inb <% filter_inb_policy_get("f_inb_status","enable"); %>><B><script>Capture(share.enabled)</script></B>&nbsp;
		<INPUT type=radio value=disable name=f_status1_inb <% filter_inb_policy_get("f_inb_status","disable"); %>><B><script>Capture(share.disabled)</scrit></B></td>
                <td width="13" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="15" height="25" bgcolor="#FFFFFF">
                <img border="0" src="image/UI_05.gif" height="25" width="15"></td>
              </tr>
              
              <tr>
              	<td width="156" height="25" bgcolor="#E7E7E7" colspan=3>&nbsp;</td> 
              	<td width="8" height="25">
                <img border="0" src="image/UI_04.gif" width="8" height="25"></td>
                <td width="14" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="17" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td height="25" bgcolor="#FFFFFF" width="13">&nbsp;</td>
                <td height="25" bgcolor="#FFFFFF" width="101"><script>Capture(accinbtraffic.enterplcname)</script>:</td>
                <td height="25" bgcolor="#FFFFFF" width="296">&nbsp;<INPUT maxLength=30 size=22 name=f_name_inb value='<% filter_inb_policy_get("f_inb_name",""); %>'></td>
                <td width="13" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="15" height="25" bgcolor="#FFFFFF">
                <img border="0" src="image/UI_05.gif" height="25" width="15"></td>
              </tr>

              <tr>
                <td width="156" height="25" bgcolor="#E7E7E7" colspan=3>&nbsp;</td>
                <td width="8" height="25">
                <img border="0" src="image/UI_04.gif" width="8" height="25"></td>
                <td width="14" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="17" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="13" height="25" bgcolor="#FFFFFF"> 
                &nbsp;</td>
                <td width="397" height="25" bgcolor="#FFFFFF" colspan="2">
                <hr color="#B5B5E6" size="1"> </td>
                <td width="13" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="15" height="25" bgcolor="#FFFFFF">
                <img border="0" src="image/UI_05.gif" height="25" width="15"></td>
              </tr>
<!-- -----------------------jungo inbound table code------------------------------------- -->
              <tr>
                <td width="156" height="25" bgcolor="#E7E7E7" colspan=3>&nbsp;</td>
                <td width="8">
                <img border="0" src="image/UI_04.gif" width="8" height="340"></td>
                <td width="454" height="25" bgcolor="#FFFFFF" colspan="6">
                
                <TABLE cellSpacing=0 cellPadding=3 align=center border=1>
        <TBODY>
        <TR>
          <TD class=headrow noWrap height=36><script>Capture(accinbtraffic.frominetipaddr)</script></TD>
          <TD class=headrow noWrap height=36><script>Capture(accinbtraffic.proto)</script></TD>
          <TD class=headrow noWrap height=36><script>Capture(accinbtraffic.portnum)</script></TD>
          <TD class=headrow noWrap height=36><script>Capture(accinbtraffic.toinetipaddr)</script></TD>
        </TR>
          
        <TR>
          <TD noWrap class=incell height=27>
	    <INPUT class=num maxLength=3 size=3 name=inbwan0_0 value='<% filter_inb_mix_get("inbwan0_","0"); %>' onchange=valid_range(this,0,255,"IP")> .
	    <INPUT class=num maxLength=3 size=3 name=inbwan0_1 value='<% filter_inb_mix_get("inbwan0_","1"); %>' onchange=valid_range(this,0,255,"IP")> .
	    <INPUT class=num maxLength=3 size=3 name=inbwan0_2 value='<% filter_inb_mix_get("inbwan0_","2"); %>' onchange=valid_range(this,0,255,"IP")> .
	    <INPUT class=num maxLength=3 size=3 name=inbwan0_3 value='<% filter_inb_mix_get("inbwan0_","3"); %>' onchange=valid_range(this,0,254,"IP")></TD>
          <TD class=incell height=27>
          <SELECT name=inbpro0>
	        <OPTION value=tcp <% filter_inb_mix_get("inbpro0","tcp"); %>><script>Capture(accinbtraffic.tcp)</script></OPTION>
		<OPTION value=udp <% filter_inb_mix_get("inbpro0","udp"); %>><script>Capture(accinbtraffic.udp)</script></OPTION>
	        <option value=both <% filter_inb_mix_get("inbpro0","both"); %>><script>Capture(accinbtraffic.both)</script></option>
          </SELECT></TD>
          <TD class=incell align=left height=27>
	  <table><tr><td>
	  <INPUT type=radio value=0 name=inbport0 <% filter_inb_mix_get("inbport0","0"); %> onClick="port_enable_disable(this.form, 0, 0)" checked><script>Capture(accinbtraffic.any)</script></td></tr>
	  <tr><td>
          <INPUT type=radio value=1 name=inbport0 <% filter_inb_mix_get("inbport0","1"); %> onClick="port_enable_disable(this.form, 0, 1)"><INPUT class=num  maxLength=5 size=3 name=inbstart0 value='<% filter_inb_mix_get("inbport_range0","0"); %>' onBlur=valid_range(this,0,65535,"Port")> to
    	  <INPUT class=num   maxLength=5 size=3 name=inbend0 value='<% filter_inb_mix_get("inbport_range0","1"); %>' onBlur=valid_range(this,0,65535,"Port")></td></tr></table>
	  </TD>
          <TD noWrap class=incell height=27>
	  <table><tr><td>
	  <INPUT type=radio value=0 name=alllan0 <% filter_inb_mix_get("alllan0","0"); %> onClick="lan_ip_enable_disable(this.form, 0, 0)" checked><script>Capture(accinbtraffic.any)</script></td></tr>
	  <tr><td><INPUT type=radio value=1 name=alllan0 <% filter_inb_mix_get("alllan0","1"); %> onClick="lan_ip_enable_disable(this.form, 0, 1)">
	  <% prefix_ip_get("lan_ipaddr", 1); %><INPUT class=num maxLength=3 size=3 name=inblan0 value='<% filter_inb_mix_get("inblan0",""); %>' onchange=valid_range(this,0,254,"IP")></td></tr></table></TD>
        </TR>
         
         <TR>
          <TD noWrap class=incell height=27>
	    <INPUT class=num maxLength=3 size=3 name=inbwan1_0 value='<% filter_inb_mix_get("inbwan1_","0"); %>' onchange=valid_range(this,0,255,"IP")> .
	    <INPUT class=num maxLength=3 size=3 name=inbwan1_1 value='<% filter_inb_mix_get("inbwan1_","1"); %>' onchange=valid_range(this,0,255,"IP")> .
	    <INPUT class=num maxLength=3 size=3 name=inbwan1_2 value='<% filter_inb_mix_get("inbwan1_","2"); %>' onchange=valid_range(this,0,255,"IP")> .
	    <INPUT class=num maxLength=3 size=3 name=inbwan1_3 value='<% filter_inb_mix_get("inbwan1_","3"); %>' onchange=valid_range(this,0,254,"IP")></TD>
          <TD class=incell height=27>
          <SELECT name=inbpro1>
	        <OPTION value=tcp <% filter_inb_mix_get("inbpro1","tcp"); %>><script>Capture(accinbtraffic.tcp)</script></OPTION>
		<OPTION value=udp <% filter_inb_mix_get("inbpro1","udp"); %>><script>Capture(accinbtraffic.udp)</script></OPTION>
	        <option value=both <% filter_inb_mix_get("inbpro1","both"); %>><script>Capture(accinbtraffic.both)</script></option>
          </SELECT></TD>
          <TD class=incell align=left height=27>
	  <table><tr><td>
	  <INPUT type=radio value=0 name=inbport1 <% filter_inb_mix_get("inbport1","0"); %> onClick="port_enable_disable(this.form, 1, 0)" checked><script>Capture(accinbtraffic.any)</script></td></tr>
	  <tr><td>
          <INPUT type=radio value=1 name=inbport1 <% filter_inb_mix_get("inbport1","1"); %> onClick="port_enable_disable(this.form, 1, 1)"><INPUT class=num  maxLength=5 size=3 name=inbstart1 value='<% filter_inb_mix_get("inbport_range1","0"); %>' onBlur=valid_range(this,0,65535,"Port")> <script>Capture(accinbtraffic.to)</script>
    	  <INPUT class=num   maxLength=5 size=3 name=inbend1 value='<% filter_inb_mix_get("inbport_range1","1"); %>' onBlur=valid_range(this,0,65535,"Port")></td></tr></table>
	  </TD>
          <TD noWrap class=incell height=27>
	  <table><tr><td>
	  <INPUT type=radio value=0 name=alllan1 <% filter_inb_mix_get("alllan1","0"); %> onClick="lan_ip_enable_disable(this.form, 1, 0)" checked><script>Capture(accinbtraffic.any)</script></td></tr>
	  <tr><td><INPUT type=radio value=1 name=alllan1 <% filter_inb_mix_get("alllan1","1"); %> onClick="lan_ip_enable_disable(this.form, 1, 1)">
	  <% prefix_ip_get("lan_ipaddr", 1); %><INPUT class=num maxLength=3 size=3 name=inblan1 value='<% filter_inb_mix_get("inblan1",""); %>' onchange=valid_range(this,0,254,"IP")></td></tr></table></TD>
        </TR>
 	
	<TR>
          <TD noWrap class=incell height=27>
	    <INPUT class=num maxLength=3 size=3 name=inbwan2_0 value='<% filter_inb_mix_get("inbwan2_","0"); %>' onchange=valid_range(this,0,255,"IP")> .
	    <INPUT class=num maxLength=3 size=3 name=inbwan2_1 value='<% filter_inb_mix_get("inbwan2_","1"); %>' onchange=valid_range(this,0,255,"IP")> .
	    <INPUT class=num maxLength=3 size=3 name=inbwan2_2 value='<% filter_inb_mix_get("inbwan2_","2"); %>' onchange=valid_range(this,0,255,"IP")> .
	    <INPUT class=num maxLength=3 size=3 name=inbwan2_3 value='<% filter_inb_mix_get("inbwan2_","3"); %>' onchange=valid_range(this,0,254,"IP")></TD>
          <TD class=incell height=27>
          <SELECT name=inbpro2>
	        <OPTION value=tcp <% filter_inb_mix_get("inbpro2","tcp"); %>><script>Capture(accinbtraffic.tcp)</script></OPTION>
		<OPTION value=udp <% filter_inb_mix_get("inbpro2","udp"); %>><script>Capture(accinbtraffic.udp)</script></OPTION>
	        <option value=both <% filter_inb_mix_get("inbpro2","both"); %>><script>Capture(accinbtraffic.both)</script></option>
          </SELECT></TD>
          <TD class=incell align=left height=27>
	  <table><tr><td>
	  <INPUT type=radio value=0 name=inbport2 <% filter_inb_mix_get("inbport2","0"); %> onClick="port_enable_disable(this.form, 2, 0)" checked><script>Capture(accinbtraffic.any)</script></td></tr>
	  <tr><td>
          <INPUT type=radio value=1 name=inbport2 <% filter_inb_mix_get("inbport2","1"); %> onClick="port_enable_disable(this.form, 2, 1)"><INPUT class=num  maxLength=5 size=3 name=inbstart2 value='<% filter_inb_mix_get("inbport_range2","0"); %>' onBlur=valid_range(this,0,65535,"Port")><script>Capture(accinbtraffic.to)</script>
    	  <INPUT class=num   maxLength=5 size=3 name=inbend2 value='<% filter_inb_mix_get("inbport_range2","1"); %>' onBlur=valid_range(this,0,65535,"Port")></td></tr></table>
	  </TD>
          <TD noWrap class=incell height=27>
	  <table><tr><td>
	  <INPUT type=radio value=0 name=alllan2 <% filter_inb_mix_get("alllan2","0"); %> onClick="lan_ip_enable_disable(this.form, 2, 0)" checked><script>Capture(accinbtraffic.any)</script></td></tr>
	  <tr><td><INPUT type=radio value=1 name=alllan2 <% filter_inb_mix_get("alllan2","1"); %> onClick="lan_ip_enable_disable(this.form, 2, 1)">
	  <% prefix_ip_get("lan_ipaddr", 1); %><INPUT class=num maxLength=3 size=3 name=inblan2 value='<% filter_inb_mix_get("inblan2",""); %>' onchange=valid_range(this,0,254,"IP")></td></tr></table></TD>
        </TR>
	
	<TR>
          <TD noWrap class=incell height=27>
	    <INPUT class=num maxLength=3 size=3 name=inbwan3_0 value='<% filter_inb_mix_get("inbwan3_","0"); %>' onchange=valid_range(this,0,255,"IP")> .
	    <INPUT class=num maxLength=3 size=3 name=inbwan3_1 value='<% filter_inb_mix_get("inbwan3_","1"); %>' onchange=valid_range(this,0,255,"IP")> .
	    <INPUT class=num maxLength=3 size=3 name=inbwan3_2 value='<% filter_inb_mix_get("inbwan3_","2"); %>' onchange=valid_range(this,0,255,"IP")> .
	    <INPUT class=num maxLength=3 size=3 name=inbwan3_3 value='<% filter_inb_mix_get("inbwan3_","3"); %>' onchange=valid_range(this,0,254,"IP")></TD>
          <TD class=incell height=27>
          <SELECT name=inbpro3>
	        <OPTION value=tcp <% filter_inb_mix_get("inbpro3","tcp"); %>><script>Capture(accinbtraffic.tcp)</script></OPTION>
		<OPTION value=udp <% filter_inb_mix_get("inbpro3","udp"); %>><script>Capture(accinbtraffic.udp)</script></OPTION>
	        <option value=both <% filter_inb_mix_get("inbpro3","both"); %>><script>Capture(accinbtraffic.both)</script></option>
          </SELECT></TD>
          <TD class=incell align=left height=27>
	  <table><tr><td>
	  <INPUT type=radio value=0 name=inbport3 <% filter_inb_mix_get("inbport3","0"); %> onClick="port_enable_disable(this.form, 3, 0)" checked><script>Capture(accinbtraffic.any)</script></td></tr>
	  <tr><td>
          <INPUT type=radio value=1 name=inbport3 <% filter_inb_mix_get("inbport3","1"); %> onClick="port_enable_disable(this.form, 3, 1)"><INPUT class=num  maxLength=5 size=3 name=inbstart3 value='<% filter_inb_mix_get("inbport_range3","0"); %>' onBlur=valid_range(this,0,65535,"Port")> to
    	  <INPUT class=num   maxLength=5 size=3 name=inbend3 value='<% filter_inb_mix_get("inbport_range3","1"); %>' onBlur=valid_range(this,0,65535,"Port")></td></tr></table>
	  </TD>
          <TD noWrap class=incell height=27>
	  <table><tr><td>
	  <INPUT type=radio value=0 name=alllan3 <% filter_inb_mix_get("alllan3","0"); %> onClick="lan_ip_enable_disable(this.form, 3, 0)" checked><script>Capture(accinbtraffic.any)</script></td></tr>
	  <tr><td><INPUT type=radio value=1 name=alllan3 <% filter_inb_mix_get("alllan3","1"); %> onClick="lan_ip_enable_disable(this.form, 3, 1)">
	  <% prefix_ip_get("lan_ipaddr", 1); %><INPUT class=num maxLength=3 size=3 name=inblan3 value='<% filter_inb_mix_get("inblan3",""); %>' onchange=valid_range(this,0,254,"IP")></td></tr></table></TD>
        </TR>

	</TBODY></TABLE>
              
                </td>
                <td width="15" height="25" bgcolor="#FFFFFF">
                <img border="0" src="image/UI_05.gif" height="340" width="15"></td>
              </tr>
<!-- ------------------------------------------------------------ -->
              <tr>
                <td width="156" height="25" bgcolor="#E7E7E7" colspan=3>&nbsp;</td>
                <td width="8" height="25">
                <img border="0" src="image/UI_04.gif" width="8" height="25"></td>
                <td width="14" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="17" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="13" height="25" bgcolor="#FFFFFF"> 
                &nbsp;</td>
                <td width="397" height="25" bgcolor="#FFFFFF" colspan="2">
                <hr color="#B5B5E6" size="1"> </td>
                <td width="13" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="15" height="25" bgcolor="#FFFFFF">
                <img border="0" src="image/UI_05.gif" height="25" width="15"></td>
              </tr>
              
              <tr>
                <td width="156" height="25" bgcolor="#E7E7E7" colspan=3>&nbsp;</td>
                <td width="8" height="25">
                <img border="0" src="image/UI_04.gif" width="8" height="40"></td>
                <td width="14" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="17" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="13" height="25" bgcolor="#FFFFFF"> 
                &nbsp;</td>
                <td width="101" height="25" bgcolor="#FFFFFF">
                <input type=radio <% filter_inb_policy_get("f_inb_status","deny"); %> name=f_status2_inb value=deny ><script>Capture(accinbtraffic.deny)</script><br>
                <input type=radio <% filter_inb_policy_get("f_inb_status","allow"); %> name=f_status2_inb value=allow ><script>Capture(accinbtraffic.allow)</scritp></td>
                <td width="296" height="25" bgcolor="#FFFFFF"><script>Capture(accinbtraffic.inetaccess)</script></td>
                <td width="13" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="15" height="25" bgcolor="#FFFFFF">
                <img border="0" src="image/UI_05.gif" height="40" width="15"></td>
              </tr>
              
              <tr>
                <td width="156" height="25" bgcolor="#E7E7E7" colspan=3>&nbsp;</td>
                <td width="8" height="25">
                <img border="0" src="image/UI_04.gif" width="8" height="25"></td>
                <td width="14" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="17" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="410" height="25" bgcolor="#FFFFFF" colspan="3" align="left"> 
                <b><font face="Arial" style="font-size: 8pt"><script>Capture(accinbtraffic.days)</script></font></b></td>
                <td width="13" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="15" height="25" bgcolor="#FFFFFF">
                <img border="0" src="image/UI_05.gif" height="25" width="15"></td>
              </tr> 
              
              <tr>
                <td width="156" height="25" bgcolor="#E7E7E7" colspan=3>&nbsp;</td>
                <td width="8" height="25">
                <img border="0" src="image/UI_04.gif" width="8" height="28"></td>
                <td width="14" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="17" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="13" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td height="25" bgcolor="#FFFFFF" width="101">
                <input type=checkbox value=1 name=day_all_inb <% filter_inb_tod_get("day_all_inb"); %> onClick=dayall(this.form)>
                <span style="font-size: 8pt"><script>Capture(accinbtraffic.everyday)</script></span><font style="font-size: 8pt">&nbsp;&nbsp;
                </font></td>
                <td height="25" bgcolor="#FFFFFF" width="296">
                	<table><tr>
                	<td width=50><INPUT type=checkbox value=1 name=week0_inb <% filter_inb_tod_get("week0_inb"); %>>&nbsp;&nbsp;<script>Capture(accinbtraffic.suns)</script>&nbsp;</td>
                	<td width=50><INPUT type=checkbox value=1 name=week1_inb <% filter_inb_tod_get("week1_inb"); %>>&nbsp;&nbsp;<script>Capture(accinbtraffic.mon)</script>&nbsp;</td>
                	<td width=50><INPUT type=checkbox value=1 name=week2_inb <% filter_inb_tod_get("week2_inb"); %>>&nbsp;&nbsp;<script>Capture(accinbtraffic.tue)</script>&nbsp;</td>
                	<td width=50><INPUT type=checkbox value=1 name=week3_inb <% filter_inb_tod_get("week3_inb"); %>>&nbsp;&nbsp;<script>Capture(accinbtraffic.wed)</script>&nbsp;</td>
                	</tr></table>
                </td>
                <td width="13" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="15" height="25" bgcolor="#FFFFFF">
                <img border="0" src="image/UI_05.gif" height="28" width="15"></td>
              </tr>
              
              <tr>
                <td width="156" height="25" bgcolor="#E7E7E7" colspan=3>&nbsp;</td>
                <td width="8" height="25">
                <img border="0" src="image/UI_04.gif" width="8" height="28"></td>
                <td width="14" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="17" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="13" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td height="25" bgcolor="#FFFFFF" width="101">
                <span style="font-size: 8pt"></span><font style="font-size: 8pt">&nbsp;&nbsp;
                </font></td>
                <td height="25" bgcolor="#FFFFFF" width="296">
                	<table><tr>
                	<td width=50><INPUT type=checkbox value=1 name=week4_inb <% filter_inb_tod_get("week4_inb"); %>>&nbsp;&nbsp;<script>Capture(accinbtraffic.thu)</script>&nbsp;</td>
                	<td width=50><INPUT type=checkbox value=1 name=week5_inb <% filter_inb_tod_get("week5_inb"); %>>&nbsp;&nbsp;<script>Capture(accinbtraffic.fri)</script>&nbsp;</td>
                	<td width=50><INPUT type=checkbox value=1 name=week6_inb <% filter_inb_tod_get("week6_inb"); %>>&nbsp;&nbsp;<script>Capture(accinbtraffic.sat)</script>&nbsp;</td>
                	</tr></table>
                </td>
                <td width="13" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="15" height="25" bgcolor="#FFFFFF">
                <img border="0" src="image/UI_05.gif" height="28" width="15"></td>
              </tr>
              
              
              
              <tr>
                <td width="156" height="25" bgcolor="#E7E7E7" colspan=3>&nbsp;</td>
                <td width="8" height="25">
                <img border="0" src="image/UI_04.gif" width="8" height="28"></td>
                <td width="14" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="17" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="410" height="25" bgcolor="#FFFFFF" colspan="3" align="left"> 
                <b><font face="Arial" style="font-size: 8pt"><script>Capture(accinbtraffic.times)</script></font></b></td>
                <td width="13" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="15" height="25" bgcolor="#FFFFFF">
                <img border="0" src="image/UI_05.gif" height="28" width="15"></td>
              </tr> 
              
              <tr>
                <td width="156" height="25" bgcolor="#E7E7E7" colspan=3>&nbsp;</td>
                <td width="8" height="25">
                <img border="0" src="image/UI_04.gif" width="8" height="28"></td>
                <td width="14" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="17" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="13" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td height="25" bgcolor="#FFFFFF" width="101">
                <input type=radio value=1 name=time_all_inb <% filter_inb_tod_get("time_all_en_inb"); %> onClick=timeall(this.form,'0')>
                <span style="font-size: 8pt">24 Hours</span><font style="font-size: 8pt">&nbsp;&nbsp;
                </font></td>
                <td height="25" bgcolor="#FFFFFF" width="296" align=left>
                	<table><tr>
                	<td width="20">
                		<INPUT type=radio value=0 name=time_all_inb <% filter_inb_tod_get("time_all_dis_inb"); %> onClick=timeall(this.form,'1')>
                	</td>
                	<td width="35">
                		<span style="font-size: 8pt">&nbsp;<script>Capture(accinbtraffic.from)</script>:</span>&nbsp;
                	</td>
                	<td>
                <select name="start_hour_inb" style="font-family:Arial; font-size:9pt">
<% filter_inb_tod_get("start_hour_12_inb"); %>
                </select>
                	</td>
                	<td style="font-family:Arial; font-size:9pt">
                :	
                	</td>
                	<td>
                <select name="start_min_inb" style="font-family:Arial; font-size:9pt">
<% filter_inb_tod_get("start_min_5_inb"); %>
                </select>
                &nbsp;&nbsp;
                	</td>
                	<td>
                <select name="start_time_inb" style="font-family:Arial; font-size:9pt">
                	<option value=0 <% filter_inb_tod_get("start_time_am_inb"); %>><script>Capture(accinbtraffic.am)</script></option>
                	<option value=1 <% filter_inb_tod_get("start_time_pm_inb"); %>><script>Capture(accinbtraffic.pm)</script></option>
                </select>
                	</td>
                	</tr></table>
                </td>
                <td width="13" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="15" height="25" bgcolor="#FFFFFF">
                <img border="0" src="image/UI_05.gif" height="28" width="15"></td>
              </tr>
              
              <tr>
                <td width="156" height="25" bgcolor="#E7E7E7" colspan=3>&nbsp;</td>
                <td width="8" height="25">
                <img border="0" src="image/UI_04.gif" width="8" height="28"></td>
                <td width="14" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="17" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="13" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td height="25" bgcolor="#FFFFFF" width="101">
                <span style="font-size: 8pt"></span><font style="font-size: 8pt">&nbsp;&nbsp;
                </font></td>
                <td height="25" bgcolor="#FFFFFF" width="296" align=left>
                	<table><tr>
                	<td width="20">
                		<input type=hidden name="allday">
                	</td>
                	<td width="35">
                		<span style="font-size: 8pt">&nbsp;<script>Capture(accinbtraffic.to)</script>:</span>&nbsp;
                	</td>
                	<td>
                <select name="end_hour_inb" style="font-family:Arial; font-size:9pt">
<% filter_inb_tod_get("end_hour_12_inb"); %>
                </select>
                	</td>
                	<td style="font-family:Arial; font-size:9pt">
                :	
                	</td>
                	<td>
                <select name="end_min_inb" style="font-family:Arial; font-size:9pt">
<% filter_inb_tod_get("end_min_5_inb"); %>
                </select>
                &nbsp;&nbsp;
                	</td>
                	<td>
                <select name="end_time_inb" style="font-family:Arial; font-size:9pt">
                	<option value=0 <% filter_inb_tod_get("end_time_am_inb"); %>><script>Capture(accinbtraffic.am)</script></option>
                	<option value=1 <% filter_inb_tod_get("end_time_pm_inb"); %>><script>Capture(accinbtraffic.pm)</script></option>
                </select>
                	</td>
                	</tr></table>
                </td>
                <td width="13" height="25" bgcolor="#FFFFFF">&nbsp;</td>
                <td width="15" height="25" bgcolor="#FFFFFF">
                <img border="0" src="image/UI_05.gif" height="28" width="15"></td>
              </tr>
              
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
                <td width="156" height="25" bgcolor="#6666CC"><font color="#FFFFFF"><span>
<a target="_blank" href="help/HFilters_Inb.asp"><script>Capture(share.more)</script></a></span></font></td>
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
                <td width="15" height="25" bgcolor="#FFFFFF">
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
            <TBODY>
              <TR>
                <TD align=middle bgColor=#42498c width="101"><FONT color=#ffffff><B><A href='javascript:to_submit(document.forms[0]);'><script>Capture(share.saves)</script></A></B></FONT>
                </TD>
                <TD align=middle width=8 bgColor=#6666cc>&nbsp;</TD>
                <TD align=middle bgColor=#434a8f width="111"><FONT color=#ffffff><B><A href='Filters_inb.asp'><script>Capture(share.cancels)</script></A></B></FONT></TD>
                <TD align=middle width=4 bgColor=#6666cc>&nbsp;</TD>
                </TR></TBODY></TABLE></TD>
                <TD width=15 bgColor=#6666cc height=33>&nbsp;</TD>
                <TD width=15 bgColor=#000000 height=33>&nbsp;</TD>
              </TR>
            </TBODY>
            </TABLE>
	      
              </table>
    		</form></div>
            </body>

</html>
