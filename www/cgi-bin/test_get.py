#!/usr/bin/python3

import os
import urllib.parse

# 1. On récupère les données envoyées en GET via la variable d'environnement
query_string = os.environ.get("QUERY_STRING", "")

# 2. On parse les paramètres (ex: ?nom=Bond&prenom=James)
params = urllib.parse.parse_qs(query_string)

# 3. Préparation du contenu HTML
name = params.get("nom", ["Inconnu"])[0]
agent_id = params.get("id", ["000"])[0]

# 4. LE PLUS IMPORTANT : L'en-tête HTTP (C'est le script qui doit l'écrire !)
print("Content-Type: text/html")
print()  # Ligne vide obligatoire entre les headers et le body

# 5. Le corps de la page
print(f"""
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Résultat CGI - MIB</title>
    <style>
        body {{ background: #000; color: #00ff41; font-family: monospace; padding: 50px; }}
        .box {{ border: 1px solid #00ff41; padding: 20px; border-radius: 10px; }}
        h1 {{ color: white; }}
    </style>
</head>
<body>
    <div class="box">
        <h1>Analyse du signal CGI (GET)</h1>
        <p><strong>Agent identifié :</strong> {name}</p>
        <p><strong>ID matricule :</strong> {agent_id}</p>
        <hr>
        <p><strong>Query String brute :</strong> {query_string}</p>
    </div>
</body>
</html>
""")