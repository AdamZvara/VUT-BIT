<template>
  <v-container v-if="roleModule.admin">
      <IngredientsTab />
  </v-container>
</template>

<script lang="ts">
import Vue from 'vue';
import { mapState } from 'vuex'

import IngredientsTab from '@/components/Admin/IngredientsTab.vue'


export default Vue.extend({
  name: 'IngredientView',

  data() {
      return {
      };
    },

  components: {
    IngredientsTab
  },

  computed: {
    ...mapState([
      'roleModule'
    ]),
  },
});
</script>