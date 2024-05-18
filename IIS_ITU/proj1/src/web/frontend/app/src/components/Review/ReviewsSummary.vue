<template>
  <div>
    <v-alert type="error" :value="!loggedIn">
      Nejsi příhlášen!
    </v-alert>

    <v-row class="mt-3 ml-1">
      <h3 v-if="user.login !== ''" class="white--text">{{ user.login }}</h3>
      <h3 v-else class="white--text">{{ user.email }}</h3>

    </v-row>
    <v-row class="mt-3 ml-1 pa-sd-10">

      <v-btn rounded color="primary" dark @click="editProfile">
        <v-icon left>
          mdi-pencil
        </v-icon>
        Upravit profil
      </v-btn>
    </v-row>
    <v-row justify="center">
      <v-col sm="4">
        <div class="pa-2 text-center white--text " justify="center">
          <div class="blue--text text-xl-h4"> {{ revStats.ratings }} </div>
          Recenzí
        </div>
      </v-col>
      <v-col sm="4">
        <div class="pa-2 text-center white--text " justify="center">
          <div class="blue--text text-xl-h4"> {{ revStats.textRatings }} </div>
          Hodnocení
        </div>
      </v-col>
      <v-col sm="4">
        <div class="pa-2 text-center white--text " justify="center">
          <div class="blue--text text-xl-h4"> {{ revStats.achievements }} </div>
          Achievementy
        </div>
      </v-col>
    </v-row>
  </div>
</template>

<script>
import { mapState } from "vuex"
import userInfo from "@/services/userInfo";
import RatingService from "@/services/RatingService";

export default {
  name: 'ReviewsSummary',

  icons: {
    iconfont: 'ma',
  },

  data() {
    return {
      user: {
        login: "Nepřihlašený užívatel",
        email: "",
      },
      revStats: {
        ratings: 0,
        textRatings: 0,
        achievements: 0
      },
    }
  },

  computed: mapState({
    loggedIn: 'loggedIn'
  }),

  watch: {
    loggedIn(newValue) {
      if (newValue == true) {
        this.getUser()
        this.getRatings()
      } else {
        this.revStats.ratings = this.revStats.textRatings = this.revStats.achievements = 0;
        this.user.login = "Nepřihlašený užívatel";
        this.user.email = "";
      }
    }
  },

  methods: {
    getUser() {
      userInfo.getAll()
        .then(r => {
          this.user.login = r.data.login
          this.user.email = r.data.email
        })
    },

    getRatings() {
      RatingService.getRatingStats()
        .then(r => {
          this.revStats.ratings = r.data.ratings
          this.revStats.textRatings = r.data.textRatings
          this.revStats.achievements = r.data.achievements
        })
    },

    editProfile() {
      this.$store.commit('editProfileOpen', true);
    }
  },

  beforeMount() {
    console.log(this.loggedIn);
    if (this.loggedIn == true) {
      this.getUser()
      this.getRatings()
    }
  }
};
</script>