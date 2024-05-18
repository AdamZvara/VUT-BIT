<template>
  <v-container v-if="roleModule.admin" fluid class="pa-0">
      <RatingsList />
  </v-container>
</template>

<script lang="ts">
  import Vue from 'vue';
  import { mapState } from 'vuex'
  import RatingsList from '@/components/Admin/RatingsList.vue'

  export default Vue.extend({
    name: 'RatingsView',

    data() {
        return {
        };
      },

    components: {
      RatingsList
    },

    computed: {
      ...mapState([
        'roleModule'
      ]),
    },
  });
</script>