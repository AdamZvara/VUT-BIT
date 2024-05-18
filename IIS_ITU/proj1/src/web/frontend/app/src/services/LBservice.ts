import http from "@/http-common";

class LBservice {
  getTopRatingsToday(count: number): Promise<any> {
    return http.get(`/stats/rating/daily/topfoods/${count}`);
  }
  getTopRatingsAllTime(count: number): Promise<any> {
    return http.get(`/stats/rating/topfoods/${count}`);
  }
}

export default new LBservice();