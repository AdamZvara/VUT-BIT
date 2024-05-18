<template>
  <v-row justify="center" class="secondary" >
    <div class="pa-5 mb-2 secondary" align="center">
      <a
      > {{ type }} </a>

        <v-btn-toggle
          v-model="toggle_exclusive"
          mandatory
        >
          <v-btn
            class="mb-0"
            depressed
            outlined
            color="secondary"
             v-for="(type, index) in leaderboard_types"
             :key="index"
            @click="updateSelected(index)">
              {{ type }}
          </v-btn>
        </v-btn-toggle>

    </div>
  </v-row>
</template>

<script lang="ts">
import {defineComponent} from 'vue';

export default defineComponent({
  name: "LeaderboardSelector",
  data() {
    return {
      leaderboard_types: ["Jídlo dne", "Jídlo věků", 
    // "Nejlepší menza", "Hřbitov jídel" 
    ]
    }
  },
  methods: {
    updateSelected(index: number) {
      this.$emit('leaderboardChanged', index);
    }
  },

});
</script>

<style>

</style>
