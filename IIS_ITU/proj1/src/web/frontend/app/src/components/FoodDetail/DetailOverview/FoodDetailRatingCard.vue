<template>
  <v-card
    class="mb-5">
    <v-list-item two-line>
      <v-list-item-content>
        <v-list-item-title class="text-h5 text-wrap">
          {{ rating.user.login }}
        </v-list-item-title>
        <v-list-item-subtitle>{{ rating.added }}</v-list-item-subtitle>
        <v-rating background-color="orange lighten-3" readonly size="18" color="orange" class="mt-0 mb-2"
                  :value="rating.points"></v-rating>
        {{ rating.text }}

      </v-list-item-content>
    </v-list-item>
  </v-card>
</template>

<script lang="ts">
import {defineComponent} from "vue";

export default defineComponent({
  name: "FoodDetailRatingCard",
  data() {
    return {}
  },
  props: ['rating']
});
</script>

<style scoped>

</style>
