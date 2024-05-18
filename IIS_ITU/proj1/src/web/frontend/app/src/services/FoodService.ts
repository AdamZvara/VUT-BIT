import http from "@/http-common";

class FoodService {
  getAll(): Promise<any> {
    return http.get("/current_foods");
  }

  findByCanteen(id : number): Promise<any> {
    return http.get(`/current_foods/${id}`);
  }

  getTodayRatings(id : number): Promise<any> {
    return http.get(`/current_foods/${id}/rating_today`);
  }

  getOverallRatings(id : number): Promise<any> {
    return http.get(`/current_foods/${id}/rating_overall`);
  }
}

export default new FoodService();