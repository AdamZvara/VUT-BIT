using AutoMapper;
using RideWithMe.Common.Enums;
using RideWithMe.DAL;
using RideWithMe.DAL.Entities;

namespace RideWithMe.BL.Models;
public record RideDetailCarModel(
    Guid Id,
    string? ImageUrl,
    CarBrandId? BrandId,
    int Seats)
{
    public Guid Id { get; set; } = Id;
    public string? ImageUrl { get; set; } = ImageUrl;
    public CarBrandId? BrandId { get; set; } = BrandId;
    public int Seats { get; set; } = Seats;
    public class MapperProfile : Profile
    {
        public MapperProfile()
        {
            CreateMap<CarEntity, RideDetailCarModel>()
                .ReverseMap();
        }
    }
}
