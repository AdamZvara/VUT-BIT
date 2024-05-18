import enum
from enum import IntEnum

class DaysOfWeekEnum(IntEnum):
    monday    = 0
    tuesday   = 1
    wednesday = 2
    thursday  = 3
    friday    = 4
    saturday  = 5
    sunday    = 6

class FoodTypeEnum(IntEnum):
    main   = 0
    soup   = 1
    side   = 2
    salad  = 3
    desert = 4

class FoodCategoryEnum(IntEnum):
    vegetarian = 0
    vegan = 1
    gluten = 2

class UserRoleEnum(IntEnum):
    admin = 0
    mod   = 1
    user  = 2
