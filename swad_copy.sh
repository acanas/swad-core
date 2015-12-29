#!/bin/bash

CGI=/usr/lib/cgi-bin/swad
PUBLIC_HTML=/var/www/html/swad
APACHE_USER=www-data
APACHE_GROUP=www-data
USER=acanas
CORE=/home/$USER/swad/swad-core

cp -af $CGI/swad_ca $CGI/swad_ca.old
cp -af $CGI/swad_de $CGI/swad_de.old
cp -af $CGI/swad_en $CGI/swad_en.old
cp -af $CGI/swad_es $CGI/swad_es.old
cp -af $CGI/swad_fr $CGI/swad_fr.old
cp -af $CGI/swad_gn $CGI/swad_gn.old
cp -af $CGI/swad_it $CGI/swad_it.old
cp -af $CGI/swad_pl $CGI/swad_pl.old
cp -af $CGI/swad_pt $CGI/swad_pt.old

cp -f $CORE/swad_ca $CORE/swad_de $CORE/swad_en $CORE/swad_es $CORE/swad_fr $CORE/swad_gn $CORE/swad_it $CORE/swad_pl $CORE/swad_pt $CGI
cp -f $CORE/js/swad*.js $PUBLIC_HTML
cp -f $CORE/css/swad*.css $PUBLIC_HTML

chown -R $APACHE_USER:$APACHE_GROUP $CGI
chown $APACHE_USER:$APACHE_GROUP $PUBLIC_HTML/swad*.js
chown $APACHE_USER:$APACHE_GROUP $PUBLIC_HTML/swad*.css
