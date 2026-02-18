#!/usr/bin/env python3
import sys
import time

# 1. On envoie des headers corrects pour mettre le serveur en confiance
print("Content-Type: text/html; charset=utf-8")
print("") # Fin des headers

# 2. On commence à envoyer du contenu
print("<html><body>")
print("<h1>Tout va bien... pour l'instant. 😇</h1>")
sys.stdout.flush() # On force l'envoi vers le serveur

# 3. Petite pause pour le suspense
time.sleep(1)

# 4. CRASH ! Division par zéro
print("<p>Et là... c'est le drame ! 💥</p>")
sys.stdout.flush()

# L'erreur fatale
result = 1 / 0 

print("</body></html>")