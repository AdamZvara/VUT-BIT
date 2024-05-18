using RideWithMe.Common.Enums;

namespace RideWithMe.DAL.Entities
{
    public record CarEntity(
        Guid Id,
        DateTime RegistrationDate,
        string? ImageUrl,
        // Connection ID
        Guid OwnerId,
        CarTypeId CarTypeId,
        CarBrandId BrandId,
        int Seats
        ) : IEntity
    {
        //Automapper requires parameter less constructor for collection synchronization for now
#nullable disable
        public CarEntity() : this(default, default, default, default, default, default, 0) { }
#nullable enable
        public UserEntity? Owner { get; init; }
        public ICollection<RideEntity> Rides { get; init; } = new List<RideEntity>();
    }
}
