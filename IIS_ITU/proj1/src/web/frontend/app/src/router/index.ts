import Vue from 'vue'
import VueRouter, { RouteConfig } from 'vue-router'
import HomeView from '../views/HomeView.vue'
import store from '@/store/index'

Vue.use(VueRouter)

const routes: Array<RouteConfig> = [
  {
    path: '/',
    name: 'home',
    component: HomeView
  },
  {
    path: '/leaderboards',
    name: 'leaderboards',
    component: () => import('../views/LeaderboardsView.vue')
  },
  {
    path: '/ratings',
    name: 'ratings',
    component: () => import('../views/RatingsView.vue')
  },
  {
    path: '/menu',
    name: 'menu',
    component: () => import('../views/MenuView.vue')
  },
  {
    path: '/food_detail/overview/:id',
    name: 'food_overview',
    component: () => import('../components/FoodDetail/FoodDetail.vue')
  },
  {
    path: '/food_detail/allergens/:id',
    name: 'food_allergens',
    component: () => import('../components/FoodDetail/FoodDetail.vue')
  },
  {
    path: '/food_detail/ratings/:id',
    name: 'food_ratings',
    component: () => import('../components/FoodDetail/FoodDetail.vue')
  },
  {
    path: '/users',
    name: 'users',
    component: () => import('../views/Admin/UserListView.vue'),
    meta: {
      requiresAuth: true
    }
  },
  {
    path: '/ingredients',
    name: 'ingredients',
    component: () => import('../views/Admin/IngredientView.vue'),
    meta: {
      requiresAuth: true
    }
  },
  {
    path: '/manage_ratings',
    name: 'manage_ratings',
    component: () => import('../views/Admin/RatingsView.vue'),
    meta: {
      requiresAuth: true
    }
  },
  {
    path: '/overview',
    name: 'overview',
    component: () => import('../views/Mod/OverviewView.vue'),
  },
  {
    path: '/:catchAll(.*)*',
    name: "PageNotFound",
    component: () => import('../views/PageNotFound.vue')
  }
]

const router = new VueRouter({
  mode: 'history',
  base: process.env.BASE_URL,
  routes
})

// for each admin route, divert non-admin user to homepage
router.beforeEach((to, from, next) => {
  if (to.matched.some(record => record.meta.requiresAuth)) {
    if (!store.getters.isAdmin) {
      next({ name: 'home' })
    } else {
      next()
    }
  } else {
    next()
  }
})

export default router
