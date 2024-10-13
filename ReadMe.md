Grupa_Nume_Prenume_Tema4PC.zip

# Tema 4

Pentru prelucarea entitatilor json a fost aleasa libraria parson pentru mariea sa redusa, usurinta de a parsa string-uri si de a serializa obiecte.

#### In cererile catre server, este setat header-ul de connection: keep alive pentru a putea transmite mai multe request-uri pe aceeasi conexiune.

### Clientul web ce interactioneaza cu REST API implementeaza urmatoarele functionalitati:

* ### Register:
        Se citesc de la stdin un username si o parola si se completeaza un header de HTTP. Acesta are in sectiunea de date un obiect json cu username-ul si parola utilizatorului ce va fi creat.
        Daca server-ul semnaleaza ca exista deja username-ul respectiv, mesajul de eroare respectiv este afisat.

* ### Login:
        Se citesc de la stdin un username si o parola si se completeaza un header de HTTP. Acesta are in sectiunea de date un obiect json cu username-ul si parola utilizatorului ce doreste sa se autentifice.
        Daca credentialele nu se portrivesc, se va afisa un mesaj de eroare.
        Daca autentificarea s-a efectuat cu succes, cookie-ul de sesiune este extras din raspunsul server-ului si memorat.

* ### Enter Library:
        Este trimisa, catre server, o cerere de acces la biblioteca care contine cookie-ul primit la autentificare. Daca nu s-a realizat o autentificare precedand aceasta cerere, campul cookie va ramane gol si server-ul va intoarce un mesaj de eroare in reply. Mesajul este extras si afisat.
        Daca s-a efectuat cu succes, este extras din reply un token de acces la biblioteca care este memorizat.

* ### Get Books:
        Trimite spre server o cerere pentru un sumar al tuturor cartilor. Aceasta cerere contine cookie-ul de sesiune si token-ul de acces la biblioteca. Daca token-ul este gol(nu s-a obtinut acces la biblioteca precedent), server-ul va transmite un mesaj de eroare in reply, mesaj care este extras si afisat.
        Daca s-a efectuat cu succes, se va afisa un vector de obiente json so id-ul si titlul tuturor cartilor.

* ### Get Book:
        Trimite spre server o cerere pentru informatiile unei carti al carei id se citeste de la stdin. In cerere sunt atasate cookie-ul de sesiune si token-ul de acces. Daca token-ul este gol(nu s-a obtinut acces la biblioteca), server-ul intoarce un reply cu un mesaj de eroare, ce este afisat.
        Daca s-a realizat cu succes, este extras din reply-ul server-ului string-ul ce reprezinta informatiile cartii cerute.
        Daca cartea nu exista, server-ul intoarce un mesaj de eroare in reply.

* ### Add Book:
        Se trimite o cerere catre server de a adauga o noua carte in biblioteca. Se citesc de la stdin toate informatiile despre carte. Se converteste numarul de pagini la int. Daca functia returneaza eroare, inseamna ca string-ul dat nu reprezinta un numar si se intoarce o eroare.
        In cerere sunt introduse header-e pentry cookie-ul de sesiune si token-ul de acces. Daca token-ul lipseste, server-ul va intoarce o eroare in reply.
        In request este atasat un obiect json cu informatiile cartii. Daca aceste sunt incomplete, server-ul va intaorce un mesaj de eroare.

* ### Delete Book:
        Trimite spre server un request de a sterge o carte cu un anumit id. Id-ul este citit de la stdin. In cerere sunt incluse header-e cu cookie-ul de sesiune si token-ul de acces. Daca token-ul este gol( nu s-a obtinut acces la biblioteca), server-ul va intoarce o eroare in reply.
        Daca token-ul este corect, cartea cu id-ul dat este stearsa daca exista. Altfel, server-ul va intoarce o eroare in reply.

* ### Logout:
        Se trimite o cerere de logout spre server. Request-ul contine cookie-ul de sesiune care s-a obtinut la autentificare. Daca cookie-ul este gol(nu s-a realizat o autentificare), server-ul intoarce o eroare in reply.
        La completarea cu succes a request-ului, cookie-ul de sesiune si token-ul de acces sunt sterse.

* ### Exit:
        Se inchide conexiunea cu sever-ul si inchide programul.