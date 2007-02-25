var admtopmenu = new Object();
admtopmenu.credval="Validation des r�f�rences";

admtopmenu.manage="Gestion";

admtopmenu.report="Rapport";

admtopmenu.diag="Diagnostics";

admtopmenu.facdef="Param�tres d'usine";

admtopmenu.bakrest="Sauvegarder/Restaurer";

admtopmenu.upgrade="Mise � jour du micrologiciel";

admtopmenu.reboot="Red�marrage";

var admleftmenu = new Object();
//admleftmenu.gwaccess="Acc�s � la passerelle";
admleftmenu.gwaccess="Acc�s local";

admleftmenu.localgwaccess="Acc�s local";

admleftmenu.remotegwaccess="Acc�s distant";

admleftmenu.remoteupgrade="Mise � jour � distance";

admleftmenu.snmp="SNMP";

admleftmenu.upnp="UPnP";

admleftmenu.tr064="TR064";

admleftmenu.tr069="TR-069";

admleftmenu.igmpproxy="Proxy IGMP";

admleftmenu.wlan="r�seau Sans Fil";

admleftmenu.report="Rapport";

admleftmenu.emailalt="Alertes par messagerie �lectronique";

admleftmenu.ping="Test Ping";

admleftmenu.pingparam="Param�tres";

admleftmenu.bakconfig="Sauvegarde de la configuration";

admleftmenu.restconfig="Restauration de la configuration";

admleftmenu.facdef="Param�tres d'usine";

admleftmenu.upgradewan="Mise � jour � partir du WAN";

admleftmenu.upgradelan="Mise � jour � partir du LAN";

admleftmenu.reboot="Red�marrage";


var admgwaccess = new Object();
admgwaccess.gwusername="Nom d'utilisateur";

admgwaccess.gwpassword="Mot de passe";

admgwaccess.reconfirm="Confirmation du mot de passe";

admgwaccess.remotemgt="Gestion � distance";

admgwaccess.mgtport="Num�ro de Port";

admgwaccess.allowip="Adresse Ip autoris�e";

admgwaccess.all="TOUS";

admgwaccess.ipaddr="Adresse IP";

admgwaccess.iprange="Plage IP";

admgwaccess.rmtupgrade="Mise � jour � distance";

admgwaccess.usehttps="Utiliser Https";


var admsnmp = new Object();
admsnmp.snmp="SNMP";

admsnmp.devname="Nom du p�riph�rique";

admsnmp.snmp_v1v2="Snmp V1/V2";

admsnmp.getcomm="Obtenir la communaut�";

admsnmp.setcomm="D�finir la communaut�";

admsnmp.snmp_v3="Snmp V3";

admsnmp.rwuser="Rw User (autoris� en lect/�criture) ";

admsnmp.authprotocol="Protocole d'authentication";

admsnmp.md5="MD5";

admsnmp.sha="SHA";

admsnmp.authpassword="Mot de passe Authentication";

admsnmp.privpassword="Mot de passe Privacy";

admsnmp.md5passwd="Mot de passe MD5";

admsnmp.trapmgt="Gestion des TRAP";

admsnmp.trapto="envoyer les TRAP vers";

admsnmp.ipaddr="Ad. IP";

admsnmp.iprange="Plage IP";


var admupnp = new Object();
admupnp.upnp="UPnP";

admupnp.selectpvc="S�lectionnez une connexion pvc pour la liaison";

var admigmpproxy = new Object();
admigmpproxy.pvcavail="Circuit Virtuel Permanent disponible";

admigmpproxy.igmpproxy="Proxy IGMP";

var admtr064 = new Object();
admtr064.tr064="TR-064";

var admtr069 = new Object();
admtr069.tr069="TR-069";

admtr069.cpeusername="Nom d'utilisateur du CPE";

admtr069.acsurl="URL ACS";

admtr069.acsusername="Nom d'utilisateur ACS";

admtr069.acspassword="Mot de passe ACS";

var admwlan = new Object();
admwlan.mgtwlan="Gestion via le r�seau sans fil";

