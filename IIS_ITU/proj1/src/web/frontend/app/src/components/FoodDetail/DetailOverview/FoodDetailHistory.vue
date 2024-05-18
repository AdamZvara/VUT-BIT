<template>

  <v-container
   v-if="implemented">
    <h3>Historie dostupnosti jídla</h3>
  </v-container>
</template>

<script lang="ts">
import {defineComponent} from "vue";

export default defineComponent( {
  name: "FoodDetailHistory",
  data() {
    return {
      implemented: false
    }
  },
  methods: {

  },
  props: ['food_id']
});
</script>

<style scoped>

</style>
