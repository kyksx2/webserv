#!/bin/bash

# 1. Headers HTTP obligatoires
echo "Content-Type: text/html"
echo ""

# 2. Début du HTML
echo "<html><head><title>CGI Bash MIB</title>"
echo "<style>body{background:#050505;color:#00ff41;font-family:monospace;padding:20px;}"
echo ".header{color:#ff0000; font-weight:bold;}</style></head><body>"
echo "<h1>[ ACCÈS TERMINAL BASH ]</h1>"

# 3. Logique de détection de la méthode
echo "<p>METHODE DETECTEE : <span class='header'>$REQUEST_METHOD</span></p>"

if [ "$REQUEST_METHOD" = "POST" ]; then
    # On lit le nombre d'octets spécifié par CONTENT_LENGTH sur l'entrée standard
    # 'read -n' est parfait pour ça
    read -n "$CONTENT_LENGTH" QUERY_DATA
    echo "<p>DONNEES POST REÇUES : <strong>$QUERY_DATA</strong></p>"
else
    # En GET, on utilise simplement la variable d'environnement
    echo "<p>QUERY_STRING REÇUE : <strong>$QUERY_STRING</strong></p>"
fi

echo "<hr><p>VÉRIFICATION DES VARIABLES D'ENVIRONNEMENT :</p>"
echo "<ul>"
echo "<li>SERVER_PROTOCOL : $SERVER_PROTOCOL</li>"
echo "<li>REMOTE_ADDR : $REMOTE_ADDR</li>"
echo "<li>PATH_INFO : $PATH_INFO</li>"
echo "</ul>"

echo "</body></html>"