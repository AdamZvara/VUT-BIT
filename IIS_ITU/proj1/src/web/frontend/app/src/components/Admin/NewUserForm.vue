<template>
  <v-card>

    <!-- Search bar -->
    <v-card-title>
      <v-text-field v-model="search" append-icon="mdi-magnify" label="Filtrovat podle emailu" single-line hide-details
      class="grey-label-color"></v-text-field>
    </v-card-title>

    <!-- Data table -->
    <v-data-table :headers="headers" :items="users" class="elevation-1" dense :search="search">
    <template v-slot:top>

      <!-- Toolbar -->
      <v-toolbar flat>
        <v-spacer></v-spacer>

        <!-- Dialog for new/editing user -->
        <v-dialog v-model="dialog" max-width="500px">

          <!-- New user button -->
          <template v-slot:activator="{ on, attrs }">
            <v-btn color="primary" dark class="mb-2" v-bind="attrs" v-on="on">
              Nový uživatel
            </v-btn>
          </template>

          <v-card>
            <v-card-title>
              <span class="text-h5">{{ formTitle }}</span>
            </v-card-title>

            <!-- User form -->
            <v-card-text>
              <v-container>
                <v-form ref="form" v-model="isValidForm">
                  <v-row>
                    <v-col cols="12" sm="6" md="4">
                      <v-text-field v-model="editedUser.new_email" label="Email" class="grey-label-color" :rules="[rules.required, rules.email]"></v-text-field>
                    </v-col>
                    <v-col cols="12" sm="6" md="4">
                      <v-text-field v-model="editedUser.login" label="Login" class="grey-label-color"></v-text-field>
                    </v-col>
                    <v-col cols="12" sm="6" md="4">
                      <v-text-field v-model="editedUser.password" label="Heslo" class="grey-label-color" type="password" :rules="editedIndex === -1 ? [rules.required] : []"></v-text-field>
                    </v-col>
                    <v-col cols="12" sm="6" md="4">
                      <v-select :items="roles" v-model="editedUser.role" label="Právomoci" class="grey-label-color black-select-text"></v-select>
                    </v-col>
                  </v-row>
                </v-form>
              </v-container>
            </v-card-text>

            <!-- Close/save buttons -->
            <v-card-actions>
              <v-spacer></v-spacer>
              <v-btn color="blue darken-1" text @click="close">
                Zrušit
              </v-btn>
              <v-btn color="blue darken-1" :disabled="!isValidForm" text @click="save">
                Uložit
              </v-btn>
            </v-card-actions>
          </v-card>

        <!-- Delete dialog -->
        </v-dialog>
        <v-dialog v-model="dialogBlock" max-width="500px">
          <ConfirmDialog :title="blockTitle" @confirm="blockConfirm" @close="closeBlock"></ConfirmDialog>
        </v-dialog>
      </v-toolbar>
    </template>

    <!-- Color chips for user roles -->
    <template v-slot:item.role="{ item }">
      <v-chip :color="roleColor(item.role)" dark>
        {{ roleText(item.role) }}
      </v-chip>
    </template>

    <!-- Options for editing/deleting user -->
    <template v-slot:item.actions="{ item }">
      <v-icon small class="mr-2" @click="editItem(item)">mdi-pencil</v-icon>
      <v-icon small @click="blockUser(item)" v-if="!item.blocked">mdi-cancel</v-icon>
      <v-icon small @click="unblockUser(item)" v-if="item.blocked">mdi-check</v-icon>
    </template>
  </v-data-table>
  </v-card>
</template>

