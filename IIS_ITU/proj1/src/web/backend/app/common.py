from .enums import FoodCategoryEnum
from .models.models import db, Rating
from .app import app
from datetime import timedelta, date, datetime
import random

def ingredients_to_categories(ingredients):
    if len(ingredients) == 0:
        return []
    gluten_category = False not in {ingredient.is_gluten_free for ingredient in ingredients}
    vegan_category = False not in {ingredient.is_vegan for ingredient in ingredients}
    vegetarian_category = False not in {ingredient.is_vegetarian for ingredient in ingredients}
    categories = []
    # equivalent of if (vegetarian_category): out_categories.append(...)
    vegetarian_category and categories.append(FoodCategoryEnum.vegetarian)
    vegan_category and categories.append(FoodCategoryEnum.vegan)
    gluten_category and categories.append(FoodCategoryEnum.gluten)
    return categories

def seed_ratings():
    # Ratings
    ratings = [
        Rating(5, 2, 1, "Vynikajuce"),
        Rating(4, 3, 1, "Dobre"),
        Rating(3, 4, 1, "OK"),
        Rating(2, 5, 1, "ZLE"),
        Rating(3, 6, 1, "Humus"),
        Rating(4, 2, 2, "Fuj"),
        Rating(5, 3, 2, "Uzasne"),
        Rating(4, 4, 20, "Lepsie som nejedol"),
        Rating(3, 5, 12, "Vynikajuce"),
        Rating(2, 6, 26, "Mohlo to byt aj lepsie"),
        Rating(4, 2, 30, "Fuj"),
        Rating(5, 3, 3, "Uzasne"),
        Rating(4, 4, 3, "Lepsie som nejedol"),
        Rating(3, 5, 4, "Vynikajuce"),
        Rating(2, 6, 2, "Mohlo to byt aj lepsie"),
        Rating(4, 4, 52, "Lepsie som nejedol"),
        Rating(3, 5, 52, "Vynikajuce"),
        Rating(2, 6, 52, "Mohlo to byt aj lepsie"),
        Rating(5, 2, 12, "Uzasne", added=datetime.now() - timedelta(days=1)),
        Rating(4, 3, 14, "Lepsie som nejedol", added=datetime.now() - timedelta(days=3)),
        Rating(3, 4, 2, "Vynikajuce", added=datetime.now() - timedelta(days=1)),
        Rating(2, 5, 1, "Mohlo to byt aj lepsie", added=datetime.now() - timedelta(days=2)),
        Rating(1, 6, 38, "Fuj", added=datetime.now() - timedelta(days=2)),
        Rating(5, 2, 3, "Uzasne", added=datetime.now() - timedelta(days=2)),
        Rating(4, 3, 39, "Lepsie som nejedol", added=datetime.now() - timedelta(days=1)),
        Rating(3, 4, 4, "Vynikajuce", added=datetime.now() - timedelta(days=1)),
        Rating(1, 5, 2, "Mohlo to byt aj lepsie", added=datetime.now() - timedelta(days=4)),

      ]

    with app.app_context():
        db.session.execute(db.delete(Rating))
        db.session.commit()
        db.session.add_all(ratings)
        db.session.commit()


def get_random_czech_word():
    with open("/usr/src/app/app/static_files/czech_words") as fp:
        rnd_line = random.randint(0, 2329)
        for i, line in enumerate(fp):
            if i == rnd_line:
                return line.lower().strip() # strip to remove newline at the end of line

def is_today_rating(rating):
    return rating.added.day == date.today().day
