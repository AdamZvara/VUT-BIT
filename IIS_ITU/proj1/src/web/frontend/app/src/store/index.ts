import Vue from 'vue'
import Vuex from 'vuex'
import {Commit, Dispatch} from 'vuex'
import UserRoleEnum from '@/enums'

Vue.use(Vuex)

const snackModule = {
  state: {
    appSnackText: '',
    appSnackType: '',
    appSnackShow: false
  },
  mutations: {
    snackSetText(state: any, text: string) {
      state.appSnackText = text;
    },

    snackSetType(state: any, type: string) {
      state.appSnackType = type;
    },

    snackSetShow(state: any, show: boolean) {
      state.appSnackShow = show;
    }
  },
  actions: {
    setSnackSuccess({commit} : {commit : Commit}, text : string) {
      commit('snackSetText', text);
      commit('snackSetType', "success");
      commit('snackSetShow', true);
    },

    setSnackError({commit} : {commit : Commit}, text : string) {
      commit('snackSetText', text);
      commit('snackSetType', "error");
      commit('snackSetShow', true);
    },

    setSnackWarning({commit} : {commit : Commit}, text : string) {
      commit('snackSetText', text);
      commit('snackSetType', "warning");
      commit('snackSetShow', true);
    }
  },
  getters: {}
}

const roleModule = {
  state: {
    admin: false,
    mod: false
  },
  mutations: {
    setAdmin(state: any, value: boolean) {
      state.admin = value;
    },

    setMod(state: any, value: boolean) {
      state.mod = value;
    }
  },
  actions: {
    role_set({commit}: { commit: Commit }, role: UserRoleEnum) {
      if (role === UserRoleEnum.ADMIN) {
        commit('setAdmin', true);
      } else if (role === UserRoleEnum.MOD) {
        commit('setMod', true);
      }
    },

    role_unset({commit}: { commit: Commit }) {
      commit('setAdmin', false);
      commit('setMod', false);
    }
  },
  getters: {
    isAdmin(state: any) {
      return state.admin == true;
    },

    isMod(state: any) {
      return state.mod == true;
    }
  }
}

interface FoodDetail {
  food_id: number,
  current_food_id: number,
  food_name: string
}

const foodDetailModule = {
  state: {
    food_id: 0,
    current_food_id: 0,
    food_name: "",
    detail_opened: false
  },
  mutations: {
    changeFoodId(state: any, id: number) {
      state.food_id = id;
    },
    changeCurrentFoodId(state: any, id: number) {
      state.current_food_id = id;
    },
    changeCurrentFoodName(state: any, food_name: string) {
      state.food_name = food_name;
    },
    changeDetailOpened(state: any, value: boolean) {
      state.detail_opened = value;
    },
  },
  actions: {
    selectFood({commit}: { commit: Commit }, food_ids: FoodDetail) {
      commit('changeFoodId', food_ids.food_id);
      commit('changeCurrentFoodId', food_ids.current_food_id);
      commit('changeCurrentFoodName', food_ids.food_name);
      commit('changeDetailOpened', true);
    },
    closeDetail({commit}: { commit: Commit }) {
      commit('changeFoodId', 0);
      commit('changeCurrentFoodId', 0);
      commit('changeCurrentFoodName', "");
      commit('changeDetailOpened', false);
    }
  },
  getters: {
    getFoodId(state: any) {
      return state.food_id;
    },
    getCurrentFoodId(state: any) {
      return state.current_food_id;
    },
    getFoodName(state: any) {
      return state.food_name;
    },
    getDetailOpened(state: any) {
      return state.detail_opened;
    },
  }
}

export default new Vuex.Store({
  state: {
    loggedIn: false,
    editProfileRequest: false, // request from RatingsView to App to show profile tab
  },
  getters: {},
  mutations: {
    logInUser(state: any) {
      state.loggedIn = true;
    },

    logOutUser(state: any) {
      state.loggedIn = false;
    },

    editProfileOpen(state) {
      state.editProfileRequest = true;
    },

    editProfileClose(state) {
      state.editProfileRequest = false;
    }
  },
  actions: {
    login({dispatch, commit}: { dispatch: Dispatch, commit: Commit }, role: UserRoleEnum) {
      commit('logInUser')
      dispatch('role_set', role);
    },

    logout({dispatch, commit}: { dispatch: Dispatch, commit: Commit }) {
      commit('logOutUser');
      dispatch('role_unset');
    },
  },
  modules: {
    snackModule,
    roleModule,
    foodDetailModule
  }
})
