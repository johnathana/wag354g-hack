
//
// *********************************************************
// *   Copyright 2003, CyberTAN  Inc.  All Rights Reserved *
// *********************************************************
//
// This is UNPUBLISHED PROPRIETARY SOURCE CODE of CyberTAN Inc.
// the contents of this file may not be disclosed to third parties,
// copied or duplicated in any form without the prior written
// permission of CyberTAN Inc.
//
// This software should be used as a reference only, and it not
// intended for production use!
//
//
// THIS SOFTWARE IS OFFERED "AS IS", AND CYBERTAN GRANTS NO WARRANTIES OF ANY
// KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.  CYBERTAN
// SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE

ie4 = ((navigator.appName == "Microsoft Internet Explorer") && (parseInt(navigator.appVersion) >= 4 ))
ns4 = ((navigator.appName == "Netscape") && (parseInt(navigator.appVersion) < 6 ))
ns6 = ((navigator.appName == "Netscape") && (parseInt(navigator.appVersion) >= 6 ))

// 0.0.0.0
var ZERO_NO = 1;	// 0x0000 0001
var ZERO_OK = 2;	// 0x0000 0010
// x.x.x.0
var MASK_NO = 4;	// 0x0000 0100
var MASK_OK = 8;	// 0x0000 1000
// 255.255.255.255
var BCST_NO = 16;	// 0x0001 0000
var BCST_OK = 32;	// 0x0010 0000

var SPACE_NO = 1;
var SPACE_OK = 2;

function choose_enable(en_object)
{
	if(!en_object)	return;
	en_object.disabled = false;			// netscape 4.x can not work, but 6.x can work

	if(!ns4)
		en_object.style.backgroundColor = "";	// netscape 4.x have error
}

function vpiCheck(I)
{	
	d =parseInt(I.value, 10);	
	if ( !(d>=0 && d<=255) )	
	{		
		alert('VPI value is out of range [0 - 255]');		
		return false;
	}
	return true;
}

function vciCheck(I)
{	
	d =parseInt(I.value, 10);	
	if ( !(d>=1 && d<=65535) )	
	{		
		alert('VCI value is out of range [1 - 65535]');		
		return false;
	}
	return true;
}

function valid_vpi(I)
{
	d =parseInt(I.value, 10);
	if ( !(d>=0 && d<=255) )
	{
		alert('VPI value is out of range [0 - 255]');
		return false;
	}
	return true;
}

function valid_vci(I)
{
	d =parseInt(I.value, 10);
	if ( !(d>=1 && d<=65535) )
	{
		alert('VCI value is out of range [1 - 65535]');
		return false;
	}
	else if ( d==5 )
	{	
		alert('VCI value can Not be 5');	
		return false;
	}	
	else if ( d==16 )
	{	
		alert('VCI value can Not be 16');	
		return false;
	}
	return true;
}

