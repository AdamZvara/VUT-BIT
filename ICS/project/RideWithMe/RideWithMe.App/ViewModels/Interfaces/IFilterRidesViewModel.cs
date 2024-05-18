using System.Collections.ObjectModel;
using RideWithMe.App.Model;

namespace RideWithMe.App.ViewModels.Interfaces;

public interface IFilterRidesViewModel 
{
    public ObservableCollection<FilterModel> FilterModels { get; }
}