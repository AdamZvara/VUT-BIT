import axios, { AxiosInstance } from "axios";
import StorageService from "./services/StorageService";
import store from '@/store'

const apiClient: AxiosInstance = axios.create({
  baseURL:  process.env.NODE_ENV === 'production' ? 'https://api.hlad.jevlk.cz/api': process.env.VUE_APP_API_URL,
  headers: {
    "Content-type": "application/json",
  },
});

apiClient.interceptors.response.use(function (response) {
  // Any status code that lie within the range of 2xx cause this function to trigger
  // Do something with response data
  return response;
}, function (error) {
  // Any status codes that falls outside the range of 2xx cause this function to trigger
  // Do something with response error
  if (error.response.status === 498) {
    StorageService.setAuthorzed(false);
    store.dispatch('logout'); // automatic logout of user
    store.dispatch('setSnackWarning', 'Platnost sezení expirovala! Je nutné se znova přihlásit');
  } else if (error.response.status === 403) {
    store.dispatch('setSnackError', 'Nemáte potřebné práva');
  }
  return Promise.reject(error);
});

export default apiClient;