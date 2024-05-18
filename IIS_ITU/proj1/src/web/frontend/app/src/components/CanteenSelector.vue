<template>
  <div>
    <v-row justify="center">
      <v-col>
        <v-select
            v-model="canteen_id"
            :items="canteens"
            item-text="name"
            item-value="id"
            label="Menza"
            :clearable="isAdmin ? true : false"
            @input="onClickedCanteen"
        >
          <template #item="{item}">
            <span id="selectStyle">{{ item.name }}</span>
          </template>
        </v-select>
      </v-col>
    </v-row>
  </div>
</template>

<script lang="ts">
import {defineComponent} from 'vue';
import { mapGetters } from 'vuex'

import CanteenService from '@/services/CanteenService';
import CanteenData from '@/types/CanteenData';
import ResponseData from '@/types/ResponseData';

export default defineComponent({
  name: 'CanteenSelector',

  data() {
    return {
      canteen_id: Number,
      canteens: [] as CanteenData[],
    };
  },

  methods: {
    getCanteens() {
      CanteenService.getAll()
          .then((response: ResponseData) => {
            this.canteens = response.data;
          })
    },

    onClickedCanteen(event: Event) {
      // send event to parent (HomeView) that will update its displayed_canteen
      // and current foods
      this.$emit('clicked', event);
    }
  },

  mounted() {
    this.canteen_id = this.default_canteen;
    this.getCanteens();
    // default canteen is 1, future extension could set initial canteen based
    // on geographic location
    this.$emit('clicked', this.canteen_id);
  },

  computed: {
    ...mapGetters ([
      'isAdmin'
    ])
  },

  props: [ 'default_canteen' ]
});
</script>

<style>
.v-select__selection,
.v-select__selection--comma,
.v-select.v-text-field input,
.v-label {
  color: white !important;
}

#selectStyle {
  /* Styling of showed items in selector */
  color: black;
}
</style>
