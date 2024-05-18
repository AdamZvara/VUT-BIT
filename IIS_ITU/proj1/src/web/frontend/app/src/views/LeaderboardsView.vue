<template>
  <v-container fluid>
    <leaderboard-selector
    @leaderboardChanged="changeSelected"/>

    <v-row justify="center">
      <v-col justify="space-around" md="6">
        <LB_FoodOfTheDay
          v-if="selected_detail_type === LeaderboardSelected.FoodOfTheDay"
        />

        <LB_TopFood
          v-if="selected_detail_type === LeaderboardSelected.TopFood"
        />

        <LB_TopCanteen
          v-if="selected_detail_type === LeaderboardSelected.TopCanteen"
        />

        <LB_FoodCemetery
          v-if="selected_detail_type === LeaderboardSelected.FoodCemetery"
        />


      </v-col>
    </v-row>

  </v-container>
</template>

<script lang="ts">
import Vue from 'vue';
import LeaderboardSelector from '@/components/Leaderboards/LeaderboardSelector.vue'
import LB_FoodCemetery from '@/components/Leaderboards/LB_FoodCemetery.vue'
import LB_TopCanteen from '@/components/Leaderboards/LB_TopCanteen.vue'
import LB_TopFood from '@/components/Leaderboards/LB_TopFood.vue'
import LB_FoodOfTheDay from '@/components/Leaderboards/LB_FoodOfTheDay.vue'
import LeaderboardSelected from "@/types/LeaderboardSelectedEnum";

export default Vue.extend({
  name: 'LeaderboardView',

  data() {
    return {
      selected_detail_type: LeaderboardSelected.FoodOfTheDay,
      LeaderboardSelected
    }
  },

  methods: {
    changeSelected(leaderboard_type: LeaderboardSelected) {
      this.selected_detail_type = leaderboard_type;
    }
  },

  components: {
    LeaderboardSelector,
    LB_FoodCemetery,
    LB_TopCanteen,
    LB_TopFood,
    LB_FoodOfTheDay
  },

});
</script>
