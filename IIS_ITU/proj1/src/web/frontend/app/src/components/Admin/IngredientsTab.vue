<template>
  <v-card>
    <!-- Search bar -->
    <v-card-title>
      <v-text-field v-model="search" append-icon="mdi-magnify" label="Filtrovat podle názvu" single-line hide-details
      class="grey-label-color"></v-text-field>
    </v-card-title>

    <!-- Data table -->
    <v-data-table :headers="headers" :items="ingredients" class="elevation-1" dense :search="search">
      <template v-slot:top>
      <v-toolbar flat>
        <v-spacer></v-spacer>
        <v-btn color="primary" @click="saveIngredient">Uložiť</v-btn>
      </v-toolbar>
    </template>

      <!-- Checked checkbox -->
      <template v-slot:item.is_checked="{item}">
        <v-checkbox v-model="item.is_checked"></v-checkbox>
      </template>

      <!-- GlutenFree checkbox -->
      <template v-slot:item.is_gluten_free="{item}">
        <v-checkbox v-model="item.is_gluten_free"></v-checkbox>
      </template>

      <!-- Vegetarian checkbox -->
      <template v-slot:item.is_vegetarian="{item}">
        <v-checkbox v-model="item.is_vegetarian"></v-checkbox>
      </template>

      <!-- Vegan checkbox -->
      <template v-slot:item.is_vegan="{item}">
        <v-checkbox v-model="item.is_vegan"></v-checkbox>
      </template>

    </v-data-table>
  </v-card>
</template>

<script lang="ts">
  import Vue from 'vue';
  import IngredientService from '@/services/Admin/IngredientService'
  import IngredientData from '@/types/Admin/IngredientData'
  import ResponseData from '@/types/ResponseData'

  export default Vue.extend({
    name: 'IngredientsTab',

    data() {
      return {
        search: '',
        headers: [
          {
            text: 'Název',
            sortable: false,
            value: 'name',
          },
          { text: 'Bezlepkové', value: 'is_gluten_free'},
          { text: 'Vegetarian', value: 'is_vegetarian'},
          { text: 'Vegan',      value: 'is_vegan'},
          { text: 'Uložené',    value: 'is_checked'},
        ],
        ingredients: [] as IngredientData[]
      }
    },

    methods: {
      loadIngredients() {
        IngredientService.getAll()
          .then((response: ResponseData) => {
            this.ingredients = response.data;
          })
      },

      saveIngredient() {
        IngredientService.saveAll(this.ingredients.filter(ingredient => ingredient.is_checked))
          .then((response: ResponseData) => {
            this.$store.dispatch('setSnackSuccess', 'Ingredience byly uloženy');
          }).catch(r => {
            this.$store.dispatch('setSnackError', 'Někde nastala chyba');
          })
      }
    },

    mounted() {
      this.loadIngredients();
    }
  })
</script>