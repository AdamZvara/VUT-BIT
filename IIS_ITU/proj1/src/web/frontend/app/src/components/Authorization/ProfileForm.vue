<template>
  <v-card class="elevation-12">
    <v-toolbar dark color="primary" justify="center">
      <v-toolbar-title>Můj profil</v-toolbar-title>
    </v-toolbar>
    <v-card-text>
      <v-form ref="form" v-model="isValidForm" class="mt-4">
        <v-row align="center">
          <v-col md="2"><h3>Login:</h3></v-col>
          <v-text-field name="login" type="text" class="black-label-color"
            v-model="user.login"></v-text-field>
        </v-row>
        <v-row align="center">
          <v-col md="2"><h3>Email:</h3></v-col>
          <v-text-field :rules="[rules.required, rules.email]" name="email"
            type="text" class="black-label-color"  v-model="user.email"></v-text-field>
        </v-row>
        <v-row align="center">
          <v-col md="2"><h3>Nové heslo:</h3></v-col>
          <v-text-field name="password" type="password" class="black-label-color" v-model="user.new_password"></v-text-field>
        </v-row>
      </v-form>
    </v-card-text>
    <v-card-actions>
      <v-spacer></v-spacer>
      <v-btn color="warning" @click="dropChanges" >Zahoď změny</v-btn>
      <v-btn color="primary" @click="update" :loading="btnLoading" :disabled="!isValidForm">Uložit</v-btn>
    </v-card-actions>
  </v-card>
</template>

<script lang="ts">
  import Vue from 'vue';
  import userInfo from "@/services/userInfo";
  import UserData from "@/types/UserData";
  import StorageService from "@/services/StorageService";
  import apiClient from '@/http-common';

  export default Vue.extend({
    name: 'RegistrationForm',

    data() {
      return {
        user: {
          login: '',
          email: '',
          new_password: '',
        } as UserData,
        rules: {
          required: (value: any) => !!value || 'Povinné pole',
          // minlen: (value: any) => value.length >= 8 || 'Heslo musí mít alespoň 8 znaků',
          email: (value: any) => {
            const pattern = /^(([^<>()[\]\\.,;:\s@"]+(\.[^<>()[\]\\.,;:\s@"]+)*)|(".+"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/
            return pattern.test(value) || 'Neplatný email'
          },
        },
        isValidForm: false,
        btnLoading: false,
      }
    },

    methods: {
      load() {
        userInfo.getAll()
          .then(r => {
            this.user.login = r.data.login;
            this.user.email = r.data.email;
            this.user.new_password = '';
          })
      },

      update() {
        this.btnLoading = true;
        userInfo.update(this.user)
          .then(r => {
            StorageService.clearToken();
            StorageService.setToken(r.data.token)
            apiClient.defaults.headers.common['Authorization'] = 'Bearer ' + r.data.token;
            this.$store.commit('editProfileClose');
            this.$store.dispatch('setSnackSuccess', 'Změny byly uloženy');
            this.user.new_password = '';
            this.$emit('clearForm');
          })
        this.btnLoading = false;
      },

      dropChanges() {
        this.load(); // load data again so next time old data are availiable
        this.$store.commit('editProfileClose');
        this.$emit('clearForm');
      }
    },

    mounted() {
      this.load();
    }
  });
</script>