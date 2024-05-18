<template>
  <v-container>
    <v-card>
      <v-card-title primary-title>
        {{ canteen_name }}
      </v-card-title>
      <v-data-table :headers="headers" :items="ratings" sort-by="points" v-if="ratings.length != 0"></v-data-table>
      <v-card-text v-else>Menza neobsahuje žádné recenze pro dané období</v-card-text>
    </v-card>
  </v-container>
</template>

<script lang="ts">
  import { defineComponent } from 'vue';
  import RatingListData from '@/types/RatingListData';
  import OverviewService from '@/services/Mod/OverviewService';
  import ResponseData from '@/types/ResponseData';


  export default defineComponent({
    name: 'CanteenRating',


    data() {
      return {
        headers: [
          {
            text: 'Název jídla',
            align: 'start',
            value: 'to_food.food.name',
          },
          { text: 'Body', value: 'points' },
          { text: 'Text', value: 'text' },
        ],
        ratings: [] as RatingListData[]
      }
    },

    methods: {
      loadRatings() {
        OverviewService.getCanteenRatings(this.canteen_id, this.dates)
          .then((response: ResponseData) => {
            this.ratings = response.data;
          })
      },
    },

    watch: {
      dates() {
        console.log('update from timepicker')
        this.loadRatings();
      }
    },

    props: [ 'canteen_name', 'canteen_id', 'dates' ],

    mounted() {
      this.loadRatings();
    }

  });
</script>