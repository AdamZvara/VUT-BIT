import http from "@/http-common";

class userInfo {
  getAll(): Promise<any> {
    return http.get("/user/self");
  }

  getUser(): Promise<any> {
    return http.get("/users");
  }

  update(data: any): Promise<any> {
    return http.post("user/self", data)
  }

  delete(): Promise<any> {
    return http.post('user/delete')
  }
}

export default new userInfo();