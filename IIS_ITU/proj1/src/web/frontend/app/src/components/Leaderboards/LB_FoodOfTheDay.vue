<template>
  <v-container>
    <v-simple-table>
      <template v-slot:default>
        <thead>
          <tr>
            <th class="text-left">
              Umístění
            </th>
            <th class="text-left">
              Jídlo
            </th>
            <th class="text-left">
              Průměrné hodnocení
            </th>
            <th class="text-left">
              Menza
            </th>
            <th class="text-left">
              počet hodnocení
            </th>

          </tr>
        </thead>
        <tbody>
          <tr v-for="(item, index) in topfoods" :key="item.name">
            <td>{{ index + 1 }}</td>
            <td>{{ item.food_name }}</td>
            <td>{{ item.avg_rating }}</td>
            <td>{{ item.cateen_name }}</td>
            <td>{{ item.count }}</td>

          </tr>
        </tbody>
      </template>
    </v-simple-table>
  </v-container>
</template>

<script lang="ts">
import Vue from 'vue';
import { defineComponent } from "vue";
import LBservice from '@/services/LBservice';
import leaderboardsTypes from '@/types/leaderboardsTypes'

export default defineComponent({
  name: "LB_FoodBestEver",

  data() {
    return {
      topfoods: [] as leaderboardsTypes[],
    };
  },

  methods: {
    loaddailyTop() {
      LBservice.getTopRatingsToday(5).then(r => {
        this.topfoods = r.data;
      }).catch(r => {
        console.log("Nepodařilo se nic stáhnout Z API.")
      })
    }
  },
  mounted() {
    this.loaddailyTop();
  },
});
</script>

<style scoped>

</style>
