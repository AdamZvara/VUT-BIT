<template>
  <v-container class="ml-4">
    <div v-if="loggedIn">
      <v-row class="pb-0" justify="center">
        <p color="grey"><small>Sdílej svou zkušenost abys pomohl ostatním!</small></p>
      </v-row>

      <v-row justify="center">
        <v-rating
            v-model="rating.points"
            background-color="orange lighten-3"
            color="orange"
            class="mt-0"
        ></v-rating>
      </v-row>

      <v-row justify="center" v-if="rating.points != 0">
        <v-col md="7">
          <v-textarea
              outlined
              v-model="rating.text"
              auto-grow
              label="Textové hodnocení (volitelné)"
              rows="1"
          ></v-textarea>
        </v-col>
      </v-row>

      <v-row class="mr-8">
        <v-spacer></v-spacer>
        <v-btn
            v-if="rating.points"
            fab
            x-small
            @click="clicked">
          <v-icon dark>
            mdi-send
          </v-icon>
        </v-btn>
      </v-row>
    </div>

    <v-row class="pb-0" justify="center" v-else>
      <p color="grey"><small >Pro hodnocení je nutné se přihlásit</small></p>
    </v-row>

  </v-container>
</template>

<script lang="ts">
import {defineComponent} from 'vue';
import RatingService from '@/services/RatingService';
import RatingData from '@/types/RatingData';
import ResponseData from '@/types/ResponseData';
import {mapState} from 'vuex'

export default defineComponent({
  name: "FoodRating",
  data() {
    return {
      rating: {
        points: 0,
        text: "",
        food_id: this.food_id,
      } as RatingData,
      sentSuccessful: false,
    };
  },
  computed: mapState({
    loggedIn: 'loggedIn'
  }),

  methods: {
    addRating() {
      let data = {
        points: this.rating.points,
        text: this.rating.text,
        food_id: this.food_id,
      };

      RatingService.createRating(data)
          .then((response: ResponseData) => {
            if (response.data == "OK") {
              console.log("New rating created");
              this.$emit('ratingSent');
              this.rating = {text: ''} as RatingData;
              this.$store.dispatch('setSnackSuccess', 'Hodocení bylo úspěšně přidáno');
            }
          })
          .catch(r => {
            if (r.response.status === 409) {
              console.log("One user 2 rating same day.");
              this.$store.dispatch('setSnackError', 'Pokoušíš se ohodnotit stejné jídlo dvakrát ve stejný den!');
            }
          })
    },

    clicked() {
      this.addRating();
    },
  },

  props: ['food_id'],

  // TODO: add rules to textarea character limit
  // counter
  // :rules="[v => v.length <= 100 || 'Max 100 znakov']"

});
</script>
