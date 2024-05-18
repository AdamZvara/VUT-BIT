using System;
using Microsoft.EntityFrameworkCore;
using RideWithMe.DAL.Entities;

namespace RideWithMe.Common.Tests.Seeds;

public static class AddressSeeds
{
    public static readonly AddressEntity EmptyAddressEntity = new(
        Id: default,
        State: default!,
        Street: default!,
        City: default!,
        HouseNumber: default
    );

    public static readonly AddressEntity AddressEntity = new(
        Id: Guid.Parse("20C51389-1180-4606-8B79-11F3CE583292"),
        State: "Czech Republic",
        Street: "Kolejni",
        City: "Brno",
        HouseNumber: 12
    );

    public static readonly AddressEntity AddressEntityWithoutCollections = AddressEntity with { StartOfRides = Array.Empty<RideEntity>(), EndOfRides = Array.Empty<RideEntity>()};
    public static readonly AddressEntity AddressEntityDelete = AddressEntity with { Id = Guid.Parse("DE4A1EB6-E86E-4A4C-AD04-E835AF61F3D3"), StartOfRides = Array.Empty<RideEntity>(), EndOfRides = Array.Empty<RideEntity>() };
    public static readonly AddressEntity AddressEntityUpdate = AddressEntity with { Id = Guid.Parse("141DC9E1-2D9B-4735-9542-1489A217A4C1"), StartOfRides = Array.Empty<RideEntity>(), EndOfRides = Array.Empty<RideEntity>() };

    // Add Rides entities connected to address
    static AddressSeeds()
    {
        AddressEntity.StartOfRides.Add(RideSeeds.RideEntity);
        AddressEntity.EndOfRides.Add(RideSeeds.RideEntity);
    }

    public static void Seed(this ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<AddressEntity>().HasData(
            AddressEntityWithoutCollections,
            AddressEntityDelete,
            AddressEntityUpdate
        );
    }

    private static AddressEntity DeleteNavigationProps(AddressEntity e)
    {
        return e with
        {
            StartOfRides = Array.Empty<RideEntity>(), EndOfRides = Array.Empty<RideEntity>()
        };
    }

}