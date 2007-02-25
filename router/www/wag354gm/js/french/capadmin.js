var admtopmenu = new Object();
admtopmenu.credval="Validation des références";

admtopmenu.manage="Gestion";

admtopmenu.report="Rapport";

admtopmenu.diag="Diagnostics";

admtopmenu.facdef="Paramètres d'usine";

admtopmenu.bakrest="Sauvegarder/Restaurer";

admtopmenu.upgrade="Mise à jour du micrologiciel";

admtopmenu.reboot="Redémarrage";

var admleftmenu = new Object();
//admleftmenu.gwaccess="Accès à la passerelle";
admleftmenu.gwaccess="Accès local";

admleftmenu.localgwaccess="Accès local";

admleftmenu.remotegwaccess="Accès distant";

admleftmenu.remoteupgrade="Mise à jour à distance";

admleftmenu.snmp="SNMP";

admleftmenu.upnp="UPnP";

admleftmenu.tr064="TR064";

admleftmenu.tr069="TR-069";

admleftmenu.igmpproxy="Proxy IGMP";

admleftmenu.wlan="réseau Sans Fil";

admleftmenu.report="Rapport";

admleftmenu.emailalt="Alertes par messagerie électronique";

admleftmenu.ping="Test Ping";

admleftmenu.pingparam="Paramètres";

admleftmenu.bakconfig="Sauvegarde de la configuration";

admleftmenu.restconfig="Restauration de la configuration";

admleftmenu.facdef="Paramètres d'usine";

admleftmenu.upgradewan="Mise à jour à partir du WAN";

admleftmenu.upgradelan="Mise à jour à partir du LAN";

admleftmenu.reboot="Redémarrage";


var admgwaccess = new Object();
admgwaccess.gwusername="Nom d'utilisateur";

admgwaccess.gwpassword="Mot de passe";

admgwaccess.reconfirm="Confirmation du mot de passe";

admgwaccess.remotemgt="Gestion à distance";

admgwaccess.mgtport="Numéro de Port";

admgwaccess.allowip="Adresse Ip autorisée";

admgwaccess.all="TOUS";

admgwaccess.ipaddr="Adresse IP";

admgwaccess.iprange="Plage IP";

admgwaccess.rmtupgrade="Mise à jour à distance";

admgwaccess.usehttps="Utiliser Https";


var admsnmp = new Object();
admsnmp.snmp="SNMP";

admsnmp.devname="Nom du périphérique";

admsnmp.snmp_v1v2="Snmp V1/V2";

admsnmp.getcomm="Obtenir la communauté";

admsnmp.setcomm="Définir la communauté";

admsnmp.snmp_v3="Snmp V3";

admsnmp.rwuser="Rw User (autorisé en lect/écriture) ";

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

admupnp.selectpvc="Sélectionnez une connexion pvc pour la liaison";

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
admwlan.mgtwlan="Gestion via le réseau sans fil";

var admreport = new Object();
admreport.logs="Fichier journal";

admreport.enable="Activer";

admreport.disable="Désactiver";

admreport.logviewerip="Adresse IP de réception <br>des fichiers journaux";

admreport.emailalerts="Alertes de messagerie électronique";

admreport.denservice="Seuil de refus de service";

admreport.events="évènements";

admreport.eventsrange="(20 - 100)";

admreport.smtpserver="Serveur de messagerie SMTP";

admreport.emailaddr="Adresse de messagerie électronique pour fichiers journaux d'alertes";

admreport.returnaddr="Adresse de messagerie électronique de retour";

admreport.alls="TOUS";

admreport.systemlog="Fichier journal système";

admreport.accesslog="Fichier journal des accès";

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

admping.timeout="Délai de Ping";

admping.milliseconds="ms";

admping.result="Résultat de Ping";

admping.pktsend="Pqt. TX";

admping.pktrecv="Pqt. RX";

admping.avgrtt="Délai moyen";

admping.pingtest="Test Ping";

admping.ipaddr_domainname="Adresse IP ou Nom de domaine";

admping.unlimited="Illimité";



var admrestore = new Object();
admrestore.selectfile="Veuillez choisir un fichier pour la restauration";

var admfacdef = new Object();
admfacdef.refacdefault="Restaurer les paramètres d'usine";

admfacdef.yes="Oui";

admfacdef.no="Non";

var admupgradewan = new Object();
admupgradewan.upgradeweb="Emplacement du micrologiciel sur le WEB";

admupgradewan.url="URL";

admupgradewan.warning="<B>Avertissement: </B>La mise à jour via le WAN peut prendre plusieurs <BR>minutes, ne coupez pas l'alimentation et n'appuyez pas sur le <BR>bouton de réinitialisation.";

admupgradewan.notinterrupted="Ne PAS interrompre la mise à jour !";

var admupgradelan = new Object();
admupgradelan.upgradefw="Mise à jour du micrologiciel";

admupgradelan.filepath="Chemin du fichier";

admupgradelan.warning="<B>Avertissement: </B>la mise à jour via le LAN peut prendre plusieurs <BR>minutes, ne coupez pas l'alimentation et n'appuyez pas sur le <BR>bouton de réinitialisation.";

admupgradelan.notinterrupted="Ne PAS interrompre la mise à jour !";

var admpostupgrade = new Object();
admpostupgrade.upgradesuccess="La mise à jour est un succès";

admpostupgrade.reboot="Redémarrage";

var admreboot = new Object();
admreboot.mode="Mode de redémarrage";

admreboot.hard="Matériel";

admreboot.soft="Logiciel";

admreboot.gwusername="Liste utilisateur";

admreboot.gwusername="Nom d'utilisateur";

admreboot.gwpassword="Mot de passe";

var admbutton =new Object();
admbutton.viewlogs="Afficher les fichiers journaux";

admbutton.starttest="Démarrer le test";

admbutton.backup="Sauvegarder";

admbutton.restore="Restaurer";

admbutton.browse="Rechercher";

admbutton.upgrade="Mise à jour";

admbutton.clears="Effacer";

admbutton.pagerefresh="Actualiser";

admbutton.prevpage="Page précédente";

admbutton.nextpage="Page suivante";

admbutton.cancelupgrade="Annuler les mises à jour";

admbutton.stop="Arrêt";

admbutton.clearlog="Effacer les fichiers journaux";

admbutton.close="Fermer";
