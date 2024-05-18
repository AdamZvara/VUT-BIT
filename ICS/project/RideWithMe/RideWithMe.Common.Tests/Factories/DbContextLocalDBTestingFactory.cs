using Microsoft.EntityFrameworkCore;
using RideWithMe.Common.Tests;
using RideWithMe.DAL;

namespace RideWithMe.Common.Tests.Factories;

public class DbContextLocalDBTestingFactory : IDbContextFactory<RideWithMeDbContext>
{
    private readonly string _databaseName;
    private readonly bool _seedTestingData;

    public DbContextLocalDBTestingFactory(string databaseName, bool seedTestingData = false)
    {
        _databaseName = databaseName;
        _seedTestingData = seedTestingData;
    }
    public RideWithMeDbContext CreateDbContext()
    {
        DbContextOptionsBuilder<RideWithMeDbContext> builder = new();
        builder.EnableSensitiveDataLogging();
        builder.UseSqlServer($"Data Source=(LocalDB)\\MSSQLLocalDB;Initial Catalog = {_databaseName};MultipleActiveResultSets = True;Integrated Security = True; ");
        // contextOptionsBuilder.LogTo(System.Console.WriteLine); //Enable in case you want to see tests details, enabled may cause some inconsistencies in tests
        // builder.EnableSensitiveDataLogging();
        

        return new RideWithMeTestingDbContext(builder.Options, _seedTestingData);
    }
}