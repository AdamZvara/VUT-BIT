<template>
  <div class="pa-3">
    <div v-for="(allergen, index) in allergens" :key="index">
      <v-row align="center" class="pa-3">
        <v-avatar
            class="elevation-4"
            color="primary"
            size="65"
        >
          <div class="text-h4">
          {{ allergen.id }}</div>

        </v-avatar>

        <v-col class="pl-5">
          <h2>{{ allergen.name }}</h2>
          <div class="overline line-height-zero">
            {{ allergen.descr }}
          </div>
        </v-col>

      </v-row>

    </div>
  </div>
</template>

<script lang="ts">
import FoodDetailService from "@/services/FoodDetailService";
import {defineComponent} from "vue";
import ResponseData from "@/types/ResponseData";

export default defineComponent({
  name: "FoodDetailAllergens",
  data() {
    return {
      allergens: Array
    }
  },

  methods: {
    get_food_allergens() {
      FoodDetailService.getFoodAllergens(this.food_id)
          .then((response: ResponseData) => {
            this.allergens = response.data;
          })
    }
  },
  mounted() {
    this.get_food_allergens();
  },

  props: ['food_id']
});
</script>

<style scoped>
.line-height-zero{
  line-height: 18px;
}
</style>
