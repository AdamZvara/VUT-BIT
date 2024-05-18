using AutoMapper;
using RideWithMe.Common.Enums;
using RideWithMe.DAL;
using RideWithMe.DAL.Entities;

namespace RideWithMe.BL.Models
{
    public record CarListModel(
        CarTypeId CarTypeId,
        CarBrandId BrandId,
        UserModel Owner) : ModelBase
    {
        public UserModel Owner { get; set; } = Owner;
        public CarTypeId CarTypeID { get; set; } = CarTypeId;

        public CarBrandId BrandId { get; set; } = BrandId;

        public string? ImageUrl { get; set; }
        public class MapperProfile : Profile
        {
            public MapperProfile()
            {
                CreateMap<CarEntity, CarListModel>();

            }
        }
    
    }
}
