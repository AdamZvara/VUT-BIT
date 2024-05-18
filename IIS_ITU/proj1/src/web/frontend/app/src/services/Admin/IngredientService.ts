import http from "@/http-common";

class IngredientService {
  getAll(): Promise<any> {
    return http.get("/ingredients");
  }

  saveAll(data: any): Promise<any> {
    return http.post("/ingredients", data);
  }
}

export default new IngredientService();