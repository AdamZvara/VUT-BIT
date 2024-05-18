import http from "@/http-common";

class RatingService {
  createRating(data: any): Promise<any> {
    return http.post("/rating", data);
  }

  getMyRatings(): Promise<any> {
    return http.get("/my_ratings");
  }

  getRatingStats(): Promise<any> {
    return http.get("stats/rating/count/self");
  }

  rmRating(data: any): Promise<any> {
    return http.post(`my_ratings/remove`, data);
  }

  editRating(data: any): Promise<any> {
    return http.post('my_ratings/edit', data)
  }
}

export default new RatingService();