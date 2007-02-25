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

<HTML><HEAD><TITLE>PVC Selection Table</TITLE>
<meta http-equiv="expires" content="0">
<meta http-equiv="cache-control" content="no-cache">
<meta http-equiv="pragma" content="no-cache">
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
<SCRIPT language=JavaScript>
document.title=(share.pvctable);
function my_valid_range(I,start,end,M)

{

	//if(I.value == ""){

	//	if(M == "IP" || M == "Port")

	//		I.value = "0";

	//}

	M1 = unescape(M);

	isdigit(I,M1);



	d = parseInt(I.value, 10);	

	if ( !(d<=end && d>=start) )		

	{		

		//I.focus();

		//alert(M1 +' value is out of range ['+ start + ' - ' + end +']');
		alert(M1 + errmsg.err120 + '[' + start + ' - ' + end + ']');
		//I.value = I.defaultValue;

		//I.value = defaultvalue;

		I.focus();

		return false;	

	}

	else

	{

		I.value = d;	// strip 0

		return true;

	}



}

function my_valid_cos(I)
{
	for(i=0; i<I.value.length; i++)
	{
		ch = I.value.charAt(i);
		if((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))
			continue;

		//alert("Cos Value Incorrect!");
		alert(errmsg.err75);

		I.focus();
		return false;
	}
	return true;

}

function my_valid_type(I)
{
	for(i=0; i<I.value.length; i++)
	{
		ch = I.value.charAt(i);
		if((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))
			continue;

		//alert("Type/Length Value Incorrect!");
		alert(errmsg.err76);

		I.focus();
		return false;
	}
	return true;

}

function my_valid_length(I1,I2)
{
	if(!my_valid_range(I1,0,65535,"Packet%20length%20min%20value"))
		return false;
	if(!my_valid_range(I2,0,65535,"Packet%20length%20max%20value"))
		return false;
	d1 = parseInt(I1.value, 10);	
	d2 = parseInt(I2.value, 10);	
	if(d1>d2)
	{
		//alert("Packet length min value and max value setting incorrect!");
		alert(errmsg.err77);

		I1.focus();
		return false;
	}
	return true;
}



function my_valid_dscp(I)

{

	if(I.value.length == 2)

	{

		ch = I.value.charAt(0);

		if(ch < '0' || ch > '3')

		{

			//alert("DSCP Value Incorrect!");
			alert(errmsg.err78);


			I.focus();

			return false;

		}

		ch = I.value.charAt(1);

	}

	else

		ch = I.value.charAt(0);

	if((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))

		return true;

	else

	{

		//alert("DSCP Value Incorrect!");
		alert(errmsg.err78);

		I.focus();

		return false;

	}

	return true;

}



function my_valid_macs_17(I)

{

	oldmac = I.value;

	var mac = ignoreSpaces(oldmac);

	if (mac == "") 

	{

		return true;

		//alert("Enter MAC Address in (xx-xx-xx-xx-xx-xx) format");

		//return false;

	}

	var m = mac.split("-");

	if (m.length != 6) 

	{

		//alert("Invalid MAC address format");
		alert(errmsg.err79);


		//I.value = I.defaultValue;		

		return false;

	}

	var idx = oldmac.indexOf('-');

	if (idx != -1) {

		var pairs = oldmac.substring(0, oldmac.length).split('-');

		for (var i=0; i<pairs.length; i++) {

			nameVal = pairs[i];

			len = nameVal.length;

			if (len < 1 || len > 2) {

				//alert ("The WAN MAC Address is not correct!!");
				alert(errmsg.err80);

				//I.value = I.defaultValue;		

				I.focus();

				return false;

			}

			for(iln = 0; iln < len; iln++) {

				ch = nameVal.charAt(iln).toLowerCase();

				if (ch >= '0' && ch <= '9' || ch >= 'a' && ch <= 'f') {

				}

				else {

				//	alert ("Invalid hex value " + nameVal + " found in MAC address " + oldmac);
					alert(errmsg.err81 + nameVal + errmsg.err82 + oldmac);


					//I.value = I.defaultValue;		

					I.focus();

					return false;

				}

			}	

		}

	}

	I.value = I.value.toUpperCase();

	if(I.value == "FF-FF-FF-FF-FF-FF"){

		//alert('The MAC Address cannot be the broadcast address!');
		alert(errmsg.err83);

		//I.value = I.defaultValue;	

	}

	m3 = I.value.charAt(1);

	if((m3 & 1) == 1){                               

		//alert('The second character of MAC must be even number : [0, 2, 4, 6, 8, A, C, E]');
		alert(errmsg.err84);

		//I.value = I.defaultValue;                       

	}                                                       

	return true;

}



function SelCONNECTION(num,F)

{

	F.submit_button.value = "PVC_Selection";

	F.change_action.value = "gozila_cgi";

	F.active_connection_selection.value=F.active_connection_selection[num].value;

	F.submit();

}



function SelPro(num,f,i)

