# Informarčný systém na hodnotenie jedál vo VUT menzách

## Softwarové požiadavky
Docker a docker-compose. Všetko potrebné sa automaticky nakonfiguruje. Všetky použité knižnice sú napsisáné v requirements.txt, package.json a dockerfiles pre BE a FE

## Ako spustiť projekt
Rozbalenie, konfiguráciu i inicializáciu zaistí spustenie docker-compose. Do databázy sa po spustení uloží inicializačné dáta v súboroch manage.py a common.py
`docker-compose up -d --build`

## Zdroje

Počiatočná konfigurácia projektu: [odkaz](https://testdriven.io/blog/dockerizing-flask-with-postgres-gunicorn-and-nginx)

JWT autorizacia: [odkaz](https://stackabuse.com/single-page-apps-with-vue-js-and-flask-jwt-authentication/)

## Použité frameworky a knižnice

#### Backend

[Flask](https://flask.palletsprojects.com/en/2.2.x/)

[marshmallow](https://marshmallow.readthedocs.io/en/stable/)

[requests](https://pypi.org/project/requests/)

[SQLAlchemy](https://www.sqlalchemy.org/)

[pyjwt](https://pyjwt.readthedocs.io/en/stable/)

#### Frontend

[Vue](https://vuejs.org/)

[vuetify](https://vuetifyjs.com/en/)