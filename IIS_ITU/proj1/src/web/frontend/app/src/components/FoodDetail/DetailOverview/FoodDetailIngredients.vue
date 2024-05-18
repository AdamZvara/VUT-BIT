<template>
  <v-container>
    <h5>Ingredience</h5>
    <p class="pl-4">
      {{ getIngredientList() }}
    </p>
    <!--
    <v-simple-table dense>
      <template v-slot:default>
        <thead>
        <tr>
          <th class="text-left">
            Ingredience
          </th>
          <th class="text-left">
            Veganské
          </th>
          <th class="text-left">
            Vegetarianské
          </th>
          <th class="text-left">
            Bezlepkové
          </th>
        </tr>
        </thead>
        <tbody>
        <tr
            v-for="ingredient in ingredients"
            :key="ingredient.name"
        >
          <td>{{ ingredient.name }}</td>
          <td>{{ ingredient.is_vegan }}</td>
          <td>{{ ingredient.is_vegetarian }}</td>
          <td>{{ ingredient.is_gluten_free }}</td>
        </tr>
        </tbody>
      </template>
    </v-simple-table>
    -->
  </v-container>
</template>

<script lang="ts">

import FoodDetailService from "@/services/FoodDetailService";
import {defineComponent} from "vue";
import ResponseData from "@/types/ResponseData";
import IngredientType from "@/types/IngredientType";

export default defineComponent( {
  name: "FoodDetailIngredients",
  data() {
    return {
      ingredients: new Array<IngredientType>()
    }
  },
  methods: {
    getIngredientList(){
      const names = this.ingredients.map((ingredient) => ingredient.name);
      return names.join(", ");
    }
  },
  mounted() {
      FoodDetailService.getIngredients(this.food_id)
          .then((response: ResponseData) => {
            this.ingredients = response.data.ingredients;
          })
  },
  props: ['food_id']
});
</script>

<style scoped>

</style>
