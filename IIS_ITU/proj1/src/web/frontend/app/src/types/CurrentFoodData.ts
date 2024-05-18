import FoodDetailsData from "@/types/FoodDetailsData";

export default interface CurrentFoodData {
  available: boolean;
  id: number;
  canteen: {
    id: number;
    name: string;
  };
  food: FoodDetailsData
}
