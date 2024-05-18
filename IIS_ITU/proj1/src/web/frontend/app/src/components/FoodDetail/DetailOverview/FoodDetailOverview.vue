<template>
  <v-container>
    <food-detail-common-info
      :food_id="food_id"/>
    <food-detail-ingredients
      :food_id="food_id"/>
    <food-detail-ratings
      :food_id="food_id"
      :current_food_id="current_food_id"/>
    <food-detail-history
      :food_id="food_id"/>

  </v-container>
</template>

<script lang="ts">
import {defineComponent} from "vue";

import FoodDetailCommonInfo from "@/components/FoodDetail/DetailOverview/FoodDetailCommonInfo.vue";
import FoodDetailHistory from "@/components/FoodDetail/DetailOverview/FoodDetailHistory.vue";
import FoodDetailIngredients from "@/components/FoodDetail/DetailOverview/FoodDetailIngredients.vue";
import FoodDetailRatings from "@/components/FoodDetail/DetailOverview/FoodDetailRatings.vue";

export default defineComponent({
  name: "FoodDetailOverview",

  components: {
    FoodDetailCommonInfo,
    FoodDetailHistory,
    FoodDetailIngredients,
    FoodDetailRatings
  },

  props: ['food_id', 'current_food_id']
});

</script>
