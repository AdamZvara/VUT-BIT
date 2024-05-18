namespace RideWithMe.DAL.Entities
{
    public record RideEntity(
        Guid Id,
        DateTime StartTime,
        DateTime EndTime,
        // Connection IDs
        Guid CarId,
        Guid DriverId,
        Guid StartLocationId,
        Guid EndLocationId
        ) : IEntity
    {
        public CarEntity? Car { get; init; }
        public UserEntity? Driver { get; init; }
        public ICollection<RidePassengers> RidePassengers { get; init; } = new List<RidePassengers>();
        public AddressEntity? StartLocation { get; init; }
        public AddressEntity? EndLocation { get; init; }
    }
}
