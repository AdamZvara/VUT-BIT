using System;
using System.Collections.ObjectModel;
using System.Windows.Input;
using Microsoft.Toolkit.Mvvm.Input;
using RideWithMe.App.Enums;
using RideWithMe.App.Factories;
using RideWithMe.App.Messages;
using RideWithMe.App.Model;
using RideWithMe.App.Services;
using RideWithMe.App.ViewModels.Interfaces;
using RideWithMe.App.Views;
using RideWithMe.App.Wrappers;

namespace RideWithMe.App.ViewModels.MainViewVMs;

public class FilterRidesViewModel : ViewModelBase, IFilterRidesViewModel
{
    private readonly IMediator _mediator;

    public FilterRidesViewModel(
        IMediator mediator)
    {
        FilterModels = GetFilters();
        _mediator = mediator;

        ApplyFilterCommand = new RelayCommand<FilterModel>(OnApplyFilter);
    }
    public ObservableCollection<FilterModel> FilterModels { get; set; }

    public ICommand ApplyFilterCommand { get; }

    private ObservableCollection<FilterModel> GetFilters()
    {
        var filters = new ObservableCollection<FilterModel>();
        foreach (var filterType in Enum.GetValues<FilterOption>())
        {
            filters.Add(NewFilterModel(filterType));
        }

        return filters;
    }

    private FilterModel NewFilterModel(FilterOption name, bool check = false)
    {
        var model = new FilterModel
        {
            Name = name,
            Checked = check
        };
        return model;
    }

    private void OnApplyFilter(FilterModel a)
    {
        _mediator.Send(new UpdateMessage<RideWrapper>());
    }


}