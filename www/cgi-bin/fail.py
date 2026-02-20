#!/usr/bin/env python3
import sys
import time

print("Content-Type: text/html; charset=utf-8")
print("") # Fin des headers

print("<html><body>")
print("<h1>Tout va bien</h1>")

sys.stdout.flush() # force l'envoi vers le serveur
time.sleep(1)
sys.stdout.flush()

# div par 0 = erreure
result = 1 / 0 

print("</body></html>")