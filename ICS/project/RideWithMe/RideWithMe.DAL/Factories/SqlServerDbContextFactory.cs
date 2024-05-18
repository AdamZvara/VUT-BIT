using Microsoft.EntityFrameworkCore;

namespace RideWithMe.DAL.Factories
{
    public class SqlServerDbContextFactory : IDbContextFactory<RideWithMeDbContext>
    {
        private readonly string _connectionString;
        private readonly bool _seedDemoData;

        public SqlServerDbContextFactory(string connectionString, bool seedDemoData = false)
        {
            _connectionString = connectionString;
            _seedDemoData = seedDemoData;
        }

        public RideWithMeDbContext CreateDbContext()
        {
            var optionsBuilder = new DbContextOptionsBuilder<RideWithMeDbContext>();
            optionsBuilder.UseSqlServer(_connectionString);

            //optionsBuilder.LogTo(System.Console.WriteLine); //Enable in case you want to see tests details, enabled may cause some inconsistencies in tests
            //optionsBuilder.EnableSensitiveDataLogging();

            return new RideWithMeDbContext(optionsBuilder.Options, _seedDemoData);
        }
    }
}