import json
import time

import requests
import random
from ..schemas.skm_schemas import SkmCanteenSchema
from sqlalchemy import select
from ..models.models import *
from .. import app
from ..common import *

skm_api_url = "https://www.skm.vutbr.cz/app06/export/mx_json.aspx"
LOGGING = False
SEEDING = True
SEEDED = False

def log(value):
    if LOGGING:
        print(value)


def log_ingredients(ingredients):
    if LOGGING:
        if ingredients is not None:
            print("Ingredients: " + str([str(ingr) for ingr in ingredients]))


def update():
    global SEEDED
    print('Update run at: ' + str(datetime.now()))
    # data = get_data()
    data = get_data(from_file=True)
    update_data(data)
    if SEEDING and not SEEDED:
        seed_ratings()
        SEEDED = True


def get_data(from_file=False):
    data = load_data(from_file)
    cleaned_data = clean_data(data)

    schema = SkmCanteenSchema(many=True)
    canteens = schema.load(cleaned_data)
    return canteens


def find_existing_canteen(canteen):
    fetched_canteen = db.session.execute(select(Canteen).
                                         where(Canteen.name == canteen.name))
    return fetched_canteen.scalars().first()


def copy_canteen_data(db_canteen, canteen):
    # log("type: " + str(type(db_canteen)))
    # log("type: " + str(type(canteen)))
    db_canteen.address = canteen.address
    # Todo copy more data/update as instance


def try_find_food(name):
    fetched_canteen = db.session.execute(select(Food)
                                         .where(Food.name == name))
    return fetched_canteen.scalars().first()


def fetch_today_menu(canteen_id):
    menu = db.session.execute(select(CurrentFood)
                              .where(CurrentFood.canteen_id == canteen_id))
    # .where(CurrentFood.last_available.day == datetime.now().day))
    return menu.scalars().all()


def try_find_ingredient(name):
    fetched_ingredient = db.session.execute(select(Ingredient)
                                            .where(Ingredient.name == name))
    return fetched_ingredient.scalars().first()


def append_unique(ingredient_list, ingredient):
    if ingredient.name not in [i.name for i in ingredient_list]:
        ingredient_list.append(ingredient)


def add_ingredients(food):
    ingredients = []
    for ingredient in food.ingredients:
        db_ingredient = try_find_ingredient(ingredient.name)
        if db_ingredient is not None:
            append_unique(ingredients, db_ingredient)
            log("found existing ingredient: " + str(ingredient.name))
        else:
            log("Add ingredient: " + str(ingredient.name))
            db.session.add(ingredient)
            append_unique(ingredients, ingredient)
    log_ingredients(ingredients)
    return ingredients


def add_allergens(food):
    allergens = []
    for allergen in food.allergens:
        fetched_ingredient = db.session.execute(select(Allergen)
                                                .where(Allergen.id == int(allergen.name)))
        found = fetched_ingredient.scalars().first()
        allergens.append(found)
    return allergens


def add_food(food):
    food.ingredients = add_ingredients(food)
    food.category = ingredients_to_categories(food.ingredients)
    food.allergens = add_allergens(food)
    db.session.add(food)
    return food


def add_to_available(skm_food, canteen):
    # find food in db
    food = try_find_food(skm_food.name)
    # create new food if not found:
    if food is None:
        food = add_food(skm_food)
        log("adding food: " + skm_food.name)
        # add new food to db
    return CurrentFood(food, canteen)


def find_in_menu(menu, skm_food):
    for food in menu:
        #  Todo: Maybe start to compare more than one attribute
        if food.food.name == skm_food.name:
            return food
    return None


def log_menu(skm_canteen):
    log(f"Menu ma {len(skm_canteen.menu)} polozek")
    for m in skm_canteen.menu:
        log(m.name)


def update_canteen_menu(db_canteen, skm_canteen):
    # log_menu(skm_canteen)
    menu = fetch_today_menu(db_canteen.id)
    for skm_food in skm_canteen.menu:
        current_food = find_in_menu(menu, skm_food)
        if current_food is None:
            current_food = add_to_available(skm_food, db_canteen)
            db.session.add(current_food)
        else:
            current_food.last_available = datetime.now()
            current_food.available = True
            menu.remove(current_food)
    for food in menu:
        log(f"{db_canteen.name} - food not available anymore: {food}")
        food.available = False


def push_canteen_menu(skm_canteen):
    for food in skm_canteen.menu:
        current_food = add_to_available(food, skm_canteen)
        # log("adding current_food: " + food.name + " - " + skm_canteen.name)
        db.session.add(current_food)


def update_canteen(skm_canteen):
    db_canteen = find_existing_canteen(skm_canteen)
    if db_canteen is not None:
        copy_canteen_data(db_canteen, skm_canteen)
        update_canteen_menu(db_canteen, skm_canteen)
    else:
        db.session.add(skm_canteen)
        log("adding canteen:" + skm_canteen.name)
        push_canteen_menu(skm_canteen)


def update_data(data):
    with app.app_context():
        for canteen in data:
            update_canteen(canteen)
        db.session.commit()


def to_file(data):
    time_formatted = time.strftime("%d-%m-%Y--%H:%M:%S")
    f = open(f"skm_json3-{time_formatted}.json", "w")
    f.write(data)
    f.close()


def open_jsonfile(path):
    f = open(path, "r")
    data = f.read()
    f.close()
    return data


def request_skm():
    data = requests.get(skm_api_url).text
    to_file(data)
    return data


def load_data(from_file=False):
    if from_file:
        # return open_jsonfile("/usr/src/app/app/logs/skm_api/mozzarela_two_foods.json")
        return open_jsonfile("/usr/src/app/app/logs/skm_api/skm_json3-21-11-2022--11:31:15.json")
    else:
        return request_skm()


def clean_data(loaded):
    """ Cleans data to contain only elements with type 'menza' """
    data = json.loads(loaded)
    # Removes times element
    data = data[1:]
    # Exclude 'bufet'
    data = [d for d in data if d['typ'] == 'menza']
    # Remove blank element from empty menu [ { } ] -> []
    for m in data:
        if len(m['menu'][0]) == 0:
            m['menu'] = []
    return data
