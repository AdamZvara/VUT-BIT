<style scoped>
.black-label-color>>>.v-label {
  color: black !important;
  opacity: 0.7;
}
</style>

<template>
  <v-card class="elevation-12">
    <v-toolbar dark color="primary">
      <v-toolbar-title>Přihlásení:</v-toolbar-title>
    </v-toolbar>
    <v-card-text>
      <v-alert type="error" :value="alertVis">
        {{ alertText }}
      </v-alert>
      <v-form v-model="isValidForm">
        <v-text-field prepend-icon="person" :rules="[rules.required, rules.email]" name="email" label="Email"
          type="text" v-model="email" class="black-label-color" color="red"></v-text-field>
        <v-text-field id="password" prepend-icon="lock" name="password" label="Heslo" :rules="[rules.required]"
          v-model="password" class="black-label-color" color="black" :type="showPasswd ? 'text' : 'password'"
          @click:append="showPasswd = !showPasswd" :append-icon="showPasswd ? 'mdi-eye' : 'mdi-eye-off'"></v-text-field>
      </v-form>
    </v-card-text>

    <v-card-actions>
      <v-spacer></v-spacer>
      <v-btn color="primary" @click="sendLogin" :loading="btnLoading" :disabled="!isValidForm">Přihlásit</v-btn>
    </v-card-actions>
  </v-card>
</template>

<script lang="ts">
import Vue from 'vue';
import UsersAuth from "@/services/UsersAuth";
import apiClient from "@/http-common"
import StorageService from '@/services/StorageService';

export default Vue.extend({
  name: 'LoginForm',

  props: {
    source: String,
  },

  data() {
    return {
      showPasswd: false,
      email: '',
      password: '',
      btnLoading: false,
      alertVis: false,
      alertText: '',
      isValidForm: false,
      rules: {
        required: (value: any) => !!value || 'Povinné pole',
        email: (value: any) => {
          const pattern = /^(([^<>()[\]\\.,;:\s@"]+(\.[^<>()[\]\\.,;:\s@"]+)*)|(".+"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/
          return pattern.test(value) || 'Neplatný e-mail'
        },
      }
    }
  },

  methods: {
    sendLogin() {
      this.btnLoading = true;
      UsersAuth.login({ email: this.email, password: this.password })
        .then(r => {
          apiClient.defaults.headers.common['Authorization'] = 'Bearer ' + r.data.token;
          StorageService.saveUser(r.data.token, true, r.data.role);
          this.alertVis = false;
          this.$store.dispatch('login', r.data.role);
          this.$emit("loggedInUser");
        }).catch(r => {
          console.log(r);
          if (r.response.status == 403) {
            this.alertText = 'Tento účet je zablokovaný'
          } else {
            this.alertText = 'Špatné jmeno nebo heslo!'
          }
          this.alertVis = true;
          this.btnLoading = false;
        })
    }
  }
});
</script>