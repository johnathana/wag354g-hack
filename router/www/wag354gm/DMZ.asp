
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

<HTML><HEAD><TITLE>DMZ</TITLE>
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
<SCRIPT language="javascript" type="text/javascript" src="capapp.js"></SCRIPT>
<SCRIPT language=JavaScript>
document.title=(apptopmenu.dmz);
<% wireless_support(5); %>share.productname = share.productname1;<% wireless_support(6); %>
var EN_DIS = '<% nvram_get("dmz_enable"); %>'
var share_wanip = '<% nvram_get("share_wanip"); %>'

function to_submit(F)

{
	var flag;
	flag = 0;
	if(F.dmz_enable[0].checked == true){

		if(F.dmz_ipaddr.value == "0"){

			//alert("Illegal DMZ IP Address!");
      	               alert(errmsg.err90);
			F.dmz_ipaddr.focus();
			flag = 1;
		//	return false;

		}	

	}

	if(flag == 0)
	{

		F.submit_button.value = "DMZ";

		F.action.value = "Apply";

        	F.submit();
	}

}

function dmz_enable_disable(F,I)

{

	EN_DIS1 = I;

	if ( I == "0" ){

		choose_disable(F.dmz_ipaddr);

	}

	else{

		choose_enable(F.dmz_ipaddr);

	}

}

function SelDMZ(F,num)

{
	if(F.dmz_enable[0].checked == true)
	{
		if (share_wanip == 1)
		{
			//alert("Share WAN IP function is enabled, this function can not work!");
			alert(errmsg.err91);
			F.dmz_enable[0].checked = false;
			F.dmz_enable[1].checked = true;
			return false;
		}
	}
	dmz_enable_disable(F,num);

}

function init() 

{               

	dmz_enable_disable(document.dmz,<% nvram_get("dmz_enable"); %>);

}

</SCRIPT>

</HEAD>

<BODY onload=init()>

<DIV align=center>

<FORM name=dmz method=<% get_http_method(); %> action=apply.cgi>

<input type=hidden name=submit_button value="DMZ">

<input type=hidden name=change_action>

<input type=hidden name=action value="Apply">

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
			<TD width=120 class=small><FONT style="COLOR: white"><script>Capture(apptopmenu.dmz)</script></a></TD>
			<TD><P class=bar><font color='white'><b>|</b></font></P></TD>
			<TD width=120 class=small><A href="qos.asp"><script>Capture(apptopmenu.qos)</script></a></TD>
                    </TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE>

<TABLE height=5 cellSpacing=0 cellPadding=0 width=809 bgColor=black border=0>

  <TBODY>

  <TR bgColor=black>

    <TD>

    <IMG height=15 src="image/UI_03.gif" width=164 border=0></TD>

    <TD>

    <IMG height=15 src="image/UI_02.gif" width=645 border=0></TD></TR></TBODY></TABLE>



<TABLE height=23 cellSpacing=0 cellPadding=0 width=809 border=0>

  <TBODY>

  <TR>

    <TD width=633>

      <TABLE cellSpacing=0 cellPadding=0 border=0>

        <TBODY>

        <TR>

          <TD width=156 bgColor=#000000 colSpan=3 height=25 align=right><B><FONT style="FONT-SIZE: 9pt" color=#ffffff><script>Capture(appleftmenu.dmz)</script></FONT></B></TD>

          <TD width=8 bgColor=#000000 height=25></TD>

          <TD width=42 height=25>&#12288</TD>

          <!-- TD width=18 height=25></TD>

          <TD width=13 height=25></TD -->

          <TD width=141 height=25>&nbsp;</TD>

          <TD width=258 height=25>&nbsp;</TD>

          <TD width=13 height=25>&nbsp;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&#12288;</TD></TR>

     

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&#12288;</TD>

          <td width="8" height="25" align=right><img border="0" src="image/UI_04.gif" width="8" height="28"></td>

          <TD height=25 width=42>&#12288;</TD>

          <TD width=141 height=25><script>Capture(appdmz.hosting)</script>:&nbsp;</TD>

          <TD width=258 height=25>
			<INPUT type=radio value=1 name=dmz_enable <% nvram_match("dmz_enable","1","checked"); %> onClick=SelDMZ(this.form,1) checked><B><script>Capture(share.enabled)</script></B>&nbsp;<INPUT type=radio value=0 name=dmz_enable <% nvram_match("dmz_enable","0","checked"); %> onClick=SelDMZ(this.form,0)><B><script>Capture(share.disabled)</script></B></TD>

          <TD width=13 height=25>&#12288;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&#12288;</TD></TR>

        <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&#12288;</TD>

          <td width="8" height="25" align=right><img border="0" src="image/UI_04.gif" width="8" height="28"></td>

          <TD height=25 width="42">&#12288;</TD>

          <TD width=141 height=25><script>Capture(appdmz.dmzhostip)</script>:&nbsp;</TD>

          <TD width=258 height=25><B>&nbsp;<% prefix_ip_get("lan_ipaddr",1); %><INPUT class=num maxLength=3 onBlur=valid_range(this,1,254,"IP") size=3 value='<% nvram_get("dmz_ipaddr"); %>' name="dmz_ipaddr">&nbsp;
			<span style="font-weight: 400"><script>Capture(appdmz.validrange)</script></span></b></TD>

          <TD width=13 height=25>&#12288;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&#12288;</TD></TR>

       <TR>

          <TD width=156 bgColor=#e7e7e7 colSpan=3 height=25>&#12288;</TD>

          <td width="8" height="25" align=right><img border="0" src="image/UI_04.gif" width="8" height="28"></td>

          <TD height=25 width="42">&#12288;</TD>

          <TD width=141 height=25></TD>

          <TD width=258 height=25></TD>

          <TD width=13 height=25>&#12288;</TD>

          <TD width=15 background=image/UI_05.gif height=25>&#12288;</TD></TR>



      </TBODY>

      </TABLE></TD>

    <TD vAlign=top width=176 bgColor=#6666cc>

      <TABLE cellSpacing=0 cellPadding=0 width=176 border=0>

        <TBODY>

        <TR>

          <TD width=10 bgColor=#6666cc height=25>&#12288;</TD>

          <TD width=156 bgColor=#6666cc height=25><font color="#FFFFFF">

          <a target="_blank" href="help/HDMZ.asp"><script>Capture(share.more)</script>...</a></font></TD>

          <TD width=9 bgColor=#6666cc height=25>&#12288;</TD>

        </TR>

        </TBODY>

      </TABLE>

    </TD>

  </TR>
  
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
                <TD align=middle bgColor=#42498c width="101"><FONT color=#ffffff><B><A href='javascript:to_submit(document.forms[0])'><script>Capture(share.saves)</script></A></B></FONT>
                </TD>
                <TD align=middle width=8 bgColor=#6666cc>&nbsp;</TD>
                <TD align=middle bgColor=#434a8f width="111"><FONT color=#ffffff><B><A href='DMZ.asp'><script>Capture(share.cancels)</script></A></B></FONT></TD>
                <TD align=middle width=4 bgColor=#6666cc>&nbsp;</TD>
                </TR></TBODY></TABLE></TD>
                <TD width=15 bgColor=#6666cc height=33>&nbsp;</TD>
                <TD width=15 bgColor=#000000 height=33>&nbsp;</TD>
              </TR>
            </TBODY>
            </TABLE>
          </TD>
        </TR>


    </TBODY>

    </TABLE>

    </FORM>

    </DIV>

    </BODY>

    </HTML>
    
