<p align="center">
  <img src="assets/logo.svg" alt="Logo TorBridge" width="160">
</p>

<h1 align="center">TorBridge</h1>

<p align="center">
  Un petit client SOCKS4 en C qui fait passer une connexion TCP par le réseau Tor.
</p>

---

## Présentation

TorBridge se connecte au proxy SOCKS local de Tor (`127.0.0.1:9050`), lui demande
d'ouvrir une connexion vers l'hôte et le port donnés (commande SOCKS4 `CONNECT`),
puis envoie une requête HTTP `HEAD /` à travers ce tunnel et affiche la réponse.

C'est un projet d'apprentissage : il montre, en une centaine de lignes, comment
fonctionne une poignée de main SOCKS4 avec les sockets POSIX (`socket`, `connect`,
`htons`, `inet_addr`).

## Prérequis

- Linux (ou un autre système POSIX)
- `gcc` et `make`
- Tor lancé localement, avec son port SOCKS sur `9050` :

  ```sh
  sudo systemctl start tor
  ```

## Compilation

```sh
make
```

Le binaire `torbridge` est créé à la racine du projet.

## Utilisation

```sh
./torbridge <ip> <port>
```

```sh
dig +short example.com
./torbridge <IP> 80
```

Exemple de sortie :

```
[+] Connected to proxy...
[+] Successfully connected through the proxy to <IP>:80
'HTTP/1.0 200 OK
...'
```

## Fonctionnement

1. Ouverture d'une socket TCP vers le proxy Tor (`PROXY`:`PROXYPORT`).
2. Envoi d'une requête SOCKS4 :

   ```
   +----+----+----+----+----+----+----+----+----+----+....+----+
   | VN | CD | DSTPORT |      DSTIP        | USERID       |NULL|
   +----+----+----+----+----+----+----+----+----+----+....+----+
     1    1      2              4           variable       1
   ```

   `VN = 4` (SOCKS4), `CD = 1` (CONNECT), `USERID = "toraliz"`.

3. Lecture de la réponse de 8 octets. Le champ `CD` indique le résultat :

   | Code | Signification                                                     |
   | ---- | ----------------------------------------------------------------- |
   | 90   | Requête acceptée                                                  |
   | 91   | Requête rejetée ou échouée                                        |
   | 92   | Rejetée : le serveur SOCKS ne peut pas joindre `identd` du client |
   | 93   | Rejetée : `identd` et le client annoncent des user-id différents  |

4. Si le code vaut 90, envoi de `HEAD / HTTP/1.0` dans le tunnel, puis affichage
   de la réponse.
