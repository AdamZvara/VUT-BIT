using Microsoft.EntityFrameworkCore;
using RideWithMe.Common.Tests;
using RideWithMe.DAL;

namespace RideWithMe.Common.Tests.Factories;

public class DbContextSQLiteTestingFactory : IDbContextFactory<RideWithMeDbContext>
{
    private readonly string _databaseName;
    private readonly bool _seedTestingData;

    public DbContextSQLiteTestingFactory(string databaseName, bool seedTestingData = false)
    {
        _databaseName = databaseName;
        _seedTestingData = seedTestingData;
    }
    public RideWithMeDbContext CreateDbContext()
    {
        DbContextOptionsBuilder<RideWithMeDbContext> builder = new();
        builder.UseSqlite($"Data Source={_databaseName};Cache=Shared");
        // contextOptionsBuilder.LogTo(System.Console.WriteLine); //Enable in case you want to see tests details, enabled may cause some inconsistencies in tests
        builder.EnableSensitiveDataLogging();
        
        return new RideWithMeTestingDbContext(builder.Options, _seedTestingData);
    }
}