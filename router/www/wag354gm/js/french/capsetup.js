var settopmenu = new Object();

settopmenu.basicsetup="Configuration de base";

settopmenu.ddns="DDNS";

settopmenu.advrouting="Routage avanc�";

settopmenu.hotspot="Point d'acc�s public sans fil";

settopmenu.voip="Voice";

var setleftmenu = new Object();

setleftmenu.inetsetup="Configuration Internet (WAN)";

setleftmenu.pvcconn="Connexion PVC";

setleftmenu.inetconntype="Type de connexion Internet";

setleftmenu.vcsetting="Param�tres VC";

setleftmenu.ipsetting="Param�tres IP";

setleftmenu.optsetting="Param�tres facultatifs";

setleftmenu.info="requis par certains FAI";

setleftmenu.netsetup="Configuration r�seau local (LAN)";

setleftmenu.routerip="adresse IP du modem routeur";

setleftmenu.cloudsetting="Configuration de CLOUD";

setleftmenu.cloud="CLOUD";

setleftmenu.netaddress="";

setleftmenu.serversetting="Param�tres du serveur d'adresses";

setleftmenu.timesetting="R�glage de l'heure";

setleftmenu.ddns="DDNS";

setleftmenu.advrouting="Routage avanc�";

setleftmenu.operamode="Mode de fonctionnement";

setleftmenu.dynrouting="Routage dynamique";

setleftmenu.staticrouting="Routage statique";

setleftmenu.pvcrout="Strat�gie de routage PVC";

var setinetsetup = new Object();

setinetsetup.please="S�lectionner une connexion ";

setinetsetup.enablenow="Activer maintenant";

setinetsetup.encap="Encapsulation";

setinetsetup.multiplexing="Multiplexage";

setinetsetup.qostype="Type QS";

setinetsetup.pcrrate="Taux PCR";

setinetsetup.scrrate="Taux SCR";

setinetsetup.autodetect="D�tection automatique";

setinetsetup.vircir="Circuit virtuel;";

setinetsetup.obtain="Obtenir une adresse IP automatiquement";

setinetsetup.usefollow="Utiliser l'adresse IP suivante";

setinetsetup.inetipaddr="Adresse IP Internet";

setinetsetup.subnetmask="Masque de sous-r�seau";

setinetsetup.pridns="Nom de domaine principal";

setinetsetup.secdns="Nom de domaine secondaire";

setinetsetup.pppoesession="Session PPPoE";

setinetsetup.hostname="Nom d'h�te";

setinetsetup.doname="Nom de domaine";

setinetsetup.mtu="MTU";

setinetsetup.size="Taille";

setinetsetup.symbol=".";

var setnetsetup = new Object();

setnetsetup.localip="Adresse IP locale";

setnetsetup.subnet="Masque de sous-r�seau";

setnetsetup.ldhcpserver="Serveur DHCP";

setnetsetup.dhcprelay="Relais DHCP";

setnetsetup.dhcprelayserver="Serveur relais DHCP";

setnetsetup.autodhcp="D�tection automatique du serveur DHCP";

setnetsetup.startip="Adresse IP de d�part";

setnetsetup.maxnum="Nombre maximal de clients DHCP";

setnetsetup.clientlease="Dur�e de bail du client";

setnetsetup.minutes="minutes";

setnetsetup.moneday="signifie une journ�e";

setnetsetup.staticdns="DNS statique";

setnetsetup.wins="WINS";

setnetsetup.timezone="Fuseau horaire";

setnetsetup.tinterval="Intervalle de temps";

setnetsetup.seconds="secondes";

setnetsetup.lastinfo="R�gler automatiquement l'horloge en fonction des modifications de l'heure d'�t�";

var setddns = new Object();

setddns.service="Service DDNS";

setddns.org="DynDNS.org";

setddns.com="TZO.com";

setddns.username="Nom d'utilisateur";

setddns.password="Mot de passe";

