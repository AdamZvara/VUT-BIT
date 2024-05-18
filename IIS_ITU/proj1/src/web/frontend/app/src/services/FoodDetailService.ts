import http from "@/http-common";

class FoodDetailService {
  getCurrentFoodDetail(id : number): Promise<any> {
    return http.get(`/current_foods/${id}`);
  }
  getFoodDetail(id : number): Promise<any> {
    return http.get(`/food_detail/${id}`);
  }
  getFoodName(id : number): Promise<any> {
    return http.get(`/food_detail/name/${id}`);
  }
  getFoodAllergens(id : number): Promise<any> {
    return http.get(`/food_detail/allergens/${id}`);
  }
  getIngredients(id : number): Promise<any> {
    return http.get(`/food_detail/ingredients/${id}`);
  }
  getBothRatings(current_food_id : number): Promise<any> {
    return http.get(`/food_detail/both_ratings/${current_food_id}`);
  }
  getAllRatings(current_food_id : number): Promise<any> {
    return http.get(`/food_detail/all_ratings/${current_food_id}`);
  }
  getAll(): Promise<any> {
    return http.get(`/current_foods`);
  }
}

export default new FoodDetailService();
