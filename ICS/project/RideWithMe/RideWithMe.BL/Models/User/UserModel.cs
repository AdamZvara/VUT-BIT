using AutoMapper;
using RideWithMe.DAL;
using RideWithMe.DAL.Entities;


namespace RideWithMe.BL.Models
{
    public record UserModel(
        string FirstName,
        string LastName) : ModelBase
    {
        public string FirstName { get; set; } = FirstName;
        public string LastName { get; set; } = LastName;
        public string? ImageUrl { get; set; }
        public class MapperProfile : Profile
        {
            public MapperProfile()
            {
                CreateMap<UserEntity, UserModel>()
                    .ReverseMap();
            }
        }

        public static UserModel Empty => new(string.Empty, string.Empty);
    }
}