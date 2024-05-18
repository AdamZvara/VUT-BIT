<template>
  <v-container fluid  class="px-0">
    <div v-show="openDetail === false">
      <div class="secondary pb-5 px-5 ">
        <v-row justify="center" class="pt-4">
          <h4 class="white--text">Výběr menzy</h4>
        </v-row>
        <v-row justify="center">
          <v-col md="4">
            <canteen-selector :default_canteen="1" @clicked="onClickCanteen"/>
          </v-col>
        </v-row>
      </div>

      <v-row
          justify="center"
      >
        <v-col justify="space-around" md="5">
          <food-card-list
              :canteen_id="canteen_id"/>
        </v-col>
      </v-row>
    </div>


    <food-detail
        v-if="openDetail === true"/>
  </v-container>
</template>

<script lang="ts">
import Vue from 'vue';
import FoodCardList from "@/components/FoodCardList.vue";
import CanteenSelector from "@/components/CanteenSelector.vue";
import FoodDetail from "@/components/FoodDetail/FoodDetail.vue";
import { mapGetters } from 'vuex'

export default Vue.extend({
  name: "HomeView",
  components: {
    FoodCardList,
    CanteenSelector,
    FoodDetail
  },

  data() {
    return {
      canteen_id: 0,
    };
  },

  methods: {
    onClickCanteen(value: number) {
      this.canteen_id = value;
    },
  },

  computed:{
    ...mapGetters({
      openDetail: 'getDetailOpened',
    }),
  }

})
;

</script>
