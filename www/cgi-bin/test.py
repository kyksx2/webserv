#!/usr/bin/env python3
import os
# import time

# time.sleep(10)
print("Content-Type: text/plain\r\n\r\n")
print("Coucou depuis le script Python !")
# On affiche une variable d'env pour prouver qu'on les reçoit bien
print("Méthode reçue : " + os.environ.get("REQUEST_METHOD", "Inconnue"))