setddns.hostname="Nom d'h�te";

setddns.email="Adresse E-mail";

setddns.doname="Nom de domaine";

setddns.ipaddr="Adresse IP Internet";

setddns.statu="Etat";

var ddnsm = new Object();

ddnsm.dyn_good="Mise � jour du service DDNS r�ussie"

ddnsm.dyn_noupdate="Mise � jour du service DDNS r�ussie"

ddnsm.dyn_nochg="Mise � jour du service DDNS r�ussie"

ddnsm.dyn_badauth="Echec de l''autorisation (nom d'utilisateur ou mots de passe)";

ddnsm.all_noip="Pas de connexion Internet";

ddnsm.dyn_yours="Nom d'utilisateur incorrect";
 
ddnsm.dyn_nohost="Le nom d'h�te n'existe pas";

ddnsm.dyn_notfqdn="Nom de domaine incorrect";

ddnsm.dyn_abuse="Le nom d'h�te est bloqu� par le serveur DDNS";

ddnsm.dyn_badsys="Les parametres systeme sont invalides";

ddnsm.dyn_badagent="cet agent a �t� bloqu�";

ddnsm.dyn_numhost="Trop ou pas assez d'h�tes trouv�s";

ddnsm.dyn_dnserr= "Erreur DNS";

ddnsm.dyn_911= "Erreur innatendue (1)";

ddnsm.dyn_999= "Erreur innatendue (2)";

ddnsm.dyn_donator="L'option demand�e est disponible moyennant une participation, veuillez contribuer";

ddnsm.dyn_strange= "R�ponse incoh�rente du serveur, etes-vous connect� au bon serveur?";

ddnsm.dyn_uncode= "code erreur inconnu";
 
ddnsm.tzo_good= "Op�ration termin�e";

ddnsm.tzo_noupdate="Op�ration termin�e";

ddnsm.tzo_notfqdn= "Nom de domaine invalide";

ddnsm.tzo_notmail= "Email invalide";

ddnsm.tzo_notact= "Action invalide";

ddnsm.tzo_notkey= "Cl� Invalide";

ddnsm.tzo_notip= "IP adresse invalide";

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

var setadvrouting = new Object();

setadvrouting.nat="NAT";

setadvrouting.rip="RIP";

setadvrouting.compatible="Compatible";

setadvrouting.defaultroute="Route par d�faut";

setadvrouting.multibroad="Multidiffusion ou Diffusion";

setadvrouting.broadcast="Diffusion";

setadvrouting.multicast="Multidiffusion";

setadvrouting.transver="Version RIP en transmission";

setadvrouting.receiver="Version RIP en r�ception";

setadvrouting.selnum="S�lectionner le num�ro de jeu";

setadvrouting.dstipaddr="Adresse IP de destination";

setadvrouting.subnet="Masque de sous-r�seau";

setadvrouting.hop="Nombre de sauts";

setadvrouting.deletes="Supprimer cette entr�e";

setadvrouting.showtable="Afficher la table de routage";

setadvrouting.routingtable="Table de routage";

setadvrouting.pvcsetting="Param�tre de routage PVC";

setadvrouting.pvcpolicy="Strat�gie de routage PVC";

var setother = new Object();

setother.pppoasetting="Param�tres PPPoA";

setother.condemand="Connexion sur demande";

setother.maxidle="Temps d'inactivit� max";

setother.keepalive="Garde connect�";

setother.radial="D�lai de reconnexion";

setother.pppoesetting="Param�tres PPPoE";

setother.servicename="Nom de service";

setother.secondpppoe="Deuxi�me PPPoE";

setother.matchs="Egaler le nom de domaine";

setother.macclone="Duplication MAC";

setother.userdefine="Valeur d�finie par l'utilisateur";

setother.clone="Dupliquez l'adresse MAC de votre PC";

setother.hotspot="Point d'acc�s public sans fil";

setother.macaddrclone="Adresse MAC dupliqu�e";
