from flask.cli import FlaskGroup

from app import app, db
from app.models.models import *
from app.enums import UserRoleEnum

from datetime import datetime

cli = FlaskGroup(app)

@cli.command("create_db")
def create_db():
    db.drop_all()
    db.create_all()
    db.session.commit()

@cli.command("seed_db")
def seed_db():
    # Users
    delete_account_user = User(email="deleteReserved", password="deleteReserved")
    user1 = User(email="adam@zvara.com", login="Adam", password="abcdefgh")
    user2 = User(email="jakub@vlk.com", login="Jakub", password="testing")
    user3 = User(email="anton@jarol.com", login="Tonda", password="abcdefgh")
    user4 = User(email="mod@mod.com", login="moderator", password="ModJeZamestnanecMenzy", role=UserRoleEnum.mod)
    user5 = User(email="admin@admin.com", login="admin", password="AdminJeSpravce", role=UserRoleEnum.admin)
    user6 = User(email="user@user.com", login="user", password="UzivatelJeHodny")

    # Allergens
    allergen1 = Allergen(name="Lepek",
                         descr="pšenice, žito, ječmen, oves, špalda, kamut nebo jejich odrůdy")
    allergen2 = Allergen(name="Korýši")
    allergen3 = Allergen(name="Vejce")
    allergen4 = Allergen(name="Ryby")
    allergen5 = Allergen(name="Arašídy",
                         descr="podzemnice olejná")
    allergen6 = Allergen(name="Sója",
                         descr="sójové boby")
    allergen7 = Allergen(name="Mléko")
    allergen8 = Allergen(name="Skořápkové plody",
                         descr="mandle, lískové ořechy, vlašské ořechy, kešu ořechy, pekanové ořechy, para ořechy, pistácie, makadamie")
    allergen9 = Allergen(name="Celer")
    allergen10 = Allergen(name="Hořčice")
    allergen11 = Allergen(name="Seznam",
                          descr="sezamová semena")
    allergen12 = Allergen(name="Oxid siřičitý a siřičitany",
                          descr="E220, E221, E222, E223, E224, E226, E227, E228")
    allergen13 = Allergen(name="Vlčí bob",
                          descr="lupina")
    allergen14 = Allergen(name="Měkkýši")

    # Seeding everything
    db.session.add_all([delete_account_user])
    db.session.add_all([user1, user2, user3, user4, user5, user6])
    db.session.add_all([allergen1, allergen2, allergen3, allergen4, allergen5,
                        allergen6, allergen7, allergen8, allergen9, allergen10,
                        allergen11, allergen12, allergen13, allergen14])
    db.session.commit()

if __name__ == "__main__":
    cli()