{	

		switch(i)

		{

			case 0:

					if(num == 0)

					{

						f.dstport_0.disabled = true;

						f.srcport_0.disabled = true;

					}

					else

					{

						f.dstport_0.disabled = false;

						f.srcport_0.disabled = false;

					}

					break;

			case 1:

					if(num == 0)

					{

						f.dstport_1.disabled = true;

						f.srcport_1.disabled = true;

					}

					else

					{

						f.dstport_1.disabled = false;

						f.srcport_1.disabled = false;

					}

					break;

			case 2:

					if(num == 0)

					{

						f.dstport_2.disabled = true;

						f.srcport_2.disabled = true;

					}

					else

					{

						f.dstport_2.disabled = false;

						f.srcport_2.disabled = false;

					}

					break;

			case 3:

					if(num == 0)

					{

						f.dstport_3.disabled = true;

						f.srcport_3.disabled = true;

					}

					else

					{

						f.dstport_3.disabled = false;

						f.srcport_3.disabled = false;

					}

					break;

			case 4:

					if(num == 0)

					{

						f.dstport_4.disabled = true;

						f.srcport_4.disabled = true;

					}

					else

					{

						f.dstport_4.disabled = false;

						f.srcport_4.disabled = false;

					}

					break;

			case 5:

					if(num == 0)

					{

						f.dstport_5.disabled = true;

						f.srcport_5.disabled = true;

					}

					else

					{

						f.dstport_5.disabled = false;

						f.srcport_5.disabled = false;

					}

					break;

			case 6:

					if(num == 0)

					{

						f.dstport_6.disabled = true;

						f.srcport_6.disabled = true;

					}

					else

					{

						f.dstport_6.disabled = false;

						f.srcport_6.disabled = false;

					}

					break;

			case 7:

					if(num == 0)

					{

						f.dstport_7.disabled = true;

						f.srcport_7.disabled = true;

					}

					else

					{

						f.dstport_7.disabled = false;

						f.srcport_7.disabled = false;

					}

					break;

			case 8:

					if(num == 0)

					{

						f.dstport_8.disabled = true;

						f.srcport_8.disabled = true;

					}

					else

					{

						f.dstport_8.disabled = false;

						f.srcport_8.disabled = false;

					}

					break;

			case 9:

					if(num == 0)

					{

						f.dstport_9.disabled = true;

						f.srcport_9.disabled = true;

					}

					else

					{

						f.dstport_9.disabled = false;

						f.srcport_9.disabled = false;

					}

					break;

		}	

}



function valid_parts()

{

<% dump_valid_parts(); %>	

}



function valid_value(F)

{

	if(F.active_connection_selection.selectedIndex == parseInt(F.active_connection_selection.length,10)-1)

	{

		if(F.active_connection_selection.value == 32)

		{

			//alert("Shouldn't Save!");
			alert(errmsg.err85);

			return false;

		}

	}

	if(valid_parts())

		return true;

}



function to_submit(F)

{

	if(valid_value(F))

	{

		F.submit_button.value = "PVC_Selection";

		F.action.value = "Apply";

       		F.submit();

	}

}



function init()

{

<% dump_match_condition(); %>	

}



</SCRIPT>

</HEAD>

<BODY onload=init()> 

<DIV align=center>

<FORM name=pvcselect action=apply.cgi method=post>

<input type=hidden name=submit_button>

<input type=hidden name=change_action>

<input type=hidden name=action>

<input type=hidden name=pvc_selection_config>



<p align="center">&nbsp;</p>

<p><b><font face="Arial" size="4" color="#0000FF">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <script>Capture(share.pvctable)</script></font></b><FONT face=Arial color=blue size="4"><B> </B></FONT></p>

<p align="left"><script>Capture(share.selactconn)</script>:&nbsp;&nbsp; 



<SELECT name="active_connection_selection" onChange=SelCONNECTION(this.form.active_connection_selection.selectedIndex,this.form)>	

<% active_connection_table(); %>	



</SELECT>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; </p>

