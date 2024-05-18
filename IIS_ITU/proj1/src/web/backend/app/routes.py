from flask import Blueprint, request, Response, redirect, flash, jsonify, current_app, json
from .models.models import *
from .schemas.schemas import *
from .authMethods import *
from .common import ingredients_to_categories
from datetime import timedelta, date
from werkzeug.security import generate_password_hash
from sqlalchemy import cast, Date
from sqlalchemy.sql import text

import jwt

api = Blueprint('api', __name__)

session_time_hrs = 2

# User API definitions #


@api.route('/users', methods=['GET'])
@admin_required
def get_users(admin):
    users = db.session.execute(db.select(User))
    return users_schema.dump(users.scalars().all())


@api.route('admin/user_create', methods=['POST'])
@admin_required
def create_user(admin):
    email, password = get_user_credentials(request)
    login = get_user_login(request)
    role = get_user_role(request)
    new_email = get_user_newemail(request)

    user = db.session.execute(db.select(User).where(User.email == new_email))
    user = user.scalars().all()

    if user:
        return Response(f"User with {email} exists!", status=409, mimetype='application/json')

    new_user = User(password=password, email=new_email, login=login, role=role)
    db.session.add(new_user)
    db.session.commit()
    return Response("OK", status=200)


@api.route('admin/user_block', methods=['POST'])
@admin_required
def delete_user(admin):
    email, _ = get_user_credentials(request)
    blocked = get_user_blocked(request)

    db.session.execute(db.update(User)
                       .where(User.email == email)
                       .values(blocked=blocked))
    db.session.commit()
    return Response("OK", status=200)


@api.route('admin/user_update', methods=['POST'])
@admin_required
def update_user(admin):
    email, password = get_user_credentials(request)
    login = get_user_login(request)
    role = get_user_role(request)
    blocked = get_user_blocked(request)
    new_email = get_user_newemail(request)

    db.session.execute(db.update(User)
        .where(User.email == email)
        .values(email = new_email, login = login, blocked=blocked, role=role))
    if (len(password)):
        db.session.execute(db.update(User)
            .where(User.email == email)
            .values(password = generate_password_hash(password)))
    # todo: check for errors (e.g user does not exist)
    db.session.commit()
    return Response("OK", status=200)


@api.route('/user/self', methods=['GET'], strict_slashes=False)
@token_required
def get_self_user(user):
    users = db.session.execute(db.select(User).where(
        User.email == user["email"]))
    return user_self_info.dump(users.scalars().first())


@api.route('/user/self', methods=['POST'], strict_slashes=False)
@token_required
def set_self_user(user):
    # todo: when user want to update password, he needs to send original password too
    data = request.get_json(force=True)
    if (data["new_password"] != ""):
        db.session.execute(db.update(User)
                           .where(User.email == user["email"])
                           .values(email=data["email"], login=data["login"], password=generate_password_hash(data["new_password"])))
    else:
        db.session.execute(db.update(User)
                           .where(User.email == user["email"])
                           .values(email=data["email"], login=data["login"]))
    db.session.commit()
    token = jwt.encode({
        'sub': data['email'],
        'iat': datetime.utcnow(),
        'exp': datetime.utcnow() + timedelta(minutes=session_time_hrs)},
        current_app.config['SECRET_KEY'])

    return jsonify({'token': token})


@api.route('/user/delete', methods=['POST'], strict_slashes=False)
@token_required
def delete_self_user(user):
    # save user reviews by setting their user_id to special account with id == 1 (used for keeping FK on rating valid)
    user_id = db.session.execute(db.select(User.id).where(
        User.email == user['email'])).scalars().first()
    db.session.execute(db.update(Rating).where(
        Rating.user_id == user_id).values(user_id=1))
    db.session.execute(db.delete(User).where(
        User.email == user['email']))
    db.session.commit()
    return Response("OK", status=200)

# Canteens API definitions #


@api.route('/canteens', methods=['GET'])
def get_canteens():
    canteens = db.session.execute(db.select(Canteen))
    return canteens_schema.dump(canteens.scalars().all())

# Food API definitions #


