using System;
using System.Globalization;
using Microsoft.EntityFrameworkCore;
using RideWithMe.DAL.Entities;

namespace RideWithMe.Common.Tests.Seeds;

public static class RideSeeds
{
    public static readonly RideEntity EmptyRideEntity = new(
        Id: default,
        StartTime: default,
        EndTime: default,
        CarId: default,
        DriverId: default,
        StartLocationId: default,
        EndLocationId: default
    );

    public static readonly RideEntity RideEntity = new(
        Id: Guid.Parse("3B714706-3741-42E9-8B47-65577D9805FB"),
        StartTime: DateTime.ParseExact(
            "18/08/2018", "dd/MM/yyyy", CultureInfo.InvariantCulture),
        EndTime: DateTime.ParseExact(
            "18/08/2018", "dd/MM/yyyy", CultureInfo.InvariantCulture),
        CarId: Guid.Parse("9A6F3CF1-B512-4660-854D-D62703D57A10"),
        DriverId: Guid.Parse("3D90E905-8C83-471A-9E71-CC13182B49CC"),
        StartLocationId: Guid.Parse("20C51389-1180-4606-8B79-11F3CE583292"),
        EndLocationId: Guid.Parse("20C51389-1180-4606-8B79-11F3CE583292")
    )
    {
        Car = CarSeeds.CarEntity,
        Driver = UserSeeds.DriverEntity,
        StartLocation = AddressSeeds.AddressEntity,
        EndLocation = AddressSeeds.AddressEntity
    };

    public static readonly RideEntity RideEntityWithoutCollections = RideEntity with {Car = null, Driver = null, RidePassengers = Array.Empty<RidePassengers>(), StartLocation = null, EndLocation = null};

    public static readonly RideEntity RideEntityUpdate = RideEntity with {Id = Guid.Parse("0FFE7F1C-68E7-43CE-8CDF-F022CE153F13"), Car = null, Driver = null, RidePassengers = Array.Empty<RidePassengers>(), StartLocation = null, EndLocation = null};
    public static readonly RideEntity RideEntityDelete = RideEntity with {Id = Guid.Parse("FBC1E9C6-E111-4F19-A128-0FA1089026DC"), Car = null, Driver = null, RidePassengers = Array.Empty<RidePassengers>(), StartLocation = null, EndLocation = null};
    public static readonly RideEntity RideEntityToDelete = RideEntity with {Id = Guid.Parse("1C97165C-9550-477F-815B-B560B0BA914E"), Car = null, Driver = null, RidePassengers = Array.Empty<RidePassengers>(), StartLocation = null, EndLocation = null};
    public static readonly RideEntity RideEntityToUpdate = RideEntity with {Id = Guid.Parse("081918BA-EC91-40F1-B341-7D53D4E6CCA1"), Car = null, Driver = null, RidePassengers = Array.Empty<RidePassengers>(), StartLocation = null, EndLocation = null};
    public static readonly RideEntity RideEntityWithPassenger = RideEntity with
    {
        Id = Guid.Parse("89760FCF-2DE6-4A44-98CF-3A5960BD4277"),
        Car = null,
        Driver = null,
        StartLocation = null,
        EndLocation = null
    };
    public static readonly RideEntity RideEntityWithPassengerWithoutCollections = RideEntityWithPassenger with {Car = null, Driver = null, RidePassengers = Array.Empty<RidePassengers>(), StartLocation = null, EndLocation = null};


    static RideSeeds()
    {
        RideEntity.RidePassengers.Add(RidePassengersSeeds.RidePassengers);
        //RideEntity.RidePassengers.Add(RidePassengersSeeds.RidePassengersDelete);
        //RideEntityWithPassenger.RidePassengers.Add(RidePassengersSeeds.RidePassengersRandom);
    }

    public static void Seed(this ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<RideEntity>().HasData(
            RideEntityWithoutCollections,
            RideEntityWithPassengerWithoutCollections,
            RideEntityToDelete,
            RideEntityToUpdate,
            RideEntityUpdate,
            RideEntityDelete
        );
    }

    private static RideEntity DeleteNavigationProps(RideEntity r)
    {
        return r with
        {
            Car = null,
            Driver = null, 
            RidePassengers = Array.Empty<RidePassengers>(), 
            StartLocation = null,
            EndLocation = null
        };
    }
}