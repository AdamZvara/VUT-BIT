using AutoMapper;
using RideWithMe.DAL;
using RideWithMe.DAL.Entities;

namespace RideWithMe.BL.Models;
public record RideDetailDriverModel(
    Guid Id,
    string FirstName,
    string LastName)
{
    public Guid Id { get; set; } = Id;
    public string? FirstName { get; set; } = FirstName;
    public string? LastName { get; set; } = LastName;

    public class MapperProfile : Profile
    {
        public MapperProfile()
        {
            CreateMap<UserEntity, RideDetailDriverModel>()
                .ReverseMap();
        }
    }
}