@api.route('/foods', methods=['GET'])
def get_foods():
    foods = db.session.execute(db.select(Food))
    return foods_schema.dump(foods.scalars().all())


@api.route('/current_foods', methods=['GET'])
def get_current_foods():
    current_foods = db.session.execute(db.select(CurrentFood))
    return currentfoods_schema.dump(current_foods.scalars().all())

# Ingredient API definitions

@api.route('/ingredients', methods=['GET'])
@admin_required
def get_ingredients(admin):
    ingredients = db.session.execute(db.select(Ingredient))
    return ingredients_schema.dump(ingredients.scalars().all())

@api.route('/ingredients', methods=['POST'])
@admin_required
def update_ingredients(admin):
    ingredients = request.get_json(force=True)
    for i in ingredients:
        db.session.execute(db.update(Ingredient)
            .where(Ingredient.name == i['name'])
            .values(is_checked = i['is_checked'])
            .values(is_gluten_free = i['is_gluten_free'])
            .values(is_vegan = i['is_vegan'])
            .values(is_vegetarian = i['is_vegetarian']))
    update_food_categories()
    db.session.commit()
    return Response("OK", status=200)

def update_food_categories():
    foods = db.session.execute(db.select(Food)).scalars().all()
    for food in foods:
        if (len(food.ingredients) == 0):
            continue
        categories = ingredients_to_categories(food.ingredients)
        db.session.execute(db.update(Food)
            .where(Food.id == food.id)
            .values(category = categories if len(categories) > 0 else None))
    db.session.commit()

# Food detail definitions #

@api.route('/food_detail/<food_id>', methods=['GET'])
def get_food_details(food_id):
    food = db.session.execute(db.select(Food).where(Food.id == food_id)).scalars().first()
    return food_schema.dump(food)

@api.route('/food_detail/ingredients/<food_id>', methods=['GET'])
def get_food_ingredients(food_id):
    food = db.session.execute(db.select(Food).where(
        Food.id == food_id)).scalars().first()
    return food_ingredients_schema.dump(food)


@api.route('/food_detail/allergens/<food_id>', methods=['GET'])
def get_food_allergens(food_id):
    food = db.session.execute(db.select(Food).where(Food.id == food_id)).scalars().first()
    return allergens_schema.dump(food.allergens)


@api.route('/food_detail/name/<food_id>', methods=['GET'])
def get_food_name(food_id):
    food = db.session.execute(db.select(Food).where(
        Food.id == food_id)).scalars().first()
    return jsonify({"food_name": food.name})


@api.route('/food_detail/all_ratings/<current_food_id>', methods=['GET'])
def all_ratings(current_food_id):
    current_food = db.session.execute(db.select(CurrentFood).where(CurrentFood.id == current_food_id)
                                      ).scalars().first()

    food = db.session.execute(db.select(Rating)
                              .where(Rating.food_id == current_food_id)
                              .where(Rating.to_food.has(CurrentFood.canteen_id == current_food.canteen_id))
                              ).scalars().all()
    return separate_today_ratings_schema.dump(food)


@api.route('/food_detail/both_ratings/<current_food_id>', methods=['GET'])
def get_food_both_ratings(current_food_id):
    current_food = db.session.execute(db.select(CurrentFood).where(CurrentFood.id == current_food_id)
                                      ).scalars().first()

    current_foods = db.session.execute(db.select(CurrentFood)
                                 .where(CurrentFood.food_id == current_food.food_id)
                                 .where(CurrentFood.canteen_id == current_food.canteen_id)
                                 ).scalars().all()

    ratings = [rating for food in current_foods for rating in food.ratings]
    return rating_statistics_schema.dump(ratings)

@api.route('/current_foods/<canteen_id>', methods=['GET'])
def get_canteenfoods(canteen_id):
    currfoods = db.session.execute(db.select(CurrentFood)
                                   .where(CurrentFood.canteen_id == canteen_id)
                                   .where(CurrentFood.last_available >= date.today()))
    return currentfoods_schema.dump(currfoods.scalars().all())


