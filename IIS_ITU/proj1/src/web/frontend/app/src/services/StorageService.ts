// Class for storing data on client side

// Storing data in localStorage, data is expected to be in JSON format and is stringified
function setData(name: string, data : any) {
  localStorage.setItem(name, JSON.stringify(data));
}

// Getting data from localStorage, data is expected to be in string format and is parsed as JSON
function getData(name: string) {
  return JSON.parse(localStorage.getItem(name)!);
}

// Clear data from localStorage
function clearData(name: string) {
  localStorage.removeItem(name);
}

class StorageService {

  saveUser(token : any, authorized: boolean, role : number) {
    setData('token', token);
    setData('authorized', authorized);
    setData('role', role);
  }

  // Token actions

  setToken(token: any) {
    setData('token', token);
  }

  getToken() {
    return getData('token');
  }

  clearToken() {
    clearData('token');
  }

  // Authorization actions

  isAuthorized() {
    return getData('authorized') == true;
  }

  setAuthorzed(value: boolean) {
    setData('authorized', value);
  }

  getRole() {
    return getData('role');
  }

  logout() {
    clearData('token');
    clearData('role');
  }
}

export default new StorageService();