<template>
  <v-container v-if="roleModule.admin">
      <NewUserForm />
  </v-container>
</template>

<script lang="ts">
import Vue from 'vue';
import { mapState } from 'vuex'

import NewUserForm from '@/components/Admin/NewUserForm.vue'

export default Vue.extend({
  name: 'UserListView',

  data() {
      return {
      };
    },

  components: {
    NewUserForm
  },

  computed: {
    ...mapState([
      'roleModule'
    ]),
  },
});
</script>