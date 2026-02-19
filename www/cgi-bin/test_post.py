#!/usr/bin/python3

import os
import sys

# 1. On récupère la taille des données (obligatoire pour lire le stdin proprement)
content_length = int(os.environ.get("CONTENT_LENGTH", 0))

# 2. On lit le body depuis l'entrée standard (stdin)
body = sys.stdin.read(content_length)

# 3. Préparation de l'affichage
# On affiche les données brutes reçues pour vérifier que le Webserv transmet bien tout
print("Content-Type: text/html")
print()  # Ligne vide entre headers et body

print(f"""
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Test CGI POST</title>
    <style>
        body {{ background: #1a1a1a; color: #ffcc00; font-family: 'Courier New', monospace; padding: 40px; }}
        .terminal {{ border: 2px solid #ffcc00; padding: 20px; background: #000; box-shadow: 0 0 15px rgba(255,204,0,0.5); }}
        h1 {{ color: #fff; text-transform: uppercase; }}
        .data {{ color: #00ff41; font-weight: bold; word-break: break-all; }}
    </style>
</head>
<body>
    <div class="terminal">
        <h1>Rapport de Transmission POST</h1>
        <p><strong>Taille détectée (CONTENT_LENGTH) :</strong> {content_length} octets</p>
        <hr>
        <p><strong>Données reçues sur stdin :</strong></p>
        <div class="data">{body if body else "[Aucune donnée reçue]"}</div>
    </div>
</body>
</html>
""")