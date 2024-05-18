<template>
  <v-container fluid>
    <v-row class="flex-column flex-md-row secondary mb-2 px-4" justify="center">
      <v-col md="3">
        <v-text-field v-model="email" placeholder="Všechny účty" label="Email" dark></v-text-field>
      </v-col>
      <v-col md="3">
        <canteen-selector @clicked="onClickCanteen"/>
      </v-col>
      <v-col md="3">
        <DatePicker @dateChanged="dateChanged"/>
      </v-col>
    </v-row>
    <v-row justify="center">
      <v-col md="3" class="d-flex justify-space-between">
        <v-btn color="primary" @click="load">Hledat</v-btn>
        <v-btn color="error"   @click="remove" :disabled="deleteDisabled">Smazat</v-btn>
      </v-col>
    </v-row>

    <v-row justify="center">
      <v-col md="6">
        <RatingListItem
          v-for="rating in ratings"
          :key="rating.id"
          :Ratingid="rating.id"
          :FoodName="rating.to_food.food.name"
          :RatingDate="rating.added"
          :RatingText="rating.text"
          :RatingValue="rating.points"
          :UserEmail="rating.user.email"
          @cardClicked="cardClicked"/>
      </v-col>
    </v-row>
  </v-container>
</template>

<script lang="ts">
  import { defineComponent } from 'vue';
  import { mapState } from 'vuex'
  import DatePicker from '@/components/DatePicker.vue'
  import CanteenSelector from '@/components/CanteenSelector.vue';
  import RatingListItem from '@/components/Admin/RatingListItem.vue';

  import RatingService from '@/services/Admin/RatingService'
  import ResponseData from '@/types/ResponseData';
  import RatingListData from '@/types/RatingListData';

  export default defineComponent({
    name: 'RatingsList',

    components: {
      DatePicker,
      CanteenSelector,
      RatingListItem
    },

    data() {
      return {
        email: '',
        canteen_id: 0,
        dates: [],
        ratings: [] as RatingListData[],
        to_delete: [] as number[],
      }
    },

    computed: {
      deleteDisabled() : boolean {
        console.log(this.to_delete);
        return this.to_delete.length == 0;
      },
      ...mapState({
        snackModule: 'snackModule'
      })
    },

    methods: {
      load() {
        RatingService.getAll({email: this.email, canteen_id: this.canteen_id, dates: this.dates})
          .then((response: ResponseData) => {
            this.ratings = response.data;
          })
      },

      remove() {
        RatingService.remove({ids : this.to_delete})
          .then((response: ResponseData) => {
            for (var i = this.ratings.length -1; i >= 0; i--) {
              if (this.to_delete.includes(this.ratings[i].id)) {
                this.ratings.splice(i, 1);
              }
              this.$store.dispatch('setSnackSuccess', 'Hodnocení byly úspěšne smazána')
            }
            this.to_delete = [];
          })
      },

      dateChanged(date: any) {
        this.dates = date;
      },

      onClickCanteen(value: number) {
        this.canteen_id = value;
      },

      cardClicked(selected: boolean, id: number) {
        if (selected) {
          this.to_delete.push(id)
        } else {
          var index = this.to_delete.indexOf(id);
          this.to_delete.splice(index, 1);
        }
      }
    },

    mounted() {
      this.canteen_id = -1;
      this.load();
    }
  });
</script>