function choose_disable(dis_object)
{
	if(!dis_object)	return;
	dis_object.disabled = true;

	if(!ns4)
		dis_object.style.backgroundColor = "#e0e0e0";
}
function check_action(I,N)
{
	if(ns4){	//ie.  will not need and will have question in "select"
		if(N == 0){
			if(EN_DIS == 1) I.focus();
			else I.blur();
		}
		else if(N == 1){
			if(EN_DIS1 == 1) I.focus();
			else I.blur();
		}
		else if(N == 2){
			if(EN_DIS2 == 1) I.focus();
			else I.blur();
		}
		else if(N == 3){
			if(EN_DIS3 == 1) I.focus();
			else I.blur();
		}
			
	}
}
function check_action1(I,T,N)
{
	if(ns4){	//ie.  will not need and will have question in "select"
		if(N == 0){
			if(EN_DIS == 1) I.focus();
			else I.value = I.defaultChecked;
		}
		if(N == 1){
			if(EN_DIS1 == 1) I.focus();
			else I.value = I.defaultChecked;
		}
	}
}
function valid_range(I,start,end,M)
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
		alert(M1 +' value is out of range ['+ start + ' - ' + end +']');
		I.value = I.defaultValue;
		return false;	
	}
	else
	{
		I.value = d;	// strip 0
		return true;
	}

}
function valid_mac(I,T)
{
	var m1,m2=0;

	if(I.value.length == 1)
		I.value = "0" + I.value;

	m1 =parseInt(I.value.charAt(0), 16);
	m2 =parseInt(I.value.charAt(1), 16);
	if( isNaN(m1) || isNaN(m2) )
	{
		alert('The WAN MAC Address is out of range [00 - ff]');	
		I.value = I.defaultValue;
	}
	I.value = I.value.toUpperCase();

	if(T == 0)                                                              
        {                                                                       
		if((m2 & 1) == 1){                               
			alert('The second character of MAC must be even number : [0, 2, 4, 6, 8, A, C, E]');
			I.value = I.defaultValue;                       
		}                                                       
        }
                       
}
function valid_macs_12(I){	
	var m,m3;	
	if(I.value == "")
		return true;
//	if(I.value.length<2)		
//		I.value=0;	
	else if(I.value.length==12){
		for(i=0;i<12;i++){			
			m=parseInt(I.value.charAt(i), 16);			
			if( isNaN(m) )				
				break;		
		}		
		if( i!=12 ){
			alert('The MAC Address is not correct!!');
			I.value = I.defaultValue;		
		}	
	}	
	else{		
		alert('The MAC Address length is not correct!!');
		I.value = I.defaultValue;	
	}
	I.value = I.value.toUpperCase();
	if(I.value == "FFFFFFFFFFFF"){
		alert('The MAC Address cannot be the broadcast address!!');
		I.value = I.defaultValue;	
	}
	m3 = I.value.charAt(1);
	if((m3 & 1) == 1){                               
		alert('The second character of MAC must be even number : [0, 2, 4, 6, 8, A, C, E]');
		I.value = I.defaultValue;                       
	}                                                       
}
function valid_macs_17(I)
{
	oldmac = I.value;
	var mac = ignoreSpaces(oldmac);
	if (mac == "") 
	{
		return true;
		//alert("Enter MAC Address in (xx:xx:xx:xx:xx:xx) format");
		//return false;
	}
	var m = mac.split(":");
	if (m.length != 6) 
	{
		alert("Invalid MAC address format");
		I.value = I.defaultValue;		
		return false;
	}
	var idx = oldmac.indexOf(':');
	if (idx != -1) {
		var pairs = oldmac.substring(0, oldmac.length).split(':');
		for (var i=0; i<pairs.length; i++) {
			nameVal = pairs[i];
			len = nameVal.length;
			if (len < 1 || len > 2) {
				alert ("The WAN MAC Address is not correct!!");
				I.value = I.defaultValue;		
				return false;
			}
			for(iln = 0; iln < len; iln++) {
				ch = nameVal.charAt(iln).toLowerCase();
				if (ch >= '0' && ch <= '9' || ch >= 'a' && ch <= 'f') {
				}
				else {
					alert ("Invalid hex value " + nameVal + " found in MAC address " + oldmac);
					I.value = I.defaultValue;		
					return false;
				}
			}	
		}
	}
	I.value = I.value.toUpperCase();
	if(I.value == "FF:FF:FF:FF:FF:FF"){
		alert('The MAC Address cannot be the broadcast address!');
		I.value = I.defaultValue;	
	}
	m3 = I.value.charAt(1);
	if((m3 & 1) == 1){                               
		alert('The second character of MAC must be even number : [0, 2, 4, 6, 8, A, C, E]');
		I.value = I.defaultValue;                       
	}                                                       
	return true;
}
function ignoreSpaces(string) {
  var temp = "";

  string = '' + string;
  splitstring = string.split(" ");
  for(i = 0; i < splitstring.length; i++)
    temp += splitstring[i];
  return temp;
}

