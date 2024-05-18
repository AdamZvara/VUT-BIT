import http from "@/http-common";

class UsersAuth {
  login(data:any): Promise<any> {
    return http.post("/login", JSON.stringify(data));
  }

  signup(data:any): Promise<any> {
    return http.post("/signup", data);
  }

  logout(data:any): Promise<any> {
    return http.post("/logout", data);
  }
}

export default new UsersAuth();