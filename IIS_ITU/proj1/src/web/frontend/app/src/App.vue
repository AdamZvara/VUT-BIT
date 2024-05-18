<template>
  <v-app app>

    <!-- App Menu -->
    <v-app-bar app class="primary">
      <v-container fluid>
        <v-row>
          <v-col class="d-flex justify-center">
            <v-btn-toggle mandatory group light>
              <v-btn
                  v-for="route in routes"
                  :key="route.path"
                  :to="route.path"
                  class="white--text"
              >{{ route.name }}
              </v-btn>
            </v-btn-toggle>
            <v-spacer></v-spacer>
            <v-app-bar-nav-icon @click.stop="drawer = !drawer" color="white"></v-app-bar-nav-icon>
          </v-col>
        </v-row>
      </v-container>
    </v-app-bar>

    <!-- NavBar Menu -->
    <v-navigation-drawer
        v-model="drawer" temporary app right>
      <v-list nav dense>
        <v-list-item-group v-model="group" value="0">
          <!-- Needed to add v-list-item value so that the items are not scrambled after each redrawing -->
          <!-- Logging in user -->
          <v-list-item value="1" v-if="!loggedIn">
            <v-dialog v-model="loginDialog" max-width="600px">
              <template v-slot:activator="{ on, attrs }">
                <v-btn v-bind="attrs" v-on="on" block plain>
                  Přihlásit
                </v-btn>
              </template>
              <LoginForm @loggedInUser="onLoggedInUser"></LoginForm>
            </v-dialog>
          </v-list-item>

          <!-- Sign in user -->
          <v-list-item value="2" v-if="!loggedIn">
            <v-dialog v-model="registerDialog" max-width="600px">
              <template v-slot:activator="{ on, attrs }">
                <v-btn v-bind="attrs" v-on="on" block plain>
                  Registrovat
                </v-btn>
              </template>
              <RegistrationForm @registeredUser="onRegisteredUser" @clearForm="closeNavbar"></RegistrationForm>
            </v-dialog>
          </v-list-item>

          <!-- Logout -->
          <v-list-item value="3" v-if="loggedIn">
            <v-btn v-if="loggedIn" @click="logout" block plain>
              Odhlásit
            </v-btn>
          </v-list-item>

          <!-- My profile -->
          <v-list-item value="4" v-if="loggedIn">
            <v-dialog v-model="profileDialog" max-width="600px">
              <template v-slot:activator="{ on, attrs }">
                <v-btn v-bind="attrs" v-on="on" block plain>
                  Můj profil
                </v-btn>
              </template>
              <ProfileForm @registeredUser="onRegisteredUser" @clearForm="closeNavbar"></ProfileForm>
            </v-dialog>
          </v-list-item>

          <!-- Delete account -->
          <v-list-item value="5" v-if="loggedIn">
            <v-dialog v-model="deleteDialog" max-width="600px">
              <template v-slot:activator="{ on, attrs }">
                <v-btn v-bind="attrs" v-on="on" block plain>
                  Smazat účet
                </v-btn>
              </template>
              <DeleteAcc @closeDialog="closeNavbar"></DeleteAcc>
            </v-dialog>
          </v-list-item>

          <div v-if="roleModule.admin" >
            <v-list-item
                v-for="(route, index) in admin_routes"
                :value="6 + index"
                :key="route.path"
                :to="route.path">
              <v-btn block plain color="error">
                {{ route.name }}
              </v-btn>
            </v-list-item>
          </div>

        <div v-if="roleModule.mod">
          <v-list-item
              v-for="(route, index) in mod_routes"
              :value="9 + index"
              :key="route.path"
              :to="route.path">
            <v-btn block plain color="warning">
              {{ route.name }}
            </v-btn>
          </v-list-item>
        </div>

        </v-list-item-group>
      </v-list>
    </v-navigation-drawer>

    <!-- Separate snackbar for whole application (is accessed by snackModule in vuex) -->
    <v-snackbar :color="snackModule.appSnackType" v-model="snackModule.appSnackShow" :timeout="2000" justify="center">
      {{ snackModule.appSnackText }}
    </v-snackbar>

    <v-main>
      <router-view/>
    </v-main>

  </v-app>
</template>

<script lang="ts">
import Vue from 'vue';
import {mapState} from 'vuex'
import Vuetify from 'vuetify/lib'

import StorageService from './services/StorageService';
import apiClient from "@/http-common"

import LoginForm from '@/components/Authorization/LoginForm.vue'
import RegistrationForm from '@/components/Authorization/RegistrationForm.vue'
import ProfileForm from '@/components/Authorization/ProfileForm.vue'
import DeleteAcc from '@/components/Authorization/DeleteAcc.vue'


Vue.use(Vuetify)
export default Vue.extend({
  name: 'App',

  components: {
    LoginForm,
    RegistrationForm,
    ProfileForm,
    DeleteAcc
  },

  data() {
    return {
      routes: [
        {path: "/", name: "Domů"},
        {path: "/leaderboards", name: "Žebříčky"},
        {path: "/ratings", name: "Hodnocení"},
      ],
      admin_routes: [
        {path: "/users", name: "Správa užívatelů"},
        {path: "/ingredients", name: "Ingredience"},
        {path: "/manage_ratings", name: "Správa recenzí"}
      ],
      mod_routes: [
        { path: "/overview", name: "Přehled menz" }
      ],
      drawer: false,  // display navbar menu
      group: null,    // display items withing navbar menu
      loginDialog: false,
      registerDialog: false,
      profileDialog: false,
      deleteDialog: false,
    }
  },

  // mapped property loggedIn to vuex store
  computed: mapState([
    'loggedIn', 'editProfileRequest', 'snackModule', 'roleModule'
  ]),

  methods: {
    onLoggedInUser() {
      this.loginDialog = false;
      this.closeNavbar();
      this.$store.dispatch('setSnackSuccess', 'Přihlášení bylo úspěšné');
    },

    onRegisteredUser() {
      this.registerDialog = false;
      this.closeNavbar()
      this.$store.dispatch('setSnackSuccess', 'Užívatel byl úspěšne registrován');
    },

    onEditProfile() {
      this.profileDialog = true;
    },

    closeNavbar() {
      this.drawer = false;
    },

    logout() {
      this.$store.dispatch('setSnackSuccess', 'Odhlášní bylo úspěšné');
      this.closeNavbar();
      this.$store.dispatch('logout');
      StorageService.setAuthorzed(false);
      StorageService.logout();
    },
  },

  watch: {
    group() {
      this.drawer = false;
    },

    editProfileRequest(newValue) {
      console.log(newValue);
      if (newValue == true) {
        this.profileDialog = true;
      } else {
        this.profileDialog = false;
      }
    }
  },

  mounted() {
    if (StorageService.isAuthorized() == true) {
      apiClient.defaults.headers.common['Authorization'] = 'Bearer ' + StorageService.getToken();
      var role = StorageService.getRole();
      this.$store.dispatch('login', role);
    } else {
      StorageService.clearToken();
    }
  }
});
</script>
