# changelog for C16 cable
# Wed 04 Jan 2017: add default services and bouquets (Luton- bandit area)- AL ;)
# Thu 12 Jan 2017: let's have some helpfiles for cable, too? -AL
# Sun 22 Jan 2017: added scan.end and scan.start scripts- AL

cd /var/bin
rm -f epg* stats
mv -f miniops-c miniops
chmod 755 miniops
cd $etc
rm -fr headers
rm -f .ab-fast .parent a*.csv bq* ch* cu* dic* mult* net* sat* sml* sup* the*
mv -f issue.net-c issue.net
cd $etc/info
rm -f Auto* Get* Multi* Set*
cd /var/tuxbox/plugins
rm -f 00-shell* 02-auto* 03-get*
cd $cfg
rm -fr enigma
mv -f scan.end-c scan.end
mv -f scan.start-c scan.start
rm -f menu_* shell*
cd $cfg/zapit
mv -f bouquets-c.xml bouquets.xml
mv -f services-c.xml services.xml
rm -f tv* *.pid
cd /
