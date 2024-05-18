import http from "@/http-common";

class UserInfo {
  getAll(): Promise<any> {
    return http.get("/users");
  }

  createUser(data: any): Promise<any> {
    return http.post("admin/user_create", data)
  }

  updateUser(data: any): Promise<any> {
    return http.post("admin/user_update", data)
  }

  blockUser(data: any): Promise<any> {
    return http.post("admin/user_block", data)
  }
}

export default new UserInfo();