<TABLE style="BORDER-COLLAPSE: collapse" borderColor=#111111 height=23 cellSpacing=0 cellPadding=0 width=809 border=0>

  <TBODY>

  <TR>

    <TD>

      <TABLE cellSpacing=0 cellPadding=0 width=1007 border=0>

        <TBODY>

        <TR>

          <TD align=middle width=1007 height=51>

            <TABLE id=AutoNumber16 style="BORDER-COLLAPSE: collapse" 

            borderColor=#e7e7e7 height=24 cellSpacing=0 cellPadding=0 width=1335 border=0>

              <TBODY>

              <TR>

                <TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle width=292 bgColor=#CCCCCC height=30 colspan="2"><b>

				<script>Capture(share.dest)</script></b></TD>

                <TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle width=295 bgColor=#CCCCCC height=30 colspan="2"><b>

				<script>Capture(share.srcs)</script></b></TD>

                <TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle width=126 bgColor=#CCCCCC height=30></TD>

                <TD style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle bgColor=#CCCCCC height=30>&nbsp;</TD>

                <TD style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=center bgColor=#CCCCCC height=30>&nbsp;</TD>

                <TD style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=center bgColor=#CCCCCC height=30>&nbsp;</TD>

              	<TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle width=76 bgColor=#CCCCCC height=30><b>802.1D</b></TD>

              	<TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle width=65 bgColor=#CCCCCC height=30><b>802.3</b></TD>

              	<TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle width=60 bgColor=#CCCCCC height=30><b>802.1Q</b></TD>

              	<TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle width=138 bgColor=#CCCCCC height=30 colspan="2"><b>
				<script>Capture(share.packlen)</script></b></TD>

              	<TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle width=42 bgColor=#CCCCCC height=30>&nbsp;</TD>

              	<TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle width=45 bgColor=#CCCCCC height=30></TD></TR>

              <TR>

                <TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle bgColor=#CCCCCC  height=30 width="149"><b><script>Capture(share.ipaddrr)</script></b></TD>

                <TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle bgColor=#CCCCCC  height=30 width="149"><b><script>Capture(share.netmask)</script></b></TD>

                <TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle bgColor=#CCCCCC  height=30 width="149"><b><script>Capture(share.ipaddrr)</script></b></TD>

                <TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle bgColor=#CCCCCC  height=30 width="149"><b><script>Capture(share.netmask)</script></b></TD>

                <TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle bgColor=#CCCCCC  height=30><b><script>Capture(share.srcmac)</script></b></TD>

                <TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle width=77 bgColor=#CCCCCC height=30><b><script>Capture(share.protocols)</script></b></TD>

                <TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=center width=58 bgColor=#CCCCCC height=30><b>Dst Port</b></TD>

                <TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=center width=58 bgColor=#CCCCCC height=30><b>Src Port</b></TD>

                <TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle width=76 bgColor=#CCCCCC height=30><b><script>Capture(shar.userpri)</script></b></TD>

                <TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle width=65 bgColor=#CCCCCC height=30><b><script>Capture(share.tlen)</script></b></TD>

                <TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle width=60 bgColor=#CCCCCC height=30><b><script>Capture(share.vlanid)</script></b></TD>

                <TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle width=57 bgColor=#CCCCCC height=30><b><script>Capture(share.mins)</script></b></TD>

                <TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle width=57 bgColor=#CCCCCC height=30><b><script>Capture(share.maxs)</script></b></TD>

                <TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle width=64 bgColor=#CCCCCC height=30><b><script>Capture(share.dscp)</script></b></TD>

                <TD 

                style="BORDER-RIGHT: 1px solid; BORDER-TOP: 1px solid; BORDER-LEFT: 1px solid; BORDER-BOTTOM: 1px solid" 

                align=middle width=45 bgColor=#CCCCCC height=30><B><script>Capture(share.applys)</script> 

                </B></TD>

              </TR>

              

              <TR>

                <TD align=middle height=30 width="151"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=dstipaddr_0_0 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_0_1 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_0_2 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_0_3 onBlur=my_valid_range(this,0,253,"DstIp") value=''></FONT></TD>

                <TD align=middle height=30 width="145"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=dstnetmask_0_0 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_0_1 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_0_2 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_0_3 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''></FONT></TD>

                <TD align=middle height=30 width="146"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=srcipaddr_0_0 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_0_1 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_0_2 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_0_3 onBlur=my_valid_range(this,0,253,"SrcIp") value=''></FONT></TD>

                <TD align=middle height=30 width="148"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=srcnetmask_0_0 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_0_1 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_0_2 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_0_3 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''></FONT></TD>

                <TD align=middle height=30><FONT size=2>

				<INPUT class=num maxLength=17 size=17 name=srcmac_0 onBlur=my_valid_macs_17(this) value=''></FONT></TD>

                <TD align=middle height=30><SELECT size=1 name=pro_0 onChange=SelPro(this.selectedIndex,this.form,0)>



				<OPTION value=0 ><script>Capture(share.alls)</script></OPTION>

				<OPTION value=1 >TCP</OPTION>

				<OPTION value=2 >UDP</OPTION>



			</SELECT></TD></TD>

                <TD align=center height=30>

				<INPUT class=num maxLength=5 size=5 name=dstport_0 onBlur=my_valid_range(this,0,65535,"DstPort")></TD>

                <TD align=center height=30>

				<INPUT class=num maxLength=5 size=5 name=srcport_0 onBlur=my_valid_range(this,0,65535,"SrcPort")></TD>

                <TD align=middle height=30>

                0x<INPUT class=num maxLength=2 size=2 name=cos_0 onBlur=my_valid_cos(this)></TD>    

                <TD align=middle height=30>

                0x<INPUT class=num maxLength=4 size=4 name=type_0 onBlur=my_valid_type(this)></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=4 size=4 name=vid_0 onBlur=my_valid_range(this,0,4095,"VlanID")></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=5 size=5 name=lengthmin_0 onBlur=my_valid_range(this,0,65535,"Packet%20length%20min%20value")></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=5 size=5 name=lengthmax_0 onBlur=my_valid_range(this,0,65535,"Packet%20length%20max%20value")></TD>    

                <TD align=middle height=30>

                0x<INPUT class=num maxLength=2 size=2 name=tos_0 onBlur=my_valid_dscp(this)></TD>    

                <TD align=middle height=30>

                <INPUT type=checkbox name=enable_0 value=1></TD>    

              </TR>

              <TR>

                <TD align=middle height=30 width="151"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=dstipaddr_1_0 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_1_1 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_1_2 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_1_3 onBlur=my_valid_range(this,0,253,"DstIp") value=''></FONT></TD>

                <TD align=middle height=30 width="145"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=dstnetmask_1_0 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_1_1 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_1_2 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_1_3 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''></FONT></TD>

                <TD align=middle height=30 width="146"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=srcipaddr_1_0 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_1_1 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_1_2 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_1_3 onBlur=my_valid_range(this,0,253,"SrcIp") value=''></FONT></TD>

                <TD align=middle height=30 width="148"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=srcnetmask_1_0 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_1_1 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_1_2 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_1_3 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''></FONT></TD>

                <TD align=middle height=30><FONT size=2>

				<INPUT class=num maxLength=17 size=17 name=srcmac_1 onBlur=my_valid_macs_17(this) value=''></FONT></TD>

                <TD align=middle height=30><SELECT size=1 name=pro_1 onChange=SelPro(this.selectedIndex,this.form,1)>



				<OPTION value=0 ><script>Capture(share.alls)</script></OPTION>

				<OPTION value=1 >TCP</OPTION>

				<OPTION value=2 >UDP</OPTION>



			</SELECT></TD>

                <TD align=center height=30>

				<INPUT class=num maxLength=5 size=5 name=dstport_1 onBlur=my_valid_range(this,0,65535,"DstPort")></TD>

                <TD align=center height=30>

				<INPUT class=num maxLength=5 size=5 name=srcport_1 onBlur=my_valid_range(this,0,65535,"SrcPort")></TD>

                <TD align=middle height=30>

 				0x<INPUT class=num maxLength=2 size=2 name=cos_1 onBlur=my_valid_cos(this)></TD>    

                <TD align=middle height=30>

                0x<INPUT class=num maxLength=4 size=4 name=type_1 onBlur=my_valid_type(this)></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=4 size=4 name=vid_1 onBlur=my_valid_range(this,0,4095,"VlanID")></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=5 size=5 name=lengthmin_1 onBlur=my_valid_range(this,0,65535,"Packet%20length%20min%20value")></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=5 size=5 name=lengthmax_1 onBlur=my_valid_range(this,0,65535,"Packet%20length%20max%20value")></TD> 
                <TD align=middle height=30>

                0x<INPUT class=num maxLength=2 size=2 name=tos_1 onBlur=my_valid_dscp(this)></TD>    

                <TD align=middle height=30>

                <INPUT type=checkbox name=enable_1 value=1></TD>    

              </TR>

              <TR>

                <TD align=middle height=30 width="151"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=dstipaddr_2_0 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_2_1 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_2_2 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_2_3 onBlur=my_valid_range(this,0,253,"DstIp") value=''></FONT></TD>

                <TD align=middle height=30 width="145"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=dstnetmask_2_0 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_2_1 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_2_2 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_2_3 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''></FONT></TD>

                <TD align=middle height=30 width="146"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=srcipaddr_2_0 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_2_1 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_2_2 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_2_3 onBlur=my_valid_range(this,0,253,"SrcIp") value=''></FONT></TD>

                <TD align=middle height=30 width="148"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=srcnetmask_2_0 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_2_1 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_2_2 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_2_3 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''></FONT></TD>

                <TD align=middle height=30><FONT size=2>

				<INPUT class=num maxLength=17 size=17 name=srcmac_2 onBlur=my_valid_macs_17(this) value=''></FONT></TD>

                <TD align=middle height=30><SELECT size=1 name=pro_2 onChange=SelPro(this.selectedIndex,this.form,2)>



				<OPTION value=0 ><script>Capture(share.alls)</script></OPTION>

				<OPTION value=1 >TCP</OPTION>

				<OPTION value=2 >UDP</OPTION>



			</SELECT></TD>

                <TD align=center height=30>

				<INPUT class=num maxLength=5 size=5 name=dstport_2 onBlur=my_valid_range(this,0,65535,"DstPort")></TD>

                <TD align=center height=30>

				<INPUT class=num maxLength=5 size=5 name=srcport_2 onBlur=my_valid_range(this,0,65535,"SrcPort")></TD>

                <TD align=middle height=30>

                0x<INPUT class=num maxLength=2 size=2 name=cos_2 onBlur=my_valid_cos(this)></TD>    

                <TD align=middle height=30>

                0x<INPUT class=num maxLength=4 size=4 name=type_2 onBlur=my_valid_type(this)></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=4 size=4 name=vid_2 onBlur=my_valid_range(this,0,4095,"VlanID")></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=5 size=5 name=lengthmin_2 onBlur=my_valid_range(this,0,65535,"Packet%20length%20min%20value")></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=5 size=5 name=lengthmax_2 onBlur=my_valid_range(this,0,65535,"Packet%20length%20max%20value")></TD>   

                <TD align=middle height=30>

                0x<INPUT class=num maxLength=2 size=2 name=tos_2 onBlur=my_valid_dscp(this)></TD>    

                <TD align=middle height=30>

                <INPUT type=checkbox name=enable_2 value=1></TD>    

              </TR>

              <TR>

                <TD align=middle height=30 width="151"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=dstipaddr_3_0 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_3_1 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_3_2 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_3_3 onBlur=my_valid_range(this,0,253,"DstIp") value=''></FONT></TD>

                <TD align=middle height=30 width="145"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=dstnetmask_3_0 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_3_1 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_3_2 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_3_3 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''></FONT></TD>

                <TD align=middle height=30 width="146"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=srcipaddr_3_0 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_3_1 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_3_2 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_3_3 onBlur=my_valid_range(this,0,253,"SrcIp") value=''></FONT></TD>

                <TD align=middle height=30 width="148"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=srcnetmask_3_0 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_3_1 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_3_2 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_3_3 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''></FONT></TD>

                <TD align=middle height=30><FONT size=2>

				<INPUT class=num maxLength=17 size=17 name=srcmac_3 onBlur=my_valid_macs_17(this) value=''></FONT></TD>

                <TD align=middle height=30><SELECT size=1 name=pro_3 onChange=SelPro(this.selectedIndex,this.form,3)>



				<OPTION value=0 ><script>Capture(share.alls)</script></OPTION>

				<OPTION value=1 >TCP</OPTION>

				<OPTION value=2 ><UDP</OPTION>



			</SELECT></TD>

                <TD align=center height=30>

				<INPUT class=num maxLength=5 size=5 name=dstport_3 onBlur=my_valid_range(this,0,65535,"DstPort")></TD>

                <TD align=center height=30>

				<INPUT class=num maxLength=5 size=5 name=srcport_3 onBlur=my_valid_range(this,0,65535,"SrcPort")></TD>

                <TD align=middle height=30>

                0x<INPUT class=num maxLength=2 size=2 name=cos_3 onBlur=my_valid_cos(this)></TD>    

                <TD align=middle height=30>

                0x<INPUT class=num maxLength=4 size=4 name=type_3 onBlur=my_valid_type(this)></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=4 size=4 name=vid_3 onBlur=my_valid_range(this,0,4095,"VlanID")></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=5 size=5 name=lengthmin_3 onBlur=my_valid_range(this,0,65535,"Packet%20length%20min%20value")></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=5 size=5 name=lengthmax_3 onBlur=my_valid_range(this,0,65535,"Packet%20length%20max%20value")></TD> 
                <TD align=middle height=30>

                0x<INPUT class=num maxLength=2 size=2 name=tos_3 onBlur=my_valid_dscp(this)></TD>    

                <TD align=middle height=30>

                <INPUT type=checkbox name=enable_3 value=1></TD>    

              </TR>

              <TR>

                <TD align=middle height=30 width="151"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=dstipaddr_4_0 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_4_1 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_4_2 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_4_3 onBlur=my_valid_range(this,0,253,"DstIp") value=''></FONT></TD>

                <TD align=middle height=30 width="145"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=dstnetmask_4_0 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_4_1 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_4_2 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_4_3 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''></FONT></TD>

                <TD align=middle height=30 width="146"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=srcipaddr_4_0 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_4_1 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_4_2 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_4_3 onBlur=my_valid_range(this,0,253,"SrcIp") value=''></FONT></TD>

                <TD align=middle height=30 width="148"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=srcnetmask_4_0 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_4_1 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_4_2 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_4_3 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''></FONT></TD>

                <TD align=middle height=30><FONT size=2>

				<INPUT class=num maxLength=17 size=17 name=srcmac_4 onBlur=my_valid_macs_17(this) value=''></FONT></TD>

                <TD align=middle height=30><SELECT size=1 name=pro_4 onChange=SelPro(this.selectedIndex,this.form,4)>



				<OPTION value=0 ><script>Capture(share.alls)</script></OPTION>

				<OPTION value=1 >TCP</OPTION>

				<OPTION value=2 >UDP</OPTION>



			</SELECT></TD>

                <TD align=center height=30>

				<INPUT class=num maxLength=5 size=5 name=dstport_4 onBlur=my_valid_range(this,0,65535,"DstPort")></TD>

                <TD align=center height=30>

				<INPUT class=num maxLength=5 size=5 name=srcport_4 onBlur=my_valid_range(this,0,65535,"SrcPort")></TD>

                <TD align=middle height=30>

                0x<INPUT class=num maxLength=2 size=2 name=cos_4 onBlur=my_valid_cos(this)></TD>    

                <TD align=middle height=30>

                0x<INPUT class=num maxLength=4 size=4 name=type_4 onBlur=my_valid_type(this)></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=4 size=4 name=vid_4 onBlur=my_valid_range(this,0,4095,"VlanID")></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=5 size=5 name=lengthmin_4 onBlur=my_valid_range(this,0,65535,"Packet%20length%20min%20value")></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=5 size=5 name=lengthmax_4 onBlur=my_valid_range(this,0,65535,"Packet%20length%20max%20value")></TD>  

                <TD align=middle height=30>

                0x<INPUT class=num maxLength=2 size=2 name=tos_4 onBlur=my_valid_dscp(this)></TD>    

                <TD align=middle height=30>

                <INPUT type=checkbox name=enable_4 value=1></TD>    

              </TR>

              <TR>

                <TD align=middle height=30 width="151"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=dstipaddr_5_0 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_5_1 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_5_2 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_5_3 onBlur=my_valid_range(this,0,253,"DstIp") value=''></FONT></TD>

                <TD align=middle height=30 width="145"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=dstnetmask_5_0 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_5_1 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_5_2 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_5_3 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''></FONT></TD>

                <TD align=middle height=30 width="146"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=srcipaddr_5_0 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_5_1 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_5_2 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_5_3 onBlur=my_valid_range(this,0,253,"SrcIp") value=''></FONT></TD>

                <TD align=middle height=30 width="148"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=srcnetmask_5_0 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_5_1 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_5_2 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_5_3 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''></FONT></TD>

                <TD align=middle height=30><FONT size=2>

				<INPUT class=num maxLength=17 size=17 name=srcmac_5 onBlur=my_valid_macs_17(this) value=''></FONT></TD>

                <TD align=middle height=30><SELECT size=1 name=pro_5 onChange=SelPro(this.selectedIndex,this.form,5)>



				<OPTION value=0 ><script>Capture(share.alls)</script></OPTION>

				<OPTION value=1 >TCP</OPTION>

				<OPTION value=2 >UDP</OPTION>



			</SELECT></TD>

                <TD align=center height=30>

				<INPUT class=num maxLength=5 size=5 name=dstport_5 onBlur=my_valid_range(this,0,65535,"DstPort")></TD>

                <TD align=center height=30>

				<INPUT class=num maxLength=5 size=5 name=srcport_5 onBlur=my_valid_range(this,0,65535,"SrcPort")></TD>

                <TD align=middle height=30>

                0x<INPUT class=num maxLength=2 size=2 name=cos_5 onBlur=my_valid_cos(this)></TD>    

                <TD align=middle height=30>

                0x<INPUT class=num maxLength=4 size=4 name=type_5 onBlur=my_valid_type(this)></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=4 size=4 name=vid_5 onBlur=my_valid_range(this,0,4095,"VlanID")></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=5 size=5 name=lengthmin_5 onBlur=my_valid_range(this,0,65535,"Packet%20length%20min%20value")></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=5 size=5 name=lengthmax_5 onBlur=my_valid_range(this,0,65535,"Packet%20length%20max%20value")></TD>  

                <TD align=middle height=30>

                0x<INPUT class=num maxLength=2 size=2 name=tos_5 onBlur=my_valid_dscp(this)></TD>    

                <TD align=middle height=30>

                <INPUT type=checkbox name=enable_5 value=1></TD>    

              </TR>

              <TR>

                <TD align=middle height=30 width="151"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=dstipaddr_6_0 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_6_1 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_6_2 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_6_3 onBlur=my_valid_range(this,0,253,"DstIp") value=''></FONT></TD>

                <TD align=middle height=30 width="145"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=dstnetmask_6_0 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_6_1 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_6_2 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_6_3 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''></FONT></TD>

                <TD align=middle height=30 width="146"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=srcipaddr_6_0 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_6_1 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_6_2 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_6_3 onBlur=my_valid_range(this,0,253,"SrcIp") value=''></FONT></TD>

                <TD align=middle height=30 width="148"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=srcnetmask_6_0 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_6_1 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_6_2 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_6_3 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''></FONT></TD>

                <TD align=middle height=30><FONT size=2>

				<INPUT class=num maxLength=17 size=17 name=srcmac_6 onBlur=my_valid_macs_17(this) value=''></FONT></TD>

                <TD align=middle height=30><SELECT size=1 name=pro_6 onChange=SelPro(this.selectedIndex,this.form,6)>



				<OPTION value=0 ><script>Capture(share.alls)</script></OPTION>

				<OPTION value=1 >TCP</OPTION>

				<OPTION value=2 >UDP</OPTION>



			</SELECT></TD>

                <TD align=center height=30>

				<INPUT class=num maxLength=5 size=5 name=dstport_6 onBlur=my_valid_range(this,0,65535,"DstPort")></TD>

                <TD align=center height=30>

				<INPUT class=num maxLength=5 size=5 name=srcport_6 onBlur=my_valid_range(this,0,65535,"SrcPort")></TD>

                <TD align=middle height=30>

                0x<INPUT class=num maxLength=2 size=2 name=cos_6 onBlur=my_valid_cos(this)></TD>    

                <TD align=middle height=30>

                0x<INPUT class=num maxLength=4 size=4 name=type_6 onBlur=my_valid_type(this)></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=4 size=4 name=vid_6 onBlur=my_valid_range(this,0,4095,"VlanID")></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=5 size=5 name=lengthmin_6 onBlur=my_valid_range(this,0,65535,"Packet%20length%20min%20value")></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=5 size=5 name=lengthmax_6 onBlur=my_valid_range(this,0,65535,"Packet%20length%20max%20value")></TD> 
                <TD align=middle height=30>

                0x<INPUT class=num maxLength=2 size=2 name=tos_6 onBlur=my_valid_dscp(this)></TD>    

                <TD align=middle height=30>

                <INPUT type=checkbox name=enable_6 value=1></TD>    

              </TR>

              <TR>

                <TD align=middle height=30 width="151"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=dstipaddr_7_0 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_7_1 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_7_2 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_7_3 onBlur=my_valid_range(this,0,253,"DstIp") value=''></FONT></TD>

                <TD align=middle height=30 width="145"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=dstnetmask_7_0 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_7_1 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_7_2 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_7_3 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''></FONT></TD>

                <TD align=middle height=30 width="146"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=srcipaddr_7_0 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_7_1 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_7_2 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_7_3 onBlur=my_valid_range(this,0,253,"SrcIp") value=''></FONT></TD>

                <TD align=middle height=30 width="148"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=srcnetmask_7_0 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_7_1 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_7_2 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_7_3 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''></FONT></TD>

                <TD align=middle height=30><FONT size=2>

				<INPUT class=num maxLength=17 size=17 name=srcmac_7 onBlur=my_valid_macs_17(this) value=''></FONT></TD>

                <TD align=middle height=30><SELECT size=1 name=pro_7 onChange=SelPro(this.selectedIndex,this.form,7)>



				<OPTION value=0 ><script>Capture(share.alls)</script></OPTION>

				<OPTION value=1 >TCP</OPTION>

				<OPTION value=2 >UDP</OPTION>



			</SELECT></TD>

                <TD align=center height=30>

				<INPUT class=num maxLength=5 size=5 name=dstport_7 onBlur=my_valid_range(this,0,65535,"DstPort")></TD>

                <TD align=center height=30>

				<INPUT class=num maxLength=5 size=5 name=srcport_7 onBlur=my_valid_range(this,0,65535,"SrcPort")></TD>

                <TD align=middle height=30>

                0x<INPUT class=num maxLength=2 size=2 name=cos_7 onBlur=my_valid_cos(this)></TD>    

                <TD align=middle height=30>

                0x<INPUT class=num maxLength=4 size=4 name=type_7 onBlur=my_valid_type(this)></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=4 size=4 name=vid_7 onBlur=my_valid_range(this,0,4095,"VlanID")></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=5 size=5 name=lengthmin_7 onBlur=my_valid_range(this,0,65535,"Packet%20length%20min%20value")></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=5 size=5 name=lengthmax_7 onBlur=my_valid_range(this,0,65535,"Packet%20length%20max%20value")></TD>  

                <TD align=middle height=30>

                0x<INPUT class=num maxLength=2 size=2 name=tos_7 onBlur=my_valid_dscp(this)></TD>    

                <TD align=middle height=30>

                <INPUT type=checkbox name=enable_7 value=1></TD>    

              </TR>

              <TR>

                <TD align=middle height=30 width="151"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=dstipaddr_8_0 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_8_1 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_8_2 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_8_3 onBlur=my_valid_range(this,0,253,"DstIp") value=''></FONT></TD>

                <TD align=middle height=30 width="145"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=dstnetmask_8_0 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_8_1 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_8_2 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_8_3 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''></FONT></TD>

                <TD align=middle height=30 width="146"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=srcipaddr_8_0 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_8_1 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_8_2 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_8_3 onBlur=my_valid_range(this,0,253,"SrcIp") value=''></FONT></TD>

                <TD align=middle height=30 width="148"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=srcnetmask_8_0 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_8_1 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_8_2 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_8_3 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''></FONT></TD>

                <TD align=middle height=30><FONT size=2>

				<INPUT class=num maxLength=17 size=17 name=srcmac_8 onBlur=my_valid_macs_17(this) value=''></FONT></TD>

                <TD align=middle height=30><SELECT size=1 name=pro_8 onChange=SelPro(this.selectedIndex,this.form,8)>



				<OPTION value=0 ><script>Capture(share.alls)</script></OPTION>

				<OPTION value=1 >TCP</OPTION>

				<OPTION value=2 >UDP</OPTION>



			</SELECT></TD>

                <TD align=center height=30>

				<INPUT class=num maxLength=5 size=5 name=dstport_8 onBlur=my_valid_range(this,0,65535,"DstPort")></TD>

                <TD align=center height=30>

				<INPUT class=num maxLength=5 size=5 name=srcport_8 onBlur=my_valid_range(this,0,65535,"SrcPort")></TD>

                <TD align=middle height=30>

                0x<INPUT class=num maxLength=2 size=2 name=cos_8 onBlur=my_valid_cos(this)></TD>    

                <TD align=middle height=30>

                0x<INPUT class=num maxLength=4 size=4 name=type_8 onBlur=my_valid_type(this)></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=4 size=4 name=vid_8 onBlur=my_valid_range(this,0,4095,"VlanID")></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=5 size=5 name=lengthmin_8 onBlur=my_valid_range(this,0,65535,"Packet%20length%20min%20value")></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=5 size=5 name=lengthmax_8 onBlur=my_valid_range(this,0,65535,"Packet%20length%20max%20value")></TD> 
                <TD align=middle height=30>

                0x<INPUT class=num maxLength=2 size=2 name=tos_8 onBlur=my_valid_dscp(this)></TD>    

                <TD align=middle height=30>

                <INPUT type=checkbox name=enable_8 value=1></TD>    

              </TR>

              <TR>

                <TD align=middle height=30 width="151"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=dstipaddr_9_0 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_9_1 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_9_2 onBlur=my_valid_range(this,0,253,"DstIp") value=''>.<INPUT class=num maxLength=3 size=3 name=dstipaddr_9_3 onBlur=my_valid_range(this,0,253,"DstIp") value=''></FONT></TD>

                <TD align=middle height=30 width="145"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=dstnetmask_9_0 my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_9_1 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_9_2 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=dstnetmask_9_3 onBlur=my_valid_range(this,0,255,"DstNetmask") value=''></FONT></TD>

                <TD align=middle height=30 width="146"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=srcipaddr_9_0 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_9_1 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_9_2 onBlur=my_valid_range(this,0,253,"SrcIp") value=''>.<INPUT class=num maxLength=3 size=3 name=srcipaddr_9_3 onBlur=my_valid_range(this,0,253,"SrcIp") value=''></FONT></TD>

                <TD align=middle height=30 width="148"><FONT size=2>

				<INPUT class=num maxLength=3 size=3 name=srcnetmask_9_0 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_9_1 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_9_2 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''>.<INPUT class=num maxLength=3 size=3 name=srcnetmask_9_3 onBlur=my_valid_range(this,0,255,"SrcNetmask") value=''></FONT></TD>

                <TD align=middle height=30><FONT size=2>

				<INPUT class=num maxLength=17 size=17 name=srcmac_9 onBlur=my_valid_macs_17(this) value=''></FONT></TD>

                <TD align=middle height=30><SELECT size=1 name=pro_9 onChange=SelPro(this.selectedIndex,this.form,9)>



				<OPTION value=0 ><script>Capture(share.alls)</script></OPTION>

				<OPTION value=1 >TCP</OPTION>

				<OPTION value=2 >UDP</OPTION>



			</SELECT></TD>

                <TD align=center height=30>

				<INPUT class=num maxLength=5 size=5 name=dstport_9 onBlur=my_valid_range(this,0,65535,"DstPort")></TD>

                <TD align=center height=30>

				<INPUT class=num maxLength=5 size=5 name=srcport_9 onBlur=my_valid_range(this,0,65535,"SrcPort")></TD>

                <TD align=middle height=30>

                0x<INPUT class=num maxLength=2 size=2 name=cos_9 onBlur=my_valid_cos(this)></TD>    

                <TD align=middle height=30>

                0x<INPUT class=num maxLength=4 size=4 name=type_9 onBlur=my_valid_type(this)></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=4 size=4 name=vid_9 onBlur=my_valid_range(this,0,4095,"VlanID")></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=5 size=5 name=lengthmin_9 onBlur=my_valid_range(this,0,65535,"Packet%20length%20min%20value")></TD>    

                <TD align=middle height=30>

                <INPUT class=num maxLength=5 size=5 name=lengthmax_9 onBlur=my_valid_range(this,0,65535,"Packet%20length%20max%20value")></TD> 

                <TD align=middle height=30>

                0x<INPUT class=num maxLength=2 size=2 name=tos_9 onBlur=my_valid_dscp(this)></TD>    

                <TD align=middle height=30>

                <INPUT type=checkbox name=enable_9 value=1></TD>    

              </TR>

              </TBODY></TABLE></TD>

          </TR></TBODY></TABLE></TD>

    </TR> 

  

  </TBODY></TABLE>

  <p>&nbsp;</p>
<p>

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;

<input type="button" value=" Save " name="save_button" onClick=to_submit(this.form)>&nbsp;&nbsp;&nbsp;

<input type="button" value="Cancel" name="can_button" onClick="window.location.replace('PVC_Selection.asp')">&nbsp;&nbsp;&nbsp;

<input type="button" value="Close" name="close_buttong" onclick="self.close()"></p>



  </FORM></DIV></BODY></HTML>
