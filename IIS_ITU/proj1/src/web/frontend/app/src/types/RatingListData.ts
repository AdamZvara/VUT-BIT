export default interface RatingListData {
  id:number;
  points: number;
  text: string;
  added: string;
  to_food: {
    food: {
      name: string;
    };
    canteen: {
      name: string;
    }
  },
  user: {
    email: string;
    login: string;
  }
}
