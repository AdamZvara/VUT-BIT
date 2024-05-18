<template>
  <v-container>
    <v-card class="mb-2 " :disabled="food_availablity == false"
            :style="{background: food_availablity == false ? 'lightgray': ''}"
    >

      <v-row>
        <v-col cols="9" class="d-flex justify-space-between flex-column">
          <!-- Food name and icon -->
          <v-card-title primary-title style="word-break: break-word">
            <!-- add icons depending on food type -->
            <v-icon v-if="type === 0" class="mr-2">
              mdi-food-drumstick
            </v-icon>
            <v-icon v-else-if="type === 1" class="mr-2">
              mdi-noodles
            </v-icon>
            <v-icon v-else-if="type === 2" class="mr-2">
              mdi-french-fries
            </v-icon>
            <v-icon v-else-if="type === 3" class="mr-2">
              mdi-bowl
            </v-icon>
            <v-icon v-else-if="type === 4" class="mr-2">
              mdi-cake
            </v-icon>
            <a @click="onFoodDetailSelected">
              {{ name }}
            </a>
          </v-card-title>
          <!-- Food price -->
          <v-card-text>
            <small>stud./zam./veř.</small><br>
            $ {{ price_student }}/{{ price_employee }}/{{ price_extern }}
          </v-card-text>
          <!-- Food categories -->
          <div>
            <v-chip class="mr-2 ml-5"
                    v-for="c in category"
                    :key="c"
            >{{ (c == 'vegan') ? "veganské" : (c == 'vegetarian' ? 'vegetariánské' : 'bezlepkové') }}
            </v-chip>
          </div>

        </v-col>
        <v-col cols="3" class="text-right">
          <!-- Food ratings -->
          <div class="mt-3" align="center"
               v-if="today_rating">
            <food-rating-circular
                :today_rating="today_rating.rating"
                :based_on="today_rating.count"/>
          </div>
          <!-- Show rating button -->
          <v-spacer></v-spacer>

          <v-btn class="mr-8"
                 icon
                 justify="right"
                 @click="showRating = !showRating"
          >
            <v-icon large>{{ showRating ? 'mdi-chevron-up' : 'mdi-chevron-down' }}</v-icon>
          </v-btn>
        </v-col>

      </v-row>


      <v-expand-transition>
        <div v-show="showRating" class="my-2 pb-4">
          <v-divider></v-divider>
          <v-row class="ma-2 pl-4" v-if="today_rating">
            <v-col cols="2">
              <h4>Dnes</h4>
            </v-col>

            <v-col class="mt-2" md="8">
              <v-progress-linear
                  :value="toPercent(today_rating.rating)"
                  :color="today_rating.rating < 2.3 ? 'red' : (today_rating.rating < 3.6 ? 'orange' : 'green')"
              ></v-progress-linear>
            </v-col>
          </v-row>

          <v-row class="ma-2 pl-4" v-if="overall_rating">
            <v-col cols="2">
              <h4>Celkově</h4>
            </v-col>

            <v-col class="mt-2" md="8">
              <v-progress-linear
                  :value="toPercent(overall_rating.rating)"
                  :color="overall_rating.rating < 2.3 ? 'red' : (overall_rating.rating < 3.6 ? 'orange' : 'green')"
              ></v-progress-linear>
            </v-col>
          </v-row>

          <!-- Display this only if user is logged in -->
          <v-container>
            <v-row justify="center">
              <FoodRating
                  :food_id="current_food_id"
                  @ratingSent="onRatingSent"></FoodRating>

            </v-row>
          </v-container>
        </div>
      </v-expand-transition>
    </v-card>
  </v-container>
</template>

<script lang="ts">
import {defineComponent} from 'vue';
import FoodRating from '@/components/Rating/FoodRating.vue';
import FoodRatingCircular from '@/components/Rating/FoodRatingCircular.vue';
import {mapState} from "vuex";
import UserRoleEnum from '@/enums';

export default defineComponent({
  name: 'FoodCard',

  components: {
    FoodRatingCircular,
    FoodRating,
  },

  data() {
    return {
      showRating: false,
    };
  },

  methods: {
    onRatingSent() {
      console.log(this.type)
      this.showRating = false;
      this.$emit('ratingSent');
    },
    onFoodDetailSelected() {
      this.$store.dispatch('selectFood',
          {
            food_id: this.food_id,
            current_food_id: this.current_food_id,
            food_name: this.name
          })
    },
    toPercent(value: number) {
      return (value - 1) / 4 * 100;
    }
  },

  props: ['food_id', 'name', 'price_student', 'price_employee', 'price_extern', 'type', 'category', 'today_rating', 'overall_rating', 'current_food_id', "food_availablity"],
});
</script>
