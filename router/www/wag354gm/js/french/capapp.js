var apptopmenu = new Object();
apptopmenu.forward="Connexion";

apptopmenu.singleport="Transfert de connexion unique";

apptopmenu.portrange="Transfert de connexion";

apptopmenu.porttrigger="D�clenchement de connexion";

apptopmenu.dmz="DMZ";

apptopmenu.qos=" QS";

var appleftmenu = new Object();
appleftmenu.singleport="Transfert de connexion unique";

appleftmenu.pvcselect="S�lectionnez une connexion PVC";

appleftmenu.portmaplist="Transfert de connexion";

appleftmenu.portrange="Transfert de connexion";

appleftmenu.porttrigger="D�clenchement de connexion";

appleftmenu.dmz="DMZ";

appleftmenu.qos="QS";

appleftmenu.appbaseqos="QS par application";

appleftmenu.advqos="QS avanc�e";


var appsingleport = new Object();
appsingleport.pvcselect="S�lectionnez une connexion pvc";

appsingleport.app="Application";

appsingleport.extport="Port externe";

appsingleport.intport="Port interne";

appsingleport.proto="Protocole";

appsingleport.ipaddr="Adresse IP";

appsingleport.enabled="Activ�";

appsingleport.tcp="TCP";

appsingleport.udp="UDP";


var appportrange = new Object();
appportrange.portrange="Plage des ports";

appportrange.app="Application";

appportrange.start="D�but";

appportrange.end="Fin";

appportrange.proto="Protocole";

appportrange.ipaddr="Adresse IP";

appportrange.enabled="Activer";

appportrange.to="�";

appportrange.tcp="TCP";

appportrange.udp="UDP";

appportrange.both="Les deux";

appportrange.advanced="Avanc�e";



var appadvport = new Object();
appadvport.srcs="Origine";

appadvport.dst="Destination";

appadvport.ipaddr="Adresse IP";

appadvport.startport="Port de d�part";

appadvport.endport="Port de fin";

appadvport.proto="Protocole";

appadvport.ipdomain="Adresse IP ou Domaine";

appadvport.domainname="Nom de domaine";

appadvport.enable="Activer";

var appporttrigger = new Object();
appporttrigger.triggerrange="Connexion d�clench�e";

appporttrigger.forwardrange="Connexion transf�r�e";

appporttrigger.app="Application";

appporttrigger.startport="Port de d�but";

appporttrigger.endport="Port de fin";

appporttrigger.enable="Activer";

appporttrigger.to="�";

var appdmz = new Object();
appdmz.hosting="H�bergement DMZ";

appdmz.dmzhostip="Adresse IP de l'h�te DMZ";

appdmz.validrange="(de 1 � 254)";

var appbaseqos = new Object();
appbaseqos.hpriority="Priorit� <br>�lev�e";

appbaseqos.mpriority="Priorit� <br>moyenne";

appbaseqos.lpriority="Priorit�<br>faible";

appbaseqos.ftp="FTP";

appbaseqos.http="HTTP";

appbaseqos.telnet="Telnet";

appbaseqos.smtp="SMTP";

appbaseqos.pop3="POP3";

appbaseqos.specport="Port";


var appadvqos = new Object();

appadvqos.fragsize="Fragmenter la taille des paquets du trafic AF et BE pour qu'elle soit �gale � celle du trafic EF";

appadvqos.bytes="octets";

appadvqos.enable8021p="Activer la planification de bits P. 802.1p.";

appadvqos.vlanvid="VID VLAN";
