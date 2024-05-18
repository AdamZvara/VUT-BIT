using AutoMapper;
using RideWithMe.Common.Enums;
using RideWithMe.DAL;
using RideWithMe.DAL.Entities;

namespace RideWithMe.BL.Models
{
    public record CarDetailModel(
        Guid OwnerId,
        DateTime RegistrationDate,
        CarTypeId CarTypeId,
        CarBrandId BrandId,
        int Seats
    ) : ModelBase
    {
        public Guid OwnerId { get; set; } = OwnerId;
        public DateTime RegistrationDate { get; set; } = RegistrationDate;
        public CarTypeId CarTypeId { get; set; } = CarTypeId;
        public CarBrandId BrandId { get; set; } = BrandId;
        public UserModel? Owner { get; set; }
        public string? ImageUrl { get; set; }
        public int Seats { get; set; } = Seats;


        public class MapperProfile : Profile
        {
            public MapperProfile()
            {
                CreateMap<CarEntity, CarDetailModel>()
                    .ReverseMap();
            }
        }
        public static CarDetailModel Empty => new(Guid.Empty, DateTime.Today, CarTypeId.None, CarBrandId.None, 0);
    }
}
