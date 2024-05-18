<template>
  <v-row no-gutters>
    <v-col class="flex-shrink-1" cols="1">
      <v-icon
          @click="onCloseDetail"
          large> mdi-arrow-left
      </v-icon>
    </v-col>
    <v-col class="flex-grow-1">
      <v-row justify="center">
        <v-card-title class="white--text px-8" style="word-break: break-word">
          {{ food_name }}
        </v-card-title>
      </v-row>
    </v-col>

    <v-col cols="1">
      <!-- Just an empty col for food_name centering  -->
    </v-col>
  </v-row>
</template>

<script lang="ts">
import {defineComponent} from "vue";

export default defineComponent({
  name: "FoodDetailHeader",

  methods: {
    onCloseDetail(){
      this.$store.dispatch("closeDetail");
    }
  },

  props: ["food_id", "food_name"]
});
</script>
