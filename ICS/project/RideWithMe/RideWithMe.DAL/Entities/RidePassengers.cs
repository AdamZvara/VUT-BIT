using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RideWithMe.DAL.Entities
{
    public record RidePassengers(
        Guid Id,
        Guid PassengerId,
        Guid RideId
        ) : IEntity
    {
        public RidePassengers(
            Guid PassengerId,
            Guid RideId)
            : this(default, PassengerId, RideId) { }

        public UserEntity? Passenger { get; init; } 
        public RideEntity? Ride { get; init; }

    }
}
