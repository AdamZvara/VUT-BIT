<template>
  <v-card>
    <v-card-title class="text-h5">{{title}}</v-card-title>
    <v-card-actions>
      <v-spacer></v-spacer>
      <v-btn color="blue darken-1" text @click="close">Zrušit</v-btn>
      <v-btn color="blue darken-1" text @click="confirm">OK</v-btn>
      <v-spacer></v-spacer>
    </v-card-actions>
  </v-card>
</template>

<script lang="ts">
  import { defineComponent } from 'vue';

  export default defineComponent({
    name: 'ConfirmDialog',

    data () {
      return {
      };
    },

    methods: {
      close() {
        this.$emit('close');
      },

      confirm() {
        this.$emit('confirm');
      }
    },

    props: [ 'title' ]

  });
</script>