<template>
  <v-card class="elevation-12">
    <v-toolbar dark color="primary">
      <v-toolbar-title>Registrace</v-toolbar-title>
    </v-toolbar>
    <v-card-text>
      <!-- Alert for informing user that something bad happened -->
      <v-alert type="error" :value="registrationErrorAlert">
        {{ alertText }}
      </v-alert>

      <!-- Registration form -->
      <v-form ref="form" v-model="isValidForm">
        <v-text-field prepend-icon="person" :rules="[rules.required, rules.email]" name="email"
            label="Email*" type="text" class="black-label-color"  v-model="email"></v-text-field>
        <v-text-field id="password_reg" prepend-icon="lock" name="password" label="Password*"
            :rules="[rules.required]" class="black-label-color"  v-model="password" :type="showPasswd ? 'text' : 'password'"
          @click:append="showPasswd = !showPasswd" :append-icon="showPasswd ? 'mdi-eye' : 'mdi-eye-off'"></v-text-field>
        <v-text-field name="login" label="Login" type="text" class="black-label-color" hint="Pod tímto názvem jsou zobrazované vaše recenze"
            v-model="login"></v-text-field>
      </v-form>
    </v-card-text>
    <v-card-text>
      <small>*značí povinné pole</small>
    </v-card-text>
    <v-card-actions>
      <v-spacer></v-spacer>
      <v-btn color="error" @click="clearForm">Zrušit</v-btn>
      <v-btn color="primary" @click="register" :loading="btnLoading" :disabled="!isValidForm">Registrovat</v-btn>
    </v-card-actions>
  </v-card>
</template>

<script lang="ts">
  import Vue from 'vue';
  import UsersAuth from "@/services/UsersAuth";

  export default Vue.extend({
    name: 'RegistrationForm',

    data() {
      return {
        showPasswd: false,
        email: '',
        password: '',
        login: '',
        errorMsg: '',
        btnLoading: false,
        alertText: "",
        registrationErrorAlert: false,
        isValidForm: false,
        rules: {
          required: (value: any) => !!value || 'Povinné pole',
          // minlen: (value: any) => value.length >= 8 || 'Heslo musí mít alespoň 8 znaků',
          email: (value: any) => {
            const pattern = /^(([^<>()[\]\\.,;:\s@"]+(\.[^<>()[\]\\.,;:\s@"]+)*)|(".+"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/
            return pattern.test(value) || 'Neplatný email'
          },
        }
      }
    },

    methods: {
      register() {
      this.btnLoading = true;
        UsersAuth.signup({ email: this.email, password: this.password, login: this.login })
          .then(r => {
            this.clearForm(true);
          }).catch(r => {
            this.registrationErrorAlert = true;
            if (r.code == 'ERR_BAD_REQUEST') {
              this.alertText = "Tento email je už použitý";
            } else {
              this.alertText = "Něco se pokazilo";
            }
          })
        this.btnLoading = false;
      },

      // This functions emits action to App.vue to close the navbar
      // and display message if user was registered (showRegistedAlert == true)
      // showRegistedAlert == false if button "Close" was pressed and no message is displayed
      clearForm(showRegistedAlert: any) {
        this.alertText = "";
        this.registrationErrorAlert = false;
        this.email = this.password = this.login = '';
        (this.$refs.form as Vue & { resetValidation: () => boolean }).resetValidation(); // reset rule validation on form
        if (showRegistedAlert == true) {
          this.$emit('registeredUser');
        } else {
          this.$emit('clearForm');
        }
      }
    }
  });
</script>

<style scoped>
.black-label-color >>> .v-label {
  color: black!important;
  opacity:0.7;
}
</style>