from flask_marshmallow import Marshmallow
from marshmallow import fields, pre_dump

from ..models.models import *
from ..enums import *
from ..common import get_random_czech_word, is_today_rating
from datetime import date

ma = Marshmallow()


class UserSchema(ma.SQLAlchemyAutoSchema):
    @pre_dump(pass_many=False)
    def change_name(self, user, many):
        if user.login == '' or user.email == 'deleteReserved':
            user.login = "Anonymní " + get_random_czech_word()
        return user

    class Meta:
        model = User
        load_instance = True
        exclude = ("password", )


user_schema = UserSchema()
users_schema = UserSchema(many=True)


class CanteenSchema(ma.SQLAlchemyAutoSchema):
    class Meta:
        model = Canteen
        load_instance = True


canteen_schema = CanteenSchema()
canteens_schema = CanteenSchema(many=True)


class AllergenSchema(ma.SQLAlchemyAutoSchema):
    class Meta:
        model = Allergen
        load_instance = True


allergen_schema = AllergenSchema()
allergens_schema = AllergenSchema(many=True)


class FoodSchema(ma.SQLAlchemyAutoSchema):
    type = ma.Enum(FoodTypeEnum)
    category = ma.List(ma.Enum(FoodCategoryEnum))
    ratings = ma.Nested('RatingSchema', many=True)

    class Meta:
        model = Food
        load_instance = True


food_schema = FoodSchema()
foods_schema = FoodSchema(many=True)


class CurrentFoodSchema(ma.SQLAlchemyAutoSchema):
    food = ma.Nested(FoodSchema, attribute='food', exclude=['ratings'])
    canteen = ma.Nested(CanteenSchema, attribute='canteen', only=['id', 'name'])

    class Meta:
        model = CurrentFood
        load_instance = True


currentfood_schema = CurrentFoodSchema()
currentfoods_schema = CurrentFoodSchema(many=True)


class RatingSchema(ma.SQLAlchemyAutoSchema):
    to_food = ma.Nested(CurrentFoodSchema)
    user = ma.Nested(UserSchema)
    added = fields.Method("formatDate", required=True)

    @classmethod
    def formatDate(cls, rating):
        return rating.added.strftime("%d.%m.%Y  %H:%M:%S")

    class Meta:
        model = Rating
        load_instance = True

    @pre_dump(pass_many=True)
    def sort_by_date(self, ratings, many):
        print(ratings)
        if many:
            ratings.sort(key=lambda x: x.added, reverse=True)  # has to be on separate line
        return ratings

rating_schema = RatingSchema()
ratings_schema = RatingSchema(many=True)


class RatingSeparateTodaySchema(ma.SQLAlchemyAutoSchema):
    today_ratings = ma.Nested(RatingSchema(exclude=['to_food'], many=True))
    other_ratings = ma.Nested(RatingSchema(exclude=['to_food'], many=True))

    @pre_dump(pass_many=True)
    def separate_today(self, ratings, many):
        if many:
            return
        return {'today_ratings': [rating for rating in ratings if is_today_rating(rating)],
               'other_ratings': [rating for rating in ratings if not is_today_rating(rating)] }

separate_today_ratings_schema = RatingSeparateTodaySchema()


class UserSelfInfo(ma.SQLAlchemyAutoSchema):
    class Meta:
        model = User
        fields = ('login', 'email')


user_self_info = UserSelfInfo()


class IngredientSchema(ma.SQLAlchemyAutoSchema):
    class Meta:
        model = Ingredient
        load_instance = True
        fields = ('name', 'is_vegan', 'is_vegetarian', 'is_gluten_free', 'is_checked')


ingredients_schema = IngredientSchema(many=True)


class AllergenSchema(ma.SQLAlchemyAutoSchema):
    class Meta:
        model = Allergen
        load_instance = True
        fields = ('name', 'descr', 'id')


allergens_schema = AllergenSchema(many=True)


class FoodIngredientSchema(ma.SQLAlchemyAutoSchema):
    ingredients = ma.Nested(IngredientSchema, many=True)

    class Meta:
        model = Food
        load_instance = True
        fields = ('ingredients',)


food_ingredients_schema = FoodIngredientSchema()


class FoodAlergensSchema(ma.SQLAlchemyAutoSchema):
    allergens = ma.Nested(AllergenSchema, many=True)

    class Meta:
        model = Food
        load_instance = True
        fields = ('name', 'allergens')


food_allergens_schema = FoodAlergensSchema()


class ValueRatingCounts(fields.Field):
    def _serialize(self, value, attr, obj, **kwargs):
        print(value)
        print(obj)
        print(attr)
        if value is None:
            return ""
        return "".join(str(d) for d in value)


class FoodRatingStatisticsSchema(ma.SQLAlchemyAutoSchema):
    counts_percentage = fields.Method("getPercentages", required=True)
    average = fields.Method("getAverage", required=True)
    based_on = fields.Method("getSum", required=True)

    @classmethod
    def getAverage(cls, ratings):
        if len(ratings) == 0:
            return 0
        sum_points = sum([rating.points for rating in ratings])
        return round(sum_points / len(ratings), 1)

    @classmethod
    def getSum(cls, ratings):
        return len(ratings)

    @classmethod
    def getPercentages(cls, ratings):
        number_of_ratings = len(ratings)
        if number_of_ratings == 0:
            return [0, 0, 0, 0, 0]
        # rating count list for rating point values 1-5
        counts = [len([rating.points for rating in ratings if rating.points == points_value])
                for points_value in range(1, 6)]
        # convert to percentages
        return [round(count / number_of_ratings * 100) for count in counts]



class FoodRatingBothStatisticsSchema(ma.SQLAlchemyAutoSchema):
    today_ratings = fields.Nested(FoodRatingStatisticsSchema, require=True)
    all_ratings = fields.Nested(FoodRatingStatisticsSchema, require=True)
    
    @pre_dump(pass_many=True)
    def create_both_statistics(self, ratings, many):
        return {'today_ratings': self.only_today_ratings(ratings),
                'all_ratings': ratings}

    @staticmethod
    def only_today_ratings(ratings):
        return [rating for rating in ratings if is_today_rating(rating)]

rating_statistics_schema = FoodRatingBothStatisticsSchema()
