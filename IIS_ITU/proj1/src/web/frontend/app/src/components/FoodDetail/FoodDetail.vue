<template>

  <div>
    <v-row justify="center" class="secondary">
      <v-col md="6">
        <food-detail-header
          :food_id="$route.params.id"
          :food_name="food_name"/>
        <food-detail-selector
          :detail_type="selected_detail_type"
          :food_id="$route.params.id"
          @detailTypeChanged="changeSelected"/>
      </v-col>
    </v-row>

    <v-row justify="center">
      <v-col justify="space-around" md="6">
        <FoodDetailOverview
          v-if="selected_detail_type === FoodDetailSelected.Overview"
          :food_id="food_id"
          :current_food_id="current_food_id"/>

        <FoodDetailRatings
          v-if="selected_detail_type === FoodDetailSelected.Ratings"
          :food_id="food_id"/>

        <FoodDetailAllergens
          v-if="selected_detail_type === FoodDetailSelected.Allergens"
          :food_id="food_id"/>

      </v-col>
    </v-row>
  </div>

</template>


<script lang="ts">
import {defineComponent} from 'vue';

import FoodDetailSelector from '@/components/FoodDetail/FoodDetailSelector.vue';
import FoodDetailHeader from '@/components/FoodDetail/FoodDetailHeader.vue';

import FoodDetailOverview from '@/components/FoodDetail/DetailOverview/FoodDetailOverview.vue'
import FoodDetailAllergens from '@/components/FoodDetail/FoodDetailAllergens.vue'
import FoodDetailRatings from '@/components/FoodDetail/FoodDetailRatings.vue'

import FoodDetailSelected from '@/types/FoodDetailSelectedEnum';

import { mapGetters } from 'vuex'

export default defineComponent({
  name: 'FoodDetail',
  components: {
    FoodDetailSelector,
    FoodDetailHeader,
    FoodDetailOverview,
    FoodDetailAllergens,
    FoodDetailRatings
  },

  data() {
    return {
      selected_detail_type: FoodDetailSelected.Overview,
      FoodDetailSelected
    }
  },

  methods: {
    changeSelected(detail_type: FoodDetailSelected) {
      this.selected_detail_type = detail_type;
    }
  },

  computed:{
    ...mapGetters({
      food_id: 'getFoodId',
      current_food_id: 'getCurrentFoodId',
      food_name: 'getFoodName',
    }),
  }

});

</script>
