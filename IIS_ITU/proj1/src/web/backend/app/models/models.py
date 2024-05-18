from flask_sqlalchemy import SQLAlchemy
from sqlalchemy.dialects.postgresql import ARRAY
from datetime import datetime
from ..enums import FoodTypeEnum, FoodCategoryEnum, UserRoleEnum
from werkzeug.security import generate_password_hash, check_password_hash

db = SQLAlchemy()


class Rating(db.Model):
    __tablename__ = "ratings"

    id = db.Column(db.Integer, primary_key=True)
    points = db.Column(db.Integer, nullable=False)  # storing points as integer or enum {low, middle, high}?
    text = db.Column(db.String(300))
    added = db.Column(db.DateTime, default=datetime.now())

    user_id = db.Column(db.Integer, db.ForeignKey('users.id'))
    user = db.relationship('User', backref='food_ratings', lazy=True)

    food_id = db.Column(db.Integer, db.ForeignKey('current_foods.id'))
    to_food = db.relationship('CurrentFood', backref='ratings', lazy=True)

    def __init__(self, points, user_id, food_id, text=None, added=None):
        self.points = points
        self.text = text
        self.user_id = user_id
        self.food_id = food_id
        if added is not None:
            self.added = added


class User(db.Model):
    __tablename__ = "users"

    id = db.Column(db.Integer, primary_key=True)
    login = db.Column(db.String(100))
    password = db.Column(db.String(250), nullable=False)
    email = db.Column(db.String(100), unique=True, nullable=False)
    blocked = db.Column(db.Boolean)
    role = db.Column(db.Enum(UserRoleEnum))

    # maybe store path to photos from users which can be stored in project/media?
    # photoURL = db.Column(db.string(256), unique=True)

    def __init__(self, password, email, login=None, blocked=False, role=UserRoleEnum.user):
        self.email = email
        self.password = generate_password_hash(password)
        self.login = login
        self.blocked = blocked
        self.role = role

    def verify_password(self, passw: str):
        return check_password_hash(self.password, passw)


class Canteen(db.Model):
    __tablename__ = "canteens"

    id = db.Column(db.Integer, primary_key=True)
    name = db.Column(db.String(100), nullable=False)
    address = db.Column(db.String(100))
    opened_first = db.Column(ARRAY(db.Time))
    opened_second = db.Column(ARRAY(db.Time), nullable=True)
    opened_first_friday = db.Column(ARRAY(db.Time))
    opened_second_friday = db.Column(ARRAY(db.Time), nullable=True)

    def __init__(self, **kwargs):
        self.__dict__.update(kwargs)


food_allergens = db.Table('food_allergens',
                          db.Column('food_id', db.Integer, db.ForeignKey('foods.id'), primary_key=True),
                          db.Column('allergen_id', db.Integer, db.ForeignKey('allergens.id'), primary_key=True)
                          )


class Allergen(db.Model):
    __tablename__ = "allergens"

    id = db.Column(db.Integer, primary_key=True)
    name = db.Column(db.String(100))
    descr = db.Column(db.String(200))

    def __init__(self, name=None, descr=None):
        self.name = name
        self.descr = descr


class CurrentFood(db.Model):
    __tablename__ = "current_foods"

    id = db.Column(db.Integer, primary_key=True)

    food = db.relationship('Food', backref='available_in')
    food_id = db.Column(db.Integer, db.ForeignKey('foods.id'))

    canteen = db.relationship('Canteen', backref='available_foods')
    canteen_id = db.Column(db.Integer, db.ForeignKey('canteens.id'))

    last_available = db.Column(db.Date, default=datetime.now())
    available = db.Column(db.Boolean)

    __table_args__ = (db.UniqueConstraint('food_id', 'canteen_id', 'last_available', name='uc_food_canteen_date'),)

    def __init__(self, food, canteen, available=True):
        self.food = food
        self.canteen = canteen
        self.available = available


class Ingredient(db.Model):
    __tablename__ = "ingredients"
    id = db.Column(db.Integer, primary_key=True)
    name = db.Column(db.String(100), nullable=False, unique=True)

    is_vegan = db.Column(db.Boolean, default=False)
    is_vegetarian = db.Column(db.Boolean, default=False)
    is_gluten_free = db.Column(db.Boolean, default=False)

    is_checked = db.Column(db.Boolean, default=False)

    def __init__(self, name, **kwargs):
        self.name = name

    def __str__(self):
        return str(self.name) + "(" + str(self.id) + ")"


food_ingredients = db.Table('food_ingredients',
                            db.Column('food_id', db.Integer, db.ForeignKey('foods.id'), primary_key=True),
                            db.Column('ingredient_id', db.Integer, db.ForeignKey('ingredients.id'), primary_key=True)
                            )


class Food(db.Model):
    __tablename__ = "foods"

    id = db.Column(db.Integer, primary_key=True)
    name = db.Column(db.String(300), nullable=False, unique=True)
    name_en = db.Column(db.String(300), nullable=False, unique=True)
    food_type = db.Column(db.Enum(FoodTypeEnum), default=FoodTypeEnum.main)
    category = db.Column(ARRAY(db.Enum(FoodCategoryEnum)))
    weight = db.Column(db.String(50), nullable=True)

    price_student = db.Column(db.Integer, nullable=False)
    price_employee = db.Column(db.Integer, nullable=False)
    price_extern = db.Column(db.Integer, nullable=False)

    allergens = db.relationship('Allergen', secondary=food_allergens)
    ingredients = db.relationship('Ingredient', secondary=food_ingredients)

    def __init__(self,
                 name,
                 name_en,
                 food_type,
                 weight,
                 price_student,
                 price_employee,
                 price_extern,
                 ingredients=[],
                 allergens=[],
                 category=None,
                 **kwargs):
        self.name = name
        self.name_en = name_en
        self.food_type = food_type
        self.category = category
        self.weight = weight
        self.price_student = price_student
        self.price_employee = price_employee
        self.price_extern = price_extern
        self.allergens = allergens
        self.ingredients = ingredients

    def __str__(self):
        return str(self.name) + "(" + str(self.id) + ")"
