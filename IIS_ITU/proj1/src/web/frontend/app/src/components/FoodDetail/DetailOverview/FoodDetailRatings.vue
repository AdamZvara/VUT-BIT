<template>
  <v-container>
    <h3 class="pb-3"> Hodnocení</h3>
    <h5>Dnes</h5>
    <food-detail-rating-component
    :count_percentages="today_ratings.counts_percentage"
    :based_on="today_ratings.based_on"
    :rating="today_ratings.average"/>

    <h5>Celkově</h5>
    <food-detail-rating-component
    :count_percentages="all_ratings.counts_percentage"
    :based_on="all_ratings.based_on"
    :rating="all_ratings.average"/>
  </v-container>
</template>


<script lang="ts">

import {defineComponent} from "vue";
import FoodDetailService from "@/services/FoodDetailService";
import ResponseData from "@/types/ResponseData";
import RatingStatistics from "@/types/RatingStatistics";
import FoodDetailRatingComponent from "@/components/FoodDetail/DetailOverview/FoodDetailRatingComponent.vue";

export default defineComponent({
  name: "FoodDetailRatings",
  components: {
    FoodDetailRatingComponent
  },
  data() {
    return {
      today_ratings: {} as RatingStatistics,
      all_ratings: {} as RatingStatistics,
    }
  },

  mounted() {
    FoodDetailService.getBothRatings(this.current_food_id)
        .then((response: ResponseData) => {
          this.today_ratings = response.data.today_ratings;
          this.all_ratings = response.data.all_ratings;
        })
  },
  props: ['food_id', 'current_food_id'],
});
</script>
<style scoped>

</style>
