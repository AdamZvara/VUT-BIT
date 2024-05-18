<template>
  <v-row justify="center">
    <div class="pl-3 pr-3 mb-2"
         v-for="(type, index) in detail_types"
         :key="index">
      <a @click="updateSelected(index)"
         class="mb-0 white--text text-subtitle-1"
      > {{ type }} </a>
      <div v-if="index === detail_type"
           class="text-underline"></div>
    </div>
  </v-row>
</template>

<script lang="ts">
import {defineComponent} from 'vue';

export default defineComponent({
  name: "FoodDetailSelector",
  data() {
    return {
      detail_types: ["Přehled", "Recenze", "Alergeny"]
    }
  },
  methods: {
    updateSelected(index: number) {
      this.$emit('detailTypeChanged', index);
    }
  },

  props: ['detail_type', 'food_id']
});
</script>

<style>
.text-underline{
  border-bottom: 5px solid white;
  border-radius: 5px;
}
</style>
