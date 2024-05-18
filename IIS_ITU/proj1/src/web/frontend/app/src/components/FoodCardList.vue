<template>
  <v-container>
    <FoodCard
        v-for="(food, index) in foods"
        :food_id="food.food.id"
        :current_food_id="food.id"
        :name="food.food.name"
        :price_student="food.food.price_student"
        :price_employee="food.food.price_employee"
        :price_extern="food.food.price_extern"
        :category="food.food.category"
        :type="food.food.food_type"
        :food_availablity="food.available"
        :today_rating="todayRatings.find(item => item.food_id == food.id)"
        :overall_rating="overallRatings.find(item => item.food_id == food.id)"
        :key="index"
        @ratingSent="onRatingSent"
    />
  </v-container>
</template>

<script lang="ts">
import {defineComponent} from "vue";

import FoodService from "@/services/FoodService";

import CurrentFoodData from "@/types/CurrentFoodData";
import ResponseData from '@/types/ResponseData';
import CurrentFoodRating from '@/types/CurrentFoodRating';

import FoodCard from '@/components/FoodCard.vue';

export default defineComponent({
  name: 'FoodCardList',

  components: {
    FoodCard
  },

  data() {
    return {
      foods: [] as CurrentFoodData[],
      todayRatings: [] as CurrentFoodRating[],
      overallRatings: [] as CurrentFoodRating[],
    };
  },

  methods: {

    loadFood() {
      FoodService.findByCanteen(this.canteen_id)
          .then((response: ResponseData) => {
            this.foods = response.data;
          })
    },

    getTodayRatings() {
      FoodService.getTodayRatings(this.canteen_id)
          .then((response: ResponseData) => {
            this.todayRatings = response.data;
          })
    },

    getOverallRatings() {
      FoodService.getOverallRatings(this.canteen_id)
          .then((response: ResponseData) => {
            this.overallRatings = response.data;
          })
    },

    onRatingSent() {
      setTimeout(this.getTodayRatings, 1000);
    },

  },

  mounted() {
    setInterval(this.getTodayRatings, 10000); // update ratings every 10 seconds
  },

  watch: {
    // Set watcher for canteen id and each time its value is changed (selector changes)
    // update food list
    canteen_id() {
      this.loadFood();
      this.getTodayRatings();
      this.getOverallRatings();
    }
  },

  props: ['canteen_id']

});

</script>

<style scoped>

</style>
