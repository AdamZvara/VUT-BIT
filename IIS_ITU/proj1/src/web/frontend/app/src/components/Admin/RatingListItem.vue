<template>
  <v-sheet outlined color="gray" rounded class="mb-2">
    <v-card class="pb-4" outlined @click="customAction" :color="selected ? 'grey' : 'white'">
      <v-container>
        <v-row>
          <v-col>
            <v-list-item two-line>
              <v-list-item-content>
                <v-list-item-title class="text-h6 text-wrap">
                  {{ UserEmail }}
                </v-list-item-title>
                <v-list-item-subtitle>{{ FoodName }} - {{ RatingDate }} </v-list-item-subtitle>
              </v-list-item-content>
            </v-list-item>
          </v-col>
          <v-col>
            <v-rating background-color="yellow lighten-3" readonly size="30" color="yellow" class="mt-0 mb-0"
              :value="parseInt(RatingValue)"></v-rating>
          </v-col>
        </v-row>
        <v-row>
          <v-col class="pl-4">
            {{ RatingText }}
          </v-col>
        </v-row>
      </v-container>
    </v-card>
  </v-sheet>
</template>

<script lang="ts">
  import { defineComponent } from 'vue';

  export default defineComponent({
    name: 'RatingListItem',

    data() {
      return {
        selected: false
      }
    },

    methods: {
      customAction() {
        this.selected = !this.selected;
        this.$emit('cardClicked', this.selected, this.Ratingid)
      },
    },

    props: {
      Ratingid: Number,
      FoodName: String,
      RatingText: String,
      RatingValue: Number,
      RatingDate: String,
      UserEmail: String,
      deleted: Boolean
    },
  });
</script>

<style>
.grey-label-color >>> .v-label {
  color: grey!important;
  opacity:0.7;
}
</style>
