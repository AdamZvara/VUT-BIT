using System;
using Microsoft.EntityFrameworkCore;
using RideWithMe.DAL.Entities;

namespace RideWithMe.Common.Tests.Seeds;

public static class RidePassengersSeeds
{

    public static readonly RidePassengers EmptyRidePassengers = new(
        Id: default,
        PassengerId: default,
        RideId: default
    );

    public static readonly RidePassengers RidePassengers = new(
        Id: Guid.Parse("C589E0DA-78D4-4E64-AF27-74E5652F856C"),
        PassengerId: Guid.Parse("83130753-8E32-483A-A16A-2B53A5B6BE34"),
        RideId: Guid.Parse("3B714706-3741-42E9-8B47-65577D9805FB")
    )
    {
        Passenger = UserSeeds.PassengerEntity,
        Ride = RideSeeds.RideEntity
    };
    

    public static readonly RidePassengers RidePassengersRandom = new(
        Id: Guid.Empty,
        PassengerId: Guid.Parse("83130753-8E32-483A-A16A-2B53A5B6BE34"),
        RideId: Guid.Parse("89760FCF-2DE6-4A44-98CF-3A5960BD4277")
    )
    {
        Passenger = UserSeeds.PassengerEntity,
        Ride = RideSeeds.RideEntityWithPassenger
    };


    public static void Seed(this ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<RidePassengers>().HasData(
            DeleteNavigationProps(RidePassengers),
            DeleteNavigationProps(RidePassengersRandom)
        );
    }

    private static RidePassengers DeleteNavigationProps(RidePassengers r)
    {
        return r with
        {
            Passenger = null,
            Ride = null
        };
    }
}