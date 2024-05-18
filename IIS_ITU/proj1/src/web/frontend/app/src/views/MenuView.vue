<template>
  <v-container>
    <LoginForm />
    <RegistrationForm/>
  </v-container>
</template>

<script lang="ts">
import Vue from 'vue';
import Vuetify from 'vuetify/lib'
import 'vuetify/dist/vuetify.min.css';

import RegistrationForm from '@/components/Authorization/RegistrationForm.vue'
import LoginForm from '@/components/Authorization/LoginForm.vue'

Vue.use(Vuetify)
export default Vue.extend({
  name: 'MenuView',
  data: () => ({
    //
  }),
  components: {
    LoginForm,
    RegistrationForm
  }
});
</script>