using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RideWithMe.App.Settings
{
    public class DALSettings
    {
        public string? ConnectionString { get; set; }
        public bool SkipMigrationAndSeedDemoData { get; set; }
    }
}
