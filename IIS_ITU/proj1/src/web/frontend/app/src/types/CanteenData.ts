export default interface CanteenData {
  id: number;
  address: string;
  name: string;
  opened_first: [
    t1: string,
    t2: string
  ];
  opened_second: [
    t1: string,
    t2: string
  ];
}