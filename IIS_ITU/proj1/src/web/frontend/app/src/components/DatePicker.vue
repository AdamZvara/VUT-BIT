<template>
  <v-menu
    ref="menu" v-model="menu" :close-on-content-click="false" :return-value.sync="dates"
    transition="scale-transition" offset-y min-width="auto">
    <template v-slot:activator="{ on, attrs }">
      <v-text-field v-model="dateRangeText" label="Datum" prepend-icon="mdi-calendar" readonly v-bind="attrs" v-on="on" dark></v-text-field>
    </template>
    <v-date-picker v-model="dates" no-title scrollable range>
      <v-spacer></v-spacer>
      <v-btn text color="primary" @click="menu = false">Zrušit</v-btn>
      <v-btn text color="primary" @click="$refs.menu.save(dates); dateChanged()">OK</v-btn>
    </v-date-picker>
  </v-menu>
</template>

<script lang="ts">
  import { defineComponent } from 'vue';

  var current_date = (new Date(Date.now() - (new Date()).getTimezoneOffset() * 60000)).toISOString().substr(0, 10);

  export default defineComponent({

    data () {
      return {
        menu: false,
        dates: [current_date, current_date]
      };
    },

    methods: {
      dateChanged() {
        if (this.dates.length == 1) {
          this.dates.push(this.dates[0]);
        }
        this.$emit('dateChanged', this.dates);
      }
    },

    computed: {
      dateRangeText() : string {
        if (this.dates[0] === this.dates[1]) {
          return this.dates[0];
        } else {
          return this.dates.join(' - ')
        }
      },
    },

    watch: {
      dates() {
        if (this.dates[0] > this.dates[1]) {
          this.dates.reverse();
        }
      }
    },

    mounted() {
      this.$emit('dateChanged', this.dates);
    }
});
</script>
