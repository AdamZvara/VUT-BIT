namespace RideWithMe.DAL.Entities
{
    public record AddressEntity(
        Guid Id,
        string State,
        string Street,
        string City,
        int HouseNumber
        ) : IEntity
    {
        //Automapper requires parameter less constructor for collection synchronization for now
#nullable disable
        public AddressEntity() : this(default, default, default, default, default) { }
#nullable enable
        public ICollection<RideEntity> StartOfRides { get; init; } = new List<RideEntity>();
        public ICollection<RideEntity> EndOfRides { get; init; } = new List<RideEntity>();
    }
}
