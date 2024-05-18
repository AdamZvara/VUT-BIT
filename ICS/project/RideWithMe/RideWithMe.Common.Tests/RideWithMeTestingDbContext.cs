using Microsoft.EntityFrameworkCore;
using RideWithMe.Common.Tests.Seeds;
using RideWithMe.DAL;

namespace RideWithMe.Common.Tests;

public class RideWithMeTestingDbContext : RideWithMeDbContext
{
    private readonly bool _seedTestingData;

    public RideWithMeTestingDbContext(DbContextOptions contextOptions, bool seedTestingData = false)
        : base(contextOptions, seedDemoData: false)
    {
        _seedTestingData = seedTestingData;
    }

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        base.OnModelCreating(modelBuilder);

        if (_seedTestingData)
        {
            AddressSeeds.Seed(modelBuilder);
            RidePassengersSeeds.Seed(modelBuilder);
            UserSeeds.Seed(modelBuilder);
            CarSeeds.Seed(modelBuilder);
            RideSeeds.Seed(modelBuilder);
        }
    }
}