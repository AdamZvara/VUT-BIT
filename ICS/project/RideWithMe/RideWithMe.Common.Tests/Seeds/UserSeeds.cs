using System;
using System.Collections.Generic;
using System.Net.NetworkInformation;
using Microsoft.EntityFrameworkCore;
using RideWithMe.DAL.Entities;

namespace RideWithMe.Common.Tests.Seeds;

public static class UserSeeds
{
    public static readonly UserEntity EmptyUserEntity = new(
        Id: default,
        FirstName: default!,
        LastName: default!,
        ImageUrl: default
    );

    public static readonly UserEntity DriverEntity = new(
        Id: Guid.Parse("3D90E905-8C83-471A-9E71-CC13182B49CC"),
        FirstName: "Seeded Driver Entity",
        LastName: "Seeded Driver Entity",
        ImageUrl: null
    );

    public static readonly UserEntity PassengerEntity = new(
        Id: Guid.Parse("83130753-8E32-483A-A16A-2B53A5B6BE34"),
        FirstName: "Seeded Passenger Entity",
        LastName: "Seeded Passenger Entity",
        ImageUrl: null
    );
    public static readonly UserEntity PassengerEntity2 = new(
        Id: Guid.Parse("83130753-8E00-483A-A16A-2B53A5B6BE34"),
        FirstName: "Seeded Passenger Entity2",
        LastName: "Seeded Passenger Entity2",
        ImageUrl: null
    );

    public static readonly UserEntity DriverEntityWithoutCollections = DriverEntity with { Cars = Array.Empty<CarEntity>(), DriverRides = Array.Empty<RideEntity>(), RidePassengers = Array.Empty<RidePassengers>() };
    public static readonly UserEntity UserEntityDelete = DriverEntity with { Id = Guid.Parse("4504639D-85FC-41E1-9907-B7BC08B1CD14"), Cars = Array.Empty<CarEntity>(), DriverRides = Array.Empty<RideEntity>(), RidePassengers = Array.Empty<RidePassengers>() };
    public static readonly UserEntity UserEntityUpdate = DriverEntity with {Id = Guid.Parse("E180D513-4713-4467-9398-A053BF02C042"), Cars = Array.Empty<CarEntity>(), DriverRides = Array.Empty<RideEntity>(), RidePassengers = Array.Empty<RidePassengers>()};
    public static readonly UserEntity PassengerEntityToAdd = DriverEntity with {Id = Guid.Parse("AE601546-3EF5-4D22-A401-629E53EF41D2"), Cars = Array.Empty<CarEntity>(), DriverRides = Array.Empty<RideEntity>(), RidePassengers = Array.Empty<RidePassengers>()};
    public static readonly UserEntity PassengerEntityToAdd2 = DriverEntity with {Id = Guid.Parse("DB1FB65F-314A-41B7-A56B-592542F5F068"), Cars = Array.Empty<CarEntity>(), DriverRides = Array.Empty<RideEntity>(), RidePassengers = Array.Empty<RidePassengers>()};

    // Add Cars and Rides collections
    static UserSeeds()
    {
        DriverEntity.Cars.Add(CarSeeds.CarEntity);
        DriverEntity.DriverRides.Add(RideSeeds.RideEntity);
        PassengerEntity.RidePassengers.Add(RidePassengersSeeds.RidePassengers);
    }

    public static void Seed(this ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<UserEntity>().HasData(
            DriverEntityWithoutCollections,
            DeleteNavigationProps(PassengerEntity),
            UserEntityDelete,
            UserEntityUpdate,
            DeleteNavigationProps(PassengerEntity2),
            DeleteNavigationProps(PassengerEntityToAdd2)
        );
    }

    public static UserEntity DeleteNavigationProps(UserEntity u)
    {
        return u with
        {
            Cars = Array.Empty<CarEntity>(),
            DriverRides = Array.Empty<RideEntity>(),
            RidePassengers = Array.Empty<RidePassengers>(),
        };
    }
}