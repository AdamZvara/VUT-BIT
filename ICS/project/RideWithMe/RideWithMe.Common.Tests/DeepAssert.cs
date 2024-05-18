using System.Collections.Generic;
using KellermanSoftware.CompareNetObjects;


namespace RideWithMe.Common.Tests
{
    public static class DeepAssert
    {
        public static void Equal<T>(T? expected, T? actual, params string[] propertiesToIgnore)
        {
            CompareLogic compareLogic = new()
            {
                Config =
                {
                    MembersToIgnore = new List<string>(),
                    IgnoreCollectionOrder = true,
                    IgnoreObjectTypes = true,
                    CompareStaticProperties = false,
                    CompareStaticFields = false
                }
            };

            foreach (var str in propertiesToIgnore)
                compareLogic.Config.MembersToIgnore.Add(str);

            var comparisonResult = compareLogic.Compare((object)expected!, (object)actual!);
            if (!comparisonResult.AreEqual)
                throw new ObjectEqualException((object)expected!, (object)actual!, comparisonResult.DifferencesString);
        }
    }
}