var admreport = new Object();
admreport.logs="Fichier journal";

admreport.enable="Activer";

admreport.disable="D�sactiver";

admreport.logviewerip="Adresse IP de r�ception <br>des fichiers journaux";

admreport.emailalerts="Alertes de messagerie �lectronique";

admreport.denservice="Seuil de refus de service";

admreport.events="�v�nements";

admreport.eventsrange="(20 - 100)";

admreport.smtpserver="Serveur de messagerie SMTP";

admreport.emailaddr="Adresse de messagerie �lectronique pour fichiers journaux d'alertes";

admreport.returnaddr="Adresse de messagerie �lectronique de retour";

admreport.alls="TOUS";

admreport.systemlog="Fichier journal syst�me";

admreport.accesslog="Fichier journal des acc�s";

admreport.firewalllog="Fichier journal du pare-feu";

admreport.vpnlog="Fichier journal VPN";

admreport.upnplog="Fichier journal UPnP";


var admping = new Object();
admping.targetip="IP de cible Ping";

admping.size="Taille de Ping";

admping.bytes="Octets";

admping.numbers="Nombre de Pings";

admping.range="(Intervalle 1 ~ 100)";

admping.interval="Intervalle de Ping";

admping.timeout="D�lai de Ping";

admping.milliseconds="ms";

admping.result="R�sultat de Ping";

admping.pktsend="Pqt. TX";

admping.pktrecv="Pqt. RX";

admping.avgrtt="D�lai moyen";

admping.pingtest="Test Ping";

admping.ipaddr_domainname="Adresse IP ou Nom de domaine";

admping.unlimited="Illimit�";



var admrestore = new Object();
admrestore.selectfile="Veuillez choisir un fichier pour la restauration";

var admfacdef = new Object();
admfacdef.refacdefault="Restaurer les param�tres d'usine";

admfacdef.yes="Oui";

admfacdef.no="Non";

var admupgradewan = new Object();
admupgradewan.upgradeweb="Emplacement du micrologiciel sur le WEB";

admupgradewan.url="URL";

admupgradewan.warning="<B>Avertissement: </B>La mise � jour via le WAN peut prendre plusieurs <BR>minutes, ne coupez pas l'alimentation et n'appuyez pas sur le <BR>bouton de r�initialisation.";

admupgradewan.notinterrupted="Ne PAS interrompre la mise � jour !";

var admupgradelan = new Object();
admupgradelan.upgradefw="Mise � jour du micrologiciel";

admupgradelan.filepath="Chemin du fichier";

admupgradelan.warning="<B>Avertissement: </B>la mise � jour via le LAN peut prendre plusieurs <BR>minutes, ne coupez pas l'alimentation et n'appuyez pas sur le <BR>bouton de r�initialisation.";

admupgradelan.notinterrupted="Ne PAS interrompre la mise � jour !";

var admpostupgrade = new Object();
admpostupgrade.upgradesuccess="La mise � jour est un succ�s";

admpostupgrade.reboot="Red�marrage";

var admreboot = new Object();
admreboot.mode="Mode de red�marrage";

admreboot.hard="Mat�riel";

admreboot.soft="Logiciel";

admreboot.gwusername="Liste utilisateur";

admreboot.gwusername="Nom d'utilisateur";

admreboot.gwpassword="Mot de passe";

var admbutton =new Object();
admbutton.viewlogs="Afficher les fichiers journaux";

admbutton.starttest="D�marrer le test";

admbutton.backup="Sauvegarder";

admbutton.restore="Restaurer";

admbutton.browse="Rechercher";

admbutton.upgrade="Mise � jour";

admbutton.clears="Effacer";

admbutton.pagerefresh="Actualiser";

admbutton.prevpage="Page pr�c�dente";

admbutton.nextpage="Page suivante";

admbutton.cancelupgrade="Annuler les mises � jour";

admbutton.stop="Arr�t";

admbutton.clearlog="Effacer les fichiers journaux";

admbutton.close="Fermer";
