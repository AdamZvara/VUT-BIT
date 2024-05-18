<template>
  <v-container>
    <h3 class="pb-3">Moje hodnocení</h3>
    <food-rating
        class="pb-5"
        :food_id="current_food_id"
        @ratingSent="onRatingSent()"/>

    <h3 class="pb-5">Všechny hodnocení</h3>
    <h4 class="pb-3"
        v-if="today_ratings.length !== 0"
    >Dnes:</h4>

    <food-detail-rating-card
        v-for="(rating, index) in today_ratings"
        :key="index"
        :rating="rating"/>


    <h4 class="py-3"
        v-if="other_ratings.length !== 0">Starší:</h4>
    <food-detail-rating-card
        v-for="(rating, index) in other_ratings"
        :key="index"
        :rating="rating"/>
  </v-container>
</template>

<script lang="ts">
import {defineComponent} from "vue";
import FoodDetailRatingCard from "@/components/FoodDetail/DetailOverview/FoodDetailRatingCard.vue";
import FoodRating from "@/components/Rating/FoodRating.vue";
import {mapGetters} from "vuex";
import FoodDetailService from "@/services/FoodDetailService";
import ResponseData from "@/types/ResponseData";
import RatingListData from "@/types/RatingListData";

export default defineComponent({
  name: "FoodDetailRatings",
  components: {FoodRating, FoodDetailRatingCard},
  data() {
    return {
      today_ratings: [] as RatingListData[],
      other_ratings: [] as RatingListData[],
    }
  },
  methods: {
    onRatingSent() {
      this.getAllRatings();
    },
    getAllRatings() {
      FoodDetailService.getAllRatings(this.current_food_id)
          .then((response: ResponseData) => {
            this.today_ratings = response.data.today_ratings;
            this.other_ratings = response.data.other_ratings;
          })
    }
  },
  mounted() {
    this.getAllRatings();
  },

  computed: {
    ...mapGetters({
      current_food_id: 'getCurrentFoodId',
    }),
  }
});
</script>

<style scoped>

</style>
