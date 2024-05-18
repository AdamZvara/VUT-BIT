<template>
  <div>
    <v-card class="mb-4" outlined>
      <v-container>
        <v-row>
          <v-col>
            <v-list-item two-line>
              <v-list-item-content>
                <v-list-item-title class="text-h5 text-wrap">
                  {{ FoodName }}
                </v-list-item-title>
                <v-list-item-subtitle>{{ CanteenName }} - {{ RatingDate }}</v-list-item-subtitle>
              </v-list-item-content>
            </v-list-item>
          </v-col>
          <v-col>
            <v-rating background-color="yellow lighten-3" readonly size="30" color="yellow" class="mt-0 mb-0"
                      :value="parseInt(value)"></v-rating>
          </v-col>
        </v-row>
        <v-row>
          <v-col class="pl-4">
            {{ newRatingText }}
          </v-col>
        </v-row>
        <v-card-actions justify="right">
          <v-dialog v-model="dialog" persistent max-width="600px">
            <template v-slot:activator="{ on, attrs }">
              <v-btn color="primary" dark v-bind="attrs" v-on="on">
                <v-icon left>
                  mdi-pencil
                </v-icon>
                Upravit
              </v-btn>
            </template>
            <v-card>
              <v-card-title>
                <span class="text-h5">Uprava recenze: <br>{{ FoodName }}</span>
              </v-card-title>
              <v-card-text>
                <v-container>
                  <v-row justify="center">
                    <v-rating v-model="value" background-color="yellow" color="yellow" class="mt-0"></v-rating>
                  </v-row>
                  <v-row justify="center">
                    <v-col>
                      <v-textarea outlined v-model="newRatingText" auto-grow label="Textové hodnocení (volitelné)"
                                  rows="3">
                      </v-textarea>
                    </v-col>
                  </v-row>
                </v-container>
              </v-card-text>
              <v-card-actions>
                <v-spacer></v-spacer>
                <v-btn color="blue darken-1" text @click="dialog = false">
                  Zavřít
                </v-btn>
                <v-btn color="blue darken-1" text @click="dialog = false; edit_user()">
                  Uložit
                </v-btn>
              </v-card-actions>
            </v-card>
          </v-dialog>
          <v-btn @click="rm_user" color="error">
            <v-icon left>
              mdi-delete
            </v-icon>
            Smazat
          </v-btn>
        </v-card-actions>
      </v-container>
    </v-card>
  </div>
</template>

<script>
import userInfo from "@/services/userInfo";
import RatingService from "@/services/RatingService";
import ResponseData from "@/types/ResponseData";
import apiClient from "@/http-common.ts";
import ConfirmationDialog from '@/components/ConfirmDialog';
import store from "@/store";


export default {
  name: 'ReviewDetail',

  icons: {
    iconfont: 'ma',
  },
  data() {
    return {
      dialog: false,
      newRatingText: this.RatingText,
      value: this.RatingValue,
    }
  },
  methods: {
    rm_user() {
      var r = this.Ratingid;
      RatingService.rmRating({r}).then(r => {
            this.$store.dispatch('setSnackSuccess', 'Recenze byla uspěšně smazána');
            this.$emit('deletedReview')
          }
      ).catch(r => {
            this.$store.dispatch('setSnackError', 'Chyba: recenzi se nepodařilo smazat.');
      })
    },
    edit_user() {
      console.log(this.newRatingText)
      RatingService.editRating({id: this.Ratingid, text: this.newRatingText, val: this.value}).then(r => {

            this.$store.dispatch('setSnackSuccess', 'Recenze byla uspěšně upravena');
          }
      )
          .catch(r => {
            this.$store.dispatch('setSnackError', 'Chyba: recenze nemohla být uložena');
          })
      this.alertVis = true
    }


  },
  components: {},
  props: {
    Ratingid: Number,
    CanteenName: String,
    FoodName: String,
    RatingText: String,
    RatingValue: Number,
    RatingDate: String
  },

};
</script>
