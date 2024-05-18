import http from "@/http-common";

class RatingService {
  getAll(data: any): Promise<any> {
    return http.post("/ratings", data);
  }

  remove(data: any): Promise<any> {
    return http.post("/ratings/delete", data);
  }
}

export default new RatingService();