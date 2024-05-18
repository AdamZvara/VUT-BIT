using Microsoft.EntityFrameworkCore;
using RideWithMe.DAL;

namespace RideWithMe.Common.Tests;

public class DbContextTestingInMemoryFactory : IDbContextFactory<RideWithMeDbContext>
{
    private readonly string _databaseName;
    private readonly bool _seedTestingData;

    public DbContextTestingInMemoryFactory(string databaseName, bool seedTestingData = false)
    {
        _databaseName = databaseName;
        _seedTestingData = seedTestingData;
    }

    public RideWithMeDbContext CreateDbContext()
    {
        DbContextOptionsBuilder<RideWithMeDbContext> contextOptionsBuilder = new();
        contextOptionsBuilder.UseInMemoryDatabase(_databaseName);


        return new RideWithMeTestingDbContext(contextOptionsBuilder.Options, _seedTestingData);
    }
}