<template>
  <v-container>
    <h3 class="pb-3">Základní informace</h3>
    <h5>Jméno anglicky</h5>
    <p class="pl-4">
      {{ foodDetails.name_en }}
    </p>

    <h5>Cena</h5>
    <v-simple-table dense>
      <template v-slot:default>
        <thead>
        <tr>
          <th class="text-left">
            Strávník
          </th>
          <th class="text-left">
            Cena
          </th>
        </tr>
        </thead>
        <tbody>
        <tr
            v-for="price in getPriceTable()"
            :key="price.category"
        >
          <td>{{ price.category }}</td>
          <td>{{ price.price }}</td>
        </tr>
        </tbody>
      </template>
    </v-simple-table>

  </v-container>
</template>

<script lang="ts">
import {defineComponent} from "vue";
import FoodDetailService from "@/services/FoodDetailService";
import FoodDetailsData from "@/types/FoodDetailsData";
import ResponseData from "@/types/ResponseData";

interface PriceTableMember {
  category: string;
  price: number;
}

export default defineComponent({
  name: "FoodDetailCommonInfo",
  data() {
    return {
      foodDetails: {
        category: [],
        name: "",
        name_en: "",
        price_student: 0,
        price_employee: 0,
        price_extern: 0,
        food_type: "",
        weight: "",
      } as FoodDetailsData,
    }
  },
  methods: {
    getPriceTable() {
      return [
        {
          category: "Student",
          price: this.foodDetails.price_student
        },
        {
          category: "Zaměstnanec",
          price: this.foodDetails.price_employee
        },
        {
          category: "Externí",
          price: this.foodDetails.price_extern
        },
      ] as PriceTableMember[];
    }
  },
  mounted() {
      FoodDetailService.getFoodDetail(this.food_id)
          .then((response: ResponseData) => {
            this.foodDetails = response.data;
          })
  },
  props: ['food_id']
});
</script>

<style scoped>

</style>
