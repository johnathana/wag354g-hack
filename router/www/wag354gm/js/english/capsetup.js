var settopmenu = new Object();
settopmenu.basicsetup="Basic Setup";

settopmenu.ddns="DDNS";

settopmenu.advrouting="Advanced Routing";

settopmenu.hotspot="Hot Spot";

settopmenu.voip="Voice";

var setleftmenu = new Object();
setleftmenu.inetsetup="Internet Setup";

setleftmenu.pvcconn="PVC Connection";

setleftmenu.inetconntype="Internet Connection Type";

setleftmenu.vcsetting="VC Settings";

setleftmenu.ipsetting="IP Settings";

setleftmenu.optsetting="Optional Settings";

setleftmenu.info="required by some ISPs";

setleftmenu.netsetup="Network Setup";

setleftmenu.routerip="Router IP";

setleftmenu.cloudsetting="CLOUD Setting";

setleftmenu.cloud="CLOUD";

setleftmenu.netaddress="Network Address";

setleftmenu.serversetting="Server Settings";

setleftmenu.timesetting="Time Setting";

setleftmenu.ddns="DDNS";

setleftmenu.advrouting="Advanced Routing";

setleftmenu.operamode="Operating Mode";

setleftmenu.dynrouting="Dynamic Routing";

setleftmenu.staticrouting="Static Routing";

setleftmenu.pvcrout="PVC Route Policy";

//index.asp
var setinetsetup = new Object();
setinetsetup.please="Please Select a Connection";

setinetsetup.enablenow="Enable Now";

setinetsetup.encap="Encapsulation";

setinetsetup.multiplexing="Multiplexing";

setinetsetup.qostype="Qos Type";

setinetsetup.pcrrate="PCR Rate";

setinetsetup.scrrate="SCR Rate";

setinetsetup.autodetect="Autodetect";

setinetsetup.vircir="Virtual Circuit";

setinetsetup.obtain="Obtain an IP Address Automatically";

setinetsetup.usefollow="Use the following IP Address";

setinetsetup.inetipaddr="Internet IP Address";

setinetsetup.subnetmask="Subnet Mask";

setinetsetup.pridns="Primary DNS";

setinetsetup.secdns="Second DNS";

setinetsetup.pppoesession="PPPoE Session";

setinetsetup.hostname="Host Name";

setinetsetup.doname="Domain Name";

setinetsetup.mtu="MTU";

setinetsetup.size="Size";

setinetsetup.symbol=".";

var setnetsetup = new Object();
setnetsetup.localip="Local IP Address";

setnetsetup.subnet="Subnet Mask";

setnetsetup.ldhcpserver="Local DHCP Server";

setnetsetup.dhcprelay="DHCP Relay";

setnetsetup.dhcprelayserver="DHCP Relay Server";

setnetsetup.autodhcp="AutoDetect LAN DHCP Server";

setnetsetup.startip="Starting IP Address";

setnetsetup.maxnum="Maximum Number of DHCP Users";

setnetsetup.clientlease="Client Lease Time";

setnetsetup.minutes="minutes";

setnetsetup.moneday="means one day";

setnetsetup.staticdns="Static DNS";

setnetsetup.wins="WINS";

setnetsetup.timezone="Time Zone";

setnetsetup.tinterval="Time Interval";

setnetsetup.seconds="seconds";

setnetsetup.lastinfo="Automatically adjust clock for daylight saving changes";

//DDNS.asp
var setddns = new Object();
setddns.service="DDNS Service";

setddns.org="DynDNS.org";

setddns.com="TZO.com";

setddns.username="User Name";

setddns.password="Password";

setddns.hostname="Host Name";

setddns.email="E-mail Address";

setddns.doname="Domain Name";

setddns.ipaddr="Internet IP Address";

setddns.statu="Status";

var ddnsm = new Object();

ddnsm.dyn_good="DDNS is updated successfully";

ddnsm.dyn_noupdate="DDNS is updated successfully";

ddnsm.dyn_nochg="DDNS is updated successfully";

ddnsm.dyn_badauth="Authorization fails (username or passwords)";

ddnsm.all_noip="No Internet connection";

ddnsm.dyn_yours="Username is not correct";
 
ddnsm.dyn_nohost="The hostname does not exist";

ddnsm.dyn_notfqdn="Domain Name is not correct";

ddnsm.dyn_abuse="The hostname is blocked by the DDNS server";

ddnsm.dyn_badsys="The system parameters are invalid";

ddnsm.dyn_badagent="This useragent has been blocked";

ddnsm.dyn_numhost="Too many or too few hosts found";

ddnsm.dyn_dnserr= "DNS error encountered";

ddnsm.dyn_911= "An unexpected error (1)";

ddnsm.dyn_999= "An unexpected error (2)";

ddnsm.dyn_donator="A feature requested is only available to donators, please donate";

ddnsm.dyn_strange= "Strange server response, are you connecting to the right server?";

ddnsm.dyn_uncode= "Unknown return code";
 
ddnsm.tzo_good= "Operation Complete";

ddnsm.tzo_noupdate="Operation Complete";

ddnsm.tzo_notfqdn= "Invalid Domain Name";

ddnsm.tzo_notmail= "Invalis Email";

ddnsm.tzo_notact= "Invalid Action";

ddnsm.tzo_notkey= "Invalid Key";

ddnsm.tzo_notip= "Invalid IP address";

ddnsm.tzo_dupfqdn= "Duplicate Domain Name";

ddnsm.tzo_fqdncre= "Domain Name has already been created for this domain name";

ddnsm.tzo_expired= "The account has expired";

ddnsm.tzo_error= "An unexpected server error";

ddnsm.all_closed= "DDNS server is currently closed";

ddnsm.all_resolving= "Domain name resolve fail";

ddnsm.all_errresolv= "Domain name resolve fail";

ddnsm.all_connecting= "Connect to server fail";

ddnsm.all_connectfail= "Connect to server fail";

ddnsm.all_disabled= "DDNS function is disabled";

//Routing.asp
var setadvrouting = new Object();
setadvrouting.nat="NAT";

setadvrouting.rip="RIP";

setadvrouting.compatible="Compatible";

setadvrouting.defaultroute="Send Default Route";

setadvrouting.multibroad="Multicast or Broadcast";

setadvrouting.broadcast="Broadcast";

setadvrouting.multicast="Multicast";

setadvrouting.transver="Transmit RIP Version";

setadvrouting.receiver="Receive RIP Version";

setadvrouting.selnum="Select set number";

setadvrouting.dstipaddr="Destination IP Address";

setadvrouting.subnet="Subnet Mask";

setadvrouting.hop="Hop Count";

setadvrouting.deletes="Delete This Entry";

setadvrouting.showtable="Show Routing Table";

setadvrouting.routingtable="Routing Table";

setadvrouting.pvcsetting="PVC Routing Setting";

setadvrouting.pvcpolicy="PVC Routing Policy";

//index_1483bridged.asp
var setother = new Object();
setother.pppoasetting="PPPoA Settings";

setother.condemand="Connect on Demand";

setother.maxidle="Max Idle Time";

setother.keepalive="Keep Alive";

setother.radial="Redial Period";

setother.pppoesetting="PPPoE Settings";

setother.servicename="Service Name";

setother.secondpppoe="Second PPPoE";

setother.matchs="Match Domain Name";

setother.macclone="MAC Clone";

setother.userdefine="User Defined Entry";

setother.clone="Clone Your PC's MAC";

setother.hotspot="Hot Spot";

setother.macaddrclone="MAC Address Clone";
