using AutoMapper;
using RideWithMe.DAL;
using RideWithMe.DAL.Entities;

namespace RideWithMe.BL.Models
{
    public record RideListModel(
        DateTime StartTime,
        DateTime EndTime,
        Guid StartLocationId,
        string StartLocationCity,
        Guid EndLocationId,
        string EndLocationCity) : ModelBase
    {
        public DateTime StartTime { get; set; } = StartTime;
        public DateTime EndTime { get; set; } = EndTime;
        public Guid StartLocationId { get; set; } = StartLocationId;
        public string StartLocationCity { get; set; } = StartLocationCity;
        public string EndLocationCity { get; set; } = EndLocationCity;
        public Guid EndLocationId { get; set; } = EndLocationId;

        public class MapperProfile : Profile
        {
            public MapperProfile()
            {
                CreateMap<RideEntity, RideListModel>();
            }
        }
    }
}