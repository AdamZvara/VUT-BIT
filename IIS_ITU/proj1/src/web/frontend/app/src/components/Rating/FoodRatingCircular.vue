<template>
  <v-col>
    <v-progress-circular
        :rotate="270"
        :size="65"
        :width="5"
        :value="toPercent(today_rating)"
        :color="today_rating < 2.3 ? 'red' : (today_rating < 3.6 ? 'orange' : 'green')"
    >
      {{ today_rating }}
    </v-progress-circular>
    <br>
    <small>{{ based_on }} hodnocení</small>
  </v-col>
</template>

<script lang="ts">
import {defineComponent} from "vue";

export default defineComponent({
  name: "FoodRatingCircular",

  methods: {
    toPercent(value: number) {
      return (value - 1) / 4 * 100;
    }
  },
  props: ['today_rating', 'based_on'],

});

</script>