def prepare_output(rating):
    output = []
    for row in rating:
        output.append({"rating": float(row[0]), "count": int(
            row[1]), "food_id": int(row[2])})
    return output


@api.route('/current_foods/<canteen_id>/rating_today', methods=['GET'])
def get_todayrating(canteen_id):
    stmt = db.select(db.func.round(db.func.avg(Rating.points), 1), db.func.count(CurrentFood.id), CurrentFood.id)\
        .join(CurrentFood, CurrentFood.id == Rating.food_id)\
        .where(Rating.added > date.today())\
        .where(CurrentFood.canteen_id >= canteen_id)\
        .group_by(CurrentFood.id)
    foodratings = db.session.execute(stmt)
    out = prepare_output(foodratings)
    return json.dumps(out)


@api.route('/current_foods/<canteen_id>/rating_overall', methods=['GET'])
def get_overallrating(canteen_id):
    stmt = db.select(db.func.round(db.func.avg(Rating.points), 1), db.func.count(CurrentFood.id), CurrentFood.id)\
        .join(CurrentFood, CurrentFood.id == Rating.food_id)\
        .where(CurrentFood.canteen_id == canteen_id)\
        .group_by(CurrentFood.id)
    foodratings = db.session.execute(stmt)
    out = prepare_output(foodratings)
    return json.dumps(out)

# Rating API definitions

@api.route('/my_ratings', methods=['GET'])
@token_required
def my_ratings(user):
    ratings = db.session.execute(
        db.select(Rating).where(Rating.user_id == user['id']))
    return ratings_schema.dump(ratings.scalars().all())


@api.route('/rating', methods=['POST'])
@token_required
def new_rating(user):
    data = request.get_json(force=True)

    q = db.session.execute(db.select(
        Rating
    ).where(cast(Rating.added, Date) == date.today()
    ).where(Rating.user_id == user["id"]
    ).where(Rating.food_id == data["food_id"] )

    )

    if q.first() != None:
        return Response("Rating was already added!", status=409)

    points = data['points']
    text = data['text']
    user_id = user['id']
    food_id = data['food_id']

    db.session.add(Rating(points=points, text=text,
                   user_id=user_id, food_id=food_id, added=datetime.now()))
    db.session.commit()
    return Response("OK", status=200)


@api.route('/my_ratings/remove', methods=['POST'])
@token_required
def rm_rating(user):
    data = request.get_json(force=True)

    db.session.execute(db.delete(Rating)
                       .where(Rating.user_id == user['id'])
                       .where(Rating.id == data["r"]))
    db.session.commit()
    return Response("OK", status=200)


@api.route('/my_ratings/edit', methods=['POST'])
@token_required
def edit_rating(user):
    data = request.get_json(force=True)
    db.session.execute(db.update(Rating)
                       .where(Rating.id == data['id'])
                       .values(text=data['text'])
                       .values(points=data['val']))
    db.session.commit()
    return Response("OK", status=200)


@api.route('/ratings', methods=['POST'])
@admin_required
def filtered_ratings(admin):
    data = request.get_json(force=True)
    query = db.select(Rating)
    if (len(data['email'])):
        query = query.where(Rating.user.has(User.email == data['email']))
    if (data['canteen_id'] != -1 and data['canteen_id'] != None):
        print(data['canteen_id'])
        query = query.where(Rating.to_food.has(CurrentFood.canteen_id == data['canteen_id']))
    query = query.where(cast(Rating.added, Date).between(data['dates'][0], data['dates'][1]))
    result = db.session.execute(query).scalars().all()
    return ratings_schema.dumps(result)

@api.route('/ratings/delete', methods=['POST'])
@admin_required
def delete_ratings(admin):
    data = request.get_json(force=True)
    db.session.execute(db.delete(Rating).where(Rating.id.in_(data['ids'])))
    db.session.commit()
    return Response("OK", status=200)

# Authentication API definitions

@api.route('/signup', methods=['POST'])
def signup_post():
    email, password = get_user_credentials(request)
    login = get_user_login(request)

    user = db.session.execute(db.select(User).where(User.email == email))
    user = user.scalars().all()

    if user:
        return Response(f"User with {email} exists!", status=409, mimetype='application/json')

    new_user = User(password=password, email=email, login=login)
    db.session.add(new_user)
    db.session.commit()
    return Response("OK", status=200)