function check_comon(I,M1)
{
	M = unescape(M1);
	for(i = 0;i < I.value.length; i++)
	{
		ch = I.value.charAt(i);
		if(ch == '\'')
		{
			alert(M +' is not allow \'\'\'');
			I.value = I.defaultValue;	
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
			alert(M +' is not allow \':\'');
			//I.value = I.defaultValue;	
			return false;
		}
	}
	return true;
}
function check_space(I,M1){
	M = unescape(M1);
	for(i=0 ; i<I.value.length; i++){
		ch = I.value.charAt(i);
		if(ch == ' '){
			alert(M +' is not allow space!');
			I.value = I.defaultValue;	
			return false;
		}
	}
	return true;
}
function valid_key(I,l){	
	var m;	
	if(I.value.length==l*2)	{		
		for(i=0;i<l*2;i++){			 
			m=parseInt(I.value.charAt(i), 16);
			if( isNaN(m) )				
				break;		
		}		
		if( i!=l*2 ){		
			alert('The key value is not correct!!');			
			I.value = I.defaultValue;		
		}	
	}	
	else{		
		alert('The key length is not correct!!');		
		I.value = I.defaultValue;	
	}
}
function valid_name(I,M,flag)
{
	isascii(I,M);

	var bbb = I.value.replace(/^\s*/,"");
        var ccc = bbb.replace(/\s*$/,"");

        I.value = ccc;

	if(flag & SPACE_NO){
		check_space(I,M);
	}
	//check_colon(I,M);
	//check_comon(I,M);
}
function valid_mask(F,N,flag){
	var match0 = -1;
	var match1 = -1;
	var m = new Array(4);

	for(i=0;i<4;i++)
		m[i] = eval(N+"_"+i).value;

	if(m[0] == "0" && m[1] == "0" && m[2] == "0" && m[3] == "0"){
		if(flag & ZERO_NO){
			alert("Illegal subnet mask!");
			return false;
		}
		else if(flag & ZERO_OK){
			return true;
		}
	}

	if(m[0] == "255" && m[1] == "255" && m[2] == "255" && m[3] == "255"){
		if(flag & BCST_NO){
			alert("Illegal subnet mask!");
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
		alert("Illegal subnet mask!");
		return false;
	}
	return true;
}
function isdigit(I,M)
{
	for(i=0 ; i<I.value.length; i++){
		ch = I.value.charAt(i);
		if(ch < '0' || ch > '9'){
			alert(M +' have illegal characters, must be [ 0 - 9 ]');
			I.value = I.defaultValue;	
			return false;
		}
	}
	return true;
}
function isascii(I,M)
{
	for(i=0 ; i<I.value.length; i++){
		ch = I.value.charAt(i);
		if(ch < ' ' || ch > '~'){
			alert(M +' have illegal ascii code!');
			I.value = I.defaultValue;	
			return false;
		}
	}
	return true;
}
function isxdigit(I,M)
{
	for(i=0 ; i<I.value.length; i++){
		ch = I.value.charAt(i).toLowerCase();
		if(ch >= '0' && ch <= '9' || ch >= 'a' && ch <= 'f'){}
		else{
			alert(M +' have illegal hexadecimal digits!');
			I.value = I.defaultValue;	
			return false;
		}
	}
	return true;
}
function closeWin(var_win){
	if ( ((var_win != null) && (var_win.close)) || ((var_win != null) && (var_win.closed==false)) )
		var_win.close();
}
function valid_ip(F,N,M1,flag){
	var m = new Array(4);
	M = unescape(M1);

	for(i=0;i<4;i++)
		m[i] = eval(N+"_"+i).value

	if(m[0] == 127 || m[0] == 224){
		alert(M+" value is illegal!");
		return false;
	}

	if(m[0] == "0" && m[1] == "0" && m[2] == "0" && m[3] == "0"){
		if(flag & ZERO_NO){
			alert(M+' value is illegal!');
			return false;
		}
	}

	if((m[0] != "0" || m[1] != "0" || m[2] != "0") && m[3] == "0"){
		if(flag & MASK_NO){
			alert(M+' value is illegal!');
			return false;
		}
	}
	return true;
}
function valid_ip_gw(F,I,N,G)
{
	var IP = new Array(4);
	var NM = new Array(4);
	var GW = new Array(4);
	
	for(i=0;i<4;i++)
		IP[i] = eval(I+"_"+i).value
	for(i=0;i<4;i++)
		NM[i] = eval(N+"_"+i).value
	for(i=0;i<4;i++)
		GW[i] = eval(G+"_"+i).value

	for(i=0;i<4;i++){
		if((IP[i] & NM[i]) != (GW[i] & NM[i])){
			alert("IP address and gateway is not at same subnet mask!");
			return false;
		}
	}
	if((IP[0] == GW[0]) && (IP[1] == GW[1]) && (IP[2] == GW[2]) && (IP[3] == GW[3])){
		alert("IP address and gateway can't be same!");
		return false;
	}
	
	return true;
}