<script lang="ts">
  import Vue from 'vue';
  import UserRoleEnum from '@/enums';
  import { getRoleText, getRoleColor } from '@/enums'
  import UserInfo from '@/services/Admin/UserInfo'
  import ResponseData from '@/types/ResponseData'
  import UserData from '@/types/Admin/UserData'
  import ConfirmDialog from '@/components/ConfirmDialog.vue'

  export default Vue.extend({
    name: 'NewUserForm',

    components: {
      ConfirmDialog
    },

    data() {
      return {
        users: [] as UserData[],
        search: '',
        isValidForm: false,
        dialog: false,
        dialogBlock: false,
        editedIndex: -1, // index of selected item, e.g new user will have editedIndex == -1, existing user != -1 which can be distinguished in save action
        editedUser: { // storage of inputs from admin
          // 2 emails stored in case admin changes email of user, the old email is used to find user in database
          email: '',
          new_email: '',
          login: '',
          role: 2,
          password: '',
          blocked: false
        },
        defaultUser: { // serves as a template (default values) which are moved to editedUser on demand
          email: '',
          new_email: '',
          login: '',
          role: 2,
          password: '',
          blocked: false
        },
        roles: [
          { text: 'administrátor', value: UserRoleEnum.ADMIN },
          { text: 'uživatel', value: UserRoleEnum.USER },
          { text: 'moderátor', value: UserRoleEnum.MOD }
        ],
        rules: {
          required: (value: any) => !!value || 'Povinné pole',
          // minlen: (value: any) => value.length >= 8 || 'Heslo musí mít alespoň 8 znaků',
          email: (value: any) => {
            const pattern = /^(([^<>()[\]\\.,;:\s@"]+(\.[^<>()[\]\\.,;:\s@"]+)*)|(".+"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/
            return pattern.test(value) || 'Neplatný email'
          },
        },
        headers: [ // headers of table
          {
            text: 'Email',
            sortable: false,
            value: 'email',
          },
          { text: 'Právomoci', value: 'role'},
          { text: 'Login',     value: 'login'},
          { text: 'Blokován',  value: 'blocked'},
          { text: 'Akce', value: 'actions', sortable: false },
        ]
      }
    },

    computed: {
      formTitle() { // title of dialog
        if (this.editedIndex === -1) {
          return 'Nový uživatel';
        } else {
          return 'Upravit uživatele'
        }
      },

      blockTitle() {
        if (this.editedUser.blocked === true) {
          return "Vážne chcete zablokovat uživatele?";
        } else {
          return "Vážne chcete odblokovat uživatele?";
        }
      }
    },

    methods: {
      save() {
        if (this.editedIndex > -1) {
          UserInfo.updateUser(this.editedUser)
          // update existing user
            .then(r => {
              this.$store.dispatch('setSnackSuccess', 'Uživatel byl editován');
            }).catch(r => {
              this.$store.dispatch('setSnackError', 'Nastala nejaka chyba!');
            })
        } else {
          // create new user
          UserInfo.createUser(this.editedUser)
            .then(r => {
              this.$store.dispatch('setSnackSuccess', 'Uživatel byl přidán')
            }).catch(r => {
              this.$store.dispatch('setSnackWarning', 'Užívatel už existuje!')
            })
        }
        setTimeout(() => this.getUsers(), 500); // update list of users
        this.close();
      },

      close() { // closing dialog
        (this.$refs.form as Vue & { resetValidation: () => boolean }).resetValidation(); // reset rule validation on form
        this.dialog = false;
        this.$nextTick(() => {
          this.editedUser = Object.assign({}, this.defaultUser);
          this.editedIndex = -1;
        })
      },

      roleColor(role: number) { // mapping user role to color
        return getRoleColor(role);
      },

      roleText(role: number){ // mapping user role to text
        return getRoleText(role);
      },

      getUsers() {
        UserInfo.getAll()
        .then((response: ResponseData) => {
          this.users = response.data;
        })
      },

      editItem(item: any) {
        this.editedIndex = this.users.indexOf(item);
        this.editedUser = Object.assign({}, item);
        this.editedUser.email = item.email;
        this.editedUser.new_email = item.email;
        this.editedUser.password = '';
        this.dialog = true;
      },

      closeBlock () {
        this.dialogBlock = false
        this.$nextTick(() => {
          this.editedUser = Object.assign({}, this.defaultUser);
          this.editedIndex = -1;
        })
      },

      unblockUser(item: any) {
        this.editedIndex = this.users.indexOf(item)
        this.editedUser = Object.assign({}, item)
        this.editedUser.blocked = false;
        this.dialogBlock = true
      },

      blockUser(item: any) {
        this.editedIndex = this.users.indexOf(item)
        this.editedUser = Object.assign({}, item)
        this.editedUser.blocked = true;
        this.dialogBlock = true
      },

      blockConfirm () {
        UserInfo.blockUser(this.editedUser)
          .then(r => {
            this.$store.dispatch('setSnackSuccess', 'Akce byla úspěšne provedena');
          })
        setTimeout(() => this.getUsers(), 500); // update list of users
        this.closeBlock();
      },

    },

    mounted() {
      this.getUsers();
    }
  });
</script>

<style scoped>
.grey-label-color >>> .v-label {
  color: grey!important;
  opacity:0.6;
}
.black-label-color >>> .v-label {
  color: black!important;
}
.black-select-text >>> .v-select__selection {
  color: black!important;
}
</style>