@api.route('/login', methods=["POST"])
def login():
    email, password = get_user_credentials(request)

    if not (email and password):
        return jsonify({'message': 'Invalid credentials', 'authenticated': False}), 401

    auth_user = db.session.execute(db.select(User).where(User.email == email))
    auth_user = auth_user.scalars().first()
    if not auth_user:
        return jsonify({'message': 'Invalid credentials', 'authenticated': False}), 401

    if not (auth_user.verify_password(password)):
        return jsonify({'message': 'Invalid credentials', 'authenticated': False}), 401

    if (auth_user.blocked == True):
        return jsonify({'message': 'Blocked user', 'authenticated': False}), 403

    role = auth_user.role
    token = jwt.encode({
        'sub': email,
        'role': role,
        'iat': datetime.utcnow(),
        'exp': datetime.utcnow() + timedelta(hours=session_time_hrs)},
        current_app.config['SECRET_KEY'])

    return jsonify({'token': token, 'role': auth_user.role})

# Stats API definitions


@api.route('/stats/rating/count/self', methods=['GET'])
@token_required
def stats_rating(user):
    user_id = user['id']

    cRating = db.session.execute(
        db.select(db.func.count(Rating.id))
        .where((Rating.user_id == user_id)))\
        .scalar_one()
    textRatings = db.session.execute(
        db.select(db.func.count(Rating.id))
        .where((Rating.user_id == user_id) & (Rating.text != '')))\
        .scalar_one()
    return jsonify(
        {
            "ratings": cRating,
            "textRatings": textRatings,
            "achievements": 0
        }
    )


def outputForTopFood(rating):
    output = []
    for row in rating:
        output.append({"avg_rating": float(row[2]), "count": int(
            row[3]), "food_name": row[0], "cateen_name": row[1]})
    return output


@api.route('/stats/rating/topfoods/<count>', methods=['GET'])
def stats_topfood(count):
    stmt = db.select(Food.name, Canteen.name, db.func.round(db.func.avg(Rating.points), 2).label('avrg'), db.func.count(Rating.id))\
        .join(CurrentFood, Rating.food_id == CurrentFood.id)\
        .join(Food, CurrentFood.food_id == Food.id)\
        .join(Canteen, CurrentFood.canteen_id == Canteen.id)\
        .group_by(CurrentFood.id, Canteen.id, Food.name)\
        .order_by(text('avrg DESC'))\
        .limit(int(count))
    foodratings = db.session.execute(stmt)
    out = outputForTopFood(foodratings)
    return json.dumps(out)


@api.route('/stats/rating/daily/topfoods/<count>', methods=['GET'])
def stats_topfood_today(count):
    stmt = db.select(Food.name, Canteen.name, db.func.round(db.func.avg(Rating.points), 2).label('avrg'), db.func.count(Rating.id))\
        .join(CurrentFood, Rating.food_id == CurrentFood.id)\
        .join(Food, CurrentFood.food_id == Food.id)\
        .where(CurrentFood.last_available == date.today().strftime('%Y-%m-%d'))\
        .join(Canteen, CurrentFood.canteen_id == Canteen.id)\
        .group_by(CurrentFood.id, Canteen.id, Food.name)\
        .order_by(text('avrg DESC'))\
        .limit(int(count))
    print( date.today().strftime('%Y-%m-%d'))
    foodratings = db.session.execute(stmt)
    out = outputForTopFood(foodratings)
    return json.dumps(out)

# Moderator API definitions

@api.route('/overview/<canteen_id>/<date_1>_<date_2>', methods=['GET'])
@moderator_required
def get_canteen_overview(mod, canteen_id, date_1, date_2):
    stmt = db.select(Rating).where(Rating.to_food.has(CurrentFood.canteen_id == canteen_id))
    stmt = stmt.where(cast(Rating.added, Date).between(date_1, date_2))
    stmt = db.session.execute(stmt).scalars().all()
    return ratings_schema.dumps(stmt)