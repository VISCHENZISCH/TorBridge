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

L'adresse doit être une **adresse IPv4** (par exemple `93.184.215.14`) et non un
nom de domaine : SOCKS4 ne résout pas les noms. Pour obtenir l'IP d'un site :

```sh
dig +short example.com
./torbridge 93.184.215.14 80
```

Exemple de sortie :

```
[+] Connected to proxy...
[+] Successfully connected through the proxy to 93.184.215.14:80
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

   | Code | Signification                                                      |
   |------|--------------------------------------------------------------------|
   | 90   | Requête acceptée                                                   |
   | 91   | Requête rejetée ou échouée                                         |
   | 92   | Rejetée : le serveur SOCKS ne peut pas joindre `identd` du client  |
   | 93   | Rejetée : `identd` et le client annoncent des user-id différents   |

4. Si le code vaut 90, envoi de `HEAD / HTTP/1.0` dans le tunnel, puis affichage
   de la réponse.

## Configuration

Les paramètres se trouvent dans [`torbridge.h`](torbridge.h) :

| Constante   | Valeur par défaut | Rôle                          |
|-------------|-------------------|-------------------------------|
| `PROXY`     | `"127.0.0.1"`     | Adresse du proxy SOCKS        |
| `PROXYPORT` | `9050`            | Port du proxy SOCKS de Tor    |
| `USERNAME`  | `"toraliz"`       | User-id envoyé dans SOCKS4    |

Recompilez avec `make` après toute modification.

## Limites actuelles

- IPv4 uniquement, pas de résolution DNS (SOCKS4, pas SOCKS4a ni SOCKS5).
- L'en-tête `Host` de la requête HTTP est fixé dans le code
  (`www.networktechnology.org`).
- Une seule lecture de 511 octets maximum : seul le début de la réponse est affiché.
- Pas de TLS : préférez le port 80 pour les tests.

## Structure du projet

```
.
├── assets/logo.svg   # logo du projet
├── Makefile          # compilation
├── torbridge.c       # requête SOCKS4 et programme principal
└── torbridge.h       # constantes, structures requête/réponse
```
