namespace RideWithMe.DAL.Entities
{
    public record UserEntity(
        Guid Id,
        string FirstName,
        string LastName,
        string? ImageUrl) : IEntity
    {
        //Automapper requires parameter less constructor for collection synchronization for now
#nullable disable
        public UserEntity() : this(default, default, default, default) { }
#nullable enable
        public ICollection<CarEntity> Cars { get; init; } = new List<CarEntity>();
        public ICollection<RideEntity> DriverRides { get; init; } = new List<RideEntity>();
        public ICollection<RidePassengers> RidePassengers { get; init; } = new List<RidePassengers>();
    }
}
