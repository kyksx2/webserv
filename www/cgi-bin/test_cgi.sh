#!/bin/bash

echo "Content-Type: text/html"
echo ""

echo "<html><head><title>CGI Bash MIB</title>"
echo "<style>body{background:#050505;color:#00ff41;font-family:monospace;padding:20px;}"
echo ".header{color:#ff0000; font-weight:bold;}</style></head><body>"
echo "<h1>[ ACCÈS TERMINAL BASH ]</h1>"

echo "<p>METHODE DETECTEE : <span class='header'>$REQUEST_METHOD</span></p>"

if [ "$REQUEST_METHOD" = "POST" ]; then
    read -n "$CONTENT_LENGTH" QUERY_DATA
    echo "<p>DONNEES POST REÇUES : <strong>$QUERY_DATA</strong></p>"
else
    echo "<p>QUERY_STRING REÇUE : <strong>$QUERY_STRING</strong></p>"
fi

echo "<hr><p>VÉRIFICATION DES VARIABLES D'ENVIRONNEMENT :</p>"
echo "<ul>"
echo "<li>SERVER_PROTOCOL : $SERVER_PROTOCOL</li>"
echo "<li>REMOTE_ADDR : $REMOTE_ADDR</li>"
echo "<li>PATH_INFO : $PATH_INFO</li>"
echo "</ul>"

echo "</body></html>"