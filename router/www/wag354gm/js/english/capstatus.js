var statopmenu = new Object();
statopmenu.gateway="Gateway";

statopmenu.localnet="Local Network";

statopmenu.wireless="Wireless";

statopmenu.dslconn="DSL Connection";

var staleftmenu = new Object();
//Gateway
staleftmenu.gwinfo="Gateway Information";

staleftmenu.inetconn="Internet Connection";

//Local Network
staleftmenu.localnet="Local Network";

//Wireless
staleftmenu.wireless="Wireless";

//DSL Connection
staleftmenu.dslstatus="DSL Status";

staleftmenu.pvcconn="PVC Connection";

//Gateway Information
var stagwinfo = new Object();
stagwinfo.firmwarever="Firmware Version";

stagwinfo.macaddr="MAC Address";

stagwinfo.curtime="Current Time";

var stainetconn = new Object();
stainetconn.connection="Connection";

stainetconn.logintype="Login Type";

stainetconn.iface="Interface";

stainetconn.ipaddr="IP Address";

stainetconn.subnetmask="Subnet Mask";

stainetconn.defaultgw="Default Gateway";

stainetconn.dns1="DNS 1";

stainetconn.dns2="DNS 2";

stainetconn.dns3="DNS 3";

stainetconn.wins="WINS";

stainetconn.svcname="Service Name";

stainetconn.acname="AC Name";

//Local Network
var stalocalnet = new Object();
stalocalnet.macaddr="MAC Address";

stalocalnet.ipaddr="IP Address";

stalocalnet.subnetmask="Subnet Mask";

stalocalnet.dhcpsvr="DHCP Server";

stalocalnet.startipaddr="Start IP Address";

stalocalnet.endipaddr="End IP Address";

//Wireless
var stawireless = new Object();
stawireless.firmwarever="Firmware Version";

stawireless.macaddr="MAC Address";

stawireless.mode="Mode";

stawireless.mixed="Mixed";

stawireless.gonly="G-only";

stawireless.bonly="B-only";

stawireless.ssid="SSID";

stawireless.dhcpsvr="DHCP Server";

stawireless.channel="Channel";

stawireless.encryptfunc="Encryption Function";

stawireless.enabled="Enabled";

stawireless.disabled="Disabled"

//DSL Status
var stadslstatus = new Object();
stadslstatus.dslstatus="DSL Status";

stadslstatus.dslmodulmode="DSL Modulation Mode";

stadslstatus.dslpathmode="DSL Path Mode";

stadslstatus.downrate="Downstream Rate";

stadslstatus.uprate="Upstream Rate";

stadslstatus.downmargin="Downstream Margin";

stadslstatus.upmargin="Upstream Margin";

stadslstatus.downlineatn="Downstream Line Attenuation";

stadslstatus.uplineatn="Upstream Line Attenuation";

stadslstatus.downtranspower="Downstream Transmit Power";

stadslstatus.uptranspower="Upstream Transmit Power";

//PVC Connection
var stapvcconn = new Object();
stapvcconn.connect="Connection";

stapvcconn.encaps="Encapsulation";

stapvcconn.multiplex="Multiplexing";

stapvcconn.qos="QoS";

stapvcconn.pcrrate="PCR Rate";

stapvcconn.scrrate="SCR Rate";

stapvcconn.autodetect="Autodetect";

stapvcconn.vpi="VPI";

stapvcconn.vci="VCI";

stapvcconn.enable="Enable";

stapvcconn.pvcstatus="PVC Status";

//DHCP Client Table
var stadhcptbl = new Object();
stadhcptbl.dhcpactiveiptbl="DHCP Active IP Table";

stadhcptbl.dhcpsrvipaddr="DHCP Sever IP Address";

stadhcptbl.clienthostname="Client Host Name";

stadhcptbl.ipaddr="IP Address";

stadhcptbl.macaddr="MAC Address";

stadhcptbl.expires="Expires";

stadhcptbl.del="Delete";

stadhcptbl.closes="Close";

//DNS Entries Table
var stadnsentrytbl = new Object();
stadnsentrytbl.dnsentrytbl="DNS Entries Table";

stadnsentrytbl.domainname="Domain Name";

stadnsentrytbl.dnsipaddr="DNS IP Addr";

stadnsentrytbl.ipaddr="IP Address";

stadnsentrytbl.selects="Select";

stadnsentrytbl.dsldevice="dsldevice";

//Wireless Clients Connected
var stawltbl = new Object();
stawltbl.networkedcomputers="Networked Computers";

stawltbl.computername="Computer Name";

stawltbl.ipaddr="IP Address";

stawltbl.macaddr="MAC Address";

//ARP/RARP Table
var staarptbl = new Object();
staarptbl.arptbl="ARP/RARP Table";

//Status' buttons
var stabutton = new Object();
stabutton.connect="Connect";

stabutton.disconnect="Disconnect";

stabutton.dhcptbl="DHCP Client Table";

stabutton.dnsentrytbl="DNS Entries Table";

stabutton.arptbl="ARP/RARP Table";

stabutton.wlclntconn="Wireless Clients Connected";

stabutton.refresh="Refresh";

stabutton.closes="Close";

stabutton.dhcprelease="DHCP Release";

stabutton.dhcprenew="DHCP Renew";
