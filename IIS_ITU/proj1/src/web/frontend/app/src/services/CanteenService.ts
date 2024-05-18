import http from "@/http-common";

class CanteenService {
  getAll(): Promise<any> {
    return http.get("/canteens");
  }
}

export default new CanteenService();