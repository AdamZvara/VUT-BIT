<template>
  <ConfirmDialog title="Vážne chcete smazat váš účet?" @close="closeDialog" @confirm="deleteAcc"></ConfirmDialog>
</template>

<script lang="ts">
  import Vue from 'vue';
  import UserData from '@/types/UserData'
  import userInfo from '@/services/userInfo'
  import ConfirmDialog from '@/components/ConfirmDialog.vue'

  export default Vue.extend({
    name: 'DeleteAcc',

    components: {
      ConfirmDialog
    },

    data() {
      return {
        user: {
          email: '',
        } as UserData
      }
    },

    methods: {
      closeDialog() {
        this.$emit('closeDialog');
      },

      deleteAcc() {
        userInfo.delete()
          .then(r => {
            this.$store.dispatch('setSnackSuccess', 'Účet byl odstránen');
            this.$store.dispatch('logout');
            this.$emit('closeDialog');
          })
      }
    },

  });
</script>