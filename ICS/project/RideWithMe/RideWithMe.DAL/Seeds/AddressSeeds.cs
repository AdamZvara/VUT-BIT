using Microsoft.EntityFrameworkCore;
using RideWithMe.DAL.Entities;

namespace RideWithMe.DAL.Seeds;

public static class AddressSeeds
{
    public static readonly AddressEntity Address1 = new(
        Id: Guid.Parse(input: "D37DFFE5-4FD5-4457-BFCC-1D3F0073CC98"),
        State: "Czech Republic",
        Street: "Kolejni",
        City: "Brno",
        HouseNumber: 1)
    {
    };

    public static readonly AddressEntity Address2 = new(
        Id: Guid.Parse(input: "59BF8999-2086-4B19-BB79-D616B68FA2B5"),
        State: "Slovakia",
        Street: "Masarykova",
        City: "Bratislava",
        HouseNumber: 10)
    {
    };

    public static readonly AddressEntity Address3 = new(
        Id: Guid.Parse(input: "971293E5-CE4F-4097-9ADA-CF25923D442C"),
        State: "Czech Republic",
        Street: "Hlavni nadrazi",
        City: "Praha",
        HouseNumber: 20)
    {
    };

    public static void Seed(this ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<AddressEntity>().HasData(
            Address1 with { StartOfRides = Array.Empty<RideEntity>(), EndOfRides = Array.Empty<RideEntity>() },
            Address2 with { StartOfRides = Array.Empty<RideEntity>(), EndOfRides = Array.Empty<RideEntity>() },
            Address3 with { StartOfRides = Array.Empty<RideEntity>(), EndOfRides = Array.Empty<RideEntity>() }
            );
    }
}