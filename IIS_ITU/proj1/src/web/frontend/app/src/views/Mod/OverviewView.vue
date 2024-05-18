<template>
  <v-container fluid>
    <v-row justify="center" class="secondary">
      <v-col md="3">
          <DatePicker @dateChanged="dateChanged"/>
      </v-col>
    </v-row>
    <v-row justify="center" v-for="canteen in canteens" :key="canteen.id">
      <v-col md="6">
          <CanteenRating :canteen_name="canteen.name" :canteen_id="canteen.id" :dates="dates"/>
      </v-col>
    </v-row>
  </v-container>
</template>

<script lang="ts">
  import Vue from 'vue';
  import DatePicker from '@/components/DatePicker.vue'
  import CanteenRating from '@/components/Mod/CanteenRating.vue'
  import CanteenService from '@/services/CanteenService'
  import ResponseData from '@/types/ResponseData'
  import CanteenData from '@/types/CanteenData';

  export default Vue.extend({
    name: 'OverviewView',

    components: {
      DatePicker,
      CanteenRating
    },

    data() {
      return {
        dates: [],
        canteens: [] as CanteenData[],
      };
    },

    methods: {
      dateChanged(date: any) {
        this.dates = date;
      },

      loadCanteens() {
        CanteenService.getAll()
        .then((response: ResponseData) => {
          this.canteens = response.data;
        })
      }
    },

    mounted() {
      this.loadCanteens();
    }

  });
</script>
