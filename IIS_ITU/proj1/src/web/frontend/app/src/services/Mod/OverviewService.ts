import http from "@/http-common";

class OverviewService {
  getCanteenRatings(canteen_id: number, dates: any): Promise<any> {
    return http.get(`/overview/${canteen_id}/${dates[0]}_${dates[1]}`);
  }
}

export default new OverviewService();