<template>
  <v-container fluid>
    <v-row justify="center" class="secondary">
      <v-col md="6">
        <ReviewsSummary/>
      </v-col>

    </v-row>
    <v-row class="fill-height" justify="center">
      <v-col md="6">
        <H2>Recenze</H2>
      </v-col>

    </v-row>
    <v-row justify="center" v-if="loggedIn">
      <v-col md="6">
        <ReviewDetail
            v-for="(rating, index) in ratings"
            :Ratingid="rating.id"
            :key="index"
            :CanteenName="rating.to_food.canteen.name"
            :FoodName="rating.to_food.food.name"
            :RatingDate="rating.added"
            :RatingText="rating.text"
            :RatingValue="rating.points"
            @deletedReview="onDeleteRating()"/>
      </v-col>
    </v-row>

  </v-container>
</template>

<script lang="ts">
import Vue from 'vue';
import {mapState} from 'vuex'
import ReviewsSummary from '@/components/Review/ReviewsSummary.vue'
import ReviewDetail from '@/components/Review/ReviewDetail.vue'
import RatingService from '@/services/RatingService';
import RatingListData from '@/types/RatingListData'
import ResponseData from '@/types/ResponseData';

export default Vue.extend({
  name: 'RatingView',

  data() {
    return {
      ratings: [] as RatingListData[]
    }
  },

  methods: {
    loadRatings() {
      RatingService.getMyRatings()
          .then((response: ResponseData) => {
            this.ratings = response.data;
            console.log(this.ratings);
          })
    },
    onDeleteRating() {
      this.loadRatings();
    }
  },

  components: {
    ReviewsSummary,
    ReviewDetail
  },

  computed: mapState({
    loggedIn: 'loggedIn'
  }),

  beforeMount() {
    if (this.loggedIn) {
      this.loadRatings()
    }
  }
});
</script>
