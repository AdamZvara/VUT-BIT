from ..models.models import Food, Canteen, Ingredient, Allergen
from ..enums import FoodTypeEnum
from marshmallow import Schema, EXCLUDE, fields, post_load
from datetime import datetime

class FoodTypeFeild(fields.Field):
    def _deserialize(self, value, attr, data, **kwargs):
        skm_type = value
        if skm_type == 'Hl. jídlo':
            return FoodTypeEnum.main
        if skm_type == 'Polévka':
            return FoodTypeEnum.soup
        return FoodTypeEnum.side

class TimeArrayField(fields.Field):
    def _deserialize(self, value, attr, data, **kwargs):
        time_to_parse = value
        if time_to_parse == '-':
            return None

        times = list(map(lambda x: datetime.strptime(x, "%H:%M"), time_to_parse.split('-')))
        return [times[0], times[1]]


class WeightFeild(fields.Field):
    def _deserialize(self, value, attr, data, **kwargs):
        return value if value != "" else None


class FoodAllergenFeild(fields.Field):
    def _deserialize(self, value, attr, data, **kwargs):
        if value == "":
            return []
        return [Allergen(name=v) for v in value.split(',')]


class ListOrEmpty(fields.Field):
    def _deserialize(self, value, attr, data, **kwargs):
        if len(value) == 0:
            return []
        return [Ingredient(name) for name in value]


class SkmFoodSchema(Schema):
    name = fields.Str(data_key="popis", required=True)
    name_en = fields.Str(data_key="epopis")
    food_type = FoodTypeFeild(data_key="typ")
    weight = WeightFeild(data_key="gramaz")                             
    price_student = fields.Int(data_key="cenaS")
    price_employee = fields.Int(data_key="cenaZ")
    price_extern = fields.Int(data_key="cenaE")
    allergens = FoodAllergenFeild(data_key="alergen")
    ingredients = ListOrEmpty(data_key="slozeni")

    class Meta:
        unknown = EXCLUDE

    @post_load
    def make_food(self, data, **kwargs):
        return Food(**data)


class SkmCanteenSchema(Schema):
    name = fields.Str(data_key="nazev")
    address = fields.Str(data_key="adresa")

    opened_first = TimeArrayField(data_key="otv1-4a")
    opened_second = TimeArrayField(data_key="otv1-4p")
    opened_first_friday = TimeArrayField(data_key="otv5a")
    opened_second_friday = TimeArrayField(data_key="otv5p")

    menu = fields.Nested(SkmFoodSchema, many=True)

    class Meta:
        unknown = EXCLUDE

    @post_load
    def make_canteen(self, data, **kwargs):
        return Canteen(**data)
