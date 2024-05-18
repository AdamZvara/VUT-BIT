using AutoMapper;
using RideWithMe.DAL;
using RideWithMe.DAL.Entities;

namespace RideWithMe.BL.Models;
public record RideDetailModel(
    DateTime StartTime,
    DateTime EndTime,
    Guid DriverId,
    Guid CarId,
    Guid StartLocationId,
    Guid EndLocationId) : ModelBase
{
    public DateTime StartTime { get; set; } = StartTime;
    public DateTime EndTime { get; set; } = EndTime;
    public RideDetailCarModel? Car { get; set; }
    public RideDetailDriverModel? Driver { get; set; }
    public List<UserModel> Passengers { get; init; } = new();
    public RideDetailLocationModel? StartLocation { get; set; } 
    public RideDetailLocationModel? EndLocation { get; set; } 

    /*
    public RideDetailModel(
        DateTime StartTime,
        DateTime EndTime,
        Guid DriverId,
        Guid CarId,
        Guid StartLocationId,
        Guid EndLocationId) : this(StartTime, EndTime, DriverId, CarId, StartLocationId, EndLocationId) { }
    */
    public class MapperProfile : Profile
    {
        public MapperProfile()
        {
            CreateMap<RideEntity, RideDetailModel>()
                .ForMember(dst => dst.Passengers,
                    opt => opt.MapFrom(src => src.RidePassengers.Select(x => x.Passenger)))
                .ReverseMap()
                .ForMember(dst => dst.RidePassengers,
                    opt => opt.MapFrom(src => src.Passengers.Select(x => new RidePassengers(x.Id, src.Id))));
        }
    }

    public static RideDetailModel Empty => new(DateTime.Now, DateTime.Now, Guid.Empty, Guid.Empty, Guid.Empty, Guid.Empty );

}

