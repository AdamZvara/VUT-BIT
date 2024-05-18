using AutoMapper;
using RideWithMe.DAL;
using RideWithMe.DAL.Entities;

namespace RideWithMe.BL.Models;
public record RideDetailLocationModel(
    Guid Id,
    string City,
    string Street)
{
    public Guid Id { get; set; } = Id;
    public string City { get; set; } = City;
    public string Street { get; set; } = Street;
    public class MapperProfile : Profile
    {
        public MapperProfile()
        {
            CreateMap<AddressEntity, RideDetailLocationModel>()
                .ReverseMap();
        }
    }
}
