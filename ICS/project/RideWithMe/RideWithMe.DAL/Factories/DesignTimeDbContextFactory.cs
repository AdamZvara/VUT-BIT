using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Design;

namespace RideWithMe.DAL.Factories;

public class DesignTimeDbContextFactory : IDesignTimeDbContextFactory<RideWithMeDbContext>
{
    public RideWithMeDbContext CreateDbContext(string[] args)
    {
        DbContextOptionsBuilder<RideWithMeDbContext> builder = new();
        builder.UseSqlServer(
            @"Data Source=(LocalDB)\MSSQLLocalDB;
                Initial Catalog = RideWithMe;
                MultipleActiveResultSets = True;
                Integrated Security = True; ");

        return new RideWithMeDbContext(builder.Options);
    }
}