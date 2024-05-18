using System;
using System.Threading.Tasks;
using RideWithMe.Common.Tests;
using RideWithMe.Common.Tests.Factories;
using Microsoft.EntityFrameworkCore;
using Xunit;
using Xunit.Abstractions;

namespace RideWithMe.DAL.Tests;

public class DbContextTestsBase : IAsyncLifetime
{
    protected DbContextTestsBase(ITestOutputHelper output)
    {
        XUnitTestOutputConverter converter = new(output);
        Console.SetOut(converter);

        //DbContextFactory = new DbContextTestingInMemoryFactory(GetType().Name, seedTestingData: true);
        //DbContextFactory = new DbContextLocalDBTestingFactory(GetType().FullName!, seedTestingData: true);
        DbContextFactory = new DbContextSQLiteTestingFactory(GetType().FullName!, seedTestingData: true);
        RideWithMeDbContextSUT = DbContextFactory.CreateDbContext();
    }

    protected IDbContextFactory<RideWithMeDbContext> DbContextFactory { get; }
    protected RideWithMeDbContext RideWithMeDbContextSUT { get; }


    public async Task InitializeAsync()
    {
        await RideWithMeDbContextSUT.Database.EnsureDeletedAsync();
        await RideWithMeDbContextSUT.Database.EnsureCreatedAsync();
    }

    public async Task DisposeAsync()
    {
        await RideWithMeDbContextSUT.Database.EnsureDeletedAsync();
        await RideWithMeDbContextSUT.DisposeAsync();
    }
}