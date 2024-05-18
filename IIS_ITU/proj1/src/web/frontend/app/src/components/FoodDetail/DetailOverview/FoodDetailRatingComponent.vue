<template>
    <v-row
      v-if="based_on > 0">
      <v-col align="center">
        <food-rating-circular
            :today_rating="rating"
            :based_on="based_on" />
      </v-col>
      <v-col justify="bottom">
        <v-row
            align="center"
            no-gutters
            v-for="(count, index) in count_percentages"
            :key="index">
          <v-col
              cols="3">
            {{ index + 1 }}
            <v-icon color="orange" >
              mdi-star
            </v-icon>

          </v-col>
          <v-col
              cols="7">
            <v-progress-linear
                :value="count"
                :color="'orange'"
            ></v-progress-linear>
          </v-col>

          <v-col
              class="text-caption px-4"
              cols="2">
            {{ count }}%
          </v-col>
        </v-row>
      </v-col>
    </v-row>
</template>

<script lang=ts>
import {defineComponent} from "vue";
import FoodRatingCircular from "@/components/Rating/FoodRatingCircular.vue";

export default defineComponent({
  name: "FoodDetailRatings",
  components: {
    FoodRatingCircular,
  },

  props: ['count_percentages', 'based_on', 'rating' ]
});


</script>